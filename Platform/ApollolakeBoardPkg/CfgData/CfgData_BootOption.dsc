## @file
#
#  Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
#  SPDX-License-Identifier: BSD-2-Clause-Patent
#
##

  # !BSF PAGE:{OS}
  # !HDR HEADER:{OFF}

  #                               |ImgType|flag|DevType|DevNum|HwPart|SwPart|FsType|ImageName or LBA#
  # eMMC boot P1
  # !BSF SUBT:{BOOT_OPTION_TMPL:0 :  0    :  0 :   2   :  0   :   0  :    0 :    0 :'iasimage.bin'}
  # eMMC boot P2
  # !BSF SUBT:{BOOT_OPTION_TMPL:1 :  0    :  0 :   2   :  0   :   0  :    1 :    3 :'#0'          }
  # SATA boot
  # !BSF SUBT:{BOOT_OPTION_TMPL:2 :  0    :  0 :   0   :  0   :   0  :    0 :    0 :'iasimage.bin'}
  # USB boot
  # !BSF SUBT:{BOOT_OPTION_TMPL:3 :  0    :  0 :   5   :  0   :   0  :    0 :    0 :'iasimage.bin'}
  # SPI boot (for fastboot recovery)
  # !BSF SUBT:{BOOT_OPTION_TMPL:4 :  4    :  0 :   7   :  0   :   0  :    0 :    3 :'#0'          }

  # !HDR HEADER:{ON}