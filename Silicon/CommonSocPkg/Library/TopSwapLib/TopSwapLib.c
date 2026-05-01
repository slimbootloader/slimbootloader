/** @file
  Library for top swap.

  Copyright (c) 2022, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Library/TopSwapLib.h>
#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <Library/PcdLib.h>
#include <Library/BaseLib.h>
#include <Library/P2sbLib.h>
#include <Register/P2sbRegs.h>
#include <Library/PciLib.h>
#include <Library/BootloaderCommonLib.h>
#include <Guid/OsBootOptionGuid.h>

#define R_RTC_PCR_BUC                   0x3414

/**
  Get P2SB device address

  @retval     Device address for P2SB device. If the device could not be
              found in the device table, a default P2SB device address is
              returned.

**/
UINTN
GetP2sbDeviceAddr (
  VOID
  )
{
  UINTN               P2sbBase;

  P2sbBase = GetDeviceAddr (PlatformDeviceP2sb, 0);
  if ((P2sbBase & 0x0FFFFFFF) != 0) {
    P2sbBase = TO_PCI_LIB_ADDRESS(P2sbBase);
  } else {
    DEBUG ((DEBUG_WARN, "P2SB device not found in device table. Use default P2SB device address.\n"));
    P2sbBase = PCI_LIB_ADDRESS(0, PCI_DEVICE_NUMBER_PCH_P2SB, PCI_FUNCTION_NUMBER_PCH_P2SB, 0);
  }

  return P2sbBase;
}

/**
  Switch between the boot partitions.

  This function will set the top swap register bit with the given partition.

  @param[in] Partition        Partition to be indicated in register.

  @retval  EFI_SUCCESS        Parition successfully set.
  @retval  others             Error occurred.
**/
EFI_STATUS
EFIAPI
SetBootPartition (
  IN BOOT_PARTITION  Partition
  )
{
  BOOLEAN             P2sbIsHidden;
  UINT32              AndData;
  UINT32              OrData;
  UINTN               P2sbBase;

  P2sbBase = GetP2sbDeviceAddr ();
  P2sbIsHidden = FALSE;
  if (PciRead16 (P2sbBase) == 0xFFFF) {
    PciWrite8 (P2sbBase + R_P2SB_CFG_P2SBC + 1, 0);
    DEBUG ((DEBUG_INFO, "P2SB is hidden, Read VID_DID(0x%x) after unhiding.\n", PciRead32 (P2sbBase)));
    P2sbIsHidden = TRUE;
  }

  AndData = (UINT32)~BIT0;
  if (Partition == BackupPartition) {
    OrData = BIT0;
  } else {
    OrData = 0;
  }

  P2SbAndThenOr32 (P2sbBase, PcdGet8 (PcdPidRtcHostNumber), 0, R_RTC_PCR_BUC, AndData, OrData);

  if (P2sbIsHidden) {
    PciWrite8 (P2sbBase + R_P2SB_CFG_P2SBC + 1, B_P2SB_CFG_P2SBC_HIDE >> 8);
  }

  return EFI_SUCCESS;
}

/**
  Get the current boot partition.

  This function will read the top swap register bit for the current partition.

  @param[in] Partition        Partition indicated by register.

  @retval  EFI_SUCCESS        Partition successfully retrieved.
  @retval  others             Error occurred.
**/
EFI_STATUS
EFIAPI
GetBootPartition (
  OUT BOOT_PARTITION *Partition
  )
{
  BOOLEAN             P2sbIsHidden;
  UINT32              RegValue;
  UINTN               P2sbBase;

  if (Partition == NULL) {
    DEBUG ((DEBUG_ERROR, "Partition not initialized!\n"));
    return EFI_INVALID_PARAMETER;
  }

  P2sbBase = GetP2sbDeviceAddr ();
  P2sbIsHidden = FALSE;
  if (PciRead16 (P2sbBase) == 0xFFFF) {
    PciWrite8 (P2sbBase + R_P2SB_CFG_P2SBC + 1, 0);
    DEBUG ((DEBUG_INFO, "P2SB is hidden, Read VID_DID(0x%x) after unhide.\n", PciRead32 (P2sbBase)));
    P2sbIsHidden = TRUE;
  }

  RegValue = P2sbRead32 (P2sbBase, PcdGet8 (PcdPidRtcHostNumber), 0, R_RTC_PCR_BUC);
  *Partition = (BOOT_PARTITION)(RegValue & BIT0);

  DEBUG ((DEBUG_INFO, "*Partition = 0x%x\n", *Partition));

  if (P2sbIsHidden) {
    PciWrite8(P2sbBase + R_P2SB_CFG_P2SBC + 1, B_P2SB_CFG_P2SBC_HIDE >> 8);
  }

  return EFI_SUCCESS;
}
