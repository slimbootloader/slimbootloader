/** @file
  Header file for Amt functionality and configuration.

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/
#ifndef _AMT_LIB_H_
#define _AMT_LIB_H_

#include <Uefi/UefiBaseType.h>


/**
  Me Measured Boot setup and init

  @retval     RETURN_SUCCESS       If getting the MBP data successfully
  @retval     EFI_NOT_FOUND        Failed to get MBP data
**/
EFI_STATUS
DisableAmtFeatures (
  VOID
  );
#endif // _AMT_LIB_H_
