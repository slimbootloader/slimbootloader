/** @file
  Chipset definition for ME Devices.

 Copyright (c) 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#ifndef _ME_CHIPSET_H_
#define _ME_CHIPSET_H_

///
/// ME Bus and Device Info
///
#define ME_BUS                          0
#define ME_DEVICE_NUMBER                24
#define HECI_FUNCTION_NUMBER            0x00
#define HECI2_FUNCTION_NUMBER           0x01
#define HECI3_FUNCTION_NUMBER           0x04
#define HECI4_FUNCTION_NUMBER           0x06

#define R_ME_HFS                        0x40
#define ME_OPERATION_MODE_SPS_IGNITION  0x07

typedef enum {
  HECI1_DEVICE = HECI_FUNCTION_NUMBER,
  HECI2_DEVICE = HECI2_FUNCTION_NUMBER,
  HECI4_DEVICE = HECI4_FUNCTION_NUMBER,
  HECI3_DEVICE = HECI3_FUNCTION_NUMBER,
  HECI_INVALID_DEVICE = 0xFF
} HECI_DEVICE;

//
// HFSTS1, offset 40h
//
typedef union {
  UINT32  ul;
  struct {
    UINT32  CurrentState : 4;         // 0:3 - Current State
    UINT32  ManufacturingMode : 1;    // 4 Manufacturing Mode
    UINT32  FptBad : 1;               // 5 FPT(Flash Partition Table ) Bad
    UINT32  MeOperationState : 3;     // 6:8 - ME Operation State
    UINT32  FwInitComplete : 1;       // 9
    UINT32  FtBupLdFlr : 1;           // 10 - This bit is set when firmware is not able to load BRINGUP from the fault tolerant (FT) code.
    UINT32  FwUpdateInprogress : 1;   // 11
    UINT32  ErrorCode : 4;            // 12:15 - Error Code
    UINT32  MeOperationMode : 4;      // 16:19 - Management Engine Current Operation Mode
    UINT32  MeRstCnt : 4;             // 20:23 - ME reset counter
    UINT32  MeBootOptionsPresent : 1; // 24 - If this bit is set, a Boot Options is present
    UINT32  AckData : 3;              // 25:27 Ack Data
    UINT32  BiosMessageAck : 4;       // 28:31 BIOS Message Ack
  } r;
} HECI_FWS_REGISTER;

//
// HECI PCI Access Macro
//
#define HeciPciRead32(Register) PciRead32 (PCI_LIB_ADDRESS (ME_BUS, ME_DEVICE_NUMBER, HECI_FUNCTION_NUMBER, Register))

#endif // _ME_CHIPSET_H_
