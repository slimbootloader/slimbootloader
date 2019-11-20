## @file
#
#  Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
#  SPDX-License-Identifier: BSD-2-Clause-Patent
#
##

[Defines]
  # Define all conditions used in configurations
  DEFINE  COND_GPIO_SKIP            = ($$(1)_Half0.GpioSkip == 0)
  DEFINE  COND_GPIO_PID_ENABLE      = ($$(1).Enable==1) and ($PlatformId==0)
  DEFINE  COND_PCIE_RP_PWR_PIN_SKIP = ($PcieRpPower$(1).Skip == 0)
  DEFINE  COND_PCIE_RP_RST_PIN_SKIP = ($PcieRpReset$(1).Skip == 0)
  DEFINE  COND_PCIE_RP_EN           = ($PcieRpFeatures$(1).En == 1)
  DEFINE  COND_PCIE_RP_CLK_REQ_SUP  = (($PcieRpFeatures$(1).ClkReqSup == 1) and ($PcieRpFeatures$(1).En == 1))
  DEFINE  COND_HDA_EN               = ($HdaEnable == 1)
  DEFINE  COND_HDA_DSP_EN           = (($HdaEnable == 1) and ($DspEnable == 1))

[PcdsDynamicVpd.Tmp]
  !include Template_CfgData.dsc

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
  # !BSF PAGES:{PLT::"Platform", MEM::"Memory Settings", SIL::"Silicon Settings", GEN::"General Settings", GIO::"Gpio Settings", OS::"OS Boot Options"}
  # !BSF BLOCK:{NAME:"APLI Platform", VER:"0.1"}

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
  gCfgData.TotalLength            |      * | 0x04 | 0x2000

  # ---------------------------------------------------------------------------------------
  !include Platform/CommonBoardPkg/CfgData/CfgData_Platform.dsc

  # ---------------------------------------------------------------------------------------
  # !BSF PAGES:{IOCUART:PLT:"IOC Uart Settings"}
  # !BSF PAGE:{IOCUART}
  # !BSF SUBT:{CFGHDR_TMPL:IOC_UART_CFG_DATA:0:0}

  # !HDR EMBED:{IOC_UART_CFG_DATA:TAG_120:START}
  # !BSF NAME:{Device Index}
  # !BSF TYPE:{Combo}
  # !BSF OPTION:{0:UART0, 1:UART1, 2:UART2, 3:UART3, 0xF:Disable}
  # !BSF HELP:{UART device index for IOC interface (0..3 or Disable)}
  gCfgData.DeviceIndex            |      * | 0x01 | 0xF

  # !BSF NAME:{Baud Rate}
  # !BSF TYPE:{Combo}
  # !BSF OPTION:{0:9600, 1:19200, 2:38400, 3:57600, 4:115200, 5:921600, 6:1.5M }
  # !BSF HELP:{UART Baud Rate}
  gCfgData.BaudRate               |      * | 0x01 | 0

  # !BSF NAME:{Retries}
  # !BSF TYPE:{EditNum, HEX, (0x00,0xFF)}
  # !BSF HELP:{specify retry count}
  gCfgData.Retries                |      * | 0x01 | 0

  # !BSF NAME:{TimeoutInitial}
  # !BSF TYPE:{EditNum, HEX, (0x00,0xFF)}
  # !BSF HELP:{initial/setup time-out (in milliseconds)}
  gCfgData.TimeoutInitial         |      * | 0x01 | 0

  # !BSF NAME:{TimeoutXmit}
  # !BSF TYPE:{EditNum, HEX, (0x00,0xFF)}
  # !BSF HELP:{transmission time-out}
  gCfgData.TimeoutXmit            |      * | 0x01 | 0

  gCfgData.Rsvd              |      * | 0x03 | 0
  # !HDR EMBED:{IOC_UART_CFG_DATA:TAG_120:END}


  # !BSF PAGES:{PSEL:PLT:"Payload Selection GPIO"}
  # !BSF PAGE:{PSEL}

  # !BSF SUBT:{CFGHDR_TMPL:PLATFORM_CFG_DATA:0:0}
  # !HDR EMBED:{PLATFORM_CFG_DATA:TAG_280:START}
  # !BSF NAME:{GPIO pin for switching payload}
  # !HDR STRUCT:{PAYLOAD_SEL_GPIO_PIN}
  gCfgData.PayloadSelGpio         |      * | 0x04 | 0x000000c5
    # !BSF NAME:{Pin Number}
    # !BSF TYPE:{Combo}
    !include CfgData_GpioPinOption.dsc
    # !BSF CONDITION:{($PayloadSelGpio.Enable > 0)}
    # !BSF HELP:{Specify GPIO Pin Number}
    # !BSF FIELD:{PadInfo:24b}

    # !BSF NAME:{Reserved} TYPE:{Reserved}
    # !BSF FIELD:{Rsvd1:7b}

    # !BSF NAME:{Payload Selection Pin Enable} TYPE:{Combo} OPTION:{$EN_DIS}
    # !BSF HELP:{Enable/Disable this pin for payload selection.}
    # !BSF ORDER:{0000.0000}
    # !BSF FIELD:{Enable:1b}

  # !HDR EMBED:{PLATFORM_CFG_DATA:TAG_280:END}

  # ---------------------------------------------------------------------------------------
  !include CfgData_Memory.dsc

  # ---------------------------------------------------------------------------------------
  !include CfgData_Silicon.dsc

  # ---------------------------------------------------------------------------------------
  !include CfgData_Usb.dsc

  # ---------------------------------------------------------------------------------------
  !include CfgData_Gpio.dsc

  # ---------------------------------------------------------------------------------------
  !include Platform/CommonBoardPkg/CfgData/CfgData_Common.dsc

  # ---------------------------------------------------------------------------------------
  !include CfgData_BootOption.dsc

  # ---------------------------------------------------------------------------------------
  !include CfgData_PidGpioPins.dsc

  # ---------------------------------------------------------------------------------------
  !include CfgData_PcieRp.dsc

  # ---------------------------------------------------------------------------------------
  !include CfgData_GpuConfig.dsc

  # ---------------------------------------------------------------------------------------
  !include CfgData_Features.dsc

  # ---------------------------------------------------------------------------------------
  !include CfgData_DeviceEnable.dsc

  # ---------------------------------------------------------------------------------------
  !include CfgData_Hda.dsc

  # ---------------------------------------------------------------------------------------
  !include CfgData_CapsuleInformation.dsc
