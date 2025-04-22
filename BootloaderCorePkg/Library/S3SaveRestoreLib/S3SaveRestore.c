/** @file

  Copyright (c) 2019 - 2021, Intel Corporation. All rights reserved.<BR>
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
#include <Guid/SmmS3CommunicationInfoGuid.h>
#include <Library/BootloaderCommonLib.h>

#define REG_APM_CNT   0xB2

/**
  Read register

  Reads a register with register type in Type and length in Width

  @param[in]      Type          type of register
  @param[in]      Width         width of register
  @param[in]      Addr          register address
  @param[in,out]  Val           register value

  @retval         EFI_SUCCESS   read done
  @retval         EFI_INVALID_PARAMETER   Invalid Type and Width

**/
static
EFI_STATUS
RegRead (
  IN      UINT8   Type,
  IN      UINT8   Width,
  IN      UINTN   Addr,
  IN OUT  UINT32 *Val
  )
{
  if (Val == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (Type == REG_TYPE_MMIO) {
    if (Width == WIDE8) {
      *Val = MmioRead8 (Addr);
    } else if (Width == WIDE16) {
      *Val = MmioRead16 (Addr);
    } else if (Width == WIDE32) {
      *Val = MmioRead32 (Addr);
    } else {
      return EFI_INVALID_PARAMETER;
    }
  } else if (Type == REG_TYPE_IO) {
    if (Width == WIDE8) {
      *Val = IoRead8 (Addr);
    } else if (Width == WIDE16) {
      *Val = IoRead16 (Addr);
    } else if (Width == WIDE32) {
      *Val = IoRead32 (Addr);
    } else {
      return EFI_INVALID_PARAMETER;
    }
  } else {
    return EFI_INVALID_PARAMETER;
  }

  return EFI_SUCCESS;
}

/**
  Write register

  Writes a register with register type in Type and length in Width

  @param[in]  Type         type of register
  @param[in]  Width        width of register
  @param[in]  Addr         register address
  @param[in]  Val          value to write

  @retval     EFI_SUCCESS  write done
  @retval     EFI_INVALID_PARAMETER   Invalid Type and Width

**/
static
EFI_STATUS
RegWrite (
  IN  UINT8   Type,
  IN  UINT8   Width,
  IN  UINTN   Addr,
  IN  UINT32  Val
  )
{
  if (Type == REG_TYPE_MMIO) {
    if (Width == WIDE8) {
      (void)MmioWrite8 (Addr, (UINT8)Val);
    } else if (Width == WIDE16) {
      (void)MmioWrite16 (Addr, (UINT16)Val);
    } else if (Width == WIDE32) {
      (void)MmioWrite32 (Addr, Val);
    } else {
      return EFI_INVALID_PARAMETER;
    }
  } else if (Type == REG_TYPE_IO) {
    if (Width == WIDE8) {
      (void)IoWrite8 (Addr, (UINT8)Val);
    } else if (Width == WIDE16) {
      (void)IoWrite16 (Addr, (UINT16)Val);
    } else if (Width == WIDE32) {
      (void)IoWrite32 (Addr, Val);
    } else {
      return EFI_INVALID_PARAMETER;
    }
  } else {
    return EFI_INVALID_PARAMETER;
  }

  return EFI_SUCCESS;
}

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
  PLD_TO_BL_SMM_INFO *PldToBlSmmInfo;

  if ((PcdGet8(PcdBuildSmmHobs) & BIT1) != 0) {
    PldToBlSmmInfo = FindS3Info (BL_SW_SMI_COMM_ID);
    if ((PldToBlSmmInfo != NULL) && CompareGuid(&PldToBlSmmInfo->Header.Name, &gPldS3CommunicationGuid)) {
      DEBUG((DEBUG_INFO, "TriggerPayloadSwSmi: 0x%x\n", PldToBlSmmInfo->S3Info.SwSmiTriggerValue));
      IoWrite8(REG_APM_CNT, PldToBlSmmInfo->S3Info.SwSmiTriggerValue);
      return;
    }
  }

  if ((PcdGet8(PcdBuildSmmHobs) & BIT0) != 0) {
    DEBUG((DEBUG_INFO, "TriggerPayloadSwSmi -: 0x%x\n", SwSmiNumber));
    IoWrite8(REG_APM_CNT, SwSmiNumber);
  }
}

/**
  This function clears TSEG area designated for S3
  save/restore purpose.

**/
VOID
EFIAPI
ClearS3SaveRegion (
  VOID
)
{
  LDR_SMM_INFO      LdrSmmInfo;
  UINT8             *SmmBase;

  PlatformUpdateHobInfo (&gSmmInformationGuid, &LdrSmmInfo);
  SmmBase = (UINT8 *)(UINTN)LdrSmmInfo.SmmBase;
  if (LdrSmmInfo.Flags & SMM_FLAGS_4KB_COMMUNICATION) {
    ZeroMem (SmmBase, SIZE_4KB);
  }
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
  LDR_SMM_INFO         LdrSmmInfo;
  UINT8                *SmmBase;
  UINT32               SmmSize;
  BL_PLD_COMM_HDR      *CommHdr;
  SMMBASE_INFO         *SmmBaseInfo;
  PLD_TO_BL_SMM_INFO   *PldToBlSmmInfo;

  PlatformUpdateHobInfo (&gSmmInformationGuid, &LdrSmmInfo);
  if ((PcdGet8(PcdBuildSmmHobs) & BIT1) != 0) {
    PldToBlSmmInfo = (PLD_TO_BL_SMM_INFO *) (UINTN)LdrSmmInfo.SmmBase;
    if (CompareGuid(&PldToBlSmmInfo->Header.Name, &gPldS3CommunicationGuid)) {
      return PldToBlSmmInfo;
    }
  }

  if ((PcdGet8(PcdBuildSmmHobs) & BIT0) != 0) {
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
        if (Id == SMMBASE_INFO_COMM_ID) {
          SmmBaseInfo = (SMMBASE_INFO *) CommHdr;
          if ((SmmBaseInfo->SmmBase[0].SmmBase == 0) && (SmmBaseInfo->SmmBase[0].ApicId == 0)){
            // It means SMM rebase info is not really filled.
            return NULL;
          }
        }
        return (VOID *) CommHdr;
      }
      CommHdr = (BL_PLD_COMM_HDR *) ((UINT8 *)CommHdr + CommHdr->TotalSize);
    }
  }

  DEBUG((DEBUG_ERROR, "FindS3Info: not found ID:0x%x\n", Id));
  return NULL;
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
  EFI_STATUS Status;

  if (S3SaveReg == NULL || S3SaveReg->S3SaveHdr.Id != S3_SAVE_REG_COMM_ID) {
    return EFI_INVALID_PARAMETER;
  }

  for (Index = 0; Index < S3SaveReg->S3SaveHdr.Count; Index++) {
    if (S3SaveReg->RegInfo[Index].Addr != 0x00) {
      Type = S3SaveReg->RegInfo[Index].Type;
      Width = S3SaveReg->RegInfo[Index].Width;

      Status = RegWrite (Type, Width, S3SaveReg->RegInfo[Index].Addr, S3SaveReg->RegInfo[Index].Val);
      if (EFI_ERROR (Status)) {
        return Status;
      }
      Status = RegRead (Type, Width, S3SaveReg->RegInfo[Index].Addr, &Data32);
      if (EFI_ERROR (Status)) {
        return Status;
      }
      DEBUG ((DEBUG_INFO, "Value after restore reg @ 0x%08X=0x%08X\n", S3SaveReg->RegInfo[Index].Addr, Data32));
    }
  }

  return EFI_SUCCESS;
}
