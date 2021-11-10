/** @file
  The EFI_SD_MMC_PASS_THRU_PROTOCOL provides the ability to send SD/MMC Commands
  to any SD/MMC device attached to the SD compatible pci host controller.

  Copyright (c) 2015, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __SD_MMC_PASS_THRU_H__
#define __SD_MMC_PASS_THRU_H__

typedef enum {
  SdMmcCommandTypeBc,  // Broadcast commands, no response
  SdMmcCommandTypeBcr, // Broadcast commands with response
  SdMmcCommandTypeAc,  // Addressed(point-to-point) commands
  SdMmcCommandTypeAdtc // Addressed(point-to-point) data transfer commands
} EFI_SD_MMC_COMMAND_TYPE;

typedef enum {
  SdMmcResponseTypeR1,
  SdMmcResponseTypeR1b,
  SdMmcResponseTypeR2,
  SdMmcResponseTypeR3,
  SdMmcResponseTypeR4,
  SdMmcResponseTypeR5,
  SdMmcResponseTypeR5b,
  SdMmcResponseTypeR6,
  SdMmcResponseTypeR7
} EFI_SD_MMC_RESPONSE_TYPE;

typedef struct _EFI_SD_MMC_COMMAND_BLOCK {
  UINT16                            CommandIndex;
  UINT32                            CommandArgument;
  UINT32                            CommandType;      // One of the EFI_SD_MMC_COMMAND_TYPE values
  UINT32                            ResponseType;     // One of the EFI_SD_MMC_RESPONSE_TYPE values
} EFI_SD_MMC_COMMAND_BLOCK;

typedef struct _EFI_SD_MMC_STATUS_BLOCK {
  UINT32                            Resp0;
  UINT32                            Resp1;
  UINT32                            Resp2;
  UINT32                            Resp3;
} EFI_SD_MMC_STATUS_BLOCK;

typedef struct _EFI_SD_MMC_PASS_THRU_COMMAND_PACKET {
  UINT64                            Timeout;
  EFI_SD_MMC_COMMAND_BLOCK          *SdMmcCmdBlk;
  EFI_SD_MMC_STATUS_BLOCK           *SdMmcStatusBlk;
  VOID                              *InDataBuffer;
  VOID                              *OutDataBuffer;
  UINT32                            InTransferLength;
  UINT32                            OutTransferLength;
  EFI_STATUS                        TransactionStatus;
} EFI_SD_MMC_PASS_THRU_COMMAND_PACKET;

#endif
