import os
from pathlib import Path
import shutil
import struct
import subprocess
import sys
import uuid

from test_base import get_tool_dir
from ui_variable_reclaim_stress_test import check_absent_tokens, run_shell_session


VARIABLE_GUID = '26f6a9ea-8f4a-49b3-b014-443f7eda3c91'
AUTH_HEADER = struct.Struct('<HBBIQ16sIII16s')


def require(text, marker):
    if marker not in text:
        raise RuntimeError('Missing marker: ' + marker)


def get_uvar(image, sbl_dir):
    sys.path.insert(0, str(sbl_dir / 'BootloaderCorePkg/Tools'))
    from IfwiUtility import IFWI_PARSER

    firmware = IFWI_PARSER.parse_ifwi_binary(bytearray(image))
    regions = IFWI_PARSER.locate_components(firmware, 'IFWI/BIOS/NRD/UVAR')
    if len(regions) != 1 or regions[0].length != 0x40000:
        raise RuntimeError('Expected one 256 KiB UVAR region')
    region = regions[0]
    expected_base = int(os.environ.get('SBL_QEMU_UVAR_BASE', '0xFFC05000'), 0)
    if 0x100000000 - len(image) + region.offset != expected_base:
        raise RuntimeError('SBL UVAR base does not match the payload build configuration')
    return region


def replace_marker(working):
    offset = 72 + 28
    found = False
    while offset + AUTH_HEADER.size <= 0x1E000:
        header = AUTH_HEADER.unpack_from(working, offset)
        if header[0] != 0x55AA:
            break
        name_size, data_size = header[7:9]
        name_start = offset + AUTH_HEADER.size
        data_start = name_start + name_size
        entry_end = (data_start + data_size + 3) & ~3
        if name_size < 2 or name_size % 2 or entry_end > 0x1E000:
            raise RuntimeError('Invalid authenticated-format test record')
        name = working[name_start:name_start + name_size].decode('utf-16-le').rstrip('\0')
        if header[1] == 0x3F and name == 'VARTST0' and header[9] == uuid.UUID(VARIABLE_GUID).bytes_le:
            if data_size != 4:
                raise RuntimeError('Unexpected marker size')
            struct.pack_into('<I', working, data_start, 0x11223344)
            found = True
        offset = entry_end
    if not found:
        raise RuntimeError('No live SBL marker in fault-injection image')


def run_ftw_fault_tests(test_image, output_dir, sbl_dir):
    original = test_image.read_bytes()
    region = get_uvar(original, sbl_dir)
    half_size = region.length // 2
    workspace = half_size - 0x2000
    for case in ['invalid', 'erased', 'partial', 'pending', 'aborted', 'dest_complete', 'workspace',
                 'pending_header', 'allocation_header', 'allocation_dirty', 'bad_count', 'bad_private',
                 'bad_record_state', 'allocation_metadata', 'allocation_state', 'allocation_partial_count',
                 'allocation_dirty_tail', 'allocation_unallocated_dirty']:
        image = bytearray(original)
        live = bytearray(image[region.offset:region.offset + half_size])
        live[workspace + 32:] = b'\xFF' * (half_size - workspace - 32)
        spare_fv = bytearray(live[:workspace])
        replace_marker(spare_fv)
        image[region.offset + half_size:region.offset + region.length] = b'\xFF' * half_size
        image[region.offset + half_size:region.offset + half_size + workspace] = spare_fv
        if case in ['erased', 'partial', 'pending', 'aborted', 'dest_complete', 'pending_header',
                    'bad_record_state']:
            record_state = {'aborted': 0xFF, 'dest_complete': 0xF9, 'bad_record_state': 0xFB}.get(case, 0xFD)
            transaction = struct.pack('<B3x16s4xQQ', 0xFC, uuid.UUID(VARIABLE_GUID).bytes_le, 1, 0)
            transaction += struct.pack('<B7xQQQq', record_state, 0, 72, 0x1DFB8, -half_size)
            live[workspace + 32:workspace + 32 + len(transaction)] = transaction
        elif case in ['allocation_header', 'allocation_dirty', 'bad_count', 'bad_private']:
            count = 0xFFFFFFFFFFFFFFFF if case == 'bad_count' else 1
            private_size = 0xFFFFFFFFFFFFFFFF if case == 'bad_private' else 0
            transaction = struct.pack('<B3x16s4xQQ', 0xFE, uuid.UUID(VARIABLE_GUID).bytes_le,
                                      count, private_size)
            live[workspace + 32:workspace + 32 + len(transaction)] = transaction
            if case in ['bad_count', 'bad_private']:
                live[workspace + 32] = 0xFC
            if case == 'allocation_dirty':
                live[workspace + 32 + len(transaction)] = 0
        elif case.startswith('allocation_'):
            transaction = bytearray(b'\xFF' * 40)
            if case != 'allocation_state':
                transaction[4:20] = uuid.UUID(VARIABLE_GUID).bytes_le
            if case in ['allocation_state', 'allocation_partial_count']:
                transaction[0] = 0xFE
            if case == 'allocation_partial_count':
                transaction[24:28] = struct.pack('<I', 1)
            live[workspace + 32:workspace + 72] = transaction
            if case in ['allocation_dirty_tail', 'allocation_unallocated_dirty']:
                if case == 'allocation_dirty_tail':
                    live[workspace + 32] = 0xFE
                    struct.pack_into('<QQ', live, workspace + 56, 1, 0)
                live[-1] = 0
        if case in ['invalid', 'workspace']:
            image[region.offset + half_size:region.offset + region.length] = b'\xFF' * half_size
            image[region.offset + half_size:region.offset + half_size + 0x2000] = live[workspace:]
            if case == 'invalid':
                live[workspace + 20] &= 0xFD
            else:
                live[workspace:] = b'\xFF' * 0x2000
        elif case == 'erased':
            live[:workspace] = b'\xFF' * workspace
        elif case == 'partial':
            live[4096:workspace] = b'\xFF' * (workspace - 4096)
        elif case == 'pending_header':
            live[72:100] = b'\xFF' * 28
        image[region.offset:region.offset + half_size] = live
        fault_image = output_dir / ('ftw_' + case + '.bin')
        fault_image.write_bytes(image)
        if case in ['allocation_dirty', 'bad_count', 'bad_private', 'bad_record_state',
                'allocation_dirty_tail', 'allocation_unallocated_dirty']:
            text = run_shell_session(str(fault_image), [], completion_marker='Volume Corrupt',
                                     log_path=output_dir / ('ftw_' + case + '.log'))
            require(text, 'Volume Corrupt')
            if fault_image.read_bytes() != image:
                raise RuntimeError('Invalid FTW queue changed protected flash: ' + case)
            print('FTW fault-injection case PASSED: ' + case)
            continue
        text = run_shell_session(str(fault_image), [], log_path=output_dir / ('ftw_' + case + '.log'))
        expected = '0x55667788' if case in ['invalid', 'aborted', 'dest_complete', 'workspace',
                                            'allocation_header', 'allocation_metadata', 'allocation_state',
                                            'allocation_partial_count'] else '0x11223344'
        require(text, 'initial GetVariable status=Success data=' + expected + ' size=4')
        if case in ['erased', 'partial', 'pending', 'pending_header']:
            require(text, 'UVAR FTW: replayed pending variable transaction')
        elif case in ['invalid', 'workspace']:
            require(text, 'UVAR FTW: restored EDK2 workspace from spare')
            if fault_image.read_bytes()[region.offset + half_size + 20] != 0xFC:
                raise RuntimeError('Spare FTW workspace copy was not retired: ' + case)
        if check_absent_tokens(text, ['ASSERT', 'Device Error']) != 0:
            raise RuntimeError('FTW replay failed: ' + case)
        print('FTW fault-injection case PASSED: ' + case)


def get_reserved(image, sbl_dir):
    sys.path.insert(0, str(sbl_dir / 'BootloaderCorePkg/Tools'))
    from IfwiUtility import IFWI_PARSER
    firmware = IFWI_PARSER.parse_ifwi_binary(bytearray(image))
    regions = IFWI_PARSER.locate_components(firmware, 'IFWI/BIOS/NVS/RSVD')
    if len(regions) != 1 or regions[0].length != 0x1000:
        raise RuntimeError('Expected a 4 KiB RSVD region')
    return regions[0]


def variable_records(image, region):
    offset = region.offset + 100
    end = region.offset + 0x1E000
    records = []
    while offset + AUTH_HEADER.size <= end:
        header = AUTH_HEADER.unpack_from(image, offset)
        if header[0] != 0x55AA:
            break
        name_size, data_size = header[7:9]
        data_start = offset + AUTH_HEADER.size + name_size
        next_offset = (data_start + data_size + 3) & ~3
        if name_size < 2 or name_size % 2 or next_offset > end:
            raise RuntimeError('Invalid variable record in reclaim test')
        name = image[offset + AUTH_HEADER.size:data_start].decode('utf-16-le').rstrip('\0')
        records.append((name, header, data_start, offset))
        offset = next_offset
    return records, offset


def variable_record(name, data, state=0x3F):
    encoded = (name + '\0').encode('utf-16-le')
    header = AUTH_HEADER.pack(0x55AA, state, 0, 7, 0, bytes(16), 0,
                              len(encoded), len(data), uuid.UUID(VARIABLE_GUID).bytes_le)
    record = header + encoded + data
    return record + b'\xFF' * (-len(record) % 4)


def check_reclaim_result(image, region, reserved, firmware_status):
    if image[reserved.offset:reserved.offset + reserved.length] != firmware_status:
        raise RuntimeError('Variable reclaim changed firmware-update status')
    records, _ = variable_records(image, region)
    live = {}
    for name, header, data_start, _ in records:
        if header[1] == 0x3F:
            live.setdefault(name, image[data_start:data_start + header[8]])
    if live.get('SharedValue') != bytes.fromhex('deadbeef') or 'DeletedValue' in live:
        raise RuntimeError('Latest-value or deletion preservation failed')
    if live.get('SharedAppend') != bytes.fromhex('1122334455667788'):
        raise RuntimeError('Append-value preservation failed')
    for name in ['AddedBeforeTransition', 'AddedAfterTransition']:
        if live.get(name) != b'committed':
            raise RuntimeError('ADDED precedence was not preserved: ' + name)


def run_provisioning_tests(test_image, output_dir, sbl_dir):
    original = test_image.read_bytes()
    region = get_uvar(original, sbl_dir)
    for case in ['fv_only', 'fv_torn', 'fv_with_data', 'fv_with_spare_data']:
        image = bytearray(original)
        image[region.offset + 72:region.offset + region.length] = b'\xFF' * (region.length - 72)
        if case == 'fv_torn':
            image[region.offset + 40] = 0
        elif case == 'fv_with_data':
            image[region.offset + 0x100] = 0
        elif case == 'fv_with_spare_data':
            image[region.offset + region.length - 1] = 0
        fault_image = output_dir / ('provisioning_' + case + '.bin')
        fault_image.write_bytes(image)
        marker = None if case == 'fv_only' else 'Volume Corrupt'
        text = run_shell_session(str(fault_image), [], completion_marker=marker,
                                 log_path=output_dir / ('provisioning_' + case + '.log'))
        if case == 'fv_only':
            require(text, 'UVAR completing interrupted FV-only provisioning')
            if check_absent_tokens(text, ['ASSERT', 'Device Error', 'Volume Corrupt']) != 0:
                raise RuntimeError('FV-only provisioning recovery failed')
        else:
            require(text, 'Volume Corrupt')
            if fault_image.read_bytes() != image:
                raise RuntimeError('Provisioning modified a protected non-erased image: ' + case)
        print('Provisioning recovery case PASSED: ' + case)


def run_streaming_reclaim_tests(test_image, output_dir, sbl_dir):
    run_provisioning_tests(test_image, output_dir, sbl_dir)
    original = bytearray(test_image.read_bytes())
    region = get_uvar(original, sbl_dir)
    reserved = get_reserved(original, sbl_dir)
    records, tail = variable_records(original, region)
    for name, header, data_start, _ in records:
        if name == 'VARTST0' and header[1] == 0x3F:
            struct.pack_into('<I', original, data_start, 0xAABBCCDD)
    seeds = (variable_record('SharedValue', bytes.fromhex('deadbeef')) +
             variable_record('SharedValue', bytes.fromhex('01020304')) +
             variable_record('DeletedValue', b'deleted', state=0x3D) +
             variable_record('AddedBeforeTransition', b'committed') +
             variable_record('AddedBeforeTransition', b'fallback', state=0x3E) +
             variable_record('AddedAfterTransition', b'fallback', state=0x3E) +
             variable_record('AddedAfterTransition', b'committed') +
             variable_record('SharedAppend', bytes.fromhex('1122334455667788')))
    original[tail:tail + len(seeds)] = seeds
    tail += len(seeds)
    end = region.offset + 0x1E000
    name = 'ReclaimPadding\0'.encode('utf-16-le')
    data_size = end - tail - AUTH_HEADER.size - len(name)
    if data_size < 0:
        raise RuntimeError('No space to construct full-tail test')
    padding = AUTH_HEADER.pack(0x55AA, 0x3D, 0, 7, 0, bytes(16), 0,
                               len(name), data_size, uuid.UUID(VARIABLE_GUID).bytes_le)
    original[tail:end] = padding + name + bytes(data_size)
    original[reserved.offset + 0xFE0:reserved.offset + 0x1000] = bytes(range(32))
    firmware_status = bytes(original[reserved.offset:reserved.offset + 0x1000])
    recovered = None
    for case in ['full_tail', 'torn_header', 'middle_dirty_tail', 'header_only', 'transition', 'added',
                 'uncommitted_spare', 'workspace_reclaim', 'append_reclaim', 'delete_replacement']:
        image = bytearray(original)
        if case in ['torn_header', 'header_only', 'transition', 'added']:
            image[tail:end] = b'\xFF' * (end - tail)
            pending = variable_record('VARTST0', struct.pack('<I', 0x55667788),
                                      state=0x3F if case == 'added' else 0x7F)
            if case == 'torn_header':
                pending = pending[:3]
            image[tail:tail + len(pending)] = pending
            if case in ['transition', 'added']:
                for record_name, header, _, offset in records:
                    if record_name == 'VARTST0' and header[1] == 0x3F:
                        image[offset + 2] = 0x3E
            if case == 'transition':
                image[tail + len(pending):tail + len(pending) + 3] = b'\xAA\x55\xFF'
        elif case == 'middle_dirty_tail':
            image[tail:end] = b'\xFF' * (end - tail)
            image[end - 16:end] = bytes(16)
        elif case == 'uncommitted_spare':
            spare = region.offset + 0x20000
            image[spare:spare + 0x20000] = image[region.offset:spare]
            image[spare + 20] = 0xFF
        elif case == 'workspace_reclaim':
            completed = (struct.pack('<B3x16s4xQQ', 0xF8, uuid.UUID(VARIABLE_GUID).bytes_le, 1, 0) +
                         struct.pack('<B7xQQQq', 0xF9, 0, 72, 0x1DFB8, -0x20000))
            queue_start = region.offset + 0x1E000 + 32
            image[queue_start:region.offset + 0x20000] = completed * 102
        elif case in ['append_reclaim', 'delete_replacement']:
            image[tail:end] = b'\xFF' * (end - tail)
            if case == 'append_reclaim':
                padding = variable_record('ReclaimPadding', bytes(data_size - 0x300), state=0x3D)
                image[tail:tail + len(padding)] = padding
            else:
                pending = (variable_record('STREAMAPPEND', b'old', state=0x3E) +
                           variable_record('STREAMAPPEND', b'new') +
                           variable_record('DELPRED', b'old', state=0x3E) +
                           variable_record('DELPRED', b'new'))
                image[tail:tail + len(pending)] = pending
            for record_name, header, data_start, _ in records:
                if record_name == 'VARTST0' and header[1] == 0x3F:
                    struct.pack_into('<I', image, data_start, 0x55667788)
        path = output_dir / ('streaming_' + case + '.bin')
        path.write_bytes(image)
        text = run_shell_session(str(path), [], log_path=output_dir / ('streaming_' + case + '.log'))
        expected = '0x55667788' if case in ['added', 'append_reclaim', 'delete_replacement'] else '0xAABBCCDD'
        require(text, 'initial GetVariable status=Success data=' + expected + ' size=4')
        if case in ['full_tail', 'torn_header', 'middle_dirty_tail', 'transition', 'uncommitted_spare',
                    'workspace_reclaim']:
            require(text, 'UVAR streaming reclaim: Success')
            if text.index('UVAR streaming reclaim: Success') > text.index('Memory Init'):
                raise RuntimeError('Expected reclaim before memory initialization: ' + case)
        require(text, 'TestVariableService: streaming append status=Success')
        require(text, 'TestVariableService: absent zero-length append status=Success')
        if case == 'append_reclaim':
            require(text, 'UVAR streaming reclaim: Success')
            if text.index('UVAR streaming reclaim: Success') < text.index('Memory Init'):
                raise RuntimeError('Expected append-driven Stage2 reclaim')
        if check_absent_tokens(text, ['ASSERT', 'Device Error']) != 0:
            raise RuntimeError('Streaming reclaim failed: ' + case)
        result_bytes = path.read_bytes()
        check_reclaim_result(result_bytes, region, reserved, firmware_status)
        if case == 'workspace_reclaim':
            queue_start = region.offset + 0x1E000 + 32
            if (result_bytes[queue_start] != 0xF8 or result_bytes[queue_start + 40] != 0xF9 or
                    result_bytes[queue_start + 80] != 0xFF):
                raise RuntimeError('FTW workspace reclaim did not compact completed queue entries')
        if case == 'delete_replacement':
            remaining, _ = variable_records(result_bytes, region)
            if any(name in ['STREAMAPPEND', 'DELPRED'] and header[1] in [0x3E, 0x3F]
                   for name, header, _, _ in remaining):
                raise RuntimeError('Deleted or replaced variable left active predecessor records')
        if case == 'full_tail':
            recovered = result_bytes
        print('Streaming reclaim case PASSED: ' + case)
    for case in ['erased', 'partial', 'repeated_partial', 'stale_spare']:
        image = bytearray(recovered)
        spare_start = region.offset + 0x20000
        workspace_start = region.offset + 0x1E000
        if case == 'stale_spare':
            image[workspace_start + 20] = 0
            path = output_dir / ('streaming_recovery_' + case + '.bin')
            path.write_bytes(image)
            text = run_shell_session(str(path), [], completion_marker='Volume Corrupt',
                                     log_path=output_dir / ('streaming_recovery_' + case + '.log'))
            require(text, 'Volume Corrupt')
            if path.read_bytes() != image:
                raise RuntimeError('Corrupt workspace rolled back to stale spare contents')
            print('Streaming recovery case PASSED: ' + case)
            continue
        image[workspace_start + 32] = 0xFC
        image[workspace_start + 72] = 0xFD
        image[region.offset:workspace_start] = b'\xFF' * 0x1E000
        if case != 'erased':
            copy_size = 0x1000 if case == 'partial' else 0x10000
            image[region.offset:region.offset + copy_size] = image[spare_start:spare_start + copy_size]
        path = output_dir / ('streaming_recovery_' + case + '.bin')
        path.write_bytes(image)
        text = run_shell_session(str(path), [], log_path=output_dir / ('streaming_recovery_' + case + '.log'))
        require(text, 'UVAR FTW: replayed pending variable transaction')
        require(text, 'initial GetVariable status=Success data=0x11223344 size=4')
        if check_absent_tokens(text, ['ASSERT', 'Device Error']) != 0:
            raise RuntimeError('Interrupted streaming reclaim failed: ' + case)
        check_reclaim_result(path.read_bytes(), region, reserved, firmware_status)
        reboot = run_shell_session(str(path), [], log_path=output_dir / ('streaming_reboot_' + case + '.log'))
        require(reboot, 'initial GetVariable status=Success data=0x55667788 size=4')
        if 'UVAR FTW: replayed' in reboot or 'UVAR FTW: restored' in reboot:
            raise RuntimeError('Completed recovery was replayed again')
        print('Streaming recovery case PASSED: ' + case)
    for case in ['live_full_update', 'live_full_shrink', 'live_full_missing_marker']:
        image = bytearray(original)
        marker_data = (bytes(0x10000 - AUTH_HEADER.size - len('VARTST0\0'.encode('utf-16-le')))
                       if case == 'live_full_shrink' else struct.pack('<I', 0xAABBCCDD))
        marker = variable_record('VARTST0', marker_data) if case != 'live_full_missing_marker' else b''
        full_start = region.offset + 100
        live = bytearray(marker)
        while len(live) < end - full_start:
            name = 'LiveCapacity' + str(len(live))
            entry_size = min(0x10000, end - full_start - len(live))
            live.extend(variable_record(name, bytes(entry_size - len(variable_record(name, b'')))))
        image[full_start:end] = live
        path = output_dir / ('streaming_' + case + '.bin')
        path.write_bytes(image)
        text = run_shell_session(str(path), [], log_path=output_dir / ('streaming_' + case + '.log'),
                     shell_timeout=240)
        if case == 'live_full_missing_marker':
            require(text, 'TestVariableService: SetVariable status=Out of Resources')
            require(text, 'Stage1B variable test skipped: insufficient variable-store resources')
            if path.read_bytes() != image:
                raise RuntimeError('Capacity preflight modified a full live store: ' + case)
        else:
            require(text, 'TestVariableService: SetVariable status=Success')
            require(text, 'UVAR streaming reclaim: Success')
            result, _ = variable_records(path.read_bytes(), region)
            markers = [header for name, header, _, _ in result
                       if name == 'VARTST0' and header[1] in [0x3E, 0x3F]]
            if len(markers) != 1 or markers[0][8] != 4:
                raise RuntimeError('Full-store replacement did not retain one small marker: ' + case)
        require(text, 'Memory Init')
        if check_absent_tokens(text, ['ASSERT', 'Device Error']) != 0:
            raise RuntimeError('Full variable store prevented normal boot: ' + case)
        print('Streaming full-live-store preflight PASSED: ' + case)
    test_image.write_bytes(recovered)


def run_duplicate_handoff_test(test_image, output_dir, sbl_dir):
    image = bytearray(test_image.read_bytes())
    region = get_uvar(image, sbl_dir)
    records, tail = variable_records(image, region)
    for name, header, _, offset in records:
        if name == 'VARTST0':
            image[offset + 2] = 0x3D
    duplicates = (variable_record('VARTST0', struct.pack('<I', 0xAABBCCDD)) +
                  variable_record('VARTST0', struct.pack('<I', 0x55667788)))
    if image[tail:tail + len(duplicates)] != b'\xFF' * len(duplicates):
        raise RuntimeError('No erased tail for duplicate handoff test')
    image[tail:tail + len(duplicates)] = duplicates
    path = output_dir / 'duplicate_handoff.bin'
    path.write_bytes(image)
    sbl = run_shell_session(str(path), [], log_path=output_dir / 'duplicate_handoff_sbl.log')
    require(sbl, 'initial GetVariable status=Success data=0xAABBCCDD size=4')
    if 'UVAR streaming reclaim' in sbl:
        raise RuntimeError('Duplicate handoff test unexpectedly reclaimed the store')
    records, _ = variable_records(path.read_bytes(), region)
    if sum(name == 'VARTST0' and header[1] in [0x3E, 0x3F]
           for name, header, _, _ in records) != 1:
        raise RuntimeError('Duplicate handoff left multiple active marker records')
    disk = output_dir / 'duplicate_disk'
    disk.mkdir(exist_ok=True)
    commands = ['echo -off', 'setvar VARTST0 -guid ' + VARIABLE_GUID,
                'echo DUPLICATE_HANDOFF_DONE']
    (disk / 'startup.nsh').write_text('\n'.join(commands) + '\n', encoding='ascii')
    uefi = run_shell_session(str(path), [], uefi=True, disk_dir=str(disk),
                            completion_marker='DUPLICATE_HANDOFF_DONE',
                            log_path=output_dir / 'duplicate_handoff_uefi.log')
    require(uefi, '88 77 66 55')
    if check_absent_tokens(uefi, ['Unable to get', 'ASSERT', 'Device Error']) != 0:
        raise RuntimeError('Duplicate handoff UEFI read failed')
    print('First-ADDED selection and no-reclaim duplicate update handoff PASSED')


def main():
    if len(sys.argv) != 4:
        print('usage: unified_variable_test.py bios_image uefi_payload out_dir')
        return 2

    bios_image, payload, output_dir = [Path(argument).resolve() for argument in sys.argv[1:]]
    output_dir.mkdir(parents=True, exist_ok=True)
    sbl_dir = Path(__file__).resolve().parents[4]
    get_uvar(bios_image.read_bytes(), sbl_dir)
    test_image = output_dir / 'UnifiedVariable.bin'
    shutil.copyfile(bios_image, test_image)
    shutil.copyfile(payload, output_dir / 'UefiPayload.bin')
    os.environ['SBL_KEY_DIR'] = str(sbl_dir.parent / 'SblKeys')
    layout = [
        "('EPLD', 'EPAYLOAD.bin', 'NORMAL', 'RSA3072_PSS_SHA2_384', 'KEY_ID_CONTAINER_RSA3072', 0x10, 0, 0)",
        "('UEFI', 'UefiPayload.bin', 'Lzma', 'SHA2_384', '', 0x10, 0, 0)",
    ]
    layout_path = output_dir / 'epld.txt'
    layout_path.write_text(',\n'.join(layout), encoding='ascii')
    subprocess.run([
        sys.executable, str(sbl_dir / 'BootloaderCorePkg/Tools/GenContainer.py'),
        'create', '-l', str(layout_path), '-td', get_tool_dir(str(sbl_dir)),
    ], cwd=output_dir, check=True)
    subprocess.run([
        sys.executable, str(sbl_dir / 'BootloaderCorePkg/Tools/IfwiUtility.py'),
        'replace', '-i', str(test_image), '-f', str(output_dir / 'EPAYLOAD.bin'),
        '-p', 'IFWI/BIOS/NRD/EPLD',
    ], check=True)

    first = run_shell_session(str(test_image), [], log_path=output_dir / 'sbl_initial.log')
    require(first, 'TestVariableService: SetVariable status=Success')
    require(first, 'TestVariableService: zero-length append status=Success')
    run_streaming_reclaim_tests(test_image, output_dir, sbl_dir)
    run_duplicate_handoff_test(test_image, output_dir, sbl_dir)
    commands = ['setvar VARTST0 -guid ' + VARIABLE_GUID]
    commands.extend(['setvar SharedValue -guid ' + VARIABLE_GUID,
                     'setvar SharedAppend -guid ' + VARIABLE_GUID])
    for index in range(450):
        value = (struct.pack('<I', index) + bytes(252)).hex()
        commands.append('setvar UVARSTRESS -guid ' + VARIABLE_GUID + ' -nv -bs -rt =' + value)
    commands.extend([
        'setvar VARTST0 -guid ' + VARIABLE_GUID + ' =44332211',
        'setvar VARTST0 -guid ' + VARIABLE_GUID,
    ])
    disk_dir = output_dir / 'uefi_disk'
    disk_dir.mkdir(exist_ok=True)
    commands.insert(0, 'echo -off')
    commands.append('echo UNIFIED_UEFI_STRESS_DONE')
    (disk_dir / 'startup.nsh').write_text('\n'.join(commands) + '\n', encoding='ascii')
    uefi = run_shell_session(str(test_image), [], uefi=True,
                            log_path=output_dir / 'uefi_exchange.log',
                            disk_dir=str(disk_dir), completion_marker='UNIFIED_UEFI_STRESS_DONE')
    require(uefi, '88 77 66 55')
    require(uefi, '44 33 22 11')
    require(uefi, 'Ftw: Write() success')
    require(uefi, 'DE AD BE EF')
    require(uefi, '11 22 33 44 55 66 77 88')
    if check_absent_tokens(uefi, ['Unable to set', 'Unable to get', 'ASSERT', 'Device Error']) != 0:
        raise RuntimeError('UEFI variable exchange failed')

    final = run_shell_session(str(test_image), [], log_path=output_dir / 'sbl_after_uefi.log')
    require(final, 'initial GetVariable status=Success data=0x11223344 size=4')
    require(final, 'TestVariableService: zero-length append status=Success')
    require(final, 'TestVariableService: boot-services persistence status=Success')
    print('Unified variable SBL -> UEFI -> SBL persistence and UEFI reclaim test PASSED')
    run_ftw_fault_tests(test_image, output_dir, sbl_dir)
    return 0


if __name__ == '__main__':
    sys.exit(main())