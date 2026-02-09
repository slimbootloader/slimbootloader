/** @file
  Boot partition switcher using a runtime-copied assembly stub.

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BootPartitionSwitcher.h>
#include <Library/BootloaderCommonLib.h>
#include <Library/PcdLib.h>
#include <Library/TopSwapLib.h>

#define PCI_DEVICE_NUMBER_PCH_LPC       31
#define PCI_FUNCTION_NUMBER_PCH_P2SB    1

typedef struct {
  UINT32  TopSwapBase;
  UINT32  P2sbBase;
  UINT8   Partition;
  UINT8   P2sbWasHidden;
  UINT8   Reserved[2];
} BOOT_PARTITION_SWITCHER_CONTEXT;

extern VOID BootPartitionSwitcherAsmStart (VOID);
extern VOID BootPartitionSwitcherAsmEnd (VOID);

VOID
EFIAPI
BootPartitionSwitcher (
  IN BOOT_PARTITION Partition
  )
{
  EFI_STATUS                      Status;
  UINT32                          TopSwapBase;
  BOOLEAN                         P2sbWasHidden;
  BOOT_PARTITION_SWITCHER_CONTEXT  Context;
  UINT8                            *Stub;
  UINTN                            StubSize;
  VOID (EFIAPI *StubFunc)(BOOT_PARTITION_SWITCHER_CONTEXT *Context);

  Status = GetTopSwapBase (&TopSwapBase, &P2sbWasHidden);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "BootPartitionSwitcher: GetTopSwapBase failed: %r\n", Status));
    return;
  }

  Context.TopSwapBase  = TopSwapBase;
  Context.P2sbBase     = (UINT32)MM_PCI_ADDRESS (0, PCI_DEVICE_NUMBER_PCH_LPC, PCI_FUNCTION_NUMBER_PCH_P2SB, 0);
  Context.Partition    = (UINT8)Partition;
  Context.P2sbWasHidden = (UINT8)(P2sbWasHidden ? 1 : 0);
  Context.Reserved[0]  = 0;
  Context.Reserved[1]  = 0;

  StubSize = (UINTN)((UINT8 *)(UINTN)BootPartitionSwitcherAsmEnd - (UINT8 *)(UINTN)BootPartitionSwitcherAsmStart);
  if (StubSize == 0) {
    DEBUG ((DEBUG_ERROR, "BootPartitionSwitcher: stub size is 0\n"));
    return;
  }

  Stub = AllocatePool (StubSize);
  if (Stub == NULL) {
    DEBUG ((DEBUG_ERROR, "BootPartitionSwitcher: failed to allocate stub buffer\n"));
    return;
  }

  CopyMem (Stub, (UINT8 *)(UINTN)BootPartitionSwitcherAsmStart, StubSize);
  StubFunc = (VOID (EFIAPI *)(BOOT_PARTITION_SWITCHER_CONTEXT *Context))(UINTN)Stub;
  StubFunc (&Context);

  CpuDeadLoop ();
}
