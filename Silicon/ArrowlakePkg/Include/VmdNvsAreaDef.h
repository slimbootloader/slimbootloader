/**@file

  Copyright (c) 2024, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

  //
  // Define VMD NVS Area operation region.
  //
#ifndef _VMD_NVS_AREA_DEF_H_
#define _VMD_NVS_AREA_DEF_H_

#pragma pack (push,1)
typedef struct {
  UINT8    VmdEnable;                               ///< Offset 0       VMD Device Enable
  UINT32   VmdSocPciePorts;                         ///< Offset 1       VMD SOC and IOE PCIe RP mapped under VMD
  UINT32   VmdPchPcieRp;                            ///< Offset 5       VMD PCH PCIe RP mapped under VMD
  UINT8    VmdSataPort0to7;                         ///< Offset 9       VMD SATA PORT 0 to 7 mapped under VMD
  UINT8    PchRpBusNo;                              ///< Offset 10      PCH ROOT PORT BUS Number
} VMD_NVS_AREA;

#pragma pack(pop)
#endif
