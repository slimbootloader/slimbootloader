/** @file
  This file contains functions that initializes High Speed Phy

  Copyright (c) 2020 - 2021, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#ifndef __FW_LOAD_PLAT_H__
#define __FW_LOAD_PLAT_H__

#include <Uefi/UefiBaseType.h>

#define HSPHY_PAYLOAD_SIZE                          32 * 1024; // For ADL allocate 32KB of memory for Hsphy
#define CPU_SB_PID_PCIE_PHYX16_BROADCAST            0x55

typedef struct {
  UINT16               Count;
  union {
    UINT16             Address;
    UINT16             Type; // Valid only for Extended mode push model operations
  } Mode;
  UINT32               DataBlob[];
} IP_PUSH_MODEL;

typedef enum {
  HsPhyFwLoad = 1
} IpLoadRequest;

/**
  This function loads HsPhy Firmware via CSME HECI calls,
  authenticates the buffer after the loading completes
  and pushes HsPhy Firmware to Phy Controller

  @retval EFI_SUCCESS                On successfully pushing IP Firmware to Hsphy controller
  @retval Others                     Failed to load the HS PHY FW
**/
EFI_STATUS
HsPhyLoadAndInit (
  VOID
  );

/**
  Return CPU Sku

  @param[in]  UINT32             CpuFamilyModel
  @param[in]  UINT16             CpuDid

  @retval     UINT8              CPU Sku
**/
UINT8
GetCpuSkuInfo (
  IN UINT32 CpuFamilyModel,
  IN UINT16 CpuDid
  );

/**
  Return CPU Sku

  @retval UINT8              CPU Sku
**/
UINT8
EFIAPI
GetCpuSku (
  VOID
  );

#endif
