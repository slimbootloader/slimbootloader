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

  # !BSF PAGE:{OS}
  # !BSF SUBT:{CFGHDR_TMPL:OS_CFG_DATA:0:0}

  # !HDR HEADER:{OFF}
  # !HDR EMBED:{OS_CFG_DATA:TAG_700:START}


  # !HDR HEADER:{ON}
  # !HDR EMBED:{OS_CFG_BOOT:OsBootList:START}
  gCfgData.Rserved                |      * | 0x03 | 0

  gCfgData.OsBootOptionCount      |      * | 0x01 | 5

  gCfgData.OsBootOptions          |      * |   0  | 0
  # !HDR EMBED:{OS_CFG_BOOT:OsBootList:END}
  # !HDR HEADER:{OFF}

  # AOS boot from EMMC # |ImgType|flag|DevType|DevNum|HwPart|SwPart|FsType|  image0 name  | sw0 part| sw0 lba|  image1 name  | sw1 part| sw1 lba|  image2 name  | sw2 part| sw2 lba|
  # eMMC boot P1
  # !BSF SUBT:{OS_TMPL:0 :  0    :  0 :   2   :  0   :   0  :    0 :    0 :'iasimage.bin' :       0 :      0 :     0         :     0   :  0     :     0         :     0   :   0    }
  # eMMC boot P2
  # !BSF SUBT:{OS_TMPL:1 :  0    :  0 :   2   :  0   :   0  :    0 :    3 :       0       :       1 :      0 :     0         :     0   :  0     :     0         :     0   :   0    }
  # SATA boot
  # !BSF SUBT:{OS_TMPL:2 :  0    :  0 :   0   :  0   :   0  :    0 :    0 :'iasimage.bin' :       0 :      0 :     0         :     0   :  0     :     0         :     0   :   0    }
  # USB boot
  # !BSF SUBT:{OS_TMPL:3 :  0    :  0 :   5   :  0   :   0  :    0 :    0 :'iasimage.bin' :       0 :      0 :     0         :     0   :  0     :     0         :     8   :   0    }
  # SPI boot (for fastboot recovery)
  # !BSF SUBT:{OS_TMPL:4 :  4    :  0 :   7   :  0   :   0  :    0 :    3 :      0        :       0 :      0 :     0         :     0   :  0     :     0         :     0   :   0    }

  # !HDR EMBED:{OS_CFG_DATA:TAG_700:END}

  # !HDR HEADER:{ON}