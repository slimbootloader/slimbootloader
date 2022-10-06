/** @file

  Copyright (c) 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _GLOBAL_NVS_AREA_H_
#define _GLOBAL_NVS_AREA_H_

#define NVS_MAX_SOCKETS          8
#define NVS_MAX_LOGIC_IIO_STACKS 14

//
// Global NVS Area definition
//
#pragma pack (push,1)

//
// The definitions below need to be matched with GNVS definitions in GloblNvs.asl
//
typedef struct {
  // IOAPIC Start
  UINT32  PlatformId;
  UINT64  IoApicEnable;
  UINT8   ApicIdOverrided     :1;
  UINT8   PchIoApic_24_119    :1;
  UINT8   Cpx4Detect          :1;
  UINT8   Reserved0           :5;

  // Power Management Start
  UINT8   TpmEnable           :1;
  UINT8   CStateEnable        :1;
  UINT8   C3Enable            :1;
  UINT8   C6Enable            :1;
  UINT8   C7Enable            :1;
  UINT8   MonitorMwaitEnable  :1;
  UINT8   PStateEnable        :1;
  UINT8   EmcaEn              :1;
  UINT8   HWAllEnable         :2;
  UINT8   KBPresent           :1;
  UINT8   MousePresent        :1;
  UINT8   TStateEnable        :1;
  UINT8   TStateFineGrained   :1;
  UINT8   OSCX                :1;
  UINT8   Reserved1           :1;

  // RAS Start
  UINT8   CpuChangeMask;
  UINT8   IioChangeMask;
  UINT16  IioPresentBitMask[NVS_MAX_SOCKETS];
  UINT32  SocketBitMask;            // make sure this is at 4byte boundary
  UINT8   CpuCoreThreadsCount;
  UINT32  ProcessorApicIdBase[NVS_MAX_SOCKETS];
  UINT64  ProcessorBitMask[NVS_MAX_SOCKETS];   // cores 0-63 for each socket
  UINT64  ProcessorBitMaskHi[NVS_MAX_SOCKETS]; // cores 64-127 for each socket
  UINT32  MmCfg;
  UINT32  TsegSize;
  UINT32  SmiRequestParam[4];
  UINT32  SciRequestParam[4];
  UINT64  MigrationActionRegionAddress;
  UINT8   Cpu0Uuid[16];
  UINT8   Cpu1Uuid[16];
  UINT8   Cpu2Uuid[16];
  UINT8   Cpu3Uuid[16];
  UINT8   Cpu4Uuid[16];
  UINT8   Cpu5Uuid[16];
  UINT8   Cpu6Uuid[16];
  UINT8   Cpu7Uuid[16];
  UINT8   CpuSpareMask;
  UINT8   Mem0Uuid[16];
  UINT8   Mem1Uuid[16];
  UINT8   Mem2Uuid[16];
  UINT8   Mem3Uuid[16];
  UINT8   Mem4Uuid[16];
  UINT8   Mem5Uuid[16];
  UINT8   Mem6Uuid[16];
  UINT8   Mem7Uuid[16];
  UINT8   Mem8Uuid[16];
  UINT8   Mem9Uuid[16];
  UINT8   Mem10Uuid[16];
  UINT8   Mem11Uuid[16];
  UINT8   Mem12Uuid[16];
  UINT8   Mem13Uuid[16];
  UINT8   Mem14Uuid[16];
  UINT8   Mem15Uuid[16];
  UINT64  EmcaL1DirAddr;
  UINT32  ProcessorId;
  UINT8   PcieAcpiHotPlugEnable;
  UINT8   WheaEnabled;
  UINT8   WheaSci;
  UINT8   PropagateSerrOption;
  UINT8   PropagatePerrOption;

  // VTD Start
  UINT64  DrhdAddr[3];
  UINT64  AtsrAddr[3];
  UINT64  RhsaAddr[3];

  // SR-IOV WA Start
  UINT8   WmaaSICaseValue;
  UINT16  WmaaSISeg;
  UINT8   WmaaSIBus;
  UINT8   WmaaSIDevice;
  UINT8   WmaaSIFunction;
  UINT8   WmaaSISts;
  UINT8   WheaSupportEn;

  // BIOS Guard Start
  UINT64  BiosGuardMemAddress;
  UINT8   BiosGuardMemSize;
  UINT16  BiosGuardIoTrapAddress;
  UINT8   CpuSkuNumOfBitShift;

  // USB3 Start
  UINT8   XhciMode;
  UINT8   HostAlertVector1;
  UINT8   HostAlertVector2;

  // HWPM Start
  UINT8   HWPMEnable          :2; // HWPM
  UINT8   Reserved3           :1; // reserved bit
  UINT8   HwpInterrupt        :1; // HWP Interrupt
  UINT8   Reserved2           :4; // reserved bits

  // SGX Start
  UINT8  SgxStatus;
  UINT64 EpcLength[8];            // MAX_IMC * MAX_SOCKET
  UINT64 EpcBaseAddress[8];       // MAX_IMC * MAX_SOCKET

  // PCIe Multi-Seg Start
  UINT8   BusBase[NVS_MAX_SOCKETS][NVS_MAX_LOGIC_IIO_STACKS]; // PCI bus base number for each stack
  UINT8   PcieMultiSegSupport;    // Enable /Disable switch
  UINT8   PcieSegNum[NVS_MAX_SOCKETS]; // PCI segment number array for each socket

  UINT8   SncAnd2Cluster;         // 0 - SNC disabled, 2 - SNC enabled (2 clusters), 4 - SNC enabled (4 clusters)

  // XTU Start
  UINT32  XTUBaseAddress;         // 193 XTU Base Address
  UINT32  XTUSize;                // 197 XTU Entries Size
  UINT32  XMPBaseAddress;         // 201 XTU Base Address
  UINT8   DDRReferenceFreq;       // 205 DDR Reference Frequency
  UINT8   Rtd3Support;            // 206 Runtime D3 support.
  UINT8   Rtd3P0dl;               // 207 User selctable Delay for Device D0 transition.
  UINT8   Rtd3P3dl;               // 208 User selctable Delay for Device D0 transition.

  // FPGA Root Port Bus
  UINT8   FpgaBusBase[8];
  UINT8   FpgaBusLimit[8];

  // FPGA present bit
  UINT8   FpgaPresent[8];

  // FPGA Resource Allocation
  UINT32  VFPBMemBase[8];
  UINT32  VFPBMemLimit[8];

  // FPGA KTI present bitmap
  UINT32  FpgaKtiPresent;
  // FPGA Bus for KTI
  UINT8   FpgaKtiBase[8];

  UINT16  PmBase;                  // ACPI IO Base Address
  UINT8   DebugModeIndicator;      // Debug Mode Indicator
  UINT8   IioPcieRpCapOffset;      // IIO PCIe root port PCIe Capability offset
  UINT8   ArtTscLinkFlag;          // Flag to indicate if TSC is linked to ART

  // IIO Bus Limit (for Socket 0)
  UINT16  BusLimit[6];

  // IIO MMIO resources (32-bit & 64-bit) (Base & Length) (for Socket 0)
  UINT32  Mmio32Base[6];
  UINT32  Mmio32Length[6];
  UINT64  Mmio64Base[6];
  UINT64  Mmio64Length[6];

  // IIO IO resources (16-bit) (Base & Length) (for Socket 0)
  UINT16  Io16Base[6];
  UINT16  Io16Length[6];

  // Uncore Bus resources (Base & Limit)
  UINT8   UcBusBase[2];
  UINT8   UcBusLimit[2];
} BIOS_ACPI_PARAM;

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
  UINT8    SM0[3];                                  ///< Offset 254     SerialIo SPI Controller 0 Mode
                                                    ///< Offset 255     SerialIo SPI Controller 1 Mode
                                                    ///< Offset 256     SerialIo SPI Controller 2 Mode
  UINT64   SC0[3];                                  ///< Offset 257     SerialIo SPI Controller 0 Pci Config
                                                    ///< Offset 265     SerialIo SPI Controller 1 Pci Config
                                                    ///< Offset 273     SerialIo SPI Controller 2 Pci Config
  UINT8    IM0[6];                                  ///< Offset 281     SerialIo I2C Controller 0 Mode
                                                    ///< Offset 282     SerialIo I2C Controller 1 Mode
                                                    ///< Offset 283     SerialIo I2C Controller 2 Mode
                                                    ///< Offset 284     SerialIo I2C Controller 3 Mode
                                                    ///< Offset 285     SerialIo I2C Controller 4 Mode
                                                    ///< Offset 286     SerialIo I2C Controller 5 Mode
  UINT64   IC0[6];                                  ///< Offset 287     SerialIo I2C Controller 0 Pci Config
                                                    ///< Offset 295     SerialIo I2C Controller 1 Pci Config
                                                    ///< Offset 303     SerialIo I2C Controller 2 Pci Config
                                                    ///< Offset 311     SerialIo I2C Controller 3 Pci Config
                                                    ///< Offset 319     SerialIo I2C Controller 4 Pci Config
                                                    ///< Offset 327     SerialIo I2C Controller 5 Pci Config
  UINT8    UM0[3];                                  ///< Offset 335     SerialIo UART Controller 0 Mode
                                                    ///< Offset 336     SerialIo UART Controller 1 Mode
                                                    ///< Offset 337     SerialIo UART Controller 2 Mode
  UINT16   UP0[3];                                  ///< Offset 338     SerialIo UART Controller 0 IO Port
                                                    ///< Offset 340     SerialIo UART Controller 1 IO Port
                                                    ///< Offset 342     SerialIo UART Controller 2 IO Port
  UINT8    UI0[3];                                  ///< Offset 344     SerialIo UART Controller 0 Int
                                                    ///< Offset 345     SerialIo UART Controller 1 Int
                                                    ///< Offset 346     SerialIo UART Controller 2 Int
  //end of SerialIo block
  UINT8    SGIR;                                    ///< Offset 347     GPIO IRQ
  UINT8    GPHD;                                    ///< Offset 348     Hide GPIO ACPI device
  UINT8    RstPcieStorageInterfaceType[3];          ///< Offset 349     RST PCIe Storage Cycle Router#1 Interface Type
                                                    ///< Offset 350     RST PCIe Storage Cycle Router#2 Interface Type
                                                    ///< Offset 351     RST PCIe Storage Cycle Router#3 Interface Type
  UINT8    RstPcieStoragePmCapPtr[3];               ///< Offset 352     RST PCIe Storage Cycle Router#1 Power Management Capability Pointer
                                                    ///< Offset 353     RST PCIe Storage Cycle Router#2 Power Management Capability Pointer
                                                    ///< Offset 354     RST PCIe Storage Cycle Router#3 Power Management Capability Pointer
  UINT8    RstPcieStoragePcieCapPtr[3];             ///< Offset 355     RST PCIe Storage Cycle Router#1 PCIe Capabilities Pointer
                                                    ///< Offset 356     RST PCIe Storage Cycle Router#2 PCIe Capabilities Pointer
                                                    ///< Offset 357     RST PCIe Storage Cycle Router#3 PCIe Capabilities Pointer
  UINT16   RstPcieStorageL1ssCapPtr[3];             ///< Offset 358     RST PCIe Storage Cycle Router#1 L1SS Capability Pointer
                                                    ///< Offset 360     RST PCIe Storage Cycle Router#2 L1SS Capability Pointer
                                                    ///< Offset 362     RST PCIe Storage Cycle Router#3 L1SS Capability Pointer
  UINT8    RstPcieStorageEpL1ssControl2[3];         ///< Offset 364     RST PCIe Storage Cycle Router#1 Endpoint L1SS Control Data2
                                                    ///< Offset 365     RST PCIe Storage Cycle Router#2 Endpoint L1SS Control Data2
                                                    ///< Offset 366     RST PCIe Storage Cycle Router#3 Endpoint L1SS Control Data2
  UINT32   RstPcieStorageEpL1ssControl1[3];         ///< Offset 367     RST PCIe Storage Cycle Router#1 Endpoint L1SS Control Data1
                                                    ///< Offset 371     RST PCIe Storage Cycle Router#2 Endpoint L1SS Control Data1
                                                    ///< Offset 375     RST PCIe Storage Cycle Router#3 Endpoint L1SS Control Data1
  UINT16   RstPcieStorageLtrCapPtr[3];              ///< Offset 379     RST PCIe Storage Cycle Router#1 LTR Capability Pointer
                                                    ///< Offset 381     RST PCIe Storage Cycle Router#2 LTR Capability Pointer
                                                    ///< Offset 383     RST PCIe Storage Cycle Router#3 LTR Capability Pointer
  UINT32   RstPcieStorageEpLtrData[3];              ///< Offset 385     RST PCIe Storage Cycle Router#1 Endpoint LTR Data
                                                    ///< Offset 389     RST PCIe Storage Cycle Router#2 Endpoint LTR Data
                                                    ///< Offset 393     RST PCIe Storage Cycle Router#3 Endpoint LTR Data
  UINT16   RstPcieStorageEpLctlData16[3];           ///< Offset 397     RST PCIe Storage Cycle Router#1 Endpoint LCTL Data
                                                    ///< Offset 399     RST PCIe Storage Cycle Router#2 Endpoint LCTL Data
                                                    ///< Offset 401     RST PCIe Storage Cycle Router#3 Endpoint LCTL Data
  UINT16   RstPcieStorageEpDctlData16[3];           ///< Offset 403     RST PCIe Storage Cycle Router#1 Endpoint DCTL Data
                                                    ///< Offset 405     RST PCIe Storage Cycle Router#2 Endpoint DCTL Data
                                                    ///< Offset 407     RST PCIe Storage Cycle Router#3 Endpoint DCTL Data
  UINT16   RstPcieStorageEpDctl2Data16[3];          ///< Offset 409     RST PCIe Storage Cycle Router#1 Endpoint DCTL2 Data
                                                    ///< Offset 411     RST PCIe Storage Cycle Router#2 Endpoint DCTL2 Data
                                                    ///< Offset 413     RST PCIe Storage Cycle Router#3 Endpoint DCTL2 Data
  UINT16   RstPcieStorageRpDctl2Data16[3];          ///< Offset 415     RST PCIe Storage Cycle Router#1 RootPort DCTL2 Data
                                                    ///< Offset 417     RST PCIe Storage Cycle Router#2 RootPort DCTL2 Data
                                                    ///< Offset 419     RST PCIe Storage Cycle Router#3 RootPort DCTL2 Data
  UINT32   RstPcieStorageUniqueTableBar[3];         ///< Offset 421     RST PCIe Storage Cycle Router#1 Endpoint unique MSI-X Table BAR
                                                    ///< Offset 425     RST PCIe Storage Cycle Router#2 Endpoint unique MSI-X Table BAR
                                                    ///< Offset 429     RST PCIe Storage Cycle Router#3 Endpoint unique MSI-X Table BAR
  UINT32   RstPcieStorageUniqueTableBarValue[3];    ///< Offset 433     RST PCIe Storage Cycle Router#1 Endpoint unique MSI-X Table BAR value
                                                    ///< Offset 437     RST PCIe Storage Cycle Router#2 Endpoint unique MSI-X Table BAR value
                                                    ///< Offset 441     RST PCIe Storage Cycle Router#3 Endpoint unique MSI-X Table BAR value
  UINT32   RstPcieStorageUniquePbaBar[3];           ///< Offset 445     RST PCIe Storage Cycle Router#1 Endpoint unique MSI-X PBA BAR
                                                    ///< Offset 449     RST PCIe Storage Cycle Router#2 Endpoint unique MSI-X PBA BAR
                                                    ///< Offset 453     RST PCIe Storage Cycle Router#3 Endpoint unique MSI-X PBA BAR
  UINT32   RstPcieStorageUniquePbaBarValue[3];      ///< Offset 457     RST PCIe Storage Cycle Router#1 Endpoint unique MSI-X PBA BAR value
                                                    ///< Offset 461     RST PCIe Storage Cycle Router#2 Endpoint unique MSI-X PBA BAR value
                                                    ///< Offset 465     RST PCIe Storage Cycle Router#3 Endpoint unique MSI-X PBA BAR value
  UINT32   RstPcieStorageRootPortNum[3];            ///< Offset 469     RST PCIe Storage Cycle Router#1 Root Port number
                                                    ///< Offset 473     RST PCIe Storage Cycle Router#2 Root Port number
                                                    ///< Offset 477     RST PCIe Storage Cycle Router#3 Root Port number
  UINT8    EMH4;                                    ///< Offset 481     eMMC HS400 mode enabled
  UINT8    EMDS;                                    ///< Offset 482     eMMC Driver Strength
  UINT8    CpuSku;                                  ///< Offset 483     CPU SKU
  UINT16   IoTrapAddress[4];                        ///< Offset 484
  UINT8    IoTrapStatus[4];                         ///< Offset 492
  UINT16   PMBS;                                    ///< Offset 496     ACPI IO BASE address
  UINT32   PWRM;                                    ///< Offset 498     PWRM MEM BASE address
  // CNVi specific
  UINT8    CnviMode;                                ///< Offset 502     CNVi mode
  UINT8    CnviBtCore;                              ///< Offset 503     CNVi BT Core
  UINT8    CnviBtAudioOffload;                      ///< Offset 504     CNVi BT Audio Offload
  //Voltage Margining
  UINT8    DynamicS0i3Support;                      ///< Offset 505     Dynamic S0i3 Support
  // PCH Trace Hub
  UINT8    PchTraceHubMode;                         ///< Offset 506     PCH Trace Hub Mode
  // PCH PS_ON support
  UINT8    PsOnEnable;                              ///< Offset 507     PCH PS_ON enable
  //
  // These are for PchApciTablesSelfTest use
  //
  UINT8    LtrEnable[24];                           ///< Offset 508     Latency Tolerance Reporting Enable
                                                    ///< Offset 509     Latency Tolerance Reporting Enable
                                                    ///< Offset 510     Latency Tolerance Reporting Enable
                                                    ///< Offset 511     Latency Tolerance Reporting Enable
                                                    ///< Offset 512     Latency Tolerance Reporting Enable
                                                    ///< Offset 513     Latency Tolerance Reporting Enable
                                                    ///< Offset 514     Latency Tolerance Reporting Enable
                                                    ///< Offset 515     Latency Tolerance Reporting Enable
                                                    ///< Offset 516     Latency Tolerance Reporting Enable
                                                    ///< Offset 517     Latency Tolerance Reporting Enable
                                                    ///< Offset 518     Latency Tolerance Reporting Enable
                                                    ///< Offset 519     Latency Tolerance Reporting Enable
                                                    ///< Offset 520     Latency Tolerance Reporting Enable
                                                    ///< Offset 521     Latency Tolerance Reporting Enable
                                                    ///< Offset 522     Latency Tolerance Reporting Enable
                                                    ///< Offset 523     Latency Tolerance Reporting Enable
                                                    ///< Offset 524     Latency Tolerance Reporting Enable
                                                    ///< Offset 525     Latency Tolerance Reporting Enable
                                                    ///< Offset 526     Latency Tolerance Reporting Enable
                                                    ///< Offset 527     Latency Tolerance Reporting Enable
                                                    ///< Offset 528     Latency Tolerance Reporting Enable
                                                    ///< Offset 529     Latency Tolerance Reporting Enable
                                                    ///< Offset 530     Latency Tolerance Reporting Enable
                                                    ///< Offset 531     Latency Tolerance Reporting Enable
  UINT8    GBES;                                    ///< Offset 532     GbE Support
  UINT32   PchxDCIPwrDnScale;                       ///< Offset 533     PCH xDCI Power Down Scale Value, DWC_USB3_GCTL_INIT[31:19]
  UINT8    SdCardPowerEnableActiveHigh;             ///< Offset 537     SD_PWREN# active high indication
  UINT8    EmmcEnabled;                             ///< Offset 538     Set to indicate that eMMC is enabled
  UINT8    SdCardEnabled;                           ///< Offset 539     Set to indicate that SD card is enabled
  UINT8    PIR[8];                                  ///< Offset 540     PIRQA -> IRQx mapping
                                                    ///< Offset 541     PIRQB -> IRQx mapping
                                                    ///< Offset 542     PIRQC -> IRQx mapping
                                                    ///< Offset 543     PIRQD -> IRQx mapping
                                                    ///< Offset 544     PIRQE -> IRQx mapping
                                                    ///< Offset 545     PIRQF -> IRQx mapping
                                                    ///< Offset 546     PIRQG -> IRQx mapping
                                                    ///< Offset 547     PIRQH -> IRQx mapping
  UINT32   MmCfgAddress;                            ///< Offset 548     PCI Express BAR (PcieMmCfgBaseAddress)
} PCH_NVS_AREA;

typedef struct {
  UINT16   OperatingSystem;                         ///< Offset 0       Operating System
  UINT8    PciDelayOptimizationEcr;                 ///< Offset 2
  UINT8    Reserved0;                               ///< Offset 3
  UINT32   UartSerialDebugBase;                     ///< Offset 4
} PLATFORM_NVS_AREA;

//
// Global NVS Area Protocol
//
typedef struct {
  PLATFORM_NVS_AREA   PlatformNvs;
  PCH_NVS_AREA        PchNvs;
  BIOS_ACPI_PARAM     BiosAcpiParam;
} GLOBAL_NVS_AREA;

#pragma pack(pop)

#endif
