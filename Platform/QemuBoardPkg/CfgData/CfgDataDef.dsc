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

[PcdsDynamicVpd.Tmp]

  # !BSF DEFT:{CFGHDR_TMPL:START}
    gCfgData.CfgHeader   |      * | 0x04 | {0x01:2b, ((_LENGTH_$(1)_+8)/4):10b, $(2):4b, $(3):4b, _TAG_$(1)_:12b}
    gCfgData.CondValue   |      * | 0x04 | 0x00000000
  # !BSF DEFT:{CFGHDR_TMPL:END}


  # !BSF DEFT:{GPIO_TMPL:START}
    # !BSF PAGES:{GIO_$(1):GIO:"GPIO $(1)"}
    # !BSF PAGE:{GIO_$(1)}
    # !BSF NAME:{GPIO $(1) PAD 0}
    # !BSF TYPE:{EditNum, HEX, (0x00000000,0xFFFFFFFF)}
    # !BSF HELP:{GPIO $(1) PAD 0}
    # !BSF MARKER:{GPIO_DATA_$(1)}
    gCfgData.GpioConfPad0_$(1)        |      * | 0x04 | $(2)
      # !BSF NAME:{GPIOTxState} TYPE:{Combo}	OPTION:{0:0, 1:1}
      # !BSF HELP:{GPIO TX State}
      # !BSF CONDITION:{$GpioConfPad1_$(1).GPIOSkip == 0}
      # !BSF FIELD:{GPIOTxState:1b}

      # !BSF NAME:{GPIORxState} TYPE:{Reserved}
      # !BSF FIELD:{GPIORxState:1b}

      # !BSF NAME:{Reserved} TYPE:{Reserved}
      # !BSF FIELD:{Reserved:28b}

      # !BSF NAME:{Direction}
      # !BSF TYPE:{Combo}
      # !BSF OPTION:{0x0:DirIn, 0x1:DirOut, 0x2:DirInOut, 0x3:DirNone}
      # !BSF CONDITION:{$GpioConfPad1_$(1).GPIOSkip == 0}
      # !BSF HELP:{GPIO direction}
      # !BSF FIELD:{GPIODirection:2b}

    # !BSF NAME:{GPIO $(1) PAD 1}
    # !BSF TYPE:{EditNum, HEX, (0x00000000,0xFFFFFFFF)}
    # !BSF HELP:{GPIO $(1) PAD 1}
    gCfgData.GpioConfPad1_$(1)        |      * | 0x04 | $(3)
      # !BSF NAME:{GPIO Pin ID} TYPE:{Reserved}
      # !BSF FIELD:{GPIOPin:16b}

      # !BSF NAME:{Reserved} TYPE:{Reserved}
      # !BSF FIELD:{Reserved:15b}

      # !BSF NAME:{GPIO Skip} TYPE:{Combo} OPTION:{$EN_DIS}
      # !BSF HELP:{ENABLE: Skip GPIO PAD programming (keep the defaults). DISABLE: Program GPIO PAD configuration. }
      # !BSF ORDER:{0000.0000}
      # !BSF FIELD:{GPIOSkip:1b}

  # !BSF DEFT:{GPIO_TMPL:END}

[PcdsDynamicVpd.Upd]
  #
  # This section is not used by the normal build process
  # However, FSP will use dedicated tool to handle it and generate a
  # VPD similar binary block (User Configuration Data). This block will
  # be accessed through a generated data structure directly rather than
  # PCD services. This is for size consideration.
  # Format:
  #   gCfgData.Updxxxxxxxxxxxxn      | OFFSET | LENGTH | VALUE
  # Only simple data type is supported
  #

  #
  # Comments with !BSF will be used to generate BSF file
  # Comments with !HDR will be used to generate H header file
  #

  # Global definitions in BSF
  # !BSF PAGES:{PLT::"Platform", MEM::"Memory Settings", SIL::"Silicon Settings", GIO::"GPIO Settings"}
  # !BSF BLOCK:{NAME:"QEMU Platform", VER:"0.1"}

  # CFG HDR Format
	#   UINT32  ConditionNum   :  2;      // [1:0]   #of condition dwords present
	#   UINT32  Length         : 10;      // [11:2]  total size of item (in dwords)
	#   UINT32  Flags          :  4;      // [15:12] unused/reserved so far
	#   UINT32  Version        :  4;      // [19:16] item (payload) format version
	#   UINT32  Tag            : 12;      // [31:20] identifies item (in payload)

  # !BSF FIND:{CFGD}
  gCfgData.Signature              | 0x0000 | 0x04 | {'CFGD'}
  gCfgData.HeaderLength           |      * | 0x01 | 0x10
  gCfgData.Reserved               |      * | 0x03 | {0,0,0}
  gCfgData.UsedLength             |      * | 0x04 | _LENGTH_
  gCfgData.TotalLength            |      * | 0x04 | 0x1000

  # ---------------------------------------------------------------------------------------
  !include Platform/CommonBoardPkg/CfgData/CfgData_Platform.dsc


  # !BSF SUBT:{CFGHDR_TMPL:PLATFORM_CFG_DATA:0:0}
  # !HDR EMBED:{PLATFORM_CFG_DATA:TAG_280:START}
  # !BSF NAME:{Payload Test Data}
  # !BSF TYPE:{EditText}
  # !BSF HELP:{Specify platfomr test data string}
  gCfgData.PlatTest             |      * | 0x04 | ''

  # !HDR EMBED:{PLATFORM_CFG_DATA:TAG_280:END}


  # ---------------------------------------------------------------------------------------
  # !BSF PAGE:{MEM}

  # !BSF SUBT:{CFGHDR_TMPL:MEMORY_CFG_DATA:0:0}
  # !HDR EMBED:{MEMORY_CFG_DATA:TAG_180:START}
  # !BSF NAME:{Memory Test 1}
  # !BSF TYPE:{EditNum, HEX, (0x00000000,0xFFFFFFFF)}
  # !BSF HELP:{Memory Test 1}
  gCfgData.MemTest1               |      * | 0x04 | 0x11223344


  # !BSF NAME:{Memory Test 9}
  # !BSF TYPE:{EditNum, HEX, (0x00000000,0xFFFFFFFF)}
  # !BSF HELP:{Memory Test 9}
  gCfgData.MemTest2               |      * | 0x04 | 0x11223346
  # !HDR EMBED:{MEMORY_CFG_DATA:TAG_180:END}

  # ---------------------------------------------------------------------------------------
  # !BSF PAGE:{SIL}

  # !BSF SUBT:{CFGHDR_TMPL:SILICON_CFG_DATA:0:0}
  # !HDR EMBED:{SILICON_CFG_DATA:TAG_200:START}
  # !BSF NAME:{Silicon Test 1}
  # !BSF TYPE:{EditNum, HEX, (0x00000000,0xFFFFFFFF)}
  # !BSF HELP:{Silicon Test 1}
  gCfgData.SiliconTest1           |      * | 0x04 | 0x11223347

  # !BSF NAME:{Silicon Test 9}
  # !BSF TYPE:{EditNum, HEX, (0x00000000,0xFFFFFFFF)}
  # !BSF HELP:{Silicon Test 9}
  gCfgData.SiliconTest2           |      * | 0x04 | 0x11223348
  # !HDR EMBED:{SILICON_CFG_DATA:TAG_200:END}

  # ---------------------------------------------------------------------------------------
  # !BSF PAGE:{GEN}
  # ---------------------------------------------------------------------------------------
  !include Platform/CommonBoardPkg/CfgData/CfgData_Common.dsc


  # ---------------------------------------------------------------------------------------
  # !BSF PAGE:{GIO}
  # ---------------------------------------------------------------------------------------
  !include CfgDataGpio.dsc

  # ---------------------------------------------------------------------------------------
  !include CfgData_CapsuleInformation.dsc

