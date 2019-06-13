/** @file
  Shell command `hob` to list HOBs.

  Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Library/ShellLib.h>
#include <Library/DebugLib.h>
#include <Library/HobLib.h>
#include <Guid/LoaderFspInfoGuid.h>

/**
  List HOBs.

  @param[in]  Shell        shell instance
  @param[in]  Argc         number of command line arguments
  @param[in]  Argv         command line arguments

  @retval EFI_SUCCESS

**/
STATIC
EFI_STATUS
EFIAPI
ShellCommandHobFunc (
  IN SHELL  *Shell,
  IN UINTN   Argc,
  IN CHAR16 *Argv[]
  );

CONST SHELL_COMMAND ShellCommandHob = {
  L"hob",
  L"List HOBs",
  &ShellCommandHobFunc
};

/**
  Get string description of HOB type.

  @param[in]  Type         Numeric HOB type

  @retval Type string

**/
STATIC
CONST CHAR16 *
EFIAPI
HobTypeToStr (
  IN UINTN Type
  )
{
  switch (Type) {
  case EFI_HOB_TYPE_HANDOFF:
    return L"HANDOFF";
  case EFI_HOB_TYPE_MEMORY_ALLOCATION:
    return L"MEMORY_ALLOCATION";
  case EFI_HOB_TYPE_RESOURCE_DESCRIPTOR:
    return L"RESOURCE_DESCRIPTOR";
  case EFI_HOB_TYPE_GUID_EXTENSION:
    return L"GUID_EXTENSION";
  case EFI_HOB_TYPE_FV:
    return L"FV";
  case EFI_HOB_TYPE_CPU:
    return L"CPU";
  case EFI_HOB_TYPE_MEMORY_POOL:
    return L"MEMORY_POOL";
  case EFI_HOB_TYPE_FV2:
    return L"FV2";
  case EFI_HOB_TYPE_LOAD_PEIM_UNUSED:
    return L"LOAD_PEIM_UNUSED";
  case EFI_HOB_TYPE_UEFI_CAPSULE:
    return L"UEFI_CAPSULE";
  case EFI_HOB_TYPE_UNUSED:
    return L"UNUSED";
  default:
    return L"Unknown";
  }
}

/**
  List HOBs.

  @param[in]  HobList      hob list pointer

  @retval EFI_SUCCESS
**/
STATIC
EFI_STATUS
EFIAPI
ListHobs (
  IN VOID *HobList
  )
{
  EFI_PEI_HOB_POINTERS Hob;
  UINTN                Count;

  Count = 0;

  Hob.Raw = HobList;
  while (!END_OF_HOB_LIST (Hob.Raw)) {
    ShellPrint (L"%3d: 0x%08p 0x%04x %s",
                Count,
                Hob.Raw,
                Hob.Header->HobLength,
                HobTypeToStr (Hob.Header->HobType));

    // Display HOB type-specific information
    switch (Hob.Header->HobType) {

    case EFI_HOB_TYPE_GUID_EXTENSION:
      ShellPrint (L" (%g)\n", & (Hob.Guid->Name));
      break;

    default:
      ShellPrint (L"\n");
      break;
    }

    Hob.Raw = GET_NEXT_HOB (Hob);
    Count++;
  }

  return EFI_SUCCESS;
}

/**
  List HOBs.

  @param[in]  Shell        shell instance
  @param[in]  Argc         number of command line arguments
  @param[in]  Argv         command line arguments

  @retval EFI_SUCCESS

**/
STATIC
EFI_STATUS
EFIAPI
ShellCommandHobFunc (
  IN SHELL  *Shell,
  IN UINTN   Argc,
  IN CHAR16 *Argv[]
  )
{
  VOID            *HobList;
  LOADER_FSP_INFO *FspInfo;

  HobList = GetHobList();
  ASSERT (HobList != NULL);

  // Display Loader HOBs
  ShellPrint (L"Loader HOBs\n");
  ShellPrint (L"========================================\n");
  ListHobs (HobList);

  // Display FSP HOBs
  FspInfo = GetNextGuidHob (&gLoaderFspInfoGuid, HobList);
  ASSERT (FspInfo != NULL);
  FspInfo = GET_GUID_HOB_DATA (FspInfo);

  ShellPrint (L"\nFSP HOBs\n");
  ShellPrint (L"========================================\n");
  ListHobs (FspInfo->FspHobList);

  return EFI_SUCCESS;
}
