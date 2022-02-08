## @ GpioDataConfig.py
#  This is a Gpio config script for Slim Bootloader
#
# Copyright (c) 2020, Intel Corporation. All rights reserved. <BR>
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
    'GPP_S' : [ 0x5],
    'GPP_H' : [ 0x6],
    'GPP_D' : [ 0x7],
    'GPP_U' : [ 0x8],
    'GPP_C' : [ 0xA],
    'GPP_F' : [ 0xB],
    'GPP_E' : [ 0xC],
}

grp_info_h = {
    # Grp     Index
    'GPP_A' : [ 0x1],
    'GPP_R' : [ 0x2],
    'GPP_B' : [ 0x3],
    'GPP_D' : [ 0x4],
    'GPP_C' : [ 0x5],
    'GPP_S' : [ 0x6],
    'GPP_G' : [ 0x7],
    'GPP_E' : [ 0x9],
    'GPP_F' : [ 0xA],
    'GPP_H' : [ 0xB],
    'GPP_K' : [ 0xC],
    'GPP_J' : [ 0xD],
    'GPP_I' : [ 0xE],
}


def get_grp_info(pch_series):
    if pch_series not in ['lp', 'h']:
        raise Exception ('Invalid pch series passed')
    else:
        if pch_series == 'lp':
            return grp_info_lp
        elif pch_series == 'h':
            return grp_info_h

def rxraw_override_cfg():
    return False

def vir_to_phy_grp():
    return False

def plat_name():
    return 'tgl'
