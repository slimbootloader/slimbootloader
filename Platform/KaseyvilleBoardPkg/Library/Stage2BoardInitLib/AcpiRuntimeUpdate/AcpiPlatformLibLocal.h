/** @file

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _ACPI_PLATFORM__LIB_LOCAL_H_
#define _ACPI_PLATFORM__LIB_LOCAL_H_


#define OEM1_SSDT_TABLE_SIGNATURE  SIGNATURE_32('O', 'E', 'M', '1')

#define AML_NAME_OP           0x08
#define AML_NAME_DWORD_SIZE   0x0C

#ifndef AML_EXTERNAL_OP
#define AML_EXTERNAL_OP       0x15 /* ACPI 6.0 */
#endif

#pragma pack(1)

typedef struct {
  UINT8   StartByte;
  UINT32  NameStr;
  UINT8   Size;
  UINT32  Value;
} EFI_ACPI_NAMEPACK_DWORD;

typedef struct {
  UINT8   StartByte;
  UINT32  NameStr;
  UINT8   OpCode;
  UINT16  Size;                     // Hardcode to 16bit width because the table we use is fixed size
  UINT8   NumEntries;
} EFI_ACPI_NAME_COMMAND;

typedef struct {
  UINT8     NameOp;           // 08h ;First opcode is a NameOp.
  UINT32    PackageName;      // PSDC/PSDE
  UINT8     Length;
  UINT8     DwordPrefix1;
  UINT8     Revision;
  UINT8     PackageOp;
  UINT8     PackageLen;
  UINT8     PackLen;
  UINT16    WordValue1;
  UINT16    WordValue2;
  UINT8     BytePrefix2;
  UINT32    Domain;
  UINT8     BytePrefix3;
  UINT8     CoordType;        // 0xFC(SW_ALL), 0xFE(HW_ALL)
  UINT8     BytePrefix4;
  UINT32    NumProcessors;
} PSD_PACKAGE_LAYOUT;

typedef struct {
  UINT8  ExternalOp;
  UINT8  RootChar;
  UINT8  MultiNamePrefix;
  UINT8  SegCount;
  UINT32 NameStr[0];
} EXTERNAL_OBJECT_DECL;

#define ACPI_NAME_COMMAND_FROM_NAME_STR(a)  BASE_CR (a, EFI_ACPI_NAME_COMMAND, NameStr)
#define ACPI_NAME_COMMAND_FROM_NAMEPACK_STR(a)  BASE_CR (a, EFI_ACPI_NAMEPACK_DWORD, NameStr)
#define ACPI_EXTERNAL_OBJECT_DECL_FROM_NAME_STR(a) BASE_CR (a, EXTERNAL_OBJECT_DECL, NameStr)

#pragma pack()

#endif
