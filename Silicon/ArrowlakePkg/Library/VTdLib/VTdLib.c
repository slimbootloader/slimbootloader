/** @file
  This code provides a initialization of Intel VT-d (Virtualization Technology for Directed I/O).

  Copyright (c) 2024, Intel Corporation. All rights reserved.<BR>
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
#include <Register/VtdRegs.h>
#include <IndustryStandard/Pci22.h>
#include <Library/PciSegmentLib.h>
#include <Library/BaseLib.h>
#include <PlatformBase.h>

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

  IoApicAddress = IO_APIC_BASE_ADDRESS;
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
  Update the SATC structure

  @param[in, out] SatcPtr             - A pointer to SATC structure
**/
VOID
UpdateSatc (
  IN OUT EFI_ACPI_SATC_STRUCT *Satc
  )
{
  UINT16                 Length;
  UINTN                  DeviceScopeNum;
  UINTN                  ValidDeviceScopeNum;
  UINTN                  Index;
  BOOLEAN                NeedRemove;
  VTD_ENGINE             VtdEngine;
  UINT32                 VtdBase;
  UINT64                 Ecap;

  if (Satc == NULL) {
    return;
  }

  Length              = Satc->SatcHeader.Header.Length;
  DeviceScopeNum      = (Satc->SatcHeader.Header.Length - sizeof (EFI_ACPI_DMAR_SATC_HEADER)) / sizeof (EFI_ACPI_DEV_SCOPE_STRUCTURE);
  ValidDeviceScopeNum = 0;

  for (Index = 0; Index < DeviceScopeNum; Index++) {
    NeedRemove = FALSE;
    if ((PciSegmentRead32 (PCI_SEGMENT_LIB_ADDRESS (SA_SEG_NUM, 0, Satc->DeviceScope[Index].PciPath.Device, Satc->DeviceScope[Index].PciPath.Function, 0x00)) == 0xFFFFFFFF)) {
      NeedRemove = TRUE;
    }

    VtdEngine = GetVtdEngineForPciDevice (0, Satc->DeviceScope[Index].PciPath.Device, Satc->DeviceScope[Index].PciPath.Function);
    VtdBase   = ReadVtdBaseAddress (VtdEngine);
    Ecap = MmioRead64 (VtdBase + ECAP_VTD_IOMMU_REG);
    DEBUG ((DEBUG_INFO, "VtdBase 0x%08X, Ecap 0x%16llX\n", VtdBase, Ecap));

    //
    // If IOMMU doesn't support Device-TLBs, then respective devices mustn't be published in SATC table
    //
    if ((Ecap & B_ECAP_DEVICE_TLB_SUPPORT) == 0) {
      NeedRemove = TRUE;
    }

    CopyMem (
      &Satc->DeviceScope[ValidDeviceScopeNum],
      &Satc->DeviceScope[Index],
      sizeof (EFI_ACPI_DEV_SCOPE_STRUCTURE)
      );

    if (NeedRemove) {
      Length -= sizeof (EFI_ACPI_DEV_SCOPE_STRUCTURE);
    } else {
      ValidDeviceScopeNum++;
    }
  }

  ///
  /// If no deviceScope is left, set length as 0x00
  ///
  if (Length > sizeof (EFI_ACPI_DMAR_SATC_HEADER)) {
    Satc->SatcHeader.Header.Length = Length;
  } else {
    Satc->SatcHeader.Header.Length = 0;
  }
}

/**
  Update the SIDP structure

  @param[in, out] Sidp       A pointer to SIDP structure
**/
VOID
UpdateSidp (
  IN OUT EFI_ACPI_SIDP_STRUCT *Sidp
  )
{
  UINT16                 Length;
  UINTN                  DeviceScopeNum;
  UINTN                  ValidDeviceScopeNum;
  UINTN                  Index;
  BOOLEAN                NeedRemove;
  VTD_ENGINE             VtdEngine;
  UINT32                 VtdBase;
  UINT64                 Ecap;

  if (Sidp == NULL) {
    return;
  }

  Length              = Sidp->SidpHeader.Header.Length;
  DeviceScopeNum      = (Sidp->SidpHeader.Header.Length - sizeof (EFI_ACPI_DMAR_SIDP_HEADER)) / sizeof (EFI_ACPI_DEV_SCOPE_STRUCTURE);
  ValidDeviceScopeNum = 0;

  for (Index = 0; Index < DeviceScopeNum; Index++) {
    NeedRemove = FALSE;
    if ((PciSegmentRead32 (PCI_SEGMENT_LIB_ADDRESS (SA_SEG_NUM, 0, Sidp->DeviceScope[Index].PciPath.Device, Sidp->DeviceScope[Index].PciPath.Function, 0x00)) == 0xFFFFFFFF)) {
      NeedRemove = TRUE;
    }

    Sidp->DeviceScope[Index].DeviceScopeStructureHeader.Flags = GetPropertiesForPciDevice (
                                                                  0,
                                                                  Sidp->DeviceScope[Index].PciPath.Device,
                                                                  Sidp->DeviceScope[Index].PciPath.Function
                                                                  );

    VtdEngine = GetVtdEngineForPciDevice (0, Sidp->DeviceScope[Index].PciPath.Device, Sidp->DeviceScope[Index].PciPath.Function);
    VtdBase   = ReadVtdBaseAddress (VtdEngine);
    Ecap = MmioRead64 (VtdBase + ECAP_VTD_IOMMU_REG);
    DEBUG ((DEBUG_INFO, "VtdBase 0x%08X, Ecap 0x%16llX\n", VtdBase, Ecap));

    //
    // If IOMMU doesn't support Device-TLBs, then respective devices mustn't be published in SIDP table
    //
    if ((Ecap & B_ECAP_DEVICE_TLB_SUPPORT) == 0) {
      NeedRemove = TRUE;
    }

    CopyMem (
      &Sidp->DeviceScope[ValidDeviceScopeNum],
      &Sidp->DeviceScope[Index],
      sizeof (EFI_ACPI_DEV_SCOPE_STRUCTURE)
      );

    if (NeedRemove) {
      Length -= sizeof (EFI_ACPI_DEV_SCOPE_STRUCTURE);
    } else {
      ValidDeviceScopeNum++;
    }
  }

  ///
  /// If no deviceScope is left, set length as 0x00
  ///
  if (Length > sizeof (EFI_ACPI_DMAR_SIDP_HEADER)) {
    Sidp->SidpHeader.Header.Length = Length;
  } else {
    Sidp->SidpHeader.Header.Length = 0;
  }
}

/**
  Get VT-d BAR size.

  @return     VT-d BAR size
**/
UINT32
GetVtdBarSize (
  )
{
  //
  // For Desktop SKU VT-d regiser space is 64KB, for other SKUs - 4KB
  //
#if FixedPcdGet8(PcdMtlSSupport) == 1
  return SIZE_64KB;
#else
  return SIZE_4KB;
#endif
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
    case IGD_VTD:
      return (MmioRead32 (MchBar + R_MCHBAR_VTD1_OFFSET) & (~BIT0));
      break;
    case IOP_VTD:
      return ((MmioRead32 (MchBar + R_MCHBAR_VTD1_OFFSET) & (~BIT0)) + GetVtdBarSize());
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
  UINT16              GttMode;
  UINT32              GttMemSize;
  MEMORY_CFG_DATA     *MemCfgData;

  MemCfgData = (MEMORY_CFG_DATA *)FindConfigDataByTag (CDATA_MEMORY_TAG);
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
  DEBUG ((DEBUG_INFO, "McD0BaseAddress 0x%16llX\n", McD0BaseAddress));

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

  ///
  /// Update DRHD structures of DmarTable
  ///
  DmarTable->DrhdEngine1.DrhdHeader.RegisterBaseAddress = ReadVtdBaseAddress(IGD_VTD);
  DmarTable->DrhdEngine3.DrhdHeader.RegisterBaseAddress = ReadVtdBaseAddress(IOP_VTD);

  if (GetVtdBarSize () == SIZE_64KB) {
    //
    // Below field stores information about the size of the remapping hardware register set.
    // Bits 3:0: If the values in this field is N, the size of register set is 2^N 4KB pages.
    //
    if (PciSegmentRead16 (McD2BaseAddress) != 0xFFFF) {
      DmarTable->DrhdEngine1.DrhdHeader.Size = 0x4;
    }
  }
  if (GetVtdBarSize () == SIZE_64KB) {
    DmarTable->DrhdEngine3.DrhdHeader.Size = 0x4;
  }

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
  /// Update SATC structure of temp DMAR table
  ///
  UpdateSatc (&TempDmarTable.Satc);

  ///
  /// Update SIDP structure of temp DMAR table
  ///
  UpdateSidp (&TempDmarTable.Sidp);

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

  if (TempDmarTable.Satc.SatcHeader.Header.Length != 0) {
    StructureLen = TempDmarTable.Satc.SatcHeader.Header.Length;
    CopyMem ((VOID *) Offset, (VOID *) &TempDmarTable.Satc, TempDmarTable.Satc.SatcHeader.Header.Length);
    Offset += StructureLen;
  }

  if (TempDmarTable.Sidp.SidpHeader.Header.Length != 0) {
    StructureLen = TempDmarTable.Sidp.SidpHeader.Header.Length;
    CopyMem ((VOID *) Offset, (VOID *) &TempDmarTable.Sidp, TempDmarTable.Sidp.SidpHeader.Header.Length);
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

typedef struct {
  struct {
    UINT8  Bus;
    UINT8  Device   : 5;
    UINT8  Function : 3;
  } Bdf;
  VTD_ENGINE VtdEngine;
  UINT8      Properties; // DeviceScope[].Flags
} PCIE_INFO;

PCIE_INFO PcieInfo[] = {
  {{0, 0x2, 0x0}, IGD_VTD, (BIT0 | BIT1 | BIT2 | BIT3 | BIT4)}, // GFX
  {{0, 0x5, 0x0}, IOP_VTD, (BIT0 | BIT1 | BIT2 | BIT3 | BIT4)}, // IPU
  {{0, 0xB, 0x0}, IOP_VTD, (BIT2 | BIT3 | BIT4)}, // VPU
};

/**
  The functions returns the IOMMU instance which supports a given PCIe device.

  @attention
  The function doesn't check if a given PCIe device exists or is enabled. This is
  a responsibility of a caller.

  @param[in]  Bus          PCIe Bus number
  @param[in]  Device       PCIe Device number
  @param[in]  Function     PCIe Function number

  @retval  VT-d engine number
**/
VTD_ENGINE
GetVtdEngineForPciDevice (
  IN UINT8  Bus,
  IN UINT8  Device,
  IN UINT8  Function
  )
{
  UINT8  Index;

  for (Index = 0; Index < ARRAY_SIZE (PcieInfo); Index++) {
    if ((Bus      == PcieInfo[Index].Bdf.Bus) &&
        (Device   == PcieInfo[Index].Bdf.Device) &&
        (Function == PcieInfo[Index].Bdf.Function))
    {
      return PcieInfo[Index].VtdEngine;
    }
  }

  return IOP_VTD;
}

/**
  The functions returns the properties for a given PCIe device.

  @attention
  The function doesn't check if a given PCIe device exists or is enabled. This is
  a responsibility of a caller.

  @param[in]  Bus          PCIe Bus number
  @param[in]  Device       PCIe Device number
  @param[in]  Function     PCIe Function number

  @retval  PCIe device properties
**/
UINT8
GetPropertiesForPciDevice (
  IN UINT8  Bus,
  IN UINT8  Device,
  IN UINT8  Function
  )
{
  UINT8  Index;

  for (Index = 0; Index < ARRAY_SIZE (PcieInfo); Index++) {
    if ((Bus      == PcieInfo[Index].Bdf.Bus) &&
        (Device   == PcieInfo[Index].Bdf.Device) &&
        (Function == PcieInfo[Index].Bdf.Function))
    {
      return PcieInfo[Index].Properties;
    }
  }

  return 0;
}