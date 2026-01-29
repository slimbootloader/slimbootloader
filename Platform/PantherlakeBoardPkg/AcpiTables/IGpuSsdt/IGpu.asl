/** @file
  This file contains the IGD OpRegion/Software ACPI Reference
  Code.
  It defines the methods to enable/disable output switching,
  store display switching and LCD brightness BIOS control
  and return valid addresses for all display device encoders
  present in the system, etc.

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

External (\PLD1, MethodObj)
External (\PLD2, MethodObj)
External (\PBCL, MethodObj)
External (HDOS, MethodObj)
External (DSEN)

//
// Default Physical Location of Device
//
Name (DPLD, Package (0x1) {
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
) //DPLD

// Enable/Disable Output Switching.  In WIN2K/WINXP, _DOS = 0 will
// get called during initialization to prepare for an ACPI Display
// Switch Event.  During an ACPI Display Switch, the OS will call
// _DOS = 2 immediately after a Notify=0x80 to temporarily disable
// all Display Switching.  After ACPI Display Switching is complete,
// the OS will call _DOS = 0 to re-enable ACPI Display Switching.
Method (_DOS,1)
{
  //
  // Store Display Switching and LCD brightness BIOS control bit
  //
  Store (And (Arg0,7),DSEN)

  If (LEqual (And (Arg0,  0x3), 0))     // If _DOS[1:0]=0
  {
    If (CondRefOf (HDOS))
    {
      HDOS ()
    }
  }
}

// This is the method to program the requested backlight percentage for LFP.
// Input:     Arg0 = ASLE command function code:
//               0 = Set ALS illuminance
//               1 = Set backlight brightness
//               2 = Reserved
//               4 = Reserved
//               5 = Button Indicator Event
//               6 = Convertible Indicator Event
//               7 = Docking Indicator Event

Method (AINT, 1)
{
  Store (Divide (Multiply (Arg0, 255), 100), BCL1) // Convert from percent to 0-255.
  Or (BCL1, 0x80000000, BCL1) // Set the valid bit.

  Return (0x0) // Return success
}

//
// Enumerate the Display Environment.  This method will return
// valid addresses for all display device encoders present in the
// system.  The Miniport Driver will reject the addresses for every
// encoder that does not have an attached display device.  After
// enumeration is complete, the OS will call the _DGS methods
// during a display switch only for the addresses accepted by the
// Miniport Driver.  For hot-insertion and removal of display
// devices, a re-enumeration notification will be required so the
// address of the newly present display device will be accepted by
// the Miniport Driver.
//
Method (_DOD,0)
{
  If (LAnd (LNotEqual (DIDL, 0x0), LNotEqual (DDL2, 0x0)))
  {
    DID1 = 0x80010400
    DID2 = 0x80010401
    If (LEqual (IPTP,1))
    {
      //
      // IGFX need report IPUA as GFX0 child
      //
      Return (Package () {0x80010400, 0x80010401, 0x80023480})
    }
    Else
    {
      Return (Package () {0x80010400, 0x80010401})
    }
  }
  ElseIf (LAnd (LNotEqual (DIDL, 0x0), LEqual (DDL2, 0x0)))
  {
    DID1 = 0x80010400
    If (LEqual (IPTP,1)) {
      //
      // IGFX need report IPUA as GFX0 child
      //
      Return (Package () {0x80010400, 0x80023480})
    }
    Else
    {
      Return (Package () {0x80010400})
    }
  }
  //
  // Return if no LFP.
  // (Prevents compiler warning.)
  //
  If (LEqual (IPTP,1)) {
    //
    // IGFX need report IPUA as GFX0 child
    //
    Return (Package () {0x80023480})
  }
  Return (Package () {0x00000400})
}

Device (DD01)
{
  //
  // Return Unique ID.
  //
  Method (_ADR,0,Serialized)
  {
    If (LEqual (And (0x0F00,DID1),0x400))
    {
      Store (0x1, EDPV)
      Store (DID1, DIDX)
      Return (1)
    }
    If (LEqual (DID1,0))
    {
      Return (1)
    }
    Else
    {
      Return (And (0xFFFF,DID1))
    }
  }
}

Device (DD02)
{
  //
  // Return Unique ID.
  //
  Method (_ADR,0,Serialized)
  {
    If (LEqual (And (0x0F00,DID2),0x400))
    {
      If (LEqual (And (0xF,DID2),0x1))
      {
        Store (0x2, EDPV)
        Store (DID2, DIDY)
        Return (2)
      }
      Store (0x2, EDPV)
      Store (DID2, DIDX)
      Return (2)
    }
    If (LEqual (DID2,0))
    {
      Return (2)
    }
    Else
    {
      Return (And (0xFFFF,DID2))
    }
  }
}

//
//Device for eDP
//
Device (DD1F)
{
  //
  // Return Unique ID.
  //
  Method (_ADR,0,Serialized)
  {
    If (LEqual (EDPV, 0x0))
    {
      Return (0x1F)
    }
    Else
    {
      Return (And (0xFFFF,DIDX))
    }
  }

  //
  // Query List of Brightness Control Levels Supported.
  //
  Method (_BCL,0)
  {
    //
    // List of supported brightness levels in the following sequence.
    // Level when machine has full power.
    // Level when machine is on batteries.
    // Other supported levels.
    //
    If (CondRefOf (\PBCL)) {
      Return (PBCL ())
    } Else {
      Return (Package () {80, 50, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100})
    }
  }

  //
  // Set the Brightness Level.
  //
  Method (_BCM,1)
  {
    //
    // Set the requested level if it is between 0 and 100%.
    //
    If (LAnd (LGreaterEqual (Arg0,0),LLessEqual (Arg0,100)))
    {
      AINT (Arg0)
      Store (Arg0,BRTL)  // Store Brightness Level.
    }
  }

  //
  // Brightness Query Current level.
  //
  Method (_BQC,0)
  {
    Return (BRTL)
  }

  //
  // Physical Location of Device
  //
  Method (_PLD,0)
  {
    If (CondRefOf (\PLD1)) {
      Return (PLD1 ())
    } Else {
      Return (DPLD)
    }
  }
}

//
// Second Display
//
Device (DD2F)
{
  //
  // Return Unique ID.
  //
  Method (_ADR,0,Serialized)
  {
    If (LEqual (EDPV, 0x0))
    {
      Return (0x1F)
    }
    If (LEqual (EDPV, 0x1))
    {
      Return (0x1F)
    }
    Else
    {
      Return (And (0xFFFF,DIDY))
    }
  }

  //
  // Query List of Brightness Control Levels Supported.
  //
  Method (_BCL,0)
  {
    //
    // List of supported brightness levels in the following sequence.
    // Level when machine has full power.
    // Level when machine is on batteries.
    // Other supported levels.
    //
    If (CondRefOf (\PBCL)) {
      Return (PBCL ())
    } Else {
      Return (Package () {80, 50, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100})
    }
  }

  //
  // Set the Brightness Level.
  //
  Method (_BCM,1)
  {
    //
    // Set the requested level if it is between 0 and 100%.
    //
    If (LAnd (LGreaterEqual (Arg0,0),LLessEqual (Arg0,100)))
    {
      AINT (Arg0)
      Store (Arg0,BRTL)  // Store Brightness Level.
    }
  }

  //
  // Brightness Query Current level.
  //
  Method (_BQC,0)
  {
    Return (BRTL)
  }

  Method (_PLD,0)
  {
    If (CondRefOf (\PLD2)) {
      Return (PLD2 ())
    } Else {
      Return (DPLD)
    }
  }
}

/************************************************************************;
;*
;* Name:        PDRD
;*
;* Description: Check if the graphics driver is ready to process
;*              notifications and video extensions.
;*
;* Usage:       This method is to be called prior to performing any
;*              notifications or handling video extensions.
;*              Ex: If (PDRD ()) {Return (FAIL)}
;*
;* Input:       None
;*
;* Output:      None
;*
;* References:  DRDY (Driver ready status), ASLP (Driver recommended
;*              sleep timeout value).
;*
;************************************************************************/

External (HNOT, MethodObj)

Method (PDRD)
{
  //
  // If DRDY is clear, the driver is not ready.  If the return value is
  // !=0, do not perform any notifications or video extension handling.
  //
  Return (LNot (DRDY))
}

/************************************************************************;
;*
;* Name:  GNOT
;*
;* Description: Call the appropriate methods to query the graphics driver
;*              status.  If all methods return success, do a notification of
;*              the graphics device.
;*
;* Usage:       This method is to be called when a graphics device
;*              notification is required (display switch hotkey, etc).
;*
;* Input:       Arg0 = Current event type:
;*                1 = reserved
;*                2 = lid
;*                3 = dock
;*              Arg1 = Notification type:
;*                0 = Re-enumeration
;*                0x80 = Display switch
;*
;* Output:      Returns 0 = success, 1 = failure
;*
;* References:  PDRD and PSTS methods.  OSYS (OS version)
;*
;************************************************************************/

Method (GNOT, 2)
{
  //
  // Check for 1. Driver loaded, 2. Driver ready.
  // If any of these cases is not met, skip this event and return failure.
  //
  If (PDRD ())
  {
    Return (0x1) // Return failure if driver not loaded.
  }

  Store (Arg0, CEVT) // Set up the current event value
  Store (3, CSTS) // CSTS=BIOS dispatched an event

  If (LEqual (Arg1, 0)) // Do not re-enum if driver supports hotplug
  {
    //
    // Re-enumerate the Graphics Device for non-XP operating systems.
    //
    Notify (\_SB.PC00.GFX0, Arg1)
  }

  If (CondRefOf (HNOT))
  {
    HNOT (Arg0)  //Notification handler for Hybrid graphics
  }
  Else
  {
    Notify (\_SB.PC00.GFX0,0x80)
  }

  Return (0x0) // Return success
}

/************************************************************************;
;*
;* Name:  GDCK
;*
;* Description: Handle a docking event by updating the current docking status
;*              and doing a notification.
;*
;* Usage:       This method must be called when a docking event occurs.
;*
;* Input:       Arg0 = Docking state:
;*                0 = Undocked
;*                1 = Docked
;*
;* Output:      Returns 0=success, 1=failure.
;*              CDCK and CEVT are indirect outputs.
;*
;* References:  CDCK, GNOT
;*
;************************************************************************/

Method (GDCK, 1)
{
  Store (Arg0, CDCK) // Store the current dock state
  //
  // Call GNOT for CEVT=4=Dock, notify value = 0
  //
  Return (GNOT (4, 0)) // Return stats from GNOT
}

//
// Include IGD OpRegion/Software SCI interrupt handler/DSM which is used by
// the graphics drivers to request data from system BIOS.
//
include ("IGpuOpRn.asl")
include ("IGpuDsm.asl")
