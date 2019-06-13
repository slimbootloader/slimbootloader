## @file
#
#  Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
#  SPDX-License-Identifier: BSD-2-Clause-Patent
#
##

  # !BSF PAGES:{USB:SIL:"USB"}

  # !BSF PAGES:{USB_OVER_CUR:USB:"USB OverCurrent Settings"}
  # !BSF PAGE:{USB_OVER_CUR}

  # !BSF SUBT:{CFGHDR_TMPL:USB_CFG_DATA:0:0}

  # !HDR EMBED:{USB_CFG_DATA:TAG_301:START}

  # !BSF SUBT:{USB_TMPL:0x0554A554}

  # !HDR EMBED:{USB_CFG_DATA:TAG_301:END}