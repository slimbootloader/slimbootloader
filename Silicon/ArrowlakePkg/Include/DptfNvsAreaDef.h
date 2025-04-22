
/**@file


  Copyright (c) 2024, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _DPTF_NVS_AREA_DEF_H_
#define _DPTF_NVS_AREA_DEF_H_

#pragma pack (push,1)
typedef struct {
  //
  // Intel(R) Dynamic Tuning Technology Devices and trip points
  //
  UINT8    EnableDptf;                              ///< Offset 0       EnableDptf
  UINT32   EnableDCFG;                              ///< Offset 1       EnableDCFG
  UINT8    EnableSaDevice;                          ///< Offset 5       EnableSaDevice
  UINT8    EnableFan1Device;                        ///< Offset 6       EnableFan1Device
  UINT8    EnableFan2Device;                        ///< Offset 7       EnableFan2Device
  UINT8    EnableFan3Device;                        ///< Offset 8       EnableFan3Device
  UINT8    EnableChargerParticipant;                ///< Offset 9       EnableChargerParticipant
  UINT8    EnableBatteryParticipant;                ///< Offset 10      EnableBatteryParticipant
  UINT8    EnableInt3400Device;                     ///< Offset 11      EnableInt3400Device. new ACPI ID is INTC1040
  UINT8    EnableSen1Participant;                   ///< Offset 12      EnableSen1Participant
  UINT8    EnableSen2Participant;                   ///< Offset 13      EnableSen2Participant
  UINT8    EnableSen3Participant;                   ///< Offset 14      EnableSen3Participant
  UINT8    EnableSen4Participant;                   ///< Offset 15      EnableSen4Participant
  UINT8    EnableSen5Participant;                   ///< Offset 16      EnableSen5Participant
  UINT8    EnablePchFivrParticipant;                ///< Offset 17      EnablePchFivrParticipant
  //
  // Miscellaneous Intel(R) Dynamic Tuning Technology
  //
  UINT32   PpccStepSize;                            ///< Offset 18      PPCC Step Size
  UINT8    EnablePowerParticipant;                  ///< Offset 22      EnablePowerParticipant
  UINT16   PowerParticipantPollingRate;             ///< Offset 23      PowerParticipantPollingRate
  UINT8    OemDesignVariable0;                      ///< Offset 25      Intel(R) Dynamic Tuning Technology Oem Design Variables
  UINT8    OemDesignVariable1;                      ///< Offset 26      Intel(R) Dynamic Tuning Technology Oem Design Variables
  UINT8    OemDesignVariable2;                      ///< Offset 27      Intel(R) Dynamic Tuning Technology Oem Design Variables
  UINT8    OemDesignVariable3;                      ///< Offset 28      Intel(R) Dynamic Tuning Technology Oem Design Variables
  UINT8    OemDesignVariable4;                      ///< Offset 29      Intel(R) Dynamic Tuning Technology Oem Design Variables
  UINT8    OemDesignVariable5;                      ///< Offset 30      Intel(R) Dynamic Tuning Technology Oem Design Variables
  //
  // Enable System Self healing for fan participant
  //
  UINT8    FanSysHeal;                              ///< Offset 31      Fan System Self Healing
  UINT8    EnableFanAutoMode;                       ///< Offset 32      Enabling Auto Mode for DTT
} DPTF_NVS_AREA;

#pragma pack(pop)
#endif
