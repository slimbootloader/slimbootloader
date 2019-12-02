## @file
#
#  Copyright (c) 2018 - 2019, Intel Corporation. All rights reserved.<BR>
#  SPDX-License-Identifier: BSD-2-Clause-Patent
#
##

  # !BSF PAGE:{OS}
  # !HDR HEADER:{OFF}

  # AOS boot from EMMC # |ImgType|flag|DevType|DevNum|HwPart|SwPart|FsType|  image0 name
  # USB  boot
  # !BSF SUBT:{BOOT_OPTION_TMPL:0 :  0    :  0 :   5   :  0   :   0  :    0 :    0 :'iasimage.bin'}
  # SATA boot
  # !BSF SUBT:{BOOT_OPTION_TMPL:1 :  0    :  0 :   0   :  0   :   2  :    0 :    0 :'iasimage.bin'}
  # EMMC boot
  # !BSF SUBT:{BOOT_OPTION_TMPL:2 :  0    :  0 :   2   :  0   :   0  :    0 :    0 :'iasimage.bin'}

  # !HDR HEADER:{ON}
