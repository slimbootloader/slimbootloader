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

# mPchLpGpioGroupInfo in Silicon/AlderlakePkg/Library/GpioSiLib/GpioSiLib.c
grp_info_lp = {
    # Grp     Index
    'GPP_B' : [ 0x0],
    'GPP_T' : [ 0x1],
    'GPP_A' : [ 0x2],
    'GPP_R' : [ 0x3],
    # 'SPI' : [ 0x4],
    'GPD'   : [ 0x5],
    'GPP_S' : [ 0x6],
    'GPP_H' : [ 0x7],
    'GPP_D' : [ 0x8],
    'GPP_U' : [ 0x9],
    # 'VGPIO' : [ 0xA],
    'GPP_C' : [ 0xB],
    'GPP_F' : [ 0xC],
    # 'HVCMOS' : [ 0xD],
    'GPP_E' : [ 0xE],
}

# mPchNGpioGroupInfo in Silicon/AlderlakePkg/Library/GpioSiLib/GpioSiLib.c
grp_info_n = {
    # Grp     Index
    'GPP_B' : [ 0x0],
    'GPP_T' : [ 0x1],
    'GPP_A' : [ 0x2],
    'GPP_R' : [ 0x3],
    # 'SPI' : [ 0x4],
    'GPD'   : [ 0x5],
    'GPP_S' : [ 0x6],
    'GPP_H' : [ 0x7],
    'GPP_D' : [ 0x8],
    'GPP_I' : [ 0x9],
    # 'VGPIO' : [ 0xA],
    'GPP_C' : [ 0xB],
    'GPP_F' : [ 0xC],
    # 'HVCMOS' : [ 0xD],
    'GPP_E' : [ 0xE],
}

# mAdlPchSGpioGroupInfo in ./Silicon/AlderlakePkg/Library/GpioSiLib/GpioSiLib.c
grp_info_s = {
    # Grp     Index
    'GPP_I' : [ 0x0],
    'GPP_R' : [ 0x1],
    'GPP_J' : [ 0x2],
    # 'VGPIO' : [ 0x3],
    # 'VGPIO_0' : [ 0x4],
    'GPP_B' : [ 0x5],
    'GPP_G' : [ 0x6],
    'GPP_H' : [ 0x7],
    'GPD'   : [ 0x8],
    # 'SPI' : [ 0x9],
    'GPP_A' : [ 0xA],
    'GPP_C' : [ 0xB],
    # 'VGPIO_3' : [ 0xC],
    'GPP_S' : [ 0xD],
    'GPP_E' : [ 0xE],
    'GPP_K' : [ 0xF],
    'GPP_F' : [ 0x10],
    'GPP_D' : [ 0x11],
}

def get_grp_info(pch_series):
    if pch_series not in ['s', 'p', 'n']:
        raise Exception ('Invalid pch series passed')
    else:
        if pch_series == 's':
            return grp_info_s
        elif pch_series == 'p':
            return grp_info_lp
        elif pch_series == 'n':
            return grp_info_n

def rxraw_override_cfg():
    return True

def vir_to_phy_grp():
    return True

def plat_name():
    return 'adl'
