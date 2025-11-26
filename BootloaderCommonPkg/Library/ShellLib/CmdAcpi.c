/** @file
  Shell command `acpi` to display ACPI data.

  Copyright (c) 2023, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Library/ShellLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseLib.h>
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
  Print ACPI table content in hex dump format

  @param  Table          Pointer to ACPI table
**/
VOID
PrintAcpiTableContent (
  IN EFI_ACPI_DESCRIPTION_HEADER *Table
  )
{
  UINT8   *Data;
  UINT32  Length;
  UINT32  Offset;
  UINT32  i;

  if (Table == NULL) {
    return;
  }

  Data = (UINT8 *)Table;
  Length = Table->Length;

  ShellPrint (L"\nTable Content (Length: 0x%x bytes):\n", Length);
  ShellPrint (L"Offset   00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F  ASCII\n");
  ShellPrint (L"------   -----------------------------------------------  ----------------\n");

  for (Offset = 0; Offset < Length; Offset += 16) {
    ShellPrint (L"%06x   ", Offset);

    // Print hex bytes
    for (i = 0; i < 16; i++) {
      if (Offset + i < Length) {
        ShellPrint (L"%02x ", Data[Offset + i]);
      } else {
        ShellPrint (L"   ");
      }
    }

    ShellPrint (L" ");

    // Print ASCII
    for (i = 0; i < 16 && Offset + i < Length; i++) {
      UINT8 c = Data[Offset + i];
      if (c >= 32 && c <= 126) {
        ShellPrint (L"%c", (CHAR16)c);
      } else {
        ShellPrint (L".");
      }
    }

    ShellPrint (L"\n");
  }
}

/**
  Print ACPI table information

  @param  AcpiTableBase          ACPI table start address in memory
  @param  TableSignature         Signature of specific table to dump (0 = show all)

**/
VOID
PrintAcpiInfo (
  IN   UINT64           AcpiTableBase,
  IN   UINT32           TableSignature
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

  // If showing all tables, print header
  if (TableSignature == 0) {
    ShellPrint (L" Signature   address    length   Rev  OemId   TableId   OemRev     Creator\n");
    ShellPrint (L" %8a  0x%08p  0x%06x  v%02d  %4a\n", Sig, Rsdp, Rsdp->Length, Rsdp->Revision, OemId);
  }

  Rsdt = (EFI_ACPI_DESCRIPTION_HEADER *)(UINTN)(Rsdp->RsdtAddress);
  if (Rsdt != NULL) {
    Entry32    = (UINT32 *)(Rsdt + 1);
    Entry32Num = (Rsdt->Length - sizeof (EFI_ACPI_DESCRIPTION_HEADER)) >> 2;
    for (Idx = 0; Idx < Entry32Num; Idx++) {
      Signature = (UINT32 *)(UINTN)Entry32[Idx];

      switch (*Signature) {
      case EFI_ACPI_5_0_FIXED_ACPI_DESCRIPTION_TABLE_SIGNATURE:
        Facp = (EFI_ACPI_5_0_FIXED_ACPI_DESCRIPTION_TABLE *)Signature;
        if (TableSignature == 0) {
          // Show all tables
          PrintAcpiHeader ((EFI_ACPI_DESCRIPTION_HEADER *)(UINTN)Facp->Dsdt, TRUE);
          PrintAcpiHeader ((EFI_ACPI_DESCRIPTION_HEADER *)(UINTN)Facp->FirmwareCtrl, FALSE);
          PrintAcpiHeader ((EFI_ACPI_DESCRIPTION_HEADER *)Signature, TRUE);
        } else if (TableSignature == EFI_ACPI_5_0_DIFFERENTIATED_SYSTEM_DESCRIPTION_TABLE_SIGNATURE) {
          // Dump DSDT table
          PrintAcpiHeader ((EFI_ACPI_DESCRIPTION_HEADER *)(UINTN)Facp->Dsdt, TRUE);
          PrintAcpiTableContent ((EFI_ACPI_DESCRIPTION_HEADER *)(UINTN)Facp->Dsdt);
          return;
        } else if (TableSignature == EFI_ACPI_5_0_FIRMWARE_ACPI_CONTROL_STRUCTURE_SIGNATURE) {
          // Dump FACS table
          PrintAcpiHeader ((EFI_ACPI_DESCRIPTION_HEADER *)(UINTN)Facp->FirmwareCtrl, FALSE);
          PrintAcpiTableContent ((EFI_ACPI_DESCRIPTION_HEADER *)(UINTN)Facp->FirmwareCtrl);
          return;
        } else if (TableSignature == *Signature) {
          // Dump FACP table itself
          PrintAcpiHeader ((EFI_ACPI_DESCRIPTION_HEADER *)Signature, TRUE);
          PrintAcpiTableContent ((EFI_ACPI_DESCRIPTION_HEADER *)Signature);
          return;
        }
        break;
      default:
        if (TableSignature == 0) {
          // Show all tables
          PrintAcpiHeader ((EFI_ACPI_DESCRIPTION_HEADER *)Signature, TRUE);
        } else if (TableSignature == *Signature) {
          // Dump specific table
          PrintAcpiHeader ((EFI_ACPI_DESCRIPTION_HEADER *)Signature, TRUE);
          PrintAcpiTableContent ((EFI_ACPI_DESCRIPTION_HEADER *)Signature);
          return;
        }
        break;
      }
    }
  }

  // If we reach here and TableSignature != 0, table was not found
  if (TableSignature != 0) {
    CHAR8 SigStr[5];
    *(UINT32 *)SigStr = TableSignature;
    SigStr[4] = 0;
    ShellPrint (L"Error: ACPI table '%a' not found\n", SigStr);
  }
}

/**
  Display ACPI table data.

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
  UINT32                        TableSignature;
  BOOLEAN                       ShowHelp;
  BOOLEAN                       ShowTableContent;
  UINTN                         Index;
  CHAR8                         SigStr[5];

  ShowHelp = FALSE;
  ShowTableContent = FALSE;
  TableSignature = 0;

  // Parse command line arguments
  if (Argc > 1) {
    for (Index = 1; Index < Argc; Index++) {
      if ((StrCmp (Argv[Index], L"-h") == 0) || (StrCmp (Argv[Index], L"--help") == 0)) {
        ShowHelp = TRUE;
        break;
      } else {
        // Convert table name to signature
        if (StrLen (Argv[Index]) == 4) {
          // Convert Unicode to ASCII and create signature
          SigStr[0] = (CHAR8)Argv[Index][0];
          SigStr[1] = (CHAR8)Argv[Index][1];
          SigStr[2] = (CHAR8)Argv[Index][2];
          SigStr[3] = (CHAR8)Argv[Index][3];
          SigStr[4] = 0;
          // Convert to uppercase
          if (SigStr[0] >= 'a' && SigStr[0] <= 'z') SigStr[0] -= 32;
          if (SigStr[1] >= 'a' && SigStr[1] <= 'z') SigStr[1] -= 32;
          if (SigStr[2] >= 'a' && SigStr[2] <= 'z') SigStr[2] -= 32;
          if (SigStr[3] >= 'a' && SigStr[3] <= 'z') SigStr[3] -= 32;

          TableSignature = *(UINT32 *)SigStr;
          ShowTableContent = TRUE;
        }
      }
    }
  }

  if (ShowHelp) {
    ShellPrint (L"\nDisplay ACPI table information\n\n");
    ShellPrint (L"Usage: acpi [OPTIONS] [TABLE_NAME]\n\n");
    ShellPrint (L"OPTIONS:\n");
    ShellPrint (L"  -h, --help     Display this help message\n\n");
    ShellPrint (L"TABLE_NAME:\n");
    ShellPrint (L"  Four-character table signature (e.g., DSDT, SSDT, FACP)\n\n");
    ShellPrint (L"Examples:\n");
    ShellPrint (L"  acpi           - Display all ACPI tables summary\n");
    ShellPrint (L"  acpi dsdt      - Display DSDT table content\n");
    ShellPrint (L"  acpi facp      - Display FACP table content\n\n");
    return EFI_SUCCESS;
  }

  GuidHob = GetNextGuidHob (&gUniversalPayloadAcpiTableGuid, GetHobList());
  if (GuidHob == NULL) {
    ShellPrint (L"Error: ACPI table information not found\n");
    return RETURN_NOT_FOUND;
  }

  AcpiHob = (UNIVERSAL_PAYLOAD_ACPI_TABLE *)GET_GUID_HOB_DATA (GuidHob);

  if (ShowTableContent) {
    // Display specific table content
    ShellPrint (L"\nACPI Table: %a\n", SigStr);
    PrintAcpiInfo (AcpiHob->Rsdp, TableSignature);
    // Note: PrintAcpiInfo will print error message if table not found
  } else {
    // Display all tables summary (default behavior)
    PrintAcpiInfo (AcpiHob->Rsdp, 0);
  }

  return EFI_SUCCESS;
}

