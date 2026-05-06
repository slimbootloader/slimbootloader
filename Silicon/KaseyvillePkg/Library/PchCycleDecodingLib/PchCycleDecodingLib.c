/** @file

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Uefi/UefiBaseType.h>
#include <Library/DebugLib.h>
#include <Library/PciLib.h>
#include <Library/PchCycleDecodingLib.h>
#include <Register/BdfAssignment.h>
#include <Register/PchRegsLpc.h>
#include <Register/EspiRegs.h>

/**
  Returns LPC device PCI base address.

  @retval                   LPC PCI base address.
**/
UINT64
LpcPciCfgBase (
  VOID
  )
{
  return PCI_LIB_ADDRESS (
           DEFAULT_PCI_BUS_NUMBER_PCH,
           PCI_DEVICE_NUMBER_PCH_LPC,
           PCI_FUNCTION_NUMBER_PCH_LPC,
           0
           );
}

/**
  Set IBL eSPI IO decode ranges.
  Program eSPI I/O Decode Ranges in eSPI PCI offset 80h.
  Please check EDS for detail of eSPI IO decode ranges bit definition.
  Bit  12: FDD range
  Bit 9:8: LPT range
  Bit 6:4: ComB range
  Bit 2:0: ComA range

  @param[in] LpcIoDecodeRanges          eSPI IO decode ranges bit settings.

  @retval EFI_SUCCESS                   Successfully completed.
**/
EFI_STATUS
EFIAPI
PchLpcIoDecodeRangesSet (
  IN  UINT16                            LpcIoDecodeRanges
  )
{
  UINT64                                LpcBaseAddr;

  //
  // Note: Inside this function, don't use debug print since it's could used before debug print ready.
  //

  LpcBaseAddr = LpcPciCfgBase ();

  //
  // check if setting is identical
  //
  if (LpcIoDecodeRanges == PciRead16 ((UINTN)(LpcBaseAddr + R_LPC_CFG_IOD))) {
    return EFI_SUCCESS;
  }

  //
  // program eSPI PCI offset 80h.
  //
  PciWrite16 ((UINTN)(LpcBaseAddr + R_LPC_CFG_IOD), LpcIoDecodeRanges);

  return EFI_SUCCESS;
}

/**
  Set IBL eSPI and eSPI CS1# IO enable decoding.
  Setup I/O Enables in eSPI PCI offset 82h (eSPI CS0#) or A0h (eSPI CS1#).
  Note: Bit[15:10] of the source decode register is Read-Only. The IO range indicated by the Enables field
  in eSPI PCI offset 82h[13:10] or A0h[13:10] is always forwarded to subtractive agent for handling.
  Please check EDS for detail of Lpc/eSPI IO decode ranges bit definition.

  @param[in] IoEnableDecoding           eSPI IO enable decoding bit settings.
  @param[in] DeviceId                    Device ID (refer to DEVICE_ID_INDEX)
  @param[in] EspiPcrAccess              eSPI Sideband register access protocol

  @retval EFI_SUCCESS                   Successfully completed.
**/
EFI_STATUS
EFIAPI
EspiIoEnableDecodingSetHelper (
  IN  UINT16                            IoEnableDecoding,
  IN  DEVICE_ID_INDEX                   DeviceId
  )
{
  UINT64      LpcBaseAddr;
  UINT16      Cs1IoEnableDecodingOrg;
  UINT16      Cs0IoEnableDecodingOrg;
  UINT16      IoEnableDecodingMerged;

  LpcBaseAddr = LpcPciCfgBase ();

  Cs0IoEnableDecodingOrg = PciRead16 ((UINTN)(LpcBaseAddr + R_ESPI_CFG_ESPI_IOE));
  Cs1IoEnableDecodingOrg = 0;

  if ((IoEnableDecoding | Cs1IoEnableDecodingOrg) == Cs0IoEnableDecodingOrg) {
    return EFI_SUCCESS;
  } else {
    IoEnableDecodingMerged = (Cs1IoEnableDecodingOrg | IoEnableDecoding);
  }

  //
  // program PCI offset 82h for eSPI.
  //
  PciWrite16 ((UINTN)(LpcBaseAddr + R_ESPI_CFG_ESPI_IOE), IoEnableDecodingMerged);

  if (DeviceId == DeviceEspiCS1) {
    //
    // For eSPI CS1# device program eSPI PCI offset A0h.
    //
    PciWrite16 ((UINTN)(LpcBaseAddr + R_ESPI_CFG_ESPI_CS1IORE), IoEnableDecoding);
  }

  return EFI_SUCCESS;
}

/**
  Set IBL LPC and eSPI CS0# IO enable decoding.
  Setup I/O Enables in eSPI PCI offset 82h.
  Note: Bit[15:10] of the source decode register is Read-Only. The IO range indicated by the Enables field
  in eSPI PCI offset 82h[13:10] is always forwarded to subtractive agent for handling.
  Please check EDS for detail of eSPI IO decode ranges bit definition.

  @param[in] LpcIoEnableDecoding        LPC IO enable decoding bit settings.

  @retval EFI_SUCCESS                   Successfully completed.
**/
EFI_STATUS
EFIAPI
PchLpcIoEnableDecodingSet (
  IN  UINT16                            LpcIoEnableDecoding
  )
{
  return EspiIoEnableDecodingSetHelper (LpcIoEnableDecoding, DeviceEspiCS0);
}
