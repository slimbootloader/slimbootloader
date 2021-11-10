/** @file

  Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

/*
 * RpmbLib.h
 */
#ifndef _RPMB_LIB_H_
#define _RPMB_LIB_H_

#include <Uefi/UefiBaseType.h>
#include <Guid/OsBootOptionGuid.h>

typedef enum {
  RpmbResponseOK,
  RpmbResponseGeneralFailure,
  RpmbResponseAuthFailure,
  RpmbResponseCounterFailure,
  RpmbResponseAddressFailure,
  RpmbResponseWriteFailure,
  RpmbResponseReadFailure,
  RpmbResponseNoAuthKeyProgram,
  RpmbResponseWriteCounterExpired = 0X80,
} RPMB_RESPONSE_RESULT;

#pragma pack(1)
typedef struct {
  UINT8 Stuff[196];
  UINT8 KeyMac[32];
  UINT8 Data[256];
  UINT8 Nonce[16];
  UINT32 WriteCounter;
  UINT16 Address;
  UINT16 BlockCnt;
  UINT16 Result;
  UINT16 ReqResp;
} RPMB_DATA_FRAME;
#pragma pack()

#define RPMB_RESPONSE_KEY_WRITE     0x0100
#define RPMB_RESPONSE_COUNTER_READ  0x0200
#define RPMB_RESPONSE_AUTH_WRITE    0x0300
#define RPMB_RESPONSE_AUTH_READ     0x0400
#define RPMB_RESPONSE_READ_RESULT   0x0500

#define RPMB_REQUEST_KEY_WRITE      0x0001
#define RPMB_REQUEST_COUNTER_READ   0x0002
#define RPMB_REQUEST_AUTH_WRITE     0x0003
#define RPMB_REQUEST_AUTH_READ      0x0004
#define RPMB_REQUEST_STATUS         0x0005
#define RPMB_BLOCK_SIZE             256

/**
  This function reads the counter value of the RPMB Partition.

  @param[in]       MediumType           Current Medium Type option: eMMC, UFS, NVMe
  @param[in]       Key                  Input Key to authenticate and compare against the
                                        embbeded MAC in RPMB_DATA_FRAME.
  @param[in]       KeySize              Size of the Input Key.
  @param[out]      WriteCounter         How many times a successful write request has been done so far.
  @param[out]      ResponseResult       Output coming from CounterFrame->Result


  @retval          EFI_SUCCESS          The operation is done correctly.
  @retval          Others               The operation fails due to some error.
**/
EFI_STATUS
EFIAPI
RpmbGetCounter (
  IN OS_BOOT_MEDIUM_TYPE    MediumType,
  IN CONST UINT8           *Key,
  IN UINT8                  KeySize,
  OUT UINT32               *WriteCounter,
  OUT RPMB_RESPONSE_RESULT *ResponseResult
  );

/**
  This function programs the RPMB key into RPMB Partition.

  @param[in]       MediumType     Current Medium Type option: eMMC, UFS, NVMe
  @param[in]       Lba            The starting logical block address to be read/written.
                                  The caller is responsible for reading/writing to only
                                  legitimate locations.
  @param[in]       Key            Input Key to check if programmed or not.
  @param[in]       KeySize        Size of the Input Key.
  @param[out]      ResponseResult Output coming from the Response DataFrame.

  @retval          EFI_SUCCESS    The operation is done correctly.
  @retval          Others         The operation fails due to some error.
**/
EFI_STATUS
EFIAPI
RpmbProgramKey (
  IN OS_BOOT_MEDIUM_TYPE            MediumType,
  IN  EFI_LBA                       Lba,
  IN  CONST UINT8                   *Key,
  IN UINT8                          KeySize,
  OUT RPMB_RESPONSE_RESULT          *ResponseResult
  );

/**
  This function writes to RPMB memory partition. Request Message Type is
  Authenticated data write. If the MAC in the request message and the calculated MAC are
  different, then this returns Authentication Failure. No data will be written.

  @param[in]       MediumType             Current Medium Type option: eMMC, UFS, NVMe
  @param[in]       Lba                    The starting logical block address to be read/written.
                                          The caller is responsible for reading/writing to only
                                          legitimate locations.
  @param[in]       BlkCnt                 Number of RPMB_DATA_FRAME(s).
  @param[in]       BlkAddr                Start address of the full packet.
  @param[in]       Buffer                 Data to be written. Should be a multiple of RPMB_BLOCK_SIZE.
  @param[in]       Key                    Input Key to calculate the MAC.
  @param[in]       KeySize                Size of the Input Key.
  @param[out]      Result                 Output coming from the Response DataFrame.

  @retval          EFI_SUCCESS            The operation is done correctly.
  @retval          Others                 The operation fails due to some error.

**/
EFI_STATUS
EFIAPI
RpmbWriteData (
  IN OS_BOOT_MEDIUM_TYPE           MediumType,
  IN EFI_LBA                       Lba,
  IN UINT8                         BlkCnt,
  IN UINT16                        BlkAddr,
  IN VOID                          *Buffer,
  IN CONST UINT8                   *Key,
  IN UINT8                         KeySize,
  OUT RPMB_RESPONSE_RESULT         *Result
  );

/**
  This function reads data from RPMB memory partition. Request Message Type is
  Authenticated data read. If the MAC in the request message and the calculated MAC are
  different, then this returns Authentication Failure. No data will be read back.

  @param[in]       MediumType             Current Medium Type option: eMMC, UFS, NVMe
  @param[in]       Lba                    The starting logical block address to be read/written.
                                          The caller is responsible for reading/writing to only
                                          legitimate locations.
  @param[in]       BlkCount               Number of RPMB_DATA_FRAME(s).
  @param[in]       BlkAddr                Start address of the full packet.
  @param[out]      Buffer                 Data to be read back. Should be a multiple of RPMB_BLOCK_SIZE.
  @param[in]       Key                    Input Key to calculate the MAC.
  @param[in]       KeySize                Size of the Input Key.
  @param[out]      Result                 This is the output coming from the Response DataFrame.

  @retval          EFI_SUCCESS            The operation is done correctly.
  @retval          Others                 The operation fails due to some error.

**/
EFI_STATUS
EFIAPI
RpmbReadData (
  IN OS_BOOT_MEDIUM_TYPE    MediumType,
  IN EFI_LBA                Lba,
  IN UINT8                  BlkCount,
  IN UINT16                 BlkAddr,
  OUT VOID                  *Buffer,
  IN CONST UINT8            *Key,
  IN UINT8                  KeySize,
  OUT RPMB_RESPONSE_RESULT  *Result
  );

#endif
