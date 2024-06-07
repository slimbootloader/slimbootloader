/** @file

  Copyright (c) 2024, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

  OperationRegion(DNVS,SystemMemory,0xFFFF0000,0xAA55)
  Field(DNVS,AnyAcc,Lock,Preserve)
  {  //
  // Intel(R) Dynamic Tuning Technology Devices and trip points
  //
  Offset(0),      DPTF, 8,  // Offset(0),     EnableDptf
  Offset(1),      DCFE, 32, // Offset(1),     EnableDCFG
  Offset(5),      SADE, 8,  // Offset(5),     EnableSaDevice
  Offset(6),      FND1, 8,  // Offset(6),     EnableFan1Device
  Offset(7),      FND2, 8,  // Offset(7),     EnableFan2Device
  Offset(8),      FND3, 8,  // Offset(8),     EnableFan3Device
  Offset(9),      CHGE, 8,  // Offset(9),     EnableChargerParticipant
  Offset(10),     BATR, 8,  // Offset(10),    EnableBatteryParticipant
  Offset(11),     IN34, 8,  // Offset(11),    EnableInt3400Device. new ACPI ID is INTC1040
  Offset(12),     S1DE, 8,  // Offset(12),    EnableSen1Participant
  Offset(13),     S2DE, 8,  // Offset(13),    EnableSen2Participant
  Offset(14),     S3DE, 8,  // Offset(14),    EnableSen3Participant
  Offset(15),     S4DE, 8,  // Offset(15),    EnableSen4Participant
  Offset(16),     S5DE, 8,  // Offset(16),    EnableSen5Participant
  Offset(17),     PCHE, 8,  // Offset(17),    EnablePchFivrParticipant
  //
  // Miscellaneous Intel(R) Dynamic Tuning Technology
  //
  Offset(18),     PPSZ, 32, // Offset(18),    PPCC Step Size
  Offset(22),     PWRE, 8,  // Offset(22),    EnablePowerParticipant
  Offset(23),     PPPR, 16, // Offset(23),    PowerParticipantPollingRate
  Offset(25),     ODV0, 8,  // Offset(25),    Intel(R) Dynamic Tuning Technology Oem Design Variables
  Offset(26),     ODV1, 8,  // Offset(26),    Intel(R) Dynamic Tuning Technology Oem Design Variables
  Offset(27),     ODV2, 8,  // Offset(27),    Intel(R) Dynamic Tuning Technology Oem Design Variables
  Offset(28),     ODV3, 8,  // Offset(28),    Intel(R) Dynamic Tuning Technology Oem Design Variables
  Offset(29),     ODV4, 8,  // Offset(29),    Intel(R) Dynamic Tuning Technology Oem Design Variables
  Offset(30),     ODV5, 8,  // Offset(30),    Intel(R) Dynamic Tuning Technology Oem Design Variables
  //
  // Enable System Self healing for fan participant
  //
  Offset(31),     FNSH, 8,  // Offset(31),    Fan System Self Healing
  Offset(32),     FNAU, 8,  // Offset(32),    Enabling Auto Mode for DTT
  }
