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

  # !BSF PAGES:{USB:SIL:"USB"}

  # !BSF PAGES:{USB_OVER_CUR:USB:"USB OverCurrent Settings"}
  # !BSF PAGE:{USB_OVER_CUR}

  # !BSF SUBT:{CFGHDR_TMPL:USB_CFG_DATA:0:0}

  # !HDR EMBED:{USB_CFG_DATA:TAG_301:START}

  # !BSF SUBT:{USB_TMPL:0x0554A554}

  # !HDR EMBED:{USB_CFG_DATA:TAG_301:END}