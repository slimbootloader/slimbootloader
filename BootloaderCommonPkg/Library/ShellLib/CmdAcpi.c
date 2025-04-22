/** @file
  Shell command `acpi` to display ACPI data.

  Copyright (c) 2023, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Library/ShellLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Guid/CsmePerformanceInfoGuid.h>
#include <Guid/PerformanceInfoGuid.h>
#include <Library/HobLib.h>
#include <Library/BootloaderCommonLib.h>
#include <Library/LoaderPerformanceLib.h>
#include <IndustryStandard/Acpi.h>
#include <UniversalPayload/AcpiTable.h>


/**
  Display acpi table.

  @param[in]  Shell        shell instance
  @param[in]  Argc         number of command line arguments
  @param[in]  Argv         command line arguments

  @retval EFI_SUCCESS

**/
STATIC
EFI_STATUS
EFIAPI
ShellCommandAcpiFunc (
  IN SHELL  *Shell,
  IN UINTN   Argc,
  IN CHAR16 *Argv[]
  );

CONST SHELL_COMMAND ShellCommandAcpi = {
  L"acpi",
  L"Display ACPI table data",
  &ShellCommandAcpiFunc
};


/**
  Print ACPI table header

  @param  AcpiHdr           ACPI table start address in memory
  @param  PrintOem          Indicate if print revision and OEM info
**/
VOID
PrintAcpiHeader (
  EFI_ACPI_DESCRIPTION_HEADER   *AcpiHdr,
  BOOLEAN                       PrintOem
  )
{
  UINT8          Signature[5];
  UINT8          OemId[7];
  UINT8          OemTableId[9];
  UINT8          CreatorId[5];

  Signature[4]  = 0;
  OemId[6]      = 0;
  OemTableId[8] = 0;
  CreatorId[4]  = 0;

  CopyMem (Signature, &AcpiHdr->Signature,  4);
  if (PrintOem) {
    CopyMem (OemId,      AcpiHdr->OemId,       6);
    CopyMem (OemTableId, &AcpiHdr->OemTableId, 8);
    CopyMem (CreatorId,  &AcpiHdr->CreatorId,  4);
    ShellPrint (L" %8a  0x%08p  0x%06x  v%02d  %6a  %8a  0x%08x  %4a\n", Signature, AcpiHdr, AcpiHdr->Length, AcpiHdr->Revision, OemId, OemTableId, AcpiHdr->OemRevision, CreatorId);
  } else {
    ShellPrint (L" %8a  0x%08p  0x%06x\n", Signature, AcpiHdr, AcpiHdr->Length);
  }
}


/**
  Print ACPI table

  @param  AcpiTableBase          ACPI table start address in memory

**/
VOID
PrintAcpiInfo (
  IN   UINT64           AcpiTableBase
  )
{
  EFI_ACPI_5_0_ROOT_SYSTEM_DESCRIPTION_POINTER                   *Rsdp;
  EFI_ACPI_DESCRIPTION_HEADER                                    *Rsdt;
  UINT32                                                         *Entry32;
  UINTN                                                          Entry32Num;
  UINTN                                                          Idx;
  UINT32                                                         *Signature;
  EFI_ACPI_5_0_FIXED_ACPI_DESCRIPTION_TABLE                      *Facp;
  UINT8                                                          Sig[9];
  UINT8                                                          OemId[7];

  OemId[6] = 0;
  Sig[8]   = 0;
  Rsdp = (EFI_ACPI_5_0_ROOT_SYSTEM_DESCRIPTION_POINTER *)(UINTN) AcpiTableBase;
  CopyMem (OemId, Rsdp->OemId, 6);
  CopyMem (Sig, &Rsdp->Signature, 8);
  ShellPrint (L" Signature   address    length   Rev  OemId   TableId   OemRev     Creator\n");
  ShellPrint (L" %8a  0x%08p  0x%06x  v%02d  %4a\n", Sig, Rsdp, Rsdp->Length, Rsdp->Revision, OemId);

  Rsdt = (EFI_ACPI_DESCRIPTION_HEADER *)(UINTN)(Rsdp->RsdtAddress);
  if (Rsdt != NULL) {
    Entry32    = (UINT32 *)(Rsdt + 1);
    Entry32Num = (Rsdt->Length - sizeof (EFI_ACPI_DESCRIPTION_HEADER)) >> 2;
    for (Idx = 0; Idx < Entry32Num; Idx++) {
      Signature = (UINT32 *)(UINTN)Entry32[Idx];
      switch (*Signature) {
      case EFI_ACPI_5_0_FIXED_ACPI_DESCRIPTION_TABLE_SIGNATURE:
        Facp = (EFI_ACPI_5_0_FIXED_ACPI_DESCRIPTION_TABLE *)Signature;
        PrintAcpiHeader ((EFI_ACPI_DESCRIPTION_HEADER *)(UINTN)Facp->Dsdt, TRUE);
        PrintAcpiHeader ((EFI_ACPI_DESCRIPTION_HEADER *)(UINTN)Facp->FirmwareCtrl, FALSE);
        PrintAcpiHeader ((EFI_ACPI_DESCRIPTION_HEADER *)Signature, TRUE);
        break;
      default:
        PrintAcpiHeader ((EFI_ACPI_DESCRIPTION_HEADER *)Signature, TRUE);
        break;
      }
    }
  }
}

/**
  Display performance data.

  @param[in]  Shell        shell instance
  @param[in]  Argc         number of command line arguments
  @param[in]  Argv         command line arguments

  @retval EFI_SUCCESS

**/
STATIC
EFI_STATUS
EFIAPI
ShellCommandAcpiFunc (
  IN SHELL  *Shell,
  IN UINTN   Argc,
  IN CHAR16 *Argv[]
  )
{
  VOID                          *GuidHob;
  UNIVERSAL_PAYLOAD_ACPI_TABLE  *AcpiHob;

  GuidHob = GetNextGuidHob (&gUniversalPayloadAcpiTableGuid, GetHobList());
  if (GuidHob == NULL) {
    ASSERT (FALSE);
    return RETURN_NOT_FOUND;
  }

  AcpiHob = (UNIVERSAL_PAYLOAD_ACPI_TABLE *)GET_GUID_HOB_DATA (GuidHob);
  PrintAcpiInfo (AcpiHob->Rsdp);

  return EFI_SUCCESS;
}

