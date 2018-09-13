/** @file

  Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef __INTERNAL_PCI_ENUMERTION_LIB_H__
#define __INTERNAL_PCI_ENUMERTION_LIB_H__

#include <IndustryStandard/Pci.h>

#define PPB_BAR_0                             0
#define PPB_BAR_1                             1

#define BASE_CR(Record, TYPE, Field)     ((TYPE *) ((CHAR8 *) (Record) - (CHAR8 *) &(((TYPE *) 0)->Field)))
#define PCI_IO_DEVICE_FROM_LINK(a)       BASE_CR (a, PCI_IO_DEVICE, Link)
#define PCI_BAR_RESOURCE_FROM_LINK(a)    BASE_CR (a, PCI_BAR_RESOURCE, Link)
#define ALIGN(Base, Align)               ((Base + Align) & (~(Align)))

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
  // The bridge device this pci device is subject to
  //
  PCI_IO_DEVICE                             *Parent;

  //
  // A linked list for children Pci Device if it is bridge device
  //
  LIST_ENTRY                                ChildList;

};

#endif
