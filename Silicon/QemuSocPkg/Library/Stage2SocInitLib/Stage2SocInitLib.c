/** @file

  Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <PiPei.h>
#include <Base.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PciLib.h>
#include <Library/PcdLib.h>
#include <RegAccess.h>
#include <GlobalNvsArea.h>

/**
  Get size of global nvs area.

  @param[in] none

  @retval    UINT32     Size of global nvs area.

**/
UINT32
EFIAPI
GetAcpiGnvsSize (
  VOID
)
{
  return sizeof(EFI_GLOBAL_NVS_AREA);
}


/**
 Update GPIO address and length to global NVS data.

 @param [in] GnvsIn Pointer to Global NVS data.
 **/
VOID
EFIAPI
SocUpdateAcpiGnvs (
  IN VOID  *GnvsIn
)
{
}

VOID
EFIAPI
EnableLegacyRegions (
  VOID
)
{
  PciWrite32 (PCI_LIB_ADDRESS(0, 0, 0, Q35_HOST_BRIDGE_PAM0), 0x00003333);
  PciWrite32 (PCI_LIB_ADDRESS(0, 0, 0, Q35_HOST_BRIDGE_PAM0), 0x33333330);
}
