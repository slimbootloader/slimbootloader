/** @file
  Copyright (c) 2014, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _UFS_PCI_HOST_CONTROLLER_PEI_H_
#define _UFS_PCI_HOST_CONTROLLER_PEI_H_

#include <PiPei.h>

#include <IndustryStandard/Pci.h>

#include <Library/DebugLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PciLib.h>
#include <Library/MemoryAllocationLib.h>

#define UFS_HC_PEI_SIGNATURE    SIGNATURE_32 ('U', 'F', 'S', 'P')
#define MAX_UFS_HCS             8

typedef struct {
  UINTN                         Signature;
  UINTN                         TotalUfsHcs;
  UINTN                         UfsHcPciAddr[MAX_UFS_HCS];
} UFS_HC_PEI_PRIVATE_DATA;


/**
  Get the MMIO base address of UFS host controller.

  @param[in]  Private            The pointer to the UFS_PEIM_HC_PRIVATE_DATA data structure.
  @param[in]  ControllerId       The ID of the UFS host controller.
  @param[out] MmioBar            Pointer to the UFS host controller MMIO base address.

  @retval EFI_SUCCESS            The operation succeeds.
  @retval EFI_INVALID_PARAMETER  The parameters are invalid.

**/
EFI_STATUS
EFIAPI
GetUfsHcMmioBar (
  IN     UFS_HC_PEI_PRIVATE_DATA      *Private,
  IN     UINT8                         ControllerId,
  OUT UINTN                         *MmioBar
  );

/**
  The user code starts with this function.

  @param[out]  Private           A pointer to UFS_PEIM_HC_PRIVATE_DATA data structure.
  @param[in]   UfsHcPciBase      UFS Host Controller's PCI ConfigSpace Base address

  @retval EFI_SUCCESS            The driver is successfully initialized.
  @retval Others                 Can't initialize the driver.

**/
EFI_STATUS
EFIAPI
InitializeUfsHcPeim (
  OUT UFS_HC_PEI_PRIVATE_DATA    *Private,
  IN  UINTN                       UfsHcPciBase
  );

#endif
