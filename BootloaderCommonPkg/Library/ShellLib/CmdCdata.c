/** @file
  Shell command `Cdata` to display configuration data.

  Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Library/ShellLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/FileSystemLib.h>
#include <Library/HobLib.h>
#include <Library/BootloaderCommonLib.h>
#include <Library/PcdLib.h>
#include <Library/PrintLib.h>
#include <Library/ConfigDataLib.h>

/**
  display configuration data.

  @param[in]  Shell        shell instance
  @param[in]  Argc         number of command line arguments
  @param[in]  Argv         command line arguments

  @retval EFI_SUCCESS

**/
STATIC
EFI_STATUS
EFIAPI
ShellCommandCdataFunc (
  IN SHELL  *Shell,
  IN UINTN   Argc,
  IN CHAR16 *Argv[]
  );

CONST SHELL_COMMAND ShellCommandCdata = {
  L"cdata",
  L"Display configuration data.",
  &ShellCommandCdataFunc
};


/**
  Print the OS boot option list.

  @param[in]  OsBootOptionList    the OS boot option list

**/
VOID
PrintBlobheader (
  CDATA_BLOB          *CdataBlob
  )
{
  ShellPrint (L"Found config database:\n");
  ShellPrint (L"    Signature   :%x\n", CdataBlob->Signature);
  ShellPrint (L"    HeaderLength:%x\n", CdataBlob->HeaderLength);
  ShellPrint (L"    Attribute   :%x\n", CdataBlob->Attribute);
  ShellPrint (L"    IntCfgOffset:%x\n", CdataBlob->ExtraInfo.InternalDataOffset * 4);
  ShellPrint (L"    UsedLength  :%x\n", CdataBlob->UsedLength);
  ShellPrint (L"    TotalLength :%x\n", CdataBlob->TotalLength);
}

/**
  Print configuration data only for specified tag.

  @param[in]  Address      The buffer address to dump
  @param[in]  Length       The total length to dump

**/
VOID
ShellDumpBuffer (
  IN CHAR8                   *Address,
  IN UINT32                  Length
  )
{
  UINT32                     Index;

  if (Length > 0x20) {
    ShellPrint (L"Dump buffer from:0x%p, length %x\n", Address, Length);
  }
  for (Index = 0; Index < Length; Index++) {
    if ( (Index % 16) == 0) {
      ShellPrint (L"\n0x%08x:", Index);
    }
    ShellPrint (L" %02x ", Address[Index] & 0xFF);
  }
  ShellPrint (L"\n\n");
}

/**
  Print all the configuration data.

  @param[in]  CdataBlob    The pointer to config database

**/
VOID
PrintCfgItem (
  CDATA_BLOB          *CdataBlob
  )
{
  CDATA_HEADER        *CdataHeader;
  UINT32              Index;
  UINT32              Offset;

  Index  = 0;
  Offset = CdataBlob->HeaderLength;
  ShellPrint (L"Idx | Tag | Version | Flags | Length | PID mask \n");
  while (Offset < CdataBlob->UsedLength) {
    CdataHeader = (CDATA_HEADER *) ((UINT8 *)CdataBlob + Offset);
    ShellPrint (L"%3x |%4x | %6x  | %4x  | %4xh  | %5x \n", \
                 Index, \
                 CdataHeader->Tag, \
                 CdataHeader->Version, \
                 CdataHeader->Flags, \
                 CdataHeader->Length * sizeof (UINT32), \
                 CdataHeader->Condition[0].Value \
                 );
    Offset += CdataHeader->Length * sizeof (UINT32);
    Index++;
  }

  ShellPrint (L"\n");
}

/**
  Print configuration data only for specified tag.

  @param[in]  CdataBlob    The pointer to config database
  @param[in]  Tag          The configuration tag

**/
VOID
PrintTag (
  CDATA_BLOB          *CdataBlob,
  UINT32              Tag
  )
{
  CDATA_HEADER        *CdataHeader;
  UINT32              Index;
  UINT32              Offset;

  Index  = 0;
  Offset = CdataBlob->HeaderLength;
  ShellPrint (L"Idx | Tag | Version | Flags | Length | PID mask \n");
  while (Offset < CdataBlob->UsedLength) {
    CdataHeader = (CDATA_HEADER *) ((UINT8 *)CdataBlob + Offset);
    if (CdataHeader->Tag == Tag) {
      ShellPrint (L"%3x |%4x | %6x  | %4x  | %4xh  | %5x \n", \
                 Index, \
                 CdataHeader->Tag, \
                 CdataHeader->Version, \
                 CdataHeader->Flags, \
                 CdataHeader->Length * sizeof (UINT32), \
                 CdataHeader->Condition[0].Value \
                 );
      Index++;
      ShellDumpBuffer ((CHAR8 *) CdataHeader, CdataHeader->Length * sizeof (UINT32));
    }
    Offset += CdataHeader->Length * sizeof (UINT32);
  }
  ShellPrint (L"\n");
}

/**
  Print configuration data.

  @param[in]  Shell        shell instance
  @param[in]  Argc         number of command line arguments
  @param[in]  Argv         command line arguments

  @retval EFI_SUCCESS

**/
EFI_STATUS
EFIAPI
ShellCommandCdataFunc (
  IN SHELL            *Shell,
  IN UINTN            Argc,
  IN CHAR16           *Argv[]
  )
{
  CDATA_BLOB          *CdataBlob;
  UINT32              Index;
  BOOLEAN             BlobPrint;
  BOOLEAN             CfgHeaderPrint;
  BOOLEAN             DataBaseDump;
  BOOLEAN             UsagePrint;
  UINT32              Tag;


  CdataBlob = (CDATA_BLOB *) GetConfigDataPtr ();
  if ((CdataBlob == NULL) || (CdataBlob->Signature != 0x44474643)) {
    ShellPrint (L"Not find Cdata blob!\n");
    return EFI_NOT_FOUND;
  }

  UsagePrint     = TRUE;
  BlobPrint      = FALSE;
  CfgHeaderPrint = FALSE;
  DataBaseDump   = FALSE;
  Tag            = 0;

  // Check flags
  for (Index = 0; Index < Argc; Index++) {
    if (StrCmp (Argv[Index], L"-h") == 0) {
      UsagePrint = TRUE;
    }
    if (StrCmp (Argv[Index], L"-b") == 0) {
      BlobPrint  = TRUE;
      UsagePrint = FALSE;
    }
    if (StrCmp (Argv[Index], L"-c") == 0) {
      CfgHeaderPrint = TRUE;
      UsagePrint = FALSE;
    }
    if (StrCmp (Argv[Index], L"-d") == 0) {
      DataBaseDump = TRUE;
      UsagePrint = FALSE;
    }
    if (StrCmp (Argv[Index], L"-t") == 0) {
      Index++;
      Tag = (UINT32)StrHexToUintn (Argv[Index]);
      UsagePrint = FALSE;
    }
  }

  if (BlobPrint) {
    PrintBlobheader (CdataBlob);
  }

  if (CfgHeaderPrint) {
    PrintCfgItem (CdataBlob);
  }

  if (DataBaseDump) {
    ShellDumpBuffer ((CHAR8 *)CdataBlob, CdataBlob->UsedLength);
  }

  if (Tag != 0) {
    PrintTag (CdataBlob, Tag);
  }

  if (UsagePrint) {
    ShellPrint (L"Usage: %s [-h] [-b] [-c] [-d] [-t tag]\n", Argv[0]);
    ShellPrint (L"\n"
                L"Flags:\n"
                L"  -h     help\n"
                L"  -b     Print config database header\n"
                L"  -c     Print all the configuratin header\n"
                L"  -d     Dump config database\n"
                L"  -t tag Print configuration data with specified tag\n");
  }
  return EFI_SUCCESS;
}
