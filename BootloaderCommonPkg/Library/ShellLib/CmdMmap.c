/** @file
  Shell command `mmap` to display the memory map.

  Copyright (c) 2017 - 2023, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Library/ShellLib.h>
#include <Library/HobLib.h>
#include <Guid/MemoryMapInfoGuid.h>
#include <Library/DebugLib.h>

/**
  Display memory map.

  @param[in]  Shell        shell instance
  @param[in]  Argc         number of command line arguments
  @param[in]  Argv         command line arguments

  @retval EFI_SUCCESS

**/
STATIC
EFI_STATUS
EFIAPI
ShellCommandMmapFunc (
  IN SHELL  *Shell,
  IN UINTN   Argc,
  IN CHAR16 *Argv[]
  );

CONST SHELL_COMMAND ShellCommandMmap = {
  L"mmap",
  L"Display memory map",
  &ShellCommandMmapFunc
};

/**
  Get string description of memory type.

  @param[in]  Type         Numeric memory type

  @retval Type string

**/
STATIC
CONST CHAR16 *
EFIAPI
MemTypeToStr (
  IN UINTN Type
  )
{
  switch (Type) {
  case 0x01:
    return L"System Memory";
  case 0x02:
    return L"Reserved";
  case 0x03:
    return L"ACPI Reclaim";
  case 0x04:
    return L"ACPI NVS";
  default:
    return L"Unknown";
  }
}

/**
  Get string description of MTRR type.

  @param[in]  Type         Numeric MTRR type

  @retval Memory Map Info pointer

**/
STATIC
MEMORY_MAP_INFO *
EFIAPI
GetMemoryMapInfo (
  VOID
  )
{
  EFI_HOB_GUID_TYPE *GuidHob;

  GuidHob = GetNextGuidHob (&gLoaderMemoryMapInfoGuid, GetHobList());
  if (GuidHob == NULL) {
    ASSERT (GuidHob);
    return NULL;
  }

  return (MEMORY_MAP_INFO *)GET_GUID_HOB_DATA (GuidHob);
}

/**
  Display memory map.

  @param[in]  Shell        shell instance
  @param[in]  Argc         number of command line arguments
  @param[in]  Argv         command line arguments

  @retval EFI_SUCCESS

**/
STATIC
EFI_STATUS
EFIAPI
ShellCommandMmapFunc (
  IN SHELL  *Shell,
  IN UINTN   Argc,
  IN CHAR16 *Argv[]
  )
{
  UINT32          Index;
  MEMORY_MAP_INFO *MemoryMapInfo;

  MemoryMapInfo = GetMemoryMapInfo();
  ASSERT (MemoryMapInfo != NULL);

  for (Index = 0; Index < MemoryMapInfo->Count; Index++) {
    ShellPrint (L"[0x%09llx-0x%09llx] %s\n",
                MemoryMapInfo->Entry[Index].Base,
                MemoryMapInfo->Entry[Index].Base + MemoryMapInfo->Entry[Index].Size - 1,
                MemTypeToStr (MemoryMapInfo->Entry[Index].Type));
  }

  return EFI_SUCCESS;
}
