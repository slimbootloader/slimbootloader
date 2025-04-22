/** @file

  Copyright (c) 2020 Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include <PiPei.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/HobLib.h>
#include <Library/BootloaderCommonLib.h>
#include <Library/BootloaderCoreLib.h>
#include <Library/TccLib.h>
#include <Library/SpiFlashLib.h>
#include <Library/ContainerLib.h>
#include "TccRtctHob.h"


// ACPI Definations for RTCT Table
#define EFI_ACPI_RTCT_TABLE_REVISION      0x1
#define EFI_ACPI_RTCT_OEM_ID              "INTEL "  // OEMID 6 bytes long
#define EFI_ACPI_RTCT_OEM_TABLE_ID        SIGNATURE_64('S','B','L',' ',' ',' ',' ',' ')
#define EFI_ACPI_RTCT_OEM_REVISION        0x00000005
#define EFI_ACPI_RTCT_CREATOR_ID          SIGNATURE_32('I','N','T','L')
#define EFI_ACPI_RTCT_CREATOR_REVISION    0x0100000D


/**
  Update RTCT  (Platform Tuning Configuration Table) ACPI Table

  @param[in] RtctTable          PTCT ACPI Table entry

  @retval EFI_SUCCESS           The RTCT table is updated.
  @retval EFI_NOT_FOUND         Loader Global Pointer or TccHob not found
  @retval EFI_INVALID_PARAMETER RTCT table is NULL
**/
EFI_STATUS
EFIAPI
UpdateAcpiRtctTable (
  IN EFI_ACPI_DESCRIPTION_HEADER *RtctTable
  )
{
  VOID                          *FspHobList;
  EFI_HOB_GUID_TYPE             *GuidHob;
  EFI_ACPI_DESCRIPTION_HEADER   *HobRtctTable;

  if (RtctTable == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  GuidHob    = NULL;
  FspHobList = GetFspHobListPtr();
  if (FspHobList != NULL) {
    GuidHob = GetNextGuidHob (&gTccRtctHobGuid, FspHobList);
  }

  if (GuidHob == NULL) {
    DEBUG ((DEBUG_INFO, "RTCT FSP HOB not found.\n"));
    return EFI_NOT_FOUND;
  }

  //
  // Copy RTCT table from HOB, the table Length is already updated by HOB.
  //
  HobRtctTable = (EFI_ACPI_DESCRIPTION_HEADER *)GET_GUID_HOB_DATA (GuidHob);
  CopyMem (RtctTable, HobRtctTable, HobRtctTable->Length);

  DEBUG ((DEBUG_INFO, "HobRtctTable = 0x%p, HobRtctTable->Length = 0x%x\n", HobRtctTable, HobRtctTable->Length));

  //
  // Update other fields in ACPI table header
  //
  RtctTable->Signature       = EFI_ACPI_RTCT_SIGNATURE;
  RtctTable->Revision        = EFI_ACPI_RTCT_TABLE_REVISION;
  RtctTable->Checksum        = 0;
  CopyMem (&RtctTable->OemId, EFI_ACPI_RTCT_OEM_ID, 6);
  RtctTable->OemTableId      = EFI_ACPI_RTCT_OEM_TABLE_ID;
  RtctTable->OemRevision     = EFI_ACPI_RTCT_OEM_REVISION;
  RtctTable->CreatorId       = EFI_ACPI_RTCT_CREATOR_ID;
  RtctTable->CreatorRevision = EFI_ACPI_RTCT_CREATOR_REVISION;

  return EFI_SUCCESS;
}

/**
  Check if the TCC DSO is a bad binary by checking its signature.

  @retval TRUE     TCC DSO was marked as a bad binary
  @retval FALSE    TCC DSO was not found or not marked as a bad binary

 */
BOOLEAN
EFIAPI
IsMarkedBadDso (
  VOID
  )
{
  EFI_STATUS               Status;
  UINT32                   Length;
  LOADER_COMPRESSED_HEADER *Hdr;

  Status = LocateComponent (SIGNATURE_32 ('I', 'P', 'F', 'W'), SIGNATURE_32 ('T', 'C', 'C', 'T'), (VOID *)&Hdr, &Length);
  if (EFI_ERROR (Status) || (Length < sizeof(LOADER_COMPRESSED_HEADER))) {
    return FALSE;
  }

  if (Hdr->Signature == 0) {
    DEBUG ((DEBUG_INFO, "BAD DSO(TCCT) detected!\n"));
    return TRUE;
  } else {
    return FALSE;
  }
}

/**
  Mark TCC DSO as a bad binary by changing its signature to 0.

  @retval EFI_SUCCESS
  @retval EFI_NOT_FOUND          Unable to find IPFW/TCCT
  @retval Others                 Errors during SPI operations

 */
EFI_STATUS
EFIAPI
InvalidateBadDso (
  VOID
  )
{
  EFI_STATUS       Status;
  UINT32           Address;
  UINT32           BaseAddress;
  UINT32           RegionSize;
  CONTAINER_ENTRY  *ContainerEntry;
  COMPONENT_ENTRY  *CompEntry;
  CONTAINER_HDR    *ContainerHdr;
  UINT32           Signature;

  Status = LocateComponentEntry (SIGNATURE_32 ('I', 'P', 'F', 'W'), SIGNATURE_32 ('T', 'C', 'C', 'T'), &ContainerEntry, &CompEntry);
  if (EFI_ERROR (Status) || (ContainerEntry == NULL) || (CompEntry == NULL)) {
    return EFI_NOT_FOUND;
  }

  Status = SpiGetRegionAddress (FlashRegionBios, NULL, &RegionSize);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  BaseAddress = ((UINT32)(~RegionSize) + 1);

  ContainerHdr = (CONTAINER_HDR *)(UINTN)ContainerEntry->HeaderCache;
  Address = ContainerEntry->Base + ContainerHdr->DataOffset + CompEntry->Offset;
  Address -= BaseAddress;

  // Update 'Signature' field to 0 to mark bad DSO
  Signature = 0;
  Status = SpiFlashWrite (FlashRegionBios, Address, sizeof(Signature), (VOID *)&Signature);
  if (!EFI_ERROR(Status)) {
    DEBUG ((DEBUG_INFO, "Mark BAD DSO(TCCT) successfully\n"));
  }
  return Status;
}
