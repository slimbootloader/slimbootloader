/** @file
  Header file for CSME Extended Measurement library.

  Copyright (c) 2019 - 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/
#ifndef _DXE_ME_EXT_MEASUREMENT_LIB_H_
#define _DXE_ME_EXT_MEASUREMENT_LIB_H_

#define PCI_BASE_ADDRESSREG_OFFSET     0x10
#define PCI_COMMAND_OFFSET             0x04
#define PCI_DEVICE_ID_OFFSET                        0x02

//
// Standard event type
// @TODO: Below definitions of EV_NONHOST_CONFIG & EV_NONHOST_INFO should be removed after adding these definitions into Edk2\MdePkg\Include\IndustryStandard\UefiTcgPlatform.h file.
//
#define EV_NONHOST_CONFIG              ((TCG_EVENTTYPE) 0x00000010)
#define EV_NONHOST_INFO                ((TCG_EVENTTYPE) 0x00000011)

#define MAX_EVENTLOG_SIZE              1024
#define SIG_LENGTH                     20
#define HER_REG_COUNT_SHA384           12   // HER1-HER12
#define HER_REG_COUNT_SHA256           8    // HER1-HER8

///
/// CSME Measurement Event in TPM Log
///
typedef struct {
  UINT8                    Signature [SIG_LENGTH];
  UINT32                   ERHashAlgorithm;
  UINT8                    Events [MAX_EVENTLOG_SIZE];
} CSME_MEASUREMENT_EVENT;


/**
  Me Measured Boot setup and init

  @retval     RETURN_SUCCESS       If getting the MBP data successfully
  @retval     EFI_NOT_FOUND        Failed to get MBP data
**/
EFI_STATUS
MeMeasuredBootInit (
  VOID
  );
#endif // _DXE_ME_EXT_MEASUREMENT_LIB_H_
