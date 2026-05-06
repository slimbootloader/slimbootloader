/** @file

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _EFI_GLOBAL_NVS_AREA_H_
#define _EFI_GLOBAL_NVS_AREA_H_

// Because of ASL constrains cannot use MAX_SOCKET to configure ACPI objects. The symbol
// below is the largest values of MAX_SOCKET supported in BiosParameterRegion.asi.
//
#define NVS_MAX_SOCKETS               8
//
// Because of ASL constrains cannot use MAX_IIO_STACKS_PER_SOCKET to configure ACPI objects. The symbol
// below is the largest possible value of MAX_IIO_STACKS_PER_SOCKET supported in BiosParameterRegion.asi.
//
#define NVS_MAX_IIO_STACKS_PER_SOCKET 16
#define NVS_MAX_IIO_PCIE_PER_SOCKET   10
#define NVS_MAX_IIO_IOAT_PER_SOCKET   8

#if NVS_MAX_SOCKETS < MAX_SOCKET
#error "Must update NVS_MAX_SOCKETS and BiosParameterRegion.asi to handle so many sockets"
#endif
#if NVS_MAX_IIO_STACKS_PER_SOCKET < MAX_IIO_STACKS_PER_SOCKET
#error "Must update NVS_MAX_IIO_STACKS_PER_SOCKET and BiosParameterRegion.asi to handle so many stacks"
#endif
#if NVS_MAX_IIO_PCIE_PER_SOCKET < MAX_IIO_PCIE_PER_SOCKET
#error "Must update NVS_MAX_IIO_PCIE_PER_SOCKET and BiosParameterRegion.asi to handle so many PCIEs"
#endif
#if NVS_MAX_IIO_IOAT_PER_SOCKET < MAX_IIO_IOAT_PER_SOCKET
#error "Must update NVS_MAX_IIO_IOAT_PER_SOCKET and BiosParameterRegion.asi to handle so many IOATs"
#endif
#if MAX_IIO_STACKS_PER_SOCKET > 16
#error "Must use larger type for IioStackPresentBitmap[] and IioCxlxxxPresentBitmap[] elements"
#endif
#if MAX_IIO_PCIROOTS_PER_STACK > 8
#error "Must use larger type for IioStackPresentBitmap[] and IioCxlxxxPresentBitmap[] elements"
#endif

//
// Global NVS Area definition
// BIOS parameters region provided by POST code to ASL, defined as PSYS in BiosParametersRegion.asi
//
#pragma pack (push, 1)

typedef struct {
  // IOAPIC Start
  UINT32  PlatformId;
  UINT64  IoApicEnable;
  UINT8   ApicIdOverrided     :1;
  UINT8   PchIoApic_24_119    :1;
  UINT8   Reserved0           :6;

  // Power Management Start
  UINT8   TpmEnable           :1;
  UINT8   CStateEnable        :1;
  UINT8   C6Enable            :3;
  UINT8   MonitorMwaitEnable  :1;
  UINT8   IsAtom              :1;
  UINT8   C1eEnable           :1;
  UINT8   PStateEnable        :1;
  UINT8   EmcaEn              :1;
  UINT8   HWAllEnable         :2;
  UINT8   KBPresent           :1;
  UINT8   MousePresent        :1;
  UINT8   Reserved1           :2;

  // RAS Start
  UINT8   CpuChangeMask;
  UINT8   IioChangeMask;
  UINT8   IioPcieRootBitmap[NVS_MAX_SOCKETS][NVS_MAX_IIO_PCIE_PER_SOCKET];
  UINT8   IioIoatRootBitmap[NVS_MAX_SOCKETS][NVS_MAX_IIO_IOAT_PER_SOCKET];
  UINT8   IioIoatSegment[NVS_MAX_SOCKETS][NVS_MAX_IIO_IOAT_PER_SOCKET];
  UINT16  IioCxlCapableBitmap[NVS_MAX_SOCKETS];
  UINT8   UncorePresentBitmap[NVS_MAX_SOCKETS];
  UINT32  SocketBitMask;                       // make sure this is at 4byte boundary
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
  UINT8   WheaSupportEn;
  UINT8   WheaEnabled;
  UINT8   WheaSci;
  UINT64  ErrorBufferAddr;
  UINT64  ReadAckRegAddr;

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
  UINT8   Reserved2           :6; // reserved bits

  // SGX Start
  UINT8  SgxStatus;
  UINT64 EpcLength[8];            // MAX_IMC * MAX_SOCKET
  UINT64 EpcBaseAddress[8];       // MAX_IMC * MAX_SOCKET

  // PCIe Multi-Seg Start
  UINT8   PcieMultiSegSupport;    // Enable /Disable switch
  UINT8   PcieSegNum[NVS_MAX_SOCKETS]; // PCI segment number array for each socket

  // Sub-NUMA Cluster support (CLOD)
  UINT8   SncAnd2Cluster;         // 0 - SNC disabled, 2 - SNC enabled (2 clusters), 4 - SNC enabled (4 clusters)
  //
  // ProximityDomainPcie[][] in ASL is objects PX<socket><pe> defined per Socket/PCI Express and contain proximity domain for each PCIe.
  // ProximityDomainIoat[][] in ASL is objects IX<socket><ioat> defined per Socket/IO Accelerator contain proximity domain for each IOAT.
  //
  UINT16  ProximityDomainPcie[NVS_MAX_SOCKETS][NVS_MAX_IIO_PCIE_PER_SOCKET];
  UINT16  ProximityDomainIoat[NVS_MAX_SOCKETS][NVS_MAX_IIO_IOAT_PER_SOCKET];

  UINT16  PmBase;                  // ACPI IO Base Address
  UINT8   DebugModeIndicator;      // Debug Mode Indicator
  UINT8   IioPcieRpCapOffset;      // IIO PCIe root port PCIe Capability offset

  UINT8   TxtEnable;               // Indicates whether TXT is enabled or not based on MSR 0x3A

  // DiscreteConnecivity
  UINT8   SkipVidDidCheck;                          // Skip VDID check
  // Connectivity
  UINT32  DiscreteBtModuleRfKillGpio;               // Gpio for Bluetooth RfKill
  UINT32  DiscreteBtModuleIrqGpio;                  // Gpio for Bluetooth interrupt
  UINT8   AntennaDiversity;                         // AntennaDiversity
  UINT32  CnvExtClock;                              // CNV external 32KHz Clock
  // Connectivity WIFI
  UINT32   DiscreteWiFiRfKillGpio;                  //  Gpio for WiFi RF-Kill
  //WifiThermal.asl
  UINT8    WifiEnable;                              //  WifiEnable
  UINT8    WifiDomainType1;                         //  WifiDomainType1
  UINT16   WifiPowerLimit1;                         //  WifiPowerLimit1
  UINT32   WifiTimeWindow1;                         //  WifiTimeWindow1
  UINT8    TRxDelay0;                               //  TRxDelay0
  UINT8    TRxCableLength0;                         //  TRxCableLength0
  UINT8    TRxDelay1;                               //  TRxDelay1
  UINT8    TRxCableLength1;                         //  TRxCableLength1
  //WifiRegulatory.asl
  // WRDD -Method
  UINT8    WrddDomainType1;                         //  WrddDomainType1
  UINT16   WrddCountryIndentifier1;                 //  WrddCountryIndentifier1
  // WRDS -Method
  UINT8    WrdsWiFiSarEnable;                       // WrdsWiFiSarEnable
  UINT8    WrdsWiFiSarTxPowerSet1Limit1;            // WrdsWiFiSarTxPowerSet1Limit1
  UINT8    WrdsWiFiSarTxPowerSet1Limit2;            // WrdsWiFiSarTxPowerSet1Limit2
  UINT8    WrdsWiFiSarTxPowerSet1Limit3;            // WrdsWiFiSarTxPowerSet1Limit3
  UINT8    WrdsWiFiSarTxPowerSet1Limit4;            // WrdsWiFiSarTxPowerSet1Limit4
  UINT8    WrdsWiFiSarTxPowerSet1Limit5;            // WrdsWiFiSarTxPowerSet1Limit5
  UINT8    WrdsWiFiSarTxPowerSet1Limit6;            // WrdsWiFiSarTxPowerSet1Limit6
  UINT8    WrdsWiFiSarTxPowerSet1Limit7;            // WrdsWiFiSarTxPowerSet1Limit7
  UINT8    WrdsWiFiSarTxPowerSet1Limit8;            // WrdsWiFiSarTxPowerSet1Limit8
  UINT8    WrdsWiFiSarTxPowerSet1Limit9;            // WrdsWiFiSarTxPowerSet1Limit9
  UINT8    WrdsWiFiSarTxPowerSet1Limit10;           // WrdsWiFiSarTxPowerSet1Limit10
  UINT8    WrdsWiFiSarTxPowerSet1Limit11;           // WrdsWiFiSarTxPowerSet1Limit11
  UINT8    WrdsWiFiSarTxPowerSet1Limit12;           // WrdsWiFiSarTxPowerSet1Limit12
  UINT8    WrdsWiFiSarTxPowerSet1Limit13;           // WrdsWiFiSarTxPowerSet1Limit13
  UINT8    WrdsWiFiSarTxPowerSet1Limit14;           // WrdsWiFiSarTxPowerSet1Limit14
  UINT8    WrdsWiFiSarTxPowerSet1Limit15;           // WrdsWiFiSarTxPowerSet1Limit15
  UINT8    WrdsWiFiSarTxPowerSet1Limit16;           // WrdsWiFiSarTxPowerSet1Limit16
  UINT8    WrdsWiFiSarTxPowerSet1Limit17;           // WrdsWiFiSarTxPowerSet1Limit17
  UINT8    WrdsWiFiSarTxPowerSet1Limit18;           // WrdsWiFiSarTxPowerSet1Limit18
  UINT8    WrdsWiFiSarTxPowerSet1Limit19;           // WrdsWiFiSarTxPowerSet1Limit19
  UINT8    WrdsWiFiSarTxPowerSet1Limit20;           // WrdsWiFiSarTxPowerSet1Limit20
  UINT8    WrdsWiFiSarTxPowerSet1Limit21;           // WrdsWiFiSarTxPowerSet1Limit21
  UINT8    WrdsWiFiSarTxPowerSet1Limit22;           // WrdsWiFiSarTxPowerSet1Limit22

  UINT8    WrdsCdbWiFiSarTxPowerSet1Limit1;         // WrdsCdbWiFiSarTxPowerSet1Limit1
  UINT8    WrdsCdbWiFiSarTxPowerSet1Limit2;         // WrdsCdbWiFiSarTxPowerSet1Limit2
  UINT8    WrdsCdbWiFiSarTxPowerSet1Limit3;         // WrdsCdbWiFiSarTxPowerSet1Limit3
  UINT8    WrdsCdbWiFiSarTxPowerSet1Limit4;         // WrdsCdbWiFiSarTxPowerSet1Limit4
  UINT8    WrdsCdbWiFiSarTxPowerSet1Limit5;         // WrdsCdbWiFiSarTxPowerSet1Limit5
  UINT8    WrdsCdbWiFiSarTxPowerSet1Limit6;         // WrdsCdbWiFiSarTxPowerSet1Limit6
  UINT8    WrdsCdbWiFiSarTxPowerSet1Limit7;         // WrdsCdbWiFiSarTxPowerSet1Limit7
  UINT8    WrdsCdbWiFiSarTxPowerSet1Limit8;         // WrdsCdbWiFiSarTxPowerSet1Limit8
  UINT8    WrdsCdbWiFiSarTxPowerSet1Limit9;         // WrdsCdbWiFiSarTxPowerSet1Limit9
  UINT8    WrdsCdbWiFiSarTxPowerSet1Limit10;        // WrdsCdbWiFiSarTxPowerSet1Limit10
  UINT8    WrdsCdbWiFiSarTxPowerSet1Limit11;        // WrdsCdbWiFiSarTxPowerSet1Limit11
  UINT8    WrdsCdbWiFiSarTxPowerSet1Limit12;        // WrdsCdbWiFiSarTxPowerSet1Limit12
  UINT8    WrdsCdbWiFiSarTxPowerSet1Limit13;        // WrdsCdbWiFiSarTxPowerSet1Limit13
  UINT8    WrdsCdbWiFiSarTxPowerSet1Limit14;        // WrdsCdbWiFiSarTxPowerSet1Limit14
  UINT8    WrdsCdbWiFiSarTxPowerSet1Limit15;        // WrdsCdbWiFiSarTxPowerSet1Limit15
  UINT8    WrdsCdbWiFiSarTxPowerSet1Limit16;        // WrdsCdbWiFiSarTxPowerSet1Limit16
  UINT8    WrdsCdbWiFiSarTxPowerSet1Limit17;        // WrdsCdbWiFiSarTxPowerSet1Limit17
  UINT8    WrdsCdbWiFiSarTxPowerSet1Limit18;        // WrdsCdbWiFiSarTxPowerSet1Limit18
  UINT8    WrdsCdbWiFiSarTxPowerSet1Limit19;        // WrdsCdbWiFiSarTxPowerSet1Limit19
  UINT8    WrdsCdbWiFiSarTxPowerSet1Limit20;        // WrdsCdbWiFiSarTxPowerSet1Limit20
  UINT8    WrdsCdbWiFiSarTxPowerSet1Limit21;        // WrdsCdbWiFiSarTxPowerSet1Limit21
  UINT8    WrdsCdbWiFiSarTxPowerSet1Limit22;        // WrdsCdbWiFiSarTxPowerSet1Limit22
  // EWRD - Method
  UINT8    EwrdWiFiDynamicSarEnable;                // EwrdWiFiDynamicSarEnable
  UINT8    EwrdWiFiDynamicSarRangeSets;             // EwrdWiFiDynamicSarRangeSets

  UINT8    EwrdWiFiSarTxPowerSet2Limit1;            // EwrdWiFiSarTxPowerSet2Limit1
  UINT8    EwrdWiFiSarTxPowerSet2Limit2;            // EwrdWiFiSarTxPowerSet2Limit2
  UINT8    EwrdWiFiSarTxPowerSet2Limit3;            // EwrdWiFiSarTxPowerSet2Limit3
  UINT8    EwrdWiFiSarTxPowerSet2Limit4;            // EwrdWiFiSarTxPowerSet2Limit4
  UINT8    EwrdWiFiSarTxPowerSet2Limit5;            // EwrdWiFiSarTxPowerSet2Limit5
  UINT8    EwrdWiFiSarTxPowerSet2Limit6;            // EwrdWiFiSarTxPowerSet2Limit6
  UINT8    EwrdWiFiSarTxPowerSet2Limit7;            // EwrdWiFiSarTxPowerSet2Limit
  UINT8    EwrdWiFiSarTxPowerSet2Limit8;            // EwrdWiFiSarTxPowerSet2Limit8
  UINT8    EwrdWiFiSarTxPowerSet2Limit9;            // EwrdWiFiSarTxPowerSet2Limit9
  UINT8    EwrdWiFiSarTxPowerSet2Limit10;           // EwrdWiFiSarTxPowerSet2Limit10
  UINT8    EwrdWiFiSarTxPowerSet2Limit11;           // EwrdWiFiSarTxPowerSet2Limit11
  UINT8    EwrdWiFiSarTxPowerSet2Limit12;           // EwrdWiFiSarTxPowerSet2Limit12
  UINT8    EwrdWiFiSarTxPowerSet2Limit13;           // EwrdWiFiSarTxPowerSet2Limit13
  UINT8    EwrdWiFiSarTxPowerSet2Limit14;           // EwrdWiFiSarTxPowerSet2Limit14
  UINT8    EwrdWiFiSarTxPowerSet2Limit15;           // EwrdWiFiSarTxPowerSet2Limit15
  UINT8    EwrdWiFiSarTxPowerSet2Limit16;           // EwrdWiFiSarTxPowerSet2Limit16
  UINT8    EwrdWiFiSarTxPowerSet2Limit17;           // EwrdWiFiSarTxPowerSet2Limit17
  UINT8    EwrdWiFiSarTxPowerSet2Limit18;           // EwrdWiFiSarTxPowerSet2Limit18
  UINT8    EwrdWiFiSarTxPowerSet2Limit19;           // EwrdWiFiSarTxPowerSet2Limit19
  UINT8    EwrdWiFiSarTxPowerSet2Limit20;           // EwrdWiFiSarTxPowerSet2Limit20
  UINT8    EwrdWiFiSarTxPowerSet2Limit21;           // EwrdWiFiSarTxPowerSet2Limit21
  UINT8    EwrdWiFiSarTxPowerSet2Limit22;           // EwrdWiFiSarTxPowerSet2Limit22

  UINT8    EwrdWiFiSarTxPowerSet3Limit1;            // EwrdWiFiSarTxPowerSet3Limit1
  UINT8    EwrdWiFiSarTxPowerSet3Limit2;            // EwrdWiFiSarTxPowerSet3Limit2
  UINT8    EwrdWiFiSarTxPowerSet3Limit3;            // EwrdWiFiSarTxPowerSet3Limit3
  UINT8    EwrdWiFiSarTxPowerSet3Limit4;            // EwrdWiFiSarTxPowerSet3Limit4
  UINT8    EwrdWiFiSarTxPowerSet3Limit5;            // EwrdWiFiSarTxPowerSet3Limit5
  UINT8    EwrdWiFiSarTxPowerSet3Limit6;            // EwrdWiFiSarTxPowerSet3Limit6
  UINT8    EwrdWiFiSarTxPowerSet3Limit7;            // EwrdWiFiSarTxPowerSet3Limit7
  UINT8    EwrdWiFiSarTxPowerSet3Limit8;            // EwrdWiFiSarTxPowerSet3Limit8
  UINT8    EwrdWiFiSarTxPowerSet3Limit9;            // EwrdWiFiSarTxPowerSet3Limit9
  UINT8    EwrdWiFiSarTxPowerSet3Limit10;           // EwrdWiFiSarTxPowerSet3Limit10
  UINT8    EwrdWiFiSarTxPowerSet3Limit11;           // EwrdWiFiSarTxPowerSet3Limit11
  UINT8    EwrdWiFiSarTxPowerSet3Limit12;           // EwrdWiFiSarTxPowerSet3Limit12
  UINT8    EwrdWiFiSarTxPowerSet3Limit13;           // EwrdWiFiSarTxPowerSet3Limit13
  UINT8    EwrdWiFiSarTxPowerSet3Limit14;           // EwrdWiFiSarTxPowerSet3Limit14
  UINT8    EwrdWiFiSarTxPowerSet3Limit15;           // EwrdWiFiSarTxPowerSet3Limit15
  UINT8    EwrdWiFiSarTxPowerSet3Limit16;           // EwrdWiFiSarTxPowerSet3Limit16
  UINT8    EwrdWiFiSarTxPowerSet3Limit17;           // EwrdWiFiSarTxPowerSet3Limit17
  UINT8    EwrdWiFiSarTxPowerSet3Limit18;           // EwrdWiFiSarTxPowerSet3Limit18
  UINT8    EwrdWiFiSarTxPowerSet3Limit19;           // EwrdWiFiSarTxPowerSet3Limit19
  UINT8    EwrdWiFiSarTxPowerSet3Limit20;           // EwrdWiFiSarTxPowerSet3Limit20
  UINT8    EwrdWiFiSarTxPowerSet3Limit21;           // EwrdWiFiSarTxPowerSet3Limit21
  UINT8    EwrdWiFiSarTxPowerSet3Limit22;           // EwrdWiFiSarTxPowerSet3Limit22

  UINT8    EwrdWiFiSarTxPowerSet4Limit1;            // EwrdWiFiSarTxPowerSet4Limit1
  UINT8    EwrdWiFiSarTxPowerSet4Limit2;            // EwrdWiFiSarTxPowerSet4Limit2
  UINT8    EwrdWiFiSarTxPowerSet4Limit3;            // EwrdWiFiSarTxPowerSet4Limit3
  UINT8    EwrdWiFiSarTxPowerSet4Limit4;            // EwrdWiFiSarTxPowerSet4Limit4
  UINT8    EwrdWiFiSarTxPowerSet4Limit5;            // EwrdWiFiSarTxPowerSet4Limit5
  UINT8    EwrdWiFiSarTxPowerSet4Limit6;            // EwrdWiFiSarTxPowerSet4Limit6
  UINT8    EwrdWiFiSarTxPowerSet4Limit7;            // EwrdWiFiSarTxPowerSet4Limit7
  UINT8    EwrdWiFiSarTxPowerSet4Limit8;            // EwrdWiFiSarTxPowerSet4Limit8
  UINT8    EwrdWiFiSarTxPowerSet4Limit9;            // EwrdWiFiSarTxPowerSet4Limit9
  UINT8    EwrdWiFiSarTxPowerSet4Limit10;           // EwrdWiFiSarTxPowerSet4Limit10
  UINT8    EwrdWiFiSarTxPowerSet4Limit11;           // EwrdWiFiSarTxPowerSet4Limit11
  UINT8    EwrdWiFiSarTxPowerSet4Limit12;           // EwrdWiFiSarTxPowerSet4Limit12
  UINT8    EwrdWiFiSarTxPowerSet4Limit13;           // EwrdWiFiSarTxPowerSet4Limit13
  UINT8    EwrdWiFiSarTxPowerSet4Limit14;           // EwrdWiFiSarTxPowerSet4Limit14
  UINT8    EwrdWiFiSarTxPowerSet4Limit15;           // EwrdWiFiSarTxPowerSet4Limit15
  UINT8    EwrdWiFiSarTxPowerSet4Limit16;           // EwrdWiFiSarTxPowerSet4Limit16
  UINT8    EwrdWiFiSarTxPowerSet4Limit17;           // EwrdWiFiSarTxPowerSet4Limit17
  UINT8    EwrdWiFiSarTxPowerSet4Limit18;           // EwrdWiFiSarTxPowerSet4Limit18
  UINT8    EwrdWiFiSarTxPowerSet4Limit19;           // EwrdWiFiSarTxPowerSet4Limit19
  UINT8    EwrdWiFiSarTxPowerSet4Limit20;           // EwrdWiFiSarTxPowerSet4Limit20
  UINT8    EwrdWiFiSarTxPowerSet4Limit21;           // EwrdWiFiSarTxPowerSet4Limit21
  UINT8    EwrdWiFiSarTxPowerSet4Limit22;           // EwrdWiFiSarTxPowerSet4Limit22

  UINT8    EwrdCdbWiFiSarTxPowerSet2Limit1;         // EwrdCdbWiFiSarTxPowerSet2Limit1
  UINT8    EwrdCdbWiFiSarTxPowerSet2Limit2;         // EwrdCdbWiFiSarTxPowerSet2Limit2
  UINT8    EwrdCdbWiFiSarTxPowerSet2Limit3;         // EwrdCdbWiFiSarTxPowerSet2Limit3
  UINT8    EwrdCdbWiFiSarTxPowerSet2Limit4;         // EwrdCdbWiFiSarTxPowerSet2Limit4
  UINT8    EwrdCdbWiFiSarTxPowerSet2Limit5;         // EwrdCdbWiFiSarTxPowerSet2Limit5
  UINT8    EwrdCdbWiFiSarTxPowerSet2Limit6;         // EwrdCdbWiFiSarTxPowerSet2Limit6
  UINT8    EwrdCdbWiFiSarTxPowerSet2Limit7;         // EwrdCdbWiFiSarTxPowerSet2Limit7
  UINT8    EwrdCdbWiFiSarTxPowerSet2Limit8;         // EwrdCdbWiFiSarTxPowerSet2Limit8
  UINT8    EwrdCdbWiFiSarTxPowerSet2Limit9;         // EwrdCdbWiFiSarTxPowerSet2Limit9
  UINT8    EwrdCdbWiFiSarTxPowerSet2Limit10;        // EwrdCdbWiFiSarTxPowerSet2Limit10
  UINT8    EwrdCdbWiFiSarTxPowerSet2Limit11;        // EwrdCdbWiFiSarTxPowerSet2Limit11
  UINT8    EwrdCdbWiFiSarTxPowerSet2Limit12;        // EwrdCdbWiFiSarTxPowerSet2Limit12
  UINT8    EwrdCdbWiFiSarTxPowerSet2Limit13;        // EwrdCdbWiFiSarTxPowerSet2Limit13
  UINT8    EwrdCdbWiFiSarTxPowerSet2Limit14;        // EwrdCdbWiFiSarTxPowerSet2Limit14
  UINT8    EwrdCdbWiFiSarTxPowerSet2Limit15;        // EwrdCdbWiFiSarTxPowerSet2Limit15
  UINT8    EwrdCdbWiFiSarTxPowerSet2Limit16;        // EwrdCdbWiFiSarTxPowerSet2Limit16
  UINT8    EwrdCdbWiFiSarTxPowerSet2Limit17;        // EwrdCdbWiFiSarTxPowerSet2Limit17
  UINT8    EwrdCdbWiFiSarTxPowerSet2Limit18;        // EwrdCdbWiFiSarTxPowerSet2Limit18
  UINT8    EwrdCdbWiFiSarTxPowerSet2Limit19;        // EwrdCdbWiFiSarTxPowerSet2Limit19
  UINT8    EwrdCdbWiFiSarTxPowerSet2Limit20;        // EwrdCdbWiFiSarTxPowerSet2Limit20
  UINT8    EwrdCdbWiFiSarTxPowerSet2Limit21;        // EwrdCdbWiFiSarTxPowerSet2Limit21
  UINT8    EwrdCdbWiFiSarTxPowerSet2Limit22;        // EwrdCdbWiFiSarTxPowerSet2Limit22

  UINT8    EwrdCdbWiFiSarTxPowerSet3Limit1;         // EwrdCdbWiFiSarTxPowerSet3Limit1
  UINT8    EwrdCdbWiFiSarTxPowerSet3Limit2;         // EwrdCdbWiFiSarTxPowerSet3Limit2
  UINT8    EwrdCdbWiFiSarTxPowerSet3Limit3;         // EwrdCdbWiFiSarTxPowerSet3Limit3
  UINT8    EwrdCdbWiFiSarTxPowerSet3Limit4;         // EwrdCdbWiFiSarTxPowerSet3Limit4
  UINT8    EwrdCdbWiFiSarTxPowerSet3Limit5;         // EwrdCdbWiFiSarTxPowerSet3Limit5
  UINT8    EwrdCdbWiFiSarTxPowerSet3Limit6;         // EwrdCdbWiFiSarTxPowerSet3Limit6
  UINT8    EwrdCdbWiFiSarTxPowerSet3Limit7;         // EwrdCdbWiFiSarTxPowerSet3Limit7
  UINT8    EwrdCdbWiFiSarTxPowerSet3Limit8;         // EwrdCdbWiFiSarTxPowerSet3Limit8
  UINT8    EwrdCdbWiFiSarTxPowerSet3Limit9;         // EwrdCdbWiFiSarTxPowerSet3Limit9
  UINT8    EwrdCdbWiFiSarTxPowerSet3Limit10;        // EwrdCdbWiFiSarTxPowerSet3Limit10
  UINT8    EwrdCdbWiFiSarTxPowerSet3Limit11;        // EwrdCdbWiFiSarTxPowerSet3Limit11
  UINT8    EwrdCdbWiFiSarTxPowerSet3Limit12;        // EwrdCdbWiFiSarTxPowerSet3Limit12
  UINT8    EwrdCdbWiFiSarTxPowerSet3Limit13;        // EwrdCdbWiFiSarTxPowerSet3Limit13
  UINT8    EwrdCdbWiFiSarTxPowerSet3Limit14;        // EwrdCdbWiFiSarTxPowerSet3Limit14
  UINT8    EwrdCdbWiFiSarTxPowerSet3Limit15;        // EwrdCdbWiFiSarTxPowerSet3Limit15
  UINT8    EwrdCdbWiFiSarTxPowerSet3Limit16;        // EwrdCdbWiFiSarTxPowerSet3Limit16
  UINT8    EwrdCdbWiFiSarTxPowerSet3Limit17;        // EwrdCdbWiFiSarTxPowerSet3Limit17
  UINT8    EwrdCdbWiFiSarTxPowerSet3Limit18;        // EwrdCdbWiFiSarTxPowerSet3Limit18
  UINT8    EwrdCdbWiFiSarTxPowerSet3Limit19;        // EwrdCdbWiFiSarTxPowerSet3Limit19
  UINT8    EwrdCdbWiFiSarTxPowerSet3Limit20;        // EwrdCdbWiFiSarTxPowerSet3Limit20
  UINT8    EwrdCdbWiFiSarTxPowerSet3Limit21;        // EwrdCdbWiFiSarTxPowerSet3Limit21
  UINT8    EwrdCdbWiFiSarTxPowerSet3Limit22;        // EwrdCdbWiFiSarTxPowerSet3Limit22

  UINT8    EwrdCdbWiFiSarTxPowerSet4Limit1;         // EwrdCdbWiFiSarTxPowerSet4Limit1
  UINT8    EwrdCdbWiFiSarTxPowerSet4Limit2;         // EwrdCdbWiFiSarTxPowerSet4Limit2
  UINT8    EwrdCdbWiFiSarTxPowerSet4Limit3;         // EwrdCdbWiFiSarTxPowerSet4Limit3
  UINT8    EwrdCdbWiFiSarTxPowerSet4Limit4;         // EwrdCdbWiFiSarTxPowerSet4Limit4
  UINT8    EwrdCdbWiFiSarTxPowerSet4Limit5;         // EwrdCdbWiFiSarTxPowerSet4Limit5
  UINT8    EwrdCdbWiFiSarTxPowerSet4Limit6;         // EwrdCdbWiFiSarTxPowerSet4Limit6
  UINT8    EwrdCdbWiFiSarTxPowerSet4Limit7;         // EwrdCdbWiFiSarTxPowerSet4Limit7
  UINT8    EwrdCdbWiFiSarTxPowerSet4Limit8;         // EwrdCdbWiFiSarTxPowerSet4Limit8
  UINT8    EwrdCdbWiFiSarTxPowerSet4Limit9;         // EwrdCdbWiFiSarTxPowerSet4Limit9
  UINT8    EwrdCdbWiFiSarTxPowerSet4Limit10;        // EwrdCdbWiFiSarTxPowerSet4Limit10
  UINT8    EwrdCdbWiFiSarTxPowerSet4Limit11;        // EwrdCdbWiFiSarTxPowerSet4Limit11
  UINT8    EwrdCdbWiFiSarTxPowerSet4Limit12;        // EwrdCdbWiFiSarTxPowerSet4Limit12
  UINT8    EwrdCdbWiFiSarTxPowerSet4Limit13;        // EwrdCdbWiFiSarTxPowerSet4Limit13
  UINT8    EwrdCdbWiFiSarTxPowerSet4Limit14;        // EwrdCdbWiFiSarTxPowerSet4Limit14
  UINT8    EwrdCdbWiFiSarTxPowerSet4Limit15;        // EwrdCdbWiFiSarTxPowerSet4Limit15
  UINT8    EwrdCdbWiFiSarTxPowerSet4Limit16;        // EwrdCdbWiFiSarTxPowerSet4Limit16
  UINT8    EwrdCdbWiFiSarTxPowerSet4Limit17;        // EwrdCdbWiFiSarTxPowerSet4Limit17
  UINT8    EwrdCdbWiFiSarTxPowerSet4Limit18;        // EwrdCdbWiFiSarTxPowerSet4Limit18
  UINT8    EwrdCdbWiFiSarTxPowerSet4Limit19;        // EwrdCdbWiFiSarTxPowerSet4Limit19
  UINT8    EwrdCdbWiFiSarTxPowerSet4Limit20;        // EwrdCdbWiFiSarTxPowerSet4Limit20
  UINT8    EwrdCdbWiFiSarTxPowerSet4Limit21;        // EwrdCdbWiFiSarTxPowerSet4Limit21
  UINT8    EwrdCdbWiFiSarTxPowerSet4Limit22;        // EwrdCdbWiFiSarTxPowerSet4Limit22

  // Method -WGDS
  UINT8    WgdsWiFiSarDeltaGroup1PowerMax1;         // WgdsWiFiSarDeltaGroup1PowerMax1
  UINT8    WgdsWiFiSarDeltaGroup1PowerChainA1;      // WgdsWiFiSarDeltaGroup1PowerChainA1
  UINT8    WgdsWiFiSarDeltaGroup1PowerChainB1;      // WgdsWiFiSarDeltaGroup1PowerChainB1
  UINT8    WgdsWiFiSarDeltaGroup1PowerMax2;         // WgdsWiFiSarDeltaGroup1PowerMax2
  UINT8    WgdsWiFiSarDeltaGroup1PowerChainA2;      // WgdsWiFiSarDeltaGroup1PowerChainA2
  UINT8    WgdsWiFiSarDeltaGroup1PowerChainB2;      // WgdsWiFiSarDeltaGroup1PowerChainB2
  UINT8    WgdsWiFiSarDeltaGroup1PowerMax3;         // WgdsWiFiSarDeltaGroup1PowerMax3
  UINT8    WgdsWiFiSarDeltaGroup1PowerChainA3;      // WgdsWiFiSarDeltaGroup1PowerChainA3
  UINT8    WgdsWiFiSarDeltaGroup1PowerChainB3;      // WgdsWiFiSarDeltaGroup1PowerChainB3

  UINT8    WgdsWiFiSarDeltaGroup2PowerMax1;         // WgdsWiFiSarDeltaGroup2PowerMax1
  UINT8    WgdsWiFiSarDeltaGroup2PowerChainA1;      // WgdsWiFiSarDeltaGroup2PowerChainA1
  UINT8    WgdsWiFiSarDeltaGroup2PowerChainB1;      // WgdsWiFiSarDeltaGroup2PowerChainB1
  UINT8    WgdsWiFiSarDeltaGroup2PowerMax2;         // WgdsWiFiSarDeltaGroup2PowerMax2
  UINT8    WgdsWiFiSarDeltaGroup2PowerChainA2;      // WgdsWiFiSarDeltaGroup2PowerChainA2
  UINT8    WgdsWiFiSarDeltaGroup2PowerChainB2;      // WgdsWiFiSarDeltaGroup2PowerChainB2
  UINT8    WgdsWiFiSarDeltaGroup2PowerMax3;         // WgdsWiFiSarDeltaGroup2PowerMax3
  UINT8    WgdsWiFiSarDeltaGroup2PowerChainA3;      // WgdsWiFiSarDeltaGroup2PowerChainA3
  UINT8    WgdsWiFiSarDeltaGroup2PowerChainB3;      // WgdsWiFiSarDeltaGroup2PowerChainB3

  UINT8    WgdsWiFiSarDeltaGroup3PowerMax1;         // WgdsWiFiSarDeltaGroup3PowerMax1
  UINT8    WgdsWiFiSarDeltaGroup3PowerChainA1;      // WgdsWiFiSarDeltaGroup3PowerChainA1
  UINT8    WgdsWiFiSarDeltaGroup3PowerChainB1;      // WgdsWiFiSarDeltaGroup3PowerChainB1
  UINT8    WgdsWiFiSarDeltaGroup3PowerMax2;         // WgdsWiFiSarDeltaGroup3PowerMax2
  UINT8    WgdsWiFiSarDeltaGroup3PowerChainA2;      // WgdsWiFiSarDeltaGroup3PowerChainA2
  UINT8    WgdsWiFiSarDeltaGroup3PowerChainB2;      // WgdsWiFiSarDeltaGroup3PowerChainB2
  UINT8    WgdsWiFiSarDeltaGroup3PowerMax3;         // WgdsWiFiSarDeltaGroup3PowerMax3
  UINT8    WgdsWiFiSarDeltaGroup3PowerChainA3;      // WgdsWiFiSarDeltaGroup3PowerChainA3
  UINT8    WgdsWiFiSarDeltaGroup3PowerChainB3;      // WgdsWiFiSarDeltaGroup3PowerChainB3

  // Method -SGOM
  UINT8    WifiSarGeoMappingMode;                   // SAR GEO Mapping Mode
  UINT8    WifiSarGeoOffset0;                       // 0 - DEFAULT
  UINT8    WifiSarGeoOffset1;                       // 1 - FCC
  UINT8    WifiSarGeoOffset2;                       // 2 - USA TW
  UINT8    WifiSarGeoOffset3;                       // 3 - Canada IC
  UINT8    WifiSarGeoOffset4;                       // 4 - ETSI 5G8SRD
  UINT8    WifiSarGeoOffset5;                       // 5 - ETSI 5G8Med
  UINT8    WifiSarGeoOffset6;                       // 6 - Japan
  UINT8    WifiSarGeoOffset7;                       // 7 - Brazil
  UINT8    WifiSarGeoOffset8;                       // 8 - ETSI 5G8FCC
  UINT8    WifiSarGeoOffset9;                       // 9 - Indonesia
  UINT8    WifiSarGeoOffset10;                      // 10 - South Korea
  UINT8    WifiSarGeoOffset11;                      // 11 - Chile
  UINT8    WifiSarGeoOffset12;                      // 12 - ETSI 5G8Pass
  UINT8    WifiSarGeoOffset13;                      // 13 - Pakistan
  UINT8    WifiSarGeoOffset14;                      // 14 - No 11ac
  UINT8    WifiSarGeoOffset15;                      // 15 - 2G4 Only
  UINT8    WifiSarGeoOffset16;                      // 16 - China BIOS
  UINT8    WifiSarGeoOffset17;                      // 17 - Russia
  UINT8    WifiSarGeoOffset18;                      // 18 - EU + ETSI + 5G8SRD

  // Method -PPAG
  UINT8    WifiAntGainEnable;                       // Wifi Ant Gain Enable
  UINT8    WifiAntGainChainA1;                      // Ant Gain Table Chain A 2400
  UINT8    WifiAntGainChainA2;                      // Ant Gain Table Chain A 5150-5350
  UINT8    WifiAntGainChainA3;                      // Ant Gain Table Chain A 5350-5470
  UINT8    WifiAntGainChainA4;                      // Ant Gain Table Chain A 5470-5725
  UINT8    WifiAntGainChainA5;                      // Ant Gain Table Chain A 5725-5950
  UINT8    WifiAntGainChainA6;                      // Ant Gain Table Chain A 5945-6165git
  UINT8    WifiAntGainChainA7;                      // Ant Gain Table Chain A 6165-6405
  UINT8    WifiAntGainChainA8;                      // Ant Gain Table Chain A 6405-6525
  UINT8    WifiAntGainChainA9;                      // Ant Gain Table Chain A 6525-6705
  UINT8    WifiAntGainChainA10;                     // Ant Gain Table Chain A 6705-6865
  UINT8    WifiAntGainChainA11;                     // Ant Gain Table Chain A 6865-7105

  UINT8    WifiAntGainChainB1;                      // Ant Gain Table Chain B 2400
  UINT8    WifiAntGainChainB2;                      // Ant Gain Table Chain B 5150-5350
  UINT8    WifiAntGainChainB3;                      // Ant Gain Table Chain B 5350-5470
  UINT8    WifiAntGainChainB4;                      // Ant Gain Table Chain B 5470-5725
  UINT8    WifiAntGainChainB5;                      // Ant Gain Table Chain B 5725-5950
  UINT8    WifiAntGainChainB6;                      // Ant Gain Table Chain B 5945-6165
  UINT8    WifiAntGainChainB7;                      // Ant Gain Table Chain B 6165-6405
  UINT8    WifiAntGainChainB8;                      // Ant Gain Table Chain B 6405-6525
  UINT8    WifiAntGainChainB9;                      // Ant Gain Table Chain B 6525-6705
  UINT8    WifiAntGainChainB10;                     // Ant Gain Table Chain B 6705-6865
  UINT8    WifiAntGainChainB11;                     // Ant Gain Table Chain B 6865-7105
  // Method - WPFC
  UINT32   WifiPhyFilterConfigA;                    // Platform PHY Config Chain A
  UINT32   WifiPhyFilterConfigB;                    // Platform PHY Config Chain B
  UINT32   WifiPhyFilterConfigC;                    // Platform PHY Config Chain C
  UINT32   WifiPhyFilterConfigD;                    // Platform PHY Config Chain D
  // Method - WTAS
  UINT8   WifiTASSelection;                         // WifiTASSelection
  UINT8   WifiTASListEntries;                       // WifiTASListEntries
  // WifiFeatureDsm.asl
  UINT8    WifiActiveChannelSrd;                    // SRD Active Channels Selection
  UINT8    WifiIndonesia5GhzSupport;                // Indonesia 5.15-5.35 GHz Band Support Selection
  UINT32   WifiUltraHighBandSupport;                // WifiUltraHighBandSupport
  // WifiDynamicSar.asl
  UINT8    WiFiDynamicSarAntennaACurrentSet;        // WiFiDynamicSarAntennaACurrentSet
  UINT8    WiFiDynamicSarAntennaBCurrentSet;        // WiFiDynamicSarAntennaBCurrentSet
  // Connectivity BT
  // BtRegulatory.asl
  UINT8    BluetoothSar;                            // BluetoothSar
  UINT8    BluetoothSarBr;                          // BluetoothSarBr
  UINT8    BluetoothSarEdr2;                        // BluetoothSarEdr2
  UINT8    BluetoothSarEdr3;                        // BluetoothSarEdr3
  UINT8    BluetoothSarLe;                          // BluetoothSarLe
  UINT8    BluetoothSarLe2Mhz;                      // BluetoothSarLe2Mhz
  UINT8    BluetoothSarLeLr;                        // BluetoothSarLeLr
  UINT8    SDS9;                                    // DiscreteBtModule
  UINT8    SDM9;                                    // DiscreteBtModuleIrqMode

  UINT8    ECR1;                                    // ECR1
} BIOS_ACPI_PARAM;

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
  UINT64   SBRG;                                    ///< Offset 242     SBREG_BAR
  UINT8    GEI0;                                    ///< Offset 250     GPIO GroupIndex mapped to GPE_DW0
  UINT8    GEI1;                                    ///< Offset 251     GPIO GroupIndex mapped to GPE_DW1
  UINT8    GEI2;                                    ///< Offset 252     GPIO GroupIndex mapped to GPE_DW2
  UINT8    GED0;                                    ///< Offset 253     GPIO DW part of group mapped to GPE_DW0
  UINT8    GED1;                                    ///< Offset 254     GPIO DW part of group mapped to GPE_DW1
  UINT8    GED2;                                    ///< Offset 255     GPIO DW part of group mapped to GPE_DW2
  UINT16   PcieLtrMaxSnoopLatency[28];              ///< Offset 256     PCIE LTR max snoop Latency 1
                                                    ///< Offset 258     PCIE LTR max snoop Latency 2
                                                    ///< Offset 260     PCIE LTR max snoop Latency 3
                                                    ///< Offset 262     PCIE LTR max snoop Latency 4
                                                    ///< Offset 264     PCIE LTR max snoop Latency 5
                                                    ///< Offset 266     PCIE LTR max snoop Latency 6
                                                    ///< Offset 268     PCIE LTR max snoop Latency 7
                                                    ///< Offset 270     PCIE LTR max snoop Latency 8
                                                    ///< Offset 272     PCIE LTR max snoop Latency 9
                                                    ///< Offset 274     PCIE LTR max snoop Latency 10
                                                    ///< Offset 276     PCIE LTR max snoop Latency 11
                                                    ///< Offset 278     PCIE LTR max snoop Latency 12
                                                    ///< Offset 280     PCIE LTR max snoop Latency 13
                                                    ///< Offset 282     PCIE LTR max snoop Latency 14
                                                    ///< Offset 284     PCIE LTR max snoop Latency 15
                                                    ///< Offset 286     PCIE LTR max snoop Latency 16
                                                    ///< Offset 288     PCIE LTR max snoop Latency 17
                                                    ///< Offset 290     PCIE LTR max snoop Latency 18
                                                    ///< Offset 292     PCIE LTR max snoop Latency 19
                                                    ///< Offset 294     PCIE LTR max snoop Latency 20
                                                    ///< Offset 296     PCIE LTR max snoop Latency 21
                                                    ///< Offset 298     PCIE LTR max snoop Latency 22
                                                    ///< Offset 300     PCIE LTR max snoop Latency 23
                                                    ///< Offset 302     PCIE LTR max snoop Latency 24
                                                    ///< Offset 304     PCIE LTR max snoop Latency 25
                                                    ///< Offset 306     PCIE LTR max snoop Latency 26
                                                    ///< Offset 308     PCIE LTR max snoop Latency 27
                                                    ///< Offset 310     PCIE LTR max snoop Latency 28
  UINT16   PcieLtrMaxNoSnoopLatency[28];            ///< Offset 312     PCIE LTR max no snoop Latency 1
                                                    ///< Offset 314     PCIE LTR max no snoop Latency 2
                                                    ///< Offset 316     PCIE LTR max no snoop Latency 3
                                                    ///< Offset 318     PCIE LTR max no snoop Latency 4
                                                    ///< Offset 320     PCIE LTR max no snoop Latency 5
                                                    ///< Offset 322     PCIE LTR max no snoop Latency 6
                                                    ///< Offset 324     PCIE LTR max no snoop Latency 7
                                                    ///< Offset 326     PCIE LTR max no snoop Latency 8
                                                    ///< Offset 328     PCIE LTR max no snoop Latency 9
                                                    ///< Offset 330     PCIE LTR max no snoop Latency 10
                                                    ///< Offset 332     PCIE LTR max no snoop Latency 11
                                                    ///< Offset 334     PCIE LTR max no snoop Latency 12
                                                    ///< Offset 336     PCIE LTR max no snoop Latency 13
                                                    ///< Offset 338     PCIE LTR max no snoop Latency 14
                                                    ///< Offset 340     PCIE LTR max no snoop Latency 15
                                                    ///< Offset 342     PCIE LTR max no snoop Latency 16
                                                    ///< Offset 344     PCIE LTR max no snoop Latency 17
                                                    ///< Offset 346     PCIE LTR max no snoop Latency 18
                                                    ///< Offset 348     PCIE LTR max no snoop Latency 19
                                                    ///< Offset 350     PCIE LTR max no snoop Latency 20
                                                    ///< Offset 352     PCIE LTR max no snoop Latency 21
                                                    ///< Offset 354     PCIE LTR max no snoop Latency 22
                                                    ///< Offset 356     PCIE LTR max no snoop Latency 23
                                                    ///< Offset 358     PCIE LTR max no snoop Latency 24
                                                    ///< Offset 360     PCIE LTR max no snoop Latency 25
                                                    ///< Offset 362     PCIE LTR max no snoop Latency 26
                                                    ///< Offset 364     PCIE LTR max no snoop Latency 27
                                                    ///< Offset 366     PCIE LTR max no snoop Latency 28
  UINT8    XHPC;                                    ///< Offset 368     Number of HighSpeed ports implemented in XHCI controller
  UINT8    XRPC;                                    ///< Offset 369     Number of USBR ports implemented in XHCI controller
  UINT8    XSPC;                                    ///< Offset 370     Number of SuperSpeed ports implemented in XHCI controller
  UINT8    XSPA;                                    ///< Offset 371     Address of 1st SuperSpeed port
  UINT32   HPTB;                                    ///< Offset 372     HPET base address
  UINT8    HPTE;                                    ///< Offset 376     HPET enable
  //SerialIo block
  UINT8    SM0[7];                                  ///< Offset 377     SerialIo SPI Controller 0 Mode
                                                    ///< Offset 378     SerialIo SPI Controller 1 Mode
                                                    ///< Offset 379     SerialIo SPI Controller 2 Mode
                                                    ///< Offset 380     SerialIo SPI Controller 3 Mode
                                                    ///< Offset 381     SerialIo SPI Controller 4 Mode
                                                    ///< Offset 382     SerialIo SPI Controller 5 Mode
                                                    ///< Offset 383     SerialIo SPI Controller 6 Mode
  UINT64   SC0[7];                                  ///< Offset 384     SerialIo SPI Controller 0 Pci Config
                                                    ///< Offset 392     SerialIo SPI Controller 1 Pci Config
                                                    ///< Offset 400     SerialIo SPI Controller 2 Pci Config
                                                    ///< Offset 408     SerialIo SPI Controller 3 Pci Config
                                                    ///< Offset 416     SerialIo SPI Controller 4 Pci Config
                                                    ///< Offset 424     SerialIo SPI Controller 5 Pci Config
                                                    ///< Offset 432     SerialIo SPI Controller 6 Pci Config
  UINT8    IM0[8];                                  ///< Offset 440     SerialIo I2C Controller 0 Mode
                                                    ///< Offset 441     SerialIo I2C Controller 1 Mode
                                                    ///< Offset 442     SerialIo I2C Controller 2 Mode
                                                    ///< Offset 443     SerialIo I2C Controller 3 Mode
                                                    ///< Offset 444     SerialIo I2C Controller 4 Mode
                                                    ///< Offset 445     SerialIo I2C Controller 5 Mode
                                                    ///< Offset 446     SerialIo I2C Controller 6 Mode
                                                    ///< Offset 447     SerialIo I2C Controller 7 Mode
  UINT64   IC0[8];                                  ///< Offset 448     SerialIo I2C Controller 0 Pci Config
                                                    ///< Offset 456     SerialIo I2C Controller 1 Pci Config
                                                    ///< Offset 464     SerialIo I2C Controller 2 Pci Config
                                                    ///< Offset 472     SerialIo I2C Controller 3 Pci Config
                                                    ///< Offset 480     SerialIo I2C Controller 4 Pci Config
                                                    ///< Offset 488     SerialIo I2C Controller 5 Pci Config
                                                    ///< Offset 496     SerialIo I2C Controller 6 Pci Config
                                                    ///< Offset 504     SerialIo I2C Controller 7 Pci Config
  UINT8    UM0[7];                                  ///< Offset 512     SerialIo UART Controller 0 Mode
                                                    ///< Offset 513     SerialIo UART Controller 1 Mode
                                                    ///< Offset 514     SerialIo UART Controller 2 Mode
                                                    ///< Offset 515     SerialIo UART Controller 3 Mode
                                                    ///< Offset 516     SerialIo UART Controller 4 Mode
                                                    ///< Offset 517     SerialIo UART Controller 5 Mode
                                                    ///< Offset 518     SerialIo UART Controller 6 Mode
  UINT64   UC0[7];                                  ///< Offset 519     SerialIo UART Controller 0 Pci Config
                                                    ///< Offset 527     SerialIo UART Controller 1 Pci Config
                                                    ///< Offset 535     SerialIo UART Controller 2 Pci Config
                                                    ///< Offset 543     SerialIo UART Controller 3 Pci Config
                                                    ///< Offset 551     SerialIo UART Controller 4 Pci Config
                                                    ///< Offset 559     SerialIo UART Controller 5 Pci Config
                                                    ///< Offset 567     SerialIo UART Controller 6 Pci Config
  UINT8    UD0[7];                                  ///< Offset 575     SerialIo UART Controller 0 DmaEnable
                                                    ///< Offset 576     SerialIo UART Controller 1 DmaEnable
                                                    ///< Offset 577     SerialIo UART Controller 2 DmaEnable
                                                    ///< Offset 578     SerialIo UART Controller 3 DmaEnable
                                                    ///< Offset 579     SerialIo UART Controller 4 DmaEnable
                                                    ///< Offset 580     SerialIo UART Controller 5 DmaEnable
                                                    ///< Offset 581     SerialIo UART Controller 6 DmaEnable
  UINT8    UP0[7];                                  ///< Offset 582     SerialIo UART Controller 0 Power Gating
                                                    ///< Offset 583     SerialIo UART Controller 1 Power Gating
                                                    ///< Offset 584     SerialIo UART Controller 2 Power Gating
                                                    ///< Offset 585     SerialIo UART Controller 3 Power Gating
                                                    ///< Offset 586     SerialIo UART Controller 4 Power Gating
                                                    ///< Offset 587     SerialIo UART Controller 5 Power Gating
                                                    ///< Offset 588     SerialIo UART Controller 6 Power Gating
  UINT8    UI0[7];                                  ///< Offset 589     SerialIo UART Controller 0 Irq
                                                    ///< Offset 590     SerialIo UART Controller 1 Irq
                                                    ///< Offset 591     SerialIo UART Controller 2 Irq
                                                    ///< Offset 592     SerialIo UART Controller 3 Irq
                                                    ///< Offset 593     SerialIo UART Controller 4 Irq
                                                    ///< Offset 594     SerialIo UART Controller 5 Irq
                                                    ///< Offset 595     SerialIo UART Controller 6 Irq
  //end of SerialIo block
  UINT8    SGIR;                                    ///< Offset 596     GPIO IRQ
  UINT8    GPHD;                                    ///< Offset 597     Hide GPIO ACPI device
  UINT8    RstPcieStorageInterfaceType[3];          ///< Offset 598     RST PCIe Storage Cycle Router#1 Interface Type
                                                    ///< Offset 599     RST PCIe Storage Cycle Router#2 Interface Type
                                                    ///< Offset 600     RST PCIe Storage Cycle Router#3 Interface Type
  UINT8    RstPcieStoragePmCapPtr[3];               ///< Offset 601     RST PCIe Storage Cycle Router#1 Power Management Capability Pointer
                                                    ///< Offset 602     RST PCIe Storage Cycle Router#2 Power Management Capability Pointer
                                                    ///< Offset 603     RST PCIe Storage Cycle Router#3 Power Management Capability Pointer
  UINT8    RstPcieStoragePcieCapPtr[3];             ///< Offset 604     RST PCIe Storage Cycle Router#1 PCIe Capabilities Pointer
                                                    ///< Offset 605     RST PCIe Storage Cycle Router#2 PCIe Capabilities Pointer
                                                    ///< Offset 606     RST PCIe Storage Cycle Router#3 PCIe Capabilities Pointer
  UINT16   RstPcieStorageL1ssCapPtr[3];             ///< Offset 607     RST PCIe Storage Cycle Router#1 L1SS Capability Pointer
                                                    ///< Offset 609     RST PCIe Storage Cycle Router#2 L1SS Capability Pointer
                                                    ///< Offset 611     RST PCIe Storage Cycle Router#3 L1SS Capability Pointer
  UINT8    RstPcieStorageEpL1ssControl2[3];         ///< Offset 613     RST PCIe Storage Cycle Router#1 Endpoint L1SS Control Data2
                                                    ///< Offset 614     RST PCIe Storage Cycle Router#2 Endpoint L1SS Control Data2
                                                    ///< Offset 615     RST PCIe Storage Cycle Router#3 Endpoint L1SS Control Data2
  UINT32   RstPcieStorageEpL1ssControl1[3];         ///< Offset 616     RST PCIe Storage Cycle Router#1 Endpoint L1SS Control Data1
                                                    ///< Offset 620     RST PCIe Storage Cycle Router#2 Endpoint L1SS Control Data1
                                                    ///< Offset 624     RST PCIe Storage Cycle Router#3 Endpoint L1SS Control Data1
  UINT16   RstPcieStorageLtrCapPtr[3];              ///< Offset 628     RST PCIe Storage Cycle Router#1 LTR Capability Pointer
                                                    ///< Offset 630     RST PCIe Storage Cycle Router#2 LTR Capability Pointer
                                                    ///< Offset 632     RST PCIe Storage Cycle Router#3 LTR Capability Pointer
  UINT32   RstPcieStorageEpLtrData[3];              ///< Offset 634     RST PCIe Storage Cycle Router#1 Endpoint LTR Data
                                                    ///< Offset 638     RST PCIe Storage Cycle Router#2 Endpoint LTR Data
                                                    ///< Offset 642     RST PCIe Storage Cycle Router#3 Endpoint LTR Data
  UINT16   RstPcieStorageEpLctlData16[3];           ///< Offset 646     RST PCIe Storage Cycle Router#1 Endpoint LCTL Data
                                                    ///< Offset 648     RST PCIe Storage Cycle Router#2 Endpoint LCTL Data
                                                    ///< Offset 650     RST PCIe Storage Cycle Router#3 Endpoint LCTL Data
  UINT16   RstPcieStorageEpDctlData16[3];           ///< Offset 652     RST PCIe Storage Cycle Router#1 Endpoint DCTL Data
                                                    ///< Offset 654     RST PCIe Storage Cycle Router#2 Endpoint DCTL Data
                                                    ///< Offset 656     RST PCIe Storage Cycle Router#3 Endpoint DCTL Data
  UINT16   RstPcieStorageEpDctl2Data16[3];          ///< Offset 658     RST PCIe Storage Cycle Router#1 Endpoint DCTL2 Data
                                                    ///< Offset 660     RST PCIe Storage Cycle Router#2 Endpoint DCTL2 Data
                                                    ///< Offset 662     RST PCIe Storage Cycle Router#3 Endpoint DCTL2 Data
  UINT16   RstPcieStorageRpDctl2Data16[3];          ///< Offset 664     RST PCIe Storage Cycle Router#1 RootPort DCTL2 Data
                                                    ///< Offset 666     RST PCIe Storage Cycle Router#2 RootPort DCTL2 Data
                                                    ///< Offset 668     RST PCIe Storage Cycle Router#3 RootPort DCTL2 Data
  UINT32   RstPcieStorageUniqueTableBar[3];         ///< Offset 670     RST PCIe Storage Cycle Router#1 Endpoint unique MSI-X Table BAR
                                                    ///< Offset 674     RST PCIe Storage Cycle Router#2 Endpoint unique MSI-X Table BAR
                                                    ///< Offset 678     RST PCIe Storage Cycle Router#3 Endpoint unique MSI-X Table BAR
  UINT32   RstPcieStorageUniqueTableBarValue[3];    ///< Offset 682     RST PCIe Storage Cycle Router#1 Endpoint unique MSI-X Table BAR value
                                                    ///< Offset 686     RST PCIe Storage Cycle Router#2 Endpoint unique MSI-X Table BAR value
                                                    ///< Offset 690     RST PCIe Storage Cycle Router#3 Endpoint unique MSI-X Table BAR value
  UINT32   RstPcieStorageUniquePbaBar[3];           ///< Offset 694     RST PCIe Storage Cycle Router#1 Endpoint unique MSI-X PBA BAR
                                                    ///< Offset 698     RST PCIe Storage Cycle Router#2 Endpoint unique MSI-X PBA BAR
                                                    ///< Offset 702     RST PCIe Storage Cycle Router#3 Endpoint unique MSI-X PBA BAR
  UINT32   RstPcieStorageUniquePbaBarValue[3];      ///< Offset 706     RST PCIe Storage Cycle Router#1 Endpoint unique MSI-X PBA BAR value
                                                    ///< Offset 710     RST PCIe Storage Cycle Router#2 Endpoint unique MSI-X PBA BAR value
                                                    ///< Offset 714     RST PCIe Storage Cycle Router#3 Endpoint unique MSI-X PBA BAR value
  UINT32   RstPcieStorageRootPortNum[3];            ///< Offset 718     RST PCIe Storage Cycle Router#1 Root Port number
                                                    ///< Offset 722     RST PCIe Storage Cycle Router#2 Root Port number
                                                    ///< Offset 726     RST PCIe Storage Cycle Router#3 Root Port number
  UINT8    EMH4;                                    ///< Offset 730     eMMC HS400 mode enabled
  UINT8    EMDS;                                    ///< Offset 731     eMMC Driver Strength
  UINT8    CpuSku;                                  ///< Offset 732     CPU SKU
  UINT16   IoTrapAddress[4];                        ///< Offset 733
  UINT8    IoTrapStatus[4];                         ///< Offset 741
  UINT16   PMBS;                                    ///< Offset 745     ACPI IO BASE address
  UINT64   PWRM;                                    ///< Offset 747     PWRM MEM BASE address
  // CNVi specific
  UINT8    CnviCrfPresent;                          ///< Offset 755     CNVi CRF Present
  UINT16   CnviPortId;                              ///< Offset 756     CNVi Sideband Port ID
  UINT8    CnviWifiCore;                            ///< Offset 758     CNVi WiFi Core
  UINT8    CnviBtCore;                              ///< Offset 759     CNVi BT Core
  UINT8    CnviBtInterface;                         ///< Offset 760     CNVi BT Interface
  UINT8    CnviBtAudioOffload;                      ///< Offset 761     CNVi BT Audio Offload
  UINT32   CnviBtEnableGpio;                        ///< Offset 762     CNVi BT Enable (CNV_BTEN) pin
  UINT8    CnviVPro;                                ///< Offset 766     CNVi vPro
  UINT8    CnviDdrRfim;                             ///< Offset 767     CNVi DDR RFI Mitigation
  // TraceHub
  UINT8    TraceVerbosity;                          ///< Offset 768     BIOS trace verbosity
  UINT32   TraceHubFwBase;                          ///< Offset 769     BIOS Trace Hub FW base address
  // PCH PS_ON support
  UINT8    PsOnEnable;                              ///< Offset 773     PCH PS_ON enable
  //
  // These are for PchApciTablesSelfTest use
  //
  UINT8    LtrEnable[28];                           ///< Offset 774     Latency Tolerance Reporting Enable
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
                                                    ///< Offset 793     Latency Tolerance Reporting Enable
                                                    ///< Offset 794     Latency Tolerance Reporting Enable
                                                    ///< Offset 795     Latency Tolerance Reporting Enable
                                                    ///< Offset 796     Latency Tolerance Reporting Enable
                                                    ///< Offset 797     Latency Tolerance Reporting Enable
                                                    ///< Offset 798     Latency Tolerance Reporting Enable
                                                    ///< Offset 799     Latency Tolerance Reporting Enable
                                                    ///< Offset 800     Latency Tolerance Reporting Enable
                                                    ///< Offset 801     Latency Tolerance Reporting Enable
  UINT8    GBES;                                    ///< Offset 802     GbE Support
  UINT32   PchxDCIPwrDnScale;                       ///< Offset 803     PCH xDCI Power Down Scale Value, DWC_USB3_GCTL_INIT[31:19]
  UINT8    EmmcEnabled;                             ///< Offset 807     Set to indicate that eMMC is enabled
  UINT8    SdCardEnabled;                           ///< Offset 808     Set to indicate that SD card is enabled
  UINT8    EnableTimedGpio0;                        ///< Offset 809     Set to indicate that Timed GPIO 0 is enabled
  UINT8    EnableTimedGpio1;                        ///< Offset 810     Set to indicate that Timed GPIO 1 is enabled
  UINT8    ClockToRootPortMap[18];                  ///< Offset 811     CLOCK index to root port index map. Used during PCIe D3Cold flows
  UINT16   TcoBase;                                 ///< Offset 829     TCO base address
  UINT16   IclkPid;                                 ///< Offset 831     Iclk PID number
  UINT8    PchUsb2PortCount;                        ///< Offset 833     Number of USB2 ports in PCH
  UINT8    PchUsb3PortCount;                        ///< Offset 834     Number of USB3 ports in PCH
  UINT8    SataPortPresence;                        ///< Offset 835     Holds information from SATA PCS register about SATA ports which recieved COMINIT from connected devices.
  UINT8    ClkreqIpcCmdSupported;                   ///< Offset 836     PCIE CLKREQ IPC command support
  UINT8    ClockReq[18];                            ///< Offset 837     CLOCK Source index to ClkReq Number. Used during PCIe D3Cold flows
  UINT32   ThcWakOnTouchInterrupt[2];               ///< Offset 855     Touch Host Controller Wake On Touch Interrupt Number - when 0 wake is disabled
  UINT16   ThcMode[2];                              ///< Offset 863     Touch Host Controller Mode THC or HID over SPI
  UINT8    Ufs0Enabled;                             ///< Offset 867     Is UFS0 Enabled
  UINT8    Ufs1Enabled;                             ///< Offset 868     Is UFS1 Enabled
  UINT8    UaolEnabled;                             ///< Offset 869     Is USB Audio Offload enabled
  UINT32   ThcResetPad[2];                          ///< Offset 870     Touch Host Controller Reset Pad
  UINT8    ThcResetPadTrigger[2];                   ///< Offset 878     Touch Host Controller Reset Pad Trigger
  UINT32   ThcHidSpiConnectionSpeed[2];             ///< Offset 880     Touch Host Controller HID over SPI Connection Speed
  UINT32   ThcHidSpiInputReportHeaderAddress[2];    ///< Offset 888     Touch Host Controller HID over SPI Input Report Header Address
  UINT32   ThcHidSpiInputReportBodyAddress[2];      ///< Offset 896     Touch Host Controller HID over SPI Input Report Body Address
  UINT32   ThcHidSpiOutputReportAddress[2];         ///< Offset 904     Touch Host Controller HID over SPI Output Report Address
  UINT32   ThcHidSpiReadOpcode[2];                  ///< Offset 912     Touch Host Controller HID over SPI Read Opcode
  UINT32   ThcHidSpiWriteOpcode[2];                 ///< Offset 920     Touch Host Controller HID over SPI Write Opcode
  UINT32   ThcHidSpiFlags[2];                       ///< Offset 928     Touch Host Controller HID over SPI Flags
  UINT32   ThcActiveLtr[2];                         ///< Offset 936     Touch Host Controller Active Ltr
  UINT32   ThcIdleLtr[2];                           ///< Offset 944     Touch Host Controller Idle Ltr
  UINT32   ThcHidSpiLimitPacketSize[2];             ///< Offset 952     Touch Host Controller Limit Packet Size
  UINT32   ThcPerformanceLimitation[2];             ///< Offset 960     Touch Host Controller Performance Limitation
  UINT32   ThcDisplayFrameSyncPeriod[2];            ///< Offset 968     Touch Host Controller Display Frame Sync Period
  UINT32   ThcResetSequencingDelay[2];              ///< Offset 976     Touch Host Controller Reset Sequencing Delay
  UINT16   ThcHidI2cDeviceAddress[2];               ///< Offset 984     Touch Host Controller HID over I2C Descriptor Address
  UINT32   ThcHidI2cConnectionSpeed[2];             ///< Offset 988     Touch Host Controller HID over I2C Connection Speed
  UINT8    ThcHidI2cAddressingMode[2];              ///< Offset 996     Touch Host Controller HID over I2C Addressing Mode
  UINT64   ThcHidI2cStandardModeSerialClockLineHighPeriod[2]; ///< Offset 998     Touch Host Controller HID over I2C Serial Clock Line High Period
  UINT64   ThcHidI2cStandardModeSerialClockLineLowPeriod[2]; ///< Offset 1014    Touch Host Controller HID over I2C Standard Mode Serial Clock Line Low Period
  UINT64   ThcHidI2cStandardModeSerialDataLineTransmitHoldPeriod[2]; ///< Offset 1030    Touch Host Controller HID over I2C Standard Mode Serial Data Line Transmit Hold Period
  UINT64   ThcHidI2cStandardModeSerialDataLineReceiveHoldPeriod[2]; ///< Offset 1046    Touch Host Controller HID over I2C Standard Mode Serial Data Receive Hold Period
  UINT64   ThcHidI2cFastModeSerialClockLineHighPeriod[2]; ///< Offset 1062    Touch Host Controller HID over I2C Fast Mode Serial Clock Line High Period
  UINT64   ThcHidI2cFastModeSerialClockLineLowPeriod[2]; ///< Offset 1078    Touch Host Controller HID over I2C Fast Mode Serial Clock Line Low Period
  UINT64   ThcHidI2cFastModeSerialDataLineTransmitHoldPeriod[2]; ///< Offset 1094    Touch Host Controller HID over I2C Fast Mode Serial Data Line Transmit Hold Period
  UINT64   ThcHidI2cFastModeSerialDataLineReceiveHoldPeriod[2]; ///< Offset 1110    Touch Host Controller HID over I2C Fast Mode Serial Data Line Receive Hold Period
  UINT64   ThcHidI2cMaxSuppressedSpikesSMFMFMP[2];  ///< Offset 1126    Touch Host Controller HID over I2C Maximum Length Of Suppressed Spikes In Std Mode Fast Mode And Fast Mode Plus
  UINT64   ThcHidI2cFastModePlusSerialClockLineHighPeriod[2]; ///< Offset 1142    Touch Host Controller HID over I2C Fast Mode Plus Serial Clock Line High Period
  UINT64   ThcHidI2cFastModePlusSerialClockLineLowPeriod[2]; ///< Offset 1158    Touch Host Controller HID over I2C Fast Mode Plus Serial Clock Line Low Period
  UINT64   ThcHidI2cFastModePlusSerialDataLineTransmitHoldPeriod[2]; ///< Offset 1174    Touch Host Controller HID over I2C Fast Mode Plus Serial Data Line Transmit Hold Period
  UINT64   ThcHidI2cFastModePlusSerialDataLineReceiveHoldPeriod[2]; ///< Offset 1190    Touch Host Controller HID over I2C Fast Mode Plus Serial Data Line Receive Hold Period
  UINT64   ThcHidI2cHighSpeedModePlusSerialClockLineHighPeriod[2]; ///< Offset 1206    Touch Host Controller HID over I2C High Speed Mode Plus Serial Clock Line High Period
  UINT64   ThcHidI2cHighSpeedModePlusSerialClockLineLowPeriod[2]; ///< Offset 1222    Touch Host Controller HID over I2C High Speed Mode Plus Serial Clock Line Low Period
  UINT64   ThcHidI2cHighSpeedModePlusSerialDataLineTransmitHoldPeriod[2]; ///< Offset 1238    Touch Host Controller HID over I2C High Speed Mode Plus Serial Data Line Transmit Hold Period
  UINT64   ThcHidI2cHighSpeedModePlusSerialDataLineReceiveHoldPeriod[2]; ///< Offset 1254    Touch Host Controller HID over I2C High Speed Mode Plus Serial Data Line Receive Hold Period
  UINT64   ThcHidI2cMaximumLengthOfSuppressedSpikesInHighSpeedMode[2]; ///< Offset 1270    Touch Host Controller HID over I2C Maximum Length Of Suppressed Spikes In High Speed Mode
  UINT8    CppmFaEn;                                ///< Offset 1286    CPPM Forced Alignment Enable
  UINT8    PchAttached;                             ///< Offset 1287    PCH Attached
} PCH_NVS_AREA;


//
// Global NVS Area Protocol
//
typedef struct {
  PCH_NVS_AREA        PchNvs;
  BIOS_ACPI_PARAM     BiosAcpiParam;
} GLOBAL_NVS_AREA;

#pragma pack(pop)

#endif
