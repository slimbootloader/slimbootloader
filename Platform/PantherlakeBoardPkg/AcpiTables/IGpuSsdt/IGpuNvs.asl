/** @file
  Define IGPU NVS Area operation region

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

  //
  // Define IGPU NVS Area operation region.
  //


  OperationRegion(IGNV,SystemMemory,IGNB,IGNL)
  Field(IGNV,AnyAcc,Lock,Preserve)
  {
  ASLB, 32, // Offset(0),     IGD OpRegion base address
  IGDS, 8,  // Offset(4),     IGD State (Primary Display = 1)
  BRTL, 8,  // Offset(5),     Brightness Level Percentage
  ALSE, 8,  // Offset(6),     Ambient Light Sensor Enable
  EDPV, 8,  // Offset(7),     Check for eDP display device
  LIDS, 8,  // Offset(8),     Lid State (Lid Open = 1)
  NXD1, 32, // Offset(9),     Next state DID1 for _DGS
  NXD2, 32, // Offset(13),    Next state DID2 for _DGS
  NXD3, 32, // Offset(17),    Next state DID3 for _DGS
  NXD4, 32, // Offset(21),    Next state DID4 for _DGS
  NXD5, 32, // Offset(25),    Next state DID5 for _DGS
  NXD6, 32, // Offset(29),    Next state DID6 for _DGS
  NXD7, 32, // Offset(33),    Next state DID7 for _DGS
  NXD8, 32, // Offset(37),    Next state DID8 for _DGS
  NXDX, 32, // Offset(41),    Next state DID for eDP
  NXDY, 32, // Offset(45),    Next state DID for Second Display
  NDID, 8,  // Offset(49),    Number of Valid Device IDs
  DID1, 32, // Offset(50),    Device ID 1
  DID2, 32, // Offset(54),    Device ID 2
  DID3, 32, // Offset(58),    Device ID 3
  DID4, 32, // Offset(62),    Device ID 4
  DID5, 32, // Offset(66),    Device ID 5
  DID6, 32, // Offset(70),    Device ID 6
  DID7, 32, // Offset(74),    Device ID 7
  DID8, 32, // Offset(78),    Device ID 8
  DID9, 32, // Offset(82),    Device ID 9
  DIDA, 32, // Offset(86),    Device ID 10
  DIDB, 32, // Offset(90),    Device ID 11
  DIDC, 32, // Offset(94),    Device ID 12
  DIDD, 32, // Offset(98),    Device ID 13
  DIDE, 32, // Offset(102),   Device ID 14
  DIDF, 32, // Offset(106),   Device ID 15
  DIDX, 32, // Offset(110),   Device ID for eDP device
  DIDY, 32, // Offset(114),   Device ID for second LFP device
  }
