## @ StitchLoader.py
#  This is a python stitching script for Slim Bootloader PTL build
#
# Copyright (c) 2026, Intel Corporation. All rights reserved. <BR>
# SPDX-License-Identifier: BSD-2-Clause-Patent
#
##
import os
import re
import sys
import argparse
from   ctypes import *
from   functools import reduce

sys.dont_write_bytecode = True
sblopen_dir = os.path.abspath(os.path.join(os.path.dirname(__file__), '../../../'))
if not os.path.exists (sblopen_dir):
    sblopen_dir = os.getenv('SBL_SOURCE', '')

if not os.path.exists (sblopen_dir):
    raise Exception ("Please make sure 'SBL_SOURCE' environment variable is set to open source SBL root folder.")

tools_dir = os.path.join(sblopen_dir, "BootloaderCorePkg" , "Tools")
if not os.path.exists (tools_dir):
    raise Exception ("Cannot find SBL BootloaderCorePkg Tools directory at '%s'" % tools_dir)
sys.path.insert (0, tools_dir)

# Current Platform specific Script folder to path for module import and should take higher precedent over CommonBoardPkg
sys.path.insert (1, os.path.abspath(os.path.dirname(__file__)))

common_script_dir = os.path.join(sblopen_dir, "Platform" , "CommonBoardPkg", "Script")
if not os.path.exists (common_script_dir):
    raise Exception ("Cannot find SBL CommonBoardPkg Script directory at '%s'" % common_script_dir)
sys.path.insert (2, common_script_dir)

try:
    from   IfwiUtility import *
except ImportError:
    err_msg  = "Cannot find IfwiUtility module!\n"
    err_msg += "Please make sure 'SBL_SOURCE' environment variable is set to open source SBL root folder."
    raise  ImportError(err_msg)

extra_usage_txt = \
"""This script creates a new PantherLake Slim Bootloader IFWI image basing
on an existing IFWI base image.  Please note, this stitching method will work
only if Boot Guard in the base image is not enabled, and the silicon is not
fused with Boot Guard enabled.
Please follow steps below:
  1.  Get an existing PantherLake UEFI IFWI image associated with the target platform.
      Alternatively, the original IFWI image from the onboard SPI flash can be
      read out as the base image too.
  2.  Build Slim Bootloader binary, the generated ZIP package is located at:
      $(WORKSPACE)/Outputs/ptl/SlimBootloader.bin
  3.  Stitch to create a new IFWI image.
      EX:
      python StitchLoader.py -i PTL_UEFI_IFWI.bin -s SlimBootloader.bin -o PTL_SBL_IFWI.bin
  4.  Optionally, to view the flash layout for an given IFWI image,
      specify '-i' option only.
      EX:
      python StitchLoader.py -i IFWI.bin
"""

def read_platform_ids():
    ids = {}
    with open(os.path.join(os.path.dirname(__file__), "..", "Include", "platformboardid.h"), "r") as f:
        for line in f:
            m = re.search(
                r"#define\s+(PLATFORM_ID_[A-Za-z0-9_]+)\s+(0x[0-9A-Fa-f]+)",
                line
            )
            if m:
                ids[m.group(1)] = int(m.group(2), 16)

    if not ids:
        raise RuntimeError("No PLATFORM_ID_* found in platformboardid.h")

    return ids

def add_platform_data (bios_data, platform_data = None):
    if platform_data:
        platform_id = platform_data & 0xFF
        platform_ids = read_platform_ids()
        EDGE_PLATFORM_IDS = {
            platform_ids["PLATFORM_ID_PTL_UH_LP5X_Robinson"],
            }
        # Parse BIOS image
        ifwi_parser = IFWI_PARSER ()
        bios = ifwi_parser.parse_ifwi_binary (bios_data)
        if not bios:
            return None

        for part in range(2):
          path = 'IFWI/BIOS/TS%d/SG1A' % part
          stage1A = ifwi_parser.locate_component (bios, path)
          if stage1A:
              if platform_id in EDGE_PLATFORM_IDS:
                plat_data_offset = stage1A.offset +0x40
              else:
                plat_data_offset = stage1A.offset + stage1A.length - 12
              c_uint32.from_buffer (bios_data, plat_data_offset).value = platform_data
          print("Platform data was patched for %s" % path)

    return bios_data


def parse_fit_acm_addr (ifwi_data, fit_off):
    # Returns the ACM address from the FIT type 2 entry, or None if not a valid FIT
    size = len (ifwi_data)
    if not (0 <= fit_off < size - 16):
        return None
    if bytes (ifwi_data[fit_off : fit_off + 8]) != b'_FIT_   ':
        return None
    num_entries = int.from_bytes (ifwi_data[fit_off + 8 : fit_off + 11], 'little')
    if not (1 <= num_entries <= 0xFF) or fit_off + num_entries * 16 > size:
        return None
    if (ifwi_data[fit_off + 14] & 0x7F) != 0:
        return None
    for idx in range (num_entries):
        entry = ifwi_data[fit_off + idx * 16 : fit_off + (idx + 1) * 16]
        if (entry[14] & 0x7F) == 2:
            return int.from_bytes (entry[0:8], 'little')
    return None


def find_fit_offsets (ifwi_data):
    # Each top swap partition carries its own FIT, so scan the whole image
    offsets = []
    pos = 0
    while True:
        pos = ifwi_data.find (b'_FIT_   ', pos)
        if pos < 0:
            break
        if parse_fit_acm_addr (ifwi_data, pos) is not None:
            offsets.append (pos)
        pos += 8
    return offsets


def get_fit_acm (ifwi_data):
    # Returns (offset, length) of the ACM named by the FIT pointer at the image end
    size = len (ifwi_data)
    flash_base = (1 << 32) - size
    fit_off = int.from_bytes (ifwi_data[size - 0x40 : size - 0x3C], 'little') - flash_base
    acm_addr = parse_fit_acm_addr (ifwi_data, fit_off)
    if acm_addr is None:
        return (None, None, 0)
    acm_off = acm_addr - flash_base
    if not (0 <= acm_off < size - 0x20):
        return (None, None, 0)
    acm_len = int.from_bytes (ifwi_data[acm_off + 0x18 : acm_off + 0x1C], 'little') * 4
    return (fit_off, acm_off, acm_len)


def preserve_base_acm (ifwi_data, base_ifwi_data):
    # SBL only builds a 0xFF filled ACM placeholder, so keep the real one from the base IFWI
    _, src_off, src_len = get_fit_acm (base_ifwi_data)
    if src_off is None:
        print ("No startup ACM found in base image, nothing to preserve")
        return 0

    if src_len == 0 or src_off + src_len > len (base_ifwi_data):
        print ("Invalid ACM length 0x%X in base image!" % src_len)
        return -1

    ref_fit_off, ref_acm_off, _ = get_fit_acm (ifwi_data)
    if ref_acm_off is None:
        print ("Could not locate FIT ACM entry in the stitched image!")
        return -1

    acm_bin = base_ifwi_data[src_off : src_off + src_len]

    # Every FIT names the same ACM address because it is written for the post
    # top swap view, so shift it by each partition's distance from the last FIT.
    for fit_off in find_fit_offsets (ifwi_data):
        dst_off = ref_acm_off - (ref_fit_off - fit_off)
        if not (0 <= dst_off and dst_off + src_len <= len (ifwi_data)):
            print ("ACM target 0x%X for FIT 0x%X is out of range!" % (dst_off, fit_off))
            return -1
        ifwi_data[dst_off : dst_off + src_len] = acm_bin
        print ("Preserved ACM (0x%X bytes) from base 0x%X to 0x%X" % (src_len, src_off, dst_off))
    return 0


def create_ifwi_image (ifwi_in, ifwi_out, sbl_in, platform_data):
    ifwi_data   = bytearray (get_file_data (ifwi_in))
    base_data   = bytearray (ifwi_data)
    ifwi_parser = IFWI_PARSER ()
    ifwi = ifwi_parser.parse_ifwi_binary (ifwi_data)
    if not ifwi:
        print ("Invalid IFWI input image!")
        return -1

    # update bios region
    bios = ifwi_parser.locate_component (ifwi, 'IFWI/BIOS')
    if not bios:
        print ("Cound not find BIOS region!")
        return -2

    bios_data = bytearray (get_file_data (sbl_in))
    bios_data = add_platform_data (bios_data, platform_data)
    if not bios_data:
        print ("Failed to parse BIOS image!")
        return -3

    padding  = bios.length - len(bios_data)
    if padding < 0:
        print ("BIOS image is too big to fit into BIOS region!")
        return -4

    bios_data = b'\xff' * padding + bios_data
    ret = ifwi_parser.replace_component (ifwi_data, bios_data, 'IFWI/BIOS')
    if ret != 0:
        print ("Failed to replace BIOS region!")
        return -5

    ret = preserve_base_acm (ifwi_data, base_data)
    if ret != 0:
        return -6

    # create new ifwi
    print("Creating IFWI image ...")
    if ifwi_out == '':
        ifwi_out = ifwi_in
    gen_file_from_object (ifwi_out, ifwi_data)

    print ('done!')


def print_ifwi_layout (ifwi_file):
    ifwi_data   = bytearray (get_file_data (ifwi_file))
    ifwi_parser = IFWI_PARSER ()
    ifwi = ifwi_parser.parse_ifwi_binary (ifwi_data)
    if ifwi:
        ifwi_parser.print_tree (ifwi)


if __name__ == '__main__':
    hexstr = lambda x: int(x, 16)

    ap = argparse.ArgumentParser()
    ap.add_argument('-i',
                    '--input-ifwi-file',
                    dest='ifwi_in',
                    type=str,
                    required=True,
                    help='Specify input template IFWI image file path')

    ap.add_argument('-o',
                    '--output-ifwi-file',
                    dest='ifwi_out',
                    type=str,
                    default='',
                    help='Specify generated output IFWI image file path')

    ap.add_argument('-s',
                    '--sbl-input',
                    dest='sbl_in',
                    type=str,
                    default='',
                    help='Specify input sbl binary file path')

    ap.add_argument('-p',
                    '--platform-data',
                    dest='plat_data',
                    type=hexstr,
                    default=None,
                    help='Specify a platform specific data (HEX, DWORD) for customization')


    if len(sys.argv) == 1:
        print ('%s' % extra_usage_txt)

    args = ap.parse_args()

    if args.ifwi_out == '' and args.sbl_in == '':
        print_ifwi_layout (args.ifwi_in)
        ret = 0
    else:
        ret = create_ifwi_image (args.ifwi_in, args.ifwi_out, args.sbl_in, args.plat_data)


    sys.exit(ret)

