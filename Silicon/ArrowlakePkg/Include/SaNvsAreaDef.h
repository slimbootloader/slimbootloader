
/**@file


  Copyright (c) 2024, Intel Corporation. All rights reserved.<BR>
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
  UINT8    IgdState;                                ///< Offset 4       IGD State (Primary Display = 1)
  UINT8    NumberOfValidDeviceId;                   ///< Offset 5       Number of Valid Device IDs
  UINT32   DeviceId1;                               ///< Offset 6       Device ID 1
  UINT32   DeviceId2;                               ///< Offset 10      Device ID 2
  UINT32   DeviceId3;                               ///< Offset 14      Device ID 3
  UINT32   DeviceId4;                               ///< Offset 18      Device ID 4
  UINT32   DeviceId5;                               ///< Offset 22      Device ID 5
  UINT32   DeviceId6;                               ///< Offset 26      Device ID 6
  UINT32   DeviceId7;                               ///< Offset 30      Device ID 7
  UINT32   DeviceId8;                               ///< Offset 34      Device ID 8
  UINT32   DeviceId9;                               ///< Offset 38      Device ID 9
  UINT32   DeviceId10;                              ///< Offset 42      Device ID 10
  UINT32   DeviceId11;                              ///< Offset 46      Device ID 11
  UINT32   DeviceId12;                              ///< Offset 50      Device ID 12
  UINT32   DeviceId13;                              ///< Offset 54      Device ID 13
  UINT32   DeviceId14;                              ///< Offset 58      Device ID 14
  UINT32   DeviceId15;                              ///< Offset 62      Device ID 15
  UINT32   DeviceIdX;                               ///< Offset 66      Device ID for eDP device
  UINT32   NextStateDid1;                           ///< Offset 70      Next state DID1 for _DGS
  UINT32   NextStateDid2;                           ///< Offset 74      Next state DID2 for _DGS
  UINT32   NextStateDid3;                           ///< Offset 78      Next state DID3 for _DGS
  UINT32   NextStateDid4;                           ///< Offset 82      Next state DID4 for _DGS
  UINT32   NextStateDid5;                           ///< Offset 86      Next state DID5 for _DGS
  UINT32   NextStateDid6;                           ///< Offset 90      Next state DID6 for _DGS
  UINT32   NextStateDid7;                           ///< Offset 94      Next state DID7 for _DGS
  UINT32   NextStateDid8;                           ///< Offset 98      Next state DID8 for _DGS
  UINT32   NextStateDidEdp;                         ///< Offset 102     Next state DID for eDP
  UINT8    LidState;                                ///< Offset 106     Lid State (Lid Open = 1)
  UINT8    BrightnessPercentage;                    ///< Offset 107     Brightness Level Percentage
  UINT8    AlsEnable;                               ///< Offset 108     Ambient Light Sensor Enable
  UINT8    LuxLowValue;                             ///< Offset 109     LUX Low Value
  UINT8    LuxHighValue;                            ///< Offset 110     LUX High Value
  UINT8    IpuAcpiMode;                             ///< Offset 111     IPU ACPI device type (0=Disabled, 1=AVStream virtual device as child of GFX)
  UINT8    EdpValid;                                ///< Offset 112     Check for eDP display device
  UINT16   DelayAfterPwrEn;                         ///< Offset 113     Delay after power enable for PCIe
  UINT16   DelayAfterHoldReset;                     ///< Offset 115     Delay after Hold Reset for PCIe
  UINT32   XPcieCfgBaseAddress;                     ///< Offset 117     Any Device's PCIe Config Space Base Address
  UINT32   ApXmOpRegionAddress;                     ///< Offset 121     AMDA opregion address
  UINT8    Peg2LtrEnable;                           ///< Offset 125     Latency Tolerance Reporting Enable
  UINT64   Mmio64Base;                              ///< Offset 126     Base of above 4GB MMIO resource
  UINT64   Mmio64Length;                            ///< Offset 134     Length of above 4GB MMIO resource
  UINT32   CpuIdInfo;                               ///< Offset 142     CPU ID info to get Family Id or Stepping
  UINT32   Mmio32Base;                              ///< Offset 146     Base of below 4GB MMIO resource
  UINT32   Mmio32Length;                            ///< Offset 150     Length of below 4GB MMIO resource
  UINT32   Pcie0WakeGpioNo;                         ///< Offset 154     PCIe0 RTD3 Device Wake GPIO Number
  UINT32   Pcie1WakeGpioNo;                         ///< Offset 158     PCIe1 RTD3 Device Wake GPIO Number
  UINT32   Pcie2WakeGpioNo;                         ///< Offset 162     PCIe2 RTD3 Device Wake GPIO Number
  UINT8    VtdDisable;                              ///< Offset 166     VT-d Enable/Disable
  UINT32   VtdBaseAddress[7];                       ///< Offset 167     VT-d Base Address 1
                                                    ///< Offset 171     VT-d Base Address 2
                                                    ///< Offset 175     VT-d Base Address 3
                                                    ///< Offset 179     VT-d Base Address 4 (iTBT PCIE0)
                                                    ///< Offset 183     VT-d Base Address 5 (iTBT PCIE1)
                                                    ///< Offset 187     VT-d Base Address 6 (iTBT PCIE2)
                                                    ///< Offset 191     VT-d Base Address 7 (iTBT PCIE3)
  UINT16   VtdEngine1Vid;                           ///< Offset 195     VT-d Engine#1 Vendor ID
  UINT16   VtdEngine2Vid;                           ///< Offset 197     VT-d Engine#2 Vendor ID
  UINT8    RootPortIndex;                           ///< Offset 199     RootPort Number
  UINT32   RootPortAddress;                         ///< Offset 200     RootPortAddress
  UINT8    SimicsEnvironment;                       ///< Offset 204     Simics Environment information
  UINT8    ItbtXhciEn;                              ///< Offset 205     TCSS XHCI Device Enable
  UINT8    ItbtXdciEn;                              ///< Offset 206     TCSS XDCI Device Enable
  UINT8    ItbtDmaEn[2];                            ///< Offset 207     TCSS DMA 0 Device Enable
                                                    ///< Offset 208     TCSS DMA 1 Device Enable
  UINT8    ItbtPcieRpEn[4];                         ///< Offset 209     TCSS ItbtPcieRp PCIE RP 0 Device Enable
                                                    ///< Offset 210     TCSS ItbtPcieRp PCIE RP 1 Device Enable
                                                    ///< Offset 211     TCSS ItbtPcieRp PCIE RP 2 Device Enable
                                                    ///< Offset 212     TCSS ItbtPcieRp PCIE RP 3 Device Enable
  UINT32   ItbtPcieRpAddress[4];                    ///< Offset 213     TCSS ItbtPcie Root Port address 0
                                                    ///< Offset 217     TCSS ItbtPcie Root Port address 1
                                                    ///< Offset 221     TCSS ItbtPcie Root Port address 2
                                                    ///< Offset 225     TCSS ItbtPcie Root Port address 3
  UINT32   TcssxDCIPwrDnScale;                      ///< Offset 229     TCSS xDCI Power Down Scale Value, DWC_USB3_GCTL_INIT[31:19]
  UINT8    TcssxDCIInt;                             ///< Offset 233     TCSS xDCI Int Pin
  UINT8    TcssxDCIIrq;                             ///< Offset 234     TCSS xDCI Irq Number
  UINT8    TcssRtd3;                                ///< Offset 235     TCSS RTD3
  UINT32   TcssDma0RmrrAddr;                        ///< Offset 236     TCSS DMA0 RMRR address
  UINT32   TcssDma1RmrrAddr;                        ///< Offset 240     TCSS DMA1 RMRR address
  UINT8    LtrEnable[4];                            ///< Offset 244     Latency Tolerance Reporting Mechanism. <b>0: Disable</b>; 1: Enable.
                                                    ///< Offset 245     Latency Tolerance Reporting Mechanism. <b>0: Disable</b>; 1: Enable.
                                                    ///< Offset 246     Latency Tolerance Reporting Mechanism. <b>0: Disable</b>; 1: Enable.
                                                    ///< Offset 247     Latency Tolerance Reporting Mechanism. <b>0: Disable</b>; 1: Enable.
  UINT16   PcieLtrMaxSnoopLatency[4];               ///< Offset 248     PCIE LTR max snoop Latency 0
                                                    ///< Offset 250     PCIE LTR max snoop Latency 1
                                                    ///< Offset 252     PCIE LTR max snoop Latency 2
                                                    ///< Offset 254     PCIE LTR max snoop Latency 3
  UINT16   PcieLtrMaxNoSnoopLatency[4];             ///< Offset 256     PCIE LTR max no snoop Latency 0
                                                    ///< Offset 258     PCIE LTR max no snoop Latency 1
                                                    ///< Offset 260     PCIE LTR max no snoop Latency 2
                                                    ///< Offset 262     PCIE LTR max no snoop Latency 3
  UINT8    IomReady;                                ///< Offset 264     IOM Ready
  UINT8    TcssIomVccSt;                            ///< Offset 265     TCSS IOM VccSt
  UINT32   DeviceIdY;                               ///< Offset 266     Device ID for second LFP device
  UINT32   NextStateDidEdp2;                        ///< Offset 270     Next state DID for Second Display
  UINT8    SlotSelection;                           ///< Offset 274     PCIe slot selection
  UINT8    MaxPegPortNumber;                        ///< Offset 275     Max PEG port number
  UINT8    MemBootMode;                             ///< Offset 276     Current Memory Boot Mode <0: BOOT_MODE_1LM(Default), 1: BOOT_MODE_2LM, 2: BOOT_MODE_PROVISION>
  UINT8    DpmemSupport;                            ///< Offset 277     Dynamic PMem Support <0: Disabled, 1:Enabled>
  UINT64   PmemStartingAddress;                     ///< Offset 278     Private Pmem Starting address
  UINT64   PmemRangeLength;                         ///< Offset 286     Private Pmem Range Length
  UINT8    IsBridgeDeviceBehindPeg1;                ///< Offset 294     Is bridge device behind Peg1
  UINT8    IsBridgeDeviceBehindPeg2;                ///< Offset 295     Is bridge device behind Peg2
  UINT8    IsBridgeDeviceBehindPeg3;                ///< Offset 296     Is bridge device behind Peg3
  UINT16   RegBarOffset;                            ///< Offset 297     MCH RegBar Offset
  UINT64   IomBase;                                 ///< Offset 299     Iom RegBar Base
  UINT8    RetimerMap;                              ///< Offset 307     USBC Retimer Mapping.
  UINT32   PcieRpCcfgOffset;                        ///< Offset 308     TCSS PCIE RP Channel Configuration Offset
} SYSTEM_AGENT_NVS_AREA;

#pragma pack(pop)
#endif
