## @ ifwi_utility.py
#
# copyright (c) 2019 - 2021, intel corporation. all rights reserved.<BR>
# SPDX-license-identifier: BSD-2-clause-patent
#
##
import sys
import os
import argparse
from   ctypes import Structure, c_char, c_uint32, c_uint8, c_uint64, c_uint16, sizeof, ARRAY

sys.dont_write_bytecode = True
from   CommonUtility import *

class UCODE_HEADER (Structure):
    _pack_ = 1
    _fields_ = [
        ('header_version',  c_uint32),
        ('update_revision',  c_uint32),
        ('date',  c_uint32),
        ('processor_signature',  c_uint32),
        ('checksum',  c_uint32),
        ('loader_revision',  c_uint32),
        ('processor_flags',  c_uint32),
        ('data_size',  c_uint32),
        ('total_size',  c_uint32),
        ('reserved',  ARRAY(c_uint8, 12)),
        ]


class FIT_ENTRY(Structure):
    FIT_OFFSET    = -0x40
    FIT_SIGNATURE = b'_FIT_   '

    _pack_ = 1
    _fields_ = [
        ('address',  c_uint64),
        ('size',     c_uint32), # Bits[31:24] Reserved
        ('version',  c_uint16),
        ('type',     c_uint8), # Bit[7] = C_V
        ('checksum', c_uint8),
        ]

    def set_values(self, _address, _size, _version, _type, _checksum):
        self.address  = _address
        self.size     = _size
        self.version  = _version
        self.type     = _type
        self.checksum = _checksum

class FIT_ENTRY_V200(Structure):
    # Support FIT entry version 0x200 structure
    _pack_ = 1
    _fields_ = [
        ('address',  c_uint64),
        ('fms',      c_uint32),
        ('version',  c_uint16),
        ('type',     c_uint8), # Bit[7] = C_V
        ('ext_fms',  c_uint8),
        ]

    def set_values(self, _address, _fms, _version, _type, _ext_fms):
        self.address  = _address
        self.fms      = _fms
        self.version  = _version
        self.type     = _type
        self.ext_fms   = _ext_fms

class BPDT_ENTRY_TYPE(Structure):
    _pack_ = 1
    _fields_ = [('data', c_uint16)]

    BPDT_PART_VAL = {
        "BpdtOemSmip"       : 0,
        "BpdtCseRbe"        : 1,
        "BpdtCseBup"        : 2,
        "BpdtUcode"         : 3,
        "BpdtIbb"           : 4,
        "BpdtSbpdt"         : 5,
        "BpdtObb"           : 6,
        "BpdtCseMain"       : 7,
        "BpdtIsh"           : 8,
        "BpdtCseIdlm"       : 9,
        "BpdtIfpOverride"   : 10,
        "BpdtDebugTokens"   : 11,
        "BpdtUfsPhyConfig"  : 12,
        "BpdtUfsGppLunId"   : 13,
        "BpdtPmc"           : 14,
        "BpdtIunit"         : 15,
        "BpdtNvmConfig"     : 16,
        "BpdtUepType"       : 17,
        "BpdtUfsRateType"   : 18,
        "BpdtInvalidType"   : 19,
    }

    BPDT_PART_NAME = {v: k for k, v in BPDT_PART_VAL.items()}

    def __init__(self, val=0):
        self.set_value(val)

    def __str__(self):
        if self.value < 0 or self.value >= self.BPDT_PART_VAL['BpdtInvalidType']:
            str = "BpdtInvalidType"
        else:
            str = self.BPDT_PART_NAME[self.value]
        return str

    def __int__(self):
        return self.get_value()

    def set_value(self, val):
        self.data = val

    def get_value(self):
        return self.data

    value = property(get_value, set_value)


class BPDT_INFO():
    def __init__(self, name, offset, bpdt_offset, primary):
        self.name        = name
        self.primary     = primary
        self.offset      = offset
        self.bpdt_offset = bpdt_offset


class BPDT_HEADER(Structure):
    _pack_   = 1
    _fields_ = [
        ('signature',    c_uint32),
        ('desc_cnt',     c_uint16),
        ('version',      c_uint16),
        ('xor_sum',      c_uint32),
        ('ifwi_ver',     c_uint32),
        ('reserved',     ARRAY(c_uint8, 8))
    ]


class BPDT_ENTRY(Structure):
    _pack_   = 1
    _fields_ = [
        ('type',            BPDT_ENTRY_TYPE),
        ('flags',           c_uint16),
        ('sub_part_offset', c_uint32),
        ('sub_part_size',   c_uint32),
    ]


class SUBPART_DIR_HEADER(Structure):
    _pack_ = 1
    _fields_ = [
        ('header_marker',   ARRAY(c_char, 4)),
        ('num_of_entries',  c_uint32),
        ('header_version',  c_uint8),
        ('entry_version',   c_uint8),
        ('header_length',   c_uint8),
        ('checksum',        c_uint8),
        ('sub_part_name',   ARRAY(c_char, 4)),
    ]


class SUBPART_DIR_ENTRY(Structure):
    _pack_ = 1
    _fields_ = [
        ('entry_name',     ARRAY(c_char, 12)),
        ('entry_offset',   c_uint32, 24),
        ('reserved1',      c_uint32, 8),
        ('entry_size',     c_uint32),
        ('reserved2',      c_uint32),
    ]

class BIOS_ENTRY(Structure):
    _pack_ = 1
    _fields_ = [
        ('name',           ARRAY(c_char, 4)),
        ('offset',         c_uint32),
        ('length',         c_uint32),
        ('reserved',       c_uint32),
    ]

class SPI_DESCRIPTOR(Structure):
    DESC_SIGNATURE = 0x0FF0A55A
    FLASH_REGIONS  = {
        "descriptor"       : 0x00,
        "bios"             : 0x04,
        "txe"              : 0x08,
        "gbe"              : 0x0c,
        "pdr"              : 0x10,
        "dev_expansion"    : 0x14,
    }
    _pack_ = 1
    _fields_ = [
        ('reserved',       ARRAY(c_char, 16)),
        ('fl_val_sig',     c_uint32),
        ('fl_map0',        c_uint32),
        ('fl_map1',        c_uint32),
        ('fl_map2',        c_uint32),
        ('remaining',      ARRAY(c_char, 0x1000 - 0x20)),
    ]


class COMPONENT:
    COMP_TYPE = {
        "IFWI" : 0,
        "RGN"  : 1,
        "BP"   : 2,
        "BPDT" : 3,
        "PART" : 4,
        "FILE" : 5,
    }

    def __init__(self, name, com_type, offset, length):
        self.name   = name
        self.type   = com_type
        self.offset = offset
        self.length = length
        self.child  = []
        self.data   = None
        self.parent = None

    def add_child(self, child, index = -1):
        child.parent = self
        if index == -1:
            self.child.append (child)
        else:
            self.child.insert (index, child)

    def set_data(self, file):
        if file:
            fd =open(file, 'rb')
            data = bytearray(fd.read())
            fd.close()
        else:
            data = bytearray(b'\xff' * self.length)
        if self.length > len(data):
            self.data = data + b'\xff' * (self.length - len(data))
        else:
            self.data = data[:self.length]

    def get_data(self):
        return self.data


class FLASH_MAP_DESC(Structure):
    _pack_   = 1
    _fields_ = [
        ('sig',     ARRAY(c_char, 4)),
        ('flags',   c_uint32),
        ('offset',  c_uint32),
        ('size',    c_uint32),
    ]


class FLASH_MAP(Structure):
    FLASH_MAP_SIGNATURE  = b'FLMP'

    FLASH_MAP_COMPONENT_SIGNATURE = {
        "STAGE1A"       : "SG1A",
        "STAGE1B"       : "SG1B",
        "STAGE2"        : "SG02",
        "ACM"           : "ACM0",
        "DIAGNOSTICACM" : "DACM",
        "UCODE"         : "UCOD",
        "MRCDATA"       : "MRCD",
        "VARIABLE"      : "VARS",
        "PAYLOAD"       : "PYLD",
        "EPAYLOAD"      : "EPLD",
        "SIIPFW"        : "IPFW",
        "UEFIVARIABLE"  : "UVAR",
        "SPI_CONTAINER1": "CTR1",
        "SPI_CONTAINER2": "CTR2",
        "FWUPDATE"      : "FWUP",
        "CFGDATA"       : "CNFG",
        "KEYHASH"       : "KEYH",
        "BPM"           : "_BPM",
        "OEMKEY"        : "OEMK",
        "SBLRSVD"       : "RSVD",
        "EMPTY"         : "EMTY",
        "UNKNOWN"       : "UNKN",
      }

    FLASH_MAP_ATTRIBUTES = {
        "PRIMARY_REGION"  : 0x00000000,
        "BACKUP_REGION"   : 0x00000001,
    }

    FLASH_MAP_DESC_FLAGS = {
        "TOP_SWAP"     : 0x00000001,
        "REDUNDANT"    : 0x00000002,
        "NON_REDUNDANT": 0x00000004,
        "NON_VOLATILE" : 0x00000008,
        "COMPRESSED"   : 0x00000010,
        "BACKUP"       : 0x00000040,
    }

    FLASH_MAP_REGION = {
        0x00:  "RGN",
        0x01:  "TS0",
        0x41:  "TS1",
        0x02:  "RD0",
        0x42:  "RD1",
        0x04:  "NRD",
        0x08:  "NVS",
    }

    _pack_ = 1
    _fields_ = [
        ('sig',          ARRAY(c_char, 4)),
        ('version',      c_uint16),
        ('length',       c_uint16),
        ('attributes',   c_uint8),
        ('reserved',     ARRAY(c_char, 3)),
        ('romsize',      c_uint32),
        ]

    def __init__(self):
        self.sig = FLASH_MAP.FLASH_MAP_SIGNATURE
        self.version = 1
        self.romsize = 0
        self.attributes = 0
        self.length  = sizeof(self)
        self.descriptors = []

    def add(self, desc):
        self.descriptors.append(desc)

    def finalize (self):
        # Calculate size of the flash map
        self.romsize = sum ([x.size for x in self.descriptors])
        self.length  = sizeof(self) + len(self.descriptors) * sizeof(FLASH_MAP_DESC)


class UCODE_PARSER:

    @staticmethod
    def dump (bin):
        ucode_list = UCODE_PARSER.parse (bin)
        for idx, bin in enumerate(ucode_list):
            print ('Microcode %d:' % (idx + 1))
            ucode_hdr = UCODE_HEADER.from_buffer(bin)
            print ('    Processor : %X' % (ucode_hdr.processor_signature))
            print ('    Revision  : %X' % (ucode_hdr.update_revision))
            month  = (ucode_hdr.date & 0xFF000000) >> 24
            day    = (ucode_hdr.date & 0xFF0000) >> 16
            year   = ucode_hdr.date & 0xFFFF
            print ('    Date      : %02X/%02X/%04X' % (month, day, year))
            print ('    Length    : %X' % (ucode_hdr.total_size))

    @staticmethod
    def extract (bin, out_dir):
        ucode_list = UCODE_PARSER.parse (bin)
        for idx, bin in enumerate(ucode_list):
            ucode_hdr = UCODE_HEADER.from_buffer(bin)
            name = '%03d0_%08X_%08X.mcb' % (idx, ucode_hdr.processor_signature, ucode_hdr.update_revision)
            path = os.path.join (out_dir, name)
            gen_file_from_object (path, bin)
        print ("%d microcode binaries were extraced to directory '%s' !" % (idx + 1, out_dir))

    @staticmethod
    def is_valid (ucode):
        valid = True
        ucode_hdr = UCODE_HEADER.from_buffer(ucode)
        if ucode_hdr.header_version != 1:
            print ('ERROR: Invalid header version !')
            valid = False
        if bytearray(ucode_hdr.reserved) != b'\x00' * 12:
            print ('ERROR: Invalid reserved bytes !')
            valid = False
        if ucode_hdr.total_size % 1024 != 0:
            print ('ERROR: Invalid total size !')
            valid = False
        data = ARRAY(c_uint32, ucode_hdr.total_size >> 2).from_buffer(ucode)
        if (sum(data) & 0xffffffff) != 0:
            print ('ERROR: Invalid checksum !')
            valid = False
        return valid

    @staticmethod
    def pack (ucode_files, out_file = None):
        bins = bytearray()
        if type(ucode_files) is type([]):
            ucode_list = ucode_files
        elif os.path.isdir(ucode_files):
            ucode_list = [os.path.join(ucode_files, f) for f in sorted(os.listdir(ucode_files)) if f.endswith('.mcb')]
        else:
            return bins

        for ucode in ucode_list:
            bin = bytearray (get_file_data (ucode))
            if UCODE_PARSER.is_valid (bin):
                ucode_hdr = UCODE_HEADER.from_buffer(bin)
                bins.extend (bin[:ucode_hdr.total_size])
            else:
                print ("Microcode file '%s' is ignored !" % ucode)
        if out_file:
            gen_file_from_object (out_file, bins)
        return bins

    @staticmethod
    def parse (bin):
        ucode  = []
        offset = 0
        valid  = True
        while valid and (offset < len(bin)):
            ucode_hdr = UCODE_HEADER.from_buffer(bin, offset)
            if ucode_hdr.header_version == 0xffffffff:
                break
            valid = UCODE_PARSER.is_valid (bin)
            if valid:
                ucode.append (bytearray(bin[offset:offset+ucode_hdr.total_size]))
            offset += ucode_hdr.total_size
        return ucode


class IFWI_PARSER:
    def __init__(self):
        return

    @staticmethod
    def is_ifwi_image(bios_bins):
        spi_desc = SPI_DESCRIPTOR.from_buffer(bios_bins)
        return spi_desc.fl_val_sig == spi_desc.DESC_SIGNATURE

    @staticmethod
    def locate_components(root, path):
        result = []
        nodes  = path.split('/')
        if len(nodes) < 1 or root.name != nodes[0]:
            return []
        if len(nodes) == 1:
            return [root]
        for comp in root.child:
            ret = IFWI_PARSER.locate_components(comp, '/'.join(nodes[1:]))
            if len(ret) > 0:
                result.extend(ret)
        return result

    @staticmethod
    def locate_component(root, path):
        result = IFWI_PARSER.locate_components(root, path)
        if len(result) > 0:
            return result[0]
        else:
            return None

    @staticmethod
    def find_components(root, name, comp_type = COMPONENT.COMP_TYPE['FILE']):
        result = []
        if root.type == comp_type and root.name == name:
            return [root]
        for comp in root.child:
            ret = IFWI_PARSER.find_components(comp, name, comp_type)
            if len(ret) > 0:
                result.extend(ret)
        return result

    @staticmethod
    def get_component_path (comp):
        path = []
        while comp:
            path.append (comp.name)
            comp = comp.parent
        return '/'.join(path[::-1])

    @staticmethod
    def print_tree(root, level=0):
        if root is None:
            return
        print ("%-24s [O:0x%08X  L:0x%08X]" % ('  ' * level + root.name,
                root.offset, root.length))
        for comp in root.child:
            level += 1
            IFWI_PARSER.print_tree(comp, level)
            level -= 1

        bp = IFWI_PARSER.locate_component (root, 'IFWI/BIOS/BP0')
        if bp:
            print ("\nBPDT Space Information:")
            for idx in range(2):
                bp = IFWI_PARSER.locate_component (root, 'IFWI/BIOS/BP%d' % idx)
                if len(bp.child) > 1:
                    sbpdt = bp.child[1]
                    bplen = bp.length - ((sbpdt.offset + sbpdt.length) - bp.offset)
                else:
                    bplen = bp.length
                print ("  BP%d Free Space: 0x%05X" % (idx, bplen))


    @staticmethod
    def find_ifwi_region (spi_descriptor, rgn_name):
        frba      = ((spi_descriptor.fl_map0 >> 16) & 0xFF) << 4
        reg_off   = spi_descriptor.FLASH_REGIONS[rgn_name]
        fl_reg    = reg_off + frba
        rgn_off   = c_uint32.from_buffer(spi_descriptor, fl_reg)
        rgn_base  = (rgn_off.value & 0x7FFF) << 12
        rgn_limit = ((rgn_off.value & 0x7FFF0000) >> 4) | 0xFFF
        if (reg_off > 0 and rgn_off.value == 0) or (rgn_off.value == 0xFFFFFFFF) or (rgn_limit <= rgn_base):
            return None, None
        else:
            return (rgn_base, rgn_limit)

    @staticmethod
    def get_boot_partition_from_path (comp_path):
        if   '/RD0/' in comp_path or '/TS0/' in comp_path:
            bp = 0
        elif '/RD1/' in comp_path or '/TS1/' in comp_path:
            bp = 1
        else:
            bp = 0
        return bp

    @staticmethod
    def update_ucode_fit_entry (ifwi_bin, ucode_path):
        ifwi = IFWI_PARSER.parse_ifwi_binary (ifwi_bin)
        if not ifwi:
            print ("Not a valid ifwi image!")
            return -2

        # Get microcode
        ucode_comps = IFWI_PARSER.locate_components (ifwi, ucode_path)
        if len(ucode_comps) == 0:
            print ("Cannot find microcode component in ifwi image!" % path)
            return -3

        # Get partition from path
        bp = IFWI_PARSER.get_boot_partition_from_path (ucode_path)

        # Get fit entry
        path = 'IFWI/BIOS/TS%d/SG1A' % bp
        ifwi_comps = IFWI_PARSER.locate_components (ifwi, path)
        if len(ifwi_comps) == 0:
            path = 'IFWI/BIOS/SG1A' % bp
            ifwi_comps = IFWI_PARSER.locate_components (ifwi, path)
        if len(ifwi_comps) == 0:
            print ("Cannot find 'SG1A' in ifwi image!" % path)
            return -4

        img_base   = 0x100000000 - len(ifwi_bin)
        fit_addr   = c_uint32.from_buffer(ifwi_bin, ifwi_comps[0].offset + ifwi_comps[0].length + FIT_ENTRY.FIT_OFFSET)
        fit_offset = fit_addr.value - img_base
        fit_header = FIT_ENTRY.from_buffer(ifwi_bin, fit_offset)

        if fit_header.address != bytes_to_value (bytearray(FIT_ENTRY.FIT_SIGNATURE)):
            print ("Cannot find FIT table !" % path)
            return -4

        # Update Ucode entry address
        ucode_idx  = 0
        ucode_off  = ucode_comps[0].offset
        ucode_list = UCODE_PARSER.parse (ifwi_bin[ucode_off:])
        for fit_type in [0x01, 0x7f]:
            for idx in range(fit_header.size):
                fit_entry = FIT_ENTRY.from_buffer(ifwi_bin, fit_offset + (idx + 1) * 16)
                if fit_entry.type == fit_type:
                    if ucode_idx < len(ucode_list):
                        fit_entry.set_values(img_base + ucode_off, 0, 0x100, 0x1, 0)
                        ucode_off += len(ucode_list[ucode_idx])
                        ucode_idx += 1
                    else:
                        # more fit entry is available, clear this entry
                        fit_entry.type = 0x7f

        if ucode_idx != len(ucode_list):
            print ("Not all microcode can be listed in FIT table due to limited FIT entry number !")
            return -5

        # Update FIT checksum
        fit_header.checksum  = 0
        fit_sum = sum(ifwi_bin[fit_offset:fit_offset+fit_header.size*16])
        fit_header.checksum = (0 - fit_sum) & 0xff
        return 0

    @staticmethod
    def replace_component (ifwi_bin, comp_bin, path):

        ifwi = IFWI_PARSER.parse_ifwi_binary (ifwi_bin)
        if not ifwi:
            print ("Not a valid ifwi image!")
            return -2

        ifwi_comps = IFWI_PARSER.locate_components (ifwi, path)
        if len(ifwi_comps) == 0:
            print ("Cannot find path '%s' in ifwi image!" % path)
            return -4

        for ifwi_comp in ifwi_comps:
            gap = len(comp_bin) - ifwi_comp.length
            if gap > 0:
                print ("Component image file is too big (0x%x vs 0x%x)!" % (ifwi_comp.length, len(comp_bin)))
                return -5
            elif gap < 0:
                gap = -gap
                print ("Padding 0x%x bytes at the end to fill the region '%s'" % (gap, ifwi_comp.name))
                comp_bin.extend (b'\xff' * gap)

            ifwi_bin[ifwi_comp.offset:ifwi_comp.offset + ifwi_comp.length] = \
                comp_bin[0:ifwi_comp.length]

        return 0

    @staticmethod
    def extract_component (ifwi_bin, comp_bin, path):
        bins_comp = bytearray ()

        ifwi = IFWI_PARSER.parse_ifwi_binary (ifwi_bin)
        if not ifwi:
            print ("Not a valid ifwi image!")
            return -1

        ifwi_comps = IFWI_PARSER.locate_components (ifwi, path)
        if len(ifwi_comps) == 0:
            print ("Cannot find path '%s' in ifwi image!" % path)
            return -2

        if len(ifwi_comps) > 1:
            print ("Found multiple components for '%s'!" % path)
            return -3

        ifwi_comp   = ifwi_comps[0]
        comp_bin[:] = ifwi_bin[ifwi_comp.offset:ifwi_comp.offset + ifwi_comp.length]

        return 0


    @staticmethod
    def bpdt_parser (bin_data, bpdt_offset, offset):
        sub_part_list = []
        idx = bpdt_offset + offset
        bpdt_hdr = BPDT_HEADER.from_buffer(bytearray(bin_data[idx:idx + sizeof(BPDT_HEADER)]))
        idx += sizeof(bpdt_hdr)

        sbpdt = None
        for desc in range(bpdt_hdr.desc_cnt):
            bpdt_entry = BPDT_ENTRY.from_buffer(bytearray(bin_data[idx:idx + sizeof(BPDT_ENTRY)]))
            idx += sizeof(bpdt_entry)
            dir_list = []
            if 'BpdtSbpdt' == str(bpdt_entry.type):
                sbpdt = bpdt_entry

            if bpdt_entry.sub_part_size > sizeof(SUBPART_DIR_HEADER):
                part_idx = bpdt_offset + bpdt_entry.sub_part_offset
                if  part_idx > len(bin_data):
                    break
                sub_part_dir_hdr = SUBPART_DIR_HEADER.from_buffer(
                    bytearray(bin_data[part_idx:part_idx + sizeof(
                        SUBPART_DIR_HEADER)]), 0)
                part_idx += sizeof(sub_part_dir_hdr)
                if b'$CPD' == sub_part_dir_hdr.header_marker:
                    for dir in range(sub_part_dir_hdr.num_of_entries):
                        part_dir = SUBPART_DIR_ENTRY.from_buffer(
                            bytearray(bin_data[part_idx:part_idx + sizeof(
                                SUBPART_DIR_ENTRY)]), 0)
                        part_idx += sizeof(part_dir)
                        dir_list.append(part_dir)

            sub_part_list.append((bpdt_entry, dir_list))

        return sub_part_list, sbpdt

    @staticmethod
    def parse_bios_bpdt (img_data):
        offset = 0
        bios_hdr = BIOS_ENTRY.from_buffer(img_data, offset)
        if bios_hdr.name != b"BIOS":
            return None

        bios_comp = COMPONENT(bios_hdr.name.decode(), COMPONENT.COMP_TYPE['RGN'], 0, len(img_data))
        offset += sizeof(bios_hdr)
        entry_num = bios_hdr.offset
        for idx in range(entry_num):
            part_entry = BIOS_ENTRY.from_buffer(img_data, offset)
            part_comp = COMPONENT(part_entry.name.decode(), COMPONENT.COMP_TYPE['PART'],
                                 part_entry.offset, part_entry.length)
            bios_comp.add_child(part_comp)
            sub_part_dir_hdr = SUBPART_DIR_HEADER.from_buffer(img_data,
                                                           part_entry.offset)
            if b'$CPD' == sub_part_dir_hdr.header_marker:
                for dir in range(sub_part_dir_hdr.num_of_entries):
                    part_dir = SUBPART_DIR_ENTRY.from_buffer(
                        img_data, part_entry.offset + sizeof(SUBPART_DIR_HEADER) +
                        sizeof(SUBPART_DIR_ENTRY) * dir)
                    dir_comp = COMPONENT(part_dir.entry_name.decode(), COMPONENT.COMP_TYPE['FILE'],
                                        part_entry.offset + part_dir.entry_offset,
                                        part_dir.entry_size)
                    part_comp.add_child(dir_comp)
            offset += sizeof(part_entry)

        return bios_comp

    @staticmethod
    def parse_bios_region (img_data, base_off = 0):
        rgn_size = len(img_data)
        offset = bytes_to_value(img_data[-8:-4]) - (0x100000000 - rgn_size)
        if offset < 0 or offset >= rgn_size - sizeof(FLASH_MAP_DESC):
            return None

        fla_map_off = offset
        mst_desc    = FLASH_MAP_DESC.from_buffer (img_data, fla_map_off)
        if mst_desc.sig != FLASH_MAP.FLASH_MAP_SIGNATURE:
            return None

        sbl_size   = mst_desc.size
        sbl_off    = rgn_size - sbl_size
        if sbl_off < 0:
            return None

        bios_comp  = COMPONENT('BIOS', COMPONENT.COMP_TYPE['RGN'], base_off, rgn_size)
        curr_part  = -1
        fla_map_str = FLASH_MAP.from_buffer (img_data, fla_map_off)
        entry_num  = (fla_map_str.length - sizeof(FLASH_MAP)) // sizeof(FLASH_MAP_DESC)
        for idx in range (entry_num):
            idx   = entry_num - 1 - idx
            desc  = FLASH_MAP_DESC.from_buffer (img_data, fla_map_off + sizeof(FLASH_MAP) + idx * sizeof(FLASH_MAP_DESC))
            file_comp = COMPONENT(desc.sig.decode(), COMPONENT.COMP_TYPE['FILE'], desc.offset + sbl_off + base_off, desc.size)
            if curr_part != desc.flags & 0x4F:
                curr_part = desc.flags & 0x4F
                part_comp = COMPONENT('%s' % (FLASH_MAP.FLASH_MAP_REGION[curr_part]), COMPONENT.COMP_TYPE['PART'], desc.offset + base_off, desc.size)
                bios_comp.add_child (part_comp)
            else:
                part_comp.length += desc.size

            part_comp.add_child(file_comp)

        return bios_comp

    @staticmethod
    def parse_ifwi_binary(img_data):
        if len(img_data) < 0x1000:
            return None

        ifwi_comp = COMPONENT('IFWI', COMPONENT.COMP_TYPE['IFWI'], 0, len(img_data))
        bios_comp = IFWI_PARSER.parse_bios_bpdt (img_data)
        if bios_comp is not None:
            ifwi_comp.add_child (bios_comp)
            return ifwi_comp

        spi_descriptor = SPI_DESCRIPTOR.from_buffer(img_data)
        if spi_descriptor.fl_val_sig != spi_descriptor.DESC_SIGNATURE:
            # no SPI descriptor, try to check the flash map
            bios_comp = IFWI_PARSER.parse_bios_region (img_data, 0)
            if bios_comp is not None:
                ifwi_comp.add_child (bios_comp)
            return ifwi_comp

        # It is a full IFWI image
        bios_comp = None
        ifwi_comp = COMPONENT('IFWI', COMPONENT.COMP_TYPE['IFWI'], 0, len(img_data))
        rgn_dict  = sorted(SPI_DESCRIPTOR.FLASH_REGIONS, key=SPI_DESCRIPTOR.FLASH_REGIONS.get)
        for rgn in rgn_dict:
            rgn_start, rgn_limit = IFWI_PARSER.find_ifwi_region(spi_descriptor, rgn)
            if rgn_start is None:
                continue
            rgn_comp = COMPONENT(rgn.upper(), COMPONENT.COMP_TYPE['RGN'], rgn_start, rgn_limit - rgn_start + 1)
            if rgn == 'bios':
                bios_comp = rgn_comp
            else:
                ifwi_comp.add_child (rgn_comp)
        if bios_comp is None:
            return None

        bios_start = bios_comp.offset
        bios_limit = bios_comp.offset + bios_comp.length - 1
        if not (img_data[bios_start] == 0xAA and img_data[bios_start + 1] == 0x55):
            # normal layout
            new_bios_comp = IFWI_PARSER.parse_bios_region (img_data[bios_start:bios_limit+1], bios_start)
            if new_bios_comp is not None:
                bios_comp = new_bios_comp
            ifwi_comp.add_child (bios_comp)
            ifwi_comp.child.sort (key=lambda x: x.offset)
            return ifwi_comp

        # Sort region by offset
        ifwi_comp.add_child (bios_comp)
        ifwi_comp.child.sort (key=lambda x: x.offset)

        # It is BPDT format
        bp_offset = [bios_start, (bios_start + bios_limit + 1) // 2]
        for idx, offset in enumerate(bp_offset):
            bp_comp = COMPONENT('BP%d' % idx, COMPONENT.COMP_TYPE['BP'], offset,
                               (bios_limit - bios_start + 1) // 2)
            sub_part_offset = 0
            while True:
                bpdt, sbpdt_entry = IFWI_PARSER.bpdt_parser(img_data, offset, sub_part_offset)
                bpdt_prefix = '' if sub_part_offset == 0 else 'S'
                bpdt_size = sbpdt_entry.sub_part_offset if sbpdt_entry else bpdt_comp.child[-1].length
                bpdt_comp = COMPONENT('%sBPDT' % bpdt_prefix, COMPONENT.COMP_TYPE['BPDT'],
                                     offset + sub_part_offset, bpdt_size)
                sorted_bpdt = sorted(bpdt, key=lambda x: x[0].sub_part_offset)
                for part, dir_list in sorted_bpdt:
                    if not part.sub_part_size:
                        continue
                    part_comp = COMPONENT(
                        str(part.type), COMPONENT.COMP_TYPE['PART'],
                        offset + part.sub_part_offset, part.sub_part_size)
                    sorted_dir = sorted(dir_list, key=lambda x: x.entry_offset)
                    for dir in sorted_dir:
                        file_comp = COMPONENT(dir.entry_name.decode(), COMPONENT.COMP_TYPE['FILE'],
                                             part_comp.offset + dir.entry_offset,
                                             dir.entry_size)
                        part_comp.add_child(file_comp)
                    bpdt_comp.add_child(part_comp)
                bp_comp.add_child(bpdt_comp)
                if sbpdt_entry:
                    sub_part_offset = sbpdt_entry.sub_part_offset
                else:
                    break
            bios_comp.add_child(bp_comp)
        return ifwi_comp


if __name__ == '__main__':
    parser     = argparse.ArgumentParser()
    subparsers = parser.add_subparsers(title='commands')

    parser_view     = subparsers.add_parser('view',  help='print IFWI component layout')
    parser_view.set_defaults(which='view')
    parser_view.add_argument('-i', '--input-image', dest='ifwi_image', type=str,
                    required=True, help='Specify input IFWI image file path')


    parser_replace  = subparsers.add_parser('replace',  help='replace component in IFWI')
    parser_replace.set_defaults(which='replace')
    parser_replace.add_argument('-f', '--component-image', dest='comp_image', type=str,
                    default = '', help="Specify component image file")
    parser_replace.add_argument('-i', '--input-image', dest='ifwi_image', type=str,
                    required=True, help='Specify input IFWI image file path')
    parser_replace.add_argument('-o', '--output-image', dest='output_image', type=str,
                    default = '',  help='Specify output IFWI image file path')
    parser_replace.add_argument('-p', '--path', dest='component_path', type=str,
                    default = '',  help='Specify replace path in IFWI image flashmap')
    parser_replace.add_argument('-u', '--input-ucode-dir', dest='input_ucode_dir', type=str,
                    default = '',  help="Specify a directory containing all microcode to pack if the '-p' path is a microcode component")

    parser_extract  = subparsers.add_parser('extract',  help='extract component from IFWI')
    parser_extract.set_defaults(which='extract')
    parser_extract.add_argument('-i', '--input-image', dest='ifwi_image', type=str,
                    required=True, help='Specify input IFWI image file path')
    parser_extract.add_argument('-o', '--output-component', dest='output_image', type=str,
                    default = '',  help='Specify output component image file path')
    parser_extract.add_argument('-p', '--path', dest='component_path', type=str,
                    default = '',  help='Specify component path to be extracted from IFWI image')
    parser_extract.add_argument('-u', '--output-ucode-dir', dest='output_ucode_dir', type=str,
                    default = '',  help="Specify a directory to store the extraced microcode binaries if the '-p' path is a microcode component")

    args = parser.parse_args()

    ifwi = None
    ifwi_bin = bytearray (get_file_data (args.ifwi_image))

    ret  = -1
    show = False
    if args.which == 'view':
        show = True

    elif args.which == 'extract':
        comp_bin = bytearray ()
        if not args.component_path:
            show = True
        else:
            ret = IFWI_PARSER.extract_component (ifwi_bin, comp_bin, args.component_path)
            if ret == 0:
                out_image = args.output_image
                if out_image:
                    gen_file_from_object (out_image, comp_bin)
                    print ("Components @ %s was extracted successfully!" % args.component_path)
                parts = args.component_path.split('/')
                if len(parts) > 0 and parts[-1] == 'UCOD' and args.output_ucode_dir:
                    out_dir = args.output_ucode_dir
                    if not os.path.exists(out_dir):
                        os.mkdir (out_dir)
                    else:
                        if not os.path.isdir (out_dir):
                            parser.error('-u needs to be a directory !')
                    ucode = UCODE_PARSER ()
                    ucode.dump (comp_bin)
                    ucode.extract (comp_bin, out_dir)

    elif args.which == 'replace':
        if args.comp_image and args.input_ucode_dir:
            parser_replace.error("Option '-f' and '-u' are exclusive !")

        if not args.component_path:
            show = True
        else:
            if args.input_ucode_dir:
                parts = args.component_path.split('/')
                if len(parts) > 0 and parts[-1] == 'UCOD':
                    comp_bin = UCODE_PARSER.pack (args.input_ucode_dir)
                else:
                    parser_replace.error("Option '-p' needs to be a microcode component path !")
            else:
                if not args.comp_image:
                    parser_replace.error('Component image file is required when path is specified!')
                comp_bin = bytearray (get_file_data (args.comp_image))

            ret = IFWI_PARSER.replace_component (ifwi_bin, comp_bin, args.component_path)
            if ret == 0:
                if args.input_ucode_dir:
                    ret = IFWI_PARSER.update_ucode_fit_entry (ifwi_bin, args.component_path)
                if ret == 0:
                    out_image = args.output_image if args.output_image else args.ifwi_image
                    gen_file_from_object (out_image, ifwi_bin)
                    print ("Components @ %s was replaced successfully!" % args.component_path)

    if show:
        ifwi = IFWI_PARSER.parse_ifwi_binary (ifwi_bin)
        if ifwi:
            IFWI_PARSER.print_tree (ifwi)
            ret = 0

    if ret != 0:
        raise Exception ('Execution failed for %s !' % sys.argv[0])

    sys.exit(ret)
