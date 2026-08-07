#!/usr/bin/env python
## @ ui_variable_reclaim_stress_test.py
#
# Stress test UiSetup cfgdata variable updates on QEMU to force reclaim pressure.
#
# The QEMU OsLoader shell in this tree does not expose generic setvar/getvar/delete
# commands, so this test uses cfgset/cfgget loops that persist through SetVariable
# and exercises repeated update/clear/rewrite behavior on the backing variable.
#
# Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
# SPDX-License-Identifier: BSD-2-Clause-Patent
##

import os
import re
import sys
import time
import socket
import shutil
import subprocess


def usage():
    print("usage:\n  python %s bios_image out_dir\n" % sys.argv[0])
    print("   bios_image :  QEMU Slim Bootloader firmware image.")
    print("      out_dir :  Output directory.")
    print("")


def get_qemu_path():
    if os.name == 'nt':
        return r"C:\Program Files\qemu\qemu-system-x86_64.exe"
    return 'qemu-system-x86_64'


SERIAL_PORT = int(os.environ.get('SBL_QEMU_SERIAL_PORT', '5555'))
STRESS_WRITES = int(os.environ.get('SBL_QEMU_VAR_STRESS_WRITES', '600'))


def launch_qemu(bios_img, uefi=False, disk_dir=None):
    qemu_path = get_qemu_path()
    cmd = [
        qemu_path,
        '-machine', 'q35,accel=tcg',
        '-cpu', 'max',
        '-m', '256M',
        '-no-reboot',
        '-drive', 'file=%s,if=pflash,format=raw' % bios_img,
        '-display', 'none',
        '-monitor', 'none',
        '-serial', 'tcp:127.0.0.1:%d,server=on,wait=on' % SERIAL_PORT
    ]
    if uefi:
        cmd.extend(['-boot', 'order=dba'])
    if disk_dir is not None:
        cmd.extend(['-drive', 'id=vardisk,if=none,format=raw,file=fat:rw:%s' % disk_dir,
                    '-device', 'ide-hd,drive=vardisk'])

    return subprocess.Popen(
        cmd,
        stdin=subprocess.DEVNULL,
        stdout=subprocess.DEVNULL,
        stderr=subprocess.DEVNULL,
        bufsize=0
    )


def connect_serial(timeout=10):
    start = time.time()
    while time.time() - start < timeout:
        try:
            sock = socket.create_connection(('127.0.0.1', SERIAL_PORT), timeout=1)
            sock.settimeout(0.1)
            return sock
        except OSError:
            time.sleep(0.1)
    raise RuntimeError("Timed out connecting to QEMU serial port")


def wait_for_text(proc, sock, output, pattern, timeout, send_key=False):
    start = time.time()
    key_sent = False

    while time.time() - start < timeout:
        if proc.poll() is not None:
            break

        try:
            chunk = sock.recv(256)
        except socket.timeout:
            continue

        if not chunk:
            continue

        output.extend(chunk)
        text = output.decode('utf-8', errors='ignore')
        if send_key and (not key_sent) and ('Press any key within' in text):
            sock.sendall(b'\r')
            key_sent = True

        if pattern in text:
            return text

    raise RuntimeError("Timed out waiting for '%s'\n%s" %
                       (pattern, output.decode('utf-8', errors='ignore')[-6000:]))


def send_shell_command(proc, sock, output, command, timeout=30):
    command_start = len(output)
    sock.sendall((command + '\r').encode('utf-8'))

    start = time.time()
    while time.time() - start < timeout:
        if proc.poll() is not None:
            break

        try:
            chunk = sock.recv(256)
        except socket.timeout:
            continue

        if not chunk:
            continue

        output.extend(chunk)
        text = output.decode('utf-8', errors='ignore')
        response = output[command_start:].decode('utf-8', errors='ignore')
        response = re.sub(r'\x1b\[[0-?]*[ -/]*[@-~]', '', response)
        response = response.replace('\r', '').replace('\n', '')
        echoed = response.find(command)
        if echoed >= 0 and 'Shell> ' in response[echoed + len(command):]:
            return text

    raise RuntimeError("Timed out waiting for shell prompt after '%s'\n%s" %
                       (command, output.decode('utf-8', errors='ignore')[-6000:]))


def run_shell_session(bios_img, commands, uefi=False, log_path=None,
                      disk_dir=None, completion_marker=None, shell_timeout=None):
    proc = launch_qemu(bios_img, uefi, disk_dir)
    output = bytearray()
    sock = None

    try:
        sock = connect_serial()
        if completion_marker is not None:
            wait_for_text(proc, sock, output, completion_marker, 600)
        else:
            wait_for_text(proc, sock, output, 'Shell> ',
                          shell_timeout if shell_timeout is not None else (90 if uefi else 45),
                          send_key=not uefi)
            for command in commands:
                send_shell_command(proc, sock, output, command)
        text = output.decode('utf-8', errors='ignore')
    finally:
        if sock is not None:
            sock.close()
        proc.terminate()
        try:
            proc.wait(timeout=5)
        except subprocess.TimeoutExpired:
            proc.kill()
            proc.wait(timeout=5)
        if log_path is not None:
            with open(log_path, 'w', encoding='utf-8') as log:
                log.write(output.decode('utf-8', errors='ignore').replace('\r', '\n'))

    return text.replace('\r', '\n')


def generate_stress_commands(iterations):
    cmds = [
        'cfgset PLATFORM_CFG_DATA.PlatTest "SEED"',
        'cfgget PLATFORM_CFG_DATA.PlatTest',
    ]

    for idx in range(iterations):
        value = 0xA5000000 | (idx & 0x00FFFFFF)
        cmds.append('cfgset QEMU_TYPE_TEST_CFG_DATA.ScalarU32 0x%08X' % value)

        if (idx % 40) == 0:
            cmds.append('cfgget QEMU_TYPE_TEST_CFG_DATA.ScalarU32')

        # Exercise clear-and-rewrite behavior on a text field.
        if (idx % 100) == 50:
            cmds.append('cfgset PLATFORM_CFG_DATA.PlatTest ""')
            cmds.append('cfgget PLATFORM_CFG_DATA.PlatTest')
            cmds.append('cfgset PLATFORM_CFG_DATA.PlatTest "S%03d"' % (idx % 1000))

    cmds.extend([
        'cfgset PLATFORM_CFG_DATA.PlatTest ""',
        'cfgset PLATFORM_CFG_DATA.PlatTest "DONE"',
        'cfgset QEMU_TYPE_TEST_CFG_DATA.ScalarU32 0xCAFEBABE',
        'cfgget PLATFORM_CFG_DATA.PlatTest',
        'cfgget QEMU_TYPE_TEST_CFG_DATA.ScalarU32',
    ])

    return cmds


def check_lines(output, required_lines):
    for line in required_lines:
        if line not in output:
            print("Failed locating '%s' !" % line)
            return -1
    return 0


def check_absent_tokens(output, forbidden_tokens):
    lines = output.splitlines()
    text = output.lower()
    for token in forbidden_tokens:
        if token.lower() in text:
            print("Forbidden token found: '%s'" % token)
            for line in lines:
                if token.lower() in line.lower():
                    print("  Matched line: %s" % line.strip())
                    break
            return -1
    return 0


def main():
    if sys.version_info.major < 3:
        print("This script needs Python3 !")
        return -1

    if len(sys.argv) != 3:
        usage()
        return -2

    bios_img = sys.argv[1]
    out_dir = sys.argv[2]
    test_img = os.path.join(out_dir, 'SblVarReclaimStress.bin')

    print('UiSetup variable reclaim pressure stress test for Slim BootLoader')
    print('Stress writes: %d' % STRESS_WRITES)

    if not os.path.exists(bios_img):
        print("Could not find BIOS image '%s' !" % bios_img)
        return -3

    os.makedirs(out_dir, exist_ok=True)
    shutil.copyfile(bios_img, test_img)

    first_commands = generate_stress_commands(STRESS_WRITES)
    second_commands = [
        'cfgget PLATFORM_CFG_DATA.PlatTest',
        'cfgget QEMU_TYPE_TEST_CFG_DATA.ScalarU32',
    ]

    first_output = run_shell_session(test_img, first_commands,
                                     log_path=os.path.join(out_dir, 'ui_var_reclaim_stress_session1.log'))
    second_output = run_shell_session(test_img, second_commands,
                                      log_path=os.path.join(out_dir, 'ui_var_reclaim_stress_session2.log'))

    ret = check_lines(first_output, [
        'PLATFORM_CFG_DATA.PlatTest = "SEED"',
        'PLATFORM_CFG_DATA.PlatTest = "DONE"',
        'QEMU_TYPE_TEST_CFG_DATA.ScalarU32 = 3405691582 (0xCAFEBABE)',
    ])

    if ret == 0:
        ret = check_absent_tokens(first_output, [
            'error:',
            'out of resources',
            'device error',
            'assert',
            'timed out waiting for shell prompt',
        ])

    if ret == 0:
        ret = check_lines(second_output, [
            'PLATFORM_CFG_DATA.PlatTest = "DONE"',
            'QEMU_TYPE_TEST_CFG_DATA.ScalarU32 = 3405691582 (0xCAFEBABE)',
        ])

    if ret == 0:
        ret = check_absent_tokens(second_output, [
            'error:',
            'out of resources',
            'device error',
            'assert',
        ])

    print('\nUiSetup variable reclaim pressure stress test %s !\n' % ('PASSED' if ret == 0 else 'FAILED'))
    return ret


if __name__ == '__main__':
    sys.exit(main())
