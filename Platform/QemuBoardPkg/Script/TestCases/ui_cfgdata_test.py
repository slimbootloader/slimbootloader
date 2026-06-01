#!/usr/bin/env python
## @ ui_cfgdata_test.py
#
# Test UiSetup CFGDATA persistence via OsLoader shell commands on QEMU.
#
# This test validates the UiSetup configuration framework by:
#   1. Executing shell commands to modify configuration fields (cfgset, cfgget, cfgfind)
#   2. Performing a system reboot (QEMU restart with same image)
#   3. Verifying that all configuration changes persisted across the reboot
#
# Tested functionality:
#   - cfgfind: Search config fields by name/path pattern
#   - cfgget:  Read field values with formatted output (decimal, hex, enum names)
#   - cfgset:  Write field values (numeric, text, combo types)
#   - Config persistence: CFGDATA storage survives firmware reboot cycles
#
# Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
# SPDX-License-Identifier: BSD-2-Clause-Patent
#
##

import os
import sys
import time
import socket
import shutil
import subprocess


def usage():
    print("usage:\n  python %s bios_image out_dir\n" % sys.argv[0])
    print("   bios_image :  QEMU Slim Bootloader firmware image.")
    print("                 This image can be generated through the normal Slim Bootloader build process.")
    print("      out_dir :  Output directory.")
    print("")


def get_qemu_path():
    if os.name == 'nt':
        return r"C:\Program Files\qemu\qemu-system-x86_64.exe"
    return 'qemu-system-x86_64'


SERIAL_PORT = int(os.environ.get('SBL_QEMU_SERIAL_PORT', '5555'))


def launch_qemu(bios_img):
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
        '-serial', 'tcp:127.0.0.1:%d,server=on,wait=off' % SERIAL_PORT
    ]

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

    raise RuntimeError("Timed out waiting for '%s'" % pattern)


def send_shell_command(proc, sock, output, command, timeout=20):
    sock.sendall((command + '\r').encode('utf-8'))

    prompt_count = output.decode('utf-8', errors='ignore').count('Shell> ')
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
        if text.count('Shell> ') > prompt_count:
            return text

    raise RuntimeError("Timed out waiting for shell prompt after '%s'" % command)


def run_shell_session(bios_img, commands):
    proc = launch_qemu(bios_img)
    output = bytearray()
    sock = None

    try:
        sock = connect_serial()
        wait_for_text(proc, sock, output, 'Shell> ', 45, send_key=True)
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

    return text.replace('\r', '\n')


def check_lines(output, required_lines):
    for line in required_lines:
        if line not in output:
            print("Failed locating '%s' !" % line)
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
    test_img = os.path.join(out_dir, 'SblShellCfgTest.bin')

    print('OsLoader shell config update test for Slim BootLoader')

    if not os.path.exists(bios_img):
        print("Could not find BIOS image '%s' !" % bios_img)
        return -3

    os.makedirs(out_dir, exist_ok=True)
    shutil.copyfile(bios_img, test_img)

    first_session_commands = [
        'cfgfind CurrentBoot',
        'cfgget GEN_CFG_DATA.CurrentBoot',
        'cfgset GEN_CFG_DATA.CurrentBoot 2',
        'cfgget GEN_CFG_DATA.CurrentBoot',
        'cfgset PLATFORM_CFG_DATA.PlatTest "ABCD"',
        'cfgget PLATFORM_CFG_DATA.PlatTest',
        'cfgset GPIO_CFG_DATA.GpioConfPad1_GPP_A4.GPIOElectricalCfg 5',
        'cfgget GPIO_CFG_DATA.GpioConfPad1_GPP_A4.GPIOElectricalCfg',
        'cfgfind GpioConfPad1_GPP_A4.GPIOElectricalCfg',
        'cfgset QEMU_TYPE_TEST_CFG_DATA.ScalarU8 0x5A',
        'cfgset QEMU_TYPE_TEST_CFG_DATA.ScalarU16 0xA55A',
        'cfgset QEMU_TYPE_TEST_CFG_DATA.ScalarU32 0xDEADBEEF',
        'cfgset QEMU_TYPE_TEST_CFG_DATA.ScalarU64 0x1122334455667788',
        'cfgset QEMU_TYPE_TEST_CFG_DATA.TableU8[3] 0xAB',
        'cfgset QEMU_TYPE_TEST_CFG_DATA.TableU16[2] 0xBEEF',
        'cfgset QEMU_TYPE_TEST_CFG_DATA.TableU32[2] 0x89ABCDEF',
        'cfgset QEMU_TYPE_TEST_CFG_DATA.TableU64[1] 0x123456789ABCDEF0',
        'cfgset QEMU_TYPE_TEST_CFG_DATA.ArrayU8_512[511] 0xCD',
        'cfgget QEMU_TYPE_TEST_CFG_DATA.ScalarU8',
        'cfgget QEMU_TYPE_TEST_CFG_DATA.ScalarU16',
        'cfgget QEMU_TYPE_TEST_CFG_DATA.ScalarU32',
        'cfgget QEMU_TYPE_TEST_CFG_DATA.ScalarU64',
        'cfgget QEMU_TYPE_TEST_CFG_DATA.TableU8[3]',
        'cfgget QEMU_TYPE_TEST_CFG_DATA.TableU16[2]',
        'cfgget QEMU_TYPE_TEST_CFG_DATA.TableU32[2]',
        'cfgget QEMU_TYPE_TEST_CFG_DATA.TableU64[1]',
        'cfgget QEMU_TYPE_TEST_CFG_DATA.ArrayU8_512[511]',
        'cfgget QEMU_TYPE_TEST_CFG_DATA.ArrayU8_512 --dump 496 16',
        'cfgfind ScalarU64',
        'cfgfind TableU64',
        'cfgfind ArrayU8_512',
        'cfgfind QEMU_TYPE_TEST_CFG_DATA',
    ]

    second_session_commands = [
        'cfgget GEN_CFG_DATA.CurrentBoot',
        'cfgget PLATFORM_CFG_DATA.PlatTest',
        'cfgget GPIO_CFG_DATA.GpioConfPad1_GPP_A4.GPIOElectricalCfg',
        'cfgfind PlatTest',
        'cfgget QEMU_TYPE_TEST_CFG_DATA.ScalarU8',
        'cfgget QEMU_TYPE_TEST_CFG_DATA.ScalarU16',
        'cfgget QEMU_TYPE_TEST_CFG_DATA.ScalarU32',
        'cfgget QEMU_TYPE_TEST_CFG_DATA.ScalarU64',
        'cfgget QEMU_TYPE_TEST_CFG_DATA.TableU8[3]',
        'cfgget QEMU_TYPE_TEST_CFG_DATA.TableU16[2]',
        'cfgget QEMU_TYPE_TEST_CFG_DATA.TableU32[2]',
        'cfgget QEMU_TYPE_TEST_CFG_DATA.TableU64[1]',
        'cfgget QEMU_TYPE_TEST_CFG_DATA.ArrayU8_512[511]',
        'cfgget QEMU_TYPE_TEST_CFG_DATA.ArrayU8_512 --dump 496 16',
        'cfgfind QEMU_TYPE_TEST_CFG_DATA',
    ]

    first_output = run_shell_session(test_img, first_session_commands)
    second_output = run_shell_session(test_img, second_session_commands)

    ret = check_lines(first_output, [
        'GEN_CFG_DATA.CurrentBoot =',
        'GEN_CFG_DATA.CurrentBoot = 2 (0x2, 2)',
        'PLATFORM_CFG_DATA.PlatTest = "ABCD"',
        'GPIO_CFG_DATA.GpioConfPad1_GPP_A4.GPIOElectricalCfg = 5 (0x5, TermNone)',
        'QEMU_TYPE_TEST_CFG_DATA.ScalarU8 = 90 (0x5A)',
        'QEMU_TYPE_TEST_CFG_DATA.ScalarU16 = 42330 (0xA55A)',
        'QEMU_TYPE_TEST_CFG_DATA.ScalarU32 = 3735928559 (0xDEADBEEF)',
        'QEMU_TYPE_TEST_CFG_DATA.ScalarU64 = 1234605616436508552 (0x1122334455667788)',
        'QEMU_TYPE_TEST_CFG_DATA.TableU8[3] = 171 (0xAB)',
        'QEMU_TYPE_TEST_CFG_DATA.TableU16[2] = 48879 (0xBEEF)',
        'QEMU_TYPE_TEST_CFG_DATA.TableU32[2] = 2309737967 (0x89ABCDEF)',
        'QEMU_TYPE_TEST_CFG_DATA.TableU64[1] = 1311768467463790320 (0x123456789ABCDEF0)',
        'QEMU_TYPE_TEST_CFG_DATA.ArrayU8_512[511] = 205 (0xCD)',
        'QEMU_TYPE_TEST_CFG_DATA.ArrayU8_512 dump [496..511]',
        '01F0:',
        'QEMU_TYPE_TEST_CFG_DATA.TableU64 = table[4] elem_bits=64',
        'QEMU_TYPE_TEST_CFG_DATA.ArrayU8_512 = table[512] elem_bits=8',
        '9 match(es)',
        '1 match(es)',
    ])
    if ret == 0:
        ret = check_lines(second_output, [
            'GEN_CFG_DATA.CurrentBoot = 2 (0x2, 2)',
            'PLATFORM_CFG_DATA.PlatTest = "ABCD"',
            'GPIO_CFG_DATA.GpioConfPad1_GPP_A4.GPIOElectricalCfg = 5 (0x5, TermNone)',
            'PLATFORM_CFG_DATA.PlatTest = "ABCD"',
            'QEMU_TYPE_TEST_CFG_DATA.ScalarU8 = 90 (0x5A)',
            'QEMU_TYPE_TEST_CFG_DATA.ScalarU16 = 42330 (0xA55A)',
            'QEMU_TYPE_TEST_CFG_DATA.ScalarU32 = 3735928559 (0xDEADBEEF)',
            'QEMU_TYPE_TEST_CFG_DATA.ScalarU64 = 1234605616436508552 (0x1122334455667788)',
            'QEMU_TYPE_TEST_CFG_DATA.TableU8[3] = 171 (0xAB)',
            'QEMU_TYPE_TEST_CFG_DATA.TableU16[2] = 48879 (0xBEEF)',
            'QEMU_TYPE_TEST_CFG_DATA.TableU32[2] = 2309737967 (0x89ABCDEF)',
            'QEMU_TYPE_TEST_CFG_DATA.TableU64[1] = 1311768467463790320 (0x123456789ABCDEF0)',
            'QEMU_TYPE_TEST_CFG_DATA.ArrayU8_512[511] = 205 (0xCD)',
            'QEMU_TYPE_TEST_CFG_DATA.ArrayU8_512 dump [496..511]',
            '01F0:',
            'QEMU_TYPE_TEST_CFG_DATA.TableU64 = table[4] elem_bits=64',
            'QEMU_TYPE_TEST_CFG_DATA.ArrayU8_512 = table[512] elem_bits=8',
            '9 match(es)',
        ])

    print('\nOsLoader shell config update test %s !\n' % ('PASSED' if ret == 0 else 'FAILED'))

    return ret


if __name__ == '__main__':
    sys.exit(main())