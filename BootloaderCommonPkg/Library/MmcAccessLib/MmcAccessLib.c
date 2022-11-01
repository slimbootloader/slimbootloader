/** @file
  This file provides some helper functions which are specific for EMMC device.

  Copyright (c) 2015 - 2022, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Library/BaseLib.h>
#include <Library/MmcAccessLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BootloaderCommonLib.h>
#include <Library/IoLib.h>
#include "SdMmcPciHcDxe.h"
#include "MmcAccessLibPrivate.h"
#include <Library/TimerLib.h>

/**
  Get a pointer to the SD_MMC_HC_PRIVATE_DATA instance.

  @retval Private      A pointer to the SD_MMC_HC_PRIVATE_DATA instance.
**/
SD_MMC_HC_PRIVATE_DATA *
MmcGetHcPrivateData (
  VOID
  )
{
  EFI_STATUS                Status;
  SD_MMC_HC_PRIVATE_DATA   *PrivateData;

  Status = GetLibraryData (PcdGet8 (PcdEmmcBlockDeviceLibId), (VOID **)&PrivateData);
  if (Status == EFI_NOT_FOUND) {
    PrivateData = AllocatePool (sizeof (SD_MMC_HC_PRIVATE_DATA));
    if (PrivateData != NULL) {
      ZeroMem (PrivateData, sizeof (SD_MMC_HC_PRIVATE_DATA));
      if (GetLoaderStage () == LOADER_STAGE_PAYLOAD) {
        PrivateData->PrivateDataMemType = PayloadMemory;
      } else {
        PrivateData->PrivateDataMemType = ReservedMemory;
      }
      Status = SetLibraryData (PcdGet8 (PcdEmmcBlockDeviceLibId), PrivateData, sizeof (SD_MMC_HC_PRIVATE_DATA));
    } else {
      Status = EFI_OUT_OF_RESOURCES;
    }
  }

  if (EFI_ERROR (Status)) {
    PrivateData = NULL;
  }

  return PrivateData;
}

/**
  Send command GO_IDLE_STATE (CMD0 with argument of 0x00000000) to the device to
  make it go to Idle State.

  Refer to EMMC Electrical Standard Spec 5.1 Section 6.4 for details.

  @param[in]  Private       A pointer to the SD_MMC_HC_PRIVATE_DATA instance.

  @retval EFI_SUCCESS       The EMMC device is reset correctly.
  @retval Others            The device reset fails.

**/
EFI_STATUS
MmcReset (
  IN SD_MMC_HC_PRIVATE_DATA          *Private
  )
{
  EFI_SD_MMC_COMMAND_BLOCK              SdMmcCmdBlk;
  EFI_SD_MMC_STATUS_BLOCK               SdMmcStatusBlk;
  EFI_SD_MMC_PASS_THRU_COMMAND_PACKET   Packet;
  EFI_STATUS                            Status;

  ZeroMem (&SdMmcCmdBlk, sizeof (SdMmcCmdBlk));
  ZeroMem (&SdMmcStatusBlk, sizeof (SdMmcStatusBlk));
  ZeroMem (&Packet, sizeof (Packet));

  Packet.SdMmcCmdBlk    = &SdMmcCmdBlk;
  Packet.SdMmcStatusBlk = &SdMmcStatusBlk;
  Packet.Timeout        = SD_MMC_HC_GENERIC_TIMEOUT;

  SdMmcCmdBlk.CommandIndex = EMMC_GO_IDLE_STATE;
  SdMmcCmdBlk.CommandType  = SdMmcCommandTypeBc;
  SdMmcCmdBlk.ResponseType = 0;
  SdMmcCmdBlk.CommandArgument = 0;

  Status = SdMmcSendCommand (Private, &Packet);

  return Status;
}

/**
  Send command SEND_OP_COND to the EMMC device to get the data of the OCR register.

  Refer to EMMC Electrical Standard Spec 5.1 Section 6.4 for details.

  @param[in]  Private       A pointer to the SD_MMC_HC_PRIVATE_DATA instance.
  @param[in, out] Argument  On input, the argument of SEND_OP_COND is to send to the device.
                            On output, the argument is the value of OCR register.

  @retval EFI_SUCCESS       The operation is done correctly.
  @retval Others            The operation fails.

**/
EFI_STATUS
MmcGetOcr (
  IN     SD_MMC_HC_PRIVATE_DATA         *Private,
  IN OUT UINT32                         *Argument
  )
{
  EFI_SD_MMC_COMMAND_BLOCK              SdMmcCmdBlk;
  EFI_SD_MMC_STATUS_BLOCK               SdMmcStatusBlk;
  EFI_SD_MMC_PASS_THRU_COMMAND_PACKET   Packet;
  EFI_STATUS                            Status;

  ZeroMem (&SdMmcCmdBlk, sizeof (SdMmcCmdBlk));
  ZeroMem (&SdMmcStatusBlk, sizeof (SdMmcStatusBlk));
  ZeroMem (&Packet, sizeof (Packet));

  Packet.SdMmcCmdBlk    = &SdMmcCmdBlk;
  Packet.SdMmcStatusBlk = &SdMmcStatusBlk;
  Packet.Timeout        = SD_MMC_HC_GENERIC_TIMEOUT;

  SdMmcCmdBlk.CommandIndex = EMMC_SEND_OP_COND;
  SdMmcCmdBlk.CommandType  = SdMmcCommandTypeBcr;
  SdMmcCmdBlk.ResponseType = SdMmcResponseTypeR3;
  SdMmcCmdBlk.CommandArgument = *Argument;

  Status = SdMmcSendCommand (Private, &Packet);

  if (!EFI_ERROR (Status)) {
    //
    // For details, refer to SD Host Controller Simplified Spec 3.0 Table 2-12.
    //
    *Argument = SdMmcStatusBlk.Resp0;
  }

  return Status;
}

/**
  Send command SEND_IF_COND to the device to inquiry the SD Memory Card interface
  condition.

  @param[in]  Private       A pointer to the SD_MMC_HC_PRIVATE_DATA instance.

  @retval EFI_SUCCESS       The operation is done correctly.
  @retval Others            The operation fails.

**/
EFI_STATUS
SdCardVoltageCheck (
  IN  SD_MMC_HC_PRIVATE_DATA            *Private
  )
{
  EFI_SD_MMC_COMMAND_BLOCK              SdMmcCmdBlk;
  EFI_SD_MMC_STATUS_BLOCK               SdMmcStatusBlk;
  EFI_SD_MMC_PASS_THRU_COMMAND_PACKET   Packet;
  EFI_STATUS                            Status;

  ZeroMem (&SdMmcCmdBlk, sizeof (SdMmcCmdBlk));
  ZeroMem (&SdMmcStatusBlk, sizeof (SdMmcStatusBlk));
  ZeroMem (&Packet, sizeof (Packet));

  Packet.SdMmcCmdBlk    = &SdMmcCmdBlk;
  Packet.SdMmcStatusBlk = &SdMmcStatusBlk;
  Packet.Timeout        = SD_MMC_HC_GENERIC_TIMEOUT;

  SdMmcCmdBlk.CommandIndex = SD_SEND_IF_COND;
  SdMmcCmdBlk.CommandType  = SdMmcCommandTypeBcr;
  SdMmcCmdBlk.ResponseType = SdMmcResponseTypeR7;
  SdMmcCmdBlk.CommandArgument = 0x1FF;

  Status = SdMmcSendCommand (Private, &Packet);
  if (!EFI_ERROR (Status)) {
    if (SdMmcStatusBlk.Resp0 != SdMmcCmdBlk.CommandArgument) {
      return EFI_DEVICE_ERROR;
    }
  }

  return Status;
}

/**
  Send command SD_SEND_OP_COND to the device to see whether it is SDIO device.

  @param[in]  Private        A pointer to the SD_MMC_HC_PRIVATE_DATA instance.
  @param[in, out]  Argument       Command argument for input.
                             Command response0 for output.
  @retval EFI_SUCCESS        The operation is done correctly.
  @retval Others             The operation fails.

**/
EFI_STATUS
SdCardSendOpCond (
  IN  SD_MMC_HC_PRIVATE_DATA            *Private,
  IN OUT UINT32                         *Argument
  )
{
  EFI_SD_MMC_COMMAND_BLOCK              SdMmcCmdBlk;
  EFI_SD_MMC_STATUS_BLOCK               SdMmcStatusBlk;
  EFI_SD_MMC_PASS_THRU_COMMAND_PACKET   Packet;
  EFI_STATUS                            Status;

  ZeroMem (&SdMmcCmdBlk, sizeof (SdMmcCmdBlk));
  ZeroMem (&SdMmcStatusBlk, sizeof (SdMmcStatusBlk));
  ZeroMem (&Packet, sizeof (Packet));

  Packet.SdMmcCmdBlk    = &SdMmcCmdBlk;
  Packet.SdMmcStatusBlk = &SdMmcStatusBlk;
  Packet.Timeout        = SD_MMC_HC_GENERIC_TIMEOUT;

  SdMmcCmdBlk.CommandIndex = SD_APP_CMD;
  SdMmcCmdBlk.CommandType  = SdMmcCommandTypeAc;
  SdMmcCmdBlk.ResponseType = SdMmcResponseTypeR1;
  SdMmcCmdBlk.CommandArgument = 0;

  Status = SdMmcSendCommand (Private, &Packet);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  SdMmcCmdBlk.CommandIndex = SD_SEND_OP_COND;
  SdMmcCmdBlk.CommandType  = SdMmcCommandTypeBcr;
  SdMmcCmdBlk.ResponseType = SdMmcResponseTypeR3;
  SdMmcCmdBlk.CommandArgument = *Argument;

  Status = SdMmcSendCommand (Private, &Packet);
  if (!EFI_ERROR (Status)) {
    //
    // For details, refer to SD Host Controller Simplified Spec 3.0 Table 2-12.
    //
    *Argument = SdMmcStatusBlk.Resp0;
  }

  return Status;
}

/**
  This function executes very minimum initialization of EMMC device.

  @param[in]  Private           A pointer to the SD_MMC_HC_PRIVATE_DATA instance.

  @retval EFI_SUCCESS           The request is executed successfully.
  @retval EFI_OUT_OF_RESOURCES  The request could not be executed due to a lack of resources.
  @retval Others                The request could not be executed successfully.

**/
RETURN_STATUS
EarlyMmcInitialize (
  IN  SD_MMC_HC_PRIVATE_DATA  *Private
  )
{
  RETURN_STATUS    Status;
  EMMC_CARD_DATA  *CardData;
  UINT8            HostCtrl1;
  UINT8            BusWidth;
  UINTN            Retry;

  Status = RETURN_SUCCESS;
  CardData = NULL;

  Status   = SdMmcHcRwMmio (Private->SdMmcHcBase, SD_MMC_HC_HOST_CTRL1, TRUE, sizeof (HostCtrl1), &HostCtrl1);
  if (EFI_ERROR (Status)) {
    BusWidth = 0;
  } else {
    // BusWidth will be set only after eMMC phase2 init
    // If the card is fully initialized already, need to reset to do re-initialization flow
    BusWidth = HostCtrl1 & (BIT1 | BIT5);
  }

  CardData = (EMMC_CARD_DATA *) Private->Slot.CardData;
  if ((CardData->Ocr != 0) && (BusWidth == 0)) {
    DEBUG ((DEBUG_INFO, "Early EMMC is skipped \n"));
    return EFI_SUCCESS;
  }

  DEBUG ((DEBUG_INFO, "MMC Phase 1 init\n"));

  Status = SdMmcHcReset (Private->SdMmcHcBase);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "SdMmcHcReset Fail Status = 0x%x\n", Status));
    goto Done;
  }

  Status = SdMmcHcInitHost (Private->SdMmcHcBase, Private->Capability);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "SdMmcHcInitHost Fail Status = 0x%x\n", Status));
    goto Done;
  }

  //
  // Supplied 400KHz clock at init phase above. Record it.
  //
  Private->Slot.CurrentFreq = 400;

  Status = MmcReset (Private);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "MmcReset Fail Status = 0x%x\n", Status));
    goto Done;
  }

  if (Private->Slot.CardType == SdCardType) {
    for (Retry = 0; Retry < SD_VOLTAGE_CHECK_MAX_RETRY; Retry++) {
      Status = SdCardVoltageCheck (Private);
      if (!EFI_ERROR (Status)) {
        DEBUG ((DEBUG_INFO, "SdCardVoltageCheck the %d time Success\n", Retry));
        break;
      }
      MicroSecondDelay (10000);
    }

    if (Retry == SD_VOLTAGE_CHECK_MAX_RETRY) {
      DEBUG ((DEBUG_ERROR, "SdCardVoltageCheck Fail Status = %r\n", Status));
      goto Done;
    }

    CardData->Ocr = 0;
    Status = SdCardSendOpCond (Private, &CardData->Ocr);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "SdCardSendOpCond Fail Status = 0x%x\n", Status));
      goto Done;
    }
  } else {
    CardData->Ocr = 0x40FF8080;
    Status = MmcGetOcr (Private, &CardData->Ocr);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "MmcGetOcr Fail Status = 0x%x\n", Status));
      goto Done;
    }
    DEBUG ((DEBUG_VERBOSE, "MmcGetOcr Ocr = 0x%x\n", CardData->Ocr));
  }

Done:
  return Status;
}

/**
  This function initializes EMMC device

  @param[in]  SdMmcHcPciBase SD/MMC Host Controller's PCI ConfigSpace Base address
  @param[in]  CardType       SD/MMC card type
  @param[in]  MmcInitMode    For the performance optimization,
                             EMMC initialization is separated to early init and the rest of init.

                             DevInitAll        : Execute generic SD device initialization
                             DevInitOnlyPhase1 : Execute only early phase initialization
                             DevInitOnlyPhase2 : Skip early phase initialization,
                                                 and then initialize the rest of initialization

  @retval EFI_SUCCESS           The request is executed successfully.
  @retval EFI_OUT_OF_RESOURCES  The request could not be executed due to a lack of resources.
  @retval Others                The request could not be executed successfully.

**/
EFI_STATUS
SdMmcInitialize (
  IN  UINTN               SdMmcHcPciBase,
  IN  SD_MMC_CARD_TYPE    CardType,
  IN  DEVICE_INIT_PHASE   MmcInitMode
  )
{
  EFI_STATUS                      Status;
  SD_MMC_HC_PRIVATE_DATA         *Private;
  UINT32                          SdMmcHcBase;

  Private = MmcGetHcPrivateData ();
  if (Private == NULL) {
    DEBUG ((DEBUG_ERROR, "Memory Allocation Fail - Out Of Resource\n"));
    Status = EFI_OUT_OF_RESOURCES;
    goto Done;
  }

  SdMmcHcBase = MmioRead32 (SdMmcHcPciBase + PCI_BASE_ADDRESSREG_OFFSET) & 0xFFFFF000;
  if (SdMmcHcBase != 0) {
    // Ensure PCI decoding is on
    MmioOr8 (SdMmcHcPciBase + PCI_COMMAND_OFFSET, EFI_PCI_COMMAND_MEMORY_SPACE | EFI_PCI_COMMAND_BUS_MASTER);
  }
  Private->SdMmcHcPciBase = SdMmcHcPciBase;
  Private->SdMmcHcBase    = SdMmcHcBase;
  if (Private->Signature == SD_MMC_HC_PRIVATE_SIGNATURE) {
    if (Private->Slot.CardType != CardType) {
      ZeroMem (Private, sizeof(SD_MMC_HC_PRIVATE_DATA));
      Private->SdMmcHcBase = SdMmcHcBase;
    }

    if (Private->Slot.Initialized) {
      Status = EFI_SUCCESS;
      goto Done;
    }
  }

  DEBUG ((DEBUG_INFO, "MMC global data init\n"));
  Private->Signature   = SD_MMC_HC_PRIVATE_SIGNATURE;
  Status = SdMmcHcGetCapability (Private->SdMmcHcBase, &Private->Capability);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "SdMmcHcGetCapability Fail Status = 0x%x\n", Status));
    goto Done;
  }

  DEBUG_CODE_BEGIN ();
  DumpCapabilityReg (&Private->Capability);
  DEBUG_CODE_END ();

  if (Private->Capability.Adma2 && Private->Capability.Sdma) {
    DEBUG ((DEBUG_INFO, "Use SDMA instead of ADMA2\n"));
    Private->Capability.Adma2 = 0;
  }

  /* Only support eMMC and SD for now */
  if ((CardType == EmmcCardType) || (CardType == SdCardType)) {
    Private->Slot.CardType = CardType;
  } else {
    DEBUG ((DEBUG_ERROR, "Unsupported card type!\n"));
    goto Done;
  }

  Private->Slot.MediaPresent = TRUE;


  if (MmcInitMode != DevInitOnlyPhase2) {
    Status = EarlyMmcInitialize (Private);
    if (EFI_ERROR (Status)) {
      goto Done;
    }
  }

  if (MmcInitMode != DevInitOnlyPhase1) {
    Status = ContinueMmcInitialize (Private);
    if (EFI_ERROR (Status)) {
      goto Done;
    }
  }

Done:
  if (EFI_ERROR (Status)) {
    if (Private != NULL) {
      if (Private->PrivateDataMemType == PayloadMemory) {
        FreePool ((VOID *)Private);
      }
      SetLibraryData (PcdGet8 (PcdEmmcBlockDeviceLibId), NULL, sizeof (SD_MMC_HC_PRIVATE_DATA));
    }
  }
  return Status;
}


/**
  This function initializes EMMC device

  @param[in]  MmcHcPciBase   MMC Host Controller's PCI ConfigSpace Base address
  @param[in]  MmcInitMode    For the performance optimization,
                             EMMC initialization is separated to early init and the rest of init.

                             DevInitAll        : Execute generic MMC device initialization
                             DevInitOnlyPhase1 : Execute only early phase initialization
                             DevInitOnlyPhase2 : Skip early phase initialization,
                                                 and then initialize the rest of initialization


  @retval EFI_SUCCESS           The request is executed successfully.
  @retval EFI_OUT_OF_RESOURCES  The request could not be executed due to a lack of resources.
  @retval Others                The request could not be executed successfully.

**/
EFI_STATUS
EFIAPI
MmcInitialize (
  IN  UINTN               MmcHcPciBase,
  IN  DEVICE_INIT_PHASE   MmcInitMode
  )
{
  SD_MMC_HC_PRIVATE_DATA   *Private;

  if (MmcInitMode == DevDeinit) {
    // Handle Deinit if required.
    Private = MmcGetHcPrivateData ();
    if (Private != NULL) {
      // Disable controller
      MmioAnd8 (Private->SdMmcHcPciBase + PCI_COMMAND_OFFSET,  (UINT8)(~(EFI_PCI_COMMAND_MEMORY_SPACE | EFI_PCI_COMMAND_BUS_MASTER)));
      ZeroMem (Private, sizeof(SD_MMC_HC_PRIVATE_DATA));
    }
    return EFI_SUCCESS;
  }
  return SdMmcInitialize (MmcHcPciBase, EmmcCardType, MmcInitMode);
}

/**
  This function initializes SD device

  @param[in]  SdHcPciBase    SD Host Controller's PCI ConfigSpace Base address
  @param[in]  SdInitMode     For the performance optimization,
                             SD initialization is separated to early init and the rest of init.

                             DevInitAll        : Execute generic SD device initialization
                             DevInitOnlyPhase1 : Execute only early phase initialization
                             DevInitOnlyPhase2 : Skip early phase initialization,
                                                 and then initialize the rest of initialization

  @retval EFI_SUCCESS           The request is executed successfully.
  @retval EFI_OUT_OF_RESOURCES  The request could not be executed due to a lack of resources.
  @retval Others                The request could not be executed successfully.

**/
EFI_STATUS
EFIAPI
SdInitialize (
  IN  UINTN               SdHcPciBase,
  IN  DEVICE_INIT_PHASE   SdInitMode
  )
{
  SD_MMC_HC_PRIVATE_DATA   *Private;

  if (SdInitMode == DevDeinit) {
    // Handle Deinit if required.
    Private = MmcGetHcPrivateData ();
    if (Private != NULL) {
      ZeroMem (Private, sizeof(SD_MMC_HC_PRIVATE_DATA));
    }
    return EFI_SUCCESS;
  }
  return SdMmcInitialize (SdHcPciBase, SdCardType, SdInitMode);
}
