/** @file
  ITBT PCIE root port library.
  All function in this library is available for PEI, DXE, and SMM.
  This library has a dependency with TcssHob.

  Copyright (c) 2017 - 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include <Base.h>
#include <Uefi/UefiBaseType.h>
#include <Library/IoLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseLib.h>
//#include <Library/PciSegmentLib.h>
#include <Library/ItbtPcieRpLib.h>
#include <Register/SaRegsItbtPcie.h>

GLOBAL_REMOVE_IF_UNREFERENCED ITBT_PCIE_RP_INFO mIbtbPcieRpInfo[] = {
//
// Segment       Bus                  Device                 Function                 Enable
//
  { 0,     SA_ITBT_PCIE_BUS_NUM, SA_ITBT_PCIE_DEV_NUM, SA_ITBT_PCIE_0_DEFAULT_FUN_NUM, TRUE },  // Itbt Pcie Rp 1
  { 0,     SA_ITBT_PCIE_BUS_NUM, SA_ITBT_PCIE_DEV_NUM, SA_ITBT_PCIE_1_DEFAULT_FUN_NUM, TRUE },  // Itbt Pcie Rp 2
  { 0,     SA_ITBT_PCIE_BUS_NUM, SA_ITBT_PCIE_DEV_NUM, SA_ITBT_PCIE_2_DEFAULT_FUN_NUM, FALSE }, // Itbt Pcie Rp 3
  { 0,     SA_ITBT_PCIE_BUS_NUM, SA_ITBT_PCIE_DEV_NUM, SA_ITBT_PCIE_3_DEFAULT_FUN_NUM, FALSE }  // Itbt Pcie Rp 4
};

GLOBAL_REMOVE_IF_UNREFERENCED UINT32 mItbtPcieRpCount = sizeof (mIbtbPcieRpInfo) / sizeof (mIbtbPcieRpInfo[0]);

GLOBAL_REMOVE_IF_UNREFERENCED BOOLEAN mItbtPcieRpInfoUpdated = FALSE;

/**
  Get ITBT Pcie Root Port Segment Bus Device and Function Number by Port Index

  @param[in]  PortIndex             Root port index. (0-based)
  @param[out] Segment               Return corresponding root port segment number.
  @param[out] Bus                   Return corresponding root port bus number.
  @param[out] Device                Return corresponding root port device number.
  @param[out] Function              Return corresponding root port function number.

  @retval     EFI_SUCCESS           Root port Info is retrieved
  @retval     EFI_UNSUPPORTED       PortIndex is invalid/disabled, or TcssHob is not existed when mItbtPcieRpInfoUpdated is FALSE.
**/
EFI_STATUS
EFIAPI
GetItbtPcieRpInfo (
  IN   UINTN    PortIndex,
  OUT  UINTN    *Segment,
  OUT  UINTN    *Bus,
  OUT  UINTN    *Device,
  OUT  UINTN    *Function
  )
{
  *Segment  = 0;
  *Bus      = 0;
  *Device   = 0;
  *Function = 0;

  if(PortIndex >= MAX_ITBT_PCIE_PORT) {
    DEBUG ((DEBUG_ERROR, "Invalid PortIndex %x", PortIndex));
    return EFI_UNSUPPORTED;
  }

  if (mItbtPcieRpInfoUpdated == TRUE) {
    if (mIbtbPcieRpInfo[PortIndex].Enable == TRUE) {
      *Segment  = mIbtbPcieRpInfo[PortIndex].Segment;
      *Bus      = mIbtbPcieRpInfo[PortIndex].Bus;
      *Device   = mIbtbPcieRpInfo[PortIndex].Device;
      *Function = mIbtbPcieRpInfo[PortIndex].Function;
      return EFI_SUCCESS;
    }
  }

  return EFI_UNSUPPORTED;
}
