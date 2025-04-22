
/**@file


  Copyright (c) 2021, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
  //
  // Define CPU NVS Area operation region.
  //
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
  UINT8    HwpVersion;                              ///< Offset 57      HWP Version
  UINT8    Reserved3[27];                           ///< Offset 58:84
  UINT8    LowestMaxPerf;                           ///< Offset 85      Max ratio of the slowest core.
  UINT8    EnableItbm;                              ///< Offset 86      Enable/Disable Intel Turbo Boost Max Technology 3.0.
  UINT8    OcBins;                                  ///< Offset 87      Indicates bins of Oc support. MSR 194h FLEX_RATIO Bits (19:17)
  UINT8    TxtEnabled;                              ///< Offset 88      Indicates whether TXT is enabled or not based on MSR 0x3A
  UINT8    HwpEnable;                               ///< Offset 89      HWP Enable
  UINT8    HeteroStatus;                            ///< Offset 90      Indicates whether Hetero is supported by SoC
  UINT8    CoreModule[64];                          ///< Offset 91      Indicates the number of cores in a module and module ID of Processor 0. 07:4 - Number of cores in a module. 3:0 - ModuleID
                                                    ///< Offset 92      Indicates the number of cores in a module and module ID of Processor 1. 07:4 - Number of cores in a module. 3:0 - ModuleID
                                                    ///< Offset 93      Indicates the number of cores in a module and module ID of Processor 2. 07:4 - Number of cores in a module. 3:0 - ModuleID
                                                    ///< Offset 94      Indicates the number of cores in a module and module ID of Processor 3. 07:4 - Number of cores in a module. 3:0 - ModuleID
                                                    ///< Offset 95      Indicates the number of cores in a module and module ID of Processor 4. 07:4 - Number of cores in a module. 3:0 - ModuleID
                                                    ///< Offset 96      Indicates the number of cores in a module and module ID of Processor 5. 07:4 - Number of cores in a module. 3:0 - ModuleID
                                                    ///< Offset 97      Indicates the number of cores in a module and module ID of Processor 6. 07:4 - Number of cores in a module. 3:0 - ModuleID
                                                    ///< Offset 98      Indicates the number of cores in a module and module ID of Processor 7. 07:4 - Number of cores in a module. 3:0 - ModuleID
                                                    ///< Offset 99      Indicates the number of cores in a module and module ID of Processor 8. 07:4 - Number of cores in a module. 3:0 - ModuleID
                                                    ///< Offset 100     Indicates the number of cores in a module and module ID of Processor 9. 07:4 - Number of cores in a module. 3:0 - ModuleID
                                                    ///< Offset 101     Indicates the number of cores in a module and module ID of Processor 10. 07:4 - Number of cores in a module. 3:0 - ModuleID
                                                    ///< Offset 102     Indicates the number of cores in a module and module ID of Processor 11. 07:4 - Number of cores in a module. 3:0 - ModuleID
                                                    ///< Offset 103     Indicates the number of cores in a module and module ID of Processor 12. 07:4 - Number of cores in a module. 3:0 - ModuleID
                                                    ///< Offset 104     Indicates the number of cores in a module and module ID of Processor 13. 07:4 - Number of cores in a module. 3:0 - ModuleID
                                                    ///< Offset 105     Indicates the number of cores in a module and module ID of Processor 14. 07:4 - Number of cores in a module. 3:0 - ModuleID
                                                    ///< Offset 106     Indicates the number of cores in a module and module ID of Processor 15. 07:4 - Number of cores in a module. 3:0 - ModuleID
                                                    ///< Offset 107     Indicates the number of cores in a module and module ID of Processor 16. 07:4 - Number of cores in a module. 3:0 - ModuleID
                                                    ///< Offset 108     Indicates the number of cores in a module and module ID of Processor 17. 07:4 - Number of cores in a module. 3:0 - ModuleID
                                                    ///< Offset 109     Indicates the number of cores in a module and module ID of Processor 18. 07:4 - Number of cores in a module. 3:0 - ModuleID
                                                    ///< Offset 110     Indicates the number of cores in a module and module ID of Processor 19. 07:4 - Number of cores in a module. 3:0 - ModuleID
                                                    ///< Offset 111     Indicates the number of cores in a module and module ID of Processor 20. 07:4 - Number of cores in a module. 3:0 - ModuleID
                                                    ///< Offset 112     Indicates the number of cores in a module and module ID of Processor 21. 07:4 - Number of cores in a module. 3:0 - ModuleID
                                                    ///< Offset 113     Indicates the number of cores in a module and module ID of Processor 22. 07:4 - Number of cores in a module. 3:0 - ModuleID
                                                    ///< Offset 114     Indicates the number of cores in a module and module ID of Processor 23. 07:4 - Number of cores in a module. 3:0 - ModuleID
                                                    ///< Offset 115     Indicates the number of cores in a module and module ID of Processor 24. 07:4 - Number of cores in a module. 3:0 - ModuleID
                                                    ///< Offset 116     Indicates the number of cores in a module and module ID of Processor 25. 07:4 - Number of cores in a module. 3:0 - ModuleID
                                                    ///< Offset 117     Indicates the number of cores in a module and module ID of Processor 26. 07:4 - Number of cores in a module. 3:0 - ModuleID
                                                    ///< Offset 118     Indicates the number of cores in a module and module ID of Processor 27. 07:4 - Number of cores in a module. 3:0 - ModuleID
                                                    ///< Offset 119     Indicates the number of cores in a module and module ID of Processor 28. 07:4 - Number of cores in a module. 3:0 - ModuleID
                                                    ///< Offset 120     Indicates the number of cores in a module and module ID of Processor 29. 07:4 - Number of cores in a module. 3:0 - ModuleID
                                                    ///< Offset 121     Indicates the number of cores in a module and module ID of Processor 30. 07:4 - Number of cores in a module. 3:0 - ModuleID
                                                    ///< Offset 122     Indicates the number of cores in a module and module ID of Processor 31. 07:4 - Number of cores in a module. 3:0 - ModuleID
                                                    ///< Offset 123     Indicates the number of cores in a module and module ID of Processor 32. 07:4 - Number of cores in a module. 3:0 - ModuleID
                                                    ///< Offset 124     Indicates the number of cores in a module and module ID of Processor 33. 07:4 - Number of cores in a module. 3:0 - ModuleID
                                                    ///< Offset 125     Indicates the number of cores in a module and module ID of Processor 34. 07:4 - Number of cores in a module. 3:0 - ModuleID
                                                    ///< Offset 126     Indicates the number of cores in a module and module ID of Processor 35. 07:4 - Number of cores in a module. 3:0 - ModuleID
                                                    ///< Offset 127     Indicates the number of cores in a module and module ID of Processor 36. 07:4 - Number of cores in a module. 3:0 - ModuleID
                                                    ///< Offset 128     Indicates the number of cores in a module and module ID of Processor 37. 07:4 - Number of cores in a module. 3:0 - ModuleID
                                                    ///< Offset 129     Indicates the number of cores in a module and module ID of Processor 38. 07:4 - Number of cores in a module. 3:0 - ModuleID
                                                    ///< Offset 130     Indicates the number of cores in a module and module ID of Processor 39. 07:4 - Number of cores in a module. 3:0 - ModuleID
                                                    ///< Offset 131     Indicates the number of cores in a module and module ID of Processor 40. 07:4 - Number of cores in a module. 3:0 - ModuleID
                                                    ///< Offset 132     Indicates the number of cores in a module and module ID of Processor 41. 07:4 - Number of cores in a module. 3:0 - ModuleID
                                                    ///< Offset 133     Indicates the number of cores in a module and module ID of Processor 42. 07:4 - Number of cores in a module. 3:0 - ModuleID
                                                    ///< Offset 134     Indicates the number of cores in a module and module ID of Processor 43. 07:4 - Number of cores in a module. 3:0 - ModuleID
                                                    ///< Offset 135     Indicates the number of cores in a module and module ID of Processor 44. 07:4 - Number of cores in a module. 3:0 - ModuleID
                                                    ///< Offset 136     Indicates the number of cores in a module and module ID of Processor 45. 07:4 - Number of cores in a module. 3:0 - ModuleID
                                                    ///< Offset 137     Indicates the number of cores in a module and module ID of Processor 46. 07:4 - Number of cores in a module. 3:0 - ModuleID
                                                    ///< Offset 138     Indicates the number of cores in a module and module ID of Processor 47. 07:4 - Number of cores in a module. 3:0 - ModuleID
                                                    ///< Offset 139     Indicates the number of cores in a module and module ID of Processor 48. 07:4 - Number of cores in a module. 3:0 - ModuleID
                                                    ///< Offset 140     Indicates the number of cores in a module and module ID of Processor 49. 07:4 - Number of cores in a module. 3:0 - ModuleID
                                                    ///< Offset 141     Indicates the number of cores in a module and module ID of Processor 50. 07:4 - Number of cores in a module. 3:0 - ModuleID
                                                    ///< Offset 142     Indicates the number of cores in a module and module ID of Processor 51. 07:4 - Number of cores in a module. 3:0 - ModuleID
                                                    ///< Offset 143     Indicates the number of cores in a module and module ID of Processor 52. 07:4 - Number of cores in a module. 3:0 - ModuleID
                                                    ///< Offset 144     Indicates the number of cores in a module and module ID of Processor 53. 07:4 - Number of cores in a module. 3:0 - ModuleID
                                                    ///< Offset 145     Indicates the number of cores in a module and module ID of Processor 54. 07:4 - Number of cores in a module. 3:0 - ModuleID
                                                    ///< Offset 146     Indicates the number of cores in a module and module ID of Processor 55. 07:4 - Number of cores in a module. 3:0 - ModuleID
                                                    ///< Offset 147     Indicates the number of cores in a module and module ID of Processor 56. 07:4 - Number of cores in a module. 3:0 - ModuleID
                                                    ///< Offset 148     Indicates the number of cores in a module and module ID of Processor 57. 07:4 - Number of cores in a module. 3:0 - ModuleID
                                                    ///< Offset 149     Indicates the number of cores in a module and module ID of Processor 58. 07:4 - Number of cores in a module. 3:0 - ModuleID
                                                    ///< Offset 150     Indicates the number of cores in a module and module ID of Processor 59. 07:4 - Number of cores in a module. 3:0 - ModuleID
                                                    ///< Offset 151     Indicates the number of cores in a module and module ID of Processor 60. 07:4 - Number of cores in a module. 3:0 - ModuleID
                                                    ///< Offset 152     Indicates the number of cores in a module and module ID of Processor 61. 07:4 - Number of cores in a module. 3:0 - ModuleID
                                                    ///< Offset 153     Indicates the number of cores in a module and module ID of Processor 62. 07:4 - Number of cores in a module. 3:0 - ModuleID
                                                    ///< Offset 154     Indicates the number of cores in a module and module ID of Processor 63. 07:4 - Number of cores in a module. 3:0 - ModuleID
  UINT32   CoreType;                                ///< Offset 155     Each bit indicates the core type
  UINT16   ScalingFactorSmallCore;                  ///< Offset 159     Indicates the Scaling Factor for Small Core
  UINT16   ScalingFactorBigCore;                    ///< Offset 161     Indicates the Scaling Factor for Big Core
  UINT16   NominalFrequency;                        ///< Offset 163     Nominal Frequency
  UINT8    IsNominalFrequencySupported;             ///< Offset 165     Indicates if Nominal Frequency is supported
} CPU_NVS_AREA;

#pragma pack(pop)
#endif
