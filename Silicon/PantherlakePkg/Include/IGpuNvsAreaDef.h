/**@file
  Interface definition details for MBP.

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

  //
  // Define IGPU NVS Area operation region.
  //
#ifndef _I_GPU_NVS_AREA_DEF_H_
#define _I_GPU_NVS_AREA_DEF_H_

#pragma pack (push,1)
typedef struct {
  UINT32   IgdOpRegionAddress;                      ///< Offset 0       IGD OpRegion base address
  UINT8    IgdState;                                ///< Offset 4       IGD State (Primary Display = 1)
  UINT8    BrightnessPercentage;                    ///< Offset 5       Brightness Level Percentage
  UINT8    AlsEnable;                               ///< Offset 6       Ambient Light Sensor Enable
  UINT8    EdpValid;                                ///< Offset 7       Check for eDP display device
  UINT8    LidState;                                ///< Offset 8       Lid State (Lid Open = 1)
  UINT32   NextStateDid1;                           ///< Offset 9       Next state DID1 for _DGS
  UINT32   NextStateDid2;                           ///< Offset 13      Next state DID2 for _DGS
  UINT32   NextStateDid3;                           ///< Offset 17      Next state DID3 for _DGS
  UINT32   NextStateDid4;                           ///< Offset 21      Next state DID4 for _DGS
  UINT32   NextStateDid5;                           ///< Offset 25      Next state DID5 for _DGS
  UINT32   NextStateDid6;                           ///< Offset 29      Next state DID6 for _DGS
  UINT32   NextStateDid7;                           ///< Offset 33      Next state DID7 for _DGS
  UINT32   NextStateDid8;                           ///< Offset 37      Next state DID8 for _DGS
  UINT32   NextStateDidEdp;                         ///< Offset 41      Next state DID for eDP
  UINT32   NextStateDidEdp2;                        ///< Offset 45      Next state DID for Second Display
  UINT8    NumberOfValidDeviceId;                   ///< Offset 49      Number of Valid Device IDs
  UINT32   DeviceId1;                               ///< Offset 50      Device ID 1
  UINT32   DeviceId2;                               ///< Offset 54      Device ID 2
  UINT32   DeviceId3;                               ///< Offset 58      Device ID 3
  UINT32   DeviceId4;                               ///< Offset 62      Device ID 4
  UINT32   DeviceId5;                               ///< Offset 66      Device ID 5
  UINT32   DeviceId6;                               ///< Offset 70      Device ID 6
  UINT32   DeviceId7;                               ///< Offset 74      Device ID 7
  UINT32   DeviceId8;                               ///< Offset 78      Device ID 8
  UINT32   DeviceId9;                               ///< Offset 82      Device ID 9
  UINT32   DeviceId10;                              ///< Offset 86      Device ID 10
  UINT32   DeviceId11;                              ///< Offset 90      Device ID 11
  UINT32   DeviceId12;                              ///< Offset 94      Device ID 12
  UINT32   DeviceId13;                              ///< Offset 98      Device ID 13
  UINT32   DeviceId14;                              ///< Offset 102     Device ID 14
  UINT32   DeviceId15;                              ///< Offset 106     Device ID 15
  UINT32   DeviceIdX;                               ///< Offset 110     Device ID for eDP device
  UINT32   DeviceIdY;                               ///< Offset 114     Device ID for second LFP device
} IGPU_NVS_AREA;

#pragma pack(pop)

#endif
