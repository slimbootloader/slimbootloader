/** @file

  Copyright (c) 2020 - 2021, Intel Corporation. All rights reserved.<BR>
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
  UINT8                       *Current;
  SYS_CPU_INFO                *CpuInfo;
  UINT8                        CpuCount;
  UINT8                        Index;
  UINT32                       Base;
  UINT16                       Size;
  TYPE_CHOPTYPE_ENUM           ChopType;

  ChopType = GetChopType (0);
  DEBUG ((DEBUG_INFO, "PatchDsdtTable: ChopType %d\n", ChopType));
  if ((ChopType != TypeLcc) && (ChopType != TypeHcc)) {
    DEBUG ((DEBUG_ERROR, "Invalid ChopType-%d detected! Use TypeLcc.\n", ChopType));
    ChopType = TypeLcc;
  }

  CpuInfo  = MpGetInfo ();
  CpuCount = (CpuInfo != NULL) ? (UINT8)CpuInfo->CpuCount : 1;

  GlobalNvs = (GLOBAL_NVS_AREA *)(UINTN) PcdGet32 (PcdAcpiGnvsAddress);
  DEBUG ((DEBUG_INFO, "GlobalNvsGlobalNvs 0x%p\n", GlobalNvs));

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
      } else if (*(UINT32 *)Ptr == SIGNATURE_32 ('B','N','V','B')) {
        Base = (UINT32) (UINTN) &GlobalNvs->BiosAcpiParam;
        DEBUG ((DEBUG_INFO, "BNVB Old=0x%08X New=0x%08X\n", *(UINT32 *)(Ptr + 5), Base));
        *(UINT32 *)(Ptr + 5) = Base;
      } else if (*(UINT32 *)Ptr == SIGNATURE_32 ('B','N','V','L')) {
        Size = sizeof (BIOS_ACPI_PARAM);
        DEBUG ((DEBUG_INFO, "BNVL Old=0x%08X New=0x%08X\n", *(UINT16 *)(Ptr + 5), Size));
        *(UINT16 *)(Ptr + 5) = Size;
      } else if ((Ptr[0] == 'A') && (Ptr[1] == 'P') && (Ptr[2] == 'T') && (Ptr[3] == '0')) {
        if (CpuInfo == NULL) {
          continue;
        }

        Current = Ptr + 9;
        DEBUG ((DEBUG_VERBOSE, "APT0\n"));
        for (Index = 0; Index < CpuCount; Index++) {
          DEBUG ((DEBUG_VERBOSE, " %d: 0x%02X ->", Index, Current[Index]));
          Current[Index] = (UINT8)CpuInfo->CpuInfo[Index].ApicId;
          DEBUG ((DEBUG_VERBOSE, " 0x%02X\n", Current[Index]));
        }
      }
    }
  }

  return RETURN_SUCCESS;
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

/**
  Patch RTCT ACPI Table

  @param[in] Table            Pointer to RTCT ACPI Table

  @retval    RETURN_SUCCESS   Operation completed successfully.
  @retval    EFI_UNSUPPORTED  Returned Unsupported

**/
RETURN_STATUS
PatchRtctAcpiTable (
  IN  EFI_ACPI_DESCRIPTION_HEADER           *Table
  )
{
  EFI_STATUS                   Status;
  TCC_CFG_DATA                *TccCfgData;

  if ((FeaturePcdGet (PcdTccEnabled)) && (GetBootMode() != BOOT_ON_FLASH_UPDATE)) {
    TccCfgData = (TCC_CFG_DATA *) FindConfigDataByTag(CDATA_TCC_TAG);
    if ((TccCfgData != NULL) && (TccCfgData->TccEnable != 0)) {
      Status = UpdateAcpiRtctTable(Table);
      DEBUG ( (DEBUG_INFO, "Updated ACPI RTCT Table : %r\n", Status) );
      return Status;
    }
  }
  return EFI_UNSUPPORTED;
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
  MiscCfgData = (MISC_CFG_DATA *)FindConfigDataByTag (CDATA_MEMORY_TAG);
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
UINT32
EFIAPI
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

/**
  Update the SPS NM SSDT table

  @param[in] Table            Pointer to SPS NM SSDT Table

  @retval    RETURN_SUCCESS   Operation completed successfully.

**/
RETURN_STATUS
PatchSpsNmSsdtTable (
  IN  EFI_ACPI_DESCRIPTION_HEADER           *Table
  )
{
  UINT8                       *CurrPtr;
  UINT8                       *EndPtr;
  UINT8                       *Ptr;
  SYS_CPU_INFO                *CpuInfo;
  UINT32                      Signature;
  UINT16                      ThreadCount;
  UINTN                       Base;

  CurrPtr = (UINT8 *)Table + sizeof (EFI_ACPI_DESCRIPTION_HEADER);
  EndPtr  = (UINT8 *)Table + Table->Length;
  DEBUG ((DEBUG_VERBOSE, "PatchSpsNmSsdtTable - T:0x%p C:0x%p E:0x%p\n", Table, CurrPtr, EndPtr));

  CpuInfo = MpGetInfo ();
  ThreadCount = (CpuInfo != NULL) ? (UINT16)CpuInfo->CpuCount : 1;

  for (Ptr = CurrPtr; Ptr <= EndPtr; ++Ptr) {
    Signature = *(UINT32 *) Ptr;

    if ((Signature == SIGNATURE_32('H', '2', 'S', 'T')) &&
        (Ptr[4] == AML_BYTE_PREFIX)) {
      DEBUG ((DEBUG_VERBOSE, "[SPS] Updating 'H2ST' 0x%02X", *(UINT8 *)&Ptr[5]));
      *(UINT8 *)&Ptr[5] = AML_STA_PRESENT | AML_STA_ENABLED | AML_STA_FUNCTIONAL;
      DEBUG ((DEBUG_VERBOSE, " -> 0x%02X\n", *(UINT8 *)&Ptr[5]));
    } else if ((Signature == SIGNATURE_32('T', 'H', 'N', 'U')) &&
               (Ptr[4] == AML_WORD_PREFIX)) {
      DEBUG ((DEBUG_VERBOSE, "[SPS] Updating 'THNU' %d", *(UINT16 *)&Ptr[5]));
      *(UINT16 *)&Ptr[5] = ThreadCount;
      DEBUG ((DEBUG_VERBOSE, " -> %d\n", *(UINT16 *)&Ptr[5]));
    } else if ((Signature == SIGNATURE_32('H', '2', 'C', 'S')) &&
               (Ptr[4] == EFI_ACPI_5_0_SYSTEM_MEMORY) &&
               (Ptr[5] == AML_QWORD_PREFIX)) {
      DEBUG ((DEBUG_VERBOSE, "[SPS] Updating 'H2CR' address 0x%16lX", *(UINT32 *)&Ptr[10], *(UINT32 *)&Ptr[6]));
      Base = MM_PCI_ADDRESS (ME_BUS, ME_DEVICE_NUMBER, HECI2_FUNCTION_NUMBER, 0x0);
      *(UINT64 *)&Ptr[6] = (MmioRead32 (Base + 0x14) | (MmioRead32 (Base + 0x10) & (UINT32)~0xF));
      DEBUG ((DEBUG_VERBOSE, " -> 0x%16lX\n", *(UINT64 *)&Ptr[6]));
    }
  }

  return EFI_SUCCESS;
}
