## @file
#
#  Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
#  SPDX-License-Identifier: BSD-2-Clause-Patent
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

      # !BSF NAME:{Pid Pad Info}
      # !BSF TYPE:{Combo}
      # !BSF OPTION:{ 0x000C5:GPIO_0, 0x008C5:GPIO_1, 0x010C5:GPIO_2, 0x018C5:GPIO_3, 0x020C5:GPIO_4, 0x028C5:GPIO_5, 0x030C5:GPIO_6, 0x038C5:GPIO_7, }
      # !BSF OPTION:{+0x040C5:GPIO_8, 0x048C5:GPIO_9, 0x050C5:GPIO_10, 0x058C5:GPIO_11, 0x060C5:GPIO_12, 0x068C5:GPIO_13, 0x070C5:GPIO_14, 0x078C5:GPIO_15, }
      # !BSF OPTION:{+0x080C5:GPIO_16, 0x088C5:GPIO_17, 0x090C5:GPIO_18, 0x098C5:GPIO_19, 0x0A0C5:GPIO_20, 0x0A8C5:GPIO_21, 0x0B0C5:GPIO_22, 0x0B8C5:GPIO_23, }
      # !BSF OPTION:{+0x0C0C5:GPIO_24, 0x0C8C5:GPIO_25, 0x0D0C5:GPIO_26, 0x0D8C5:GPIO_27, 0x0E0C5:GPIO_28, 0x0E8C5:GPIO_29, 0x0F0C5:GPIO_30, 0x0F8C5:GPIO_31, }
      # !BSF OPTION:{+0x100C5:GPIO_32, 0x108C5:GPIO_33, 0x110C5:GPIO_34, 0x118C5:GPIO_35, 0x120C5:GPIO_36, 0x128C5:GPIO_37, 0x130C5:GPIO_38, 0x138C5:GPIO_39, }
      # !BSF OPTION:{+0x140C5:GPIO_40, 0x148C5:GPIO_41, 0x150C5:GPIO_42, 0x158C5:GPIO_43, 0x160C5:GPIO_44, 0x168C5:GPIO_45, 0x170C5:GPIO_46, 0x178C5:GPIO_47, }
      # !BSF OPTION:{+0x180C5:GPIO_48, 0x188C5:GPIO_49, 0x190C5:GPIO_62, 0x198C5:GPIO_63, 0x1A0C5:GPIO_64, 0x1A8C5:GPIO_65, 0x1B0C5:GPIO_66, 0x1B8C5:GPIO_67, }
      # !BSF OPTION:{+0x1C0C5:GPIO_68, 0x1C8C5:GPIO_69, 0x1D0C5:GPIO_70, 0x1D8C5:GPIO_71, 0x1E0C5:GPIO_72, 0x1E8C5:GPIO_73, 0x110C4:GPIO_74, 0x118C4:GPIO_75, }
      # !BSF OPTION:{+0x120C4:GPIO_76, 0x128C4:GPIO_77, 0x130C4:GPIO_78, 0x138C4:GPIO_79, 0x140C4:GPIO_80, 0x148C4:GPIO_81, 0x150C4:GPIO_82, 0x158C4:GPIO_83, }
      # !BSF OPTION:{+0x160C4:GPIO_84, 0x168C4:GPIO_85, 0x170C4:GPIO_86, 0x178C4:GPIO_87, 0x180C4:GPIO_88, 0x188C4:GPIO_89, 0x190C4:GPIO_90, 0x198C4:GPIO_91, }
      # !BSF OPTION:{+0x1A0C4:GPIO_92, 0x1A8C4:GPIO_97, 0x1B0C4:GPIO_98, 0x1B8C4:GPIO_99, 0x1C0C4:GPIO_100, 0x1C8C4:GPIO_101, 0x1D0C4:GPIO_102, 0x1D8C4:GPIO_103, }
      # !BSF OPTION:{+0x1E8C4:GPIO_104, 0x1F0C4:GPIO_105, 0x1F8C4:GPIO_106, 0x200C4:GPIO_109, 0x208C4:GPIO_110, 0x210C4:GPIO_111, 0x218C4:GPIO_112, 0x220C4:GPIO_113, }
      # !BSF OPTION:{+0x228C4:GPIO_116, 0x230C4:GPIO_117, 0x238C4:GPIO_118, 0x240C4:GPIO_119, 0x248C4:GPIO_120, 0x250C4:GPIO_121, 0x258C4:GPIO_122, 0x260C4:GPIO_123, }
      # !BSF OPTION:{+0x000C7:GPIO_124, 0x008C7:GPIO_125, 0x010C7:GPIO_126, 0x018C7:GPIO_127, 0x020C7:GPIO_128, 0x028C7:GPIO_129, 0x030C7:GPIO_130, 0x038C7:GPIO_131, }
      # !BSF OPTION:{+0x040C7:GPIO_132, 0x048C7:GPIO_133, 0x050C7:GPIO_134, 0x058C7:GPIO_135, 0x060C7:GPIO_136, 0x068C7:GPIO_137, 0x070C7:GPIO_138, 0x078C7:GPIO_139, }
      # !BSF OPTION:{+0x080C7:GPIO_146, 0x088C7:GPIO_147, 0x090C7:GPIO_148, 0x098C7:GPIO_149, 0x0A0C7:GPIO_150, 0x0A8C7:GPIO_151, 0x0B0C7:GPIO_152, 0x0B8C7:GPIO_153, }
      # !BSF OPTION:{+0x0C0C7:GPIO_154, 0x0C8C7:GPIO_155, 0x020C0:GPIO_156, 0x028C0:GPIO_157, 0x030C0:GPIO_158, 0x038C0:GPIO_159, 0x040C0:GPIO_160, 0x048C0:GPIO_161, }
      # !BSF OPTION:{+0x050C0:GPIO_162, 0x058C0:GPIO_163, 0x060C0:GPIO_164, 0x068C0:GPIO_165, 0x070C0:GPIO_166, 0x078C0:GPIO_167, 0x080C0:GPIO_168, 0x088C0:GPIO_169, }
      # !BSF OPTION:{+0x090C0:GPIO_170, 0x098C0:GPIO_171, 0x0A0C0:GPIO_172, 0x0A8C0:GPIO_179, 0x0B0C0:GPIO_173, 0x0B8C0:GPIO_174, 0x0C0C0:GPIO_175, 0x0C8C0:GPIO_176, }
      # !BSF OPTION:{+0x0D0C0:GPIO_177, 0x0D8C0:GPIO_178, 0x0E0C0:GPIO_186, 0x0E8C0:GPIO_182, 0x0F0C0:GPIO_183, 0x000C4:GPIO_187, 0x008C4:GPIO_188, 0x010C4:GPIO_189, }
      # !BSF OPTION:{+0x018C4:GPIO_190, 0x020C4:GPIO_191, 0x028C4:GPIO_192, 0x030C4:GPIO_193, 0x038C4:GPIO_194, 0x040C4:GPIO_195, 0x048C4:GPIO_196, 0x050C4:GPIO_197, }
      # !BSF OPTION:{+0x058C4:GPIO_198, 0x060C4:GPIO_199, 0x068C4:GPIO_200, 0x070C4:GPIO_201, 0x078C4:GPIO_202, 0x080C4:GPIO_203, 0x088C4:GPIO_204, 0x000C0:GPIO_205, }
      # !BSF OPTION:{+0x008C0:GPIO_206, 0x010C0:GPIO_207, 0x018C0:GPIO_208, 0x0D0C7:GPIO_209, 0x0D8C7:GPIO_210, 0x0E0C7:GPIO_211, 0x0E8C7:GPIO_212, 0x0D0C4:GPIO_213, }
      # !BSF OPTION:{+0x0D8C4:GPIO_214, 0x0E0C4:GPIO_215, 0x0F0C4:GPIO_PMIC_STDBY, 0x0C8C4:GPIO_PMIC_RESET_B, 0x0C0C4:GPIO_PMIC_PWRGOOD}
      # !BSF CONDITION:{$(COND_GPIO_PID_ENABLE)}
      # !BSF HELP:{Specify the GPIO pin}
      # !BSF FIELD:{PidPadInfo:24b}

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
      # !BSF OPTION:{0:M0, 1:M1, 2:M2, 3:M3, 4:M4, 5:M5}
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