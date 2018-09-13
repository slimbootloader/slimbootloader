/** @file

  Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _HECI_REGS_H
#define _HECI_REGS_H

//
// HECI PCI register definition
//
#define R_VENDORID                      0x00
#define R_DEVICEID                      0x02
#define R_COMMAND                       0x04
#define B_BME                           0x04
#define B_MSE                           0x02
#define R_REVID                         0x08
#define R_HECIMBAR0                     0x10
#define R_HECIMBAR1                     0x14
#define R_SEC_FW_STS0                   0x40
#define R_GEN_STS                       0x60 //0x4C
#define R_HIDM                          0xA0


#define H_CB_WW                         0x00
#define H_CSR                           0x04
#define SEC_CB_RW                       0x08
#define SEC_CSR_HA                      0x0C

/****************** REGISTER EQUATES ****************************************************/

//
// SEC_CSR_HA - SEC Control Status Host Access
//
typedef union {
  UINT32  ul;
  struct {
    UINT32  SecIeHra : 1;    // 0 - SEC Interrupt Enable (Host Read Access)
    UINT32  SecIsHra : 1;    // 1 - SEC Interrupt Status (Host Read Access)
    UINT32  SecIgHra : 1;    // 2 - SEC Interrupt Generate (Host Read Access)
    UINT32  SecRdyHra : 1;   // 3 - SEC Ready (Host Read Access)
    UINT32  SecRstHra : 1;   // 4 - SEC Reset (Host Read Access)
    UINT32  Reserved : 3;     // 7:5
    UINT32  SecCbrpHra : 8;  // 15:8 - SEC CB Read Pointer (Host Read Access)
    UINT32  SecCbwpHra : 8;  // 23:16 - SEC CB Write Pointer (Host Read Access)
    UINT32  SecCbdHra : 8;   // 31:24 - SEC Circular Buffer Depth (Host Read Access)
  } r;
} HECI_SEC_CONTROL_REGISTER;

// H_CSR - Host Control Status
typedef union {
  UINT32  ul;
  struct {
    UINT32  Hie : 1;     // 0 - Host Interrupt Enable SEC
    UINT32  His : 1;     // 1 - Host Interrupt Status SEC
    UINT32  Hig : 1;     // 2 - Host Interrupt Generate
    UINT32  Hrdy : 1;    // 3 - Host Ready
    UINT32  Hrst : 1;    // 4 - Host Reset
    UINT32  Reserved : 3; // 7:5
    UINT32  Hcbrp : 8;   // 15:8 - Host CB Read Pointer
    UINT32  Hcbwp : 8;   // 23:16 - Host CB Write Pointer
    UINT32  Hcbd : 8;    // 31:24 - Host Circular Buffer Depth
  } r;
} HECI_HOST_CONTROL_REGISTER;



// FWS
typedef union {
  UINT32  ul;
  struct {
    UINT32  CurrentState : 4;         // 0:3 - Current State
    UINT32  ManufacturingMode : 1;    // 4 Manufacturing Mode
    UINT32  FptBad : 1;               // 5 FPT(Flash Partition Table ) Bad
    UINT32  SeCOperationState : 3;    // 6:8 - SEC Operation State
    UINT32  FwInitComplete : 1;       // 9
    UINT32  FtBupLdFlr : 1;           // 10 - This bit is set when firmware is not able to load BRINGUP from the fault tolerant (FT) code.
    UINT32  FwUpdateInprogress : 1;   // 11
    UINT32  ErrorCode : 4;            // 12:15 - Error Code
    UINT32  SeCOperationMode : 4;     // 16:19 - Management Engine Current Operation Mode
    UINT32  Reserved2 : 4;            // 20:23
    UINT32  SeCBootOptionsPresent : 1;// 24 - If this bit is set, an Boot Options is present
    UINT32  AckData : 3;              // 25:27 Ack Data
    UINT32  BiosMessageAck : 4;       // 28:31 BIOS Message Ack
  } r;
} HECI_FWS_REGISTER;

/// MISC_SHDW
typedef union {
  UINT32  ul;
  struct {
    UINT32  Musz : 6;       ///< 0:5 - ME UMA Size
    UINT32  Reserved : 8;   ///< 6:13 - Reserved
    UINT32  Reserved2 : 2;  ///< 14:15 - Reserved
    UINT32  Muszv : 1;      ///< 16:16 - ME UMA Size Valid
    UINT32  Reserved3 : 8;  ///< 17:24 - Reserved
    UINT32  Reserved4 : 6;  ///< 25:30 - Reserved
    UINT32  Msvld : 1;      ///< 31:31 - Miscellaneous Shadow Valid
  } r;
} HECI_MISC_SHDW_REGISTER;

// SEC Current State Values
#define SEC_STATE_RESET                 0x00
#define SEC_STATE_INIT                  0x01
#define SEC_STATE_RECOVERY              0x02
#define SEC_STATE_NORMAL                0x05
#define SEC_STATE_DISABLE_WAIT          0x06
#define SEC_STATE_TRANSITION            0x07
#define SEC_STATE_INVALID_CPU           0x08

// SEC Firmware FwInitComplete
#define SEC_FIRMWARE_COMPLETED          0x01
#define SEC_FIRMWARE_INCOMPLETED        0x00

// SEC Boot Options Present
#define SEC_BOOT_OPTIONS_PRESENT        0x01
#define SEC_BOOT_OPTIONS_NOT_PRESENT    0x00

// Management Engine Current Operation Mode
#define SEC_OPERATION_MODE_NORMAL            0x00

#define SEC_OPERATION_MODE_ALT_DISABLED      0x02
#define SEC_OPERATION_MODE_SOFT_TEMP_DISABLE 0x03
#define SEC_OPERATION_MODE_SECOVR_JMPR       0x04
#define SEC_OPERATION_MODE_SECOVR_HECI_MSG   0x05

#endif // HECI_REGS_H
