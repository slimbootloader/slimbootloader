/** @file
  This file defines the hob structure for the PCI Root Bridge Info.

  Copyright (c) 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __PCI_ROOT_BRIDGE_INFO_GUID_H__
#define __PCI_ROOT_BRIDGE_INFO_GUID_H__

#include <IndustryStandard/Pci.h>

///
/// PCI Root Bridge Information GUID
///
extern EFI_GUID gLoaderPciRootBridgeInfoGuid;

typedef enum {
  PciBarTypeUnknown = 0,
  PciBarTypeIo16,
  PciBarTypeIo32,
  PciBarTypeMem32,
  PciBarTypePMem32,
  PciBarTypeMem64,
  PciBarTypePMem64,
  PciBarTypeIo,
  PciBarTypeMem,
  PciBarTypeMaxType
} PCI_BAR_TYPE;

//
// PCI Root Bridge Resource Info
//
typedef struct {
  UINT64                    ResBase;
  UINT64                    ResLength;
} PCI_ROOT_BRIDGE_RESOURCE;

typedef struct {
  UINT8                     BusBase;
  UINT8                     BusLimit;
  UINT8                     Reserved[2];
  PCI_ROOT_BRIDGE_RESOURCE  Resource[PCI_MAX_BAR];  // PCI_BAR_TYPE - 1
} PCI_ROOT_BRIDGE_ENTRY;

typedef struct {
  UINT8                     Revision;
  UINT8                     Reserved[2];
  UINT8                     Count;
  PCI_ROOT_BRIDGE_ENTRY     Entry[0];
} PCI_ROOT_BRIDGE_INFO_HOB;

#endif // __PCI_ROOT_BRIDGE_INFO_GUID_H__
