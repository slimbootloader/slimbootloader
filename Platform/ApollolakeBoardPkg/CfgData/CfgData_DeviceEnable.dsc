## @file
#
#  Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
#  This program and the accompanying materials
#  are licensed and made available under the terms and conditions of the BSD License
#  which accompanies this distribution.  The full text of the license may be found at
#  http://opensource.org/licenses/bsd-license.php
#
#  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
#  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
#
##

  # !BSF PAGES:{DEV_EN_DIS:SIL:"Device Enable/Disable"}
  # !BSF PAGE:{DEV_EN_DIS}

  # !BSF SUBT:{CFGHDR_TMPL:DEV_EN_CFG_DATA:0:0}
  # !HDR EMBED:{DEV_EN_CFG_DATA:TAG_320:START}

  #                            CTRL0      CTRL1     CTRL2
  # !BSF SUBT:{DEV_EN_TMPL: 0x55555557  : 0x03FF  : 0x3FFF }

  # !HDR EMBED:{DEV_EN_CFG_DATA:TAG_320:END}
