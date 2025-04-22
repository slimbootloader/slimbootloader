
/**@file


  Copyright (c) 2021, Intel Corporation. All rights reserved.<BR>
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
  UINT64   NHLA;                                    ///< Offset 118     HD-Audio NHLT ACPI address
  UINT32   NHLL;                                    ///< Offset 126     HD-Audio NHLT ACPI length
  UINT32   ADFM;                                    ///< Offset 130     HD-Audio DSP Feature Mask
  UINT8    SWQ0;                                    ///< Offset 134     HD-Audio SoundWire Link #1 quirk mask
  UINT8    SWQ1;                                    ///< Offset 135     HD-Audio SoundWire Link #2 quirk mask
  UINT8    SWQ2;                                    ///< Offset 136     HD-Audio SoundWire Link #3 quirk mask
  UINT8    SWQ3;                                    ///< Offset 137     HD-Audio SoundWire Link #4 quirk mask
  UINT8    ACS0;                                    ///< Offset 138     HD-Audio SoundWire Link #1 Autonomous Clock Stop
  UINT8    ACS1;                                    ///< Offset 139     HD-Audio SoundWire Link #2 Autonomous Clock Stop
  UINT8    ACS2;                                    ///< Offset 140     HD-Audio SoundWire Link #3 Autonomous Clock Stop
  UINT8    ACS3;                                    ///< Offset 141     HD-Audio SoundWire Link #4 Autonomous Clock Stop
  UINT8    DAI0;                                    ///< Offset 142     HD-Audio SoundWire Link #1 Data On Active Interval Select
  UINT8    DAI1;                                    ///< Offset 143     HD-Audio SoundWire Link #2 Data On Active Interval Select
  UINT8    DAI2;                                    ///< Offset 144     HD-Audio SoundWire Link #3 Data On Active Interval Select
  UINT8    DAI3;                                    ///< Offset 145     HD-Audio SoundWire Link #4 Data On Active Interval Select
  UINT8    DOD0;                                    ///< Offset 146     HD-Audio SoundWire Link #1 Data On Delay Select
  UINT8    DOD1;                                    ///< Offset 147     HD-Audio SoundWire Link #2 Data On Delay Select
  UINT8    DOD2;                                    ///< Offset 148     HD-Audio SoundWire Link #3 Data On Delay Select
  UINT8    DOD3;                                    ///< Offset 149     HD-Audio SoundWire Link #4 Data On Delay Select
  UINT8    SWMC;                                    ///< Offset 150     HD-Audio SoundWire Host Count
  UINT32   XTAL;                                    ///< Offset 151     XTAL frequency: 0: 24MHz; 1: 38.4MHz; 2: Unsupported
  UINT8    AFDF;                                    ///< Offset 155     Audio FPGA Number of Device and Function. 0: Audio FPGA not detected
  UINT32   SBRG;                                    ///< Offset 156     SBREG_BAR
  UINT8    GEI0;                                    ///< Offset 160     GPIO GroupIndex mapped to GPE_DW0
  UINT8    GEI1;                                    ///< Offset 161     GPIO GroupIndex mapped to GPE_DW1
  UINT8    GEI2;                                    ///< Offset 162     GPIO GroupIndex mapped to GPE_DW2
  UINT8    GED0;                                    ///< Offset 163     GPIO DW part of group mapped to GPE_DW0
  UINT8    GED1;                                    ///< Offset 164     GPIO DW part of group mapped to GPE_DW1
  UINT8    GED2;                                    ///< Offset 165     GPIO DW part of group mapped to GPE_DW2
  UINT16   PcieLtrMaxSnoopLatency[28];              ///< Offset 166     PCIE LTR max snoop Latency 1
                                                    ///< Offset 168     PCIE LTR max snoop Latency 2
                                                    ///< Offset 170     PCIE LTR max snoop Latency 3
                                                    ///< Offset 172     PCIE LTR max snoop Latency 4
                                                    ///< Offset 174     PCIE LTR max snoop Latency 5
                                                    ///< Offset 176     PCIE LTR max snoop Latency 6
                                                    ///< Offset 178     PCIE LTR max snoop Latency 7
                                                    ///< Offset 180     PCIE LTR max snoop Latency 8
                                                    ///< Offset 182     PCIE LTR max snoop Latency 9
                                                    ///< Offset 184     PCIE LTR max snoop Latency 10
                                                    ///< Offset 186     PCIE LTR max snoop Latency 11
                                                    ///< Offset 188     PCIE LTR max snoop Latency 12
                                                    ///< Offset 190     PCIE LTR max snoop Latency 13
                                                    ///< Offset 192     PCIE LTR max snoop Latency 14
                                                    ///< Offset 194     PCIE LTR max snoop Latency 15
                                                    ///< Offset 196     PCIE LTR max snoop Latency 16
                                                    ///< Offset 198     PCIE LTR max snoop Latency 17
                                                    ///< Offset 200     PCIE LTR max snoop Latency 18
                                                    ///< Offset 202     PCIE LTR max snoop Latency 19
                                                    ///< Offset 204     PCIE LTR max snoop Latency 20
                                                    ///< Offset 206     PCIE LTR max snoop Latency 21
                                                    ///< Offset 208     PCIE LTR max snoop Latency 22
                                                    ///< Offset 210     PCIE LTR max snoop Latency 23
                                                    ///< Offset 212     PCIE LTR max snoop Latency 24
                                                    ///< Offset 214     PCIE LTR max snoop Latency 25
                                                    ///< Offset 216     PCIE LTR max snoop Latency 26
                                                    ///< Offset 218     PCIE LTR max snoop Latency 27
                                                    ///< Offset 220     PCIE LTR max snoop Latency 28
  UINT16   PcieLtrMaxNoSnoopLatency[28];            ///< Offset 222     PCIE LTR max no snoop Latency 1
                                                    ///< Offset 224     PCIE LTR max no snoop Latency 2
                                                    ///< Offset 226     PCIE LTR max no snoop Latency 3
                                                    ///< Offset 228     PCIE LTR max no snoop Latency 4
                                                    ///< Offset 230     PCIE LTR max no snoop Latency 5
                                                    ///< Offset 232     PCIE LTR max no snoop Latency 6
                                                    ///< Offset 234     PCIE LTR max no snoop Latency 7
                                                    ///< Offset 236     PCIE LTR max no snoop Latency 8
                                                    ///< Offset 238     PCIE LTR max no snoop Latency 9
                                                    ///< Offset 240     PCIE LTR max no snoop Latency 10
                                                    ///< Offset 242     PCIE LTR max no snoop Latency 11
                                                    ///< Offset 244     PCIE LTR max no snoop Latency 12
                                                    ///< Offset 246     PCIE LTR max no snoop Latency 13
                                                    ///< Offset 248     PCIE LTR max no snoop Latency 14
                                                    ///< Offset 250     PCIE LTR max no snoop Latency 15
                                                    ///< Offset 252     PCIE LTR max no snoop Latency 16
                                                    ///< Offset 254     PCIE LTR max no snoop Latency 17
                                                    ///< Offset 256     PCIE LTR max no snoop Latency 18
                                                    ///< Offset 258     PCIE LTR max no snoop Latency 19
                                                    ///< Offset 260     PCIE LTR max no snoop Latency 20
                                                    ///< Offset 262     PCIE LTR max no snoop Latency 21
                                                    ///< Offset 264     PCIE LTR max no snoop Latency 22
                                                    ///< Offset 266     PCIE LTR max no snoop Latency 23
                                                    ///< Offset 268     PCIE LTR max no snoop Latency 24
                                                    ///< Offset 270     PCIE LTR max no snoop Latency 25
                                                    ///< Offset 272     PCIE LTR max no snoop Latency 26
                                                    ///< Offset 274     PCIE LTR max no snoop Latency 27
                                                    ///< Offset 276     PCIE LTR max no snoop Latency 28
  UINT8    XHPC;                                    ///< Offset 278     Number of HighSpeed ports implemented in XHCI controller
  UINT8    XRPC;                                    ///< Offset 279     Number of USBR ports implemented in XHCI controller
  UINT8    XSPC;                                    ///< Offset 280     Number of SuperSpeed ports implemented in XHCI controller
  UINT8    XSPA;                                    ///< Offset 281     Address of 1st SuperSpeed port
  UINT32   HPTB;                                    ///< Offset 282     HPET base address
  UINT8    HPTE;                                    ///< Offset 286     HPET enable
  //SerialIo block
  UINT8    SM0[7];                                  ///< Offset 287     SerialIo SPI Controller 0 Mode
                                                    ///< Offset 288     SerialIo SPI Controller 1 Mode
                                                    ///< Offset 289     SerialIo SPI Controller 2 Mode
                                                    ///< Offset 290     SerialIo SPI Controller 3 Mode
                                                    ///< Offset 291     SerialIo SPI Controller 4 Mode
                                                    ///< Offset 292     SerialIo SPI Controller 5 Mode
                                                    ///< Offset 293     SerialIo SPI Controller 6 Mode
  UINT64   SC0[7];                                  ///< Offset 294     SerialIo SPI Controller 0 Pci Config
                                                    ///< Offset 302     SerialIo SPI Controller 1 Pci Config
                                                    ///< Offset 310     SerialIo SPI Controller 2 Pci Config
                                                    ///< Offset 318     SerialIo SPI Controller 3 Pci Config
                                                    ///< Offset 326     SerialIo SPI Controller 4 Pci Config
                                                    ///< Offset 334     SerialIo SPI Controller 5 Pci Config
                                                    ///< Offset 342     SerialIo SPI Controller 6 Pci Config
  UINT8    IM0[8];                                  ///< Offset 350     SerialIo I2C Controller 0 Mode
                                                    ///< Offset 351     SerialIo I2C Controller 1 Mode
                                                    ///< Offset 352     SerialIo I2C Controller 2 Mode
                                                    ///< Offset 353     SerialIo I2C Controller 3 Mode
                                                    ///< Offset 354     SerialIo I2C Controller 4 Mode
                                                    ///< Offset 355     SerialIo I2C Controller 5 Mode
                                                    ///< Offset 356     SerialIo I2C Controller 6 Mode
                                                    ///< Offset 357     SerialIo I2C Controller 7 Mode
  UINT64   IC0[8];                                  ///< Offset 358     SerialIo I2C Controller 0 Pci Config
                                                    ///< Offset 366     SerialIo I2C Controller 1 Pci Config
                                                    ///< Offset 374     SerialIo I2C Controller 2 Pci Config
                                                    ///< Offset 382     SerialIo I2C Controller 3 Pci Config
                                                    ///< Offset 390     SerialIo I2C Controller 4 Pci Config
                                                    ///< Offset 398     SerialIo I2C Controller 5 Pci Config
                                                    ///< Offset 406     SerialIo I2C Controller 6 Pci Config
                                                    ///< Offset 414     SerialIo I2C Controller 7 Pci Config
  UINT8    UM0[7];                                  ///< Offset 422     SerialIo UART Controller 0 Mode
                                                    ///< Offset 423     SerialIo UART Controller 1 Mode
                                                    ///< Offset 424     SerialIo UART Controller 2 Mode
                                                    ///< Offset 425     SerialIo UART Controller 3 Mode
                                                    ///< Offset 426     SerialIo UART Controller 4 Mode
                                                    ///< Offset 427     SerialIo UART Controller 5 Mode
                                                    ///< Offset 428     SerialIo UART Controller 6 Mode
  UINT64   UC0[7];                                  ///< Offset 429     SerialIo UART Controller 0 Pci Config
                                                    ///< Offset 437     SerialIo UART Controller 1 Pci Config
                                                    ///< Offset 445     SerialIo UART Controller 2 Pci Config
                                                    ///< Offset 453     SerialIo UART Controller 3 Pci Config
                                                    ///< Offset 461     SerialIo UART Controller 4 Pci Config
                                                    ///< Offset 469     SerialIo UART Controller 5 Pci Config
                                                    ///< Offset 477     SerialIo UART Controller 6 Pci Config
  UINT8    UD0[7];                                  ///< Offset 485     SerialIo UART Controller 0 DmaEnable
                                                    ///< Offset 486     SerialIo UART Controller 1 DmaEnable
                                                    ///< Offset 487     SerialIo UART Controller 2 DmaEnable
                                                    ///< Offset 488     SerialIo UART Controller 3 DmaEnable
                                                    ///< Offset 489     SerialIo UART Controller 4 DmaEnable
                                                    ///< Offset 490     SerialIo UART Controller 5 DmaEnable
                                                    ///< Offset 491     SerialIo UART Controller 6 DmaEnable
  UINT8    UP0[7];                                  ///< Offset 492     SerialIo UART Controller 0 Power Gating
                                                    ///< Offset 493     SerialIo UART Controller 1 Power Gating
                                                    ///< Offset 494     SerialIo UART Controller 2 Power Gating
                                                    ///< Offset 495     SerialIo UART Controller 3 Power Gating
                                                    ///< Offset 496     SerialIo UART Controller 4 Power Gating
                                                    ///< Offset 497     SerialIo UART Controller 5 Power Gating
                                                    ///< Offset 498     SerialIo UART Controller 6 Power Gating
  UINT8    UI0[7];                                  ///< Offset 499     SerialIo UART Controller 0 Irq
                                                    ///< Offset 500     SerialIo UART Controller 1 Irq
                                                    ///< Offset 501     SerialIo UART Controller 2 Irq
                                                    ///< Offset 502     SerialIo UART Controller 3 Irq
                                                    ///< Offset 503     SerialIo UART Controller 4 Irq
                                                    ///< Offset 504     SerialIo UART Controller 5 Irq
                                                    ///< Offset 505     SerialIo UART Controller 6 Irq
  //end of SerialIo block
  UINT8    SGIR;                                    ///< Offset 506     GPIO IRQ
  UINT8    GPHD;                                    ///< Offset 507     Hide GPIO ACPI device
  UINT8    RstPcieStorageInterfaceType[3];          ///< Offset 508     RST PCIe Storage Cycle Router#1 Interface Type
                                                    ///< Offset 509     RST PCIe Storage Cycle Router#2 Interface Type
                                                    ///< Offset 510     RST PCIe Storage Cycle Router#3 Interface Type
  UINT8    RstPcieStoragePmCapPtr[3];               ///< Offset 511     RST PCIe Storage Cycle Router#1 Power Management Capability Pointer
                                                    ///< Offset 512     RST PCIe Storage Cycle Router#2 Power Management Capability Pointer
                                                    ///< Offset 513     RST PCIe Storage Cycle Router#3 Power Management Capability Pointer
  UINT8    RstPcieStoragePcieCapPtr[3];             ///< Offset 514     RST PCIe Storage Cycle Router#1 PCIe Capabilities Pointer
                                                    ///< Offset 515     RST PCIe Storage Cycle Router#2 PCIe Capabilities Pointer
                                                    ///< Offset 516     RST PCIe Storage Cycle Router#3 PCIe Capabilities Pointer
  UINT16   RstPcieStorageL1ssCapPtr[3];             ///< Offset 517     RST PCIe Storage Cycle Router#1 L1SS Capability Pointer
                                                    ///< Offset 519     RST PCIe Storage Cycle Router#2 L1SS Capability Pointer
                                                    ///< Offset 521     RST PCIe Storage Cycle Router#3 L1SS Capability Pointer
  UINT8    RstPcieStorageEpL1ssControl2[3];         ///< Offset 523     RST PCIe Storage Cycle Router#1 Endpoint L1SS Control Data2
                                                    ///< Offset 524     RST PCIe Storage Cycle Router#2 Endpoint L1SS Control Data2
                                                    ///< Offset 525     RST PCIe Storage Cycle Router#3 Endpoint L1SS Control Data2
  UINT32   RstPcieStorageEpL1ssControl1[3];         ///< Offset 526     RST PCIe Storage Cycle Router#1 Endpoint L1SS Control Data1
                                                    ///< Offset 530     RST PCIe Storage Cycle Router#2 Endpoint L1SS Control Data1
                                                    ///< Offset 534     RST PCIe Storage Cycle Router#3 Endpoint L1SS Control Data1
  UINT16   RstPcieStorageLtrCapPtr[3];              ///< Offset 538     RST PCIe Storage Cycle Router#1 LTR Capability Pointer
                                                    ///< Offset 540     RST PCIe Storage Cycle Router#2 LTR Capability Pointer
                                                    ///< Offset 542     RST PCIe Storage Cycle Router#3 LTR Capability Pointer
  UINT32   RstPcieStorageEpLtrData[3];              ///< Offset 544     RST PCIe Storage Cycle Router#1 Endpoint LTR Data
                                                    ///< Offset 548     RST PCIe Storage Cycle Router#2 Endpoint LTR Data
                                                    ///< Offset 552     RST PCIe Storage Cycle Router#3 Endpoint LTR Data
  UINT16   RstPcieStorageEpLctlData16[3];           ///< Offset 556     RST PCIe Storage Cycle Router#1 Endpoint LCTL Data
                                                    ///< Offset 558     RST PCIe Storage Cycle Router#2 Endpoint LCTL Data
                                                    ///< Offset 560     RST PCIe Storage Cycle Router#3 Endpoint LCTL Data
  UINT16   RstPcieStorageEpDctlData16[3];           ///< Offset 562     RST PCIe Storage Cycle Router#1 Endpoint DCTL Data
                                                    ///< Offset 564     RST PCIe Storage Cycle Router#2 Endpoint DCTL Data
                                                    ///< Offset 566     RST PCIe Storage Cycle Router#3 Endpoint DCTL Data
  UINT16   RstPcieStorageEpDctl2Data16[3];          ///< Offset 568     RST PCIe Storage Cycle Router#1 Endpoint DCTL2 Data
                                                    ///< Offset 570     RST PCIe Storage Cycle Router#2 Endpoint DCTL2 Data
                                                    ///< Offset 572     RST PCIe Storage Cycle Router#3 Endpoint DCTL2 Data
  UINT16   RstPcieStorageRpDctl2Data16[3];          ///< Offset 574     RST PCIe Storage Cycle Router#1 RootPort DCTL2 Data
                                                    ///< Offset 576     RST PCIe Storage Cycle Router#2 RootPort DCTL2 Data
                                                    ///< Offset 578     RST PCIe Storage Cycle Router#3 RootPort DCTL2 Data
  UINT32   RstPcieStorageUniqueTableBar[3];         ///< Offset 580     RST PCIe Storage Cycle Router#1 Endpoint unique MSI-X Table BAR
                                                    ///< Offset 584     RST PCIe Storage Cycle Router#2 Endpoint unique MSI-X Table BAR
                                                    ///< Offset 588     RST PCIe Storage Cycle Router#3 Endpoint unique MSI-X Table BAR
  UINT32   RstPcieStorageUniqueTableBarValue[3];    ///< Offset 592     RST PCIe Storage Cycle Router#1 Endpoint unique MSI-X Table BAR value
                                                    ///< Offset 596     RST PCIe Storage Cycle Router#2 Endpoint unique MSI-X Table BAR value
                                                    ///< Offset 600     RST PCIe Storage Cycle Router#3 Endpoint unique MSI-X Table BAR value
  UINT32   RstPcieStorageUniquePbaBar[3];           ///< Offset 604     RST PCIe Storage Cycle Router#1 Endpoint unique MSI-X PBA BAR
                                                    ///< Offset 608     RST PCIe Storage Cycle Router#2 Endpoint unique MSI-X PBA BAR
                                                    ///< Offset 612     RST PCIe Storage Cycle Router#3 Endpoint unique MSI-X PBA BAR
  UINT32   RstPcieStorageUniquePbaBarValue[3];      ///< Offset 616     RST PCIe Storage Cycle Router#1 Endpoint unique MSI-X PBA BAR value
                                                    ///< Offset 620     RST PCIe Storage Cycle Router#2 Endpoint unique MSI-X PBA BAR value
                                                    ///< Offset 624     RST PCIe Storage Cycle Router#3 Endpoint unique MSI-X PBA BAR value
  UINT32   RstPcieStorageRootPortNum[3];            ///< Offset 628     RST PCIe Storage Cycle Router#1 Root Port number
                                                    ///< Offset 632     RST PCIe Storage Cycle Router#2 Root Port number
                                                    ///< Offset 636     RST PCIe Storage Cycle Router#3 Root Port number
  UINT8    EMH4;                                    ///< Offset 640     eMMC HS400 mode enabled
  UINT8    EMDS;                                    ///< Offset 641     eMMC Driver Strength
  UINT8    CpuSku;                                  ///< Offset 642     CPU SKU
  UINT16   IoTrapAddress[4];                        ///< Offset 643
  UINT8    IoTrapStatus[4];                         ///< Offset 651
  UINT16   PMBS;                                    ///< Offset 655     ACPI IO BASE address
  UINT32   PWRM;                                    ///< Offset 657     PWRM MEM BASE address
  // CNVi specific
  UINT8    CnviMode;                                ///< Offset 661     CNVi mode
  UINT8    CnviBtCore;                              ///< Offset 662     CNVi BT Core
  UINT8    CnviBtAudioOffload;                      ///< Offset 663     CNVi BT Audio Offload
  UINT8    CnviVPro;                                ///< Offset 664     CNVi vPro
  UINT8    CnviDdrRfim;                             ///< Offset 665     CNVi DDR RFI Mitigation
  UINT8    CnviCrfPresent;                          ///< Offset 666     CNVi CRF present
  // PCH Trace Hub
  UINT8    PchTraceHubMode;                         ///< Offset 667     PCH Trace Hub Mode
  // PCH PS_ON support
  UINT8    PsOnEnable;                              ///< Offset 668     PCH PS_ON enable
  //
  // These are for PchApciTablesSelfTest use
  //
  UINT8    LtrEnable[28];                           ///< Offset 669     Latency Tolerance Reporting Enable
                                                    ///< Offset 670     Latency Tolerance Reporting Enable
                                                    ///< Offset 671     Latency Tolerance Reporting Enable
                                                    ///< Offset 672     Latency Tolerance Reporting Enable
                                                    ///< Offset 673     Latency Tolerance Reporting Enable
                                                    ///< Offset 674     Latency Tolerance Reporting Enable
                                                    ///< Offset 675     Latency Tolerance Reporting Enable
                                                    ///< Offset 676     Latency Tolerance Reporting Enable
                                                    ///< Offset 677     Latency Tolerance Reporting Enable
                                                    ///< Offset 678     Latency Tolerance Reporting Enable
                                                    ///< Offset 679     Latency Tolerance Reporting Enable
                                                    ///< Offset 680     Latency Tolerance Reporting Enable
                                                    ///< Offset 681     Latency Tolerance Reporting Enable
                                                    ///< Offset 682     Latency Tolerance Reporting Enable
                                                    ///< Offset 683     Latency Tolerance Reporting Enable
                                                    ///< Offset 684     Latency Tolerance Reporting Enable
                                                    ///< Offset 685     Latency Tolerance Reporting Enable
                                                    ///< Offset 686     Latency Tolerance Reporting Enable
                                                    ///< Offset 687     Latency Tolerance Reporting Enable
                                                    ///< Offset 688     Latency Tolerance Reporting Enable
                                                    ///< Offset 689     Latency Tolerance Reporting Enable
                                                    ///< Offset 690     Latency Tolerance Reporting Enable
                                                    ///< Offset 691     Latency Tolerance Reporting Enable
                                                    ///< Offset 692     Latency Tolerance Reporting Enable
                                                    ///< Offset 693     Latency Tolerance Reporting Enable
                                                    ///< Offset 694     Latency Tolerance Reporting Enable
                                                    ///< Offset 695     Latency Tolerance Reporting Enable
                                                    ///< Offset 696     Latency Tolerance Reporting Enable
  UINT8    GBES;                                    ///< Offset 697     GbE Support
  UINT32   PchxDCIPwrDnScale;                       ///< Offset 698     PCH xDCI Power Down Scale Value, DWC_USB3_GCTL_INIT[31:19]
  UINT8    EmmcEnabled;                             ///< Offset 702     Set to indicate that eMMC is enabled
  UINT8    SdCardEnabled;                           ///< Offset 703     Set to indicate that SD card is enabled
  UINT8    EnableTimedGpio0;                        ///< Offset 704     Set to indicate that Timed GPIO 0 is enabled
  UINT8    EnableTimedGpio1;                        ///< Offset 705     Set to indicate that Timed GPIO 1 is enabled
  UINT8    ClockToRootPortMap[18];                  ///< Offset 706     CLOCK index to root port index map. Used during PCIe D3Cold flows
  UINT16   TcoBase;                                 ///< Offset 724     TCO base address
  UINT16   IclkPid;                                 ///< Offset 726     Iclk PID number
  UINT16   CnviPortId;                              ///< Offset 728     CNVi sideband port id
  UINT32   HybridStorageLocation;                   ///< Offset 730
  UINT8    PchUsb2PortCount;                        ///< Offset 734     Number of USB2 ports in PCH
  UINT8    PchUsb3PortCount;                        ///< Offset 735     Number of USB3 ports in PCH
  UINT8    SataPortPresence;                        ///< Offset 736     Holds information from SATA PCS register about SATA ports which recieved COMINIT from connected devices.
  UINT8    ClkreqIpcCmdSupported;                   ///< Offset 737     PCIE CLKREQ IPC command support
  UINT8    ClockReq[18];                            ///< Offset 738     CLOCK Source index to ClkReq Number. Used during PCIe D3Cold flows
  UINT32   ThcWakOnTouchInterrupt[2];               ///< Offset 756     Touch Host Controller Wake On Touch Interrupt Number - when 0 wake is disabled
  UINT16   ThcMode[2];                              ///< Offset 764     Touch Host Controller Mode THC or HID over SPI
  UINT8    Ufs0Enabled;                             ///< Offset 768     Is UFS0 Enabled
  UINT8    Ufs1Enabled;                             ///< Offset 769     Is UFS1 Enabled
  UINT8    UaolEnabled;                             ///< Offset 770     Is USB Audio Offload enabled
  UINT32   ThcHidResetPad[2];                       ///< Offset 771     Touch Host Controller HID over SPI Reset Pad
  UINT8    ThcHidResetPadTrigger[2];                ///< Offset 779     Touch Host Controller HID over SPI Reset Pad Trigger
  UINT32   ThcHidFrequency[2];                      ///< Offset 781     Touch Host Controller HID over SPI Frequency
  UINT32   ThcHidInputReportHeaderAddress[2];       ///< Offset 789     Touch Host Controller HID over SPI Input Report Header Address
  UINT32   ThcHidInputReportBodyAddress[2];         ///< Offset 797     Touch Host Controller HID over SPI Input Report Body Address
  UINT32   ThcHidOutputReportAddress[2];            ///< Offset 805     Touch Host Controller HID over SPI Output Report Address
  UINT32   ThcHidReadOpcode[2];                     ///< Offset 813     Touch Host Controller HID over SPI Read Opcode
  UINT32   ThcHidWriteOpcode[2];                    ///< Offset 821     Touch Host Controller HID over SPI Write Opcode
  UINT32   ThcHidFlags[2];                          ///< Offset 829     Touch Host Controller HID over SPI Flags
  UINT32   ThcActiveLtr[2];                         ///< Offset 837     Touch Host Controller Active Ltr
  UINT32   ThcIdleLtr[2];                           ///< Offset 845     Touch Host Controller Idle Ltr
  UINT32   HybridStorageCpuRpLocation;              ///< Offset 853     Cpu Root port used for hybrid storage
  UINT32   HybridStorageCpuAttachPchRpLocation;     ///< Offset 857     Pch Root port used for hybrid storage
  UINT32   AcpiL6dPmeHandling;                      ///< Offset 861     _L6D Enable, BIOS-ACPI can verify PMEENABLE and PMESTATUS of each device that requires GPE related wake
} PCH_NVS_AREA;

#pragma pack(pop)
#endif
