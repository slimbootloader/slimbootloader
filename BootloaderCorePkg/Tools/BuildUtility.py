#!/usr/bin/env python
## @ BuildUtility.py
# Build bootloader main script
#
# Copyright (c) 2016 - 2022, Intel Corporation. All rights reserved.<BR>
# SPDX-License-Identifier: BSD-2-Clause-Patent
#
##

##
# Import Modules
#
import os
import sys
import re
import glob
import struct
import shutil
import hashlib
import subprocess
import datetime
import zipfile
import ntpath
from   CommonUtility import *
from   IfwiUtility   import FLASH_MAP, FLASH_MAP_DESC, FIT_ENTRY, UCODE_HEADER
from SingleSign import MESSAGE_SBL_KEY_DIR
from distutils.version import LooseVersion

sys.dont_write_bytecode = True
sys.path.append (os.path.join(os.path.dirname(__file__), '..', '..', 'IntelFsp2Pkg', 'Tools'))
from   SplitFspBin  import RebaseFspBin, FirmwareDevice, EFI_SECTION_TYPE, FSP_INFORMATION_HEADER, PeTeImage
from   GenContainer import gen_container_bin

# Mimimum Toolchain Requirement
build_toolchains = {
    'python'    : '3.6.0',
    'nasm'      : '2.12.02',
    'iasl'      : '20190509',
    'openssl'   : '1.1.0g',
    'git'       : '2.20.0',
    'vs'        : '2015',
    'gcc'       : '7.3',
    'clang'     : '9.0.0'
}

AUTO_GEN_DSC_HDR = """#
#  DO NOT EDIT
#  FILE auto-generated
#  Module name:
#    Platform.dsc
#  Abstract:       Auto-generated Platform.dsc to be included in primary DSC.
#
"""

gtools = {
    'FV_PATCH'   : 'BootloaderCorePkg/Tools/PatchFv.py',
    'GEN_CFG'    : 'BootloaderCorePkg/Tools/GenCfgData.py',
    'FSP_SPLIT'  : 'IntelFsp2Pkg/Tools/SplitFspBin.py',
    'IMG_REPORT' : 'BootloaderCorePkg/Tools/GenReport.py',
    'CFG_DATA'   : 'BootloaderCorePkg/Tools/CfgDataTool.py'
}

class STITCH_OPS:
    MODE_FILE_NOP   = 0x00
    MODE_FILE_ALIGN = 0x01
    MODE_FILE_PAD   = 0x02
    MODE_FILE_IGNOR = 0x80
    MODE_POS_TAIL   = 0
    MODE_POS_HEAD   = 1


class FLASH_REGION_TYPE:
    DESCRIPTOR   = 0x0
    BIOS         = 0x1
    ME           = 0x2
    GBE          = 0x3
    PLATFORMDATA = 0x4
    DER          = 0x5
    ALL          = 0x6
    MAX          = 0x7

IPP_CRYPTO_OPTIMIZATION_MASK = {
    # Opt Type        : Mask
    "SHA256_V8"       : 0x0001,
    "SHA256_NI"       : 0x0002,
    "SHA384_W7"       : 0x0004,
    "SHA384_G9"       : 0x0008,
    }

IPP_CRYPTO_ALG_MASK = {
    # Hash_type      : Mask
    "SHA1"           : 0x0001,
    "SHA2_256"       : 0x0002,
    "SHA2_384"       : 0x0004,
    "SHA2_512"       : 0x0008,
    "SM3_256"        : 0x0010
    }

HASH_USAGE = {
    'STAGE_1B'            : (1<<0),
    'STAGE_2'             : (1<<1),
    'PAYLOAD'             : (1<<2),
    'PAYLOAD_FWU'         : (1<<3),
    'PUBKEY_MASTER'       : (1<<8),
    'PUBKEY_CFG_DATA'     : (1<<9),
    'PUBKEY_FWU'          : (1<<10),
    'PUBKEY_OS'           : (1<<11),
    'PUBKEY_CONT_DEF'     : (1<<12),
    'PUBKEY_OEM_0'        : (1<<24),
    'PUBKEY_OEM_1'        : (1<<25),
    'PUBKEY_OEM_2'        : (1<<26),
    'PUBKEY_OEM_3'        : (1<<27),
    'PUBKEY_OEM_4'        : (1<<28),
    'PUBKEY_OEM_5'        : (1<<29),
    'PUBKEY_OEM_6'        : (1<<30),
    'PUBKEY_OEM_7'        : (1<<31),
}

class HashStoreData(Structure):

    _pack_ = 1
    _fields_ = [
        ('Usage',           c_uint32),
        ('HashAlg',         c_uint8),
        ('Reserved',        c_uint8),
        ('DigestLen',       c_uint16),
        ('Digest',          ARRAY(c_uint8, 0)),
        ]

class HashStoreTable(Structure):

    HASH_STORE_SIGNATURE    = b'_HS_'
    HASH_STORE_MAX_IDX_NUM  = 5         #STAGE1B.hash, STAGE2.hash, PAYLOAD.hash, FWUPDATE.hash, MSTKEY.hash

    _pack_ = 1
    _fields_ = [
        ('Signature',         ARRAY(c_char, 4)),
        ('Revision',          c_uint8),
        ('HeaderLength',      c_uint8),
        ('Reserved',          ARRAY(c_uint8, 2)),
        ('UsedLength',        c_uint32),
        ('TotalLength',       c_uint32),
        ('Data',              ARRAY(c_uint8, 0)),
        ]

    def __init__(self):
        self.Signature = HashStoreTable.HASH_STORE_SIGNATURE
        self.Revision  = 1
        self.HeaderLength  = sizeof(HashStoreTable)


class ImageVer(Structure):
    _pack_ = 1
    _fields_ = [
        ('BuildNumber',       c_uint16),
        ('ProjMinorVersion',  c_uint8),
        ('ProjMajorVersion',  c_uint8),
        ('CoreMinorVersion',  c_uint8),
        ('CoreMajorVersion',  c_uint8),
        ('SecureVerNum',      c_uint8),
        ('Reserved',          c_uint8, 4),
        ('ImageArch',         c_uint8, 1),
        ('BldDebug',          c_uint8, 1),
        ('FspDebug',          c_uint8, 1),
        ('Dirty',             c_uint8, 1),
        ]


class VerInfo(Structure):
    _pack_ = 1
    _fields_ = [
        ('Signature',         ARRAY(c_char, 4)),
        ('HeaderLength',      c_uint16),
        ('HeaderRevision',    c_uint8),
        ('Reserved',          c_uint8),
        ('ImageId',           c_uint64),
        ('ImageVersion',      ImageVer),
        ('SourceVersion',     c_uint64),
        ]


class VariableRegionHeader(Structure):
    _pack_ = 1
    _fields_ = [
        ('Signature',        ARRAY(c_char, 4)),
        ('Size',             c_uint32),
        ('Format',           c_uint8),
        ('State',            c_uint8),
        ('Reserved',         ARRAY(c_char, 6))
        ]


class PciEnumPolicyInfo(Structure):
    _pack_ = 1
    _fields_ = [
        ('DowngradeIo32',           c_uint16, 1),
        ('DowngradeMem64',          c_uint16, 1),
        ('DowngradePMem64',         c_uint16, 1),
        # 0: Do not downgrade PCI devices on bus 0
        # 1: Downgrade all PCI devices on bus 0
        # 2: Downgrade all PCI devices on bus 0 but GFX
        # 3: Reserved
        ('DowngradeBus0',           c_uint16, 2),
        ('DowngradeReserved',       c_uint16, 11),
        ('FlagAllocPmemFirst',      c_uint16, 1),
        ('FlagAllocRomBar',         c_uint16, 1),
        ('FlagReserved',            c_uint16, 14),
        ('BusScanType',             c_uint8), # 0: list, 1: range
        ('NumOfBus',                c_uint8),
        ('BusScanItems',            ARRAY(c_uint8, 0))
    ]

    def __init__(self):
        self.DowngradeIo32      = 1
        self.DowngradeMem64     = 1
        self.DowngradePMem64    = 1
        self.DowngradeBus0      = 1
        self.DowngradeReserved  = 0
        self.FlagAllocPmemFirst = 0
        self.FlagAllocRomBar    = 0
        self.FlagReserved       = 0
        self.Reserved           = 0
        self.BusScanType        = 0
        self.NumOfBus           = 0

def is_valid_tool_version(cmd, current_version, optional=False):
    try:
        name_str = ntpath.basename(cmd).split('.')[0]
        name = re.sub(r'\d+', '', name_str)
        minimum_version = build_toolchains[name]
        valid = LooseVersion(current_version) >= LooseVersion(minimum_version)
    except:
        print('Unexpected exception while checking %s tool version' % cmd)
        return False

    try:
        if os.name == 'posix':
            cmd = subprocess.check_output(['which', cmd], stderr=subprocess.STDOUT).decode().strip()
    except:
        pass

    print ('- %s: Version %s (>= %s) [%s]' % (cmd, current_version, minimum_version, \
           'PASS' if valid else 'RECOMMEND' if optional else 'FAIL'))
    return valid | optional

def get_gcc_info ():
    toolchain = 'GCC5'
    cmd = 'gcc'
    try:
        prefix = os.environ.get(toolchain + '_BIN')
        ver = subprocess.check_output([(prefix if prefix else '') + cmd, '-dumpfullversion']).decode().strip()
    except:
        ver = ''
        pass
    valid = is_valid_tool_version(cmd, ver)
    return (toolchain if valid else None, None, None, ver)

def get_clang_info ():
    if os.name == 'posix':
        toolchain_path   = ''
    else:
        # On windows, still need visual studio to provide nmake build utility
        toolchain, toolchain_prefix, toolchain_path, toolchain_ver = get_visual_studio_info ()
        os.environ['CLANG_HOST_BIN'] =  os.path.join(toolchain_path, "bin\\Hostx64\\x64\\n")
        toolchain_path   = 'C:\\Program Files\\LLVM\\bin\\'
    toolchain        = 'CLANGPDB'
    toolchain_prefix = 'CLANG_BIN'
    cmd = os.path.join(toolchain_path, 'clang')
    try:
        ver_str = subprocess.check_output([cmd, '--version']).decode().strip()
        ver = re.search(r'version\s*([\d.]+)', ver_str).group(1)
    except:
        ver = ''
        pass
    valid = is_valid_tool_version(cmd, ver)
    return (toolchain if valid else None, toolchain_prefix, toolchain_path, ver)

def get_visual_studio_info (preference = ''):

    toolchain        = ''
    toolchain_prefix = ''
    toolchain_path   = ''
    toolchain_ver    = ''
    vs_ver_list      = ['2019', '2017']
    vs_ver_list_old  = ['2015', '2013']

    if preference:
        preference = preference.strip().lower()
        if not preference.startswith('vs'):
            print("Invalid vistual studio toolchain type '%s' !" % preference)
            return (None,None,None,None)
        vs_str = preference[2:]
        if vs_str in vs_ver_list:
            vs_ver_list     = [vs_str]
            vs_ver_list_old = []
        elif vs_str in vs_ver_list_old:
            vs_ver_list     = []
            vs_ver_list_old = [vs_str]
        else:
            print("Unsupported toolchain version '%s' !" % preference)
            return (None,None,None,None)

    # check new Visual Studio Community version first
    vswhere_path = "%s/Microsoft Visual Studio/Installer/vswhere.exe" % os.environ['ProgramFiles(x86)']
    if os.path.exists (vswhere_path):
        cmd = [vswhere_path, '-all', '-property', 'installationPath']
        lines = run_process (cmd, capture_out = True)
        vscommon_paths = []
        for each in lines.splitlines ():
            each = each.strip()
            if each and os.path.isdir(each):
                vscommon_paths.append(each)

        for vs_ver in vs_ver_list:
            for vscommon_path in vscommon_paths:
                vcver_file = vscommon_path + '\\VC\\Auxiliary\\Build\\Microsoft.VCToolsVersion.default.txt'
                if os.path.exists(vcver_file):
                    check_path = '\\Microsoft Visual Studio\\%s\\' % vs_ver
                    if check_path in vscommon_path:
                        toolchain_ver    = get_file_data (vcver_file, 'r').strip()
                        toolchain_prefix = 'VS%s_PREFIX' % (vs_ver)
                        toolchain_path   = vscommon_path + '\\VC\\Tools\\MSVC\\%s\\' % toolchain_ver
                        toolchain = 'VS%s' % (vs_ver)
                        break
            if toolchain:
                break

    if toolchain == '':
        vs_ver_dict = {
            '2015': 'VS140COMNTOOLS',
            '2013': 'VS120COMNTOOLS'
        }
        for vs_ver in vs_ver_list_old:
            vs_tool = vs_ver_dict[vs_ver]
            if vs_tool in os.environ:
                toolchain        ='VS%s%s' % (vs_ver, 'x86')
                toolchain_prefix = 'VS%s_PREFIX' % (vs_ver)
                toolchain_path   = os.path.join(os.environ[vs_tool], '..//..//')
                toolchain_ver    = vs_ver
                parts   = os.environ[vs_tool].split('\\')
                vs_node = 'Microsoft Visual Studio '
                for part in parts:
                    if part.startswith(vs_node):
                        toolchain_ver = part[len(vs_node):]
                break

    valid = is_valid_tool_version('vs', vs_ver)
    return (toolchain if valid else None, toolchain_prefix, toolchain_path, toolchain_ver)

def split_fsp(path, out_dir):
    run_process ([
                sys.executable,
                gtools['FSP_SPLIT'],
                "split",
                "-f", path,
                "-n", "FSP.bin",
                "-o", out_dir])


def rebase_fsp(path, out_dir, base_t, base_m, base_s):
    run_process ([
        sys.executable,
        gtools['FSP_SPLIT'],
        "rebase",
        "-f", path,
        "-b", "0x%x" % base_t, "0x%x" % base_m, "0x%x" % base_s,
        "-c", "t" , "m", "s",
        "-n", "Fsp.bin",
        "-o", out_dir])


def patch_fv(fv_dir, fvs, *vargs):
    sys.stdout.flush()
    args = [x for x in list(vargs) if x != '']
    run_process ([sys.executable, gtools['FV_PATCH'], fv_dir, fvs] + args, False)


def gen_cfg_data (command, dscfile, outfile):
    run_process ([
            sys.executable,
            gtools['GEN_CFG'],
            command,
            dscfile,
            outfile])


def cfg_data_tool (command, infiles, outfile, extra = []):
    arg_list = [sys.executable, gtools['CFG_DATA'], command, '-o', outfile]
    arg_list.extend (extra)
    arg_list.extend (infiles)
    run_process (arg_list)


def report_image_layout (fv_dir, stitch_file, report_file):
    sys.stdout.flush()
    rpt_file = open(os.path.join(fv_dir, report_file), "w")
    x = subprocess.call([sys.executable, gtools['IMG_REPORT'], fv_dir, stitch_file, ""], stdout=rpt_file)
    rpt_file.close()
    if x: sys.exit(1)


def get_fsp_size (path):
    di = open(path,'rb').read()[0x20:0x24]
    return struct.unpack('I', di)[0]


def get_fsp_upd_size (path):
    di = open(path,'rb').read()[0xBC:0xC0]
    return ((struct.unpack('I', di)[0] + 0x10) & 0xFFFFFFF0)

def get_fsp_upd_signature (path):
    bins = open(path,'rb').read()
    off  = bytes_to_value (bins[0xB8:0xBC])
    return bins[off:off+8]

def get_fsp_revision (path):
    di = open(path,'rb').read()[0xA0:0xA4]
    return struct.unpack('I', di)[0]


def get_fsp_image_id (path):
    di = open(path,'rb').read()[0xA4:0xAC]
    return struct.unpack('8s', di[:8])[0].rstrip(b'\x00').decode()


def get_redundant_info (comp_name):
    comp_base = os.path.splitext(os.path.basename(comp_name))[0].upper()
    match = re.match('(\w+)_([AB])$', comp_base)
    if match:
        comp_name = match.group(1)
        part_name = match.group(2)
    else:
        comp_name = comp_base
        part_name = ''
    return comp_name, part_name


def get_payload_list (payloads):
    pld_tmp  = dict()
    pld_lst  = []
    pld_num  = len(payloads)

    for idx, pld in enumerate(payloads):
        items    = pld.split(':')
        item_cnt = len(items)
        pld_tmp['file'] = items[0]

        if item_cnt > 1 and items[1].strip():
            pld_tmp['name'] = ("%-4s" % items[1])[:4]
        else:
            pld_tmp['name'] = 'PLD%d' % idx if pld_num > 1 else ''

        if item_cnt > 2 and items[2].strip():
            pld_tmp['algo'] = items[2]
        else:
            pld_tmp['algo'] = 'Lz4'

        pld_lst.append(dict(pld_tmp))

    return pld_lst

def gen_pub_key_hash_store (signing_key, pub_key_hash_list, hash_alg, sign_scheme, svn, pub_key_dir, out_file):
    # Build key hash blob
    key_hash_buf = bytearray ()
    idx = 0
    for usage, key_file in pub_key_hash_list:
        pub_key_file = os.path.dirname(out_file) + '/PUBKEY%02d.bin' % idx
        gen_pub_key (os.path.join(pub_key_dir, key_file), pub_key_file)
        key_hash_alg = adjust_hash_type (pub_key_file)
        hash_data = gen_hash_file (pub_key_file, key_hash_alg, None, True)
        key_hash_entry = HashStoreData()
        key_hash_entry.Usage     = usage
        key_hash_entry.HashAlg   = HASH_TYPE_VALUE[key_hash_alg]
        key_hash_entry.DigestLen = len(hash_data)
        key_hash_buf.extend (bytearray(key_hash_entry) + hash_data)
        idx += 1

    key_store_bin_file = out_file + '.raw'
    gen_file_from_object (key_store_bin_file, key_hash_buf)

    key_store_cnt_file = os.path.basename(out_file)
    key_store_bin_file = os.path.basename(key_store_bin_file)

    key_type = get_key_type(signing_key)
    sign_scheme = sign_scheme[sign_scheme.index("_")+1:]
    auth_type   = key_type + '_' + sign_scheme +  '_' + hash_alg
    hash_store  = [('KEYH', key_store_cnt_file, '', auth_type, signing_key, 0x10, 0, svn)]
    hash_store.append ((HashStoreTable.HASH_STORE_SIGNATURE.decode(), key_store_bin_file, '', hash_alg, '', 0x10, 0, svn))
    out_dir = os.path.dirname(out_file)
    gen_container_bin ([hash_store], out_dir, out_dir, '', '')


def gen_ias_file (rel_file_path, file_space, out_file):
    bins = bytearray()
    file_path = os.path.join(os.environ['PLT_SOURCE'], rel_file_path)
    if os.path.exists(file_path):
        ias_fh   = open (file_path, 'rb')
        file_bin = ias_fh.read()
        ias_fh.close ()
    else:
        file_bin = bytearray ()
    file_size = len(file_bin)
    if file_size > file_space:
        raise Exception ("Insufficient region size 0x%X for file '%s', requires size 0x%X!" % (file_space, os.path.basename(file_path), file_size))
    bins.extend (file_bin + b'\xff' * (file_space - file_size))
    open (out_file, 'wb').write (bins)


def gen_flash_map_bin (flash_map_file, comp_list):
    flash_map = FLASH_MAP()
    for comp in reversed(comp_list):
        desc  = FLASH_MAP_DESC ()
        if comp['bname'] not in FLASH_MAP.FLASH_MAP_COMPONENT_SIGNATURE:
            if len(comp['bname']) < 4:
                # For short names, prefix with '_'
                bname = '_' * (4 - len(comp['bname'])) + comp['bname']
            else:
                # For long names, use the 1st 4 chars
                bname = comp['bname'][:4]
            desc.sig    = bname.encode()
        else:
            desc.sig    = FLASH_MAP.FLASH_MAP_COMPONENT_SIGNATURE[comp['bname']].encode()
        desc.flags  = comp['flag']
        desc.offset = comp['offset']
        desc.size   = comp['size']
        flash_map.add (desc)
    flash_map.finalize ()

    fd = open (flash_map_file, 'wb')
    fd.write(flash_map)
    for desc in flash_map.descriptors:
        fd.write(desc)
    fd.close()

def copy_expanded_file (src, dst):
    gen_cfg_data ("GENDLT", src, dst)

def gen_config_file (fv_dir, brd_name_override, brd_name, platform_id, pri_key, cfg_db_size, cfg_size, cfg_int, cfg_ext, sign_scheme, hash_type, svn, brd_build_name):
    # Remove previous generated files
    for file in glob.glob(os.path.join(fv_dir, "CfgData*.*")):
            os.remove(file)

    CfgIntLen = len(cfg_int)

    file_ext = 'yaml'
    gen_cmd  = { 'yaml':'GENYML', 'dsc':'GENDSC' }

    # Generate CFG data
    brd_name_dir      = os.path.join(os.environ['PLT_SOURCE'], 'Platform', brd_name)
    if not os.path.exists(brd_name_dir):
        brd_name_dir      = os.path.join(os.environ['SBL_SOURCE'], 'Platform', brd_name)
    comm_brd_dir      = os.path.join(os.environ['SBL_SOURCE'], 'Platform', 'CommonBoardPkg')
    brd_cfg_dir       = os.path.join(brd_name_dir, 'CfgData')
    com_brd_cfg_dir   = os.path.join(comm_brd_dir, 'CfgData')
    cfg_hdr_file      = os.path.join(brd_name_dir, 'Include', 'ConfigDataStruct.h')
    cfg_com_hdr_file  = os.path.join(comm_brd_dir, 'Include', 'ConfigDataCommonStruct.h')
    cfg_inc_file      = os.path.join(brd_name_dir, 'Include', 'ConfigDataBlob.h')
    cfg_dsc_file      = os.path.join(brd_cfg_dir, 'CfgDataDef.' + file_ext)
    cfg_hdr_dyn_file  = os.path.join(brd_name_dir, 'Include', 'ConfigDataDynamic.h')
    cfg_dsc_dyn_file  = os.path.join(brd_cfg_dir, 'CfgDataDynamic.' + file_ext)
    cfg_pkl_file      = os.path.join(fv_dir, "CfgDataDef.pkl")
    cfg_bin_file      = os.path.join(fv_dir, "CfgDataDef.bin")  #default core dsc file cfg data
    cfg_bin_int_file  = os.path.join(fv_dir, "CfgDataInt.bin")  #_INT_CFG_DATA_FILE settings
    cfg_bin_ext_file  = os.path.join(fv_dir, "CfgDataExt.bin")  #_EXT_CFG_DATA_FILE settings
    cfg_comb_dsc_file = os.path.join(fv_dir, 'CfgDataDef.' + file_ext)
    if brd_name_override != '':
        brd_cfg2_dir  = os.path.join(os.environ['PLT_SOURCE'], 'Platform', brd_name_override, 'CfgData')

    # Generate parsed result into pickle file to improve performance
    if os.path.exists(cfg_dsc_dyn_file):
            gen_cfg_data ("GENHDR", cfg_dsc_dyn_file, cfg_hdr_dyn_file)

    gen_cfg_data ("GENPKL", cfg_dsc_file, cfg_pkl_file)
    gen_cfg_data (gen_cmd[file_ext], cfg_dsc_file, cfg_comb_dsc_file)
    gen_cfg_data ("GENHDR", cfg_pkl_file, ';'.join([cfg_hdr_file, cfg_com_hdr_file]))
    gen_cfg_data ("GENBIN", cfg_pkl_file, cfg_bin_file)

    # Update cfg_hdr_file to add a new #define for platform name
    if brd_build_name != "":
        with open(cfg_hdr_file, "r") as in_file:
            lines = in_file.readlines()

        with open(cfg_hdr_file, "w") as out_file:
            line_added = False
            for line in lines:
                out_file.write (line)
                if not line_added:
                    match = re.match('^#define ', line)
                    if match:
                        out_file.write ("\n#define PLATFORM_%s        1" % brd_build_name.upper())
                        line_added = True

    cfg_base_file = None
    for cfg_file_list in [cfg_int, cfg_ext]:
        if cfg_file_list is cfg_int:
            cfg_merged_bin_file = cfg_bin_int_file
            cfg_file_list.insert(0, 'CfgDataDef.bin');
        else:
            cfg_merged_bin_file = cfg_bin_ext_file

        cfg_bin_list = []
        for dlt_file in cfg_file_list:
            cfg_dlt_file = ''
            if brd_name_override != '':
                cfg_dlt_file = os.path.join(brd_cfg2_dir, dlt_file)

            if cfg_dlt_file == '' or not os.path.exists(cfg_dlt_file):
                cfg_dlt_file = os.path.join(brd_cfg_dir, dlt_file)

            if not os.path.exists(cfg_dlt_file):
                test_file = os.path.join(fv_dir, dlt_file)
                if os.path.exists(test_file):
                    cfg_dlt_file = test_file
            if dlt_file.lower().endswith('.dlt'):
                bas_path = os.path.join (fv_dir, os.path.basename(cfg_dlt_file))
                bas_path = os.path.splitext(bas_path)[0]
                cfg_brd_bin_file = bas_path + '.bin'
                gen_cfg_data ("GENBIN", '%s;%s' % (cfg_pkl_file, cfg_dlt_file), cfg_brd_bin_file)
            else:
                cfg_brd_bin_file = cfg_dlt_file if os.path.exists(cfg_dlt_file) else os.path.join(fv_dir, dlt_file)
            if (cfg_file_list is cfg_int) and (cfg_base_file is None):
                cfg_base_file = cfg_bin_int_file
            cfg_bin_list.append (cfg_brd_bin_file)

        if cfg_bin_list:
            extra = []
            if cfg_file_list is cfg_ext:
                cfg_bin_list.insert(0, cfg_base_file + '*')
            else:
                if platform_id is not None:
                    extra = ['-p', '%d' % platform_id]
            cfg_data_tool ('merge', cfg_bin_list, cfg_merged_bin_file, extra)
            bin_file_size = os.path.getsize(cfg_merged_bin_file)
            cfg_db_size
            if cfg_file_list is cfg_int:
                cfg_rgn_size = cfg_db_size
                cfg_rgn_name = 'internal'
            else:
                cfg_rgn_size = cfg_size
                cfg_rgn_name = 'external'
            if bin_file_size >= cfg_rgn_size:
                raise Exception ('CFGDATA_SIZE is too small, requested 0x%X for %s CFGDATA !' % (bin_file_size, cfg_rgn_name))

    if not os.path.exists(cfg_merged_bin_file):
        cfg_merged_bin_file = cfg_bin_int_file

    cfg_final_file = os.path.join(fv_dir, "CFGDATA.bin")
    if pri_key:
        cfg_data_tool ('sign', ['-k', pri_key, '-a', hash_type, '-s', sign_scheme, '-svn', str(svn), cfg_merged_bin_file], cfg_final_file)
    else:
        shutil.copy(cfg_merged_bin_file, cfg_final_file)

    # copy delta files
    dlt_list  = cfg_int[1:] + cfg_ext
    for dlt_file in dlt_list:
        src_dlt_file = ''
        if brd_name_override != '':
            src_dlt_file = os.path.join(brd_cfg2_dir, dlt_file)
        if src_dlt_file == '' or not os.path.exists(src_dlt_file):
            src_dlt_file = os.path.join (brd_cfg_dir, dlt_file)
        if not os.path.exists(src_dlt_file):
            src_dlt_file = os.path.join (fv_dir, dlt_file)
        if not os.path.exists(src_dlt_file):
            raise Exception ('dlt file %s could not be found!' % (os.path.join (brd_cfg_dir, dlt_file)))
        copy_expanded_file (src_dlt_file, os.path.join (fv_dir, dlt_file))

    # generate CfgDataStitch script
    tool_dir    = os.path.abspath(os.path.dirname(__file__))
    src_file    = os.path.join (tool_dir, 'CfgDataStitch.py')
    dst_file    = os.path.join (fv_dir,   'CfgDataStitch.py')

    # locate pid in dlt
    dlt_id_list = []
    dlt_list    = cfg_ext
    dlt_text    = []
    for each in dlt_list:
        fd    = open (os.path.join (fv_dir, each))
        lines = fd.readlines()
        fd.close()
        pid   = None
        for line in lines:
            if line.startswith('PLATFORMID_CFG_DATA.PlatformId'):
                pid = int(line.split('|')[1].strip(), 0)
                break
        if pid is None:
            raise Exception ("Failed to identify PlatformId from file '' !" % each)
        dlt_id_list.append((pid, each))
        dlt_text.append("  (0x%02X, '%s')" % (pid, each))

    # patch pid list in CfgDataStitch script
    fd = open(src_file, 'r')
    script_txt  = fd.read()
    fd.close ()
    new_txt = 'dlt_files = [\n%s\n]\n' % (',\n'.join(dlt_text))
    replace_txt = script_txt.replace ('dlt_files = [] # TO BE PATCHED', new_txt)

    if new_txt not in replace_txt:
        raise Exception ('Failed to generate project CfgDataStitch.py script !')
    fd = open(dst_file, 'w')
    fd.write(replace_txt)
    fd.close()


def gen_payload_bin (fv_dir, arch_dir, pld_list, pld_bin, priv_key, hash_alg, sign_scheme, brd_name = None):
    fv_dir = os.path.dirname (pld_bin)
    for idx, pld in enumerate(pld_list):
        if pld['file'] in ['OsLoader.efi', 'FirmwareUpdate.efi']:
            pld_base_name = pld['file'].split('.')[0]
            src_file = "../%s/PayloadPkg/%s/%s/OUTPUT/%s.efi" % (arch_dir, pld_base_name, pld_base_name, pld_base_name)
            src_file = os.path.join(fv_dir, src_file)
        else:
            src_file = os.path.join(os.environ['PLT_SOURCE'], 'Platform', brd_name, 'Binaries', pld['file'])
            if (brd_name is None) or (not os.path.exists(src_file)):
                src_file = os.path.join("PayloadPkg", "PayloadBins", pld['file'])
                if not os.path.exists(src_file):
                        src_file = os.path.join(fv_dir, pld['file'])

        if idx == 0:
            dst_path = pld_bin
        else :
            dst_path = os.path.join(fv_dir, os.path.basename(src_file))

        if not os.path.exists(src_file):
            raise Exception ("Cannot find payload file '%s' !" % src_file)

        if src_file != dst_path:
            shutil.copy (src_file, dst_path)

    epld_bin   = 'E' + os.path.basename(pld_bin)
    ext_list   = pld_list[1:]
    if len(ext_list) == 0:
        # Create a empty EPAYLOAD.bin
        open (os.path.join(fv_dir, epld_bin), 'wb').close()
        return

    # E-payloads container format
    svn = 0x0
    alignment = 0x10
    key_dir  = os.path.dirname (priv_key)
    key_type = get_key_type(priv_key)
    sign_scheme = sign_scheme[sign_scheme.index("_")+1:]
    auth_type = key_type + '_' + sign_scheme +  '_' + hash_alg
    pld_list = [('EPLD', '%s' % epld_bin, '', auth_type, '%s' % os.path.basename(priv_key), alignment, 0, svn)]
    for pld in ext_list:
        pld_list.append ((pld['name'], pld['file'], pld['algo'], hash_alg, '', 0, 0, svn))
    gen_container_bin ([pld_list], fv_dir, fv_dir, key_dir, '')

def pub_key_valid (pubkey):
    if (len(pubkey) - sizeof(PUB_KEY_HDR)) in [0x104, 0x184]:
        return True
    else:
        return False

def gen_hash_file (src_path, hash_type, hash_path = '', is_key = False):
    if not hash_path:
        hash_path = os.path.splitext(src_path)[0] + '.hash'
    with open(src_path,'rb') as fi:
        di = bytearray(fi.read())
    if is_key:
        if pub_key_valid(di) == False:
            raise Exception ("Invalid public key binary!")
        di = di[sizeof(PUB_KEY_HDR):]
    if hash_type == 'SHA2_256':
        ho = hashlib.sha256(di)
    elif hash_type == 'SHA2_384':
        ho = hashlib.sha384(di)
    else:
        raise Exception ("Unsupported hash type provided!")
    hash = ho.digest()
    if hash_path:
        with open(hash_path,'wb') as fo:
            fo.write(hash)
    return hash


def align_pad_file (src, dst, val, mode = STITCH_OPS.MODE_FILE_ALIGN, pos = STITCH_OPS.MODE_POS_TAIL):
    fi = open(src, 'rb')
    di = fi.read()
    fi.close()
    srclen = len(di)
    if mode == STITCH_OPS.MODE_FILE_ALIGN:
        if not (((val & (val - 1)) == 0) and val != 0):
            raise Exception ("Invalid alignment %X for file '%s'!" % (val, os.path.basename(src)))
        val   -= 1
        newlen = (srclen + val) & ((~val) & 0xFFFFFFFF)
    elif mode == STITCH_OPS.MODE_FILE_PAD:
        if val < srclen:
            raise Exception ("File '%s' size 0x%X is greater than padding size 0x%X !" % \
                    (os.path.basename(src), srclen, val))
        newlen = val
    elif mode == STITCH_OPS.MODE_FILE_NOP:
        return
    else:
        raise Exception ('Unsupported align mode %d !' % mode)
    padding = b'\xff' * (newlen - srclen)
    if dst == '':
        dst = src
    fo = open(dst,'wb')
    if pos == STITCH_OPS.MODE_POS_HEAD:
        fo.write(padding)
    fo.write(di)
    if pos == STITCH_OPS.MODE_POS_TAIL:
        fo.write(padding)
    fo.close()


def gen_vbt_file (brd_pkg_name, vbt_dict, vbt_file):
    if len(vbt_dict) == 0:
        # One VBT file
        src_path = os.path.join(os.environ['PLT_SOURCE'], 'Platform', brd_pkg_name, 'VbtBin', 'Vbt.dat')
        shutil.copy (src_path, vbt_file)
        return

    # Multiple VBT files, create signature and entry number.
    vbtbin = bytearray (b'$MVB')
    vbtbin.extend(bytearray(value_to_bytes(len(vbt_dict), 1)) + b'\x00' * 3)
    for vbt in vbt_dict:
        if type(vbt) == str:
            if len(vbt) != 4:
                raise Exception ("VBT key needs to be 4 chars, got '%s' !" % vbt)
            imageid = bytearray(vbt)
        else:
            imageid = bytearray(value_to_bytes(vbt, 4))
        src_path = os.path.join(os.environ['PLT_SOURCE'], 'Platform', brd_pkg_name, 'VbtBin', vbt_dict[vbt])
        if not os.path.exists(src_path):
            raise Exception ("File '%s' not found !" % src_path)
        fp  = open(src_path, 'rb')
        bin = bytearray(fp.read())
        fp.close()
        # Write image id and length (DWORD aligned) for VBT image
        vbtbin.extend(imageid)
        padding = ((len(bin) + 3) & ~3) - len(bin)
        vbtbin.extend(bytearray(value_to_bytes(len(bin) + padding + 8, 4)))
        vbtbin.extend(bin + b'\x00' * padding)
    fp = open(vbt_file, 'wb')
    fp.write(vbtbin)
    fp.close()


def get_verinfo_via_file (ver_dict, file):
    if not os.path.exists(file):
        raise Exception ("Version TXT file '%s' does not exist!" % file)
    hfile = open(file)
    lines = hfile.readlines()
    hfile.close()

    for line in lines:
        elements = line.strip().split('=')
        if len(elements) == 2:
                ver_dict[elements[0].strip()] = elements[1].strip()
    image_id = '%-8s' % ver_dict['ImageId']
    image_id = image_id[0:8].encode()

    ver_info = VerInfo ()
    ver_info.Signature      = b'$SBH'
    ver_info.HeaderLength   = sizeof(ver_info)
    ver_info.HeaderRevision = 1
    ver_info.ImageId        = struct.unpack('Q', image_id)[0]
    try:
        ver_info.SourceVersion  = int(ver_dict['SourceVersion'], 16)
        ver_info.ImageVersion.ProjMinorVersion = int(ver_dict['ProjMinorVersion'])
        ver_info.ImageVersion.ProjMajorVersion = int(ver_dict['ProjMajorVersion'])
        ver_info.ImageVersion.CoreMinorVersion = int(ver_dict['CoreMinorVersion'])
        ver_info.ImageVersion.CoreMajorVersion = int(ver_dict['CoreMajorVersion'])
        ver_info.ImageVersion.BuildNumber  = int(ver_dict['BuildNumber'])
        ver_info.ImageVersion.SecureVerNum = int(ver_dict['SecureVerNum'])
        ver_info.ImageVersion.ImageArch    = 1 if ver_dict['BUILD_ARCH'] == 'X64' else 0;
        ver_info.ImageVersion.FspDebug     = 1 if ver_dict['FSPDEBUG_MODE'] else 0;
        ver_info.ImageVersion.BldDebug     = 0 if ver_dict['RELEASE_MODE']  else 1;
        ver_info.ImageVersion.Dirty        = int(ver_dict['Dirty'])
    except KeyError:
        raise Exception ("Invalid version TXT file format!")

    return ver_info


def get_verinfo_via_git (ver_dict, repo_dir = '.'):
    gitcmd   = 'git describe --dirty --abbrev=16 --always'
    command  = subprocess.Popen(gitcmd, shell=True, cwd=repo_dir, stdout=subprocess.PIPE)
    line     = command.stdout.readline().strip()
    commitid = 0
    dirty    = 0
    if len(line) >= 16:
        if line.endswith(b'dirty'):
            dirty = 1
            line = line[:-6]
        try:
            commitid = int(line[-16:], 16)
        except ValueError:
            commitid = 0
    imgid = '%-8s' % ver_dict['VERINFO_IMAGE_ID']
    imgid = imgid[0:8].encode()

    date_format = "%m/%d/%Y"
    base_date = datetime.datetime.strptime(ver_dict['VERINFO_BUILD_DATE'], date_format)
    delta     = datetime.datetime.now() - base_date

    ver_info = VerInfo ()
    ver_info.Signature      = b'$SBH'
    ver_info.HeaderLength   = sizeof(ver_info)
    ver_info.HeaderRevision = 1
    if os.environ.get('BUILD_NUMBER'):
        build_number = int(os.environ['BUILD_NUMBER'])
        if build_number >= 65536:
            raise Exception ('BUILD_NUMBER is too large (<65536)')
    else:
        build_number  = int(delta.total_seconds()) // 3600
    ver_info.ImageVersion.BuildNumber  = build_number
    ver_info.ImageId                   = struct.unpack('Q', imgid)[0]
    ver_info.SourceVersion             = commitid
    ver_info.ImageVersion.ProjMinorVersion = ver_dict['VERINFO_PROJ_MINOR_VER']
    ver_info.ImageVersion.ProjMajorVersion = ver_dict['VERINFO_PROJ_MAJOR_VER']
    ver_info.ImageVersion.CoreMinorVersion = ver_dict['VERINFO_CORE_MINOR_VER']
    ver_info.ImageVersion.CoreMajorVersion = ver_dict['VERINFO_CORE_MAJOR_VER']
    ver_info.ImageVersion.SecureVerNum = ver_dict['VERINFO_SVN']
    ver_info.ImageVersion.ImageArch    = 1 if ver_dict['BUILD_ARCH'] == 'X64' else 0;
    ver_info.ImageVersion.FspDebug     = 1 if ver_dict['FSPDEBUG_MODE'] else 0;
    ver_info.ImageVersion.BldDebug     = 0 if ver_dict['RELEASE_MODE']  else 1;
    ver_info.ImageVersion.Dirty        = dirty

    return ver_info


def gen_ver_info_txt (ver_file, ver_info):
    h_file = open (ver_file, 'w')
    h_file.write('#\n')
    h_file.write('# This file is automatically generated. Please do NOT modify !!!\n')
    h_file.write('#\n\n')
    h_file.write('ImageId       = %s\n'  % struct.pack('<Q', ver_info.ImageId).decode())
    h_file.write('SourceVersion = %016x\n' % ver_info.SourceVersion)
    h_file.write('SecureVerNum  = %03d\n'  % ver_info.ImageVersion.SecureVerNum)
    h_file.write('ProjMajorVersion  = %03d\n'  % ver_info.ImageVersion.ProjMajorVersion)
    h_file.write('ProjMinorVersion  = %03d\n'  % ver_info.ImageVersion.ProjMinorVersion)
    h_file.write('CoreMajorVersion  = %03d\n'  % ver_info.ImageVersion.CoreMajorVersion)
    h_file.write('CoreMinorVersion  = %03d\n'  % ver_info.ImageVersion.CoreMinorVersion)
    h_file.write('BuildNumber   = %05d\n'  % ver_info.ImageVersion.BuildNumber)
    h_file.write('Dirty         = %d\n'    % ver_info.ImageVersion.Dirty)
    h_file.close()

def check_for_python():
    '''
    Verify Python executable is at required version
    '''
    os.environ['PYTHON_COMMAND'] = sys.executable
    cmd = os.environ['PYTHON_COMMAND']
    try:
        ver = subprocess.check_output([cmd, '--version']).decode().strip().split()[-1]
    except:
        ver = ''
        pass
    return is_valid_tool_version(cmd, ver)

def check_for_openssl():
    '''
    Verify OpenSSL executable is available
    '''
    cmd = get_openssl_path ()
    try:
        ver = subprocess.check_output([cmd, 'version']).decode().strip().split()[1]
    except:
        print('ERROR: OpenSSL not available. Please set OPENSSL_PATH.')
        ver = ''
        pass
    return is_valid_tool_version(cmd, ver)

def check_for_nasm():
    '''
    Verify NASM executable is available
    '''
    if os.name == 'nt' and 'NASM_PREFIX' not in os.environ:
        os.environ['NASM_PREFIX'] = "C:\\Nasm\\"

    cmd = os.path.join(os.environ.get('NASM_PREFIX', ''), 'nasm')
    try:
        ver_str = subprocess.check_output([cmd, '-v']).decode().strip()
        ver = re.search(r'version\s*([\d.]+)', ver_str).group(1)
    except:
        print('ERROR: NASM not available. Please set NASM_PREFIX.')
        ver = ''
        pass
    return is_valid_tool_version(cmd, ver)

def check_for_iasl():
    '''
    Verify iasl executable is available
    '''
    if os.name == 'nt' and 'IASL_PREFIX' not in os.environ:
        os.environ['IASL_PREFIX'] = "C:\\ASL\\"

    cmd = os.path.join(os.environ.get('IASL_PREFIX', ''), 'iasl')
    try:
        ver_str = subprocess.check_output([cmd, '-v']).decode().strip()
        ver = re.search(r'version\s*([\d.]+)', ver_str).group(1)
    except:
        print('ERROR: iasl not available. Please set IASL_PREFIX.')
        ver = ''
        pass
    return is_valid_tool_version(cmd, ver)

def check_for_git():
    '''
    Verify Git executable is available
    '''
    cmd = 'git'
    try:
        ver_str = subprocess.check_output([cmd, '--version']).decode().strip()
        ver = re.search(r'version\s*([\d.]+)', ver_str).group(1)
    except:
        print('ERROR: Git not found. Please install Git or check if Git is in the PATH environment variable.')
        ver = ''
        pass
    return is_valid_tool_version(cmd, ver, True)

def check_for_toolchain(toolchain_preferred):
    toolchain = None
    if toolchain_preferred.startswith('clang'):
        toolchain, toolchain_prefix, toolchain_path, toolchain_ver = get_clang_info ()
    elif sys.platform == 'darwin':
        toolchain, toolchain_prefix, toolchain_path, toolchain_ver = get_clang_info ()
        toolchain, toolchain_prefix, toolchain_path = 'XCODE5', None, None
    elif os.name == 'posix':
        toolchain, toolchain_prefix, toolchain_path, toolchain_ver = get_gcc_info ()
    elif os.name == 'nt':
        toolchain, toolchain_prefix, toolchain_path, toolchain_ver = get_visual_studio_info (toolchain_preferred)

    if not toolchain:
        return False

    os.environ['TOOL_CHAIN'] = toolchain
    if toolchain_prefix:
        os.environ[toolchain_prefix] = toolchain_path
    return True

def verify_toolchains(toolchain_preferred, toolchain_dict = None):
    print('Checking Toolchain Versions...')

    if toolchain_dict:
        build_toolchains.update(toolchain_dict)

    valid  = check_for_python()
    valid &= check_for_openssl()
    valid &= check_for_nasm()
    valid &= check_for_iasl()
    valid &= check_for_git()
    valid &= check_for_toolchain(toolchain_preferred)

    if valid != True:
        print('...Failed! Please check toolchain versions!')
        sys.exit(-1)
    print('...Done!\n')

def check_for_slimbootkeydir():
    if not os.path.exists(os.environ.get('SBL_KEY_DIR')):
        print ("!!! ERROR: SBL_KEY_DIR is set to directory %s does not exist!!! \n"  % os.environ['SBL_KEY_DIR'])
        print (MESSAGE_SBL_KEY_DIR)
        sys.exit(1)
    else:
        print ("SBL_KEY_DIR is set to %s !!" % os.path.abspath(os.environ.get('SBL_KEY_DIR')))

def copy_images_to_output (fv_dir, zip_file, img_list, rgn_name_list, out_list):
    zip_path_file = os.path.join (os.environ['WORKSPACE'], zip_file)
    output_dir    = os.path.dirname(zip_path_file)
    if not os.path.exists(output_dir):
        os.makedirs(output_dir)
    stitch_zip = zipfile.ZipFile(zip_path_file, 'w')

    zipped_list = []
    for out_file in out_list:
        src_file = os.path.join(fv_dir, out_file)
        for each_file in glob.glob(src_file):
            shutil.copy (each_file, output_dir)
            comp_file = ntpath.basename(each_file)
            stitch_zip.write (os.path.join(output_dir, comp_file), comp_file, compress_type = zipfile.ZIP_DEFLATED)
            zipped_list.append(comp_file)

    for idx, (out_file, file_list) in enumerate(img_list):
        if out_file in rgn_name_list:
            continue
        Ignore = True
        # Loop through the file list to see if all of them are ignored
        for src, algo, val, mode, pos in file_list:
            if mode & STITCH_OPS.MODE_FILE_IGNOR:
                continue
            # Found one file which is not ignored, so look for the file in build directory
            Ignore = False
            break
        # Out file is marked ignored, so ignore it.
        if Ignore == True:
            continue
        shutil.copy(os.path.join(fv_dir, out_file), output_dir)
        comp_file = ntpath.basename(out_file)
        if comp_file not in zipped_list:
            stitch_zip.write (os.path.join(output_dir, comp_file), comp_file, compress_type = zipfile.ZIP_DEFLATED)

    stitch_zip.close()

def rebase_stage (in_file, out_file, delta):

    if not os.path.exists(in_file):
        raise Exception("file '%s' not found !" % in_file)

    fd = FirmwareDevice(0, in_file)
    fd.ParseFd ()
    fd.ParseFsp ()

    # Data for the output file, this data will be modified below
    out_bins = fd.FdData

    # Base address for the stage1b FV is populated at offset 0 in Stage1b.fd
    old_entry = c_uint32.from_buffer(out_bins, 0)
    old_base  = c_uint32.from_buffer(out_bins, 4)

    # Calculate the delta between the old base and new base
    new_entry = old_entry.value + delta
    new_base  = old_base.value  + delta

    fsp_fv_idx_list = []
    for fsp in fd.FspList:
        fsp_fv_idx_list.extend(fsp.FvIdxList)

    for idx, fv in enumerate(fd.FvList):
        if idx in fsp_fv_idx_list:
            continue

        # Rebase stage1b redundant copy to the redundant stage1b base address
        rebase_fv (fv, out_bins, delta)

    # update the redundant stage1b fv base address at offset 0
    old_entry.value = new_entry
    old_base.value  = new_base

    # Open bios image and write rebased stage1b.fd to the redundant stage1b region
    open(out_file, 'wb').write(out_bins)


def rebase_fv (fv, out_bin, delta):
    if len(fv.FfsList) == 0:
        return

    # Loop through the ffslist to identify TE and PE images
    imglist = []
    for ffs in fv.FfsList:
        for sec in ffs.SecList:
            if sec.SecHdr.Type in [EFI_SECTION_TYPE.TE, EFI_SECTION_TYPE.PE32]:   # TE or PE32
                offset = fv.Offset + ffs.Offset + sec.Offset + sizeof(sec.SecHdr)
                imglist.append ((offset, len(sec.SecData) - sizeof(sec.SecHdr)))

    # Rebase all TE and PE images to new base address
    fcount  = 0
    pcount  = 0
    for (offset, length) in imglist:
        img = PeTeImage(offset, out_bin[offset:offset + length])
        img.ParseReloc()
        pcount += img.Rebase(delta, out_bin)
        fcount += 1

    print("Patched %d entries in %d TE/PE32 images." % (pcount, fcount))


def decode_flash_map (flash_map_file, print_address = True):

    if not os.path.exists(flash_map_file):
        raise Exception("No layout file '%s' found !" % flash_map_file)
        return

    fmap_bins = open (flash_map_file, 'rb')
    flash_map_data = bytearray(fmap_bins.read())
    fmap_bins.close()

    flash_map = FLASH_MAP.from_buffer (flash_map_data)
    entry_num = (flash_map.length - sizeof(FLASH_MAP)) // sizeof(FLASH_MAP_DESC)

    image_size = flash_map.romsize
    image_base = 0x100000000 - image_size

    flash_map_lines = [
            "\nFlash Map Information:\n" \
            "\t+------------------------------------------------------------------------+\n" \
            "\t|                              FLASH  MAP                                |\n" \
            "\t|                         (RomSize = 0x%08X)                         |\n"     \
            "\t+------------------------------------------------------------------------+\n" \
            "\t|   NAME   |     OFFSET  (BASE)     |    SIZE    |         FLAGS         |\n" \
            "\t+----------+------------------------+------------+-----------------------+\n"  % image_size]

    region   = '      '
    prev_rgn = 'TS'
    disp_rgn = ''

    for idx in range (entry_num):
        desc  = FLASH_MAP_DESC.from_buffer (flash_map_data, sizeof(FLASH_MAP) + idx * sizeof(FLASH_MAP_DESC))
        flags = 'Compressed  '  if (desc.flags & FLASH_MAP.FLASH_MAP_DESC_FLAGS['COMPRESSED']) else 'Uncompressed'
        for rgn_name, rgn_flag in list(FLASH_MAP.FLASH_MAP_DESC_FLAGS.items()):
            if rgn_flag == (desc.flags & 0x0F):
                if rgn_flag & (FLASH_MAP.FLASH_MAP_DESC_FLAGS['NON_REDUNDANT'] | FLASH_MAP.FLASH_MAP_DESC_FLAGS['NON_VOLATILE']):
                    rgn_suf      = ''
                    disp_rgn_suf = ''
                else:
                    suffixes = 'B' if desc.flags & FLASH_MAP.FLASH_MAP_DESC_FLAGS['BACKUP'] else 'A'
                    rgn_suf      = '_' + suffixes
                    disp_rgn_suf = ' ' + suffixes
                region   = ''.join([word[0] for word in rgn_name.split('_')]) + rgn_suf
                disp_rgn = rgn_name.replace('_', ' ') + disp_rgn_suf
                region   = region.center(4, ' ')
                disp_rgn = disp_rgn.center(23, ' ')
                break

        if region != '      ':
            if region != prev_rgn:
                prev_rgn = region
                flash_map_lines.append (
                  "\t+------------------------------------------------------------------------+\n" \
                  "\t|                        %s                         |\n"                      \
                  "\t+------------------------------------------------------------------------+\n" % disp_rgn)
            flags += ', '
        flags += region
        if print_address:
            address = '0x%08X' % (desc.offset + image_base)
        else:
            address = ' ???????? '
        flash_map_lines.append ("\t|   %s   |  0x%06x(%s)  |  0x%06x  |  %s   |\n" \
            % (desc.sig.decode(), desc.offset, address, desc.size, flags))

    flash_map_lines.append ("\t+----------+------------------------+------------+-----------------------+\n")

    return ''.join(flash_map_lines)


def find_component_in_image_list (comp_name, img_list):
    for (out_file, file_list) in img_list:
        for file in file_list:
            if comp_name == file[0]:
                return file
    return None


def print_component_list (comp_list):
    for comp in comp_list:
        print('%-20s BASE=0x%08X' % (comp['name'], comp['base']))

def gen_pci_enum_policy_info (policy_dict):
    policy_info   = PciEnumPolicyInfo()
    struct_string = ''
    items_string  = ''
    try:
        policy_info.DowngradeIo32   = policy_dict['DOWNGRADE_IO32']
        policy_info.DowngradeMem64  = policy_dict['DOWNGRADE_MEM64']
        policy_info.DowngradePMem64 = policy_dict['DOWNGRADE_PMEM64']
        policy_info.DowngradeBus0   = policy_dict['DOWNGRADE_BUS0']
        policy_info.FlagAllocPmemFirst  = policy_dict['FLAG_ALLOC_PMEM_FIRST']
        policy_info.FlagAllocRomBar = policy_dict['FLAG_ALLOC_ROM_BAR']
        policy_info.BusScanType     = policy_dict['BUS_SCAN_TYPE']
        bus_scan_items              = policy_dict['BUS_SCAN_ITEMS']

        # Bus Scan List Type
        if policy_info.BusScanType == 0:
            # use dictionary key to remove duplicated bus number
            items_dict = dict((int(elem, 0), 0) for elem in bus_scan_items.split(','))
            for i in sorted (items_dict.keys()):
                if i > 255:
                    raise Exception('Bus number cannot exceed 255!')
                items_string += (',0x%02x' % i)

            policy_info.NumOfBus = len(items_dict)
        # Bus Scan Range Type
        else:
            items_list = [int(elem, 0) for elem in bus_scan_items.split(',')]
            policy_info.NumOfBus = len(items_list)
            if policy_info.NumOfBus != 2:
                raise Exception('Bus Scan Range type must have two bus number for start and end!')

            items_list.sort()
            items_string = ',0x%02x,0x%02x' % (items_list[0], items_list[1])

        # combine structure and data
        struct_data = list(bytearray(policy_info))
        struct_string = ','.join(['0x%02x' % elem for elem in struct_data])
        struct_string = '{' + struct_string + items_string + '}'
    except KeyError:
        raise Exception ("Failed to generate PCI_ENUM_POLICY_INFO!")

    return struct_string

def get_vtf_patch_base (stage1a_fd):
    stage1a_bin = bytearray (get_file_data (stage1a_fd))
    dlen = len(stage1a_bin) & ~0xF
    if dlen > 0x1000:
      dlen = 0x1000

    found = 0
    for i in range (0, dlen, 16):
      if stage1a_bin[-i:-i+8] == b"\xF0\x0F\xAA\x55\x78\x56\x34\x12":
        found = 0x100000000 - i
        break

    if not found:
      raise Exception ("Could not find patchable data region in VTF !")

    return found
