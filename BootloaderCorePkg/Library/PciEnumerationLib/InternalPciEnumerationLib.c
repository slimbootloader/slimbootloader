/** @file

  Copyright (c) 2020 - 2022, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Uefi/UefiBaseType.h>
#include <Library/BaseLib.h>
#include <Library/IoLib.h>
#include <Library/PcdLib.h>
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
  OriginalValue = MmioRead32 (MCFG_ADDR (PciIoDevice, Offset));
  AllOne = 0xFFFFFFFF;
  Mask   = 0xFFFFFFFF;
  if (Offset == PCI_EXPANSION_ROM_BASE) {
    AllOne &=  ~BIT0;
    Mask   &= ~0x7FF;
  }
  MmioWrite32 (MCFG_ADDR (PciIoDevice, Offset), AllOne);
  Value = MmioRead32 (MCFG_ADDR (PciIoDevice, Offset)) & Mask;
  MmioWrite32 (MCFG_ADDR (PciIoDevice, Offset), OriginalValue);

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

/**
  Return the host bridge table. If PcdPciHostBridgeTableBase is not set,
  synthesise a single-entry table from PcdPciExpressBaseAddress.

  @return Pointer to the active PCI_HOST_BRIDGE_TABLE.
**/
PCI_HOST_BRIDGE_TABLE *
GetHostBridgeTable (
  VOID
  )
{
  STATIC PCI_HOST_BRIDGE_TABLE  mFallback;
  PCI_HOST_BRIDGE_TABLE        *Table;

  Table = (PCI_HOST_BRIDGE_TABLE *)(UINTN)PcdGet32 (PcdPciHostBridgeTableBase);
  if ((Table != NULL) && (Table->Count > 0)) {
    if (Table->Count <= MAX_HOST_BRIDGES) {
      return Table;
    }
    DEBUG ((DEBUG_WARN, "PCI host bridge table Count %u exceeds MAX_HOST_BRIDGES %u; using fallback\n",
            Table->Count, MAX_HOST_BRIDGES));
  }

  //
  // No platform-supplied table: build a single-entry fallback from the
  // existing PcdPciExpressBaseAddress covering the full bus range 0..0xFF.
  //
  mFallback.Count                  = 1;
  mFallback.HostBridge[0].McfgBase = PcdGet64 (PcdPciExpressBaseAddress);
  mFallback.HostBridge[0].Segment  = 0;
  mFallback.HostBridge[0].BusBase  = 0;
  mFallback.HostBridge[0].BusLimit = PCI_MAX_BUS;
  return &mFallback;
}

UINT64
GetSegmentMcfgBase (
  IN UINT8  Segment
  )
{
  CONST PCI_HOST_BRIDGE_TABLE  *Table;
  UINT8                         Index;

  Table = GetHostBridgeTable ();
  for (Index = 0; Index < Table->Count; Index++) {
    if (Table->HostBridge[Index].Segment == Segment) {
      return Table->HostBridge[Index].McfgBase;
    }
  }
  DEBUG ((DEBUG_ERROR, "PCI segment %u not found in host bridge table; falling back to entry 0\n", Segment));
  ASSERT (FALSE);
  return Table->HostBridge[0].McfgBase;
}

UINTN
McfgAddr (
  IN CONST PCI_IO_DEVICE  *Dev,
  IN UINTN                 Off
  )
{
  return (UINTN)GetSegmentMcfgBase (Dev->Segment) + (UINTN)Dev->Address + Off;
}
