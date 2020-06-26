## @file
#
#  Copyright (c) 2018 - 2019, Intel Corporation. All rights reserved.<BR>
#  SPDX-License-Identifier: BSD-2-Clause-Patent
#
##

[Defines]
  # Define all conditions used in configurations
  DEFINE  COND_GPIO_SKIP            = ($GpioPinConfig1_$(1).GPIOSkip_$(1) == 0)

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
  # !BSF PAGES:{PLT::"Platform", MEM::"Memory Settings", SIL::"Silicon Settings", GEN::"General Settings", GIO::"GPIO Settings", OS::"OS Boot Options", SEC::"Security Settings"}
  # !BSF BLOCK:{NAME:"CFL Platform", VER:"0.1"}

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
  !include Platform/CommonBoardPkg/CfgData/CfgData_Common.dsc

  # ---------------------------------------------------------------------------------------
  !include CfgData_Gpio.dsc

  # ---------------------------------------------------------------------------------------
  !include CfgData_PcieRp.dsc

  # ---------------------------------------------------------------------------------------
  !include CfgData_Memory.dsc

  !include CfgData_MemSpd.dsc

  # ---------------------------------------------------------------------------------------
  !include CfgData_BootOption.dsc

  # ---------------------------------------------------------------------------------------
  !include CfgData_Silicon.dsc

  # ---------------------------------------------------------------------------------------
  !include CfgData_GpuConfig.dsc

  # ---------------------------------------------------------------------------------------
  !include CfgData_CapsuleInformation.dsc

  # ---------------------------------------------------------------------------------------
  !include CfgData_Features.dsc

  # ---------------------------------------------------------------------------------------
  !include CfgData_Security.dsc

  # ---------------------------------------------------------------------------------------
  !include CfgData_SgxConfig.dsc

  # ---------------------------------------------------------------------------------------
  !include CfgData_Power.dsc
