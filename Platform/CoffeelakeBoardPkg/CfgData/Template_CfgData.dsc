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

  # Gpio CfgHdr structure
  # UINT8  GpioHeaderSize             // Size of the GPIO Hdr. CfgDataTool calculates this field
  # UINT8  GpioBaseTableId            // GPIO base table platform ID. 0xFF = it has not been processed yet.
  #                                   // CfgDataTool generates this to be either 0x80 - indicates its GPIO base table.
  #                                   // or (0-15) indicates real board id which means GPIO programming is different from base GPIO table.
  # UINT16 GpioItemSize               // Size of each GPIO entry in bytes
  # UINT16 GpioItemCount              // Number of GPIO entries in bytes
  # UINT8  GpioBaseTableBitMask[26]   // CfgDataTool will mark a bit in GpioBaseTableBitMask array to indicate a GPIO can be skipped from programming. 1 = skip, 0 = donot skip.
  # UINT8  GpioTableData[0]           // Pointer to the start of GPIO table data. CfgDataTool populates this data.
  #                                   // For DSC file, it is only a placeholder to hold the GPIOTableData.

  # GPIO Template structure
  # Dword0: GpioPinOffset
  # UINT32  PinOffset                  // 4 bytes - 32-bit pin offset address

  # Dword1: GpioPADConfig DW0
  # UINT32  PadMode         :  5;      // [4:0]     #GPIO PadMode
  # UINT32  HostSoftPadOwn  :  2;      // [6:5]     #GPIO HostSoftPadOwn
  # UINT32  Direction       :  6;      // [12:7]    #GPIO Direction
  # UINT32  OutputState     :  2;      // [14:13]   #GPIO OutputState
  # UINT32  InterruptConfig :  9;      // [23:15]   #GPIO InterruptConfig
  # UINT32  PowerConfig     :  8;      // [32:24]   #PowerConfig

  # Dword2: GpioPADConfig DW1
  # UINT32  ElectricalConfig:  9;      // [8:0]     #GPIO ElectricalConfig
  # UINT32  LockConfig      :  4;      // [12:9]    #GPIO LockConfig
  # UINT32  Reserved        :  18;     // [30:13]   #Reserved/Unused
  # UINT32  GPIOSkip        :  1;      // [31]      #Used by the CfgDataTool to check whether to include or skip custom GPIO programming

  # =============================
  # CFG DATA HDR Format
  # UINT32  ConditionNum   :  2;      // [1:0]   #of condition dwords present
  # UINT32  Length         : 10;      // [11:2]  total size of item (in dwords)
  # UINT32  Flags          :  4;      // [15:12]
  #                                   //     [12]  Reuse config from base platform ID
  #                                   //     [13]  Item array configurations
  # UINT32  Version        :  4;      // [19:16] item (payload) format version
  # UINT32  Tag            : 12;      // [31:20] identifies item (in payload)
  # UINT32  Value          : 32;      // [63:32] value

  # !BSF DEFT:{CFGHDR_TMPL:START}
    gCfgData.CfgHeader   |      * | 0x04 | {0x01:2b, ((_LENGTH_$(1)_+8)/4):10b, $(2):4b, $(3):4b, _TAG_$(1)_:12b}
    gCfgData.CondValue   |      * | 0x04 | 0x00000000
  # !BSF DEFT:{CFGHDR_TMPL:END}

  # =============================
  # !BSF DEFT:{GPIO_TMPL:START}
    # !BSF PAGES:{GIO_$(1):GIO:"GPIO $(1)"}
    # !BSF PAGE:{GIO_$(1)}

    #Dword0: GpioPinOffset (Hide it)
    # !BSF NAME:{Reserved} TYPE:{Reserved}
    # !BSF MARKER:{GpioPinOffset_$(1)}
    gCfgData.GpioPinOffset_$(1)        |      * | 0x04 | $(2)

    #Dword1: GpioPinConfig0
    # !BSF NAME:{GPIO $(1) PIN Config0}
    # !BSF TYPE:{EditNum, HEX, (0x00000000,0xFFFFFFFF)}
    # !BSF HELP:{GPIO $(1) PIN Configuration}
    gCfgData.GpioPinConfig0_$(1)              |      * | 0x04 | $(3)
      # !BSF NAME:{PadMode}
      # !BSF TYPE:{Combo}
      # !BSF OPTION:{0x0:Hardware Default, 0x1:GPIO control of the pad, 0x3:Native function 1, 0x5:Native function 2, 0x7:Native function 3, 0x9:Native function 4}
      # !BSF HELP:{GPIO PAD Mode. If GPIO is set to one of NativeX modes then following settings are not applicable and can be skipped:-}
      # !BSF HELP:{+ Interrupt related settings, Host Software Ownership, Output/Input enabling/disabling, Output lock}
      # !BSF CONDITION:{$GpioPinConfig1_$(1).Hide_$(1) == 0}
      # !BSF FIELD:{GPIOPADMode_$(1):5b}

      # !BSF NAME:{HostSoftPadOwn}
      # !BSF TYPE:{Combo}
      # !BSF OPTION:{0x0:Host Ownership Default(Leave ownership value unmodified), 0x1:Host ownership to ACPI, 0x3:Host ownership to GPIO Driver mode}
      # !BSF HELP:{GPIO Host Software Pad Ownership. This setting affects GPIO interrupt status registers. Depending on chosen ownership}
      # !BSF HELP:{+ some GPIO Interrupt status register get updated and other masked:-}
      # !BSF HELP:{+ - HOST ownership to ACPI - Use this setting if pad is not going to be used by GPIO OS driver.}
      # !BSF HELP:{+ If GPIO is configured to generate SCI/SMI/NMI then this setting must be used for interrupts to work.}
      # !BSF HELP:{+ - HOST ownership to GPIO Driver mode - Use this setting only if GPIO pad should be controlled by GPIO OS Driver.}
      # !BSF HELP:{+ GPIO OS Driver will be able to control the pad if appropriate entry in ACPI exists.}
      # !BSF CONDITION:{$GpioPinConfig1_$(1).Hide_$(1) == 0}
      # !BSF FIELD:{GPIOHostSoftPadOwn_$(1):2b}

      # !BSF NAME:{Direction}
      # !BSF TYPE:{Combo}
      # !BSF OPTION:{0x0:DirDefault, 0x9:DirInOut, 0x19:DirInInvOut, 0xB:DirIn, 0x1B:DirInInv, 0x5:DirOut, 0x7:DirNone}
      # !BSF HELP:{GPIO Host Software Pad Ownership:}
      # !BSF HELP:{+ - DirDefault = Leave pad direction setting unmodified, - DirInOut = Set pad for both output and input, }
      # !BSF HELP:{+ - DirInInvOut = Set pad for both output and input with inversion, - DirIn = Set pad for input only, }
      # !BSF HELP:{+ - DirInInv = Set pad for input with inversion, - DirOut = Set pad for output only, }
      # !BSF HELP:{+ - DirNone  = Disable both output and input. }
      # !BSF CONDITION:{$GpioPinConfig1_$(1).Hide_$(1) == 0}
      # !BSF FIELD:{GPIODirection_$(1):6b}

      # !BSF NAME:{OutputState}
      # !BSF TYPE:{Combo}
      # !BSF OPTION:{0x0:OutDefault, 0x1:OutLow, 0x3:OutHigh}
      # !BSF HELP:{GPIO Output State.This field is relevant only if output is enabled:-}
      # !BSF HELP:{+ - OutDefault = Leave output value unmodified, - OutLow = Set output to low, - OutHigh = Set output to high}
      # !BSF CONDITION:{$GpioPinConfig1_$(1).Hide_$(1) == 0}
      # !BSF FIELD:{GPIOOutputState_$(1):2b}

      # !BSF NAME:{InterruptConfig}
      # !BSF TYPE:{Combo}
      # !BSF OPTION:{0x0:IntDefault , 0x1:IntDisable, 0x3:IntNmi , 0x5:IntSmi, 0x9:IntSci, 0x11:IntApic, 0x20:IntLevel, 0x60:IntEdge, 0xA0:IntLvlEdgDis, 0xE0:IntBothEdge,}
      # !BSF OPTION:{+ 0x31:IntLevel | IntApic, 0x29:IntLevel | IntSci, 0x25:IntLevel | IntSmi, 0x69:IntEdge | IntSci, 0x71:IntEdge | IntApic, 0x65:IntEdge | IntSmi, 0x60:IntEdge | IntDefault}
      # !BSF HELP:{GPIO Interrupt Configuration - This setting is applicable only if pad is in GPIO mode and has input enabled.}
      # !BSF HELP:{+ GPIO_INT_CONFIG allows to choose which interrupt is generated (IOxAPIC/SCI/SMI/NMI) and how it is triggered (edge or level). }
      # !BSF HELP:{+ Refer to PADCFG_DW0 register description in EDS for details on this settings. Field from GpioIntNmi to GpioIntApic can be OR'ed with }
      # !BSF HELP:{+ GpioIntLevel to GpioIntBothEdge to describe an interrupt e.g. GpioIntApic | GpioIntLevel.}
      # !BSF HELP:{+ If GPIO is set to cause an SCI then also GPI_GPE_EN is enabled for this pad.}
      # !BSF HELP:{+ If GPIO is set to cause an NMI then also GPI_NMI_EN is enabled for this pad.}
      # !BSF HELP:{+ Not all GPIO are capable of generating an SMI or NMI interrupt.}
      # !BSF HELP:{+ When routing GPIO to cause an IOxAPIC interrupt care must be taken, as this interrupt cannot be shared and its IRQn number is not configurable.}
      # !BSF HELP:{+ If GPIO is under GPIO OS driver control and appropriate ACPI GpioInt descriptor exist then use only trigger type setting (from GpioIntLevel to GpioIntBothEdge).}
      # !BSF HELP:{+ This type of GPIO Driver interrupt doesn't have any additional routing setting required to be set by BIOS. Interrupt is handled by GPIO OS Driver.}
      # !BSF HELP:{+ - IntDefault = Leave value of interrupt routing unmodified, - IntDisable = Disable IOxAPIC/SCI/SMI/NMI interrupt generation,}
      # !BSF HELP:{+ - IntNmi = Enable NMI interrupt only, - IntSmi = Enable SMI interrupt only, - IntSci = Enable SCI interrupt only, - IntApic = Enable IOxAPIC interrupt only, }
      # !BSF HELP:{+ - IntLevel = Set interrupt as level triggered, - IntEdge = Set interrupt as edge triggered, - IntLvlEdgDis = Disable interrupt trigger, - IntBothEdge = Set interrupt as both edge triggered}
      # !BSF CONDITION:{$GpioPinConfig1_$(1).Hide_$(1) == 0}
      # !BSF FIELD:{GPIOInterruptConfig_$(1):9b}

      # !BSF NAME:{Power/ResetConfig}
      # !BSF TYPE:{Combo}
      # !BSF OPTION:{0x0:ResetDefault, 0x1:ResetResume, 0x3:HostDeepReset, 0x5:PlatformReset, 0x7:DswReset}
      # !BSF HELP:{GPIO Power Configuration. This allows to set GPIO Reset type which will be used to reset certain GPIO settings.}
      # !BSF HELP:{+ - ResetDefault = Leave value of pad reset unmodified, - ResumeReset = Pad setting will reset on DeepSx transition and G3,}
      # !BSF HELP:{+ - HostDeepReset = Pad settings will reset on:Warm/Cold/Global reset,DeepSx transition,G3,}
      # !BSF HELP:{+ - PlatformReset = Pad settings will reset on:S3/S4/S5 transition, Warm/Cold/Global reset, DeepSx transition, G3}
      # !BSF HELP:{+ - DswReset = Pad settings will reset on G3}
      # !BSF CONDITION:{$GpioPinConfig1_$(1).Hide_$(1) == 0}
      # !BSF FIELD:{GPIOResetConfig_$(1):8b}

    #Dword2: GpioPinConfig1
    # !BSF NAME:{GPIO $(1) PIN Config1}
    # !BSF TYPE:{EditNum, HEX, (0x00000000,0xFFFFFFFF)}
    # !BSF HELP:{GPIO $(1) PIN Configuration}
    gCfgData.GpioPinConfig1_$(1)              |      * | 0x04 | $(4)
      # !BSF NAME:{GPIO ElectricalCfg} TYPE:{Combo}
      # !BSF OPTION:{0x0:TermDefault, 0x1:TermNone, 0x5:TermNone, 0x9:TermWpd20K, 0x13:TermWpu1K, 0x17:TermWpu2K, 0x15:TermWpu5K, 0x19:TermWpu20K, 0x1B:TermWpu1K2K, 0x1F:TermNative, 0x20:NoTolerance1v8, 0x60:Tolerance1v8}
      # !BSF HELP:{GPIO Electrical Configuration. Set GPIO termination and Pad Tolerance.(applicable only for some pads):-}
      # !BSF HELP:{+ - TermDefault = Leave termination setting unmodified, - TermNone = None, - TermWpd5K = 5kOhm weak pull-down, - TermWpd20K = 20kOhm weak pull-down,}
      # !BSF HELP:{+ - TermWpu1K = 1kOhm weak pull-up, - TermWpu2K = 2kOhm weak pull-up, - TermWpu5K = 5kOhm weak pull-up, - TermWpu20K = 20kOhm weak pull-up, - TermWpu1K2K = 1kOhm & 2kOhm weak pull-up,}
      # !BSF HELP:{+ - TermNative = Native function. This setting is applicable only to some native modes,}
      # !BSF HELP:{+ - NoTolerance1v8 = Disable 1.8V pad tolerance, Tolerance1v8 = Enable 1.8V pad tolerance}
      # !BSF CONDITION:{$GpioPinConfig1_$(1).Hide_$(1) == 0}
      # !BSF FIELD:{GPIOElectricalCfg_$(1):9b}

      # !BSF NAME:{LockConfig}
      # !BSF TYPE:{Combo}
      # !BSF OPTION:{0x0:LockDefault, 0x1:PadConfigLock, 0x3:PadConfigUnlock, 0x5:PadLock , 0xC:OutputStateUnlock, 0xF:PadUnlock}
      # !BSF HELP:{GPIO Lock Configuration.}
      # !BSF HELP:{+ - LockDefault = Leave value of pad as-is, - PadConfigLock = Lock Pad configuration,}
      # !BSF HELP:{+ - PadConfigUnlock = Leave Pad configuration unlocked, - PadLock = Lock both Pad configuration and output control,}
      # !BSF HELP:{+ - OutputStateUnlock = Leave Pad output control unlocked, PadUnlock = Leave both Pad configuration and output control unlocked}
      # !BSF CONDITION:{$GpioPinConfig1_$(1).Hide_$(1) == 0}
      # !BSF FIELD:{GPIOLockConfig_$(1):4b}

      # !BSF NAME:{Reserved} TYPE:{Reserved}
      # !BSF FIELD:{Reserved_$(1):17b}

      # !BSF NAME:{Hide} TYPE:{Reserved}
      # !BSF FIELD:{Hide_$(1):1b}

      # !BSF NAME:{GPIO Skip} TYPE:{Combo} OPTION:{$EN_DIS}
      # !BSF HELP:{ENABLE: Skip GPIO PAD programming (keep the defaults). DISABLE: Program GPIO PAD configuration. }
      # !BSF ORDER:{0000.0000}
      # !BSF CONDITION:{$GpioPinConfig1_$(1).Hide_$(1) == 0}
      # !BSF FIELD:{GPIOSkip_$(1):1b}

  # !BSF DEFT:{GPIO_TMPL:END}


  # ---------------------------------------------------------------------------------------
  # OS_TMPL
  !include Platform/CommonBoardPkg/CfgData/Template_BootOption.dsc

  # ---------------------------------------------------------------------------------------
  # RP_TMPL
  !include Template_PcieRp.dsc
