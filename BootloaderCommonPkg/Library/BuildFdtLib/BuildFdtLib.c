/** @file
  Copyright (c) 2024, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include <PiPei.h>
#include <IndustryStandard/Pci22.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/DebugLib.h>
#include <Library/HobLib.h>
#include <Library/PcdLib.h>
#include <Library/IoLib.h>
#include <Library/PrintLib.h>
#include <Library/FdtLib.h>
#include <Library/PciLib.h>
#include <UniversalPayload/UniversalPayload.h>
#include <UniversalPayload/AcpiTable.h>
#include <UniversalPayload/SerialPortInfo.h>
#include <UniversalPayload/PciRootBridges.h>
#include <UniversalPayload/DeviceTree.h>
#include <UniversalPayload/SmbiosTable.h>
#include <Guid/GraphicsInfoHob.h>
#include <Guid/UniversalPayloadBase.h>
#include <Library/BootloaderCoreLib.h>

#define IGD_BUS_NUM   0x00
#define IGD_DEV_NUM   0x02
#define IGD_FUN_NUM   0x00

BOOLEAN               mResourceAssigned;

CHAR8  *mMemoryAllocType[] = {
  "Reserved",
  "LoaderCode",
  "LoaderData",
  "boot-code",
  "boot-data",
  "runtime-code",
  "runtime-data",
  "ConventionalMemory",
  "UnusableMemory",
  "acpi",
  "acpi-nvs",
  "mmio",
  "MemoryMappedIOPortSpace",
  "PalCode",
  "PersistentMemory",
};

/**
  It will build FDT based on memory information from Hobs.

  @param[in] FdtBase         Address of the Fdt data.

  @retval EFI_SUCCESS        If it completed successfully.
  @retval Others             If it failed to build required FDT.
**/
EFI_STATUS
BuildFdtForMemory (
  IN     VOID  *FdtBase
  )
{
  EFI_STATUS                   Status;
  EFI_PEI_HOB_POINTERS         Hob;
  EFI_HOB_RESOURCE_DESCRIPTOR  *ResourceHob;
  VOID                         *HobStart;
  VOID                         *Fdt;
  INT32                        TempNode;
  CHAR8                        TempStr[32];
  UINT64                       RegTmp[2];

  Fdt = FdtBase;

  HobStart = GetFirstHob (EFI_HOB_TYPE_RESOURCE_DESCRIPTOR);
  //
  // Scan resource descriptor hobs to set memory nodes
  //
  for (Hob.Raw = HobStart; !END_OF_HOB_LIST (Hob); Hob.Raw = GET_NEXT_HOB (Hob)) {
    if (GET_HOB_TYPE (Hob) == EFI_HOB_TYPE_RESOURCE_DESCRIPTOR) {
      ResourceHob = Hob.ResourceDescriptor;
      // Memory
      if (ResourceHob->ResourceType == EFI_RESOURCE_SYSTEM_MEMORY) {
        // DEBUG ((DEBUG_ERROR, "Found hob for memory: base %016lX  length %016lX\n", ResourceHob->PhysicalStart, ResourceHob->ResourceLength));

        AsciiSPrint (TempStr, sizeof (TempStr), "memory@%lX", ResourceHob->PhysicalStart);
        TempNode = FdtAddSubnode (Fdt, 0, TempStr);
        ASSERT (TempNode > 0);

        RegTmp[0] = CpuToFdt64 (ResourceHob->PhysicalStart);
        RegTmp[1] = CpuToFdt64 (ResourceHob->ResourceLength);
        Status    = FdtSetProperty (Fdt, TempNode, "reg", &RegTmp, sizeof (RegTmp));
        ASSERT_EFI_ERROR (Status);

        Status = FdtSetProperty (Fdt, TempNode, "device_type", "memory", (UINT32)(AsciiStrLen ("memory")+1));
        ASSERT_EFI_ERROR (Status);
      }
    }
  }

  return Status;
}

/**
  It will build FDT based on memory allocation information from Hobs.

  @param[in] FdtBase         Address of the Fdt data.

  @retval EFI_SUCCESS        If it completed successfully.
  @retval Others             If it failed to build required FDT.
**/
EFI_STATUS
BuildFdtForMemAlloc (
  IN     VOID  *FdtBase
  )
{
  EFI_STATUS                      Status;
  EFI_PEI_HOB_POINTERS            Hob;
  VOID                            *HobStart;
  VOID                            *Fdt;
  INT32                           ParentNode;
  INT32                           TempNode;
  CHAR8                           TempStr[32];
  UINT64                          RegTmp[2];
  UINT32                          AllocMemType;
  EFI_GUID                        *AllocMemName;
  UINT8                           IsStackHob;
  UINT8                           IsBspStore;
  UINT32                          Data32;
  UNIVERSAL_PAYLOAD_SMBIOS_TABLE  *SmbiosTable;
  UNIVERSAL_PAYLOAD_ACPI_TABLE    *AcpiTable;
  EFI_HOB_GUID_TYPE               *GuidHob;

  Fdt = FdtBase;

  ParentNode = FdtAddSubnode (Fdt, 0, "reserved-memory");
  ASSERT (ParentNode > 0);

  Data32 = CpuToFdt32 (2);
  Status = FdtSetProperty (Fdt, ParentNode, "#address-cells", &Data32, sizeof (UINT32));
  ASSERT_EFI_ERROR (Status);
  Status = FdtSetProperty (Fdt, ParentNode, "#size-cells", &Data32, sizeof (UINT32));
  ASSERT_EFI_ERROR (Status);

  GuidHob     = NULL;
  SmbiosTable = NULL;
  GuidHob     = GetFirstGuidHob (&gUniversalPayloadSmbios3TableGuid);
  if (GuidHob != NULL) {
    SmbiosTable = GET_GUID_HOB_DATA (GuidHob);
    DEBUG ((DEBUG_INFO, "To build Smbios memory FDT ,SmbiosTable :%lx, SmBiosEntryPoint :%lx\n", (UINTN)SmbiosTable, SmbiosTable->SmBiosEntryPoint));
    AsciiSPrint (TempStr, sizeof (TempStr), "memory@%lX", SmbiosTable->SmBiosEntryPoint);
    DEBUG ((DEBUG_INFO, "To build Smbios memory FDT #2, SmbiosTable->Header.Length  :%x\n", SmbiosTable->Header.Length));
    TempNode = 0;
    TempNode = FdtAddSubnode (Fdt, ParentNode, TempStr);
    DEBUG ((DEBUG_INFO, "FdtAddSubnode %x", TempNode));
    RegTmp[0] = CpuToFdt64 (SmbiosTable->SmBiosEntryPoint);
    RegTmp[1] = CpuToFdt64 (SmbiosTable->Header.Length);
    Status = FdtSetProperty (Fdt, TempNode, "reg", &RegTmp, sizeof (RegTmp));
    ASSERT_EFI_ERROR (Status);
    Status = FdtSetProperty (Fdt, TempNode, "compatible", "smbios", (UINT32)(AsciiStrLen ("smbios")+1));
    ASSERT_EFI_ERROR (Status);
  }

  GuidHob = GetFirstGuidHob (&gUniversalPayloadAcpiTableGuid);
  if (GuidHob != NULL) {
    AcpiTable = (UNIVERSAL_PAYLOAD_ACPI_TABLE *)GET_GUID_HOB_DATA (GuidHob);
  }

  HobStart = GetFirstHob (EFI_HOB_TYPE_MEMORY_ALLOCATION);
  //
  // Scan memory allocation hobs to set memory type
  //
  for (Hob.Raw = HobStart; !END_OF_HOB_LIST (Hob); Hob.Raw = GET_NEXT_HOB (Hob)) {
    if (GET_HOB_TYPE (Hob) == EFI_HOB_TYPE_MEMORY_ALLOCATION) {
      AllocMemName = NULL;
      IsStackHob   = 0;
      IsBspStore   = 0;
      if (CompareGuid (&(Hob.MemoryAllocationModule->MemoryAllocationHeader.Name), &gEfiHobMemoryAllocModuleGuid)) {
        continue;
      } else if (IsZeroGuid (&(Hob.MemoryAllocationModule->MemoryAllocationHeader.Name)) == FALSE) {
        AllocMemName = &(Hob.MemoryAllocationModule->MemoryAllocationHeader.Name);

        if (CompareGuid (AllocMemName, &gEfiHobMemoryAllocStackGuid)) {
          IsStackHob = 1;
        } else if (CompareGuid (AllocMemName, &gEfiHobMemoryAllocBspStoreGuid)) {
          IsBspStore = 1;
        }
      }

      DEBUG ((
        DEBUG_INFO,
        "Found hob for rsvd memory alloc: base %016lX  length %016lX  type %x\n",
        Hob.MemoryAllocation->AllocDescriptor.MemoryBaseAddress,
        Hob.MemoryAllocation->AllocDescriptor.MemoryLength,
        Hob.MemoryAllocation->AllocDescriptor.MemoryType
        ));

      AllocMemType = Hob.MemoryAllocation->AllocDescriptor.MemoryType;
      if (IsStackHob == 1) {
        AsciiSPrint (
                   TempStr,
                   sizeof (TempStr),
                   "%a@%lX",
                   "stackhob",
                   Hob.MemoryAllocation->AllocDescriptor.MemoryBaseAddress
                   );
      } else if (IsBspStore == 1) {
        AsciiSPrint (
                   TempStr,
                   sizeof (TempStr),
                   "%a@%lX",
                   "bspstore",
                   Hob.MemoryAllocation->AllocDescriptor.MemoryBaseAddress
                   );
      } else {
        AsciiSPrint (
                   TempStr,
                   sizeof (TempStr),
                   "%a@%lX",
                   mMemoryAllocType[AllocMemType],
                   Hob.MemoryAllocation->AllocDescriptor.MemoryBaseAddress
                   );
      }

      if (AsciiStrCmp (mMemoryAllocType[AllocMemType], "ConventionalMemory") == 0) {
        continue;
      }

      if (AsciiStrCmp (mMemoryAllocType[AllocMemType], "mmio") == 0) {
        AsciiSPrint (TempStr, sizeof (TempStr), "mmio@%lX", Hob.MemoryAllocation->AllocDescriptor.MemoryBaseAddress);
      } else {
        AsciiSPrint (TempStr, sizeof (TempStr), "memory@%lX", Hob.MemoryAllocation->AllocDescriptor.MemoryBaseAddress);
      }

      TempNode = FdtAddSubnode (Fdt, ParentNode, TempStr);
      DEBUG ((DEBUG_INFO, "FdtAddSubnode %x", TempNode));
      if (TempNode < 0) {
        continue;
      }

      RegTmp[0] = CpuToFdt64 (Hob.MemoryAllocation->AllocDescriptor.MemoryBaseAddress);
      RegTmp[1] = CpuToFdt64 (Hob.MemoryAllocation->AllocDescriptor.MemoryLength);
      Status    = FdtSetProperty (Fdt, TempNode, "reg", &RegTmp, sizeof (RegTmp));
      ASSERT_EFI_ERROR (Status);

      if ((AsciiStrCmp (mMemoryAllocType[AllocMemType], "mmio") == 0)) {
        continue;
      }

      if (!(AsciiStrCmp (mMemoryAllocType[AllocMemType], "acpi-nvs") == 0) && (AsciiStrCmp (mMemoryAllocType[AllocMemType], "acpi") == 0)) {
        DEBUG ((DEBUG_INFO, "find acpi memory hob MemoryBaseAddress:%x , AcpiTable->Rsdp :%x\n", Hob.MemoryAllocation->AllocDescriptor.MemoryBaseAddress, AcpiTable->Rsdp));
        if (Hob.MemoryAllocation->AllocDescriptor.MemoryBaseAddress == AcpiTable->Rsdp) {
          DEBUG ((DEBUG_INFO, "keep acpi memory hob  \n"));
          Status = FdtSetProperty (Fdt, TempNode, "compatible", mMemoryAllocType[AllocMemType], (UINT32)(AsciiStrLen (mMemoryAllocType[AllocMemType])+1));
          ASSERT_EFI_ERROR (Status);
        } else {
          DEBUG ((DEBUG_INFO, "change acpi memory hob  \n"));
          Status = FdtSetProperty (Fdt, TempNode, "compatible", mMemoryAllocType[4], (UINT32)(AsciiStrLen (mMemoryAllocType[4])+1));
          ASSERT_EFI_ERROR (Status);
        }
      } else {
        DEBUG ((DEBUG_INFO, "other memory hob  \n"));
        Status = FdtSetProperty (Fdt, TempNode, "compatible", mMemoryAllocType[AllocMemType], (UINT32)(AsciiStrLen (mMemoryAllocType[AllocMemType])+1));
        ASSERT_EFI_ERROR (Status);
      }
    }
  }

  return Status;
}

/**
  It will build FDT based on serial information.

  @param[in] FdtBase         Address of the Fdt data.

  @retval EFI_SUCCESS        If it completed successfully.
  @retval Others             If it failed to build required FDT.
**/
EFI_STATUS
BuildFdtForSerial (
  IN     VOID   *FdtBase
  )
{
  EFI_HOB_GUID_TYPE                   *GuidHob;
  EFI_STATUS                          Status;
  UNIVERSAL_PAYLOAD_SERIAL_PORT_INFO  *SerialPortInfo;
  VOID                                *Fdt;
  INT32                               TempNode;
  UINT32                              Data32;
  UINT32                              RegData[2];
  CHAR8                               TempStr[32];

  Status = EFI_SUCCESS;
  Fdt    = FdtBase;

  DEBUG ((DEBUG_INFO, "BuildFdtForSerial start \n"));
  GuidHob = GetFirstGuidHob (&gUniversalPayloadSerialPortInfoGuid);
  while (GuidHob != NULL) {
    SerialPortInfo = (UNIVERSAL_PAYLOAD_SERIAL_PORT_INFO *)GET_GUID_HOB_DATA (GuidHob);

    AsciiSPrint (TempStr, sizeof (TempStr), "serial@%lX", SerialPortInfo->RegisterBase);
    TempNode = FdtAddSubnode (Fdt, 0, TempStr);
    ASSERT (TempNode > 0);

    Data32 = CpuToFdt32 (SerialPortInfo->BaudRate);
    Status = FdtSetProperty (Fdt, TempNode, "current-speed", &Data32, sizeof (Data32));
    ASSERT_EFI_ERROR (Status);

    Data32 = CpuToFdt32 ((UINT32)HighBitSet32 (SerialPortInfo->RegisterStride));
    Status = FdtSetProperty (Fdt, TempNode, "reg-shift", &Data32, sizeof (Data32));
    ASSERT_EFI_ERROR (Status);

    if (SerialPortInfo->UseMmio) {
      RegData[0] = CpuToFdt32 (0);
      Data32     = CpuToFdt32 (4);
    } else {
      RegData[0] = CpuToFdt32 (1);
      Data32     = CpuToFdt32 (1);
    }
    RegData[1] = CpuToFdt32 ((UINT32)(UINTN)(SerialPortInfo->RegisterBase & 0x0FFFFFFFF));
    Status     = FdtSetProperty (Fdt, TempNode, "reg", &RegData, sizeof (RegData));
    ASSERT_EFI_ERROR (Status);

    Status = FdtSetProperty (Fdt, TempNode, "reg-io-width", &Data32, sizeof (Data32));
    ASSERT_EFI_ERROR (Status);

    Status = FdtSetProperty (Fdt, TempNode, "compatible", "ns16550a", (UINT32) AsciiStrSize ("ns16550a"));
    ASSERT_EFI_ERROR (Status);

    DEBUG ((DEBUG_INFO, "SerialPortInfo->UseMmio        = 0x%x\n", SerialPortInfo->UseMmio));
    DEBUG ((DEBUG_INFO, "SerialPortInfo->RegisterStride = 0x%x\n", SerialPortInfo->RegisterStride));
    DEBUG ((DEBUG_INFO, "SerialPortInfo->RegisterBase   = 0x%llx\n", SerialPortInfo->RegisterBase));
    DEBUG ((DEBUG_INFO, "SerialPortInfo->BaudRate       = %d\n", SerialPortInfo->BaudRate));

    GuidHob = GET_NEXT_HOB (GuidHob);
    GuidHob = GetNextGuidHob (&gUniversalPayloadSerialPortInfoGuid, GuidHob);
  }

  return Status;
}

/**
  It will build FDT based on BuildFdtForPciRootBridge information.

  @param[in] FdtBase         Address of the Fdt data.

  @retval EFI_SUCCESS        If it completed successfully.
  @retval Others             If it failed to build required FDT.
**/
EFI_STATUS
BuildFdtForPciRootBridge (
  IN     VOID  *FdtBase
  )
{
  EFI_STATUS                                        Status;
  VOID                                              *Fdt;
  INT32                                             TempNode;
  INT32                                             GmaNode;
  CHAR8                                             TempStr[32];
  CHAR8                                             GmaStr[32];
  UINT32                                            RegTmp[2];
  UINT32                                            RegData[21];
  UINT32                                            DMARegData[8];
  UINT64                                            Reg64Data[2];
  UINT32                                            Data32;
  UINT64                                            Data64;
  UINT8                                             BusNumber;
  UINT8                                             BusLimit;
  EFI_HOB_GUID_TYPE                                 *GuidHob;
  UNIVERSAL_PAYLOAD_GENERIC_HEADER                  *GenericHeader;
  UNIVERSAL_PAYLOAD_PCI_ROOT_BRIDGES                *PciRootBridgeInfo;
  UINT8                                             Index;
  PCI_TYPE00                                        PciData;

  Fdt               = FdtBase;
  BusNumber         = 0;
  BusLimit          = 0;
  Status            = EFI_SUCCESS;
  PciRootBridgeInfo = NULL;

  DEBUG ((DEBUG_INFO, "%a: #1 \n", __func__));
  //
  // Create BuildFdtForPciRootBridge FDT node.
  //

  GuidHob = GetFirstGuidHob (&gUniversalPayloadPciRootBridgeInfoGuid);
  if (GuidHob != NULL) {
    GenericHeader = (UNIVERSAL_PAYLOAD_GENERIC_HEADER *)GET_GUID_HOB_DATA (GuidHob);
    if ((sizeof (UNIVERSAL_PAYLOAD_GENERIC_HEADER) <= GET_GUID_HOB_DATA_SIZE (GuidHob)) && (GenericHeader->Length <= GET_GUID_HOB_DATA_SIZE (GuidHob))) {
      if ((GenericHeader->Revision == UNIVERSAL_PAYLOAD_PCI_ROOT_BRIDGES_REVISION) && (GenericHeader->Length >= sizeof (UNIVERSAL_PAYLOAD_PCI_ROOT_BRIDGES))) {
        DEBUG ((DEBUG_INFO, "%a: #2 \n", __func__));

        //
        // UNIVERSAL_PAYLOAD_PCI_ROOT_BRIDGES structure is used when Revision equals to UNIVERSAL_PAYLOAD_PCI_ROOT_BRIDGES_REVISION
        //
        PciRootBridgeInfo = (UNIVERSAL_PAYLOAD_PCI_ROOT_BRIDGES *)GET_GUID_HOB_DATA (GuidHob);

        if (PciRootBridgeInfo != NULL) {
          DEBUG ((DEBUG_INFO, "PciRootBridgeInfo->Count %x\n", PciRootBridgeInfo->Count));
          DEBUG ((DEBUG_INFO, "PciRootBridge->Segment %x, \n", PciRootBridgeInfo->RootBridge[0].Segment));

          DEBUG ((DEBUG_INFO, "PciRootBridge->Bus.Base %x, \n", PciRootBridgeInfo->RootBridge[0].Bus.Base));
          DEBUG ((DEBUG_INFO, "PciRootBridge->Bus.limit %x, \n", PciRootBridgeInfo->RootBridge[0].Bus.Limit));

          DEBUG ((DEBUG_INFO, "PciRootBridge->Mem.Base %x, \n", PciRootBridgeInfo->RootBridge[0].Mem.Base));
          DEBUG ((DEBUG_INFO, "PciRootBridge->Mem.limit %x, \n", PciRootBridgeInfo->RootBridge[0].Mem.Limit));

          DEBUG ((DEBUG_INFO, "PciRootBridge->MemAbove4G.Base %llx, \n", PciRootBridgeInfo->RootBridge[0].MemAbove4G.Base));
          DEBUG ((DEBUG_INFO, "PciRootBridge->MemAbove4G.limit %llx, \n", PciRootBridgeInfo->RootBridge[0].MemAbove4G.Limit));

          DEBUG ((DEBUG_INFO, "PciRootBridge->PMem.Base %llx, \n", PciRootBridgeInfo->RootBridge[0].PMem.Base));
          DEBUG ((DEBUG_INFO, "PciRootBridge->PMem.limit %llx, \n", PciRootBridgeInfo->RootBridge[0].PMem.Limit));

          DEBUG ((DEBUG_INFO, "PciRootBridge->Bus.Base %x, \n", PciRootBridgeInfo->RootBridge[1].Bus.Base));
          DEBUG ((DEBUG_INFO, "PciRootBridge->Bus.limit %x, \n", PciRootBridgeInfo->RootBridge[1].Bus.Limit));

          DEBUG ((DEBUG_INFO, "PciRootBridge->Mem.Base %x, \n", PciRootBridgeInfo->RootBridge[1].Mem.Base));
          DEBUG ((DEBUG_INFO, "PciRootBridge->Mem.limit %x, \n", PciRootBridgeInfo->RootBridge[1].Mem.Limit));

          DEBUG ((DEBUG_INFO, "PciRootBridge->MemAbove4G.Base %llx, \n", PciRootBridgeInfo->RootBridge[1].MemAbove4G.Base));
          DEBUG ((DEBUG_INFO, "PciRootBridge->MemAbove4G.limit %llx, \n", PciRootBridgeInfo->RootBridge[1].MemAbove4G.Limit));

          DEBUG ((DEBUG_INFO, "PciRootBridge->PMem.Base %x, \n", PciRootBridgeInfo->RootBridge[1].PMem.Base));
          DEBUG ((DEBUG_INFO, "PciRootBridge->PMem.limit %x, \n", PciRootBridgeInfo->RootBridge[1].PMem.Limit));
        }
      }
    }
  }

  if (PciRootBridgeInfo != NULL) {
    for (Index = 0; Index < PciRootBridgeInfo->Count; Index++) {
      UINTN  PciExpressBaseAddress;

      mResourceAssigned     = PciRootBridgeInfo->ResourceAssigned;
      PciExpressBaseAddress = (UINTN) PcdGet64 (PcdPciExpressBaseAddress) + (PCI_LIB_ADDRESS (PciRootBridgeInfo->RootBridge[Index].Bus.Base, 0, 0, 0));
      AsciiSPrint (TempStr, sizeof (TempStr), "pci-rb%d@%lX", Index, PciExpressBaseAddress);
      TempNode              = FdtAddSubnode (Fdt, 0, TempStr);
      ASSERT (TempNode > 0);
      SetMem (RegData, sizeof (RegData), 0);

      // non-reloc/non-prefetch/mmio, child-addr, parent-addr, length
      Data32     = (N_NON_RELOCATABLE + SS_32BIT_MEMORY_SPACE);
      RegData[0] = CpuToFdt32 (Data32);
      DEBUG ((DEBUG_INFO, "PciRootBridge->Mem.Base RegData[0] %x, \n", Data32));

      // child-addr
      RegData[1] = CpuToFdt32 (0);
      Data32     = (UINT32)PciRootBridgeInfo->RootBridge[Index].Mem.Base;
      RegData[2] = CpuToFdt32 (Data32);
      DEBUG ((DEBUG_INFO, "PciRootBridge->Mem.Base RegData[2] %x, \n", Data32));

      // parent-addr
      RegData[3] = CpuToFdt32 (0);
      RegData[4] = CpuToFdt32 (Data32);
      DEBUG ((DEBUG_INFO, "PciRootBridge->Mem.Base RegData[4] %x, \n", Data32));

      // size
      Data64 = (PciRootBridgeInfo->RootBridge[Index].Mem.Limit - PciRootBridgeInfo->RootBridge[Index].Mem.Base + 1);
      if (Data64 & 0xFFFFFFFF00000000) {
        Data32 = (UINT32)RShiftU64 ((Data64 & 0xFFFFFFFF00000000), 31);
      } else {
        Data32 = 0;
      }

      DEBUG ((DEBUG_INFO, "PciRootBridge->Mem.size RegData[5] %x, \n", Data32));
      RegData[5] = CpuToFdt32 (Data32);
      Data32     = (UINT32)((Data64 & 0x0FFFFFFFF));
      DEBUG ((DEBUG_INFO, "PciRootBridge->Mem.size RegData[6] %x, \n", Data32));

      RegData[6] = CpuToFdt32 (Data32);

      // non-reloc/non-prefetch/64 mmio, child-addr, parent-addr, length
      Data32     = (N_NON_RELOCATABLE + SS_64BIT_MEMORY_SPACE);
      RegData[7] = CpuToFdt32 (Data32);
      DEBUG ((DEBUG_INFO, "PciRootBridge->MemAbove4G.Base RegData[7] %x, \n", Data32));

      // child-addr
      Data64 = PciRootBridgeInfo->RootBridge[Index].MemAbove4G.Base;
      Data32 = (UINT32)RShiftU64 ((Data64 & 0xFFFFFFFF00000000), 32);

      RegData[8] = CpuToFdt32 (Data32);
      DEBUG ((DEBUG_INFO, "PciRootBridge->MemAbove4G.Base RegData[8] %x, \n", Data32));
      Data32     = (UINT32)((Data64 & 0x0FFFFFFFF));
      RegData[9] = CpuToFdt32 (Data32);
      DEBUG ((DEBUG_INFO, "PciRootBridge->MemAbove4G.Base RegData[9] %x, \n", Data32));

      // parent-addr
      RegData[10] = RegData[8];
      RegData[11] = RegData[9];

      // size
      Data64 = (PciRootBridgeInfo->RootBridge[Index].MemAbove4G.Limit - PciRootBridgeInfo->RootBridge[Index].MemAbove4G.Base + 1);
      if (Data64 & 0xFFFFFFFF00000000) {
        Data32 = (UINT32)RShiftU64 ((Data64 & 0xFFFFFFFF00000000), 32);
      } else {
        Data32 = 0;
      }

      RegData[12] = CpuToFdt32 (Data32);
      DEBUG ((DEBUG_INFO, "PciRootBridge->MemAbove4G.size RegData[12] %x, \n", Data32));

      Data32      = (UINT32)((Data64 & 0x0FFFFFFFF));
      RegData[13] = CpuToFdt32 (Data32);
      DEBUG ((DEBUG_INFO, "PciRootBridge->MemAbove4G.size RegData[13] %x, \n", Data32));

      // non-reloc/32bit/io, child-addr, parent-addr, length
      Data32 = (N_NON_RELOCATABLE + SS_IO_SPACE);

      RegData[14] = CpuToFdt32 (Data32);
      DEBUG ((DEBUG_INFO, "PciRootBridge->Io.base RegData[14] %x, \n", Data32));

      Data32 = (UINT32)PciRootBridgeInfo->RootBridge[Index].Io.Base;
      // child-addr
      RegData[15] = CpuToFdt32 (0);
      RegData[16] = CpuToFdt32 (Data32);
      DEBUG ((DEBUG_INFO, "PciRootBridge->Io.base RegData[16] %x, \n", Data32));

      // parent-addr
      RegData[17] = CpuToFdt32 (0);
      RegData[18] = CpuToFdt32 (Data32);
      // size
      Data64 = (PciRootBridgeInfo->RootBridge[Index].Io.Limit - PciRootBridgeInfo->RootBridge[Index].Io.Base + 1);
      if (Data64 & 0xFFFFFFFF00000000) {
        Data32 = (UINT32)RShiftU64 ((Data64 & 0xFFFFFFFF00000000), 32);
      } else {
        Data32 = 0;
      }

      RegData[19] = CpuToFdt32 (Data32);
      DEBUG ((DEBUG_INFO, "PciRootBridge->Io.base size [19] %x, \n", Data32));

      Data32      = (UINT32)((Data64 & 0x0FFFFFFFF));
      RegData[20] = CpuToFdt32 (Data32);
      DEBUG ((DEBUG_INFO, "PciRootBridge->Io.base size [20] %x, \n", Data32));

      Status = FdtSetProperty (Fdt, TempNode, "ranges", &RegData, sizeof (RegData));
      ASSERT_EFI_ERROR (Status);

      // non-reloc/non-prefetch/memory, child-addr, parent-addr, length
      // indicate rb1 does not support above 4GB DMA
      Data32 = (N_NON_RELOCATABLE + SS_32BIT_MEMORY_SPACE);

      DMARegData[0] = CpuToFdt32 (Data32);
      DEBUG ((DEBUG_INFO, "PciRootBridge->DMA base  RegData[0] %x, \n", Data32));

      // child-addr
      DMARegData[2] = CpuToFdt32 (0);
      DMARegData[3] = CpuToFdt32 (0);
      // parent-addr
      DMARegData[4] = CpuToFdt32 (0);
      DMARegData[5] = CpuToFdt32 (0);
      // size
      DMARegData[6] = CpuToFdt32 (1);
      DMARegData[7] = CpuToFdt32 (0);

      Status = FdtSetProperty (Fdt, TempNode, "dma-ranges", &DMARegData, sizeof (DMARegData));
      ASSERT_EFI_ERROR (Status);

      ASSERT (PciRootBridgeInfo->RootBridge[Index].Bus.Base <= 0xFF);
      ASSERT (PciRootBridgeInfo->RootBridge[Index].Bus.Limit <= 0xFF);

      Reg64Data[0] = CpuToFdt64 (PciExpressBaseAddress + LShiftU64 (PciRootBridgeInfo->RootBridge[Index].Bus.Base, 20));
      Reg64Data[1] = CpuToFdt64 (LShiftU64 (PciRootBridgeInfo->RootBridge[Index].Bus.Limit +1, 20));

      Status = FdtSetProperty (Fdt, TempNode, "reg", &Reg64Data, sizeof (Reg64Data));
      ASSERT_EFI_ERROR (Status);

      BusNumber = PciRootBridgeInfo->RootBridge[Index].Bus.Base & 0xFF;
      RegTmp[0] = CpuToFdt32 (BusNumber);
      BusLimit  = PciRootBridgeInfo->RootBridge[Index].Bus.Limit & 0xFF;
      RegTmp[1] = CpuToFdt32 (BusLimit);
      DEBUG ((DEBUG_INFO, "PciRootBridge->BusNumber %x, \n", BusNumber));
      DEBUG ((DEBUG_INFO, "PciRootBridge->BusLimit  %x, \n", BusLimit));

      Status = FdtSetProperty (Fdt, TempNode, "bus-range", &RegTmp, sizeof (RegTmp));
      ASSERT_EFI_ERROR (Status);

      Data32 = CpuToFdt32 (2);
      Status = FdtSetProperty (Fdt, TempNode, "#size-cells", &Data32, sizeof (UINT32));
      ASSERT_EFI_ERROR (Status);

      Data32 = CpuToFdt32 (3);
      Status = FdtSetProperty (Fdt, TempNode, "#address-cells", &Data32, sizeof (UINT32));
      ASSERT_EFI_ERROR (Status);

      Status = FdtSetProperty (Fdt, TempNode, "compatible", "pci-rb", (UINT32)(AsciiStrLen ("pci-rb")+1));
      ASSERT_EFI_ERROR (Status);

      if (Index == 0) {
        PciExpressBaseAddress = (UINTN) PCI_LIB_ADDRESS (IGD_BUS_NUM, IGD_DEV_NUM, IGD_FUN_NUM, 0);
        AsciiSPrint (GmaStr, sizeof (GmaStr), "gma@%lX", PciExpressBaseAddress);
        GmaNode               = FdtAddSubnode (Fdt, TempNode, GmaStr);

        PciData.Hdr.VendorId   = PciRead16 (PciExpressBaseAddress + PCI_VENDOR_ID_OFFSET);

        PciData.Hdr.DeviceId   = PciRead16 (PciExpressBaseAddress + PCI_DEVICE_ID_OFFSET);
        PciData.Hdr.RevisionID = PciRead8  (PciExpressBaseAddress + PCI_REVISION_ID_OFFSET);
        PciData.Device.SubsystemVendorID = PciRead16 (PciExpressBaseAddress + PCI_SVID_OFFSET);
        PciData.Device.SubsystemID = PciRead16 (PciExpressBaseAddress + PCI_SID_OFFSET);

        Data32 = CpuToFdt32 (PciData.Device.SubsystemID);
        Status = FdtSetProperty (Fdt, GmaNode, "subsystem-id", &Data32, sizeof (UINT32));
        ASSERT_EFI_ERROR (Status);

        Data32 = CpuToFdt32 (PciData.Device.SubsystemVendorID);
        Status = FdtSetProperty (Fdt, GmaNode, "subsystem-vendor-id", &Data32, sizeof (UINT32));
        ASSERT_EFI_ERROR (Status);

        Data32 = CpuToFdt32 (PciData.Hdr.RevisionID);
        Status = FdtSetProperty (Fdt, GmaNode, "revision-id", &Data32, sizeof (UINT32));
        ASSERT_EFI_ERROR (Status);

        Data32 = CpuToFdt32 (PciData.Hdr.DeviceId);
        Status = FdtSetProperty (Fdt, GmaNode, "device-id", &Data32, sizeof (UINT32));
        ASSERT_EFI_ERROR (Status);

        Data32 = CpuToFdt32 (PciData.Hdr.VendorId);
        Status = FdtSetProperty (Fdt, GmaNode, "vendor-id", &Data32, sizeof (UINT32));
        ASSERT_EFI_ERROR (Status);
      }
    }
  }

  return Status;
}

/**
  It will build FDT based on FrameBuffer.

  @param[in] FdtBase         Address of the Fdt data.

  @retval EFI_SUCCESS        If it completed successfully.
  @retval Others             If it failed to build required FDT.
**/
EFI_STATUS
BuildFdtForFrameBuffer (
  IN     VOID  *FdtBase
  )
{
  EFI_STATUS                 Status;
  VOID                       *Fdt;
  INT32                      TempNode;
  UINT32                     Data32;
  CHAR8                      TempStr[32];
  UINT64                     RegData[2];
  EFI_HOB_GUID_TYPE          *GuidHob;
  EFI_PEI_GRAPHICS_INFO_HOB  *GraphicsInfo;

  Fdt = FdtBase;

  GuidHob = GetFirstGuidHob (&gEfiGraphicsInfoHobGuid);
  if (GuidHob != NULL) {
    GraphicsInfo = (EFI_PEI_GRAPHICS_INFO_HOB *)(GET_GUID_HOB_DATA (GuidHob));
    AsciiSPrint (TempStr, sizeof (TempStr), "framebuffer@%lX", GraphicsInfo->FrameBufferBase);
    TempNode     = FdtAddSubnode (Fdt, 0, TempStr);
    ASSERT (TempNode > 0);

    Status = FdtSetProperty (Fdt, TempNode, "display", "&gma", (UINT32)(AsciiStrLen ("&gma")+1));
    ASSERT_EFI_ERROR (Status);

    Status = FdtSetProperty (Fdt, TempNode, "format", "a8r8g8b8", (UINT32)(AsciiStrLen ("a8r8g8b8")+1));
    ASSERT_EFI_ERROR (Status);

    Data32 = CpuToFdt32 (GraphicsInfo->GraphicsMode.VerticalResolution);
    Status = FdtSetProperty (Fdt, TempNode, "height", &Data32, sizeof (UINT32));
    ASSERT_EFI_ERROR (Status);

    Data32 = CpuToFdt32 (GraphicsInfo->GraphicsMode.HorizontalResolution);
    Status = FdtSetProperty (Fdt, TempNode, "width", &Data32, sizeof (UINT32));
    ASSERT_EFI_ERROR (Status);

    RegData[0] = CpuToFdt64 (GraphicsInfo->FrameBufferBase);
    RegData[1] = CpuToFdt64 (GraphicsInfo->FrameBufferSize);
    Status     = FdtSetProperty (Fdt, TempNode, "reg", &RegData, sizeof (RegData));
    ASSERT_EFI_ERROR (Status);

    Status = FdtSetProperty (Fdt, TempNode, "compatible", "simple-framebuffer", (UINT32)(AsciiStrLen ("simple-framebuffer")+1));
    ASSERT_EFI_ERROR (Status);
  } else {
    AsciiSPrint (TempStr, sizeof (TempStr), "framebuffer@%lX", 0xB0000000);
    TempNode = FdtAddSubnode (Fdt, 0, TempStr);
    ASSERT (TempNode > 0);

    Status = FdtSetProperty (Fdt, TempNode, "display", "&gma", (UINT32)(AsciiStrLen ("&gma")+1));
    ASSERT_EFI_ERROR (Status);

    Status = FdtSetProperty (Fdt, TempNode, "format", "a8r8g8b8", (UINT32)(AsciiStrLen ("a8r8g8b8")+1));
    ASSERT_EFI_ERROR (Status);

    Data32 = CpuToFdt32 (1024);
    Status = FdtSetProperty (Fdt, TempNode, "height", &Data32, sizeof (UINT32));
    ASSERT_EFI_ERROR (Status);

    Data32 = CpuToFdt32 (1280);
    Status = FdtSetProperty (Fdt, TempNode, "width", &Data32, sizeof (UINT32));
    ASSERT_EFI_ERROR (Status);

    RegData[0] = CpuToFdt64 (0xB0000000);
    RegData[1] = CpuToFdt64 (0x500000);
    Status     = FdtSetProperty (Fdt, TempNode, "reg", &RegData, sizeof (RegData));
    ASSERT_EFI_ERROR (Status);

    Status = FdtSetProperty (Fdt, TempNode, "compatible", "simple-framebuffer", (UINT32)(AsciiStrLen ("simple-framebuffer")+1));
    ASSERT_EFI_ERROR (Status);
  }

  return Status;
}

/**
  It will build FDT for UPL required data.

  @param[in] FdtBase         Address of the Fdt data.

  @retval EFI_SUCCESS        If it completed successfully.
  @retval Others             If it failed to build required FDT.
**/
EFI_STATUS
BuildFdtForUplRequired (
  IN     VOID  *FdtBase
  )
{
  EFI_STATUS              Status;
  VOID                    *Fdt;
  VOID                    *Fit;
  INT32                   ParentNode;
  INT32                   CustomNode;
  INT32                   UPLParaNode;
  INT32                   UPLImageNode;
  EFI_HOB_CPU             *CpuHob;
  UINT64                  Data64;
  UINT32                  Data32;
  VOID                    *HobPtr;
  EFI_BOOT_MODE           BootMode;
  CHAR8                   TempStr[32];
  UINT8                   *GuidHob;
  UNIVERSAL_PAYLOAD_BASE  *PayloadBase;

  Fdt = FdtBase;
  Fit = NULL;

  //
  // Create Hob list FDT node.
  //
  ParentNode = FdtAddSubnode (Fdt, 0, "options");
  ASSERT (ParentNode > 0);

  UPLParaNode = FdtAddSubnode (Fdt, ParentNode, "upl-params");
  ASSERT (UPLParaNode > 0);

  //
  // Create CPU info FDT node
  //
  CpuHob = GetFirstHob (EFI_HOB_TYPE_CPU);
  ASSERT (CpuHob != NULL);

  if (mResourceAssigned) {
    Status = FdtSetProperty (Fdt, UPLParaNode, "pci-enum-done", &mResourceAssigned, sizeof (mResourceAssigned));
    ASSERT_EFI_ERROR (Status);
  }

  BootMode = GetBootMode ();

  Data32 = CpuToFdt32 ((UINT32)CpuHob->SizeOfMemorySpace);
  Status = FdtSetProperty (Fdt, UPLParaNode, "addr-width", &Data32, sizeof (Data32));
  ASSERT_EFI_ERROR (Status);

  if (BootMode == BOOT_WITH_FULL_CONFIGURATION) {
    Status = FdtSetProperty (Fdt, UPLParaNode, "boot-mode", "normal", (UINT32)(AsciiStrLen ("normal")+1));
  } else if (BootMode == BOOT_WITH_MINIMAL_CONFIGURATION) {
    Status = FdtSetProperty (Fdt, UPLParaNode, "boot-mode", "fast", (UINT32)(AsciiStrLen ("fast")+1));
  } else if (BootMode == BOOT_WITH_FULL_CONFIGURATION_PLUS_DIAGNOSTICS) {
    Status = FdtSetProperty (Fdt, UPLParaNode, "boot-mode", "full", (UINT32)(AsciiStrLen ("full")+1));
  } else if (BootMode == BOOT_WITH_DEFAULT_SETTINGS) {
    Status = FdtSetProperty (Fdt, UPLParaNode, "boot-mode", "default", (UINT32)(AsciiStrLen ("default")+1));
  } else if (BootMode == BOOT_ON_S4_RESUME) {
    Status = FdtSetProperty (Fdt, UPLParaNode, "boot-mode", "s4", (UINT32)(AsciiStrLen ("s4")+1));
  } else if (BootMode == BOOT_ON_S3_RESUME) {
    Status = FdtSetProperty (Fdt, UPLParaNode, "boot-mode", "s3", (UINT32)(AsciiStrLen ("s3")+1));
  } else {
    Status = FdtSetProperty (Fdt, UPLParaNode, "boot-mode", "na", (UINT32)(AsciiStrLen ("na")+1));
  }

  ASSERT_EFI_ERROR (Status);

  Status = FdtSetProperty (Fdt, UPLParaNode, "compatible", "upl", (UINT32)(AsciiStrLen ("upl")+1));
  ASSERT_EFI_ERROR (Status);

  GuidHob = GetFirstGuidHob (&gUniversalPayloadBaseGuid);
  if (GuidHob != NULL) {
    PayloadBase = (UNIVERSAL_PAYLOAD_BASE *)GET_GUID_HOB_DATA (GuidHob);
    Fit         = (VOID *)(UINTN)PayloadBase->Entry;
    DEBUG ((DEBUG_INFO, "PayloadBase Entry = 0x%08x\n", PayloadBase->Entry));

    AsciiSPrint (TempStr, sizeof (TempStr), "upl-images@%lX", (UINTN)(Fit));
    UPLImageNode = FdtAddSubnode (Fdt, ParentNode, TempStr);

    Data64 = CpuToFdt64 ((UINTN)Fit);
    Status = FdtSetProperty (FdtBase, UPLImageNode, "addr", &Data64, sizeof (Data64));
    ASSERT_EFI_ERROR (Status);
  }

  CustomNode = FdtAddSubnode (Fdt, ParentNode, "upl-custom");
  ASSERT (CustomNode > 0);

  HobPtr = GetHobList ();
  Data64 = CpuToFdt64 ((UINT64)(EFI_PHYSICAL_ADDRESS)HobPtr);
  Status = FdtSetProperty (Fdt, CustomNode, "hoblistptr", &Data64, sizeof (Data64));
  ASSERT_EFI_ERROR (Status);

  return Status;
}

/**
  It will build FDT nodes

  @param[in] FdtBase         Address of the Fdt data.

  @retval EFI_SUCCESS        If it completed successfully.
  @retval Others             If it failed to build required FDT.
**/
EFI_STATUS
BuildFdt (
  IN     VOID  *FdtBase
  )
{
  EFI_STATUS  Status;

  //
  // Build FDT for memory related
  //
  Status = BuildFdtForMemory (FdtBase);
  ASSERT_EFI_ERROR (Status);

  Status = BuildFdtForMemAlloc (FdtBase);
  ASSERT_EFI_ERROR (Status);

  Status = BuildFdtForSerial (FdtBase);
  ASSERT_EFI_ERROR (Status);

  Status = BuildFdtForPciRootBridge (FdtBase);
  ASSERT_EFI_ERROR (Status);

  Status = BuildFdtForFrameBuffer (FdtBase);
  ASSERT_EFI_ERROR (Status);

  Status = BuildFdtForUplRequired (FdtBase);
  ASSERT_EFI_ERROR (Status);

  return Status;
}


/**
  Print FDT data.

  @param[in] Fdt         Address of the Fdt data.
**/
VOID
PrintFdt (
  IN     VOID           *Fdt
  )
{
  INT32                 Node;
  INT32                 PreNode;
  INT32                 Depth;
  FDT_NODE_HEADER       *NodePtr;

  Depth = 0;

  for (Node = FdtNextNode (Fdt, 0, &Depth); Node >= 0; Node = FdtNextNode (Fdt, Node, &Depth)) {
    NodePtr = (FDT_NODE_HEADER *)((CONST CHAR8 *)Fdt + Node + Fdt32ToCpu (((FDT_HEADER *)Fdt)->OffsetDtStruct));
    PreNode = Node;
    DEBUG ((DEBUG_INFO, "Node(%08x)  %a\n", Node, NodePtr->Name));
  }
  DumpHex (2, 0, PreNode + 0x200, Fdt);
}


/**
  Build FDT node table for universal payload.

  @retval         Start address of FDT node table. NULL if there is failure to build FDT node.
**/
VOID *
EFIAPI
BuildFdtForUpl (
  VOID
  )
{
  EFI_STATUS                     Status;
  UNIVERSAL_PAYLOAD_DEVICE_TREE  *Fdt;
  UINT32                         FdtSize;
  VOID                           *FdtBase;
  UINT32                         Data32;

  FdtSize  = SIZE_32KB;
  FdtBase  = AllocatePages (EFI_SIZE_TO_PAGES (FdtSize));
  if (FdtBase == NULL) {
    DEBUG ((DEBUG_INFO, "FDT AllocatePages failed\n"));
    return NULL;
  }

  Status = FdtCreateEmptyTree (FdtBase, FdtSize);
  ASSERT_EFI_ERROR (Status);

  // Set cell property of root node
  Data32 = CpuToFdt32 (2);
  Status = FdtSetProperty (FdtBase, 0, "#address-cells", &Data32, sizeof (UINT32));
  ASSERT_EFI_ERROR (Status);
  Status = FdtSetProperty (FdtBase, 0, "#size-cells", &Data32, sizeof (UINT32));
  ASSERT_EFI_ERROR (Status);

  Status = BuildFdt (FdtBase);
  ASSERT_EFI_ERROR (Status);

  Fdt = BuildGuidHob (&gUniversalPayloadDeviceTreeGuid, sizeof (UNIVERSAL_PAYLOAD_DEVICE_TREE));
  if (Fdt == NULL) {
    DEBUG ((DEBUG_INFO, "ERROR:Build FDT Hob failed\n"));
    FreePages (FdtBase, EFI_SIZE_TO_PAGES (FdtSize));
    return NULL;
  }

  DEBUG ((DEBUG_INFO, "Build FDT at 0x%x (size %d)\n", FdtBase, Fdt32ToCpu (((FDT_HEADER *)FdtBase)->TotalSize)));
  Fdt->Header.Revision   = UNIVERSAL_PAYLOAD_DEVICE_TREE_REVISION;
  Fdt->Header.Length     = sizeof (UNIVERSAL_PAYLOAD_DEVICE_TREE);
  Fdt->DeviceTreeAddress = (UINT64)FdtBase;

  return FdtBase;
}

