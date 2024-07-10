/** @file
  PCH Pcie root port policy

  Copyright (c) 2024, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/
#ifndef _PCH_PCIERP_CONFIG_H_
#define _PCH_PCIERP_CONFIG_H_

#include <PchLimits.h>
#include <MemInfoHob.h>
#include <PcieConfig.h>

/**
 Making any structure change after API Lock
 will need to maintain backward compatibility, bump up
 structure revision and update below history table\n
  <b>Revision 1</b>:  - Initial version.
  <b>Revision 2</b>:
    - Moved EnablePort8xhDecode policy to PCIE_COMMON_CONFIG
    - Add DmiPowerGatingDis
**/
#define PCH_PCIE_CONFIG_REVISION  2

/**
 Making any structure change after API Lock
 will need to maintain backward compatibility, bump up
 structure revision and update below history table\n
  <b>Revision 1</b>:  - Initial version.
**/
#define PCH_PCIE_RP_PREMEM_CONFIG_REVISION 1

/**
 Making any structure change after API Lock
 will need to maintain backward compatibility, bump up
 structure revision and update below history table\n
  <b>Revision 1</b>:  - Initial version.
**/
#define PCIE_RP_DXE_CONFIG_REVISION 1

extern EFI_GUID gPchPcieConfigGuid;
extern EFI_GUID gPcieRpPreMemConfigGuid;

#pragma pack (push,1)

/**
  PCIe device table entry

  The PCIe device table is being used to override PCIe device ASPM settings.
  Last entry VendorId must be 0.
**/
typedef struct {
  UINT16  VendorId;                    ///< The vendor Id of Pci Express card ASPM setting override, 0xFFFF means any Vendor ID
  UINT16  DeviceId;                    ///< The Device Id of Pci Express card ASPM setting override, 0xFFFF means any Device ID
  UINT8   RevId;                       ///< The Rev Id of Pci Express card ASPM setting override, 0xFF means all steppings
  UINT8   BaseClassCode;               ///< The Base Class Code of Pci Express card ASPM setting override, 0xFF means all base class
  UINT8   SubClassCode;                ///< The Sub Class Code of Pci Express card ASPM setting override, 0xFF means all sub class
  UINT8   EndPointAspm;                ///< Override device ASPM (see: PCH_PCIE_ASPM_CONTROL)
                                       ///< Bit 1 must be set in OverrideConfig for this field to take effect
  UINT16  OverrideConfig;              ///< The override config bitmap (see: PCH_PCIE_OVERRIDE_CONFIG).
  /**
    The L1Substates Capability Offset Override. (applicable if bit 2 is set in OverrideConfig)
    This field can be zero if only the L1 Substate value is going to be override.
  **/
  UINT16  L1SubstatesCapOffset;
  /**
    L1 Substate Capability Mask. (applicable if bit 2 is set in OverrideConfig)
    Set to zero then the L1 Substate Capability [3:0] is ignored, and only L1s values are override.
    Only bit [3:0] are applicable. Other bits are ignored.
  **/
  UINT8   L1SubstatesCapMask;
  /**
    L1 Substate Port Common Mode Restore Time Override. (applicable if bit 2 is set in OverrideConfig)
    L1sCommonModeRestoreTime and L1sTpowerOnScale can have a valid value of 0, but not the L1sTpowerOnValue.
    If L1sTpowerOnValue is zero, all L1sCommonModeRestoreTime, L1sTpowerOnScale, and L1sTpowerOnValue are ignored,
    and only L1SubstatesCapOffset is override.
  **/
  UINT8   L1sCommonModeRestoreTime;
  /**
    L1 Substate Port Tpower_on Scale Override. (applicable if bit 2 is set in OverrideConfig)
    L1sCommonModeRestoreTime and L1sTpowerOnScale can have a valid value of 0, but not the L1sTpowerOnValue.
    If L1sTpowerOnValue is zero, all L1sCommonModeRestoreTime, L1sTpowerOnScale, and L1sTpowerOnValue are ignored,
    and only L1SubstatesCapOffset is override.
  **/
  UINT8   L1sTpowerOnScale;
  /**
    L1 Substate Port Tpower_on Value Override. (applicable if bit 2 is set in OverrideConfig)
    L1sCommonModeRestoreTime and L1sTpowerOnScale can have a valid value of 0, but not the L1sTpowerOnValue.
    If L1sTpowerOnValue is zero, all L1sCommonModeRestoreTime, L1sTpowerOnScale, and L1sTpowerOnValue are ignored,
    and only L1SubstatesCapOffset is override.
  **/
  UINT8   L1sTpowerOnValue;

  /**
    SnoopLatency bit definition
    Note: All Reserved bits must be set to 0

    BIT[15]     - When set to 1b, indicates that the values in bits 9:0 are valid
                  When clear values in bits 9:0 will be ignored
    BITS[14:13] - Reserved
    BITS[12:10] - Value in bits 9:0 will be multiplied with the scale in these bits
                  000b - 1 ns
                  001b - 32 ns
                  010b - 1024 ns
                  011b - 32,768 ns
                  100b - 1,048,576 ns
                  101b - 33,554,432 ns
                  110b - Reserved
                  111b - Reserved
    BITS[9:0]   - Snoop Latency Value. The value in these bits will be multiplied with
                  the scale in bits 12:10

    This field takes effect only if bit 3 is set in OverrideConfig.
  **/
  UINT16  SnoopLatency;
  /**
    NonSnoopLatency bit definition
    Note: All Reserved bits must be set to 0

    BIT[15]     - When set to 1b, indicates that the values in bits 9:0 are valid
                  When clear values in bits 9:0 will be ignored
    BITS[14:13] - Reserved
    BITS[12:10] - Value in bits 9:0 will be multiplied with the scale in these bits
                  000b - 1 ns
                  001b - 32 ns
                  010b - 1024 ns
                  011b - 32,768 ns
                  100b - 1,048,576 ns
                  101b - 33,554,432 ns
                  110b - Reserved
                  111b - Reserved
    BITS[9:0]   - Non Snoop Latency Value. The value in these bits will be multiplied with
                  the scale in bits 12:10

    This field takes effect only if bit 3 is set in OverrideConfig.
  **/
  UINT16  NonSnoopLatency;

  /**
    Forces LTR override to be permanent
    The default way LTR override works is:
      rootport uses LTR override values provided by BIOS until connected device sends an LTR message, then it will use values from the message
    This settings allows force override of LTR mechanism. If it's enabled, then:
      rootport will use LTR override values provided by BIOS forever; LTR messages sent from connected device will be ignored
  **/
  UINT8  ForceLtrOverride;
  UINT8  Reserved[3];
} PCH_PCIE_DEVICE_OVERRIDE;


typedef enum {
  PchPcieOverrideDisabled             = 0,
  PchPcieL1L2Override                 = 0x01,
  PchPcieL1SubstatesOverride          = 0x02,
  PchPcieL1L2AndL1SubstatesOverride   = 0x03,
  PchPcieLtrOverride                  = 0x04
} PCH_PCIE_OVERRIDE_CONFIG;

///
/// The values before AutoConfig match the setting of PCI Express Base Specification 1.1, please be careful for adding new feature
///
typedef enum {
  PchPcieAspmDisabled,
  PchPcieAspmL0s,
  PchPcieAspmL1,
  PchPcieAspmL0sL1,
  PchPcieAspmAutoConfig,
  PchPcieAspmMax
} PCH_PCIE_ASPM_CONTROL;

/**
  Refer to PCH EDS for the PCH implementation values corresponding
  to below PCI-E spec defined ranges
**/
typedef enum {
  PchPcieL1SubstatesDisabled,
  PchPcieL1SubstatesL1_1,
  PchPcieL1SubstatesL1_1_2,
  PchPcieL1SubstatesMax
} PCH_PCIE_L1SUBSTATES_CONTROL;

enum PCH_PCIE_MAX_PAYLOAD {
  PchPcieMaxPayload128 = 0,
  PchPcieMaxPayload256,
  PchPcieMaxPayload512,
  PchPcieMaxPayload64 = 7,
  PchPcieMaxPayloadMax
};


typedef enum {
  PchClockUsagePchPcie0      = 0,
  PchClockUsagePchPcie1      = 1,
  PchClockUsagePchPcie2      = 2,
  PchClockUsagePchPcie3      = 3,
  PchClockUsagePchPcie4      = 4,
  PchClockUsagePchPcie5      = 5,
  PchClockUsagePchPcie6      = 6,
  PchClockUsagePchPcie7      = 7,
  PchClockUsagePchPcie8      = 8,
  PchClockUsagePchPcie9      = 9,
  PchClockUsagePchPcie10     = 10,
  PchClockUsagePchPcie11     = 11,
  PchClockUsagePchPcie12     = 12,
  PchClockUsagePchPcie13     = 13,
  PchClockUsagePchPcie14     = 14,
  PchClockUsagePchPcie15     = 15,
  PchClockUsagePchPcie16     = 16,
  PchClockUsagePchPcie17     = 17,
  PchClockUsagePchPcie18     = 18,
  PchClockUsagePchPcie19     = 19,
  PchClockUsagePchPcie20     = 20,
  PchClockUsagePchPcie21     = 21,
  PchClockUsagePchPcie22     = 22,
  PchClockUsagePchPcie23     = 23,
  /**
    Quantity of PCH and CPU PCIe ports, as well as their encoding in this enum, may change between
    silicon generations and series. Do not assume that PCH port 0 will be always encoded by 0.
    Instead, it is recommended to use (PchClockUsagePchPcie0 + PchPortIndex) style to be forward-compatible
  **/
  PchClockUsageCpuPcie0      = 0x40,
  PchClockUsageCpuPcie1      = 0x41,
  PchClockUsageCpuPcie2      = 0x42,
  PchClockUsageCpuPcie3      = 0x43,

  PchClockUsageLan           = 0x70,
  PchClockUsageUnspecified   = 0x80, ///< In use for a purpose not listed above
  PchClockUsageNotUsed       = 0xFF
} PCH_PCIE_CLOCK_USAGE;

/**
  PCH_PCIE_CLOCK describes PCIe source clock generated by PCH.
**/
typedef struct {
  UINT8   Usage;             ///< Purpose of given clock (see PCH_PCIE_CLOCK_USAGE). Default: Unused, 0xFF
  UINT8   ClkReq;            ///< ClkSrc - ClkReq mapping. Default: 1:1 mapping with Clock numbers
  UINT8   RsvdBytes[2];      ///< Reserved byte
  UINT32  ClkReqGpioPinMux;  /// Muxed GPIO details. Refer GPIO_*_MUXING_SRC_CLKREQ_x*
} PCH_PCIE_CLOCK;

///
/// The PCH_DMI_LANE_CONFIG block describes specific lane configuration
///
typedef struct {
  UINT32   DmiTranCoOverEn            : 2;    ///< Enable/Disable Lane Transmitter Coefficient
  UINT32   Rsvd1                      : 6;    ///< Reserved
  UINT32   DmiTranCoOverPostCur       : 6;    ///< Lane Transmitter Post-Cursor Coefficient Override
  UINT32   Rsvd2                      : 2;    ///< Reserved
  UINT32   DmiTranCoOverPreCur        : 6;    ///< Lane Transmitter Pre-Cursor Coefficient Override
  UINT32   Rsvd3                      : 2;    ///< Reserved
  UINT32   DmiUpPortTranPreset        : 4;    ///< Upstream Port Lane Transmitter Preset
  UINT32   Rsvd4                      : 4;    ///< Reserved
} DMI_LANE_CONFIG;

typedef struct {
  DMI_LANE_CONFIG   Lane[PCH_MAX_DMI_LANES];               ///< DMI Lanes configuration
  DMI_LANE_CONFIG   LaneGen4[PCH_MAX_DMI_LANES];           ///< DMI Lanes configuration
  UINT32            DmiUpPortTranPresetEn  :  2;           ///< 0: POR setting, 1: force enable, 2: force disable.
  UINT32            DmiRtlepceb            :  2;           ///< DMI Remote Transmit Link Equalization Preset/Coefficient Evaluation Bypass (RTLEPCEB)
  /**
   When CNP is paired with SKL CPU, TX SAI Filtering Check performed for cycles
   targeting MEUMA (ie. VCm upstream). As part of this check,
   IOSF Root Space attribute is check to ensure that it is RS3.
   Setting valid only when DMI VCm is enabled.
   0: POR setting, 1: force enable, 2: force disable.
   */
  UINT32            TestDmiMeUmaRootSpaceCheck :  2;
  UINT32            Rsvdbits                   : 26;
  UINT8             MvcEnabled;                            ///< Enable/Disable MultiVC Option. 0: Disable; 1: Enable
  UINT8             Reserved[3];
} DMI_PREMEM_CONFIG;

/**
  The PCH_PCI_EXPRESS_ROOT_PORT_CONFIG describe the feature and capability of each PCH PCIe root port.
**/
typedef struct {
  PCIE_ROOT_PORT_COMMON_CONFIG  PcieRpCommonConfig; ///an instance of Pcie Common Config
  UINT8  ExtSync;              ///< Indicate whether the extended synch is enabled. <b>0: Disable</b>; 1: Enable.
  //
  // Error handlings
  //
  UINT8  SystemErrorEnable;    ///< Indicate whether the System Error is enabled. <b>0: Disable</b>; 1: Enable.
  /**
    The Multiple VC (MVC) supports hardware to avoid HoQ block for latency sensitive TC.
    Currently it is only applicable to Root Ports with 2pX4 port configuration with 2 VCs,or
    DMI port configuration with 3 VCs. For Root Ports 2pX4 configuration, two RPs (RP0,
    RP2) shall support two PCIe VCs (VC0 & VC1) and the other RPs (RP1, RP3) shall be
    disabled.
    <b>0: Disable</b>; 1: Enable
  **/
  UINT8  MvcEnabled;
  /**
    Virtual Pin Port is industry standard introduced to PCIe Hot Plug support in systems
    when GPIO pins expansion is needed. It is server specific feature.
    <b>0x00: Default</b>; 0xFF: Disabled
  **/
  UINT8   VppPort;
  UINT8   VppAddress;                               ///< PCIe Hot Plug VPP SMBus Address. Default is zero.
  UINT8   MultiVc;
  UINT8   Vc1Ctrl;
  UINT8   VcMCtrl;
  UINT8   MeUmaRootSpaceCheck;
  UINT8   RsvdBytes0[3];                            ///< Reserved bytes
} PCH_PCIE_ROOT_PORT_CONFIG;

typedef struct _CONFIG_BLOCK_HEADER {
  EFI_HOB_GUID_TYPE GuidHob;                      ///< Offset 0-23  GUID extension HOB header
  UINT8             Revision;                     ///< Offset 24    Revision of this config block
  UINT8             Attributes;                   ///< Offset 25    The main revision for config block
  UINT8             Reserved[2];                  ///< Offset 26-27 Reserved for future use
} CONFIG_BLOCK_HEADER;
/**
  The PCH_PCIE_CONFIG block describes the expected configuration of the PCH PCI Express controllers
  <b>Revision 1</b>:
  - Initial version.
  <b>Revision 2</b>:
  - Moved EnablePort8xhDecode policy to PCIE_COMMON_CONFIG
  <b>Revision 3</b>:
  - Add DmiPowerGatingDis
**/
typedef struct {
  CONFIG_BLOCK_HEADER   Header;                   ///< Config Block Header
  ///
  /// These members describe the configuration of each PCH PCIe root port.
  ///
  PCIE_COMMON_CONFIG                PcieCommonConfig;
  PCH_PCIE_ROOT_PORT_CONFIG         RootPort[PCH_MAX_PCIE_ROOT_PORTS];
  PCH_PCIE_ROOT_PORT_CONFIG         DmiPort;

  ///
  /// <b>(Test)</b> The Index of PCIe Port that is selected for Port8xh Decode (0 Based)
  ///
  UINT8                             PchPciePort8xhDecodePortIndex;
  UINT8                             DmiPowerReduction;
  UINT8                             DmiPowerGatingDis;
  UINT8                             RsvdBytes0;
} PCH_PCIE_CONFIG;

/**
  The PCIE_RP_DXE_CONFIG block describes the expected configuration of the PCH PCI Express controllers in DXE phase

  <b>Revision 1</b>:
  - Init version
**/
typedef struct {
  CONFIG_BLOCK_HEADER      Header;                     ///< Config Block Header

  /**
    PCIe device override table
    The PCIe device table is being used to override PCIe device ASPM settings.
    And it's only used in DXE phase.
    Please refer to PCIE_DEVICE_OVERRIDE structure for the table.
    Last entry VendorId must be 0.
  **/
  PCIE_DEVICE_OVERRIDE     *PcieDeviceOverrideTablePtr;
} PCIE_RP_DXE_CONFIG;

#pragma pack (pop)

#endif // _PCH_PCIERP_CONFIG_H_
