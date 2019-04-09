/** @file

  Copyright (c) 2017 - 2019, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <IndustryStandard/SmBios.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseLib.h>
#include <Library/BootloaderCoreLib.h>
#include <Library/SmbiosInitLib.h>

/**
  This function is called to initialize the SMBIOS tables.

**/
VOID
EFIAPI
SmbiosInit (
  IN  UINT32    *SmbiosEntry
  )
{
  SMBIOS_TABLE_ENTRY_POINT      *SmbiosEntryPoint;

  SmbiosEntryPoint = (SMBIOS_TABLE_ENTRY_POINT *) AllocateZeroPool (SMBIOS_TABLE_MAX_LENGTH_SBL);

  SetSmbiosPtr ((VOID *) SmbiosEntryPoint);

  SmbiosEntryPoint->AnchorString[0]               =  '_';
  SmbiosEntryPoint->AnchorString[1]               =  'S';
  SmbiosEntryPoint->AnchorString[2]               =  'M';
  SmbiosEntryPoint->AnchorString[3]               =  '_';
  SmbiosEntryPoint->EntryPointLength              = sizeof (SMBIOS_TABLE_ENTRY_POINT);
  SmbiosEntryPoint->MajorVersion                  = 0;
  SmbiosEntryPoint->MinorVersion                  = 1;
  SmbiosEntryPoint->TableLength                   = SMBIOS_TABLE_MAX_LENGTH_SBL;
  SmbiosEntryPoint->TableAddress                  = (UINT32)SmbiosEntryPoint + sizeof (SMBIOS_TABLE_ENTRY_POINT) + sizeof (UINT8);
  SmbiosEntryPoint->EntryPointStructureChecksum   = CalculateCheckSum8 ((UINT8 *)SmbiosEntryPoint, sizeof (SMBIOS_TABLE_ENTRY_POINT));

  *SmbiosEntry = (UINT32) (UINTN) SmbiosEntryPoint;
}
