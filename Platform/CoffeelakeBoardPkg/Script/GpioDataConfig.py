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
    'GPP_A' : [ 0x0],
    'GPP_B' : [ 0x1],
    'GPP_C' : [ 0x2],
    'GPP_D' : [ 0x3],
    'GPP_E' : [ 0x4],
    'GPP_F' : [ 0x5],
    'GPP_G' : [ 0x6],
    'GPP_H' : [ 0x7],
}

grp_info_h = {
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
    return 'cfl'
