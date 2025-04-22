
/**@file

  Copyright (c) 2016 - 2020, Intel Corporation. All rights reserved.<BR>
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
  UINT8    Pcie1GpioSupport;                        ///< Offset 151     PCIe1 GPIO Support (0=Disabled, 1=PCH Based, 2=I2C Based)
  UINT8    Pcie1HoldRstExpanderNo;                  ///< Offset 152     PCIe1 HLD RST IO Expander Number
  UINT32   Pcie1HoldRstGpioNo;                      ///< Offset 153     PCIe1 HLD RST GPIO Number
  UINT8    Pcie1HoldRstActiveInfo;                  ///< Offset 157     PCIe1 HLD RST GPIO Active Information
  UINT8    Pcie1PwrEnExpanderNo;                    ///< Offset 158     PCIe1 PWR Enable IO Expander Number
  UINT32   Pcie1PwrEnGpioNo;                        ///< Offset 159     PCIe1 PWR Enable GPIO Number
  UINT8    Pcie1PwrEnActiveInfo;                    ///< Offset 163     PCIe1 PWR Enable GPIO Active Information
  UINT8    Pcie2GpioSupport;                        ///< Offset 164     PCIe2 GPIO Support (0=Disabled, 1=PCH Based, 2=I2C Based)
  UINT8    Pcie2HoldRstExpanderNo;                  ///< Offset 165     PCIe2 HLD RST IO Expander Number
  UINT32   Pcie2HoldRstGpioNo;                      ///< Offset 166     PCIe2 HLD RST GPIO Number
  UINT8    Pcie2HoldRstActiveInfo;                  ///< Offset 170     PCIe2 HLD RST GPIO Active Information
  UINT8    Pcie2PwrEnExpanderNo;                    ///< Offset 171     PCIe2 PWR Enable IO Expander Number
  UINT32   Pcie2PwrEnGpioNo;                        ///< Offset 172     PCIe2 PWR Enable GPIO Number
  UINT8    Pcie2PwrEnActiveInfo;                    ///< Offset 176     PCIe2 PWR Enable GPIO Active Information
  UINT8    Pcie3GpioSupport;                        ///< Offset 177     PCIe3 GPIO Support (0=Disabled, 1=PCH Based, 2=I2C Based)
  UINT8    Pcie3HoldRstExpanderNo;                  ///< Offset 178     PCIe3 HLD RST IO Expander Number
  UINT32   Pcie3HoldRstGpioNo;                      ///< Offset 179     PCIe3 HLD RST GPIO Number
  UINT8    Pcie3HoldRstActiveInfo;                  ///< Offset 183     PCIe3 HLD RST GPIO Active Information
  UINT8    Pcie3PwrEnExpanderNo;                    ///< Offset 184     PCIe3 PWR Enable IO Expander Number
  UINT32   Pcie3PwrEnGpioNo;                        ///< Offset 185     PCIe3 PWR Enable GPIO Number
  UINT8    Pcie3PwrEnActiveInfo;                    ///< Offset 189     PCIe3 PWR Enable GPIO Active Information
  UINT32   Pcie3WakeGpioNo;                         ///< Offset 190     PCIe3 RTD3 Device Wake GPIO Number
  UINT16   DelayAfterPwrEn;                         ///< Offset 194     Delay after power enable for PCIe
  UINT16   DelayAfterHoldReset;                     ///< Offset 196     Delay after Hold Reset for PCIe
  UINT8    Pcie0EpCapOffset;                        ///< Offset 198     PCIe0 Endpoint Capability Structure Offset
  UINT32   XPcieCfgBaseAddress;                     ///< Offset 199     Any Device's PCIe Config Space Base Address
  UINT16   GpioBaseAddress;                         ///< Offset 203     GPIO Base Address
  UINT32   NvIgOpRegionAddress;                     ///< Offset 205     NVIG opregion address
  UINT32   NvHmOpRegionAddress;                     ///< Offset 209     NVHM opregion address
  UINT32   ApXmOpRegionAddress;                     ///< Offset 213     AMDA opregion address
  UINT8    Peg0LtrEnable;                           ///< Offset 217     Latency Tolerance Reporting Enable
  UINT8    Peg0ObffEnable;                          ///< Offset 218     Optimized Buffer Flush and Fill
  UINT8    Peg1LtrEnable;                           ///< Offset 219     Latency Tolerance Reporting Enable
  UINT8    Peg1ObffEnable;                          ///< Offset 220     Optimized Buffer Flush and Fill
  UINT8    Peg2LtrEnable;                           ///< Offset 221     Latency Tolerance Reporting Enable
  UINT8    Peg2ObffEnable;                          ///< Offset 222     Optimized Buffer Flush and Fill
  UINT8    Peg3LtrEnable;                           ///< Offset 223     Latency Tolerance Reporting Enable
  UINT8    Peg3ObffEnable;                          ///< Offset 224     Optimized Buffer Flush and Fill
  UINT16   PegLtrMaxSnoopLatency;                   ///< Offset 225     SA Peg Latency Tolerance Reporting Max Snoop Latency
  UINT16   PegLtrMaxNoSnoopLatency;                 ///< Offset 227     SA Peg Latency Tolerance Reporting Max No Snoop Latency
  UINT64   Mmio64Base;                              ///< Offset 229     Base of above 4GB MMIO resource
  UINT64   Mmio64Length;                            ///< Offset 237     Length of above 4GB MMIO resource
  UINT32   CpuIdInfo;                               ///< Offset 245     CPU ID info to get Family Id or Stepping
  UINT32   Mmio32Base;                              ///< Offset 249     Base of below 4GB MMIO resource
  UINT32   Mmio32Length;                            ///< Offset 253     Length of below 4GB MMIO resource
  UINT32   Pcie0WakeGpioNo;                         ///< Offset 257     PCIe0 RTD3 Device Wake GPIO Number
  UINT32   Pcie1WakeGpioNo;                         ///< Offset 261     PCIe1 RTD3 Device Wake GPIO Number
  UINT32   Pcie2WakeGpioNo;                         ///< Offset 265     PCIe2 RTD3 Device Wake GPIO Number
  UINT8    VtdDisable;                              ///< Offset 269     VT-d Enable/Disable
  UINT32   VtdBaseAddress[7];                       ///< Offset 270     VT-d Base Address 1
                                                    ///< Offset 274     VT-d Base Address 2
                                                    ///< Offset 278     VT-d Base Address 3
                                                    ///< Offset 282     VT-d Base Address 4 (iTBT PCIE0)
                                                    ///< Offset 286     VT-d Base Address 5 (iTBT PCIE1)
                                                    ///< Offset 290     VT-d Base Address 6 (iTBT PCIE2)
                                                    ///< Offset 294     VT-d Base Address 7 (iTBT PCIE3)
  UINT16   VtdEngine1Vid;                           ///< Offset 298     VT-d Engine#1 Vendor ID
  UINT16   VtdEngine2Vid;                           ///< Offset 300     VT-d Engine#2 Vendor ID
  UINT8    RootPortIndex;                           ///< Offset 302     RootPort Number
  UINT32   RootPortAddress;                         ///< Offset 303     RootPortAddress
  UINT8    CpuTraceHubMode;                         ///< Offset 307     CPU Trace Hub Mode
  UINT8    SimicsEnvironment;                       ///< Offset 308     Simics Environment information
  UINT8    ItbtXhciEn;                              ///< Offset 309     TCSS XHCI Device Enable
  UINT8    ItbtXdciEn;                              ///< Offset 310     TCSS XDCI Device Enable
  UINT8    ItbtDmaEn[2];                            ///< Offset 311     TCSS DMA 0 Device Enable
                                                    ///< Offset 312     TCSS DMA 1 Device Enable
  UINT8    ItbtPcieRpEn[4];                         ///< Offset 313     TCSS ItbtPcieRp PCIE RP 0 Device Enable
                                                    ///< Offset 314     TCSS ItbtPcieRp PCIE RP 1 Device Enable
                                                    ///< Offset 315     TCSS ItbtPcieRp PCIE RP 2 Device Enable
                                                    ///< Offset 316     TCSS ItbtPcieRp PCIE RP 3 Device Enable
  UINT32   ItbtPcieRpAddress[4];                    ///< Offset 317     TCSS ItbtPcie Root Port address 0
                                                    ///< Offset 321     TCSS ItbtPcie Root Port address 1
                                                    ///< Offset 325     TCSS ItbtPcie Root Port address 2
                                                    ///< Offset 329     TCSS ItbtPcie Root Port address 3
  UINT32   TcssxDCIPwrDnScale;                      ///< Offset 333     TCSS xDCI Power Down Scale Value, DWC_USB3_GCTL_INIT[31:19]
  UINT8    TcssxDCIInt;                             ///< Offset 337     TCSS xDCI Int Pin
  UINT8    TcssxDCIIrq;                             ///< Offset 338     TCSS xDCI Irq Number
  UINT8    TcssRtd3;                                ///< Offset 339     TCSS RTD3
  UINT32   TcssDma0RmrrAddr;                        ///< Offset 340     TCSS DMA0 RMRR address
  UINT32   TcssDma1RmrrAddr;                        ///< Offset 344     TCSS DMA1 RMRR address
  UINT8    LtrEnable[4];                            ///< Offset 348     Latency Tolerance Reporting Mechanism. <b>0: Disable</b>; 1: Enable.
                                                    ///< Offset 349     Latency Tolerance Reporting Mechanism. <b>0: Disable</b>; 1: Enable.
                                                    ///< Offset 350     Latency Tolerance Reporting Mechanism. <b>0: Disable</b>; 1: Enable.
                                                    ///< Offset 351     Latency Tolerance Reporting Mechanism. <b>0: Disable</b>; 1: Enable.
  UINT16   PcieLtrMaxSnoopLatency[4];               ///< Offset 352     PCIE LTR max snoop Latency 0
                                                    ///< Offset 354     PCIE LTR max snoop Latency 1
                                                    ///< Offset 356     PCIE LTR max snoop Latency 2
                                                    ///< Offset 358     PCIE LTR max snoop Latency 3
  UINT16   PcieLtrMaxNoSnoopLatency[4];             ///< Offset 360     PCIE LTR max no snoop Latency 0
                                                    ///< Offset 362     PCIE LTR max no snoop Latency 1
                                                    ///< Offset 364     PCIE LTR max no snoop Latency 2
                                                    ///< Offset 366     PCIE LTR max no snoop Latency 3
  UINT8    IomReady;                                ///< Offset 368     IOM Ready
  UINT8    TcssIomVccSt;                            ///< Offset 369     TCSS IOM VccSt
  UINT8    CpuPcieRp0Enable;                        ///< Offset 370     <0:Disabled, 1:Enabled>
  UINT8    CpuPcieRp1Enable;                        ///< Offset 371     <0:Disabled, 1:Enabled>
  UINT8    CpuPcieRp2Enable;                        ///< Offset 372     <0:Disabled, 1:Enabled>
  UINT8    CpuPcieRp3Enable;                        ///< Offset 373     <0:Disabled, 1:Enabled>
  UINT8    VmdEnable;                               ///< Offset 374     VMD Device Enable
  UINT32   DeviceIdY;                               ///< Offset 375     Device ID for second LFP device
  UINT32   NextStateDidEdp2;                        ///< Offset 379     Next state DID for Second Display
  UINT8    SlotSelection;                           ///< Offset 383     PCIe slot selection
  UINT8    VmdRp1to8;                               ///< Offset 384     VMD PCH RP 1 to 8 <0:Disabled, 1:Enabled>
  UINT8    VmdRp9to16;                              ///< Offset 385     VMD PCH RP 9 to 16 <0:Disabled, 1:Enabled>
  UINT8    VmdRp17to24;                             ///< Offset 386     VMD PCH RP 17 to 24 <0:Disabled, 1:Enabled>
  UINT8    VmdSataPort[8];                          ///< Offset 387     VMD SATA PORT 0 <0:Disabled, 1:Enabled>
                                                    ///< Offset 388     VMD SATA PORT 1 <0:Disabled, 1:Enabled>
                                                    ///< Offset 389     VMD SATA PORT 2 <0:Disabled, 1:Enabled>
                                                    ///< Offset 390     VMD SATA PORT 3 <0:Disabled, 1:Enabled>
                                                    ///< Offset 391     VMD SATA PORT 4 <0:Disabled, 1:Enabled>
                                                    ///< Offset 392     VMD SATA PORT 5 <0:Disabled, 1:Enabled>
                                                    ///< Offset 393     VMD SATA PORT 6 <0:Disabled, 1:Enabled>
                                                    ///< Offset 394     VMD SATA PORT 7 <0:Disabled, 1:Enabled>
  UINT8    VmdCpuRp;                                ///< Offset 395     VMD CPU RP      <0:Disabled, 1:Enabled>
  UINT8    CpuPcieRtd3;                             ///< Offset 396     RTD3 Support for CPU PCIE.
  UINT32   LaneUsed;                                ///< Offset 397     Lane Used of each CSI Port <0:Not Configured, 1:x1, 2:x2, 3:x3 4:x4>
  UINT32   CsiSpeed;                                ///< Offset 401     Speed of each CSI Port <0:Not configured, 1:<416GMbps, 2:<1.5Gbps, 3:<2.0Gbps, 4:<2.5Gbps, 5:<4Gbps, 6:>4Gbps>
  UINT8    MaxPegPortNumber;                        ///< Offset 405     Max PEG port number
  UINT8    MemBootMode;                             ///< Offset 406     Current Memory Boot Mode <0: BOOT_MODE_1LM(Default), 1: BOOT_MODE_2LM, 2: BOOT_MODE_PROVISION>
  UINT8    DpmemSupport;                            ///< Offset 407     Dynamic PMem Support <0: Disabled, 1:Enabled>
  UINT64   PmemStartingAddress;                     ///< Offset 408     Private Pmem Starting address
  UINT64   PmemRangeLength;                         ///< Offset 416     Private Pmem Range Length
  UINT8    Pcie3EpCapOffset;                        ///< Offset 424     PCIe3 Endpoint Capability Structure Offset
  UINT8    Pcie0SrcClkNo;                           ///< Offset 425     PCIe0 RTD3 Device Source Clock Number
  UINT8    Pcie1SrcClkNo;                           ///< Offset 426     PCIe1 RTD3 Device Source Clock Number
  UINT8    Pcie2SrcClkNo;                           ///< Offset 427     PCIe2 RTD3 Device Source Clock Number
  UINT8    Pcie3SrcClkNo;                           ///< Offset 428     PCIe2 RTD3 Device Source Clock Number
  UINT8    Pcie0SecBusNum;                          ///< Offset 429     PCIe0 Secondary Bus Number (PCIe0 Endpoint Bus Number)
  UINT8    Pcie1SecBusNum;                          ///< Offset 430     PCIe1 Secondary Bus Number (PCIe0 Endpoint Bus Number)
  UINT8    Pcie2SecBusNum;                          ///< Offset 431     PCIe2 Secondary Bus Number (PCIe0 Endpoint Bus Number)
  UINT8    Pcie3SecBusNum;                          ///< Offset 432     PCIe2 Secondary Bus Number (PCIe0 Endpoint Bus Number)
  UINT8    Pcie1EpCapOffset;                        ///< Offset 433     PCIe1 Endpoint Capability Structure Offset
  UINT8    Pcie2EpCapOffset;                        ///< Offset 434     PCIe2 Endpoint Capability Structure Offset
  UINT8    IsBridgeDeviceBehindPeg1;                ///< Offset 435     Is bridge device behind PEG1
  UINT8    IsBridgeDeviceBehindPeg2;                ///< Offset 436     Is bridge device behind PEG2
  UINT8    IsBridgeDeviceBehindPeg3;                ///< Offset 437     Is bridge device behind PEG3
  UINT8    HgSlot;                                  ///< Offset 438     Slot selection between PCH/PEG
  UINT8    DPinDynamicSwitch;                       ///< Offset 439     DPin Dynamic Switch
  UINT16   DPinDynamicSwitchDelay0;                 ///< Offset 440     DPin Dynamic Switch delay 0, unit is ms
  UINT16   DPinDynamicSwitchDelay1;                 ///< Offset 442     DPin Dynamic Switch delay 1, unit is ms
} SYSTEM_AGENT_NVS_AREA;

#pragma pack(pop)
#endif
