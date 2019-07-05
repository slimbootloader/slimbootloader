## @file
#
#  Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
#  SPDX-License-Identifier: BSD-2-Clause-Patent
#
##

  # !BSF PAGES:{DEV_EN_DIS:SIL:"Device Enable/Disable"}
  # !BSF PAGE:{DEV_EN_DIS}

  # !BSF SUBT:{CFGHDR_TMPL:DEV_EN_CFG_DATA:0:0}
  # !HDR EMBED:{DEV_EN_CFG_DATA:TAG_320:START}

  #                            CTRL0      CTRL1     CTRL2
  # !BSF SUBT:{DEV_EN_TMPL: 0x15555555  : 0x03FF  : 0x3FFF }

  # !HDR EMBED:{DEV_EN_CFG_DATA:TAG_320:END}
