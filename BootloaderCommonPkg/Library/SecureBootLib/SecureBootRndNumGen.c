/** @file

  Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

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
