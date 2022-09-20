/** @file
  Shell command corrupt an SBL component (for test purposes).

  Copyright (c) 2022, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Library/ShellLib.h>
#include <Library/BootloaderCommonLib.h>
#include <Library/RngLib.h>
#include <Service/SpiFlashService.h>
#include <Library/MemoryAllocationLib.h>

/**
  Perform random corruption in SBL region.

  @param[in]  Shell        shell instance
  @param[in]  Argc         number of command line arguments
  @param[in]  Argv         command line arguments

  @retval EFI_SUCCESS

**/
EFI_STATUS
EFIAPI
ShellCommandCorruptCompFunc (
  IN SHELL  *Shell,
  IN UINTN   Argc,
  IN CHAR16 *Argv[]
  );

CONST SHELL_COMMAND ShellCommandCorruptComp = {
  L"corruptcomp",
  L"Perform random corruption in SBL component",
  &ShellCommandCorruptCompFunc
};

/**
  Show usage prompt to screen.

  @param[in]  Argv         Command line arguments

  @retval EFI_SUCCESS

**/
VOID
ShowUsage (
  VOID
  )
{
  ShellPrint (L"Usage: corruptcomp <boot partition> <component>\n");
  ShellPrint (L"Example: corruptcomp 1 SG1A\n");
  ShellPrint (L"Example: corruptcomp 0 SG1B\n");
  ShellPrint (L"Example: corruptcomp 0 SG02\n");
}

/**
  Perform random corruption in SBL component.

  @param[in]  Shell        Shell instance
  @param[in]  Argc         Number of command line arguments
  @param[in]  Argv         Command line arguments

  @retval EFI_SUCCESS

**/
EFI_STATUS
EFIAPI
ShellCommandCorruptCompFunc (
  IN SHELL  *Shell,
  IN UINTN   Argc,
  IN CHAR16 *Argv[]
  )
{
  EFI_STATUS          Status;
  BOOLEAN             IsBackup;
  CHAR16              *CompStr;
  UINT32              CompSig;
  SPI_FLASH_SERVICE   *SpiService;
  UINT32              BiosStart;
  FLASH_MAP           *FlashMap;
  UINT8               *Buf;
  UINT32              CompOffset;
  UINT32              CompSize;
  UINT32              RandOffset;
  UINT32              ChunkAddr;
  UINT32              ChunkAddrAligned;
  UINT8*              CurrVal;
  UINT16              RandVal;
  UINT8               NewVal;

  Status = EFI_SUCCESS;

  if (Argc == 3) {
    switch (StrDecimalToUintn (Argv[1])) {
      case 0:
        IsBackup = FALSE;
        break;
      case 1:
        IsBackup = TRUE;
        break;
      default:
        ShellPrint (L"Error, invalid boot partition\n");
        ShowUsage ();
        return EFI_INVALID_PARAMETER;
    }
    CompStr = Argv[2];
    if (StrLen(CompStr) != 4) {
      ShellPrint (L"Error, invalid component\n");
      ShowUsage ();
      return EFI_INVALID_PARAMETER;
    }
    CompSig = SIGNATURE_32 (CompStr[0], CompStr[1], CompStr[2], CompStr[3]);
  } else if (Argc < 3) {
    ShellPrint (L"Error, too few arguments\n");
    ShowUsage ();
    return EFI_INVALID_PARAMETER;
  } else if (Argc > 3) {
    ShellPrint (L"Error, too many arguments\n");
    ShowUsage ();
    return EFI_INVALID_PARAMETER;
  }

  SpiService = (SPI_FLASH_SERVICE *) GetServiceBySignature (SPI_FLASH_SERVICE_SIGNATURE);
  if (SpiService == NULL) {
    ShellPrint (L"Error, unable to retrieve SPI flash service\n");
    return EFI_UNSUPPORTED;
  }

  Status = SpiService->SpiInit ();
  if (EFI_ERROR(Status)) {
    ShellPrint (L"Error, unable to initialize SPI flash service %r\n", Status);
    return Status;
  }

  Status = SpiService->SpiGetRegion (FlashRegionBios, NULL, &BiosStart);
  if (EFI_ERROR(Status)) {
    ShellPrint (L"Error, unable to retrieve BIOS SPI flash address %r\n", Status);
    return Status;
  }

  FlashMap = GetFlashMapPtr();
  if (FlashMap == NULL) {
    ShellPrint (L"Error, unable to retrieve flash map\n");
    return EFI_NO_MAPPING;
  }

  if (BiosStart < FlashMap->RomSize) {
    ShellPrint (L"Error, SBL image is larger than the BIOS region\n");
    return EFI_ABORTED;
  }

  Buf = AllocatePages (EFI_SIZE_TO_PAGES (SIZE_4KB));
  if (Buf == NULL) {
    ShellPrint (L"Error, unable to allocate buffer\n");
    return EFI_OUT_OF_RESOURCES;
  }

  Status = GetComponentInfoByPartition (CompSig, IsBackup, &CompOffset, &CompSize);
  if (EFI_ERROR (Status)) {
    ShellPrint (L"Error, unable to retrieve SBL component info\n");
    return Status;
  }

  do {
    // Choose a random offset in the component to corrupt
    if (!GetRandomNumber32 (&RandOffset)) {
      ShellPrint (L"Error, unable to generate random offset\n");
      return EFI_UNSUPPORTED;
    }

    RandOffset %= CompSize;
    ChunkAddr = BiosStart + CompOffset + RandOffset;
    ChunkAddrAligned = ALIGN_DOWN (ChunkAddr, SIZE_4KB);

    // Read the whole 4KB chunk containing the offset from SPI flash
    Status = SpiService->SpiRead (FlashRegionBios,
                                  ChunkAddrAligned,
                                  SIZE_4KB,
                                  Buf);
    if (EFI_ERROR(Status)) {
      ShellPrint (L"Error, unable to be read SPI flash %r\n", Status);
      return Status;
    }

    CurrVal = Buf + ChunkAddr - ChunkAddrAligned;

    // Redo if byte is null, might not affect code execution otherwise
  } while (*CurrVal == 0xFF || *CurrVal == 0x00);

  // Erase the whole 4KB chunk containing the offset from SPI flash
  Status = SpiService->SpiErase (FlashRegionBios, ChunkAddrAligned, SIZE_4KB);
  if (EFI_ERROR(Status)) {
    ShellPrint (L"Error, unable to be erase SPI flash %r\n", Status);
    return Status;
  }

  // Make sure the random value to be put at the offset
  // is not the same as the existing value at the offset
  do {
    if (!GetRandomNumber16 (&RandVal)) {
      ShellPrint (L"Error, unable to generate random value\n");
      return EFI_UNSUPPORTED;
    }
    NewVal = (UINT8)RandVal;
  } while (*CurrVal == NewVal);

  // Write the random value at the offset in memory
  *CurrVal = NewVal;

  // Write the new 4KB chunk containing the offset
  Status = SpiService->SpiWrite (FlashRegionBios,
                                 ChunkAddrAligned,
                                 SIZE_4KB,
                                 Buf);
  if (EFI_ERROR(Status)) {
    ShellPrint (L"Error, unable to write SPI flash %r\n", Status);
    return Status;
  }

  if (Buf != NULL) {
    FreePages (Buf, EFI_SIZE_TO_PAGES (SIZE_4KB));
  }

  ShellPrint (L"Successfully wrote 0x%x to 0x%x\n", NewVal, ChunkAddr);

  return EFI_SUCCESS;
}
