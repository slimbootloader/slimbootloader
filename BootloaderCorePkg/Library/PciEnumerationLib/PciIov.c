/** @file

  Copyright (c) 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Uefi/UefiBaseType.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/PciExpressLib.h>
#include "InternalPciEnumerationLib.h"
#include "PciCommand.h"

#define EFI_PCI_RID(Bus, Device, Function)  (((UINT32)Bus << 8) + ((UINT32)Device << 3) + (UINT32)Function)
#define EFI_PCI_BUS_OF_RID(RID)             ((UINT32)RID >> 8)

/**
  Parse PCI IOV VF bar information and fill them into PCI device instance.

  @param PciIoDevice  Pci device instance.
  @param Offset       Bar offset.
  @param BarIndex     Bar index.

  @return Next bar offset.

**/
UINTN
PciIovParseVfBar (
  IN PCI_IO_DEVICE  *PciIoDevice,
  IN UINTN          Offset,
  IN UINTN          BarIndex
  )
{
  UINT32                Value;
  UINT32                OriginalValue;
  UINT32                Mask;
  EFI_STATUS            Status;

  //
  // Ensure it is called properly
  //
  ASSERT (PciIoDevice->SrIovCapabilityOffset != 0);
  if (PciIoDevice->SrIovCapabilityOffset == 0) {
    return 0;
  }

  OriginalValue = 0;
  Value         = 0;

  Status = BarExisted (
            PciIoDevice,
            Offset,
            &Value,
            &OriginalValue
            );

  if (EFI_ERROR (Status)) {
    PciIoDevice->VfPciBar[BarIndex].BaseAddress = 0;
    PciIoDevice->VfPciBar[BarIndex].Length      = 0;
    PciIoDevice->VfPciBar[BarIndex].Alignment   = 0;

    //
    // Scan all the BARs anyway
    //
    PciIoDevice->VfPciBar[BarIndex].Offset = (UINT16) Offset;
    return Offset + 4;
  }

  PciIoDevice->VfPciBar[BarIndex].Offset = (UINT16) Offset;
  if ((Value & 0x01) != 0) {
    //
    // Device I/Os. Impossible
    //
    return Offset + 4;

  } else {

    Mask  = 0xfffffff0;

    PciIoDevice->VfPciBar[BarIndex].BaseAddress = OriginalValue & Mask;

    switch (Value & 0x07) {

    //
    //memory space; anywhere in 32 bit address space
    //
    case 0x00:
      if ((Value & 0x08) != 0) {
        PciIoDevice->VfPciBar[BarIndex].BarType = PciBarTypePMem32;
      } else {
        PciIoDevice->VfPciBar[BarIndex].BarType = PciBarTypeMem32;
      }

      PciIoDevice->VfPciBar[BarIndex].Length    = (~(Value & Mask)) + 1;
      PciIoDevice->VfPciBar[BarIndex].Alignment = PciIoDevice->VfPciBar[BarIndex].Length - 1;

      //
      // Adjust Length
      //
      PciIoDevice->VfPciBar[BarIndex].Length = MultU64x32 (PciIoDevice->VfPciBar[BarIndex].Length, PciIoDevice->InitialVFs);
      //
      // Adjust Alignment
      //
      if (PciIoDevice->VfPciBar[BarIndex].Alignment < PciIoDevice->SystemPageSize - 1) {
        PciIoDevice->VfPciBar[BarIndex].Alignment = PciIoDevice->SystemPageSize - 1;
      }

      break;

    //
    // memory space; anywhere in 64 bit address space
    //
    case 0x04:
      if ((Value & 0x08) != 0) {
        if ((PciIoDevice->Decodes & EFI_BRIDGE_PMEM64_DECODE_SUPPORTED) != 0) {
          PciIoDevice->VfPciBar[BarIndex].BarType = PciBarTypePMem64;
        } else {
          PciIoDevice->VfPciBar[BarIndex].BarType = PciBarTypePMem32;
        }
      } else {
        if ((PciIoDevice->Decodes & EFI_BRIDGE_MEM64_DECODE_SUPPORTED) != 0) {
          PciIoDevice->VfPciBar[BarIndex].BarType = PciBarTypeMem64;
        } else {
          PciIoDevice->VfPciBar[BarIndex].BarType = PciBarTypeMem32;
        }
      }

      //
      // According to PCI 2.2,if the bar indicates a memory 64 decoding, next bar
      // is regarded as an extension for the first bar. As a result
      // the sizing will be conducted on combined 64 bit value
      // Here just store the masked first 32bit value for future size
      // calculation
      //
      PciIoDevice->VfPciBar[BarIndex].Length    = Value & Mask;
      PciIoDevice->VfPciBar[BarIndex].Alignment = PciIoDevice->VfPciBar[BarIndex].Length - 1;

      if (PciIoDevice->VfPciBar[BarIndex].Alignment < PciIoDevice->SystemPageSize - 1) {
        PciIoDevice->VfPciBar[BarIndex].Alignment = PciIoDevice->SystemPageSize - 1;
      }

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
        return Offset + 4;
      }

      //
      // Fix the length to support some special 64 bit BAR
      //
      Value |= ((UINT32) -1 << HighBitSet32 (Value));

      //
      // Calculate the size of 64bit bar
      //
      PciIoDevice->VfPciBar[BarIndex].BaseAddress |= LShiftU64 ((UINT64) OriginalValue, 32);

      PciIoDevice->VfPciBar[BarIndex].Length    = PciIoDevice->VfPciBar[BarIndex].Length | LShiftU64 ((UINT64) Value, 32);
      PciIoDevice->VfPciBar[BarIndex].Length    = (~(PciIoDevice->VfPciBar[BarIndex].Length)) + 1;
      PciIoDevice->VfPciBar[BarIndex].Alignment = PciIoDevice->VfPciBar[BarIndex].Length - 1;

      //
      // Adjust Length
      //
      PciIoDevice->VfPciBar[BarIndex].Length = MultU64x32 (PciIoDevice->VfPciBar[BarIndex].Length, PciIoDevice->InitialVFs);
      //
      // Adjust Alignment
      //
      if (PciIoDevice->VfPciBar[BarIndex].Alignment < PciIoDevice->SystemPageSize - 1) {
        PciIoDevice->VfPciBar[BarIndex].Alignment = PciIoDevice->SystemPageSize - 1;
      }

      break;

    //
    // reserved
    //
    default:
      PciIoDevice->VfPciBar[BarIndex].BarType   = PciBarTypeUnknown;
      PciIoDevice->VfPciBar[BarIndex].Length    = (~(Value & Mask)) + 1;
      PciIoDevice->VfPciBar[BarIndex].Alignment = PciIoDevice->VfPciBar[BarIndex].Length - 1;

      if (PciIoDevice->VfPciBar[BarIndex].Alignment < PciIoDevice->SystemPageSize - 1) {
        PciIoDevice->VfPciBar[BarIndex].Alignment = PciIoDevice->SystemPageSize - 1;
      }

      break;
    }
  }

  //
  // Check the length again so as to keep compatible with some special bars
  //
  if (PciIoDevice->VfPciBar[BarIndex].Length == 0) {
    PciIoDevice->VfPciBar[BarIndex].BarType     = PciBarTypeUnknown;
    PciIoDevice->VfPciBar[BarIndex].BaseAddress = 0;
    PciIoDevice->VfPciBar[BarIndex].Alignment   = 0;
  } else {
    PciIoDevice->VfPciBar[BarIndex].OrgBarType = PciIoDevice->VfPciBar[BarIndex].BarType;
  }

  //
  // Increment number of bar
  //
  return Offset + 4;
}

/**
  Initialize SR-IOV (Single Root IO Virtualization)

  @param[in,out]  PciIoDevice     Pointer to instance of PCI_IO_DEVICE.
  @param[in]      Bus             Device Bus NO.
  @param[in]      Device          Device device NO.
  @param[in]      Func            Device func NO.

**/
VOID
EFIAPI
InitializeSrIov (
  IN OUT  PCI_IO_DEVICE              *PciIoDevice,
  IN      UINT8                       Bus,
  IN      UINT8                       Device,
  IN      UINT8                       Func
  )
{
  EFI_STATUS    Status;
  UINTN         Address;
  UINT32        SupportedPageSize;
  UINT16        VFStride;
  UINT16        FirstVFOffset;
  UINT16        Data16;
  UINT32        PFRid;
  UINT32        LastVF;

  Status = LocatePciExpressCapabilityRegBlock (
             PciIoDevice,
             EFI_PCIE_CAPABILITY_ID_SRIOV,
             &PciIoDevice->SrIovCapabilityOffset
             );

  if (!EFI_ERROR (Status)) {
    Address = PciIoDevice->Address + PciIoDevice->SrIovCapabilityOffset;

    //
    // If the SR-IOV device is an ARI device, then Set ARI Capable Hierarchy for the device.
    //
    if (FeaturePcdGet (PcdAriSupport) && PciIoDevice->AriCapabilityOffset != 0) {
      Data16  = PciExpressRead16 (Address + EFI_PCIE_CAPABILITY_ID_SRIOV_CONTROL);
      Data16 |= EFI_PCIE_CAPABILITY_ID_SRIOV_CONTROL_ARI_HIERARCHY;
      PciExpressWrite16 (Address + EFI_PCIE_CAPABILITY_ID_SRIOV_CONTROL, Data16);
    }

    //
    // Calculate SystemPageSize
    //
    SupportedPageSize = PciExpressRead32 (Address + EFI_PCIE_CAPABILITY_ID_SRIOV_SUPPORTED_PAGE_SIZE);
    PciIoDevice->SystemPageSize = (SupportedPageSize & BIT0); // BIT0: 4KB page
    ASSERT (PciIoDevice->SystemPageSize != 0);
    PciExpressWrite32 (Address + EFI_PCIE_CAPABILITY_ID_SRIOV_SUPPORTED_PAGE_SIZE, PciIoDevice->SystemPageSize);

    //
    // Adjust SystemPageSize for Alignment usage later
    //
    PciIoDevice->SystemPageSize <<= 12;

    //
    // Read First FirstVFOffset, InitialVFs, and VFStride
    //
    FirstVFOffset = PciExpressRead16 (Address + EFI_PCIE_CAPABILITY_ID_SRIOV_FIRSTVF);
    PciIoDevice->InitialVFs = PciExpressRead16 (Address + EFI_PCIE_CAPABILITY_ID_SRIOV_INITIALVFS);
    VFStride = PciExpressRead16 (Address + EFI_PCIE_CAPABILITY_ID_SRIOV_VFSTRIDE);

    //
    // Calculate LastVF
    //
    PFRid = EFI_PCI_RID (Bus, Device, Func);
    LastVF = PFRid + FirstVFOffset + (PciIoDevice->InitialVFs - 1) * VFStride;

    //
    // Calculate ReservedBusNum for this PF
    //
    PciIoDevice->ReservedBusNum = (UINT16)(EFI_PCI_BUS_OF_RID (LastVF) - Bus + 1);
  }
}
