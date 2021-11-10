/** @file
  Shell command `dmesg` to print the contents of the log buffer.

  Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Library/ShellLib.h>
#include <Library/DebugLib.h>
#include <Library/HobLib.h>
#include <Guid/LoaderFspInfoGuid.h>
#include <Library/ConsoleInLib.h>
#include <Library/ConsoleOutLib.h>
#include <Library/BootloaderCommonLib.h>
#include <Library/DebugLogBufferLib.h>

/**
  Print the contents of the log buffer

  @param[in]  Shell        shell instance
  @param[in]  Argc         number of command line arguments
  @param[in]  Argv         command line arguments

  @retval EFI_SUCCESS

**/
STATIC
EFI_STATUS
EFIAPI
ShellCommandDmesgFunc (
  IN SHELL  *Shell,
  IN UINTN   Argc,
  IN CHAR16 *Argv[]
  );

CONST SHELL_COMMAND ShellCommandDmesg = {
  L"dmesg",
  L"Print messages stored in boot log buffer",
  &ShellCommandDmesgFunc
};

CONST UINTN LinesPerPage = 30;

/**
  Print the contents of the log buffer

  @param[in]  Shell        shell instance
  @param[in]  Argc         number of command line arguments
  @param[in]  Argv         command line arguments

  @retval EFI_SUCCESS

**/
STATIC
EFI_STATUS
EFIAPI
ShellCommandDmesgFunc (
  IN SHELL  *Shell,
  IN UINTN   Argc,
  IN CHAR16 *Argv[]
  )
{
  DEBUG_LOG_BUFFER_HEADER *LogBufHdr;
  UINTN                    PageLineCount;
  UINTN                    Index;
  UINT8                    Buf[1];
  BOOLEAN                  Paged = FALSE;
  UINTN                    Length;
  UINTN                    BufIndex;

  for (Index = 1; Index < Argc; Index++) {
    if (StrCmp (Argv[Index], L"-h") == 0) {
      goto usage;
    }
    if (StrCmp (Argv[Index], L"-p") == 0) {
      Paged = TRUE;
    }
  }

  PageLineCount = 0;
  LogBufHdr = (DEBUG_LOG_BUFFER_HEADER *) GetDebugLogBufferPtr ();
  if (LogBufHdr == NULL) {
    return EFI_UNSUPPORTED;
  }

  if (LogBufHdr->UsedLength > LogBufHdr->TotalLength) {
    return EFI_LOAD_ERROR;
  }

  if ((LogBufHdr->Attribute & DEBUG_LOG_BUFFER_ATTRIBUTE_FULL) != 0) {
    BufIndex = LogBufHdr->UsedLength - LogBufHdr->HeaderLength;
    Length   = LogBufHdr->TotalLength - LogBufHdr->HeaderLength;
  } else {
    BufIndex = 0;
    Length   = LogBufHdr->UsedLength - LogBufHdr->HeaderLength;
  }

  for (Index = 0; Index < Length; Index++, BufIndex++) {
    ConsoleWrite ((UINT8 *)&LogBufHdr->Buffer[BufIndex % Length], 1);

    // Page out the log contents if requested
    if (Paged && (LogBufHdr->Buffer[BufIndex % Length] == '\n') && (++PageLineCount == LinesPerPage)) {
      ShellPrint (L"[Press <ESC> to stop, or any other key to continue...]");
      ConsoleRead (Buf, 1);
      if (Buf[0] == '\x1b') { break; }
      PageLineCount = 0;
      ShellPrint (L"\r%54a\r", "");
    }
  }

  return EFI_SUCCESS;

usage:
  ShellPrint (L"Usage: %s [-p]\n", Argv[0]);
  ShellPrint (L"\n"
              L"Flags:\n"
              L"  -p     Paged output (display %d lines at a time)\n", LinesPerPage);
  return EFI_ABORTED;
}
