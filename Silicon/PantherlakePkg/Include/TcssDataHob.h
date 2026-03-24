/** @file
  This file contains definitions of TCSS Data.

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
#ifndef _TCSS_DATA_HOB_H_
#define _TCSS_DATA_HOB_H_

#include <TcssInfo.h>
#include <MemInfoHob.h>
#include <TcssPeiPreMemConfig.h>
#include <TcssPeiConfig.h>

#pragma pack (push,1)

///
/// Structure for Tbt firmware status from IMR.
///
typedef union {
  UINT32 RegValue;
  struct {
    UINT32 TbtFwVersion:16; ///< Bits 15:0
    UINT32 Reserved:6;      ///< Bits 21:16
    UINT32 ErrorCode:8;     ///< Bits 29:22
    UINT32 Valid:1;         ///< Bits 30    - 00 = untrusted FW, 01 = successful authentication
    UINT32 Done:1;          ///< Bits 31    - 01 = FW download to IMR is done
  } Bits;
} TBT_IMR_STATUS;

///
/// Structure for MG image status from IMR.
///
typedef union {
  UINT32 RegValue;
  struct {
    UINT32 MgFwVersion:16; ///< Bits 15:0
    UINT32 Reserved:6;     ///< Bits 21:16
    UINT32 ErrorCode:8;    ///< Bits 29:22
    UINT32 Valid:1;        ///< Bits 30    - 00 = untrusted FW, 01 = successful authentication
    UINT32 Done:1;         ///< Bits 31    - 01 = FW download to IMR is done
  } Bits;
} MG_IMR_STATUS;

/**
  TCSS data definition
**/
typedef struct {
  UINT32            IOMReady:1;                             ///< Indicate the IOM ready.
  UINT32            PcieMultipleSegmentEnabled:1;           ///  Deprecate < Indicate the extra segment of iTBT PCIe is enabled or not.
  UINT32            SwConfiguration1Lock:1;                 ///< B_IOM_TYPEC_SW_CONFIGURATION_1_LOCK
  UINT32            SwConfiguration3Lock:1;                 ///< B_IOM_TYPEC_SW_CONFIGURATION_3_LOCK
  UINT32            SwConfiguration4Lock:1;                 ///< B_IOM_TYPEC_SW_CONFIGURATION_4_LOCK
  UINT32            MiscConfigurationLock:1;                ///< B_IOM_PCR_IOM_USB4HR_MISC_CONFIG_LOCK
  UINT32            Rsvd:26;                                ///< Reserved for future use.
  UINT32            IomFwVersion;                           ///< IOM FW version.
  UINT32            IomFwEngrVersion;                       ///< IOM FW Engineer version. Non-zero value used to identify engineering versions / hotfixes. Shall be zero for official releases.
  TBT_IMR_STATUS    TbtImrStatus;                           ///< Tbt FW IMR Status.
  UINT32            ItbtPcieSegMap;                         ///< iTBT PCIE RP 0-3 Segment Map
  UINT32            ItbtPcieFuncMap;                        ///< iTBT PCIE RP 0-3 Function Map
  UINT8             ItbtPcieRpEn[MAX_ITBT_PCIE_PORT];       ///< ItbtPcieRpEn PCIE RP Device Enable
  UINT8             ItbtXhciEn;                             ///< ItbtPcieRpEn XHCI Device Enable
  UINT8             ItbtDmaEn[MAX_HOST_ITBT_DMA_NUMBER];    ///< ItbtPcieRpEn DMA Device Enable
  UINT8             TcssRtd3;                               ///< TCSS RTD3
  UINT8             TcssUsbEnMap;                           ///< TCSS TypeC Port USB Function Enable Map
  MG_IMR_STATUS     MgImrStatus;                            ///< MG IMR Status
  UINT8             TcssIomVccSt;                           ///< IOM VCCST request.
  UINT8             DeepstTcState;                          ///< Maximum TC C State limit
  UINT64            IomBase;                                ///< IOM Base
  UINT8             RetimerMap;                             ///< Retimer Mapping
} TCSS_DATA;

typedef struct {
  UINT8   LtrEnable;                              ///< Latency Tolerance Reporting Mechanism. <b>0: Disable</b>; 1: Enable.
  UINT16  LtrMaxSnoopLatency;                     ///< Latency Tolerance Reporting, Max Snoop Latency.
  UINT16  LtrMaxNoSnoopLatency;                   ///< Latency Tolerance Reporting, Max Non-Snoop Latency.
  UINT8   SnoopLatencyOverrideMode;               ///< Latency Tolerance Reporting, Snoop Latency Override Mode.
  UINT8   SnoopLatencyOverrideMultiplier;         ///< Latency Tolerance Reporting, Snoop Latency Override Multiplier.
  UINT16  SnoopLatencyOverrideValue;              ///< Latency Tolerance Reporting, Snoop Latency Override Value.
  UINT8   NonSnoopLatencyOverrideMode;            ///< Latency Tolerance Reporting, Non-Snoop Latency Override Mode.
  UINT8   NonSnoopLatencyOverrideMultiplier;      ///< Latency Tolerance Reporting, Non-Snoop Latency Override Multiplier.
  UINT16  NonSnoopLatencyOverrideValue;           ///< Latency Tolerance Reporting, Non-Snoop Latency Override Value.
  UINT8   ForceLtrOverride;                       ///< <b>0: Disable</b>; 1: Enable.
  UINT8   LtrConfigLock;                          ///< <b>0: Disable</b>; 1: Enable.
} ITBT_PCILTR_DATA;

///
/// TCSS Data Hob
///
typedef struct {
  EFI_HOB_GUID_TYPE        EfiHobGuidType;                    ///< Offset 0 - 23: GUID Hob type structure for gTcssHobGuid
  TCSS_DATA                TcssData;                          ///< Offset 24 TCSS data definition.
  ITBT_PCILTR_DATA         PcieItbtLtrData[MAX_ITBT_PCIE_PORT];
  TCSS_PEI_PREMEM_CONFIG   TcssPeiPreMemCfgBlk;
  TCSS_PEI_CONFIG          TcssPeiCfgBlk;
} TCSS_DATA_HOB;


#pragma pack (pop)
#endif
