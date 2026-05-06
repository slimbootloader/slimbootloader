/** @file

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Base.h>
#include <Uefi/UefiBaseType.h>
#include <Library/IoLib.h>
#include <Library/BaseLib.h>
#include <IndustryStandard/Pci.h>
#include <Library/PchInfoLib.h>
#include <Library/BootloaderCommonLib.h>
#include <Register/BdfAssignment.h>
#include <Register/PchRegsLpc.h>

/**
  Return Pch Series

  @retval PCH_SERIES            Pch Series
**/
PCH_SERIES
PchSeries (
  VOID
  )
{
  return PCH_H;
}

/**
  Return Pch Generation

  @retval PCH_GENERATION            Pch Generation
**/
PCH_GENERATION
PchGeneration (
  VOID
  )
{
  return S3M2_IBL;
}

/**
  Return Pch stepping type

  @retval PCH_STEPPING            Pch stepping type
**/
PCH_STEPPING
PchStepping (
  VOID
  )
{
  UINT8                RevId;
  UINTN                LpcBaseAddress;
  static PCH_STEPPING  PchStepping = PCH_STEPPING_MAX;

  if (PchStepping != PCH_STEPPING_MAX) {
    return PchStepping;
  }

  LpcBaseAddress = MM_PCI_ADDRESS (
    DEFAULT_PCI_BUS_NUMBER_PCH,
    PCI_DEVICE_NUMBER_PCH_LPC,
    PCI_FUNCTION_NUMBER_PCH_LPC,
    0);

  RevId = MmioRead8 (LpcBaseAddress + PCI_REVISION_ID_OFFSET);

  PchStepping = RevId;

  return RevId;
}
