/** @file

  Copyright (c) 2016, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _SOC_INFO_LIB_H_

/**
  Get cpu max non-burst frequency.

  @retval   cpu max non-burst frequency
**/
UINT32
EFIAPI
GetCpuMaxNbFrequency (
  VOID
  );

/**
  Get cpu core count.

  @retval   cpu core count
**/
UINT32
EFIAPI
GetCpuNumCores (
  VOID
  );

/**
  Get cpu turbo ratio.

  @retval   cpu turbo ratio
**/
UINT32
EFIAPI
GetCpuTurboRatio (
  VOID
  );

/**
  Get cpu ucode revision.

  @retval   cpu ucode revision
**/
UINT32
EFIAPI
GetCpuUCodeRev (
  VOID
  );

/**
  Get pch device name.

  @retval   pch device name
**/
CONST CHAR8*
EFIAPI
GetPchDeviceName (
  VOID
  );

/**
  Get pch stepping name.

  @retval   pch stepping name
**/
CONST CHAR8*
EFIAPI
GetPchSteppingName (
  VOID
  );

#endif
