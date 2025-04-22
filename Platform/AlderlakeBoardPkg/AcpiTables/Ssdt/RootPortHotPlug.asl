/**@file

  Copyright (c) 2021, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

      External(HOST_BUS.ROOT_PORT_OBJECT.HPSX, FieldUnitObj)
      External(HOST_BUS.ROOT_PORT_OBJECT.L0SE, FieldUnitObj)
      External(HOST_BUS.ROOT_PORT_OBJECT.PDSX, FieldUnitObj)
      External(HOST_BUS.ROOT_PORT_OBJECT.PDCX, FieldUnitObj)
      External(HOST_BUS.ROOT_PORT_OBJECT.VDID, FieldUnitObj)

      //
      // 1. Check if Root Port exists.
      // 2. Check if Root Port is enabled.
      // 2. Check for a Hot Plug Event.
      //
      If (CondRefOf (HOST_BUS.ROOT_PORT_OBJECT)) {
        If(LAnd(LNotEqual(HOST_BUS.ROOT_PORT_OBJECT.VDID,0xFFFFFFFF),HOST_BUS.ROOT_PORT_OBJECT.HPSX))
        {
          If(HOST_BUS.ROOT_PORT_OBJECT.PDCX)
          {
            // Clear all status bits first.
            Store(1,HOST_BUS.ROOT_PORT_OBJECT.PDCX)
            Store(1,HOST_BUS.ROOT_PORT_OBJECT.HPSX)

            If(LNot(HOST_BUS.ROOT_PORT_OBJECT.PDSX)) {
              //
              // The PCI Express slot is empty, so disable L0s on hot unplug
              //
              Store(0,HOST_BUS.ROOT_PORT_OBJECT.L0SE)
            }
            // Perform proper notification
            // to the OS.
            Notify(HOST_BUS.ROOT_PORT_OBJECT,0)
          } Else {
            // False event.  Clear Hot-Plug Status
            // then exit.
            Store(1,HOST_BUS.ROOT_PORT_OBJECT.HPSX)
          }
        }
      }
