/**@file

  Copyright (c) 2023, Intel Corporation. All rights reserved.<BR>
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
  UINT32   RpAddress[28];                           ///< Offset 6       Root Port address 1
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
                                                    ///< Offset 102     Root Port address 25
                                                    ///< Offset 106     Root Port address 26
                                                    ///< Offset 110     Root Port address 27
                                                    ///< Offset 114     Root Port address 28
  UINT64   NHLA[8];                                 ///< Offset 118     HD-Audio NHLT ACPI address 0
                                                    ///< Offset 126     HD-Audio NHLT ACPI address 1
                                                    ///< Offset 134     HD-Audio NHLT ACPI address 2
                                                    ///< Offset 142     HD-Audio NHLT ACPI address 3
                                                    ///< Offset 150     HD-Audio NHLT ACPI address 4
                                                    ///< Offset 158     HD-Audio NHLT ACPI address 5
                                                    ///< Offset 166     HD-Audio NHLT ACPI address 6
                                                    ///< Offset 174     HD-Audio NHLT ACPI address 7
  UINT32   NHLL[8];                                 ///< Offset 182     HD-Audio NHLT ACPI length  0
                                                    ///< Offset 186     HD-Audio NHLT ACPI length  1
                                                    ///< Offset 190     HD-Audio NHLT ACPI length  2
                                                    ///< Offset 194     HD-Audio NHLT ACPI length  3
                                                    ///< Offset 198     HD-Audio NHLT ACPI length  4
                                                    ///< Offset 202     HD-Audio NHLT ACPI length  5
                                                    ///< Offset 206     HD-Audio NHLT ACPI length  6
                                                    ///< Offset 210     HD-Audio NHLT ACPI length  7
  UINT32   ADFM;                                    ///< Offset 214     HD-Audio DSP Feature Mask
  UINT8    SWQ0;                                    ///< Offset 218     HD-Audio SoundWire Link #1 quirk mask
  UINT8    SWQ1;                                    ///< Offset 219     HD-Audio SoundWire Link #2 quirk mask
  UINT8    SWQ2;                                    ///< Offset 220     HD-Audio SoundWire Link #3 quirk mask
  UINT8    SWQ3;                                    ///< Offset 221     HD-Audio SoundWire Link #4 quirk mask
  UINT8    ACS0;                                    ///< Offset 222     HD-Audio SoundWire Link #1 Autonomous Clock Stop
  UINT8    ACS1;                                    ///< Offset 223     HD-Audio SoundWire Link #2 Autonomous Clock Stop
  UINT8    ACS2;                                    ///< Offset 224     HD-Audio SoundWire Link #3 Autonomous Clock Stop
  UINT8    ACS3;                                    ///< Offset 225     HD-Audio SoundWire Link #4 Autonomous Clock Stop
  UINT8    DAI0;                                    ///< Offset 226     HD-Audio SoundWire Link #1 Data On Active Interval Select
  UINT8    DAI1;                                    ///< Offset 227     HD-Audio SoundWire Link #2 Data On Active Interval Select
  UINT8    DAI2;                                    ///< Offset 228     HD-Audio SoundWire Link #3 Data On Active Interval Select
  UINT8    DAI3;                                    ///< Offset 229     HD-Audio SoundWire Link #4 Data On Active Interval Select
  UINT8    DOD0;                                    ///< Offset 230     HD-Audio SoundWire Link #1 Data On Delay Select
  UINT8    DOD1;                                    ///< Offset 231     HD-Audio SoundWire Link #2 Data On Delay Select
  UINT8    DOD2;                                    ///< Offset 232     HD-Audio SoundWire Link #3 Data On Delay Select
  UINT8    DOD3;                                    ///< Offset 233     HD-Audio SoundWire Link #4 Data On Delay Select
  UINT16   HDBO;                                    ///< Offset 234     HD-Audio Discrete BT Offload
  UINT8    SWMC;                                    ///< Offset 236     HD-Audio SoundWire Host Count
  UINT32   XTAL;                                    ///< Offset 237     XTAL frequency: 0: 24MHz; 1: 38.4MHz; 2: Unsupported
  UINT8    AFDF;                                    ///< Offset 241     Audio FPGA Number of Device and Function. 0: Audio FPGA not detected
  UINT32   SBRG;                                    ///< Offset 242     SBREG_BAR
  UINT8    GEI0;                                    ///< Offset 246     GPIO GroupIndex mapped to GPE_DW0
  UINT8    GEI1;                                    ///< Offset 247     GPIO GroupIndex mapped to GPE_DW1
  UINT8    GEI2;                                    ///< Offset 248     GPIO GroupIndex mapped to GPE_DW2
  UINT8    GED0;                                    ///< Offset 249     GPIO DW part of group mapped to GPE_DW0
  UINT8    GED1;                                    ///< Offset 250     GPIO DW part of group mapped to GPE_DW1
  UINT8    GED2;                                    ///< Offset 251     GPIO DW part of group mapped to GPE_DW2
  UINT16   PcieLtrMaxSnoopLatency[28];              ///< Offset 252     PCIE LTR max snoop Latency 1
                                                    ///< Offset 254     PCIE LTR max snoop Latency 2
                                                    ///< Offset 256     PCIE LTR max snoop Latency 3
                                                    ///< Offset 258     PCIE LTR max snoop Latency 4
                                                    ///< Offset 260     PCIE LTR max snoop Latency 5
                                                    ///< Offset 262     PCIE LTR max snoop Latency 6
                                                    ///< Offset 264     PCIE LTR max snoop Latency 7
                                                    ///< Offset 266     PCIE LTR max snoop Latency 8
                                                    ///< Offset 268     PCIE LTR max snoop Latency 9
                                                    ///< Offset 270     PCIE LTR max snoop Latency 10
                                                    ///< Offset 272     PCIE LTR max snoop Latency 11
                                                    ///< Offset 274     PCIE LTR max snoop Latency 12
                                                    ///< Offset 276     PCIE LTR max snoop Latency 13
                                                    ///< Offset 278     PCIE LTR max snoop Latency 14
                                                    ///< Offset 280     PCIE LTR max snoop Latency 15
                                                    ///< Offset 282     PCIE LTR max snoop Latency 16
                                                    ///< Offset 284     PCIE LTR max snoop Latency 17
                                                    ///< Offset 286     PCIE LTR max snoop Latency 18
                                                    ///< Offset 288     PCIE LTR max snoop Latency 19
                                                    ///< Offset 290     PCIE LTR max snoop Latency 20
                                                    ///< Offset 292     PCIE LTR max snoop Latency 21
                                                    ///< Offset 294     PCIE LTR max snoop Latency 22
                                                    ///< Offset 296     PCIE LTR max snoop Latency 23
                                                    ///< Offset 298     PCIE LTR max snoop Latency 24
                                                    ///< Offset 300     PCIE LTR max snoop Latency 25
                                                    ///< Offset 302     PCIE LTR max snoop Latency 26
                                                    ///< Offset 304     PCIE LTR max snoop Latency 27
                                                    ///< Offset 306     PCIE LTR max snoop Latency 28
  UINT16   PcieLtrMaxNoSnoopLatency[28];            ///< Offset 308     PCIE LTR max no snoop Latency 1
                                                    ///< Offset 310     PCIE LTR max no snoop Latency 2
                                                    ///< Offset 312     PCIE LTR max no snoop Latency 3
                                                    ///< Offset 314     PCIE LTR max no snoop Latency 4
                                                    ///< Offset 316     PCIE LTR max no snoop Latency 5
                                                    ///< Offset 318     PCIE LTR max no snoop Latency 6
                                                    ///< Offset 320     PCIE LTR max no snoop Latency 7
                                                    ///< Offset 322     PCIE LTR max no snoop Latency 8
                                                    ///< Offset 324     PCIE LTR max no snoop Latency 9
                                                    ///< Offset 326     PCIE LTR max no snoop Latency 10
                                                    ///< Offset 328     PCIE LTR max no snoop Latency 11
                                                    ///< Offset 330     PCIE LTR max no snoop Latency 12
                                                    ///< Offset 332     PCIE LTR max no snoop Latency 13
                                                    ///< Offset 334     PCIE LTR max no snoop Latency 14
                                                    ///< Offset 336     PCIE LTR max no snoop Latency 15
                                                    ///< Offset 338     PCIE LTR max no snoop Latency 16
                                                    ///< Offset 340     PCIE LTR max no snoop Latency 17
                                                    ///< Offset 342     PCIE LTR max no snoop Latency 18
                                                    ///< Offset 344     PCIE LTR max no snoop Latency 19
                                                    ///< Offset 346     PCIE LTR max no snoop Latency 20
                                                    ///< Offset 348     PCIE LTR max no snoop Latency 21
                                                    ///< Offset 350     PCIE LTR max no snoop Latency 22
                                                    ///< Offset 352     PCIE LTR max no snoop Latency 23
                                                    ///< Offset 354     PCIE LTR max no snoop Latency 24
                                                    ///< Offset 356     PCIE LTR max no snoop Latency 25
                                                    ///< Offset 358     PCIE LTR max no snoop Latency 26
                                                    ///< Offset 360     PCIE LTR max no snoop Latency 27
                                                    ///< Offset 362     PCIE LTR max no snoop Latency 28
  UINT8    XHPC;                                    ///< Offset 364     Number of HighSpeed ports implemented in XHCI controller
  UINT8    XRPC;                                    ///< Offset 365     Number of USBR ports implemented in XHCI controller
  UINT8    XSPC;                                    ///< Offset 366     Number of SuperSpeed ports implemented in XHCI controller
  UINT8    XSPA;                                    ///< Offset 367     Address of 1st SuperSpeed port
  UINT32   HPTB;                                    ///< Offset 368     HPET base address
  UINT8    HPTE;                                    ///< Offset 372     HPET enable
  //SerialIo block
  UINT8    SM0[7];                                  ///< Offset 373     SerialIo SPI Controller 0 Mode
                                                    ///< Offset 374     SerialIo SPI Controller 1 Mode
                                                    ///< Offset 375     SerialIo SPI Controller 2 Mode
                                                    ///< Offset 376     SerialIo SPI Controller 3 Mode
                                                    ///< Offset 377     SerialIo SPI Controller 4 Mode
                                                    ///< Offset 378     SerialIo SPI Controller 5 Mode
                                                    ///< Offset 379     SerialIo SPI Controller 6 Mode
  UINT64   SC0[7];                                  ///< Offset 380     SerialIo SPI Controller 0 Pci Config
                                                    ///< Offset 388     SerialIo SPI Controller 1 Pci Config
                                                    ///< Offset 396     SerialIo SPI Controller 2 Pci Config
                                                    ///< Offset 404     SerialIo SPI Controller 3 Pci Config
                                                    ///< Offset 412     SerialIo SPI Controller 4 Pci Config
                                                    ///< Offset 420     SerialIo SPI Controller 5 Pci Config
                                                    ///< Offset 428     SerialIo SPI Controller 6 Pci Config
  UINT8    IM0[8];                                  ///< Offset 436     SerialIo I2C Controller 0 Mode
                                                    ///< Offset 437     SerialIo I2C Controller 1 Mode
                                                    ///< Offset 438     SerialIo I2C Controller 2 Mode
                                                    ///< Offset 439     SerialIo I2C Controller 3 Mode
                                                    ///< Offset 440     SerialIo I2C Controller 4 Mode
                                                    ///< Offset 441     SerialIo I2C Controller 5 Mode
                                                    ///< Offset 442     SerialIo I2C Controller 6 Mode
                                                    ///< Offset 443     SerialIo I2C Controller 7 Mode
  UINT64   IC0[8];                                  ///< Offset 444     SerialIo I2C Controller 0 Pci Config
                                                    ///< Offset 452     SerialIo I2C Controller 1 Pci Config
                                                    ///< Offset 460     SerialIo I2C Controller 2 Pci Config
                                                    ///< Offset 468     SerialIo I2C Controller 3 Pci Config
                                                    ///< Offset 476     SerialIo I2C Controller 4 Pci Config
                                                    ///< Offset 484     SerialIo I2C Controller 5 Pci Config
                                                    ///< Offset 492     SerialIo I2C Controller 6 Pci Config
                                                    ///< Offset 500     SerialIo I2C Controller 7 Pci Config
  UINT8    UM0[7];                                  ///< Offset 508     SerialIo UART Controller 0 Mode
                                                    ///< Offset 509     SerialIo UART Controller 1 Mode
                                                    ///< Offset 510     SerialIo UART Controller 2 Mode
                                                    ///< Offset 511     SerialIo UART Controller 3 Mode
                                                    ///< Offset 512     SerialIo UART Controller 4 Mode
                                                    ///< Offset 513     SerialIo UART Controller 5 Mode
                                                    ///< Offset 514     SerialIo UART Controller 6 Mode
  UINT64   UC0[7];                                  ///< Offset 515     SerialIo UART Controller 0 Pci Config
                                                    ///< Offset 523     SerialIo UART Controller 1 Pci Config
                                                    ///< Offset 531     SerialIo UART Controller 2 Pci Config
                                                    ///< Offset 539     SerialIo UART Controller 3 Pci Config
                                                    ///< Offset 547     SerialIo UART Controller 4 Pci Config
                                                    ///< Offset 555     SerialIo UART Controller 5 Pci Config
                                                    ///< Offset 563     SerialIo UART Controller 6 Pci Config
  UINT8    UD0[7];                                  ///< Offset 571     SerialIo UART Controller 0 DmaEnable
                                                    ///< Offset 572     SerialIo UART Controller 1 DmaEnable
                                                    ///< Offset 573     SerialIo UART Controller 2 DmaEnable
                                                    ///< Offset 574     SerialIo UART Controller 3 DmaEnable
                                                    ///< Offset 575     SerialIo UART Controller 4 DmaEnable
                                                    ///< Offset 576     SerialIo UART Controller 5 DmaEnable
                                                    ///< Offset 577     SerialIo UART Controller 6 DmaEnable
  UINT8    UP0[7];                                  ///< Offset 578     SerialIo UART Controller 0 Power Gating
                                                    ///< Offset 579     SerialIo UART Controller 1 Power Gating
                                                    ///< Offset 580     SerialIo UART Controller 2 Power Gating
                                                    ///< Offset 581     SerialIo UART Controller 3 Power Gating
                                                    ///< Offset 582     SerialIo UART Controller 4 Power Gating
                                                    ///< Offset 583     SerialIo UART Controller 5 Power Gating
                                                    ///< Offset 584     SerialIo UART Controller 6 Power Gating
  UINT8    UI0[7];                                  ///< Offset 585     SerialIo UART Controller 0 Irq
                                                    ///< Offset 586     SerialIo UART Controller 1 Irq
                                                    ///< Offset 587     SerialIo UART Controller 2 Irq
                                                    ///< Offset 588     SerialIo UART Controller 3 Irq
                                                    ///< Offset 589     SerialIo UART Controller 4 Irq
                                                    ///< Offset 590     SerialIo UART Controller 5 Irq
                                                    ///< Offset 591     SerialIo UART Controller 6 Irq
  //end of SerialIo block
  UINT8    SGIR;                                    ///< Offset 592     GPIO IRQ
  UINT8    GPHD;                                    ///< Offset 593     Hide GPIO ACPI device
  UINT8    RstPcieStorageInterfaceType[3];          ///< Offset 594     RST PCIe Storage Cycle Router#1 Interface Type
                                                    ///< Offset 595     RST PCIe Storage Cycle Router#2 Interface Type
                                                    ///< Offset 596     RST PCIe Storage Cycle Router#3 Interface Type
  UINT8    RstPcieStoragePmCapPtr[3];               ///< Offset 597     RST PCIe Storage Cycle Router#1 Power Management Capability Pointer
                                                    ///< Offset 598     RST PCIe Storage Cycle Router#2 Power Management Capability Pointer
                                                    ///< Offset 599     RST PCIe Storage Cycle Router#3 Power Management Capability Pointer
  UINT8    RstPcieStoragePcieCapPtr[3];             ///< Offset 600     RST PCIe Storage Cycle Router#1 PCIe Capabilities Pointer
                                                    ///< Offset 601     RST PCIe Storage Cycle Router#2 PCIe Capabilities Pointer
                                                    ///< Offset 602     RST PCIe Storage Cycle Router#3 PCIe Capabilities Pointer
  UINT16   RstPcieStorageL1ssCapPtr[3];             ///< Offset 603     RST PCIe Storage Cycle Router#1 L1SS Capability Pointer
                                                    ///< Offset 605     RST PCIe Storage Cycle Router#2 L1SS Capability Pointer
                                                    ///< Offset 607     RST PCIe Storage Cycle Router#3 L1SS Capability Pointer
  UINT8    RstPcieStorageEpL1ssControl2[3];         ///< Offset 609     RST PCIe Storage Cycle Router#1 Endpoint L1SS Control Data2
                                                    ///< Offset 610     RST PCIe Storage Cycle Router#2 Endpoint L1SS Control Data2
                                                    ///< Offset 611     RST PCIe Storage Cycle Router#3 Endpoint L1SS Control Data2
  UINT32   RstPcieStorageEpL1ssControl1[3];         ///< Offset 612     RST PCIe Storage Cycle Router#1 Endpoint L1SS Control Data1
                                                    ///< Offset 616     RST PCIe Storage Cycle Router#2 Endpoint L1SS Control Data1
                                                    ///< Offset 620     RST PCIe Storage Cycle Router#3 Endpoint L1SS Control Data1
  UINT16   RstPcieStorageLtrCapPtr[3];              ///< Offset 624     RST PCIe Storage Cycle Router#1 LTR Capability Pointer
                                                    ///< Offset 626     RST PCIe Storage Cycle Router#2 LTR Capability Pointer
                                                    ///< Offset 628     RST PCIe Storage Cycle Router#3 LTR Capability Pointer
  UINT32   RstPcieStorageEpLtrData[3];              ///< Offset 630     RST PCIe Storage Cycle Router#1 Endpoint LTR Data
                                                    ///< Offset 634     RST PCIe Storage Cycle Router#2 Endpoint LTR Data
                                                    ///< Offset 638     RST PCIe Storage Cycle Router#3 Endpoint LTR Data
  UINT16   RstPcieStorageEpLctlData16[3];           ///< Offset 642     RST PCIe Storage Cycle Router#1 Endpoint LCTL Data
                                                    ///< Offset 644     RST PCIe Storage Cycle Router#2 Endpoint LCTL Data
                                                    ///< Offset 646     RST PCIe Storage Cycle Router#3 Endpoint LCTL Data
  UINT16   RstPcieStorageEpDctlData16[3];           ///< Offset 648     RST PCIe Storage Cycle Router#1 Endpoint DCTL Data
                                                    ///< Offset 650     RST PCIe Storage Cycle Router#2 Endpoint DCTL Data
                                                    ///< Offset 652     RST PCIe Storage Cycle Router#3 Endpoint DCTL Data
  UINT16   RstPcieStorageEpDctl2Data16[3];          ///< Offset 654     RST PCIe Storage Cycle Router#1 Endpoint DCTL2 Data
                                                    ///< Offset 656     RST PCIe Storage Cycle Router#2 Endpoint DCTL2 Data
                                                    ///< Offset 658     RST PCIe Storage Cycle Router#3 Endpoint DCTL2 Data
  UINT16   RstPcieStorageRpDctl2Data16[3];          ///< Offset 660     RST PCIe Storage Cycle Router#1 RootPort DCTL2 Data
                                                    ///< Offset 662     RST PCIe Storage Cycle Router#2 RootPort DCTL2 Data
                                                    ///< Offset 664     RST PCIe Storage Cycle Router#3 RootPort DCTL2 Data
  UINT32   RstPcieStorageUniqueTableBar[3];         ///< Offset 666     RST PCIe Storage Cycle Router#1 Endpoint unique MSI-X Table BAR
                                                    ///< Offset 670     RST PCIe Storage Cycle Router#2 Endpoint unique MSI-X Table BAR
                                                    ///< Offset 674     RST PCIe Storage Cycle Router#3 Endpoint unique MSI-X Table BAR
  UINT32   RstPcieStorageUniqueTableBarValue[3];    ///< Offset 678     RST PCIe Storage Cycle Router#1 Endpoint unique MSI-X Table BAR value
                                                    ///< Offset 682     RST PCIe Storage Cycle Router#2 Endpoint unique MSI-X Table BAR value
                                                    ///< Offset 686     RST PCIe Storage Cycle Router#3 Endpoint unique MSI-X Table BAR value
  UINT32   RstPcieStorageUniquePbaBar[3];           ///< Offset 690     RST PCIe Storage Cycle Router#1 Endpoint unique MSI-X PBA BAR
                                                    ///< Offset 694     RST PCIe Storage Cycle Router#2 Endpoint unique MSI-X PBA BAR
                                                    ///< Offset 698     RST PCIe Storage Cycle Router#3 Endpoint unique MSI-X PBA BAR
  UINT32   RstPcieStorageUniquePbaBarValue[3];      ///< Offset 702     RST PCIe Storage Cycle Router#1 Endpoint unique MSI-X PBA BAR value
                                                    ///< Offset 706     RST PCIe Storage Cycle Router#2 Endpoint unique MSI-X PBA BAR value
                                                    ///< Offset 710     RST PCIe Storage Cycle Router#3 Endpoint unique MSI-X PBA BAR value
  UINT32   RstPcieStorageRootPortNum[3];            ///< Offset 714     RST PCIe Storage Cycle Router#1 Root Port number
                                                    ///< Offset 718     RST PCIe Storage Cycle Router#2 Root Port number
                                                    ///< Offset 722     RST PCIe Storage Cycle Router#3 Root Port number
  UINT8    EMH4;                                    ///< Offset 726     eMMC HS400 mode enabled
  UINT8    EMDS;                                    ///< Offset 727     eMMC Driver Strength
  UINT8    CpuSku;                                  ///< Offset 728     CPU SKU
  UINT16   IoTrapAddress[4];                        ///< Offset 729
  UINT8    IoTrapStatus[4];                         ///< Offset 737
  UINT16   PMBS;                                    ///< Offset 741     ACPI IO BASE address
  UINT32   PWRM;                                    ///< Offset 743     PWRM MEM BASE address
  // CNVi specific
  UINT8    CnviCrfPresent;                          ///< Offset 747     CNVi CRF Present
  UINT16   CnviPortId;                              ///< Offset 748     CNVi Sideband Port ID
  UINT8    CnviWifiCore;                            ///< Offset 750     CNVi WiFi Core
  UINT8    CnviBtCore;                              ///< Offset 751     CNVi BT Core
  UINT8    CnviBtInterface;                         ///< Offset 752     CNVi BT Interface
  UINT32   CnviBtEnableGpio;                        ///< Offset 753     CNVi BT Enable (CNV_BTEN) pin
  UINT8    CnviVPro;                                ///< Offset 757     CNVi vPro
  UINT8    CnviDdrRfim;                             ///< Offset 758     CNVi DDR RFI Mitigation
  // TraceHub
  UINT8    TraceVerbosity;                          ///< Offset 759     BIOS trace verbosity
  UINT32   TraceHubFwBase;                          ///< Offset 760     BIOS Trace Hub FW base address
  // PCH PS_ON support
  UINT8    PsOnEnable;                              ///< Offset 764     PCH PS_ON enable
  //
  // These are for PchApciTablesSelfTest use
  //
  UINT8    LtrEnable[28];                           ///< Offset 765     Latency Tolerance Reporting Enable
                                                    ///< Offset 766     Latency Tolerance Reporting Enable
                                                    ///< Offset 767     Latency Tolerance Reporting Enable
                                                    ///< Offset 768     Latency Tolerance Reporting Enable
                                                    ///< Offset 769     Latency Tolerance Reporting Enable
                                                    ///< Offset 770     Latency Tolerance Reporting Enable
                                                    ///< Offset 771     Latency Tolerance Reporting Enable
                                                    ///< Offset 772     Latency Tolerance Reporting Enable
                                                    ///< Offset 773     Latency Tolerance Reporting Enable
                                                    ///< Offset 774     Latency Tolerance Reporting Enable
                                                    ///< Offset 775     Latency Tolerance Reporting Enable
                                                    ///< Offset 776     Latency Tolerance Reporting Enable
                                                    ///< Offset 777     Latency Tolerance Reporting Enable
                                                    ///< Offset 778     Latency Tolerance Reporting Enable
                                                    ///< Offset 779     Latency Tolerance Reporting Enable
                                                    ///< Offset 780     Latency Tolerance Reporting Enable
                                                    ///< Offset 781     Latency Tolerance Reporting Enable
                                                    ///< Offset 782     Latency Tolerance Reporting Enable
                                                    ///< Offset 783     Latency Tolerance Reporting Enable
                                                    ///< Offset 784     Latency Tolerance Reporting Enable
                                                    ///< Offset 785     Latency Tolerance Reporting Enable
                                                    ///< Offset 786     Latency Tolerance Reporting Enable
                                                    ///< Offset 787     Latency Tolerance Reporting Enable
                                                    ///< Offset 788     Latency Tolerance Reporting Enable
                                                    ///< Offset 789     Latency Tolerance Reporting Enable
                                                    ///< Offset 790     Latency Tolerance Reporting Enable
                                                    ///< Offset 791     Latency Tolerance Reporting Enable
                                                    ///< Offset 792     Latency Tolerance Reporting Enable
  UINT8    GBES;                                    ///< Offset 793     GbE Support
  UINT32   PchxDCIPwrDnScale;                       ///< Offset 794     PCH xDCI Power Down Scale Value, DWC_USB3_GCTL_INIT[31:19]
  UINT8    EmmcEnabled;                             ///< Offset 798     Set to indicate that eMMC is enabled
  UINT8    SdCardEnabled;                           ///< Offset 799     Set to indicate that SD card is enabled
  UINT8    EnableTimedGpio0;                        ///< Offset 800     Set to indicate that Timed GPIO 0 is enabled
  UINT8    EnableTimedGpio1;                        ///< Offset 801     Set to indicate that Timed GPIO 1 is enabled
  UINT8    ClockToRootPortMap[18];                  ///< Offset 802     CLOCK index to root port index map. Used during PCIe D3Cold flows
  UINT16   TcoBase;                                 ///< Offset 820     TCO base address
  UINT16   IclkPid;                                 ///< Offset 822     Iclk PID number
  UINT8    PchUsb2PortCount;                        ///< Offset 824     Number of USB2 ports in PCH
  UINT8    PchUsb3PortCount;                        ///< Offset 825     Number of USB3 ports in PCH
  UINT8    SataPortPresence;                        ///< Offset 826     Holds information from SATA PCS register about SATA ports which recieved COMINIT from connected devices.
  UINT8    ClkreqIpcCmdSupported;                   ///< Offset 827     PCIE CLKREQ IPC command support
  UINT8    ClockReq[18];                            ///< Offset 828     CLOCK Source index to ClkReq Number. Used during PCIe D3Cold flows
  UINT32   ThcWakOnTouchInterrupt[2];               ///< Offset 846     Touch Host Controller Wake On Touch Interrupt Number - when 0 wake is disabled
  UINT16   ThcMode[2];                              ///< Offset 854     Touch Host Controller Mode THC or HID over SPI
  UINT8    Ufs0Enabled;                             ///< Offset 858     Is UFS0 Enabled
  UINT8    Ufs1Enabled;                             ///< Offset 859     Is UFS1 Enabled
  UINT8    UaolEnabled;                             ///< Offset 860     Is USB Audio Offload enabled
  UINT32   ThcResetPad[2];                          ///< Offset 861     Touch Host Controller Reset Pad
  UINT8    ThcResetPadTrigger[2];                   ///< Offset 869     Touch Host Controller Reset Pad Trigger
  UINT32   ThcHidSpiConnectionSpeed[2];             ///< Offset 871     Touch Host Controller HID over SPI Connection Speed
  UINT32   ThcHidSpiInputReportHeaderAddress[2];    ///< Offset 879     Touch Host Controller HID over SPI Input Report Header Address
  UINT32   ThcHidSpiInputReportBodyAddress[2];      ///< Offset 887     Touch Host Controller HID over SPI Input Report Body Address
  UINT32   ThcHidSpiOutputReportAddress[2];         ///< Offset 895     Touch Host Controller HID over SPI Output Report Address
  UINT32   ThcHidSpiReadOpcode[2];                  ///< Offset 903     Touch Host Controller HID over SPI Read Opcode
  UINT32   ThcHidSpiWriteOpcode[2];                 ///< Offset 911     Touch Host Controller HID over SPI Write Opcode
  UINT32   ThcHidSpiFlags[2];                       ///< Offset 919     Touch Host Controller HID over SPI Flags
  UINT32   ThcActiveLtr[2];                         ///< Offset 927     Touch Host Controller Active Ltr
  UINT32   ThcIdleLtr[2];                           ///< Offset 935     Touch Host Controller Idle Ltr
  UINT32   ThcHidSpiLimitPacketSize[2];             ///< Offset 943     Touch Host Controller Limit Packet Size
  UINT32   ThcPerformanceLimitation[2];             ///< Offset 951     Touch Host Controller Performance Limitation
  UINT32   ThcDisplayFrameSyncPeriod[2];            ///< Offset 959     Touch Host Controller Display Frame Sync Period
  UINT32   ThcResetSequencingDelay[2];              ///< Offset 967     Touch Host Controller Reset Sequencing Delay
  UINT16   ThcHidI2cDeviceAddress[2];               ///< Offset 975     Touch Host Controller HID over I2C Descriptor Address
  UINT32   ThcHidI2cConnectionSpeed[2];             ///< Offset 979     Touch Host Controller HID over I2C Connection Speed
  UINT8    ThcHidI2cAddressingMode[2];              ///< Offset 987     Touch Host Controller HID over I2C Addressing Mode
  UINT64   ThcHidI2cStandardModeSerialClockLineHighPeriod[2]; ///< Offset 989     Touch Host Controller HID over I2C Serial Clock Line High Period
  UINT64   ThcHidI2cStandardModeSerialClockLineLowPeriod[2]; ///< Offset 1005    Touch Host Controller HID over I2C Standard Mode Serial Clock Line Low Period
  UINT64   ThcHidI2cStandardModeSerialDataLineTransmitHoldPeriod[2]; ///< Offset 1021    Touch Host Controller HID over I2C Standard Mode Serial Data Line Transmit Hold Period
  UINT64   ThcHidI2cStandardModeSerialDataLineReceiveHoldPeriod[2]; ///< Offset 1037    Touch Host Controller HID over I2C Standard Mode Serial Data Receive Hold Period
  UINT64   ThcHidI2cFastModeSerialClockLineHighPeriod[2]; ///< Offset 1053    Touch Host Controller HID over I2C Fast Mode Serial Clock Line High Period
  UINT64   ThcHidI2cFastModeSerialClockLineLowPeriod[2]; ///< Offset 1069    Touch Host Controller HID over I2C Fast Mode Serial Clock Line Low Period
  UINT64   ThcHidI2cFastModeSerialDataLineTransmitHoldPeriod[2]; ///< Offset 1085    Touch Host Controller HID over I2C Fast Mode Serial Data Line Transmit Hold Period
  UINT64   ThcHidI2cFastModeSerialDataLineReceiveHoldPeriod[2]; ///< Offset 1101    Touch Host Controller HID over I2C Fast Mode Serial Data Line Receive Hold Period
  UINT64   ThcHidI2cMaxSuppressedSpikesSMFMFMP[2];  ///< Offset 1117    Touch Host Controller HID over I2C Maximum Length Of Suppressed Spikes In Std Mode Fast Mode And Fast Mode Plus
  UINT64   ThcHidI2cFastModePlusSerialClockLineHighPeriod[2]; ///< Offset 1133    Touch Host Controller HID over I2C Fast Mode Plus Serial Clock Line High Period
  UINT64   ThcHidI2cFastModePlusSerialClockLineLowPeriod[2]; ///< Offset 1149    Touch Host Controller HID over I2C Fast Mode Plus Serial Clock Line Low Period
  UINT64   ThcHidI2cFastModePlusSerialDataLineTransmitHoldPeriod[2]; ///< Offset 1165    Touch Host Controller HID over I2C Fast Mode Plus Serial Data Line Transmit Hold Period
  UINT64   ThcHidI2cFastModePlusSerialDataLineReceiveHoldPeriod[2]; ///< Offset 1181    Touch Host Controller HID over I2C Fast Mode Plus Serial Data Line Receive Hold Period
  UINT64   ThcHidI2cHighSpeedModePlusSerialClockLineHighPeriod[2]; ///< Offset 1197    Touch Host Controller HID over I2C High Speed Mode Plus Serial Clock Line High Period
  UINT64   ThcHidI2cHighSpeedModePlusSerialClockLineLowPeriod[2]; ///< Offset 1213    Touch Host Controller HID over I2C High Speed Mode Plus Serial Clock Line Low Period
  UINT64   ThcHidI2cHighSpeedModePlusSerialDataLineTransmitHoldPeriod[2]; ///< Offset 1229    Touch Host Controller HID over I2C High Speed Mode Plus Serial Data Line Transmit Hold Period
  UINT64   ThcHidI2cHighSpeedModePlusSerialDataLineReceiveHoldPeriod[2]; ///< Offset 1245    Touch Host Controller HID over I2C High Speed Mode Plus Serial Data Line Receive Hold Period
  UINT64   ThcHidI2cMaximumLengthOfSuppressedSpikesInHighSpeedMode[2]; ///< Offset 1261    Touch Host Controller HID over I2C Maximum Length Of Suppressed Spikes In High Speed Mode
  UINT8    CppmFaEn;                                ///< Offset 1277    CPPM Forced Alignment Enable
  UINT8    PchAttached;                             ///< Offset 1278    PCH Attached
  UINT8    CnviBtUartType;                          ///< Offset 1279    CNVi BT UART type
  UINT32   CnviBtHostWakeGpio;                      ///< Offset 1280    CNVi BT Host wake (CNV_BT_HOST_WAKEB) pin
} PCH_NVS_AREA;

#pragma pack(pop)
#endif
