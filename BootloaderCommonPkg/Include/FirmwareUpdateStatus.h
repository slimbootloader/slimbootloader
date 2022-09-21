/** @file
The header file for firmware update status definitions.

Copyright (c) 2022, Intel Corporation. All rights reserved.<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _FIRMWARE_UPDATE_STATUS_H_
#define _FIRMWARE_UPDATE_STATUS_H_

#define FW_UPDATE_SM_INIT             0xFF
#define FW_UPDATE_SM_CAP_PROCESSING   0x7F
#define FW_UPDATE_SM_PART_A           0x7E
#define FW_UPDATE_SM_PART_B           0x7D
#define FW_UPDATE_SM_PART_AB          0x7C
#define FW_UPDATE_SM_RECOVERY         0x7B
#define FW_UPDATE_SM_DONE             0x77 // Lower 3 bits are ignored

#define FW_UPDATE_IMAGE_UPDATE_NONE         0xFF
#define FW_UPDATE_IMAGE_UPDATE_PENDING      0xFE
#define FW_UPDATE_IMAGE_UPDATE_PROCESSING   0xFC
#define FW_UPDATE_IMAGE_UPDATE_DONE         0xF8

#define CSME_NEED_RESET_INIT     0xFF
#define CSME_NEED_RESET_PENDING  0xFE
#define CSME_NEED_RESET_DONE     0xFC
#define CSME_NEED_RESET_INVALID  0xF8

#define FW_UPDATE_COMP_BIOS_REGION SIGNATURE_32('B', 'I', 'O', 'S')
#define FW_UPDATE_COMP_CSME_REGION SIGNATURE_32('C', 'S', 'M', 'E')
#define FW_UPDATE_COMP_CSME_DRIVER SIGNATURE_32('C', 'S', 'M', 'D')
#define FW_UPDATE_COMP_CMD_REQUEST SIGNATURE_32('C', 'M', 'D', 'I')

#define FW_UPDATE_STATUS_SIGNATURE    SIGNATURE_32 ('F', 'W', 'U', 'S')
#define FW_RECOVERY_STATUS_SIGNATURE  SIGNATURE_32 ('F', 'W', 'R', 'S')
#define FW_UPDATE_STATUS_VERSION      0x1
#define FW_UPDATE_SIG_LENGTH          256

#pragma pack(push, 1)
//
// Firmware Update status structure
// This structure maintains the firmware update status
// in the non volatile reserved region of Slim Bootloader
// ESRT ACPI table will be populated based on this structure
//
typedef struct {
  UINT32                Signature;
  UINT16                Version;
  UINT16                Length;
  UINT8                 CapsuleSig[FW_UPDATE_SIG_LENGTH];
  UINT8                 StateMachine;
  UINT8                 RetryCount;
  UINT8                 CsmeNeedReset;
  UINT8                 Reserved[5];
} FW_UPDATE_STATUS;

typedef struct {
  EFI_GUID              FirmwareId;
  UINT64                HardwareInstance;
  UINT32                LastAttemptVersion;
  UINT32                LastAttemptStatus;
  UINT8                 UpdatePending;
  UINT8                 Reserved[3];
} FW_UPDATE_COMP_STATUS;

#pragma pack(pop)

#endif

