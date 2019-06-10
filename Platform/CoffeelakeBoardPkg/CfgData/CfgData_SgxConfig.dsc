## @file
#
#  Copyright (c) 2019, Intel Corporation. All rights reserved.<BR>
#  This program and the accompanying materials
#  are licensed and made available under the terms and conditions of the BSD License
#  which accompanies this distribution.  The full text of the license may be found at
#  http://opensource.org/licenses/bsd-license.php
#
#  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
#  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
#
##

  # !BSF PAGES:{SGX:SEC:"SGX"}
  # !BSF PAGE:{SGX}
  # !BSF SUBT:{CFGHDR_TMPL:SGX_CFG_DATA:0:0}
  # !HDR EMBED:{SGX_CFG_DATA:TAG_500:START}

  # !BSF NAME:{Software Guard eXtensions (SGX)} TYPE:{Combo}
  # !BSF OPTION:{0:Disabled, 1:Enabled}
  # !BSF HELP:{Enable/Disable Software Guard eXtensions (SGX).}
  gCfgData.EnableSgx              |      * | 0x01 | 0x0

  # !BSF CONDITION:{$EnableSgx != 0}
  # !BSF NAME:{Select Owner EPOCH input type} TYPE:{Combo}
  # !BSF OPTION:{0:No Change in Owner EPOCHs, 1:Manual User Defined Owner EPOCHs}
  # !BSF HELP:{There are two Owner EPOCH modes (Each EPOCH is 64bit): no change in owner epoch and manually entered by user. After the user enters epoch values manually, the values will not be visible, for security reasons.}
  gCfgData.EpochUpdate            |      * | 0x01 | 0x0

  # !BSF CONDITION:{$EnableSgx != 0x0 and $EpochUpdate == 0x1}
  # !BSF NAME:{Software Guard eXtensions Epoch 0}
  # !BSF TYPE:{EditNum, HEX, (0x0,0xFFFFFFFFFFFFFFFF)}
  gCfgData.SgxEpoch0            |      * | 0x8 | 0x553DFD8D5FA48F27
  # !BSF CONDITION:{$EnableSgx != 0x0 and $EpochUpdate == 0x1}
  # !BSF NAME:{Software Guard eXtensions Epoch 1}
  # !BSF TYPE:{EditNum, HEX, (0x0,0xFFFFFFFFFFFFFFFF)}
  gCfgData.SgxEpoch1            |      * | 0x8 | 0xD76767B9BE4BFDC1

  # !BSF CONDITION:{$EnableSgx == 0x1}
  # !BSF NAME:{PRMRR Size} TYPE:{Combo}
  # !BSF OPTION:{0x2000000:32MB, 0x4000000:64MB, 0x8000000:128MB}
  # !BSF HELP:{Setting the PRMRR Size}
  gCfgData.PrmrrSize              |      * | 0x04 | 0x4000000

  gCfgData.PaddingBytes           |      * | 0x02 | 0x0
  # !HDR EMBED:{SGX_CFG_DATA:TAG_500:END}

