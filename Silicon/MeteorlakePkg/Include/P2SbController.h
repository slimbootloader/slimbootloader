/** @file
  P2SB controller

  Copyright (c) 2020 - 2023, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#ifndef _P2SB_CONTROLLER_H_
#define _P2SB_CONTROLLER_H_

#define MTL_SOC_PID_GPIOCOM0   0xD1

/**
  P2SB structure
  Stores information required to access to registers
  like base address, S:B:D:F etc.
  and definitions specific to P2SB.
**/
typedef struct {
  /**
    P2SB controller PCI config space address
    in PCI Segment Library representation.
  **/
  UINT64                      PciCfgBaseAddr;
  /**
    P2SB controller MMIO base address
  **/
  UINT64                      Mmio;
  /**
    HPET MMIO base address
  **/
  UINT64                      HpetMmio;
} P2SB_CONTROLLER;

#endif // _P2SB_CONTROLLER_H_
