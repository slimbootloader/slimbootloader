/** @file

  Copyright (c) 2017 - 2025, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <PiPei.h>
#include <Library/PcdLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PciExpressLib.h>
#include <Library/SortLib.h>
#include <Library/HobLib.h>
#include <InternalPciEnumerationLib.h>
#include <Library/PciEnumerationLib.h>
#include <Library/BootloaderCommonLib.h>
#include <UniversalPayload/PciRootBridges.h>
#include "PciAri.h"
#include "PciIov.h"
#include "InternalPciEnumerationLib.h"

#define  DEBUG_PCI_ENUM    0

// PCI attribute definitions for Universal Payload HOB
#define EFI_PCI_ATTRIBUTE_ISA_MOTHERBOARD_IO          0x0001
#define EFI_PCI_ATTRIBUTE_ISA_IO                      0x0002
#define EFI_PCI_ATTRIBUTE_VGA_PALETTE_IO              0x0004
#define EFI_PCI_ATTRIBUTE_VGA_MEMORY                  0x0008
#define EFI_PCI_ATTRIBUTE_VGA_IO                      0x0010
#define EFI_PCI_ATTRIBUTE_IDE_PRIMARY_IO              0x0020
#define EFI_PCI_ATTRIBUTE_IDE_SECONDARY_IO            0x0040
#define EFI_PCI_ATTRIBUTE_ISA_IO_16                   0x10000
#define EFI_PCI_ATTRIBUTE_VGA_PALETTE_IO_16           0x20000
#define EFI_PCI_ATTRIBUTE_VGA_IO_16                   0x40000

#define EFI_PCI_HOST_BRIDGE_COMBINE_MEM_PMEM          1
#define EFI_PCI_HOST_BRIDGE_MEM64_DECODE              2

#define PNP_EISA_ID_CONST                             0x41d0
#define EISA_ID(_Name, _Num)      ((UINT32)((_Name) | (_Num) << 16))
#define EISA_PNP_ID(_PNPId)       (EISA_ID(PNP_EISA_ID_CONST, (_PNPId)))

UINT8   *mPoolPtr;

STATIC PCI_RES_ALLOC_TABLE  *mResAllocTablePtr;

//
// Default PCI Resource Allocation Range
//
STATIC PCI_RES_ALLOC_RANGE mDefaultResRange = {
  .BusBase      = 0,
  .BusLimit     = 0xFF,
  .Reserved     = 0,
  .IoBase       = 0x1000,
  .IoLimit      = 0xFFFF,
  .Mmio32Base   = 0x80000000,
  .Mmio32Limit  = 0xFFFFFFFF,
  .Mmio64Base   = 0x400000000ULL,
  .Mmio64Limit  = 0xFFFFFFFFFULL
};

/**
 Set the memory pool address to the global pointer -file scope.

 @param Ptr pointer to set the global pointer.
 **/
VOID
SetAllocationPool (
  VOID *Ptr
  )
{
  mPoolPtr = Ptr;
}

/**
  Return the memory pool global pointer -file scope.
 **/
VOID *
GetAllocationPool (
  VOID
  )
{
  return (VOID *)mPoolPtr;
}

/**
  Allocate the memory of specified size from the memory pool.

  @param AllocationSize size to be allocated.

 **/
VOID *
PciAllocatePool (
  IN UINTN            AllocationSize
  )
{
  UINT8  *Ptr;

  Ptr = mPoolPtr;
  mPoolPtr += ((AllocationSize + 0x03) & 0xFFFFFFFC);
  return Ptr;
}

/**
  This routine is used to check whether the pci device is present.

  @param Pci               Output buffer for PCI device configuration space.
  @param Bus               PCI bus NO.
  @param Device            PCI device NO.
  @param Func              PCI Func NO.

  @retval EFI_NOT_FOUND    PCI device not present.
  @retval EFI_SUCCESS      PCI device is found.

**/
EFI_STATUS
PciDevicePresent (
  OUT PCI_TYPE00                          *Pci,
  IN  UINT8                               Bus,
  IN  UINT8                               Device,
  IN  UINT8                               Func
  )
{
  UINT32      Address;
  UINT32      Loop;
  UINT32      *Src;
  UINT32      *Dst;

  //
  // Create PCI address map in terms of Bus, Device and Func
  //
  Address = PCI_EXPRESS_LIB_ADDRESS (Bus, Device, Func, 0);

  //
  // Read the Vendor ID register
  //
  if (PciExpressRead16 (Address) == 0xFFFF) {
    return EFI_NOT_FOUND;
  } else {
    //
    // We perform a memory copy here manually since using the
    // BaseMemoryLibSse2 CopyMem routine causes issue related
    // to DQWORD copying in PCI config space.
    //
    Src = (UINT32 *) ((UINTN)PcdGet64 (PcdPciExpressBaseAddress) + PCI_EXPRESS_LIB_ADDRESS (Bus, Device, Func, 0));
    Dst = (UINT32 *) Pci;
    for (Loop = 0; Loop < sizeof (PCI_TYPE00); Loop += sizeof (UINT32)) {
      *Dst++ = *Src++;
    }
    return EFI_SUCCESS;
  }
}

/**
  Parse PCI bar information and fill them into PCI device instance.

  @param PciIoDevice  Pci device instance.
  @param Offset       Bar offset.
  @param BarIndex     Bar index.

  @return Next bar offset.

**/
UINTN
PciParseBar (
  IN PCI_IO_DEVICE  *PciIoDevice,
  IN UINTN          Offset,
  IN UINTN          BarIndex
  )
{
  UINT32                Value;
  UINT32                OriginalValue;
  UINT32                Mask;
  EFI_STATUS            Status;
  PCI_BAR_TYPE          BarType;

  BarType       = PciBarTypeUnknown;
  OriginalValue = 0;
  Value         = 0;

  Status = BarExisted (
             PciIoDevice,
             Offset,
             &Value,
             &OriginalValue
             );

  if (EFI_ERROR (Status)) {
    PciIoDevice->PciBar[BarIndex].BaseAddress = 0;
    PciIoDevice->PciBar[BarIndex].Length      = 0;
    PciIoDevice->PciBar[BarIndex].Alignment   = 0;

    //
    // Some devices don't fully comply to PCI spec 2.2. So be to scan all the BARs anyway
    //
    PciIoDevice->PciBar[BarIndex].Offset = (UINT8) Offset;
    return Offset + 4;
  }

  PciIoDevice->PciBar[BarIndex].Offset = (UINT8) Offset;
  if ((Value & 0x01) != 0) {
    //
    // Device I/Os
    //
    Mask = 0xfffffffc;

    if ((Value & 0xFFFF0000) != 0) {
      //
      // It is a IO32 bar
      //
      if ((PciIoDevice->Decodes & EFI_BRIDGE_IO32_DECODE_SUPPORTED) != 0) {
        BarType = PciBarTypeIo32;
      } else {
        BarType = PciBarTypeIo16;
      }
      PciIoDevice->PciBar[BarIndex].Length    = ((~ (Value & Mask)) + 1);
    } else {
      //
      // It is a IO16 bar
      //
      BarType = PciBarTypeIo16;
      PciIoDevice->PciBar[BarIndex].Length    = 0x0000FFFF & ((~ (Value & Mask)) + 1);
    }
    PciIoDevice->PciBar[BarIndex].BarType   = BarType;
    PciIoDevice->PciBar[BarIndex].Alignment = PciIoDevice->PciBar[BarIndex].Length - 1;

    //
    // Workaround. Some platforms inplement IO bar with 0 length
    // Need to treat it as no-bar
    //
    if (PciIoDevice->PciBar[BarIndex].Length == 0) {
      PciIoDevice->PciBar[BarIndex].BarType = (PCI_BAR_TYPE) 0;
    }

    PciIoDevice->PciBar[BarIndex].BaseAddress   = OriginalValue & Mask;

  } else {

    if (Offset == PCI_EXPANSION_ROM_BASE) {
      Mask  = 0xfffff800;
    } else {
      Mask  = 0xfffffff0;
    }

    PciIoDevice->PciBar[BarIndex].BaseAddress = OriginalValue & Mask;

    switch (Value & 0x07) {

    //
    //memory space; anywhere in 32 bit address space
    //
    case 0x00:
      if ((Value & 0x08) != 0) {
        PciIoDevice->PciBar[BarIndex].BarType = PciBarTypePMem32;
      } else {
        PciIoDevice->PciBar[BarIndex].BarType = PciBarTypeMem32;
      }
      BarType = PciIoDevice->PciBar[BarIndex].BarType;
      PciIoDevice->PciBar[BarIndex].Length    = (~ (Value & Mask)) + 1;
      if (PciIoDevice->PciBar[BarIndex].Length < (SIZE_4KB)) {
        //
        // Force minimum 4KByte alignment for Virtualization technology for Directed I/O
        //
        PciIoDevice->PciBar[BarIndex].Alignment = (SIZE_4KB - 1);
      } else {
        PciIoDevice->PciBar[BarIndex].Alignment = PciIoDevice->PciBar[BarIndex].Length - 1;
      }
      break;

    //
    // memory space; anywhere in 64 bit address space
    //
    case 0x04:
      if ((Value & 0x08) != 0) {
        if ((PciIoDevice->Decodes & EFI_BRIDGE_PMEM64_DECODE_SUPPORTED) != 0) {
          BarType = PciBarTypePMem64;
        } else {
          BarType = PciBarTypePMem32;
        }
      } else {
        if ((PciIoDevice->Decodes & EFI_BRIDGE_MEM64_DECODE_SUPPORTED) != 0) {
          BarType = PciBarTypeMem64;
        } else {
          BarType = PciBarTypeMem32;
        }
      }
      PciIoDevice->PciBar[BarIndex].BarType = BarType;

      //
      // According to PCI 2.2,if the bar indicates a memory 64 decoding, next bar
      // is regarded as an extension for the first bar. As a result
      // the sizing will be conducted on combined 64 bit value
      // Here just store the masked first 32bit value for future size
      // calculation
      //
      PciIoDevice->PciBar[BarIndex].Length    = Value & Mask;
      PciIoDevice->PciBar[BarIndex].Alignment = PciIoDevice->PciBar[BarIndex].Length - 1;

      //
      // Increment the offset to point to next DWORD
      //
      Offset += 4;

      Status = BarExisted (
                 PciIoDevice,
                 Offset,
                 &Value,
                 &OriginalValue
                 );

      if (EFI_ERROR (Status)) {
        //
        // the high 32 bit does not claim any BAR, we need to re-check the low 32 bit BAR again
        //
        if (PciIoDevice->PciBar[BarIndex].Length == 0) {
          //
          // some device implement MMIO bar with 0 length, need to treat it as no-bar
          //
          PciIoDevice->PciBar[BarIndex].BarType = PciBarTypeUnknown;
          return Offset + 4;
        }
      }

      //
      // Fix the length to support some spefic 64 bit BAR
      //
      if (Value == 0) {
        Value = (UINT32) - 1;
      } else {
        Value |= ((UINT32) (-1) << HighBitSet32 (Value));
      }

      //
      // Calculate the size of 64bit bar
      //
      PciIoDevice->PciBar[BarIndex].BaseAddress |= LShiftU64 ((UINT64) OriginalValue, 32);

      PciIoDevice->PciBar[BarIndex].Length    = PciIoDevice->PciBar[BarIndex].Length | LShiftU64 ((UINT64) Value, 32);
      PciIoDevice->PciBar[BarIndex].Length    = (~ (PciIoDevice->PciBar[BarIndex].Length)) + 1;
      if (PciIoDevice->PciBar[BarIndex].Length < (SIZE_4KB)) {
        //
        // Force minimum 4KByte alignment for Virtualization technology for Directed I/O
        //
        PciIoDevice->PciBar[BarIndex].Alignment = (SIZE_4KB - 1);
      } else {
        PciIoDevice->PciBar[BarIndex].Alignment = PciIoDevice->PciBar[BarIndex].Length - 1;
      }

      break;

    //
    // reserved
    //
    default:
      PciIoDevice->PciBar[BarIndex].BarType   = PciBarTypeUnknown;
      PciIoDevice->PciBar[BarIndex].Length    = (~ (Value & Mask)) + 1;
      if (PciIoDevice->PciBar[BarIndex].Length < (SIZE_4KB)) {
        //
        // Force minimum 4KByte alignment for Virtualization technology for Directed I/O
        //
        PciIoDevice->PciBar[BarIndex].Alignment = (SIZE_4KB - 1);
      } else {
        PciIoDevice->PciBar[BarIndex].Alignment = PciIoDevice->PciBar[BarIndex].Length - 1;
      }
      break;
    }
  }

#if DEBUG_PCI_ENUM
  DEBUG((DEBUG_INFO, "    Bar: %d BarType: %d Size: 0x%llx\n",
    BarIndex,
    PciIoDevice->PciBar[BarIndex].BarType,
    PciIoDevice->PciBar[BarIndex].Length));
#endif

  //
  // Check the length again so as to keep compatible with some special bars
  //
  if (PciIoDevice->PciBar[BarIndex].Length == 0) {
    PciIoDevice->PciBar[BarIndex].BarType     = PciBarTypeUnknown;
    PciIoDevice->PciBar[BarIndex].BaseAddress = 0;
    PciIoDevice->PciBar[BarIndex].Alignment   = 0;
  } else {
    PciIoDevice->PciBar[BarIndex].OrgBarType = BarType;
  }

  //
  // Increment number of bar
  //
  return Offset + 4;
}

/**
  This routine is used to initialize the bar of a PCI-PCI Bridge device.

  @param  PciIoDevice PCI-PCI bridge device instance.

**/
VOID
InitializePpb (
  IN PCI_IO_DEVICE    *PciIoDevice
  )
{
  //
  // Put all the resource apertures including IO16
  // Io32, pMem32, pMem64 to quiescent state
  // Resource base all ones, Resource limit all zeros
  //
  PciExpressWrite16 (PciIoDevice->Address + 0x1C, 0x00FF);
  PciExpressWrite32 (PciIoDevice->Address + 0x20, 0x0000FFFF);
  PciExpressWrite32 (PciIoDevice->Address + 0x24, 0x0000FFFF);
  PciExpressWrite32 (PciIoDevice->Address + 0x28, 0xFFFFFFFF);
  PciExpressWrite32 (PciIoDevice->Address + 0x2C, 0x00000000);

  //
  // Don't support use io32 as for now
  //
  PciExpressWrite32 (PciIoDevice->Address + 0x30, 0x0000FFFF);

  //
  // Force Interrupt line to zero for cards that come up randomly
  //
  PciExpressWrite8 (PciIoDevice->Address + 0x3C, 0x00);
}

/**
  Create and initiliaze general PCI I/O device instance for
  PCI device/bridge device/hotplug bridge device.

  @param PciRootBridgeIo   Pointer to instance of EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL.
  @param Pci               Input Pci information block.
  @param Bus               Device Bus NO.
  @param Device            Device device NO.
  @param Func              Device func NO.

  @return Instance of PCI device. NULL means no instance created.

**/
PCI_IO_DEVICE *
CreatePciIoDevice (
  IN PCI_IO_DEVICE                    *Bridge,
  IN PCI_TYPE00                       *Pci,
  IN UINT8                            Bus,
  IN UINT8                            Device,
  IN UINT8                            Func
  )
{
  PCI_IO_DEVICE        *PciIoDevice;

  PciIoDevice = (PCI_IO_DEVICE *)PciAllocatePool (sizeof (PCI_IO_DEVICE));
  SetMem (PciIoDevice, sizeof (PCI_IO_DEVICE), 0);
  PciIoDevice->Parent  = Bridge;
  PciIoDevice->Address = PCI_EXPRESS_LIB_ADDRESS (Bus, Device, Func, 0);
  InitializeListHead (&PciIoDevice->ChildList);
  if (Pci != NULL) {
    CopyMem (& (PciIoDevice->Pci), Pci, sizeof (PCI_TYPE00));
  }

  PciIoDevice->Decodes                    = 0;
  PciIoDevice->IsPciExp                   = FALSE;
  PciIoDevice->PciExpressCapabilityOffset = 0;
  if (FeaturePcdGet (PcdAriSupport) || FeaturePcdGet (PcdSrIovSupport) || FeaturePcdGet (PcdResizableBarSupport)) {
    InitializePciExpCapability (PciIoDevice);
  }
  if (FeaturePcdGet (PcdAriSupport)) {
    InitializeAri (PciIoDevice);
  }
  if (FeaturePcdGet (PcdSrIovSupport)) {
    InitializeSrIov (PciIoDevice, Bus, Device, Func);
  }
  if (FeaturePcdGet (PcdResizableBarSupport)) {
    InitializeResizeBar (PciIoDevice);
  }
  return PciIoDevice;
}

/**
  Create PCI device instance for PCI device.

  @param Bridge   Parent bridge instance.
  @param Pci      Input PCI device information block.
  @param Bus      PCI device Bus NO.
  @param Device   PCI device Device NO.
  @param Func     PCI device's func NO.

  @return  Created PCI device instance.

**/
PCI_IO_DEVICE *
GatherDeviceInfo (
  IN PCI_IO_DEVICE                    *Bridge,
  IN PCI_TYPE00                       *Pci,
  IN UINT8                            Bus,
  IN UINT8                            Device,
  IN UINT8                            Func
  )
{
  UINTN                           Offset;
  UINTN                           BarIndex;
  PCI_IO_DEVICE                   *PciIoDevice;
  UINT16                          Value;
  BOOLEAN                         Downgrade;
  CONST PCI_ENUM_POLICY_INFO     *EnumPolicy;

  PciIoDevice = CreatePciIoDevice (
                  Bridge,
                  Pci,
                  Bus,
                  Device,
                  Func
                  );

  //
  // If it is a full enumeration, disconnect the device in advance
  //
  Value = PciExpressRead16 (PCI_EXPRESS_LIB_ADDRESS (Bus, Device, Func, PCI_COMMAND_OFFSET));
  PciExpressWrite16 (PCI_EXPRESS_LIB_ADDRESS (Bus, Device, Func, PCI_COMMAND_OFFSET), 0);

  //
  // Inherit parent decode capability
  //
  EnumPolicy = (PCI_ENUM_POLICY_INFO *)PcdGetPtr (PcdPciEnumPolicyInfo);
  if (PciIoDevice->Parent != NULL) {
    PciIoDevice->Decodes = PciIoDevice->Parent->Decodes;
    Downgrade = FALSE;
    if (Bus == 0) {
      if (EnumPolicy->Downgrade.Bus0 == 1) {
        Downgrade = TRUE;
      } else if (EnumPolicy->Downgrade.Bus0 == 2) {
        if (!(IS_PCI_DISPLAY(&PciIoDevice->Pci))) {
          Downgrade = TRUE;
        }
      }
      if (Downgrade) {
        PciIoDevice->Decodes &= (UINT32)~(EFI_BRIDGE_IO32_DECODE_SUPPORTED);
        PciIoDevice->Decodes &= (UINT32)~(EFI_BRIDGE_MEM64_DECODE_SUPPORTED);
        PciIoDevice->Decodes &= (UINT32)~(EFI_BRIDGE_PMEM64_DECODE_SUPPORTED);
      }
    }
  }

#if DEBUG_PCI_ENUM
  DEBUG((DEBUG_INFO, "    VID|DID: %4X|%4X\n",
    PciExpressRead16(PciIoDevice->Address), PciExpressRead16(PciIoDevice->Address + 2)));
#endif

  //
  // Start to parse the bars
  //
  for (Offset = 0x10, BarIndex = 0; Offset <= 0x24 && BarIndex < PCI_MAX_BAR; BarIndex++) {
    Offset = PciParseBar (PciIoDevice, Offset, BarIndex);
  }

  //
  // Find a available PCI bar slot for PCI expansion ROM
  //
  if (EnumPolicy->Flag.FlagAllocRomBar == 1) {
    BarIndex = PCI_MAX_BAR - 1;
    while (BarIndex > 0) {
      if (PciIoDevice->PciBar[BarIndex].BarType == PciBarTypeUnknown) {
        PciParseBar (PciIoDevice, PCI_EXPANSION_ROM_BASE, BarIndex);
        break;
      }
      BarIndex--;
    }
  }

  //
  // Parse the SR-IOV VF bars
  //
  if (PcdGetBool (PcdSrIovSupport) && PciIoDevice->SrIovCapabilityOffset != 0) {
    for (Offset = PciIoDevice->SrIovCapabilityOffset + EFI_PCIE_CAPABILITY_ID_SRIOV_BAR0, BarIndex = 0;
         Offset <= PciIoDevice->SrIovCapabilityOffset + EFI_PCIE_CAPABILITY_ID_SRIOV_BAR5 && BarIndex < PCI_MAX_BAR;
         BarIndex++) {
      Offset = PciIovParseVfBar (PciIoDevice, Offset, BarIndex);
    }
  }

  PciExpressWrite16 (PCI_EXPRESS_LIB_ADDRESS (Bus, Device, Func, PCI_COMMAND_OFFSET), Value);

  return PciIoDevice;
}

/**
  Create PCI device instance for PCI-PCI bridge.

  @param Bridge   Parent bridge instance.
  @param Pci      Input PCI device information block.
  @param Bus      PCI device Bus NO.
  @param Device   PCI device Device NO.
  @param Func     PCI device's func NO.

  @return  Created PCI device instance.

**/
PCI_IO_DEVICE *
GatherPpbInfo (
  IN PCI_IO_DEVICE                    *Bridge,
  IN PCI_TYPE00                       *Pci,
  IN UINT8                            Bus,
  IN UINT8                            Device,
  IN UINT8                            Func
  )
{
  PCI_IO_DEVICE                   *PciIoDevice;
  EFI_STATUS                      Status;
  UINT8                           Value;
  UINT8                           Temp;
  UINT32                          PMemBaseLimit;
  UINT16                          PrefetchableMemoryBase;
  UINT16                          PrefetchableMemoryLimit;

  PciIoDevice = CreatePciIoDevice (
                  Bridge,
                  Pci,
                  Bus,
                  Device,
                  Func
                  );

  //
  // Clear enable bits in command and control registers
  //
  PciExpressAnd16 (PciIoDevice->Address + PCI_COMMAND_OFFSET, (UINT16)~EFI_PCI_COMMAND_BITS_OWNED);
  PciExpressAnd16 (PciIoDevice->Address + PCI_BRIDGE_CONTROL_REGISTER_OFFSET, (UINT16)~EFI_PCI_BRIDGE_CONTROL_BITS_OWNED);

  //
  // Test whether it support 32 decode or not
  //
  Temp = PciExpressRead8 (PciIoDevice->Address + 0x1C);
  PciExpressWrite8 (PciIoDevice->Address + 0x1C, 0xFF);
  Value = PciExpressRead8 (PciIoDevice->Address + 0x1C);
  PciExpressWrite8 (PciIoDevice->Address + 0x1C, Temp);

  if (Value != 0) {
    if ((Value & 0x01) != 0) {
      PciIoDevice->Decodes |= EFI_BRIDGE_IO32_DECODE_SUPPORTED;
    } else {
      PciIoDevice->Decodes |= EFI_BRIDGE_IO16_DECODE_SUPPORTED;
    }
  }

  Status = BarExisted (
            PciIoDevice,
            0x24,
            NULL,
            &PMemBaseLimit
            );

  //
  // Test if it supports 64 memory or not
  //
  // The bottom 4 bits of both the Prefetchable Memory Base and Prefetchable Memory Limit
  // registers:
  //   0 - the bridge supports only 32 bit addresses.
  //   1 - the bridge supports 64-bit addresses.
  //
  PrefetchableMemoryBase = (UINT16)(PMemBaseLimit & 0xffff);
  PrefetchableMemoryLimit = (UINT16)(PMemBaseLimit >> 16);
  if (!EFI_ERROR (Status) &&
      (PrefetchableMemoryBase & 0x000f) == 0x0001 &&
      (PrefetchableMemoryLimit & 0x000f) == 0x0001) {
    Status = BarExisted (
              PciIoDevice,
              0x28,
              NULL,
              NULL
              );

    if (!EFI_ERROR (Status)) {
      PciIoDevice->Decodes |= EFI_BRIDGE_PMEM32_DECODE_SUPPORTED;
      PciIoDevice->Decodes |= EFI_BRIDGE_PMEM64_DECODE_SUPPORTED;
    } else {
      PciIoDevice->Decodes |= EFI_BRIDGE_PMEM32_DECODE_SUPPORTED;
    }
  }

  //
  // Memory 32 code is required for ppb
  //
  PciIoDevice->Decodes |= EFI_BRIDGE_MEM32_DECODE_SUPPORTED;

  //
  // Make sure that PPB is in scope of parent's decode capability
  //
  if (PciIoDevice->Parent != NULL) {
    PciIoDevice->Decodes &= PciIoDevice->Parent->Decodes;
  }

  //
  // PPB can have two BARs
  //
  if (PciParseBar (PciIoDevice, 0x10, PPB_BAR_0) == 0x14) {
    //
    // Not 64-bit bar
    //
    PciParseBar (PciIoDevice, 0x14, PPB_BAR_1);
  }

  //
  // Copy PciBar to PpbBar to program BAR0/1 for PPBs. PciBar field is used to
  // scope PPB Apperture resources later.
  //
  CopyMem (&(PciIoDevice->PpbBar[0]), &(PciIoDevice->PciBar[0]), sizeof(PCI_BAR) * PPB_MAX_BAR);

  return PciIoDevice;
}

/**
  This function is used to insert a PCI device node under
  a bridge.

  @param Bridge         The PCI bridge.
  @param PciDeviceNode  The PCI device needs inserting.

**/
VOID
InsertPciDevice (
  IN PCI_IO_DEVICE      *Bridge,
  IN PCI_IO_DEVICE      *PciDeviceNode
  )
{
  InsertTailList (&Bridge->ChildList, & (PciDeviceNode->Link));
  PciDeviceNode->Parent = Bridge;
}

/**
  Seach required device and create PCI device instance.

  @param Bridge     Parent bridge instance.
  @param Pci        Input PCI device information block.
  @param Bus        PCI bus NO.
  @param Device     PCI device NO.
  @param Func       PCI func  NO.
  @param PciDevice  Output of searched PCI device instance.

  @retval EFI_SUCCESS           Successfully created PCI device instance.
  @retval EFI_OUT_OF_RESOURCES  Cannot get PCI device information.

**/
EFI_STATUS
PciSearchDevice (
  IN  PCI_IO_DEVICE                         *Bridge,
  IN  PCI_TYPE00                            *Pci,
  IN  UINT8                                 Bus,
  IN  UINT8                                 Device,
  IN  UINT8                                 Func,
  OUT PCI_IO_DEVICE                         **PciDevice
  )
{
  PCI_IO_DEVICE                     *PciIoDevice;
  PLATFORM_PCI_ENUM_HOOK_PROC       PlatformPciEnumHookProc;

  PciIoDevice = NULL;

  //
  // Nofify EfiPciBeforeResourceCollection
  //
  PlatformPciEnumHookProc = (PLATFORM_PCI_ENUM_HOOK_PROC)(UINTN)PcdGet32 (PcdPciEnumHookProc);
  if (PlatformPciEnumHookProc != NULL) {
    PlatformPciEnumHookProc (Bus, Device, Func, EfiPciBeforeResourceCollection);
  }
#if DEBUG_PCI_ENUM
  DEBUG ((
           DEBUG_INFO,
           "PciBus: Discovered %s @ [%02x|%02x|%02x]\n",
           IS_PCI_BRIDGE (Pci) ?     L"PPB" :
           IS_CARDBUS_BRIDGE (Pci) ? L"P2C" :
           L"PCI",
           Bus, Device, Func
           ));
#endif

  if (!IS_PCI_BRIDGE (Pci)) {
    //
    // Create private data for Pci Device
    //
    PciIoDevice = GatherDeviceInfo (
                    Bridge,
                    Pci,
                    Bus,
                    Device,
                    Func
                    );

  } else {
    //
    // Create private data for PPB
    //
    PciIoDevice = GatherPpbInfo (
                    Bridge,
                    Pci,
                    Bus,
                    Device,
                    Func
                    );

    //
    // Special initialization for PPB including making the PPB quiet
    //
    InitializePpb (PciIoDevice);
  }

  //
  // Insert it into a global tree for future reference
  //
  InsertPciDevice (Bridge, PciIoDevice);

  //
  // Determine PCI device attributes
  //

  if (PciDevice != NULL) {
    *PciDevice = PciIoDevice;
  }

  return EFI_SUCCESS;
}

/**
  Retrieve the max bus number that is assigned to the Root Bridge hierarchy.
  It can support the case that there are multiple bus ranges.

  @param  Bridge           Bridge device instance.

  @retval                  The max bus number that is assigned to this Root Bridge hierarchy.

**/
UINT8
PciGetMaxBusNumber (
  IN PCI_IO_DEVICE                      *Bridge
  )
{
  UINT8             MaxBusNumber;
  PCI_IO_DEVICE    *CurrBridge;

  //
  // No Bridge type check here. A caller must guarantee IS_PCI_BRIDGE(Bridge)
  //
  if (Bridge != NULL) {
    // Find the root bridge to get the bus limit
    CurrBridge = Bridge;
    while (CurrBridge->Parent != NULL) {
      CurrBridge = CurrBridge->Parent;
    }
    MaxBusNumber = CurrBridge->BusNumberRanges.BusLimit;
  } else {
    MaxBusNumber = PCI_MAX_BUS;
  }
  return MaxBusNumber;
}

/**
  Scan pci bus and assign bus number to the given PCI bus system.

  @param  Bridge           Bridge device instance.
  @param  StartBusNumber   start point.
  @param  SubBusNumber     Point to sub bus number.
  @param  PaddedBusRange   Customized bus number.

  @retval EFI_SUCCESS      Successfully scanned and assigned bus number.
  @retval other            Some error occurred when scanning pci bus.

  @note   Feature flag PcdPciBusHotplugDeviceSupport determine whether need support hotplug.

**/
EFI_STATUS
PciScanBus (
  IN PCI_IO_DEVICE                      *Bridge,
  IN UINT8                              StartBusNumber,
  OUT UINT8                             *SubBusNumber,
  OUT UINT8                             *PaddedBusRange
  )
{
  EFI_STATUS                        Status;
  PCI_TYPE00                        Pci;
  UINT8                             Device;
  UINT8                             Func;
  UINT32                            Address;
  UINT8                             SecondBus;
  UINT16                            Register;
  UINT16                            TempReservedBusNum;
  PCI_IO_DEVICE                     *PciDevice;
  PLATFORM_PCI_ENUM_HOOK_PROC       PlatformPciEnumHookProc;

  SecondBus       = 0;
  Register        = 0;
  PciDevice       = NULL;

  for (Device = 0; Device <= PCI_MAX_DEVICE; Device++) {
    for (Func = 0, TempReservedBusNum = 0; Func <= PCI_MAX_FUNC; Func++) {

      //
      // Check to see whether a pci device is present
      //
      Status = PciDevicePresent (
                 &Pci,
                 StartBusNumber,
                 Device,
                 Func
                 );

      if (EFI_ERROR (Status) && Func == 0) {
        //
        // go to next device if there is no Function 0
        //
        break;
      }

      if (EFI_ERROR (Status)) {
        continue;
      }

      //
      // Get the PCI device information
      //
      Status = PciSearchDevice (
                 Bridge,
                 &Pci,
                 StartBusNumber,
                 Device,
                 Func,
                 &PciDevice
                 );

      ASSERT (!EFI_ERROR (Status));

      if (IS_PCI_BRIDGE (&Pci)) {
        *SubBusNumber += 1;
        SecondBus = *SubBusNumber;

        Register  = (UINT16) ((SecondBus << 8) | (UINT16) StartBusNumber);
        Address   = PCI_EXPRESS_LIB_ADDRESS (StartBusNumber, Device, Func, PCI_BRIDGE_PRIMARY_BUS_REGISTER_OFFSET);
        PciExpressWrite16 (Address, Register);

        //
        // Temporarily initialize SubBusNumber to maximum bus number to ensure the
        // PCI configuration transaction to go through any PPB
        //
        Register  = PciGetMaxBusNumber (Bridge);
        Address   = PCI_EXPRESS_LIB_ADDRESS (StartBusNumber, Device, Func, PCI_BRIDGE_SUBORDINATE_BUS_REGISTER_OFFSET);
        PciExpressWrite8 (Address, (UINT8)Register);

        //
        // Nofify EfiPciBeforeChildBusEnumeration
        //
        PlatformPciEnumHookProc = (PLATFORM_PCI_ENUM_HOOK_PROC)(UINTN)PcdGet32 (PcdPciEnumHookProc);
        if (PlatformPciEnumHookProc != NULL) {
          PlatformPciEnumHookProc (StartBusNumber, Device, Func, EfiPciBeforeChildBusEnumeration);
        }

        Status = PciScanBus (
                   PciDevice,
                   SecondBus,
                   SubBusNumber,
                   PaddedBusRange
                   );
        if (EFI_ERROR (Status)) {
          return Status;
        }

        //
        // Set the current maximum bus number under the PPB
        //
        Address = PCI_EXPRESS_LIB_ADDRESS (StartBusNumber, Device, Func, PCI_BRIDGE_SUBORDINATE_BUS_REGISTER_OFFSET);
        PciExpressWrite8 (Address, *SubBusNumber);
      } else {
        //
        // If PCI device has SR-IOV capability, check bus reservation for PCI IOV
        // Go through each function, just reserve the MAX ReservedBusNum for one device
        //
        if (FeaturePcdGet (PcdSrIovSupport) && PciDevice->SrIovCapabilityOffset != 0) {
          if (TempReservedBusNum < PciDevice->ReservedBusNum) {
            *SubBusNumber += (UINT8)(PciDevice->ReservedBusNum - TempReservedBusNum);
            TempReservedBusNum = PciDevice->ReservedBusNum;
          }
        }
      }

      if (Func == 0 && !IS_PCI_MULTI_FUNC (&Pci)) {

        //
        // Skip sub functions, this is not a multi function device
        //

        Func = PCI_MAX_FUNC;
      }
    }
  }

  return EFI_SUCCESS;
}

/**
  The function is called by PerformInsertionSortList to sort PCI Bar by its alignment.

  @param[in] Buffer1         The pointer to first list entry.
  @param[in] Buffer2         The pointer to second list entry.

  @retval 0                  Buffer2 alignment is less than or equal to Buffer1 alignment.
  @retval 1                  Buffer2 alignment is greater than Buffer1 alignment.

**/
INTN
EFIAPI
ComparePciBarRes (
  IN CONST VOID                 *Buffer1,
  IN CONST VOID                 *Buffer2
  )
{
  PCI_BAR_RESOURCE      *NewResBar;
  PCI_BAR_RESOURCE      *CurrResBar;

  NewResBar   = PCI_BAR_RESOURCE_FROM_LINK (Buffer1);
  CurrResBar  = PCI_BAR_RESOURCE_FROM_LINK (Buffer2);

  //
  // Descending Order
  //
  return (CurrResBar->PciBar->Alignment > NewResBar->PciBar->Alignment) ? 1 : 0;
}

/**
 Program the Base address register with the base address of PCI devices.

 @param[in] PciIoDevice  pointer to PCI IO device.
 @param[in] BarIdx       BAR index to program the address.
 @param[in] Virtual      To program virtual function device BAR

 **/
VOID
ProgramBar (
  IN PCI_IO_DEVICE         *PciIoDevice,
  IN UINT32                 BarIdx,
  IN BOOLEAN                Virtual
  )
{
  PCI_BAR_TYPE                        BarType;
  UINT64                              Address64;
  PCI_BAR                            *PciBar;

  if (FeaturePcdGet (PcdSrIovSupport) && Virtual) {
    PciBar = PciIoDevice->VfPciBar;
  } else {
    PciBar = PciIoDevice->PciBar;
    //
    // If it is a PPB device, ProgramBar() is used to
    // program BAR0/1 of this PPB device.
    //
    if (IS_PCI_BRIDGE(&(PciIoDevice->Pci))) {
      ASSERT(BarIdx < PPB_MAX_BAR);
      PciBar = PciIoDevice->PpbBar;
    }
  }

  BarType = PciBar[BarIdx].OrgBarType;
  switch (BarType) {
  case PciBarTypeIo16:
  case PciBarTypeIo32:
  case PciBarTypeMem32:
  case PciBarTypePMem32:
    PciExpressWrite32 (PciIoDevice->Address + PciBar[BarIdx].Offset,
                       (UINT32)PciBar[BarIdx].BaseAddress);
    break;

  case PciBarTypeMem64:
  case PciBarTypePMem64:
    Address64 = PciBar[BarIdx].BaseAddress;
    PciExpressWrite32 (PciIoDevice->Address + PciBar[BarIdx].Offset, (UINT32)Address64);
    PciExpressWrite32 (PciIoDevice->Address + PciBar[BarIdx].Offset + 4, (UINT32)RShiftU64 (Address64, 32));
    break;

  default:
    break;
  }
}


/**
Program PCI-PCI bridge apperture.

@param PciIoDevice  Pointer to the PCI IO devie.
@param BarIdx       Point to resoure node structure.

**/
VOID
ProgramPpbApperture (
  IN PCI_IO_DEVICE                      *PciIoDevice,
  IN UINT32                              BarIdx
  )
{
  PCI_BAR_TYPE                        BarType;
  UINT64                              Length;
  UINT64                              Address;
  UINT32                              Address32;

  BarType = PciIoDevice->PciBar[BarIdx].BarType;
  Address = PciIoDevice->PciBar[BarIdx].BaseAddress;
  Length  = PciIoDevice->PciBar[BarIdx].Length;
  switch (BarType) {
  case PciBarTypeIo16:
  case PciBarTypeIo32:
    Address32 = ((UINT32) (Address)) >> 8;
    PciExpressWrite8 (PciIoDevice->Address + 0x1C, (UINT8)Address32);
    Address32 >>= 8;
    PciExpressWrite16 (PciIoDevice->Address + 0x30, (UINT16)Address32);
    Address32 = (UINT32) (Address + Length - 1);
    Address32 = ((UINT32) (Address32)) >> 8;
    PciExpressWrite8 (PciIoDevice->Address + 0x1D, (UINT8)Address32);
    Address32 >>= 8;
    PciExpressWrite16 (PciIoDevice->Address + 0x32, (UINT16)Address32);
    break;

  case PciBarTypeMem32:
    Address32 = ((UINT32) (Address)) >> 16;
    PciExpressWrite16 (PciIoDevice->Address + 0x20, (UINT16)Address32);
    Address32 = (UINT32) (Address + Length - 1);
    Address32 = ((UINT32) (Address32)) >> 16;
    PciExpressWrite16 (PciIoDevice->Address + 0x22, (UINT16)Address32);
    break;

  case PciBarTypePMem32:
  case PciBarTypePMem64:
    Address32 = ((UINT32) (Address)) >> 16;
    PciExpressWrite16 (PciIoDevice->Address + 0x24, (UINT16)Address32);
    Address32 = (UINT32) (Address + Length - 1);
    Address32 = ((UINT32) (Address32)) >> 16;
    PciExpressWrite16 (PciIoDevice->Address + 0x26, (UINT16)Address32);

    Address32 = (UINT32)RShiftU64 (Address, 32);
    PciExpressWrite32 (PciIoDevice->Address + 0x28, (UINT16)Address32);
    Address32 = (UINT32)RShiftU64 ((Address + Length - 1), 32);
    PciExpressWrite32 (PciIoDevice->Address + 0x2C, (UINT16)Address32);
    break;

  default:
    break;
  }
}

/**
  Programs the PCI IO device BAR with the memory index
  starting from the parent.

  @param Parent   pointer to the parent device
  @param BarType  Base address register type
 **/
VOID
ProgramResource (
  IN PCI_IO_DEVICE                      *Parent,
  IN PCI_BAR_TYPE                        BarType
  )
{
  LIST_ENTRY                *CurrentLink;
  PCI_IO_DEVICE             *PciIoDevice;
  UINT32                     Idx;

  if ((BarType == PciBarTypeUnknown) || (BarType > PciBarTypePMem64)) {
    return;
  }

  CurrentLink = Parent->ChildList.ForwardLink;
  while (CurrentLink != NULL && CurrentLink != &Parent->ChildList) {
    PciIoDevice = PCI_IO_DEVICE_FROM_LINK (CurrentLink);
    if (IS_PCI_BRIDGE(&(PciIoDevice->Pci))) {
      //
      // Program BAR0/1 for a PPB device
      //
      for (Idx = 0; Idx < PPB_MAX_BAR; Idx++) {
        if ((PciIoDevice->PpbBar[Idx].Length > 0) && (PciIoDevice->PpbBar[Idx].BarType == BarType)) {
          PciIoDevice->PpbBar[Idx].BaseAddress += PciIoDevice->Parent->PciBar[BarType - 1].BaseAddress;
          ProgramBar (PciIoDevice, Idx, FALSE);
        }
      }
    }
    for (Idx = 0; Idx < PCI_MAX_BAR; Idx++) {
      if ((PciIoDevice->PciBar[Idx].Length > 0) && (PciIoDevice->PciBar[Idx].BarType == BarType)) {
        PciIoDevice->PciBar[Idx].BaseAddress += PciIoDevice->Parent->PciBar[BarType - 1].BaseAddress;
        if (PciIoDevice->PciBar[Idx].Offset >= 0x100) {
          // Bridge
          ProgramPpbApperture (PciIoDevice, Idx);
        } else {
          // Device
          ProgramBar (PciIoDevice, Idx, FALSE);
        }
      }
      if (FeaturePcdGet (PcdSrIovSupport)) {
        if ((PciIoDevice->VfPciBar[Idx].Length > 0) && (PciIoDevice->VfPciBar[Idx].BarType == BarType)) {
          PciIoDevice->VfPciBar[Idx].BaseAddress += PciIoDevice->Parent->PciBar[BarType - 1].BaseAddress;
          ProgramBar (PciIoDevice, Idx, TRUE);
        }
      }
    }
    CurrentLink = CurrentLink->ForwardLink;
  }

  CurrentLink = Parent->ChildList.ForwardLink;
  while (CurrentLink != NULL && CurrentLink != &Parent->ChildList) {
    PciIoDevice = PCI_IO_DEVICE_FROM_LINK (CurrentLink);
    if (PciIoDevice->ChildList.ForwardLink != &PciIoDevice->ChildList) {
      ProgramResource (PciIoDevice, BarType);
    }
    CurrentLink = CurrentLink->ForwardLink;
  }
}

/**
 Initialize the PCI IO device list, calculate the required memory
 resource for this PCI device.

 @param Parent   pointer to the root PCI device
 @param BarType  Base address register type.
 **/
VOID
CalculateResource (
  IN PCI_IO_DEVICE                      *Parent,
  IN PCI_BAR_TYPE                        BarType
  )
{
  LIST_ENTRY                *CurrentLink;
  PCI_IO_DEVICE             *PciIoDevice;
  UINT32                     Idx;
  PCI_BAR_RESOURCE           ParentRes;
  PCI_BAR_RESOURCE          *PciBarRes;
  UINT64                     Base;
  UINT64                     Alignment;

  if ((BarType == PciBarTypeUnknown) || (BarType > PciBarTypePMem64)) {
    return;
  }

  ParentRes.PciBar = NULL;
  InitializeListHead (&ParentRes.Link);
  CurrentLink = Parent->ChildList.ForwardLink;
  while (CurrentLink != NULL && CurrentLink != &Parent->ChildList) {
    PciIoDevice = PCI_IO_DEVICE_FROM_LINK (CurrentLink);
    if (PciIoDevice->ChildList.ForwardLink != &PciIoDevice->ChildList) {
      //
      // Scope for the BAR0/1 space for a PPB
      //
      for (Idx = 0; Idx < PPB_MAX_BAR; Idx++) {
        if ((PciIoDevice->PpbBar[Idx].Length > 0) && (PciIoDevice->PpbBar[Idx].BarType == BarType)) {
          PciBarRes = (PCI_BAR_RESOURCE *)PciAllocatePool (sizeof (PCI_BAR_RESOURCE));
          PciBarRes->PciBar = &PciIoDevice->PpbBar[Idx];
          PerformInsertionSortList (&ParentRes.Link, &PciBarRes->Link, ComparePciBarRes);
        }
      }
      CalculateResource (PciIoDevice, BarType);
    }
    for (Idx = 0; Idx < PCI_MAX_BAR; Idx++) {
      if (PciIoDevice->PciBar[Idx].BarType == BarType) {
        PciBarRes = (PCI_BAR_RESOURCE *)PciAllocatePool (sizeof (PCI_BAR_RESOURCE));
        PciBarRes->PciBar = &PciIoDevice->PciBar[Idx];
        PerformInsertionSortList (&ParentRes.Link, &PciBarRes->Link, ComparePciBarRes);
      }
      if (FeaturePcdGet (PcdSrIovSupport)) {
        if (PciIoDevice->VfPciBar[Idx].BarType == BarType) {
          PciBarRes = (PCI_BAR_RESOURCE *)PciAllocatePool (sizeof (PCI_BAR_RESOURCE));
          PciBarRes->PciBar = &PciIoDevice->VfPciBar[Idx];
          PerformInsertionSortList (&ParentRes.Link, &PciBarRes->Link, ComparePciBarRes);
        }
      }
    }
    CurrentLink = CurrentLink->ForwardLink;
  }

  Base      = 0;
  Alignment = 0;
  CurrentLink = ParentRes.Link.ForwardLink;
  while (CurrentLink != NULL && CurrentLink != &ParentRes.Link) {
    PciBarRes = PCI_BAR_RESOURCE_FROM_LINK (CurrentLink);
    if (Alignment == 0) {
      Alignment = PciBarRes->PciBar->Alignment;
    }
    Base = ALIGN (Base, PciBarRes->PciBar->Alignment);
    PciBarRes->PciBar->BaseAddress = Base;
    Base += PciBarRes->PciBar->Length;
    CurrentLink = CurrentLink->ForwardLink;
  }

  if (BarType <= PciBarTypeIo32) {
    if (Alignment < 0xFFF) {
      Alignment = 0xFFF;
    }
    Base = ALIGN (Base,  0xFFF);
  } else {
    if (Alignment < 0xFFFFF) {
      Alignment = 0xFFFFF;
    }
    Base = ALIGN (Base,  0xFFFFF);
  }

  Parent->PciBar[BarType - 1].Alignment = Alignment;
  Parent->PciBar[BarType - 1].BarType   = BarType;
  Parent->PciBar[BarType - 1].Length    = Base;
  Parent->PciBar[BarType - 1].Offset   += 0x100;
}

/**
 Recursive function that traverse the PCI IO DEVICE
 list from the given parent and enables the PCI devices.

 @param Parent  Pointer to the parent PCI IO Device.
 **/
VOID
EnablePciDevice (
  IN PCI_IO_DEVICE                      *Parent
  )
{
  LIST_ENTRY                *CurrentLink;
  PCI_IO_DEVICE             *PciIoDevice;

  CurrentLink = Parent->ChildList.ForwardLink;
  while (CurrentLink != NULL && CurrentLink != &Parent->ChildList) {
    PciIoDevice = PCI_IO_DEVICE_FROM_LINK (CurrentLink);
    PciExpressOr16 (PciIoDevice->Address + PCI_COMMAND_OFFSET,
                    EFI_PCI_COMMAND_IO_SPACE | EFI_PCI_COMMAND_MEMORY_SPACE);
    if (PciIoDevice->ChildList.ForwardLink != &PciIoDevice->ChildList) {
      if (IS_PCI_BRIDGE (&(PciIoDevice->Pci))) {
        PciExpressOr16 (PciIoDevice->Address + PCI_COMMAND_OFFSET,
                        EFI_PCI_COMMAND_BUS_MASTER);
      }
      EnablePciDevice (PciIoDevice);
    }
    CurrentLink = CurrentLink->ForwardLink;
  }
}

/**
  Get a pointer of PCI resource allocation table

  @param [out]  ResAllocTable   A table pointer for PCI resource allocation ranges

 **/
VOID
GetPciResourceAllocTable (
  OUT PCI_RES_ALLOC_TABLE       **ResAllocTable
  )
{
  UINTN                          Length;

  if (mResAllocTablePtr == NULL) {
    mResAllocTablePtr = (PCI_RES_ALLOC_TABLE *)(UINTN)PcdGet32 (PcdPciResAllocTableBase);
    if (mResAllocTablePtr == NULL) {
      Length = sizeof (PCI_RES_ALLOC_TABLE) + sizeof (PCI_RES_ALLOC_RANGE);
      mResAllocTablePtr = PciAllocatePool (Length);
      CopyMem (&mResAllocTablePtr->ResourceRange[0], &mDefaultResRange, sizeof (PCI_RES_ALLOC_RANGE));
      mResAllocTablePtr->NumOfEntries = 1;
    }
  }
  *ResAllocTable = mResAllocTablePtr;
}

/**
  Get the pointer of PCI resource allocation range for the specific RootBridge

  @param [in] ResAllocTable   PCI Resource Allocation Table
  @param [in] RootBridge      A specific RootBridge to get its resource allocation range

  @retval PCI_RES_ALLOC_RANGE The resource range pointer of the specific RootBridge

 **/
CONST PCI_RES_ALLOC_RANGE *
PciGetResAllocRange (
  IN CONST  PCI_RES_ALLOC_TABLE *ResAllocTable,
  IN        PCI_IO_DEVICE       *RootBridge
  )
{
  UINT8   Index;

  for (Index = 0; Index < ResAllocTable->NumOfEntries; Index++) {
    if ((ResAllocTable->ResourceRange[Index].BusBase <= RootBridge->BusNumberRanges.BusBase) &&
        (ResAllocTable->ResourceRange[Index].BusLimit >= RootBridge->BusNumberRanges.BusLimit)) {
      return &ResAllocTable->ResourceRange[Index];
    }
  }

  // Never reach here
  ASSERT (FALSE);
  return NULL;
}

#if DEBUG_PCI_ENUM
/**
  Dumps the PCI BaseAddress, BarType, Offset,Length and Alignment for
  the requested PCI IO device.

  @param PciIoDevice pointer the the PciIoDevice that BAR info needed.
 **/
VOID
DumpResourceBar (
  IN PCI_IO_DEVICE  *PciIoDevice
  )
{
  UINT32                     Idx;
  UINT32                     Level;
  UINT32                     Address;
  PCI_IO_DEVICE             *Parent;
  CHAR8                     *Indent;

  Indent = "                    ";

  Level = 0;
  Parent = PciIoDevice->Parent;
  while ((Parent != NULL) && (Level < 5)) {
    Level++;
    Parent = Parent->Parent;
  }

  Address = PciIoDevice->Address;
  Indent[ (Level << 1) + 1] = 0;
  if ((Address & BIT31) != 0) {
    DEBUG ((DEBUG_INFO, "PCI HOST: Bus(0x%02X-%02X)\n",
      PciIoDevice->BusNumberRanges.BusBase,
      PciIoDevice->BusNumberRanges.BusLimit));
  } else {
    DEBUG ((DEBUG_INFO, "%aPCI(%02X,%02X,%02X)\n", Indent, (Address >> 20) & 0xFF, (Address >> 15) & 0x1F,
            (Address >> 12) & 0x07));
    if (FeaturePcdGet (PcdAriSupport) && (PciIoDevice->AriCapabilityOffset != 0)) {
      DEBUG ((DEBUG_INFO, "%aARI: forwarding enabled for PPB[%02x:%02x:%02x]\n",
        Indent,
        (PciIoDevice->Parent->Address >> 20) & 0xFF,
        (PciIoDevice->Parent->Address >> 15) & 0x1F,
        (PciIoDevice->Parent->Address >> 12) & 0x07));
      DEBUG ((DEBUG_INFO, "%aARI: CapOffset = 0x%X\n",
        Indent, PciIoDevice->AriCapabilityOffset));
    }
  }
  if (IS_PCI_BRIDGE(&(PciIoDevice->Pci))) {
    //
    // Dump BAR0/1 also for a PPB
    //
    DEBUG ((DEBUG_INFO, "%a  PPB BAR0/1\n", Indent));
    for (Idx = 0; Idx < PPB_MAX_BAR; Idx++) {
      if (PciIoDevice->PpbBar[Idx].Length == 0) {
        continue;
      }
      DEBUG ((DEBUG_INFO, "%a  BAR[%d].TYP = %d\n", Indent,  Idx, PciIoDevice->PpbBar[Idx].BarType));
      DEBUG ((DEBUG_INFO, "%a  BAR[%d].OFF = 0x%02X\n", Indent,  Idx, PciIoDevice->PpbBar[Idx].Offset));
      DEBUG ((DEBUG_INFO, "%a  BAR[%d].BAS = %016lX", Indent,  Idx, PciIoDevice->PpbBar[Idx].BaseAddress));
      DEBUG ((DEBUG_INFO, "  BAR[%d].LEN = %016lX",   Idx, PciIoDevice->PpbBar[Idx].Length));
      DEBUG ((DEBUG_INFO, "  BAR[%d].ALN = %016lX\n", Idx, PciIoDevice->PpbBar[Idx].Alignment));
    }
    DEBUG ((DEBUG_INFO, "%a  PPB APPERTURE\n", Indent));
  }
  for (Idx = 0; Idx < PCI_MAX_BAR; Idx++) {
    if (PciIoDevice->PciBar[Idx].Length == 0) {
      continue;
    }
    DEBUG ((DEBUG_INFO, "%a  BAR[%d].TYP = %d\n", Indent,  Idx, PciIoDevice->PciBar[Idx].BarType));
    DEBUG ((DEBUG_INFO, "%a  BAR[%d].OFF = 0x%02X\n", Indent,  Idx, PciIoDevice->PciBar[Idx].Offset));
    DEBUG ((DEBUG_INFO, "%a  BAR[%d].BAS = %016lX", Indent,  Idx, PciIoDevice->PciBar[Idx].BaseAddress));
    DEBUG ((DEBUG_INFO, "  BAR[%d].LEN = %016lX",   Idx, PciIoDevice->PciBar[Idx].Length));
    DEBUG ((DEBUG_INFO, "  BAR[%d].ALN = %016lX\n", Idx, PciIoDevice->PciBar[Idx].Alignment));
  }
  if (FeaturePcdGet (PcdSrIovSupport)) {
    for (Idx = 0; Idx < PCI_MAX_BAR; Idx++) {
      if (PciIoDevice->VfPciBar[Idx].Length == 0) {
        continue;
      }
      DEBUG ((DEBUG_INFO, "%a  VFBAR[%d].TYP = %d\n", Indent,  Idx, PciIoDevice->VfPciBar[Idx].BarType));
      DEBUG ((DEBUG_INFO, "%a  VFBAR[%d].OFF = 0x%02X\n", Indent,  Idx, PciIoDevice->VfPciBar[Idx].Offset));
      DEBUG ((DEBUG_INFO, "%a  VFBAR[%d].BAS = %016lX", Indent,  Idx, PciIoDevice->VfPciBar[Idx].BaseAddress));
      DEBUG ((DEBUG_INFO, "  VFBAR[%d].LEN = %016lX",   Idx, PciIoDevice->VfPciBar[Idx].Length));
      DEBUG ((DEBUG_INFO, "  VFBAR[%d].ALN = %016lX\n", Idx, PciIoDevice->VfPciBar[Idx].Alignment));
    }
  }

  Indent[ (Level << 1) + 1] = ' ';
}

/**
  Dump the device tree of each PCI device.

  @param Parent Pointer to the parent PCI device.
 **/
VOID
DumpDeivceTree (
  IN PCI_IO_DEVICE                      *Parent
  )
{
  LIST_ENTRY                *CurrentLink;
  PCI_IO_DEVICE             *PciIoDevice;

  CurrentLink = Parent->ChildList.ForwardLink;
  while (CurrentLink != NULL && CurrentLink != &Parent->ChildList) {
    PciIoDevice = PCI_IO_DEVICE_FROM_LINK (CurrentLink);
    DumpResourceBar (PciIoDevice);
    if (PciIoDevice->ChildList.ForwardLink != &PciIoDevice->ChildList) {
      DumpDeivceTree (PciIoDevice);
    }
    CurrentLink = CurrentLink->ForwardLink;
  }
}

/**
  Dump device tree and resource bar from Pci Root Bridges

  @param [in] RootBridge  A pointer which has Root Bridges in ChildList

 **/
VOID
DumpPciResources (
  IN  CONST PCI_IO_DEVICE   *RootBridge
  )
{
  LIST_ENTRY                *CurrentLink;
  PCI_IO_DEVICE             *Root;

  CurrentLink = RootBridge->ChildList.ForwardLink;
  while ((CurrentLink != NULL) && (CurrentLink != &RootBridge->ChildList)) {
    Root = PCI_IO_DEVICE_FROM_LINK (CurrentLink);
    DumpDeivceTree  (Root);
    DumpResourceBar (Root);

    CurrentLink = CurrentLink->ForwardLink;
  }
}

/**
  Dump Universal Payload PCI Root Bridge HOB

**/
VOID
DumpUniversalPayloadPciRootBridgeHob (
  VOID
  )
{
  UNIVERSAL_PAYLOAD_PCI_ROOT_BRIDGES  *UpldRootBridges;
  UINT8                               Count;

  UpldRootBridges = (UNIVERSAL_PAYLOAD_PCI_ROOT_BRIDGES *) GetGuidHobData (NULL, NULL,
    &gUniversalPayloadPciRootBridgeInfoGuid);

  if (UpldRootBridges != NULL) {
    DEBUG ((DEBUG_INFO, "Universal Payload PCI Root Bridge HOB: Rev 0x%X, Count 0x%X\n",
      UpldRootBridges->Header.Revision, UpldRootBridges->Count));

    for (Count = 0; Count < UpldRootBridges->Count; Count++) {
      DEBUG ((DEBUG_INFO, "Root Bridge %d:\n", Count));
      DEBUG ((DEBUG_INFO, "  HID: 0x%08X, UID: 0x%X, Segment: 0x%X\n",
        UpldRootBridges->RootBridge[Count].HID,
        UpldRootBridges->RootBridge[Count].UID,
        UpldRootBridges->RootBridge[Count].Segment));
      DEBUG ((DEBUG_INFO, "  Bus: 0x%02lX-0x%02lX\n",
        UpldRootBridges->RootBridge[Count].Bus.Base,
        UpldRootBridges->RootBridge[Count].Bus.Limit));

      if (UpldRootBridges->RootBridge[Count].Io.Base != MAX_UINT16) {
        DEBUG ((DEBUG_INFO, "  I/O: 0x%04lX-0x%04lX\n",
          UpldRootBridges->RootBridge[Count].Io.Base,
          UpldRootBridges->RootBridge[Count].Io.Limit));
      }

      if (UpldRootBridges->RootBridge[Count].Mem.Base != MAX_UINT32) {
        DEBUG ((DEBUG_INFO, "  Mem32: 0x%08lX-0x%08lX\n",
          UpldRootBridges->RootBridge[Count].Mem.Base,
          UpldRootBridges->RootBridge[Count].Mem.Limit));
      }

      if (UpldRootBridges->RootBridge[Count].MemAbove4G.Base != MAX_UINT64) {
        DEBUG ((DEBUG_INFO, "  Mem64: 0x%016lX-0x%016lX\n",
          UpldRootBridges->RootBridge[Count].MemAbove4G.Base,
          UpldRootBridges->RootBridge[Count].MemAbove4G.Limit));
      }

      if (UpldRootBridges->RootBridge[Count].PMem.Base != MAX_UINT32) {
        DEBUG ((DEBUG_INFO, "  PMem32: 0x%08lX-0x%08lX\n",
          UpldRootBridges->RootBridge[Count].PMem.Base,
          UpldRootBridges->RootBridge[Count].PMem.Limit));
      }

      if (UpldRootBridges->RootBridge[Count].PMemAbove4G.Base != MAX_UINT64) {
        DEBUG ((DEBUG_INFO, "  PMem64: 0x%016lX-0x%016lX\n",
          UpldRootBridges->RootBridge[Count].PMemAbove4G.Base,
          UpldRootBridges->RootBridge[Count].PMemAbove4G.Limit));
      }
    }
  } else {
    DEBUG ((DEBUG_INFO, "Universal Payload PCI Root Bridge HOB not found\n"));
  }
}

/**
  Dump PCI Resource Allocation Table

**/
VOID
DumpPciResAllocTable (
  VOID
  )
{
  PCI_RES_ALLOC_TABLE   *ResAllocTable;
  PCI_RES_ALLOC_RANGE   *ResRange;
  UINT8   Index;

  GetPciResourceAllocTable (&ResAllocTable);
  ASSERT (ResAllocTable != NULL);

  DEBUG ((DEBUG_INFO, "DumpPciResAllocTable: NumOfEntries %X\n", ResAllocTable->NumOfEntries));
  for (Index = 0; Index < ResAllocTable->NumOfEntries; Index++) {
    ResRange = &ResAllocTable->ResourceRange[Index];
    DEBUG ((DEBUG_INFO, "Bus   : %X - %X\n", ResRange->BusBase, ResRange->BusLimit));
    DEBUG ((DEBUG_INFO, " Io   : %X - %X\n", ResRange->IoBase, ResRange->IoLimit));
    DEBUG ((DEBUG_INFO, " Mem32: %X - %X\n", ResRange->Mmio32Base, ResRange->Mmio32Limit));
    DEBUG ((DEBUG_INFO, " Mem64: %llX - %llX\n", ResRange->Mmio64Base, ResRange->Mmio64Limit));
  }
}
#endif

/**
  Enable all Pci devices from scanned Pci Root Bridges

  @param [in] RootBridges A pointer which has Root Bridges in ChildList

 **/
VOID
PciEnableDevices (
  IN  CONST PCI_IO_DEVICE   *RootBridges
  )
{
  LIST_ENTRY                *CurrentLink;
  PCI_IO_DEVICE             *Root;

  CurrentLink = RootBridges->ChildList.ForwardLink;
  while ((CurrentLink != NULL) && (CurrentLink != &RootBridges->ChildList)) {
    Root = PCI_IO_DEVICE_FROM_LINK (CurrentLink);
    EnablePciDevice (Root);

    CurrentLink = CurrentLink->ForwardLink;
  }
}

/**
  Program Pci resources

  @param [in] EnumPolicy    PciEnum Policy with root bridge mask to be scanned
  @param [in] RootBridges   A pointer which has Root Bridges in ChildList

 **/
VOID
PciProgramResources (
  IN CONST  PCI_ENUM_POLICY_INFO  *EnumPolicy,
  IN CONST  PCI_RES_ALLOC_TABLE   *ResAllocTable,
  IN CONST  PCI_IO_DEVICE         *RootBridges
)
{
  LIST_ENTRY                *CurrentLink;
  PCI_IO_DEVICE             *Root;
  CONST PCI_RES_ALLOC_RANGE *ResRange;
  UINT64                     Address;
  PCI_BAR_TYPE               BarType;
  PCI_BAR_TYPE               BarTypeStart;
  PCI_BAR_TYPE               BarTypeEnd;
  INT8                       BarTypeStep;
  UINT64                     ResBase[3];
  UINT64                     ResLimit[3];
  UINT8                      Index;

  CurrentLink = RootBridges->ChildList.ForwardLink;
  while ((CurrentLink != NULL) && (CurrentLink != &RootBridges->ChildList)) {
    Root = PCI_IO_DEVICE_FROM_LINK (CurrentLink);

    ResRange = PciGetResAllocRange (ResAllocTable, Root);
    ResBase[0]  = ResRange->IoBase;
    ResLimit[0] = ResRange->IoLimit;
    ResBase[1]  = ResRange->Mmio32Base;
    ResLimit[1] = ResRange->Mmio32Limit;
    ResBase[2]  = ResRange->Mmio64Base;
    ResLimit[2] = ResRange->Mmio64Limit;

    if (EnumPolicy->Flag.AllocPmemFirst != 0) {
      BarTypeStart = PciBarTypePMem64;
      BarTypeEnd   = PciBarTypeIo16 - 1;
      BarTypeStep  = -1;
    } else {
      BarTypeStart = PciBarTypeIo16;
      BarTypeEnd   = PciBarTypePMem64 + 1;
      BarTypeStep  = 1;
    }

    for (BarType = BarTypeStart; BarType != BarTypeEnd; BarType += BarTypeStep) {
      if ((BarType < PciBarTypeIo16) || (BarType > PciBarTypePMem64)) {
        break;
      }
      if (((BarType == PciBarTypeIo32) && (EnumPolicy->Downgrade.Io32 != 0)) ||
          ((BarType == PciBarTypeMem64) && (EnumPolicy->Downgrade.Mem64 != 0)) ||
          ((BarType == PciBarTypePMem64) && (EnumPolicy->Downgrade.PMem64 != 0))) {
        continue;
      }

      Index = ((UINT8)BarType - 1) / 2;
      if (BarType == PciBarTypeIo32) {
        ResBase[Index] = ALIGN (ResBase[Index], 0xFFFF);
      }

      CalculateResource (Root, BarType);
      Address = ResBase[Index];
      Address = ALIGN (Address, Root->PciBar[BarType - 1].Alignment);
      Root->PciBar[BarType - 1].BaseAddress = Address;
      if (Root->PciBar[BarType - 1].Length > 0) {
        ResBase[Index] += Root->PciBar[BarType - 1].Length;
        ASSERT (ResBase[Index] <= (ResLimit[Index] + 1));
      }

      ProgramResource (Root, BarType);
    }

    CurrentLink = CurrentLink->ForwardLink;
  }
}

/**
 Scan Root Bridges depending on Pci Enumeration Policy

 @param [in]  EnumPolicy        PciEnum Policy with root bridge mask to be scanned
 @param [out] RootBridges       A pointer which has root bridges in ChildList
 @param [out] RootBridgeCount   The number of detected Root Bridges

 **/
EFI_STATUS
PciScanRootBridges (
  IN CONST  PCI_ENUM_POLICY_INFO   *EnumPolicy,
  OUT       PCI_IO_DEVICE         **RootBridges,
  OUT       UINT8                  *RootBridgeCount
  )
{
  UINT16                            Bus;
  UINT8                             SubBusNumber;
  PCI_IO_DEVICE                    *Bridge;
  PCI_IO_DEVICE                    *Root;
  UINT16                            Index;
  UINT16                            StartIndex;
  UINT16                            EndIndex;
  UINT8                             Count;
  UINT8                             BusLimit;
  UINT32                            RootBridgeDecodes;

  if ((EnumPolicy == NULL) || (RootBridges == NULL) || (RootBridgeCount == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  Bridge = (PCI_IO_DEVICE *)PciAllocatePool (sizeof (PCI_IO_DEVICE));
  ZeroMem (Bridge, sizeof (PCI_IO_DEVICE));
  InitializeListHead (&Bridge->ChildList);
  Count = 0;

  //
  // By default, enumerate Bus-0 only
  //
  StartIndex  = 0;
  EndIndex    = 0;
  if ((EnumPolicy->BusScanType == BusScanTypeRange) && (EnumPolicy->NumOfBus == 2)) {
    StartIndex  = EnumPolicy->BusScanItems[0];
    EndIndex    = EnumPolicy->BusScanItems[1];
  } else if (EnumPolicy->BusScanType == BusScanTypeList) {
    StartIndex  = 0;
    EndIndex    = EnumPolicy->NumOfBus - 1;
  }

  //
  // Get the number of max bus number.
  // Use PCI_MAX_BUS if the enum policy has no multiple buses.
  //
  BusLimit = PCI_MAX_BUS;
  if ((EnumPolicy->BusScanType == BusScanTypeRange) && (EnumPolicy->NumOfBus > 1)) {
    if (StartIndex != EndIndex) {
      BusLimit = EnumPolicy->BusScanItems[EnumPolicy->NumOfBus - 1];
    }
  }

  //
  // Root bridge decode space
  //
  RootBridgeDecodes = 0xFFFFFFFF;
  if (EnumPolicy->Downgrade.Io32 != 0) {
    RootBridgeDecodes &= (UINT32)~(EFI_BRIDGE_IO32_DECODE_SUPPORTED);
  }
  if (EnumPolicy->Downgrade.Mem64 != 0) {
    RootBridgeDecodes &= (UINT32)~(EFI_BRIDGE_MEM64_DECODE_SUPPORTED);
  }
  if (EnumPolicy->Downgrade.PMem64 != 0) {
    RootBridgeDecodes &= (UINT32)~(EFI_BRIDGE_PMEM64_DECODE_SUPPORTED);
  }

  for (Index = StartIndex; Index <= EndIndex; Index++) {
    if (EnumPolicy->BusScanType == BusScanTypeList) {
      Bus = EnumPolicy->BusScanItems[Index];
    } else {
      Bus = Index;
    }

    Root = CreatePciIoDevice (NULL, NULL, (UINT8)Bus, 0, 0);
    Root->Decodes = RootBridgeDecodes;
    Root->BusNumberRanges.BusBase  = (UINT8)Bus;
    Root->BusNumberRanges.BusLimit = BusLimit;

    SubBusNumber = (UINT8)Bus;
    PciScanBus (Root, (UINT8)Bus, &SubBusNumber, NULL);
    if (Bus == PCI_MAX_BUS) {
      SubBusNumber = (UINT8)Bus;
    }
    Root->BusNumberRanges.BusLimit = SubBusNumber;
    Root->Address |= BIT31;

    // Only add Root Bridges with actual devices, not empty ones.
    if (Root->ChildList.ForwardLink != &Root->ChildList) {
      InsertPciDevice (Bridge, Root);
      Count++;
    }

    if (EnumPolicy->BusScanType != BusScanTypeList) {
      Index = SubBusNumber;
    }
  }
  *RootBridges = Bridge;
  *RootBridgeCount = Count;

  return EFI_SUCCESS;
}

/**
  Build Universal Payload PCI Root Bridge HOB with actual enumeration data

  @param [in] RootBridges       A pointer of Root Bridges List
  @param [in] RootBridgeCount   The number of found root bridges

  @retval EFI_SUCCESS           Create HOBs successfully
  @retval EFI_OUT_OF_RESOURCES  Out of Hob resource

 **/
EFI_STATUS
BuildUniversalPayloadPciRootBridgeHob (
  IN  CONST PCI_IO_DEVICE   *RootBridges,
  IN        UINT8            RootBridgeCount
  )
{
  UNIVERSAL_PAYLOAD_PCI_ROOT_BRIDGES  *UpldRootBridges;
  LIST_ENTRY                          *CurrentLink;
  PCI_IO_DEVICE                       *Root;
  UINTN                               Length;
  UINT8                               Index;
  UINT8                               BarIndex;
  PCI_BAR_TYPE                        BarType;

  if (RootBridgeCount == 0) {
    return EFI_SUCCESS;
  }

  Length = sizeof (UNIVERSAL_PAYLOAD_PCI_ROOT_BRIDGES) + sizeof (UNIVERSAL_PAYLOAD_PCI_ROOT_BRIDGE) * RootBridgeCount;
  UpldRootBridges = (UNIVERSAL_PAYLOAD_PCI_ROOT_BRIDGES *)BuildGuidHob (&gUniversalPayloadPciRootBridgeInfoGuid, Length);
  if (UpldRootBridges == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  ZeroMem (UpldRootBridges, Length);
  UpldRootBridges->Header.Revision  = UNIVERSAL_PAYLOAD_PCI_ROOT_BRIDGES_REVISION;
  UpldRootBridges->Header.Length    = (UINT16)Length;
  UpldRootBridges->ResourceAssigned = TRUE;
  UpldRootBridges->Count = RootBridgeCount;

  Index = 0;
  CurrentLink = RootBridges->ChildList.ForwardLink;
  while ((CurrentLink != NULL) && (CurrentLink != &RootBridges->ChildList) && (Index < RootBridgeCount)) {
    Root = PCI_IO_DEVICE_FROM_LINK (CurrentLink);

    // Configure root bridge
    UpldRootBridges->RootBridge[Index].HID = EISA_PNP_ID(0x0A03);
    UpldRootBridges->RootBridge[Index].UID = Index;
    UpldRootBridges->RootBridge[Index].Segment   = 0;
    UpldRootBridges->RootBridge[Index].Supports  = EFI_PCI_ATTRIBUTE_IDE_PRIMARY_IO |
                                                   EFI_PCI_ATTRIBUTE_IDE_SECONDARY_IO |
                                                   EFI_PCI_ATTRIBUTE_ISA_IO_16 |
                                                   EFI_PCI_ATTRIBUTE_ISA_MOTHERBOARD_IO |
                                                   EFI_PCI_ATTRIBUTE_VGA_MEMORY |
                                                   EFI_PCI_ATTRIBUTE_VGA_IO_16 |
                                                   EFI_PCI_ATTRIBUTE_VGA_PALETTE_IO_16;
    UpldRootBridges->RootBridge[Index].Attributes = UpldRootBridges->RootBridge[Index].Supports;
    UpldRootBridges->RootBridge[Index].AllocationAttributes = EFI_PCI_HOST_BRIDGE_COMBINE_MEM_PMEM |
                                                              EFI_PCI_HOST_BRIDGE_MEM64_DECODE;

    // Bus range from enumerated data
    UpldRootBridges->RootBridge[Index].Bus.Base  = Root->BusNumberRanges.BusBase;
    UpldRootBridges->RootBridge[Index].Bus.Limit = Root->BusNumberRanges.BusLimit;

    // Initialize resource ranges to invalid values
    UpldRootBridges->RootBridge[Index].Io.Base = MAX_UINT16;
    UpldRootBridges->RootBridge[Index].Mem.Base = MAX_UINT32;
    UpldRootBridges->RootBridge[Index].MemAbove4G.Base = MAX_UINT64;
    UpldRootBridges->RootBridge[Index].PMem.Base = MAX_UINT32;
    UpldRootBridges->RootBridge[Index].PMemAbove4G.Base = MAX_UINT64;

    // Fill in actual resource ranges from enumerated data
    for (BarIndex = 0; BarIndex < PCI_MAX_BAR; BarIndex++) {
      if (Root->PciBar[BarIndex].Length == 0) {
        continue;
      }
      BarType = Root->PciBar[BarIndex].BarType;

      switch (BarType) {
        case PciBarTypeIo16:
          UpldRootBridges->RootBridge[Index].Io.Base  = Root->PciBar[BarIndex].BaseAddress;
          UpldRootBridges->RootBridge[Index].Io.Limit = Root->PciBar[BarIndex].BaseAddress + Root->PciBar[BarIndex].Length - 1;
          break;
        case PciBarTypeMem32:
          UpldRootBridges->RootBridge[Index].Mem.Base  = Root->PciBar[BarIndex].BaseAddress;
          UpldRootBridges->RootBridge[Index].Mem.Limit = Root->PciBar[BarIndex].BaseAddress + Root->PciBar[BarIndex].Length - 1;
          break;
        case PciBarTypeMem64:
          UpldRootBridges->RootBridge[Index].MemAbove4G.Base  = Root->PciBar[BarIndex].BaseAddress;
          UpldRootBridges->RootBridge[Index].MemAbove4G.Limit = Root->PciBar[BarIndex].BaseAddress + Root->PciBar[BarIndex].Length - 1;
          break;
        case PciBarTypePMem32:
          UpldRootBridges->RootBridge[Index].PMem.Base  = Root->PciBar[BarIndex].BaseAddress;
          UpldRootBridges->RootBridge[Index].PMem.Limit = Root->PciBar[BarIndex].BaseAddress + Root->PciBar[BarIndex].Length - 1;
          break;
        case PciBarTypePMem64:
          UpldRootBridges->RootBridge[Index].PMemAbove4G.Base  = Root->PciBar[BarIndex].BaseAddress;
          UpldRootBridges->RootBridge[Index].PMemAbove4G.Limit = Root->PciBar[BarIndex].BaseAddress + Root->PciBar[BarIndex].Length - 1;
          break;
        default:
          break;
      }
    }

    Index++;
    CurrentLink = CurrentLink->ForwardLink;
  }

  return EFI_SUCCESS;
}

/**
 Enumerates the PCI devices allocates the required memory resource.
 Program the allocated memory resource to PCI BAR.

 @param [in] MemPool point to memory pool to allocate for each PCI device.
 **/
EFI_STATUS
EFIAPI
PciEnumeration (
  IN  VOID   *MemPool
  )
{
  CONST PCI_ENUM_POLICY_INFO  *EnumPolicy;
  PCI_RES_ALLOC_TABLE         *ResAllocTable;
  PCI_IO_DEVICE               *RootBridges;
  UINT8                       RootBridgeCount;
  EFI_STATUS                  Status;

  SetAllocationPool (MemPool);

  // Update PCI default resource ranges
  mDefaultResRange.IoBase       = PcdGet32 (PcdPciResourceIoBase);
  mDefaultResRange.Mmio32Base   = PcdGet32 (PcdPciResourceMem32Base);
  mDefaultResRange.Mmio64Base   = PcdGet64 (PcdPciResourceMem64Base);
  mDefaultResRange.Mmio64Limit  = PcdGet64 (PcdPciResourceMem64Base) + (RShiftU64 (PcdGet64 (PcdPciResourceMem64Base), 1));

  EnumPolicy = (PCI_ENUM_POLICY_INFO *)PcdGetPtr (PcdPciEnumPolicyInfo);
  RootBridgeCount = 0;

  Status = PciScanRootBridges (EnumPolicy, &RootBridges, &RootBridgeCount);
  ASSERT_EFI_ERROR (Status);
  ASSERT (RootBridgeCount > 0);

  GetPciResourceAllocTable (&ResAllocTable);
  PciProgramResources (EnumPolicy, ResAllocTable, RootBridges);

  PciEnableDevices (RootBridges);

  BuildUniversalPayloadPciRootBridgeHob (RootBridges, RootBridgeCount);

#if DEBUG_PCI_ENUM
  DumpPciResAllocTable ();
  DumpPciResources (RootBridges);
  DumpUniversalPayloadPciRootBridgeHob ();
  DEBUG ((DEBUG_INFO, "MEM Pool Used: 0x%08X\n", (UINT32)(UINTN)GetAllocationPool() - (UINT32)(UINTN)MemPool));
#endif

  // Free memory pool
  SetAllocationPool (MemPool);

  return EFI_SUCCESS;
}
