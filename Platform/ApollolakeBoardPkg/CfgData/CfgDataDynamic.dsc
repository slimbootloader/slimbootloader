## @file
#
#  Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
#  SPDX-License-Identifier: BSD-2-Clause-Patent
#
##

[PcdsDynamicVpd.Tmp]
  # !BSF DEFT:{CFGHDR_TMPL:START}
    gCfgData.CfgHeader            |      * | 0x04 | {0x01:2b, ((_LENGTH_$(1)_+8)/4):10b, $(2):4b, $(3):4b, _TAG_$(1)_:12b}
    gCfgData.CondValue            |      * | 0x04 | 0x00000000
  # !BSF DEFT:{CFGHDR_TMPL:END}

[PcdsDynamicVpd.Upd]
  # !BSF FIND:{CFGD}
  gCfgData.Signature              | 0x0000 | 0x04 | {'CFGD'}
  gCfgData.HeaderLength           |      * | 0x01 | 0x10
  gCfgData.Reserved               |      * | 0x03 | {0,0,0}
  gCfgData.UsedLength             |      * | 0x04 | _LENGTH_
  gCfgData.TotalLength            |      * | 0x04 | 0x100

  # !BSF PAGE:{DYN}
  # !BSF SUBT:{CFGHDR_TMPL:DYNAMIC_CFG_DATA:0:0}

  # !HDR EMBED:{DYNAMIC_CFG_DATA:TAG_600:START}

  # !BSF NAME:{MRC Training Enforcement}
  # !BSF TYPE:{Combo} OPTION:{$EN_DIS}
  # !BSF HELP:{When enabled, it enforces memory training even if consistent memory parameters are available}
  gCfgData.MrcTrainingEnforcement      |      * | 0x01 | 0x0

  # !BSF NAME:{Emmc Tuning Enforcement}
  # !BSF TYPE:{Combo} OPTION:{$EN_DIS}
  # !BSF HELP:{When enabled, it enforces emmc tuning even if consistent dll parameters are available}
  gCfgData.EmmcTuningEnforcement       |      * | 0x01 | 0x0

  # !BSF NAME:{Recalibration}
  # !BSF TYPE:{Combo} OPTION:{$EN_DIS}
  # !BSF HELP:{When enabled, it triggers recalibration}
  gCfgData.Recalibration               |      * | 0x01 | 0x0

  gCfgData.Dummy                       |      * | 0x01 | 0x0
  # !HDR EMBED:{DYNAMIC_CFG_DATA:TAG_600:END}