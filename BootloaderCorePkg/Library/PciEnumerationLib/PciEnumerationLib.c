/** @file

  Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <PiPei.h>
#include <Library/PcdLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PciExpressLib.h>
#include <InternalPciEnumerationLib.h>

#define  DEBUG_PCI_ENUM    0
#define  DOWNGRADE_IO32    1
#define  DOWNGRADE_PMEM64  1
#define  DOWNGRADE_MEM64   1

UINT8   *mPoolPtr;

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
    CopyMem (Pci, (VOID *) ((UINTN)PcdGet64 (PcdPciExpressBaseAddress) + \
                            PCI_EXPRESS_LIB_ADDRESS (Bus, Device, Func, 0)), sizeof (PCI_TYPE00));
    return EFI_SUCCESS;
  }
}

/**
  Check whether the bar is existed or not.

  @param PciIoDevice       A pointer to the PCI_IO_DEVICE.
  @param Offset            The offset.
  @param BarLengthValue    The bar length value returned.
  @param OriginalBarValue  The original bar value returned.

  @retval EFI_NOT_FOUND    The bar doesn't exist.
  @retval EFI_SUCCESS      The bar exist.

**/
EFI_STATUS
BarExisted (
  IN  PCI_IO_DEVICE *PciIoDevice,
  IN  UINTN         Offset,
  OUT UINT32        *BarLengthValue,
  OUT UINT32        *OriginalBarValue
  )
{
  UINT32              OriginalValue;
  UINT32              Value;

  //
  // Preserve the original value
  //
  OriginalValue = PciExpressRead32 (PciIoDevice->Address + Offset);
  PciExpressWrite32 (PciIoDevice->Address + Offset, 0xFFFFFFFF);
  Value = PciExpressRead32 (PciIoDevice->Address + Offset);
  PciExpressWrite32 (PciIoDevice->Address + Offset, OriginalValue);

  if (BarLengthValue != NULL) {
    *BarLengthValue = Value;
  }

  if (OriginalBarValue != NULL) {
    *OriginalBarValue = OriginalValue;
  }

  if (Value == 0) {
    return EFI_NOT_FOUND;
  } else {
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
  UINT32        Value;
  UINT32        OriginalValue;
  UINT32        Mask;
  EFI_STATUS    Status;
  PCI_BAR_TYPE  BarType;

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
      BarType = PciBarTypeIo32;
#ifdef DOWNGRADE_IO32
      PciIoDevice->PciBar[BarIndex].BarType   = PciBarTypeIo16;
#else
      PciIoDevice->PciBar[BarIndex].BarType   = PciBarTypeIo32;
#endif
      PciIoDevice->PciBar[BarIndex].Length    = ((~ (Value & Mask)) + 1);
      PciIoDevice->PciBar[BarIndex].Alignment = PciIoDevice->PciBar[BarIndex].Length - 1;

    } else {
      //
      // It is a IO16 bar
      //
      BarType = PciBarTypeIo16;
      PciIoDevice->PciBar[BarIndex].BarType   = PciBarTypeIo16;
      PciIoDevice->PciBar[BarIndex].Length    = 0x0000FFFF & ((~ (Value & Mask)) + 1);
      PciIoDevice->PciBar[BarIndex].Alignment = PciIoDevice->PciBar[BarIndex].Length - 1;

    }
    //
    // Workaround. Some platforms inplement IO bar with 0 length
    // Need to treat it as no-bar
    //
    if (PciIoDevice->PciBar[BarIndex].Length == 0) {
      PciIoDevice->PciBar[BarIndex].BarType = (PCI_BAR_TYPE) 0;
    }

    PciIoDevice->PciBar[BarIndex].BaseAddress   = OriginalValue & Mask;

  } else {

    Mask  = 0xfffffff0;

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
        BarType = PciBarTypePMem64;
#ifdef DOWNGRADE_PMEM64
        PciIoDevice->PciBar[BarIndex].BarType = PciBarTypePMem32;
#else
        PciIoDevice->PciBar[BarIndex].BarType = PciBarTypePMem64;
#endif
      } else {
        BarType = PciBarTypeMem64;
#ifdef DOWNGRADE_MEM64
        PciIoDevice->PciBar[BarIndex].BarType = PciBarTypeMem32;
#else
        PciIoDevice->PciBar[BarIndex].BarType = PciBarTypeMem64;
#endif
      }

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
  // Start to parse the bars
  //
  for (Offset = 0x10, BarIndex = 0; Offset <= 0x24 && BarIndex < PCI_MAX_BAR; BarIndex++) {
    Offset = PciParseBar (PciIoDevice, Offset, BarIndex);
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

  PciIoDevice = CreatePciIoDevice (
                  Bridge,
                  Pci,
                  Bus,
                  Device,
                  Func
                  );

  PciExpressWrite16 (PciIoDevice->Address + PCI_COMMAND_OFFSET, 0);
  PciExpressWrite16 (PciIoDevice->Address + PCI_BRIDGE_CONTROL_REGISTER_OFFSET, 0);

  //
  // PPB can have two BARs
  //
  if (PciParseBar (PciIoDevice, 0x10, PPB_BAR_0) == 0x14) {
    //
    // Not 64-bit bar
    //
    PciParseBar (PciIoDevice, 0x14, PPB_BAR_1);
  }

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
  PCI_IO_DEVICE *PciIoDevice;

  PciIoDevice = NULL;

#if DEBUG_PCI_ENUM
  DEBUG ((
           EFI_D_INFO,
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
  PCI_IO_DEVICE                     *PciDevice;

  SecondBus       = 0;
  Register        = 0;
  PciDevice       = NULL;

  for (Device = 0; Device <= PCI_MAX_DEVICE; Device++) {
    for (Func = 0; Func <= PCI_MAX_FUNC; Func++) {

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
        Register  = 0xFF;
        Address   = PCI_EXPRESS_LIB_ADDRESS (StartBusNumber, Device, Func, PCI_BRIDGE_SUBORDINATE_BUS_REGISTER_OFFSET);
        PciExpressWrite8 (Address, (UINT8)Register);

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
  Insert the given entry to the Sorted list.

  @param ListHead   pointer to the head of the list
  @param Entry      point at which the list is inserted.
 **/
VOID
EFIAPI
InsertSortedList (
  IN OUT  LIST_ENTRY                *ListHead,
  IN OUT  LIST_ENTRY                *Entry
  )
{
  LIST_ENTRY            *Curr;
  PCI_BAR_RESOURCE      *CurrResBar;
  PCI_BAR_RESOURCE      *ResBar;

  ResBar = PCI_BAR_RESOURCE_FROM_LINK (Entry);

  Curr = ListHead->ForwardLink;
  while (Curr != ListHead) {
    CurrResBar = PCI_BAR_RESOURCE_FROM_LINK (Curr);
    if (ResBar->PciBar->Alignment >= CurrResBar->PciBar->Alignment) {
      break;
    }
    Curr = Curr->ForwardLink;
  }

  Curr->BackLink->ForwardLink = Entry;
  Entry->ForwardLink = Curr;
  Entry->BackLink = Curr->BackLink;
  Curr->BackLink = Entry;
}

/**
 Program the Base address register with the base address of PCI devices.

 @param PciIoDevice  pointer to PCI IO device.
 @param BarIdx       BAR index to program the address.
 **/
VOID
ProgramBar (
  IN PCI_IO_DEVICE         *PciIoDevice,
  IN UINT32                              BarIdx
  )
{
  PCI_BAR_TYPE                        BarType;
  UINT64                              Address64;

  BarType = PciIoDevice->PciBar[BarIdx].OrgBarType;
  switch (BarType) {
  case PciBarTypeIo16:
  case PciBarTypeIo32:
  case PciBarTypeMem32:
  case PciBarTypePMem32:
    PciExpressWrite32 (PciIoDevice->Address + PciIoDevice->PciBar[BarIdx].Offset,
                       (UINT32)PciIoDevice->PciBar[BarIdx].BaseAddress);
    break;

  case PciBarTypeMem64:
  case PciBarTypePMem64:
    Address64 = PciIoDevice->PciBar[BarIdx].BaseAddress;
    PciExpressWrite32 (PciIoDevice->Address + PciIoDevice->PciBar[BarIdx].Offset, (UINT32)Address64);
    PciExpressWrite32 (PciIoDevice->Address + PciIoDevice->PciBar[BarIdx].Offset + 4, (UINT32)RShiftU64 (Address64, 32));
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

  CurrentLink = Parent->ChildList.ForwardLink;
  while (CurrentLink != NULL && CurrentLink != &Parent->ChildList) {
    PciIoDevice = PCI_IO_DEVICE_FROM_LINK (CurrentLink);
    for (Idx = 0; Idx < PCI_MAX_BAR; Idx++) {
      if (PciIoDevice->PciBar[Idx].Length && (PciIoDevice->PciBar[Idx].BarType == BarType)) {
        PciIoDevice->PciBar[Idx].BaseAddress += PciIoDevice->Parent->PciBar[BarType - 1].BaseAddress;
        if (PciIoDevice->PciBar[Idx].Offset >= 0x100) {
          // Bridge
          ProgramPpbApperture (PciIoDevice, Idx);
        } else {
          // Device
          ProgramBar (PciIoDevice, Idx);
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
  PCI_BAR_RESOURCE           *PciBarRes;
  UINT64                      Base;
  UINT64                      Alignment;

  ParentRes.PciBar = NULL;
  InitializeListHead (&ParentRes.Link);
  CurrentLink = Parent->ChildList.ForwardLink;
  while (CurrentLink != NULL && CurrentLink != &Parent->ChildList) {
    PciIoDevice = PCI_IO_DEVICE_FROM_LINK (CurrentLink);
    if (PciIoDevice->ChildList.ForwardLink != &PciIoDevice->ChildList) {
      CalculateResource (PciIoDevice, BarType);
    }
    for (Idx = 0; Idx < PCI_MAX_BAR; Idx++) {
      if (PciIoDevice->PciBar[Idx].BarType == BarType) {
        PciBarRes = (PCI_BAR_RESOURCE *)PciAllocatePool (sizeof (PCI_BAR_RESOURCE));
        PciBarRes->PciBar = &PciIoDevice->PciBar[Idx];
        InsertSortedList (&ParentRes.Link, &PciBarRes->Link);
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
    Base = ALIGN (Base,  Alignment);
  } else {
    if (Alignment < 0xFFFFF) {
      Alignment = 0xFFFFF;
    }
  }
  Base = ALIGN (Base,  Alignment);

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
    PciExpressWrite32 (PciIoDevice->Address + PCI_COMMAND_OFFSET, \
                       EFI_PCI_COMMAND_IO_SPACE | EFI_PCI_COMMAND_MEMORY_SPACE | EFI_PCI_COMMAND_BUS_MASTER);
    if (PciIoDevice->ChildList.ForwardLink != &PciIoDevice->ChildList) {
      EnablePciDevice (PciIoDevice);
    }
    CurrentLink = CurrentLink->ForwardLink;
  }
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
  if (Address >= 0x80000000) {
    DEBUG ((EFI_D_INFO, "PCI HOST: Bus(0x%02X-%02X)\n", 0, (UINT8) (Address & 0xFF)));
  } else {
    DEBUG ((EFI_D_INFO, "%aPCI(%02X,%02X,%02X)\n", Indent, (Address >> 20) & 0xFF, (Address >> 15) & 0x1F,
            (Address >> 12) & 0x07));
  }
  for (Idx = 0; Idx < PCI_MAX_BAR; Idx++) {
    if (PciIoDevice->PciBar[Idx].Length == 0) {
      continue;
    }
    DEBUG ((EFI_D_INFO, "%a  BAR[%d].TYP = %d\n", Indent,  Idx, PciIoDevice->PciBar[Idx].BarType));
    DEBUG ((EFI_D_INFO, "%a  BAR[%d].OFF = 0x%02X\n", Indent,  Idx, PciIoDevice->PciBar[Idx].Offset));
    DEBUG ((EFI_D_INFO, "%a  BAR[%d].BAS = %016X", Indent,  Idx, PciIoDevice->PciBar[Idx].BaseAddress));
    DEBUG ((EFI_D_INFO, "  BAR[%d].LEN = %016X",   Idx, PciIoDevice->PciBar[Idx].Length));
    DEBUG ((EFI_D_INFO, "  BAR[%d].ALN = %016X\n", Idx, PciIoDevice->PciBar[Idx].Alignment));
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
#endif

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
  UINT32                     Idx;
  UINT8                      SubBusNumber;
  PCI_IO_DEVICE             *Root;

  SetAllocationPool (MemPool);

  SubBusNumber = 0;
  Root = CreatePciIoDevice (NULL, NULL, 0, 0, 0);
  PciScanBus (Root, 0, &SubBusNumber, NULL);

  CalculateResource (Root, PciBarTypeIo16);
  CalculateResource (Root, PciBarTypeMem32);
  CalculateResource (Root, PciBarTypePMem32);

  for (Idx = 0; Idx < PCI_MAX_BAR; Idx++) {
    if (Idx == 0) { //IO16
      Root->PciBar[0].BaseAddress = PcdGet32 (PcdPciResourceIoBase);
    } else if (Idx == 2) { // MEM32
      Root->PciBar[2].BaseAddress = PcdGet32 (PcdPciResourceMem32Base);
    } else if (Idx == 3) { // PMEM32
      Root->PciBar[3].BaseAddress = ALIGN (Root->PciBar[2].BaseAddress + Root->PciBar[2].Length, Root->PciBar[3].Alignment);
    }
  }
  Root->Address = PcdGet32 (PcdPciResourceMem32Base) + SubBusNumber;

  ProgramResource (Root, PciBarTypeIo16);
  ProgramResource (Root, PciBarTypeMem32);
  ProgramResource (Root, PciBarTypePMem32);

  EnablePciDevice (Root);

#if DEBUG_PCI_ENUM
  DumpDeivceTree  (Root);
  DumpResourceBar (Root);
  DEBUG ((EFI_D_INFO, "MEM Pool Used: 0x%08X\n", (UINT32)GetAllocationPool() - (UINT32)MemPool));
#endif

  // Free memory pool
  SetAllocationPool (MemPool);

  return EFI_SUCCESS;
}
