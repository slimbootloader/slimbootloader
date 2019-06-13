## @file
#
#  Copyright (c) 2018 - 2019, Intel Corporation. All rights reserved.<BR>
#  SPDX-License-Identifier: BSD-2-Clause-Patent
#
##

  # !BSF PAGE:{OS}
  # !BSF SUBT:{CFGHDR_TMPL:OS_CFG_DATA:0:0}

  # !HDR HEADER:{OFF}
  # !HDR EMBED:{OS_CFG_DATA:TAG_700:START}


  # !HDR HEADER:{ON}
  # !HDR EMBED:{OS_CFG_BOOT:OsBootList:START}
  gCfgData.Rserved                |      * | 0x03 | 0

  gCfgData.OsBootOptionCount      |      * | 0x01 | 3

  gCfgData.OsBootOptions          |      * |   0  | 0
  # !HDR EMBED:{OS_CFG_BOOT:OsBootList:END}
  # !HDR HEADER:{OFF}

  # AOS boot from EMMC # |ImgType|flag|DevType|DevNum|HwPart|SwPart|FsType|  image0 name  | sw0 part| sw0 lba|  image1 name  | sw1 part| sw1 lba|  image2 name  | sw2 part| sw2 lba|
  # USB  boot
  # !BSF SUBT:{OS_TMPL:0 :  0    :  0 :   5   :  0   :   0  :    0 :    0 :'iasimage.bin' :       0 :      0 :     0         :     0   :  0     :     0         :     8   :   0    }
  # SATA boot
  # !BSF SUBT:{OS_TMPL:1 :  0    :  0 :   0   :  0   :   2  :    0 :    0 :'iasimage.bin' :       0 :      0 :     0         :     0   :  0     :     0         :     0   :   0    }
  # EMMC boot
  # !BSF SUBT:{OS_TMPL:2 :  0    :  0 :   2   :  0   :   0  :    0 :    0 :'iasimage.bin' :       0 :      0 :     0         :     0   :  0     :     0         :     0   :   0    }
  # !HDR EMBED:{OS_CFG_DATA:TAG_700:END}


  # !HDR HEADER:{ON}
