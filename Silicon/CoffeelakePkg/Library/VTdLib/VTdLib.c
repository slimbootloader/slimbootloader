/** @file
  This code provides a initialization of Intel VT-d (Virtualization Technology for Directed I/O).

  Copyright (c) 2019 - 2023, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include <Base.h>
#include <Uefi/UefiBaseType.h>
#include <Library/IoLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/VTdLib.h>
#include <Register/SaRegs.h>
#include <Register/PchRegsP2sb.h>
#include <Library/DmaRemappingTable.h>
#include <Library/PciLib.h>
#include <Library/ConfigDataLib.h>
#include <Register/VtdRegs.h>

/**
  For device that specified by Device Num and Function Num,
  mDevEnMap is used to check device presence.
  0x80 means use Device ID to detemine presence
  0x8F means force to update

  The structure is used to check if device scope is valid when update DMAR table
**/
UINT16  mDevEnMap[][2] = {{0x0200, 0x80}, {0x1400, 0x80}, {0x1401, 0x80}, {0x1607, 0x8F}};
UINTN   mDevEnMapSize = sizeof (mDevEnMap) / (sizeof (UINT16) * 2);

UINTN   mDmarTableFlags = 0;

/**
  Get the corresponding device Enable/Disable bit according DevNum and FunNum

  @param[in] DevNum  - Device Number
  @param[in] FunNum  - Function Number

  @retval If the device is found, return disable/Enable bit in FD/Deven reigster
  @retval If not found return 0xFF
**/
UINT16
GetFunDisableBit (
  UINT8 DevNum,
  UINT8 FunNum
  )
{
  UINTN Index;

  for (Index = 0; Index < mDevEnMapSize; Index++) {
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
  IN OUT VOID *DrhdEnginePtr
  )
{
  UINT16                        Length;
  UINT16                        DisableBit;
  BOOLEAN                       NeedRemove;
  EFI_ACPI_DRHD_ENGINE1_STRUCT  *DrhdEngine;
  UINT32                        VidDid;

  //
  // Convert DrhdEnginePtr to EFI_ACPI_DRHD_ENGINE1_STRUCT Pointer
  //
  DrhdEngine      = (EFI_ACPI_DRHD_ENGINE1_STRUCT *) DrhdEnginePtr;
  Length          = DrhdEngine->DrhdHeader.Header.Length;
  DisableBit = GetFunDisableBit (
                 DrhdEngine->DeviceScope[0].PciPath.Device,
                 DrhdEngine->DeviceScope[0].PciPath.Function
                 );
  NeedRemove = FALSE;

  VidDid = PciRead32 (PCI_LIB_ADDRESS (0, DrhdEngine->DeviceScope[0].PciPath.Device, DrhdEngine->DeviceScope[0].PciPath.Function, 0x00));
  if ((DisableBit == 0xFF) ||
      (DrhdEngine->DrhdHeader.RegisterBaseAddress == 0) ||
      ((DisableBit == 0x80) && (VidDid == 0xFFFFFFFF))
      ) {
    NeedRemove = TRUE;
  }
  if (NeedRemove) {
    Length -= sizeof (EFI_ACPI_DEV_SCOPE_STRUCTURE);
  }
  ///
  /// If no devicescope is left, we set the structure length as 0x00
  ///
  if ((Length > EFI_ACPI_DRHD_ENGINE_HEADER_LENGTH) || (DrhdEngine->DrhdHeader.Flags == 0x01)) {
    DrhdEngine->DrhdHeader.Header.Length = Length;
  } else {
    DrhdEngine->DrhdHeader.Header.Length = 0;
  }
}

/**
  Get IOAPIC ID from LPC

  @retval APIC ID
**/
UINT8
GetIoApicId (
  VOID
  )
{
  UINT32                IoApicAddress;
  UINT32                IoApicId;

  IoApicAddress = 0XFEC00000;

  ///
  /// Get current IO APIC ID
  ///
  MmioWrite8 ((UINTN) (IoApicAddress + R_IO_APIC_INDEX_OFFSET), 0);
  IoApicId = MmioRead32 ((UINTN) (IoApicAddress + R_IO_APIC_DATA_OFFSET)) >> 24;

  return (UINT8) IoApicId;
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
  UINT16                        Index;
  UINT8                         Bus;
  UINT8                         Path[2];
  BOOLEAN                       NeedRemove;
  EFI_ACPI_DRHD_ENGINE3_STRUCT  *DrhdEngine;

  ///
  /// Convert DrhdEnginePtr to EFI_ACPI_DRHD_ENGINE3_STRUCT Pointer
  ///
  DrhdEngine      = (EFI_ACPI_DRHD_ENGINE3_STRUCT *) DrhdEnginePtr;

  Length          = DrhdEngine->DrhdHeader.Header.Length;
  DeviceScopeNum  = (DrhdEngine->DrhdHeader.Header.Length - EFI_ACPI_DRHD_ENGINE_HEADER_LENGTH) / sizeof (EFI_ACPI_DEV_SCOPE_STRUCTURE);
  Bus             = 0;
  ValidDeviceScopeNum = 0;
  Path[0]         = 0;
  Path[1]         = 0;

  for (Index = 0; Index < DeviceScopeNum; Index++) {
    NeedRemove = FALSE;
    /**
      For HPET and APIC, update device scope if Interrupt remapping is supported. remove device scope
      if interrupt remapping is not supported.
      - Index = 0 - IOAPIC
      - Index = 1 - HPET
    **/
    if ((mDmarTableFlags & DMAR_TABLE_FLAGS_INT_REMAPPING_SUPPORT) != 0) {
      if (Index == 0) {
        ///
        /// Update source id for IoApic's device scope entry
        ///
        Bus = V_P2SB_CFG_IBDF_BUS;
        Path[0] = V_P2SB_CFG_IBDF_DEV;
        Path[1] = V_P2SB_CFG_IBDF_FUNC;
        DrhdEngine->DeviceScope[Index].DeviceScopeStructureHeader.StartBusNumber = Bus;
        DrhdEngine->DeviceScope[Index].PciPath.Device = Path[0];
        DrhdEngine->DeviceScope[Index].PciPath.Function = Path[1];
        //
        // Update APIC ID
        //
        DrhdEngine->DeviceScope[Index].DeviceScopeStructureHeader.EnumerationId = GetIoApicId ();
      }
      if (Index == 1) {
        ///
        /// Update source id for HPET's device scope entry
        ///
        Bus     = V_P2SB_CFG_HBDF_BUS;
        Path[0] = V_P2SB_CFG_HBDF_DEV;
        Path[1] = V_P2SB_CFG_HBDF_FUNC;
        DrhdEngine->DeviceScope[Index].DeviceScopeStructureHeader.StartBusNumber = Bus;
        DrhdEngine->DeviceScope[Index].PciPath.Device = Path[0];
        DrhdEngine->DeviceScope[Index].PciPath.Function = Path[1];
      }
    } else {
      if ((Index == 0) || (Index == 1)) {
        NeedRemove = TRUE;
      }
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
  ///
  /// If no devicescope is left, we set the structure length as 0x00
  ///
  if ((Length > EFI_ACPI_DRHD_ENGINE_HEADER_LENGTH) || (DrhdEngine->DrhdHeader.Flags == 0x01)) {
    DrhdEngine->DrhdHeader.Header.Length = Length;
  } else {
    DrhdEngine->DrhdHeader.Header.Length = 0;
  }
}

/**
  Update the RMRR structure

  @param[in, out] RmrrPtr             - A pointer to RMRR structure
**/
VOID
UpdateRmrr (
  IN OUT VOID *RmrrPtr
  )
{
  UINT16                  Length;
  UINT16                  DisableBit;
  UINTN                   DeviceScopeNum;
  UINTN                   ValidDeviceScopeNum;
  UINTN                   Index;
  BOOLEAN                 NeedRemove;
  EFI_ACPI_RMRR_USB_STRUC *Rmrr;

  ///
  /// To make sure all devicescope can be checked,
  /// we convert the RmrrPtr to EFI_ACPI_RMRR_USB_STRUC pointer
  ///
  Rmrr                = (EFI_ACPI_RMRR_USB_STRUC *) RmrrPtr;

  Length              = Rmrr->RmrrHeader.Header.Length;
  ValidDeviceScopeNum = 0;
  DeviceScopeNum      = (Rmrr->RmrrHeader.Header.Length - EFI_ACPI_RMRR_HEADER_LENGTH) / sizeof (EFI_ACPI_DEV_SCOPE_STRUCTURE);
  for (Index = 0; Index < DeviceScopeNum; Index++) {
    DisableBit = GetFunDisableBit (
                   Rmrr->DeviceScope[Index].PciPath.Device,
                   Rmrr->DeviceScope[Index].PciPath.Function
                   );
    NeedRemove = FALSE;
    if ((DisableBit == 0xFF) ||
        ((DisableBit == 0x80) &&
         (PciRead32 (PCI_LIB_ADDRESS (0, Rmrr->DeviceScope[Index].PciPath.Device, Rmrr->DeviceScope[Index].PciPath.Function, 0x00)) == 0xFFFFFFFF))
        ) {
      NeedRemove = TRUE;
    } else if (DisableBit == 0x8F) {
      DEBUG ((DEBUG_INFO, "Rmrr->RmrrHeader.ReservedMemoryRegionBaseAddress %x\n", Rmrr->RmrrHeader.ReservedMemoryRegionBaseAddress));
      NeedRemove = FALSE;
    }

    CopyMem (
      &Rmrr->DeviceScope[ValidDeviceScopeNum],
      &Rmrr->DeviceScope[Index],
      sizeof (EFI_ACPI_DEV_SCOPE_STRUCTURE)
      );

    if (Rmrr->RmrrHeader.ReservedMemoryRegionLimitAddress == 0x0) {
      NeedRemove = TRUE;
    }

    if (NeedRemove) {
      Length -= sizeof (EFI_ACPI_DEV_SCOPE_STRUCTURE);
    } else {
      ValidDeviceScopeNum++;
    }
  }
  ///
  /// If No deviceScope is left, set length as 0x00
  ///
  if (Length > EFI_ACPI_RMRR_HEADER_LENGTH) {
    Rmrr->RmrrHeader.Header.Length = Length;
  } else {
    Rmrr->RmrrHeader.Header.Length = 0;
  }
}

/**
  Read VTD Engine Base Address from VTD BAR Offsets.

  @param[in]  VtdEngineNumber        - Engine number for which VTD Base Adderess is required.

  @retval   VTD Engine Base Address
**/
UINT32
EFIAPI
ReadVtdBaseAddress (
  IN UINT8        VtdEngineNumber
  )
{
  UINT64              McD0BaseAddress;
  UINTN               MchBar;

  McD0BaseAddress = PCI_LIB_ADDRESS (SA_MC_BUS, 0, 0, 0);
  MchBar          = PciRead32 ((UINTN)McD0BaseAddress + R_SA_MCHBAR) & (~BIT0);

  switch (VtdEngineNumber) {
    case 0:
      return (MmioRead32 (MchBar + R_MCHBAR_VTD1_OFFSET) & (~BIT0));
      break;
    case 2:
      return (MmioRead32 (MchBar + R_MCHBAR_VTD3_OFFSET) & (~BIT0));
      break;
    default:
      return 0x0;
      break;
  }
}

/**
  Update the DMAR table

  @param[in, out] TableHeader         - The table to be set
  @param[in, out] Version             - Version to publish
**/
VOID
DmarTableUpdate (
  IN OUT   EFI_ACPI_DESCRIPTION_HEADER       *TableHeader,
  IN OUT   EFI_ACPI_TABLE_VERSION            *Version
  )
{
  EFI_ACPI_DMAR_TABLE *DmarTable;
  EFI_ACPI_DMAR_TABLE TempDmarTable;
  UINTN               Offset;
  UINTN               StructureLen;
  UINT64              McD0BaseAddress;
  UINT16              IgdMode;
  UINT16              GttMode;
  UINT32              IgdMemSize;
  UINT32              GttMemSize;

  IgdMemSize  = 0;
  GttMemSize  = 0;
  DmarTable   = (EFI_ACPI_DMAR_TABLE *) TableHeader;
  ///
  /// Set INTR_REMAP bit (BIT 0) if interrupt remapping is supported
  ///
  if ((mDmarTableFlags & DMAR_TABLE_FLAGS_INT_REMAPPING_SUPPORT) != 0) {
    DmarTable->DmarHeader.Flags |= BIT0;
  }

  if ((mDmarTableFlags & DMAR_TABLE_FLAGS_X2APIC_OPT_OUT) != 0) {
    DmarTable->DmarHeader.Flags |= BIT1;
  } else {
    DmarTable->DmarHeader.Flags &= 0xFD;
  }


  ///
  /// Get OemId
  ///
  CopyMem (DmarTable->DmarHeader.Header.OemId, "INTEL ", sizeof (DmarTable->DmarHeader.Header.OemId));
  DmarTable->DmarHeader.Header.OemTableId      = 0x20202020324B4445;
  DmarTable->DmarHeader.Header.OemRevision     = 0x00000002;
  DmarTable->DmarHeader.Header.CreatorId       = 0x20202020;
  DmarTable->DmarHeader.Header.CreatorRevision = 0x01000013;

  ///
  /// Calculate IGD memsize
  ///
  McD0BaseAddress = PCI_LIB_ADDRESS (SA_MC_BUS, 0, 0, 0);
  IgdMode         = ((PciRead16 ((UINTN)McD0BaseAddress + R_SA_GGC) & B_SA_GGC_GMS_MASK) >> N_SA_GGC_GMS_OFFSET) & 0xFF;

  DEBUG ((DEBUG_INFO, "McD0BaseAddress 0x%08X, IgdMode 0x%04X\n", McD0BaseAddress, IgdMode));

  if (IgdMode < 0xF0) {
    IgdMemSize = IgdMode * 32 * (1024) * (1024);
  } else {
    IgdMemSize = 4 * (IgdMode - 0xF0 + 1) * (1024) * (1024);
  }
  ///
  /// Calculate GTT mem size
  ///
  GttMemSize = 0;
  GttMode = (PciRead16 ((UINTN)McD0BaseAddress + R_SA_GGC) & B_SA_GGC_GGMS_MASK) >> N_SA_GGC_GGMS_OFFSET;
  if (GttMode <= V_SA_GGC_GGMS_8MB) {
    GttMemSize = (1 << GttMode) * (1024) * (1024);
  }

  DmarTable->RmrrIgd.RmrrHeader.ReservedMemoryRegionBaseAddress   = (PciRead32 ((UINTN)McD0BaseAddress + R_SA_TOLUD) & ~(0x01)) - IgdMemSize - GttMemSize;
  DmarTable->RmrrIgd.RmrrHeader.ReservedMemoryRegionLimitAddress  = DmarTable->RmrrIgd.RmrrHeader.ReservedMemoryRegionBaseAddress + IgdMemSize + GttMemSize - 1;

  DEBUG ((DEBUG_INFO, "RMRR Base  address IGD %016lX\n", DmarTable->RmrrIgd.RmrrHeader.ReservedMemoryRegionBaseAddress));
  DEBUG ((DEBUG_INFO, "RMRR Limit address IGD %016lX\n", DmarTable->RmrrIgd.RmrrHeader.ReservedMemoryRegionLimitAddress));

  DmarTable->RmrrUsb.RmrrHeader.ReservedMemoryRegionBaseAddress   = 0;
  DmarTable->RmrrUsb.RmrrHeader.ReservedMemoryRegionLimitAddress  = 0;

  ///
  /// Convert to 4KB alignment.
  ///
  if (DmarTable->RmrrUsb.RmrrHeader.ReservedMemoryRegionLimitAddress != 0x0) {
    DmarTable->RmrrUsb.RmrrHeader.ReservedMemoryRegionBaseAddress  &= (EFI_PHYSICAL_ADDRESS) ~0xFFF;
    DmarTable->RmrrUsb.RmrrHeader.ReservedMemoryRegionLimitAddress &= (EFI_PHYSICAL_ADDRESS) ~0xFFF;
    DmarTable->RmrrUsb.RmrrHeader.ReservedMemoryRegionLimitAddress += 0x1000-1;
  }

  DEBUG ((DEBUG_INFO, "RMRR Base  address USB %016lX\n", DmarTable->RmrrUsb.RmrrHeader.ReservedMemoryRegionBaseAddress));
  DEBUG ((DEBUG_INFO, "RMRR Limit address USB %016lX\n", DmarTable->RmrrUsb.RmrrHeader.ReservedMemoryRegionLimitAddress));

  if (DmarTable->RmrrUsb.RmrrHeader.ReservedMemoryRegionBaseAddress == 0) {
    DEBUG ((DEBUG_INFO, "WARNING:  RmrrUsb.RmrrHeader.ReservedMemoryRegionBaseAddress is 0.\n"));
  }

  DmarTable->RmrrCsme.RmrrHeader.ReservedMemoryRegionBaseAddress   = 0;
  DmarTable->RmrrCsme.RmrrHeader.ReservedMemoryRegionLimitAddress  = 0;

  ///
  /// Update DRHD structures of DmarTable
  ///
  DmarTable->DrhdEngine1.DrhdHeader.RegisterBaseAddress = ReadVtdBaseAddress(0);
  DmarTable->DrhdEngine3.DrhdHeader.RegisterBaseAddress = ReadVtdBaseAddress(2);

  DEBUG ((DEBUG_INFO, "VTD base address 1 = %x\n", DmarTable->DrhdEngine1.DrhdHeader.RegisterBaseAddress));
  DEBUG ((DEBUG_INFO, "VTD base address 3 = %x\n", DmarTable->DrhdEngine3.DrhdHeader.RegisterBaseAddress));
  ///
  /// copy DmarTable to TempDmarTable to be processed
  ///
  CopyMem (&TempDmarTable, DmarTable, sizeof (EFI_ACPI_DMAR_TABLE));

  ///
  /// Update DRHD structures of temp DMAR table
  ///
  UpdateDrhd (&TempDmarTable.DrhdEngine1);
  UpdateDrhd2 (&TempDmarTable.DrhdEngine3);

  ///
  /// Update RMRR structures of temp DMAR table
  ///
  UpdateRmrr ((VOID *) &TempDmarTable.RmrrUsb);
  UpdateRmrr ((VOID *) &TempDmarTable.RmrrIgd);
  UpdateRmrr ((VOID *) &TempDmarTable.RmrrCsme);

  ///
  /// Remove unused device scope or entire DRHD structures
  ///
  Offset = (UINTN) (&TempDmarTable.DrhdEngine1);
  if (TempDmarTable.DrhdEngine1.DrhdHeader.Header.Length != 0) {
    Offset += TempDmarTable.DrhdEngine1.DrhdHeader.Header.Length;
  }

  if (TempDmarTable.DrhdEngine3.DrhdHeader.Header.Length != 0) {
    StructureLen = TempDmarTable.DrhdEngine3.DrhdHeader.Header.Length;
    CopyMem ((VOID *) Offset, (VOID *) &TempDmarTable.DrhdEngine3, TempDmarTable.DrhdEngine3.DrhdHeader.Header.Length);
    Offset += StructureLen;
  }
  ///
  /// Remove unused device scope or entire RMRR structures
  ///
  if (TempDmarTable.RmrrUsb.RmrrHeader.Header.Length != 0) {
    StructureLen = TempDmarTable.RmrrUsb.RmrrHeader.Header.Length;
    CopyMem ((VOID *) Offset, (VOID *) &TempDmarTable.RmrrUsb, TempDmarTable.RmrrUsb.RmrrHeader.Header.Length);
    Offset += StructureLen;
  }

  if (TempDmarTable.RmrrIgd.RmrrHeader.Header.Length != 0) {
    StructureLen = TempDmarTable.RmrrIgd.RmrrHeader.Header.Length;
    CopyMem ((VOID *) Offset, (VOID *) &TempDmarTable.RmrrIgd, TempDmarTable.RmrrIgd.RmrrHeader.Header.Length);
    Offset += StructureLen;
  }

  if (TempDmarTable.RmrrCsme.RmrrHeader.Header.Length != 0) {
    StructureLen = TempDmarTable.RmrrCsme.RmrrHeader.Header.Length;
    CopyMem ((VOID *) Offset, (VOID *) &TempDmarTable.RmrrCsme, TempDmarTable.RmrrCsme.RmrrHeader.Header.Length);
    Offset += StructureLen;
  }

  Offset = Offset - (UINTN) &TempDmarTable;
  ///
  /// Re-calculate DMAR table check sum
  ///
  TempDmarTable.DmarHeader.Header.Checksum = (UINT8) (TempDmarTable.DmarHeader.Header.Checksum + TempDmarTable.DmarHeader.Header.Length - Offset);
  ///
  /// Set DMAR table length
  ///
  TempDmarTable.DmarHeader.Header.Length = (UINT32) Offset;
  ///
  /// Replace DMAR table with rebuilt table TempDmarTable
  ///
  CopyMem ((VOID *) DmarTable, (VOID *) &TempDmarTable, TempDmarTable.DmarHeader.Header.Length);
}

/**
  Update the DMAR table

  @param[in, out] Table              The pointer to ACPI DMAR table
  @param[in] DmarTableFlags          The flags related with DMAR table
**/
VOID
EFIAPI
UpdateDmarAcpi (
  EFI_ACPI_DESCRIPTION_HEADER *Table,
  UINTN                       DmarTableFlags
  )
{
  EFI_ACPI_TABLE_VERSION          Version;
  STATIC BOOLEAN                  Triggered = FALSE;

  if (Triggered) {
    return;
  }

  Triggered     = TRUE;

  DEBUG ((DEBUG_INFO, "Update Dmar ACPI Table\n"));

  ///
  /// Fix DMAR Table always created, skip install when disabled
  ///
  if ((FeaturePcdGet (PcdVtdEnabled) == FALSE) || (PciRead32 (PCI_LIB_ADDRESS (SA_MC_BUS, 0, 0, R_SA_MC_CAPID0_A_OFFSET)) & BIT23)) {
    DEBUG ((DEBUG_INFO, "Vtd Disabled, skip DMAR Table install\n"));
    return;
  }

  mDmarTableFlags  = DmarTableFlags;

  ///
  /// By default, a table belongs in all ACPI table versions published.
  ///
  Version = EFI_ACPI_TABLE_VERSION_1_0B | EFI_ACPI_TABLE_VERSION_2_0 | EFI_ACPI_TABLE_VERSION_3_0;

  DmarTableUpdate (Table, &Version);
}
