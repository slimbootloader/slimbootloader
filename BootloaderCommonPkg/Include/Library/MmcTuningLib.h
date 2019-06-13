/** @file

  Copyright (c) 2017 - 2018, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _MMC_TUNING_LIB_H_
#define _MMC_TUNING_LIB_H_

#include <Uefi/UefiBaseType.h>

/**
  This structure describes the return value after HS400 tuning
**/
typedef struct {
  UINT8       Hs400DataValid;      ///< Set if Hs400 Tuning Data is valid after tuning
  UINT8       Hs400RxStrobe1Dll;   ///< Rx Strobe Delay Control - Rx Strobe Delay DLL 1 (HS400 Mode)
  UINT8       Hs400TxDataDll;      ///< Tx Data Delay Control 1 - Tx Data Delay (HS400 Mode)
  UINT8       Hs400DriverStrength; ///< Hs400 Driver Strength
  CHAR8       SerialNumber[16];    ///< Stores Serial Number
} EMMC_TUNING_DATA;

typedef struct {
  UINT32         TxDataDllCntl1;
  UINT32         RxStrobeDllCntl;
} EMMC_DLL_TUNING_REG_LIST;


/**
  Populates the dll tuning register list from loader hob.

  @param[in] RegList  DLL Tuning Register list

**/
VOID
EFIAPI
SetEmmcDllTuningLibData (
  IN EMMC_DLL_TUNING_REG_LIST *RegList
  );

/**
  Get the dll tuning register list.

  @param[out] RegList  DLL Tuning Register list

  @retval EFI_SUCCESS          Getting lib data successfully.
  @retval EFI_NOT_FOUND        The Lib data is not found.

**/
EFI_STATUS
EFIAPI
GetEmmcDllTuningLibData (
  OUT EMMC_DLL_TUNING_REG_LIST *RegList
  );

/**
  This function tuning the device.

  @param[in]  DevHcPciBase     Device Host Controller's PCI ConfigSpace Base address


  @retval EFI_SUCCESS          The request is executed successfully.
  @retval Others               The request could not be executed successfully.

**/
EFI_STATUS
EFIAPI
MmcTuning (
  IN  UINTN                 MmcHcPciBase
  );

#endif
