/** @file

  Copyright (c) 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Base.h>
#include <Uefi/UefiBaseType.h>
#include <Library/IoLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/VTdLib.h>
#include <Library/DmaRemap.h>
#include <Library/HobLib.h>
#include <Guid/PciRootBridgeInfoGuid.h>
#include <Library/PciExpressLib.h>
#include <PlatformBase.h>
#include <Library/BootloaderCoreLib.h>
#include <Library/BootloaderCommonLib.h>
#include <IndustryStandard/DmaRemappingReportingTable.h>
#include <Library/UncoreLib.h>
#include <Register/PchRegsP2sb.h>
#include <Register/IioRegs.h>
#include <Register/RegsUsb.h>
#include <Library/MemoryAllocationLib.h>
#include <CpuDataStruct.h>
#include <Library/BaseLib.h>

#define MAX_IOU_NUM_PER_SOCKET      8
#define MAX_STACK_NUM_PER_SOCKET    12

typedef struct {
  UINT8  MaxPortNumPerSocket;
  UINT8  MaxIouNumPerSocket;
  UINT8  MaxPortNumPerIou;
  UINT8  MaxNtbNumPerSocket;
  UINT8  MaxStackNumPerSocket;
  UINT8  ClusterPortByNtbIndex[MAX_IOU_NUM_PER_SOCKET];
  UINT8  StackIndexByIouIndex[MAX_IOU_NUM_PER_SOCKET];
  UINT8  MaxPortNumByStackIndex[MAX_STACK_NUM_PER_SOCKET];
} IIO_SYS_INFO;

//
// ICX-D LCC IIO SYS INFO
//
CONST IIO_SYS_INFO  IioSysInfoTableIcxDLcc = {
  5,  // MaxPortNumPerSocket;
  1,  // MaxIouNumPerSocket;
  4,  // MaxPortNumPerIou;
  1,  // MaxNtbNumPerSocket;
  6,  // MaxStackNumPerSocket;
  { 1, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },  // ClusterPortByNtbIndex[MAX_IOU_NUM_PER_SOCKET];
  { 1, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },  // StackIndexByIouIndex[MAX_IOU_NUM_PER_SOCKET];
  { 1, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }            // MaxPortNumByStackIndex[MAX_STACK_NUM_PER_SOCKET];;
};

//
// ICX-D HCC IIO SYS INFO
//
CONST IIO_SYS_INFO  IioSysInfoTableIcxDHcc = {
  9,  // MaxPortNumPerSocket;
  2,  // MaxIouNumPerSocket;
  4,  // MaxPortNumPerIou;
  2,  // MaxNtbNumPerSocket;
  6,  // MaxStackNumPerSocket;
  { 1, 5, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},        // ClusterPortByNtbIndex[MAX_IOU_NUM_PER_SOCKET];
  { 1, 5, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},        // StackIndexByIouIndex[MAX_IOU_NUM_PER_SOCKET];
  { 1, 4, 0, 0, 0, 4, 0, 0 , 0, 0, 0, 0 }                        // MaxPortNumByStackIndex[MAX_STACK_NUM_PER_SOCKET];;
};

//-----------------------------------------------------------------------------------
// Port Index definition
//------------------------------------------------------------------------------------
#define PCIE_PORT_0_DEV_0     0x03
#define PCIE_PORT_0_FUNC_0    0x00

#define PCIE_PORT_1A_DEV_1    0x02
#define PCIE_PORT_1B_DEV_1    0x03
#define PCIE_PORT_1C_DEV_1    0x04
#define PCIE_PORT_1D_DEV_1    0x05
#define PCIE_PORT_1A_FUNC_1   0x00
#define PCIE_PORT_1B_FUNC_1   0x00
#define PCIE_PORT_1C_FUNC_1   0x00
#define PCIE_PORT_1D_FUNC_1   0x00

#define PCIE_PORT_5A_DEV_5    0x02
#define PCIE_PORT_5B_DEV_5    0x03
#define PCIE_PORT_5C_DEV_5    0x04
#define PCIE_PORT_5D_DEV_5    0x05
#define PCIE_PORT_5A_FUNC_5   0x00
#define PCIE_PORT_5B_FUNC_5   0x00
#define PCIE_PORT_5C_FUNC_5   0x00
#define PCIE_PORT_5D_FUNC_5   0x00


//
//  PciePath for ICXD keeping same table for both HCC and LCC
//  Maximum PCIe ports in LCC is 4 and in HCC is 8.
//
CONST PCI_NODE  mPciPath2_0_Icxd[] = {
  { PCIE_PORT_0_DEV_0, PCIE_PORT_0_FUNC_0 },
  { (UINT8)-1, (UINT8)-1 },
};
CONST PCI_NODE  mPciPath2_1_Icxd[] = {
  { PCIE_PORT_1A_DEV_1, PCIE_PORT_1A_FUNC_1 },
  { (UINT8)-1, (UINT8)-1 },
};
CONST PCI_NODE  mPciPath2_2_Icxd[] = {
  { PCIE_PORT_1B_DEV_1, PCIE_PORT_1B_FUNC_1 },
  { (UINT8)-1, (UINT8)-1 },
};
CONST PCI_NODE  mPciPath2_3_Icxd[] = {
  { PCIE_PORT_1C_DEV_1, PCIE_PORT_1C_FUNC_1 },
  { (UINT8)-1, (UINT8)-1 },
};
CONST PCI_NODE  mPciPath2_4_Icxd[] = {
  { PCIE_PORT_1D_DEV_1, PCIE_PORT_1D_FUNC_1 },
  { (UINT8)-1, (UINT8)-1 },
};
CONST PCI_NODE  mPciPath2_5_Icxd[] = {
  { PCIE_PORT_5A_DEV_5, PCIE_PORT_5A_FUNC_5 },
  { (UINT8)-1, (UINT8)-1 },
};
CONST PCI_NODE  mPciPath2_6_Icxd[] = {
  { PCIE_PORT_5B_DEV_5, PCIE_PORT_5B_FUNC_5 },
  { (UINT8)-1, (UINT8)-1 },
};
CONST PCI_NODE  mPciPath2_7_Icxd[] = {
  { PCIE_PORT_5C_DEV_5, PCIE_PORT_5C_FUNC_5 },
  { (UINT8)-1, (UINT8)-1 },
};
CONST PCI_NODE  mPciPath2_8_Icxd[] = {
  { PCIE_PORT_5D_DEV_5, PCIE_PORT_5D_FUNC_5 },
  { (UINT8)-1, (UINT8)-1 },
};

#define DMI_BUS_NUM                   0
#define DMI_DEV_NUM                   0
#define DMI_FUNC_NUM                  0

//
// DEVICE_SCOPE for ICXD_LCC
//
CONST DEVICE_SCOPE  mDevScopeATSR_Icxd[] = {
  {
    EFI_ACPI_DEVICE_SCOPE_ENTRY_TYPE_PCI_BRIDGE,      // Pcie Port1
    00,                                           // Enumeration ID
    DMI_BUS_NUM,
    &mPciPath2_0_Icxd[0]
  },
  {
    EFI_ACPI_DEVICE_SCOPE_ENTRY_TYPE_PCI_BRIDGE,      // Pcie Port2
    00,                                           // Enumeration ID
    DMI_BUS_NUM,
    &mPciPath2_1_Icxd[0]
  },
  {
    EFI_ACPI_DEVICE_SCOPE_ENTRY_TYPE_PCI_BRIDGE,      // Pcie Port3
    00,                                           // Enumeration ID
    DMI_BUS_NUM,
    &mPciPath2_2_Icxd[0]
  },
  {
    EFI_ACPI_DEVICE_SCOPE_ENTRY_TYPE_PCI_BRIDGE,      // Pcie Port4
    00,                                           // Enumeration ID
    DMI_BUS_NUM,
    &mPciPath2_3_Icxd[0]
  },
  {
    EFI_ACPI_DEVICE_SCOPE_ENTRY_TYPE_PCI_BRIDGE,      // Pcie Port5
    00,                                           // Enumeration ID
    DMI_BUS_NUM,
    &mPciPath2_4_Icxd[0]
  },
  {
    EFI_ACPI_DEVICE_SCOPE_ENTRY_TYPE_PCI_BRIDGE,      // Pcie Port6
    00,                                           // Enumeration ID
    DMI_BUS_NUM,
    &mPciPath2_5_Icxd[0]
  },
  {
    EFI_ACPI_DEVICE_SCOPE_ENTRY_TYPE_PCI_BRIDGE,      // Pcie Port7
    00,                                           // Enumeration ID
    DMI_BUS_NUM,
    &mPciPath2_6_Icxd[0]
  },
  {
    EFI_ACPI_DEVICE_SCOPE_ENTRY_TYPE_PCI_BRIDGE,      // Pcie Port8
    00,                                           // Enumeration ID
    DMI_BUS_NUM,
    &mPciPath2_7_Icxd[0]
  },
  {
    EFI_ACPI_DEVICE_SCOPE_ENTRY_TYPE_PCI_BRIDGE,      // Pcie Port9
    00,                                           // Enumeration ID
    DMI_BUS_NUM,
    &mPciPath2_8_Icxd[0]
  },
};

//
// PciePath for RMRR #1
//
PCI_NODE     mPciPathRmrr1[] = {
  { PCI_DEVICE_NUMBER_PCH_XHCI,      PCI_FUNCTION_NUMBER_PCH_XHCI  },
  { (UINT8)-1,   (UINT8)-1},
};

//
//  Stack #s for LCC & HCC
//
UINT8   mValidStackLcc[MAX_IIO_STACK] = {IIO_STACK0, IIO_STACK1, IIO_STACK2, 0, 0, 0};
UINT8   mValidStackHcc[MAX_IIO_STACK] = {IIO_STACK0, IIO_STACK1, IIO_STACK2, IIO_STACK3, IIO_STACK5, 0};

/**
  Get the VTD BAR for this Bus.

  @param[in]    Bus     Input bus # to get VTD BAR

  @retval               VTD BAR for this Bus
                        0, if invalid Bus
**/
UINT32
GetVtdBar (
  IN  UINT8   Bus
  )
{
  UINTN     Address;
  UINT32    Value;

  if (Bus == (UINT8) -1) {
    return 0;
  }

  Address = MM_PCI_ADDRESS (Bus, 0, 0, 0);
  Value   = MmioRead32 (Address + R_IIO_VTBAR) & ~B_IIO_VT_BAR_EN;

  return Value;
}

/**
  Get the Max Num of Ports per socket.

  @param[in]    Socket  Input socket # to get max ports

  @retval               Max Num of Ports on this socket
**/
UINT8
GetMaxPortPerSocket (
  IN  UINT8   Socket
  )
{
  if (GetChopType(0) == TypeLcc) {
    return IioSysInfoTableIcxDLcc.MaxPortNumPerSocket;
  } else if (GetChopType(0) == TypeHcc) {
    return IioSysInfoTableIcxDHcc.MaxPortNumPerSocket;
  } else {
    return 0;
  }
}

/**
  Get the Stack to which Port belongs.

  @param[in]    Port    Input port # to get Stack

  @retval               Stack to which 'Port' belongs.
**/
UINT8
GetStackForPort (
  IN  UINT8   Port
  )
{
  UINT8   IouIndex;
  UINT8   Stack;

  if (GetChopType(0) == TypeLcc) {
    IouIndex = (Port - 1) / (IioSysInfoTableIcxDLcc.MaxPortNumPerIou);
    Stack    = IioSysInfoTableIcxDLcc.StackIndexByIouIndex[IouIndex];
  } else if (GetChopType(0) == TypeHcc) {
    IouIndex = (Port - 1) / (IioSysInfoTableIcxDHcc.MaxPortNumPerIou);
    Stack    = IioSysInfoTableIcxDHcc.StackIndexByIouIndex[IouIndex];
  } else {
    DEBUG ((DEBUG_INFO, "Unsupported Chop Type\n"));
    Stack = 0;
  }

  return Stack;
}

/**
  Append DRHD structs to DMAR ACPI table.

  @param[in,out]  CurrentPtr            The address to start adding DRHD.
  @param[in]      Stack                 Current STack to add DRHD for.
  @param[in,out]  DrhdLen               Length of DRHD struct addded.
  @param[in]      RootBridgeInfoHob     Pointer to the root bridge info hob.

  @retval         EFI_SUCCESS    ,      if DRHD is appended successfully
  @retval         EFI_UNSUPPORTED,      if Bus number is -1
  @retval         EFI_INVALID_PARAMETER,if CurrentPtr/DrhdLen/RootBridgeInfoHob is NULL
**/
EFI_STATUS
AppendDrhd (
  IN OUT VOID                 **CurrentPtr,
  IN     UINT8                  Stack,
  IN OUT UINT16                *DrhdLen,
  IN PCI_ROOT_BRIDGE_INFO_HOB  *RootBridgeInfoHob
  )
{
  UINT8                       Idx;
  UINT8                       Port;
  UINT8                       MaxPort;
  UINT8                       PortStack;
  UINT8                       Bus;
  UINT8                       Dev;
  UINT8                       Func;
  UINT32                      Address;
  UINT8                       PathIdx;
  UINT16                      NumberOfHpets;
  UINT16                      HpetCapIdValue;
  EFI_ACPI_DMAR_HARDWARE_UNIT_DEFINITION_STRUCTURE *Drhd;
  EFI_ACPI_DMAR_DEVICE_SCOPE_ENTRY_STRUCTURE       *DevScope;
  EFI_ACPI_DMAR_PCI_PATH                           *PciPath;

  if ((*CurrentPtr == NULL) || (DrhdLen == NULL) || (RootBridgeInfoHob == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  Drhd = (EFI_ACPI_DMAR_HARDWARE_UNIT_DEFINITION_STRUCTURE *)*CurrentPtr;
  *DrhdLen = 0;
  Bus  = (UINT8) -1;

  //
  // Init Drhd header, and patch later
  //
  Drhd->Type                = EFI_ACPI_DMA_REMAPPING_STRUCTURE_TYPE_DRHD;
  Drhd->Length              = sizeof (EFI_ACPI_DMAR_HARDWARE_UNIT_DEFINITION_STRUCTURE);
  Drhd->Flags               = EFI_ACPI_DMAR_DRHD_FLAGS_INCLUDE_ALL_CLEAR;
  Drhd->SegmentNumber       = 0;
  Drhd->RegisterBaseAddress = 0;

  //
  // Calculate primary Bus # for this Stack.
  // If no valid Bus # found, return error.
  //
  for (Idx = 0; Idx < MAX_IIO_STACK; Idx++) {
    if (GetChopType(0) == TypeLcc) {
      if (mValidStackLcc[Idx] == Stack) {
        Bus = RootBridgeInfoHob->Entry[Idx].BusBase;
        break;
      }
    } else {
      if (mValidStackHcc[Idx] == Stack) {
        Bus = RootBridgeInfoHob->Entry[Idx].BusBase;
        break;
      }
    }
  }

  if (Bus == (UINT8) -1) {
    DEBUG ((DEBUG_INFO, "Error getting Bus Number for IIO Stack %d\n", Stack));
    return EFI_UNSUPPORTED;
  }

  //
  // Add PCI-E ports for IIO Stack 1 to N in respective DRHD.
  // Deal with IIO Stack 0 PCI-E port separately.
  // Also entries for other stack items are added after
  // PCI-E ports are done.
  //

  MaxPort = GetMaxPortPerSocket(0);
  for (Port = 1; Port < MaxPort; Port++) {
    PortStack = GetStackForPort (Port);
    //
    // If this port doesn't belong to the current stack or
    // it is from Stack0, just continue with next port.
    //
    if ((PortStack != Stack) || (PortStack == 0)) {
      continue;
    }

    //
    // Get Bus/Dev/Func for this PCI-E port (Bus already calculated)
    //
    Dev  = mDevScopeATSR_Icxd[Port].PciNode->Device;
    Func = mDevScopeATSR_Icxd[Port].PciNode->Function;

    //
    // Check if the port is hidden (skip) or available (proceed)
    //
    Address = PCI_EXPRESS_LIB_ADDRESS (Bus, Dev, Func, 0);
    if (PciExpressRead32 (Address) == 0xFFFFFFFF) {
      DEBUG ((DEBUG_INFO, "[DMAR] [DRHD-PCIe] {S%d:P%d} (B%02X:D%02X:F%d) hidden - skip\n", Stack, Port, Bus, Dev, Func));
      continue;
    }

    //
    // Add DeviceScope+PciNode for this PCI-E port
    // Patch Drhd header with this new info
    //
    DevScope = (EFI_ACPI_DMAR_DEVICE_SCOPE_ENTRY_STRUCTURE *) ((UINT8 *)Drhd + Drhd->Length);
    DevScope->DeviceScopeEntryType = EFI_ACPI_DEVICE_SCOPE_ENTRY_TYPE_PCI_BRIDGE;
    DevScope->Length               = sizeof(EFI_ACPI_DMAR_DEVICE_SCOPE_ENTRY_STRUCTURE);
    DevScope->EnumerationID        = 0;
    DevScope->StartingBusNumber    = Bus;

    PciPath = (EFI_ACPI_DMAR_PCI_PATH *) ((UINT8 *)DevScope + DevScope->Length);
    for (PathIdx = 0; mDevScopeATSR_Icxd[Port].PciNode[PathIdx].Device != (UINT8) -1; PathIdx++,PciPath++) {
      CopyMem (PciPath, &(mDevScopeATSR_Icxd[Port].PciNode[PathIdx]), sizeof(EFI_ACPI_DMAR_PCI_PATH));
      DevScope->Length += sizeof(EFI_ACPI_DMAR_PCI_PATH);
    }

    DEBUG ((DEBUG_INFO, "[DMAR] [DRHD-PCIe] {S%d:P%d} (B%02X:D%02X:F%d)\n", Stack, Port, Bus, Dev, Func));
    Drhd->Length += DevScope->Length;
  } // PCI-E Port loop

  //
  // Create new DevScopes for other stack 1 items
  //
  if (Stack == IIO_STACK1) {
  } // Stack1

  //
  // Create new DevScopes for other stack 2 items
  //
  if (Stack == IIO_STACK2) {
    //
    // Virtual Root Port 0 for CPK Device is present in Stack 2 for ICXD LCC
    // Virtual Root Port 1 for CPM Device is present in Stack 2 for ICXD HCC
    //
    DevScope = (EFI_ACPI_DMAR_DEVICE_SCOPE_ENTRY_STRUCTURE *) ((UINT8 *)Drhd + Drhd->Length);
    DevScope->DeviceScopeEntryType = EFI_ACPI_DEVICE_SCOPE_ENTRY_TYPE_PCI_BRIDGE;
    DevScope->Length               = sizeof(EFI_ACPI_DMAR_DEVICE_SCOPE_ENTRY_STRUCTURE);
    DevScope->EnumerationID        = 0;
    DevScope->StartingBusNumber    = Bus;

    PciPath = (EFI_ACPI_DMAR_PCI_PATH *) ((UINT8 *)DevScope + DevScope->Length);
    if (GetChopType(0) == TypeLcc) {
      PciPath->Device   = 4;
      PciPath->Function = 0;
      DEBUG ((DEBUG_INFO, "[DMAR] [DRHD- CPK] {S%d}    (B%02X:D%02X:F%d)\n", Stack,
                                                                           DevScope->StartingBusNumber,
                                                                           PciPath->Device,
                                                                           PciPath->Function));
    } else if (GetChopType(0) == TypeHcc) {
      PciPath->Device   = 5;
      PciPath->Function = 0;
      DEBUG ((DEBUG_INFO, "[DMAR] [DRHD- CPM] {S%d}    (B%02X:D%02X:F%d)\n", Stack,
                                                                           DevScope->StartingBusNumber,
                                                                           PciPath->Device,
                                                                           PciPath->Function));
    } else {
      DEBUG ((DEBUG_INFO, "Unsupported Chop Type\n"));
    }
    DevScope->Length += sizeof(EFI_ACPI_DMAR_PCI_PATH);

    Drhd->Length += DevScope->Length;

  } // Stack2

  //
  // Create new DevScopes for other stack 3 items
  //
  if (Stack == IIO_STACK3) {
    //
    // Virtual Root Port 0 for CPK Device is present in Stack 3 for ICXD HCC
    //
    if (GetChopType(0) == TypeHcc) {
      DevScope = (EFI_ACPI_DMAR_DEVICE_SCOPE_ENTRY_STRUCTURE *) ((UINT8 *)Drhd + Drhd->Length);
      DevScope->DeviceScopeEntryType = EFI_ACPI_DEVICE_SCOPE_ENTRY_TYPE_BRIDGE;
      DevScope->Length               = sizeof(EFI_ACPI_DMAR_DEVICE_SCOPE_ENTRY_STRUCTURE);
      DevScope->EnumerationID        = 0;
      DevScope->StartingBusNumber    = Bus;

      PciPath = (EFI_ACPI_DMAR_PCI_PATH *) ((UINT8 *)DevScope + DevScope->Length);
      PciPath->Device   = 4;
      PciPath->Function = 0;
      DevScope->Length += sizeof(EFI_ACPI_DMAR_PCI_PATH);

      DEBUG ((DEBUG_INFO, "[DMAR] [DRHD- CPK] {S%d}    (B%02X:D%02X:F%d)\n", Stack,
                                                                          DevScope->StartingBusNumber,
                                                                          PciPath->Device,
                                                                          PciPath->Function));
      Drhd->Length += DevScope->Length;
    }
  } // Stack 3

  //
  // Create new DevScopes for stack 0 items
  //
  if (Stack == IIO_STACK0) {
    //
    // Update Flags for legacy Stack
    //
    Drhd->Flags = EFI_ACPI_DMAR_DRHD_FLAGS_INCLUDE_ALL_SET;

    //
    // Add the PCH IOAPIC and HPET device scopes
    // (iff InterruptRemap is enabled, check later)
    //

    //
    // PCH IOAPIC
    //
    DevScope = (EFI_ACPI_DMAR_DEVICE_SCOPE_ENTRY_STRUCTURE *) ((UINT8 *)Drhd + Drhd->Length);
    DevScope->DeviceScopeEntryType = EFI_ACPI_DEVICE_SCOPE_ENTRY_TYPE_IOAPIC;
    DevScope->Length               = sizeof(EFI_ACPI_DMAR_DEVICE_SCOPE_ENTRY_STRUCTURE);
    DevScope->EnumerationID        = PCH_IOAPIC_ID;
    DevScope->StartingBusNumber    = V_P2SB_CFG_IBDF_BUS;

    PciPath = (EFI_ACPI_DMAR_PCI_PATH *) ((UINT8 *)DevScope + DevScope->Length);
    PciPath->Device   = V_P2SB_CFG_IBDF_DEV;
    PciPath->Function = V_P2SB_CFG_IBDF_FUNC;
    DevScope->Length += sizeof(EFI_ACPI_DMAR_PCI_PATH);

    DEBUG ((DEBUG_INFO, "[DMAR] [DRHD-IOAP] {S%d}    (B%02X:D%02X:F%d)\n", Stack,
                                                                            DevScope->StartingBusNumber,
                                                                            PciPath->Device,
                                                                            PciPath->Function));
    Drhd->Length += DevScope->Length;

    //
    // HPET
    //
    HpetCapIdValue = *(UINT16 *)(UINTN)(HPET_BLOCK_ADDRESS);
    NumberOfHpets  = (HpetCapIdValue >> 0x08) & 0x1F;  // Bits [8:12] contains the number of Hpets
    if (NumberOfHpets && (NumberOfHpets != 0x1F) &&
      (*((volatile UINT32 *)(UINTN)(HPET_BLOCK_ADDRESS + 0x100)) & BIT15)) { // FSB Interrupt Delivery Capability

      DevScope = (EFI_ACPI_DMAR_DEVICE_SCOPE_ENTRY_STRUCTURE *) ((UINT8 *)Drhd + Drhd->Length);
      DevScope->DeviceScopeEntryType = EFI_ACPI_DEVICE_SCOPE_ENTRY_TYPE_MSI_CAPABLE_HPET;
      DevScope->Length               = sizeof(EFI_ACPI_DMAR_DEVICE_SCOPE_ENTRY_STRUCTURE);
      DevScope->EnumerationID        = 0;
      DevScope->StartingBusNumber    = V_P2SB_CFG_HBDF_BUS;

      PciPath = (EFI_ACPI_DMAR_PCI_PATH *) ((UINT8 *)DevScope + DevScope->Length);
      PciPath->Device   = V_P2SB_CFG_HBDF_DEV;
      PciPath->Function = V_P2SB_CFG_HBDF_FUNC;
      DevScope->Length += sizeof(EFI_ACPI_DMAR_PCI_PATH);

      DEBUG ((DEBUG_INFO, "[DMAR] [DRHD-HPET] {S%d}    (B%02X:D%02X:F%d)\n", Stack,
                                                                            DevScope->StartingBusNumber,
                                                                            PciPath->Device,
                                                                            PciPath->Function));
      Drhd->Length += DevScope->Length;
    }

  } // Stack0

  //
  // Get VtdBase for this stack
  //
  Drhd->RegisterBaseAddress = GetVtdBar (Bus);

  //
  // Start from next byte location for another DMAR type.
  //
  *CurrentPtr = (VOID *) ((UINT8 *)Drhd + Drhd->Length);

  //
  // Update Dmar Hdr with Drhd Length for this stack
  //
  *DrhdLen = Drhd->Length;

  return EFI_SUCCESS;
}


/**
  Append ATSR structs to DMAR ACPI table.

  @param[in,out]  CurrentPtr            The address to start adding ATSR.
  @param[in]      Stack                 Current STack to add ATSR for.
  @param[in,out]  AtsrLen               Length of ATSR struct addded.
  @param[in]      RootBridgeInfoHob     Pointer to the root bridge info hob.

  @retval         EFI_SUCCESS    ,      if ATSR is appended successfully
  @retval         EFI_UNSUPPORTED,      if Bus number is -1
  @retval         EFI_INVALID_PARAMETER,if CurrentPtr/AtsrLen/RootBridgeInfoHob is NULL
**/
EFI_STATUS
AppendAtsr (
  IN OUT VOID                 **CurrentPtr,
  IN     UINT8                  Stack,
  IN OUT UINT16                *AtsrLen,
  IN PCI_ROOT_BRIDGE_INFO_HOB  *RootBridgeInfoHob
  )
{
  UINT8                       Idx;
  UINT8                       Port;
  UINT8                       MaxPort;
  UINT8                       PortStack;
  UINT8                       Bus;
  UINT8                       Dev;
  UINT8                       Func;
  UINT32                      Address;
  UINT8                       PathIdx;
  UINTN                       VtdBase;
  UINT64                      VtdExtCap;
  EFI_ACPI_DMAR_ROOT_PORT_ATS_CAPABILITY_REPORTING_STRUCTURE *Atsr;
  EFI_ACPI_DMAR_DEVICE_SCOPE_ENTRY_STRUCTURE                 *DevScope;
  EFI_ACPI_DMAR_PCI_PATH                                     *PciPath;

  if ((*CurrentPtr == NULL) || (AtsrLen == NULL) || (RootBridgeInfoHob == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  Atsr = (EFI_ACPI_DMAR_ROOT_PORT_ATS_CAPABILITY_REPORTING_STRUCTURE *)*CurrentPtr;
  *AtsrLen = 0;
  Bus  = (UINT8) -1;

  //
  // Init Atsr header, and patch later
  //
  Atsr->Type          = EFI_ACPI_DMA_REMAPPING_STRUCTURE_TYPE_ATSR;
  Atsr->Length        = sizeof(EFI_ACPI_DMAR_ROOT_PORT_ATS_CAPABILITY_REPORTING_STRUCTURE);
  Atsr->Flags         = 0;
  Atsr->SegmentNumber = 0; // single socket

  //
  // Calculate primary Bus # for this Stack.
  // If no valid Bus # found, return error.
  //
  for (Idx = 0; Idx < MAX_IIO_STACK; Idx++) {
    if (GetChopType(0) == TypeLcc) {
      if (mValidStackLcc[Idx] == Stack) {
        Bus = RootBridgeInfoHob->Entry[Idx].BusBase;
        break;
      }
    } else {
      if (mValidStackHcc[Idx] == Stack) {
        Bus = RootBridgeInfoHob->Entry[Idx].BusBase;
        break;
      }
    }
  }

  if (Bus == (UINT8) -1) {
    DEBUG ((DEBUG_INFO, "Error getting Bus Number for IIO Stack %d\n", Stack));
    return EFI_UNSUPPORTED;
  }

  //
  // Add Atsr entry for each PCIe stack port.
  // Start from Port1 for legacy IOH.
  //

  MaxPort = GetMaxPortPerSocket(0);
  for (Port = 1; Port < MaxPort; Port++) {
    PortStack = GetStackForPort (Port);
    //
    // If this port doesn't belong to the current stack or
    // it is from Stack0, just continue with next port.
    //
    if ((PortStack != Stack) || (PortStack == 0)) {
      continue;
    }

    //
    // Get Bus/Dev/Func for this PCI-E port (Bus already calculated)
    //
    Dev  = mDevScopeATSR_Icxd[Port].PciNode->Device;
    Func = mDevScopeATSR_Icxd[Port].PciNode->Function;

    //
    // Check if the port is hidden (skip) or available (proceed)
    //
    Address = PCI_EXPRESS_LIB_ADDRESS (Bus, Dev, Func, 0);
    if (PciExpressRead32 (Address) == 0xFFFFFFFF) {
      DEBUG ((DEBUG_INFO, "[DMAR] [ATSR-PCIe] {S%d:P%d} (B%02X:D%02X:F%d) hidden - skip\n", Stack, Port, Bus, Dev, Func));
      continue;
    }

    //
    // Check if device IOTLB is supported by this stack port
    //
    VtdBase = (UINTN) GetVtdBar (Bus);
    VtdExtCap = MmioRead64 (VtdBase + R_VTD_EXT_CAP_LOW);
    if ((VtdExtCap & B_VTD_EXT_CAP_LOW_DT) != 0) {
      //
      // Add DevScope for this stack port
      //
      DevScope = (EFI_ACPI_DMAR_DEVICE_SCOPE_ENTRY_STRUCTURE *)((UINT8 *)Atsr + Atsr->Length);
      DevScope->DeviceScopeEntryType = EFI_ACPI_DEVICE_SCOPE_ENTRY_TYPE_PCI_BRIDGE;
      DevScope->Length               = sizeof(EFI_ACPI_DMAR_DEVICE_SCOPE_ENTRY_STRUCTURE);
      DevScope->EnumerationID        = 0;
      DevScope->StartingBusNumber    = Bus;

      PciPath = (EFI_ACPI_DMAR_PCI_PATH *) ((UINT8 *)DevScope + DevScope->Length);
      for (PathIdx = 0; mDevScopeATSR_Icxd[Port].PciNode[PathIdx].Device != (UINT8) -1; PathIdx++,PciPath++) {
        CopyMem (PciPath, &(mDevScopeATSR_Icxd[Port].PciNode[PathIdx]), sizeof(EFI_ACPI_DMAR_PCI_PATH));
        DevScope->Length += sizeof(EFI_ACPI_DMAR_PCI_PATH);
      }

      DEBUG ((DEBUG_INFO, "[DMAR] [ATSR-PCIe] {S%d:P%d} (B%02X:D%02X:F%d)\n", Stack, Port, Bus, Dev, Func));
      Atsr->Length += DevScope->Length;

      //
      // Start from next byte location for another DMAR type.
      //
      *CurrentPtr = (VOID *) ((UINT8 *)Atsr + Atsr->Length);

      //
      // Update Dmar Hdr with Atsr Length for this stack
      //
      *AtsrLen = Atsr->Length;

    }
  } // PCIe port loop

  return EFI_SUCCESS;
}

/**
  Append RMRR structs to DMAR ACPI table.

  @param[in,out]  CurrentPtr            The address to start adding RMRR.
  @param[in,out]  RmrrLen               Length of RMRR struct addded.

  @retval         EFI_SUCCESS    ,      if RMRR is appended successfully
  @retval         EFI_OUT_OF_RESOURCES, if failed to assign Reserved memory for DMA
  @retval         EFI_INVALID_PARAMETER,if CurrentPtr is NULL
**/
EFI_STATUS
AppendRmrr (
  IN OUT VOID   **CurrentPtr,
  IN OUT UINT16  *RmrrLen
  )
{
  UINT8           PathIdx;
  UINTN           NumPages;
  EFI_ACPI_DMAR_RESERVED_MEMORY_REGION_REPORTING_STRUCTURE *Rmrr;
  EFI_ACPI_DMAR_DEVICE_SCOPE_ENTRY_STRUCTURE               *DevScope;
  EFI_ACPI_DMAR_PCI_PATH                                   *PciPath;

  if ((*CurrentPtr == NULL) || (RmrrLen == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  Rmrr = (EFI_ACPI_DMAR_RESERVED_MEMORY_REGION_REPORTING_STRUCTURE *) *CurrentPtr;
  *RmrrLen = 0;

  //
  // Add Reserved Region #1
  //
  Rmrr->Type                = EFI_ACPI_DMA_REMAPPING_STRUCTURE_TYPE_RMRR;
  Rmrr->Length              = sizeof (EFI_ACPI_DMAR_RESERVED_MEMORY_REGION_REPORTING_STRUCTURE);
  Rmrr->SegmentNumber       = 0;

  NumPages = 3;
  Rmrr->ReservedMemoryRegionBaseAddress = (UINT64) (UINTN) AllocatePages (NumPages);
  if (Rmrr->ReservedMemoryRegionBaseAddress == 0) {
    return EFI_OUT_OF_RESOURCES;
  } else {
    Rmrr->ReservedMemoryRegionLimitAddress = Rmrr->ReservedMemoryRegionBaseAddress + (NumPages * EFI_PAGE_SIZE) - 1;
  }

  DevScope = (EFI_ACPI_DMAR_DEVICE_SCOPE_ENTRY_STRUCTURE *) ((UINT8 *)Rmrr + Rmrr->Length);
  DevScope->DeviceScopeEntryType = EFI_ACPI_DEVICE_SCOPE_ENTRY_TYPE_ENDPOINT;
  DevScope->Length               = sizeof(EFI_ACPI_DMAR_DEVICE_SCOPE_ENTRY_STRUCTURE);
  DevScope->EnumerationID        = 0;
  DevScope->StartingBusNumber    = 0;

  PciPath = (EFI_ACPI_DMAR_PCI_PATH *) ((UINT8 *)DevScope + DevScope->Length);
  for (PathIdx = 0; mPciPathRmrr1[PathIdx].Device != (UINT8) -1; PathIdx++,PciPath++) {
    CopyMem (PciPath, &(mPciPathRmrr1[PathIdx]), sizeof(EFI_ACPI_DMAR_PCI_PATH));
    DevScope->Length += sizeof(EFI_ACPI_DMAR_PCI_PATH);
  }

  Rmrr->Length += DevScope->Length;

  DEBUG ((DEBUG_INFO, "[DMAR] [RMRR-PCIe] {RegBase=0x%016lX:RegLimit=0x%016lX}\n",
                                                                                Rmrr->ReservedMemoryRegionBaseAddress,
                                                                                Rmrr->ReservedMemoryRegionLimitAddress ));

  //
  // Start from next byte location for another DMAR type.
  //
  *CurrentPtr = (VOID *) ((UINT8 *)Rmrr + Rmrr->Length);

  //
  // Update Dmar Hdr with Rmrr Length for this stack
  //
  *RmrrLen = Rmrr->Length;

  return EFI_SUCCESS;
}

/**
  Update DMAR ACPI table header

  Set flags and Host Address Width.

  @param[in,out]  Table             The start address of DMAR table.
  @param[in]      RootBridgeInfoHob Pointer to the root bridge info hob.

  @retval         EFI_SUCCESS
  @retval         EFI_INVALID_PARAMETER, if RootBridge Info not found or Bus = -1
**/
EFI_STATUS
UpdateDmarHeader (
  IN OUT EFI_ACPI_DMAR_DESCRIPTION_TABLE   *Dmar,
  IN PCI_ROOT_BRIDGE_INFO_HOB              *RootBridgeInfoHob
  )
{
  UINTN                       VtdBar;
  UINT8                       Bus;
  UINT8                       Idx;
  UINT64                      VtdExtCap;
  EFI_CPUID_REGISTER          CpuidRegister;

  Dmar->Flags = EFI_ACPI_DMAR_TABLE_FLAGS_INTR_REMAP_CLEAR;
  Bus = (UINT8) -1;

  if (RootBridgeInfoHob == NULL) {
    DEBUG ((DEBUG_INFO, "RootBridgeInfoHob not found, returning\n"));
    return EFI_INVALID_PARAMETER;
  }

  //
  // Check INTR REMAP capability (on Stack0)
  //
  for (Idx = 0; Idx < RootBridgeInfoHob->Count; Idx++){
    if (Idx == IIO_STACK0) {
      Bus = RootBridgeInfoHob->Entry[Idx].BusBase;
      break;
    }
  }
  if (Bus == (UINT8) -1) {
    return EFI_INVALID_PARAMETER;
  }

  VtdBar = (UINTN) GetVtdBar (Bus);
  VtdExtCap = MmioRead64 (VtdBar + R_VTD_EXT_CAP_LOW);
  if ((VtdExtCap & B_VTD_EXT_CAP_LOW_IR) != 0) {
    Dmar->Flags |= EFI_ACPI_DMAR_TABLE_FLAGS_INTR_REMAP_SET; // iff InterruptRemap is enabled in config data also, check later
  }

  //
  // Check if CPU is in X2APIC mode
  //
  Dmar->Flags |= EFI_ACPI_DMAR_TABLE_FLAGS_X2APIC_OPT_OUT; // get from CPU's APIC mode (x2apiclib), check later

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
  UINT8                                         IioCount;
  UINT8                                         Idx;
  UINT8                                         Stack;
  UINT8                                        *Current;
  EFI_STATUS                                    Status;
  EFI_ACPI_DMAR_DESCRIPTION_TABLE              *Dmar;
  UINT16                                        DrhdLen;
  UINT16                                        AtsrLen;
  UINT16                                        RmrrLen;
  PCI_ROOT_BRIDGE_INFO_HOB                     *RootBridgeInfoHob;

  Status = EFI_SUCCESS;

  RootBridgeInfoHob = (PCI_ROOT_BRIDGE_INFO_HOB *) GetGuidHobData (NULL, NULL, &gLoaderPciRootBridgeInfoGuid);
  if (RootBridgeInfoHob == NULL) {
    DEBUG ((DEBUG_INFO, "RootBridgeInfoHob not found, returning\n"));
    Status = EFI_UNSUPPORTED;
    return Status;
  }

  Dmar = (EFI_ACPI_DMAR_DESCRIPTION_TABLE *) Table;
  Current = (UINT8 *) Dmar + sizeof(EFI_ACPI_DMAR_HEADER);

  //
  // Add DRHD for each IIO stack
  //
  GetIioCount (0, &IioCount);
  for (Idx = 0; Idx < IioCount; Idx++) {
    DrhdLen  = 0;
    if (GetChopType(0) == TypeHcc){
      Stack = mValidStackHcc[Idx];
    } else {
      Stack = mValidStackLcc[Idx];
    }
    Status   = AppendDrhd ((VOID **)&Current, Stack, &DrhdLen, RootBridgeInfoHob);
    if (EFI_ERROR(Status)) {
      DEBUG ((DEBUG_INFO, "Append DRHD failed, returning\n"));
      return Status;
    }
    Dmar->Header.Length += DrhdLen;
  }

  //
  // Add ATSR for each IIO stack RootPorts
  //
  for (Idx = 0; Idx < IioCount; Idx++) {
    AtsrLen  = 0;
    if (GetChopType(0) == TypeHcc){
      Stack = mValidStackHcc[Idx];
    } else {
      Stack = mValidStackLcc[Idx];
    }
    Status   = AppendAtsr ((VOID **)&Current, Stack, &AtsrLen, RootBridgeInfoHob);
    if (EFI_ERROR(Status)) {
      DEBUG ((DEBUG_INFO, "Append ATSR failed, returning\n"));
      return Status;
    }
    Dmar->Header.Length += AtsrLen;
  }

  // Add RMRR for DMA targets
  //
  RmrrLen = 0;
  Status = AppendRmrr ((VOID **)&Current, &RmrrLen);
  if (EFI_ERROR(Status)) {
    DEBUG ((DEBUG_INFO, "Append RMRR failed, returning\n"));
    return Status;
  }
  Dmar->Header.Length += RmrrLen;

  //
  // Update Dmar header fields
  //
  UpdateDmarHeader (Dmar, RootBridgeInfoHob);

  return Status;
}
