/** @file

  Copyright (c) 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <PiPei.h>
#include <Library/DebugLib.h>
#include <Library/HobLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/S3SaveRestoreLib.h>
#include <Library/IoLib.h>
#include <Guid/SmmInformationGuid.h>
#include <Library/BoardInitLib.h>
#include <Guid/SmmInformationGuid.h>
#include <Base.h>

typedef UINT32 (*REG_READ)  (UINTN);
typedef VOID   (*REG_WRITE) (UINTN, UINT32);

#define NUM_TYPE    2
#define NUM_WIDTH   3
#define REG_APM_CNT   0xB2

const REG_WRITE  mRegWrite[NUM_TYPE][NUM_WIDTH] = {
  { (REG_WRITE) MmioWrite8, (REG_WRITE) MmioWrite16, (REG_WRITE) MmioWrite32 },
  { (REG_WRITE) IoWrite8, (REG_WRITE) IoWrite16, (REG_WRITE) IoWrite32 }
};

const REG_READ mRegRead[NUM_TYPE][NUM_WIDTH] = {
  { (REG_READ) MmioRead8, (REG_READ) MmioRead16, (REG_READ) MmioRead32 },
  { (REG_READ) IoRead8, (REG_READ) IoRead16, (REG_READ) IoRead32 }
};

const UINT8 mWidthToIdx[NUM_WIDTH][2] = {
  { 0, WIDE8  },
  { 1, WIDE16 },
  { 2, WIDE32 }
};

/**
  Trigger payload software SMI

  This function triggers software SMI. SMI number will be obtained
  from SMM communication area.

  @param[in]  SwSmiNumber   Software smi number to be triggered.

**/
VOID
TriggerPayloadSwSmi (
  IN UINT8  SwSmiNumber
)
{
  IoWrite8(REG_APM_CNT, SwSmiNumber);
}

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
  )
{
  LDR_SMM_INFO      LdrSmmInfo;
  UINT8             *SmmBase;
  UINT32            SmmSize;
  BL_PLD_COMM_HDR   *CommHdr;
  BL_PLD_COMM_HDR   *DataHdr;

  PlatformUpdateHobInfo (&gSmmInformationGuid, &LdrSmmInfo);
  SmmBase = (UINT8 *)(UINTN)LdrSmmInfo.SmmBase;
  if (LdrSmmInfo.Flags & SMM_FLAGS_4KB_COMMUNICATION) {
    SmmSize = SIZE_4KB;
  } else {
    return EFI_OUT_OF_RESOURCES;
  }

  CommHdr = (BL_PLD_COMM_HDR *) SmmBase;
  DataHdr = (BL_PLD_COMM_HDR *) DataPtr;

  while ( ((UINT8 *)CommHdr < SmmBase + SmmSize) &&
          CommHdr->Signature == BL_PLD_COMM_SIG) {
    CommHdr = (BL_PLD_COMM_HDR *) ((UINT8 *)CommHdr + CommHdr->TotalSize);
  }

  if ( (UINT8 *)CommHdr + DataHdr->TotalSize > SmmBase + SmmSize ) {
    return EFI_OUT_OF_RESOURCES;
  }

  //
  // For some IDs, we are only copying the header and rest of TotalSize
  // is placeholder for payload to fill in. So, do a ZeroMem on data (TotalSize - sizeof(header)).
  //
  if (IsHdrOnly) {
    CopyMem ((VOID *)CommHdr, DataPtr, sizeof(BL_PLD_COMM_HDR));
    ZeroMem ((UINT8 *)CommHdr + sizeof(BL_PLD_COMM_HDR), DataHdr->TotalSize - sizeof(BL_PLD_COMM_HDR));
  } else {
    CopyMem ((VOID *)CommHdr, DataPtr, DataHdr->TotalSize);
  }

  return EFI_SUCCESS;
}


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
  )
{
  LDR_SMM_INFO      LdrSmmInfo;
  UINT8             *SmmBase;
  UINT32            SmmSize;
  BOOLEAN           FoundInfo = FALSE;
  BL_PLD_COMM_HDR   *CommHdr;

  PlatformUpdateHobInfo (&gSmmInformationGuid, &LdrSmmInfo);
  SmmBase = (UINT8 *)(UINTN)LdrSmmInfo.SmmBase;
  if (LdrSmmInfo.Flags & SMM_FLAGS_4KB_COMMUNICATION) {
    SmmSize = SIZE_4KB;
  } else {
    DEBUG((DEBUG_INFO, "Invalid Communication area size\n"));
    return NULL;
  }

  CommHdr = (BL_PLD_COMM_HDR *) SmmBase;
  while ( ((UINT8 *)CommHdr < SmmBase + SmmSize) &&
          CommHdr->Signature == BL_PLD_COMM_SIG) {
    if (CommHdr->Id == Id) {
      FoundInfo = TRUE;
      break;
    }
    CommHdr = (BL_PLD_COMM_HDR *) ((UINT8 *)CommHdr + CommHdr->TotalSize);
  }

  if (!FoundInfo) {
    return NULL;
  }

  return (VOID *) CommHdr;
}


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
  )
{
  UINT8     Index;
  UINT32    Data32;
  UINT8     Type;
  UINT8     Width;
  UINT8     Idx;

  if (S3SaveReg == NULL || S3SaveReg->S3SaveHdr.Id != S3_SAVE_REG_COMM_ID) {
    return EFI_INVALID_PARAMETER;
  }

  for (Index = 0; Index < S3SaveReg->S3SaveHdr.Count; Index++) {
    if (S3SaveReg->RegInfo[Index].Addr != 0x00) {
      Type = S3SaveReg->RegInfo[Index].Type;
      Width = S3SaveReg->RegInfo[Index].Width;
      for (Idx = 0; Idx < NUM_WIDTH; Idx++) {
        if (mWidthToIdx[Idx][1] == Width) {
          Width = Idx;
          break;
        }
      }
      if (Type >= NUM_TYPE || Width >= NUM_WIDTH) {
        return EFI_INVALID_PARAMETER;
      }
      mRegWrite[Type][Width] (S3SaveReg->RegInfo[Index].Addr, S3SaveReg->RegInfo[Index].Val);
      Data32 = mRegRead[Type][Width] (S3SaveReg->RegInfo[Index].Addr);
      DEBUG ((DEBUG_INFO, "Value after restore reg @ 0x%08X=0x%08X\n", S3SaveReg->RegInfo[Index].Addr, Data32));
    }
  }

  return EFI_SUCCESS;
}
