
/**@file

  Copyright (c) 2017 - 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/


#ifndef _CPU_NVS_AREA_DEF_H_
#define _CPU_NVS_AREA_DEF_H_

#pragma pack (push,1)
typedef struct {
  UINT8    Revision;                                ///< Offset 0       CPU GlobalNvs Revision
  UINT32   PpmFlags;                                ///< Offset 1       PPM Flags Values
  UINT8    Reserved0[1];                            ///< Offset 5:5
  UINT8    AutoCriticalTripPoint;                   ///< Offset 6       Auto Critical Trip Point
  UINT8    AutoPassiveTripPoint;                    ///< Offset 7       Auto Passive Trip Point
  UINT8    AutoActiveTripPoint;                     ///< Offset 8       Auto Active Trip Point
  UINT32   Cpuid;                                   ///< Offset 9       CPUID
  UINT8    ConfigurablePpc;                         ///< Offset 13      Boot Mode vlues for _PPC
  UINT8    CtdpLevelsSupported;                     ///< Offset 14      ConfigTdp Number Of Levels
  UINT8    ConfigTdpBootModeIndex;                  ///< Offset 15      CTDP Boot Mode Index
  UINT16   CtdpPowerLimit1[3];                      ///< Offset 16      CTDP Level 0 Power Limit1
                                                    ///< Offset 18      CTDP Level 1 Power Limit1
                                                    ///< Offset 20      CTDP Level 2 Power Limit1
  UINT16   CtdpPowerLimit2[3];                      ///< Offset 22      CTDP Level 0 Power Limit2
                                                    ///< Offset 24      CTDP Level 1 Power Limit2
                                                    ///< Offset 26      CTDP Level 2 Power Limit2
  UINT8    CtdpPowerLimitWindow[3];                 ///< Offset 28      CTDP Level 0 Power Limit1 Time Window
                                                    ///< Offset 29      CTDP Level 1 Power Limit1 Time Window
                                                    ///< Offset 30      CTDP Level 2 Power Limit1 Time Window
  UINT8    CtdpCtc[3];                              ///< Offset 31      CTDP Level 0 CTC
                                                    ///< Offset 32      CTDP Level 1 CTC
                                                    ///< Offset 33      CTDP Level 2 CTC
  UINT8    CtdpTar[3];                              ///< Offset 34      CTDP Level 0 TAR
                                                    ///< Offset 35      CTDP Level 1 TAR
                                                    ///< Offset 36      CTDP Level 2 TAR
  UINT8    CtdpPpc[3];                              ///< Offset 37      CTDP Level 0 PPC
                                                    ///< Offset 38      CTDP Level 1 PPC
                                                    ///< Offset 39      CTDP Level 2 PPC
  UINT8    Reserved1[1];                            ///< Offset 40:40
  UINT8    C6MwaitValue;                            ///< Offset 41      Mwait Hint value for C6
  UINT8    C7MwaitValue;                            ///< Offset 42      Mwait Hint value for C7/C7s
  UINT8    CDMwaitValue;                            ///< Offset 43      Mwait Hint value for C7/C8/C9/C10
  UINT8    Reserved2[2];                            ///< Offset 44:45
  UINT16   C6Latency;                               ///< Offset 46      Latency Value for C6
  UINT16   C7Latency;                               ///< Offset 48      Latency Value for C7/C7S
  UINT16   CDLatency;                               ///< Offset 50      Latency Value for C8/C9/C10
  UINT16   CDIOLevel;                               ///< Offset 52      IO LVL value for C8/C9/C10
  UINT16   CDPowerValue;                            ///< Offset 54      Power value for C8/C9/C10
  UINT8    MiscPowerManagementFlags;                ///< Offset 56      MiscPowerManagementFlags
  UINT8    EnableDigitalThermalSensor;              ///< Offset 57      Digital Thermal Sensor Enable
  UINT8    DigitalThermalSensorSmiFunction;         ///< Offset 58      DTS SMI Function Call via DTS IO Trap
  UINT8    PackageDTSTemperature;                   ///< Offset 59      Package Temperature or Max Core temperature.
  UINT8    IsPackageTempMSRAvailable;               ///< Offset 60      Package Temperature MSR available
  UINT16   DtsIoTrapAddress;                        ///< Offset 61      DTS IO trap Address
  UINT8    DtsIoTrapLength;                         ///< Offset 63      DTS IO trap Length
  UINT8    DtsAcpiEnable;                           ///< Offset 64      DTS is in ACPI Mode Enabled
  UINT8    SgxStatus;                               ///< Offset 65      SGX Status
  UINT64   EpcBaseAddress;                          ///< Offset 66      EPC Base Address
  UINT64   EpcLength;                               ///< Offset 74      EPC Length
  UINT8    HwpVersion;                              ///< Offset 82      HWP Version
  UINT8    HwpInterruptStatus;                      ///< Offset 83      HWP Interrupt Status
  UINT8    DtsInterruptStatus;                      ///< Offset 84      DTS Interrupt Status
  UINT8    LowestMaxPerf;                           ///< Offset 85      Max ratio of the slowest core.
  UINT8    EnableItbm;                              ///< Offset 86      Enable/Disable Intel Turbo Boost Max Technology 3.0.
  UINT8    EnableItbmDriver;                        ///< Offset 87      Enable/Disable Intel Turbo Boost Max Technology 3.0 Driver.
  UINT8    ItbmInterruptStatus;                     ///< Offset 88      Intel Turbo Boost Max Technology 3.0 interrupt status.
  UINT8    ItbmSmi;                                 ///< Offset 89      SMI to resume periodic SMM for Intel Turbo Boost Max Technology 3.0.
  UINT8    OcBins;                                  ///< Offset 90      Indicates bins of Oc support. MSR 194h FLEX_RATIO Bits (19:17)
  UINT8    TxtEnabled;                              ///< Offset 91      Indicates whether TXT is enabled or not based on MSR 0x3A
  UINT8    HwpEnable;                               ///< Offset 92      HWP Enable
  UINT8    HeteroStatus;                            ///< Offset 93      Indicates whether Hetero is supported by SoC
  UINT32   CoreType;                                ///< Offset 94      Each bit indicates the core type
  UINT8    ModuleId;                                ///< Offset 98      Indicates the Module Id of Small Core
  UINT8    SmallCoreCount;                          ///< Offset 99      Indicates the total number of Small Cores
  UINT8    BigCoreCount;                            ///< Offset 100     Indicates the total number of Big Cores
} CPU_NVS_AREA;

#pragma pack(pop)
#endif
