
/**@file

 Copyright (c) 2013 - 2019, Intel Corporation. All rights reserved.<BR>
 SPDX-License-Identifier: BSD-2-Clause-Patent
**/

  //
  // Define PCH NVS Area operatino region.
  //
#ifndef _PCH_NVS_AREA_DEF_H_
#define _PCH_NVS_AREA_DEF_H_

#pragma pack (push,1)
typedef struct {
  UINT16   PchSeries;                               ///< Offset 0       PCH Series
  UINT16   PchGeneration;                           ///< Offset 2       PCH Generation
  UINT16   PchStepping;                             ///< Offset 4       PCH Stepping
  UINT32   RpAddress[24];                           ///< Offset 6       Root Port address 1
                                                    ///< Offset 10      Root Port address 2
                                                    ///< Offset 14      Root Port address 3
                                                    ///< Offset 18      Root Port address 4
                                                    ///< Offset 22      Root Port address 5
                                                    ///< Offset 26      Root Port address 6
                                                    ///< Offset 30      Root Port address 7
                                                    ///< Offset 34      Root Port address 8
                                                    ///< Offset 38      Root Port address 9
                                                    ///< Offset 42      Root Port address 10
                                                    ///< Offset 46      Root Port address 11
                                                    ///< Offset 50      Root Port address 12
                                                    ///< Offset 54      Root Port address 13
                                                    ///< Offset 58      Root Port address 14
                                                    ///< Offset 62      Root Port address 15
                                                    ///< Offset 66      Root Port address 16
                                                    ///< Offset 70      Root Port address 17
                                                    ///< Offset 74      Root Port address 18
                                                    ///< Offset 78      Root Port address 19
                                                    ///< Offset 82      Root Port address 20
                                                    ///< Offset 86      Root Port address 21
                                                    ///< Offset 90      Root Port address 22
                                                    ///< Offset 94      Root Port address 23
                                                    ///< Offset 98      Root Port address 24
  UINT64   NHLA;                                    ///< Offset 102     HD-Audio NHLT ACPI address
  UINT32   NHLL;                                    ///< Offset 110     HD-Audio NHLT ACPI length
  UINT32   ADFM;                                    ///< Offset 114     HD-Audio DSP Feature Mask
  UINT8    SWQ0;                                    ///< Offset 118     HD-Audio SoundWire Link #1 quirk mask
  UINT8    SWQ1;                                    ///< Offset 119     HD-Audio SoundWire Link #2 quirk mask
  UINT8    SWQ2;                                    ///< Offset 120     HD-Audio SoundWire Link #3 quirk mask
  UINT8    SWQ3;                                    ///< Offset 121     HD-Audio SoundWire Link #4 quirk mask
  UINT8    ACS0;                                    ///< Offset 122     HD-Audio SoundWire Link #1 Autonomous Clock Stop
  UINT8    ACS1;                                    ///< Offset 123     HD-Audio SoundWire Link #2 Autonomous Clock Stop
  UINT8    ACS2;                                    ///< Offset 124     HD-Audio SoundWire Link #3 Autonomous Clock Stop
  UINT8    ACS3;                                    ///< Offset 125     HD-Audio SoundWire Link #4 Autonomous Clock Stop
  UINT8    DAI0;                                    ///< Offset 126     HD-Audio SoundWire Link #1 Data On Active Interval Select
  UINT8    DAI1;                                    ///< Offset 127     HD-Audio SoundWire Link #2 Data On Active Interval Select
  UINT8    DAI2;                                    ///< Offset 128     HD-Audio SoundWire Link #3 Data On Active Interval Select
  UINT8    DAI3;                                    ///< Offset 129     HD-Audio SoundWire Link #4 Data On Active Interval Select
  UINT8    DOD0;                                    ///< Offset 130     HD-Audio SoundWire Link #1 Data On Delay Select
  UINT8    DOD1;                                    ///< Offset 131     HD-Audio SoundWire Link #2 Data On Delay Select
  UINT8    DOD2;                                    ///< Offset 132     HD-Audio SoundWire Link #3 Data On Delay Select
  UINT8    DOD3;                                    ///< Offset 133     HD-Audio SoundWire Link #4 Data On Delay Select
  UINT8    XTAL;                                    ///< Offset 134     XTAL frequency: 0: 24MHz; 1: 38.4MHz; 2: Unsupported
  UINT32   DSPM;                                    ///< Offset 135     HD-Audio DSP Stolen Memory Base Address (@todo: Remove after CNL-LP B0)
  UINT32   SBRG;                                    ///< Offset 139     SBREG_BAR
  UINT8    GEI0;                                    ///< Offset 143     GPIO GroupIndex mapped to GPE_DW0
  UINT8    GEI1;                                    ///< Offset 144     GPIO GroupIndex mapped to GPE_DW1
  UINT8    GEI2;                                    ///< Offset 145     GPIO GroupIndex mapped to GPE_DW2
  UINT8    GED0;                                    ///< Offset 146     GPIO DW part of group mapped to GPE_DW0
  UINT8    GED1;                                    ///< Offset 147     GPIO DW part of group mapped to GPE_DW1
  UINT8    GED2;                                    ///< Offset 148     GPIO DW part of group mapped to GPE_DW2
  UINT16   PcieLtrMaxSnoopLatency[24];              ///< Offset 149     PCIE LTR max snoop Latency 1
                                                    ///< Offset 151     PCIE LTR max snoop Latency 2
                                                    ///< Offset 153     PCIE LTR max snoop Latency 3
                                                    ///< Offset 155     PCIE LTR max snoop Latency 4
                                                    ///< Offset 157     PCIE LTR max snoop Latency 5
                                                    ///< Offset 159     PCIE LTR max snoop Latency 6
                                                    ///< Offset 161     PCIE LTR max snoop Latency 7
                                                    ///< Offset 163     PCIE LTR max snoop Latency 8
                                                    ///< Offset 165     PCIE LTR max snoop Latency 9
                                                    ///< Offset 167     PCIE LTR max snoop Latency 10
                                                    ///< Offset 169     PCIE LTR max snoop Latency 11
                                                    ///< Offset 171     PCIE LTR max snoop Latency 12
                                                    ///< Offset 173     PCIE LTR max snoop Latency 13
                                                    ///< Offset 175     PCIE LTR max snoop Latency 14
                                                    ///< Offset 177     PCIE LTR max snoop Latency 15
                                                    ///< Offset 179     PCIE LTR max snoop Latency 16
                                                    ///< Offset 181     PCIE LTR max snoop Latency 17
                                                    ///< Offset 183     PCIE LTR max snoop Latency 18
                                                    ///< Offset 185     PCIE LTR max snoop Latency 19
                                                    ///< Offset 187     PCIE LTR max snoop Latency 20
                                                    ///< Offset 189     PCIE LTR max snoop Latency 21
                                                    ///< Offset 191     PCIE LTR max snoop Latency 22
                                                    ///< Offset 193     PCIE LTR max snoop Latency 23
                                                    ///< Offset 195     PCIE LTR max snoop Latency 24
  UINT16   PcieLtrMaxNoSnoopLatency[24];            ///< Offset 197     PCIE LTR max no snoop Latency 1
                                                    ///< Offset 199     PCIE LTR max no snoop Latency 2
                                                    ///< Offset 201     PCIE LTR max no snoop Latency 3
                                                    ///< Offset 203     PCIE LTR max no snoop Latency 4
                                                    ///< Offset 205     PCIE LTR max no snoop Latency 5
                                                    ///< Offset 207     PCIE LTR max no snoop Latency 6
                                                    ///< Offset 209     PCIE LTR max no snoop Latency 7
                                                    ///< Offset 211     PCIE LTR max no snoop Latency 8
                                                    ///< Offset 213     PCIE LTR max no snoop Latency 9
                                                    ///< Offset 215     PCIE LTR max no snoop Latency 10
                                                    ///< Offset 217     PCIE LTR max no snoop Latency 11
                                                    ///< Offset 219     PCIE LTR max no snoop Latency 12
                                                    ///< Offset 221     PCIE LTR max no snoop Latency 13
                                                    ///< Offset 223     PCIE LTR max no snoop Latency 14
                                                    ///< Offset 225     PCIE LTR max no snoop Latency 15
                                                    ///< Offset 227     PCIE LTR max no snoop Latency 16
                                                    ///< Offset 229     PCIE LTR max no snoop Latency 17
                                                    ///< Offset 231     PCIE LTR max no snoop Latency 18
                                                    ///< Offset 233     PCIE LTR max no snoop Latency 19
                                                    ///< Offset 235     PCIE LTR max no snoop Latency 20
                                                    ///< Offset 237     PCIE LTR max no snoop Latency 21
                                                    ///< Offset 239     PCIE LTR max no snoop Latency 22
                                                    ///< Offset 241     PCIE LTR max no snoop Latency 23
                                                    ///< Offset 243     PCIE LTR max no snoop Latency 24
  UINT8    XHPC;                                    ///< Offset 245     Number of HighSpeed ports implemented in XHCI controller
  UINT8    XRPC;                                    ///< Offset 246     Number of USBR ports implemented in XHCI controller
  UINT8    XSPC;                                    ///< Offset 247     Number of SuperSpeed ports implemented in XHCI controller
  UINT8    XSPA;                                    ///< Offset 248     Address of 1st SuperSpeed port
  UINT32   HPTB;                                    ///< Offset 249     HPET base address
  UINT8    HPTE;                                    ///< Offset 253     HPET enable
  //SerialIo block
  UINT8    SM0[7];                                  ///< Offset 254     SerialIo SPI Controller 0 Mode
                                                    ///< Offset 255     SerialIo SPI Controller 1 Mode
                                                    ///< Offset 256     SerialIo SPI Controller 2 Mode
                                                    ///< Offset 257     SerialIo SPI Controller 3 Mode
                                                    ///< Offset 258     SerialIo SPI Controller 4 Mode
                                                    ///< Offset 259     SerialIo SPI Controller 5 Mode
                                                    ///< Offset 260     SerialIo SPI Controller 6 Mode
  UINT64   SC0[7];                                  ///< Offset 261     SerialIo SPI Controller 0 Pci Config
                                                    ///< Offset 269     SerialIo SPI Controller 1 Pci Config
                                                    ///< Offset 277     SerialIo SPI Controller 2 Pci Config
                                                    ///< Offset 285     SerialIo SPI Controller 3 Pci Config
                                                    ///< Offset 293     SerialIo SPI Controller 4 Pci Config
                                                    ///< Offset 301     SerialIo SPI Controller 5 Pci Config
                                                    ///< Offset 309     SerialIo SPI Controller 6 Pci Config
  UINT8    IM0[8];                                  ///< Offset 317     SerialIo I2C Controller 0 Mode
                                                    ///< Offset 318     SerialIo I2C Controller 1 Mode
                                                    ///< Offset 319     SerialIo I2C Controller 2 Mode
                                                    ///< Offset 320     SerialIo I2C Controller 3 Mode
                                                    ///< Offset 321     SerialIo I2C Controller 4 Mode
                                                    ///< Offset 322     SerialIo I2C Controller 5 Mode
                                                    ///< Offset 323     SerialIo I2C Controller 6 Mode
                                                    ///< Offset 324     SerialIo I2C Controller 7 Mode
  UINT64   IC0[8];                                  ///< Offset 325     SerialIo I2C Controller 0 Pci Config
                                                    ///< Offset 333     SerialIo I2C Controller 1 Pci Config
                                                    ///< Offset 341     SerialIo I2C Controller 2 Pci Config
                                                    ///< Offset 349     SerialIo I2C Controller 3 Pci Config
                                                    ///< Offset 357     SerialIo I2C Controller 4 Pci Config
                                                    ///< Offset 365     SerialIo I2C Controller 5 Pci Config
                                                    ///< Offset 373     SerialIo I2C Controller 6 Pci Config
                                                    ///< Offset 381     SerialIo I2C Controller 7 Pci Config
  UINT8    UM0[7];                                  ///< Offset 389     SerialIo UART Controller 0 Mode
                                                    ///< Offset 390     SerialIo UART Controller 1 Mode
                                                    ///< Offset 391     SerialIo UART Controller 2 Mode
                                                    ///< Offset 392     SerialIo UART Controller 3 Mode
                                                    ///< Offset 393     SerialIo UART Controller 4 Mode
                                                    ///< Offset 394     SerialIo UART Controller 5 Mode
                                                    ///< Offset 395     SerialIo UART Controller 6 Mode
  UINT64   UC0[7];                                  ///< Offset 396     SerialIo UART Controller 0 Pci Config
                                                    ///< Offset 404     SerialIo UART Controller 1 Pci Config
                                                    ///< Offset 412     SerialIo UART Controller 2 Pci Config
                                                    ///< Offset 420     SerialIo UART Controller 3 Pci Config
                                                    ///< Offset 428     SerialIo UART Controller 4 Pci Config
                                                    ///< Offset 436     SerialIo UART Controller 5 Pci Config
                                                    ///< Offset 444     SerialIo UART Controller 6 Pci Config
  UINT8    UD0[7];                                  ///< Offset 452     SerialIo UART Controller 0 DmaEnable
                                                    ///< Offset 453     SerialIo UART Controller 1 DmaEnable
                                                    ///< Offset 454     SerialIo UART Controller 2 DmaEnable
                                                    ///< Offset 455     SerialIo UART Controller 3 DmaEnable
                                                    ///< Offset 456     SerialIo UART Controller 4 DmaEnable
                                                    ///< Offset 457     SerialIo UART Controller 5 DmaEnable
                                                    ///< Offset 458     SerialIo UART Controller 6 DmaEnable
  UINT8    UP0[7];                                  ///< Offset 459     SerialIo UART Controller 0 Power Gating
                                                    ///< Offset 460     SerialIo UART Controller 1 Power Gating
                                                    ///< Offset 461     SerialIo UART Controller 2 Power Gating
                                                    ///< Offset 462     SerialIo UART Controller 3 Power Gating
                                                    ///< Offset 463     SerialIo UART Controller 4 Power Gating
                                                    ///< Offset 464     SerialIo UART Controller 5 Power Gating
                                                    ///< Offset 465     SerialIo UART Controller 6 Power Gating
  UINT8    UI0[7];                                  ///< Offset 466     SerialIo UART Controller 0 Irq
                                                    ///< Offset 467     SerialIo UART Controller 1 Irq
                                                    ///< Offset 468     SerialIo UART Controller 2 Irq
                                                    ///< Offset 469     SerialIo UART Controller 3 Irq
                                                    ///< Offset 470     SerialIo UART Controller 4 Irq
                                                    ///< Offset 471     SerialIo UART Controller 5 Irq
                                                    ///< Offset 472     SerialIo UART Controller 6 Irq
  //end of SerialIo block
  UINT8    SGIR;                                    ///< Offset 473     GPIO IRQ
  UINT8    GPHD;                                    ///< Offset 474     Hide GPIO ACPI device
  UINT8    RstPcieStorageInterfaceType[3];          ///< Offset 475     RST PCIe Storage Cycle Router#1 Interface Type
                                                    ///< Offset 476     RST PCIe Storage Cycle Router#2 Interface Type
                                                    ///< Offset 477     RST PCIe Storage Cycle Router#3 Interface Type
  UINT8    RstPcieStoragePmCapPtr[3];               ///< Offset 478     RST PCIe Storage Cycle Router#1 Power Management Capability Pointer
                                                    ///< Offset 479     RST PCIe Storage Cycle Router#2 Power Management Capability Pointer
                                                    ///< Offset 480     RST PCIe Storage Cycle Router#3 Power Management Capability Pointer
  UINT8    RstPcieStoragePcieCapPtr[3];             ///< Offset 481     RST PCIe Storage Cycle Router#1 PCIe Capabilities Pointer
                                                    ///< Offset 482     RST PCIe Storage Cycle Router#2 PCIe Capabilities Pointer
                                                    ///< Offset 483     RST PCIe Storage Cycle Router#3 PCIe Capabilities Pointer
  UINT16   RstPcieStorageL1ssCapPtr[3];             ///< Offset 484     RST PCIe Storage Cycle Router#1 L1SS Capability Pointer
                                                    ///< Offset 486     RST PCIe Storage Cycle Router#2 L1SS Capability Pointer
                                                    ///< Offset 488     RST PCIe Storage Cycle Router#3 L1SS Capability Pointer
  UINT8    RstPcieStorageEpL1ssControl2[3];         ///< Offset 490     RST PCIe Storage Cycle Router#1 Endpoint L1SS Control Data2
                                                    ///< Offset 491     RST PCIe Storage Cycle Router#2 Endpoint L1SS Control Data2
                                                    ///< Offset 492     RST PCIe Storage Cycle Router#3 Endpoint L1SS Control Data2
  UINT32   RstPcieStorageEpL1ssControl1[3];         ///< Offset 493     RST PCIe Storage Cycle Router#1 Endpoint L1SS Control Data1
                                                    ///< Offset 497     RST PCIe Storage Cycle Router#2 Endpoint L1SS Control Data1
                                                    ///< Offset 501     RST PCIe Storage Cycle Router#3 Endpoint L1SS Control Data1
  UINT16   RstPcieStorageLtrCapPtr[3];              ///< Offset 505     RST PCIe Storage Cycle Router#1 LTR Capability Pointer
                                                    ///< Offset 507     RST PCIe Storage Cycle Router#2 LTR Capability Pointer
                                                    ///< Offset 509     RST PCIe Storage Cycle Router#3 LTR Capability Pointer
  UINT32   RstPcieStorageEpLtrData[3];              ///< Offset 511     RST PCIe Storage Cycle Router#1 Endpoint LTR Data
                                                    ///< Offset 515     RST PCIe Storage Cycle Router#2 Endpoint LTR Data
                                                    ///< Offset 519     RST PCIe Storage Cycle Router#3 Endpoint LTR Data
  UINT16   RstPcieStorageEpLctlData16[3];           ///< Offset 523     RST PCIe Storage Cycle Router#1 Endpoint LCTL Data
                                                    ///< Offset 525     RST PCIe Storage Cycle Router#2 Endpoint LCTL Data
                                                    ///< Offset 527     RST PCIe Storage Cycle Router#3 Endpoint LCTL Data
  UINT16   RstPcieStorageEpDctlData16[3];           ///< Offset 529     RST PCIe Storage Cycle Router#1 Endpoint DCTL Data
                                                    ///< Offset 531     RST PCIe Storage Cycle Router#2 Endpoint DCTL Data
                                                    ///< Offset 533     RST PCIe Storage Cycle Router#3 Endpoint DCTL Data
  UINT16   RstPcieStorageEpDctl2Data16[3];          ///< Offset 535     RST PCIe Storage Cycle Router#1 Endpoint DCTL2 Data
                                                    ///< Offset 537     RST PCIe Storage Cycle Router#2 Endpoint DCTL2 Data
                                                    ///< Offset 539     RST PCIe Storage Cycle Router#3 Endpoint DCTL2 Data
  UINT16   RstPcieStorageRpDctl2Data16[3];          ///< Offset 541     RST PCIe Storage Cycle Router#1 RootPort DCTL2 Data
                                                    ///< Offset 543     RST PCIe Storage Cycle Router#2 RootPort DCTL2 Data
                                                    ///< Offset 545     RST PCIe Storage Cycle Router#3 RootPort DCTL2 Data
  UINT32   RstPcieStorageUniqueTableBar[3];         ///< Offset 547     RST PCIe Storage Cycle Router#1 Endpoint unique MSI-X Table BAR
                                                    ///< Offset 551     RST PCIe Storage Cycle Router#2 Endpoint unique MSI-X Table BAR
                                                    ///< Offset 555     RST PCIe Storage Cycle Router#3 Endpoint unique MSI-X Table BAR
  UINT32   RstPcieStorageUniqueTableBarValue[3];    ///< Offset 559     RST PCIe Storage Cycle Router#1 Endpoint unique MSI-X Table BAR value
                                                    ///< Offset 563     RST PCIe Storage Cycle Router#2 Endpoint unique MSI-X Table BAR value
                                                    ///< Offset 567     RST PCIe Storage Cycle Router#3 Endpoint unique MSI-X Table BAR value
  UINT32   RstPcieStorageUniquePbaBar[3];           ///< Offset 571     RST PCIe Storage Cycle Router#1 Endpoint unique MSI-X PBA BAR
                                                    ///< Offset 575     RST PCIe Storage Cycle Router#2 Endpoint unique MSI-X PBA BAR
                                                    ///< Offset 579     RST PCIe Storage Cycle Router#3 Endpoint unique MSI-X PBA BAR
  UINT32   RstPcieStorageUniquePbaBarValue[3];      ///< Offset 583     RST PCIe Storage Cycle Router#1 Endpoint unique MSI-X PBA BAR value
                                                    ///< Offset 587     RST PCIe Storage Cycle Router#2 Endpoint unique MSI-X PBA BAR value
                                                    ///< Offset 591     RST PCIe Storage Cycle Router#3 Endpoint unique MSI-X PBA BAR value
  UINT32   RstPcieStorageRootPortNum[3];            ///< Offset 595     RST PCIe Storage Cycle Router#1 Root Port number
                                                    ///< Offset 599     RST PCIe Storage Cycle Router#2 Root Port number
                                                    ///< Offset 603     RST PCIe Storage Cycle Router#3 Root Port number
  UINT8    EMH4;                                    ///< Offset 607     eMMC HS400 mode enabled
  UINT8    EMDS;                                    ///< Offset 608     eMMC Driver Strength
  UINT8    CpuSku;                                  ///< Offset 609     CPU SKU
  UINT16   IoTrapAddress[4];                        ///< Offset 610
  UINT8    IoTrapStatus[4];                         ///< Offset 618
  UINT16   PMBS;                                    ///< Offset 622     ACPI IO BASE address
  UINT32   PWRM;                                    ///< Offset 624     PWRM MEM BASE address
  // CNVi specific
  UINT8    CnviMode;                                ///< Offset 628     CNVi mode
  UINT8    CnviBtCore;                              ///< Offset 629     CNVi BT Core
  UINT8    CnviBtAudioOffload;                      ///< Offset 630     CNVi BT Audio Offload
  // PCH Trace Hub
  UINT8    PchTraceHubMode;                         ///< Offset 631     PCH Trace Hub Mode
  // PCH PS_ON support
  UINT8    PsOnEnable;                              ///< Offset 632     PCH PS_ON enable
  //
  // These are for PchApciTablesSelfTest use
  //
  UINT8    LtrEnable[24];                           ///< Offset 633     Latency Tolerance Reporting Enable
                                                    ///< Offset 634     Latency Tolerance Reporting Enable
                                                    ///< Offset 635     Latency Tolerance Reporting Enable
                                                    ///< Offset 636     Latency Tolerance Reporting Enable
                                                    ///< Offset 637     Latency Tolerance Reporting Enable
                                                    ///< Offset 638     Latency Tolerance Reporting Enable
                                                    ///< Offset 639     Latency Tolerance Reporting Enable
                                                    ///< Offset 640     Latency Tolerance Reporting Enable
                                                    ///< Offset 641     Latency Tolerance Reporting Enable
                                                    ///< Offset 642     Latency Tolerance Reporting Enable
                                                    ///< Offset 643     Latency Tolerance Reporting Enable
                                                    ///< Offset 644     Latency Tolerance Reporting Enable
                                                    ///< Offset 645     Latency Tolerance Reporting Enable
                                                    ///< Offset 646     Latency Tolerance Reporting Enable
                                                    ///< Offset 647     Latency Tolerance Reporting Enable
                                                    ///< Offset 648     Latency Tolerance Reporting Enable
                                                    ///< Offset 649     Latency Tolerance Reporting Enable
                                                    ///< Offset 650     Latency Tolerance Reporting Enable
                                                    ///< Offset 651     Latency Tolerance Reporting Enable
                                                    ///< Offset 652     Latency Tolerance Reporting Enable
                                                    ///< Offset 653     Latency Tolerance Reporting Enable
                                                    ///< Offset 654     Latency Tolerance Reporting Enable
                                                    ///< Offset 655     Latency Tolerance Reporting Enable
                                                    ///< Offset 656     Latency Tolerance Reporting Enable
  UINT8    GBES;                                    ///< Offset 657     GbE Support
  UINT32   PchxDCIPwrDnScale;                       ///< Offset 658     PCH xDCI Power Down Scale Value, DWC_USB3_GCTL_INIT[31:19]
  UINT8    EmmcEnabled;                             ///< Offset 662     Set to indicate that eMMC is enabled
  UINT8    SdCardEnabled;                           ///< Offset 663     Set to indicate that SD card is enabled
  UINT8    EnableTimedGpio0;                        ///< Offset 664     Set to indicate that Timed GPIO 0 is enabled
  UINT8    EnableTimedGpio1;                        ///< Offset 665     Set to indicate that Timed GPIO 1 is enabled
  UINT8    ClockToRootPortMap[16];                  ///< Offset 666     CLOCK index to root port index map. Used during PCIe D3Cold flows
  UINT16   TcoBase;                                 ///< Offset 682     TCO base address
  UINT16   IclkPid;                                 ///< Offset 684     Iclk PID number
  UINT32   PseDma1Address;                          ///< Offset 686     PSE DMA1 Address
  UINT32   PseDma1Length;                           ///< Offset 690     PSE DMA1 Length
  UINT32   PseDma2Address;                          ///< Offset 694     PSE DMA2 Address
  UINT32   PseDma2Length;                           ///< Offset 698     PSE DMA2 Length
  UINT8    PseDma1En;                               ///< Offset 702     PSE DMA1 Enable
  UINT8    PseDma2En;                               ///< Offset 703     PSE DMA2 Enable
  UINT8    Ufs0Enabled;                             ///< Offset 704     Is UFS0 Enabled
  UINT8    Ufs1Enabled;                             ///< Offset 705     Is UFS1 Enabled
  UINT8    PseCan0Enabled;                          ///< Offset 706     PSE Can0 Enabled
  UINT8    PseCan1Enabled;                          ///< Offset 707     PSE Can1 Enabled
} PCH_NVS_AREA;

#pragma pack(pop)
#endif
