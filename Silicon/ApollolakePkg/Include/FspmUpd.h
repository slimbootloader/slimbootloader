/** @file

  Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/


#ifndef __FSPMUPD_H__
#define __FSPMUPD_H__

#include <FspUpd.h>

#pragma pack(1)


#define MAX_CHANNELS_NUM 4
#define MAX_DIMMS_NUM    1

typedef struct {
  UINT8         DimmId;
  UINT32        SizeInMb;
  UINT16        MfgId;
  /** Module part number for DRR3 is 18 bytes
  but DRR4 is 20 bytes as per JEDEC Spec, so
  reserving 20 bytes **/
  UINT8         ModulePartNum[20];
} DIMM_INFO;

typedef struct {
  UINT8         ChannelId;
  UINT8         DimmCount;
  DIMM_INFO     DimmInfo[MAX_DIMMS_NUM];
} CHANNEL_INFO;

typedef struct {
  UINT8         Revision;
  UINT8         DataWidth;
  /** As defined in SMBIOS 3.0 spec
    Section 7.18.2 and Table 75
  **/
  UINT16         MemoryType;
  UINT16        MemoryFrequencyInMHz;
  /** As defined in SMBIOS 3.0 spec
    Section 7.17.3 and Table 72
  **/
  UINT8         ErrorCorrectionType;
  UINT8         ChannelCount;
  CHANNEL_INFO  ChannelInfo[MAX_CHANNELS_NUM];
} FSP_SMBIOS_MEMORY_INFO;


/** Fsp M Configuration
**/
typedef struct {

/** Offset 0x0040 - Debug Serial Port Base address
  Debug serial port base address. This option will be used only when the 'Serial Port
  Debug Device' option is set to 'External Device'. 0x00000000(Default).
**/
  UINT32                      SerialDebugPortAddress;

/** Offset 0x0044 - Debug Serial Port Type
  16550 compatible debug serial port resource type. NONE means no serial port support.
  0x02:MMIO(Default).
  0:NONE, 1:I/O, 2:MMIO
**/
  UINT8                       SerialDebugPortType;

/** Offset 0x0045 - Serial Port Debug Device
  Select active serial port device for debug. For SOC UART devices,'Debug Serial Port
  Base' options will be ignored. 0x02:SOC UART2(Default).
  0:SOC UART0, 1:SOC UART1, 2:SOC UART2, 3:External Device
**/
  UINT8                       SerialDebugPortDevice;

/** Offset 0x0046 - Debug Serial Port Stride Size
  Debug serial port register map stride size in bytes. 0x00:1, 0x02:4(Default).
  0:1, 2:4
**/
  UINT8                       SerialDebugPortStrideSize;

/** Offset 0x0047 - Memory Fast Boot
  Enable/Disable MRC fast boot support. 0x00:Disable, 0x01:Enable(Default).
  $EN_DIS
**/
  UINT8                       MrcFastBoot;

/** Offset 0x0048 - Integrated Graphics Device
  Enable : Enable Integrated Graphics Device (IGD) when selected as the Primary Video
  Adaptor. Disable: Always disable IGD. 0x00:Disable, 0x01:Enable(Default).
  $EN_DIS
**/
  UINT8                       Igd;

/** Offset 0x0049 - DVMT Pre-Allocated
  Select DVMT 5.0 Pre-Allocated (Fixed) Graphics Memory size used by the Internal
  Graphics Device. 0x02:64 MB(Default).
  0x02:64 MB, 0x03:96 MB, 0x04:128 MB, 0x05:160 MB, 0x06:192 MB, 0x07:224 MB, 0x08:256
  MB, 0x09:288 MB, 0x0A:320 MB, 0x0B:352 MB, 0x0C:384 MB, 0x0D:416 MB, 0x0E:448 MB,
  0x0F:480 MB, 0x10:512 MB
**/
  UINT8                       IgdDvmt50PreAlloc;

/** Offset 0x004A - Aperture Size
  Select the Aperture Size used by the Internal Graphics Device. 0x1:128 MB(Default),
  0x2:256 MB, 0x3:512 MB.
  0x1:128 MB, 0x2:256 MB, 0x3:512 MB
**/
  UINT8                       IgdApertureSize;

/** Offset 0x004B - GTT Size
  Select the GTT Size used by the Internal Graphics Device. 0x1:2 MB, 0x2:4 MB, 0x3:8
  MB(Default).
  0x1:2 MB, 0x2:4 MB, 0x3:8 MB
**/
  UINT8                       GttSize;

/** Offset 0x004C - Primary Display
  Select which of IGD/PCI Graphics device should be Primary Display. 0x0:AUTO(Default),
  0x2:IGD, 0x3:PCI
  0x0:AUTO, 0x2:IGD, 0x3:PCI
**/
  UINT8                       PrimaryVideoAdaptor;

/** Offset 0x004D - Package
  NOTE: Specifies CA Mapping for all technologies. Supported CA Mappings: 0 - SODIMM(Default);
  1 - BGA; 2 - BGA mirrored (LPDDR3 only); 3 - SODIMM/UDIMM with Rank 1 Mirrored
  (DDR3L); Refer to the IAFW spec for specific details about each CA mapping.
  0x0:SODIMM, 0x1:BGA, 0x2:BGA mirrored (LPDDR3 only), 0x3:SODIMM/UDIMM with Rank
  1 Mirrored (DDR3L)
**/
  UINT8                       Package;

/** Offset 0x004E - Profile
  Profile list. 0x19(Default).
  0x1:WIO2_800_7_8_8, 0x2:WIO2_1066_9_10_10, 0x3:LPDDR3_1066_8_10_10, 0x4:LPDDR3_1333_10_12_12,
  0x5:LPDDR3_1600_12_15_15, 0x6:LPDDR3_1866_14_17_17, 0x7:LPDDR3_2133_16_20_20, 0x8:LPDDR4_1066_10_10_10,
  0x9:LPDDR4_1600_14_15_15, 0xA:LPDDR4_2133_20_20_20, 0xB:LPDDR4_2400_24_22_22, 0xC:LPDDR4_2666_24_24_24,
  0xD:LPDDR4_2933_28_27_27, 0xE:LPDDR4_3200_28_29_29, 0xF:DDR3_1066_6_6_6, 0x10:DDR3_1066_7_7_7,
  0x11:DDR3_1066_8_8_8, 0x12:DDR3_1333_7_7_7, 0x13:DDR3_1333_8_8_8, 0x14:DDR3_1333_9_9_9,
  0x15:DDR3_1333_10_10_10, 0x16:DDR3_1600_8_8_8, 0x17:DDR3_1600_9_9_9, 0x18:DDR3_1600_10_10_10,
  0x19:DDR3_1600_11_11_11, 0x1A:DDR3_1866_10_10_10, 0x1B:DDR3_1866_11_11_11, 0x1C:DDR3_1866_12_12_12,
  0x1D:DDR3_1866_13_13_13, 0x1E:DDR3_2133_11_11_11, 0x1F:DDR3_2133_12_12_12, 0x20:DDR3_2133_13_13_13,
  0x21:DDR3_2133_14_14_14, 0x22:DDR4_1333_10_10_10, 0x23:DDR4_1600_10_10_10, 0x24:DDR4_1600_11_11_11,
  0x25:DDR4_1600_12_12_12, 0x26:DDR4_1866_12_12_12, 0x27:DDR4_1866_13_13_13, 0x28:DDR4_1866_14_14_14,
  0x29:DDR4_2133_14_14_14, 0x2A:DDR4_2133_15_15_15, 0x2B:DDR4_2133_16_16_16, 0x2C:DDR4_2400_15_15_15,
  0x2D:DDR4_2400_16_16_16, 0x2E:DDR4_2400_17_17_17, 0x2F:DDR4_2400_18_18_18
**/
  UINT8                       Profile;

/** Offset 0x004F - MemoryDown
  Memory Down. 0x0(Default).
  0x0:No, 0x1:Yes, 0x2:1MD+SODIMM (for DDR3L only) ACRD, 0x3:1x32 LPDDR4
**/
  UINT8                       MemoryDown;

/** Offset 0x0050 - DDR3LPageSize
  NOTE: Only for memory down (soldered down memory with no SPD). 0x01:1KB(Default), 0x02:2KB.
  0x1:1KB, 0x2:2KB
**/
  UINT8                       DDR3LPageSize;

/** Offset 0x0051 - DDR3LASR
  NOTE: Only for memory down. This is specific to ddr3l and used for refresh adjustment
  in Self Refresh, does not affect LP4. 0x00:Not Supported(Default), 0x01:Supported.
  0x0:Not Supported, 0x1:Supported
**/
  UINT8                       DDR3LASR;

/** Offset 0x0052 - ScramblerSupport
  Scrambler Support -  Enable or disable the memory scrambler. Data scrambling is
  provided as a means to increase signal integrity/reduce RFI generated by the DRAM
  interface. This is achieved by randomizing seed that encodes/decodes memory data
  so repeating a worse case pattern is hard to repeat. 00: Disable Scrambler Support,
  01: Enable Scrambler Support
  $EN_DIS
**/
  UINT8                       ScramblerSupport;

/** Offset 0x0053 - InterleavedMode
  This field is ignored if one of the PnP channel configurations is used. If the memory
  configuration is different, then the field is used directly to populate. 0x00:Disable(Default),
  0x02:Enable.
  0x0:Disable, 0x2:Enable
**/
  UINT8                       InterleavedMode;

/** Offset 0x0054 - ChannelHashMask
  ChannelHashMask and SliceHashMask allow for the channel hashing algorithm to be
  modified. These inputs are not used for configurations where an optimized ChannelHashMask
  has been provided by the PnP validation teams. 0x00(Default).
**/
  UINT16                      ChannelHashMask;

/** Offset 0x0056 - SliceHashMask
  ChannelHashMask and SliceHashMask allow for the channel hashing algorithm to be
  modified. These inputs are not used for configurations where an optimized ChannelHashMask
  has been provided by the PnP validation teams. 0x00(Default).
**/
  UINT16                      SliceHashMask;

/** Offset 0x0058 - ChannelsSlicesEnable
  ChannelSlicesEnable field is not used at all on BXTP. The Channel Slice Configuration
  is calculated internally based on the enabled channel configuration. 0x00:Disable(Default),
  0x01:Enable.
  $EN_DIS
**/
  UINT8                       ChannelsSlicesEnable;

/** Offset 0x0059 - MinRefRate2xEnable
  Provided as a means to defend against Row-Hammer attacks. 0x00:Disable(Default),
  0x01:Enable.
  $EN_DIS
**/
  UINT8                       MinRefRate2xEnable;

/** Offset 0x005A - DualRankSupportEnable
  Dual Rank Support Enable. 0x00:Disable, 0x01:Enable(Default).
  $EN_DIS
**/
  UINT8                       DualRankSupportEnable;

/** Offset 0x005B - RmtMode
  Rank Margin Tool Mode. 0x00(Default), 0x3(Enabled).
  0x0:Disabled, 0x3:Enabled
**/
  UINT8                       RmtMode;

/** Offset 0x005C - MemorySizeLimit
  Memory Size Limit: This value is used to restrict the total amount of memory and
  the calculations based on it. Value is in MB. Example encodings are: 0x400 = 1GB,
  0x800 = 2GB, 0x1000 = 4GB, 0x2000 8GB. 0x0000(Default)
**/
  UINT16                      MemorySizeLimit;

/** Offset 0x005E - LowMemoryMaxValue
  Low Memory Max Value: This value is used to restrict the amount of memory below
  4GB and the calculations based on it. Value is in MB.Example encodings are: 0x400
  = 1GB, 0x800 = 2GB, 0x1000 = 4GB, 0x2000 8GB. 0x0000(Default).
**/
  UINT16                      LowMemoryMaxValue;

/** Offset 0x0060 - HighMemoryMaxValue
  High Memory Max Value: This value is used to restrict the amount of memory above
  4GB and the calculations based on it. Value is in MB. Example encodings are: 0x0400:1GB,
  0x0800:2GB, 0x1000:4GB, 0x2000:8GB. 0x00(Default).
**/
  UINT16                      HighMemoryMaxValue;

/** Offset 0x0062 - DisableFastBoot
  00:Disabled; Use saved training data (if valid) after first boot(Default), 01:Enabled;
  Full re-train of memory on every boot.
  $EN_DIS
**/
  UINT8                       DisableFastBoot;

/** Offset 0x0063 - DIMM0SPDAddress
  DIMM0 SPD Address (NOTE: Only for DDR3L only. Please put 0 for MemoryDown. 0xA0(Default).
**/
  UINT8                       DIMM0SPDAddress;

/** Offset 0x0064 - DIMM1SPDAddress
  DIMM1 SPD Address (NOTE: Only for DDR3L only. Please put 0 for MemoryDown. 0xA4(Default).
**/
  UINT8                       DIMM1SPDAddress;

/** Offset 0x0065 - Ch0_RankEnable
  NOTE: Only for memory down. This is a bit mask which specifies what ranks are enabled.
  NOTE: Only for memory down (soldered down memory with no SPD): BIT[0] Enable Rank
  0:  Must be set to 1 to enable use of this rank; BIT1[1] Enable Rank 1:  Must be
  set to 1 to enable use of this rank.
**/
  UINT8                       Ch0_RankEnable;

/** Offset 0x0066 - Ch0_DeviceWidth
  NOTE: Only for memory down. Must specify the DRAM device width per DRAM channel
  (not to be confused with the SoC Memory Channel width which is always x32 for LPDDR3\LPDDR4
  and x64 for DDR3L). LPDDR4 devices typically have two channels per die and a x16
  device width: 00 - x8; 01 - x16; 10 - x32; 11 - x64
  0b0000:x8, 0b0001:x16, 0b0010:x32, 0b0011:x64
**/
  UINT8                       Ch0_DeviceWidth;

/** Offset 0x0067 - Ch0_DramDensity
  NOTE: Only for memory down. For LPDDR3 and LPDDR4: Must specify the DRAM device
  density per rank (per Chip Select). The simplest way of identifying the density
  per rank is to divide the total SoC memory channel density by the number of ranks.
  For DDR3L: Must specify the DRAM device density per DRAM device. For example, an
  8GB 2Rx8 configuration will utilize sixteen 4Gb density DRAMS. In this configuration,
  a 4Gb density setting would be selected in the MRC: 000 - 4Gb; 001 - 6Gb; 010 -
  8Gb; 011 - 12Gb; 100 - 16Gb; 101 - 2Gb; 110-111 - Reserved
  0b0000:4Gb, 0b0001:6Gb, 0b0010:8Gb, 0b0011:12Gb, 0b0100:16Gb
**/
  UINT8                       Ch0_DramDensity;

/** Offset 0x0068 - Ch0_Option
  BIT[0] Rank Select Interleaving Enable.  See Address Mapping section for full description:
  0 - Rank Select Interleaving disabled; 1 - Rank Select Interleaving enabled. BIT[1]
  Bank Address Hashing Enable. See Address Mapping section for full description:
  0 - Bank Address Hashing disabled; 1 - Bank Address Hashing enabled. BIT[2] CH1
  CLK Disable. Disables the CH1 CLK PHY Signal when set to 1. This is used on board
  designs where the CH1 CLK is not routed and left floating or stubbed out: 0 - CH1
  CLK is enabled; 1 -  CH1 CLK is disabled. BIT[3] Reserved; BIT[5:4] This register
  specifies the address mapping to be used: 00 - 1KB (A); 01 - 2KB (B)
**/
  UINT8                       Ch0_Option;

/** Offset 0x0069 - Ch0_OdtConfig
  [0] RX ODT - DDR3L & LPDDR3 only: Change the READ ODT strength , for SOC termination
  during a READ transaction, ON DQ BITs. STRONG ==> 60 OHMS roughly, WEAK ==> 120
  OHMS or so roughly. Purpose: Save power on these technologies which burn power
  directly proportional to ODT strength, because ODT looks like a PU and PD (e.g.
  a resistor divider, which always burns power when ODT is ON).   0 - WEAK_ODT_CONFIG,
  1 - STRONG_ODT_CONFIG. LPDDR4:   X - Don't Care. [1] CA ODT - LPDDR4 Only: The
  customer needs to choose this based on their actual board strapping (how they tie
  the DRAM's ODT PINs). Effect: LPDDR4 MR11 will be set based on this setting. CAODT_A_B_HIGH_LOW
  ==> MR11 = 0x34, which is CA ODT = 80 ohms. CAODT_A_B_HIGH_HIGH ==> MR11 = 0x24,
  which is CA ODT = 120 ohms (results in 60 ohm final   effective impedance on CA/CLK/CS
  signals). Purpose: To improve signal integrity and provide a much more optimized
  CA VREF value during training.  Not to save power.  0 - ODT_AB_HIGH_LOW (default),
  1 - ODT_AB_HIGH_HIGH. DDR3L & LPDDR3:  X - Don't Care. [4] TX ODT. DDR3L only:
  0 = RZQ/4 (60 Ohms)  = MRC_SMIP_DDR3L_TX_ODT_RTT_WR_60_OHMS,   1 = RZQ/2 (120
  Ohms) = MRC_SMIP_DDR3L_TX_ODT_RTT_WR_120_OHMS. LPDDR3 & LPDDR4:   X = Don't Care
**/
  UINT8                       Ch0_OdtConfig;

/** Offset 0x006A - Ch0_TristateClk1
  Not used
**/
  UINT8                       Ch0_TristateClk1;

/** Offset 0x006B - Ch0_Mode2N
  DDR3L Only: Configures the DDR3L command timing mode. 2N Mode is a stretched command
  mode that provides more setup and hold time for DRAM commands on the DRAM command
  bus. This is useful for platforms with unusual CMD bus routing or marginal signal
  integrity: 0 - Auto (1N or 2N mode is automatically selected during Command and
  Control training), 1 - Force 2N Mode
  0x0:Auto, 0x1:Force 2N CMD Timing Mode
**/
  UINT8                       Ch0_Mode2N;

/** Offset 0x006C - Ch0_OdtLevels
  Parameter used to determine if ODT will be held high or low: 0 - ODT Connected to
  SoC, 1 - ODT held high
**/
  UINT8                       Ch0_OdtLevels;

/** Offset 0x006D - Ch1_RankEnable
  NOTE: Only for memory down. This is a bit mask which specifies what ranks are enabled.
  NOTE: Only for memory down (soldered down memory with no SPD): BIT[0] Enable Rank
  0:  Must be set to 1 to enable use of this rank; BIT1[1] Enable Rank 1:  Must be
  set to 1 to enable use of this rank.
**/
  UINT8                       Ch1_RankEnable;

/** Offset 0x006E - Ch1_DeviceWidth
  NOTE: Only for memory down. Must specify the DRAM device width per DRAM channel
  (not to be confused with the SoC Memory Channel width which is always x32 for LPDDR3\LPDDR4
  and x64 for DDR3L). LPDDR4 devices typically have two channels per die and a x16
  device width: 00 - x8; 01 - x16; 10 - x32; 11 - x64
  0b0000:x8, 0b0001:x16, 0b0010:x32, 0b0011:x64
**/
  UINT8                       Ch1_DeviceWidth;

/** Offset 0x006F - Ch1_DramDensity
  NOTE: Only for memory down. For LPDDR3 and LPDDR4: Must specify the DRAM device
  density per rank (per Chip Select). The simplest way of identifying the density
  per rank is to divide the total SoC memory channel density by the number of ranks.
  For DDR3L: Must specify the DRAM device density per DRAM device. For example, an
  8GB 2Rx8 configuration will utilize sixteen 4Gb density DRAMS. In this configuration,
  a 4Gb density setting would be selected in the MRC: 000 - 4Gb; 001 - 6Gb; 010 -
  8Gb; 011 - 12Gb; 100 - 16Gb; 101 - 2Gb; 110-111 - Reserved
  0b0000:4Gb, 0b0001:6Gb, 0b0010:8Gb, 0b0011:12Gb, 0b0100:16Gb
**/
  UINT8                       Ch1_DramDensity;

/** Offset 0x0070 - Ch1_Option
  BIT[0] Rank Select Interleaving Enable.  See Address Mapping section for full description:
  0 - Rank Select Interleaving disabled; 1 - Rank Select Interleaving enabled. BIT[1]
  Bank Address Hashing Enable. See Address Mapping section for full description:
  0 - Bank Address Hashing disabled; 1 - Bank Address Hashing enabled. BIT[2] CH1
  CLK Disable. Disables the CH1 CLK PHY Signal when set to 1. This is used on board
  designs where the CH1 CLK is not routed and left floating or stubbed out: 0 - CH1
  CLK is enabled; 1 -  CH1 CLK is disabled. BIT[3] Reserved; BIT[5:4] This register
  specifies the address mapping to be used: 00 - 1KB (A); 01 - 2KB (B)
**/
  UINT8                       Ch1_Option;

/** Offset 0x0071 - Ch1_OdtConfig
  [0] RX ODT - DDR3L & LPDDR3 only: Change the READ ODT strength , for SOC termination
  during a READ transaction, ON DQ BITs. STRONG ==> 60 OHMS roughly, WEAK ==> 120
  OHMS or so roughly. Purpose: Save power on these technologies which burn power
  directly proportional to ODT strength, because ODT looks like a PU and PD (e.g.
  a resistor divider, which always burns power when ODT is ON).   0 - WEAK_ODT_CONFIG,
  1 - STRONG_ODT_CONFIG. LPDDR4:   X - Don't Care. [1] CA ODT - LPDDR4 Only: The
  customer needs to choose this based on their actual board strapping (how they tie
  the DRAM's ODT PINs). Effect: LPDDR4 MR11 will be set based on this setting. CAODT_A_B_HIGH_LOW
  ==> MR11 = 0x34, which is CA ODT = 80 ohms. CAODT_A_B_HIGH_HIGH ==> MR11 = 0x24,
  which is CA ODT = 120 ohms (results in 60 ohm final   effective impedance on CA/CLK/CS
  signals). Purpose: To improve signal integrity and provide a much more optimized
  CA VREF value during training.  Not to save power.  0 - ODT_AB_HIGH_LOW (default),
  1 - ODT_AB_HIGH_HIGH. DDR3L & LPDDR3:  X - Don't Care. [4] TX ODT. DDR3L only:
  0 = RZQ/4 (60 Ohms)  = MRC_SMIP_DDR3L_TX_ODT_RTT_WR_60_OHMS,   1 = RZQ/2 (120
  Ohms) = MRC_SMIP_DDR3L_TX_ODT_RTT_WR_120_OHMS. LPDDR3 & LPDDR4:   X = Don't Care
**/
  UINT8                       Ch1_OdtConfig;

/** Offset 0x0072 - Ch1_TristateClk1
  Not used
**/
  UINT8                       Ch1_TristateClk1;

/** Offset 0x0073 - Ch1_Mode2N
  DDR3L Only: Configures the DDR3L command timing mode. 2N Mode is a stretched command
  mode that provides more setup and hold time for DRAM commands on the DRAM command
  bus. This is useful for platforms with unusual CMD bus routing or marginal signal
  integrity: 0 - Auto (1N or 2N mode is automatically selected during Command and
  Control training), 1 - Force 2N Mode
  0x0:Auto, 0x1:Force 2N CMD Timing Mode
**/
  UINT8                       Ch1_Mode2N;

/** Offset 0x0074 - Ch1_OdtLevels
  DDR3L Only: Parameter used to determine if ODT will be held high or low: 0 - ODT_AB_HIGH_LOW
  (default), 1 - ODT_AB_HIGH_HIGH
**/
  UINT8                       Ch1_OdtLevels;

/** Offset 0x0075 - Ch2_RankEnable
  NOTE: Only for memory down. This is a bit mask which specifies what ranks are enabled.
  NOTE: Only for memory down (soldered down memory with no SPD): BIT[0] Enable Rank
  0:  Must be set to 1 to enable use of this rank; BIT1[1] Enable Rank 1:  Must be
  set to 1 to enable use of this rank.
**/
  UINT8                       Ch2_RankEnable;

/** Offset 0x0076 - Ch2_DeviceWidth
  NOTE: Only for memory down. Must specify the DRAM device width per DRAM channel
  (not to be confused with the SoC Memory Channel width which is always x32 for LPDDR3\LPDDR4
  and x64 for DDR3L). LPDDR4 devices typically have two channels per die and a x16
  device width: 00 - x8; 01 - x16; 10 - x32; 11 - x64
  0b0000:x8, 0b0001:x16, 0b0010:x32, 0b0011:x64
**/
  UINT8                       Ch2_DeviceWidth;

/** Offset 0x0077 - Ch2_DramDensity
  NOTE: Only for memory down. For LPDDR3 and LPDDR4: Must specify the DRAM device
  density per rank (per Chip Select). The simplest way of identifying the density
  per rank is to divide the total SoC memory channel density by the number of ranks.
  For DDR3L: Must specify the DRAM device density per DRAM device. For example, an
  8GB 2Rx8 configuration will utilize sixteen 4Gb density DRAMS. In this configuration,
  a 4Gb density setting would be selected in the MRC: 000 - 4Gb; 001 - 6Gb; 010 -
  8Gb; 011 - 12Gb; 100 - 16Gb; 101 - 2Gb; 110-111 - Reserved
  0b0000:4Gb, 0b0001:6Gb, 0b0010:8Gb, 0b0011:12Gb, 0b0100:16Gb
**/
  UINT8                       Ch2_DramDensity;

/** Offset 0x0078 - Ch2_Option
  BIT[0] Rank Select Interleaving Enable.  See Address Mapping section for full description:
  0 - Rank Select Interleaving disabled; 1 - Rank Select Interleaving enabled. BIT[1]
  Bank Address Hashing Enable. See Address Mapping section for full description:
  0 - Bank Address Hashing disabled; 1 - Bank Address Hashing enabled. BIT[2] CH1
  CLK Disable. Disables the CH1 CLK PHY Signal when set to 1. This is used on board
  designs where the CH1 CLK is not routed and left floating or stubbed out: 0 - CH1
  CLK is enabled; 1 -  CH1 CLK is disabled. BIT[3] Reserved; BIT[5:4] This register
  specifies the address mapping to be used: 00 - 1KB (A); 01 - 2KB (B)
**/
  UINT8                       Ch2_Option;

/** Offset 0x0079 - Ch2_OdtConfig
  [0] RX ODT - DDR3L & LPDDR3 only: Change the READ ODT strength , for SOC termination
  during a READ transaction, ON DQ BITs. STRONG ==> 60 OHMS roughly, WEAK ==> 120
  OHMS or so roughly. Purpose: Save power on these technologies which burn power
  directly proportional to ODT strength, because ODT looks like a PU and PD (e.g.
  a resistor divider, which always burns power when ODT is ON).   0 - WEAK_ODT_CONFIG,
  1 - STRONG_ODT_CONFIG. LPDDR4:   X - Don't Care. [1] CA ODT - LPDDR4 Only: The
  customer needs to choose this based on their actual board strapping (how they tie
  the DRAM's ODT PINs). Effect: LPDDR4 MR11 will be set based on this setting. CAODT_A_B_HIGH_LOW
  ==> MR11 = 0x34, which is CA ODT = 80 ohms. CAODT_A_B_HIGH_HIGH ==> MR11 = 0x24,
  which is CA ODT = 120 ohms (results in 60 ohm final   effective impedance on CA/CLK/CS
  signals). Purpose: To improve signal integrity and provide a much more optimized
  CA VREF value during training.  Not to save power.  0 - ODT_AB_HIGH_LOW (default),
  1 - ODT_AB_HIGH_HIGH. DDR3L & LPDDR3:  X - Don't Care. [4] TX ODT. DDR3L only:
  0 = RZQ/4 (60 Ohms)  = MRC_SMIP_DDR3L_TX_ODT_RTT_WR_60_OHMS,   1 = RZQ/2 (120
  Ohms) = MRC_SMIP_DDR3L_TX_ODT_RTT_WR_120_OHMS. LPDDR3 & LPDDR4:   X = Don't Care
**/
  UINT8                       Ch2_OdtConfig;

/** Offset 0x007A - Ch2_TristateClk1
  Not used
**/
  UINT8                       Ch2_TristateClk1;

/** Offset 0x007B - Ch2_Mode2N
  DDR3L Only: Configures the DDR3L command timing mode. 2N Mode is a stretched command
  mode that provides more setup and hold time for DRAM commands on the DRAM command
  bus. This is useful for platforms with unusual CMD bus routing or marginal signal
  integrity: 0 - Auto (1N or 2N mode is automatically selected during Command and
  Control training), 1 - Force 2N Mode
  0x0:Auto, 0x1:Force 2N CMD Timing Mode
**/
  UINT8                       Ch2_Mode2N;

/** Offset 0x007C - Ch2_OdtLevels
  DDR3L Only: Parameter used to determine if ODT will be held high or low: 0 - ODT_AB_HIGH_LOW
  (default), 1 - ODT_AB_HIGH_HIGH
**/
  UINT8                       Ch2_OdtLevels;

/** Offset 0x007D - Ch3_RankEnable
  NOTE: Only for memory down. This is a bit mask which specifies what ranks are enabled.
  NOTE: Only for memory down (soldered down memory with no SPD): BIT[0] Enable Rank
  0:  Must be set to 1 to enable use of this rank; BIT1[1] Enable Rank 1:  Must be
  set to 1 to enable use of this rank.
**/
  UINT8                       Ch3_RankEnable;

/** Offset 0x007E - Ch3_DeviceWidth
  NOTE: Only for memory down. Must specify the DRAM device width per DRAM channel
  (not to be confused with the SoC Memory Channel width which is always x32 for LPDDR3\LPDDR4
  and x64 for DDR3L). LPDDR4 devices typically have two channels per die and a x16
  device width: 00 - x8; 01 - x16; 10 - x32; 11 - x64
  0b0000:x8, 0b0001:x16, 0b0010:x32, 0b0011:x64
**/
  UINT8                       Ch3_DeviceWidth;

/** Offset 0x007F - Ch3_DramDensity
  NOTE: Only for memory down. For LPDDR3 and LPDDR4: Must specify the DRAM device
  density per rank (per Chip Select). The simplest way of identifying the density
  per rank is to divide the total SoC memory channel density by the number of ranks.
  For DDR3L: Must specify the DRAM device density per DRAM device. For example, an
  8GB 2Rx8 configuration will utilize sixteen 4Gb density DRAMS. In this configuration,
  a 4Gb density setting would be selected in the MRC: 000 - 4Gb; 001 - 6Gb; 010 -
  8Gb; 011 - 12Gb; 100 - 16Gb; 101 - 2Gb; 110-111 - Reserved
  0b0000:4Gb, 0b0001:6Gb, 0b0010:8Gb, 0b0011:12Gb, 0b0100:16Gb
**/
  UINT8                       Ch3_DramDensity;

/** Offset 0x0080 - Ch3_Option
  BIT[0] Rank Select Interleaving Enable.  See Address Mapping section for full description:
  0 - Rank Select Interleaving disabled; 1 - Rank Select Interleaving enabled. BIT[1]
  Bank Address Hashing Enable. See Address Mapping section for full description:
  0 - Bank Address Hashing disabled; 1 - Bank Address Hashing enabled. BIT[2] CH1
  CLK Disable. Disables the CH1 CLK PHY Signal when set to 1. This is used on board
  designs where the CH1 CLK is not routed and left floating or stubbed out: 0 - CH1
  CLK is enabled; 1 -  CH1 CLK is disabled. BIT[3] Reserved; BIT[5:4] This register
  specifies the address mapping to be used: 00 - 1KB (A); 01 - 2KB (B)
**/
  UINT8                       Ch3_Option;

/** Offset 0x0081 - Ch3_OdtConfig
  [0] RX ODT - DDR3L & LPDDR3 only: Change the READ ODT strength , for SOC termination
  during a READ transaction, ON DQ BITs. STRONG ==> 60 OHMS roughly, WEAK ==> 120
  OHMS or so roughly. Purpose: Save power on these technologies which burn power
  directly proportional to ODT strength, because ODT looks like a PU and PD (e.g.
  a resistor divider, which always burns power when ODT is ON).   0 - WEAK_ODT_CONFIG,
  1 - STRONG_ODT_CONFIG. LPDDR4:   X - Don't Care. [1] CA ODT - LPDDR4 Only: The
  customer needs to choose this based on their actual board strapping (how they tie
  the DRAM's ODT PINs). Effect: LPDDR4 MR11 will be set based on this setting. CAODT_A_B_HIGH_LOW
  ==> MR11 = 0x34, which is CA ODT = 80 ohms. CAODT_A_B_HIGH_HIGH ==> MR11 = 0x24,
  which is CA ODT = 120 ohms (results in 60 ohm final   effective impedance on CA/CLK/CS
  signals). Purpose: To improve signal integrity and provide a much more optimized
  CA VREF value during training.  Not to save power.  0 - ODT_AB_HIGH_LOW (default),
  1 - ODT_AB_HIGH_HIGH. DDR3L & LPDDR3:  X - Don't Care. [4] TX ODT. DDR3L only:
  0 = RZQ/4 (60 Ohms)  = MRC_SMIP_DDR3L_TX_ODT_RTT_WR_60_OHMS,   1 = RZQ/2 (120
  Ohms) = MRC_SMIP_DDR3L_TX_ODT_RTT_WR_120_OHMS. LPDDR3 & LPDDR4:   X = Don't Care
**/
  UINT8                       Ch3_OdtConfig;

/** Offset 0x0082 - Ch3_TristateClk1
  Not used
**/
  UINT8                       Ch3_TristateClk1;

/** Offset 0x0083 - Ch3_Mode2N
  DDR3L Only: Configures the DDR3L command timing mode. 2N Mode is a stretched command
  mode that provides more setup and hold time for DRAM commands on the DRAM command
  bus. This is useful for platforms with unusual CMD bus routing or marginal signal
  integrity: 0 - Auto (1N or 2N mode is automatically selected during Command and
  Control training), 1 - Force 2N Mode
  0x0:Auto, 0x1:Force 2N CMD Timing Mode
**/
  UINT8                       Ch3_Mode2N;

/** Offset 0x0084 - Ch3_OdtLevels
  DDR3L Only: Parameter used to determine if ODT will be held high or low: 0 - ODT_AB_HIGH_LOW
  (default), 1 - ODT_AB_HIGH_HIGH
**/
  UINT8                       Ch3_OdtLevels;

/** Offset 0x0085 - RmtCheckRun
  Parameter used to determine whether to run the margin check. Bit 0 is used for MINIMUM
  MARGIN CHECK and bit 1 is used for DEGRADE MARGIN CHECK
**/
  UINT8                       RmtCheckRun;

/** Offset 0x0086 - RmtMarginCheckScaleHighThreshold
  Percentage used to determine the margin tolerances over the failing margin.
**/
  UINT16                      RmtMarginCheckScaleHighThreshold;

/** Offset 0x0088 - Ch0_Bit_swizzling
  Channel 0 PHY to DUnit DQ mapping (only used if not 1-1 mapping)Range: 0-32. Frequently
  asked questions: Q: The DQS (strobes) need to go with the corresponding byte lanes
  on the DDR module. Are the DQS being swapped around as well? Ans: Yes, DQ strobes
  need to follow the DQ byte lane they correspond too. So for example if you have
  DQ[7:0] swapped with DQ[15:8], DQS0 pair also need to be swapped with DQS1 pair.
  Also, the spreadsheet used for Amenia is essentially a swizzle value lookup that
  specifies what DRAM DQ bit a particular SoC DQ bit is connected to. Some confusion
  can arrise from the fact that the indexes to the array do not necessarily map 1:1
  to an SoC DQ pin. For example, the CH0 array at index 0 maps to SoC DQB8. The value
  of 9 at index 0 tells us that SoC DQB8 is connected to DRAM DQA9. Q: The PDG indicates
  a 2 physical channels need to be stuffed and operated together. Are the CHx_A and
  CHx_B physical channels operated in tandem or completely separate? If separate,
  why requirement of pairing them? Ans: We have 2 PHY instances on the SoC each supporting
  up to 2 x32 LP4 channels. If you have 4 channels both PHYs are active, but if you
  have 2 channels in order to power gate one PHY, those two channel populated must
  be on one PHY instance. So yes all channels are independent of each other, but
  there are some restrictions on how they need to be populated. Q: How is it that
  an LPDDR4 device is identified as having a x16 width when all 32-bits are used
  at the same time with a single chip select? That's effectively a x32 device. Ans:LPDDR4
  DRAM devices are x16. Each die has 2 x16 devices on them. To make a x32 channel
  the CS of the two devices in the same die are connected together to make a single
  rank of one x32 channel (SDP). The second die in the DDP package makes the second rank.
**/
  UINT8                       Ch0_Bit_swizzling[32];

/** Offset 0x00A8 - Ch1_Bit_swizzling
  Channel 1 PHY to DUnit DQ mapping (only used if not 1-1 mapping)Range: 0-32.
**/
  UINT8                       Ch1_Bit_swizzling[32];

/** Offset 0x00C8 - Ch2_Bit_swizzling
  Channel 2 PHY to DUnit DQ mapping (only used if not 1-1 mapping)Range: 0-32.
**/
  UINT8                       Ch2_Bit_swizzling[32];

/** Offset 0x00E8 - Ch3_Bit_swizzling
  Channel 3 PHY to DUnit DQ mapping (only used if not 1-1 mapping)Range: 0-32.
**/
  UINT8                       Ch3_Bit_swizzling[32];

/** Offset 0x0108 - MsgLevelMask
  32 bits used to mask out debug messages. Masking out bit 0 mask all other messages.
**/
  UINT32                      MsgLevelMask;

/** Offset 0x010C
**/
  UINT8                       UnusedUpdSpace0[4];

/** Offset 0x0110 - PreMem GPIO Pin Number for each table
  Number of Pins in each PreMem GPIO Table. 0(Default).
**/
  UINT8                       PreMemGpioTablePinNum[4];

/** Offset 0x0114 - PreMem GPIO Table Pointer
  Pointer to Array of pointers to PreMem GPIO Table. 0x00000000(Default).
**/
  UINT32                      PreMemGpioTablePtr;

/** Offset 0x0118 - PreMem GPIO Table Entry Number. Currently maximum entry number is 4
  Number of Entries in PreMem GPIO Table. 0(Default).
**/
  UINT8                       PreMemGpioTableEntryNum;

/** Offset 0x0119 - Enhance the port 8xh decoding
  Enable/Disable Enhance the port 8xh decoding. 0:Disable, 1:Enable(Default).
  $EN_DIS
**/
  UINT8                       EnhancePort8xhDecoding;

/** Offset 0x011A - SPD Data Write
  Enable/Disable SPD data write on the SMBUS. 0x00:Disable(Default), 0x01:Enable.
  $EN_DIS
**/
  UINT8                       SpdWriteEnable;

/** Offset 0x011B - MRC Training Data Saving
  Enable/Disable MRC training data saving in FSP. 0x00:Disable(Default), 0x01:Enable.
  $EN_DIS
**/
  UINT8                       MrcDataSaving;

/** Offset 0x011C - OEM File Loading Address
  Determine the memory base address to load a specified file from CSE file system
  after memory is available.
**/
  UINT32                      OemLoadingBase;

/** Offset 0x0120 - OEM File Name to Load
  Specify a file name to load from CSE file system after memory is available. Empty
  indicates no file needs to be loaded.
**/
  UINT8                       OemFileName[16];

/** Offset 0x0130
**/
  VOID*                       MrcBootDataPtr;

/** Offset 0x0134 - eMMC Trace Length
  Select eMMC trace length to load OEM file from when loading OEM file name is specified.
  0x0:Long(Default), 0x1:Short.
  0x0:Long, 0x1:Short
**/
  UINT8                       eMMCTraceLen;

/** Offset 0x0135 - Skip CSE RBP to support zero sized IBB
  Enable/Disable skip CSE RBP for bootloader which loads IBB without assistance of
  CSE. 0x00:Disable(Default), 0x01:Enable.
  $EN_DIS
**/
  UINT8                       SkipCseRbp;

/** Offset 0x0136 - Npk Enable
  Enable/Disable Npk. 0:Disable, 1:Enable, 2:Debugger, 3:Auto(Default).
  0:Disable, 1:Enable, 2:Debugger, 3:Auto
**/
  UINT8                       NpkEn;

/** Offset 0x0137 - FW Trace Enable
  Enable/Disable FW Trace. 0:Disable, 1:Enable(Default).
  $EN_DIS
**/
  UINT8                       FwTraceEn;

/** Offset 0x0138 - FW Trace Destination
  FW Trace Destination. 1-NPK_TRACE_TO_MEMORY, 2-NPK_TRACE_TO_DCI, 3-NPK_TRACE_TO_BSSB,
  4-NPK_TRACE_TO_PTI(Default).
**/
  UINT8                       FwTraceDestination;

/** Offset 0x0139 - NPK Recovery Dump
  Enable/Disable NPK Recovery Dump. 0:Disable(Default), 1:Enable.
  $EN_DIS
**/
  UINT8                       RecoverDump;

/** Offset 0x013A - Memory Region 0 Buffer WrapAround
  Memory Region 0 Buffer WrapAround. 0-n0-warp, 1-warp(Default).
**/
  UINT8                       Msc0Wrap;

/** Offset 0x013B - Memory Region 1 Buffer WrapAround
  Memory Region 1 Buffer WrapAround. 0-n0-warp, 1-warp(Default).
**/
  UINT8                       Msc1Wrap;

/** Offset 0x013C - Memory Region 0 Buffer Size
  Memory Region 0 Buffer Size. 0-0MB(Default), 1-1MB, 2-8MB, 3-64MB, 4-128MB, 5-256MB,
  6-512MB, 7-1GB.
**/
  UINT32                      Msc0Size;

/** Offset 0x0140 - Memory Region 1 Buffer Size
  Memory Region 1 Buffer Size, 0-0MB(Default), 1-1MB, 2-8MB, 3-64MB, 4-128MB, 5-256MB,
  6-512MB, 7-1GB.
**/
  UINT32                      Msc1Size;

/** Offset 0x0144 - PTI Mode
  PTI Mode. 0-0ff, 1-x4(Default), 2-x8, 3-x12, 4-x16.
**/
  UINT8                       PtiMode;

/** Offset 0x0145 - PTI Training
  PTI Training. 0-off(Default), 1-6=1-6.
**/
  UINT8                       PtiTraining;

/** Offset 0x0146 - PTI Speed
  PTI Speed. 0-full, 1-half, 2-quarter(Default).
**/
  UINT8                       PtiSpeed;

/** Offset 0x0147 - Punit Message Level
  Punit Message Output Verbosity Level. 0, 1(Default), 2-4=2-4.
**/
  UINT8                       PunitMlvl;

/** Offset 0x0148 - PMC Message Level
  PMC Message Output Verbosity Level. 0, 1(Default), 2-4=2-4.
**/
  UINT8                       PmcMlvl;

/** Offset 0x0149 - SW Trace Enable
  Enable/Disable SW Trace. 0:Disable(Default), 1:Enable.
  $EN_DIS
**/
  UINT8                       SwTraceEn;

/** Offset 0x014A - Periodic Retraining Disable
  Periodic Retraining Disable - This option allows customers to disable LPDDR4 Periodic
  Retraining for debug purposes. Periodic Retraining should be enabled in production.
  Periodic retraining allows the platform to operate reliably over a larger voltage
  and temperature range. This field has no effect for DDR3L and LPDDR3 memory type
  configurations. 0x00: Enable Periodic Retraining (default); 0x01: Disable Periodic
  Retraining (debug configuration only)
  0x0:Enabled, 0x1:Disabled
**/
  UINT8                       PeriodicRetrainingDisable;

/** Offset 0x014B - Enable Reset System
  Enable FSP to trigger reset instead of returning reset request. 0x00: Return the
  Return Status from FSP if a reset is required. (default); 0x01: Perform Reset inside
  FSP instead of returning from the API.
  0x0:Disabled, 0x1:Eabled
**/
  UINT8                       EnableResetSystem;

/** Offset 0x014C - Enable HECI2 in S3 resume path
  Enable HECI2 in S3 resume path. 0x00: Skip HECI2 initialization in S3 resume. ;
  0x01: Enable HECI2 in S3 resume path.(Default)
  0x0:Disabled, 0x1:Eabled
**/
  UINT8                       EnableS3Heci2;

/** Offset 0x014D
**/
  UINT8                       UnusedUpdSpace1[3];

/** Offset 0x0150
**/
  VOID*                       VariableNvsBufferPtr;

/** Offset 0x0154 - PCIE SLOT Power Enable Assert Time - PFET.
  ACPI Timer Ticker to measure when PCIE Slot Power is enabled through PFET. FSP will
  wait for 100ms for the power to be stable, before de-asserting PERST bin. Customer
  who designed the board PCIE slot Power automatically enabled, can pass value of
  zero here.
**/
  UINT64                      StartTimerTickerOfPfetAssert;

/** Offset 0x015C - Real Time Enabling
  Real-Time Feature Configuration Bits settings. 0x0:Disabled (default), 0x1:Enabled
  $EN_DIS
**/
  UINT8                       RtEn;

/** Offset 0x015D
**/
  UINT8                       ReservedFspmUpd[3];
} FSP_M_CONFIG;

/** Fsp M UPD Configuration
**/
typedef struct {

/** Offset 0x0000
**/
  FSP_UPD_HEADER              FspUpdHeader;

/** Offset 0x0020
**/
  FSPM_ARCH_UPD               FspmArchUpd;

/** Offset 0x0040
**/
  FSP_M_CONFIG                FspmConfig;

/** Offset 0x0160
**/
  UINT8                       UnusedUpdSpace2[158];

/** Offset 0x01FE
**/
  UINT16                      UpdTerminator;
} FSPM_UPD;

#pragma pack()

#endif
