/** @file
  TCSS PEI policy

  @copyright
  INTEL CONFIDENTIAL
  Copyright (C) 2016 Intel Corporation.

  This software and the related documents are Intel copyrighted materials,
  and your use of them is governed by the express license under which they
  were provided to you ("License"). Unless the License provides otherwise,
  you may not use, modify, copy, publish, distribute, disclose or transmit
  this software or the related documents without Intel's prior written
  permission.

  This software and the related documents are provided as is, with no
  express or implied warranties, other than those that are expressly stated
  in the License.

@par Specification Reference:
**/
#ifndef _TCSS_PEI_CONFIG_H_
#define _TCSS_PEI_CONFIG_H_

#include <ConfigBlock.h>
#include <UsbConfig.h>
#include <TcssInfo.h>
#include <TcssPeiPreMemConfig.h>

#define TCSS_PEI_CONFIG_REVISION 5
extern EFI_GUID gTcssPeiConfigGuid;

#pragma pack (push,1)

typedef struct {
  UINT32     VccSt;         ///< IOM VCCST request. (Not equal to actual VCCST value)
  UINT32     D3ColdEnable;  ///< Enable/disable D3 Cold support in TCSS
  UINT8      Rsvd[8];       ///< Reserved bytes
} IOM_INTERFACE_CONFIG;

///
/// The PMC_INTERFACE_CONFIG block describes interaction between BIOS and PMC
///
typedef struct {
  UINT8      PmcPdEnable;    ///< PMC PD Solution Enable
  UINT8      Rsvd[3];
} PMC_INTERFACE_CONFIG;

//
// TCSS Port Convert to TypeA.
// The Data is avtive when Enable field is '1'.
//
typedef union {
  struct {
    UINT8  MappingPchXhciUsb2:4; ///< PCH xhci port number for Type C Port x mapping
    UINT8  Rsvd:2;               ///< Reserved bytes
    UINT8  Orientational:1;      ///< Enable / Disable Orientation bit. <b>0: Disable</b>; 1: Enable.
    UINT8  Enable : 1;           ///< Enable / Disable Type C Port X Convert to TypeA.
    UINT8  Rsvd2[3];
  } Field;
  UINT8 Data8;
} TCSS_TYPEC_CONV_USBA;

///
/// The TCSS_PCIE_PORT_POLICY block describes PCIe settings for TCSS.
///
typedef struct {
  UINT8   AcsEnabled;                         ///< Indicate whether the ACS is enabled. 0: Disable; <b>1: Enable</b>.
  UINT8   Rsvd[2];
  UINT8   LtrEnable;                          ///< Latency Tolerance Reporting Mechanism. <b>0: Disable</b>; 1: Enable.
  UINT8   PtmEnabled;                         ///< Enables PTM capability

  UINT8   Aspm;                               ///< The ASPM configuration of the root port (see: PCH_PCIE_ASPM_CONTROL). Default is <b>
  UINT8   SlotNumber;                         ///< Indicates the slot number for the root port. Default is the value as root port index.
  UINT8   SlotPowerLimitScale;                ///< <b>(Test)</b> Specifies scale used for slot power limit value. Leave as 0 to set to default. Default is <b>zero</b>.
  UINT16  SlotPowerLimitValue;                ///< <b>(Test)</b> Specifies upper limit on power supplies by slot. Leave as 0 to set to default. Default is <b>zero</b>.

  UINT8   AdvancedErrorReporting;             ///< Indicate whether the Advanced Error Reporting is enabled. <b>0: Disable</b>; 1: Enable.
  UINT8   UnsupportedRequestReport;           ///< Indicate whether the Unsupported Request Report is enabled. <b>0: Disable</b>; 1: Enable.
  UINT8   FatalErrorReport;                   ///< Indicate whether the Fatal Error Report is enabled. <b>0: Disable</b>; 1: Enable.
  UINT8   NoFatalErrorReport;                 ///< Indicate whether the No Fatal Error Report is enabled. <b>0: Disable</b>; 1: Enable.
  UINT8   CorrectableErrorReport;             ///< Indicate whether the Correctable Error Report is enabled. <b>0: Disable</b>; 1: Enable.
  UINT8   SystemErrorOnFatalError;            ///< Indicate whether the System Error on Fatal Error is enabled. <b>0: Disable</b>; 1: Enable.
  UINT8   SystemErrorOnNonFatalError;         ///< Indicate whether the System Error on Non Fatal Error is enabled. <b>0: Disable</b>; 1: Enable.
  UINT8   SystemErrorOnCorrectableError;      ///< Indicate whether the System Error on Correctable Error is enabled. <b>0: Disable</b>; 1: Enable.

  UINT16  LtrMaxSnoopLatency;                 ///< Latency Tolerance Reporting, Max Snoop Latency.
  UINT16  LtrMaxNoSnoopLatency;               ///< Latency Tolerance Reporting, Max Non-Snoop Latency.
  UINT8   SnoopLatencyOverrideMode;           ///< Latency Tolerance Reporting, Snoop Latency Override Mode.
  UINT8   SnoopLatencyOverrideMultiplier;     ///< Latency Tolerance Reporting, Snoop Latency Override Multiplier.
  UINT16  SnoopLatencyOverrideValue;          ///< Latency Tolerance Reporting, Snoop Latency Override Value.
  UINT8   NonSnoopLatencyOverrideMode;        ///< Latency Tolerance Reporting, Non-Snoop Latency Override Mode.
  UINT8   NonSnoopLatencyOverrideMultiplier;  ///< Latency Tolerance Reporting, Non-Snoop Latency Override Multiplier.
  UINT16  NonSnoopLatencyOverrideValue;       ///< Latency Tolerance Reporting, Non-Snoop Latency Override Value.
  UINT8   ForceLtrOverride;                   ///< <b>0: Disable</b>; 1: Enable.
  UINT8   LtrConfigLock;                      ///< <b>0: Disable</b>; 1: Enable.
  // SA:RestrictedBegin
  UINT8   FpbEnable;                          ///< FPB FpbEnable option.
  UINT8   RsvdBytes[3];
  // SA:RestrictedEnd
} TCSS_PCIE_PORT_POLICY;

///
/// TCSS_PCIE_PEI_POLICY describes PCIe port settings for TCSS.
///
typedef struct {
  TCSS_PCIE_PORT_POLICY  PciePortPolicy[MAX_ITBT_PCIE_PORT];
  /**
    Bitmap of TBT performance boost enabled TCSS PCIe root ports.
    Each bit represents a specific port:
    BIT0 - BIT7: TCSS PCIe root ports bitmap, where bit i corresponds to TCSS PCIe root port(i),
                 e.g. BIT0 - TCSS PCIe root port0. The max supported TCSS PCIe root port number
                 depends on silicon.
  **/
  UINT8   TcssTbtPerfBoost;
  UINT8   Reserved[3];                  ///< Reserved bytes
} TCSS_PCIE_PEI_POLICY;

///
/// The TCSS_IOM_PEI_CONFIG block describes IOM settings for TCSS.
///
typedef struct {
  IOM_INTERFACE_CONFIG      IomInterface;                            ///< Config settings are BIOS <-> IOM interface.
  PMC_INTERFACE_CONFIG      PmcInterface;                            ///< Config settings for BIOS <-> PMC interface
  UINT8                     TcStateLimit;                            ///< Tcss C-State deep stage
  UINT8                     TcNotifyIgd;                             ///< Wake iGfx from D3 on HPD_ASSERT or HPD_IRQ assocaited with the DP_ALT and DP tunneling connection. 0: Disable; 1: Enable.
  UINT8                     Reserved[2];                             ///< Reserved bytes
} TCSS_IOM_PEI_CONFIG;



typedef struct {
  UINT32     GpioLsxOe; ///< GPIO Lsx Oe Pin number that is for GPIO to drive direction signal on external Level shifter in USB4 20G retimerless port.
} TBT_LSX_OE_PAD_CONFIG;

///
/// The TCSS_MISC_PEI_CONFIG block describes MISC settings for TCSS.
///
typedef struct {
  TCSS_TYPEC_CONV_USBA      TcssConvUsbA[MAX_TCSS_USB3_PORTS];       ///< TCSS Port Convert to TypeA
  TBT_LSX_OE_PAD_CONFIG     LsxOePortPad[MAX_TCSS_USB3_PORTS];       ///< The Lsx Oe port config setting.
  UINT8                     LsxOe;                                   ///< Bits defining value for LSx OE Orientation
  UINT8                     Reserved[3];                             ///< Reserved bytes
} TCSS_MISC_PEI_CONFIG;

///
/// The TCSS_PEI_CONFIG block describes TCSS settings for SA.
///
typedef struct {
  CONFIG_BLOCK_HEADER     Header;     ///< Offset 0-27 Config Block Header
  TCSS_PCIE_PEI_POLICY    PciePolicy; ///< The PCIe Config
  USB_CONFIG              UsbConfig;  ///< USB config is shared between PCH and SA.
  TCSS_IOM_PEI_CONFIG     IomConfig;  ///< The Iom Config
  TCSS_MISC_PEI_CONFIG    MiscConfig; ///< The MISC Config
} TCSS_PEI_CONFIG;

#pragma pack (pop)

/**
  Print TCSS_PEI_CONFIG and serial out.

  @param[in] TcssPeiConfig            Pointer to a TCSS_PEI_CONFIG
**/

VOID
TcssConfigDump (
  IN TCSS_PEI_CONFIG           *TcssPeiConfig
  );

#endif /* _TCSS_PEI_CONFIG_H_ */
