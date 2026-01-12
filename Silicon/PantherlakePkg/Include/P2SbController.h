/** @file
  P2SB controller

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#ifndef _P2SB_CONTROLLER_H_
#define _P2SB_CONTROLLER_H_

#define PTL_PCD_PID_GPIOCOM0                0x59
#define PTL_PCD_PID_CNVI                    0x51
#define PTL_PCD_PID_ICLK                    0x72

// IP: PCIe
//
#define PTL_SID_F2_PID_PXPA                     0xF201
#define PTL_SID_F2_PID_PXPB                     0xF202

//
// IP: PCIe
//
#define PTL_P_H_SID_F2_PID_PXPC                 0xF203
#define PTL_H_SID_F2_PID_PXPD                   0xF205
#define PTL_H_SID_F2_PID_PXPE                   0xF206
#define PTL_A_SID_F2_PID_PXPD                   0xF205
#define PTL_A_SID_F2_PID_PXPE                   0xF206

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
