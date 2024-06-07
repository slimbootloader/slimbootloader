/** @file
  ACPI EC SSDT table that contains base EC ACPI definitions.

  Copyright (c) 2024, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
External (\D8XH, MethodObj)
External (\PB1E, IntObj)
External (\ACTT, IntObj)
External (\_TZ.ETMD, IntObj)
External (\_TZ.FN00._OFF, MethodObj)
External (\_SB.PC00.GFX0.CLID)
External (\LIDS, IntObj)
External (\_SB.PC00.GFX0.CDCK)
External (\PWRS, FieldUnitObj)
External (\S0ID, IntObj)
External (\ADBG, MethodObj)
External (\BPNT, MethodObj)
External (\SPNT, MethodObj)
External(\_SB.PC00.RP12.CLID)

Name (_HID, EISAID ("PNP0C09"))

Name (_UID, 1)

Method (_CRS, 0, Serialized)
{
  Name (BFFR, ResourceTemplate ()
  {
    IO (Decode16, 0x62, 0x62, 0, 1)  // DIN/DOUT
    IO (Decode16, 0x66, 0x66, 0, 1)  // CMD/STS
  })

  Return (BFFR)
}

Method (_STA)
{
  Return (0x0F)
}

OperationRegion (ECF3, EmbeddedControl, 0, 0xFF)
Field (ECF3, ByteAcc, Lock, Preserve)
{
  Offset(4),
  IWCW, 160,    // EC buffer, send data to EC
  Offset(24),
  IWCR, 160,
}

/************************************************************************;
;*
;* Name:  CHDK
;*
;* Description: Check DOCK status, returen True if Dock status equal Arg0
;*
;************************************************************************/
Method (CHDK, 1)
{
  //@todo: Waiting for DOCK offect data
  //      If (LEqual (ECRD (RefOf (DOCK)), Arg0))
  If (LEqual (0, Arg0))
  { // If not docked then it's hot plug
    Return (1)
  }
  Return (0)
}

#define ROTATION_LOCK_BUTTON 4
#define CONVERTIBLE_BUTTON   6
#define DOCK_INDICATOR       7

//
// Subfunctions for VB driver and HID Event Filter driver. Those were all defined under VGBI before but
// due to HID Event Filter driver integration effort, they are relocated under EC device so that both VGBI and HIDD
// can use those methods.
// Relocated methods: VGBS, UPBT
// Removed method: PBIN (no longer used)
//
Name (VBDS, 0)
Name (ONTM, 0) // Temporary variable to initialise VBDS only once in the boot
Method (VGBS, 0, Serialized)  // Virtual GPIO Button Status - Refer Bios Arch Spec
{
  // Bit[7] : Docking Indicatory Status
  // Bit[6] : Convertible/Slate Indicator Status
  // Bit[5] : Reserved
  // Bit[4] : Rotation Lock Button Status
  // Bit[3] : Volume Down Button Status
  // Bit[2] : Volume Up Button Status
  // Bit[1] : Windows Home Button Status
  // Bit[0] : Power Button Status

  ADBG ("VGBS")
  If (LEqual (ONTM, 0))
  {
    // Initial setup option on VBDS and thereafter hotkeys should be updating the VBDS
    If (LEqual (And (\PB1E, 0x04), 0x04)) // Rotation Lock
    {
      UPBT (ROTATION_LOCK_BUTTON, One)
    }

    If (LEqual (And (\PB1E, 0x08), 0x08)) // Slate/Laptop
    {
      UPBT (CONVERTIBLE_BUTTON, One)
    }

    If (LEqual (And (\PB1E, 0x10), 0x10))  // Undock/Dock
    {
      UPBT (DOCK_INDICATOR, One)
    }
    Store (One, ONTM)
  }
  Return (VBDS)
}// End of Method

//
// UPBT Update Button Status
//
// Arg0: Bit location of the target button
//       0: Power Button
//       1: Windows Button
//       2: Volume up Button
//       3: Volume down Button
//       4: Rotation Lock Button
//       5: Reserved
//       6: Convertible state 0 - Slate, 1 - Notebook
//       7: Dock Indicator 0 - Undock, 1 - Dock
//
// Arg1: On/Off state, 0 - Clear the target bit, 1 - Set the target bit.
//
Method (UPBT, 2, Serialized)  // Update Button Status
{
  ShiftLeft (One, Arg0, Local0)
  If (Arg1){                // Button Press/ON
    Or (VBDS, Local0, VBDS)
  } Else {                 // Button Press/OFF
    And (VBDS, Not (Local0), VBDS)
  }
} // End of UPBT

// EREG method will be used in _REG (evaluated by OS without ECDT support) or _INI (for OS with ECDT support)
Method (EREG)
{
  // Update ECAV Object. ASL should check for this value to be One before accessing EC OpRegion.
  Store (One, ECAV)

  // Turn off the CPU Fan if Active Cooling is disabled.
  If (LEqual (0, \ACTT))
  {
    ECWT (0, RefOf (CFAN))
  }

  // Turn off pwm fan so it starts in a known state for legacy thermal zone only.
  If (CondRefOf (\_TZ.ETMD))
  {
    If (LEqual (\_TZ.ETMD, 1))
    {
      \_TZ.FN00._OFF ()
    }
  }

  // Save the Lid State in global NVS and IGD OpRegion.
  //Store (LSTE, \_SB.PC00.GFX0.CLID)
  If (LEqual (ECRD (RefOf (LSTE)), 0))
  {
    Store (0, \_SB.PC00.GFX0.CLID)
    // Update DG LFP Lid status if DG is present
    If (LNotEqual(DGBA, 0))
    {
      Store(0,\_SB.PC00.RP12.CLID)
    }
  }
  If (LEqual (ECRD (RefOf (LSTE)), 1))
  {
    Store (3, \_SB.PC00.GFX0.CLID)
    // Update DG LFP Lid status if DG is present
    If (LNotEqual(DGBA, 0))
    {
      Store(3,\_SB.PC00.RP12.CLID)
    }
  }
  Store (ECRD (RefOf (LSTE)), \LIDS)

  // Update the Dock Status
  Store (DSTS, \_SB.PC00.GFX0.CDCK)

  // Unconditionally fix up the Battery and Power State.

  // Initialize the Number of Present Batteries.
  //  1 = Real Battery 1 is present
  //  2 = Real Battery 2 is present
  //  3 = Real Battery 1 and 2 are present
  Store (0, BNUM)
  Or (BNUM, ShiftRight (And (ECRD (RefOf (B1ST)), 0x08), 3), BNUM)


  // Initialize the Power State.
  //  BNUM = 0 = Virtual Power State
  //  BNUM > 0 = Real Power State
  If (LEqual (BNUM, 0))
  {
    Store (ECRD (RefOf (VPWR)), \PWRS)
  }
  Else
  {
    Store (ECRD (RefOf (RPWR)), \PWRS)
  }

  // Perform needed ACPI Notifications.

  SPNT ()
  BPNT ()
}

// The _REG Method is needed because communication with the EC
// before the driver is loaded is prohibited in WIN2000/WINXP.
// According to ACPI spec, the arguments:
//   Arg0 - Opertion Region Address space ID
//   Arg1 - handler connection code
Method (_REG, 2)
{
  // When OS runs _REG control method with Arg0 = 3 (Embedded Controller Operation Region) and Arg1 = 1 (connect the handler)
  If (LAnd (LEqual (Arg0, 3), LEqual (Arg1, 1)))
  {
    // Must be running NT 5.0 OS or newer.
    EREG ()
  }
}

// The _REG Method will not evaluated if ECDT is loaded by OS with ECDT support
// Uising _INI Method to cover EC initialization done in _REG
Method (_INI)
{
  If (LGreaterEqual (_REV, 2)) {
    // Only for OS with ECDT support, such as Vista or newer Windows OS
    EREG ()
  }

  If (LEqual (\S0ID, 1)) {
    //
    // Enable SCIs in EC to trigger QD5 and QD6 event
    //
    ECWT (0x01, RefOf (BTEN))                    // Enable EC Power Button in ACPI name space
    ECMD (0x38)                                  // Enable/Disable SCIs from buttons
  }
}

//
// The _GPE method needs to be the same as the GPE number assignment in ECDT
//
Method (_GPE)
{
  return (EGPE)
}
