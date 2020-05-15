## @file
#
#  Copyright (c) 2018 - 2019, Intel Corporation. All rights reserved.<BR>
#  SPDX-License-Identifier: BSD-2-Clause-Patent
#
##

  # !BSF PAGE:{OS}
  # !HDR HEADER:{OFF}

  #                            ID |ImgType|flag|DevType|DevNum|HwPart|SwPart|FsType|image name / LBS
  #  SD  boot
  # !BSF SUBT:{BOOT_OPTION_TMPL:0 :  0    :  0 :   1   :  0   :   0  :    0 :    2 :'iasimage.bin'}
  # SATA boot
  # !BSF SUBT:{BOOT_OPTION_TMPL:1 :  0    :  0 :   0   :  0   :   5  :    0 :    2 :'iasimage.bin'}
  # NVMe boot
  # !BSF SUBT:{BOOT_OPTION_TMPL:2 :  0    :  0 :   6   :  0   :   0  :    0 :    2 :'iasimage.bin'}
  # USB  boot
  # !BSF SUBT:{BOOT_OPTION_TMPL:3 :  0    :  0 :   5   :  0   :   0  :    0 :    2 :'iasimage.bin'}

  # !HDR HEADER:{ON}
