
/**@file

 Copyright (c) 2016 - 2019, Intel Corporation. All rights reserved.<BR>
 SPDX-License-Identifier: BSD-2-Clause-Patent
**/

  //
  // Define SA NVS Area operation region.
  //
#ifndef _SA_NVS_AREA_DEF_H_
#define _SA_NVS_AREA_DEF_H_

#pragma pack (push,1)
typedef struct {
  UINT32   IgdOpRegionAddress;                      ///< Offset 0       IGD OpRegion base address
  UINT8    GfxTurboIMON;                            ///< Offset 4       IMON Current Value
  UINT8    IgdState;                                ///< Offset 5       IGD State (Primary Display = 1)
  UINT8    IgdBootType;                             ///< Offset 6       IGD Boot Display Device
  UINT8    IgdPanelType;                            ///< Offset 7       IGD Panel Type CMOS option
  UINT8    IgdPanelScaling;                         ///< Offset 8       IGD Panel Scaling
  UINT8    IgdBiaConfig;                            ///< Offset 9       IGD BIA Configuration
  UINT8    IgdSscConfig;                            ///< Offset 10      IGD SSC Configuration
  UINT8    IgdDvmtMemSize;                          ///< Offset 11      IGD DVMT Memory Size
  UINT8    IgdFunc1Enable;                          ///< Offset 12      IGD Function 1 Enable
  UINT8    IgdHpllVco;                              ///< Offset 13      HPLL VCO
  UINT8    IgdSciSmiMode;                           ///< Offset 14      GMCH SMI/SCI mode (0=SCI)
  UINT8    IgdPAVP;                                 ///< Offset 15      IGD PAVP data
  UINT8    CurrentDeviceList;                       ///< Offset 16      Current Attached Device List
  UINT16   CurrentDisplayState;                     ///< Offset 17      Current Display State
  UINT16   NextDisplayState;                        ///< Offset 19      Next Display State
  UINT8    NumberOfValidDeviceId;                   ///< Offset 21      Number of Valid Device IDs
  UINT32   DeviceId1;                               ///< Offset 22      Device ID 1
  UINT32   DeviceId2;                               ///< Offset 26      Device ID 2
  UINT32   DeviceId3;                               ///< Offset 30      Device ID 3
  UINT32   DeviceId4;                               ///< Offset 34      Device ID 4
  UINT32   DeviceId5;                               ///< Offset 38      Device ID 5
  UINT32   DeviceId6;                               ///< Offset 42      Device ID 6
  UINT32   DeviceId7;                               ///< Offset 46      Device ID 7
  UINT32   DeviceId8;                               ///< Offset 50      Device ID 8
  UINT32   DeviceId9;                               ///< Offset 54      Device ID 9
  UINT32   DeviceId10;                              ///< Offset 58      Device ID 10
  UINT32   DeviceId11;                              ///< Offset 62      Device ID 11
  UINT32   DeviceId12;                              ///< Offset 66      Device ID 12
  UINT32   DeviceId13;                              ///< Offset 70      Device ID 13
  UINT32   DeviceId14;                              ///< Offset 74      Device ID 14
  UINT32   DeviceId15;                              ///< Offset 78      Device ID 15
  UINT32   DeviceIdX;                               ///< Offset 82      Device ID for eDP device
  UINT32   NextStateDid1;                           ///< Offset 86      Next state DID1 for _DGS
  UINT32   NextStateDid2;                           ///< Offset 90      Next state DID2 for _DGS
  UINT32   NextStateDid3;                           ///< Offset 94      Next state DID3 for _DGS
  UINT32   NextStateDid4;                           ///< Offset 98      Next state DID4 for _DGS
  UINT32   NextStateDid5;                           ///< Offset 102     Next state DID5 for _DGS
  UINT32   NextStateDid6;                           ///< Offset 106     Next state DID6 for _DGS
  UINT32   NextStateDid7;                           ///< Offset 110     Next state DID7 for _DGS
  UINT32   NextStateDid8;                           ///< Offset 114     Next state DID8 for _DGS
  UINT32   NextStateDidEdp;                         ///< Offset 118     Next state DID for eDP
  UINT8    LidState;                                ///< Offset 122     Lid State (Lid Open = 1)
  UINT32   AKsv0;                                   ///< Offset 123     First four bytes of AKSV (manufacturing mode)
  UINT8    AKsv1;                                   ///< Offset 127     Fifth byte of AKSV (manufacturing mode)
  UINT8    BrightnessPercentage;                    ///< Offset 128     Brightness Level Percentage
  UINT8    AlsEnable;                               ///< Offset 129     Ambient Light Sensor Enable
  UINT8    AlsAdjustmentFactor;                     ///< Offset 130     Ambient Light Adjusment Factor
  UINT8    LuxLowValue;                             ///< Offset 131     LUX Low Value
  UINT8    LuxHighValue;                            ///< Offset 132     LUX High Value
  UINT8    ActiveLFP;                               ///< Offset 133     Active LFP
  UINT8    IpuAcpiMode;                             ///< Offset 134     IPU ACPI device type (0=Disabled, 1=AVStream virtual device as child of GFX)
  UINT8    EdpValid;                                ///< Offset 135     Check for eDP display device
  UINT8    HgMode;                                  ///< Offset 136     SG Mode (0=Disabled, 1=HG Muxed, 2=HG Muxless, 3=DGPU Only)
  UINT8    HgFeatureList;                           ///< Offset 137     HG Feature List
  UINT8    Pcie0GpioSupport;                        ///< Offset 138     PCIe0 GPIO Support (0=Disabled, 1=PCH Based, 2=I2C Based)
  UINT8    Pcie0HoldRstExpanderNo;                  ///< Offset 139     PCIe0 HLD RST IO Expander Number
  UINT32   Pcie0HoldRstGpioNo;                      ///< Offset 140     PCIe0 HLD RST GPIO Number
  UINT8    Pcie0HoldRstActiveInfo;                  ///< Offset 144     PCIe0 HLD RST GPIO Active Information
  UINT8    Pcie0PwrEnExpanderNo;                    ///< Offset 145     PCIe0 PWR Enable IO Expander Number
  UINT32   Pcie0PwrEnGpioNo;                        ///< Offset 146     PCIe0 PWR Enable GPIO Number
  UINT8    Pcie0PwrEnActiveInfo;                    ///< Offset 150     PCIe0 PWR Enable GPIO Active Information
  UINT16   DelayAfterPwrEn;                         ///< Offset 151     Delay after power enable for PCIe
  UINT16   DelayAfterHoldReset;                     ///< Offset 153     Delay after Hold Reset for PCIe
  UINT8    Pcie0EpCapOffset;                        ///< Offset 155     PCIe0 Endpoint Capability Structure Offset
  UINT32   XPcieCfgBaseAddress;                     ///< Offset 156     Any Device's PCIe Config Space Base Address
  UINT16   GpioBaseAddress;                         ///< Offset 160     GPIO Base Address
  UINT32   NvIgOpRegionAddress;                     ///< Offset 162     NVIG opregion address
  UINT32   NvHmOpRegionAddress;                     ///< Offset 166     NVHM opregion address
  UINT32   ApXmOpRegionAddress;                     ///< Offset 170     AMDA opregion address
  UINT8    Peg0LtrEnable;                           ///< Offset 174     Latency Tolerance Reporting Enable
  UINT8    Peg0ObffEnable;                          ///< Offset 175     Optimized Buffer Flush and Fill
  UINT8    Peg1LtrEnable;                           ///< Offset 176     Latency Tolerance Reporting Enable
  UINT8    Peg1ObffEnable;                          ///< Offset 177     Optimized Buffer Flush and Fill
  UINT8    Peg2LtrEnable;                           ///< Offset 178     Latency Tolerance Reporting Enable
  UINT8    Peg2ObffEnable;                          ///< Offset 179     Optimized Buffer Flush and Fill
  UINT8    Peg3LtrEnable;                           ///< Offset 180     Latency Tolerance Reporting Enable
  UINT8    Peg3ObffEnable;                          ///< Offset 181     Optimized Buffer Flush and Fill
  UINT16   PegLtrMaxSnoopLatency;                   ///< Offset 182     SA Peg Latency Tolerance Reporting Max Snoop Latency
  UINT16   PegLtrMaxNoSnoopLatency;                 ///< Offset 184     SA Peg Latency Tolerance Reporting Max No Snoop Latency
  UINT64   Mmio64Base;                              ///< Offset 186     Base of above 4GB MMIO resource
  UINT64   Mmio64Length;                            ///< Offset 194     Length of above 4GB MMIO resource
  UINT32   CpuIdInfo;                               ///< Offset 202     CPU ID info to get Family Id or Stepping
  UINT32   Mmio32Base;                              ///< Offset 206     Base of below 4GB MMIO resource
  UINT32   Mmio32Length;                            ///< Offset 210     Length of below 4GB MMIO resource
  UINT8    VtdDisable;                              ///< Offset 214     VT-d Enable/Disable
  UINT32   VtdBaseAddress[7];                       ///< Offset 215     VT-d Base Address 1
                                                    ///< Offset 219     VT-d Base Address 2
                                                    ///< Offset 223     VT-d Base Address 3
                                                    ///< Offset 227     VT-d Base Address 4 (iTBT PCIE0)
                                                    ///< Offset 231     VT-d Base Address 5 (iTBT PCIE1)
                                                    ///< Offset 235     VT-d Base Address 6 (iTBT PCIE2)
                                                    ///< Offset 239     VT-d Base Address 7 (iTBT PCIE3)
  UINT16   VtdEngine1Vid;                           ///< Offset 243     VT-d Engine#1 Vendor ID
  UINT16   VtdEngine2Vid;                           ///< Offset 245     VT-d Engine#2 Vendor ID
  UINT8    RootPortIndex;                           ///< Offset 247     RootPort Number
  UINT32   RootPortAddress;                         ///< Offset 248     RootPortAddress
  UINT8    CpuTraceHubMode;                         ///< Offset 252     CPU Trace Hub Mode
  UINT8    SimicsEnvironment;                       ///< Offset 253     Simics Environment information
  UINT8    ItbtXhciEn;                              ///< Offset 254     TCSS XHCI Device Enable
  UINT8    ItbtXdciEn;                              ///< Offset 255     TCSS XDCI Device Enable
  UINT8    ItbtDmaEn[2];                            ///< Offset 256     TCSS DMA 0 Device Enable
                                                    ///< Offset 257     TCSS DMA 1 Device Enable
  UINT8    ItbtPcieRpEn[4];                         ///< Offset 258     TCSS ItbtPcieRp PCIE RP 0 Device Enable
                                                    ///< Offset 259     TCSS ItbtPcieRp PCIE RP 1 Device Enable
                                                    ///< Offset 260     TCSS ItbtPcieRp PCIE RP 2 Device Enable
                                                    ///< Offset 261     TCSS ItbtPcieRp PCIE RP 3 Device Enable
  UINT32   ItbtPcieRpAddress[4];                    ///< Offset 262     TCSS ItbtPcie Root Port address 0
                                                    ///< Offset 266     TCSS ItbtPcie Root Port address 1
                                                    ///< Offset 270     TCSS ItbtPcie Root Port address 2
                                                    ///< Offset 274     TCSS ItbtPcie Root Port address 3
  UINT32   TcssxDCIPwrDnScale;                      ///< Offset 278     TCSS xDCI Power Down Scale Value, DWC_USB3_GCTL_INIT[31:19]
  UINT8    TcssxDCIInt;                             ///< Offset 282     TCSS xDCI Int Pin
  UINT8    TcssxDCIIrq;                             ///< Offset 283     TCSS xDCI Irq Number
  UINT8    TcssRtd3;                                ///< Offset 284     TCSS RTD3
  UINT8    LtrEnable[4];                            ///< Offset 285     Latency Tolerance Reporting Mechanism. <b>0: Disable</b>; 1: Enable.
                                                    ///< Offset 286     Latency Tolerance Reporting Mechanism. <b>0: Disable</b>; 1: Enable.
                                                    ///< Offset 287     Latency Tolerance Reporting Mechanism. <b>0: Disable</b>; 1: Enable.
                                                    ///< Offset 288     Latency Tolerance Reporting Mechanism. <b>0: Disable</b>; 1: Enable.
  UINT16   PcieLtrMaxSnoopLatency[4];               ///< Offset 289     PCIE LTR max snoop Latency 0
                                                    ///< Offset 291     PCIE LTR max snoop Latency 1
                                                    ///< Offset 293     PCIE LTR max snoop Latency 2
                                                    ///< Offset 295     PCIE LTR max snoop Latency 3
  UINT16   PcieLtrMaxNoSnoopLatency[4];             ///< Offset 297     PCIE LTR max no snoop Latency 0
                                                    ///< Offset 299     PCIE LTR max no snoop Latency 1
                                                    ///< Offset 301     PCIE LTR max no snoop Latency 2
                                                    ///< Offset 303     PCIE LTR max no snoop Latency 3
  UINT8    IomReady;                                ///< Offset 305     IOM Ready
  UINT8    TcssIomVccSt;                            ///< Offset 306     TCSS IOM VccSt
  UINT8    CpuPcieRp0Enable;                        ///< Offset 307     <0:Disabled, 1:Enabled>
  UINT8    CpuPcieRp1Enable;                        ///< Offset 308     <0:Disabled, 1:Enabled>
  UINT8    CpuPcieRp2Enable;                        ///< Offset 309     <0:Disabled, 1:Enabled>
  UINT8    CpuPcieRp3Enable;                        ///< Offset 310     <0:Disabled, 1:Enabled>
  UINT8    VmdEnable;                               ///< Offset 311     VMD Device Enable
  UINT32   DeviceIdY;                               ///< Offset 312     Device ID for second LFP device
  UINT32   NextStateDidEdp2;                        ///< Offset 316     Next state DID for Second Display
  UINT8    Reserved0[180];                          ///< Offset 320:499
} SYSTEM_AGENT_NVS_AREA;

#pragma pack(pop)
#endif
