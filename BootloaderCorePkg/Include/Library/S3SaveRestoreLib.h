/** @file

  Copyright (c) 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _S3_SAVE_RESTORE_LIB_H_
#define _S3_SAVE_RESTORE_LIB_H_

#include <Guid/SmmS3CommunicationInfoGuid.h>

#define BL_PLD_COMM_SIG       SIGNATURE_32('B', 'P', 'C', 'O')

#define SMMBASE_INFO_COMM_ID  1
#define S3_SAVE_REG_COMM_ID   2
#define BL_SW_SMI_COMM_ID     3

//
// Format to share info between bootloader and payload.
// Structures can be present in any order within the 4KB
// communication area starting from TSEG. They are
// identified by their Id.
//

      //  -------------------------- <----TSEG_BASE------
      //  |     BL_PLD_COMM_HDR    |
      //  |         (Id:1)         |
      //  --------------------------
      //  |       SMMBASE_INFO     |
      //  --------------------------
      //  --------------------------
      //  |     BL_PLD_COMM_HDR    |
      //  |         (Id:*)         |
      //  --------------------------
      //  |      COMM_STRUCT_2     |
      //  --------------------------
      //  --------------------------
      //  |     BL_PLD_COMM_HDR    |
      //  |         (Id:*)         |
      //  --------------------------
      //  |      COMM_STRUCT_3     |
      //  --------------------------
      //  --------------------------
      //  |     BL_PLD_COMM_HDR    |
      //  |         (Id:2)         |
      //  --------------------------
      //  |      S3_SAVE_REG       |
      //  --------------------------
      //  --------------------------
      //  |     BL_PLD_COMM_HDR    |
      //  |         (Id:*)         |
      //  --------------------------
      //  |      COMM_STRUCT_5     |
      //  --------------------------
      //  --------------------------
      //            .....
      //            .....
      //            .....


#pragma pack(1)

typedef struct {
  UINT32  Signature;
  UINT8   Id;
  UINT8   Count;
  UINT16  TotalSize;
} BL_PLD_COMM_HDR;

typedef struct {
  UINT8         Type;
  UINT8         Width;
  UINT8         Rsvd[2];
  UINT32        Addr;
  UINT32        Val;
} REG_INFO;

typedef struct {
  BL_PLD_COMM_HDR S3SaveHdr;
  REG_INFO        RegInfo[];
} S3_SAVE_REG;

typedef struct {
  BL_PLD_COMM_HDR SmmBaseHdr;
  CPU_SMMBASE     SmmBase[];
} SMMBASE_INFO;

typedef struct {
  BL_PLD_COMM_HDR BlSwSmiHdr;
  UINT8           BlSwSmiHandlerInput;
} BL_SW_SMI_INFO;

#pragma pack()

/**
  Trigger Payload software SMI

  This function triggers software SMI. SMI number will be obtained
  from SMM communication area.

  @param[in]  SwSmiNumber   Software smi number to be triggered.

**/
VOID
TriggerPayloadSwSmi (
  IN UINT8  SwSmiNumber
);

/**
  This function clears TSEG area designated for S3
  save/restore purpose.

**/
VOID
EFIAPI
ClearS3SaveRegion (
  VOID
);

/**
  This function appends information in TSEG area
  designated for S3 save/restore purpose.

  @param    DataPtr               Address of the structure to be copied to TSEG
  @param    IsHdrOnly             Reserve TotalSize, but populate only Header info

  @retval   EFI_OUT_OF_RESOURCES  If SmmSize is exceeding 4KiB
  @retval   EFI_OUT_OF_RESOURCES  If appeding new struct exceeds SmmSize
  @retval   EFI_SUCCESS           Append is successful

**/
EFI_STATUS
EFIAPI
AppendS3Info (
  IN  VOID     *DataPtr,
  IN  BOOLEAN   IsHdrOnly
  );


/**
  Get pointer to a specific struct area in TSEG

  @param      Id                Id of the struct to be retreived

  @retval     NULL              Struct with signature not found
  @retval     Ptr               Pointer to struct with signature

**/
VOID *
EFIAPI
FindS3Info (
  IN  UINT8   Id
  );


/**
  This function restores the states of the registers that were
  set to be saved by the bootloader in the normal boot path.
  After the payload has updated the 'Val' field of the reg info
  in the normal boot path, we simply OR the updated 'Val'
  with the existing vale of the register in the S3 resume boot path.
  This function is only called in the S3 resume path.

  @param    S3SaveReg               S3_SAVE_REG info offset

  @retval   EFI_INVALID_PARAMETER   Invalid pointer to S3_SAVE_REG in Communicaton region
  @retval   EFI_INVALID_PARAMETER   Invalid Type and Width
  @retval   EFI_SUCCESS             Restore successful

**/
EFI_STATUS
EFIAPI
RestoreS3RegInfo (
  IN  S3_SAVE_REG   *S3SaveReg
  );


#endif
