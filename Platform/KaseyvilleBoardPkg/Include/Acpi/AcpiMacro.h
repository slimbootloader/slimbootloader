/** @file
  Definitions shared with ACPI Source Language (ASL) files.

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _ACPIMACRO_H_
#define _ACPIMACRO_H_

#define AcpiTrace(msg)                      Store(msg, Debug)

#define _STR(s)                             #s
#define STR(s)                              _STR(s)

/**
 * The macros below create names for PCI root devices in ASL code. Each device contains two letters
 * of device type ID then socket and instance (PCIe instance, IO Accelerator instance, etc).
 * 'PC00' must be the first PCI root device to integrate properly with legacy PCH devices.
 * The names defined here must be in sync with AcpiPciDevPathParse() in AcpiPlatformLibDsdt.c
 **/
#define IIO_DEVICE_NAME(str, skt, stk)      CONCATENATE3(str, skt, stk) // Obsolete, to be replaced with macros below
#define ACPI_CXL1_DEVNAME(skt, pe)          CONCATENATE3(C1, skt, pe)
#define ACPI_UBOX_DEVNAME(skt, ubi)         CONCATENATE3(UB, skt, ubi)
#define ACPI_TIP_DEVNAME(skt, ioat)         CONCATENATE3(TI, skt, ioat)
#define ACPI_TIP1_DEVNAME(skt, ioat)        CONCATENATE3(TJ, skt, ioat)

#define ACPI_CPM0_DEVNAME(skt, ioat)        CONCATENATE3(CP, skt, ioat)
#define ACPI_CPM1_DEVNAME(skt, ioat)        CONCATENATE3(BP, skt, ioat)
#define ACPI_HQM_DEVNAME(skt, ioat)         CONCATENATE3(LQ, skt, ioat)
#define ACPI_HQM0_DEVNAME(skt, ioat)        CONCATENATE3(HQ, skt, ioat)
#define ACPI_HQM1_DEVNAME(skt, ioat)        CONCATENATE3(BQ, skt, ioat)

#define ACPI_IOAT_DEVNAME(skt, ioat)      CONCATENATE3(PC, skt, ioat)
#define ACPI_PCIE_DEVNAME(skt, pe)        CONCATENATE3(CC, skt, pe)
#define DINO_UID                            0
#define PC_UID                              1

#define C1_UID                              2
#define C2_UID                              3
#define UB_UID                              4
#define CPM0_UID                            5
#define HQM0_UID                            6
#define CPM1_UID                            7
#define TIP_UID                             9

#define IOAT_ROOT                           0
#define ACC_ROOT_A                          1
#define ACC_ROOT_B                          2
#define ACC_ROOT_C                          3
#define ACC_ROOT_D                          4

#define IIO_DEVICE_UID_ASL(str, skt, stk)   S_NUMBER(CONCATENATE3(skt, stk, str))                       // In case of modification one of
#define IIO_DEVICE_UID_C(str, skt, stk)     (UINT32)((skt & 0xF) << 8 | (stk & 0xF) << 4 | (str & 0xF)) // these also change another one

#define IIO_DEVICE_UID(str, skt, stk)       STR(CONCATENATE3(str, skt, stk))


#define S_NUMBER(n)                         CONCATENATE2(0x, n)

//
//  EISA ID Macro
//  EISA ID Definition 32-bits
//   bits[15:0] - three character compressed ASCII EISA ID.
//   bits[31:16] - binary number
//    Compressed ASCII is 5 bits per character 0b00001 = 'A' 0b11010 = 'Z'
//
#define ACPI_EISA_ID_CONST         0x8E09
#define EISA_ACPI_ID(_PNPId)       (EISA_ID(ACPI_EISA_ID_CONST, (_PNPId)))

/**
 * PCIROOT types defined here are used when creating PCI hierarchy roots for a stack in UDS and shared in UEFI and ACPI.
 */
#define PCIROOT_CXL1P1  1 // Root + CXL 1.1
#define PCIROOT_TIP     1 // Root + TIP
#define PCIROOT_HQM     1 // Root + HQM
#define PCIROOT_CPM0    1 // Root + CPM0
#define PCIROOT_HQM0    2 // Root + CPM0 + HQM0
#define PCIROOT_CPM1    3 // Root + CPM0 + HQM0 + CPM1
#define PCIROOT_HQM1    4 // Root + CPM0 + HQM0 + CPM1 + HQM1
#define PCIROOT_TIP1    4 // Root + CPM0 + HQM0 + CPM1 + TIP


#define ROOTBRIDGE_NOT_PRESENT 0
#define ROOTBRIDGE_PCIE        1
#define ROOTBRIDGE_CXL20       2

#endif // _ACPIMACRO_H_
