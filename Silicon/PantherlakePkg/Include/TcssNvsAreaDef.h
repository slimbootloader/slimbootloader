/** @file

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

  //
  // Define TCSS NVS Area operation region.
  //
#ifndef _TCSS_NVS_AREA_DEF_H_
#define _TCSS_NVS_AREA_DEF_H_

#pragma pack (push,1)
typedef struct {
  UINT8    ItbtXhciEn;                              ///< Offset 0       TCSS XHCI Device Enable
  UINT8    ItbtDmaEn[2];                            ///< Offset 1       TCSS DMA 0 Device Enable
                                                    ///< Offset 2       TCSS DMA 1 Device Enable
  UINT8    ItbtPcieRpEn[4];                         ///< Offset 3       TCSS ItbtPcieRp PCIE RP 0 Device Enable
                                                    ///< Offset 4       TCSS ItbtPcieRp PCIE RP 1 Device Enable
                                                    ///< Offset 5       TCSS ItbtPcieRp PCIE RP 2 Device Enable
                                                    ///< Offset 6       TCSS ItbtPcieRp PCIE RP 3 Device Enable
  UINT32   ItbtPcieRpAddress[4];                    ///< Offset 7       TCSS ItbtPcie Root Port address 0
                                                    ///< Offset 11      TCSS ItbtPcie Root Port address 1
                                                    ///< Offset 15      TCSS ItbtPcie Root Port address 2
                                                    ///< Offset 19      TCSS ItbtPcie Root Port address 3
  UINT8    TcssRtd3;                                ///< Offset 23      TCSS RTD3
  UINT8    LtrEnable[4];                            ///< Offset 24      Latency Tolerance Reporting Mechanism. <b>0: Disable</b>; 1: Enable.
                                                    ///< Offset 25      Latency Tolerance Reporting Mechanism. <b>0: Disable</b>; 1: Enable.
                                                    ///< Offset 26      Latency Tolerance Reporting Mechanism. <b>0: Disable</b>; 1: Enable.
                                                    ///< Offset 27      Latency Tolerance Reporting Mechanism. <b>0: Disable</b>; 1: Enable.
  UINT16   PcieLtrMaxSnoopLatency[4];               ///< Offset 28      PCIE LTR max snoop Latency 0
                                                    ///< Offset 30      PCIE LTR max snoop Latency 1
                                                    ///< Offset 32      PCIE LTR max snoop Latency 2
                                                    ///< Offset 34      PCIE LTR max snoop Latency 3
  UINT16   PcieLtrMaxNoSnoopLatency[4];             ///< Offset 36      PCIE LTR max no snoop Latency 0
                                                    ///< Offset 38      PCIE LTR max no snoop Latency 1
                                                    ///< Offset 40      PCIE LTR max no snoop Latency 2
                                                    ///< Offset 42      PCIE LTR max no snoop Latency 3
  UINT8    IomReady;                                ///< Offset 44      IOM Ready
  UINT8    TcssIomVccSt;                            ///< Offset 45      TCSS IOM VccSt
  UINT16   RegBarOffset;                            ///< Offset 46      MCH RegBar Offset
  UINT64   IomBase;                                 ///< Offset 48      Iom RegBar Base
  UINT8    RetimerMap;                              ///< Offset 56      USBC Retimer Mapping.
} TCSS_NVS_AREA;

#pragma pack(pop)
#endif
