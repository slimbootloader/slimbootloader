/** @file

  This file implements an API to get IOAT/PXP subsystem information.

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

  **/

#include <Base.h>
#include <Uefi.h>
#include "SiSubsysInfoLib.h"
#include "Stage2BoardInitLib.h"

/**
  Get Max subsystem number in Socket.

  @param[in]  Socket          - Socket index.
  @param[in]  SubsysType      - subsystem type.

  @return Max subsystem number.

**/
UINT8
GetMaxSubsys (
  UINT8                     Socket,
  UINT8                     SubsysType
)
{
  UINT8     MaxSubsys;

  switch (SubsysType) {
  case SUBSYS_TYPE_IOAT:
    MaxSubsys = MAX_IIO_IOAT_PER_SOCKET;
    break;
  case SUBSYS_TYPE_PXP:
    MaxSubsys = MAX_IIO_PCIE_PER_SOCKET;
    break;
  default:
    MaxSubsys = 0;
    break;
  }

  return MaxSubsys;
}

/**
  Get subsystem information.

  @param[in]  Socket          - Socket index.
  @param[in]  Inst            - Instance in socket.
  @param[in]  SubsysType      - subsystem type.
  @param[out] SubsysInfoEntry - Pointer to the subsystem information.

  @return True if subsystem information was found, false otherwise.

**/
BOOLEAN
GetSubsysInfo (
  UINT8                     Socket,
  UINT8                     Inst,
  UINT8                     SubsysType,
  SUBSYS_INFO_ENTRY         *SubsysInfoEntry
)
{
  VOID                      *FspHobList;
  EFI_HOB_GUID_TYPE         *GuidHob;
  SYS_INFO                  *SysInfo;

  if (Inst >= GetMaxSubsys (Socket, SubsysType)) {
    return FALSE;
  }

  GuidHob = NULL;
  FspHobList = GetFspHobListPtr ();
  if (FspHobList != NULL) {
    GuidHob = GetNextGuidHob (&gSubsysInfoHobGuid, FspHobList);
  }
  if (GuidHob == NULL) {
    DEBUG ((DEBUG_ERROR, "Sys info Hob not found!\n"));
    ASSERT (FALSE);
    return FALSE;
  }

  SysInfo = GET_GUID_HOB_DATA (GuidHob);

  SysInfo = (SYS_INFO *)GET_GUID_HOB_DATA (GuidHob);

  switch (SubsysType) {
  case SUBSYS_TYPE_IOAT:
    SubsysInfoEntry->Data32[0] = SysInfo->Socket[Socket].IoatInfoEntrys[Inst].Data32[0];
    SubsysInfoEntry->Data32[1] = SysInfo->Socket[Socket].IoatInfoEntrys[Inst].Data32[1];
    break;
  case SUBSYS_TYPE_PXP:
    SubsysInfoEntry->Data32[0] = SysInfo->Socket[Socket].PxpInfoEntrys[Inst].Data32[0];
    SubsysInfoEntry->Data32[1] = SysInfo->Socket[Socket].PxpInfoEntrys[Inst].Data32[1];
    break;
  default:
    SubsysInfoEntry->Data32[0] = 0;
    SubsysInfoEntry->Data32[1] = 0;
    break;
  }
  return TRUE;
}
