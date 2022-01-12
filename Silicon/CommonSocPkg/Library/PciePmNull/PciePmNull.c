/** @file
  This file implements default PCIe PM interfaces

  Copyright (c) 2022, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/
#include <Uefi.h>

/**
  Store Root Port Config based on FSP-s UPDs

  @param[in]  FspsConfig       The pointer to FSP-S Config

**/
VOID
EFIAPI
StoreRpConfig (
  VOID        *FspsConfig
)
{
  return;
}

/**
  Config PCIE power management settings
**/
VOID
EFIAPI
PciePmConfig (
  VOID
)
{
  return;
}

