/** @file

  Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <Library/BootloaderCommonLib.h>
#include <Library/RngLib.h>

/**
  Generate RandomNumbers.

  @param[out] Data                   Array of random numbers generated
  @param[in] Size                    Size of the array to be generated.

  @retval EFI_SUCCESS                Random number generation successful.
  @retval EFI_UNSUPPORTED            Couldnt generate a random number.
**/
EFI_STATUS
EFIAPI
GenerateRandomNumbers (
  OUT CHAR8 *Data,
  IN UINTN Size
  )
{
  INT32   Status;
  UINTN   TempI, TempJ;
  UINT32  Random;

  for (TempI = 0; TempI < Size; ) {
    Status = GetRandomNumber32(&Random);
    if (Status != 1) {
      return EFI_UNSUPPORTED;
    }

    for (TempJ = 0; TempJ < sizeof(Random) && TempI < Size; TempJ++, TempI++) {
      Data[TempI] = ((CHAR8 *)&Random)[TempJ];
    }
  }
  return EFI_SUCCESS;
}