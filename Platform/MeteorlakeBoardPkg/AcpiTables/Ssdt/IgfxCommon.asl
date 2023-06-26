/** @file
  IGD OpRegion/Software SCI Reference Code.
  This file contains ASL code with the purpose of handling events
  i.e. hotkeys and other system interrupts.

  Copyright (c) 2023, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/
//
// Notes:
// 1. The following routines are to be called from the appropriate event
//    handlers.
// 2. This code cannot comprehend the exact implementation in the OEM's BIOS.
//    Therefore, an OEM must call these methods from the existing event
//    handler infrastructure.  Details on when/why to call each method is
//    included in the method header under the "usage" section.
//

/************************************************************************;
;* ACPI Notification Methods
;************************************************************************/


/************************************************************************;
;*
;* Name:        PDRD
;*
;* Description: Check if the graphics driver is ready to process
;*              notifications and video extensions.
;*
;* Usage:       This method is to be called prior to performing any
;*              notifications or handling video extensions.
;*              Ex: If (PDRD()) {Return (FAIL)}
;*
;* Input:       None
;*
;* Output:      None
;*
;* References:  DRDY (Driver ready status), ASLP (Driver recommended
;*              sleep timeout value).
;*
;************************************************************************/

External(HNOT, MethodObj)

Method(PDRD)
{
  //
  // If DRDY is clear, the driver is not ready.  If the return value is
  // !=0, do not perform any notifications or video extension handling.
  //
  Return(LNot(DRDY))
}

/************************************************************************;
;*
;* Name:        PSTS
;*
;* Description: Check if the graphics driver has completed the previous
;*              "notify" command.
;*
;* Usage:       This method is called before every "notify" command.  A
;*              "notify" should only be set if the driver has completed the
;*              previous command.  Else, ignore the event and exit the parent
;*              method.
;*              Ex: If (PSTS()) {Return (FAIL)}
;*
;* Input:       None
;*
;* Output:      None
;*
;* References:  CSTS (Notification status), ASLP (Driver recommended sleep
;*              timeout value).
;*
;************************************************************************/

Method(PSTS)
{
  If(LGreater(CSTS, 2))
  {
    //
    // Sleep for ASLP milliseconds if the status is not "success,
    // failure, or pending"
    //
    Sleep(ASLP)
  }

  Return(LEqual(CSTS, 3)) // Return True if still Dispatched
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

Method(GNOT, 2)
{
  //
  // Check for 1. Driver loaded, 2. Driver ready.
  // If any of these cases is not met, skip this event and return failure.
  //
  If(PDRD())
  {
    Return(0x1) // Return failure if driver not loaded.
  }

  Store(Arg0, CEVT) // Set up the current event value
  Store(3, CSTS) // CSTS=BIOS dispatched an event

  If (LEqual (Arg1, 0)) // Do not re-enum if driver supports hotplug
  {
    //
    // Re-enumerate the Graphics Device for non-XP operating systems.
    //
    Notify(\_SB.PC00.GFX0, Arg1)
  }

  If(CondRefOf(HNOT))
  {
    HNOT(Arg0)  //Notification handler for Hybrid graphics
  }
  Else
  {
    Notify(\_SB.PC00.GFX0,0x80)
  }

  Return (0x0) // Return success
}

/************************************************************************;
;*
;* Name:        GLID
;*
;* Description: Handle a lid event (performs the Notify(GFX0, 0), but not the
;*              lid notify).
;*
;* Usage:       This method must be called when a lid event occurs.  A
;*              Notify(LID0, 0x80) must follow the call to this method.
;*
;* Input:       Arg0 = Lid state:
;*                0 = All closed
;*                1 = internal LFP lid open
;*                2 = external lid open
;*                3 = both external and internal open
;*
;* Output:      Returns 0=success, 1=failure.
;*              CLID and CEVT are indirect outputs.
;*
;* References:  CLID, GNOT
;*
;************************************************************************/

Method(GLID, 1)
{

  If (LEqual(Arg0,1))
  {
    Store(3,CLID)
  }
  Else
  {
    Store(Arg0, CLID)
  }
  //
  //Store(Arg0, CLID) // Store the current lid state
  // Call GNOT for CEVT=2=Lid, notify value = 0
  //
  if (GNOT(2, 0)) {
    Or (CLID, 0x80000000, CLID)
    Return (1) // Return Fail
  }

  Return (0) // Return Pass
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

Method(GDCK, 1)
{
  Store(Arg0, CDCK) // Store the current dock state
  //
  // Call GNOT for CEVT=4=Dock, notify value = 0
  //
  Return(GNOT(4, 0)) // Return stats from GNOT
}

/************************************************************************;
;* ASLE Interrupt Methods
;************************************************************************/

//
// Intel Ultrabook Event Handler.  Arg0 represents the Ultrabook Event Bit # to pass
// to the Intel Graphics Driver.  Note that this is a serialized method, meaning
// sumultaneous events are not allowed.
//
Method(IUEH,1,Serialized)
{
  And(IUER,0xC0,IUER) // Clear all button events on entry.
  XOr(IUER,Shiftleft(1,Arg0),IUER) // Toggle status.

  If(LLessEqual(Arg0,4)) // Button Event?
  {
    Return(AINT(5,0)) // Generate event and return status.

  }
  Else // Indicator Event.
  {
    Return(AINT(Arg0,0)) // Generate event and return status.
  }
}

/************************************************************************;
;*
;* Name:        AINT
;*
;* Description: Call the appropriate methods to generate an ASLE interrupt.
;*              This process includes ensuring the graphics driver is ready
;*              to process the interrupt, ensuring the driver supports the
;*              interrupt of interest, and passing information about the event
;*              to the graphics driver.
;*
;* Usage:       This method must called to generate an ASLE interrupt.
;*
;* Input:       Arg0 = ASLE command function code:
;*                0 = Set ALS illuminance
;*                1 = Set backlight brightness
;*                2 = Reserved
;*                4 = Reserved
;*                5 = Button Indicator Event
;*                6 = Convertible Indicator Event
;*                7 = Docking Indicator Event
;*              Arg1 = If Arg0 = 0, current ALS reading:
;*                0 = Reading below sensor range
;*                1-0xFFFE = Current sensor reading
;*                0xFFFF = Reading above sensor range
;*              Arg1 = If Arg0 = 1, requested backlight percentage
;*
;* Output:      Returns 0 = success, 1 = failure
;*
;*
;************************************************************************/

Method (AINT, 2)
{
  //
  // Handle Intel Ultrabook Events.
  //
  If(LAnd(LGreaterEqual(Arg0,5),LLessEqual(Arg0,7)))
  {
    Store (0x01, ASLE) // Generate ASLE interrupt

    Store(0,Local2) // Use Local2 as a timeout counter.  Intialize to zero.

    While (LLess (Local2,250)) // Wait 1 second or until Driver ACKs a success.
    {
      Sleep(4) // Delay 4 ms.
      Increment(Local2) // Increment Timeout.
    }

    Return(0) // Return success
  }
  //
  // Evaluate the first argument (backlight brightness, or ALS).
  //
  If (LEqual (Arg0, 1)) // Arg0=1, so set the backlight brightness.
  {
    Store (Divide (Multiply (Arg1, 255), 100), BCL1) // Convert from percent to 0-255.
    Or (BCL1, 0x80000000, BCL1) // Set the valid bit.
  }

  Store(0x01, ASLE) // Generate ASLE interrupt
  Return(0x0) // Return success
}
