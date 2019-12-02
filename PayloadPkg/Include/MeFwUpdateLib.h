/** @file

  Copyright (c) 2017 - 2019, Intel Corporation. All rights reserved.<BR>
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
#define FPT_PARTITION_NAME_WCOD         0x444f4357
#define FPT_PARTITION_NAME_IOMP         0x504D4F49
#define FPT_PARTITION_NAME_MGPP         0x5050474D
#define FPT_PARTITION_NAME_TBTP         0x50544254
#define FPT_PARTITION_NAME_DPHY         0x59485044

#define MFT_PART_INFO_EXT_UPDATE_ACTION_NONE         0
#define MFT_PART_INFO_EXT_UPDATE_ACTION_HOST_RESET   1
#define MFT_PART_INFO_EXT_UPDATE_ACTION_CSE_RESET    2
#define MFT_PART_INFO_EXT_UPDATE_ACTION_GLOBAL_RESET 3

#define FW_UPDATE_DISABLED 0
#define FW_UPDATE_ENABLED 1

#define FWU_FW_TYPE_INVALID 0
#define FWU_FW_TYPE_RESERVED 1
#define FWU_FW_TYPE_SLIM 2
#define FWU_FW_TYPE_CONSUMER 3
#define FWU_FW_TYPE_CORPORATE 4

#define FWU_PCH_SKU_INVALID 0
#define FWU_PCH_SKU_H 1
#define FWU_PCH_SKU_LP 2

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
FwuFullUpdateFromBuffer(
  IN  UINT8 *Buffer,
  IN  UINT32 BufferLength,
  IN  EFI_GUID *OemId,
  IN  void(*Func)(UINT32, UINT32)
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
FwuPartialUpdateFromBuffer(
  IN  UINT8 *Buffer,
  IN  UINT32 BufferLength,
  IN  UINT32 PartitionId,
  IN  void(*Func)(UINT32, UINT32)
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
FwuCheckUpdateProgress(
  OUT BOOLEAN *InProgress,
  OUT UINT32 *CurrentPercent,
  OUT UINT32 *FwUpdateStatus,
  OUT UINT32 *NeededResetType
  );

/**
  Get FW Update enabling state: enabled, disabled.

  @param[out] EnabledState  FW Update enabling state: enabled, disabled. Caller allocated.
                            FW_UPDATE_DISABLED = 0.
                            FW_UPDATE_ENABLED = 1.

  @retval SUCCESS  If succeeded. Error code otherwise.
**/
UINT32
FwuEnabledState(
  OUT UINT16 *EnabledState
  );

/**
  Get OEM ID from flash.

  @param[out] OemId  OEM ID from flash. Caller allocated.

  @retval SUCCESS  If succeeded. Error code otherwise.
**/
UINT32
FwuOemId(
  OUT EFI_GUID *OemId
  );

/**
  Get FW Type.

  @param[out] FwType  FW Type. Caller allocated.
                      FWU_FW_TYPE_INVALID 0
                      FWU_FW_TYPE_RESERVED 1
                      FWU_FW_TYPE_SLIM 2
                      FWU_FW_TYPE_CONSUMER 3
                      FWU_FW_TYPE_CORPORATE 4

  @retval SUCCESS  If succeeded. Error code otherwise.
**/
UINT32
FwuFwType(
  OUT UINT32 *FwType
  );

/**
  Get PCH SKU.

  @param[out] PchSku  PCH SKU. Caller allocated.
                      FWU_PCH_SKU_INVALID 0
                      FWU_PCH_SKU_H 1
                      FWU_PCH_SKU_LP 2

  @retval SUCCESS  If succeeded. Error code otherwise.
**/
UINT32
FwuPchSku(
  OUT UINT32 *PchSku
  );

/**
  Get version of a specific partition, from the flash image.

  @param[in]  PartitionId  ID of partition. If the FW version of CSE is needed,
                           use FTPR partition ID: FPT_PARTITION_NAME_FTPR.
  @param[out] Major        Major number of version. Caller allocated.
  @param[out] Minor        Minor number of version. Caller allocated.
  @param[out] HotFix       Hotfix number of version. Caller allocated.
  @param[out] Build        Build number of version. Caller allocated.

  @retval SUCCESS  If succeeded. Error code otherwise.
**/
UINT32
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
                            use FTPR partition ID: FPT_PARTITION_NAME_FTPR.
  @param[out] Major         Major number of version. Caller allocated.
  @param[out] Minor         Minor number of version. Caller allocated.
  @param[out] HotFix        Hotfix number of version. Caller allocated.
  @param[out] Build         Build number of version. Caller allocated.

  @retval SUCCESS  If succeeded. Error code otherwise.
**/
UINT32
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
FwuPartitionVendorIdFromFlash(
  IN  UINT32 PartitionId,
  OUT UINT32 *VendorId
  );

/**
  Get the the current image from the flash - Restore Point Image, and save it to buffer.

  @param[out] buffer        Buffer of the saved Restore Point Image.
                            Allocated by the function, only in case of SUCCESS. NULL otherwise.
                            Caller should free the buffer
                            using FreePool() in EFI.
  @param[out] bufferLength  Length of the buffer in bytes.

  @retval SUCCESS  If succeeded. Error code otherwise.
**/
UINT32
FwuSaveRestorePointToBuffer(
  OUT UINT8 **buffer,
  OUT UINT32 *bufferLength
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
FwuGetIshPdtVersion(
  OUT UINT8 *PdtVersion,
  OUT UINT8 *VdvVersion
  );

#endif
