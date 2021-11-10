/** @file
  This code provides a initialization of intel VT-d (Virtualization Technology for Directed I/O).

  Copyright (c) 2017 - 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Base.h>
#include <Uefi/UefiBaseType.h>
#include <Library/IoLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/VtdLib.h>
#include <RegAccess.h>
#include <Library/DmaRemappingTable.h>
#include <Library/PciLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseLib.h>
#include <Library/ScSbiAccessLib.h>
#include <Library/BootloaderCommonLib.h>

/**
  For device that specified by Device Num and Function Num,
  mDevEnMap is used to check device presence.
  0x80 means use Device ID to determine presence

  The structure is used to check if device scope is valid when update DMAR table
**/
UINT16  mDevEnMap[][2] = {{0x0200, 0x80}, {0x1500, 0x80}, {0x1501, 0x80}};

/**
  Get the corresponding device Enable/Disable bit according DevNum and FunNum

  @param[in] DevNum           - Device Number
  @param[in] FunNum           - Function Number

  @retval Bit                 - If the device is found, return Disable/Enable bit in FD/DevEn register
  @retval 0xFF                - When device not found
**/
UINT16
GetFunDisableBit (
  IN UINT8    DevNum,
  IN UINT8    FunNum
  )
{
  UINTN Index;

  for (Index = 0; Index < sizeof (mDevEnMap) / 4; Index++) {
    if (mDevEnMap[Index][0] == ((DevNum << 0x08) | FunNum)) {
      return mDevEnMap[Index][1];
    }
  }

  return 0xFF;
}

/**
  Update the DRHD structure

  @param[in, out] DrhdEnginePtr       - A pointer to DRHD structure
**/
VOID
UpdateDrhd (
  IN OUT VOID    *DrhdEnginePtr
  )
{
  UINT16                        Length;
  UINT16                        DisableBit;
  BOOLEAN                       NeedRemove;
  EFI_ACPI_DRHD_ENGINE1_STRUCT  *DrhdEngine;
  UINTN                         PciAddress;

  //
  // Convert DrhdEnginePtr to EFI_ACPI_DRHD_ENGINE1_STRUCT Pointer
  //
  DrhdEngine      = (EFI_ACPI_DRHD_ENGINE1_STRUCT *) DrhdEnginePtr;
  Length          = DrhdEngine->Length;

  DisableBit = GetFunDisableBit (
                DrhdEngine->DeviceScope[0].PciPath[0],
                DrhdEngine->DeviceScope[0].PciPath[1]
                );

  NeedRemove = FALSE;
  PciAddress = MM_PCI_ADDRESS (0, DrhdEngine->DeviceScope[0].PciPath[0], DrhdEngine->DeviceScope[0].PciPath[1], 0);
  if ((DisableBit == 0xFF)                   ||
      (DrhdEngine->RegisterBaseAddress == 0) ||
      ((DisableBit == 0x80)                  &&
      (MmioRead32 (PciAddress + 0x00) == 0xFFFFFFFF))
        ) {
      NeedRemove = TRUE;
  }

  if (NeedRemove) {
    Length -= sizeof (EFI_ACPI_DEV_SCOPE_STRUCTURE);
  }

  //
  // If no device scope is left, we set the structure length as 0x00
  //
  if ((Length > EFI_ACPI_DRHD_ENGINE_HEADER_LENGTH) || (DrhdEngine->Flags == 0x01)) {
    DrhdEngine->Length = Length;
  } else {
    DrhdEngine->Length = 0;
  }
  return;
}

/**
  Get IOAPIC ID from LPC

  @retval UINT8               - IO APIC ID
**/
UINT8
GetIoApicId (
  VOID
  )
{
  UINT32 IoApicId;

  ///
  /// Get current IO APIC ID
  ///
  MmioWrite8 ((UINTN) IO_APIC_INDEX_REGISTER, 0x00);
  IoApicId = MmioRead32 ((UINTN) IO_APIC_DATA_REGISTER) >> 24;

  return (UINT8)IoApicId;
}

/**
  Update the second DRHD structure

  @param[in, out] DrhdEnginePtr       - A pointer to DRHD structure
**/
VOID
UpdateDrhd2 (
  IN OUT VOID *DrhdEnginePtr
  )
{
  UINT16                        Length;
  UINTN                         DeviceScopeNum;
  UINTN                         ValidDeviceScopeNum;
  UINT16                        Data16;
  UINT16                        Index;
  UINT8                         Bus;
  UINT8                         Path[2] = { 0, 0 };
  BOOLEAN                       NeedRemove;
  EFI_ACPI_DRHD_ENGINE2_STRUCT  *DrhdEngine;
  UINTN                         P2sbMmbase;
  BOOLEAN                       P2sbOrgStatus;

  P2sbOrgStatus = FALSE;
  P2sbMmbase    = MM_PCI_ADDRESS (0, PCI_DEVICE_NUMBER_P2SB, PCI_FUNCTION_NUMBER_P2SB, 0);
  PchRevealP2sb (P2sbMmbase, &P2sbOrgStatus);

  //
  // Convert DrhdEnginePtr to EFI_ACPI_DRHD_ENGINE2_STRUCT Pointer
  //
  DrhdEngine      = (EFI_ACPI_DRHD_ENGINE2_STRUCT *) DrhdEnginePtr;

  Length          = DrhdEngine->Length;
  DeviceScopeNum  = (DrhdEngine->Length - EFI_ACPI_DRHD_ENGINE_HEADER_LENGTH) / sizeof (EFI_ACPI_DEV_SCOPE_STRUCTURE);
  Data16          = 0;
  Bus             = 0;
  ValidDeviceScopeNum = 0;

  for (Index = 0; Index < DeviceScopeNum; Index++) {
    NeedRemove = FALSE;
    /**
      For HPET and APIC, update device scope if Interrupt remapping is supported. remove device scope
      if interrupt remapping is not supported.
      - Index = 0 - IOAPIC
      - Index = 1 - HPET
    **/

    if (Index == 0) {
      //
      // Update source id for IoApic's device scope entry
      //
      Data16 = MmioRead16 (P2sbMmbase + R_PCH_P2SB_IBDF);
      DEBUG ((DEBUG_INFO, " VtdLib: P2sbMmbase = 0x%X, IBDF = 0x%X\n", P2sbMmbase, Data16));
      Bus     = (UINT8) (Data16 >> 8);
      Path[0] = (UINT8) ((Data16 & 0xff) >> 3);
      Path[1] = (UINT8) (Data16 & 0x7);

      DrhdEngine->DeviceScope[Index].StartBusNumber = Bus;
      DrhdEngine->DeviceScope[Index].PciPath[0] = Path[0];
      DrhdEngine->DeviceScope[Index].PciPath[1] = Path[1];

      //
      // Update APIC ID
      //
      DrhdEngine->DeviceScope[Index].EnumId = GetIoApicId();
      DEBUG ((DEBUG_INFO, " VtdLib: IoApicId 0x%x\n", GetIoApicId()));
    }

    if (Index == 1) {
      //
      // Update source id for HPET's device scope entry
      //
      Data16 = MmioRead16 (P2sbMmbase + R_PCH_P2SB_HBDF);
      Bus     = (UINT8) (Data16 >> 8);
      Path[0] = (UINT8) ((Data16 & 0xFF) >> 3);
      Path[1] = (UINT8) (Data16 & 0x7);

      DrhdEngine->DeviceScope[Index].StartBusNumber = Bus;
      DrhdEngine->DeviceScope[Index].PciPath[0] = Path[0];
      DrhdEngine->DeviceScope[Index].PciPath[1] = Path[1];
    }

    CopyMem (
      &DrhdEngine->DeviceScope[ValidDeviceScopeNum],
      &DrhdEngine->DeviceScope[Index],
      sizeof (EFI_ACPI_DEV_SCOPE_STRUCTURE)
      );

    if (NeedRemove) {
      Length -= sizeof (EFI_ACPI_DEV_SCOPE_STRUCTURE);
    } else {
      ValidDeviceScopeNum++;
    }
  }

  //
  // If no device scope is left, we set the structure length as 0x00
  //
  if ((Length > EFI_ACPI_DRHD_ENGINE_HEADER_LENGTH) || (DrhdEngine->Flags == 0x01)) {
    DrhdEngine->Length = Length;
  } else {
    DrhdEngine->Length = 0;
  }

  if (!P2sbOrgStatus) {
    PchHideP2sb (P2sbMmbase);
  }
}

/**
  Update the RMRR structure

  @param[in, out] RmrrPtr             - A pointer to RMRR structure
**/
VOID
UpdateRmrr (
  IN OUT VOID    *RmrrPtr
  )
{
  UINT16                    Length;
  UINT16                    DisableBit;
  UINTN                     DeviceScopeNum;
  UINTN                     ValidDeviceScopeNum;
  UINTN                     Index;
  BOOLEAN                   NeedRemove;
  EFI_ACPI_RMRR_HECI_STRUC  *Rmrr;
  UINTN                     PciAddress;

  //
  // To make sure all device scope can be checked,
  // we convert the RmrrPtr to EFI_ACPI_RMRR_HECI_STRUC pointer
  //
  Rmrr                = (EFI_ACPI_RMRR_HECI_STRUC *) RmrrPtr;

  Length              = Rmrr->Length;
  ValidDeviceScopeNum = 0;
  DeviceScopeNum      = (Rmrr->Length - EFI_ACPI_RMRR_HEADER_LENGTH) / sizeof (EFI_ACPI_DEV_SCOPE_STRUCTURE);

  for (Index = 0; Index < DeviceScopeNum; Index++) {
    DisableBit = GetFunDisableBit (
                  Rmrr->DeviceScope[Index].PciPath[0],
                  Rmrr->DeviceScope[Index].PciPath[1]
                  );
    NeedRemove = FALSE;

    PciAddress = MM_PCI_ADDRESS (0, Rmrr->DeviceScope[Index].PciPath[0], Rmrr->DeviceScope[Index].PciPath[1], 0);
    if ((DisableBit == 0xFF) ||
        ((DisableBit == 0x80) &&
        (MmioRead32 (PciAddress + 0x00) == 0xFFFFFFFF))
        ){
      NeedRemove = TRUE;
    }

    CopyMem (
      &Rmrr->DeviceScope[ValidDeviceScopeNum],
      &Rmrr->DeviceScope[Index],
      sizeof (EFI_ACPI_DEV_SCOPE_STRUCTURE)
      );

    if (NeedRemove) {
      Length -= sizeof (EFI_ACPI_DEV_SCOPE_STRUCTURE);
    } else {
      ValidDeviceScopeNum++;
    }
  }

  //
  // If No deviceScope is left, set length as 0x00
  //
  if (Length > EFI_ACPI_RMRR_HEADER_LENGTH) {
    Rmrr->Length = Length;
  } else {
    Rmrr->Length = 0;
  }
}

/**
  Update the DMAR table

  @param[in, out] TableHeader         - The table to be set
**/
VOID
DmarTableUpdate (
  IN OUT EFI_ACPI_DESCRIPTION_HEADER       *TableHeader
  )
{
  EFI_ACPI_DMAR_TABLE         *DmarTable;
  EFI_ACPI_DMAR_TABLE         TempDmarTable;
  UINTN                       Offset;
  UINTN                       StructureLen;
  UINTN                       McD0BaseAddress;
  UINTN                       MchBar;
  UINT16                      IgdMode;
  UINT16                      GttMode;
  UINT32                      IgdMemSize;
  UINT32                      GttMemSize;


  IgdMemSize  = 0;
  GttMemSize  = 0;
  DmarTable   = (EFI_ACPI_DMAR_TABLE *) TableHeader;

  DEBUG ((DEBUG_INFO, "DmarTableUpdate() - start\n"));

  //
  // Set INTR_REMAP bit (BIT 0) if interrupt remapping is supported
  //
  if ((MmioRead32 (0xFED64000 + 0x10) & BIT3)
         && (MmioRead32 (DEF_VTD_BASE_ADDRESS + 0x10) & BIT3)) {
    DmarTable->Flags |= BIT0;
  }

  if (((AsmReadMsr64 (0x1B)) & (BIT11 + BIT10)) == BIT11 + BIT10) {
    DmarTable->Flags |= BIT1;
  } else {
    DmarTable->Flags &= 0xFD;
  }

  //
  // Calculate IGD mem size
  //
  McD0BaseAddress = MM_PCI_ADDRESS (SA_MC_BUS, 0, 0, 0);
  MchBar          = MmioRead32 (McD0BaseAddress + R_SA_MCHBAR_REG) & ~BIT0;
  IgdMode = (MmioRead16 (McD0BaseAddress + R_SA_GGC) & B_SA_GGC_GMS_MASK) >> N_SA_GGC_GMS_OFFSET;

  DEBUG ((DEBUG_INFO, "McD0BaseAddress 0x%08X, MchBar 0x%08X, IgdMode 0x%04X\n", McD0BaseAddress, MchBar, IgdMode));

  if (IgdMode < V_SA_GGC_GMS_2016MB) {
    IgdMemSize = IgdMode * 32 * (1024) * (1024);
  } else {
    IgdMemSize = 0;
  }

  //
  // Calculate GTT mem size
  //
  GttMemSize = 0;
  GttMode = (MmioRead16 (McD0BaseAddress + R_SA_GGC) & B_SA_GGC_GGMS_MASK) >> N_SA_GGC_GGMS_OFFSET;

  if (GttMode <= V_SA_GGC_GGMS_8MB) {
    GttMemSize = (1 << GttMode) * (1024) * (1024);
  }
  DEBUG ((DEBUG_INFO, "GttMode 0x%04X, GttMemSize 0x%08X\n", GttMode, GttMemSize));

  DmarTable->RmrrIgd.RmrBaseAddress   = (MmioRead32 (McD0BaseAddress + R_SA_TOLUD) & ~(0x01)) - IgdMemSize - GttMemSize;
  DmarTable->RmrrIgd.RmrLimitAddress  = DmarTable->RmrrIgd.RmrBaseAddress + IgdMemSize + GttMemSize - 1;

  DEBUG ((DEBUG_INFO, "RMRR Base  address IGD %016lX\n", DmarTable->RmrrIgd.RmrBaseAddress));
  DEBUG ((DEBUG_INFO, "RMRR Limit address IGD %016lX\n", DmarTable->RmrrIgd.RmrLimitAddress));

  DEBUG ((DEBUG_INFO, "RMRR Base  address HECI %016lX\n", DmarTable->RmrrHeci.RmrBaseAddress));
  DEBUG ((DEBUG_INFO, "RMRR Limit address HECI %016lX\n", DmarTable->RmrrHeci.RmrLimitAddress));

  //
  // Update DRHD structures of DmarTable
  //
  DmarTable->DrhdEngine1.RegisterBaseAddress = (MmioRead32 (MchBar + R_SA_MCHBAR_VTD1_OFFSET) &~3);
  DmarTable->DrhdEngine2.RegisterBaseAddress = (MmioRead32 (MchBar + R_SA_MCHBAR_VTD2_OFFSET) &~3);

  DEBUG ((DEBUG_INFO, "VTD base address1 0x%016lX\n", DmarTable->DrhdEngine1.RegisterBaseAddress));
  DEBUG ((DEBUG_INFO, "VTD base address2 0x%016lX\n", DmarTable->DrhdEngine2.RegisterBaseAddress));

  //
  // Copy DmarTable to TempDmarTable to be processed
  //
  CopyMem (&TempDmarTable, DmarTable, sizeof (EFI_ACPI_DMAR_TABLE));

  //
  // Update DRHD structures of temp DMAR table
  //
  UpdateDrhd (&TempDmarTable.DrhdEngine1);
  UpdateDrhd2 (&TempDmarTable.DrhdEngine2);

  //
  // Update RMRR structures of temp DMAR table
  //
  UpdateRmrr ((VOID *) &TempDmarTable.RmrrIgd);

  //
  // Remove unused device scope or entire DRHD structures
  //
  Offset = (UINTN) (&TempDmarTable.DrhdEngine1);
  if (TempDmarTable.DrhdEngine1.Length != 0) {
    Offset += TempDmarTable.DrhdEngine1.Length;
  }

  if (TempDmarTable.DrhdEngine2.Length != 0) {
    StructureLen = TempDmarTable.DrhdEngine2.Length;
    CopyMem ((VOID *) Offset, (VOID *) &TempDmarTable.DrhdEngine2, TempDmarTable.DrhdEngine2.Length);
    Offset += StructureLen;
  }

  //
  // Remove unused device scope or entire RMRR structures
  //
  if (TempDmarTable.RmrrHeci.Length != 0) {
    StructureLen = TempDmarTable.RmrrHeci.Length;
    CopyMem ((VOID *) Offset, (VOID *) &TempDmarTable.RmrrHeci, TempDmarTable.RmrrHeci.Length);
    Offset += StructureLen;
  }

  if (TempDmarTable.RmrrIgd.Length != 0) {
    StructureLen = TempDmarTable.RmrrIgd.Length;
    CopyMem ((VOID *) Offset, (VOID *) &TempDmarTable.RmrrIgd, TempDmarTable.RmrrIgd.Length);
    Offset += StructureLen;
  }

  Offset = Offset - (UINTN) &TempDmarTable;

  //
  // Re-calculate DMAR table check sum
  //
  TempDmarTable.Header.Checksum = (UINT8) (TempDmarTable.Header.Checksum + TempDmarTable.Header.Length - Offset);

  //
  // Set DMAR table length
  //
  TempDmarTable.Header.Length = (UINT32) Offset;

  //
  // Replace DMAR table with rebuilt table TempDmarTable
  //
  CopyMem ((VOID *) DmarTable, (VOID *) &TempDmarTable, TempDmarTable.Header.Length);

  DEBUG ((DEBUG_INFO, "DmarTableUpdate()  - end\n"));
}

/**
  EndOfDxe routine for update DMAR
**/
VOID
UpdateDmarAcpi (
  EFI_ACPI_DESCRIPTION_HEADER *Table
  )
{
  UINTN                McD0BaseAddress;

  DEBUG ((DEBUG_INFO, "DMAR table update.\n"));
  McD0BaseAddress = MM_PCI_ADDRESS (SA_MC_BUS, 0, 0, 0);

  ///
  /// skip install when disabled
  ///
  if ((MmioRead32 (McD0BaseAddress + R_SA_MC_CAPID0_A) & BIT23)) {
    DEBUG ((DEBUG_INFO, "Vtd Disabled, skip DMAR Table install\n"));
    return;
  }

  DmarTableUpdate (Table);
}
