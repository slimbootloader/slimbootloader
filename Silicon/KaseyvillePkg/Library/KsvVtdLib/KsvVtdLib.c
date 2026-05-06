/** @file

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Base.h>
#include <Library/DebugLib.h>
#include <Library/KsvVtdLib.h>
#include <IndustryStandard/DmaRemappingReportingTable.h>
#include <Library/HobLib.h>
#include <Library/PciExpressLib.h>
#include <PlatformBase.h>
#include <Library/BootloaderCoreLib.h>
#include <Library/BootloaderCommonLib.h>
#include <Register/PchRegsP2sb.h>
#include <CpuDataStruct.h>
#include <IioUniversalDataHob.h>
#include <Library/PciLib.h>
#include <Library/ConfigDataLib.h>
#include <ConfigDataDefs.h>
#include <Library/DmarLib.h>

#define MAX_IIO_STACKS_PER_SOCKET      MAX_IIO_STACK
#define MAX_IOU_NUM_PER_SOCKET         8
#define MAX_STACK_NUM_PER_SOCKET       12
#define MAX_IIO_PORTS_PER_STACK        8

//
// Device and function numbers for global IEH device
//
#define IIO_GLOBAL_IEH_PCI_DEV         2
#define IIO_GLOBAL_IEH_PCI_FUN         0

//
// Device and function numbers for IOAT devices.
//
#define IIO_DSA_PCI_DEV            1
#define IIO_DSA_PCI_FUN            0
#define IIO_IAX_PCI_DEV            2
#define IIO_IAX_PCI_FUN            0

#define CPM_BUS_OFFSET             0x01
#define CPM1_BUS_OFFSET            0x05

typedef struct {
  UINT8         Device;
  UINT8         Function;
} PCI_NODE;

typedef struct {
  UINT8         DeviceType;
  UINT8         EnumerationID;
  UINT8         StartBusNumber;
  CONST PCI_NODE *PciNode;
} DEVICE_SCOPE;

typedef enum {
  IioPortA = 0,
  IioPortB = 1,
  IioPortC = 2,
  IioPortD = 3,
  IioPortE = 4,
  IioPortF = 5,
  IioPortG = 6,
  IioPortH = 7
} IIO_PORT;

//
// BDF parts (device/func.) of rootports
//
#define DEV_FUNC_TO_PORT                            \
{                                                   \
/*{ IioPortX,Dev,Fun}*/                             \
  { IioPortA,  2, 0 },  /* x16 pcie rp0 / portA */  \
  { IioPortB,  3, 0 },  /* x8  pcie rp0 / portB */  \
  { IioPortC,  4, 0 },  /* x16 pcie rp1 / portC */  \
  { IioPortD,  5, 0 },  /* x8  pcie rp1 / portD */  \
  { IioPortE,  6, 0 },  /* x16 pcie rp2 / portE */  \
  { IioPortF,  7, 0 },  /* x8  pcie rp2 / portF */  \
  { IioPortG,  8, 0 },  /* x16 pcie rp3 / portG */  \
  { IioPortH,  9, 0 },  /* x8  pcie rp4 / portH */  \
}

typedef struct {
  UINT8         Port;
  UINT8         Device;
  UINT8         Function;
} PORT_TO_DEV_FUNC;

CONST PORT_TO_DEV_FUNC  mPortToDevFunc[] = DEV_FUNC_TO_PORT;
//
//  PciePath for GNRD IOAT for HCC
//
CONST PCI_NODE  mPciPath2_0_GnrdIoat[] = {
  { IIO_DSA_PCI_DEV, IIO_DSA_PCI_FUN },
  { (UINT8)-1, (UINT8)-1 },
};
CONST PCI_NODE  mPciPath2_1_GnrdIoat[] = {
  { IIO_IAX_PCI_DEV, IIO_IAX_PCI_FUN },
  { (UINT8)-1, (UINT8)-1 },
};
CONST PCI_NODE  mPciPath2_2_GnrdIoat[] = {
  { 0,0 },
  { (UINT8)-1, (UINT8)-1 },
};
CONST PCI_NODE  mPciPath2_3_GnrdIoat[] = {
  { 0, 0 },
  { (UINT8)-1, (UINT8)-1 },
};

CONST DEVICE_SCOPE AcceleratorDevices[] = {
  {
     EFI_ACPI_DEVICE_SCOPE_ENTRY_TYPE_PCI_ENDPOINT,
     0,
     0,
     &mPciPath2_0_GnrdIoat[0]
  },
  {
     EFI_ACPI_DEVICE_SCOPE_ENTRY_TYPE_PCI_ENDPOINT,
     0,
     0,
     &mPciPath2_1_GnrdIoat[0]
  },
  {
     EFI_ACPI_DEVICE_SCOPE_ENTRY_TYPE_PCI_ENDPOINT,
     0,
     CPM_BUS_OFFSET,
     &mPciPath2_2_GnrdIoat[0]
  },
  {
     EFI_ACPI_DEVICE_SCOPE_ENTRY_TYPE_PCI_ENDPOINT,
     0,
     CPM1_BUS_OFFSET,
     &mPciPath2_3_GnrdIoat[0]
  },
};

UINTN   mAcceleratorDevicesSize = sizeof (AcceleratorDevices) / (sizeof (DEVICE_SCOPE));

/**
  Append DRHD structures to DMAR ACPI table (which represents a remapping hardware
  unit present in the platform) for each present Stack. In case of Stack0 of the
  legacy Socket it adds HPET and IOAPIC devices to device scope. For others stacks,
  the function adds DINO, CXL1.1, PCIe RP and IOE devices to device scope structure.

  @param[in, out] AcpiHeader        Pointer to the ACPI table header to which the DRHD
                                    structure will be appended.
  @param[in]      Stack             Current Stack to add DRHD for.
  @param[in]      IioUdsHobData     Pointer to the Iio Uds info hob.

  @retval         EFI_SUCCESS,          if DRHD is appended successfully
  @retval         EFI_INVALID_PARAMETER,if CurrentPtr/DrhdLen/IioUdsHobData is NULL
**/
STATIC
EFI_STATUS
AppendDrhd (
  IN OUT  EFI_ACPI_DESCRIPTION_HEADER   *AcpiHeader,
  IN      UINT8                         Stack,
  IN      IIO_UDS                       *IioUdsHobData
  )
{
  UINT8                                 Port;
  UINT16                                NumberOfHpets;
  UINT16                                HpetCapIdValue;
  UINT8                                 PciBus;
  UINT8                                 PciDev;
  UINT8                                 PciFun;
  UINT8                                 PciRootIndex;
  UINT64                                VtdBase;
  EFI_ACPI_DMAR_STRUCTURE_HEADER        *DmarHdr;
  UINT8                                 DevScopeType;

  DEBUG ((DEBUG_VERBOSE, "AppendDrhd\n"));

  if ((AcpiHeader == NULL) || (IioUdsHobData == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  VtdBase = IioUdsHobData->PlatformData.IIO_resource[0].StackRes[Stack].VtbarAddress;

  DEBUG ((DEBUG_VERBOSE, "VtdBase 0x%llx\n", VtdBase));
  DmarHdr = AddDrhdHdr (AcpiHeader, EFI_ACPI_DMAR_DRHD_FLAGS_INCLUDE_ALL_CLEAR, SIZE_64KB, 0, VtdBase);
  if (DmarHdr == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Create PCH IOAPIC and HPET for stack 0 items
  //
  if (Stack == IIO_STACK0) {
    AddScopeData (AcpiHeader, DmarHdr, EFI_ACPI_DEVICE_SCOPE_ENTRY_TYPE_IOAPIC, 0, PCH_IOAPIC_ID, V_P2SB_CFG_IBDF_BUS, V_P2SB_CFG_IBDF_DEV, V_P2SB_CFG_IBDF_FUNC);
    //
    // HPET
    //
    HpetCapIdValue = *(UINT16 *)(UINTN)(HPET_BLOCK_ADDRESS);
    NumberOfHpets  = (HpetCapIdValue >> 0x08) & 0x1F;  // Bits [8:12] contains the number of Hpets
    if (NumberOfHpets && (NumberOfHpets != 0x1F) &&
      (*((volatile UINT32 *)(UINTN)(HPET_BLOCK_ADDRESS + 0x100)) & BIT15)) { // FSB Interrupt Delivery Capability
      AddScopeData (AcpiHeader, DmarHdr, EFI_ACPI_DEVICE_SCOPE_ENTRY_TYPE_MSI_CAPABLE_HPET, 0, 0, V_P2SB_CFG_HBDF_BUS, V_P2SB_CFG_HBDF_DEV, V_P2SB_CFG_HBDF_FUNC);
    }
  }

  //
  // Calculate primary PciBus # for this Stack.
  //
  for (PciRootIndex = 0; PciRootIndex < IioUdsHobData->PlatformData.IIO_resource[0].StackRes[Stack].PciRootBridgeNum; PciRootIndex++) {
    PciBus = IioUdsHobData->PlatformData.IIO_resource[0].StackRes[Stack].PciRoot[PciRootIndex].BusBase;
    for (PciDev = 0; PciDev <= PCI_MAX_DEVICE; PciDev++) {
      for (PciFun = 0; PciFun <= PCI_MAX_FUNC; PciFun++) {
        if (PciRead32 (PCI_LIB_ADDRESS (PciBus, PciDev, PciFun, 0x00)) == 0xFFFFFFFF) {
          if (PciFun != 0) {
            continue; // go to next function
          } else {
            break;    // if no function 0 then go to next device
          }
        }
        if (((PciRead8 (PCI_LIB_ADDRESS (PciBus, PciDev, PciFun, 0x0E))) & 0x7F) == 1) {
          if (IioUdsHobData->PlatformData.IIO_resource[0].StackRes[Stack].Personality == 2) { // IIO type Personality
            continue;
          }
            DevScopeType = EFI_ACPI_DEVICE_SCOPE_ENTRY_TYPE_PCI_BRIDGE;
        } else {
          DevScopeType = EFI_ACPI_DEVICE_SCOPE_ENTRY_TYPE_PCI_ENDPOINT;
        }
        AddScopeData (AcpiHeader, DmarHdr, DevScopeType, 0, 0, PciBus, PciDev, PciFun);
      }
    }
  }

  for (Port = 0; Port < MAX_IIO_PORTS_PER_STACK; Port++) {
    if (IioUdsHobData->PlatformData.IIO_resource[0].StackRes[Stack].Personality != 2) {    // IIO type Personality
      continue;
    }
    PciBus = IioUdsHobData->PlatformData.IIO_resource[0].StackRes[Stack].BusBase;
    PciDev = mPortToDevFunc[Port].Device;
    PciFun = mPortToDevFunc[Port].Function;
    if (!(((PciRead8 (PCI_LIB_ADDRESS (PciBus, PciDev, PciFun, 0x0E))) & 0x7F) == 1)) {    // PCI Bridge
      continue;
    }
    if (PciExpressRead32 (PCI_EXPRESS_LIB_ADDRESS (PciBus, PciDev, PciFun, 0)) == 0xFFFFFFFF) {
      continue;
    }
    AddScopeData (AcpiHeader, DmarHdr, EFI_ACPI_DEVICE_SCOPE_ENTRY_TYPE_PCI_BRIDGE, 0, 0, PciBus, PciDev, PciFun);
  } // PCI Port loop

  if ((Stack == IIO_STACK0) && (IioUdsHobData->PlatformData.IIO_resource[0].StackRes[MAX_LOGIC_IIO_STACK - 1].Personality == 0)) { // UBOX type Personality
    AddScopeData (AcpiHeader,
                  DmarHdr,
                  EFI_ACPI_DEVICE_SCOPE_ENTRY_TYPE_PCI_ENDPOINT,
                  0,
                  0,
                  IioUdsHobData->PlatformData.IIO_resource[0].StackRes[MAX_LOGIC_IIO_STACK - 1].BusBase,
                  IIO_GLOBAL_IEH_PCI_DEV,
                  IIO_GLOBAL_IEH_PCI_FUN
                );
  }

  return EFI_SUCCESS;
}

/**
  Append SoC Integrated Address Translation Cache (SATC) reporting structures
  to DMAR ACPI table. The Device Scope structure contains Device Scope Entries
  that identifies SoC integrated devices (in the specified PCI segment) with
  address translation caches (ATC).

  @param[in, out] AcpiHeader             Pointer to the ACPI table header to which the DRHD
                                         structure will be appended.

  @retval         EFI_SUCCESS,           if SATC is appended successfully
  @retval         EFI_INVALID_PARAMETER, if AcpiHeader is NULL
**/
STATIC
EFI_STATUS
AppendSatc (
  IN OUT   EFI_ACPI_DESCRIPTION_HEADER       *AcpiHeader
  )
{
  UINT8                                      PciBus;
  UINT8                                      PciDev;
  UINT8                                      PciFun;
  UINT32                                     Address;
  UINT8                                      IoatIdx;
  EFI_ACPI_DMAR_STRUCTURE_HEADER             *DmarHdr;

  DEBUG ((DEBUG_VERBOSE, "AppendSatc\n"));

  if (AcpiHeader == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  DmarHdr = AddSatcHdr (AcpiHeader, EFI_ACPI_DMAR_SATC_FLAGS_ATC_REQUIRED_CLEAR, 0);
  //
  // Calculate primary PciBus # for this Stack.
  //
  for (IoatIdx = 0; IoatIdx < mAcceleratorDevicesSize; IoatIdx++) {
    //
    // Get Bus/Dev/Func for GNRD Ioat
    //
    PciBus = AcceleratorDevices[IoatIdx].StartBusNumber;
    PciDev = AcceleratorDevices[IoatIdx].PciNode->Device;
    PciFun = AcceleratorDevices[IoatIdx].PciNode->Function;

    //
    // Check if the is hidden (skip) or available (proceed)
    //
    Address = PCI_EXPRESS_LIB_ADDRESS (PciBus, PciDev, PciFun, 0);
    if (PciExpressRead32 (Address) == 0xFFFFFFFF) {
      continue;
    }

    AddScopeData (AcpiHeader, DmarHdr, EFI_ACPI_DEVICE_SCOPE_ENTRY_TYPE_PCI_ENDPOINT, 0, 0, PciBus, PciDev, PciFun);
  }

  return EFI_SUCCESS;
}

/**
  Append SoC Integrated Device Property (SIDP) reporting structures to
  DMAR ACPI table. The Device Scope structure contains Device Scope Entries
  that identifies SoC integrated devices (in the specified PCI segment) with
  special properties/restrictions that system software needs to be aware of.

  @param[in, out] AcpiHeader             Pointer to the ACPI table header to which the DRHD
                                         structure will be appended.

  @retval         EFI_SUCCESS,           if SIDP is appended successfully
  @retval         EFI_INVALID_PARAMETER, if AcpiHeader is NULL
**/
STATIC
EFI_STATUS
AppendSidp (
  IN OUT   EFI_ACPI_DESCRIPTION_HEADER       *AcpiHeader
  )
{
  UINT8                                      PciBus;
  UINT8                                      PciDev;
  UINT8                                      PciFun;
  UINT32                                     Address;
  UINT8                                      IoatIdx;
  EFI_ACPI_DMAR_STRUCTURE_HEADER             *DmarHdr;

  DEBUG ((DEBUG_VERBOSE, "AppendSidp\n"));

  if (AcpiHeader == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  DmarHdr = AddSidpHdr (AcpiHeader, 0);
  //
  // Calculate primary PciBus # for this Stack.
  //
  for (IoatIdx = 0; IoatIdx < mAcceleratorDevicesSize; IoatIdx++) {
    //
    // Get Bus/Dev/Func for GNRD Ioat
    //
    PciBus = AcceleratorDevices[IoatIdx].StartBusNumber;
    PciDev = AcceleratorDevices[IoatIdx].PciNode->Device;
    PciFun = AcceleratorDevices[IoatIdx].PciNode->Function;

    //
    // Check if the is hidden (skip) or available (proceed)
    //
    Address = PCI_EXPRESS_LIB_ADDRESS (PciBus, PciDev, PciFun, 0);
    if (PciExpressRead32 (Address) == 0xFFFFFFFF) {
      continue;
    }

    AddScopeData (AcpiHeader, DmarHdr, EFI_ACPI_DEVICE_SCOPE_ENTRY_TYPE_PCI_ENDPOINT, EFI_ACPI_DEVICE_SCOPE_FLAG_ATC_HARDENED, 0, PciBus, PciDev, PciFun);
  }
  return EFI_SUCCESS;
}

/**
  Update DMAR ACPI table

  Only the header is found until now.
  Append the rest of table here.

  @param[in, out] AcpiHeader   Start address of DMAR ACPI table header.

  @retval         EFI_SUCCESS, if DMAR update successful.
                  Status,      if error in update.
**/
EFI_STATUS
EFIAPI
UpdateDmarTable (
  IN OUT   EFI_ACPI_DESCRIPTION_HEADER       *AcpiHeader
  )
{
  UINT8                                Stack;
  EFI_STATUS                           Status;
  EFI_HOB_GUID_TYPE                    *GuidHob;
  IIO_UDS                              *IioUdsHobData;
  VOID                                 *FspHobList;
  UINT32                               StackPresentBitmap;
  MISC_CFG_DATA                        *MiscCfgData;
  UINT8                                DmarTableFlags;
  EFI_ACPI_DMAR_STRUCTURE_HEADER       *DmarHdr;

  DEBUG ((DEBUG_VERBOSE, "UpdateDmarTable\n"));
  Status = EFI_SUCCESS;

  GuidHob = NULL;
  FspHobList = GetFspHobListPtr ();
  if (FspHobList != NULL) {
    GuidHob = GetNextGuidHob (&gIioUdsHobGuid, FspHobList);
  }
  if (GuidHob == NULL) {
    DEBUG ((DEBUG_ERROR, "Iio Data Hob not found!\n"));
    Status = EFI_UNSUPPORTED;
    return Status;
  }
  IioUdsHobData = GET_GUID_HOB_DATA (GuidHob);

  MiscCfgData = (MISC_CFG_DATA *) FindConfigDataByTag (CDATA_MISC_TAG);
  if (MiscCfgData != NULL) {

    DmarTableFlags = 0;

    if (MiscCfgData->VtdConfig.DmaCtrlOptIn) {
      DmarTableFlags  |= EFI_ACPI_DMAR_TABLE_FLAGS_DMA_CTRL_PLATFORM_OPT_IN_FLAG;
    }

    //DmarTableFlags  |= EFI_ACPI_DMAR_TABLE_FLAGS_X2APIC_OPT_OUT; // On X2APIC, 0
    DmarTableFlags  |= EFI_ACPI_DMAR_TABLE_FLAGS_INTR_REMAP_SET; // Always 1
    DEBUG ((DEBUG_VERBOSE, "DmarTableFlags 0x%X\n", DmarTableFlags));

  } else {
    DEBUG ((DEBUG_WARN, "No Misc CFGDATA. Continue to boot with default values!\n"));
    DmarTableFlags = EFI_ACPI_DMAR_TABLE_FLAGS_DMA_CTRL_PLATFORM_OPT_IN_FLAG | EFI_ACPI_DMAR_TABLE_FLAGS_INTR_REMAP_SET;
  }

  // DMAR ACPI table header
  Status = AddAcpiDmarHdr (AcpiHeader, DmarTableFlags);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // DRHD
  //
  // For Stacks 1 to MAX_IIO_STACKS_PER_SOCKET
  for (Stack = 1; Stack < MAX_IIO_STACKS_PER_SOCKET; Stack++) {
    StackPresentBitmap = IioUdsHobData->PlatformData.CpuQpiInfo[0].StackPresentBitmap;
    if (!(StackPresentBitmap & (BIT0 << Stack))) {
      continue;
    }
    Status   = AppendDrhd (AcpiHeader, Stack, IioUdsHobData);
    if (EFI_ERROR(Status)) {
      DEBUG ((DEBUG_ERROR, "Append DRHD failed, returning!\n"));
      return Status;
    }
  }

  // For Stack 0
  Stack    = 0;
  Status   = AppendDrhd (AcpiHeader, Stack, IioUdsHobData);
  if (EFI_ERROR(Status)) {
    DEBUG ((DEBUG_ERROR, "Append DRHD failed, returning!\n"));
    return Status;
  }

  //
  // ATSR
  //
  DmarHdr = AddAtsrHdr (AcpiHeader, EFI_ACPI_DMAR_ATSR_FLAGS_ALL_PORTS, 0);
  if (DmarHdr == NULL) {
    DEBUG ((DEBUG_ERROR, "Append ATSR failed, returning!\n"));
    return EFI_OUT_OF_RESOURCES;
  }

  if (MiscCfgData != NULL) {
    DEBUG ((DEBUG_ERROR, "Misc CFGDATA found. Update DMAR table based on CFGDATA!\n"));
    //
    // RHSA
    //
    if (MiscCfgData->VtdConfig.RhsaSupport) {
      DEBUG ((DEBUG_ERROR, "AppendRhsaHdr\n"));
        for (Stack = 0; Stack < MAX_IIO_STACKS_PER_SOCKET; Stack++) {
          StackPresentBitmap = IioUdsHobData->PlatformData.CpuQpiInfo[0].StackPresentBitmap;
          if (!(StackPresentBitmap & (BIT0 << Stack))) {
            continue;
          }
          DmarHdr = AddRhsaHdr (AcpiHeader, IioUdsHobData->PlatformData.IIO_resource[0].StackRes[Stack].VtbarAddress, 0);
          if (DmarHdr == NULL) {
            DEBUG ((DEBUG_ERROR, "Append RHSA failed, returning!\n"));
            return EFI_OUT_OF_RESOURCES;
          }
        }
    }

    //
    // SATC
    //
    if (MiscCfgData->VtdConfig.SatcSupport) {
      Status = AppendSatc (AcpiHeader);
      if (EFI_ERROR(Status)) {
        DEBUG ((DEBUG_ERROR, "Append SATC failed, returning!\n"));
        return Status;
      }
    }

    //
    // SIDP
    //
    if (MiscCfgData->VtdConfig.SidpSupport) {
      Status = AppendSidp (AcpiHeader);
      if (EFI_ERROR(Status)) {
        DEBUG ((DEBUG_ERROR, "Append SIDP failed, returning!\n"));
        return Status;
      }
    }
  }

  return Status;
}
