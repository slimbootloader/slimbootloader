/**@file

  Copyright (c) 2023, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

  //
  // Define DTBT NVS Area operation region.
  //


  OperationRegion (DTNV, SystemMemory, 0xFFFF0000, 0xAA55)
  Field(DTNV,AnyAcc,Lock,Preserve)
  {  Offset(0),      TREV, 8,  // Offset(0),     Revison of DTbtPkgNvsArea
  Offset(1),      DTFS, 8,  // Offset(1),     Discrete Thunderbolt(TM) Feature support
  Offset(2),      TCPN, 8,  // Offset(2),     Total dTbt Controller Present Number.
  Offset(3),      RPS0, 8,  // Offset(3),     Root port Selected by the User
  Offset(4),      RPT0, 8,  // Offset(4),     Root port Type
  Offset(5),      RPN0, 8,  // Offset(5),     Root port Enabled by the User
  Offset(6),      RPS1, 8,  // Offset(6),     Root port Selected by the User
  Offset(7),      RPT1, 8,  // Offset(7),     Root port Type
  Offset(8),      RPN1, 8,  // Offset(8),     Root port Enabled by the User
  Offset(9),      DTRT, 8,  // Offset(9),     DTBT Rtd3.
  Offset(10),     DTOE, 8,  // Offset(10),    DTBT RTD3 Off delay Optimization Enable.
  Offset(11),     DTRO, 16, // Offset(11),    DTBT RTD3 Off delay in ms.
  Offset(13),     DTRC, 8,  // Offset(13),    Enable DTBT RTD3 CLKREQ mask.
  Offset(14),     DTRD, 16, // Offset(14),    DTBT RTD3 CLKREQ mask delay.
  Offset(16),     DTWA, 8,  // Offset(16),    Send Go2SxNoWake or GoSxWake according to TbtWakeupSupport
  Offset(17),     DG2S, 8,  // Offset(17),    DTBT Go2Sx command.
  }