/** @file

  Copyright (c) 2017 - 2023, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __ME_FW_UPDATELIB_H__
#define __ME_FW_UPDATELIB_H__

#define FPT_PARTITION_NAME_FTPR         0x52505446
#define FPT_PARTITION_NAME_RBEP         0x50454252
#define FPT_PARTITION_NAME_NFTP         0x5054464E
#define FPT_PARTITION_NAME_PMCP         0x50434D50
#define FPT_PARTITION_NAME_PCHC         0x43484350
#define FPT_PARTITION_NAME_OEMP         0x504D454F
#define FPT_PARTITION_NAME_ISHC         0x43485349
#define FPT_PARTITION_NAME_IUNP         0x504E5549
#define FPT_PARTITION_NAME_LOCL         0x4C434F4C
#define FPT_PARTITION_NAME_WCOD         0x444F4357
#define FPT_PARTITION_NAME_IOMP         0x504D4F49
#define FPT_PARTITION_NAME_NPHY         0x5948504E
#define FPT_PARTITION_NAME_TBTP         0x50544254
#define FPT_PARTITION_NAME_SPHY         0x59485053
#define FPT_PARTITION_NAME_ISIF         0x46495349
#define FPT_PARTITION_NAME_SAMF         0x464D4153
#define FPT_PARTITION_NAME_PPHY         0x59485050
#define FPT_PARTITION_NAME_GBST         0x54534247
#define FPT_PARTITION_NAME_PSEP         0x50455350
#define FPT_PARTITION_NAME_ADSP         0x50534441
#define FPT_PARTITION_NAME_PCOD         0x444F4350
#define FPT_PARTITION_NAME_DCOD         0x444F4344
#define FPT_PARTITION_NAME_ACOD         0x444F4341
#define FPT_PARTITION_NAME_EFWS         0x53574645
#define FPT_PARTITION_NAME_EFWP         0x50574645
#define FPT_PARTITION_NAME_PMCS         0x53434D50
#define FPT_PARTITION_NAME_SSPH         0x48505353
#define FPT_PARTITION_NAME_SOCC         0x43434F53
#define FPT_PARTITION_NAME_CNVI         0x49564E43

#define MFT_PART_INFO_EXT_UPDATE_ACTION_NONE         0
#define MFT_PART_INFO_EXT_UPDATE_ACTION_HOST_RESET   1
#define MFT_PART_INFO_EXT_UPDATE_ACTION_CSE_RESET    2
#define MFT_PART_INFO_EXT_UPDATE_ACTION_GLOBAL_RESET 3

#define FW_UPDATE_DISABLED                  0 // Full Disabled. Partial Enabled.
#define FW_UPDATE_ENABLED                   1 // Full Enabled.  Partial Enabled.
#define FW_UPDATE_FULL_AND_PARTIAL_DISABLED 3 // Full Disabled. Partial Disabled.

#define FWU_FW_TYPE_INVALID 0
#define FWU_FW_TYPE_RESERVED 1
#define FWU_FW_TYPE_SLIM 2
#define FWU_FW_TYPE_CONSUMER 3
#define FWU_FW_TYPE_CORPORATE 4

#define FWU_PCH_SKU_INVALID 0
#define FWU_PCH_SKU_H 1
#define FWU_PCH_SKU_LP 2

#define CALL_CONV
/**
  Starting a Full FW Update from a buffer.

  @param[in]  Buffer        Buffer of Update Image read from Update Image file.
  @param[in]  BufferLength  Length of the buffer in bytes.
  @param[in]  OemId         OEM ID to compare with OEM ID in FW (if exist). Can be NULL.
  @param[in]  Func          A callback function that reports the progress of sending the buffer
                            to FW (not the progress of the update itself). Can be NULL.
                            void* Func(UINT32 BytesSentToFw, UINT32 TotalBytesToSendToFw);

  @retval SUCCESS  If Update started successfully. Error code otherwise.
**/
UINT32
EFIAPI
FwuFullUpdateFromBuffer(
  IN  UINT8 *Buffer,
  IN  UINT32 BufferLength,
  IN  EFI_GUID *OemId,
  IN  void(EFIAPI *Func)(UINT32, UINT32)
  );

/**
  Starting a Partial FW Update from a buffer.

  @param[in]  Buffer        Buffer of Update Image read from Update Image file.
  @param[in]  BufferLength  Length of the buffer in bytes.
  @param[in]  PartitionId   ID of partition to update. Only partitions that are allowed to be partially updated.
  @param[in]  Func          A callback function that reports the progress of sending the buffer
                            to FW (not the progress of the update itself). Can be NULL.
                            void* Func(UINT32 BytesSentToFw, UINT32 TotalBytesToSendToFw);

  @retval SUCCESS  If Update started successfully. Error code otherwise.
**/
UINT32
EFIAPI
FwuPartialUpdateFromBuffer(
  IN  UINT8 *Buffer,
  IN  UINT32 BufferLength,
  IN  UINT32 PartitionId,
  IN  void(EFIAPI *Func)(UINT32, UINT32)
  );

/**
  Check for Update progress.
  If in progress, return the current percent.
  If finished, return the status of the update, and the needed reset type after the update.
  This function should be called only after starting the update by calling FwuUpdateFull/Partial...

  @param[out] InProgress       True if Update is still in progress. False if Update finished. Caller allocated.
  @param[out] CurrentPercent   Current percent of the update, if Update is in progress. Caller allocated.
  @param[out] FwUpdateStatus   FW error code status of the update, if it finished (success or error code). Caller allocated.
  @param[out] NeededResetType  Needed reset type after the update, if it finished. Caller allocated.
                               MFT_PART_INFO_EXT_UPDATE_ACTION_NONE         0
                               MFT_PART_INFO_EXT_UPDATE_ACTION_HOST_RESET   1
                               MFT_PART_INFO_EXT_UPDATE_ACTION_CSE_RESET    2
                               MFT_PART_INFO_EXT_UPDATE_ACTION_GLOBAL_RESET 3

  @retval SUCCESS  If Update is still in progress, or finished successfully. Error code otherwise.
**/
UINT32
EFIAPI
FwuCheckUpdateProgress(
  OUT BOOLEAN *InProgress,
  OUT UINT32 *CurrentPercent,
  OUT UINT32 *FwUpdateStatus,
  OUT UINT32 *NeededResetType
  );

/**
  Get FW Update enabling state.

  @param[out] EnabledState  FW Update enabling state. Caller allocated.
                            FW_UPDATE_DISABLED = 0. Full Disabled. Partial Enabled.
                            FW_UPDATE_ENABLED = 1. Full Enabled. Partial Enabled.
                            FW_UPDATE_FULL_AND_PARTIAL_DISABLED = 3. Full Disabled. Partial Disabled.

  @retval SUCCESS  If succeeded. Error code otherwise.
**/
UINT32
EFIAPI
FwuEnabledState(
  OUT UINT16 *EnabledState
  );

/**
  Get OEM ID from flash.

  @param[out] OemId  OEM ID from flash. Caller allocated.

  @retval SUCCESS  If succeeded. Error code otherwise.
**/
UINT32
EFIAPI
FwuOemId(
  OUT EFI_GUID *OemId
  );

/**
  Get FW Type, from the flash image.

  @param[out] FwType  FW Type. Caller allocated.
                      FWU_FW_TYPE_INVALID 0
                      FWU_FW_TYPE_RESERVED 1
                      FWU_FW_TYPE_SLIM 2
                      FWU_FW_TYPE_CONSUMER 3
                      FWU_FW_TYPE_CORPORATE 4

  @retval SUCCESS  If succeeded. Error code otherwise.
**/
UINT32
EFIAPI
FwuFwType(
  OUT UINT32 *FwType
  );

/**
  Get PCH SKU, from the flash image.

  @param[out] PchSku  PCH SKU. Caller allocated.
                      FWU_PCH_SKU_INVALID 0
                      FWU_PCH_SKU_H 1
                      FWU_PCH_SKU_LP 2

  @retval SUCCESS  If succeeded. Error code otherwise.
**/
UINT32
EFIAPI
FwuPchSku(
  OUT UINT32 *PchSku
  );

/**
  Get version of a specific partition, from the flash image.

  @param[in]  PartitionId  ID of partition. If the FW version of CSE is needed,
                           use bup partition ID: FPT_PARTITION_NAME_FTPR/FPT_PARTITION_NAME_RBEP.
  @param[out] Major        Major number of version. Caller allocated.
  @param[out] Minor        Minor number of version. Caller allocated.
  @param[out] HotFix       Hotfix number of version. Caller allocated.
  @param[out] Build        Build number of version. Caller allocated.

  @retval SUCCESS  If succeeded. Error code otherwise.
**/
UINT32
EFIAPI
FwuPartitionVersionFromFlash(
  IN  UINT32 PartitionId,
  OUT UINT16 *Major,
  OUT UINT16 *Minor,
  OUT UINT16 *Hotfix,
  OUT UINT16 *Build
  );

/**
  Get version of a specific partition, from the Update Image buffer.

  @param[in]  Buffer        Buffer of Update Image read from Update Image file.
  @param[in]  BufferLength  Length of the buffer in bytes.
  @param[in]  PartitionId   ID of partition. If the FW version of CSE is needed,
                            use bup partition ID: FPT_PARTITION_NAME_FTPR/FPT_PARTITION_NAME_RBEP.
  @param[out] Major         Major number of version. Caller allocated.
  @param[out] Minor         Minor number of version. Caller allocated.
  @param[out] HotFix        Hotfix number of version. Caller allocated.
  @param[out] Build         Build number of version. Caller allocated.

  @retval SUCCESS  If succeeded. Error code otherwise.
**/
UINT32
EFIAPI
FwuPartitionVersionFromBuffer(
  IN  UINT8 *Buffer,
  IN  UINT32 BufferLength,
  IN  UINT32 PartitionId,
  OUT UINT16 *Major,
  OUT UINT16 *Minor,
  OUT UINT16 *Hotfix,
  OUT UINT16 *Build
  );

/**
  Get vendor ID of a specific partition, from the flash image.

  @param[in]  PartitionId  ID of partition.
  @param[out] VendorId     Vendor ID of partition. Caller allocated.

  @retval SUCCESS  If succeeded. Error code otherwise.
**/
UINT32
EFIAPI
FwuPartitionVendorIdFromFlash(
  IN  UINT32 PartitionId,
  OUT UINT32 *VendorId
  );

/**
  Get the the current image from the flash - Restore Point Image, and save it to buffer.

  @param[out] Buffer        Buffer of the saved Restore Point Image.
                            Allocated by the function, only in case of SUCCESS. NULL otherwise.
                            Caller should free the buffer
                            using FreePool() in EFI.
  @param[out] BufferLength  Length of the buffer in bytes.

  @retval SUCCESS  If succeeded. Error code otherwise.
**/
UINT32
EFIAPI
FwuSaveRestorePointToBuffer(
  OUT UINT8 **Buffer,
  OUT UINT32 *BufferLength
  );

/**
  Set ISH configuration file.
  Receive PDT file payload, create bios2ish file as a composition of
  bios2ish header (with PDT Update data type) and PDT file payload
  and send it to FW to set file.
  Supported only before EOP.

  @param[in] Buffer        Buffer of PDT file payload.
  @param[in] BufferLength  Length of the buffer in bytes.

  @retval SUCCESS  If succeeded. Error code otherwise.
**/
UINT32
EFIAPI
FwuSetIshConfig(
  IN  UINT8 *Buffer,
  IN  UINT32 BufferLength
  );

/**
  Get PDT version and VDV version from ISH file INTC_pdt.

  @param[out] PdtVersion  PDT version. Caller allocated.
  @param[out] VdvVersion  VDV version. Caller allocated.

  @retval SUCCESS  If succeeded. Error code otherwise.
**/
UINT32
EFIAPI
FwuGetIshPdtVersion(
  OUT UINT8 *PdtVersion,
  OUT UINT8 *VdvVersion
  );

#endif // __ME_FW_UPDATELIB_H__
