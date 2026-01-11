/** @file
  Intel ACPI Reference Code for LCH Driver

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

External (\_SB.PC00.HCID, MethodObj)

Scope(\_SB.PC00) {

  Device(CVSS) {
    ADBG ("CVSS Device Entry")
    Name (_UID, "CVSS")
    Name(_DDN, "Intel CVS Device")

    //
    //This object is used to supply OSPM with the devices PNP ID or ACPI ID.
    //
    Method (_HID) {
      Return ("INTC10E1")
    }

    Name (IICF, 0) // 1: Shared I2C, 0: Dedicated I2C
    Method (IICS, 0, NotSerialized) {
      Return (IICF)
    }

    Method (SID,1, Serialized) {
      Return(\_SB.PC00.HCID(1))
    }

    Name(PKG0, Package() {"\\_SB.PC00.XHCI.RHUB.HS03.VGPO", "\\_SB.PC00.XHCI.RHUB.HS03.VIC0"})

    Method (_DEP, 0, NotSerialized) { // _DEP: Operation Region Dependencies
      If(LEqual (LCHS, 1)) {
        Return (PKG0)
      }
      Return (Package() {})
    }

    Method (_CRS, 0, Serialized) { // _CRS: Current Resource Settings
      //
      // Host Interrupt
      //
      Store (G_IN (LDGP, GPIO_INT_MOD_EDGE, GPIO_INT_POL_ACTIVE_LOW, GPIO_INT_SHARED, GPIO_PPI_PULL_DEFAULT, 0), Local1)

      //
      // CV Chip Reset Pin
      //
      Store (G_IO (LRGP, GPIO_IO_SHR_EXCLUSIVE , GPIO_PPI_PULL_DEFAULT, 0, 0, GPIO_IO_IOR_OUTPUT_ONLY), Local2)
      ConcatenateResTemplate (Local1, Local2, Local0)

      Name (VGB1, ResourceTemplate () {
        //
        // Virtual GPIOs from USB chip - To support GPIO interrupt.
        //
        GpioIo (Exclusive, PullUp, 0, 0, IoRestrictionOutputOnly, "\\_SB.PC00.XHCI.RHUB.HS03.VGPO",,,,) { 0x00}

        GpioIo (Exclusive, PullUp, 0, 0, IoRestrictionInputOnly, "\\_SB.PC00.XHCI.RHUB.HS03.VGPO",,,,) { 0x07}
      })

      //
      // Virtual I2C Resource to support CVS Camera
      //
      Name (IIC1, ResourceTemplate () {
              I2cSerialBusV2 (0x0000, ControllerInitiated, 0x400000,
                  AddressingMode7Bit, "\\_SB.PC00.XHCI.RHUB.HS03.VIC0",
                  0x00, ResourceConsumer, DEV1, Exclusive,
                  )
      })
      CreateWordField (IIC1, DEV1._ADR, DAD1)  // _ADR: Address
      DAD1 = 0x76
      Return(ConcatenateResTemplate(ConcatenateResTemplate(Local0, VGB1), IIC1))
    }
  } // End of CVSS(Computer Vision Sensing Support) Device
} // End of Scope (\_SB.PC00)

If (CondRefOf (\_SB.PC00.XHCI.RHUB.HS03)) {
  Scope (\_SB.PC00.XHCI.RHUB.HS03) {
    //
    // Virtual GPIO Device to access the GPIO pins from ucontroller interfaced with CVS.
    //
    Device (VGPO) {
      ADBG ("VGPO Virutal GPIO Device Entry")
      Name (_UID, "VGPO")
      Name (_DDN, "Intel UsbGpio Device")

      Method (_HID) {
        Return ("INTC10E2")
      }
    }// End of VGPO

    Device (VIC0) {
      ADBG ("VIC0: Virtual I2C Device Entry")
      Name (_UID, "VIC0")
      Name (_DDN, "Intel UsbI2C Device")

      Method (_HID) {
        Return ("INTC10B6")
      }
    }// End of VIC0


    Device (VIC1) {
      ADBG ("VIC1: Virtual I2C Device Entry")
      Name (_UID, "VIC1")
      Name (_DDN, "Intel UsbI2C Device")

      Method (_HID) {
        Return ("INTC10B6")
      }
    }// End of VIC1
  } // End of Scope (\_SB.PC00.XHCI.RHUB.HS03)
}
