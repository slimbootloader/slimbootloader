/** @file

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Base.h>
#include <Library/DebugLib.h>
#include <Library/VTdLib.h>
#include <Library/DmaRemap.h>
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
     EFI_ACPI_DEVICE_SCOPE_ENTRY_TYPE_ENDPOINT,
     0,
     0,
     &mPciPath2_0_GnrdIoat[0]
  },
  {
     EFI_ACPI_DEVICE_SCOPE_ENTRY_TYPE_ENDPOINT,
     0,
     0,
     &mPciPath2_1_GnrdIoat[0]
  },
  {
     EFI_ACPI_DEVICE_SCOPE_ENTRY_TYPE_ENDPOINT,
     0,
     CPM_BUS_OFFSET,
     &mPciPath2_2_GnrdIoat[0]
  },
  {
     EFI_ACPI_DEVICE_SCOPE_ENTRY_TYPE_ENDPOINT,
     0,
     CPM1_BUS_OFFSET,
     &mPciPath2_3_GnrdIoat[0]
  },
};

UINTN   mAcceleratorDevicesSize = sizeof (AcceleratorDevices) / (sizeof (DEVICE_SCOPE));

/**
  Append DRHD structures to DMAR ACPI table (which represents a remapping hardware
  unit present in the platform) for each present Stack. In case of Stack0 of the
  legacy Socket it adds HPET and IOAPIC devices to device scope, and sets INCLUDE_PCI_ALL
  flag. For others stacks, the function adds DINO, CXL1.1, PCIe RP and IOE
  devices to device scope structure.

  @param[in,out]  CurrentPtr            The address to start adding DRHD.
  @param[in]      Stack                 Current STack to add DRHD for.
  @param[in,out]  DrhdLen               Length of DRHD struct addded.
  @param[in]      IioUdsHobData         Pointer to the Iio Uds info hob.

  @retval         EFI_SUCCESS,          if DRHD is appended successfully
  @retval         EFI_INVALID_PARAMETER,if CurrentPtr/DrhdLen/IioUdsHobData is NULL
**/
EFI_STATUS
AppendDrhd (
  IN OUT VOID                **CurrentPtr,
  IN     UINT8               Stack,
  IN OUT UINT16              *DrhdLen,
  IN IIO_UDS                 *IioUdsHobData
  )
{
  UINT8                      Port;
  UINT16                     NumberOfHpets;
  UINT16                     HpetCapIdValue;
  EFI_ACPI_DMAR_HARDWARE_UNIT_DEFINITION_STRUCTURE *Drhd;
  EFI_ACPI_DMAR_DEVICE_SCOPE_ENTRY_STRUCTURE       *DevScope;
  EFI_ACPI_DMAR_PCI_PATH                           *PciPath;
  UINT8                      PciBus;
  UINT8                      PciDev;
  UINT8                      PciFun;
  UINT8                      PciRootIndex;
  UINT64                     VtdBase;

  DEBUG ((DEBUG_VERBOSE, "AppendDrhd\n"));

  if ((*CurrentPtr == NULL) || (DrhdLen == NULL) || (IioUdsHobData == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  Drhd = (EFI_ACPI_DMAR_HARDWARE_UNIT_DEFINITION_STRUCTURE *)*CurrentPtr;

  VtdBase = IioUdsHobData->PlatformData.IIO_resource[0].StackRes[Stack].VtbarAddress;
  //
  // Initialize the Drhd structure
  //
  Drhd->Type                 = EFI_ACPI_DMA_REMAPPING_STRUCTURE_TYPE_DRHD;
  Drhd->Length               = sizeof (EFI_ACPI_DMAR_HARDWARE_UNIT_DEFINITION_STRUCTURE);
  Drhd->Flags                = EFI_ACPI_DMAR_DRHD_FLAGS_INCLUDE_ALL_CLEAR;
  Drhd->SegmentNumber        = 0;
  Drhd->Size                 = 4;
  Drhd->RegisterBaseAddress  = VtdBase;
  DEBUG ((DEBUG_VERBOSE, "VtdBase 0x%x\n", VtdBase));

  //
  // Create PCH IOAPIC and HPET for stack 0 items
  //
  if (Stack == IIO_STACK0) {
    //
    // Add the PCH IOAPIC and HPET device scopes
    // (if InterruptRemap is enabled)
    //

    //
    // PCH IOAPIC
    //
    DevScope = (EFI_ACPI_DMAR_DEVICE_SCOPE_ENTRY_STRUCTURE *) ((UINT8 *)Drhd + Drhd->Length);
    DevScope->DeviceScopeEntryType     = EFI_ACPI_DEVICE_SCOPE_ENTRY_TYPE_IOAPIC;
    DevScope->Length                   = sizeof(EFI_ACPI_DMAR_DEVICE_SCOPE_ENTRY_STRUCTURE);
    DevScope->EnumerationID            = PCH_IOAPIC_ID;
    DevScope->StartingBusNumber        = V_P2SB_CFG_IBDF_BUS;

    PciPath = (EFI_ACPI_DMAR_PCI_PATH *) ((UINT8 *)DevScope + DevScope->Length);
    PciPath->Device                    = V_P2SB_CFG_IBDF_DEV;
    PciPath->Function                  = V_P2SB_CFG_IBDF_FUNC;
    DevScope->Length                   += sizeof(EFI_ACPI_DMAR_PCI_PATH);

    DEBUG ((DEBUG_VERBOSE, "[DMAR][DRHD-IOAP] Type:%d {S%d} (B%02X:D%02X:F%d)\n", DevScope->DeviceScopeEntryType,
                                                                               Stack,
                                                                               DevScope->StartingBusNumber,
                                                                               PciPath->Device,
                                                                               PciPath->Function));
    Drhd->Length += DevScope->Length;
    DEBUG ((DEBUG_VERBOSE, "Drhd->Length in PCH IOAPIC 0x%X\n", Drhd->Length));
    //
    // HPET
    //
    HpetCapIdValue = *(UINT16 *)(UINTN)(HPET_BLOCK_ADDRESS);
    NumberOfHpets  = (HpetCapIdValue >> 0x08) & 0x1F;  // Bits [8:12] contains the number of Hpets
    if (NumberOfHpets && (NumberOfHpets != 0x1F) &&
      (*((volatile UINT32 *)(UINTN)(HPET_BLOCK_ADDRESS + 0x100)) & BIT15)) { // FSB Interrupt Delivery Capability

      DevScope = (EFI_ACPI_DMAR_DEVICE_SCOPE_ENTRY_STRUCTURE *) ((UINT8 *)Drhd + Drhd->Length);
      DevScope->DeviceScopeEntryType   = EFI_ACPI_DEVICE_SCOPE_ENTRY_TYPE_MSI_CAPABLE_HPET;
      DevScope->Length                 = sizeof(EFI_ACPI_DMAR_DEVICE_SCOPE_ENTRY_STRUCTURE);
      DevScope->EnumerationID          = 0;
      DevScope->StartingBusNumber      = V_P2SB_CFG_HBDF_BUS;

      PciPath = (EFI_ACPI_DMAR_PCI_PATH *) ((UINT8 *)DevScope + DevScope->Length);
      PciPath->Device                  = V_P2SB_CFG_HBDF_DEV;
      PciPath->Function                = V_P2SB_CFG_HBDF_FUNC;
      DevScope->Length                 += sizeof(EFI_ACPI_DMAR_PCI_PATH);

      DEBUG ((DEBUG_VERBOSE, "[DMAR][DRHD-HPET] Type:%d {S%d} (B%02X:D%02X:F%d)\n", DevScope->DeviceScopeEntryType,
                                                                                 Stack,
                                                                                 DevScope->StartingBusNumber,
                                                                                 PciPath->Device,
                                                                                 PciPath->Function));
      Drhd->Length += DevScope->Length;
      DEBUG ((DEBUG_VERBOSE, "Drhd->Length in HPET 0x%X\n", Drhd->Length));
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
          if (IioUdsHobData->PlatformData.IIO_resource[0].StackRes[Stack].Personality == 2) {
            continue;
          }
            DevScope = (EFI_ACPI_DMAR_DEVICE_SCOPE_ENTRY_STRUCTURE *) ((UINT8 *)Drhd + Drhd->Length);
            DevScope->DeviceScopeEntryType = EFI_ACPI_DEVICE_SCOPE_ENTRY_TYPE_BRIDGE;
        } else {
          DevScope = (EFI_ACPI_DMAR_DEVICE_SCOPE_ENTRY_STRUCTURE *) ((UINT8 *)Drhd + Drhd->Length);
          DevScope->DeviceScopeEntryType = EFI_ACPI_DEVICE_SCOPE_ENTRY_TYPE_ENDPOINT;
        }

        DevScope->Length               = sizeof(EFI_ACPI_DMAR_DEVICE_SCOPE_ENTRY_STRUCTURE);
        DevScope->EnumerationID        = 0;
        DevScope->StartingBusNumber    = PciBus;
        PciPath = (EFI_ACPI_DMAR_PCI_PATH *) ((UINT8 *)DevScope + DevScope->Length);
        PciPath->Device                = PciDev;
        PciPath->Function              = PciFun;
        DevScope->Length               += sizeof(EFI_ACPI_DMAR_PCI_PATH);
        DEBUG ((DEBUG_VERBOSE, "[DMAR][DRHD] Type:%d {S%d} (B%02X:D%02X:F%d)\n", DevScope->DeviceScopeEntryType,
                                                                              Stack,
                                                                              DevScope->StartingBusNumber,
                                                                              PciPath->Device,
                                                                              PciPath->Function));
        Drhd->Length += DevScope->Length;
        DEBUG ((DEBUG_VERBOSE, "Drhd->Length in DRHD Non-Stack0  0x%X\n", Drhd->Length));
      }
    }
  }

  for (Port = 0; Port < MAX_IIO_PORTS_PER_STACK; Port++) {
    if (IioUdsHobData->PlatformData.IIO_resource[0].StackRes[Stack].Personality != 2) {    // IO Type
      continue;
    }
    PciDev = Port + 2; // DevFuncToPortPcie
    if (!(((PciRead8 (PCI_LIB_ADDRESS (PciBus, PciDev, PciFun, 0x0E))) & 0x7F) == 1)) {    // PCI Bridge
      continue;
    }
    if (PciExpressRead32 (PCI_EXPRESS_LIB_ADDRESS (PciBus, PciDev, PciFun, 0)) == 0xFFFFFFFF) {
      continue;
    }
    //
    // Add DeviceScope+PciNode for this PCI port
    //
    DevScope = (EFI_ACPI_DMAR_DEVICE_SCOPE_ENTRY_STRUCTURE *) ((UINT8 *)Drhd + Drhd->Length);
    DevScope->DeviceScopeEntryType     = EFI_ACPI_DEVICE_SCOPE_ENTRY_TYPE_PCI_BRIDGE;
    DevScope->Length                   = sizeof(EFI_ACPI_DMAR_DEVICE_SCOPE_ENTRY_STRUCTURE);
    DevScope->EnumerationID            = 0;
    DevScope->StartingBusNumber        = PciBus;

    PciPath = (EFI_ACPI_DMAR_PCI_PATH *) ((UINT8 *)DevScope + DevScope->Length);
    PciPath->Device                    = PciDev;
    PciPath->Function                  = PciFun;
    DevScope->Length                   += sizeof(EFI_ACPI_DMAR_PCI_PATH);

    DEBUG ((DEBUG_VERBOSE, "[DMAR][DRHD-PCIe] Type:%d {S%d:P%d} (B%02X:D%02X:F%d)\n", DevScope->DeviceScopeEntryType, Stack, Port, PciBus, PciDev, PciFun));
    Drhd->Length += DevScope->Length;
    DEBUG ((DEBUG_VERBOSE, "Drhd->Length in DRHD Port 0x%X\n", Drhd->Length));
  } // PCI Port loop

  if (Stack == IIO_STACK0) {
    DevScope = (EFI_ACPI_DMAR_DEVICE_SCOPE_ENTRY_STRUCTURE *) ((UINT8 *)Drhd + Drhd->Length);
    DevScope->DeviceScopeEntryType   = EFI_ACPI_DEVICE_SCOPE_ENTRY_TYPE_ENDPOINT;
    DevScope->Length                 = sizeof(EFI_ACPI_DMAR_DEVICE_SCOPE_ENTRY_STRUCTURE);
    DevScope->EnumerationID          = 0;
    DevScope->StartingBusNumber      = 0xFE;

    PciPath = (EFI_ACPI_DMAR_PCI_PATH *) ((UINT8 *)DevScope + DevScope->Length);
    PciPath->Device                  = IIO_GLOBAL_IEH_PCI_DEV;
    PciPath->Function                = IIO_GLOBAL_IEH_PCI_FUN;
    DevScope->Length                 += sizeof(EFI_ACPI_DMAR_PCI_PATH);

    DEBUG ((DEBUG_VERBOSE, "[DMAR][DRHD-PCIe] Type:%d {S%d} (B%02X:D%02X:F%d)\n", DevScope->DeviceScopeEntryType,
                                                                              Stack,
                                                                              DevScope->StartingBusNumber,
                                                                              PciPath->Device,
                                                                              PciPath->Function));
    Drhd->Length += DevScope->Length;
    DEBUG ((DEBUG_VERBOSE, "Drhd->Length is 0x%X\n", Drhd->Length));
  }

  //
  // Start from next byte location for another DMAR type.
  //
  *CurrentPtr = (VOID *) ((UINT8 *)Drhd + Drhd->Length);

  //
  // Update Dmar Hdr with Drhd Length for this stack
  //
  *DrhdLen = Drhd->Length;
   DEBUG ((DEBUG_VERBOSE, "Drhd->Length at end of DHRD 0x%X\n", Drhd->Length));

  return EFI_SUCCESS;
}

/**
  Append ATSR (Root Port ATS Capability reporting structuresstructs) to
  DMAR ACPI table. These structures are provided only for platforms
  supporting Device-TLBs.

  @param[in,out]  CurrentPtr            The address to start adding ATSR.
  @param[in,out]  AtsrLen               Length of ATSR struct addded.

  @retval         EFI_SUCCESS,          if ATSR is appended successfully
  @retval         EFI_INVALID_PARAMETER,if CurrentPtr/AtsrLen is NULL
**/
EFI_STATUS
AppendAtsr (
  IN OUT VOID                **CurrentPtr,
  IN OUT UINT16              *AtsrLen
  )
{
  EFI_ACPI_DMAR_ROOT_PORT_ATS_CAPABILITY_REPORTING_STRUCTURE *Atsr;

  if ((*CurrentPtr == NULL) || (AtsrLen == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  Atsr = (EFI_ACPI_DMAR_ROOT_PORT_ATS_CAPABILITY_REPORTING_STRUCTURE *)*CurrentPtr;
  *AtsrLen = 0;

  //
  // Initialize the Atsr structure
  //
  Atsr->Type                 = EFI_ACPI_DMA_REMAPPING_STRUCTURE_TYPE_ATSR;
  Atsr->Flags                = EFI_ACPI_DMAR_ATSR_FLAGS_ALL_PORTS_SET;
  Atsr->Length               = sizeof(EFI_ACPI_DMAR_ROOT_PORT_ATS_CAPABILITY_REPORTING_STRUCTURE);
  Atsr->SegmentNumber        = 0;

  //
  // Start from next byte location for another DMAR type.
  //
  *CurrentPtr = (VOID *) ((UINT8 *)Atsr + Atsr->Length);

  //
  // Update Dmar Hdr with Atsr Length for this stack
  //
  *AtsrLen = Atsr->Length;
  DEBUG ((DEBUG_VERBOSE, "Atsr->Length 0x%X\n", Atsr->Length));

  return EFI_SUCCESS;
}

/**
  Append RHSA structs to DMAR ACPI table. Remapping Hardware Status Affinity
  (RHSA) structures for platforms which support non-uniform memory (NUMA).

  @param[in,out]  CurrentPtr            The address to start adding RHSA.
  @param[in]      Stack                 Current Stack to add DRHD for.
  @param[in,out]  RhsaLen               Length of RHSA struct addded.
  @param[in]      IioUdsHobData         Pointer to the Iio Uds info hob.

  @retval         EFI_SUCCESS,          if RHSA is appended successfully
  @retval         EFI_OUT_OF_RESOURCES, if failed to assign VT-d Baseaddress
  @retval         EFI_INVALID_PARAMETER,if CurrentPtr is NULL
**/
EFI_STATUS
AppendRhsa (
  IN OUT VOID                **CurrentPtr,
  IN     UINT8               Stack,
  IN OUT UINT16              *RhsaLen,
  IN IIO_UDS                 *IioUdsHobData
  )
{
  EFI_ACPI_DMAR_REMAPPING_HARDWARE_STATIC_AFFINITY_STRUCTURE *Rhsa;

  DEBUG ((DEBUG_VERBOSE, "AppendRhsa\n"));

  if ((*CurrentPtr == NULL) || (RhsaLen == NULL) || (IioUdsHobData == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  Rhsa = (EFI_ACPI_DMAR_REMAPPING_HARDWARE_STATIC_AFFINITY_STRUCTURE *) *CurrentPtr;
  *RhsaLen = 0;

  //
  // Initialize the Rhsa structure
  //
  Rhsa->Type                 = EFI_ACPI_DMA_REMAPPING_STRUCTURE_TYPE_RHSA;
  Rhsa->Length               = sizeof (EFI_ACPI_DMAR_REMAPPING_HARDWARE_STATIC_AFFINITY_STRUCTURE);
  Rhsa->Reserved             = 0;
  Rhsa->ProximityDomain      = 0;

  Rhsa->RegisterBaseAddress = IioUdsHobData->PlatformData.IIO_resource[0].StackRes[Stack].VtbarAddress;
  if (Rhsa->RegisterBaseAddress == 0) {
    return EFI_OUT_OF_RESOURCES;
  }

  DEBUG ((DEBUG_VERBOSE, "[DMAR][RHSA-Address] Type:%d {S%d} {RegBase=0x%016lX}\n", Rhsa->Type,
                                                                                 Stack,
                                                                                 Rhsa->RegisterBaseAddress));

  //
  // Start from next byte location for another RSHA type.
  //
  *CurrentPtr = (VOID *) ((UINT8 *)Rhsa + Rhsa->Length);

  //
  // Update Rhsa Length for this stack
  //
  *RhsaLen = Rhsa->Length;
  DEBUG ((DEBUG_VERBOSE, "Rhsa->Length 0x%X\n", Rhsa->Length));

  return EFI_SUCCESS;
}

/**
  Append SoC Integrated Address Translation Cache (SATC) reporting structures
  to DMAR ACPI table. The Device Scope structure contains Device Scope Entries
  that identifies SoC integrated devices (in the specified PCI segment) with
  address translation caches (ATC).

  @param[in,out]  CurrentPtr            The address to start adding DRHD.
  @param[in,out]  SatcLen               Length of SATC struct addded.

  @retval         EFI_SUCCESS,          if SATC is appended successfully
  @retval         EFI_INVALID_PARAMETER,if CurrentPtr/SatcLen is NULL
**/
EFI_STATUS
AppendSatc (
  IN OUT VOID                **CurrentPtr,
  IN OUT UINT16              *SatcLen
  )
{
  EFI_ACPI_DMAR_SOC_INTEGRATED_ADDRESS_TRANSLATION_CACHE_REPORTING_STRUCTURE   *Satc;
  EFI_ACPI_DMAR_DEVICE_SCOPE_ENTRY_STRUCTURE     *DevScope;
  EFI_ACPI_DMAR_PCI_PATH                         *PciPath;
  UINT8                      PciBus;
  UINT8                      PciDev;
  UINT8                      PciFun;
  UINT32                     Address;
  UINT8                      IoatIdx;

  DEBUG ((DEBUG_VERBOSE, "AppendSatc\n"));

  if ((*CurrentPtr == NULL) || (SatcLen == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  Satc = (EFI_ACPI_DMAR_SOC_INTEGRATED_ADDRESS_TRANSLATION_CACHE_REPORTING_STRUCTURE *)*CurrentPtr;

  //
  // Initialize the Satc structure
  //
  Satc->Type                 = EFI_ACPI_DMA_REMAPPING_STRUCTURE_TYPE_SATC;
  Satc->Length               = sizeof (EFI_ACPI_DMAR_SOC_INTEGRATED_ADDRESS_TRANSLATION_CACHE_REPORTING_STRUCTURE);
  Satc->Flags                = EFI_ACPI_DMAR_SATC_FLAGS_ATC_REQUIRED_CLEAR;
  Satc->Reserved_5           = 0;
  Satc->SegmentNumber        = 0;

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

    DevScope = (EFI_ACPI_DMAR_DEVICE_SCOPE_ENTRY_STRUCTURE *) ((UINT8 *)Satc + Satc->Length);
    DevScope->DeviceScopeEntryType     = EFI_ACPI_DEVICE_SCOPE_ENTRY_TYPE_ENDPOINT;
    DevScope->Length                   = sizeof(EFI_ACPI_DMAR_DEVICE_SCOPE_ENTRY_STRUCTURE);
    DevScope->EnumerationID            = 0;
    DevScope->StartingBusNumber        = PciBus;

    PciPath = (EFI_ACPI_DMAR_PCI_PATH *) ((UINT8 *)DevScope + DevScope->Length);
    PciPath->Device                    = PciDev;
    PciPath->Function                  = PciFun;
    DevScope->Length                   += sizeof(EFI_ACPI_DMAR_PCI_PATH);

    DEBUG ((DEBUG_VERBOSE, "[DMAR][SATC] Type:%d (B%02X:D%02X:F%d)\n", DevScope->DeviceScopeEntryType,
                                                                    DevScope->StartingBusNumber,
                                                                    PciPath->Device,
                                                                    PciPath->Function));
    Satc->Length += DevScope->Length;
  }

  //
  // Start from next byte location for another DMAR type.
  //
  *CurrentPtr = (VOID *) ((UINT8 *)Satc + Satc->Length);

  //
  // Update Dmar Hdr with Drhd Length for this stack
  //
  *SatcLen = Satc->Length;
  DEBUG ((DEBUG_VERBOSE, "Satc->Length 0x%X\n", Satc->Length));

  return EFI_SUCCESS;
}

/**
  Append SoC Integrated Device Property (SIDP) reporting structures to
  DMAR ACPI table. The Device Scope structure contains Device Scope Entries
  that identifies SoC integrated devices (in the specified PCI segment) with
  special properties/restrictions that system software needs to be aware of.

  @param[in,out]  CurrentPtr            The address to start adding DRHD.
  @param[in,out]  SidpLen               Length of SIDP struct addded.

  @retval         EFI_SUCCESS,          if SIDP is appended successfully
  @retval         EFI_INVALID_PARAMETER,if CurrentPtr/SidpLen is NULL
**/
EFI_STATUS
AppendSidp (
  IN OUT VOID                **CurrentPtr,
  IN OUT UINT16              *SidpLen
  )
{
  EFI_ACPI_DMAR_SOC_INTEGRATED_ADDRESS_TRANSLATION_CACHE_REPORTING_STRUCTURE   *Sidp;
  EFI_ACPI_DMAR_DEVICE_SCOPE_ENTRY_STRUCTURE     *DevScope;
  EFI_ACPI_DMAR_PCI_PATH                         *PciPath;
  UINT8                      PciBus;
  UINT8                      PciDev;
  UINT8                      PciFun;
  UINT32                     Address;
  UINT8                      IoatIdx;

  DEBUG ((DEBUG_VERBOSE, "AppendSidp\n"));

  if ((*CurrentPtr == NULL) || (SidpLen == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  Sidp = (EFI_ACPI_DMAR_SOC_INTEGRATED_ADDRESS_TRANSLATION_CACHE_REPORTING_STRUCTURE *)*CurrentPtr;

  //
  // Initialize the SIDP structure
  //
  Sidp->Type                 = EFI_ACPI_DMA_REMAPPING_STRUCTURE_TYPE_SIDP;
  Sidp->Length               = sizeof (EFI_ACPI_DMAR_SOC_INTEGRATED_DEVICE_PROPERTY_REPORTING_STRUCTURE);
  Sidp->SegmentNumber        = 0;

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

    DevScope = (EFI_ACPI_DMAR_DEVICE_SCOPE_ENTRY_STRUCTURE *) ((UINT8 *)Sidp + Sidp->Length);
    DevScope->DeviceScopeEntryType     = EFI_ACPI_DEVICE_SCOPE_ENTRY_TYPE_ENDPOINT;
    DevScope->Length                   = sizeof(EFI_ACPI_DMAR_DEVICE_SCOPE_ENTRY_STRUCTURE);
    DevScope->EnumerationID            = 0;
    DevScope->StartingBusNumber        = PciBus;
    DevScope->Flags                    = EFI_ACPI_DEVICE_SCOPE_FLAG_ATC_HARDENED;

    PciPath = (EFI_ACPI_DMAR_PCI_PATH *) ((UINT8 *)DevScope + DevScope->Length);
    PciPath->Device                    = PciDev;
    PciPath->Function                  = PciFun;
    DevScope->Length                   += sizeof(EFI_ACPI_DMAR_PCI_PATH);

    DEBUG ((DEBUG_VERBOSE, "[DMAR][SIDP] Type:%d (B%02X:D%02X:F%d)\n", DevScope->DeviceScopeEntryType,
                                                                    DevScope->StartingBusNumber,
                                                                    PciPath->Device,
                                                                    PciPath->Function));
    Sidp->Length += DevScope->Length;
  }

  //
  // Start from next byte location for another DMAR type.
  //
  *CurrentPtr = (VOID *) ((UINT8 *)Sidp + Sidp->Length);

  //
  // Update Dmar Hdr with Drhd Length for this stack
  //
  *SidpLen = Sidp->Length;
   DEBUG ((DEBUG_VERBOSE, "Sidp->Length 0x%X\n", Sidp->Length));

  return EFI_SUCCESS;
}

/**
  Update DMAR ACPI table header

  Set flags and Host Address Width.

  @param[in,out]  Table             The start address of DMAR table.
  @param[in]      DmarTableFlags    DmarTableFlags.

  @retval         EFI_SUCCESS
**/
EFI_STATUS
UpdateDmarHeader (
  IN OUT EFI_ACPI_DMAR_DESCRIPTION_TABLE   *Dmar,
  IN   UINT8 DmarTableFlags
  )
{
  EFI_CPUID_REGISTER          CpuidRegister = {0};

  Dmar->Flags = DmarTableFlags;
  DEBUG ((DEBUG_VERBOSE, "DmarTableFlags 0x%X\n",   DmarTableFlags));

  //
  // Read Address Width from CPUID (EAX=CPUID_VIR_PHY_ADDRESS_SIZE)
  //
  AsmCpuid (
    CPUID_VIR_PHY_ADDRESS_SIZE,
    &CpuidRegister.RegEax,
    &CpuidRegister.RegEbx,
    &CpuidRegister.RegEcx,
    &CpuidRegister.RegEdx
  );
  Dmar->HostAddressWidth = (UINT8)((CpuidRegister.RegEax & 0xFF)-1);

  return EFI_SUCCESS;
}

/**
  Update DMAR ACPI table

  Only the header is found until now.
  Append the rest of table here.

  @param[in,out]  Table       The start address of DMAR header from ACPI tables.

  @retval         EFI_SUCCESS, if DMAR update successful.
                  Status,      if error in update.
**/
EFI_STATUS
UpdateDmarTable (
  IN OUT  VOID  *Table
  )
{
  UINT8                                Idx;
  UINT8                                Stack;
  UINT8                                *Current;
  EFI_STATUS                           Status;
  EFI_ACPI_DMAR_DESCRIPTION_TABLE      *Dmar;
  UINT16                               DrhdLen;
  UINT16                               AtsrLen;
  UINT16                               RhsaLen;
  UINT16                               SatcLen;
  UINT16                               SidpLen;

  EFI_HOB_GUID_TYPE                    *GuidHob;
  IIO_UDS                              *IioUdsHobData;
  VOID                                 *FspHobList;
  UINT32                               StackPresentBitmap;

  MISC_CFG_DATA                        *MiscCfgData;
  UINT8                                DmarTableFlags;

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
    DEBUG ((DEBUG_VERBOSE, "DDDDMAR  DmarTableFlags 0x%X\n",   DmarTableFlags));

  } else {
    DEBUG ((DEBUG_WARN, "No Misc CFGDATA. Continue to boot with default values!\n"));
    DmarTableFlags = 5;
  }

  Dmar = (EFI_ACPI_DMAR_DESCRIPTION_TABLE *) Table;
  Current = (UINT8 *) Dmar + sizeof(EFI_ACPI_DMAR_HEADER);

  //
  // DRHD
  //
  for (Idx = 1; Idx <= MAX_SOCKET; Idx++) {
    for (Stack = 0; Stack < MAX_IIO_STACKS_PER_SOCKET; Stack++) {
    DrhdLen  = 0;
    StackPresentBitmap = IioUdsHobData->PlatformData.CpuQpiInfo[0].StackPresentBitmap;
    if (!(StackPresentBitmap & (BIT0 << Stack))) {
      continue;
    }
    if (Stack == 0) {
      continue;
    }
    Status   = AppendDrhd ((VOID **)&Current, Stack, &DrhdLen, IioUdsHobData);
    if (EFI_ERROR(Status)) {
      DEBUG ((DEBUG_ERROR, "Append DRHD failed, returning\n"));
      return Status;
    }
    Dmar->Header.Length += DrhdLen;
    DEBUG ((DEBUG_VERBOSE, "DMAR Dmar->Header.Length 0x%x\n", Dmar->Header.Length));
    }
  }

  Stack    = 0;
  Status   = AppendDrhd ((VOID **)&Current, Stack, &DrhdLen, IioUdsHobData);
  if (EFI_ERROR(Status)) {
    DEBUG ((DEBUG_ERROR, "Append DRHD failed, returning\n"));
    return Status;
  }
  Dmar->Header.Length += DrhdLen;
  DEBUG ((DEBUG_VERBOSE, "DMAR Dmar->Header.Length 0x%x\n", Dmar->Header.Length));

  //
  // ATSR
  //
  AtsrLen  = 0;

  Status   = AppendAtsr ((VOID **)&Current, &AtsrLen);
  if (EFI_ERROR(Status)) {
    DEBUG ((DEBUG_ERROR, "Append ATSR failed, returning\n"));
    return Status;
  }
  Dmar->Header.Length += AtsrLen;

  if (MiscCfgData != NULL) {
    //
    // RHSA
    //
    if (MiscCfgData->VtdConfig.RhsaSupport) {
      for (Idx = 1; Idx <= MAX_SOCKET; Idx++) {
        for (Stack = 0; Stack < MAX_IIO_STACKS_PER_SOCKET; Stack++) {
          RhsaLen = 0;
          StackPresentBitmap = IioUdsHobData->PlatformData.CpuQpiInfo[0].StackPresentBitmap;
          if (!(StackPresentBitmap & (BIT0 << Stack))) {
            continue;
          }
          Status = AppendRhsa ((VOID **)&Current, Stack, &RhsaLen, IioUdsHobData);
          if (EFI_ERROR(Status)) {
            DEBUG ((DEBUG_ERROR, "Append RHSA failed, returning\n"));
            return Status;
          }
          Dmar->Header.Length += RhsaLen;
          DEBUG ((DEBUG_VERBOSE, "DMAR Dmar->Header.Length 0x%x\n", Dmar->Header.Length));
        }
      }
    }
    //
    // SATC
    //
    if (MiscCfgData->VtdConfig.SatcSupport) {
      SatcLen  = 0;

      Status = AppendSatc ((VOID **)&Current, &SatcLen);
      if (EFI_ERROR(Status)) {
        DEBUG ((DEBUG_ERROR, "Append SATC failed, returning\n"));
        return Status;
      }
    Dmar->Header.Length += SatcLen;
    DEBUG ((DEBUG_VERBOSE, "DMAR Dmar->Header.Length 0x%x\n", Dmar->Header.Length));
    }
    //
    // SIDP
    //
    if (MiscCfgData->VtdConfig.SidpSupport) {
      SidpLen  = 0;

      Status = AppendSidp ((VOID **)&Current, &SidpLen);
      if (EFI_ERROR(Status)) {
        DEBUG ((DEBUG_ERROR, "Append SIDP failed, returning\n"));
        return Status;
      }
      Dmar->Header.Length += SidpLen;
      DEBUG ((DEBUG_VERBOSE, "DMAR Dmar->Header.Length 0x%x\n", Dmar->Header.Length));
    }
  }

  //
  // Update Dmar header fields
  //
  UpdateDmarHeader (Dmar, DmarTableFlags);

  return Status;
}
