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

grp_info_def ={
    # Grp     Index
    'GPP_A' : [ 0xB],
    'GPP_B' : [ 0x0],
    'GPP_C' : [ 0xD],
    'GPP_D' : [ 0x5],
    'GPP_E' : [ 0x10],
    'GPP_F' : [ 0xE],
    'GPP_G' : [ 0x2],
    'GPP_H' : [ 0x4],
    'GPP_R' : [ 0x12],
    'GPP_S' : [ 0xA],
    'GPP_T' : [ 0x1],
    'GPP_U' : [ 0x6],
    'GPP_V' : [ 0x3],
    'GPPD'  : [ 0x8],
}

def get_grp_info(pch_series):
    return grp_info_def

def rxraw_override_cfg():
    return True

def vir_to_phy_grp():
    return True

def plat_name():
    return 'ehl'
