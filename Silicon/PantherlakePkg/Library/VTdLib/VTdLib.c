/** @file
  This code provides a initialization of Intel VT-d (Virtualization Technology for Directed I/O).

  Copyright (c) 1999 - 2024, Intel Corporation. All rights reserved.<BR>
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
#include <Library/DumpDmar.h>



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
  {{0, 0x2, 0x0}, GFX_VTD,     (BIT0 | BIT1 | BIT2 | BIT3 | BIT4)}, // GFX
  {{0, 0x4, 0x0}, NON_GFX_VTD, 0                                }, // Camarillo
  {{0, 0x5, 0x0}, NON_GFX_VTD, (BIT0 | BIT1 | BIT2 | BIT3 | BIT4)}, // IPU
  {{0, 0xB, 0x0}, NON_GFX_VTD, (BIT2 | BIT3 | BIT4)              }, // NPU
  {{0, 0xC, 0x0}, NON_GFX_VTD, (BIT3 | BIT4)                     }, // IAA
};
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
  if ((GtDid == V_SA_PCI_DEV_2_GT0_PTL_H_ID)
       || (GtDid == V_SA_PCI_DEV_2_GT0_PTL_U_ID)
       || (GtDid == V_SA_PCI_DEV_2_GT2_PTL_H_ID)) {
       return TRUE;
     } else {
       return FALSE;
   }
}

/**
    Checks if VtdEngine is enabled

    @param[in] VtdEngineNumber  - Enum of Type VTD_ENGINE

    @retval TRUE  - VtdEngine is Enabled
    @retval FALSE - VtdEngine is Disabled
**/

BOOLEAN
IsVtdEngineEnabled (
  IN VTD_ENGINE  VtdEngineNumber
  )
{
  UINT64              McD0BaseAddress;
  UINTN               MchBar;
  UINT32              Data = 0;

  if (VtdEngineNumber < VTD_ENGINE_NUMBER) {

    McD0BaseAddress = PCI_LIB_ADDRESS (SA_MC_BUS, 0, 0, 0);
    MchBar = PciRead32 ((UINTN)McD0BaseAddress + R_SA_MCHBAR) & (~BIT0);
    Data = MmioRead32 (MchBar + R_MCHBAR_VTD1_OFFSET);

    if (Data == 0xFFFFFFFF) {
      return FALSE;
      }

    return ( (Data >> VtdEngineNumber) & 0x1) ? TRUE : FALSE;
  } else {
    return FALSE;
  }
}


/**
  Update the Gfx VTD DRHD structure

  @param[in, out] DrhdEnginePtr       - A pointer to DRHD structure
**/
VOID
UpdateDrhd1 (
  IN OUT VOID *DrhdEnginePtr
  )
{
  UINT16                        Length;
  UINT16                        DisableBit;
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

  if ((DisableBit == 0xFF) ||
      (DrhdEngine->DrhdHeader.RegisterBaseAddress == 0) ||
      ((DisableBit == 0x80) &&
       (PciRead32 (PCI_LIB_ADDRESS (0, DrhdEngine->DeviceScope[0].PciPath.Device, DrhdEngine->DeviceScope[0].PciPath.Function, 0x00)) == 0xFFFFFFFF))
      ) {
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

  IoApicAddress = 0xFEC00000;
  ///
  /// Get current IO APIC ID
  ///
  MmioWrite8 ((UINTN) (IoApicAddress + R_IO_APIC_MEM_INDEX_OFFSET), 0);
  IoApicId = MmioRead32 ((UINTN) (IoApicAddress + R_IO_APIC_MEM_DATA_OFFSET)) >> 24;

  return (UINT8) IoApicId;
}


/**
  Update the non-Gfx VTD DRHD structure

  @param[in, out] DrhdEnginePtr       - A pointer to DRHD structure
**/
VOID
UpdateDrhd2 (
  IN OUT VOID *DrhdEnginePtr
)
{
  UINTN                         DeviceScopeNum;
  UINTN                         Index;
  UINT16                        Offset;
  UINT32                        VIDDID;
  EFI_ACPI_DRHD_ENGINE2_STRUCT  *DrhdEngine;

  DrhdEngine      = (EFI_ACPI_DRHD_ENGINE2_STRUCT *) DrhdEnginePtr;
  Offset          = sizeof(DrhdEngine->DrhdHeader);

  if (IsVtdEngineEnabled (NON_GFX_VTD)) {
    DEBUG ((DEBUG_INFO, "%a NON_GFX_VTD Enabled! \n", __FUNCTION__));
    DeviceScopeNum  = (DrhdEngine->DrhdHeader.Header.Length - EFI_ACPI_DRHD_ENGINE_HEADER_LENGTH) / sizeof (EFI_ACPI_DEV_SCOPE_STRUCTURE);
    for (Index = 0; Index < DeviceScopeNum; Index++) {
      VIDDID = PciRead32 (PCI_LIB_ADDRESS (0,
                                              DrhdEngine->DeviceScope[Index].PciPath.Device,
                                              DrhdEngine->DeviceScope[Index].PciPath.Function,
                                              0x0));

    if (DrhdEngine->DrhdHeader.RegisterBaseAddress && (VIDDID != 0xFFFFFFFF) ) {

        DEBUG ((DEBUG_INFO, "%a Add PCI Device |0x0|0x%0x|0x%0x|, VID_DID = 0x%0x  \n", __FUNCTION__,
                                                                DrhdEngine->DeviceScope[Index].PciPath.Device,
                                                                DrhdEngine->DeviceScope[Index].PciPath.Function,
                                                                VIDDID));
        CopyMem (&((UINT8*)DrhdEngine)[Offset],
                  &DrhdEngine->DeviceScope[Index],
                  sizeof (EFI_ACPI_DEV_SCOPE_STRUCTURE));
        Offset += sizeof(EFI_ACPI_DEV_SCOPE_STRUCTURE);
      }
    }

    if ((Offset > EFI_ACPI_DRHD_ENGINE_HEADER_LENGTH) || (DrhdEngine->DrhdHeader.Flags == 0x01)) {
      DrhdEngine->DrhdHeader.Header.Length = Offset;
    }
    else  {
      DrhdEngine->DrhdHeader.Header.Length = 0;
    }
  }
  else {
    DrhdEngine->DrhdHeader.Header.Length = 0;
  }
}

/**
  Update the Global/IOP VTD DRHD structure

  @param[in, out] DrhdEnginePtr       - A pointer to DRHD structure
**/
VOID
UpdateDrhd3 (
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

  return GLOBAL_VTD;
}
/**
  Update the Satc structure

  @param[in, out] Satc             - A pointer to Satc structure
**/
VOID
UpdateSatc (
  IN OUT EFI_ACPI_SATC_STRUCT *Satc
  )
{
  UINT16                  Offset;
  BOOLEAN                 NeedRemove;
  UINT32                  VIDDID;
  UINTN                   DeviceScopeNum;
  UINTN                   Index;
  VTD_ENGINE              VtdEngine;
  UINT32                  VtdBase;
  UINT64                  Ecap;

  DEBUG((DEBUG_INFO, "%a(): Enter\n", __FUNCTION__));
  if (Satc == NULL) {
    return;
  }

  Offset              = sizeof(Satc->SatcHeader);
  DeviceScopeNum      = (Satc->SatcHeader.Header.Length - sizeof (EFI_ACPI_DMAR_SATC_HEADER)) / sizeof (EFI_ACPI_DEV_SCOPE_STRUCTURE);

  for (Index = 0; Index < DeviceScopeNum; Index++) {
    NeedRemove = FALSE;

    VIDDID = PciRead32 (PCI_LIB_ADDRESS (0, Satc->DeviceScope[Index].PciPath.Device,
                                  Satc->DeviceScope[Index].PciPath.Function,
                                  0x00));
    if (VIDDID == 0xFFFFFFFF) {
      DEBUG((DEBUG_INFO, " Device Needs to be removed Device 0x%x Function: %x",
                        Satc->DeviceScope[Index].PciPath.Device,
                        Satc->DeviceScope[Index].PciPath.Function));
      NeedRemove = TRUE;
    }

    VtdEngine = GetVtdEngineForPciDevice (0,
                                          Satc->DeviceScope[Index].PciPath.Device,
                                          Satc->DeviceScope[Index].PciPath.Function);
    if (!IsVtdEngineEnabled (VtdEngine)) {
        NeedRemove = TRUE;
      }
    VtdBase   = ReadVtdBaseAddress (VtdEngine);
    Ecap = MmioRead64 (VtdBase + ECAP_VTD_IOMMU_REG);
    if ((Ecap & B_ECAP_DEVICE_TLB_SUPPORT)  == 0) {
      NeedRemove = TRUE;
    }

    DEBUG ((DEBUG_INFO, "VtdBase 0x%08X, Ecap 0x%16llX\n", VtdBase, Ecap));

    if (!NeedRemove) {
      CopyMem (
        &((UINT8*)Satc)[Offset],
        &Satc->DeviceScope[Index],
        sizeof (EFI_ACPI_DEV_SCOPE_STRUCTURE)
      );
      Offset += sizeof(EFI_ACPI_DEV_SCOPE_STRUCTURE);
    }
  }
  ///
  /// If No deviceScope is left, set length as 0x00
  ///
  if (Offset > sizeof (EFI_ACPI_DMAR_SATC_HEADER)) {
    Satc->SatcHeader.Header.Length = Offset;
  } else {
    Satc->SatcHeader.Header.Length = 0;
  }
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

/**
  Update the Sidp structure

  @param[in, out] Sidp             - A pointer to Sidp structure
**/

VOID
UpdateSidp (
  IN OUT EFI_ACPI_SIDP_STRUCT *Sidp
  )
{
  UINT16                  Offset;
  UINT32                  VIDDID;
  UINTN                   DeviceScopeNum;
  UINTN                   Index;
  BOOLEAN                 NeedRemove;
  VTD_ENGINE              VtdEngine;
  UINT32                  VtdBase;
  UINT64                  Ecap;

  DEBUG((DEBUG_INFO, "%a(): Enter\n", __FUNCTION__));

  if (Sidp == NULL) {
    return;
  }

  Offset              = sizeof(Sidp->SidpHeader);
  DeviceScopeNum      = (Sidp->SidpHeader.Header.Length - sizeof (EFI_ACPI_DMAR_SIDP_HEADER)) / sizeof (EFI_ACPI_DEV_SCOPE_STRUCTURE);

  for (Index = 0; Index < DeviceScopeNum; Index++) {
    NeedRemove = FALSE;

    VIDDID = PciRead32 (PCI_LIB_ADDRESS (0, Sidp->DeviceScope[Index].PciPath.Device,
                                            Sidp->DeviceScope[Index].PciPath.Function,
                                            0x00));
    if (VIDDID == 0xFFFFFFFF) {
      DEBUG((DEBUG_INFO, " Device Needs to be removed Device 0x%x Function: %x\n",
                        Sidp->DeviceScope[Index].PciPath.Device,  Sidp->DeviceScope[Index].PciPath.Function));
      NeedRemove = TRUE;
    }

    Sidp->DeviceScope[Index].DeviceScopeStructureHeader.Flags = GetPropertiesForPciDevice (
                                                                  0,
                                                                  Sidp->DeviceScope[Index].PciPath.Device,
                                                                  Sidp->DeviceScope[Index].PciPath.Function
                                                                  );
    //
    // For each PCIe device from SIDP device scope, there is a need to check whether its IOMMU is enabled.
    // If not, then this device must be removed from SIDP device scope.
    //
    VtdEngine = GetVtdEngineForPciDevice (0, Sidp->DeviceScope[Index].PciPath.Device, Sidp->DeviceScope[Index].PciPath.Function);
    if (!IsVtdEngineEnabled (VtdEngine)) {
      NeedRemove = TRUE;
    }

    VtdBase   = ReadVtdBaseAddress (VtdEngine);
    Ecap = MmioRead64 (VtdBase + ECAP_VTD_IOMMU_REG);

    //
    // If IOMMU doesn't support Device-TLBs, then respective devices mustn't be published in SIDP table
    //
    if ((Ecap & B_ECAP_DEVICE_TLB_SUPPORT) == 0) {
      NeedRemove = TRUE;
    }

    if (!NeedRemove) {
      CopyMem (
        &((UINT8*)Sidp)[Offset],
        &Sidp->DeviceScope[Index],
        sizeof (EFI_ACPI_DEV_SCOPE_STRUCTURE)
        );
      Offset += sizeof(EFI_ACPI_DEV_SCOPE_STRUCTURE);
      }
  }
  ///
  /// If no deviceScope is left, set length as 0x00
  ///
  if (Offset > sizeof (EFI_ACPI_DMAR_SIDP_HEADER)) {
    Sidp->SidpHeader.Header.Length = Offset;
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
  return SIZE_64KB;
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
  UINT32              Mask = BIT0;

  if (VtdEngineNumber < VTD_ENGINE_NUMBER) {
    McD0BaseAddress = PCI_LIB_ADDRESS (SA_MC_BUS, 0, 0, 0);
    MchBar          = PciRead32 ((UINTN)McD0BaseAddress + R_SA_MCHBAR) & (~BIT0);
    Mask            = ~(( Mask << VTD_ENGINE_NUMBER ) -1 );
    return ( (MmioRead32 (MchBar + R_MCHBAR_VTD1_OFFSET) & Mask) + (VtdEngineNumber*GetVtdBarSize()) );
    }
  else {
    return 0x0;
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
  MEMORY_CFG_DATA     *MemCfgData;

  MemCfgData = (MEMORY_CFG_DATA *)FindConfigDataByTag (CDATA_MEMORY_TAG);
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
  /// Update DRHD structures of DmarTable
  ///
  DmarTable->DrhdEngine1.DrhdHeader.RegisterBaseAddress = ReadVtdBaseAddress(GFX_VTD);
  DmarTable->DrhdEngine2.DrhdHeader.RegisterBaseAddress = ReadVtdBaseAddress(NON_GFX_VTD);
  DmarTable->DrhdEngine3.DrhdHeader.RegisterBaseAddress = ReadVtdBaseAddress(IOP_VTD);

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
  UpdateDrhd1 (&TempDmarTable.DrhdEngine1);
  UpdateDrhd2 (&TempDmarTable.DrhdEngine2);
  UpdateDrhd3 (&TempDmarTable.DrhdEngine3);
  /// Update Satc Structure
  UpdateSatc  (&TempDmarTable.Satc);
  /// Update Sidp Structure
  UpdateSidp (&TempDmarTable.Sidp);

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

  #ifdef PRINT_DMAR_TABLE
  DumpAcpiDMAR ( (VOID *) DmarTable);
  #endif
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
