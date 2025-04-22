/** @file
  This file contains a structure definition for the
  Debug Port Table 2 (DBG2).

  Copyright (c) 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _DBG2_H_
#define _DBG2_H_

#include <IndustryStandard/Acpi.h>

//
// DBG2 Revision (defined in spec)
//
#define EFI_ACPI_DEBUG_PORT_2_TABLE_REVISION      0x00

#pragma pack(1)

#define EFI_ACPI_DBG2_DEBUG_DEVICE_INFORMATION_STRUCT_REVISION      0x00

#define   EFI_ACPI_DBG2_PORT_TYPE_SERIAL                                               0x8000
#define   EFI_ACPI_DBG2_PORT_SUBTYPE_SERIAL_FULL_16550                                 0x0000
#define   EFI_ACPI_DBG2_PORT_SUBTYPE_SERIAL_16550_SUBSET_COMPATIBLE_WITH_MS_DBGP_SPEC  0x0001
#define   EFI_ACPI_DBG2_PORT_SUBTYPE_SERIAL_ARM_PL011_UART                             0x0003
#define   EFI_ACPI_DBG2_PORT_SUBTYPE_SERIAL_ARM_SBSA_GENERIC_UART_2X                   0x000d
#define   EFI_ACPI_DBG2_PORT_SUBTYPE_SERIAL_ARM_SBSA_GENERIC_UART                      0x000e
#define   EFI_ACPI_DBG2_PORT_SUBTYPE_SERIAL_DCC                                        0x000f
#define   EFI_ACPI_DBG2_PORT_SUBTYPE_SERIAL_BCM2835_UART                               0x0010
#define   EFI_ACPI_DBG2_PORT_TYPE_1394                                                 0x8001
#define   EFI_ACPI_DBG2_PORT_SUBTYPE_1394_STANDARD                                     0x0000
#define   EFI_ACPI_DBG2_PORT_TYPE_USB                                                  0x8002
#define   EFI_ACPI_DBG2_PORT_SUBTYPE_USB_XHCI                                          0x0000
#define   EFI_ACPI_DBG2_PORT_SUBTYPE_USB_EHCI                                          0x0001
#define   EFI_ACPI_DBG2_PORT_TYPE_NET                                                  0x8003

//
// Debug Device Information structure.
//
typedef struct {
  UINT8                                     Revision;
  UINT16                                    Length;
  UINT8                                     NumberofGenericAddressRegisters;
  UINT16                                    NameSpaceStringLength;
  UINT16                                    NameSpaceStringOffset;
  UINT16                                    OemDataLength;
  UINT16                                    OemDataOffset;
  UINT16                                    PortType;
  UINT16                                    PortSubtype;
  UINT8                                     Reserved[2];
  UINT16                                    BaseAddressRegisterOffset;
  UINT16                                    AddressSizeOffset;
} EFI_ACPI_DBG2_DEBUG_DEVICE_INFORMATION_STRUCT;

//
// Debug Port 2 Table definition.
//
typedef struct {
  EFI_ACPI_DESCRIPTION_HEADER               Header;
  UINT32                                    OffsetDbgDeviceInfo;
  UINT32                                    NumberDbgDeviceInfo;
} EFI_ACPI_DEBUG_PORT_2_DESCRIPTION_TABLE;

typedef struct {
  EFI_ACPI_DEBUG_PORT_2_DESCRIPTION_TABLE       Table;
  EFI_ACPI_DBG2_DEBUG_DEVICE_INFORMATION_STRUCT Entry;
  EFI_ACPI_6_2_GENERIC_ADDRESS_STRUCTURE        BaseAdressRegister;
  UINT32                                        AddressSize;
  CHAR8                                         NamespaceString[2];
} DBG2_DEBUG_TABLE;

#pragma pack()

#endif  //_DBG2_H_
