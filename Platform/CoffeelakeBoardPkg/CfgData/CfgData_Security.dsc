## @file
#
#  Copyright (c) 2018 - 2019, Intel Corporation. All rights reserved.<BR>
#  SPDX-License-Identifier: BSD-2-Clause-Patent
#
##
  # !BSF PAGES:{PSD:SEC:"PSD"}
  # !BSF PAGE:{PSD}
  # !BSF SUBT:{CFGHDR_TMPL:PSD_CFG_DATA:0:0}
  # !HDR EMBED:{PSD_CFG_DATA:TAG_800:START}

  # !BSF NAME:{Platform Security Discovery (PSD)} TYPE:{Combo}
  # !BSF OPTION:{0:Disabled, 1:Enabled}
  # !BSF HELP:{Enable/Disable Platform Security Discovery (PSD)}
  gCfgData.EnablePsd              |      * | 0x04 | 0x0

  # !HDR EMBED:{PSD_CFG_DATA:TAG_800:END}

