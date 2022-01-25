/**@file

  ACPI D3Cold/D3hot support for Storage device connected to CPU PCIE Root Port

  Copyright (c) 2021, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

      External(CPU_PCIE_ROOT_PORT.PEGP.PNVM, MethodObj, IntObj)
      External(CPU_PCIE_ROOT_PORT.PEGP.PAHC, MethodObj, IntObj)
      External(CPU_PCIE_ROOT_PORT.VDID, FieldUnitObj)

      //
      // 1. Check if Root Port exists.
      // 2. Check if Root Port is mapped under VMD.
      // 2. Check for a storage device.
      //
      If (CondRefOf(CPU_PCIE_ROOT_PORT)) {
        If(LNotEqual(CPU_PCIE_ROOT_PORT.VDID,0xFFFFFFFF)) {
          If (CondRefOf(\STD3)) {
            If (CondRefOf(CPU_PCIE_ROOT_PORT.PRMV)) {
              If (LEqual (CPU_PCIE_ROOT_PORT.PRMV, 1)) {
                Store(\STD3, CPU_PCIE_ROOT_PORT.RD3C)
              }
            }

            If(LOr(CPU_PCIE_ROOT_PORT.PEGP.PNVM(),CPU_PCIE_ROOT_PORT.PEGP.PAHC())) {
              Store(\STD3, CPU_PCIE_ROOT_PORT.RD3C)
            }
          }
        }
      }
