## @file
#
#  Copyright (c) 2018 - 2019, Intel Corporation. All rights reserved.<BR>
#  This program and the accompanying materials
#  are licensed and made available under the terms and conditions of the BSD License
#  which accompanies this distribution.  The full text of the license may be found at
#  http://opensource.org/licenses/bsd-license.php
#
#  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
#  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
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

