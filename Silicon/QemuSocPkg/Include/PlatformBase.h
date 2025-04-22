/** @file

  Copyright (c) 2016, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _PLATFORM_BASE_H_

#define PCI_TO_CF8_ADDRESS(A) \
  ((UINT32) ((((A) >> 4) & 0x00ffff00) | ((A) & 0xfc) | 0x80000000))

#define  INTEL_X58_ICH10_DEVICE_ID         0x3400  // Host Bridge DID for Simics QSP
#define  ACPI_BASE_ADDRESS                 0x400
#define  LOCAL_APIC_BASE_ADDRESS           0xFEE00000  // Local APIC
#define  IO_APIC_BASE_ADDRESS              0xFEC00000
#define  HPET_BASE_ADDRESS                 0xFED00000

#endif
