## @
#  This is a python script for generating required output format
#  of Gpio pin information from a given input format
#
# Copyright (c) 2019 - 2024, Intel Corporation. All rights reserved. <BR>
# SPDX-License-Identifier: BSD-2-Clause-Patent
#

# This tool's input/output can be one of the following formats:
# .h input
#
# static GPIOV2_INIT_CONFIG mGpioTable[] =
# {
#    //     GpioPad                  PadMode            HostSwOwn         Direction         OutputState           InterruptConfig          ResetConfig     TerminationConfig    LockConfig     LockTx
#    {GPIOV2_MTL_PCH_S_GPP_H_0,  {GpioV2PadModeGpio, GpioV2HostOwnAcpi, GpioV2DirInInv,  GpioV2StateDefault, GpioV2IntEdge|GpioV2IntSci, GpioV2ResetHost,  GpioV2TermDefault,  GpioV2Unlock,  GpioV2Lock  }},  //CODEC_INT_N
#    {GPIOV2_MTL_PCH_S_GPP_E_13, {GpioV2PadModeGpio, GpioV2HostOwnAcpi, GpioV2DirOut,    GpioV2StateHigh,    GpioV2IntDis,               GpioV2ResetHost,  GpioV2TermDefault                              }},  //PCH_CODEC_PWR_EN
# };

# .yaml input
#                                            SBL DW0     SBL DW1
#   - !expand { GPIO_TMPL : [ GPP_A,  00,  0x00000000,  0x08000000 ] }

# .dlt input
#
# GPIO_CFG_DATA.GpioPinConfig0_GPP_A07 | 0x******** # SBL DW0
# GPIO_CFG_DATA.GpioPinConfig1_GPP_A07 | 0x******** # SBL DW1
##

import os
import sys
import argparse
import re
from   ctypes import *

YML_LINE_HDR = "- !expand { GPIO_TMPL : [ "
DLT_CFGx_HDR = "GPIO_CFG_DATA.GpioPinConfig"
DLT_CFG0_HDR = DLT_CFGx_HDR + "0_"
DLT_CFG1_HDR = DLT_CFGx_HDR + "1_"
H_STRUCT_HDR = "static GPIOV2_INIT_CONFIG mGpioTable[] = {\n"
H_STRUCT_FTR = "\n};"

def GPIO_ASSIGN_VALUE (Val):
    return ((Val << 1) | 0x01)

# Following are dictionaries for the values
# each field in Pad Config can take
GPIOV2_PAD_MODE = {
  'GpioV2PadModeHardwareDefault' : 0x0,
  'GpioV2PadModeGpio'            : GPIO_ASSIGN_VALUE(0x0),
  'GpioV2PadModeNative1'         : GPIO_ASSIGN_VALUE(0x1),
  'GpioV2PadModeNative2'         : GPIO_ASSIGN_VALUE(0x2),
  'GpioV2PadModeNative3'         : GPIO_ASSIGN_VALUE(0x3),
  'GpioV2PadModeNative4'         : GPIO_ASSIGN_VALUE(0x4),
  'GpioV2PadModeNative5'         : GPIO_ASSIGN_VALUE(0x5),
  'GpioV2PadModeNative6'         : GPIO_ASSIGN_VALUE(0x6),
  'GpioV2PadModeNative7'         : GPIO_ASSIGN_VALUE(0x7),
  'GpioV2PadModeNative8'         : GPIO_ASSIGN_VALUE(0x8),
  'GpioV2PadModeNative9'         : GPIO_ASSIGN_VALUE(0x9),
  'GpioV2PadModeNative10'        : GPIO_ASSIGN_VALUE(0xA),
  'GpioV2PadModeNative11'        : GPIO_ASSIGN_VALUE(0xB),
  'GpioV2PadModeNative12'        : GPIO_ASSIGN_VALUE(0xC),
  'GpioV2PadModeNative13'        : GPIO_ASSIGN_VALUE(0xD),
  'GpioV2PadModeNative14'        : GPIO_ASSIGN_VALUE(0xE),
  'GpioV2PadModeNative15'        : GPIO_ASSIGN_VALUE(0xF)
}

GPIOV2_HOSTSW_OWN = {
  'GpioV2HostOwnDefault'         : 0x0,
  'GpioV2HostOwnAcpi'            : GPIO_ASSIGN_VALUE(0x0),
  'GpioV2HostOwnGpio'            : GPIO_ASSIGN_VALUE(0x1)
}

GPIOV2_DIRECTION = {
  'GpioV2DirDefault'             : 0x0,
  'GpioV2DirInOut'               : GPIO_ASSIGN_VALUE(0x1),
  'GpioV2DirInInvOut'            : GPIO_ASSIGN_VALUE(0x2),
  'GpioV2DirIn'                  : GPIO_ASSIGN_VALUE(0x3),
  'GpioV2DirInInv'               : GPIO_ASSIGN_VALUE(0x4),
  'GpioV2DirOut'                 : GPIO_ASSIGN_VALUE(0x5),
  'GpioV2DirNone'                : GPIO_ASSIGN_VALUE(0x6)
}

GPIOV2_OUTPUT_STATE = {
  'GpioV2StateDefault'           : 0x0,
  'GpioV2StateLow'               : GPIO_ASSIGN_VALUE(0x0),
  'GpioV2StateHigh'              : GPIO_ASSIGN_VALUE(0x1)
}

GPIOV2_INT_CONFIG = {
  'GpioV2IntDefault'             : 0x0,
  'GpioV2IntDis'                 : 0x1,
  'GpioV2IntNmi'                 : GPIO_ASSIGN_VALUE(0x1),
  'GpioV2IntSmi'                 : GPIO_ASSIGN_VALUE(0x2),
  'GpioV2IntSci'                 : GPIO_ASSIGN_VALUE(0x4),
  'GpioV2IntApic'                : GPIO_ASSIGN_VALUE(0x8),
  'GpioV2IntLevel'               : GPIO_ASSIGN_VALUE(0x10),
  'GpioV2IntEdge'                : GPIO_ASSIGN_VALUE(0x20),
  'GpioV2IntLvlEdgDis'           : GPIO_ASSIGN_VALUE(0x40),
  'GpioV2IntBothEdge'            : GPIO_ASSIGN_VALUE(0x80),
}

GPIOV2_RESET_CONFIG = {
  'GpioV2ResetDefault'           : 0x00,
  'GpioV2ResetResume'            : GPIO_ASSIGN_VALUE(0x0),
  'GpioV2ResetHostDeep'          : GPIO_ASSIGN_VALUE(0x1),
  'GpioV2ResetHost'              : GPIO_ASSIGN_VALUE(0x2),
  'GpioV2DswReset'               : GPIO_ASSIGN_VALUE(0x3),
  'GpioV2ResetGlobal'            : GPIO_ASSIGN_VALUE(0x4)
}

GPIOV2_TERMINATION_CONFIG = {
  'GpioV2TermDefault'            : 0x0,
  'GpioV2TermNone'               : 0x1,
  'GpioV2TermWpd5K'              : 0x5,
  'GpioV2TermWpd20K'             : 0x9,
  'GpioV2TermWpu1K'              : 0x13,
  'GpioV2TermWpu2K'              : 0x17,
  'GpioV2TermWpu5K'              : 0x15,
  'GpioV2TermWpu20K'             : 0x19,
  'GpioV2TermWpu1K2K'            : 0x1B,
  'GpioV2TermNative'             : 0x1F
}

GPIOV2_PAD_LOCK = {
  'GpioV2LockHardwareDefault'    : 0x0,
  'GpioV2Unlock'                 : GPIO_ASSIGN_VALUE(0x0),  # Leave Pad configuration unlocked
  'GpioV2Lock'                   : GPIO_ASSIGN_VALUE(0x1)   # Lock Pad configuration
}

GPIOV2_OTHER_CONFIG = {
  'GpioV2RxRaw1Default'          : 0x0,  # Use default input override value
  'GpioV2RxRaw1Dis'              : 0x1,  # Don't override input
  'GpioV2RxRaw1En'               : 0x3   # Override input to '1'
}


# SBL's GPIO config dword0
class SBL_DW0 (Union):
    class DW0_TMPL (Structure):
        _pack_ = 1
        _fields_ = [
            ('PadMode',             c_uint32,   5),
            ('HostSwOwn',           c_uint32,   2),
            ('Direction',           c_uint32,   6),
            ('OutputState',         c_uint32,   2),
            ('InterruptConfig',     c_uint32,   9),
            ('ResetConfig',         c_uint32,   8),
        ]
    _pack = 1
    _fields_ = [
        ('Dw0',     c_uint32),
        ('Dw0Tmpl', DW0_TMPL),
    ]

# SBL's GPIO config dword1
class SBL_DW1 (Union):
    class DW1_TMPL (Structure):
        _pack_ = 1
        _fields_ = [
        ('TerminationConfig',   c_uint32,   6),
        ('LockConfig',          c_uint32,   2),
        ('LockTx',              c_uint32,   2),
        ('Reserved',            c_uint32,   6), # OtherSettings
        ('PadIdx',              c_uint32,   5),
        ('GrpIdx',              c_uint32,   3),
        ('ComIdx',              c_uint32,   3),
        ('ChipId',              c_uint32,   3),
        ('Hide',                c_uint32,   1),
        ('Skip',                c_uint32,   1),
        ]
    _pack_ = 1
    _fields_ = [
        ('Dw1',     c_uint32),
        ('Dw1Tmpl', DW1_TMPL),
    ]

SBL_DW1_PAD_MASK = 0x3FFF0000


#
# Misc global start
#

# global dicts
yml_dict     = {}
dlt_dict     = {}
h_dict       = {}
cfgdata_dict = {}
pad_dict     = {}

g_debug      = False

#
# GPIO convert helpers start
#

# Verify if the in_file and out_file are valid
def validate_args (in_file, out_file):
    valid = True
    lines = open (in_file, 'r').readlines()

    if ( in_file.split('.')[1] in ['h'] and out_file.split('.')[1] not in ['yaml', 'dlt'] ):
        raise Exception ("Input and output file extension unmatched as file format!")

    if ( in_file.split('.')[1] in ['yaml', 'dlt'] and out_file.split('.')[1] not in ['h'] ):
        raise Exception ("Input and output file extension unmatched as file format!")

    if in_file.endswith ('.h'):
        valid = False
        for line in lines:
            if line.strip().startswith ('static GPIOV2_INIT_CONFIG') or \
               line.strip().startswith ('GLOBAL_REMOVE_IF_UNREFERENCED GPIOV2_INIT_CONFIG') :
                valid = True
                break
        if not valid:
            raise Exception ("Invalid .h format")
    elif in_file.endswith ('.yaml'):
        valid = True
    elif in_file.endswith ('.dlt'):
        valid = True
    else:
        raise Exception ("Invalid input type!")

    return valid


# Convert the lines and get separated parts
def get_parts_from_inp (in_file, line, next_line):
    parts = []
    if in_file.endswith ('.h'):
        line = line.replace('{', ' ').replace('}', ' ').replace(' ', '').rstrip('\n').lstrip()
        line = line.split("//")[0]
        if len(line) > 0:
            parts = line.split(',')
    elif in_file.endswith ('.yaml'):
        if line.lstrip().startswith (YML_LINE_HDR):
            line = line.lstrip()[len(YML_LINE_HDR):]
            line = line.replace(' ', '').rstrip(' ] }\n')
            parts = line.split(',')
            if len(parts) == 4:
                # Convert to standard 3 elements:
                # GROUP, PIN, DWORD0, DWORD1 ==> GROUP_PIN, DWORD0, DWORD1
                parts = [''.join(parts[0:2]), parts[2], parts[3]]
    elif in_file.endswith ('dlt'):
        if line.startswith (DLT_CFG0_HDR):
            line = line.replace(' ', '').rstrip('\n')
            parts = line.split('|')
            next_line = next_line.replace(' ', '').rstrip('\n')
            parts += next_line.split('|')
    else:
        parts = []

    return parts

# create a dict from GPIO config data yaml file
def create_cfgdata_dict(work_path):
    all_lines = []
    for rootdir in work_path.split(";"):
        list_dirs = os.walk(rootdir)
        for root, dirs, files in list_dirs:
            for f in files:
                if os.path.join(root, f).endswith(".yaml") :
                    fp=open(os.path.join(root, f),'r')
                    lines = fp.readlines()
                    fp.close()
                    all_lines.extend (lines)

    for line in all_lines:
        if 'GPIO_TMPL' in line and 'expand' in line:
            x = re.findall("\[.*\]", line)
            pads = x[0][1:-1].split(',')
            key = int(pads[3].strip(), 16) & SBL_DW1_PAD_MASK
            cfgdata_dict[key] = pads[0].strip() + pads[1].strip()


def create_pad_dict (defines):
    sbl_dw1 = SBL_DW1 ()
    for line in defines:
        if 'GPIOV2_PAD_ID' in line and ('GPP' in line or 'GPD' in line):
            pad_name = line.split(' ')[1]
            x = re.findall("\(.*\)", line)
            pads = x[0][1:-1].split(',')

            chipid_bit_position    = get_define_value (defines, pads[1].strip())
            if chipid_bit_position == 0 or chipid_bit_position > 128 :
                raise Exception ("Invalid chipid 0x%x, chipid range is from BIT0 to BIT7" % chipid_bit_position)
            sbl_dw1.Dw1Tmpl.ChipId = chipid_bit_position.bit_length() - 1
            sbl_dw1.Dw1Tmpl.ComIdx = get_define_value (defines, pads[3].strip())
            sbl_dw1.Dw1Tmpl.GrpIdx = get_define_value (defines, pads[4].strip())
            sbl_dw1.Dw1Tmpl.PadIdx = get_define_value (defines, pads[5].strip())
            pad_dict[sbl_dw1.Dw1] = pad_name


#
# [h] to [yaml, dlt] start
#

def get_value_from_string(s):
    if s.startswith(("0x", "0X")):
        try:
            return int(s, 16)
        except ValueError:
            return None
    elif s.isdigit():
        return int(s)
    else:
        return None

def get_define_value(defines, search_name):
    value = get_value_from_string(search_name)
    if value is not None:
        return value
    pattern = r'^#define[ \t]*%s[ \t]*([^\s]+)' % re.escape(search_name)
    for line in defines:
        match = re.match(pattern, line.lstrip())
        if match:
            value = get_value_from_string(match.group(1).strip())
            if value is not None:
                return value
    return 0


def get_sbl_dw1_from_pad (pad_name, sbl_dw1):
    pad_dict2   = dict((v,k) for k,v in pad_dict.items())
    sbl_dw1.Dw1 = (sbl_dw1.Dw1 & ~SBL_DW1_PAD_MASK) | pad_dict2[pad_name]


# convert from .h to SBL GPIO CFG DWs
def get_sbl_dws_from_h (parts, sbl_dw0, sbl_dw1):
    # SBL Cfg Dw0
    sbl_dw0.Dw0Tmpl.PadMode         = GPIOV2_PAD_MODE[parts[1]]
    sbl_dw0.Dw0Tmpl.HostSwOwn       = GPIOV2_HOSTSW_OWN[parts[2]]
    sbl_dw0.Dw0Tmpl.Direction       = GPIOV2_DIRECTION[parts[3]]
    sbl_dw0.Dw0Tmpl.OutputState     = GPIOV2_OUTPUT_STATE[parts[4]]
    for key in parts[5].split('|'):
        sbl_dw0.Dw0Tmpl.InterruptConfig |= GPIOV2_INT_CONFIG[key]
    sbl_dw0.Dw0Tmpl.ResetConfig     = GPIOV2_RESET_CONFIG[parts[6]]

    # SBL Cfg Dw1
    sbl_dw1.Dw1Tmpl.TerminationConfig = GPIOV2_TERMINATION_CONFIG[parts[7]]
    sbl_dw1.Dw1Tmpl.LockConfig        = GPIOV2_PAD_LOCK['GpioV2LockHardwareDefault']
    sbl_dw1.Dw1Tmpl.LockTx            = GPIOV2_PAD_LOCK['GpioV2LockHardwareDefault']
    if len(parts) > 8 and parts[8].startswith('GpioV2'):
        sbl_dw1.Dw1Tmpl.LockConfig    = GPIOV2_PAD_LOCK[parts[8]]
    if len(parts) > 9 and parts[9].startswith('GpioV2'):
        sbl_dw1.Dw1Tmpl.LockTx        = GPIOV2_PAD_LOCK[parts[9]]


# Get #define statement from h header files
def get_define_statements_from_header_files(in_file, work_path):
    all_lines = []

    for rootdir in work_path.split(";"):
        list_dirs = os.walk(rootdir)
        for root, dirs, files in list_dirs:
            for f in files:
                if (f != in_file) and os.path.join(root, f).endswith(".h") :
                    if g_debug:
                        print (" f = %s" %os.path.join(root, f))
                    fp=open(os.path.join(root, f),'r')
                    lines = fp.readlines()
                    fp.close()
                    all_lines.extend (lines)

    if g_debug:
        fd = open ("define.txt", 'w')
    define_lines = []
    for line in all_lines:
        match = re.match('^#define', line.lstrip())
        if match:
            define_lines.extend ([line])
            if g_debug:
                fd.write (line)
    if g_debug:
        fd.close()

    return define_lines


# Update the SBL Config Dw0 and dw1 from '.h'
def get_sbl_dws (in_file, parts):
    sbl_dw0 = SBL_DW0 ()
    sbl_dw1 = SBL_DW1 ()

    if not ('GPP_' in parts[0] or 'GPD' in parts[0]):
        return '', 0x0, 0x0

    get_sbl_dw1_from_pad (parts[0], sbl_dw1)
    get_sbl_dws_from_h (parts, sbl_dw0, sbl_dw1)
    pad_name = cfgdata_dict[sbl_dw1.Dw1 & SBL_DW1_PAD_MASK]
    if g_debug:
        print ("get_sbl_dws pad_name = %s" % pad_name)

    return pad_name, sbl_dw0.Dw0, sbl_dw1.Dw1

#
# [yaml, dlt] to [h] start
#

# Get a line from SBL Config DWs
def get_h_line_from_sbl_dws (sbl_dw0, sbl_dw1):
    PadMode         = list(GPIOV2_PAD_MODE.keys())[list(GPIOV2_PAD_MODE.values()).index(sbl_dw0.Dw0Tmpl.PadMode)]
    HostSwOwn       = list(GPIOV2_HOSTSW_OWN.keys())[list(GPIOV2_HOSTSW_OWN.values()).index(sbl_dw0.Dw0Tmpl.HostSwOwn)]
    Direction       = list(GPIOV2_DIRECTION.keys())[list(GPIOV2_DIRECTION.values()).index(sbl_dw0.Dw0Tmpl.Direction)]
    OutputState     = list(GPIOV2_OUTPUT_STATE.keys())[list(GPIOV2_OUTPUT_STATE.values()).index(sbl_dw0.Dw0Tmpl.OutputState)]
    if sbl_dw0.Dw0Tmpl.InterruptConfig <= 0x1F:
        IntConfig   = list(GPIOV2_INT_CONFIG.keys())[list(GPIOV2_INT_CONFIG.values()).index(sbl_dw0.Dw0Tmpl.InterruptConfig)]
    else:
        int_cfg_1   = sbl_dw0.Dw0Tmpl.InterruptConfig & 0x1F
        int_cfg_2   = sbl_dw0.Dw0Tmpl.InterruptConfig & (~0x1F) + 1
        IntConfig   =  list(GPIOV2_INT_CONFIG.keys())[list(GPIOV2_INT_CONFIG.values()).index(int_cfg_1)] + '|'
        IntConfig  +=  list(GPIOV2_INT_CONFIG.keys())[list(GPIOV2_INT_CONFIG.values()).index(int_cfg_2)]
    ResetConfig     = list(GPIOV2_RESET_CONFIG.keys())[list(GPIOV2_RESET_CONFIG.values()).index(sbl_dw0.Dw0Tmpl.ResetConfig)]

    TermConfig      = list(GPIOV2_TERMINATION_CONFIG.keys())[list(GPIOV2_TERMINATION_CONFIG.values()).index(sbl_dw1.Dw1Tmpl.TerminationConfig)]
    LockConfig      = list(GPIOV2_PAD_LOCK.keys())[list(GPIOV2_PAD_LOCK.values()).index(sbl_dw1.Dw1Tmpl.LockConfig)]
    LockTx          = list(GPIOV2_PAD_LOCK.keys())[list(GPIOV2_PAD_LOCK.values()).index(sbl_dw1.Dw1Tmpl.LockTx)]

    h_line = "%s, %s, %s, %s, %s, %s, %s, %s, %s" % \
                (PadMode, HostSwOwn, Direction, OutputState, IntConfig, ResetConfig, TermConfig, LockConfig, LockTx)

    return h_line


# Convert SBL Config DWs to .h  fomrat
def parse_sbl_dws (in_file, out_fmt, parts):
    sbl_dw0 = SBL_DW0 ()
    sbl_dw1 = SBL_DW1 ()

    if in_file.endswith ('.yaml'):
        sbl_dw0.Dw0 = int (parts[1], 0)
        sbl_dw1.Dw1 = int (parts[2], 0)
    elif in_file.endswith ('.dlt'):
        sbl_dw0.Dw0 = int (parts[1], 0)
        sbl_dw1.Dw1 = int (parts[3], 0)

    pad_name = pad_dict[sbl_dw1.Dw1 & SBL_DW1_PAD_MASK]
    if g_debug:
        print ("pad_name = %s" % pad_name)

    out_line = get_h_line_from_sbl_dws (sbl_dw0, sbl_dw1)

    return pad_name, out_line

#
# Main helpers start
#

# Convert and populate the global dictionaries
def convert_from_inp_to_out (in_file, out_fmt, parts):

    if in_file.endswith ('.h'):
        pad_name, dw0, dw1 = get_sbl_dws (in_file, parts)

        if pad_name != '':
            if out_fmt == 'yaml':
                #Ex: !expand { GPIO_TMPL : [ GPP_A,  00,  0x0350A383,  0x00002019 ] }
                idx = len(pad_name)
                while idx > 0:
                    if not pad_name[idx-1].isdigit():
                        break
                    idx = idx - 1
                if idx > 0:
                    pad_name = '%s, %s' % (pad_name[:idx], pad_name[idx:])

                sbl_yml_line = YML_LINE_HDR + pad_name + ", 0x%08X" % dw0 + ", 0x%08X" % dw1 + " ] }"
                yml_dict[pad_name] = sbl_yml_line
            else:
                sbl_dlt_line =  DLT_CFG0_HDR + pad_name + " | 0x%08X\n"  % dw0
                sbl_dlt_line += DLT_CFG1_HDR + pad_name + " | 0x%08X"    % dw1
                dlt_dict[pad_name] = sbl_dlt_line
    elif in_file.endswith ('.yaml') or in_file.endswith ('.dlt'):
        pad_name, out_line = parse_sbl_dws (in_file, out_fmt, parts)
        if pad_name != '':
            if out_fmt == 'h':
                h_line = "{" + pad_name + ", { " + out_line + "  }},\n"
                h_dict[pad_name] = h_line
            else:
                print('Invalid out type')

# Gen output file in the required format
def gen_file_from_object (file, object):
    open (file, 'wb').write(object)

# conversion between formats
def gpio_convert (args):
    # Validate input/output
    valid = validate_args (args.in_file, args.out_file)
    if not valid:
        raise Exception ("Invalid input '.%s'" % args.in_file.split('.')[1])

    g_debug = args.verbose
    if args.in_file.endswith ('.h'):
        create_cfgdata_dict (args.work_path)
        if g_debug:
            print ("Print the cfg data dict generated from yaml files")
            for key in cfgdata_dict:
                print ("0x%08X , %s" % (key, cfgdata_dict[key]))

    defines = get_define_statements_from_header_files (args.in_file, args.work_path)
    create_pad_dict (defines)
    if g_debug:
        print ("Print the pad dict generated from header files")
        for key in pad_dict:
            print ("0x%08X : %s" %(key, pad_dict[key]))

    output_fmt = args.out_file.split('.')[1]
    fp = open (args.in_file, 'r')
    lines = fp.readlines()
    fp.close ()

    # Call function to convert
    if args.in_file:
        for idx, line in enumerate(lines):
            if idx < len(lines) - 1:
                parts = get_parts_from_inp (args.in_file, line, lines[idx + 1])
            else:
                parts = get_parts_from_inp (args.in_file, line, '')
            if len(parts) == 0:
                continue
            convert_from_inp_to_out (args.in_file, output_fmt, parts)

    # Copy the final data to ouput
    sbl_data = ""
    if output_fmt == 'yaml':
        for key in (sorted(yml_dict) if args.sort else yml_dict) :
            sbl_data += yml_dict[key] + '\n'
    elif output_fmt == 'dlt':
        for key in (sorted(dlt_dict) if args.sort else dlt_dict) :
            sbl_data += dlt_dict[key] + '\n'
    elif output_fmt == 'h':
        sbl_data += H_STRUCT_HDR
        for key in (sorted(h_dict) if args.sort else h_dict) :
            sbl_data += h_dict[key]
        sbl_data += H_STRUCT_FTR

    # Copy sbl content into the required out file
    out_dir = os.path.dirname(args.out_file)
    out_file = os.path.basename(args.out_file)
    out_file = os.path.join (out_dir, out_file)
    gen_file_from_object (out_file, sbl_data.encode('utf-8'))

#
# Main helpers end
#

# Main
def main ():
    ap = argparse.ArgumentParser()
    ap.add_argument(  '-if',
                        dest='in_file',
                        type=str,
                        required=True,
                        help='Input data file, must have [yaml, dlt] or [h] file extension')
    ap.add_argument(  '-of',
                        dest='out_file',
                        default='.',
                        type=str,
                        help='Output data file, must have [h] or [yaml, dlt] file extension')
    ap.add_argument(  '-w',
                        dest='work_path',
                        default='.',
                        type=str,
                        help='work path to find convert related header files and yaml files')
    ap.add_argument(  '-v',
                        dest='verbose',
                        action='store_true',
                        default = False,
                        help='Output debug message and temp files to help debug script')
    ap.add_argument(  '-s',
                        dest='sort',
                        action='store_true',
                        default = False,
                        help='sort the generated results')

    ap.set_defaults(func=gpio_convert)
    args = ap.parse_args()
    args.func(args)

if __name__ == '__main__':
    sys.exit(main())
