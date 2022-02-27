/** @file

  Copyright (c) 2020 - 2022, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Uefi/UefiBaseType.h>
#include <Library/BaseLib.h>
#include <Library/PciExpressLib.h>
#include "InternalPciEnumerationLib.h"

/**
  Check whether the bar is existed or not.

  @param[in]  PciIoDevice       A pointer to the PCI_IO_DEVICE.
  @param[in]  Offset            The offset.
  @param[out] BarLengthValue    The bar length value returned.
  @param[out] OriginalBarValue  The original bar value returned.

  @retval EFI_NOT_FOUND    The bar doesn't exist.
  @retval EFI_SUCCESS      The bar exist.

**/
EFI_STATUS
EFIAPI
BarExisted (
  IN  PCI_IO_DEVICE *PciIoDevice,
  IN  UINTN         Offset,
  OUT UINT32        *BarLengthValue,
  OUT UINT32        *OriginalBarValue
  )
{
  UINT32              OriginalValue;
  UINT32              AllOne;
  UINT32              Mask;
  volatile UINT32     Value;

  //
  // Preserve the original value
  //
  OriginalValue = PciExpressRead32 (PciIoDevice->Address + Offset);
  AllOne = 0xFFFFFFFF;
  Mask   = 0xFFFFFFFF;
  if (Offset == PCI_EXPANSION_ROM_BASE) {
    AllOne &=  ~BIT0;
    Mask   &= ~0x7FF;
  }
  PciExpressWrite32 (PciIoDevice->Address + Offset, AllOne);
  Value = PciExpressRead32 (PciIoDevice->Address + Offset) & Mask;
  PciExpressWrite32 (PciIoDevice->Address + Offset, OriginalValue);

  if (BarLengthValue != NULL) {
    *BarLengthValue = Value;
  }

  if (OriginalBarValue != NULL) {
    *OriginalBarValue = OriginalValue;
  }

  if (Value == 0) {
    return EFI_NOT_FOUND;
  } else {
    return EFI_SUCCESS;
  }
}
