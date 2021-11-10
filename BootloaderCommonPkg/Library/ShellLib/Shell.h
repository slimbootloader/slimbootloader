/** @file
  A minimal command-line shell.

  Copyright (c) 2017 - 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _SHELL_H_
#define _SHELL_H_

#define SHELL_COMMAND_LIST_ENTRY_SIGNATURE  SIGNATURE_32('S','C','L','E')

typedef struct {
  UINT32                    Signature;
  CONST SHELL_COMMAND      *ShellCommand;
  LIST_ENTRY                Link;
} SHELL_COMMAND_LIST_ENTRY;

/**
  Read a line of input from the serial port.

  @param[in]  Shell        shell instance
  @param[out] Buffer       buffer to receive command line
  @param[in]  BufferSize   size (in bytes) of the buffer

  @retval EFI_SUCCESS
  @retval EFI_BUFFER_TOO_SMALL
  @retval EFI_TIMEOUT

**/
EFI_STATUS
ShellReadLine (
  IN        SHELL  *Shell,
  OUT       CHAR16 *Buffer,
  IN  CONST UINTN   BufferSize
  );

/**
  Read a UINT value from the serial port.

  @param[in]  Shell        shell instance
  @param[out] Buffer       buffer to receive command line
  @param[in]  BufferSize   size (in bytes) of the buffer
  @param[out] IsHex        determine if the UINT is hex or decimal format

  @retval EFI_SUCCESS
  @retval EFI_BUFFER_TOO_SMALL
  @retval EFI_TIMEOUT

**/
EFI_STATUS
ShellReadUintn (
  IN        SHELL   *Shell,
  OUT       CHAR16  *Buffer,
  IN  CONST UINTN    BufferSize,
  OUT       BOOLEAN *IsHex
  );

#endif
