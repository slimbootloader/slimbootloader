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

  # ---------------------------------------------------------------------------------------
  # !BSF DEFT:{CFGHDR_TMPL:START}
    gCfgData.CfgHeader                |      * | 0x04 | {0x01:2b, ((_LENGTH_$(1)_+8)/4):10b, $(2):4b, $(3):4b, _TAG_$(1)_:12b}
    gCfgData.CondValue                |      * | 0x04 | 0x00000000
  # !BSF DEFT:{CFGHDR_TMPL:END}

  # ---------------------------------------------------------------------------------------
  # !BSF DEFT:{FEATURES_TMPL:START}
    # !BSF PAGES:{PLT_FEATURES:FEATURES:"Features"}
    # !BSF PAGE:{PLT_FEATURES}


    # !BSF NAME:{PlatformFeatures}
    # !HDR STRUCT:{FEATURES_DATA}
    gCfgData.Features                       |      * | 0x04 | $(1)

      # !BSF NAME:{ACPI Enable} TYPE:{Combo} OPTION:{$EN_DIS}
      # !BSF HELP:{Enable/Disable ACPI feature. 1:ACPI Enabled, 0:ACPI Disabled}
      # !BSF FIELD:{Acpi:1b}

      # !BSF NAME:{MeasuredBoot Enable} TYPE:{Combo} OPTION:{$EN_DIS}
      # !BSF HELP:{Enable/Disable MeasuredBoot feature. 1:MeasuredBoot Enabled, 0:MeasuredBoot Disabled}
      # !BSF FIELD:{MeasuredBoot:1b}

      # !BSF NAME:{VT Enable} TYPE:{Combo} OPTION:{$EN_DIS}
      # !BSF HELP:{Enable/Disable Vt-x/Vt-d feature. 1:VT Enabled, 0:VT Disabled}
      # !BSF FIELD:{Vt:1b}

      # !BSF NAME:{eMMC Tuning Enable} TYPE:{Combo} OPTION:{$EN_DIS}
      # !BSF HELP:{Enable/Disable EMMC feature, if disabled get the DLLs from DLL config data}
      # !BSF FIELD:{eMMCTuning:1b}

      # !BSF NAME:{DCI Debug Enable} TYPE:{Combo} OPTION:{$EN_DIS}
      # !BSF HELP:{Enable/Disable DCI Debug feature. 1:DCI Debug Enabled, 0:DCI Debug Disabled}
      # !BSF FIELD:{DciDebug:1b}

      # !BSF NAME:{Reserved} TYPE:{Reserved}
      # !BSF FIELD:{Rsvd1:27b}

  # !BSF DEFT:{FEATURES_TMPL:END}


  # ---------------------------------------------------------------------------------------
  # !BSF DEFT:{PIDGPIO_TMPL:START}
    # !BSF PAGES:{PLT_$(1):PLT_PIDGPIO:"Platform ID Pin $(1)"}
    # !BSF PAGE:{PLT_$(1)}

    # !BSF NAME:{GPIO $(1) Data}
    # !HDR STRUCT:{PID_GPIO_PIN_DATA[]}
    gCfgData.$(1)                       |      * | 0x04 | $(2)

      # !BSF NAME:{Pad Community}
      # !BSF TYPE:{Combo}
      # !BSF OPTION:{0xC2: SOUTH, 0xC7: WEST, 0xC4: NORTHWEST, 0xC0: SOUTHWEST, 0xC5: NORTH}
      # !BSF HELP:{Specify Pad Community}
      # !BSF CONDITION:{$(COND_GPIO_PID_ENABLE)}
      # !BSF FIELD:{Community:8b}

      # !BSF NAME:{Pad Number}
      # !BSF TYPE:{EditNum, INT, (0,4095)}
      # !BSF HELP:{Specify Pad Number}
      # !BSF CONDITION:{$(COND_GPIO_PID_ENABLE)}
      # !BSF FIELD:{PadNum:12b}

      # !BSF NAME:{Allow Pull-up}
      # !BSF TYPE:{Combo}
      # !BSF OPTION:{0x0:P_NONE, 0x2:P_5K_L, 0x4:P_20K_L, 0x9:P_1K_H, 0xA:P_5K_H, 0xB:P_2K_H, 0xC:P_20K_H, 0xD:P_1K2K_H, 0xF:Native_control}
      # !BSF HELP:{Specify if pull-up needed and how much}
      # !BSF CONDITION:{$(COND_GPIO_PID_ENABLE)}
      # !BSF FIELD:{PullUp:4b}

      # !BSF NAME:{Reserved} TYPE:{Reserved}
      # !BSF FIELD:{Rsvd1:7b}

      # !BSF NAME:{PID Pin Enable} TYPE:{Combo} OPTION:{$EN_DIS}
      # !BSF HELP:{Enable/Disable this pin for PID detection. }
      # !BSF ORDER:{0000.0000}
      # !BSF CONDITION:{$PlatformId==0}
      # !BSF FIELD:{Enable:1b}

  # !BSF DEFT:{PIDGPIO_TMPL:END}


  # ---------------------------------------------------------------------------------------
  # !BSF DEFT:{GPIO_TMPL:START}
    # !BSF PAGES:{GIO_$(1):GIO:"GPIO $(1)"}
    # !BSF PAGE:{GIO_$(1)}

    # !BSF NAME:{GPIO $(1) Half0}
    # !BSF MARKER:{$(1)_Half0}
    gCfgData.$(1)_Half0               |      * | 0x04 | $(2)
      # !BSF NAME:{Reserved} TYPE:{Reserved}
      # !BSF FIELD:{Rsvd1:31b}

      # !BSF NAME:{GPIO Skip} TYPE:{Combo} OPTION:{$EN_DIS}
      # !BSF HELP:{ENABLE: Skip GPIO PAD programming (keep the defaults). DISABLE: Program GPIO PAD configuration. }
      # !BSF ORDER:{0000.0000}
      # !BSF FIELD:{GpioSkip:1b}

    # !BSF NAME:{GPIO $(1) Half1}
    # !BSF TYPE:{EditNum, HEX, (0x00000000,0xFFFFFFFF)}
    # !BSF HELP:{GPIO $(1) Half1}
    gCfgData.$(1)_Half1               |      * | 0x04 | $(3)
      # !BSF NAME:{GpioDef}
      # !BSF TYPE:{Combo}
      # !BSF OPTION:{0:Low, 1:High}
      # !BSF HELP:{GPIO Default Value}
      # !BSF CONDITION:{$(COND_GPIO_SKIP)}
      # !BSF FIELD:{DefValue:1b}

      # !BSF NAME:{GpioDirection}
      # !BSF TYPE:{Combo}
      # !BSF OPTION:{0:GPIO, 1:GPI, 2:GPO, 3:HI_Z, 0xFF:Native}
      # !BSF HELP:{GPIO GpioDirection}
      # !BSF CONDITION:{$(COND_GPIO_SKIP)}
      # !BSF FIELD:{Direction:2b}

      # !BSF NAME:{GpioAltFunc}
      # !BSF TYPE:{Combo}
      # !BSF OPTION:{0:M0, 1:M1, 2:M2, 3:M3}
      # !BSF HELP:{GPIO GpioAltFunc}
      # !BSF CONDITION:{$(COND_GPIO_SKIP)}
      # !BSF FIELD:{AltFunc:3b}

      # !BSF NAME:{GpioIntRoute}
      # !BSF TYPE:{Combo}
      # !BSF OPTION:{0:None/NA, 1:NMI, 2:SMI, 4:SCI, 8:IOAPIC}
      # !BSF HELP:{GPIO GpioIntRoute}
      # !BSF CONDITION:{$(COND_GPIO_SKIP)}
      # !BSF FIELD:{InterruptRoute:6b}

      # !BSF NAME:{GpioRxInvert}
      # !BSF TYPE:{Combo}
      # !BSF OPTION:{0:NotInverted, 1:Inverted}
      # !BSF HELP:{GPIO GpioRxInvert}
      # !BSF CONDITION:{$(COND_GPIO_SKIP)}
      # !BSF FIELD:{RxInvert:2b}

      # !BSF NAME:{GpioIntType}
      # !BSF TYPE:{Combo}
      # !BSF OPTION:{0:Level, 1:Edge, 3:Both Edge, 2:Disable}
      # !BSF HELP:{GPIO GpioIntType}
      # !BSF CONDITION:{$(COND_GPIO_SKIP)}
      # !BSF FIELD:{InterruptType:6b}

      # !BSF NAME:{GpioPuPdEnable}
      # !BSF TYPE:{Combo}
      # !BSF OPTION:{0:Same, 1:Disable PuPd, 2:Enable Pull Down, 3:Enable Pull Up}
      # !BSF HELP:{GPIO GpioPuPdEnable}
      # !BSF CONDITION:{$(COND_GPIO_SKIP)}
      # !BSF FIELD:{PuPdEnable:2b}

      # !BSF NAME:{GpioPuPdValue}
      # !BSF TYPE:{Combo}
      # !BSF OPTION:{0:None, 2:PD_5K, 4:PD_20K, 9:PU_1K, 11:PU_2K, 12:PU_20K, 10:PU_5K, 13:PU_1K2K, 15:Native}
      # !BSF HELP:{GPIO GpioPuPdValue}
      # !BSF CONDITION:{$(COND_GPIO_SKIP)}
      # !BSF FIELD:{PuPdValue:4b}

      # !BSF NAME:{GpioIoState}
      # !BSF TYPE:{Combo}
      # !BSF OPTION:{0:Last Value, 1:D0RxDRx0I, 2:D0RxDRx1I, 3:D1RxDRx0I, 4:D1RxDRx1I, 5:D0RxE, 6:D1RxE, 7:HizRx0I, 8:HizRx1I, 9:TxDRxE, 15:IOS_Masked}
      # !BSF HELP:{GPIO GpioIoState}
      # !BSF CONDITION:{$(COND_GPIO_SKIP)}
      # !BSF FIELD:{IoState:4b}

      # !BSF NAME:{HostSw}
      # !BSF TYPE:{Combo}
      # !BSF OPTION:{0:ACPI driver, 1:GPIO driver}
      # !BSF HELP:{GPIO HostSw}
      # !BSF CONDITION:{$(COND_GPIO_SKIP)}
      # !BSF FIELD:{HostSw:1b}

      # CAUTION: @#endif token MUST be placed *in front of* the BSF descriptor block to take effect!
      # !BSF CONDITION:{@#endif}
      # !BSF NAME:{WakeCtrl}
      # !BSF TYPE:{Combo}
      # !BSF OPTION:{0:Disable, 1:Enable}
      # !BSF HELP:{GPIO WakeCtrl}
      # !BSF CONDITION:{$(COND_GPIO_SKIP)}
      # !BSF FIELD:{WakeCtrl:1b}

    # !BSF DEFT:{GPIO_TMPL:END}


  # ---------------------------------------------------------------------------------------
  # !BSF DEFT:{MEMCH_TMPL:START}
    # !BSF NAME:{$(1)_RankEnable} TYPE:{EditNum, HEX, (0x00,0xFF)}
    # !BSF HELP:{Bit mask to specify what ranks are enabled for $(1)}
    gCfgData.$(1)_RankEnable      |      * | 0x01 | $(2)

    # !BSF NAME:{$(1)_DeviceWidth} TYPE:{Combo}
    # !BSF OPTION:{0x0:x8, 0x1:x16, 0x2:x32, 0x3:x64}
    # !BSF HELP:{$(1) Device Width}
    gCfgData.$(1)_DeviceWidth     |      * | 0x01 | $(3)

    # !BSF NAME:{$(1)_DramDensity} TYPE:{Combo}
    # !BSF OPTION:{0x0:4Gb, 0x1:6Gb, 0x2:8Gb, 0x3:12Gb, 0x4:16Gb}
    # !BSF HELP:{$(1) Dram Density}
    gCfgData.$(1)_DramDensity     |      * | 0x01 | $(4)

    # !BSF NAME:{$(1)_Option} TYPE:{EditNum, HEX, (0x00,0xFF)}
    # !BSF HELP:{Specify $(1) Options: BIT[0] Rank Select Interleaving Enable; BIT[1] Bank Address Hashing Enable; BIT[2] CLK Disable; BIT[5:4] Address Mapping}
    gCfgData.$(1)_Option          |      * | 0x01 | $(5)

    # !BSF NAME:{$(1)_OdtConfig} TYPE:{EditNum, HEX, (0x00,0xFF)}
    # !BSF HELP:{Specify $(1) Odt Configuration}
    gCfgData.$(1)_OdtConfig       |      * | 0x01 | $(6)

    # !BSF NAME:{$(1)_TristateClk1} TYPE:{Combo} OPTION:{$EN_DIS}
    # !BSF HELP:{Specify $(1) Tristate Clk1 (Not used)}
    gCfgData.$(1)_TristateClk1    |      * | 0x01 | $(7)

    # !BSF NAME:{$(1)_Mode2N} TYPE:{Combo}
    # !BSF OPTION:{0x0:Auto, 0x1:Force 2N CMD Timing Mode}
    # !BSF HELP:{Configures the DDR3L command timing mode for $(1)}
    gCfgData.$(1)_Mode2N          |      * | 0x01 | $(8)

    # !BSF NAME:{$(1)_OdtLevels} TYPE:{Combo}
    # !BSF OPTION:{0x0:ODT Connected to SoC, 0x1:ODT held high}
    # !BSF HELP:{Parameter used to determine if ODT will be held high or low for $(1)}
    gCfgData.$(1)_OdtLevels       |      * | 0x01 | $(9)

  # !BSF DEFT:{MEMCH_TMPL:END}


  # ---------------------------------------------------------------------------------------
  # OS_TMPL
  !include Platform/CommonBoardPkg/CfgData/Template_BootOption.dsc

  # PCIERP_TMPL
  !include Template_PcieRp.dsc

  # USB_TMPL
  !include Template_Usb.dsc

  # BOOT_MODE_GPIO_TMPL
  !include Template_DeviceEnable.dsc