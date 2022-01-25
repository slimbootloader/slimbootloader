/**@file

  ACPI D3Cold/D3hot support for Storage device connected to PCIE Root Port

  Copyright (c) 2021, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

      External(PCIE_ROOT_PORT.PXSX.PNVM, MethodObj, IntObj)
      External(PCIE_ROOT_PORT.PXSX.PAHC, MethodObj, IntObj)
      External(PCIE_ROOT_PORT.VDID, FieldUnitObj)

      //
      // 1. Check if Root Port exists.
      // 2. Check if Root Port is mapped under VMD.
      // 2. Check for a storage device.
      //
      If (CondRefOf(PCIE_ROOT_PORT)) {
        If(LNotEqual(PCIE_ROOT_PORT.VDID,0xFFFFFFFF)) {
          If (CondRefOf(\STD3)) {
            If (CondRefOf(PCIE_ROOT_PORT.PRMV)) {
              If (LEqual (PCIE_ROOT_PORT.PRMV, 1)) {
                Store(\STD3, PCIE_ROOT_PORT.RD3C)
              }
            }

            If(LOr(PCIE_ROOT_PORT.PXSX.PNVM(),PCIE_ROOT_PORT.PXSX.PAHC())) {
              Store(\STD3, PCIE_ROOT_PORT.RD3C)
            }
          }
        }
      }
