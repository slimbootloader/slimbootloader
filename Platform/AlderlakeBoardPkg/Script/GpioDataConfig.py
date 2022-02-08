## @ GpioDataConfig.py
#  This is a Gpio config script for Slim Bootloader
#
# Copyright (c) 2022, Intel Corporation. All rights reserved. <BR>
# SPDX-License-Identifier: BSD-2-Clause-Patent
#
##


#
# The index for a group has to match implementation
# in a platform specific Gpio library.
#

grp_info_lp = {
    # Grp     Index
    'GPP_B' : [ 0x0],
    'GPP_T' : [ 0x1],
    'GPP_A' : [ 0x2],
    'GPP_R' : [ 0x3],
    'GPP_S' : [ 0x6],
    'GPP_H' : [ 0x7],
    'GPP_D' : [ 0x8],
    'GPP_U' : [ 0x9],
    'GPP_C' : [ 0xB],
    'GPP_F' : [ 0xC],
    'GPP_E' : [ 0xE],
}

grp_info_s = {
    # Grp     Index
    'GPP_I' : [ 0x0],
    'GPP_R' : [ 0x1],
    'GPP_J' : [ 0x2],
    'GPP_B' : [ 0x5],
    'GPP_G' : [ 0x6],
    'GPP_H' : [ 0x7],
    'GPP_A' : [ 0xA],
    'GPP_C' : [ 0xB],
    'GPP_S' : [ 0xD],
    'GPP_E' : [ 0xE],
    'GPP_K' : [ 0xF],
    'GPP_F' : [ 0x10],
    'GPP_D' : [ 0x11],
}

def get_grp_info(pch_series):
    if pch_series not in ['s', 'p']:
        raise Exception ('Invalid pch series passed')
    else:
        if pch_series == 's':
            return grp_info_s
        elif pch_series == 'p':
            return grp_info_lp

def rxraw_override_cfg():
    return True

def vir_to_phy_grp():
    return True

def plat_name():
    return 'adl'
