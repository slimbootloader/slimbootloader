## @ GenGpioData.py
#  This is a python script for generating required output format
#  of Gpio pin information from a given input format
#
# Copyright (c) 2019 - 2020, Intel Corporation. All rights reserved. <BR>
# SPDX-License-Identifier: BSD-2-Clause-Patent
#
##

import os
import sys
import argparse
import re
from   importlib.machinery import SourceFileLoader
from   ctypes import *

YML_LINE_HDR = "- !expand { GPIO_TMPL : [ "
DLT_CFGx_HDR = "GPIO_CFG_DATA.GpioPinConfig"
DLT_CFG0_HDR = DLT_CFGx_HDR + "0_"
DLT_CFG1_HDR = DLT_CFGx_HDR + "1_"
H_STRUCT_HDR = "static GPIO_INIT_CONFIG mGpioTable[] = {\n"
H_STRUCT_FTR = "\n};"
CSV_FILE_HDR = "# GpioPad, PadMode, HostSoftPadOwn, Direction, OutputState, InterruptConfig, PowerConfig, ElectricalConfig, LockConfig, OtherSettings\n"
PAD_NAME_HDR = "GPIO_XXX_XX_"
V_PCH_SERIES = 'TBD'


# This tool's input/output can be one of the following formats:
# (Refer to 'Formats Start' below for more details)

# .h input
#
# static GPIO_INIT_CONFIG mGpioTable[] =
# {
#    //     GpioPad               Pmode            GPI_IS       GpioDir    GPIOTxState      RxEvCfg/GPIRoutConfig        PadRstCfg            Term           LockConfig        OtherSettings
#    {GPIO_XXXXXX_GPP_A7,  { GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn, GpioOutDefault, GpioIntLevel | GpioIntApic, GpioHostDeepReset, GpioTermWpu20K, GpioPadConfigUnlock,  GpioRxRaw1Default}},
# };

# .csv input
#
# GpioPad, PadMode, HostSoftPadOwn, Direction, OutputState, InterruptConfig, PowerConfig, ElectricalConfig, LockConfig, OtherSettings
# GPIO_XXXXXX_GPP_A7,GpioPadModeGpio,GpioHostOwnGpio,GpioDirIn,GpioOutDefault,GpioIntLevel|GpioIntApic,GpioHostDeepReset,GpioTermWpu20K,GpioPadConfigUnlock,GpioRxRaw1Default

# .txt input
#                    HostSwOwn  PadCfgLock PadCfgLockTx PadCfgDw0 PadCfgDw1
# GPIO_XXXXXX_GPP_A7:0x********:0x********:0x********:0x********:0x********

# .yaml input
#                                       SBL DW0     SBL DW1
#   - !expand { GPIO_TMPL : [ GPP_A00, 0x00000003, 0x00000000 ] }

# .dlt input
#
# GPIO_CFG_DATA.GpioPinConfig0_GPP_A07 | 0x******** # SBL DW0
# GPIO_CFG_DATA.GpioPinConfig1_GPP_A07 | 0x******** # SBL DW1


#
# Pad Cfg dicts begin
#

# Following are dictionaries for the values
# each field in Pad Config can take

GPIO_PAD_MODE = {
    'GpioHardwareDefault'   : 0x0,
    'GpioPadModeGpio'       : 0x1,
    'GpioPadModeNative1'    : 0x3,
    'GpioPadModeNative2'    : 0x5,
    'GpioPadModeNative3'    : 0x7,
    'GpioPadModeNative4'    : 0x9,
    'GpioPadModeNative5'    : 0xB,
    'GpioPadModeNative6'    : 0xD,
    'GpioPadModeNative7'    : 0xF,
    'GpioPadModeNative8'    : 0x11,
}

GPIO_HOSTSW_OWN = {
    'GpioHostOwnDefault'    : 0x0,
    'GpioHostOwnAcpi'       : 0x1,
    'GpioHostOwnGpio'       : 0x3,
}

GPIO_DIRECTION = {
    'GpioDirDefault'        : 0x0,
    'GpioDirInOut'          : (0x1 | (0x1 << 3)),
    'GpioDirInInvOut'       : (0x1 | (0x3 << 3)),
    'GpioDirIn'             : (0x3 | (0x1 << 3)),
    'GpioDirInInv'          : (0x3 | (0x3 << 3)),
    'GpioDirOut'            : 0x5,
    'GpioDirNone'           : 0x7,
}

GPIO_OUTPUT_STATE = {
    'GpioOutDefault'        : 0x0,
    'GpioOutLow'            : 0x1,
    'GpioOutHigh'           : 0x3,
}

GPIO_INT_CONFIG = {
    'GpioIntDefault'        : 0x0,
    'GpioIntDis'            : 0x1,
    'GpioIntNmi'            : 0x3,
    'GpioIntSmi'            : 0x5,
    'GpioIntSci'            : 0x9,
    'GpioIntApic'           : 0x11,
    'GpioIntLevel'          : (0x1 << 5),
    'GpioIntEdge'           : (0x3 << 5),
    'GpioIntLvlEdgDis'      : (0x5 << 5),
    'GpioIntBothEdge'       : (0x7 << 5),
}

GPIO_RESET_CONFIG = {
    'GpioResetDefault'      : 0x00,
    'GpioResumeReset'       : 0x01,
    'GpioHostDeepReset'     : 0x03,
    'GpioPlatformReset'     : 0x05,
    'GpioDswReset'          : 0x07,
}

GPIO_ELECTRICAL_CONFIG = {
    'GpioTermDefault'       : 0x0,
    'GpioTermNone'          : 0x1,
    'GpioTermWpd5K'         : 0x5,
    'GpioTermWpd20K'        : 0x9,
    'GpioTermWpu1K'         : 0x13,
    'GpioTermWpu2K'         : 0x17,
    'GpioTermWpu5K'         : 0x15,
    'GpioTermWpu20K'        : 0x19,
    'GpioTermWpu1K2K'       : 0x1B,
    'GpioTermNative'        : 0x1F,
}

GPIO_LOCK_CONFIG = {
    'GpioLockDefault'       : 0x0,
    'GpioPadConfigUnlock'   : 0x3,
    'GpioPadConfigLock'     : 0x1,
    'GpioOutputStateUnlock' : 0xC,
    'GpioPadUnlock'         : 0xF,
    'GpioPadLock'           : 0x5,
}

GPIO_OTHER_CONFIG = {
    'GpioRxRaw1Default'     : 0x0,
    'GpioRxRaw1Dis'         : 0x1,
    'GpioRxRaw1En'          : 0x3,
}

#
# Pad Cfg dicts end
#

#
# Formats start
#

# Input format/order for .h/.csv file
class GPIO_FORMAT_H_CSV (Structure):
    _pack_ = 1
    _fields_ = [
        ('GpioPad',         ARRAY(c_char, 20)),
        ('PadMode',         c_uint32,   5),
        ('HostSoftPadOwn',  c_uint32,   2),
        ('Direction',       c_uint32,   6),
        ('OutputState',     c_uint32,   2),
        ('InterruptConfig', c_uint32,   9),
        ('PowerConfig',     c_uint32,   8),
        ('ElectricalConfig',c_uint32,   9),
        ('LockConfig',      c_uint32,   4),
        ('OtherSettings',   c_uint32,   9),
        ('RsvdBits',        c_uint32,   10),
    ]

# Input format for .txt file
class GPIO_FORMAT_TXT (Structure):
    _pack_ = 1
    _fields_ = [
        ('GpioPad',         ARRAY(c_char, 20)),
        ('HostSwPadOwn',    c_uint32), # HOSTSW_OWN reg value of the group GpioPad belongs to
        ('PadCfgLock',      c_uint32), # PADCFGLOCK reg value of the group GpioPad belongs to
        ('PadCfgLockTx',    c_uint32), # PADCFGLOCKTX reg value of the group GpioPad belongs to
        ('EdsPadCfgDw0',    c_uint32),
        ('EdsPadCfgDw1',    c_uint32),
    ]

# EDS GPIO PAD CFG DW0
class EDS_DW0 (Union):
    class DW0_TMPL (Structure):
        _pack_ = 1
        _fields_ = [
            ('GPIOTXSTATE'      , c_uint32, 1),
            ('GPIORXSTATE'      , c_uint32, 1),
            ('Rsvd1'            , c_uint32, 6),
            ('GPIOTXDIS'        , c_uint32, 1),
            ('GPIORXDIS'        , c_uint32, 1),
            ('PMODE'            , c_uint32, 2),
            ('Rsvd2'            , c_uint32, 5),
            ('GPIROUTNMI'       , c_uint32, 1),
            ('GPIROUTSMI'       , c_uint32, 1),
            ('GPIROUTSCI'       , c_uint32, 1),
            ('GPIROUTIOXAPIC'   , c_uint32, 1),
            ('RXTXENCFG'        , c_uint32, 2),
            ('RXINV'            , c_uint32, 1),
            ('PREGFRXSEL'       , c_uint32, 1),
            ('RXEVCFG'          , c_uint32, 2),
            ('Rsvd3'            , c_uint32, 1),
            ('RXRAW1'           , c_uint32, 1),
            ('RXPADSTSEL'       , c_uint32, 1),
            ('PADRSTCFG'        , c_uint32, 2),
        ]
    _pack = 1
    _fields_ = [
        ('Dw0',     c_uint32),
        ('Dw0Tmpl', DW0_TMPL),
    ]

# EDS GPIO PAD CFG DW1
class EDS_DW1 (Union):
    class DW1_TMPL (Structure):
        _pack_ = 1
        _fields_ = [
            ('INTSEL'   , c_uint32, 8),
            ('Rsvd1'    , c_uint32, 2),
            ('TERM'     , c_uint32, 4),
            ('Rsvd2'    , c_uint32, 18),
        ]
    _pack = 1
    _fields_ = [
        ('Dw1',     c_uint32),
        ('Dw1Tmpl', DW1_TMPL),
    ]

# SBL's GPIO config dword0
class SBL_DW0 (Union):
    class DW0_TMPL (Structure):
        _pack_ = 1
        _fields_ = [
            ('PadMode',         c_uint32,   5),
            ('HostSoftPadOwn',  c_uint32,   2),
            ('Direction',       c_uint32,   6),
            ('OutputState',     c_uint32,   2),
            ('InterruptConfig', c_uint32,   9),
            ('PowerConfig',     c_uint32,   8),
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
            ('ElectricalConfig',c_uint32,   9),
            ('LockConfig',      c_uint32,   4),
            ('OtherSettings',   c_uint32,   3),
            ('PadNum',          c_uint32,   8),
            ('GrpIdx',          c_uint32,   5),
            ('Rsvd',            c_uint32,   1),
            ('HideBit',         c_uint32,   1),
            ('GpioSkip',        c_uint32,   1),
        ]
    _pack_ = 1
    _fields_ = [
        ('Dw1',     c_uint32),
        ('Dw1Tmpl', DW1_TMPL),
    ]

#
# Formats end
#

#
# Misc global begin
#

# GROUP info for GPIO PADS
#
# If vir_to_phy_grp = False, SBL's config has A->0, B->1 etc. mapping.
# And GpioSiLib.c or GpioInitLib.c corresponding libraries will map this
# virtual group #s to real physical group #s (if they are not same).
#
# If vir_to_phy_grp = True, SBL's config has A->G1, B->G2 etc. physical
# mapping directly, so the GpioLib library uses this directly.
#
grp_info_sbl = {
    # Grp     Index
    'GPP_A' : [ 0x0],
    'GPP_B' : [ 0x1],
    'GPP_C' : [ 0x2],
    'GPP_D' : [ 0x3],
    'GPP_E' : [ 0x4],
    'GPP_F' : [ 0x5],
    'GPP_G' : [ 0x6],
    'GPP_H' : [ 0x7],
    'GPP_I' : [ 0x8],
    'GPP_J' : [ 0x9],
    'GPP_K' : [ 0xA],
    'GPP_L' : [ 0xB],
    'GPP_M' : [ 0xC],
    'GPP_N' : [ 0xD],
    'GPP_O' : [ 0xE],
    'GPP_P' : [ 0xF],
    'GPP_Q' : [ 0x10],
    'GPP_R' : [ 0x11],
    'GPP_S' : [ 0x12],
    'GPP_T' : [ 0x13],
    'GPP_U' : [ 0x14],
    'GPP_V' : [ 0x15],
    'GPP_W' : [ 0x16],
    'GPP_X' : [ 0x17],
    'GPP_Y' : [ 0x18],
    'GPP_Z' : [ 0x19],
}

# global dicts
yml_dict = {}
dlt_dict = {}
h_dict   = {}
csv_dict = {}
txt_dict = {}

own_dict = {}
lock_dict= {}
locktx_dict={}

# Reset Cfg mapping info
GpioResetDefault    = 0x00
GpioResumeReset     = 0x01
GpioHostDeepReset   = 0x03
GpioPlatformReset   = 0x05
GpioDswReset        = 0x07

V_GPIO_PCR_RST_CONF_POW_GOOD    = 0x00
V_GPIO_PCR_RST_CONF_DEEP_RST    = 0x01
V_GPIO_PCR_RST_CONF_GPIO_RST    = 0x02
V_GPIO_PCR_RST_CONF_RESUME_RST  = 0x03

GPP_RST_CFG_EDS_TO_SBL = {
    0x0 : GpioResumeReset,
    0x1 : GpioHostDeepReset,
    0x2 : GpioPlatformReset,
}

GPP_RST_CFG_SBL_TO_EDS = {
    GpioResetDefault    : 0x0,
    GpioHostDeepReset   : V_GPIO_PCR_RST_CONF_DEEP_RST,
    GpioPlatformReset   : V_GPIO_PCR_RST_CONF_GPIO_RST,
    GpioResumeReset     : V_GPIO_PCR_RST_CONF_POW_GOOD,
}

# Sbl Dw fields mask & offset
SBL_DW0_DIR_RXINV_OFF   = 0x3
SBL_DW0_DIR_RXINV_MASK  = 0x18

SBL_DW0_INTCFG_TYPE_OFF = 0x5
SBL_DW0_INTCFG_TYPE_MASK= 0xE0
SBL_DW0_INTCFG_SRC_MASK = 0x1F

SBL_DW0_DIR_TXRX_MASK   = 0x7
SBL_DW0_OUT_STATE_MASK  = 0x3
SBL_DW0_PMODE_MASK      = 0xF
SBL_DW1_ELEC_CFG_MASK   = 0x1F
SBL_DW1_OTHER_RXRAW_MASK= 0x3
#
# Misc global end
#

#
# GPIO convert helpers start
#

# Verify if the inp_fmt/out_fmt are valid
def validate_args (inp_fmt, out_fmt):
    valid = True
    lines = open (inp_fmt, 'r').readlines()

    if ( inp_fmt.split('.')[1] in ['h', 'csv', 'txt'] and out_fmt not in ['yaml', 'dlt'] ):
        raise Exception ("Unmatched inp and out formats!")

    if ( inp_fmt.split('.')[1] in ['yaml', 'dlt'] and out_fmt not in ['h', 'csv', 'txt'] ):
        raise Exception ("Unmatched inp and out formats!")

    if inp_fmt.endswith ('.h'):
        valid = False
        for line in lines:
            if line.strip().startswith ('static GPIO_INIT_CONFIG'):
                valid = True
                break
        if not valid:
            raise Exception ("Invalid .h format")
    elif inp_fmt.endswith ('.csv'):
        for line in lines:
            if line.startswith('#'):
                continue
            parts = line.strip().split(',')
            while not parts[-1]:
                del(parts[-1])
            if not len(parts) in [9, 10]:
                raise Exception ("Not enough csv data!")
    elif inp_fmt.endswith ('.txt'):
       for line in lines:
            if len (line.split(':')) != 6:
                raise Exception ("Invalid txt input format!")
    elif inp_fmt.endswith ('.yaml'):
        valid = True
    elif inp_fmt.endswith ('.dlt'):
        valid = True
    else:
        raise Exception ("Invalid input type!")

    return valid

# Ex : convert GPP_A7 to GPP_A07 to match GPP_A23 etc.
def normalize_pad_name (Name):
    Match = re.match('([a-zA-Z_]+)(\d+)$', Name)
    if Match:
        if len(Match.group(2)) == 1:
            Name = Match.group(1) + '0' + Match.group(2)
    return Name

# Convert the i/p lines and get separated parts
def get_parts_from_inp (inp_fmt, line, next_line):
    parts = []
    if inp_fmt.endswith ('.h') or inp_fmt.endswith ('.csv'):
        line = line.replace('{', ' ').replace('}', ' ').replace(' ', '').rstrip('\n')
        if not line.startswith ('//'):
            parts = line.split(',')
    elif inp_fmt.endswith ('.txt'):
        parts = line.split(':')
    elif inp_fmt.endswith ('.yaml'):
        if line.lstrip().startswith (YML_LINE_HDR):
            line = line.lstrip()[len(YML_LINE_HDR):]
            line = line.replace(' ', '').rstrip(' ] }\n')
            parts = line.split(',')
            if len(parts) == 4:
                # Convert to standard 3 elements:
                # GROUP, PIN, DWORD0, DWORD1 ==> GROUP_PIN, DWORD0, DWORD1
                parts = [''.join(parts[0:2]), parts[2], parts[3]]
    elif inp_fmt.endswith ('dlt'):
        if line.startswith (DLT_CFG0_HDR):
            line = line.replace(' ', '').rstrip('\n')
            parts = line.split('|')
            next_line = next_line.replace(' ', '').rstrip('\n')
            parts += next_line.split('|')
    else:
        parts = []

    return parts

# Calculate 1's complement for an int of size num_bits
def ones_complement (num, num_bits):
    return ((1 << num_bits) - 1) ^ (num)

# Get the field length and offset given the structure
def get_field_len_offset (efield, efields):
    f_len = 0
    f_offset = 0

    for field in efields:
        if field[0] == efield:
            f_len = field[2]
            break
        else:
            f_offset += field[2]

    return f_len, f_offset

# Get SBL DW fields from EDS DW field
def get_field_from_eds_dw (dw, e_field, which_dw, bit_pos):
    f_offset = 0
    f_len = 0
    f_mask = 0

    if which_dw == 0:
        efields = EDS_DW0.DW0_TMPL._fields_
    else:
        efields = EDS_DW1.DW1_TMPL._fields_

    f_len, f_offset = get_field_len_offset (e_field, efields)

    if f_len == 0:
        raise Exception ("Invalid field len %X" % f_len)

    if e_field == 'GPIOTXDIS': # Get both TXDIS and RXDIS
        f_len = 2
    if e_field == 'GPIROUTNMI': # Get NMI, SMI, SCI, APIC together
        f_len = 4

    for i in range(f_len):
        f_mask |= 1 << (f_offset + i)

    if f_offset == 0:
        f_val = ((dw & f_mask) << (f_offset + (bit_pos + 1))) | (1 << bit_pos)
    else:
        f_val = ((dw & f_mask) >> (f_offset - (bit_pos + 1))) | (1 << bit_pos)

    return f_val

# Get the EDS DW field from SBL field
def get_field_from_sbl_dw (s_in, s_mask, bit_pos, e_field, which_dw):
    e_len = 0
    e_offset = 0

    if which_dw == 0:
        efields = EDS_DW0.DW0_TMPL._fields_
    else:
        efields = EDS_DW1.DW1_TMPL._fields_

    e_len, e_offset = get_field_len_offset (e_field, efields)

    e_val = ((s_in & s_mask) >> (bit_pos + 1)) << (e_offset)

    return e_val

#
# GPIO convert helpers end
#

#
# [h, csv, txt] to [yaml, dlt] start
#

# convert i/p from .h and .csv to SBL GPIO CFG DWs
def get_sbl_dws_from_h_csv (parts, pad_name, sbl_dw0, sbl_dw1, gpio_cfg_file):
    # SBL Cfg Dw0
    sbl_dw0.Dw0Tmpl.PadMode         = GPIO_PAD_MODE[parts[1]]
    sbl_dw0.Dw0Tmpl.HostSoftPadOwn  = GPIO_HOSTSW_OWN[parts[2]]
    sbl_dw0.Dw0Tmpl.Direction       = GPIO_DIRECTION[parts[3]]
    sbl_dw0.Dw0Tmpl.OutputState     = GPIO_OUTPUT_STATE[parts[4]]
    for key in parts[5].split('|'):
        sbl_dw0.Dw0Tmpl.InterruptConfig |= GPIO_INT_CONFIG[key]
    sbl_dw0.Dw0Tmpl.PowerConfig     = GPIO_RESET_CONFIG[parts[6]]

    # SBL Cfg Dw1
    sbl_dw1.Dw1Tmpl.ElectricalConfig= GPIO_ELECTRICAL_CONFIG[parts[7]]
    if len(parts) > 8:
        if parts[8].startswith('Gpio'):
            sbl_dw1.Dw1Tmpl.LockConfig  = GPIO_LOCK_CONFIG[parts[8]]
    else:
        sbl_dw1.Dw1Tmpl.LockConfig  = GPIO_LOCK_CONFIG['GpioLockDefault']

    if len(parts) > 9:
        if parts[9].startswith('Gpio'):
            sbl_dw1.Dw1Tmpl.OtherSettings  = GPIO_OTHER_CONFIG[parts[9]]

    sbl_dw1.Dw1Tmpl.PadNum          = int (pad_name[5:7])
    if gpio_cfg_file.vir_to_phy_grp() == True:
        sbl_dw1.Dw1Tmpl.GrpIdx      = gpio_cfg_file.get_grp_info(V_PCH_SERIES)[pad_name[0:5]][0]
    else:
        sbl_dw1.Dw1Tmpl.GrpIdx      = grp_info_sbl[pad_name[0:5]][0]

# Map the reset (power) cfg from eds dw to sbl dw
def get_reset_cfg_from_eds (eDw0):
    f_mask = 0
    f_len, f_offset = get_field_len_offset ('PADRSTCFG', EDS_DW0.DW0_TMPL._fields_)

    for i in range(f_len):
        f_mask |= 1 << (f_offset + i)

    rst_cfg = (eDw0 & f_mask) >> f_offset
    if rst_cfg < 3:
        rst_cfg = GPP_RST_CFG_EDS_TO_SBL[rst_cfg]
    else:
        rst_cfg = GpioResetDefault

    return rst_cfg

# Convert i/p from .txt (GPIO PAD CFG register (EDS) values) to SBL GPIO CFG DWs
def get_sbl_dws_from_txt (parts, pad_name, sbl_dw0, sbl_dw1, gpio_cfg_file):
    eds_dw0 = EDS_DW0 ()
    eds_dw1 = EDS_DW1 ()

    padnum      = int (pad_name[5:7])
    padnumpos   = padnum % 32
    hostswown   = int (parts[1], 0)
    padcfglock  = int (parts[2], 0)
    padcfglocktx= int (parts[3], 0)
    eds_dw0.Dw0 = int (parts[4], 0)
    eds_dw1.Dw1 = int (parts[5], 0)

    sbl_dw0.Dw0Tmpl.PadMode         =   get_field_from_eds_dw (eds_dw0.Dw0, 'PMODE',        0, 0)
    sbl_dw0.Dw0Tmpl.HostSoftPadOwn  =   (((hostswown >> padnumpos) & 0x1) << 1) | 0x1
    sbl_dw0.Dw0Tmpl.Direction       =   get_field_from_eds_dw (eds_dw0.Dw0, 'GPIOTXDIS',    0, 0)
    if (eds_dw0.Dw0 & 0x200) == 0:  # check RXDIS, BIT9
        sbl_dw0.Dw0Tmpl.Direction   |=  get_field_from_eds_dw (eds_dw0.Dw0, 'RXINV',        0, SBL_DW0_DIR_RXINV_OFF)
    sbl_dw0.Dw0Tmpl.OutputState     =   get_field_from_eds_dw (eds_dw0.Dw0, 'GPIOTXSTATE',  0, 0)
    sbl_dw0.Dw0Tmpl.InterruptConfig =   get_field_from_eds_dw (eds_dw0.Dw0, 'RXEVCFG',      0, SBL_DW0_INTCFG_TYPE_OFF)
    sbl_dw0.Dw0Tmpl.InterruptConfig |=  get_field_from_eds_dw (eds_dw0.Dw0, 'GPIROUTNMI',   0, 0)
    sbl_dw0.Dw0Tmpl.PowerConfig     =   get_reset_cfg_from_eds (eds_dw0.Dw0)
    sbl_dw1.Dw1Tmpl.ElectricalConfig=   get_field_from_eds_dw (eds_dw1.Dw1, 'TERM',         1, 0)
    sbl_dw1.Dw1Tmpl.LockConfig      =   ((not((padcfglock >> padnumpos) & 0x1)) << 1) | 0x1
    sbl_dw1.Dw1Tmpl.LockConfig      |=  ((not((padcfglocktx >> padnumpos) & 0x1)) << 2) | 0x1
    if (gpio_cfg_file.rxraw_override_cfg()):
        sbl_dw1.Dw1Tmpl.OtherSettings=  get_field_from_eds_dw (eds_dw0.Dw0, 'RXRAW1',       0, 0)
    sbl_dw1.Dw1Tmpl.PadNum          =   int (pad_name[5:7])
    if gpio_cfg_file.vir_to_phy_grp() == True:
        sbl_dw1.Dw1Tmpl.GrpIdx      = gpio_cfg_file.get_grp_info(V_PCH_SERIES)[pad_name[0:5]][0]
    else:
        sbl_dw1.Dw1Tmpl.GrpIdx      = grp_info_sbl[pad_name[0:5]][0]

# Calucalte the SBL Config Dws from '.h' or '.csv' or '.txt' data
def get_sbl_dws (inp_fmt, cfg_file, parts):
    sbl_dw0 = SBL_DW0 ()
    sbl_dw1 = SBL_DW1 ()

    if not 'GPP_' in parts[0]:
        return '', 0x0, 0x0
    splitdata = parts[0].split('_')
    pin_number = splitdata[len(splitdata)-1]
    pin_number = normalize_pad_name (pin_number)
    pad_name = 'GPP_' + pin_number

    gpio_cfg_file = SourceFileLoader ('GenGpioDataConfig', cfg_file).load_module()

    if inp_fmt.endswith ('.h') or inp_fmt.endswith ('.csv'):
        get_sbl_dws_from_h_csv (parts, pad_name, sbl_dw0, sbl_dw1, gpio_cfg_file)
    elif inp_fmt.endswith ('.txt'):
        get_sbl_dws_from_txt (parts, pad_name, sbl_dw0, sbl_dw1, gpio_cfg_file)
    else:
        return '', 0x0, 0x0

    return pad_name, sbl_dw0.Dw0, sbl_dw1.Dw1

#
# [h, csv, txt] to [yaml, dlt] end
#

#
# [yaml, dlt] to [h, csv, txt] start
#

# Get .h, .csv line from SBL Config DWs
def get_h_csv_from_sbl_dws (sbl_dw0, sbl_dw1, gpio_cfg_file):
    PadMode         = list(GPIO_PAD_MODE.keys())[list(GPIO_PAD_MODE.values()).index(sbl_dw0.Dw0Tmpl.PadMode)]
    HostSoftPadOwn  = list(GPIO_HOSTSW_OWN.keys())[list(GPIO_HOSTSW_OWN.values()).index(sbl_dw0.Dw0Tmpl.HostSoftPadOwn)]
    Direction       = list(GPIO_DIRECTION.keys())[list(GPIO_DIRECTION.values()).index(sbl_dw0.Dw0Tmpl.Direction)]
    OutputState     = list(GPIO_OUTPUT_STATE.keys())[list(GPIO_OUTPUT_STATE.values()).index(sbl_dw0.Dw0Tmpl.OutputState)]
    if sbl_dw0.Dw0Tmpl.InterruptConfig <= 0x1F:
        IntConfig   = list(GPIO_INT_CONFIG.keys())[list(GPIO_INT_CONFIG.values()).index(sbl_dw0.Dw0Tmpl.InterruptConfig)]
    else:
        int_cfg_1   = sbl_dw0.Dw0Tmpl.InterruptConfig & 0x1F
        int_cfg_2   = sbl_dw0.Dw0Tmpl.InterruptConfig & (~0x1F)
        IntConfig   =  list(GPIO_INT_CONFIG.keys())[list(GPIO_INT_CONFIG.values()).index(int_cfg_1)] + '|'
        IntConfig  +=  list(GPIO_INT_CONFIG.keys())[list(GPIO_INT_CONFIG.values()).index(int_cfg_2)]
    PowerConfig     = list(GPIO_RESET_CONFIG.keys())[list(GPIO_RESET_CONFIG.values()).index(sbl_dw0.Dw0Tmpl.PowerConfig)]

    ElecConfig      = list(GPIO_ELECTRICAL_CONFIG.keys())[list(GPIO_ELECTRICAL_CONFIG.values()).index(sbl_dw1.Dw1Tmpl.ElectricalConfig)]
    LockConfig      = list(GPIO_LOCK_CONFIG.keys())[list(GPIO_LOCK_CONFIG.values()).index(sbl_dw1.Dw1Tmpl.LockConfig)]
    if (gpio_cfg_file.rxraw_override_cfg()):
        OtherSettings= list(GPIO_OTHER_CONFIG.keys())[list(GPIO_OTHER_CONFIG.values()).index(sbl_dw1.Dw1Tmpl.OtherSettings)]
    else:
        OtherSettings= ''

    h_csv_line = "%s, %s, %s, %s, %s, %s, %s, %s, %s" % \
                (PadMode, HostSoftPadOwn, Direction, OutputState, IntConfig, PowerConfig, ElecConfig, LockConfig, OtherSettings)

    return h_csv_line

# patch the hostsodtown, lock/locktx values
# in .txt file for each pin
def patch_own_lock_txt ():
    for txt_key in txt_dict:
        txt_line = txt_dict[txt_key]
        txt_parts = txt_line.split(':')
        txt_parts[1] = "0x%08X" % own_dict[txt_key[0:5]]
        txt_parts[2] = "0x%08X" % lock_dict[txt_key[0:5]]
        txt_parts[3] = "0x%08X" % locktx_dict[txt_key[0:5]]
        txt_line = ""
        for i in range(len(txt_parts)):
            txt_line += txt_parts[i] + ':'
        txt_dict[txt_key] = txt_line.rstrip(':')

# Map the reset (power) cfg from sbl dw to eds dw
def get_reset_cfg_from_sbl (rst_cfg):
    return GPP_RST_CFG_SBL_TO_EDS[rst_cfg]

# Get .txt line from SBL Config DWs
def get_txt_from_sbl_dws (sbl_dw0, sbl_dw1, pad_name, gpio_cfg_file):
    eds_dw0 = EDS_DW0 ()
    eds_dw1 = EDS_DW1 ()

    eds_dw0.Dw0 = 0x0
    eds_dw1.Dw1 = 0x0

    f_len, f_offset = get_field_len_offset ('PADRSTCFG', EDS_DW0.DW0_TMPL._fields_)
    eds_dw0.Dw0 |=  ((get_reset_cfg_from_sbl (sbl_dw0.Dw0Tmpl.PowerConfig)) << f_offset)
    eds_dw0.Dw0 |=  get_field_from_sbl_dw (sbl_dw0.Dw0Tmpl.InterruptConfig, SBL_DW0_INTCFG_TYPE_MASK, SBL_DW0_INTCFG_TYPE_OFF,  'RXEVCFG',     0)
    eds_dw0.Dw0 |=  get_field_from_sbl_dw (sbl_dw0.Dw0Tmpl.InterruptConfig, SBL_DW0_INTCFG_SRC_MASK,                        0,  'GPIROUTNMI',  0)
    eds_dw0.Dw0 |=  get_field_from_sbl_dw (sbl_dw0.Dw0Tmpl.Direction,       SBL_DW0_DIR_TXRX_MASK,                          0,  'GPIOTXDIS',   0)
    eds_dw0.Dw0 |=  get_field_from_sbl_dw (sbl_dw0.Dw0Tmpl.Direction,       SBL_DW0_DIR_RXINV_MASK,    SBL_DW0_DIR_RXINV_OFF,   'RXINV',       0)
    eds_dw0.Dw0 |=  get_field_from_sbl_dw (sbl_dw0.Dw0Tmpl.OutputState,     SBL_DW0_OUT_STATE_MASK,                         0,  'GPIOTXSTATE', 0)
    eds_dw0.Dw0 |=  get_field_from_sbl_dw (sbl_dw0.Dw0Tmpl.PadMode,         SBL_DW0_PMODE_MASK,                             0,  'PMODE',       0)
    if (gpio_cfg_file.rxraw_override_cfg()):
        eds_dw0.Dw0 |= get_field_from_sbl_dw (sbl_dw1.Dw1Tmpl.OtherSettings,    SBL_DW1_OTHER_RXRAW_MASK,                       0,  'RXRAW1',      0)
    eds_dw1.Dw1 |=  get_field_from_sbl_dw (sbl_dw1.Dw1Tmpl.ElectricalConfig,SBL_DW1_ELEC_CFG_MASK,                          0,  'TERM',        1)

    padnum      = int (pad_name[5:7])
    padnumpos   = padnum % 32
    hsown       = (sbl_dw0.Dw0Tmpl.HostSoftPadOwn >> 1) << padnumpos
    lockcfg     = ones_complement ( ((sbl_dw1.Dw1Tmpl.LockConfig >> 1) & 0x1), 1 ) << padnumpos
    lockcfgtx   = ones_complement ( ((sbl_dw1.Dw1Tmpl.LockConfig >> 2) & 0x1), 1 ) << padnumpos

    if pad_name[0:5] in own_dict.keys():
        own_dict[pad_name[0:5]]     |= hsown
        lock_dict[pad_name[0:5]]    |= lockcfg
        locktx_dict[pad_name[0:5]]  |= lockcfgtx
    else:
        own_dict[pad_name[0:5]]     = hsown
        lock_dict[pad_name[0:5]]    = lockcfg
        locktx_dict[pad_name[0:5]]  = lockcfgtx

    txt_line = "0x%08X:0x%08X:0x%08X:0x%08X:0x%08X" % (hsown, lockcfg, lockcfgtx, eds_dw0.Dw0, eds_dw1.Dw1)

    return txt_line

# Convert SBL Config DWs to .h, .csv, .txt fomrat
def parse_sbl_dws (inp_fmt, out_fmt, cfg_file, parts):
    sbl_dw0 = SBL_DW0 ()
    sbl_dw1 = SBL_DW1 ()

    out_line = ''

    if inp_fmt.endswith ('.yaml'):
        sbl_dw0.Dw0 = int (parts[1], 0)
        sbl_dw1.Dw1 = int (parts[2], 0)
        pad_name = parts[0][-7:]
    elif inp_fmt.endswith ('.dlt'):
        sbl_dw0.Dw0 = int (parts[1], 0)
        sbl_dw1.Dw1 = int (parts[3], 0)
        Match = re.match('(%s)' % DLT_CFGx_HDR + '(\d+_)(.*)$', parts[0])
        if Match:
            pad_name = Match.group(3)
        else:
            pad_name = parts[0][-7:]

    pad_name = normalize_pad_name (pad_name)
    if not pad_name.startswith('GPP_'):
        return '', ''

    gpio_cfg_file = SourceFileLoader ('GenGpioDataConfig', cfg_file).load_module()

    if out_fmt == 'h' or out_fmt == 'csv':
        out_line = get_h_csv_from_sbl_dws (sbl_dw0, sbl_dw1, gpio_cfg_file)
    elif out_fmt == 'txt':
        out_line = get_txt_from_sbl_dws (sbl_dw0, sbl_dw1, pad_name, gpio_cfg_file)
    else:
        out_line = ''

    return pad_name, out_line

#
# [yaml, dlt] to [h, csv, txt] end
#

#
# Main helpers start
#

# Convert and populate the global dictionaries
def convert_from_inp_to_out (gpio_tmp_fmt, inp_fmt, cfg_file, out_fmt, parts):
    if inp_fmt.endswith ('.h') or inp_fmt.endswith ('.csv') or inp_fmt.endswith ('.txt'):
        pad_name, dw0, dw1 = get_sbl_dws (inp_fmt, cfg_file, parts)
        if pad_name != '':
            if out_fmt == 'yaml':
                if gpio_tmp_fmt == 'new':
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
    elif inp_fmt.endswith ('.yaml') or inp_fmt.endswith ('.dlt'):
        pad_name, out_line = parse_sbl_dws (inp_fmt, out_fmt, cfg_file, parts)
        if pad_name != '':
            if out_fmt == 'h':
                h_line = "{" + PAD_NAME_HDR + pad_name + ", { " + out_line + "  }},\n"
                h_dict[pad_name] = h_line
            elif out_fmt == 'csv':
                csv_line = PAD_NAME_HDR + pad_name + ', ' + out_line + '\n'
                csv_dict[pad_name] = csv_line
            elif out_fmt == 'txt':
                txt_line = pad_name + ':' + out_line + '\n'
                txt_dict[pad_name] = txt_line
            else:
                print('Invalid out type')

# Gen output file in the required format
def gen_file_from_object (file, object):
    open (file, 'wb').write(object)

# Driver function for conversion between formats
def gpio_convert (args):
    # Validate input/output
    valid = validate_args (args.inp_fmt, args.out_fmt)
    if not valid:
        raise Exception ("Invalid input '.%s'" % args.inp_fmt.split('.')[1])

    # Read file input
    fp = open (args.inp_fmt, 'r')
    lines = fp.readlines()

    # Call driver to convert
    if args.inp_fmt:
        for idx, line in enumerate(lines):
            if idx < len(lines) - 1:
                parts = get_parts_from_inp (args.inp_fmt, line, lines[idx + 1])
            else:
                parts = get_parts_from_inp (args.inp_fmt, line, '')
            if len(parts) == 0:
                continue
            convert_from_inp_to_out (args.gpio_tmp_fmt, args.inp_fmt, args.cfg_file, args.out_fmt, parts)

    # Copy the final data to ouput
    sbl_data = ""
    if args.out_fmt == 'yaml':
        for key in sorted(yml_dict):
            sbl_data += yml_dict[key] + '\n'
    elif args.out_fmt == 'dlt':
        for key in sorted(dlt_dict):
            sbl_data += dlt_dict[key] + '\n'
    elif args.out_fmt == 'h':
        sbl_data += H_STRUCT_HDR
        for key in sorted(h_dict):
            sbl_data += h_dict[key]
        sbl_data += H_STRUCT_FTR
    elif args.out_fmt == 'csv':
        sbl_data += CSV_FILE_HDR
        for key in sorted(csv_dict):
            sbl_data += csv_dict[key]
    else:
        patch_own_lock_txt ()
        for key in sorted(txt_dict):
            sbl_data += txt_dict[key]

    fp.close ()

    # Copy sbl content into the required out file
    if os.path.isdir(args.out_path):
        out_dir = args.out_path
        out_file = 'GpioOut' + ".%s" % args.out_fmt
    else:
        out_dir = os.path.dirname(args.out_path)
        out_file = os.path.basename(args.out_path)
    out_path = os.path.join (out_dir, out_file)
    gen_file_from_object (out_path, sbl_data.encode('utf-8'))

#
# Main helpers end
#

# Main
def main ():

    global V_PCH_SERIES

    ap = argparse.ArgumentParser()
    ap.add_argument(  '-if',
                        dest='inp_fmt',
                        type=str,
                        required=True,
                        help='Input data file, must have [yaml, dlt] or [h , csv, txt] file extension')
    ap.add_argument(  '-cf',
                        dest='cfg_file',
                        type=str,
                        required=True,
                        help='Config file containing inputs like Group Info(dict with name & index in GPIO_GROUP_INFO in Gpio Lib code)')
    ap.add_argument(  '-of',
                        dest='out_fmt',
                        choices=['yaml', 'dlt', 'h', 'csv', 'txt'],
                        type=str,
                        required=True,
                        help='Output SBL format, either [h, csv, txt] or [yaml, dlt]')
    ap.add_argument(  '-o',
                        dest='out_path',
                        default='.',
                        type=str,
                        help='Output directory/file')
    ap.add_argument(  '-t',
                        dest='gpio_tmp_fmt',
                        choices=['old', 'new'],
                        default='old',
                        type=str,
                        help='Determine the GPIO template format. For new platforms, please use new format.')
    ap.add_argument(  '-p',
                        dest='pch_series',
                        choices=['def', 'h', 'lp', 's', 'p'],
                        default='def',
                        type=str,
                        help='PCH series to get the correct group info')

    ap.set_defaults(func=gpio_convert)

    args = ap.parse_args()

    gpio_cfg_file = SourceFileLoader ('GenGpioDataConfig', args.cfg_file).load_module()
    if (gpio_cfg_file.plat_name() == 'apl'):
        raise Exception ('Platform unsupported')

    V_PCH_SERIES = args.pch_series

    args.func(args)

if __name__ == '__main__':
    sys.exit(main())
