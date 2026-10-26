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
  4.  SBL only builds a 0xFF filled startup ACM placeholder, but its FIT always
      names it as the startup ACM. The stitch step therefore copies the startup
      ACM from the base IFWI FIT into both top swap partitions by default,
      independent of the TXT setting. Use '-acm' to fail when the ACM cannot be
      copied, or '-noacm' to keep the placeholder.
      EX:
      python StitchLoader.py -i PTL_UEFI_IFWI.bin -s SlimBootloader.bin -o PTL_SBL_IFWI.bin -acm
  5.  Optionally, to view the flash layout for an given IFWI image,
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


FIT_TYPE_HEADER      = 0x00
FIT_TYPE_STARTUP_ACM = 0x02
FIT_TYPE_KM          = 0x0B
FIT_TYPE_BPM         = 0x0C
ACM_MODULE_TYPE      = 0x0002
ACM_VENDOR_INTEL     = 0x00008086


def get_fit_entries (ifwi_data, fit_off):
    # Returns a list of (type, address) for a valid FIT at fit_off, or None
    size = len (ifwi_data)
    if not (0 <= fit_off <= size - 16):
        return None
    if bytes (ifwi_data[fit_off : fit_off + 8]) != FIT_ENTRY.FIT_SIGNATURE:
        return None
    num_entries = int.from_bytes (ifwi_data[fit_off + 8 : fit_off + 11], 'little')
    if not (1 <= num_entries <= 0xFF) or fit_off + num_entries * 16 > size:
        return None
    fit_type = ifwi_data[fit_off + 14]
    if (fit_type & 0x7F) != FIT_TYPE_HEADER:
        return None
    if (fit_type & 0x80) and (sum (ifwi_data[fit_off : fit_off + num_entries * 16]) & 0xFF) != 0:
        return None
    entries = []
    for idx in range (1, num_entries):
        entry = ifwi_data[fit_off + idx * 16 : fit_off + (idx + 1) * 16]
        entries.append ((entry[14] & 0x7F, int.from_bytes (entry[0:8], 'little')))
    return entries


def get_fit_entry_addr (entries, fit_type):
    for entry_type, addr in entries:
        if entry_type == fit_type:
            return addr
    return None


def is_valid_acm (ifwi_data, acm_off):
    # Check the ACM header fields defined as ACM_HEADER_4 in
    # Silicon/PantherlakePkg/Library/BootGuardLibCBnT/BootGuardTpmEventLogLib.c
    size = len (ifwi_data)
    if not (0 <= acm_off <= size - 0x20):
        return 0
    module_type = int.from_bytes (ifwi_data[acm_off        : acm_off + 0x02], 'little')
    header_len  = int.from_bytes (ifwi_data[acm_off + 0x04 : acm_off + 0x08], 'little') * 4
    vendor      = int.from_bytes (ifwi_data[acm_off + 0x10 : acm_off + 0x14], 'little')
    acm_len     = int.from_bytes (ifwi_data[acm_off + 0x18 : acm_off + 0x1C], 'little') * 4
    if module_type != ACM_MODULE_TYPE or vendor != ACM_VENDOR_INTEL:
        return 0
    if header_len == 0 or acm_len <= header_len or acm_off + acm_len > size:
        return 0
    return acm_len


def get_base_acm (base_ifwi_data):
    size       = len (base_ifwi_data)
    flash_base = (1 << 32) - size
    fit_off    = int.from_bytes (base_ifwi_data[size + FIT_ENTRY.FIT_OFFSET : size + FIT_ENTRY.FIT_OFFSET + 4], 'little') - flash_base
    entries    = get_fit_entries (base_ifwi_data, fit_off)
    if entries is None:
        return (None, 0)
    acm_addr = get_fit_entry_addr (entries, FIT_TYPE_STARTUP_ACM)
    if acm_addr is None:
        return (None, 0)
    acm_off = acm_addr - flash_base
    acm_len = is_valid_acm (base_ifwi_data, acm_off)
    if acm_len == 0:
        return (None, 0)
    return (acm_off, acm_len)


def get_partition_acm_slots (ifwi_data):
    # Resolve the FIT of each top swap partition from its own SG1A, following
    # IFWI_PARSER.update_ucode_fit_entry () in BootloaderCorePkg/Tools/IfwiUtility.py.
    # Every copy stores addresses for the view where that partition is decoded
    # at the top of the 4GB space, so translate them with the distance between
    # the partition SG1A end and the flash end. Returns a list of
    # (partition, acm offset, acm slot limit), or None on any inconsistency.
    ifwi = IFWI_PARSER.parse_ifwi_binary (ifwi_data)
    if not ifwi:
        print ("Stitched image is not a valid IFWI!")
        return None

    size       = len (ifwi_data)
    flash_base = (1 << 32) - size
    slots      = []
    for part in range (2):
        sg1a = IFWI_PARSER.locate_component (ifwi, 'IFWI/BIOS/TS%d/SG1A' % part)
        acm0 = IFWI_PARSER.locate_component (ifwi, 'IFWI/BIOS/TS%d/ACM0' % part)
        if not sg1a or not acm0:
            print ("Could not find SG1A/ACM0 in top swap partition TS%d!" % part)
            return None

        sg1a_end = sg1a.offset + sg1a.length
        shift    = size - sg1a_end
        fit_off  = int.from_bytes (ifwi_data[sg1a_end + FIT_ENTRY.FIT_OFFSET : sg1a_end + FIT_ENTRY.FIT_OFFSET + 4], 'little') - flash_base - shift
        if not (acm0.offset < fit_off < sg1a_end):
            print ("FIT 0x%X is outside of top swap partition TS%d!" % (fit_off, part))
            return None

        entries = get_fit_entries (ifwi_data, fit_off)
        if entries is None:
            print ("Invalid FIT at 0x%X for top swap partition TS%d!" % (fit_off, part))
            return None

        acm_addr = get_fit_entry_addr (entries, FIT_TYPE_STARTUP_ACM)
        if acm_addr is None:
            print ("No startup ACM entry in FIT 0x%X for top swap partition TS%d!" % (fit_off, part))
            return None

        acm_off = acm_addr - flash_base - shift
        if not (acm0.offset <= acm_off < acm0.offset + acm0.length):
            print ("FIT ACM 0x%X is outside of ACM0 in top swap partition TS%d!" % (acm_off, part))
            return None

        # The ACM must not overlap the KM/BPM that SBL places at the end of ACM0
        limit = acm0.offset + acm0.length
        for fit_type in (FIT_TYPE_KM, FIT_TYPE_BPM):
            addr = get_fit_entry_addr (entries, fit_type)
            if addr is not None:
                off = addr - flash_base - shift
                if acm_off < off < limit:
                    limit = off
        slots.append ((part, acm_off, limit))
    return slots


def preserve_base_acm (ifwi_data, base_ifwi_data, required):
    # SBL only builds a 0xFF filled ACM placeholder while its FIT still names it as the
    # startup ACM, so fill the placeholder with the startup ACM from the base IFWI.
    src_off, src_len = get_base_acm (base_ifwi_data)
    if src_off is None:
        print ("No valid startup ACM found in base image FIT, nothing to preserve")
        return -1 if required else 0

    slots = get_partition_acm_slots (ifwi_data)
    if slots is None:
        return -1

    acm_bin = base_ifwi_data[src_off : src_off + src_len]
    for part, dst_off, limit in slots:
        if is_valid_acm (ifwi_data, dst_off):
            print ("TS%d already carries a startup ACM at 0x%X, keeping it" % (part, dst_off))
            continue
        if dst_off + src_len > limit:
            print ("Base ACM (0x%X bytes) does not fit in TS%d ACM slot 0x%X-0x%X!" % (src_len, part, dst_off, limit))
            return -1
        if ifwi_data[dst_off : dst_off + src_len] != b'\xff' * src_len:
            print ("TS%d ACM slot 0x%X is not an empty placeholder, refusing to overwrite it!" % (part, dst_off))
            return -1
        ifwi_data[dst_off : dst_off + src_len] = acm_bin
        print ("Preserved ACM (0x%X bytes) from base 0x%X to TS%d 0x%X" % (src_len, src_off, part, dst_off))
    return 0


def create_ifwi_image (ifwi_in, ifwi_out, sbl_in, platform_data, acm_mode = 'auto'):
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

    if acm_mode == 'skip':
        print ("Skipping BIOS ACM preservation, the stitched FIT names a 0xFF filled ACM placeholder!")
    else:
        print ("Preserving BIOS ACM from base IFWI ...")
        ret = preserve_base_acm (ifwi_data, base_data, acm_mode == 'preserve')
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

    acm_group = ap.add_mutually_exclusive_group()
    acm_group.add_argument('-acm',
                    '--preserve-bios-acm',
                    dest='acm_mode',
                    action='store_const',
                    const='preserve',
                    default='auto',
                    help='Require the startup ACM from the base IFWI image to be copied into both '
                         'top swap partitions, and fail if it cannot be. By default the ACM is '
                         'copied whenever the base image has one, since SBL only builds a 0xFF '
                         'filled ACM placeholder that its FIT still names as the startup ACM.')
    acm_group.add_argument('-noacm',
                    '--skip-bios-acm',
                    dest='acm_mode',
                    action='store_const',
                    const='skip',
                    help='Do not copy the startup ACM from the base IFWI image and keep the 0xFF '
                         'filled placeholder. Only for base images whose FIT has no startup ACM.')


    if len(sys.argv) == 1:
        print ('%s' % extra_usage_txt)

    args = ap.parse_args()

    if args.ifwi_out == '' and args.sbl_in == '':
        print_ifwi_layout (args.ifwi_in)
        ret = 0
    else:
        ret = create_ifwi_image (args.ifwi_in, args.ifwi_out, args.sbl_in, args.plat_data, args.acm_mode)


    sys.exit(ret)

