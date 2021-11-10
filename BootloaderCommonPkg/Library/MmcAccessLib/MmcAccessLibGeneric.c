/** @file
  This file provides some helper functions which are specific for EMMC device.

  Copyright (c) 2015 - 2017, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Library/BaseLib.h>
#include <Library/MmcAccessLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/IoLib.h>
#include <Library/TimerLib.h>
#include <Library/IoMmuLib.h>
#include "SdMmcPciHcDxe.h"
#include "MmcAccessLibPrivate.h"
#include <Library/SecureBootLib.h>
#include <Library/CryptoLib.h>
#include <Library/PrintLib.h>

/**
  Decode and print EMMC CSD Register content.

  @param[in] Csd           Pointer to EMMC_CSD data structure.

  @retval EFI_SUCCESS      The function completed successfully
**/
EFI_STATUS
DumpCsd (
  IN EMMC_CSD  *Csd
  )
{
  DEBUG ((DEBUG_VERBOSE, "== Dump Emmc Csd Register==\n"));
  DEBUG ((DEBUG_VERBOSE, "  CSD structure                    0x%x\n", Csd->CsdStructure));
  DEBUG ((DEBUG_VERBOSE, "  System specification version     0x%x\n", Csd->SpecVers));
  DEBUG ((DEBUG_VERBOSE, "  Data read access-time 1          0x%x\n", Csd->Taac));
  DEBUG ((DEBUG_VERBOSE, "  Data read access-time 2          0x%x\n", Csd->Nsac));
  DEBUG ((DEBUG_VERBOSE, "  Max. bus clock frequency         0x%x\n", Csd->TranSpeed));
  DEBUG ((DEBUG_VERBOSE, "  Device command classes           0x%x\n", Csd->Ccc));
  DEBUG ((DEBUG_VERBOSE, "  Max. read data block length      0x%x\n", Csd->ReadBlLen));
  DEBUG ((DEBUG_VERBOSE, "  Partial blocks for read allowed  0x%x\n", Csd->ReadBlPartial));
  DEBUG ((DEBUG_VERBOSE, "  Write block misalignment         0x%x\n", Csd->WriteBlkMisalign));
  DEBUG ((DEBUG_VERBOSE, "  Read block misalignment          0x%x\n", Csd->ReadBlkMisalign));
  DEBUG ((DEBUG_VERBOSE, "  DSR implemented                  0x%x\n", Csd->DsrImp));
  DEBUG ((DEBUG_VERBOSE, "  Device size                      0x%x\n", Csd->CSizeLow | (Csd->CSizeHigh << 2)));
  DEBUG ((DEBUG_VERBOSE, "  Max. read current @ VDD min      0x%x\n", Csd->VddRCurrMin));
  DEBUG ((DEBUG_VERBOSE, "  Max. read current @ VDD max      0x%x\n", Csd->VddRCurrMax));
  DEBUG ((DEBUG_VERBOSE, "  Max. write current @ VDD min     0x%x\n", Csd->VddWCurrMin));
  DEBUG ((DEBUG_VERBOSE, "  Max. write current @ VDD max     0x%x\n", Csd->VddWCurrMax));
  DEBUG ((DEBUG_VERBOSE, "  Device size multiplier           0x%x\n", Csd->CSizeMult));
  DEBUG ((DEBUG_VERBOSE, "  Erase group size                 0x%x\n", Csd->EraseGrpSize));
  DEBUG ((DEBUG_VERBOSE, "  Erase group size multiplier      0x%x\n", Csd->EraseGrpMult));
  DEBUG ((DEBUG_VERBOSE, "  Write protect group size         0x%x\n", Csd->WpGrpSize));
  DEBUG ((DEBUG_VERBOSE, "  Write protect group enable       0x%x\n", Csd->WpGrpEnable));
  DEBUG ((DEBUG_VERBOSE, "  Manufacturer default ECC         0x%x\n", Csd->DefaultEcc));
  DEBUG ((DEBUG_VERBOSE, "  Write speed factor               0x%x\n", Csd->R2WFactor));
  DEBUG ((DEBUG_VERBOSE, "  Max. write data block length     0x%x\n", Csd->WriteBlLen));
  DEBUG ((DEBUG_VERBOSE, "  Partial blocks for write allowed 0x%x\n", Csd->WriteBlPartial));
  DEBUG ((DEBUG_VERBOSE, "  Content protection application   0x%x\n", Csd->ContentProtApp));
  DEBUG ((DEBUG_VERBOSE, "  File format group                0x%x\n", Csd->FileFormatGrp));
  DEBUG ((DEBUG_VERBOSE, "  Copy flag (OTP)                  0x%x\n", Csd->Copy));
  DEBUG ((DEBUG_VERBOSE, "  Permanent write protection       0x%x\n", Csd->PermWriteProtect));
  DEBUG ((DEBUG_VERBOSE, "  Temporary write protection       0x%x\n", Csd->TmpWriteProtect));
  DEBUG ((DEBUG_VERBOSE, "  File format                      0x%x\n", Csd->FileFormat));
  DEBUG ((DEBUG_VERBOSE, "  ECC code                         0x%x\n", Csd->Ecc));

  return EFI_SUCCESS;
}

/**
  Decode and print EMMC EXT_CSD Register content.

  @param[in] ExtCsd           Pointer to the EMMC_EXT_CSD data structure.

  @retval EFI_SUCCESS         The function completed successfully
**/
EFI_STATUS
DumpExtCsd (
  IN EMMC_EXT_CSD  *ExtCsd
  )
{
  DEBUG ((DEBUG_VERBOSE, "==Dump Emmc ExtCsd Register==\n"));
  DEBUG ((DEBUG_VERBOSE, "  Supported Command Sets                 0x%x\n", ExtCsd->CmdSet));
  DEBUG ((DEBUG_VERBOSE, "  HPI features                           0x%x\n", ExtCsd->HpiFeatures));
  DEBUG ((DEBUG_VERBOSE, "  Background operations support          0x%x\n", ExtCsd->BkOpsSupport));
  DEBUG ((DEBUG_VERBOSE, "  Background operations status           0x%x\n", ExtCsd->BkopsStatus));
  DEBUG ((DEBUG_VERBOSE, "  Number of correctly programmed sectors 0x%x\n",
          * ((UINT32 *)&ExtCsd->CorrectlyPrgSectorsNum[0])));
  DEBUG ((DEBUG_VERBOSE, "  Initialization time after partitioning 0x%x\n", ExtCsd->IniTimeoutAp));
  DEBUG ((DEBUG_VERBOSE, "  TRIM Multiplier                        0x%x\n", ExtCsd->TrimMult));
  DEBUG ((DEBUG_VERBOSE, "  Secure Feature support                 0x%x\n", ExtCsd->SecFeatureSupport));
  DEBUG ((DEBUG_VERBOSE, "  Secure Erase Multiplier                0x%x\n", ExtCsd->SecEraseMult));
  DEBUG ((DEBUG_VERBOSE, "  Secure TRIM Multiplier                 0x%x\n", ExtCsd->SecTrimMult));
  DEBUG ((DEBUG_VERBOSE, "  Boot information                       0x%x\n", ExtCsd->BootInfo));
  DEBUG ((DEBUG_VERBOSE, "  Boot partition size                    0x%x\n", ExtCsd->BootSizeMult));
  DEBUG ((DEBUG_VERBOSE, "  Access size                            0x%x\n", ExtCsd->AccSize));
  DEBUG ((DEBUG_VERBOSE, "  High-capacity erase unit size          0x%x\n", ExtCsd->HcEraseGrpSize));
  DEBUG ((DEBUG_VERBOSE, "  High-capacity erase timeout            0x%x\n", ExtCsd->EraseTimeoutMult));
  DEBUG ((DEBUG_VERBOSE, "  Reliable write sector count            0x%x\n", ExtCsd->RelWrSecC));
  DEBUG ((DEBUG_VERBOSE, "  High-capacity write protect group size 0x%x\n", ExtCsd->HcWpGrpSize));
  DEBUG ((DEBUG_VERBOSE, "  Sleep/awake timeout                    0x%x\n", ExtCsd->SATimeout));
  DEBUG ((DEBUG_VERBOSE, "  Sector Count                           0x%x\n", * ((UINT32 *)&ExtCsd->SecCount[0])));
  DEBUG ((DEBUG_VERBOSE, "  Partition switching timing             0x%x\n", ExtCsd->PartitionSwitchTime));
  DEBUG ((DEBUG_VERBOSE, "  Out-of-interrupt busy timing           0x%x\n", ExtCsd->OutOfInterruptTime));
  DEBUG ((DEBUG_VERBOSE, "  I/O Driver Strength                    0x%x\n", ExtCsd->DriverStrength));
  DEBUG ((DEBUG_VERBOSE, "  Device type                            0x%x\n", ExtCsd->DeviceType));
  DEBUG ((DEBUG_VERBOSE, "  CSD STRUCTURE                          0x%x\n", ExtCsd->CsdStructure));
  DEBUG ((DEBUG_VERBOSE, "  Extended CSD revision                  0x%x\n", ExtCsd->ExtCsdRev));
  DEBUG ((DEBUG_VERBOSE, "  Command set                            0x%x\n", ExtCsd->CmdSet));
  DEBUG ((DEBUG_VERBOSE, "  Command set revision                   0x%x\n", ExtCsd->CmdSetRev));
  DEBUG ((DEBUG_VERBOSE, "  Power class                            0x%x\n", ExtCsd->PowerClass));
  DEBUG ((DEBUG_VERBOSE, "  High-speed interface timing            0x%x\n", ExtCsd->HsTiming));
  DEBUG ((DEBUG_VERBOSE, "  Bus width mode                         0x%x\n", ExtCsd->BusWidth));
  DEBUG ((DEBUG_VERBOSE, "  Erased memory content                  0x%x\n", ExtCsd->ErasedMemCont));
  DEBUG ((DEBUG_VERBOSE, "  Partition configuration                0x%x\n", ExtCsd->PartitionConfig));
  DEBUG ((DEBUG_VERBOSE, "  Boot config protection                 0x%x\n", ExtCsd->BootConfigProt));
  DEBUG ((DEBUG_VERBOSE, "  Boot bus Conditions                    0x%x\n", ExtCsd->BootBusConditions));
  DEBUG ((DEBUG_VERBOSE, "  High-density erase group definition    0x%x\n", ExtCsd->EraseGroupDef));
  DEBUG ((DEBUG_VERBOSE, "  Boot write protection status register  0x%x\n", ExtCsd->BootWpStatus));
  DEBUG ((DEBUG_VERBOSE, "  Boot area write protection register    0x%x\n", ExtCsd->BootWp));
  DEBUG ((DEBUG_VERBOSE, "  User area write protection register    0x%x\n", ExtCsd->UserWp));
  DEBUG ((DEBUG_VERBOSE, "  FW configuration                       0x%x\n", ExtCsd->FwConfig));
  DEBUG ((DEBUG_VERBOSE, "  RPMB Size                              0x%x\n", ExtCsd->RpmbSizeMult));
  DEBUG ((DEBUG_VERBOSE, "  H/W reset function                     0x%x\n", ExtCsd->RstFunction));
  DEBUG ((DEBUG_VERBOSE, "  Partitioning Support                   0x%x\n", ExtCsd->PartitioningSupport));
  DEBUG ((DEBUG_VERBOSE, "  Max Enhanced Area Size                 0x%02x%02x%02x\n", \
          ExtCsd->MaxEnhSizeMult[2], ExtCsd->MaxEnhSizeMult[1], ExtCsd->MaxEnhSizeMult[0]));
  DEBUG ((DEBUG_VERBOSE, "  Partitions attribute                   0x%x\n", ExtCsd->PartitionsAttribute));
  DEBUG ((DEBUG_VERBOSE, "  Partitioning Setting                   0x%x\n", ExtCsd->PartitionSettingCompleted));
  DEBUG ((DEBUG_VERBOSE, "  General Purpose Partition 1 Size       0x%02x%02x%02x\n", \
          ExtCsd->GpSizeMult[2], ExtCsd->GpSizeMult[1], ExtCsd->GpSizeMult[0]));
  DEBUG ((DEBUG_VERBOSE, "  General Purpose Partition 2 Size       0x%02x%02x%02x\n", \
          ExtCsd->GpSizeMult[5], ExtCsd->GpSizeMult[4], ExtCsd->GpSizeMult[3]));
  DEBUG ((DEBUG_VERBOSE, "  General Purpose Partition 3 Size       0x%02x%02x%02x\n", \
          ExtCsd->GpSizeMult[8], ExtCsd->GpSizeMult[7], ExtCsd->GpSizeMult[6]));
  DEBUG ((DEBUG_VERBOSE, "  General Purpose Partition 4 Size       0x%02x%02x%02x\n", \
          ExtCsd->GpSizeMult[11], ExtCsd->GpSizeMult[10], ExtCsd->GpSizeMult[9]));
  DEBUG ((DEBUG_VERBOSE, "  Enhanced User Data Area Size           0x%02x%02x%02x\n", \
          ExtCsd->EnhSizeMult[2], ExtCsd->EnhSizeMult[1], ExtCsd->EnhSizeMult[0]));
  DEBUG ((DEBUG_VERBOSE, "  Enhanced User Data Start Address       0x%x\n", * ((UINT32 *)&ExtCsd->EnhStartAddr[0])));
  DEBUG ((DEBUG_VERBOSE, "  Bad Block Management mode              0x%x\n", ExtCsd->SecBadBlkMgmnt));
  DEBUG ((DEBUG_VERBOSE, "  Native sector size                     0x%x\n", ExtCsd->NativeSectorSize));
  DEBUG ((DEBUG_VERBOSE, "  Sector size emulation                  0x%x\n", ExtCsd->UseNativeSector));
  DEBUG ((DEBUG_VERBOSE, "  Sector size                            0x%x\n", ExtCsd->DataSectorSize));

  return EFI_SUCCESS;
}

/**
  Broadcast command ALL_SEND_CID to the bus to ask all the EMMC devices to send the
  data of their CID registers.

  Refer to EMMC Electrical Standard Spec 5.1 Section 6.4 for details.

  @param[in]  Private       A pointer to the SD_MMC_HC_PRIVATE_DATA instance.

  @retval EFI_SUCCESS       The operation is done correctly.
  @retval Others            The operation fails.

**/
EFI_STATUS
MmcGetAllCid (
  IN     SD_MMC_HC_PRIVATE_DATA         *Private
  )
{
  EFI_SD_MMC_COMMAND_BLOCK              SdMmcCmdBlk;
  EFI_SD_MMC_STATUS_BLOCK               SdMmcStatusBlk;
  EFI_SD_MMC_PASS_THRU_COMMAND_PACKET   Packet;
  EFI_STATUS                            Status;
  EMMC_CARD_DATA                       *CardData;

  ZeroMem (&SdMmcCmdBlk, sizeof (SdMmcCmdBlk));
  ZeroMem (&SdMmcStatusBlk, sizeof (SdMmcStatusBlk));
  ZeroMem (&Packet, sizeof (Packet));

  Packet.SdMmcCmdBlk    = &SdMmcCmdBlk;
  Packet.SdMmcStatusBlk = &SdMmcStatusBlk;
  Packet.Timeout        = SD_MMC_HC_GENERIC_TIMEOUT;

  SdMmcCmdBlk.CommandIndex = EMMC_ALL_SEND_CID;
  SdMmcCmdBlk.CommandType  = SdMmcCommandTypeBcr;
  SdMmcCmdBlk.ResponseType = SdMmcResponseTypeR2;
  SdMmcCmdBlk.CommandArgument = 0;

  Status = SdMmcSendCommand (Private, &Packet);
  if (!EFI_ERROR (Status)) {
    CardData = (EMMC_CARD_DATA *) Private->Slot.CardData;
    CopyMem (((UINT8 *)&CardData->Cid) + 1, &SdMmcStatusBlk.Resp0, sizeof (EMMC_CID) - 1);
  }

  return Status;
}

/**
  Send command SET_RELATIVE_ADDR to the EMMC device to assign a Relative device
  Address (RCA).

  Refer to EMMC Electrical Standard Spec 5.1 Section 6.4 for details.

  @param[in] Private        A pointer to the SD_MMC_HC_PRIVATE_DATA instance.
  @param[in] Rca            The relative device address to be assigned.

  @retval EFI_SUCCESS       The operation is done correctly.
  @retval Others            The operation fails.

**/
EFI_STATUS
MmcSetRca (
  IN SD_MMC_HC_PRIVATE_DATA            *Private,
  IN UINT16                             Rca
  )
{
  EFI_SD_MMC_COMMAND_BLOCK              SdMmcCmdBlk;
  EFI_SD_MMC_STATUS_BLOCK               SdMmcStatusBlk;
  EFI_SD_MMC_PASS_THRU_COMMAND_PACKET   Packet;
  EFI_STATUS                            Status;
  EMMC_CARD_DATA                       *CardData;

  ZeroMem (&SdMmcCmdBlk, sizeof (SdMmcCmdBlk));
  ZeroMem (&SdMmcStatusBlk, sizeof (SdMmcStatusBlk));
  ZeroMem (&Packet, sizeof (Packet));

  Packet.SdMmcCmdBlk    = &SdMmcCmdBlk;
  Packet.SdMmcStatusBlk = &SdMmcStatusBlk;
  Packet.Timeout        = SD_MMC_HC_GENERIC_TIMEOUT;

  SdMmcCmdBlk.CommandIndex = EMMC_SET_RELATIVE_ADDR;
  SdMmcCmdBlk.CommandType  = SdMmcCommandTypeAc;
  SdMmcCmdBlk.ResponseType = SdMmcResponseTypeR1;
  SdMmcCmdBlk.CommandArgument = (UINT32)Rca << 16;

  Status = SdMmcSendCommand (Private, &Packet);
  if (!EFI_ERROR (Status)) {
    CardData = (EMMC_CARD_DATA *) Private->Slot.CardData;
    CardData->Address = SdMmcStatusBlk.Resp0 >> 16;
  }

  return Status;
}

/**
  Send command SEND_CSD to the EMMC device to get the data of the CSD register.

  Refer to EMMC Electrical Standard Spec 5.1 Section 6.10.4 for details.

  @param[in]  Private       A pointer to the SD_MMC_HC_PRIVATE_DATA instance.
  @param[in]  Rca           The relative device address of selected device.

  @retval EFI_SUCCESS       The operation is done correctly.
  @retval Others            The operation fails.

**/
EFI_STATUS
MmcGetCsd (
  IN  SD_MMC_HC_PRIVATE_DATA        *Private,
  IN  UINT16                         Rca
  )
{
  EFI_SD_MMC_COMMAND_BLOCK              SdMmcCmdBlk;
  EFI_SD_MMC_STATUS_BLOCK               SdMmcStatusBlk;
  EFI_SD_MMC_PASS_THRU_COMMAND_PACKET   Packet;
  EFI_STATUS                            Status;
  EMMC_CARD_DATA                       *CardData;

  ZeroMem (&SdMmcCmdBlk, sizeof (SdMmcCmdBlk));
  ZeroMem (&SdMmcStatusBlk, sizeof (SdMmcStatusBlk));
  ZeroMem (&Packet, sizeof (Packet));

  Packet.SdMmcCmdBlk    = &SdMmcCmdBlk;
  Packet.SdMmcStatusBlk = &SdMmcStatusBlk;
  Packet.Timeout        = SD_MMC_HC_GENERIC_TIMEOUT;

  SdMmcCmdBlk.CommandIndex = EMMC_SEND_CSD;
  SdMmcCmdBlk.CommandType  = SdMmcCommandTypeAc;
  SdMmcCmdBlk.ResponseType = SdMmcResponseTypeR2;
  SdMmcCmdBlk.CommandArgument = (UINT32)Rca << 16;

  Status = SdMmcSendCommand (Private, &Packet);
  if (!EFI_ERROR (Status)) {
    //
    // For details, refer to SD Host Controller Simplified Spec 3.0 Table 2-12.
    //
    CardData = (EMMC_CARD_DATA *) Private->Slot.CardData;
    CopyMem (((UINT8 *)&CardData->Csd) + 1, &SdMmcStatusBlk.Resp0, sizeof (EMMC_CSD) - 1);
  }

  return Status;
}

/**
  Send command SELECT_DESELECT_CARD to the EMMC device to select/deselect it.

  Refer to EMMC Electrical Standard Spec 5.1 Section 6.10.4 for details.

  @param[in]  Private       A pointer to the SD_MMC_HC_PRIVATE_DATA instance.
  @param[in]  Rca           The relative device address of selected device.

  @retval EFI_SUCCESS       The operation is done correctly.
  @retval Others            The operation fails.

**/
EFI_STATUS
MmcSelect (
  IN SD_MMC_HC_PRIVATE_DATA            *Private,
  IN UINT16                             Rca
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

  SdMmcCmdBlk.CommandIndex = EMMC_SELECT_DESELECT_CARD;
  SdMmcCmdBlk.CommandType  = SdMmcCommandTypeAc;
  SdMmcCmdBlk.ResponseType = SdMmcResponseTypeR1;
  SdMmcCmdBlk.CommandArgument = (UINT32)Rca << 16;

  Status = SdMmcSendCommand (Private, &Packet);

  return Status;
}

/**
  Send command SEND_EXT_CSD to the EMMC device to get the data of the EXT_CSD register.

  Refer to EMMC Electrical Standard Spec 5.1 Section 6.10.4 for details.

  @param[in]  Private       A pointer to the SD_MMC_HC_PRIVATE_DATA instance.

  @retval EFI_SUCCESS       The operation is done correctly.
  @retval Others            The operation fails.

**/
EFI_STATUS
MmcGetExtCsd (
  IN  SD_MMC_HC_PRIVATE_DATA            *Private
  )
{
  EFI_SD_MMC_COMMAND_BLOCK              SdMmcCmdBlk;
  EFI_SD_MMC_STATUS_BLOCK               SdMmcStatusBlk;
  EFI_SD_MMC_PASS_THRU_COMMAND_PACKET   Packet;
  EFI_STATUS                            Status;
  EMMC_CARD_DATA                       *CardData;
  EMMC_EXT_CSD                         *ExtCsd;

  CardData = (EMMC_CARD_DATA *)Private->Slot.CardData;
  ExtCsd   = &CardData->ExtCsd;

  ZeroMem (&SdMmcCmdBlk, sizeof (SdMmcCmdBlk));
  ZeroMem (&SdMmcStatusBlk, sizeof (SdMmcStatusBlk));
  ZeroMem (&Packet, sizeof (Packet));

  Packet.SdMmcCmdBlk    = &SdMmcCmdBlk;
  Packet.SdMmcStatusBlk = &SdMmcStatusBlk;
  Packet.Timeout        = SD_MMC_HC_GENERIC_TIMEOUT;

  SdMmcCmdBlk.CommandIndex = EMMC_SEND_EXT_CSD;
  SdMmcCmdBlk.CommandType  = SdMmcCommandTypeAdtc;
  SdMmcCmdBlk.ResponseType = SdMmcResponseTypeR1;
  SdMmcCmdBlk.CommandArgument = 0x00000000;

  Packet.InDataBuffer     = ExtCsd;
  Packet.InTransferLength = sizeof (EMMC_EXT_CSD);

  Status = SdMmcSendCommand (Private, &Packet);

  return Status;
}

/**
  Set the specified EXT_CSD register field through sync or async I/O request.

  @param[in]  Private           A pointer to the SD_MMC_HC_PRIVATE_DATA instance.
  @param[in]  Offset            The offset of the specified field in EXT_CSD register.
  @param[in]  Value             The byte value written to the field specified by Offset.

  @retval EFI_SUCCESS           The request is executed successfully.
  @retval Others                The request could not be executed successfully.

**/
EFI_STATUS
MmcSetExtCsd (
  IN  SD_MMC_HC_PRIVATE_DATA   *Private,
  IN  UINT8                     Offset,
  IN  UINT8                     Value
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

  SdMmcCmdBlk.CommandIndex = EMMC_SWITCH;
  SdMmcCmdBlk.CommandType  = SdMmcCommandTypeAc;
  SdMmcCmdBlk.ResponseType = SdMmcResponseTypeR1b;
  SdMmcCmdBlk.CommandArgument = (Value << 8) | (Offset << 16) | BIT24 | BIT25;

  Status = SdMmcSendCommand (Private, &Packet);

  return Status;
}

/**
  Send command SWITCH to the EMMC device to switch the mode of operation of the
  selected Device or modifies the EXT_CSD registers.

  Refer to EMMC Electrical Standard Spec 5.1 Section 6.10.4 for details.

  @param[in]  Private       A pointer to the SD_MMC_HC_PRIVATE_DATA instance.
  @param[in]  Access        The access mode of SWTICH command.
  @param[in]  Index         The offset of the field to be access.
  @param[in]  Value         The value to be set to the specified field of EXT_CSD register.
  @param[in]  CmdSet        The value of CmdSet field of EXT_CSD register.

  @retval EFI_SUCCESS       The operation is done correctly.
  @retval Others            The operation fails.

**/
EFI_STATUS
MmcSwitch (
  IN SD_MMC_HC_PRIVATE_DATA            *Private,
  IN UINT8                              Access,
  IN UINT8                              Index,
  IN UINT8                              Value,
  IN UINT8                              CmdSet
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

  SdMmcCmdBlk.CommandIndex = EMMC_SWITCH;
  SdMmcCmdBlk.CommandType  = SdMmcCommandTypeAc;
  SdMmcCmdBlk.ResponseType = SdMmcResponseTypeR1b;
  SdMmcCmdBlk.CommandArgument = (Access << 24) | (Index << 16) | (Value << 8) | CmdSet;

  Status = SdMmcSendCommand (Private, &Packet);

  return Status;
}

/**
  Send command SEND_STATUS to the addressed EMMC device to get its status register.

  Refer to EMMC Electrical Standard Spec 5.1 Section 6.10.4 for details.

  @param[in]  Private       A pointer to the SD_MMC_HC_PRIVATE_DATA instance.
  @param[in]  Rca           The relative device address of addressed device.
  @param[out] DevStatus     The returned device status.

  @retval EFI_SUCCESS       The operation is done correctly.
  @retval Others            The operation fails.

**/
EFI_STATUS
MmcSendStatus (
  IN  SD_MMC_HC_PRIVATE_DATA         *Private,
  IN  UINT16                          Rca,
  OUT UINT32                         *DevStatus
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

  SdMmcCmdBlk.CommandIndex = EMMC_SEND_STATUS;
  SdMmcCmdBlk.CommandType  = SdMmcCommandTypeAc;
  SdMmcCmdBlk.ResponseType = SdMmcResponseTypeR1;
  SdMmcCmdBlk.CommandArgument = (UINT32)Rca << 16;

  Status = SdMmcSendCommand (Private, &Packet);
  if (!EFI_ERROR (Status)) {
    *DevStatus = SdMmcStatusBlk.Resp0;
  }

  return Status;
}

/**
  Send command SEND_TUNING_BLOCK to the EMMC device for HS200 optimal sampling point
  detection.

  It may be sent up to 40 times until the host finishes the tuning procedure.

  Refer to EMMC Electrical Standard Spec 5.1 Section 6.6.8 for details.

  @param[in] Private        A pointer to the SD_MMC_HC_PRIVATE_DATA instance.
  @param[in] BusWidth       The bus width to work.

  @retval EFI_SUCCESS       The operation is done correctly.
  @retval Others            The operation fails.

**/
EFI_STATUS
MmcSendTuningBlk (
  IN SD_MMC_HC_PRIVATE_DATA            *Private,
  IN UINT8                              BusWidth
  )
{
  EFI_SD_MMC_COMMAND_BLOCK              SdMmcCmdBlk;
  EFI_SD_MMC_STATUS_BLOCK               SdMmcStatusBlk;
  EFI_SD_MMC_PASS_THRU_COMMAND_PACKET   Packet;
  EFI_STATUS                            Status;
  UINT8                                 TuningBlock[128];

  ZeroMem (&SdMmcCmdBlk, sizeof (SdMmcCmdBlk));
  ZeroMem (&SdMmcStatusBlk, sizeof (SdMmcStatusBlk));
  ZeroMem (&Packet, sizeof (Packet));

  Packet.SdMmcCmdBlk    = &SdMmcCmdBlk;
  Packet.SdMmcStatusBlk = &SdMmcStatusBlk;
  Packet.Timeout        = SD_MMC_HC_GENERIC_TIMEOUT;

  SdMmcCmdBlk.CommandIndex = EMMC_SEND_TUNING_BLOCK;
  SdMmcCmdBlk.CommandType  = SdMmcCommandTypeAdtc;
  SdMmcCmdBlk.ResponseType = SdMmcResponseTypeR1;
  SdMmcCmdBlk.CommandArgument = 0;

  Packet.InDataBuffer = TuningBlock;
  if (BusWidth == 8) {
    Packet.InTransferLength = sizeof (TuningBlock);
  } else {
    Packet.InTransferLength = 64;
  }

  Status = SdMmcSendCommand (Private, &Packet);

  return Status;
}

/**
  Tunning the clock to get HS200 optimal sampling point.

  Command SEND_TUNING_BLOCK may be sent up to 40 times until the host finishes the
  tuning procedure.

  Refer to EMMC Electrical Standard Spec 5.1 Section 6.6.8 and SD Host Controller
  Simplified Spec 3.0 Figure 2-29 for details.

  @param[in] Private        A pointer to the SD_MMC_HC_PRIVATE_DATA instance.
  @param[in] BusWidth       The bus width to work.

  @retval EFI_SUCCESS       The operation is done correctly.
  @retval Others            The operation fails.

**/
EFI_STATUS
MmcTuningClkForHs200 (
  IN SD_MMC_HC_PRIVATE_DATA            *Private,
  IN UINT8                              BusWidth
  )
{
  EFI_STATUS          Status;
  UINT8               HostCtrl2;
  UINT8               Retry;

  //
  // Notify the host that the sampling clock tuning procedure starts.
  //
  HostCtrl2 = BIT6;
  Status = SdMmcHcOrMmio (Private->SdMmcHcBase, SD_MMC_HC_HOST_CTRL2, sizeof (HostCtrl2), &HostCtrl2);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  //
  // Ask the device to send a sequence of tuning blocks till the tuning procedure is done.
  //
  Retry = 0;
  do {
    Status = MmcSendTuningBlk (Private, BusWidth);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "MmcTuningClkForHs200: Send tuning block fails with %r\n", Status));
      return Status;
    }

    Status = SdMmcHcRwMmio (Private->SdMmcHcBase, SD_MMC_HC_HOST_CTRL2, TRUE, sizeof (HostCtrl2), &HostCtrl2);
    if (EFI_ERROR (Status)) {
      return Status;
    }

    if ((HostCtrl2 & (BIT6 | BIT7)) == 0) {
      break;
    }

    if ((HostCtrl2 & (BIT6 | BIT7)) == BIT7) {
      return EFI_SUCCESS;
    }
  } while (++Retry < 40);

  DEBUG ((DEBUG_ERROR, "MmcTuningClkForHs200: Send tuning block fails at %d times with HostCtrl2 %02x\n", Retry,
          HostCtrl2));
  //
  // Abort the tuning procedure and reset the tuning circuit.
  //
  HostCtrl2 = (UINT8)~ (BIT6 | BIT7);
  Status = SdMmcHcAndMmio (Private->SdMmcHcBase, SD_MMC_HC_HOST_CTRL2, sizeof (HostCtrl2), &HostCtrl2);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  return EFI_DEVICE_ERROR;
}

/**
  Check the SWITCH operation status.

  @param[in] Private   A pointer to the SD_MMC_HC_PRIVATE_DATA instance.
  @param[in] Rca       The relative device address.

  @retval EFI_SUCCESS  The SWITCH finished siccessfully.
  @retval others       The SWITCH failed.
**/
EFI_STATUS
MmcCheckSwitchStatus (
  IN SD_MMC_HC_PRIVATE_DATA        *Private,
  IN UINT16                         Rca
  )
{
  EFI_STATUS  Status;
  UINT32      DevStatus;

  Status = MmcSendStatus (Private, Rca, &DevStatus);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "MmcCheckSwitchStatus: Send status fails with %r\n", Status));
    return Status;
  }

  //
  // Check the switch operation is really successful or not.
  //
  if ((DevStatus & BIT7) != 0) {
    DEBUG ((DEBUG_ERROR, "MmcCheckSwitchStatus: The switch operation fails as DevStatus is 0x%08x\n", DevStatus));
    return EFI_DEVICE_ERROR;
  }

  return EFI_SUCCESS;
}

/**
  Switch the bus width to specified width.

  Refer to EMMC Electrical Standard Spec 5.1 Section 6.6.9 and SD Host Controller
  Simplified Spec 3.0 Figure 3-7 for details.

  @param[in] Private        A pointer to the SD_MMC_HC_PRIVATE_DATA instance.
  @param[in] Rca            The relative device address to be assigned.
  @param[in] IsDdr          If TRUE, use dual data rate data simpling method. Otherwise
                            use single data rate data simpling method.
  @param[in] BusWidth       The bus width to be set, it could be 4 or 8.

  @retval EFI_SUCCESS       The operation is done correctly.
  @retval Others            The operation fails.

**/
EFI_STATUS
MmcSwitchBusWidth (
  IN SD_MMC_HC_PRIVATE_DATA            *Private,
  IN UINT16                             Rca,
  IN BOOLEAN                            IsDdr,
  IN UINT8                              BusWidth
  )
{
  EFI_STATUS          Status;
  UINT8               Access;
  UINT8               Index;
  UINT8               Value;
  UINT8               CmdSet;

  //
  // Write Byte, the Value field is written into the byte pointed by Index.
  //
  Access = 0x03;
  Index  = OFFSET_OF (EMMC_EXT_CSD, BusWidth);
  if (BusWidth == 4) {
    Value = 1;
  } else if (BusWidth == 8) {
    Value = 2;
  } else {
    return EFI_INVALID_PARAMETER;
  }

  if (IsDdr) {
    Value += 4;
  }

  CmdSet = 0;
  Status = MmcSwitch (Private, Access, Index, Value, CmdSet);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "MmcSwitchBusWidth: Switch to bus width %d fails with %r\n", BusWidth, Status));
    return Status;
  }

  Status = MmcCheckSwitchStatus (Private, Rca);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "MmcCheckSwitchStatus: Check status fails with %r\n", Status));
    return Status;
  }

  Status = SdMmcHcSetBusWidth (Private->SdMmcHcBase, BusWidth);

  return Status;
}

/**
  Switch the clock frequency to the specified value.

  Refer to EMMC Electrical Standard Spec 5.1 Section 6.6 and SD Host Controller
  Simplified Spec 3.0 Figure 3-3 for details.

  @param[in] Private        A pointer to the SD_MMC_HC_PRIVATE_DATA instance.
  @param[in] Rca            The relative device address to be assigned.
  @param[in] HsTiming       The value to be written to HS_TIMING field of EXT_CSD register.
  @param[in] ClockFreq      The max clock frequency to be set, the unit is MHz.

  @retval EFI_SUCCESS       The operation is done correctly.
  @retval Others            The operation fails.

**/
EFI_STATUS
MmcSwitchClockFreq (
  IN SD_MMC_HC_PRIVATE_DATA            *Private,
  IN UINT16                             Rca,
  IN UINT8                              HsTiming,
  IN UINT32                             ClockFreq
  )
{
  EFI_STATUS                Status;
  UINT8                     Access;
  UINT8                     Index;
  UINT8                     Value;
  UINT8                     CmdSet;
  UINT32                    DevStatus;

  //
  // Write Byte, the Value field is written into the byte pointed by Index.
  //
  Access = 0x03;
  Index  = OFFSET_OF (EMMC_EXT_CSD, HsTiming);
  Value  = HsTiming;
  CmdSet = 0;

  Status = MmcSwitch (Private, Access, Index, Value, CmdSet);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "MmcSwitchClockFreq: Switch to hstiming %d fails with %r\n", HsTiming, Status));
    return Status;
  }

  Status = MmcSendStatus (Private, Rca, &DevStatus);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "MmcSwitchClockFreq: Send status fails with %r\n", Status));
    return Status;
  }
  //
  // Check the switch operation is really successful or not.
  //
  if ((DevStatus & BIT7) != 0) {
    DEBUG ((DEBUG_ERROR, "MmcSwitchClockFreq: The switch operation fails as DevStatus is 0x%08x\n", DevStatus));
    return EFI_DEVICE_ERROR;
  }

  //
  // Convert the clock freq unit from MHz to KHz. And record current clock.
  //
  Status = SdMmcHcClockSupply (Private->SdMmcHcBase, ClockFreq * 1000, Private->Capability);
  Private->Slot.CurrentFreq = ClockFreq * 1000;

  return Status;
}

/**
  Switch the bus timing and clock frequency.

  Refer to EMMC Electrical Standard Spec 5.1 Section 6.6 and SD Host Controller
  Simplified Spec 3.0 Figure 3-3 for details.

  @param[in] Private         A pointer to the SD_MMC_HC_PRIVATE_DATA instance.
  @param[in] Rca             The relative device address to be assigned.
  @param[in] HsTiming        The value to be written to HS_TIMING field of EXT_CSD register.
  @param[in] ClockFreq       The max clock frequency to be set, the unit is MHz.

  @retval EFI_SUCCESS       The operation is done correctly.
  @retval Others            The operation fails.

**/
EFI_STATUS
MmcSwitchBusTiming (
  IN SD_MMC_HC_PRIVATE_DATA            *Private,
  IN UINT16                             Rca,
  IN UINT8                              HsTiming,
  IN UINT32                             ClockFreq
  )
{
  EFI_STATUS                Status;
  UINT8                     Access;
  UINT8                     Index;
  UINT8                     Value;
  UINT8                     CmdSet;
  UINT8                     HostCtrl1;
  UINT8                     HostCtrl2;
  BOOLEAN                   DelaySendStatus;

  //
  // Write Byte, the Value field is written into the byte pointed by Index.
  //
  Access = 0x03;
  Index  = OFFSET_OF (EMMC_EXT_CSD, HsTiming);
  CmdSet = 0;
  switch (HsTiming) {
    case 3:
      Value = (UINT8)((0 << 4) | 3); // DriverStrength = 0
      break;
    case 2:
      Value = (UINT8)((0 << 4) | 2); // DriverStrength = 0
      break;
    case 1:
      Value = 1;
      break;
    case 0:
      Value = 0;
      break;
    default:
      DEBUG ((DEBUG_ERROR, "MmcSwitchBusTiming: Unsupported HsTiming(%d)\n", HsTiming));
      return EFI_INVALID_PARAMETER;
  }

  Status = MmcSwitch (Private, Access, Index, Value, CmdSet);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "MmcSwitchBusTiming: Switch to bus timing %d fails with %r\n", HsTiming, Status));
    return Status;
  }

  if (HsTiming == 1) {
    HostCtrl1 = BIT2;
    Status = SdMmcHcOrMmio (Private->SdMmcHcBase, SD_MMC_HC_HOST_CTRL1, sizeof (HostCtrl1), &HostCtrl1);
    if (EFI_ERROR (Status)) {
      return Status;
    }
  } else {
    HostCtrl1 = (UINT8)~BIT2;
    Status = SdMmcHcAndMmio (Private->SdMmcHcBase, SD_MMC_HC_HOST_CTRL1, sizeof (HostCtrl1), &HostCtrl1);
    if (EFI_ERROR (Status)) {
      return Status;
    }
  }

  //
  // Clean UHS Mode Select field of Host Control 2 reigster before update
  //
  HostCtrl2 = (UINT8)~0x7;
  Status = SdMmcHcAndMmio (Private->SdMmcHcBase, SD_MMC_HC_HOST_CTRL2, sizeof (HostCtrl2), &HostCtrl2);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Set UHS Mode Select field of Host Control 2 reigster to HS200/HS400
  //
  if (HsTiming == 2) { // HS200
    HostCtrl2 = BIT0 | BIT1;
  }
  if (HsTiming == 3) { // HS400
    HostCtrl2 = BIT0 | BIT2;
  }
  Status = SdMmcHcOrMmio (Private->SdMmcHcBase, SD_MMC_HC_HOST_CTRL2, sizeof (HostCtrl2), &HostCtrl2);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // For cases when we switch bus timing to higher mode from current we want to
  // send SEND_STATUS at current, lower, frequency then the target frequency to avoid
  // stability issues. It has been observed that some designs are unable to process the
  // SEND_STATUS at higher frequency during switch to HS200 @200MHz irrespective of the number of retries
  // and only running the clock tuning is able to make them work at target frequency.
  //
  // For cases when we are downgrading the frequency and current high frequency is invalid
  // we have to first change the frequency to target frequency and then send the SEND_STATUS.
  //
  if (Private->Slot.CurrentFreq < (ClockFreq * 1000)) {
    Status = MmcCheckSwitchStatus (Private, Rca);
    if (EFI_ERROR (Status)) {
      return Status;
    }
    DelaySendStatus = FALSE;
  } else {
    DelaySendStatus = TRUE;
  }

  //
  // Convert the clock freq unit from MHz to KHz.
  //
  Status = SdMmcHcClockSupply (Private->SdMmcHcBase, ClockFreq * 1000, Private->Capability);
  Private->Slot.CurrentFreq = ClockFreq * 1000;
  if (EFI_ERROR (Status)) {
    return Status;
  }

  if (DelaySendStatus) {
    Status = MmcCheckSwitchStatus (Private, Rca);
    if (EFI_ERROR (Status)) {
      return Status;
    }
  }

  return Status;
}

/**
  Switch to the High Speed timing according to request.

  Refer to EMMC Electrical Standard Spec 5.1 Section 6.6.8 and SD Host Controller
  Simplified Spec 3.0 Figure 2-29 for details.

  @param[in] Private        A pointer to the SD_MMC_HC_PRIVATE_DATA instance.
  @param[in] Rca            The relative device address to be assigned.
  @param[in] ClockFreq      The max clock frequency to be set.
  @param[in] IsDdr          If TRUE, use dual data rate data simpling method. Otherwise
                            use single data rate data simpling method.
  @param[in] BusWidth       The bus width to be set, it could be 4 or 8.

  @retval EFI_SUCCESS       The operation is done correctly.
  @retval Others            The operation fails.

**/
EFI_STATUS
MmcSwitchToHighSpeed (
  IN SD_MMC_HC_PRIVATE_DATA            *Private,
  IN UINT16                             Rca,
  IN UINT32                             ClockFreq,
  IN BOOLEAN                            IsDdr,
  IN UINT8                              BusWidth
  )
{
  EFI_STATUS          Status;
  UINT8               HsTiming;
  UINT8               HostCtrl1;
  UINT8               HostCtrl2;

  Status = MmcSwitchBusWidth (Private, Rca, IsDdr, BusWidth);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Set to High Speed timing
  //
  HostCtrl1 = BIT2;
  Status = SdMmcHcOrMmio (Private->SdMmcHcBase, SD_MMC_HC_HOST_CTRL1, sizeof (HostCtrl1), &HostCtrl1);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Clean UHS Mode Select field of Host Control 2 reigster before update
  //
  HostCtrl2 = (UINT8)~0x7;
  Status = SdMmcHcAndMmio (Private->SdMmcHcBase, SD_MMC_HC_HOST_CTRL2, sizeof (HostCtrl2), &HostCtrl2);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  //
  // Set UHS Mode Select field of Host Control 2 reigster to SDR12/25/50
  //
  if (IsDdr) {
    HostCtrl2 = BIT2;
  } else if (ClockFreq == 52) {
    HostCtrl2 = BIT0;
  } else {
    HostCtrl2 = 0;
  }
  Status = SdMmcHcOrMmio (Private->SdMmcHcBase, SD_MMC_HC_HOST_CTRL2, sizeof (HostCtrl2), &HostCtrl2);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  HsTiming = 1;
  Status = MmcSwitchClockFreq (Private, Rca, HsTiming, ClockFreq);

  return Status;
}

/**
  Switch to the HS200 timing according to request.

  Refer to EMMC Electrical Standard Spec 5.1 Section 6.6.8 and SD Host Controller
  Simplified Spec 3.0 Figure 2-29 for details.

  @param[in] Private        A pointer to the SD_MMC_HC_PRIVATE_DATA instance.
  @param[in] Rca            The relative device address to be assigned.
  @param[in] ClockFreq      The max clock frequency to be set.
  @param[in] BusWidth       The bus width to be set, it could be 4 or 8.

  @retval EFI_SUCCESS       The operation is done correctly.
  @retval Others            The operation fails.

**/
EFI_STATUS
MmcSwitchToHS200 (
  IN SD_MMC_HC_PRIVATE_DATA            *Private,
  IN UINT16                             Rca,
  IN UINT32                             ClockFreq,
  IN UINT8                              BusWidth
  )
{
  EFI_STATUS          Status;
  UINT8               HsTiming;
  UINT16              ClockCtrl;

  if ((BusWidth != 4) && (BusWidth != 8)) {
    return EFI_INVALID_PARAMETER;
  }

  Status = MmcSwitchBusWidth (Private, Rca, FALSE, BusWidth);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  //
  // Stop bus clock at first
  //
  Status = SdMmcHcStopClock (Private->SdMmcHcBase);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  //
  // Wait Internal Clock Stable in the Clock Control register to be 1 before set SD Clock Enable bit
  //
  Status = SdMmcHcWaitMmioSet (
             Private->SdMmcHcBase,
             SD_MMC_HC_CLOCK_CTRL,
             sizeof (ClockCtrl),
             BIT1,
             BIT1,
             SD_MMC_HC_GENERIC_TIMEOUT
             );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  //
  // Set SD Clock Enable in the Clock Control register to 1
  //
  ClockCtrl = BIT2;
  Status = SdMmcHcOrMmio (Private->SdMmcHcBase, SD_MMC_HC_CLOCK_CTRL, sizeof (ClockCtrl), &ClockCtrl);

  HsTiming = 2;
  Status = MmcSwitchBusTiming (Private, Rca, HsTiming, ClockFreq);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = MmcTuningClkForHs200 (Private, BusWidth);

  return Status;
}

/**
  Switch to the HS400 timing according to request.

  Refer to EMMC Electrical Standard Spec 5.1 Section 6.6.8 and SD Host Controller
  Simplified Spec 3.0 Figure 2-29 for details.

  @param[in] Private        A pointer to the SD_MMC_HC_PRIVATE_DATA instance.
  @param[in] Rca            The relative device address to be assigned.
  @param[in] ClockFreq      The max clock frequency to be set.

  @retval EFI_SUCCESS       The operation is done correctly.
  @retval Others            The operation fails.

**/
EFI_STATUS
MmcSwitchToHS400 (
  IN SD_MMC_HC_PRIVATE_DATA            *Private,
  IN UINT16                             Rca,
  IN UINT32                             ClockFreq
  )
{
  EFI_STATUS          Status;
  UINT8               HsTiming;
  UINT8               HostCtrl2;

  Status = MmcSwitchToHS200 (Private, Rca, ClockFreq, 8);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Set the clock freq back to 52M frist.
  //
  Status = SdMmcHcClockSupply (Private->SdMmcHcBase, 52 * 1000, Private->Capability);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Set to Hight Speed timing and set the clock frequency to a value less than 52MHz.
  //
  HsTiming = 1;
  Status = MmcSwitchClockFreq (Private, Rca, HsTiming, 52);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // HS400 mode must use 8 data lines.
  //
  Status = MmcSwitchBusWidth (Private, Rca, TRUE, 8);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Clean UHS Mode Select field of Host Control 2 reigster before update
  //
  HostCtrl2 = (UINT8)~0x7;
  Status = SdMmcHcAndMmio (Private->SdMmcHcBase, SD_MMC_HC_HOST_CTRL2, sizeof (HostCtrl2), &HostCtrl2);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Set UHS Mode Select field of Host Control 2 reigster to HS400
  //
  HostCtrl2 = BIT0 | BIT2;
  Status = SdMmcHcOrMmio (Private->SdMmcHcBase, SD_MMC_HC_HOST_CTRL2, sizeof (HostCtrl2), &HostCtrl2);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  HsTiming = 3;
  Status = MmcSwitchClockFreq (Private, Rca, HsTiming, ClockFreq);

  return Status;
}

/**
  Send command SET_BUS_WIDTH to the SD device to set the bus width.

  Refer to SD Physical Layer Simplified Spec 4.1 Section 4.7 for details.

  @param[in] PassThru       A pointer to the EFI_SD_MMC_PASS_THRU_PROTOCOL instance.
  @param[in] Slot           The slot number of the SD card to send the command to.
  @param[in] Rca            The relative device address of addressed device.
  @param[in] BusWidth       The bus width to be set, it could be 1 or 4.

  @retval EFI_SUCCESS       The operation is done correctly.
  @retval Others            The operation fails.

**/
EFI_STATUS
SdCardSetBusWidth (
  IN SD_MMC_HC_PRIVATE_DATA            *Private,
  IN UINT16                             Rca,
  IN UINT8                              BusWidth
  )
{
  EFI_SD_MMC_COMMAND_BLOCK              SdMmcCmdBlk;
  EFI_SD_MMC_STATUS_BLOCK               SdMmcStatusBlk;
  EFI_SD_MMC_PASS_THRU_COMMAND_PACKET   Packet;
  EFI_STATUS                            Status;
  UINT8                                 Value;

  ZeroMem (&SdMmcCmdBlk, sizeof (SdMmcCmdBlk));
  ZeroMem (&SdMmcStatusBlk, sizeof (SdMmcStatusBlk));
  ZeroMem (&Packet, sizeof (Packet));

  Packet.SdMmcCmdBlk    = &SdMmcCmdBlk;
  Packet.SdMmcStatusBlk = &SdMmcStatusBlk;
  Packet.Timeout        = SD_MMC_HC_GENERIC_TIMEOUT;

  SdMmcCmdBlk.CommandIndex = SD_APP_CMD;
  SdMmcCmdBlk.CommandType  = SdMmcCommandTypeAc;
  SdMmcCmdBlk.ResponseType = SdMmcResponseTypeR1;
  SdMmcCmdBlk.CommandArgument = (UINT32)Rca << 16;

  Status = SdMmcSendCommand (Private, &Packet);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  SdMmcCmdBlk.CommandIndex = SD_SET_BUS_WIDTH;
  SdMmcCmdBlk.CommandType  = SdMmcCommandTypeAc;
  SdMmcCmdBlk.ResponseType = SdMmcResponseTypeR1;

  if (BusWidth == 1) {
    Value = 0;
  } else if (BusWidth == 4) {
    Value = 2;
  } else {
    return EFI_INVALID_PARAMETER;
  }

  SdMmcCmdBlk.CommandArgument = Value & 0x3;
  Status = SdMmcSendCommand (Private, &Packet);
  return Status;
}

/**
  Send command SWITCH_FUNC to the SD device to check switchable function or switch card function.

  Refer to SD Physical Layer Simplified Spec 4.1 Section 4.7 for details.

  @param[in]  Slot          The slot number of the SD card to send the command to.
  @param[in]  AccessMode    The value for access mode group.
  @param[in]  CommandSystem The value for command set group.
  @param[in]  DriveStrength The value for drive length group.
  @param[in]  PowerLimit    The value for power limit group.
  @param[in]  Mode          Switch or check function.
  @param[out] SwitchResp    The return switch function status.

  @retval EFI_SUCCESS       The operation is done correctly.
  @retval Others            The operation fails.

**/
EFI_STATUS
SdCardSwitch (
  IN SD_MMC_HC_PRIVATE_DATA          *Private,
  IN     UINT8                        AccessMode,
  IN     UINT8                        CommandSystem,
  IN     UINT8                        DriveStrength,
  IN     UINT8                        PowerLimit,
  IN     BOOLEAN                      Mode,
  OUT UINT8                          *SwitchResp
  )
{
  EFI_SD_MMC_COMMAND_BLOCK              SdMmcCmdBlk;
  EFI_SD_MMC_STATUS_BLOCK               SdMmcStatusBlk;
  EFI_SD_MMC_PASS_THRU_COMMAND_PACKET   Packet;
  EFI_STATUS                            Status;
  UINT32                                ModeValue;

  ZeroMem (&SdMmcCmdBlk, sizeof (SdMmcCmdBlk));
  ZeroMem (&SdMmcStatusBlk, sizeof (SdMmcStatusBlk));
  ZeroMem (&Packet, sizeof (Packet));

  Packet.SdMmcCmdBlk    = &SdMmcCmdBlk;
  Packet.SdMmcStatusBlk = &SdMmcStatusBlk;
  Packet.Timeout        = SD_MMC_HC_GENERIC_TIMEOUT;

  SdMmcCmdBlk.CommandIndex = SD_SWITCH_FUNC;
  SdMmcCmdBlk.CommandType  = SdMmcCommandTypeAdtc;
  SdMmcCmdBlk.ResponseType = SdMmcResponseTypeR1;

  ModeValue = Mode ? BIT31 : 0;
  SdMmcCmdBlk.CommandArgument = (AccessMode & 0xF) | ((PowerLimit & 0xF) << 4) | \
                             ((DriveStrength & 0xF) << 8) | ((DriveStrength & 0xF) << 12) | \
                             ModeValue;
  Packet.InDataBuffer     = SwitchResp;
  Packet.InTransferLength = 64;

  Status = SdMmcSendCommand (Private, &Packet);

  return Status;
}

/**
  Switch the high speed timing according to request.

  Refer to SD Physical Layer Simplified Spec 4.1 Section 4.7 and
  SD Host Controller Simplified Spec 3.0 section Figure 2-29 for details.

  @param[in] Slot           The slot number of the SD card to send the command to.
  @param[in] Rca            The relative device address to be assigned.
  @param[in] S18a           The boolean to show if it's a UHS-I SD card.

  @retval EFI_SUCCESS       The operation is done correctly.
  @retval Others            The operation fails.

**/
EFI_STATUS
SdCardSetBusMode (
  IN SD_MMC_HC_PRIVATE_DATA            *Private,
  IN UINT16                             Rca,
  IN BOOLEAN                            S18a
)
{
  EFI_STATUS                Status;
  UINT32                    ClockFreq;
  SD_MMC_HC_SLOT_CAP       *Capability;
  UINT8                     HostCtrl1;
  UINT8                     HostCtrl2;
  UINT32                    DevStatus;
  UINT8                     SwitchResp[64];
  UINT8                     AccessMode;

  Status = SdCardSetBusWidth (Private, Rca, 4);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = MmcSendStatus (Private, Rca, &DevStatus);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  SdMmcHcRwMmio (Private->SdMmcHcBase, SD_MMC_HC_HOST_CTRL1, TRUE, sizeof (HostCtrl1), &HostCtrl1);
  HostCtrl1 |= BIT1;
  HostCtrl1 &= (UINT8)~BIT5;
  SdMmcHcRwMmio (Private->SdMmcHcBase, SD_MMC_HC_HOST_CTRL1, FALSE, sizeof (HostCtrl1), &HostCtrl1);

  ZeroMem (SwitchResp, sizeof (SwitchResp));
  Status = SdCardSwitch (Private, 0xF, 0xF, 0xF, 0xF, FALSE, SwitchResp);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Capability = &(Private->Capability);
  //
  // Calculate supported bus speed/bus width/clock frequency by host and device capability.
  //
  ClockFreq = 0;
  if (S18a && (Capability->Sdr104 != 0) && ((SwitchResp[13] & BIT3) != 0)) {
    ClockFreq = 208;
    AccessMode = 3;
  } else if (S18a && (Capability->Sdr50 != 0) && ((SwitchResp[13] & BIT2) != 0)) {
    ClockFreq = 100;
    AccessMode = 2;
  } else if (S18a && (Capability->Ddr50 != 0) && ((SwitchResp[13] & BIT4) != 0)) {
    ClockFreq = 50;
    AccessMode = 4;
  } else if ((SwitchResp[13] & BIT1) != 0) {
    ClockFreq = 50;
    AccessMode = 1;
  } else {
    ClockFreq = 25;
    AccessMode = 0;
  }
  Status = SdCardSwitch (Private, AccessMode, 0xF, 0xF, 0xF, TRUE, SwitchResp);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Set to Hight Speed timing
  //
  if (AccessMode == 1) {
    HostCtrl1 = BIT2;
    SdMmcHcOrMmio (Private->SdMmcHcBase, SD_MMC_HC_HOST_CTRL1, sizeof (HostCtrl1), &HostCtrl1);
  }

  HostCtrl2 = (UINT8)~0x7;
  SdMmcHcAndMmio (Private->SdMmcHcBase, SD_MMC_HC_HOST_CTRL2, sizeof (HostCtrl2), &HostCtrl2);

  HostCtrl2 = AccessMode;
  SdMmcHcOrMmio (Private->SdMmcHcBase, SD_MMC_HC_HOST_CTRL2, sizeof (HostCtrl2), &HostCtrl2);

  Status = SdMmcHcClockSupply (Private->SdMmcHcBase, ClockFreq * 1000, Private->Capability);
  Private->Slot.CurrentFreq = ClockFreq * 1000;

  return EFI_SUCCESS;
}

/**
  Switch the high speed timing according to request.

  Refer to EMMC Electrical Standard Spec 5.1 Section 6.6.8 and SD Host Controller
  Simplified Spec 3.0 Figure 2-29 for details.

  @param[in] Private        A pointer to the SD_MMC_HC_PRIVATE_DATA instance.
  @param[in] Rca            The relative device address to be assigned.

  @retval EFI_SUCCESS       The operation is done correctly.
  @retval Others            The operation fails.

**/
EFI_STATUS
MmcSetBusMode (
  IN SD_MMC_HC_PRIVATE_DATA            *Private,
  IN UINT16                             Rca
  )
{
  EFI_STATUS                    Status;
  UINT8                         HsTiming;
  BOOLEAN                       IsDdr;
  UINT32                        ClockFreq;
  UINT8                         BusWidth;
  EMMC_CARD_DATA               *CardData;
  EMMC_EXT_CSD                 *ExtCsd;

  CardData = (EMMC_CARD_DATA *) Private->Slot.CardData;
  Status = MmcGetCsd (Private, Rca);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "MmcSetBusMode: GetCsd fails with %r\n", Status));
    return Status;
  }
  DumpCsd (&CardData->Csd);

  Status = MmcSelect (Private, Rca);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "MmcSetBusMode: Select fails with %r\n", Status));
    return Status;
  }

  if (Private->Slot.CardType == SdCardType) {
    Status = SdCardSetBusMode (Private, Rca, (CardData->Ocr & BIT24) != 0);
    DEBUG ((DEBUG_ERROR, "MmcSetBusMode: Set bus mode for SD card fails with %r\n", Status));
    return Status;
  }

  ASSERT (Private->Capability.BaseClkFreq != 0);
  //
  // Check if the Host Controller support 8bits bus width.
  //
  if (Private->Capability.BusWidth8 != 0) {
    BusWidth = 8;
  } else {
    BusWidth = 4;
  }
  //
  // Get Deivce_Type from EXT_CSD register.
  //
  Status = MmcGetExtCsd (Private);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "MmcSetBusMode: GetExtCsd fails with %r\n", Status));
    return Status;
  }
  ExtCsd = &CardData->ExtCsd;
  DumpExtCsd (ExtCsd);

  //
  // Calculate supported bus speed/bus width/clock frequency.
  //
  HsTiming  = 0;
  IsDdr     = FALSE;
  ClockFreq = 0;
  if (((ExtCsd->DeviceType & (BIT4 | BIT5))  != 0) && (Private->Capability.Sdr104 != 0)) {
    HsTiming  = 2;
    IsDdr     = FALSE;
    ClockFreq = 200;
  } else if (((ExtCsd->DeviceType & (BIT2 | BIT3))  != 0) && (Private->Capability.Ddr50 != 0)) {
    HsTiming  = 1;
    IsDdr     = TRUE;
    ClockFreq = 52;
  } else if (((ExtCsd->DeviceType & BIT1)  != 0) && (Private->Capability.HighSpeed != 0)) {
    HsTiming  = 1;
    IsDdr     = FALSE;
    ClockFreq = 52;
  } else if (((ExtCsd->DeviceType & BIT0)  != 0) && (Private->Capability.HighSpeed != 0)) {
    HsTiming  = 1;
    IsDdr     = FALSE;
    ClockFreq = 26;
  }
  //
  // Check if both of the device and the host controller support HS400 DDR mode.
  //
  if (((ExtCsd->DeviceType & (BIT6 | BIT7))  != 0) && (Private->Capability.Hs400 != 0)) {
    //
    // The host controller supports 8bits bus.
    //
    ASSERT (BusWidth == 8);
    if (FeaturePcdGet (PcdEmmcHs400SupportEnabled)) {
      HsTiming  = 3;
      IsDdr     = TRUE;
    } else {
      // Fallback to HS200 if HS400 needs to be disabled
      HsTiming  = 2;
      IsDdr     = FALSE;
      DEBUG ((DEBUG_INFO, "Degrade HS400 to HS200 per request\n"));
    }
    ClockFreq = 200;
  }

  if ((ClockFreq == 0) || (HsTiming == 0)) {
    //
    // Continue using default setting.
    //
    return EFI_SUCCESS;
  }

  DEBUG ((DEBUG_VERBOSE, "MmcSetBusMode: HsTiming %d ClockFreq %d BusWidth %d Ddr %a\n", HsTiming, ClockFreq, BusWidth,
          IsDdr ? "TRUE" : "FALSE"));

  if (HsTiming == 3) {
    //
    // Execute HS400 timing switch procedure
    //
    Status = MmcSwitchToHS400 (Private, Rca, ClockFreq);
  } else if (HsTiming == 2) {
    //
    // Execute HS200 timing switch procedure
    //
    Status = MmcSwitchToHS200 (Private, Rca, ClockFreq, BusWidth);
  } else {
    //
    // Execute High Speed timing switch procedure
    //
    Status = MmcSwitchToHighSpeed (Private, Rca, ClockFreq, IsDdr, BusWidth);
  }

  DEBUG ((DEBUG_VERBOSE, "MmcSetBusMode: Switch to %a %r\n",
          (HsTiming == 3) ? "HS400" : ((HsTiming == 2) ? "HS200" : "HighSpeed"), Status));

  return Status;
}

/**
  Set the number of blocks for a block read/write cmd through sync or async I/O request.

  @param[in]  Private           A pointer to the SD_MMC_HC_PRIVATE_DATA instance.
  @param[in]  BlockNum          The number of blocks for transfer.
  @param[in]  IsReliableWrite   Flag to specify whether the write needs to be reliable/authenticated
                                or not. This was introduced as part of RPMB library support

  @retval EFI_SUCCESS           The request is executed successfully.
  @retval EFI_OUT_OF_RESOURCES  The request could not be executed due to a lack of resources.
  @retval Others                The request could not be executed successfully.

**/
EFI_STATUS
MmcSetBlkCount (
  IN  SD_MMC_HC_PRIVATE_DATA   *Private,
  IN  UINT16                    BlockNum,
  IN  BOOLEAN                   IsReliableWrite
  )
{
  EFI_SD_MMC_COMMAND_BLOCK              SdMmcCmdBlk;
  EFI_SD_MMC_STATUS_BLOCK               SdMmcStatusBlk;
  EFI_SD_MMC_PASS_THRU_COMMAND_PACKET   Packet;
  EFI_STATUS                            Status;
  UINT32 Arg;

  Arg = BlockNum;

 if (IsReliableWrite) {
    Arg  |= (1 << 31);
  }

  ZeroMem (&SdMmcCmdBlk, sizeof (SdMmcCmdBlk));
  ZeroMem (&SdMmcStatusBlk, sizeof (SdMmcStatusBlk));
  ZeroMem (&Packet, sizeof (Packet));

  Packet.SdMmcCmdBlk    = &SdMmcCmdBlk;
  Packet.SdMmcStatusBlk = &SdMmcStatusBlk;
  Packet.Timeout        = SD_MMC_HC_GENERIC_TIMEOUT;

  SdMmcCmdBlk.CommandIndex    = EMMC_SET_BLOCK_COUNT;
  SdMmcCmdBlk.CommandType     = SdMmcCommandTypeAc;
  SdMmcCmdBlk.ResponseType    = SdMmcResponseTypeR1;
  SdMmcCmdBlk.CommandArgument = Arg;

  Status = SdMmcSendCommand (Private, &Packet);

  return Status;
}

/**
  Send stop transmission command (CMD12)in case of failure as a safety measure.

  @param[in]  Private           A pointer to the SD_MMC_HC_PRIVATE_DATA instance.
  @param[in]  Rca               The relative device address to be assigned.

  @retval EFI_SUCCESS           The request is executed successfully.
  @retval Others                The request could not be executed successfully.

**/
EFI_STATUS
MmcStopTransmission (
  IN  SD_MMC_HC_PRIVATE_DATA   *Private,
  IN  UINT16                    Rca
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

  SdMmcCmdBlk.CommandIndex    = EMMC_STOP_TRANSMISSION;
  SdMmcCmdBlk.CommandType     = SdMmcCommandTypeAc;
  SdMmcCmdBlk.ResponseType    = SdMmcResponseTypeR1;
  SdMmcCmdBlk.CommandArgument = (UINT32)Rca << 16;

  Status = SdMmcSendCommand (Private, &Packet);

  return Status;
}

/**
  Read/write multiple blocks through sync or async I/O request.

  @param[in]  Private           A pointer to the SD_MMC_HC_PRIVATE_DATA instance.
  @param[in]  Lba               The starting logical block address to be read/written.
                                The caller is responsible for reading/writing to only
                                legitimate locations.
  @param[in]  Buffer            A pointer to the destination/source buffer for the data.
  @param[in]  BufferSize        Size of Buffer, must be a multiple of device block size.
  @param[in]  IsRead            Indicates it is a read or write operation.

  @retval EFI_SUCCESS           The request is executed successfully.
  @retval EFI_OUT_OF_RESOURCES  The request could not be executed due to a lack of resources.
  @retval Others                The request could not be executed successfully.

**/
EFI_STATUS
MmcRwMultiBlocks (
  IN  SD_MMC_HC_PRIVATE_DATA   *Private,
  IN  EFI_LBA                   Lba,
  IN  VOID                     *Buffer,
  IN  UINTN                     BufferSize,
  IN  BOOLEAN                   IsRead
  )
{
  EFI_SD_MMC_COMMAND_BLOCK              SdMmcCmdBlk;
  EFI_SD_MMC_STATUS_BLOCK               SdMmcStatusBlk;
  EFI_SD_MMC_PASS_THRU_COMMAND_PACKET   Packet;
  EFI_STATUS                            Status;
  EMMC_CARD_DATA                       *CardData;

  CardData = (EMMC_CARD_DATA *) Private->Slot.CardData;

  ZeroMem (&SdMmcCmdBlk, sizeof (SdMmcCmdBlk));
  ZeroMem (&SdMmcStatusBlk, sizeof (SdMmcStatusBlk));
  ZeroMem (&Packet, sizeof (Packet));

  Packet.SdMmcCmdBlk    = &SdMmcCmdBlk;
  Packet.SdMmcStatusBlk = &SdMmcStatusBlk;
  Packet.Timeout        = (BufferSize / (2 * 1024 * 1024) + 1) * 1000 * 1000;

  if (IsRead) {
    Packet.InDataBuffer     = Buffer;
    Packet.InTransferLength = (UINT32)BufferSize;

    if (BufferSize > CardData->BlockLen) {
      SdMmcCmdBlk.CommandIndex = EMMC_READ_MULTIPLE_BLOCK;
    } else {
      SdMmcCmdBlk.CommandIndex = EMMC_READ_SINGLE_BLOCK;
    }
    SdMmcCmdBlk.CommandType  = SdMmcCommandTypeAdtc;
    SdMmcCmdBlk.ResponseType = SdMmcResponseTypeR1;
  } else {
    Packet.OutDataBuffer     = Buffer;
    Packet.OutTransferLength = (UINT32)BufferSize;

    if (BufferSize > CardData->BlockLen) {
      SdMmcCmdBlk.CommandIndex = EMMC_WRITE_MULTIPLE_BLOCK;
    } else {
      SdMmcCmdBlk.CommandIndex = EMMC_WRITE_BLOCK;
    }
    SdMmcCmdBlk.CommandType  = SdMmcCommandTypeAdtc;
    SdMmcCmdBlk.ResponseType = SdMmcResponseTypeR1;
  }

  if (Private->Slot.SectorAddressing) {
    SdMmcCmdBlk.CommandArgument = (UINT32)Lba;
  } else {
    SdMmcCmdBlk.CommandArgument = (UINT32)MultU64x32 (Lba, CardData->BlockLen);
  }

  Status = SdMmcSendCommand (Private, &Packet);

  return Status;
}

/**
  This function transfers data from/to EMMC device.

  @param[in]       Private            A pointer to the SD_MMC_HC_PRIVATE_DATA instance.
  @param[in]       Lba                The starting logical block address to be read/written.
                                      The caller is responsible for reading/writing to only
                                      legitimate locations.
  @param[in, out]  Buffer             A pointer to the destination/source buffer for the data.
  @param[in]       BufferSize         Size of Buffer, must be a multiple of device block size.
  @param[in]       IsRead             Indicates it is a read or write operation.
  @param[in]       IsReliableWrite    Flag to specify whether the write needs to be reliable/authenticated
                                      or not. This was introduced as part of RPMB library support

  @retval EFI_SUCCESS           The data was read/written correctly to the device.
  @retval EFI_WRITE_PROTECTED   The device can not be read/written to.
  @retval EFI_DEVICE_ERROR      The device reported an error while performing the read/write.
  @retval EFI_NO_MEDIA          There is no media in the device.
  @retval EFI_MEDIA_CHNAGED     The MediaId does not matched the current device.
  @retval EFI_BAD_BUFFER_SIZE   The Buffer was not a multiple of the block size of the device.
  @retval EFI_INVALID_PARAMETER The read/write request contains LBAs that are not valid,
                                or the buffer is not on proper alignment.

**/
EFI_STATUS
MmcReadWrite (
  IN     SD_MMC_HC_PRIVATE_DATA        *Private,
  IN     EFI_LBA                        Lba,
  IN OUT VOID                          *Buffer,
  IN     UINTN                          BufferSize,
  IN     BOOLEAN                        IsRead,
  IN     BOOLEAN                        IsReliableWrite
  )
{
  EFI_STATUS                            Status;
  UINTN                                 BlockNum;
  UINTN                                 Remaining;
  UINT32                                MaxBlock;
  UINT32                                DevStatus;
  UINT32                                DevState;
  UINT16                                Rca;
  EMMC_CARD_DATA                       *CardData;

  Status = EFI_SUCCESS;

  CardData = (EMMC_CARD_DATA *) Private->Slot.CardData;
  BlockNum  = (BufferSize + CardData->BlockLen - 1) / CardData->BlockLen;

  Remaining = BlockNum;

  MaxBlock = PcdGet16 (PcdEmmcMaxRwBlockNumber);
  if (FeaturePcdGet(PcdDmaProtectionEnabled)) {
    // When DMA protection is enabled, only tranfer less than DMA buffer size
    // Use half for safe.  Around 1MB will be used for CmdTable.
    MaxBlock = (PcdGet32 (PcdDmaBufferSize) >> 1) / CardData->BlockLen;
    if (MaxBlock == 0) {
      MaxBlock = 1;
    }
    if (MaxBlock > PcdGet16 (PcdEmmcMaxRwBlockNumber)) {
      MaxBlock = PcdGet16 (PcdEmmcMaxRwBlockNumber);
    }
  }

  DEBUG ((DEBUG_VERBOSE, "MmcReadWrite Lba=0x%x Buffer=0x%p BufferSize=0x%x, BlockNum=0x%x\n",
          (UINT32)Lba, Buffer, BufferSize, BlockNum));

  while (Remaining > 0) {
    if (Remaining <= MaxBlock) {
      BlockNum = Remaining;
    } else {
      BlockNum = MaxBlock;
    }

    if (Private->Slot.CardType != SdCardType) {
      Status = MmcSetBlkCount (Private, (UINT16)BlockNum, IsReliableWrite);
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "Emmc%a MmcSetBlkCount Failed: Lba 0x%x, BlockNum 0x%x with 0x%x\n", IsRead ? "Read " : "Write",
                (UINT32)Lba, BlockNum, Status));
        return Status;
      }
    }

    BufferSize = BlockNum * CardData->BlockLen;
    Status = MmcRwMultiBlocks (Private, Lba, Buffer, BufferSize, IsRead);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "Emmc%a Failed: Lba 0x%x, BlockNum 0x%x with %r\n", IsRead ? "Read " : "Write", (UINT32)Lba,
              BlockNum, Status));
      Rca = 1;
      MmcSendStatus (Private, Rca, &DevStatus);
      DevState = (DevStatus >> 9) & 0x0F;

      if (DevState == 6) {
        DEBUG ((DEBUG_ERROR, "EMMC device state is 'RCV', Sending STOP_TRANSMISSION (CMD12)\n"));
        MmcStopTransmission (Private, Rca);
      }

      return Status;
    }
    DEBUG ((DEBUG_VERBOSE, "Emmc%a: Lba 0x%x, BlockNum 0x%x with %r\n", IsRead ? "Read " : "Write", (UINT32)Lba, BlockNum,
            Status));

    Lba   += BlockNum;
    Buffer = (UINT8 *)Buffer + BufferSize;
    Remaining -= BlockNum;
  }

  return Status;
}

/**
  This function fills in EMMC Card Information

  @param[in]  Private       A pointer to the SD_MMC_HC_PRIVATE_DATA instance.

  @retval EFI_SUCCESS       The operation is done correctly.
  @retval Others            The operation fails.

**/
EFI_STATUS
MmcInitCardInfo (
  IN SD_MMC_HC_PRIVATE_DATA   *Private
  )
{
  EMMC_CARD_DATA         *CardData;
  EMMC_CSD               *Csd;
  SD_CSD                 *SdCsd;
  SD_CSD2                *SdCsd2;
  EMMC_EXT_CSD           *ExtCsd;
  UINT32                  Blocks;
  UINT32                  Ocr;
  UINT64                  Capacity;
  UINT32                  CSize;
  UINT32                  CSizeMul;
  UINT32                  ReadBlLen;

  if (Private == NULL) {
    return EFI_LOAD_ERROR;
  }

  CardData = (EMMC_CARD_DATA *)Private->Slot.CardData;
  Ocr      = CardData->Ocr;
  Csd      = &CardData->Csd;
  ExtCsd   = &CardData->ExtCsd;

  if (Csd->CsdStructure == 0) {
    Private->Slot.SectorAddressing = FALSE;
  } else {
    Private->Slot.SectorAddressing = TRUE;
  }

  if (Private->Slot.CardType == SdCardType) {
    SdCsd = (SD_CSD *)Csd;
    if (SdCsd->CsdStructure == 0) {
      CSize     = (SdCsd->CSizeHigh << 2 | SdCsd->CSizeLow) + 1;
      CSizeMul  = (1 << (SdCsd->CSizeMul + 2));
      ReadBlLen = (1 << (SdCsd->ReadBlLen));
      Capacity  = MultU64x32 (MultU64x32 ((UINT64)CSize, CSizeMul), ReadBlLen);
    } else {
      SdCsd2   = (SD_CSD2 *) (VOID *)Csd;
      CSize    = (SdCsd2->CSizeHigh << 16 | SdCsd2->CSizeLow) + 1;
      Capacity = MultU64x32 ((UINT64)CSize, SIZE_512KB);
    }
    CardData->BlockLen = (1 << SdCsd->ReadBlLen);
    CardData->BlockNum = (UINT32)DivU64x32 (Capacity, CardData->BlockLen);
    DEBUG ((DEBUG_VERBOSE, " == SD CARD INFO ==\n"));
    DEBUG ((DEBUG_VERBOSE, "  Block   Length: %d\n", CardData->BlockLen));
    DEBUG ((DEBUG_VERBOSE, "  Partition size: %dMB\n", (UINT32)DivU64x32 (MultU64x32 (CardData->BlockNum,
            CardData->BlockLen), 0x100000)));
  } else {
    if ((ExtCsd->DeviceType & BIT2) || (ExtCsd->DeviceType & BIT3)) {
      CardData->BlockLen = 512;
    } else {
      CardData->BlockLen = (1 << Csd->ReadBlLen);
    }

    Blocks = (ExtCsd->SecCount[0]) | (ExtCsd->SecCount[1] << 8) | (ExtCsd->SecCount[2] << 16) | (ExtCsd->SecCount[3] << 24);
    CardData->BlockNum = Blocks;

    DEBUG ((DEBUG_VERBOSE, " == EMMC CARD INFO ==\n"));
    DEBUG ((DEBUG_VERBOSE, "  MMC Version: %d\n", ExtCsd->ExtCsdRev));
    DEBUG ((DEBUG_VERBOSE, "  High Capacity Support: %d\n", Ocr & 0x40000000));
    DEBUG ((DEBUG_VERBOSE, "  Block Number: %d\n", Blocks));
    DEBUG ((DEBUG_VERBOSE, "  Block Length: %d\n", CardData->BlockLen));
    DEBUG ((DEBUG_VERBOSE, "  BOOT partition size: %dKB\n", (ExtCsd->BootSizeMult << 17) / 1024));
    DEBUG ((DEBUG_VERBOSE, "  RPMB partition size: %dKB\n", (ExtCsd->RpmbSizeMult << 17) / 1024));
    DEBUG ((DEBUG_VERBOSE, "  USER partition size: %dMB\n", (UINT32)DivU64x32 (MultU64x32 (Blocks, CardData->BlockLen),
            0x100000)));
    DEBUG ((DEBUG_VERBOSE, "  Current Partition: %d\n", ExtCsd->PartitionConfig));
  }

  return EFI_SUCCESS;
}

/**
  This function initializes the rest of EMMC device initialization after early initialization.

  @param[in]  Private           A pointer to the SD_MMC_HC_PRIVATE_DATA instance.

  @retval EFI_SUCCESS           The request is executed successfully.
  @retval EFI_OUT_OF_RESOURCES  The request could not be executed due to a lack of resources.
  @retval Others                The request could not be executed successfully.

**/
EFI_STATUS
ContinueMmcInitialize (
  IN  SD_MMC_HC_PRIVATE_DATA     *Private
  )
{
  EFI_STATUS                      Status;
  UINT16                          Rca;
  EMMC_CARD_DATA                 *CardData;
  UINT32                          TimeOut;

  if (Private->Slot.Initialized) {
    return EFI_SUCCESS;
  }

  CardData = (EMMC_CARD_DATA *) Private->Slot.CardData;

  DEBUG ((DEBUG_INFO, "MMC Phase 2 init\n"));

  if (Private->Slot.CardType == SdCardType) {
    TimeOut = 5000;
    while ((CardData->Ocr & BIT31) == 0) {
      CardData->Ocr |= 0x41000000;
      Status = SdCardSendOpCond (Private, &CardData->Ocr);
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "SdCardSendOpCond fails with %r\n", Status));
        goto Done;
      }

      MicroSecondDelay (1 * 1000);
      TimeOut--;
      if (TimeOut == 0) {
        DEBUG ((DEBUG_ERROR, "Card is always in busy state\n"));
        Status = EFI_TIMEOUT;
        goto Done;
      }
    }
  } else {
    if (CardData->Ocr == 0) {
      CardData->Ocr = 0x40FF8080;
      Status = MmcGetOcr (Private, &CardData->Ocr);
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "MmcGetOcr Fail Status = 0x%x\n", Status));
        goto Done;
      }
    }

    TimeOut = 2000;
    while ((CardData->Ocr & BIT31) == 0) {
      //
      // Make sure sector mode access. Some cards may not return back access mode properly.
      //
      CardData->Ocr |= 0x40000000;
      Status = MmcGetOcr (Private, &CardData->Ocr);
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "Executing Cmd1 fails with %r\n", Status));
        goto Done;
      }

      MicroSecondDelay (1 * 1000);
      TimeOut--;
      if (TimeOut == 0) {
        DEBUG ((DEBUG_ERROR, "Card is always in busy state\n"));
        Status = EFI_TIMEOUT;
        goto Done;
      }
    }
  }

  Status = MmcGetAllCid (Private);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Executing Cmd2 fails with %r\n", Status));
    goto Done;
  }

  Rca = 1;
  Status = MmcSetRca (Private, Rca);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Executing Cmd3 fails with %r\n", Status));
    goto Done;
  }

  if (Private->Slot.CardType == SdCardType) {
    Rca = (UINT16)CardData->Address;
  }
  Status = MmcSetBusMode (Private, Rca);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "MmcSetBusMode Fail Status = 0x%x\n", Status));
    goto Done;
  }

  Status = MmcInitCardInfo (Private);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "MmcInitCardInfo Fail Status = 0x%x\n", Status));
    goto Done;
  }

  Private->Slot.Initialized  = TRUE;

Done:
  return Status;
}

/**
  This function initializes EMMC device.

  @retval TRUE    if EMMC device is initialized properly
  @retval FALSE   if EMMC device is not initialized properly

**/
BOOLEAN
MmcIsInitialized (
  VOID
  )
{
  SD_MMC_HC_PRIVATE_DATA *Private;

  Private = NULL;

  Private = MmcGetHcPrivateData();
  if (Private == NULL) {
    return FALSE;
  }

  if (Private->Slot.Initialized) {
    return TRUE;
  }

  return FALSE;
}

/**
  This function selects EMMC partition

  @param[in] PartNum        The target partition number to be changed

  @retval EFI_SUCCESS       The operation is done correctly.
  @retval Others            The operation fails.

**/
EFI_STATUS
MmcSelectPart (
  IN  UINT8   PartNum
  )
{
  EFI_STATUS               Status;
  SD_MMC_HC_PRIVATE_DATA  *Private;
  EMMC_CARD_DATA          *CardData;
  EMMC_EXT_CSD            *ExtCsd;
  UINT8                    CurrentPart;
  UINT8                    Offset;

  Private = NULL;

  if (PartNum > 7) {
    return EFI_INVALID_PARAMETER;
  }

  if (!MmcIsInitialized()) {
    return EFI_NOT_READY;
  }

  Private = MmcGetHcPrivateData();
  if (Private == NULL) {
    return EFI_NOT_READY;
  }

  Status = MmcGetExtCsd (Private);
  if (EFI_ERROR (Status)) {
    goto Done;
  }

  CardData = (EMMC_CARD_DATA *)Private->Slot.CardData;
  ExtCsd = &CardData->ExtCsd;

  CurrentPart = ExtCsd->PartitionConfig & 0x7;
  if (CurrentPart == PartNum) {
    DEBUG ((DEBUG_INFO, "MmcSelectPart - Same with current partition\n"));
    Private->CurrentPartition = PartNum;
    return EFI_SUCCESS;
  }

  Offset = OFFSET_OF (EMMC_EXT_CSD, PartitionConfig);
  Status = MmcSetExtCsd (Private, Offset, (ExtCsd->PartitionConfig & ~0x7) | PartNum);
  if (EFI_ERROR (Status)) {
    goto Done;
  }

  Status = MmcGetExtCsd (Private);
  if (EFI_ERROR (Status)) {
    goto Done;
  }

  if (ExtCsd->PartitionConfig != PartNum) {
    DEBUG ((DEBUG_ERROR, "MmcSelectPart Fail\n"));
    return EFI_DEVICE_ERROR;
  }
  Private->CurrentPartition = PartNum;
Done:
  return Status;
}


/**
  This function reads data from EMMC to Memory.

  @param[in]  DeviceIndex   Specifies the block device to which the function wants
                            to talk.
  @param[in]  StartLBA      The starting logical block address (LBA) to read from
                            on the device
  @param[in]  BufferSize    The size of the Buffer in bytes. This number must be
                            a multiple of the intrinsic block size of the device.
  @param[out] Buffer        A pointer to the destination buffer for the data.
                            The caller is responsible for the ownership of the
                            buffer.

  @retval EFI_SUCCESS             The data was read correctly from the device.
  @retval EFI_DEVICE_ERROR        The device reported an error while attempting
                                  to perform the read operation.
  @retval EFI_INVALID_PARAMETER   The read request contains LBAs that are not
                                  valid, or the buffer is not properly aligned.
  @retval EFI_NO_MEDIA            There is no media in the device.
  @retval EFI_BAD_BUFFER_SIZE     The BufferSize parameter is not a multiple of
                                  the intrinsic block size of the device.

**/
EFI_STATUS
EFIAPI
MmcReadBlocks (
  IN  UINTN                         DeviceIndex,
  IN  EFI_LBA                       StartLBA,
  IN  UINTN                         BufferSize,
  OUT VOID                          *Buffer
  )
{
  EFI_STATUS                        Status;
  SD_MMC_HC_PRIVATE_DATA            *Private;


  if (!MmcIsInitialized()) {
    return EFI_NOT_READY;
  }

  Private = MmcGetHcPrivateData();
  if (Private == NULL) {
    return EFI_NOT_READY;
  }

  if (DeviceIndex != Private->CurrentPartition) {
    Status = MmcSelectPart ((UINT8)DeviceIndex);
    if (EFI_ERROR (Status)) {
      return Status;
    }
  }

  Status  = MmcReadWrite (Private, StartLBA, Buffer, BufferSize, TRUE, FALSE);
  return Status;
}

/**
  This function is an extention of MmcWriteBlocks API.

  @param[in]  DeviceIndex     Specifies the block device to which the function wants
                              to talk.
  @param[in]  StartLBA        Target EMMC block number(LBA) where data will be written
  @param[in]  DataSize        Total data size to be written in bytes unit
  @param[in]  DataAddress     Data address in Memory to be copied to EMMC
  @param[in]  IsReliableWrite Flag to specify whether the write needs to be reliable/authenticated
                              or not. This was introduced as part of RPMB library support

  @retval EFI_SUCCESS       The operation is done correctly.
  @retval Others            The operation fails.

**/
EFI_STATUS
EFIAPI
MmcWriteBlocksExt (
  IN  UINTN                         DeviceIndex,
  IN  EFI_LBA                       StartLBA,
  IN  UINTN                         DataSize,
  IN  VOID                          *DataAddress,
  IN  BOOLEAN                       IsReliableWrite
  )
{
  EFI_STATUS               Status;
  SD_MMC_HC_PRIVATE_DATA  *Private;

  if (!MmcIsInitialized()) {
    return EFI_NOT_READY;
  }

  Private = MmcGetHcPrivateData();
  if (Private == NULL) {
    return EFI_NOT_READY;
  }

  if (DeviceIndex != Private->CurrentPartition) {
    Status = MmcSelectPart ((UINT8)DeviceIndex);
    if (EFI_ERROR (Status)) {
      return Status;
    }
  }

  Status  = MmcReadWrite (Private, StartLBA, DataAddress, DataSize, FALSE, IsReliableWrite);

  return Status;
}

/**
  This function writes data from Memory to EMMC.

  @param[in]  DeviceIndex   Specifies the block device to which the function wants
                            to talk.
  @param[in]  StartLBA      Target EMMC block number(LBA) where data will be written
  @param[in]  DataSize      Total data size to be written in bytes unit
  @param[in] DataAddress   Data address in Memory to be copied to EMMC

  @retval EFI_SUCCESS       The operation is done correctly.
  @retval Others            The operation fails.

**/
EFI_STATUS
EFIAPI
MmcWriteBlocks (
  IN  UINTN                         DeviceIndex,
  IN  EFI_LBA                       StartLBA,
  IN  UINTN                         DataSize,
  IN  VOID                          *DataAddress
  )
{
  EFI_STATUS                        Status;

  Status = MmcWriteBlocksExt ( DeviceIndex, StartLBA, DataSize, DataAddress, FALSE);
  return Status;
}
/**
  Gets a block device's media information.

  This function will provide the caller with the specified block device's media
  information. If the media changes, calling this function will update the media
  information accordingly.

  @param[in]  DeviceIndex    Specifies the block device to which the function wants
                             to talk.
  @param[out] DevBlockInfo   The Block Io information of the specified block partition.

  @retval EFI_SUCCESS        The Block Io information about the specified block device
                             was obtained successfully.
  @retval EFI_DEVICE_ERROR   Cannot get the media information due to a hardware
                             error.

**/
EFI_STATUS
EFIAPI
MmcGetMediaInfo (
  IN  UINTN                          DeviceIndex,
  OUT DEVICE_BLOCK_INFO              *DevBlockInfo
  )
{
  EFI_STATUS              Status;
  SD_MMC_HC_PRIVATE_DATA  *Private;
  EMMC_CARD_DATA          *CardData;

  if (DevBlockInfo == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (!MmcIsInitialized()) {
    return EFI_NOT_READY;
  }

  Private = MmcGetHcPrivateData();
  if (Private == NULL) {
    return EFI_NOT_READY;
  }

  if (DeviceIndex != Private->CurrentPartition) {
    Status = MmcSelectPart ((UINT8)DeviceIndex);
    if (EFI_ERROR (Status)) {
      return Status;
    }
  }

  CardData = (EMMC_CARD_DATA *) Private->Slot.CardData;
  DevBlockInfo->BlockSize = CardData->BlockLen;
  DevBlockInfo->BlockNum  = CardData->BlockNum;

  return EFI_SUCCESS;
}

/**
  Get the value of the field HsTiming in EXT_CSD register.

  Refer to EMMC Electrical Standard Spec 5.1 Section 6.6 and SD Host Controller
  Simplified Spec 3.0 Figure 3-3 for details.

  @param[in]  Private        A pointer to the SD_MMC_HC_PRIVATE_DATA instance.
  @param[out] HsTiming       The value of HS_TIMING field of EXT_CSD register.

  @retval EFI_SUCCESS       The operation is done correctly.
  @retval Others            The operation fails.

**/
EFI_STATUS
MmcGetHsTiming (
  SD_MMC_HC_PRIVATE_DATA   *Private,
  OUT UINT8                *HsTiming
  )
{
  EFI_STATUS                Status;
  EMMC_CARD_DATA           *CardData;
  EMMC_EXT_CSD             *ExtCsd;

  Status = EFI_SUCCESS;
  CardData = (EMMC_CARD_DATA *) Private->Slot.CardData;

  //
  // Get HsTiming from EXT_CSD register.
  //
  Status = MmcGetExtCsd (Private);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "MmcGetHsTiming: GetExtCsd fails with %r\n", Status));
    return Status;
  }

  ExtCsd = &CardData->ExtCsd;
  *HsTiming = ExtCsd->HsTiming;

  return Status;
}


/**
  To select eMMC card operating mode HS200/HS400

  @param[in] EmmcMode                    To select HS200 or HS400 mode

  @retval EFI_SUCCESS                    Emmc Mode Select successful.
  @retval EFI_INVALID_PARAMETER          A parameter was incorrect.
**/
EFI_STATUS
EFIAPI
EmmcModeSelection (
  IN EMMC_MODE                          EmmcMode
  )
{
  EFI_STATUS                    Status;
  SD_MMC_HC_PRIVATE_DATA       *Private;
  UINT16                        Rca;
  UINT8                         BusWidth;
  UINT32                        ClockFreq;
  UINT8                         HsTiming;

  Status      = EFI_SUCCESS;
  Rca         = 1;
  BusWidth    = 8;
  ClockFreq   = 200;

  Private = MmcGetHcPrivateData ();
  if (Private == NULL) {
    DEBUG ((DEBUG_ERROR, "Memory Allocation Fail - Out Of Resource\n"));
    Status = EFI_OUT_OF_RESOURCES;
    goto Done;
  }

  Status = MmcGetHsTiming (Private, &HsTiming);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "EmmcModeSelection: GetHsTiming fails with %r\n", Status));
    return Status;
  }

  if (EmmcMode == Hs200) {
    //
    // Skip the switch operations if already in this mode
    //
    if (HsTiming == 2) {
      goto Done;
    }

    Status = MmcSwitchToHS200 (Private, Rca, ClockFreq, BusWidth);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "EmmcModeSelection: eMMC HS200 Mode Selection Failed!\n"));
    }
  } else if (EmmcMode == Hs400) {
    //
    // Skip the switch operations if already in this mode
    //
    if (HsTiming == 3) {
      goto Done;
    }

    Status = MmcSwitchToHS400 (Private, Rca, ClockFreq);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "EmmcModeSelection: eMMC HS400 Mode Selection Failed!\n"));
    }
  } else {
    DEBUG ((DEBUG_ERROR, "EmmcModeSelection: Unsupported mode! Only HS200/HS400 are supported here\n"));
    Status = EFI_INVALID_PARAMETER;
  }

Done:
  return Status;
}

/**
  This function gets serial number of eMMC card.

  @param[out] SerialNumber              Serial Number buffer.
  @param[in]  Length                    Serial Number buffer length.

  @retval EFI_SUCCESS                   Serial Number Valid.
  @retval Others                        A parameter was incorrect.
**/
EFI_STATUS
EFIAPI
EmmcGetSerialNumber (
  IN  CHAR8*                            SerialNumber,
  IN  UINT32                            Length
  )
{
  EFI_STATUS               Status;
  SD_MMC_HC_PRIVATE_DATA  *Private;
  EMMC_CARD_DATA          *CardData;
  UINT32                   ProductNumber;
  UINT8                    Index;

  Status      = EFI_SUCCESS;

  if (!MmcIsInitialized()) {
    AsciiStrCpyS (SerialNumber, Length, "badbadbadbadba");
    Status = EFI_NOT_READY;
    goto Done;
  }

  Private = MmcGetHcPrivateData ();
  if (Private == NULL) {
    DEBUG ((DEBUG_ERROR, "Memory Allocation Fail - Out Of Resource\n"));
    Status = EFI_OUT_OF_RESOURCES;
    goto Done;
  }

  CardData = (EMMC_CARD_DATA *) Private->Slot.CardData;

  ProductNumber = CardData->Cid.ProductSerialNumber[3] << 24 | CardData->Cid.ProductSerialNumber[2] << 16 |
                  CardData->Cid.ProductSerialNumber[1] << 8  | CardData->Cid.ProductSerialNumber[0];

  Index = 0;
  SerialNumber[Index++] = CardData->Cid.ProductName[5];
  SerialNumber[Index++] = CardData->Cid.ProductName[4];
  SerialNumber[Index++] = CardData->Cid.ProductName[3];
  SerialNumber[Index++] = CardData->Cid.ProductName[2];
  SerialNumber[Index++] = CardData->Cid.ProductName[1];
  SerialNumber[Index++] = CardData->Cid.ProductName[0];

  AsciiValueToStringS (&SerialNumber[Index], Length - Index, PREFIX_ZERO | RADIX_HEX, ProductNumber, 8);
  do {
    if (('A' <= SerialNumber[Index]) && (SerialNumber[Index] <= 'Z')) {
      SerialNumber[Index] += 0x20;
    }
  } while (Index++ < 14);

Done:
  return Status;
}
