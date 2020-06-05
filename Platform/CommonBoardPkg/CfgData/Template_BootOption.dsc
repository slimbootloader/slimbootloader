## @file
#
#  Copyright (c) 2018 - 2019, Intel Corporation. All rights reserved.<BR>
#  SPDX-License-Identifier: BSD-2-Clause-Patent
#
##

  #
  # ========================= BOOT_OPTION_TMPL ==========================
  #
  # NOTE: MUST match OS_BOOT_OPTION structure from field 'ImageType' to 'FsType' !!!
  # !BSF DEFT:{BOOT_OPTION_TMPL:START}
    # !BSF PAGES:{OS_$(1):OS:"Boot Option $(1)"}
    # !BSF PAGE:{OS_$(1)}

    # !BSF SUBT:{CFGHDR_TMPL:BOOT_OPTION_CFG_DATA_$(1):0:0}
    # !HDR EMBED:{BOOT_OPTION_CFG_DATA_$(1):TAG_05$(1):START}

    # !BSF NAME:{Image Type}
    # !BSF TYPE:{Combo}
    # !BSF OPTION:{0:Default, 1:Android, 2:ClearLinux, 3:Acrn, 4:Fastboot, 0xFE:Addendum, 0xFF:Not used}
    # !BSF HELP:{Specify boot image type.}
    # !BSF HELP:{+Specially 'Addendum' indicates this option is not a standalone boot option. Instead, it provides additional information for the previous boot option.}
    # !BSF HELP:{+'Not used' indicates this option will be ignored.}
    gCfgData.ImageType_$(1)                 |      * | 0x01 | $(2)

    # !BSF NAME:{Boot Flags}
    # !BSF TYPE:{Combo}
    # !BSF OPTION:{0:Normal, 1:A/B support, 2:Crash OS, 4:Trusty support, 5:Trusty and A/B support, 16:Mender support}
    # !BSF HELP:{Specify boot flags (options)}
    # !BSF CONDITION:{$ImageType_$(1) < 0xFE}
    gCfgData.BootFlags_$(1)                 |      * | 0x01 | $(3)

    gCfgData.Reserved_$(1)                  |      * | 0x01 | 0x5A
    # !BSF NAME:{Boot Device type}
    # !BSF TYPE:{Combo}
    # !BSF OPTION:{0:SATA, 1:SD, 2:EMMC, 3:UFS, 4:SPI, 5:USB, 6:NVME, 7:MEMORY}
    # !BSF HELP:{Specify boot device type}
    # !BSF ORDER:{0000.0000}
    # !BSF CONDITION:{$ImageType_$(1) < 0xFE}
    gCfgData.BootDeviceType_$(1)            |      * | 0x01 | $(4)

    # !BSF NAME:{Boot Device instance}
    # !BSF TYPE:{Combo}
    # !BSF OPTION:{0:Device 0, 1:Device 1, 2:Device 2, 3:Device 3}
    # !BSF HELP:{Specify boot device instance when then are multiple instances}
    # !BSF ORDER:{0000.0000}
    # !BSF CONDITION:{$ImageType_$(1) < 0xFE}
    gCfgData.BootDeviceInstance_$(1)        |      * | 0x01 | $(5)

    # !BSF NAME:{Hardware Partition}
    # !BSF TYPE:{Combo}
    # !BSF OPTION:{0:User Partition, 1:Boot Partition 1, 2:Boot Partition 2, 3:Boot Partition 3, 4:Boot Partition 4}
    # !BSF HELP:{Specify hardware partition number.}
    # !BSF HELP:{+ If boot device type is SPI, the hardware partition refers to SPI flash region: 0-Descriptor, 1-BIOS, 2-ME, 3-GbE, 4-PDR Regions}
    # !BSF CONDITION:{$ImageType_$(1) < 0xFE}
    gCfgData.HwPart_$(1)                    |      * | 0x01 | $(6)

    # !BSF NAME:{Software Partition}
    # !BSF TYPE:{EditNum, INT, (0,127)}
    # !BSF HELP:{Specify software partition number}
    # !BSF CONDITION:{$ImageType_$(1) < 0xFF}
    gCfgData.SwPart_$(1)                   |      * | 0x01 | $(7)

    # !BSF NAME:{File System Type}
    # !BSF TYPE:{Combo}
    # !BSF OPTION:{0:FAT, 1:EXT2, 2:AUTO, 3:RAW}
    # !BSF HELP:{Image is loaded from file system instead of raw data}
    # !BSF CONDITION:{$ImageType_$(1) < 0xFE}
    gCfgData.FsType_$(1)                    |      * | 0x01 | $(8)

    # !BSF NAME:{Normal OS info or LBA address}
    # !BSF TYPE:{EditText}
    # !BSF HELP:{Specify file name of normal OS or LBA address (16-byte max length).}
    # !BSF HELP:{+For LBA address, please use HEX prefix "#0x". EX: #0x12345678 }
    # !BSF CONDITION:{$ImageType_$(1) < 0xFF}
    gCfgData.BootImage_$(1)                 |      * | 0x10 | $(9)

    # !HDR EMBED:{BOOT_OPTION_CFG_DATA_$(1):TAG_05$(1):END}

  # !BSF DEFT:{BOOT_OPTION_TMPL:END}


  # NOTES:
  #   This is old boot option template. It is deprecated.
  #   Please use BOOT_OPTION_TMPL instead.
  #   It is still listed here to enable projects that still use old template.
  # ========================= OS_TMPL ==========================
  #
  # NOTE: MUST match OS_BOOT_OPTION structure!!!
  # !BSF DEFT:{OS_TMPL:START}
    # !BSF PAGES:{OS_$(1):OS:"Boot Option $(1)"}
    # !BSF PAGE:{OS_$(1)}

    # !BSF NAME:{Image Type}
    # !BSF TYPE:{Combo}
    # !BSF OPTION:{0:Default, 1:Android, 2:ClearLinux, 3:Acrn, 4:Fastboot}
    # !BSF HELP:{Specify boot image type}
    gCfgData.ImageType_$(1)                 |      * | 0x01 | $(2)

    # !BSF NAME:{Boot Flags}
    # !BSF TYPE:{Combo}
    # !BSF OPTION:{0:Normal, 1:A/B support, 2:Crash OS, 4:Trusty support, 5:Trusty and A/B support, 16:Mender support}
    # !BSF HELP:{Specify boot flags (options)}
    gCfgData.BootFlags_$(1)                 |      * | 0x01 | $(3)

    gCfgData.Reserved_$(1)                  |      * | 0x01 | 0x5A
    # !BSF NAME:{Boot Device type}
    # !BSF TYPE:{Combo}
    # !BSF OPTION:{0:SATA, 1:SD, 2:EMMC, 3:UFS, 4:SPI, 5:USB, 6:NVME, 7:MAX}
    # !BSF HELP:{Specify boot device type}
    # !BSF ORDER:{0000.0000}
    gCfgData.BootDeviceType_$(1)            |      * | 0x01 | $(4)

    # !BSF NAME:{Boot Device instance}
    # !BSF TYPE:{Combo}
    # !BSF OPTION:{0:Device 0, 1:Device 1, 2:Device 2, 3:Device 3}
    # !BSF HELP:{Specify boot device instance when then are multiple instances}
    # !BSF ORDER:{0000.0000}
    gCfgData.BootDeviceInstance_$(1)        |      * | 0x01 | $(5)

    # !BSF NAME:{Hardware Partition}
    # !BSF TYPE:{Combo}
    # !BSF OPTION:{0:User Partition, 1:Boot Partition 1, 2:Boot Partition 2}
    # !BSF HELP:{Specify hardware partition number}
    gCfgData.HwPart_$(1)                    |      * | 0x01 | $(6)

    # !BSF NAME:{Software Partition}
    # !BSF TYPE:{EditNum, INT, (0,127)}
    # !BSF HELP:{Specify software partition number}
    gCfgData.SwPart_$(1)                    |      * | 0x01 | $(7)

    # !BSF NAME:{File System Type}
    # !BSF TYPE:{Combo}
    # !BSF OPTION:{0:FAT, 1:EXT2, 2:AUTO, 3:RAW}
    # !BSF HELP:{Image is loaded from file system instead of raw data}
    gCfgData.FsType_$(1)                    |      * | 0x01 | $(8)


    # ---------------------------- IMAGE[0] -------------------------
    # !BSF NAME:{Normal OS info}
    # !BSF CONDITION:{$FsType_$(1) != 3}
    # !BSF TYPE:{EditText}
    # !BSF HELP:{Specify file name of normal OS (16-byte max length)}
    gCfgData.BootImage_0_$(1)               |      * | 0x10 | $(9)

    gCfgData.Valid_0_$(1)                   |      * | 0x01 | 1

    # !BSF NAME:{Software partition for normal OS Image)}
    # !BSF CONDITION:{$FsType_$(1) == 3}
    # !BSF TYPE:{EditNum, INT, (0,127)}
    # !BSF HELP:{Specify software partition number}
    gCfgData.SwPart_0_$(1)                  |      * | 0x01 | $(10)

    gCfgData.Reserved_0_$(1)                |      * | 0x02 | 0

    # !BSF NAME:{LBA address for normal OS Image)}
    # !BSF CONDITION:{$FsType_$(1) == 3}
    # !BSF TYPE:{EditNum, INT, (0,0xFFFFFFFF)}
    # !BSF HELP:{Specify LBA address where to find normal OS image}
    gCfgData.LbaAddr_0_$(1)                 |      * | 0x04 | $(11)

    # ---------------------------- IMAGE[1] -------------------------

    # !BSF NAME:{Trusty OS info}
    # !BSF CONDITION:{($BootFlags_$(1) & 0x4 == 0x4) and ($FsType_$(1) != 3)}
    # !BSF TYPE:{EditText}
    # !BSF HELP:{Specify file name of trusty OS  (16-byte max length)}
    gCfgData.BootImage_1_$(1)               |      * | 0x10 | $(12)

    gCfgData.Valid_1_$(1)                   |      * | 0x01 | 1

    # !BSF NAME:{Software partition for trusty OS}
    # !BSF CONDITION:{($BootFlags_$(1) & 0x4 == 0x4) and ($FsType_$(1) == 3)}
    # !BSF TYPE:{EditNum, INT, (0,127)}
    # !BSF HELP:{Specify software partition number}
    gCfgData.SwPart_1_$(1)                  |      * | 0x01 | $(13)

    gCfgData.Reserved_1_$(1)                |      * | 0x02 | 0

    # !BSF NAME:{LBA address for trusty OS}
    # !BSF CONDITION:{($BootFlags_$(1) & 0x4 == 0x4) and ($FsType_$(1) != 3)}
    # !BSF TYPE:{EditNum, INT, (0,0xFFFFFFFF)}
    # !BSF HELP:{Specify LBA address where to find trusty OS image}
    gCfgData.LbaAddr_1_$(1)                 |      * | 0x04 | $(14)


    # ---------------------------- IMAGE[2] -------------------------
    # !BSF NAME:{MISC Image info}
    # !BSF CONDITION:{($BootFlags_$(1) & 0x1 == 0x1) and ($FsType_$(1) != 3)}
    # !BSF TYPE:{EditText}
    # !BSF HELP:{Specify file name of misc image for OS A/B support (16-byte max length)}
    gCfgData.BootImage_2_$(1)               |      * | 0x10 | $(15)

    gCfgData.Valid_2_$(1)                   |      * | 0x01 | 1

    # !BSF NAME:{Software partition for misc Image}
    # !BSF CONDITION:{($BootFlags_$(1) & 0x1 == 0x1) and ($FsType_$(1) == 3)}
    # !BSF TYPE:{EditNum, INT, (0,127)}
    # !BSF HELP:{Specify software partition number for OS A/B support}
    gCfgData.SwPart_2_$(1)                  |      * | 0x01 | $(16)

    gCfgData.Reserved_2_$(1)                |      * | 0x02 | 0

    # !BSF NAME:{LBA address for misc image}
    # !BSF CONDITION:{($BootFlags_$(1) & 0x1 == 0x1) and ($FsType_$(1) == 3)}
    # !BSF TYPE:{EditNum, INT, (0,0xFFFFFFFF)}
    # !BSF HELP:{Specify LBA address where to find misc image to support OS A/B}
    gCfgData.LbaAddr_2_$(1)                 |      * | 0x04 | $(17)

    # ---------------------------- IMAGE[3] -------------------------
    gCfgData.BootImage_3_$(1)               |      * | 0x10 | 'ias.bin'

    gCfgData.Valid_3_$(1)                   |      * | 0x01 | 0

    gCfgData.SwPart_3_$(1)                  |      * | 0x01 | 0

    gCfgData.Reserved_3_$(1)                |      * | 0x02 | 0

    gCfgData.LbaAddr_3_$(1)                 |      * | 0x04 | 0

    # ---------------------------- IMAGE[4] -------------------------
    gCfgData.BootImage_4_$(1)               |      * | 0x10 | 'ias.bin'

    gCfgData.Valid_4_$(1)                   |      * | 0x01 | 0

    gCfgData.SwPart_4_$(1)                  |      * | 0x01 | 0

    gCfgData.Reserved_4_$(1)                |      * | 0x02 | 0

    gCfgData.LbaAddr_4_$(1)                 |      * | 0x04 | 0

    # ---------------------------- IMAGE[5] -------------------------
    gCfgData.BootImage_5_$(1)               |      * | 0x10 | 'ias.bin'

    gCfgData.Valid_5_$(1)                   |      * | 0x01 | 0

    gCfgData.SwPart_5_$(1)                  |      * | 0x01 | 0

    gCfgData.Reserved_5_$(1)                |      * | 0x02 | 0

    gCfgData.LbaAddr_5_$(1)                 |      * | 0x04 | 0

    # ---------------------------- IMAGE[6] -------------------------
    gCfgData.BootImage_6_$(1)               |      * | 0x10 | 'ias.bin'

    gCfgData.Valid_6_$(1)                   |      * | 0x01 | 0

    gCfgData.SwPart_6_$(1)                  |      * | 0x01 | 0

    gCfgData.Reserved_6_$(1)                |      * | 0x02 | 0

    gCfgData.LbaAddr_6_$(1)                 |      * | 0x04 | 0

  # !BSF DEFT:{OS_TMPL:END}
