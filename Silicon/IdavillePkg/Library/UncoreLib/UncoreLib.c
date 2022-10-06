/** @file

  Copyright (c) 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Uefi/UefiBaseType.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <Library/PcdLib.h>
#include <Library/BootloaderCoreLib.h>
#include <Library/BootloaderCommonLib.h>
#include <Library/UncoreLib.h>
#include <PlatformBase.h>

#define PCI_BUS_NUMBER_PCU      ((GetFspHobListPtr () == NULL) ? 31 : 255)
#define PCI_DEVICE_NUMBER_PCU   30
#define PCI_FUNCION_NUMBER_CR1  1
#define PCI_FUNCION_NUMBER_CR3  3

/**
  Returns the bit mask of the total Iios enabled for a given CPU socket.

  @param[in]  SocId             - Soc ID of the CPU who's enabled IIOs info is requested, 0,1,2,3,etc
  @param[out] IioPresentBitMask - Bit mask of the enabled IIOs in given socket

  @retval EFI_SUCCESS      - on successful completion
  @retval Others           - on any errors

**/
EFI_STATUS
EFIAPI
GetIioPresentBitMask (
  IN  UINT8                      SocId,
  OUT UINT32                    *IioPresentBitMask
  )
{
  UINTN   Address;
  UINT32  Value;

  if (SocId != 0) {
    DEBUG ((DEBUG_INFO, "GetIioPresentBitMask: SocketId 0 supported only for now.\n"));
    *IioPresentBitMask = 0x0;
    return EFI_UNSUPPORTED;
  }

  Address = MM_PCI_ADDRESS (PCI_BUS_NUMBER_PCU, PCI_DEVICE_NUMBER_PCU, PCI_FUNCION_NUMBER_CR3, 0);
  Value   = MmioRead32 (Address + 0xBC);
  *IioPresentBitMask = BitFieldRead32 (Value, 0, MAX_IIO_STACK - 1);

  return EFI_SUCCESS;
}

/**
  Returns the total Iios enabled for a given CPU socket.

  @param[in]  SocId             - Soc ID of the CPU who's enabled IIOs info is requested, 0,1,2,3,etc
  @param[out] IIOCount          - Total IIOs in given socket

  @retval EFI_SUCCESS      - on successful completion
  @retval Others           - on any errors

**/
EFI_STATUS
EFIAPI
GetIioCount (
  IN  UINT8                      SocId,
  OUT UINT8                     *IioCount
  )
{
  EFI_STATUS  Status;
  UINT32      IioPresentBitMask;
  UINT8       Index;
  UINT8       Count;

  Count = 0;
  Status = GetIioPresentBitMask (SocId, &IioPresentBitMask);
  if (!EFI_ERROR (Status)) {
    for (Index = 0; Index < MAX_IIO_STACK; Index++) {
      if ((IioPresentBitMask & BIT0) == BIT0) {
        Count++;
      }
      IioPresentBitMask >>= 1;
    }
  }

  *IioCount = Count;

  return Status;
}

/**
  Get the Chop Type

  @param[in] SocId        - Socket ID

  @retval                 - The Chop Type

**/
TYPE_CHOPTYPE_ENUM
EFIAPI
GetChopType (
  IN  UINT8 SocId
  )
{
  UINTN   Address;
  UINT32  Value;

  if (SocId != 0) {
    DEBUG ((DEBUG_INFO, "GetChopType: SocketId 0 supported only for now.\n"));
    return TypeChopTypeMax;
  }

  Address = MM_PCI_ADDRESS (PCI_BUS_NUMBER_PCU, PCI_DEVICE_NUMBER_PCU, PCI_FUNCION_NUMBER_CR3, 0);
  Value   = MmioRead32 (Address + 0x94);

  return (TYPE_CHOPTYPE_ENUM)((Value >> 6) & 0x3);
}

/**
  Get desired simultaneous multithreading

  @retval                 0: Disable SMT, 1: Enable SMT

**/
UINT8
EFIAPI
GetDesiredSmtEnable (
  VOID
  )
{
  UINTN   Address;
  UINT32  Value;

  Address = MM_PCI_ADDRESS (PCI_BUS_NUMBER_PCU, PCI_DEVICE_NUMBER_PCU, PCI_FUNCION_NUMBER_CR1, 0);
  Value   = MmioRead32 (Address + 0xBC);

  if ((Value & BIT30) != 0) {
    return 0;
  }
  return 1;
}
