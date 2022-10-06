/** @file
  ACPI GNVS

  Copyright (c) 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
  //
  // Platform NVS
  //
  include ("PlatformNvs.asl")

  //
  // PCH NVS
  //
  include ("PchNvs.asl")

  //
  // BIOS parameter BIOS_ACPI_PARAM
  //
  include ("BiosParameterRegion.asi")
