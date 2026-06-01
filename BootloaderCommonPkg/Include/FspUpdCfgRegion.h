/** @file
  Helpers to locate the FSP UPD config region (the part of the UPD that lives
  past the FSPx_UPD_COMMON* header). The size of the common header depends on
  the FSP spec version reported in FSP_INFO_HEADER.SpecVersion (BCD encoded,
  e.g. 0x24 == FSP 2.4).

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#ifndef _FSP_UPD_CFG_REGION_H_
#define _FSP_UPD_CFG_REGION_H_

#include <FspEas/FspApi.h>
#include <Guid/FspHeaderFile.h>

/**
  Offset (in bytes) from the start of the FSP-M UPD buffer to the beginning of
  the config region (i.e. the size of the FSPM_UPD_COMMON* header for the
  running FSP image).
**/
static inline
UINTN
GetFspmUpdCfgRegionOffset (
  IN CONST FSP_INFO_HEADER  *FspHeader
  )
{
  if (FspHeader->SpecVersion >= 0x24) {
    return sizeof (FSPM_UPD_COMMON_FSP24);
  }
  return sizeof (FSPM_UPD_COMMON);
}

/**
  Offset (in bytes) from the start of the FSP-S UPD buffer to the beginning of
  the config region (i.e. the size of the FSPS_UPD_COMMON* header for the
  running FSP image).
**/
static inline
UINTN
GetFspsUpdCfgRegionOffset (
  IN CONST FSP_INFO_HEADER  *FspHeader
  )
{
  if (FspHeader->SpecVersion >= 0x24) {
    return sizeof (FSPS_UPD_COMMON_FSP24);
  }
  if (FspHeader->SpecVersion >= 0x22) {
    return sizeof (FSPS_UPD_COMMON_FSP22);
  }
  return sizeof (FSPS_UPD_COMMON);
}

#endif
