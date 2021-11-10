/** @file
  This library class defines a set of methods related with MTRR.

Copyright (c) 2020, Intel Corporation. All rights reserved.<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __MTRR_LIB_H__
#define __MTRR_LIB_H__

#include <PiPei.h>

//
// The semantics of below macro is MAX_MTRR_NUMBER_OF_VARIABLE_MTRR, the real number can be read out from MTRR_CAP register.
//
#define  MTRR_NUMBER_OF_VARIABLE_MTRR  32

#define  MTRR_NUMBER_OF_FIXED_MTRR     11

//
// Structure to describe a fixed MTRR
//
typedef struct {
  UINT32  Msr;
  UINT32  BaseAddress;
  UINT32  Length;
} FIXED_MTRR;

//
// Structure to hold base and mask pair for variable MTRR register
//
typedef struct _MTRR_VARIABLE_SETTING_ {
  UINT64    Base;
  UINT64    Mask;
} MTRR_VARIABLE_SETTING;

//
// Array for variable MTRRs
//
typedef struct _MTRR_VARIABLE_SETTINGS_ {
  MTRR_VARIABLE_SETTING   Mtrr[MTRR_NUMBER_OF_VARIABLE_MTRR];
} MTRR_VARIABLE_SETTINGS;

//
// Array for fixed MTRRs
//
typedef  struct  _MTRR_FIXED_SETTINGS_ {
  UINT64       Mtrr[MTRR_NUMBER_OF_FIXED_MTRR];
} MTRR_FIXED_SETTINGS;

//
// Structure to hold all MTRRs
//
typedef struct _MTRR_SETTINGS_ {
  MTRR_FIXED_SETTINGS       Fixed;
  MTRR_VARIABLE_SETTINGS    Variables;
} MTRR_SETTINGS;

/**
  This function gets the content in all MTRRs (variable and fixed)

  @param[out]  MtrrSetting   A buffer to hold all MTRRs content.

  @retval  EFI_INVALID_PARAMETER   MtrrSetting is NULL.
           EFI_OUT_OF_RESOURCES    Insufficient space to save all MTRR values.
           EFI_SUCCESS             MTRR values were retrieved successfully.
**/
EFI_STATUS
EFIAPI
GetCpuMtrrs (
  OUT MTRR_SETTINGS                *MtrrSetting
  );

/**
  This function sets all MTRRs (variable and fixed)

  @param[in]  MtrrSetting   A buffer to hold all MTRRs content.

  @retval  EFI_INVALID_PARAMETER   MtrrSetting is NULL.
           EFI_SUCCESS             MTRR values were programmed successfully.
**/
EFI_STATUS
EFIAPI
SetCpuMtrrs (
  IN MTRR_SETTINGS                *MtrrSetting
  );

/**
  Worker function returns the variable MTRR count for the CPU.

  @return Variable MTRR count

**/
UINT32
EFIAPI
GetVariableMtrrCount (
  VOID
  );

/**
  Print MTRR settings.

  @param[in]  Str           A string will be print before MTRR data if it is not NULL.
  @param[in]  ConsoleOut    Redirect to consoles if TRUE. Otherwise, DEBUG()

**/
VOID
EFIAPI
PrintMtrr (
  IN  CHAR8               *Str,
  IN  BOOLEAN              ConsoleOut
  );

#endif
