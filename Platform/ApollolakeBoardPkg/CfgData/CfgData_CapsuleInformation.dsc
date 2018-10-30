  # !BSF PAGE:{PLT}
  # !BSF SUBT:{CFGHDR_TMPL:CAPSULE_INFO_CFG_DATA:0:0}

  # !HDR EMBED:{CAPSULE_INFO_CFG_DATA:TAG_080:START}

  # !BSF NAME:{DevType}
  # !BSF TYPE:{Combo}
  # !BSF OPTION:{0:SATA, 1:SD, 2:EMMC, 3:UFS, 4:SPI, 5:USB, 6:NVME, 7:MAX}
  # !BSF HELP:{Specify boot device}
  gCfgData.DevType                      |      * | 0x01 | 5

  # !BSF NAME:{Boot Device instance}
  # !BSF TYPE:{Combo}
  # !BSF OPTION:{0:Device 0, 1:Device 1, 2:Device 2, 3:Device 3}
  # !BSF HELP:{Specify boot device instance when then are multple instances}
  # !BSF ORDER:{0000.0000}
  gCfgData.DevInstance                  |      * | 0x01 | 5

  gCfgData.Reserved                     |      * | 0x03 | 0

  # !BSF NAME:{Hardware Partition}
  # !BSF TYPE:{Combo}
  # !BSF OPTION:{0:User Partition, 1:Boot Partition 1, 2:Boot Partition 2}
  # !BSF HELP:{Specify hardware partition number}
  gCfgData.HwPart                       |      * | 0x01 | 0

  # !BSF NAME:{Software Partition}
  # !BSF TYPE:{EditNum, INT, (0,127)}
  # !BSF HELP:{Specify software partition number}
  gCfgData.SwPart                       |      * | 0x01 | 0

  # !BSF NAME:{File System Type}
  # !BSF TYPE:{Combo}
  # !BSF OPTION:{0:FAT, 1:EXT2, 2:AUTO, 3:RAW}
  # !BSF HELP:{Image is loaded from file system instead of raw data}
  gCfgData.FsType                       |      * | 0x01 | 2

  # !BSF NAME:{Capsule File Name}
  # !BSF TYPE:{EditText}
  # !BSF HELP:{Specify file name of capsule image  (16-byte max length)}
  gCfgData.FileName                     |      * | 0x10 | 'FwuImage.bin'

  # !BSF NAME:{LBA address for capsule image)}
  # !BSF TYPE:{EditNum, HEX, (0,0xFFFFFFFF)}
  # !BSF HELP:{specify LBA address where to find capsule image}
  gCfgData.LbaAddr                      |      * | 0x04 | 0

  # !HDR EMBED:{CAPSULE_INFO_CFG_DATA:TAG_080:END}