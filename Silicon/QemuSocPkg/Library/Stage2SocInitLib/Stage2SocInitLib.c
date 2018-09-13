/** @file

  Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

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
EnableLegacyRegions (
  VOID
)
{
  PciWrite32 (PCI_LIB_ADDRESS(0, 0, 0, Q35_HOST_BRIDGE_PAM0), 0x00003333);
  PciWrite32 (PCI_LIB_ADDRESS(0, 0, 0, Q35_HOST_BRIDGE_PAM0), 0x33333330);
}
