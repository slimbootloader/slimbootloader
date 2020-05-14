/** @file

  Copyright (c) 2017 - 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __INTERNAL_PCI_ENUMERTION_LIB_H__
#define __INTERNAL_PCI_ENUMERTION_LIB_H__

#include <IndustryStandard/Pci.h>
#include <Guid/PciRootBridgeInfoGuid.h>

//
// The PCI Command register bits owned by PCI Bus driver.
//
// They should be cleared at the beginning. The other registers
// are owned by chipset, we should not touch them.
//
#define EFI_PCI_COMMAND_BITS_OWNED                          ( \
                EFI_PCI_COMMAND_IO_SPACE                    | \
                EFI_PCI_COMMAND_MEMORY_SPACE                | \
                EFI_PCI_COMMAND_BUS_MASTER                  | \
                EFI_PCI_COMMAND_MEMORY_WRITE_AND_INVALIDATE | \
                EFI_PCI_COMMAND_VGA_PALETTE_SNOOP           | \
                EFI_PCI_COMMAND_FAST_BACK_TO_BACK             \
                )

//
// The PCI Bridge Control register bits owned by PCI Bus driver.
//
// They should be cleared at the beginning. The other registers
// are owned by chipset, we should not touch them.
//
#define EFI_PCI_BRIDGE_CONTROL_BITS_OWNED                   ( \
                EFI_PCI_BRIDGE_CONTROL_ISA                  | \
                EFI_PCI_BRIDGE_CONTROL_VGA                  | \
                EFI_PCI_BRIDGE_CONTROL_VGA_16               | \
                EFI_PCI_BRIDGE_CONTROL_FAST_BACK_TO_BACK      \
                )

#define PPB_BAR_0                             0
#define PPB_BAR_1                             1

#define PCI_IO_DEVICE_FROM_LINK(a)       BASE_CR (a, PCI_IO_DEVICE, Link)
#define PCI_BAR_RESOURCE_FROM_LINK(a)    BASE_CR (a, PCI_BAR_RESOURCE, Link)
#define ALIGN(Base, Align)               ((Base + Align) & (~(Align)))

typedef enum {
  BusScanTypeList     = 0,
  BusScanTypeRange    = 1,
  BusScanTypeInvalid  = 0xFF
} BUS_SCAN_TYPE;

typedef struct {
  UINT8           DowngradeIo32;
  UINT8           DowngradeMem64;
  UINT8           DowngradePMem64;
  UINT8           Reserved;
  UINT8           BusScanType;
  UINT8           NumOfBus;
  UINT8           BusScanItems[0];
} PCI_ENUM_POLICY_INFO;

//
// PCI BAR parameters
//
typedef struct {
  UINT64        BaseAddress;
  UINT64        Length;
  UINT64        Alignment;
  PCI_BAR_TYPE  BarType;
  PCI_BAR_TYPE  OrgBarType;
  UINT16        Offset;
} PCI_BAR;

typedef struct _PCI_BAR_RESOURCE           PCI_BAR_RESOURCE;

struct _PCI_BAR_RESOURCE {
  LIST_ENTRY                                Link;
  PCI_BAR                                  *PciBar;
};

typedef struct _PCI_IO_DEVICE              PCI_IO_DEVICE;

struct _PCI_IO_DEVICE {
  LIST_ENTRY                                Link;

  //
  // Bus number, Device number, Function number
  //
  UINT32                                    Address;

  //
  // PCI configuration space header type
  //
  PCI_TYPE00                                Pci;

  //
  // BAR for this PCI Device
  //
  PCI_BAR                                   PciBar[PCI_MAX_BAR];

  //
  // ARI (Alternative Routing ID)
  //
  BOOLEAN                                   IsPciExp;
  UINT8                                     PciExpressCapabilityOffset;
  UINT32                                    AriCapabilityOffset;
  //
  // SR-IOV
  //
  UINT32                                    SrIovCapabilityOffset;
  PCI_BAR                                   VfPciBar[PCI_MAX_BAR];
  UINT32                                    SystemPageSize;
  UINT16                                    InitialVFs;
  UINT16                                    ReservedBusNum;

  //
  // The bridge device this pci device is subject to
  //
  PCI_IO_DEVICE                             *Parent;

  //
  // A linked list for children Pci Device if it is bridge device
  //
  LIST_ENTRY                                ChildList;

};

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
EFIAPI
BarExisted (
  IN  PCI_IO_DEVICE *PciIoDevice,
  IN  UINTN         Offset,
  OUT UINT32        *BarLengthValue,
  OUT UINT32        *OriginalBarValue
  );

#endif
