/** @file

  Copyright (c) 2008 - 2015, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __PCI_ENUMERTION_LIB_H__
#define __PCI_ENUMERTION_LIB_H__

typedef enum {
  ///
  /// This notification is only applicable to PCI-PCI bridges and
  /// indicates that the PCI enumerator is about to begin enumerating
  /// the bus behind the PCI-PCI Bridge. This notification is sent after
  /// the primary bus number, the secondary bus number and the subordinate
  /// bus number registers in the PCI-PCI Bridge are programmed to valid
  /// (not necessary final) values
  ///
  EfiPciBeforeChildBusEnumeration,

  ///
  /// This notification is sent before the PCI enumerator probes BAR registers
  /// for every valid PCI function.
  ///
  EfiPciBeforeResourceCollection
} EFI_PCI_CONTROLLER_RESOURCE_ALLOCATION_PHASE;

//
// PCI BAR definitions
//
typedef enum {
  PciBarTypeUnknown = 0,
  PciBarTypeIo16,
  PciBarTypeIo32,
  PciBarTypeMem32,
  PciBarTypePMem32,
  PciBarTypeMem64,
  PciBarTypePMem64,
  PciBarTypeMaxType
} PCI_BAR_TYPE;

//
// PCI Resource Allocation structures
//
typedef struct {
  UINT8             BusBase;
  UINT8             BusLimit;
  UINT8             Segment;
  UINT8             Reserved;
  UINT32            IoBase;
  UINT32            IoLimit;
  UINT64            Mmio32Base;
  UINT64            Mmio32Limit;
  UINT64            Mmio64Base;
  UINT64            Mmio64Limit;
} PCI_RES_ALLOC_RANGE;

typedef struct {
  UINT32                        Signature;
  UINT16                        Version;
  UINT16                        Length;
  UINT8                         NumOfEntries;
  UINT8                         Reserved[3];
  PCI_RES_ALLOC_RANGE           ResourceRange[0];
} PCI_RES_ALLOC_TABLE;

typedef VOID   (EFIAPI *PLATFORM_PCI_ENUM_HOOK_PROC) (UINT8 Bus, UINT8 Dev, UINT8 Fun, EFI_PCI_CONTROLLER_RESOURCE_ALLOCATION_PHASE Phase);

#define MAX_HOST_BRIDGES  8

///
/// Per-host-bridge (PCI segment) configuration for enumeration.
///
typedef struct {
  UINT64    McfgBase;    ///< MCFG (ECAM) base address for this host bridge
  UINT8     Segment;     ///< PCI segment group number
  UINT8     BusBase;     ///< First bus in this host bridge's window
  UINT8     BusLimit;    ///< Last bus in this host bridge's window
  UINT8     Reserved;
} PCI_HOST_BRIDGE_INFO;

///
/// Table of host bridge descriptors, pointed to by PcdPciHostBridgeTableBase.
/// If not set, a single host bridge is derived from PcdPciExpressBaseAddress.
///
typedef struct {
  UINT8                  Count;
  UINT8                  Reserved[3];
  PCI_HOST_BRIDGE_INFO   HostBridge[MAX_HOST_BRIDGES];
} PCI_HOST_BRIDGE_TABLE;

/**
 Enumerates the PCI devices allocates the required memory resource.
 Program the allocated memory resource to PCI BAR.

 @param [in] MemPool point to memory pool to allocate for each PCI device.
 **/
EFI_STATUS
EFIAPI
PciEnumeration (
  IN  VOID   *MemPool
  );

#endif
