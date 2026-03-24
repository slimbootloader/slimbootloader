/**@file

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
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
  UINT16   CtdpPowerLimit1;                         ///< Offset 16      CTDP selected Level Power Limit1
  UINT16   CtdpPowerLimit2;                         ///< Offset 18      CTDP selected Level Power Limit2
  UINT8    CtdpPowerLimitWindow;                    ///< Offset 20      CTDP selected Level Power Limit1 Time Window
  UINT8    C6MwaitValue;                            ///< Offset 21      Mwait Hint value for C6
  UINT8    C7MwaitValue;                            ///< Offset 22      Mwait Hint value for C7/C7s
  UINT8    CDMwaitValue;                            ///< Offset 23      Mwait Hint value for C7/C8/C9/C10
  UINT8    Reserved1[2];                            ///< Offset 24:25
  UINT16   C6Latency;                               ///< Offset 26      Latency Value for C6
  UINT16   C7Latency;                               ///< Offset 28      Latency Value for C7/C7S
  UINT16   CDLatency;                               ///< Offset 30      Latency Value for C8/C9/C10
  UINT16   CDPowerValue;                            ///< Offset 32      Power value for C8/C9/C10
  UINT8    MiscPowerManagementFlags;                ///< Offset 34      MiscPowerManagementFlags
  UINT8    HwpVersion;                              ///< Offset 35      HWP Version
  UINT8    Reserved2[31];                           ///< Offset 36:66
  UINT8    OcBins;                                  ///< Offset 67      Indicates bins of Oc support. MSR 194h FLEX_RATIO Bits (19:17)
  UINT8    HwpEnable;                               ///< Offset 68      HWP Enable
  UINT8    HeteroStatus;                            ///< Offset 69      Indicates whether Hetero is supported by SoC
  UINT8    DomainIndex[64];                         ///< Offset 70      Indicates the index of Domain which contains core 0
                                                    ///< Offset 71      Indicates the index of Domain which contains core 1
                                                    ///< Offset 72      Indicates the index of Domain which contains core 2
                                                    ///< Offset 73      Indicates the index of Domain which contains core 3
                                                    ///< Offset 74      Indicates the index of Domain which contains core 4
                                                    ///< Offset 75      Indicates the index of Domain which contains core 5
                                                    ///< Offset 76      Indicates the index of Domain which contains core 6
                                                    ///< Offset 77      Indicates the index of Domain which contains core 7
                                                    ///< Offset 78      Indicates the index of Domain which contains core 8
                                                    ///< Offset 79      Indicates the index of Domain which contains core 9
                                                    ///< Offset 80      Indicates the index of Domain which contains core 10
                                                    ///< Offset 81      Indicates the index of Domain which contains core 11
                                                    ///< Offset 82      Indicates the index of Domain which contains core 12
                                                    ///< Offset 83      Indicates the index of Domain which contains core 13
                                                    ///< Offset 84      Indicates the index of Domain which contains core 14
                                                    ///< Offset 85      Indicates the index of Domain which contains core 15
                                                    ///< Offset 86      Indicates the index of Domain which contains core 16
                                                    ///< Offset 87      Indicates the index of Domain which contains core 17
                                                    ///< Offset 88      Indicates the index of Domain which contains core 18
                                                    ///< Offset 89      Indicates the index of Domain which contains core 19
                                                    ///< Offset 90      Indicates the index of Domain which contains core 20
                                                    ///< Offset 91      Indicates the index of Domain which contains core 21
                                                    ///< Offset 92      Indicates the index of Domain which contains core 22
                                                    ///< Offset 93      Indicates the index of Domain which contains core 23
                                                    ///< Offset 94      Indicates the index of Domain which contains core 24
                                                    ///< Offset 95      Indicates the index of Domain which contains core 25
                                                    ///< Offset 96      Indicates the index of Domain which contains core 26
                                                    ///< Offset 97      Indicates the index of Domain which contains core 27
                                                    ///< Offset 98      Indicates the index of Domain which contains core 28
                                                    ///< Offset 99      Indicates the index of Domain which contains core 29
                                                    ///< Offset 100     Indicates the index of Domain which contains core 30
                                                    ///< Offset 101     Indicates the index of Domain which contains core 31
                                                    ///< Offset 102     Indicates the index of Domain which contains core 32
                                                    ///< Offset 103     Indicates the index of Domain which contains core 33
                                                    ///< Offset 104     Indicates the index of Domain which contains core 34
                                                    ///< Offset 105     Indicates the index of Domain which contains core 35
                                                    ///< Offset 106     Indicates the index of Domain which contains core 36
                                                    ///< Offset 107     Indicates the index of Domain which contains core 37
                                                    ///< Offset 108     Indicates the index of Domain which contains core 38
                                                    ///< Offset 109     Indicates the index of Domain which contains core 39
                                                    ///< Offset 110     Indicates the index of Domain which contains core 40
                                                    ///< Offset 111     Indicates the index of Domain which contains core 41
                                                    ///< Offset 112     Indicates the index of Domain which contains core 42
                                                    ///< Offset 113     Indicates the index of Domain which contains core 43
                                                    ///< Offset 114     Indicates the index of Domain which contains core 44
                                                    ///< Offset 115     Indicates the index of Domain which contains core 45
                                                    ///< Offset 116     Indicates the index of Domain which contains core 46
                                                    ///< Offset 117     Indicates the index of Domain which contains core 47
                                                    ///< Offset 118     Indicates the index of Domain which contains core 48
                                                    ///< Offset 119     Indicates the index of Domain which contains core 49
                                                    ///< Offset 120     Indicates the index of Domain which contains core 50
                                                    ///< Offset 121     Indicates the index of Domain which contains core 51
                                                    ///< Offset 122     Indicates the index of Domain which contains core 52
                                                    ///< Offset 123     Indicates the index of Domain which contains core 53
                                                    ///< Offset 124     Indicates the index of Domain which contains core 54
                                                    ///< Offset 125     Indicates the index of Domain which contains core 55
                                                    ///< Offset 126     Indicates the index of Domain which contains core 56
                                                    ///< Offset 127     Indicates the index of Domain which contains core 57
                                                    ///< Offset 128     Indicates the index of Domain which contains core 58
                                                    ///< Offset 129     Indicates the index of Domain which contains core 59
                                                    ///< Offset 130     Indicates the index of Domain which contains core 60
                                                    ///< Offset 131     Indicates the index of Domain which contains core 61
                                                    ///< Offset 132     Indicates the index of Domain which contains core 62
                                                    ///< Offset 133     Indicates the index of Domain which contains core 63
  UINT8    CoreCountInDomain[64];                   ///< Offset 134     Indicates the number of cores in the Domain which contains core 0
                                                    ///< Offset 135     Indicates the number of cores in the Domain which contains core 1
                                                    ///< Offset 136     Indicates the number of cores in the Domain which contains core 2
                                                    ///< Offset 137     Indicates the number of cores in the Domain which contains core 3
                                                    ///< Offset 138     Indicates the number of cores in the Domain which contains core 4
                                                    ///< Offset 139     Indicates the number of cores in the Domain which contains core 5
                                                    ///< Offset 140     Indicates the number of cores in the Domain which contains core 6
                                                    ///< Offset 141     Indicates the number of cores in the Domain which contains core 7
                                                    ///< Offset 142     Indicates the number of cores in the Domain which contains core 8
                                                    ///< Offset 143     Indicates the number of cores in the Domain which contains core 9
                                                    ///< Offset 144     Indicates the number of cores in the Domain which contains core 10
                                                    ///< Offset 145     Indicates the number of cores in the Domain which contains core 11
                                                    ///< Offset 146     Indicates the number of cores in the Domain which contains core 12
                                                    ///< Offset 147     Indicates the number of cores in the Domain which contains core 13
                                                    ///< Offset 148     Indicates the number of cores in the Domain which contains core 14
                                                    ///< Offset 149     Indicates the number of cores in the Domain which contains core 15
                                                    ///< Offset 150     Indicates the number of cores in the Domain which contains core 16
                                                    ///< Offset 151     Indicates the number of cores in the Domain which contains core 17
                                                    ///< Offset 152     Indicates the number of cores in the Domain which contains core 18
                                                    ///< Offset 153     Indicates the number of cores in the Domain which contains core 19
                                                    ///< Offset 154     Indicates the number of cores in the Domain which contains core 20
                                                    ///< Offset 155     Indicates the number of cores in the Domain which contains core 21
                                                    ///< Offset 156     Indicates the number of cores in the Domain which contains core 22
                                                    ///< Offset 157     Indicates the number of cores in the Domain which contains core 23
                                                    ///< Offset 158     Indicates the number of cores in the Domain which contains core 24
                                                    ///< Offset 159     Indicates the number of cores in the Domain which contains core 25
                                                    ///< Offset 160     Indicates the number of cores in the Domain which contains core 26
                                                    ///< Offset 161     Indicates the number of cores in the Domain which contains core 27
                                                    ///< Offset 162     Indicates the number of cores in the Domain which contains core 28
                                                    ///< Offset 163     Indicates the number of cores in the Domain which contains core 29
                                                    ///< Offset 164     Indicates the number of cores in the Domain which contains core 30
                                                    ///< Offset 165     Indicates the number of cores in the Domain which contains core 31
                                                    ///< Offset 166     Indicates the number of cores in the Domain which contains core 32
                                                    ///< Offset 167     Indicates the number of cores in the Domain which contains core 33
                                                    ///< Offset 168     Indicates the number of cores in the Domain which contains core 34
                                                    ///< Offset 169     Indicates the number of cores in the Domain which contains core 35
                                                    ///< Offset 170     Indicates the number of cores in the Domain which contains core 36
                                                    ///< Offset 171     Indicates the number of cores in the Domain which contains core 37
                                                    ///< Offset 172     Indicates the number of cores in the Domain which contains core 38
                                                    ///< Offset 173     Indicates the number of cores in the Domain which contains core 39
                                                    ///< Offset 174     Indicates the number of cores in the Domain which contains core 40
                                                    ///< Offset 175     Indicates the number of cores in the Domain which contains core 41
                                                    ///< Offset 176     Indicates the number of cores in the Domain which contains core 42
                                                    ///< Offset 177     Indicates the number of cores in the Domain which contains core 43
                                                    ///< Offset 178     Indicates the number of cores in the Domain which contains core 44
                                                    ///< Offset 179     Indicates the number of cores in the Domain which contains core 45
                                                    ///< Offset 180     Indicates the number of cores in the Domain which contains core 46
                                                    ///< Offset 181     Indicates the number of cores in the Domain which contains core 47
                                                    ///< Offset 182     Indicates the number of cores in the Domain which contains core 48
                                                    ///< Offset 183     Indicates the number of cores in the Domain which contains core 49
                                                    ///< Offset 184     Indicates the number of cores in the Domain which contains core 50
                                                    ///< Offset 185     Indicates the number of cores in the Domain which contains core 51
                                                    ///< Offset 186     Indicates the number of cores in the Domain which contains core 52
                                                    ///< Offset 187     Indicates the number of cores in the Domain which contains core 53
                                                    ///< Offset 188     Indicates the number of cores in the Domain which contains core 54
                                                    ///< Offset 189     Indicates the number of cores in the Domain which contains core 55
                                                    ///< Offset 190     Indicates the number of cores in the Domain which contains core 56
                                                    ///< Offset 191     Indicates the number of cores in the Domain which contains core 57
                                                    ///< Offset 192     Indicates the number of cores in the Domain which contains core 58
                                                    ///< Offset 193     Indicates the number of cores in the Domain which contains core 59
                                                    ///< Offset 194     Indicates the number of cores in the Domain which contains core 60
                                                    ///< Offset 195     Indicates the number of cores in the Domain which contains core 61
                                                    ///< Offset 196     Indicates the number of cores in the Domain which contains core 62
                                                    ///< Offset 197     Indicates the number of cores in the Domain which contains core 63
  UINT8    P1Ratio[64];                             ///< Offset 198     Indicate the P1 ratio of core 0
                                                    ///< Offset 199     Indicate the P1 ratio of core 1
                                                    ///< Offset 200     Indicate the P1 ratio of core 2
                                                    ///< Offset 201     Indicate the P1 ratio of core 3
                                                    ///< Offset 202     Indicate the P1 ratio of core 4
                                                    ///< Offset 203     Indicate the P1 ratio of core 5
                                                    ///< Offset 204     Indicate the P1 ratio of core 6
                                                    ///< Offset 205     Indicate the P1 ratio of core 7
                                                    ///< Offset 206     Indicate the P1 ratio of core 8
                                                    ///< Offset 207     Indicate the P1 ratio of core 9
                                                    ///< Offset 208     Indicate the P1 ratio of core 10
                                                    ///< Offset 209     Indicate the P1 ratio of core 11
                                                    ///< Offset 210     Indicate the P1 ratio of core 12
                                                    ///< Offset 211     Indicate the P1 ratio of core 13
                                                    ///< Offset 212     Indicate the P1 ratio of core 14
                                                    ///< Offset 213     Indicate the P1 ratio of core 15
                                                    ///< Offset 214     Indicate the P1 ratio of core 16
                                                    ///< Offset 215     Indicate the P1 ratio of core 17
                                                    ///< Offset 216     Indicate the P1 ratio of core 18
                                                    ///< Offset 217     Indicate the P1 ratio of core 19
                                                    ///< Offset 218     Indicate the P1 ratio of core 20
                                                    ///< Offset 219     Indicate the P1 ratio of core 21
                                                    ///< Offset 220     Indicate the P1 ratio of core 22
                                                    ///< Offset 221     Indicate the P1 ratio of core 23
                                                    ///< Offset 222     Indicate the P1 ratio of core 24
                                                    ///< Offset 223     Indicate the P1 ratio of core 25
                                                    ///< Offset 224     Indicate the P1 ratio of core 26
                                                    ///< Offset 225     Indicate the P1 ratio of core 27
                                                    ///< Offset 226     Indicate the P1 ratio of core 28
                                                    ///< Offset 227     Indicate the P1 ratio of core 29
                                                    ///< Offset 228     Indicate the P1 ratio of core 30
                                                    ///< Offset 229     Indicate the P1 ratio of core 31
                                                    ///< Offset 230     Indicate the P1 ratio of core 32
                                                    ///< Offset 231     Indicate the P1 ratio of core 33
                                                    ///< Offset 232     Indicate the P1 ratio of core 34
                                                    ///< Offset 233     Indicate the P1 ratio of core 35
                                                    ///< Offset 234     Indicate the P1 ratio of core 36
                                                    ///< Offset 235     Indicate the P1 ratio of core 37
                                                    ///< Offset 236     Indicate the P1 ratio of core 38
                                                    ///< Offset 237     Indicate the P1 ratio of core 39
                                                    ///< Offset 238     Indicate the P1 ratio of core 40
                                                    ///< Offset 239     Indicate the P1 ratio of core 41
                                                    ///< Offset 240     Indicate the P1 ratio of core 42
                                                    ///< Offset 241     Indicate the P1 ratio of core 43
                                                    ///< Offset 242     Indicate the P1 ratio of core 44
                                                    ///< Offset 243     Indicate the P1 ratio of core 45
                                                    ///< Offset 244     Indicate the P1 ratio of core 46
                                                    ///< Offset 245     Indicate the P1 ratio of core 47
                                                    ///< Offset 246     Indicate the P1 ratio of core 48
                                                    ///< Offset 247     Indicate the P1 ratio of core 49
                                                    ///< Offset 248     Indicate the P1 ratio of core 50
                                                    ///< Offset 249     Indicate the P1 ratio of core 51
                                                    ///< Offset 250     Indicate the P1 ratio of core 52
                                                    ///< Offset 251     Indicate the P1 ratio of core 53
                                                    ///< Offset 252     Indicate the P1 ratio of core 54
                                                    ///< Offset 253     Indicate the P1 ratio of core 55
                                                    ///< Offset 254     Indicate the P1 ratio of core 56
                                                    ///< Offset 255     Indicate the P1 ratio of core 57
                                                    ///< Offset 256     Indicate the P1 ratio of core 58
                                                    ///< Offset 257     Indicate the P1 ratio of core 59
                                                    ///< Offset 258     Indicate the P1 ratio of core 60
                                                    ///< Offset 259     Indicate the P1 ratio of core 61
                                                    ///< Offset 260     Indicate the P1 ratio of core 62
                                                    ///< Offset 261     Indicate the P1 ratio of core 63
  UINT8    IpcScaling[64];                          ///< Offset 262     Indicate the IPC scaling of core 0
                                                    ///< Offset 263     Indicate the IPC scaling of core 1
                                                    ///< Offset 264     Indicate the IPC scaling of core 2
                                                    ///< Offset 265     Indicate the IPC scaling of core 3
                                                    ///< Offset 266     Indicate the IPC scaling of core 4
                                                    ///< Offset 267     Indicate the IPC scaling of core 5
                                                    ///< Offset 268     Indicate the IPC scaling of core 6
                                                    ///< Offset 269     Indicate the IPC scaling of core 7
                                                    ///< Offset 270     Indicate the IPC scaling of core 8
                                                    ///< Offset 271     Indicate the IPC scaling of core 9
                                                    ///< Offset 272     Indicate the IPC scaling of core 10
                                                    ///< Offset 273     Indicate the IPC scaling of core 11
                                                    ///< Offset 274     Indicate the IPC scaling of core 12
                                                    ///< Offset 275     Indicate the IPC scaling of core 13
                                                    ///< Offset 276     Indicate the IPC scaling of core 14
                                                    ///< Offset 277     Indicate the IPC scaling of core 15
                                                    ///< Offset 278     Indicate the IPC scaling of core 16
                                                    ///< Offset 279     Indicate the IPC scaling of core 17
                                                    ///< Offset 280     Indicate the IPC scaling of core 18
                                                    ///< Offset 281     Indicate the IPC scaling of core 19
                                                    ///< Offset 282     Indicate the IPC scaling of core 20
                                                    ///< Offset 283     Indicate the IPC scaling of core 21
                                                    ///< Offset 284     Indicate the IPC scaling of core 22
                                                    ///< Offset 285     Indicate the IPC scaling of core 23
                                                    ///< Offset 286     Indicate the IPC scaling of core 24
                                                    ///< Offset 287     Indicate the IPC scaling of core 25
                                                    ///< Offset 288     Indicate the IPC scaling of core 26
                                                    ///< Offset 289     Indicate the IPC scaling of core 27
                                                    ///< Offset 290     Indicate the IPC scaling of core 28
                                                    ///< Offset 291     Indicate the IPC scaling of core 29
                                                    ///< Offset 292     Indicate the IPC scaling of core 30
                                                    ///< Offset 293     Indicate the IPC scaling of core 31
                                                    ///< Offset 294     Indicate the IPC scaling of core 32
                                                    ///< Offset 295     Indicate the IPC scaling of core 33
                                                    ///< Offset 296     Indicate the IPC scaling of core 34
                                                    ///< Offset 297     Indicate the IPC scaling of core 35
                                                    ///< Offset 298     Indicate the IPC scaling of core 36
                                                    ///< Offset 299     Indicate the IPC scaling of core 37
                                                    ///< Offset 300     Indicate the IPC scaling of core 38
                                                    ///< Offset 301     Indicate the IPC scaling of core 39
                                                    ///< Offset 302     Indicate the IPC scaling of core 40
                                                    ///< Offset 303     Indicate the IPC scaling of core 41
                                                    ///< Offset 304     Indicate the IPC scaling of core 42
                                                    ///< Offset 305     Indicate the IPC scaling of core 43
                                                    ///< Offset 306     Indicate the IPC scaling of core 44
                                                    ///< Offset 307     Indicate the IPC scaling of core 45
                                                    ///< Offset 308     Indicate the IPC scaling of core 46
                                                    ///< Offset 309     Indicate the IPC scaling of core 47
                                                    ///< Offset 310     Indicate the IPC scaling of core 48
                                                    ///< Offset 311     Indicate the IPC scaling of core 49
                                                    ///< Offset 312     Indicate the IPC scaling of core 50
                                                    ///< Offset 313     Indicate the IPC scaling of core 51
                                                    ///< Offset 314     Indicate the IPC scaling of core 52
                                                    ///< Offset 315     Indicate the IPC scaling of core 53
                                                    ///< Offset 316     Indicate the IPC scaling of core 54
                                                    ///< Offset 317     Indicate the IPC scaling of core 55
                                                    ///< Offset 318     Indicate the IPC scaling of core 56
                                                    ///< Offset 319     Indicate the IPC scaling of core 57
                                                    ///< Offset 320     Indicate the IPC scaling of core 58
                                                    ///< Offset 321     Indicate the IPC scaling of core 59
                                                    ///< Offset 322     Indicate the IPC scaling of core 60
                                                    ///< Offset 323     Indicate the IPC scaling of core 61
                                                    ///< Offset 324     Indicate the IPC scaling of core 62
                                                    ///< Offset 325     Indicate the IPC scaling of core 63
  UINT8    MaxP1Ratio;                              ///< Offset 326     Indicate the max P1 ratio of all cores
  UINT16   BusFrequency;                            ///< Offset 327     Bus frequency
  UINT8    EnableRp;                                ///< Offset 329     Enable/Disable Resource Priority Feature
  UINT8    ThETAIbattEnable;                        ///< Offset 330     Enable/Disable ThETA Ibatt Feature
  UINT8    IsysLimitL1Enable;                       ///< Offset 331     Enable/Disable Isys Limit L1 Feature
  UINT8    IsysLimitL2Enable;                       ///< Offset 332     Enable/Disable Isys Limit L2 Feature
  UINT8    TurboMode;                               ///< Offset 333     Enable/Disable Turbo Mode
} CPU_NVS_AREA;

#pragma pack(pop)
#endif
