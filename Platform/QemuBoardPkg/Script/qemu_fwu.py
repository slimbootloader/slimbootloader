#!/usr/bin/env python
## @ qemu_fwu.py
#
# QEMU firmware update test script
#
# Copyright (c) 2020, Intel Corporation. All rights reserved.<BR>
# SPDX-License-Identifier: BSD-2-Clause-Patent
#
##

import os
import sys
import re
import glob
import struct
import shutil
import subprocess
from ctypes import Structure, c_char, c_uint32, c_uint8, c_uint64, c_uint16, sizeof, ARRAY


class FlashMapDesc(Structure):
    _pack_ = 1
    _fields_ = [
        ('sig', ARRAY(c_char, 4)),
        ('flags', c_uint32),
        ('offset', c_uint32),
        ('size', c_uint32),
    ]


class FlashMap(Structure):

    FLASH_MAP_SIGNATURE = 'FLMP'

    FLASH_MAP_ATTRIBUTES = {
        "PRIMARY_REGION": 0x00000000,
        "BACKUP_REGION": 0x00000001,
    }

    FLASH_MAP_DESC_FLAGS = {
        "TOP_SWAP": 0x00000001,
        "REDUNDANT": 0x00000002,
        "NON_REDUNDANT": 0x00000004,
        "NON_VOLATILE": 0x00000008,
        "COMPRESSED": 0x00000010,
        "BACKUP": 0x00000040,
    }

    _pack_ = 1
    _fields_ = [
        ('sig', ARRAY(c_char, 4)),
        ('version', c_uint16),
        ('length', c_uint16),
        ('attributes', c_uint8),
        ('reserved', ARRAY(c_char, 3)),
        ('romsize', c_uint32),
    ]


def handle_ts(bios_image, set_ts_val=0):

    # Since QEMU does not support flash top swap, this sccript will help
    # reassemble the image according to the top swap request
    inf = open(bios_image, "rb")
    bios_bins = bytearray(inf.read())
    inf.close()

    # Get flash map
    bios_size = len(bios_bins)
    bios_base = 0x100000000 - bios_size

    flash_map_addr = c_uint32.from_buffer(bios_bins, bios_size - 8)
    flash_map_off = flash_map_addr.value - bios_base

    flash_map = FlashMap.from_buffer(bios_bins, flash_map_off)
    is_backup = 1 if flash_map.attributes & flash_map.FLASH_MAP_ATTRIBUTES[
        'BACKUP_REGION'] else 0

    top_swap_size = 0
    redundant_size = 0
    entry_num = (flash_map.length - sizeof(FlashMap)) // sizeof(FlashMapDesc)
    for idx in range(entry_num):
        desc = FlashMapDesc.from_buffer(
            bios_bins,
            flash_map_off + sizeof(FlashMap) + idx * sizeof(FlashMapDesc))
        if (desc.flags & FlashMap.FLASH_MAP_DESC_FLAGS['TOP_SWAP']) and not (
                desc.flags & FlashMap.FLASH_MAP_DESC_FLAGS['BACKUP']):
            top_swap_size += desc.size
        if (desc.flags & FlashMap.FLASH_MAP_DESC_FLAGS['REDUNDANT']) and not (
                desc.flags & FlashMap.FLASH_MAP_DESC_FLAGS['BACKUP']):
            redundant_size += desc.size

    top_a = None
    top_b = None

    curr = top_swap_size
    tmp = bios_bins[-curr:]

    # Get the FWU flag
    fwu_flg = tmp[-10]
    if fwu_flg != 0x90:
        tmp[-10] = 0x90

# Get the top swap request from image
    ts_req = tmp[-11]
    if ts_req != 0x90:
        tmp[-11] = 0x90

    print("---------------------------------------------")
    print("State: %02X %02X" % (ts_req, fwu_flg))

    if ts_req != 0x90:
        print("Automatically change TS !")
    else:
        print("Manually change TS !")
        inp = set_ts_val
        if inp == 0:
            ts_req = 0x00
        elif inp == 1:
            ts_req = 0x80
        else:
            ts_req = 0x90

    print("Booting from BP%d" % is_backup)
    if is_backup:
        top_b = tmp
    else:
        top_a = tmp

    curr += top_swap_size
    tmp = bios_bins[-curr:-curr + top_swap_size]

    if is_backup:
        top_a = tmp
    else:
        top_b = tmp

    top_n = '\xff' * top_swap_size
    redundant_n = '\xff' * redundant_size

    curr += redundant_size
    redundant_a = bios_bins[-curr:-curr + redundant_size]

    curr += redundant_size
    redundant_b = bios_bins[-curr:-curr + redundant_size]

    non_redundant = bios_bins[:bios_size - curr]

    if ts_req == 0x00:
        ts = 0
        tv = '0'
    elif ts_req == 0x80:
        ts = 1
        tv = '1'
    else:
        # do not change anything
        ts = 0x80
        tv = 'N'

    print("TS Current:    (%d)" % (is_backup))
    print("TS  Reqest: %02X (%s)" % (ts_req, tv))
    print("FWU  Flags: %02X (%d)" % (fwu_flg, 0 if fwu_flg == 0x90 else 1))

    if ts == 0x80:
        bios_dat = bios_bins
        print("No change")
    else:
        if ts & 1 == 0:
            # Boot from partition 0
            bios_dat = non_redundant + redundant_b + redundant_a + top_b + top_a
            print("Activate Partition A")
        else:
            # Boot from partition 1
            bios_dat = non_redundant + redundant_b + redundant_a + top_a + top_b
            print("Activate Partition B")

    inf = open(bios_image, "wb")
    inf.write(bios_dat)
    inf.close()

    return fwu_flg


def run_qemu(bios_img, fwu_path, fwu_mode=False, timeout=None):
    if os.name == 'nt':
        path = r"C:\Program Files\qemu\qemu-system-x86_64"
    else:
        path = r"qemu-system-x86_64"
    cmd_list = [
        path, "-nographic",  "-machine", "q35,accel=tcg",
        "-serial", "mon:stdio",
        "-m", "256M", "-drive",
        "id=mydrive,if=none,format=raw,file=fat:rw:%s" % fwu_path, "-device",
        "ide-hd,drive=mydrive", "-boot", "order=d%s" % ('an' if fwu_mode else ''),
        "-no-reboot", "-drive", "file=%s,if=pflash,format=raw" % bios_img
    ]
    x = subprocess.call(cmd_list, timeout=timeout)
    if x:
        sys.exit(1)


def usage():
    print("usage:\n  python %s bios_image fwu_cap_dir\n" % sys.argv[0])
    print("  bios_image :  QEMU Slim Bootloader firmware image.")
    print("                This image can be generated through the normal Slim Bootloader build process.")
    print("  fwu_cap_dir:  Directory containing firmware capsule file 'FwuImage.bin'.")
    print("                This image can be generated using GenCapsuleFirmware.py tool.")
    print("")


def main():
    if sys.version_info.major < 3:
        print ("This script needs Python3 !")
        return -1

    if len(sys.argv) != 3:
        usage()
        return -2

    bios_img = sys.argv[1]
    fwu_dir = sys.argv[2]

    print("Firmware update for Slim BootLoader")

    fwu_mode = 2
    run_qemu(bios_img, fwu_dir, True if fwu_mode != 0 else False)

    while fwu_mode:
        print("Reboot ... !")
        fwu_ret = handle_ts (bios_img, fwu_mode)
        if fwu_ret == 0x80:
            fwu_mode = 0
            break
        run_qemu(bios_img, fwu_dir, True if fwu_mode != 0 else False)

    # Try final normal boot
    try:
        run_qemu(bios_img, fwu_dir, True if fwu_mode != 0 else False, timeout=8)
    except:
        pass

if __name__ == '__main__':
    sys.exit(main())
