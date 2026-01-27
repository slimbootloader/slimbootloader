/** @file
  Default CSME Performance ID to String Mapping Library

  This library provides the default/common CSME performance event descriptions.
  Platform-specific implementations can override this by providing their own version.

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Library/BaseLib.h>

/**
  Provide description string for CSME performance event ID.

  This is the default implementation. Platforms can override by providing
  their own library with the same LIBRARY_CLASS.

  @param[in]  Id  CSME Performance Event ID

  @retval Description string or NULL if not found
**/
CHAR8 *
EFIAPI
CsmePerfIdToStr (
  IN UINT32 Id
  )
{
  switch (Id) {
  case 0:
    return "CSME ROM start execution";
  case 1:
    return "EC Boot Load Done (CSME ROM starts main execution)";
  case 2:
    return "CSME ROM completed execution / CSME RBE started";
  case 3:
    return "CSME got ESE Init Done indication from ESE";
  case 4:
    return "CSME RBE start PMC patch/es loading";
  case 5:
    return "CSME RBE completed PMC patch/es loading";
  case 6:
    return "CSME RBE set Boot Stall Done indication to PMC";
  case 7:
    return "CSME start poll for PMC PPS register";
  case 8:
    return "PMC set PPS";
  case 9:
    return "CSME BUP start running";
  case 10:
    return "CSME set Host Boot Prep Done indication to PMC";
  case 11:
    return "CSME starts PHYs loading";
  case 12:
    return "CSME completed PHYs loading";
  case 13:
    return "PMC indicated CSME that xxPWRGOOD was asserted";
  case 14:
    return "PMC indicated CSME that SYS_PWROK was asserted";
  case 15:
    return "PMC sent CPU_BOOT_CONFIG start message to CSME";
  case 16:
    return "CSME sent CPU_BOOT_CONFIG done message to PMC";
  case 17:
    return "PMC indicated CSME that xxPLTRST was de-asserted";
  case 18:
    return "PMC indicated CSME that TCO_S0 was asserted";
  case 19:
    return "PMC sent Core Reset Done Ack - Sent message to CSME";
  case 20:
    return "ACM Active indication - ACM started its execution";
  case 21:
    return "ACM Done indication - ACM completed execution";
  case 22:
    return "BIOS sent DRAM Init Done message";
  case 23:
    return "CSME sent DRAM Init Done message back to BIOS";
  case 24:
    return "CSME completed loading TCSS";
  case 25:
    return "CSME started loading ISH Bringup module";
  case 26:
    return "CSME completed loading ISH Bringup module";
  case 27:
    return "CSME started loading ISH Main module";
  case 28:
    return "CSME completed loading Main module";
  case 29:
    return "BIOS sent End Of Post message to CSME";
  case 30:
    return "CSME sent End Of Post ack message back to BIOS";
  case 31:
    return "BIOS sent Core BIOS Done message to CSME";
  case 32:
    return "CSME sent Core BIOS Done ack message back to BIOS";
  case 33:
    return "CSME reached Firmware Init Done";
  case 63:
    return "Timestamp when CSME responded to BupGetEarlyBootData message itself";
  default:
    return "Reserved";
  }
}
