/** @file
  UfsPciHcPei driver is used to provide platform-dependent info, mainly UFS host controller
  MMIO base, to upper layer UFS drivers.

  Copyright (c) 2014, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "UfsInternal.h"

/**
  Get the MMIO base address of UFS host controller.

  @param[in]  Private            A pointer to UFS_PEIM_HC_PRIVATE_DATA data structure.
  @param[in]  ControllerId       The ID of the UFS host controller.
  @param[out] MmioBar            Pointer to the UFS host controller MMIO base address.

  @retval EFI_SUCCESS            The operation succeeds.
  @retval EFI_INVALID_PARAMETER  The parameters are invalid.

**/
EFI_STATUS
EFIAPI
GetUfsHcMmioBar (
  IN     UFS_HC_PEI_PRIVATE_DATA       *Private,
  IN     UINT8                         ControllerId,
  OUT UINTN                         *MmioBar
  )
{


  if ((Private == NULL) || (MmioBar == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  if (ControllerId >= Private->TotalUfsHcs) {
    return EFI_INVALID_PARAMETER;
  }

  *MmioBar = (UINTN)Private->UfsHcPciAddr[ControllerId];

  return EFI_SUCCESS;
}

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
  )
{

  ZeroMem (Private, sizeof (UFS_HC_PEI_PRIVATE_DATA));

  Private->Signature    = UFS_HC_PEI_SIGNATURE;
  Private->UfsHcPciAddr[Private->TotalUfsHcs] = MmioRead32 (UfsHcPciBase + PCI_BASE_ADDRESSREG_OFFSET) & 0xFFFFF000;;
  Private->TotalUfsHcs++;

  return EFI_SUCCESS;
}


/**
  The user code starts with this function.

  @param[out]  Private           A pointer to UFS_PEIM_HC_PRIVATE_DATA data structure.

  @retval EFI_SUCCESS            The driver is successfully initialized.
  @retval Others                 Can't initialize the driver.

**/
EFI_STATUS
EFIAPI
InitializeUfsHcPeim_org (
  OUT UFS_HC_PEI_PRIVATE_DATA    *Private
  )
{
  //EFI_BOOT_MODE            BootMode;
  UINT16                   Bus;
  UINT16                   Device;
  UINT16                   Function;
  UINT32                   Size;
  UINT8                    SubClass;
  UINT8                    BaseClass;
  //Status = PeiServicesGetBootMode (&BootMode);
  ///
  /// We do not export this in S3 boot path, because it is only for recovery.
  ///
  //if (BootMode == BOOT_ON_S3_RESUME) {
  //  return EFI_SUCCESS;
  //}

  ZeroMem (Private, sizeof (UFS_HC_PEI_PRIVATE_DATA));

  Private->Signature            = UFS_HC_PEI_SIGNATURE;

  for (Bus = 0; Bus < 256; Bus++) {
    for (Device = 0; Device < 32; Device++) {
      for (Function = 0; Function < 8; Function++) {
        SubClass  = PciRead8 (PCI_LIB_ADDRESS (Bus, Device, Function, 0x0A));
        BaseClass = PciRead8 (PCI_LIB_ADDRESS (Bus, Device, Function, 0x0B));

        if ((SubClass == 0x09) && (BaseClass == PCI_CLASS_MASS_STORAGE)) {
          DEBUG ((DEBUG_ERROR, "Find UFS device: Bus:0x%x, Device:0x%x, Function:0x%x\n", Bus, Device, Function));
          //
          // Get the Ufs Pci host controller's MMIO region size.
          //
          PciAnd16 (PCI_LIB_ADDRESS (Bus, Device, Function, PCI_COMMAND_OFFSET),
                    (UINT16)~ (EFI_PCI_COMMAND_BUS_MASTER | EFI_PCI_COMMAND_MEMORY_SPACE));
          PciWrite32 (PCI_LIB_ADDRESS (Bus, Device, Function, PCI_BASE_ADDRESSREG_OFFSET), 0xFFFFFFFF);
          Size = PciRead32 (PCI_LIB_ADDRESS (Bus, Device, Function, PCI_BASE_ADDRESSREG_OFFSET));
          //
          // Assign resource to the Ufs Pci host controller's MMIO BAR.
          // Enable the Ufs Pci host controller by setting BME and MSE bits of PCI_CMD register.
          //
          PciWrite32 (PCI_LIB_ADDRESS (Bus, Device, Function, PCI_BASE_ADDRESSREG_OFFSET),
                      (UINT32) (PcdGet32 (PcdUfsPciHostControllerMmioBase) + Size * Private->TotalUfsHcs));
          PciOr16 (PCI_LIB_ADDRESS (Bus, Device, Function, PCI_COMMAND_OFFSET),
                   (EFI_PCI_COMMAND_BUS_MASTER | EFI_PCI_COMMAND_MEMORY_SPACE));
          //
          // Record the allocated Mmio base address.
          //
          Private->UfsHcPciAddr[Private->TotalUfsHcs] = PcdGet32 (PcdUfsPciHostControllerMmioBase) + Size * Private->TotalUfsHcs;
          Private->TotalUfsHcs++;
          ASSERT (Private->TotalUfsHcs < MAX_UFS_HCS);
        }
      }
    }
  }

  return EFI_SUCCESS;
}

