## @ StitchLoader.py
#  This is a python stitching script for Slim Bootloader APL build
#
# Copyright (c) 2018, Intel Corporation. All rights reserved. <BR>
# SPDX-License-Identifier: BSD-2-Clause-Patent
#
##

import os
import re
import sys
import struct
import argparse
import zipfile
import shutil
from ctypes import *
from subprocess  import check_output
from functools import reduce

sys.dont_write_bytecode = True
sys.path.append (os.path.join(os.getenv('SBL_SOURCE', ''), 'BootloaderCorePkg' , 'Tools'))
try:
    from   IfwiUtility   import *
except ImportError:
    err_msg  = "Cannot find IfwiUtility module!\n"
    err_msg += "Please make sure 'SBL_SOURCE' environment variable is set to open source SBL root folder."
    raise  ImportError(err_msg)


extra_usage_txt = \
"""This script creates a new Apollo Lake Slim Bootloader IFWI image basing
on an existing IFWI base image.  Please note, this stitching method will work
only if Boot Guard in the base image is not enabled, and the silicon is not
fused with Boot Guard enabled.

Please follow steps below:

  1.  Download an existing Apollo Lake UEFI IFWI image associated with the target platform,
      such as MinnowBoard 3, LeafHill, etc.  The URL is below:
      https://firmware.intel.com/projects/minnowboard3
      Alternatively, the original IFWI image from the onboard SPI flash can be
      read out as the base image too.

  2.  Build Slim Bootloader source tree and generate a stitching ZIP package.
      The generated ZIP package is located at:
      $(WORKSPACE)/Outputs/apl/Stitch_Components.zip

  3.  Stitch to create a new IFWI image.
      EX:
      python StitchLoader.py -i LEAFHILD.X64.0070.R01.1805070352.bin  -s
             Stitch_Components.zip -o SlimBoot.bin

  4.  Optionally, to view the flash layout for an given IFWI image,
      specify '-i' option only.
      EX:
      python StitchLoader.py -i LEAFHILD.X64.0070.R01.1805070352.bin

"""
FILE_ALIGN  = 0x1000


class IFWI_MANIPULATE:

    def add_component (self, root, path, before = '$', file_path = ''):
        nodes      = path.split('/')
        parent_path = '/'.join(nodes[:-1])
        dir_comp    = IFWI_PARSER.locate_component (root, parent_path)
        if not dir_comp:
            print ('Cannot find DIR %s !' % '/'.join(nodes[:-1]))
            return -1
        if dir_comp.type != COMPONENT.COMP_TYPE['PART']:
            print ('Can only add FILE type !')
            return -2
        index   = None
        if before == '$':
            # Add to end
            index = len(dir_comp.child)
        elif before == '^':
            # Add to top
            index = 0
        else:
            for idx, file in enumerate(dir_comp.child):
                if before == file.name:
                    index = idx
        if index is None:
            print ('Cannot find FILE %s !' % before)
            return -3
        else:
            length = os.path.getsize(file_path) if file_path else 0x1000
            comp = COMPONENT (nodes[-1], COMPONENT.COMP_TYPE['FILE'], 0, length)
            comp.set_data (file_path)
            dir_comp.add_child (comp, index)
            return 0

    def remove_component (self, root, path):
        nodes       = path.split('/')
        parent_path = '/'.join(nodes[:-1])
        dir_comp = IFWI_PARSER.locate_component (root, parent_path)

        if not dir_comp:
            print ('Cannot find DIR %s !' % '/'.join(nodes[:-1]))
            return -1
        if dir_comp.type != COMPONENT.COMP_TYPE['PART']:
            print ('Can only replace FILE type !')
            return -2
        index = None
        for idx, file in enumerate(dir_comp.child):
            if file.name == nodes[-1]:
                index = idx
                break
        if index is None:
            print ('Cannot find FILE %s !' % path)
            return -3
        else:
            del dir_comp.child[index]

        return 0

    def replace_component (self, root, path, file_path):
        comp = IFWI_PARSER.locate_component (root, path)
        if not comp:
            print ('Cannot find FILE %s !' % path)
            return -1
        if comp.type != COMPONENT.COMP_TYPE['FILE']:
            print ('Can only replace FILE type !' % path)
            return -2
        comp.length = os.path.getsize(file_path) if file_path else 0x1000
        if file_path:
            comp.set_data (file_path)
        return 0


    def copy_component (self, root, path, ifwi_data):

        print ("COPY BP0 BPDT to BP1 BPDT ...")
        # Backup BP0 BPDT and BP1 SBPDT
        bp1      = IFWI_PARSER.locate_component (root, 'IFWI/BIOS/BP1')
        bp0bpdt  = IFWI_PARSER.locate_component (root, 'IFWI/BIOS/BP0/BPDT')
        bp1bpdt  = IFWI_PARSER.locate_component (root, 'IFWI/BIOS/BP1/BPDT')
        bp1sbpdt = IFWI_PARSER.locate_component (root, 'IFWI/BIOS/BP1/SBPDT')
        bp0bpdt_data  = bytearray(ifwi_data[bp0bpdt.offset :bp0bpdt.offset  + bp0bpdt.length])
        bp1sbpdt_data = bytearray(ifwi_data[bp1sbpdt.offset:bp1sbpdt.offset + bp1sbpdt.length])

        # Copy to BP0 BPDT to BP1 BPDT
        bp1sbpdt_offset = bp1bpdt.offset + bp0bpdt.length
        ifwi_data[bp1bpdt.offset:bp1sbpdt_offset] = bp0bpdt_data

        # Append original BP1 SBPDT
        bp1sbpdt_end_offset = bp1sbpdt_offset + bp1sbpdt.length
        ifwi_data[bp1sbpdt_offset:bp1sbpdt_end_offset] = bp1sbpdt_data
        padding = bp1.offset + bp1.length - bp1sbpdt_end_offset
        if padding < 0:
            print ('Insufficiant space in BP1 partition !')
            return -1

        ifwi_data[bp1sbpdt_end_offset:bp1sbpdt_end_offset + padding] = b'\xff' * padding

        # Fix Sbpdt length in BP1 BPDT
        offset  = bp1bpdt.offset
        bpdt_hdr = BPDT_HEADER.from_buffer(ifwi_data, offset)
        offset += sizeof(BPDT_HEADER)
        for idx in range(bpdt_hdr.desc_cnt):
            bpdt_entry = BPDT_ENTRY.from_buffer(ifwi_data, offset)
            if "BpdtSbpdt" == str(bpdt_entry.type):
                bpdt_entry.sub_part_size = bp1sbpdt.length
            offset   += sizeof(BPDT_ENTRY)

        # Fix Sbpdt headers
        offset  = bp1sbpdt_offset
        bpdt_hdr = BPDT_HEADER.from_buffer(ifwi_data, offset)
        offset += sizeof(BPDT_HEADER)
        for idx in range(bpdt_hdr.desc_cnt):
            bpdt_entry = BPDT_ENTRY.from_buffer(ifwi_data, offset)
            bpdt_entry.sub_part_offset += (bp0bpdt.length - bp1bpdt.length)
            offset   += sizeof(BPDT_ENTRY)

        print ("Done!")

        return 0


    def create_dir_data (self, dir, ifwi_data):
        # Claculate new DIR length and creaet new DIR data
        support_list = ["BpdtIbb", "BpdtObb"]
        if dir.name not in support_list:
            raise Exception ('Only %s are supported !' % ' '.join(support_list))

        adjust    = True
        offset    = len(dir.child) * sizeof(SUBPART_DIR_ENTRY) + sizeof(SUBPART_DIR_HEADER)
        sub_dir_hdr = SUBPART_DIR_HEADER.from_buffer(ifwi_data, dir.offset)
        dir_data   = bytearray(sub_dir_hdr) + b'\xff' * (offset - sizeof(SUBPART_DIR_HEADER))

        for idx, comp in enumerate(dir.child):
            delta = 0
            parts = os.path.splitext(comp.name)
            if len(parts) > 1 and parts[1] in ['.man', '.met']:
                align = 1
            elif comp.name in ['IPAD', 'OPAD']:
                align = 0x40
            else:
                align = FILE_ALIGN
                delta = dir.offset & (FILE_ALIGN - 1)
            next_offset  = ((offset + delta + align - 1) & ~(align - 1))
            count = next_offset - offset
            if adjust:
                adjust = False
                count -= delta
            dir_data.extend(b'\xff' * count)
            comp_data = comp.get_data()
            if comp_data:
                dir_data.extend(comp_data)
            else:
                dir_data.extend(ifwi_data[comp.offset : comp.offset + comp.length])

            sub_dir = SUBPART_DIR_ENTRY()
            sub_dir.entry_name   = comp.name.encode()
            sub_dir.entry_offset = next_offset - delta
            sub_dir.entry_size   = comp.length
            sub_dir.reserved1   = 0
            sub_dir.reserved2   = 0
            entry_offset = idx * sizeof(SUBPART_DIR_ENTRY) + sizeof(SUBPART_DIR_HEADER)
            dir_data[entry_offset:entry_offset+sizeof(SUBPART_DIR_ENTRY)] = bytearray(sub_dir)

            next_offset += comp.length
            offset = next_offset

        align       = FILE_ALIGN
        next_offset  = ((offset + align - 1) & ~(align - 1))
        dir_data.extend(b'\xff' * (next_offset - offset))

        # Update checksum
        sub_dir_hdr = SUBPART_DIR_HEADER.from_buffer_copy(dir_data)
        sub_dir_hdr.num_of_entries = len(dir.child)
        sub_dir_hdr.checksum = 0
        dir_data[:sizeof(SUBPART_DIR_HEADER)] = bytearray(sub_dir_hdr)

        length    = sub_dir_hdr.num_of_entries * sizeof(SUBPART_DIR_ENTRY) + sizeof(SUBPART_DIR_HEADER)
        sum_buf    = (c_uint8 * length).from_buffer_copy(dir_data)
        sub_dir_hdr.checksum = (~sum(sum_buf) + 1) & 0xFF
        dir_data[:sizeof(SUBPART_DIR_HEADER)] = bytearray(sub_dir_hdr)

        remaining = (dir.offset + len(dir_data))  & (FILE_ALIGN - 1)
        if remaining:
            # Not page aligned, add padding
            dir_data.extend(b'\xff' * (FILE_ALIGN - remaining))

        return dir_data


    def refresh_ifwi_for_dir (self, dir, ifwi_data):
        # Claculate new DIR length and creaet new DIR data
        dir_data = self.create_dir_data (dir, ifwi_data)

        length  = len (dir_data)
        adjust_length = length - dir.length
        if (dir.offset + length)  & (FILE_ALIGN - 1):
            print  ('DIR total size needs to be 4KB aligned !')

        # Remember original SBPDT offset
        org_bpdt_offset  = dir.parent.parent.child[0].offset
        org_sbpdt_offset = dir.parent.parent.child[1].offset

        # Adjust offset and size for peer and up level in tree
        old_dir = dir
        while dir.type != COMPONENT.COMP_TYPE['BP']:
            for each in dir.parent.child:
                if each.offset > dir.offset:
                    each.offset += adjust_length
            dir.length += adjust_length
            dir = dir.parent
        dir = old_dir

        # Update parent BPDT header info in IFWI data
        parent  = dir.parent
        bpdt_hdr = BPDT_HEADER.from_buffer(ifwi_data, parent.offset)
        base    = parent.offset + sizeof(BPDT_HEADER)
        found   = False

        for idx in range(bpdt_hdr.desc_cnt):
            bpdt_entry = BPDT_ENTRY.from_buffer(ifwi_data, base + idx * sizeof(BPDT_ENTRY))
            comps = [x for x in parent.child if x.name == str(bpdt_entry.type)]
            if len(comps) == 0:
                continue
            if len(comps) > 1:
                raise Exception ('Found duplicated DIR %s !', bpdt_entry.type)
            bpdt_entry.sub_part_offset = comps[0].offset - parent.parent.offset
            if dir.name == str(bpdt_entry.type):
                bpdt_entry.sub_part_size = length
                found = True
        if not found:
            raise Exception ('Could not find DIR %s !', dir.name)

        # Update SBPDT DIR header in IFWI data
        bp_comp = parent.parent
        if parent.name == 'BPDT':
            bpdt_hdr    = BPDT_HEADER.from_buffer (ifwi_data, org_sbpdt_offset)
            bpdt_hdr.xor_sum = 0
            base_offset = org_sbpdt_offset + sizeof(BPDT_HEADER)
            for idx in range(bpdt_hdr.desc_cnt):
                bpdt_entry = BPDT_ENTRY.from_buffer(ifwi_data, base_offset + idx * sizeof(BPDT_ENTRY))
                bpdt_entry.sub_part_offset += adjust_length
                if  (bpdt_entry.sub_part_offset + bpdt_entry.sub_part_size) > bp_comp.length:
                    raise Exception ('Insufficiant space in layout !')
        else:
            # 'SBPDT', update length in BPDT
            bpdt_hdr    = BPDT_HEADER.from_buffer (ifwi_data, org_bpdt_offset)
            bpdt_hdr.xor_sum = 0
            base_offset = org_bpdt_offset + sizeof(BPDT_HEADER)
            for idx in range(bpdt_hdr.desc_cnt):
                bpdt_entry = BPDT_ENTRY.from_buffer(ifwi_data, base_offset + idx * sizeof(BPDT_ENTRY))
                if str(bpdt_entry.type) == "BpdtSbpdt":
                    bpdt_entry.sub_part_size += adjust_length
                if  (bpdt_entry.sub_part_offset + bpdt_entry.sub_part_size) > bp_comp.length:
                    raise Exception ('Insufficiant space in layout !')

        # Generate actual final IFWI Data
        if adjust_length > 0:
            ifwi_data[:] = ifwi_data[:old_dir.offset] + dir_data + \
                          ifwi_data[old_dir.offset + old_dir.length - adjust_length : bp_comp.offset + bp_comp.length - adjust_length] + \
                          ifwi_data[bp_comp.offset + bp_comp.length:]
        else:
            adjust_length = -adjust_length
            ifwi_data[:] = ifwi_data[:old_dir.offset] + dir_data + \
                          ifwi_data[old_dir.offset + old_dir.length + adjust_length: bp_comp.offset + bp_comp.length] + \
                          b'\xff' * adjust_length + ifwi_data[bp_comp.offset + bp_comp.length:]

        return 0




def manipulate_ifwi (action, path, ifwi_data, file_name = '', before = '$'):
    print ('%s %s ...' % (action, path))

    root    = IFWI_PARSER.parse_ifwi_binary (ifwi_data)
    ifwi_op = IFWI_MANIPULATE()

    if action == "REMOVE":
        ret  = ifwi_op.remove_component (root, path)
    elif action == "ADD":
        ret  = ifwi_op.add_component (root, path, before, file_name)
    elif action == "REPLACE":
        ret  = ifwi_op.replace_component (root, path, file_name)
    elif action == "COPY":
        ret  = ifwi_op.copy_component (root, 'IFWI/BIOS/BP0/BPDT', ifwi_data)
    else:
        ret  = -100

    if ret == 0 and path:
        dir_path = '/'.join(path.split('/')[:-1])
        dir = IFWI_PARSER.locate_component (root, dir_path)
        ifwi_op.refresh_ifwi_for_dir (dir, ifwi_data)
        print ('done!')

    return ret



def patch_flash_map (image_data, platform_data = 0xffffffff):
    comp_bpdt_dict = {
      b'RSVD' : "IFWI/BIOS/BP1/SBPDT/BpdtObb/RSVD",
      b'IAS1' : "IFWI/BIOS/BP1/SBPDT/BpdtObb/FB",
      b'EPLD' : "IFWI/BIOS/BP1/SBPDT/BpdtObb/EPLD",
      b'UVAR' : "IFWI/BIOS/BP1/SBPDT/BpdtObb/UVAR",
      b'PYLD' : "IFWI/BIOS/BP0/BPDT/BpdtIbb/PLD",
      b'VARS' : "IFWI/BIOS/BP0/BPDT/BpdtIbb/VAR",
      b'MRCD' : "IFWI/BIOS/BP0/BPDT/BpdtIbb/MRCD",
      b'CNFG' : "IFWI/BIOS/BP0/BPDT/BpdtIbb/CFGD",
      b'KEYH' : "IFWI/BIOS/BP0/BPDT/BpdtIbb/KEYH",
      b'FWUP' : "IFWI/BIOS/BP0/BPDT/BpdtIbb/FWUP",
      b'SG02' : "IFWI/BIOS/BP0/BPDT/BpdtIbb/OBB",
      b'SG1B' : "IFWI/BIOS/BP0/BPDT/BpdtIbb/IBB",
      b'SG1A' : "IFWI/BIOS/BP0/BPDT/BpdtIbb/IBBL",
      b'_BPM' : "IFWI/BIOS/BP0/BPDT/BpdtIbb/BPM.met",
    }

    print ("Patching Slim Bootloader Flash Map table ...")

    output_image_data  =  image_data
    ifwi = IFWI_PARSER.parse_ifwi_binary (output_image_data)
    if not ifwi:
        return -1

    pld  = IFWI_PARSER.locate_component (ifwi, comp_bpdt_dict[b'PYLD'])
    if not pld:
        comp_bpdt_dict[b'PYLD'] = "IFWI/BIOS/BP1/SBPDT/BpdtObb/PLD"

    bp0  = IFWI_PARSER.locate_component (ifwi, 'IFWI/BIOS/BP0')
    bp1  = IFWI_PARSER.locate_component (ifwi, 'IFWI/BIOS/BP1')
    if not bp0 or not bp1:
        return -2

    # Locate FlashMap offset
    for part in range(2):
        path = comp_bpdt_dict[b'SG1A'].replace("BP0", "BP%d" % part)
        comp = IFWI_PARSER.locate_component (ifwi, path)
        if not comp:
            if part == 0:
                raise Exception("Cannot locate %s !" % path)
            else:
                continue
        stage1AOffset = comp.offset
        stage1ALength = comp.length
        temp = stage1AOffset + stage1ALength - 8

        c_uint32.from_buffer (output_image_data, temp - 4).value = platform_data

        fla_map_off = (bytes_to_value(output_image_data[temp:temp+4]) + stage1ALength) & 0xFFFFFFFF
        fla_map_str = FLASH_MAP.from_buffer (output_image_data, stage1AOffset + fla_map_off)
        entry_num  = (fla_map_str.length - sizeof(FLASH_MAP)) // sizeof(FLASH_MAP_DESC)
        fla_map_str.romsize = bp0.length + bp1.length

        if part == 1:
            fla_map_str.attributes |= FLASH_MAP.FLASH_MAP_ATTRIBUTES['BACKUP_REGION']

        for idx in range (entry_num):
            desc  = FLASH_MAP_DESC.from_buffer (output_image_data, stage1AOffset + fla_map_off + sizeof(FLASH_MAP) + idx * sizeof(FLASH_MAP_DESC))
            path = comp_bpdt_dict[desc.sig]
            if part == 1 or (desc.flags & FLASH_MAP.FLASH_MAP_DESC_FLAGS['NON_REDUNDANT']):
                path = path.replace("BP0", "BP1")
            if part == 1 and (desc.flags & FLASH_MAP.FLASH_MAP_DESC_FLAGS['REDUNDANT']):
                desc.flags |= FLASH_MAP.FLASH_MAP_DESC_FLAGS['BACKUP']
            if desc.sig == b'RSVD':
                desc.offset = bp1.offset + bp1.length - desc.size - bp0.offset
                continue

            comp  = IFWI_PARSER.locate_component (ifwi, path)
            if not comp:
                if desc.sig == b'KEYH':
                    continue
                raise Exception("Cannot locate component '%s' in BPDT !" % path)
            if (desc.size == 0) and (desc.offset == 0):
                desc.size = comp.length
                desc.offset = comp.offset - bp0.offset
                continue
            if desc.size != comp.length and comp.name != 'FB':
                raise Exception("Mismatch component '%s' length in FlashMap and BPDT !" % comp_bpdt_dict[desc.sig])
            if desc.sig not in [b'_BPM'] and (comp.offset & 0xFFF > 0):
                raise Exception("Component '%s' %x is not aligned at 4KB boundary, " \
                                "please adjust padding size for IPAD/OPAD in BoardConfig.py and rebuild !" % (comp_bpdt_dict[desc.sig], comp.offset))
            desc.offset = comp.offset - bp0.offset

            # Last 4k in bios region is reserved for bootloader, throw Exception if any component falls in that range
            if (bp1.offset + bp1.length - 0x1000) <= (desc.offset + desc.size) <= (bp1.offset + bp1.length):
                raise Exception("Component '%s' offset is in bootloader reserved region, please try to reduce compoent size !" % comp_bpdt_dict[desc.sig])

        limit = bp1.offset + bp1.length - bp0.offset - 0x40000
        for idx in range (entry_num):
            desc  = FLASH_MAP_DESC.from_buffer (output_image_data, stage1AOffset + fla_map_off + sizeof(FLASH_MAP) + idx * sizeof(FLASH_MAP_DESC))
            if desc.sig == b'RSVD':
                continue
            # Last 256K flash space (4GB - 256KB to 4GB) is remapped to CSME read-only SRAM on APL
            # Directly access is not available.
            if desc.offset >= limit or desc.offset + desc.size > limit:
                print("WARNING: Component '%s' in BP%d is located inside CSME memory mapped region, direct access might fail." % (desc.sig, part))

    print ("Flash map was patched successfully!")

    return 0


def create_ifwi_image (ifwi_in, ifwi_out, bios_out, platform_data, non_redundant, stitch_dir):

    redundant_payload = True

    ifwi_data = bytearray (get_file_data (ifwi_in))
    root = IFWI_PARSER.parse_ifwi_binary (ifwi_data)
    if not root:
        raise Exception ('Invalid IFWI input image format !')

    # Verify if Boot Guard is enabled or not
    comp = IFWI_PARSER.locate_component (root, "IFWI/BIOS/BP0/BPDT/BpdtUepType")
    if not comp:
        raise Exception ('Unsupported base image format !')

    data = ifwi_data[comp.offset + 0x30:comp.offset + 0x32]
    if (data[0] & 0x0F) != 0x00:
        raise Exception ('Unsupported base image type. boot guard might have been enabled in this image !')

    print ('Creating %sredundant image ...' % ('non-' if non_redundant else ''))


    # Remove all in IBB/OBB
    remove_list = [
      "IFWI/BIOS/BP0/BPDT/BpdtIbb",
      "IFWI/BIOS/BP1/BPDT/BpdtIbb",
      "IFWI/BIOS/BP1/SBPDT/BpdtObb"
    ]
    for dir_path in remove_list:
        comp = IFWI_PARSER.locate_component (root, dir_path)
        if not comp:
            continue
        for each in comp.child:
            if each.name.endswith('.man') or each.name.endswith('.met'):
                continue
            ret = manipulate_ifwi  ('REMOVE', dir_path + '/' + each.name,  ifwi_data)
            if ret != 0:
                raise Exception ('REMOVE failed (error code %d) !' % (ret))

    # Copy BP0 BPDT into BP1 BPDT
    if not non_redundant:
        ret = manipulate_ifwi  ('COPY', '', ifwi_data)
        if ret != 0:
            raise Exception ('COPY failed (error code %d) !' % (ret))

    if stitch_dir:
        ibb_list = [
          ('IBBL' , 'IBBL'),
          ('IBB'  , 'IBBM'),
          ('OBB'  , 'OBB'),
          ('FWUP' , 'FWU'),
          ('CFGD' , 'CFGDATA'),
          ('KEYH' , 'KEYHASH'),
          ('VAR'  , 'VAR'),
          ('MRCD' , 'MRCDATA'),
          ('PLD'  , 'PLD'),
        ]

        obb_list = [
          ('FB' , 'FB'),
          ('EPLD' , 'EPLD'),
          ('UVAR' , 'UVAR'),
          ('PLD'  , 'PLD'),
        ]

        # optional components
        opt_list = [
          'EPLD', 'UVAR'
        ]

        if redundant_payload:
            del obb_list[-1]
        else:
            del ibb_list[-1]

        bp1sbpdt = "IFWI/BIOS/BP1/SBPDT/BpdtObb/"

        loop = 1 if non_redundant else 2
        for bp in range(loop):
            dir = "IFWI/BIOS/BP%d/BPDT/BpdtIbb/" % bp
            for comp_name, file_name in ibb_list:
                file_path = os.path.join(stitch_dir, 'Stitch_%s.bin' % file_name)
                ret = manipulate_ifwi  ('ADD', dir + comp_name,  ifwi_data, file_path)
                if ret != 0:
                    raise Exception ('ADD failed (error code %d) !' % (ret))

        for comp_name, file_name in obb_list:
            if file_name == '':
                file_path = ''
            else:
                file_path = os.path.join(stitch_dir, 'Stitch_%s.bin' % file_name)
            if (comp_name in opt_list) and not os.path.exists(file_path):
                ret = 0
            else:
                ret = manipulate_ifwi  ('ADD', bp1sbpdt + comp_name,  ifwi_data, file_path)
            if ret != 0:
                raise Exception ('ADD failed (error code %d) !' % (ret))

        patch_flash_map (ifwi_data, platform_data)

    if bios_out:
      print ('Creating BIOS image ...')
      bios = IFWI_PARSER.locate_component (root, 'IFWI/BIOS')
      fd = open (bios_out, 'wb')
      fd.write(ifwi_data[bios.offset:bios.offset+bios.length])
      fd.close()

    print ('Creating IFWI image ...')
    fd = open (ifwi_out, 'wb')
    fd.write(ifwi_data)
    fd.close()

    print ('Done!')


def print_ifwi_layout (ifwi_file):
    ifwi_parser = IFWI_PARSER ()
    ifwi_bin = bytearray (get_file_data (ifwi_file))
    ifwi = ifwi_parser.parse_ifwi_binary (ifwi_bin)
    if ifwi:
        ifwi_parser.print_tree (ifwi)
    else:
        print ('Invalid IFWI image')
    return 0


if __name__ == '__main__':

    hexstr = lambda x: int(x, 16)
    ap = argparse.ArgumentParser()
    ap.add_argument('-i',
                    '--input-ifwi-file',
                    dest='ifwi_in',
                    type=str,
                    required=True,
                    help='specify input template IFWI image file path')

    ap.add_argument('-o',
                    '--output-ifwi-file',
                    dest='ifwi_out',
                    type=str,
                    default='',
                    help='specify generated output IFWI image file path')

    ap.add_argument('-b',
                    '--output-bios-region',
                    dest='bios_out',
                    type=str,
                    default='',
                    help='specify generated output BIOS region image file path')

    ap.add_argument('-s',
                    '--sitch-zip-file',
                    dest='stitch_in',
                    type=str,
                    default='',
                    help='specify input sitching zip package file path')

    ap.add_argument('-p',
                    '--platform-data',
                    dest='plat_data',
                    type=hexstr,
                    default=0xFFFFFFFF,
                    help='specify a platform specific data (HEX, DWORD) for customization')

    ap.add_argument('-n',
                    '--non-redundant',
                    dest='non_redundant',
                    action="store_true",
                    help='specify if the flash layout will be full redundant or not')

    if len(sys.argv) == 1:
        print('%s' % extra_usage_txt)

    args = ap.parse_args()

    if args.ifwi_out == '' and args.stitch_in == '':
        print_ifwi_layout (args.ifwi_in)
        sys.exit (0)
    else:
        if args.ifwi_out and args.stitch_in == '':
            ret = create_ifwi_image (args.ifwi_in, args.ifwi_out, args.bios_out, args.plat_data, args.non_redundant, None)
            sys.exit (ret)

    # Unpack files from zip
    print ("Unpacking sitching ZIP package ...")
    output_dir = os.path.dirname(args.ifwi_out)
    stitch_dir = os.path.join(output_dir, 'stitch_comp')
    if os.path.exists(stitch_dir):
        shutil.rmtree(stitch_dir)
    zf = zipfile.ZipFile(args.stitch_in, 'r', zipfile.ZIP_DEFLATED)
    zf.extractall(stitch_dir)
    zf.close()

    # Create new IFWI
    ret = create_ifwi_image (args.ifwi_in, args.ifwi_out, args.bios_out, args.plat_data, args.non_redundant, stitch_dir)

    # Remove extracted files
    if os.path.exists(stitch_dir):
        shutil.rmtree(stitch_dir)

    sys.exit (ret)
