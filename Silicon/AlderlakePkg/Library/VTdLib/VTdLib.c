/** @file
  This code provides a initialization of Intel VT-d (Virtualization Technology for Directed I/O).

  Copyright (c) 1999 - 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include <Base.h>
#include <Uefi/UefiBaseType.h>
#include <Library/IoLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/VTdLib.h>
#include <PchAccess.h>
#include <Register/IgdRegs.h>
#include <Library/DmaRemappingTable.h>
#include <Library/PciLib.h>
#include <Include/ConfigDataDefs.h>
#include <Library/ConfigDataLib.h>
#include <Register/SaRegsHostBridge.h>
#include <GlobalNvsAreaDef.h>
//#include <Library/ItbtPcieRpLib.h>
#include <Register/VtdRegs.h>
#include <IndustryStandard/Pci22.h>

/**
  For device that specified by Device Num and Function Num,
  mDevEnMap is used to check device presence.
  0x80 means use Device ID to detemine presence
  0x8F means force to update

  The structure is used to check if device scope is valid when update DMAR table
**/
UINT16  mDevEnMap[][2] = {{0x0200, 0x80}, {0x0500, 0x80}, {0x1400, 0x80}, {0x1401, 0x80}};
UINTN   mDevEnMapSize = sizeof (mDevEnMap) / (sizeof (UINT16) * 2);

BOOLEAN mInterruptRemappingSupport;

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

//#define V_SA_PCI_DEV_2_GT0_TGL_ID          0x9A7F ///< Dev2 TGL GT0

/**
  Checks if SKU is DisplayOnly

  @retval FALSE  SKU is not DisplayOnly
  @retval TRUE   SKU is DisplayOnly
**/
BOOLEAN
EFIAPI
IsDisplayOnlySku (
  VOID
  )
{
  UINT16    GtDid;

  GtDid = PciRead16 (PCI_LIB_ADDRESS ( IGD_BUS_NUM, IGD_DEV_NUM, IGD_FUN_NUM, PCI_DEVICE_ID_OFFSET));
  if ((GtDid == V_SA_PCI_DEV_2_GT0_ADL_S_ID) || (GtDid == V_SA_PCI_DEV_2_GT0_ADL_P_ID)
     ) {
    return TRUE;
  }
  return FALSE;
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

  if ((DisableBit == 0xFF) ||
      (DrhdEngine->DrhdHeader.RegisterBaseAddress == 0) ||
      ((DisableBit == 0x80) &&
       (PciRead32 (PCI_LIB_ADDRESS (0, DrhdEngine->DeviceScope[0].PciPath.Device, DrhdEngine->DeviceScope[0].PciPath.Function, 0x00)) == 0xFFFFFFFF))
      ) {
    NeedRemove = TRUE;
  }
  if ((DrhdEngine->DeviceScope[0].PciPath.Device == IGD_DEV_NUM) && (DrhdEngine->DeviceScope[0].PciPath.Function == IGD_FUN_NUM) &&
      (PciRead32 (PCI_LIB_ADDRESS (SA_SEG_NUM, DrhdEngine->DeviceScope[0].PciPath.Device, DrhdEngine->DeviceScope[0].PciPath.Function, 0x00)) != 0xFFFFFFFF)) {
    NeedRemove = IsDisplayOnlySku();
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
  MmioWrite8 ((UINTN) (IoApicAddress + R_IO_APIC_MEM_INDEX_OFFSET), 0);
  IoApicId = MmioRead32 ((UINTN) (IoApicAddress + R_IO_APIC_MEM_DATA_OFFSET)) >> 24;

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
    if (mInterruptRemappingSupport) {
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

      if (Rmrr->RmrrHeader.ReservedMemoryRegionBaseAddress != 0) {
        NeedRemove = FALSE;
      } else {
        NeedRemove = TRUE;
      }
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
    case 1:
      return (MmioRead32 (MchBar + R_MCHBAR_VTD2_LOW_OFFSET) & (~BIT0));
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
  UINT32              GttMmAdr;
  UINT64              McD2BaseAddress;
  UINT16              IgdMode;
  UINT16              GttMode;
  UINT32              IgdMemSize;
  UINT32              GttMemSize;
  MEMORY_CFG_DATA     *MemCfgData;

  MemCfgData = (MEMORY_CFG_DATA *)FindConfigDataByTag (CDATA_MEMORY_TAG);
  IgdMemSize  = 0;
  GttMemSize  = 0;
  DmarTable   = (EFI_ACPI_DMAR_TABLE *) TableHeader;
  ///
  /// Set INTR_REMAP bit (BIT 0) if interrupt remapping is supported
  ///
  if (mInterruptRemappingSupport) {
    DmarTable->DmarHeader.Flags |= BIT0;
  }

  if (MemCfgData != NULL) {
    if (MemCfgData->X2ApicOptOut == 1) {
      DmarTable->DmarHeader.Flags |= BIT1;
    } else {
      DmarTable->DmarHeader.Flags &= 0xFD;
    }
    ///
    /// Set DMA_CONTROL_GUARANTEE bit (BIT 2) if Dma Control Guarantee is supported
    ///
    if (MemCfgData->DmaControlGuarantee == 1) {
      DmarTable->DmarHeader.Flags |= BIT2;
    }
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
  McD2BaseAddress = PCI_LIB_ADDRESS ( IGD_BUS_NUM, IGD_DEV_NUM, IGD_FUN_NUM, 0);
  GttMmAdr = (PciRead32 ((UINTN)McD2BaseAddress + R_SA_IGD_GTTMMADR)) & 0xFFFFFFF0;
  DEBUG ((DEBUG_INFO, "GttMode 0x%04X, GttMmAdr  0x%08X, GttMemSize 0x%08X\n", GttMode, GttMmAdr, GttMemSize));

  DmarTable->RmrrIgd.RmrrHeader.ReservedMemoryRegionBaseAddress   = (PciRead32 ((UINTN)McD0BaseAddress + R_SA_BGSM) & ~(0x01));
  DmarTable->RmrrIgd.RmrrHeader.ReservedMemoryRegionLimitAddress  = DmarTable->RmrrIgd.RmrrHeader.ReservedMemoryRegionBaseAddress + IgdMemSize + GttMemSize - 1;
  DEBUG ((DEBUG_INFO, "RMRR Base  address IGD %016lX\n", DmarTable->RmrrIgd.RmrrHeader.ReservedMemoryRegionBaseAddress));
  DEBUG ((DEBUG_INFO, "RMRR Limit address IGD %016lX\n", DmarTable->RmrrIgd.RmrrHeader.ReservedMemoryRegionLimitAddress));

  ///
  /// Update DRHD structures of DmarTable
  ///
  DmarTable->DrhdEngine1.DrhdHeader.RegisterBaseAddress = ReadVtdBaseAddress(0);
  DmarTable->DrhdEngine2.DrhdHeader.RegisterBaseAddress = ReadVtdBaseAddress(1);
  DmarTable->DrhdEngine3.DrhdHeader.RegisterBaseAddress = ReadVtdBaseAddress(2);

  DEBUG ((DEBUG_INFO, "VTD base address 1 = %x\n", DmarTable->DrhdEngine1.DrhdHeader.RegisterBaseAddress));
  DEBUG ((DEBUG_INFO, "VTD base address 2 = %x\n", DmarTable->DrhdEngine2.DrhdHeader.RegisterBaseAddress));
  DEBUG ((DEBUG_INFO, "VTD base address 3 = %x\n", DmarTable->DrhdEngine3.DrhdHeader.RegisterBaseAddress));
  ///
  /// copy DmarTable to TempDmarTable to be processed
  ///
  CopyMem (&TempDmarTable, DmarTable, sizeof (EFI_ACPI_DMAR_TABLE));

  ///
  /// Update DRHD structures of temp DMAR table
  ///
  UpdateDrhd (&TempDmarTable.DrhdEngine1);
  UpdateDrhd (&TempDmarTable.DrhdEngine2);
  UpdateDrhd2 (&TempDmarTable.DrhdEngine3);

  ///
  /// Update RMRR structures of temp DMAR table
  ///
  UpdateRmrr ((VOID *) &TempDmarTable.RmrrIgd);

  ///
  /// Remove unused device scope or entire DRHD structures
  ///
  Offset = (UINTN) (&TempDmarTable.DrhdEngine1);
  if (TempDmarTable.DrhdEngine1.DrhdHeader.Header.Length != 0) {
    Offset += TempDmarTable.DrhdEngine1.DrhdHeader.Header.Length;
  }
  if (TempDmarTable.DrhdEngine2.DrhdHeader.Header.Length != 0) {
    StructureLen = TempDmarTable.DrhdEngine2.DrhdHeader.Header.Length;
    CopyMem ((VOID *) Offset, (VOID *) &TempDmarTable.DrhdEngine2, TempDmarTable.DrhdEngine2.DrhdHeader.Header.Length);
    Offset += StructureLen;
  }

  if (TempDmarTable.DrhdEngine3.DrhdHeader.Header.Length != 0) {
    StructureLen = TempDmarTable.DrhdEngine3.DrhdHeader.Header.Length;
    CopyMem ((VOID *) Offset, (VOID *) &TempDmarTable.DrhdEngine3, TempDmarTable.DrhdEngine3.DrhdHeader.Header.Length);
    Offset += StructureLen;
  }
  ///
  /// Remove unused device scope or entire RMRR structures
  ///
  if (TempDmarTable.RmrrIgd.RmrrHeader.Header.Length != 0) {
    StructureLen = TempDmarTable.RmrrIgd.RmrrHeader.Header.Length;
    CopyMem ((VOID *) Offset, (VOID *) &TempDmarTable.RmrrIgd, TempDmarTable.RmrrIgd.RmrrHeader.Header.Length);
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
  EndOfDxe routine for update DMAR
**/
VOID
UpdateDmarAcpi (
  EFI_ACPI_DESCRIPTION_HEADER *Table
  )
{
  EFI_ACPI_TABLE_VERSION          Version;
  STATIC BOOLEAN                  Triggered = FALSE;
  SILICON_CFG_DATA                *SiCfgData;

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

  mInterruptRemappingSupport  = FALSE;
  SiCfgData = (SILICON_CFG_DATA *)FindConfigDataByTag (CDATA_SILICON_TAG);
  if (SiCfgData != NULL) {
    mInterruptRemappingSupport  = SiCfgData->InterruptRemappingSupport;
  }

  ///
  /// By default, a table belongs in all ACPI table versions published.
  ///
  Version = EFI_ACPI_TABLE_VERSION_1_0B | EFI_ACPI_TABLE_VERSION_2_0 | EFI_ACPI_TABLE_VERSION_3_0;

  DmarTableUpdate (Table, &Version);
}
