/** @file
  A minimal command-line shell.

  Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _SHELL_H_
#define _SHELL_H_

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
