/** @file
  This file contains definitions of PCIe controller information

  Copyright (c) 2024, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#ifndef _PCIE_RP_LIB_
#define _PCIE_RP_LIB_

#include <PcieConfig.h>

typedef struct {
  UINT32 MaxSnoopLatencyValue         : 10;
  UINT32 MaxSnoopLatencyScale         : 3;
  UINT32 MaxSnoopLatencyRequirement   : 1;
  UINT32 MaxNoSnoopLatencyValue       : 10;
  UINT32 MaxNoSnoopLatencyScale       : 3;
  UINT32 MaxNoSnoopLatencyRequirement : 1;
  UINT32 ForceOverride                : 1;
  UINT32 Reserved                     : 7;
} LTR_OVERRIDE;

/**
  Get PCIe port number for enabled Root Port.

  @param[in] RpBase    Root Port pci segment base address

  @retval Root Port number (1 based)
**/
UINT32
EFIAPI
PciePortNum (
  IN     UINT64  RpBase
  );

/**
  Get PCIe root port index
  @param[in] RpBase    Root Port pci segment base address
  @return Root Port index (0 based)
**/
UINT32
EFIAPI
PciePortIndex (
  IN     UINT64  RpBase
  );

/**
  This function checks whether PHY lane power gating is enabled on the port.

  @param[in] RpBase                 Root Port pci segment base address

  @retval TRUE                      PHY power gating is enabled
  @retval FALSE                     PHY power gating disabled
**/
BOOLEAN
EFIAPI
PcieIsPhyLanePgEnabled (
  IN     UINT64  RpBase
  );

/**
  Configures LTR override in Root Port's proprietary registers.

  @param[in] RpBase                         Root Port pci segment base address
  @param[in] DevNum                         currently visited device number
  @param[in] TreeLtr                        combination of LTR override values from all devices under this Root Port
  @param[in] LtrConfig                      Root Port LTR configuration

**/
VOID
EFIAPI
ConfigureRpLtrOverride (
  UINT64           RpBase,
  UINT32           DevNum,
  LTR_OVERRIDE     *TreeLtr,
  PCIE_LTR_CONFIG  *LtrConfig
  );

/**
  Configures proprietary parts of L1 substates configuration in Root Port

  @param[in] RpBase       Root Port pci segment base address
  @param[in] LtrCapable   TRUE if Root Port is LTR capable
  @param[in] L1LowCapable   TRUE if Root Port is L1 Low capable

**/
VOID
EFIAPI
L1ssProprietaryConfiguration (
  UINT64  RpBase,
  BOOLEAN LtrCapable,
  BOOLEAN L1LowCapable
  );
#endif
