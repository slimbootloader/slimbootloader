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

def get_grp_info():
    grp_info ={
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
    }
    return grp_info

def rxraw_override_cfg():
    return True

def plat_name():
    return 'idv'

