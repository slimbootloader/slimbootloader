/** @file
  Intel(R) SGX library.

  Copyright (c) 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#ifndef SGXLIB_H
#define SGXLIB_H

#include <FspmUpd.h>
#include <FspsUpd.h>
#include <CpuNvsAreaDef.h>

/**
  Check if SGX instruction set is supported by the processor

  @retval TRUE if SGX capability is supported by processor
  @retval FALSE if SGX capability is not supported by processor

**/
BOOLEAN
IsSgxCapSupported (
  VOID
  );

/**
  Check if input PRMRR value is supported by the processor

  @param[in]  PrmrrSizeFromConfig  PrmrrSize value entered in SGX config

  @retval     TRUE if PrmrrSizeFromConfig is supported, FALSE otherwise

**/
BOOLEAN
IsPrmrrSizeSupported (
  IN UINT32 PrmrrSizeFromConfig
  );


/**
  Check if SGX was enabled

  @retval TRUE if SGX in Feature Control MSR was set
  @retval FALSE if SGX in Feature Control MSR was not set

**/
BOOLEAN
IsSgxFeatureCtrlSet (
  VOID
  );

/**
  Check if PRM was successfully allocated and PRMRRs were set

  @retval TRUE if PRMRR base was already set
  @retval FALSE if PRMRR base was not set

**/
BOOLEAN
IsPrmrrAlreadySet (
  VOID
  );

/**
  Update FSP-M UPD SGX config data

  @param[in]  FspmUpd           Pointer to the FSP-S UPD to be updated

  @retval TRUE if SGX config was found and set
  @retval FALSE if SGX config was not found

**/
BOOLEAN
UpdateFspmSgxConfig (
  IN FSPM_UPD *FspmUpd
  );

/**
  Update FSP-S UPD SGX config data

  @param[in]  FspsUpd           Pointer to the FSP-S UPD to be updated

  @retval TRUE if SGX config was found and set
  @retval FALSE if SGX config was not found

**/
BOOLEAN
UpdateFspsSgxConfig (
  IN FSPS_UPD *FspsUpd
  );

/**
  Update SGX CPU NVS variable for EPC device in Platform.asl

  @param[in, out]  CpuNvs           Pointer to CPU NVS region

**/
VOID
UpdateSgxNvs (
  IN OUT CPU_NVS_AREA  *CpuNvs
  );

#endif // SGXLIB_H

