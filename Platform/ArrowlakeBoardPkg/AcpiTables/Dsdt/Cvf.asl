/** @file
  CloverFalls (CVF) Driver
  This ASL file will support the CVF companion die which is interfaced via USB IOs Expansion.

  Copyright (c) 2024, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

External(\_SB.PC00.HCID, MethodObj)
Include("GpioAcpiDefinesMtl.h")

Scope(\_SB.PC00) {

  Device(SPFD) {
    ADBG ("SPFD Device Entry")
    Name (_UID, "SPFD")
    //
    //DDN(DOS Device name) - This object is used to associate a logical name (for example, COM1) with a device.
    //
    Name(_DDN, "Intel SPI OED Device")

    //
    //This object is used to supply OSPM with the devices PNP ID or ACPI ID.
    //
    Method (_HID) {
      Return ("INTC10D0")
    }

    Name(PKG0, Package() {"\\_SB.PC00.XHCI.RHUB.VGPO", "\\_SB.PC00.XHCI.RHUB.VSPI"})
    Method (_DEP, 0, NotSerialized) { // _DEP: Operation Region Dependencies
      If(LEqual (CVFS, 1)) {
        Return (PKG0)
      }
      Return (Package() {})
    }

    Method (_CRS, 0, Serialized) { // _CRS: Current Resource Settings
      Name (CBF1, ResourceTemplate () {
        //
        //Host IRQ
        //
        GpioIo (Shared, PullDefault, 0, 0, IoRestrictionInputOnly,  "\\_SB.GPI0",,,,) {GPIO_VER6_SOC_M_DRIVER_GPP_C8}
        //
        //Interrupt to Host
        //
        GpioInt (Edge, ActiveLow, Shared, PullDefault, 0x0000, "\\_SB.GPI0",,,,) {  GPIO_VER6_SOC_M_DRIVER_GPP_C8}
      })

      Name (CBF2, ResourceTemplate () {
        //
        //Host IRQ
        //
        GpioIo (Shared, PullDefault, 0, 0, IoRestrictionInputOnly,  "\\_SB.GPI0",,,,) {GPIO_VER6_SOC_M_DRIVER_GPP_V23}
        //
        //Interrupt to Host
        //
        GpioInt (Edge, ActiveLow, Shared, PullDefault, 0x0000, "\\_SB.GPI0",,,,) {  GPIO_VER6_SOC_M_DRIVER_GPP_V23}
      })

      Name (CBF3, ResourceTemplate () {
        //
        //Host IRQ
        //
        GpioIo (Shared, PullDefault, 0, 0, IoRestrictionInputOnly,  "\\_SB.GPI0",,,,) {GPIO_VER6_SOC_M_DRIVER_GPP_E0}
        //
        //Interrupt to Host
        //
        GpioInt (Edge, ActiveLow, Shared, PullDefault, 0x0000, "\\_SB.GPI0",,,,) {  GPIO_VER6_SOC_M_DRIVER_GPP_E0}
      })

      Name (VGB1, ResourceTemplate () {
        //
        // Virtual GPIOs from USB chip - To support GPIO interrupt.
        //
        GpioIo (Exclusive, PullDefault, 0, 0, IoRestrictionOutputOnly, "\\_SB.PC00.XHCI.RHUB.VGPO",,,,) { 0x0}
        GpioIo (Exclusive, PullDefault, 0, 0, IoRestrictionOutputOnly, "\\_SB.PC00.XHCI.RHUB.VGPO",,,,) { 0x1}
      })

      Name (VSB1, ResourceTemplate () {
        //
        // SPI Peripheral for 9MHz
        //
        SpiSerialBusV2 (0x00,PolarityLow,FourWireMode,8,ControllerInitiated,9000000,ClockPolarityHigh,ClockPhaseSecond,"\\_SB.PC00.XHCI.RHUB.VSPI",,,,,)
      })

      If (LEqual (CVFS, 1)) {
        ADBG ("SPFD _CRS returns resources for usb based cvf")
        If ((ToInteger (PLID) == 6)){
          Return (ConcatenateResTemplate (ConcatenateResTemplate (CBF2, VGB1), VSB1))
        } ElseIf ((ToInteger (PLID) == 7)||(ToInteger (PLID) == 8)){
          Return (ConcatenateResTemplate (ConcatenateResTemplate (CBF3, VGB1), VSB1))
        }
        Return(ConcatenateResTemplate(ConcatenateResTemplate(CBF1, VGB1), VSB1))
      }
      Return (ResourceTemplate () {GpioIo(Exclusive, PullDefault, 0, 0, IoRestrictionNone, "\\_SB.PC00",,,DUMM) {0}}) // Dummy Resource
    }

    Device(CVFD) {
      ADBG ("CVFD Device Entry")
      Name (_UID, "CVFD")
      Name(_DDN, "Intel CVF Device")

      Method (_HID) {
        Return ("INTC10CF")
      }
      //
      // Returns the Camera Sensor HID which is configured on LNK1(CVF POR)
      //
      Method (SID,1, Serialized) {
        Return(\_SB.PC00.HCID(1))
      }
    } // End of CVF Device
  } // End of SPFD(CVF SPI Fucntional Device) Device
} // End of Scope (\_SB.PC00)

Scope (\_SB.PC00.XHCI.RHUB) {
  //
  // Virtual GPIO Device to access the GPIO pins from ucontroller interfaced with CVF.
  //
  Device (VGPO) {
    ADBG ("VGPO Virutal GPIO Device Entry")
    Name (_UID, "VGPO")
    Name (_DDN, "Intel UsbGpio Device")

    Method (_HID) {
      Return ("INTC10D1")
    }

    // USB Port Mapping:
    // 4 : MTL M
    // 6 : MTL P
    Name(PKG1, Package() {"\\_SB.PC00.XHCI.RHUB.HS04"})
    Name(PKG2, Package() {"\\_SB.PC00.XHCI.RHUB.HS06"})

    Method (_DEP, 0, NotSerialized) {
      If (LEqual (CUPN,0x04)) {
        Return (PKG1)
      } ElseIf (LEqual (CUPN,0x06)) {
        Return (PKG2)
      }
      Return (Package() {})
    }
  }// End of VGPO

  //
  // Virutal I2C Device to access the I2C controller from ucontroller interfaced with CVF.
  //
  Device (VIC0) {
    ADBG ("VIC0: Virtual I2C Device Entry")
    Name (_UID, "VIC0")
    Name (_DDN, "Intel UsbI2C Device")

    Method (_HID) {
      Return ("INTC10D2")
    }
    // USB Port Mapping:
    // 4 : MTL M
    // 6 : MTL P
    Name(PKG1, Package() {"\\_SB.PC00.XHCI.RHUB.HS04"})
    Name(PKG2, Package() {"\\_SB.PC00.XHCI.RHUB.HS06"})

    Method (_DEP, 0, NotSerialized) {
      If (LEqual (CUPN,0x04)) {
        Return (PKG1)
      } ElseIf (LEqual (CUPN,0x06)) {
        Return (PKG2)
      }
      Return (Package() {})
    }
  }// End of VIC0

  Device (VIC1) {
    ADBG ("VIC1: Virtual I2C Device Entry")
    Name (_UID, "VIC1")
    Name (_DDN, "Intel UsbI2C Device")

    Method (_HID) {
      Return ("INTC10D2")
    }
    // USB Port Mapping:
    // 4 : MTL M
    // 6 : MTL P
    Name(PKG1, Package() {"\\_SB.PC00.XHCI.RHUB.HS04"})
    Name(PKG2, Package() {"\\_SB.PC00.XHCI.RHUB.HS06"})

    Method (_DEP, 0, NotSerialized) {
      If (LEqual (CUPN,0x04)) {
        Return (PKG1)
      } ElseIf (LEqual (CUPN,0x06)) {
        Return (PKG2)
      }
      Return (Package() {})
    }
  }// End of VIC1

  //
  // Virutal SPI Device to access SPI from ucontroller interfaced with CVF.
  //
  Device (VSPI) {
    ADBG ("VSPI: Virtual SPI Device Entry")
    Name (_UID, "VSPI")
    Name (_DDN, "Intel UsbSPI Device")

    Method (_HID) {
      Return ("INTC10D3")
    }
    // USB Port Mapping:
    // 4 : MTL M
    // 6 : MTL P
    Name(PKG1, Package() {"\\_SB.PC00.XHCI.RHUB.HS04"})
    Name(PKG2, Package() {"\\_SB.PC00.XHCI.RHUB.HS06"})

    Method (_DEP, 0, NotSerialized) {
      If (LEqual (CUPN,0x04)) {
        Return (PKG1)
      } ElseIf (LEqual (CUPN,0x06)) {
        Return (PKG2)
      }
      Return (Package() {})
    }
  }// End of VSPI
} // End of Scope (\_SB.PC00.XHCI.RHUB)