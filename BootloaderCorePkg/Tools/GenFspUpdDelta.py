#!/usr/bin/env python
## @file
#  Generate FSP UPD per-board delta blobs for embedding in CFGDATA tags.
#
#  This script produces one delta blob per board per FSP phase.
#  Each blob is a self-contained payload suitable for injection
#  into a CFGDATA tag.
#
#  Tag assignment:  tag = base_tag + PlatformId
#    FSP-M base: CDATA_FSPM_DELTA_BASE_TAG (0x390 = same as FSPM UPD tag)
#    FSP-S base: CDATA_FSPS_DELTA_BASE_TAG (0x3A0 = same as FSPS UPD tag)
#
#  Blob layout (per-board, no header):
#    [Delta entries - packed]    (Offset:2 + Length:1 + Value[Length]) x N
#    [Padding to DWORD align]
#
#  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
#  SPDX-License-Identifier: BSD-2-Clause-Patent
#
##

import os
import re
import sys
import struct
import argparse
import subprocess
import tempfile

# FSP Info Header is at fixed offset 0x94 in each FSP component binary
FSP_INFO_HEADER_OFF = 0x94
FIH_CFGREGION_OFF_OFF  = 0x24
FIH_CFGREGION_SIZE_OFF = 0x28

# No blob header — the CFGDATA tag payload starts directly with packed
# FSPUPD_DELTA_ENTRY records.  Each entry is: Offset(2B) + Length(1B) + Value[Length].

# CFGDATA tag base IDs - same as the UPD defaults tags (must match ConfigDataLib.c)
CDATA_FSPM_DELTA_BASE_TAG = 0x500
CDATA_FSPS_DELTA_BASE_TAG = 0x520


def get_fsp_cfg_region(fsp_bin_path):
    """Extract the CfgRegion data from an FSP component binary."""
    with open(fsp_bin_path, 'rb') as f:
        data = f.read()
    h = FSP_INFO_HEADER_OFF
    cfg_offset = struct.unpack_from('<I', data, h + FIH_CFGREGION_OFF_OFF)[0]
    cfg_size   = struct.unpack_from('<I', data, h + FIH_CFGREGION_SIZE_OFF)[0]
    if cfg_size == 0:
        raise ValueError(f"CfgRegionSize is 0 in {fsp_bin_path}")
    cfg_data = data[cfg_offset:cfg_offset + cfg_size]
    if len(cfg_data) != cfg_size:
        raise ValueError(f"CfgRegion extends beyond file in {fsp_bin_path}")
    return bytearray(cfg_data), cfg_size


# The CfgHeader + CondValue from the CFGHDR_TMPL in the YAML definition
# are the SAME bytes as the outer CDATA_HEADER + CDATA_COND parsed by
# extract_tag_data_from_cfgdata_bin().  That function already skips them,
# so no additional inner-header stripping is needed (inner hdr size = 0).
CDATA_INNER_HDR_SIZE = 0


def get_fsp_config_base_offset(fsp_header_path, struct_name):
    """Parse an FSP UPD header to find the starting offset of the config struct.

    Scans for the first '/** Offset 0xNNNN' comment inside the named struct
    (e.g. FSP_M_CONFIG or FSP_S_CONFIG).  Returns the integer offset value.

    Args:
        fsp_header_path:  Path to FspmUpd.h or FspsUpd.h
        struct_name:      'FSP_M_CONFIG' or 'FSP_S_CONFIG'

    Returns:
        Integer offset of the first field (e.g. 0x60 for FSP_M_CONFIG).
    """
    with open(fsp_header_path, 'r', encoding='utf-8', errors='replace') as f:
        text = f.read()

    # Find the struct body: `typedef struct { ... } FSP_M_CONFIG;`
    # Look for the closing `} STRUCT_NAME;` then scan backwards for `typedef struct {`
    close_pat = re.compile(r'}\s+' + re.escape(struct_name) + r'\s*;')
    m_close = close_pat.search(text)
    if not m_close:
        raise ValueError(f"Could not find '}} {struct_name};' in {fsp_header_path}")

    # Extract the text region of the struct body
    struct_body = text[:m_close.start()]

    # Find the first `/** Offset 0xNNNN` comment inside the struct
    offset_pat = re.compile(r'/\*\*\s+Offset\s+(0x[0-9A-Fa-f]+)')
    # Scan from the end of struct_body backwards to find the typedef,
    # then forward to get the first Offset comment
    typedef_pos = struct_body.rfind('typedef struct')
    if typedef_pos < 0:
        raise ValueError(f"Could not find 'typedef struct' for {struct_name}")

    body_text = struct_body[typedef_pos:]
    m_offset = offset_pat.search(body_text)
    if not m_offset:
        raise ValueError(f"No 'Offset 0x...' comment found in {struct_name}")

    return int(m_offset.group(1), 16)


def gen_board_cfgdata_bin(dlt_file, pkl_file, output_bin, gen_cfg_data_tool):
    """Use GenCfgData.py to generate a per-board CFGDATA binary."""
    cmd = [sys.executable, gen_cfg_data_tool, 'GENBIN',
           '%s;%s' % (pkl_file, dlt_file), output_bin]
    ret = subprocess.call(cmd, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
    if ret != 0:
        raise RuntimeError(f"GenCfgData GENBIN failed for {dlt_file}")


def extract_tag_data_from_cfgdata_bin(cfgdata_bin_path, tag_id):
    """Extract raw payload for a given tag from a CFGDATA binary blob."""
    with open(cfgdata_bin_path, 'rb') as f:
        data = f.read()

    if data[0:4] != b'CFGD':
        raise ValueError(f"Invalid CFGDATA signature in {cfgdata_bin_path}")
    header_len  = data[4]
    used_length = struct.unpack_from('<I', data, 8)[0]

    offset = header_len
    while offset < used_length:
        word0 = struct.unpack_from('<I', data, offset)[0]
        cond_num   = word0 & 0x3
        length_dw  = (word0 >> 2) & 0x7FF
        tag        = (word0 >> 20) & 0xFFF

        entry_len_bytes = length_dw * 4
        if entry_len_bytes == 0:
            break

        hdr_size       = 4 + (cond_num * 4)
        payload_offset = offset + hdr_size
        payload_size   = entry_len_bytes - hdr_size

        if tag == tag_id and payload_size > 0:
            return bytearray(data[payload_offset:payload_offset + payload_size])

        offset += entry_len_bytes

    return None


def compute_deltas(base_data, board_data):
    """Compare base vs board bytes and produce delta entries.

    Consecutive differing bytes are merged into single entries.
    Returns list of (offset, length, value_bytes).
    """
    assert len(base_data) == len(board_data)
    deltas = []
    size = len(base_data)
    i = 0
    while i < size:
        if base_data[i] != board_data[i]:
            start = i
            while i < size and base_data[i] != board_data[i]:
                i += 1
            run_len = i - start
            while run_len > 0:
                chunk = min(run_len, 255)
                value = bytes(board_data[start:start + chunk])
                deltas.append((start, chunk, value))
                start += chunk
                run_len -= chunk
        else:
            i += 1
    return deltas


def parse_dlt_platform_id(dlt_file):
    """Extract PlatformId from a DLT file."""
    with open(dlt_file, 'r') as f:
        for line in f:
            line = line.strip()
            if line.startswith('#') or '|' not in line:
                continue
            if 'PLATFORMID_CFG_DATA.PlatformId' in line:
                parts = line.split('|')
                return int(parts[1].strip(), 0)
    return None


def build_per_board_blob(deltas):
    """Build a per-board delta blob (packed entries only, no header).

    Layout:
        [delta entries - packed]  Offset(2B) + Length(1B) + Value[Length]
        [padding to DWORD align]
    """
    output = bytearray()
    for offset, length, value in deltas:
        output += struct.pack('<HB', offset, length)
        output += value
    # DWORD-align
    while len(output) % 4 != 0:
        output += b'\x00'
    return output


def gen_fsp_upd_delta_blobs(fspm_bin, fsps_bin, output_dir,
                             dlt_files, yaml_file, gen_cfg_data_tool,
                             fspm_header=None, fsps_header=None):
    """Generate per-board FSP-M and FSP-S delta blobs (v4 format).

    Each board gets its own pair of files (one for FSP-M, one for FSP-S).
    Tag assignment:  tag = base_tag + PlatformId

    The CFGDATA tag payload returned by extract_tag_data_from_cfgdata_bin()
    is pure config field data (the outer CfgHeader + CondValue are already
    stripped during extraction).  The config field data covers only
    FSP_M_CONFIG (starting at offset fspm_cfg_base within the full
    FSPM_UPD) or FSP_S_CONFIG (at fsps_cfg_base within FSPS_UPD).
    To compute correct deltas we must:
      1. Overlay the config field data onto a copy of the FSP defaults
         at the correct base offset
      2. Then compare the full UPD buffers
    """

    # 1. Extract FSP defaults (full FSPM_UPD / FSPS_UPD)
    fspm_defaults, fspm_size = get_fsp_cfg_region(fspm_bin)
    fsps_defaults, fsps_size = get_fsp_cfg_region(fsps_bin)
    print("  FSP-M CfgRegion: 0x%X bytes" % fspm_size)
    print("  FSP-S CfgRegion: 0x%X bytes" % fsps_size)

    # 2. Determine config struct base offsets from FSP headers
    if fspm_header and os.path.exists(fspm_header):
        fspm_cfg_base = get_fsp_config_base_offset(fspm_header, 'FSP_M_CONFIG')
    else:
        fspm_cfg_base = 0x60   # Common FSP2.x default for FSPM_UPD
        print("  WARNING: No FSP-M header provided, assuming FSP_M_CONFIG at 0x%X" % fspm_cfg_base)
    if fsps_header and os.path.exists(fsps_header):
        fsps_cfg_base = get_fsp_config_base_offset(fsps_header, 'FSP_S_CONFIG')
    else:
        fsps_cfg_base = 0x40   # Common FSP2.x default for FSPS_UPD
        print("  WARNING: No FSP-S header provided, assuming FSP_S_CONFIG at 0x%X" % fsps_cfg_base)
    print("  FSP-M config base offset: 0x%X" % fspm_cfg_base)
    print("  FSP-S config base offset: 0x%X" % fsps_cfg_base)

    # Resolve pkl file
    if yaml_file.endswith('.pkl'):
        pkl_file = yaml_file
    else:
        pkl_file = os.path.join(os.path.dirname(yaml_file) or '.', 'CfgDataDef.pkl')
        if not os.path.exists(pkl_file):
            cmd = [sys.executable, gen_cfg_data_tool, 'GENPKL', yaml_file, pkl_file]
            ret = subprocess.call(cmd, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
            if ret != 0:
                raise RuntimeError("Failed to generate pkl from %s" % yaml_file)

    # CFGDATA tag IDs (must match YAML definitions)
    CDATA_FSPM_UPD_TAG  = 0x500
    CDATA_FSPS_UPD_TAG  = 0x520

    # 3. Process each board DLT — produce one pair of blobs per board
    results = []  # list of (platform_id, fspm_tag, fspm_size, fsps_tag, fsps_size, fspm_path, fsps_path)

    with tempfile.TemporaryDirectory() as tmpdir:
        for dlt_file in dlt_files:
            dlt_name = os.path.basename(dlt_file)
            platform_id = parse_dlt_platform_id(dlt_file)
            if platform_id is None:
                print("  WARNING: No PlatformId in %s, skipping" % dlt_name)
                continue

            print("  Board PlatformId=0x%02X (%s)" % (platform_id, dlt_name))

            # Generate per-board CFGDATA binary
            board_bin = os.path.join(tmpdir, os.path.splitext(dlt_name)[0] + '.bin')
            try:
                gen_board_cfgdata_bin(dlt_file, pkl_file, board_bin, gen_cfg_data_tool)
            except RuntimeError as e:
                print("    WARNING: %s — skipping" % e)
                continue

            # --- FSP-M deltas ---
            fspm_board = extract_tag_data_from_cfgdata_bin(board_bin, CDATA_FSPM_UPD_TAG)
            if fspm_board is not None:
                # Payload is already stripped of CfgHeader + CondValue by extract_tag_data_from_cfgdata_bin
                cfg_fields = fspm_board[CDATA_INNER_HDR_SIZE:]
                # Reconstruct full FSPM_UPD: start with defaults, overlay config fields
                board_m = bytearray(fspm_defaults)
                cfg_end = fspm_cfg_base + len(cfg_fields)
                if cfg_end > fspm_size:
                    cfg_fields = cfg_fields[:fspm_size - fspm_cfg_base]
                board_m[fspm_cfg_base:fspm_cfg_base + len(cfg_fields)] = cfg_fields
                m_deltas = compute_deltas(fspm_defaults, board_m)
                print("    FSP-M: %d delta entries (config fields: %d bytes at offset 0x%X)" %
                      (len(m_deltas), len(cfg_fields), fspm_cfg_base))
            else:
                m_deltas = []
                print("    FSP-M: no tag data (0 deltas)")

            fspm_blob = build_per_board_blob(m_deltas)
            fspm_tag = CDATA_FSPM_DELTA_BASE_TAG + platform_id
            fspm_path = os.path.join(output_dir, 'FspmDelta_0x%03X.bin' % fspm_tag)
            with open(fspm_path, 'wb') as f:
                f.write(fspm_blob)

            # --- FSP-S deltas ---
            fsps_board = extract_tag_data_from_cfgdata_bin(board_bin, CDATA_FSPS_UPD_TAG)
            if fsps_board is not None:
                # Payload is already stripped of CfgHeader + CondValue by extract_tag_data_from_cfgdata_bin
                cfg_fields = fsps_board[CDATA_INNER_HDR_SIZE:]
                # Reconstruct full FSPS_UPD: start with defaults, overlay config fields
                board_s = bytearray(fsps_defaults)
                cfg_end = fsps_cfg_base + len(cfg_fields)
                if cfg_end > fsps_size:
                    cfg_fields = cfg_fields[:fsps_size - fsps_cfg_base]
                board_s[fsps_cfg_base:fsps_cfg_base + len(cfg_fields)] = cfg_fields
                s_deltas = compute_deltas(fsps_defaults, board_s)
                print("    FSP-S: %d delta entries (config fields: %d bytes at offset 0x%X)" %
                      (len(s_deltas), len(cfg_fields), fsps_cfg_base))
            else:
                s_deltas = []
                print("    FSP-S: no tag data (0 deltas)")

            fsps_blob = build_per_board_blob(s_deltas)
            fsps_tag = CDATA_FSPS_DELTA_BASE_TAG + platform_id
            fsps_path = os.path.join(output_dir, 'FspsDelta_0x%03X.bin' % fsps_tag)
            with open(fsps_path, 'wb') as f:
                f.write(fsps_blob)

            results.append((platform_id, fspm_tag, len(fspm_blob),
                            fsps_tag, len(fsps_blob), fspm_path, fsps_path))
            print("    Tags: 0x%03X (%dB), 0x%03X (%dB)" %
                  (fspm_tag, len(fspm_blob), fsps_tag, len(fsps_blob)))

    print("\nGenerated %d boards, %d delta blobs in %s" %
          (len(results), len(results) * 2, output_dir))
    return results


def inject_delta_tags_into_cfgdata(bin_file, delta_blobs):
    """Inject per-board FSP UPD delta blobs as CDATA tags into CFGDATA.bin.

    This is the common implementation used by BuildLoader.py after delta
    blob generation.  It can also be invoked via the --inject CLI flag.

    Args:
        bin_file:     Path to CFGDATA.bin to patch in-place.
        delta_blobs:  List of (tag_id, blob_bytes) tuples.
    """
    data = bytearray(open(bin_file, 'rb').read())
    if len(data) < 16 or data[:4] != b'CFGD':
        raise Exception('Invalid CFGDATA.bin: bad signature')

    hdr_len = data[4]
    used    = int.from_bytes(data[8:12], 'little')
    total   = int.from_bytes(data[12:16], 'little')

    for tag, blob in delta_blobs:
        if blob is None:
            continue
        # DWORD-align payload
        payload = bytearray(blob)
        while len(payload) % 4 != 0:
            payload.append(0)

        # CDATA_HEADER (4 bytes) + CDATA_COND (4 bytes) + payload
        # Length field = total size in DWORDs (header + cond + payload)
        cond_num = 1
        tag_total_bytes = 4 + 4 * cond_num + len(payload)
        length_dwords = tag_total_bytes // 4

        # word0: ConditionNum[1:0] | Length[12:2] | Flags[15:13] | Version[19:16] | Tag[31:20]
        word0 = (cond_num & 0x3) | ((length_dwords & 0x7FF) << 2) | ((tag & 0xFFF) << 20)
        # CDATA_COND = 0xFFFFFFFF (unconditional / all platforms)
        cond_val = 0xFFFFFFFF

        tag_data = word0.to_bytes(4, 'little') + cond_val.to_bytes(4, 'little') + payload
        # Insert into the used area
        insert_off = used
        data[insert_off:insert_off+len(tag_data)] = tag_data
        used += len(tag_data)
        print('Injected CDATA tag 0x%03X: %d bytes payload, total tag %d bytes at offset 0x%X' %
              (tag, len(blob), len(tag_data), insert_off))

    if used > total:
        # Grow TotalLength to accommodate the injected data
        total = used

    # Update UsedLength and TotalLength
    data[8:12]  = used.to_bytes(4, 'little')
    data[12:16] = total.to_bytes(4, 'little')
    open(bin_file, 'wb').write(data)
    print('Updated CfgData UsedLength=0x%X TotalLength=0x%X' % (used, total))


def collect_delta_blobs(delta_dir):
    """Scan delta_dir for generated FspXDelta_0xNNN.bin files.

    Returns:
        List of (tag_id, blob_bytes) tuples sorted by filename.
    """
    import glob as _glob
    delta_blobs = []
    for delta_file in sorted(_glob.glob(os.path.join(delta_dir, 'Fsp*Delta_0x*.bin'))):
        tag_str = os.path.splitext(os.path.basename(delta_file))[0].split('_0x')[-1]
        tag_id = int(tag_str, 16)
        blob = open(delta_file, 'rb').read()
        delta_blobs.append((tag_id, blob))
    return delta_blobs


def gen_and_inject_fsp_upd_deltas(fv_dir, brd_cfg_dir, fsp_path, cfg_bin_path,
                                  dlt_files=None):
    """Generate FSP UPD per-board delta blobs and inject into CFGDATA.

    When dlt_files is provided, only those DLT files are processed.
    Otherwise, auto-discovers DLT files in brd_cfg_dir containing
    FSPM_UPD or FSPS_UPD overrides.

    Args:
        fv_dir:       Build FV output directory (contains FSP_M.bin,
                      FSP_S.bin, CfgDataDef.pkl).
        brd_cfg_dir:  Board CfgData source directory containing DLT files.
        fsp_path:     Directory containing FspmUpd.h / FspsUpd.h.
        cfg_bin_path: Path to the unsigned merged CFGDATA binary to patch.
        dlt_files:    Optional list of DLT filenames (resolved in fv_dir
                      first, then brd_cfg_dir).  When None, all DLT files
                      in brd_cfg_dir are scanned.
    """
    import glob as _glob
    import shutil

    fspm_bin  = os.path.join(fv_dir, 'FSP_M.bin')
    fsps_bin  = os.path.join(fv_dir, 'FSP_S.bin')
    delta_dir = os.path.join(fv_dir, 'FspUpdDeltas')
    pkl_file  = os.path.join(fv_dir, 'CfgDataDef.pkl')
    gen_cfg_data_tool = os.path.join('BootloaderCorePkg', 'Tools',
                                     'GenCfgData.py')

    if dlt_files is not None:
        # Resolve provided filenames: check fv_dir first, then brd_cfg_dir
        dlt_paths = []
        for name in dlt_files:
            p = os.path.join(fv_dir, name)
            if not os.path.exists(p):
                p = os.path.join(brd_cfg_dir, name)
            if os.path.exists(p):
                try:
                    content = open(p, 'r').read()
                except Exception:
                    continue
                if 'FSPM_UPD_CFG_DATA' in content or \
                   'FSPS_UPD_CFG_DATA' in content:
                    dlt_paths.append(p)
    else:
        # Auto-discover DLT files containing FSP UPD overrides
        dlt_paths = []
        for dlt_file in sorted(_glob.glob(os.path.join(brd_cfg_dir, '*.dlt'))):
            try:
                content = open(dlt_file, 'r').read()
            except Exception:
                continue
            if 'FSPM_UPD_CFG_DATA' in content or \
               'FSPS_UPD_CFG_DATA' in content:
                dlt_paths.append(dlt_file)

    if not dlt_paths:
        return

    print('  Found %d DLT files with FSP UPD overrides' % len(dlt_paths))

    # Clean stale delta files from previous builds
    if os.path.isdir(delta_dir):
        shutil.rmtree(delta_dir)
    os.makedirs(delta_dir)

    fspm_hdr = os.path.join(fsp_path, 'FspmUpd.h')
    fsps_hdr = os.path.join(fsp_path, 'FspsUpd.h')

    gen_fsp_upd_delta_blobs(fspm_bin, fsps_bin, delta_dir,
                            dlt_paths, pkl_file, gen_cfg_data_tool,
                            fspm_header=fspm_hdr,
                            fsps_header=fsps_hdr)

    delta_blobs = collect_delta_blobs(delta_dir)
    if delta_blobs:
        inject_delta_tags_into_cfgdata(cfg_bin_path, delta_blobs)


def main():
    parser = argparse.ArgumentParser(
        description='Generate FSP UPD per-board delta blobs (v4)')
    parser.add_argument('fspm_bin', help='Path to FSP_M.bin')
    parser.add_argument('fsps_bin', help='Path to FSP_S.bin')
    parser.add_argument('-o', '--output-dir', required=True,
                        help='Directory for output delta blobs')
    parser.add_argument('-d', '--dlt', action='append', required=True,
                        help='Per-board DLT file (repeat for each board)')
    parser.add_argument('-y', '--yaml', required=True,
                        help='Path to CfgDataDef.yaml or .pkl file')
    parser.add_argument('-g', '--gencfg', required=True,
                        help='Path to GenCfgData.py tool')
    parser.add_argument('-i', '--inject', default=None,
                        help='Path to CFGDATA.bin; if given, inject delta tags after generation')
    parser.add_argument('--fspm-header', default=None,
                        help='Path to FspmUpd.h (for config struct offset detection)')
    parser.add_argument('--fsps-header', default=None,
                        help='Path to FspsUpd.h (for config struct offset detection)')
    args = parser.parse_args()

    if not os.path.exists(args.output_dir):
        os.makedirs(args.output_dir)

    gen_fsp_upd_delta_blobs(
        args.fspm_bin, args.fsps_bin, args.output_dir,
        args.dlt, args.yaml, args.gencfg,
        fspm_header=args.fspm_header, fsps_header=args.fsps_header
    )

    if args.inject and os.path.exists(args.inject):
        delta_blobs = collect_delta_blobs(args.output_dir)
        if delta_blobs:
            inject_delta_tags_into_cfgdata(args.inject, delta_blobs)


if __name__ == '__main__':
    main()
