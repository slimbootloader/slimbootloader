/**@file
  Intel ACPI Sample Code for Connectivity Modules

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include <Include/CnvRegisters.h>

External (\GMIO, MethodObj)
External (RPXX, OpRegionObj)
External (FLDR, OpRegionObj)
External (PBRR, OpRegionObj)
External (PCIE_ROOT_PORT.PBNU, IntObj)
External (WDCO, IntObj)

If (LEqual (CondRefOf (RPXX), 0)) {
  If (LAnd (CondRefOf (\GMIO), CondRefOf (PCIE_ROOT_PORT.PBNU))) {
    If (LAnd (CondRefOf (PCIE_ROOT_PORT._ADR), CondRefOf (PCIE_ROOT_PORT.PXSX._ADR))) {
      OperationRegion (RPXX, SystemMemory, GMIO (PCIE_ROOT_PORT.PBNU, PCIE_ROOT_PORT._ADR, PCIE_ROOT_PORT.PXSX._ADR), 0x30)
      Field (RPXX, AnyAcc, NoLock, Preserve)
      {
        Offset (0),     // Vendor-Device ID
        VDID, 32,
        Offset (0x2C),  // SVID
        SVID, 16,
      }
    }
  }
}
If (LEqual (CondRefOf (FLDR), 0)) {
  If (LAnd (CondRefOf (\GMIO), CondRefOf (PCIE_ROOT_PORT.PBNU))) {
    If (LAnd (CondRefOf (PCIE_ROOT_PORT._ADR), CondRefOf (PCIE_ROOT_PORT.PXSX._ADR))) {
      If (CondRefOf (WDCO)) {
        OperationRegion (FLDR, SystemMemory, Add (GMIO (PCIE_ROOT_PORT.PBNU, PCIE_ROOT_PORT._ADR, PCIE_ROOT_PORT.PXSX._ADR), WDCO), 0x06)
        Field (FLDR, ByteAcc, NoLock, Preserve)
        {
          DCAP, 32,       // Device Capabilities Reg
          DCTR, 16,       // Device Control Device Status
        }
      }
    }
  }
}

If (LEqual (CondRefOf (PBRR), 0)) {
  If (LAnd (CondRefOf (\GMIO), CondRefOf (PCIE_ROOT_PORT.PBNU))) {
    If (LAnd (CondRefOf (PCIE_ROOT_PORT._ADR), CondRefOf (PCIE_ROOT_PORT.PXSX._ADR))) {
      OperationRegion (PBRR, SystemMemory, GMIO (PCIE_ROOT_PORT.PBNU, PCIE_ROOT_PORT._ADR, PCIE_ROOT_PORT.PXSX._ADR), 0x2B8)
      Field (PBRR, AnyAcc, NoLock, Preserve)
      {
        Offset (CNVD_MAX_NO_SNOOP_LATENCY_REG_OFFSET),
        MSNL, 32,         // Max No-Snoop Latency, Max Snoop Latency
        Offset (CNVD_L1_PM_SUB_STATES_CONTROL1_REG_OFFSET),
        L1C1, 32,         // L1 PM Substates Control 1 Register
        Offset (CNVD_L1_PM_SUB_STATES_CONTROL2_REG_OFFSET),
        L1C2, 32,         // L1 PM Substates Control 2 Register
        Offset (CNVD_PTM_REG_OFFSET),
        PTMR, 32,         // PTM
      }
    }
  }
}