/** @file

  Copyright (c) 2024, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "Stage2BoardInitLib.h"
#include <GpioPinsVer4S.h>
#include <GpioPinsVer2Lp.h>

#define GPIO_CFG_PIN_TO_PAD(A) \
  ((UINT32) (((A).PinNumber) | (((A).PadGroup) << 16)))

#define PCH_RESERVED_ABOVE_4G_BASE_ADDRESS  0x3FFC0000000  ///< Pch preserved MMIO base address

/**
  Platform specific Pre Pci Enumeration

**/
VOID
PlatformPrePciEnumeration (
  VOID
  )
{
  PCI_RES_ALLOC_TABLE       *ResAllocTable;
  PCI_RES_ALLOC_RANGE       *SocAllocRange;
  PCI_RES_ALLOC_RANGE       *PchAllocRange;
  PCI_RES_ALLOC_RANGE       *AllocRange;
  UINT8                     RootBrgIndex;
  EFI_HOB_GUID_TYPE         *GuidHob;
  IIO_RES_HOB               *IioResHob;
  VOID                      *FspHobList;

  if (!IsPchS()) {
    //only ARL-S needs 2nd root bridge for PCH resources.
    return;
  }

  // Get PCI Root Bridge resources from FSP HOB
  GuidHob = NULL;
  FspHobList = GetFspHobListPtr ();
  if (FspHobList != NULL) {
    GuidHob = GetNextGuidHob (&gIioResHobGuid, FspHobList);
  }
  if (GuidHob == NULL) {
    DEBUG((DEBUG_INFO, "IIO Resource HOB not found!\n"));
    return;
  }

  IioResHob = (IIO_RES_HOB*)(GuidHob + 1);

  ResAllocTable = AllocateZeroPool(sizeof(PCI_RES_ALLOC_TABLE)+2*sizeof(PCI_RES_ALLOC_RANGE));
  if (ResAllocTable == NULL) {
    return;
  }

  SocAllocRange = &ResAllocTable->ResourceRange[0];
  SocAllocRange->BusBase      = IioResHob->SocBusBase;
  SocAllocRange->BusLimit     = IioResHob->SocBusLimit;
  SocAllocRange->Reserved     = 0;
  SocAllocRange->IoBase       = IioResHob->SocIoBase;
  SocAllocRange->IoLimit      = IioResHob->SocIoLimit;
  SocAllocRange->Mmio32Base   = IioResHob->SocReservedMmioBase;
  SocAllocRange->Mmio32Limit  = IioResHob->SocReservedMmioLimit;
  SocAllocRange->Mmio64Base   = IioResHob->SocReservedMmio64Base;
  SocAllocRange->Mmio64Limit  = IioResHob->SocReservedMmio64Limit;

  ResAllocTable->NumOfEntries++;

  PchAllocRange = &ResAllocTable->ResourceRange[1];
  PchAllocRange->BusBase = IioResHob->PchBusBase;
  PchAllocRange->BusLimit = IioResHob->PchBusLimit;
  PchAllocRange->IoBase = IioResHob->PchIoBase;
  PchAllocRange->IoLimit = IioResHob->PchIoLimit;
  PchAllocRange->Mmio32Base = IioResHob->PchReservedMmioBase;
  PchAllocRange->Mmio32Limit =IioResHob->PchReservedMmioLimit;
  PchAllocRange->Mmio64Base = IioResHob->PchReservedMmio64Base;
  PchAllocRange->Mmio64Limit = IioResHob->PchReservedMmio64Limit;

  ResAllocTable->NumOfEntries++;

  for (RootBrgIndex = 0 ; RootBrgIndex < ResAllocTable->NumOfEntries ; RootBrgIndex++) {
    DEBUG ((DEBUG_INFO, "-- Resource Alloc Entry-%d Info --\n", RootBrgIndex));
    AllocRange = &ResAllocTable->ResourceRange[RootBrgIndex];
    DEBUG ((DEBUG_INFO, "  BusBase    : 0x%02X\n", AllocRange->BusBase));
    DEBUG ((DEBUG_INFO, "  BusLimit   : 0x%02X\n", AllocRange->BusLimit));
    DEBUG ((DEBUG_INFO, "  IoBase     : 0x%X\n", AllocRange->IoBase));
    DEBUG ((DEBUG_INFO, "  IoLimit    : 0x%X\n", AllocRange->IoLimit));
    DEBUG ((DEBUG_INFO, "  Mmio32Base : 0x%X\n", AllocRange->Mmio32Base));
    DEBUG ((DEBUG_INFO, "  Mmio32Limit: 0x%X\n", AllocRange->Mmio32Limit));
    DEBUG ((DEBUG_INFO, "  Mmio64Base : 0x%llX\n", AllocRange->Mmio64Base));
    DEBUG ((DEBUG_INFO, "  Mmio64Limit: 0x%llX\n\n", AllocRange->Mmio64Limit));
  }

  (VOID) PcdSet32S (PcdPciResAllocTableBase, (UINT32)(UINTN)ResAllocTable);
}


/**
  Update current boot Payload ID.

**/
VOID
UpdatePayloadId (
  VOID
  )
{
  EFI_STATUS        Status = EFI_SUCCESS;
  UINT32            PayloadId;
  GEN_CFG_DATA      *GenericCfgData;
  PLDSEL_CFG_DATA   *PldSelCfgData;
  UINT32            PayloadSelGpioData;
  UINT32            PayloadSelGpioPad;
  UINT8             CmosData;

  PayloadSelGpioData = 0;

  PayloadId = GetPayloadId ();
  GenericCfgData = (GEN_CFG_DATA *)FindConfigDataByTag (CDATA_GEN_TAG);
  if (GenericCfgData != NULL) {
    if (GenericCfgData->PayloadId == AUTO_PAYLOAD_ID_SIGNATURE) {
      PayloadId = 0;
    } else {
      //
      // If PayloadId is not 'AUTO' then we should use the value
      // provided by CFG data; might want to always use UEFI or
      // OS Loader.
      //
      PayloadId = GenericCfgData->PayloadId;
      SetPayloadId (PayloadId);
      return;
    }
  }

  //
  // Use CMOS offset 0x20 (use as magic check) and 0x21 (payloadId) for payload detection.
  //
  // Read CMOS offset 20 value
  IoWrite8 (CMOS_EXTENDED_ADDREG, CMOS_EXTENDED_OFFSET_20);
  CmosData = IoRead8 (CMOS_EXTENDED_DATAREG);
  DEBUG ((DEBUG_INFO, "CMOS boot Magic [0x%x]\n", CmosData));

  if (CmosData == CMOS_VALUE_SWITCH_PLD) {
    // Read payload target
    IoWrite8 (CMOS_EXTENDED_ADDREG, CMOS_EXTENDED_OFFSET_21);
    CmosData  = IoRead8 (CMOS_EXTENDED_DATAREG);
    DEBUG ((DEBUG_INFO, "CMOS boot target [0x%x]\n", CmosData));

    switch (CmosData) {
      case 0:
        SetPayloadId (0);
        break;
      case 1:
        SetPayloadId (UEFI_PAYLOAD_ID_SIGNATURE);
        break;
      case 2:
        SetPayloadId (LINX_PAYLOAD_ID_SIGNATURE);
        break;
      default:
       SetPayloadId (0);
       break;
    }

    return;
  }

  //
  // Switch payloads based on configured GPIO pin
  //
  PldSelCfgData = (PLDSEL_CFG_DATA *)FindConfigDataByTag (CDATA_PLDSEL_TAG);
  if ((PldSelCfgData != NULL) && (PldSelCfgData->PldSelGpio.Enable != 0)){
    if (IsPchS ()) {
      PayloadSelGpioPad = GPIO_CFG_PIN_TO_PAD(PldSelCfgData->PldSelGpio) | (GPIO_VER4_S_CHIPSET_ID << 24);
    } else if (IsPchP ()) {
      PayloadSelGpioPad = GPIO_CFG_PIN_TO_PAD(PldSelCfgData->PldSelGpio) | (GPIO_VER2_LP_CHIPSET_ID << 24);
    } else {
      DEBUG ((DEBUG_WARN, "Unknown PCH in UpdatePayloadId\n"));
      return;
    }
    Status = GpioGetInputValue (PayloadSelGpioPad, &PayloadSelGpioData);
    if (!EFI_ERROR (Status)) {
      if (PayloadSelGpioData != PldSelCfgData->PldSelGpio.PinPolarity) {
        PayloadId = UEFI_PAYLOAD_ID_SIGNATURE;
      } else {
        PayloadId = 0;
      }
      DEBUG ((DEBUG_INFO, "Set PayloadId to 0x%08X based on GPIO config\n", PayloadId));
    }
  }

  SetPayloadId (PayloadId);
}
