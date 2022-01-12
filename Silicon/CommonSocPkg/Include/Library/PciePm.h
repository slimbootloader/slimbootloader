/** @file
  Header file for Pcie PM (power management control). Platform can provide
  an implementation.

  Copyright (c) 2022, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/
#ifndef _PCIE_PM_H_
#define _PCIE_PM_H_

/**
  Store Root Port Config based on FSP-s UPDs

  @param[in]  FspsConfig       The pointer to FSP-S Config

**/
VOID
EFIAPI
StoreRpConfig (
  VOID  *FspsConfig
);


/**
  Config PCIE power management settings
**/
VOID
EFIAPI
PciePmConfig (
  VOID
);

#endif // _PCIE_PM_H_
