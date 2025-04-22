/** @file

  Provides some data structure definitions used by the SD/MMC host controller driver.

Copyright (c) 2015 - 2017, Intel Corporation. All rights reserved.<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _SD_MMC_PCI_HC_DXE_H_
#define _SD_MMC_PCI_HC_DXE_H_

#include <IndustryStandard/Pci.h>
#include <IndustryStandard/Emmc.h>
#include <IndustryStandard/Sd.h>

#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>

#include "SdMmcPciHci.h"
#include "SdMmcPassThru.h"

#define SD_MMC_HC_PRIVATE_SIGNATURE  SIGNATURE_32 ('s', 'd', 't', 'f')

//
// Generic time out value, 1 microsecond as unit.
//
#define SD_MMC_HC_GENERIC_TIMEOUT     1 * 1000 * 1000

typedef enum {
  UnknownCardType,
  SdCardType,
  SdioCardType,
  MmcCardType,
  EmmcCardType
} SD_MMC_CARD_TYPE;

typedef struct {
  UINT32          Address;
  UINT32          Ocr;
  EMMC_CID        Cid;
  EMMC_CSD        Csd;
  EMMC_EXT_CSD    ExtCsd;
  UINT32          BlockLen;
  UINT32          BlockNum;
} EMMC_CARD_DATA;

typedef enum {
  RemovableSlot,
  EmbeddedSlot,
  SharedBusSlot,
  UnknownSlot
} EFI_SD_MMC_SLOT_TYPE;

typedef enum {
  ReservedMemory,
  PayloadMemory
} PRIVATE_DATA_MEMORY_TYPE;

typedef struct {
  EFI_SD_MMC_SLOT_TYPE              SlotType;
  BOOLEAN                           MediaPresent;
  BOOLEAN                           Initialized;
  BOOLEAN                           SectorAddressing;
  SD_MMC_CARD_TYPE                  CardType;
  UINT64                            CurrentFreq;
  UINT8                             CardData[sizeof (EMMC_CARD_DATA)];
} SD_MMC_HC_SLOT;

typedef struct {
  UINTN                               Signature;
  UINTN                               SdMmcHcPciBase;
  UINT32                              SdMmcHcBase;

  SD_MMC_HC_SLOT                      Slot;
  SD_MMC_HC_SLOT_CAP                  Capability;
  UINT64                              MaxCurrent;
  UINT32                              PrivateDataMemType;
  UINT32                              ControllerVersion;
  UINTN                               CurrentPartition;
} SD_MMC_HC_PRIVATE_DATA;

#define SD_MMC_HC_TRB_SIG             SIGNATURE_32 ('T', 'R', 'B', 'T')

//
// TRB (Transfer Request Block) contains information for the cmd request.
//
typedef struct {
  UINT32                              Signature;

  UINT16                              BlockSize;

  EFI_SD_MMC_PASS_THRU_COMMAND_PACKET *Packet;
  VOID                                *Data;
  UINT32                              DataLen;
  BOOLEAN                             Read;

  EFI_PHYSICAL_ADDRESS                DataPhy;
  VOID                                *DataMap;
  SD_MMC_HC_TRANSFER_MODE             Mode;

  BOOLEAN                             Started;
  UINT64                              Timeout;

  SD_MMC_HC_ADMA_DESC_LINE            *AdmaDesc;
  EFI_PHYSICAL_ADDRESS                AdmaDescPhy;
  VOID                                *AdmaMap;
  UINT32                              AdmaPages;

  SD_MMC_HC_PRIVATE_DATA              *Private;
} SD_MMC_HC_TRB;

/**
  Create a new TRB for the SD/MMC cmd request.

  @param[in] Private        A pointer to the SD_MMC_HC_PRIVATE_DATA instance.
  @param[in] Packet         A pointer to the SD command data structure.

  @return Created Trb or NULL.
**/
SD_MMC_HC_TRB *
SdMmcCreateTrb (
  IN SD_MMC_HC_PRIVATE_DATA              *Private,
  IN EFI_SD_MMC_PASS_THRU_COMMAND_PACKET *Packet
  );

/**
  Free the resource used by the TRB.

  @param[in] Trb            The pointer to the SD_MMC_HC_TRB instance.

**/
VOID
SdMmcFreeTrb (
  IN SD_MMC_HC_TRB           *Trb
  );

/**
  Check if the env is ready for execute specified TRB.

  @param[in] Private        A pointer to the SD_MMC_HC_PRIVATE_DATA instance.
  @param[in] Trb            The pointer to the SD_MMC_HC_TRB instance.

  @retval EFI_SUCCESS       The env is ready for TRB execution.
  @retval EFI_NOT_READY     The env is not ready for TRB execution.
  @retval Others            Some erros happen.

**/
EFI_STATUS
SdMmcCheckTrbEnv (
  IN SD_MMC_HC_PRIVATE_DATA           *Private,
  IN SD_MMC_HC_TRB                    *Trb
  );

/**
  Wait for the env to be ready for execute specified TRB.

  @param[in] Private        A pointer to the SD_MMC_HC_PRIVATE_DATA instance.
  @param[in] Trb            The pointer to the SD_MMC_HC_TRB instance.

  @retval EFI_SUCCESS       The env is ready for TRB execution.
  @retval EFI_TIMEOUT       The env is not ready for TRB execution in time.
  @retval Others            Some erros happen.

**/
EFI_STATUS
SdMmcWaitTrbEnv (
  IN SD_MMC_HC_PRIVATE_DATA           *Private,
  IN SD_MMC_HC_TRB                    *Trb
  );

/**
  Execute the specified TRB.

  @param[in] Private        A pointer to the SD_MMC_HC_PRIVATE_DATA instance.
  @param[in] Trb            The pointer to the SD_MMC_HC_TRB instance.

  @retval EFI_SUCCESS       The TRB is sent to host controller successfully.
  @retval Others            Some erros happen when sending this request to the host controller.

**/
EFI_STATUS
SdMmcExecTrb (
  IN SD_MMC_HC_PRIVATE_DATA           *Private,
  IN SD_MMC_HC_TRB                    *Trb
  );

/**
  Check the TRB execution result.

  @param[in] Private        A pointer to the SD_MMC_HC_PRIVATE_DATA instance.
  @param[in] Trb            The pointer to the SD_MMC_HC_TRB instance.

  @retval EFI_SUCCESS       The TRB is executed successfully.
  @retval EFI_NOT_READY     The TRB is not completed for execution.
  @retval Others            Some erros happen when executing this request.

**/
EFI_STATUS
SdMmcCheckTrbResult (
  IN SD_MMC_HC_PRIVATE_DATA           *Private,
  IN SD_MMC_HC_TRB                    *Trb
  );

/**
  Wait for the TRB execution result.

  @param[in] Private        A pointer to the SD_MMC_HC_PRIVATE_DATA instance.
  @param[in] Trb            The pointer to the SD_MMC_HC_TRB instance.

  @retval EFI_SUCCESS       The TRB is executed successfully.
  @retval Others            Some erros happen when executing this request.

**/
EFI_STATUS
SdMmcWaitTrbResult (
  IN SD_MMC_HC_PRIVATE_DATA           *Private,
  IN SD_MMC_HC_TRB                    *Trb
  );

/**
  Sends SD command to an SD card that is attached to the SD controller.

  The PassThru() function sends the SD command specified by Packet to the SD card
  specified by Slot.

  If Packet is successfully sent to the SD card, then EFI_SUCCESS is returned.

  If a device error occurs while sending the Packet, then EFI_DEVICE_ERROR is returned.

  If Slot is not in a valid range for the SD controller, then EFI_INVALID_PARAMETER
  is returned.

  If Packet defines a data command but both InDataBuffer and OutDataBuffer are NULL,
  EFI_INVALID_PARAMETER is returned.

  @param[in] Private            A pointer to the SD_MMC_HC_PRIVATE_DATA instance.
  @param[in, out] Packet        A pointer to the SD command data structure.

  @retval EFI_SUCCESS           The SD Command Packet was sent by the host.
  @retval EFI_DEVICE_ERROR      A device error occurred while attempting to send the SD
                                command Packet.
  @retval EFI_INVALID_PARAMETER Packet, Slot, or the contents of the Packet is invalid.
  @retval EFI_INVALID_PARAMETER Packet defines a data command but both InDataBuffer and
                                OutDataBuffer are NULL.
  @retval EFI_NO_MEDIA          SD Device not present in the Slot.
  @retval EFI_UNSUPPORTED       The command described by the SD Command Packet is not
                                supported by the host controller.
  @retval EFI_BAD_BUFFER_SIZE   The InTransferLength or OutTransferLength exceeds the
                                limit supported by SD card ( i.e. if the number of bytes
                                exceed the Last LBA).

**/
EFI_STATUS
EFIAPI
SdMmcSendCommand (
  IN     SD_MMC_HC_PRIVATE_DATA                *Private,
  IN OUT EFI_SD_MMC_PASS_THRU_COMMAND_PACKET   *Packet
  );

#endif
