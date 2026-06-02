/** @file

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "Stage2BoardInitLib.h"

#define AML_STA_PRESENT           0x01
#define AML_STA_ENABLED           0x02
#define AML_STA_VISIBLE           0x04
#define AML_STA_FUNCTIONAL        0x08
#define AML_STA_BATTERY           0x10

#define AML_NAME_DWORD_SIZE       0x0C

#define ACPI_NAME_COMMAND_FROM_NAME_STR(a)  BASE_CR (a, EFI_ACPI_NAME_COMMAND, NameStr)
#define ACPI_EXTERNAL_OBJECT_DECL_FROM_NAME_STR(a) BASE_CR (a, EXTERNAL_OBJECT_DECL, NameStr)

#define CHAR_TO_NUM(C)                      \
  ((((C) >= '0') && ((C) <= '9')) ? ((C) - '0') : ((((C) >= 'A') && ((C) <= 'F')) ? ((C) - 'A' + 10) : 0))

#define ACPI_DBG2_SERIALIO_UART0_NAME_SPACE           "\\_SB.PC00.LPC0.UAR1" // Dbg2 ACPI table namespace string

#pragma pack(1)

typedef struct {
  UINT8                             DescriptorType;
  UINT16                            ResourceLength;
} AML_RESOURCE_LARGE_HEADER_COMMON;

typedef struct {
  UINT8                             ResourceType;
  UINT8                             Flags;
  UINT8                             SpecificFlags;
} AML_RESOURCE_ADDRESS_COMMON;

typedef struct {
  AML_RESOURCE_LARGE_HEADER_COMMON  LargeHeaderCommon;
  AML_RESOURCE_ADDRESS_COMMON       AddressCommon;
  UINT64                            Granularity;
  UINT64                            Minimum;
  UINT64                            Maximum;
  UINT64                            TranslationOffset;
  UINT64                            AddressLength;
} AML_RESOURCE_ADDRESS64;

typedef struct {
  AML_RESOURCE_LARGE_HEADER_COMMON  LargeHeaderCommon;
  AML_RESOURCE_ADDRESS_COMMON       AddressCommon;
  UINT32                            Granularity;
  UINT32                            Minimum;
  UINT32                            Maximum;
  UINT32                            TranslationOffset;
  UINT32                            AddressLength;
} AML_RESOURCE_ADDRESS32;

typedef struct {
  AML_RESOURCE_LARGE_HEADER_COMMON  LargeHeaderCommon;
  AML_RESOURCE_ADDRESS_COMMON       AddressCommon;
  UINT16                            Granularity;
  UINT16                            Minimum;
  UINT16                            Maximum;
  UINT16                            TranslationOffset;
  UINT16                            AddressLength;
} AML_RESOURCE_ADDRESS16;

typedef struct {
  UINT8   StartByte;
  UINT32  NameStr;
  UINT8   OpCode;
  UINT16  Size;
  UINT8   NumEntries;
} EFI_ACPI_NAME_COMMAND;

typedef struct {
  UINT8     NameOp;
  UINT32    PackageName;
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
  UINT8     CoordType;
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

#pragma pack()

/**
  Patch DBG2 ACPI Table

  @param[in] Table            Pointer to DBG2 ACPI Table

  @retval    RETURN_SUCCESS   Operation completed successfully.

**/
RETURN_STATUS
PatchDbg2Table (
  IN  EFI_ACPI_DESCRIPTION_HEADER           *Table
  )
{
  EFI_ACPI_DEBUG_PORT_2_DESCRIPTION_TABLE       *DebugPortTable2;
  EFI_ACPI_DBG2_DEBUG_DEVICE_INFORMATION_STRUCT *Dbg2DeviceTablePtr;
  EFI_ACPI_6_2_GENERIC_ADDRESS_STRUCTURE        *Dbg2DeviceGasPtr;
  UINT32                                        *Dbg2DeviceAdrSizePtr;
  UINT8                                         *Dbg2DeviceNameSpacePtr;
  CHAR8                                         *Dbg2DeviceNameSpaceStr;
  UINT16                                        Dbg2DeviceNameSpaceStrSize;
  UINT32                                        DebugPortTable2Size;
  UINT32                                        Dbg2DevNum;
  UINT16                                        DebugDevInfoStructSize;

  DebugPortTable2 = (EFI_ACPI_DEBUG_PORT_2_DESCRIPTION_TABLE*) Table;

  //
  // Initialize the number of device for DBG2 (1 UART device)
  //
  Dbg2DevNum = 1;

  //
  // Initialize the DBG2 debug device's name space string and size
  //
  Dbg2DeviceNameSpaceStr = ACPI_DBG2_SERIALIO_UART0_NAME_SPACE;
  Dbg2DeviceNameSpaceStrSize = (UINT16) AsciiStrSize (Dbg2DeviceNameSpaceStr);

  //
  // Initialize the DBG2 total size
  //
  DebugDevInfoStructSize = (UINT16) ((sizeof (EFI_ACPI_DBG2_DEBUG_DEVICE_INFORMATION_STRUCT) +        // Debug Device Information Structure
                                      sizeof (EFI_ACPI_6_2_GENERIC_ADDRESS_STRUCTURE) +               // Base Address Register - Assume 1 BAR
                                      4 +                                                            // Address Size
                                      Dbg2DeviceNameSpaceStrSize +                                   // Name Space String
                                      0) *                                                           // Oem Data - Assume none
                                      Dbg2DevNum);
  DebugPortTable2Size = sizeof (EFI_ACPI_DEBUG_PORT_2_DESCRIPTION_TABLE) + DebugDevInfoStructSize;   // DBG2 table size

  //
  // Populate data for DBG2
  //
  DebugPortTable2->Header.Signature       = EFI_ACPI_6_2_DEBUG_PORT_2_TABLE_SIGNATURE;
  DebugPortTable2->Header.Length          = DebugPortTable2Size;
  DebugPortTable2->Header.Revision        = EFI_ACPI_DEBUG_PORT_2_TABLE_REVISION;
  DebugPortTable2->Header.Checksum        = 0x00;
  CopyMem (DebugPortTable2->Header.OemId, "INTEL ", sizeof (DebugPortTable2->Header.OemId));
  DebugPortTable2->Header.OemTableId      = 0x20202020324B4445; // INTEL ID
  DebugPortTable2->Header.OemRevision     = 0x00000002;
  DebugPortTable2->Header.CreatorId       = 0x20202020;
  DebugPortTable2->Header.CreatorRevision = 0x01000013;
  DebugPortTable2->OffsetDbgDeviceInfo    = sizeof(EFI_ACPI_DEBUG_PORT_2_DESCRIPTION_TABLE);
  DebugPortTable2->NumberDbgDeviceInfo    = Dbg2DevNum;

  //
  // Set pointer to DBG2 Device Info Structure Offset
  //
  Dbg2DeviceTablePtr = (EFI_ACPI_DBG2_DEBUG_DEVICE_INFORMATION_STRUCT *) ((UINT8 *) DebugPortTable2 + DebugPortTable2->OffsetDbgDeviceInfo);

  //
  // Populate DBG2 Device Info Structure
  //
  Dbg2DeviceTablePtr[0].Revision                        = 0x00;
  Dbg2DeviceTablePtr[0].Length                          = DebugDevInfoStructSize;
  Dbg2DeviceTablePtr[0].NumberofGenericAddressRegisters = 1;
  Dbg2DeviceTablePtr[0].NameSpaceStringLength           = Dbg2DeviceNameSpaceStrSize;
  Dbg2DeviceTablePtr[0].NameSpaceStringOffset           = sizeof (EFI_ACPI_DBG2_DEBUG_DEVICE_INFORMATION_STRUCT) + sizeof (EFI_ACPI_6_2_GENERIC_ADDRESS_STRUCTURE) + 4;
  Dbg2DeviceTablePtr[0].OemDataLength                   = 0;
  Dbg2DeviceTablePtr[0].OemDataOffset                   = 0;
  Dbg2DeviceTablePtr[0].PortType                        = EFI_ACPI_DBG2_PORT_TYPE_SERIAL;
  Dbg2DeviceTablePtr[0].PortSubtype                     = EFI_ACPI_DBG2_PORT_SUBTYPE_SERIAL_FULL_16550;
  Dbg2DeviceTablePtr[0].Reserved[0]                     = 0;
  Dbg2DeviceTablePtr[0].Reserved[1]                     = 0;
  Dbg2DeviceTablePtr[0].BaseAddressRegisterOffset       = sizeof (EFI_ACPI_DBG2_DEBUG_DEVICE_INFORMATION_STRUCT);
  Dbg2DeviceTablePtr[0].AddressSizeOffset               = sizeof (EFI_ACPI_DBG2_DEBUG_DEVICE_INFORMATION_STRUCT) + sizeof (EFI_ACPI_6_2_GENERIC_ADDRESS_STRUCTURE);

  //
  // Set pointer to DBG2 Device Base Address Offset
  //
  Dbg2DeviceGasPtr = (EFI_ACPI_6_2_GENERIC_ADDRESS_STRUCTURE *) ((UINT8 *) Dbg2DeviceTablePtr + Dbg2DeviceTablePtr[0].BaseAddressRegisterOffset);

  //
  // Set pointer to DBG2 Device Address Size Offset and populate value
  //
  Dbg2DeviceAdrSizePtr = (UINT32 *) ((UINT8 *)Dbg2DeviceTablePtr + Dbg2DeviceTablePtr[0].AddressSizeOffset);

  //
  // Array of address sizes corresponding to each generic address
  //
  Dbg2DeviceAdrSizePtr[0] = 32;

  //
  // Set pointer to DBG2 Device Name Space Offset and populate value
  //
  Dbg2DeviceNameSpacePtr = (UINT8 *) ((UINT8 *)Dbg2DeviceTablePtr + Dbg2DeviceTablePtr[0].NameSpaceStringOffset);
  CopyMem (Dbg2DeviceNameSpacePtr, Dbg2DeviceNameSpaceStr, Dbg2DeviceNameSpaceStrSize);

  Dbg2DeviceGasPtr[0].AddressSpaceId    = EFI_ACPI_6_2_SYSTEM_IO;
  Dbg2DeviceGasPtr[0].Address           = GetSerialPortBase();

  Dbg2DeviceGasPtr[0].RegisterBitWidth  = 8;
  Dbg2DeviceGasPtr[0].RegisterBitOffset = 0;
  Dbg2DeviceGasPtr[0].AccessSize        = EFI_ACPI_6_2_UNDEFINED;

  return RETURN_SUCCESS;
}

/**
  Update known format AML resource region

  @param[in] AmlResouce       Pointer to Aml Resource address
  @param[in] Minimum          Minimum value
  @param[in] Maximum          Maximum value
  @param[in] IncludeMaximum   Indicate whether Maximum must be in resource range

  @retval    Next address to be handled

**/
UINT8 *
UpdateAmlResource (
  IN UINT8   *AmlResouce,
  IN UINT64   Minimum,
  IN UINT64   Maximum,
  IN BOOLEAN  IncludeMaximum
  )
{
  AML_RESOURCE_ADDRESS16    *ResAddr16;
  AML_RESOURCE_ADDRESS32    *ResAddr32;
  AML_RESOURCE_ADDRESS64    *ResAddr64;
  UINT8                     *Current;
  UINT8                      OpCode;
  UINT8                      Offset;

  Current = AmlResouce;
  OpCode = *Current;
  Offset = IncludeMaximum ? 1 : 0;
  DEBUG ((DEBUG_VERBOSE, "OpCode 0x%02X:", OpCode));
  if (OpCode == 0x88) {
    ResAddr16 = (AML_RESOURCE_ADDRESS16 *)Current;
    DEBUG ((DEBUG_VERBOSE, "0x%04X-0x%04X(0x%X) -> ",
      ResAddr16->Minimum, ResAddr16->Maximum, ResAddr16->AddressLength));
    ResAddr16->Minimum           = (UINT16)Minimum;
    ResAddr16->Maximum           = (UINT16)Maximum;
    ResAddr16->AddressLength     = ResAddr16->Maximum - ResAddr16->Minimum + Offset;
    Current = AmlResouce + ResAddr16->LargeHeaderCommon.ResourceLength + sizeof (AML_RESOURCE_LARGE_HEADER_COMMON);
    DEBUG ((DEBUG_VERBOSE, "0x%04X-0x%04X(0x%X)\n",
      ResAddr16->Minimum, ResAddr16->Maximum, ResAddr16->AddressLength));
  } else if (OpCode == 0x87) {
    ResAddr32 = (AML_RESOURCE_ADDRESS32 *)Current;
    DEBUG ((DEBUG_VERBOSE, "0x%08X-0x%08X(0x%X) -> ",
      ResAddr32->Minimum, ResAddr32->Maximum, ResAddr32->AddressLength));
    ResAddr32->Minimum           = (UINT32)Minimum;
    ResAddr32->Maximum           = (UINT32)Maximum;
    ResAddr32->AddressLength     = ResAddr32->Maximum - ResAddr32->Minimum + Offset;
    Current = AmlResouce + ResAddr32->LargeHeaderCommon.ResourceLength + sizeof (AML_RESOURCE_LARGE_HEADER_COMMON);
    DEBUG ((DEBUG_VERBOSE, "0x%08X-0x%08X(0x%X)\n",
      ResAddr32->Minimum, ResAddr32->Maximum, ResAddr32->AddressLength));
  } else if (OpCode == 0x8A) {
    ResAddr64 = (AML_RESOURCE_ADDRESS64 *)Current;
    DEBUG ((DEBUG_VERBOSE, "0x%016lX-0x%016lX(0x%lX) -> ",
      ResAddr64->Minimum, ResAddr64->Maximum, ResAddr64->AddressLength));
    ResAddr64->Minimum           = Minimum;
    ResAddr64->Maximum           = Maximum;
    ResAddr64->AddressLength     = ResAddr64->Maximum - ResAddr64->Minimum + Offset;
    Current = AmlResouce + ResAddr64->LargeHeaderCommon.ResourceLength + sizeof (AML_RESOURCE_LARGE_HEADER_COMMON);
    DEBUG ((DEBUG_VERBOSE, "0x%016lX-0x%016lX(0x%lX)\n",
      ResAddr64->Minimum, ResAddr64->Maximum, ResAddr64->AddressLength));
  } else {
    DEBUG ((DEBUG_VERBOSE, "Unknown\n"));
  }

  return Current;
}

/**
  Patch DSDT Table

  @param[in] Table            Pointer to DSDT ACPI Table

  @retval    RETURN_SUCCESS   Operation completed successfully.

**/
RETURN_STATUS
PatchDsdtTable (
  IN  EFI_ACPI_DESCRIPTION_HEADER           *Table
  )
{
  GLOBAL_NVS_AREA             *GlobalNvs;
  UINT8                       *Ptr;
  UINT8                       *End;
  UINT32                       Base;
  UINT16                       Size;
  EFI_STATUS                   Status;

  Status = UpdateSgxAcpiSblData();
  if (EFI_ERROR(Status)) {
    DEBUG ((DEBUG_WARN, "Updating SGX Parameters Failed\n"));
  }

  GlobalNvs = (GLOBAL_NVS_AREA *)(UINTN) PcdGet32 (PcdAcpiGnvsAddress);
  DEBUG ((DEBUG_INFO, "GlobalNvs 0x%p\n", GlobalNvs));

  Ptr = (UINT8 *)Table;
  End = (UINT8 *)Table + Table->Length;
  for (; Ptr < End; Ptr++) {
    if (*(Ptr - 1) == AML_NAME_OP) {
      if (*(UINT32 *)Ptr == SIGNATURE_32 ('P','N','V','B')) {
        Base = (UINT32) (UINTN) &GlobalNvs->PchNvs;
        DEBUG ((DEBUG_INFO, "PNVB Old=0x%08X New=0x%08X\n", *(UINT32 *)(Ptr + 5), Base));
        *(UINT32 *)(Ptr + 5) = Base;
      } else if (*(UINT32 *)Ptr == SIGNATURE_32 ('P','N','V','L')) {
        Size = sizeof (PCH_NVS_AREA);
        DEBUG ((DEBUG_INFO, "PNVL Old=0x%08X New=0x%08X\n", *(UINT16 *)(Ptr + 5), Size));
        *(UINT16 *)(Ptr + 5) = Size;
      }
    }
  }

  // Patch DSDT AML code
  Status = PatchDsdtTableAML(Table, GlobalNvs);

  return Status;
}

/**
  Patch TPM2 ACPI Table

  @param[in] Table            Pointer to TPM2 ACPI Table

  @retval    RETURN_SUCCESS   Operation completed successfully.

**/
RETURN_STATUS
PatchTpm2AcpiTable (
  IN  EFI_ACPI_DESCRIPTION_HEADER           *Table
  )
{
  EFI_STATUS Status;

  Status = EFI_SUCCESS;
  if (MEASURED_BOOT_ENABLED()) {
      Status = UpdateTpm2AcpiTable(Table);
  }

  return Status;
}

STATIC
UINT8 *
SkipExternalSbOpcodes (
  IN UINT8  *CurrPtr,
  IN UINT8  *EndPtr
  )
{
  UINT8                       *Ptr;
  UINT32                      Signature;

  //
  // Skip external op
  //
  for (Ptr = CurrPtr; Ptr <= EndPtr; ++Ptr) {
    Signature = *(UINT32 *) Ptr;
    if ((Signature == SIGNATURE_32 ('_', 'S', 'B', '_'))
      && ((Ptr[4] == 'S') && (Ptr[5] == 'C') && (Ptr[6] == 'K') && (Ptr[7] == '0') && (Ptr[8] == 'C'))
      && (ACPI_EXTERNAL_OBJECT_DECL_FROM_NAME_STR(Ptr)->ExternalOp != AML_EXTERNAL_OP)) {
      CurrPtr = Ptr;
      break;
    }
  }

  if (Ptr < EndPtr) {
    return Ptr;
  } else {
    DEBUG ((DEBUG_INFO, "SkipExternalSbOpcodes - Not found!\n"));
    return NULL;
  }
}

/**
  Update the CPU PM SSDT table

  @param[in] Table            Pointer to CPU PM SSDT Table

  @retval    RETURN_SUCCESS   Operation completed successfully.

**/
RETURN_STATUS
PatchCpuPmSsdtTable (
  IN  EFI_ACPI_DESCRIPTION_HEADER           *Table
  )
{
  UINT8                       *CurrPtr;
  UINT8                       *EndPtr;
  UINT8                       *Ptr;
  EFI_ACPI_NAME_COMMAND       *PsdPackage;
  PSD_PACKAGE_LAYOUT          *PsdPackageItemPtr;
  SYS_CPU_INFO                *CpuInfo;
  MISC_CFG_DATA               *MiscCfgData;
  UINT32                       Signature;
  UINT32                       CpuSkt;
  UINT32                       ThreadIndex;
  UINT8                        ThreadCount;
  UINT8                        HyperThreading;
  UINT32                       DomnValue;
  UINT32                       NcpuValue;

  CurrPtr = (UINT8 *)Table + sizeof (EFI_ACPI_DESCRIPTION_HEADER);
  EndPtr  = (UINT8 *)Table + Table->Length;

  CurrPtr = SkipExternalSbOpcodes (CurrPtr, EndPtr);
  if (CurrPtr == NULL) {
    return EFI_UNSUPPORTED;
  }
  DEBUG ((DEBUG_VERBOSE, "PatchCpuPmSsdtTable - T:0x%p C:0x%p E:0x%p\n", Table, CurrPtr, EndPtr));

  CpuInfo = MpGetInfo ();
  ThreadCount = (CpuInfo != NULL) ? (UINT8)CpuInfo->CpuCount : 1;
  MiscCfgData = (MISC_CFG_DATA *)FindConfigDataByTag (CDATA_MISC_TAG);
  HyperThreading  = (MiscCfgData != NULL) ? (UINT8)MiscCfgData->MiscCfg.HyperThreading : 0;

  CpuSkt = 0;
  ThreadIndex = 0;
  DomnValue = 0;
  NcpuValue = 0;
  for (Ptr = CurrPtr; Ptr <= EndPtr; ++Ptr) {
    Signature = *(UINT32 *) Ptr;
    if ((Signature == SIGNATURE_32 ('_', 'S', 'B', '_'))
      && ((Ptr[4] == 'S') && (Ptr[5] == 'C') && (Ptr[6] == 'K') && (Ptr[8] == 'C'))) {
      CpuSkt = CHAR_TO_NUM (Ptr[7]);
      ASSERT (CpuSkt < MAX_SOCKET);
      if (CpuSkt >= MAX_SOCKET) {
        continue;
      }

      ThreadIndex  = CHAR_TO_NUM (Ptr[9]) * 0x100;
      ThreadIndex += CHAR_TO_NUM (Ptr[10]) * 0x10;
      ThreadIndex += CHAR_TO_NUM (Ptr[11]);
      if (ThreadIndex < ThreadCount) {
        DomnValue = CpuInfo->CpuInfo[ThreadIndex].ApicId;
        if (HyperThreading != 0) {
          DomnValue = (DomnValue >> 1);
        }
        NcpuValue = (HyperThreading != 0) ? 2 : 1;
        DEBUG ((DEBUG_VERBOSE, "CpuSkt:%d ThreadIndex:%d DomnValue:%d NcpuValue:%d\n",
          CpuSkt, ThreadIndex, DomnValue, NcpuValue));
      }
    } else if ((Signature == SIGNATURE_32 ('P', 'S', 'D', 'C')) || (Signature == SIGNATURE_32 ('P', 'S', 'D', 'E'))) {
      PsdPackage = ACPI_NAME_COMMAND_FROM_NAME_STR (Ptr);
      if ((CpuSkt >= MAX_SOCKET) || (ThreadIndex >= ThreadCount)) {
        continue;
      }
      if (PsdPackage->StartByte != AML_NAME_OP) {
        continue;
      }
      PsdPackageItemPtr = (PSD_PACKAGE_LAYOUT *) ((UINT8 *) PsdPackage);
      PsdPackageItemPtr->Domain = DomnValue;
      PsdPackageItemPtr->NumProcessors = NcpuValue;
      DEBUG ((DEBUG_VERBOSE, "  PSD Updated Values: Domain = %x, CoordType = %x, NumProcessors = %x\n", \
        PsdPackageItemPtr->Domain, PsdPackageItemPtr->CoordType, PsdPackageItemPtr->NumProcessors));
    }
  }
  return EFI_SUCCESS;
}

/**
  Calculate relative power

  @param[in]  BaseRatio     Maximum bus ratio
  @param[in]  CurrRatio     Current bus ratio to get relative power
  @param[in]  TdpMilliWatt  Maximum power in mW

  @retval                   Calculated power value in mW

**/
STATIC
UINT32
CalculateRelativePower (
  IN  UINT16  BaseRatio,
  IN  UINT16  CurrRatio,
  IN  UINT32  TdpMilliWatt
  )
{
  UINT64  Power1;
  UINT64  Power2;
  UINT32  BasePower;

  ASSERT (CurrRatio <= BaseRatio);

  BasePower = (TdpMilliWatt / 1000);

  Power1 = (110000 - ((BaseRatio - CurrRatio) * 625));
  Power1 = DivU64x32 (Power1, 11);
  Power1 = MultU64x64 (Power1, Power1);

  Power2 = ((CurrRatio * 100) / BaseRatio);
  Power2 = DivU64x32 (MultU64x32 (MultU64x64 (Power2, Power1), BasePower), 10000000);

  return (UINT32)Power2;
}

/**
  Update the OEM1 P-State SSDT table (EIST)

  @param[in] Table            Pointer to CPU EIST SSDT Table

  @retval    RETURN_SUCCESS   Operation completed successfully.

**/
RETURN_STATUS
PatchOem1SsdtTable (
  IN  EFI_ACPI_DESCRIPTION_HEADER           *Table
  )
{
  UINT8                       *CurrPtr;
  UINT8                       *EndPtr;
  UINT8                       *Ptr;
  EFI_ACPI_NAME_COMMAND       *PssTable;
  SYS_CPU_INFO                *CpuInfo;
  UINT32                      Signature;
  UINT32                      CpuSkt;
  UINT32                      ThreadIndex;
  UINT8                       ThreadCount;
  UINT16                      PackageTdp;
  UINT32                      PackageTdpWatt;
  UINT8                       ProcessorPowerUnit;
  MSR_REGISTER                MsrValue;
  PSS_PARAMS                  PssParams;

  CurrPtr = (UINT8 *)Table + sizeof (EFI_ACPI_DESCRIPTION_HEADER);
  EndPtr  = (UINT8 *)Table + Table->Length;

  CurrPtr = SkipExternalSbOpcodes (CurrPtr, EndPtr);
  if (CurrPtr == NULL) {
    return EFI_UNSUPPORTED;
  }
  DEBUG ((DEBUG_VERBOSE, "PatchOem1SsdtTable T:0x%p C:0x%p E:0x%p\n", Table, CurrPtr, EndPtr));

  MsrValue.Qword = AsmReadMsr64 (MSR_PLATFORM_INFO);
  PssParams.MaxBusRatio = MsrValue.Bytes.SecondByte;
  PssParams.MinBusRatio = MsrValue.Bytes.SixthByte;

  MsrValue.Qword = AsmReadMsr64 (MSR_TURBO_RATIO_LIMIT);
  PssParams.TurboBusRatio = (UINT8)(MsrValue.Dwords.Low & 0xFF);
  if (PssParams.TurboBusRatio == 0) {
    PssParams.TurboBusRatio = PssParams.MaxBusRatio + 4;
  }

  MsrValue.Qword = AsmReadMsr64 (MSR_PACKAGE_POWER_SKU_UNIT);
  ProcessorPowerUnit = (UINT8)(MsrValue.Bytes.FirstByte & 0xF);
  if (ProcessorPowerUnit == 0) {
    ProcessorPowerUnit = 1;
  } else {
    ProcessorPowerUnit = (UINT8) LShiftU64 (2, (ProcessorPowerUnit - 1));
  }

  MsrValue.Qword = AsmReadMsr64 (MSR_PACKAGE_POWER_LIMIT);
  PackageTdp = (UINT16)(MsrValue.Dwords.Low & 0x7FFF);
  PackageTdpWatt = (UINT32)DivU64x32 (PackageTdp, ProcessorPowerUnit);

  PssParams.PackageMaxPower = (PackageTdpWatt * 1000);
  PssParams.PackageMinPower = CalculateRelativePower (PssParams.MaxBusRatio,
                                                      PssParams.MinBusRatio,
                                                      PssParams.PackageMaxPower);
  PssParams.GetRelativePower = CalculateRelativePower;
  PssParams.DoListAll = FALSE;

  DEBUG ((DEBUG_VERBOSE, "TurboBusRatio %d, MaxBusRatio %d, MinBusRatio %d, PackageMaxPower %d, PackageMinPower %d\n",
    PssParams.TurboBusRatio, PssParams.MaxBusRatio, PssParams.MinBusRatio, PssParams.PackageMaxPower, PssParams.PackageMinPower));

  CpuInfo = MpGetInfo ();
  ThreadCount = (CpuInfo != NULL) ? (UINT8)CpuInfo->CpuCount : 1;
  CpuSkt = 0;
  ThreadIndex = 0;
  for (Ptr = CurrPtr; Ptr <= EndPtr; ++Ptr) {
    Signature = *(UINT32 *) Ptr;
    if ((Signature == SIGNATURE_32 ('_', 'S', 'B', '_'))
      && ((Ptr[4] == 'S') && (Ptr[5] == 'C') && (Ptr[6] == 'K') && (Ptr[8] == 'C'))) {
      CpuSkt = CHAR_TO_NUM (Ptr[7]);
      ASSERT (CpuSkt < MAX_SOCKET);
      if (CpuSkt >= MAX_SOCKET) {
        break;
      }

      ThreadIndex  = CHAR_TO_NUM (Ptr[9]) * 0x100;
      ThreadIndex += CHAR_TO_NUM (Ptr[10]) * 0x10;
      ThreadIndex += CHAR_TO_NUM (Ptr[11]);
      if (ThreadIndex >= ThreadCount) {
        break;
      }
    } else if (Signature == SIGNATURE_32 ('N', 'P', 'S', 'S')) {
      PssTable = ACPI_NAME_COMMAND_FROM_NAME_STR (Ptr);
      if (PssTable->StartByte != AML_NAME_OP) {
        continue;
      }
      PssParams.DoListAll = FALSE;
      AcpiPatchPssTable (Ptr, &PssParams);
      DEBUG ((DEBUG_VERBOSE, "NPSS - CpuSkt:%d ThreadIndex:%d\n", CpuSkt, ThreadIndex));
    } else if (Signature == SIGNATURE_32 ('G', 'P', 'S', 'S')) {
      PssTable = ACPI_NAME_COMMAND_FROM_NAME_STR (Ptr);
      if (PssTable->StartByte != AML_NAME_OP) {
        continue;
      }
      DEBUG ((DEBUG_VERBOSE, "GPSS - CpuSkt:%d ThreadIndex:%d\n", CpuSkt, ThreadIndex));
      PssParams.DoListAll = TRUE;
      AcpiPatchPssTable (Ptr, &PssParams);
    }
  }

  return EFI_SUCCESS;
}
