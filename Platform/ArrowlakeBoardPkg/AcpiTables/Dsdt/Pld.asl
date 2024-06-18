/** @file
  ACPI DSDT Physical Location of Device definiton for GFX displays and Touch Panels

  Copyright (c) 2024, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

External (\_SB.PC00.THC0, DeviceObj)
External (\_SB.PC00.THC1, DeviceObj)

//
// Platform reference PLD Form Factor definiton
//
Scope (\_SB) {
  //
  // Cabinet 0 Primary Front
  //
  Name (C0PF, Package (0x1) {
             ToPLD (
               PLD_Revision         = 0x2,
               PLD_IgnoreColor      = 0x1,
               PLD_Red              = 0x0,
               PLD_Green            = 0x0,
               PLD_Blue             = 0x0,
               PLD_Width            = 0x320, //800 mm
               PLD_Height           = 0x7D0, //2000 mm
               PLD_UserVisible      = 0x1, //Set if the device connection point can be seen by the user without disassembly.
               PLD_Dock             = 0x0, // Set if the device connection point resides in a docking station or port replicator.
               PLD_Lid              = 0x0, // Set if this device connection point resides on the lid of laptop system.
               PLD_Panel              = "TOP",    // Describes which panel surface of the systems housing the device connection point resides on.
               PLD_VerticalPosition   = "CENTER", // Vertical Position on the panel where the device connection point resides.
               PLD_HorizontalPosition = "RIGHT",  // Horizontal Position on the panel where the device connection point resides.
               PLD_Shape              = "VERTICALRECTANGLE",
               PLD_GroupOrientation = 0x0, // if Set, indicates vertical grouping, otherwise horizontal is assumed.
               PLD_GroupToken       = 0x0, // Unique numerical value identifying a group.
               PLD_GroupPosition    = 0x0, // Identifies this device connection points position in the group (i.e. 1st, 2nd)
               PLD_Bay              = 0x0, // Set if describing a device in a bay or if device connection point is a bay.
               PLD_Ejectable        = 0x0, // Set if the device is ejectable. Indicates ejectability in the absence of _EJx objects
               PLD_EjectRequired    = 0x0, // OSPM Ejection required: Set if OSPM needs to be involved with ejection process. User-operated physical hardware ejection is not possible.
               PLD_CabinetNumber    = 0x0, // For single cabinet system, this field is always 0.
               PLD_CardCageNumber   = 0x0, // For single card cage system, this field is always 0.
               PLD_Reference        = 0x0, // if Set, this _PLD defines a reference shape that is used to help orient the user with respect to the other shapes when rendering _PLDs.
               PLD_Rotation         = 0x0, // 0 - 0deg, 1 - 45deg, 2 - 90deg, 3 - 135deg, 4 - 180deg, 5 - 225deg, 6 - 270deg, 7 - 315deg
               PLD_Order            = 0x3, // Identifies the drawing order of the connection point described by a _PLD
               PLD_VerticalOffset   = 0x0,
               PLD_HorizontalOffset = 0x0
              )
            } // Package
  ) //C0PF
  //
  // Cabinet 1 Primary Front
  //
  Name (C1PF, Package (0x1) {
             ToPLD (
               PLD_Revision         = 0x2,
               PLD_IgnoreColor      = 0x1,
               PLD_Red              = 0x0,
               PLD_Green            = 0x0,
               PLD_Blue             = 0x0,
               PLD_Width            = 0x320, //800 mm
               PLD_Height           = 0x7D0, //2000 mm
               PLD_UserVisible      = 0x1, //Set if the device connection point can be seen by the user without disassembly.
               PLD_Dock             = 0x0, // Set if the device connection point resides in a docking station or port replicator.
               PLD_Lid              = 0x0, // Set if this device connection point resides on the lid of laptop system.
               PLD_Panel              = "TOP",    // Describes which panel surface of the systems housing the device connection point resides on.
               PLD_VerticalPosition   = "CENTER", // Vertical Position on the panel where the device connection point resides.
               PLD_HorizontalPosition = "RIGHT",  // Horizontal Position on the panel where the device connection point resides.
               PLD_Shape              = "VERTICALRECTANGLE",
               PLD_GroupOrientation = 0x0, // if Set, indicates vertical grouping, otherwise horizontal is assumed.
               PLD_GroupToken       = 0x0, // Unique numerical value identifying a group.
               PLD_GroupPosition    = 0x0, // Identifies this device connection points position in the group (i.e. 1st, 2nd)
               PLD_Bay              = 0x0, // Set if describing a device in a bay or if device connection point is a bay.
               PLD_Ejectable        = 0x0, // Set if the device is ejectable. Indicates ejectability in the absence of _EJx objects
               PLD_EjectRequired    = 0x0, // OSPM Ejection required: Set if OSPM needs to be involved with ejection process. User-operated physical hardware ejection is not possible.
               PLD_CabinetNumber    = 0x1, // For single cabinet system, this field is always 0.
               PLD_CardCageNumber   = 0x0, // For single card cage system, this field is always 0.
               PLD_Reference        = 0x0, // if Set, this _PLD defines a reference shape that is used to help orient the user with respect to the other shapes when rendering _PLDs.
               PLD_Rotation         = 0x0, // 0 - 0deg, 1 - 45deg, 2 - 90deg, 3 - 135deg, 4 - 180deg, 5 - 225deg, 6 - 270deg, 7 - 315deg
               PLD_Order            = 0x3, // Identifies the drawing order of the connection point described by a _PLD
               PLD_VerticalOffset   = 0x0,
               PLD_HorizontalOffset = 0x0
              )
            } // Package
  ) //C1PF
} // end Scope (\_SB)

//
// GFX _PLD definitons cannot be defined directly due to scope limitations
//     PLDn methods are provided and called if given Display is detected
//
// \SB.PC00.GFX0.DD1F._PLD
Scope (\) {
  Method (PLD1, 0, Serialized) {Return (\_SB.C0PF)}
}
// \SB.PC00.GFX0.DD2F._PLD
Scope (\) {
  Method (PLD2, 0, Serialized) {Return (\_SB.C1PF)}
}

//
// THC _PLD Definitions
// Note: There should be as many definitions with corresponding _ADR as HID child devices created based upon
//       HID descriptor for perticular Touch device
//
Scope (\_SB.PC00.THC0) {
  // Top Level Collection
  Device (TLC1) {
    Name (_ADR, 0x1)
    Method (_PLD, 0, Serialized) {Return (\_SB.C0PF)}
    Method (_STA, 0, NotSerialized) {Return (0xF)}
  }
  Device (TLC2) {
    Name (_ADR, 0x2)
    Method (_PLD, 0, Serialized) {Return (\_SB.C0PF)}
    Method (_STA, 0, NotSerialized) {Return (0xF)}
  }
  Device (TLC3) {
    Name (_ADR, 0x3)
    Method (_PLD, 0, Serialized) {Return (\_SB.C0PF)}
    Method (_STA, 0, NotSerialized) {Return (0xF)}
  }
} // end THC0
Scope (\_SB.PC00.THC1) {
  // Top Level Collections
  Device (TLC1) {
    Name (_ADR, 0x1)
    Method (_PLD, 0, Serialized) {Return (\_SB.C1PF)}
    Method (_STA, 0, NotSerialized) {Return (0xF)}
  }
  Device (TLC2) {
    Name (_ADR, 0x2)
    Method (_PLD, 0, Serialized) {Return (\_SB.C1PF)}
    Method (_STA, 0, NotSerialized) {Return (0xF)}
  }
  Device (TLC3) {
    Name (_ADR, 0x3)
    Method (_PLD, 0, Serialized) {Return (\_SB.C1PF)}
    Method (_STA, 0, NotSerialized) {Return (0xF)}
  }
} // end THC1

