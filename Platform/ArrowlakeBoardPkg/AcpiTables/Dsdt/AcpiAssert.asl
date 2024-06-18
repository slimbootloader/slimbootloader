/** @file
  Source file for ACPI ADBG enable/disable.

  Copyright (c) 2024, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#include <Include/AcpiDebug.h>

#if ACPI_DEBUG_ENABLE
Scope (\_SB)
{
  //
  //  Acpi Assert method
  //  Arg0: Post code to publish
  //  Arg1: ACPi debug message to print
  //  On calling the ACPI method, the method outputs postcode data to Port 80 [2 nibbles].
  //  Method prints the debug serial message to serial interface.
  //  A deadloop is implemented within which is a 10 second timer countdown.
  //  Once the timer reaches zero, then ADB6 is printed for one minute before
  //  repeating the timer countdown once again. The serial debug message is printed again
  //  and the last two nibbles of the post code is printed again.
  //
  Method (ASRT, 2)
  {
    Local1 = Arg0 & 0x00FF      // Grabbing the last two hex digits

    While (1) {                 // deadloop
      Local0 = 0x9              // This contains the timer value
      P8XH (0,Local1)           // Output post code (LSB) to Port 80h.
      ADBG (ToHexString (Arg1))

      While (Local0 > 0)
      {
        P8XH (1,Local0)         // Output post code (MSB) to port 81h.
        Local0--                // Subtract one

        for (Local2 = 10000, Local2 > 0, Local2--)  // Stall has requirement that argument cannot be greater than 100
        {
          Stall (100);          // wait one second where stall is in microseconds
        }
      }
      P8XH (0,0xB6)       // Output post code (LSB) to Port 80h.
      P8XH (1,0xAD)       // Output post code (MSB) to port 81h.
      Sleep (60000);      // sleep for 1 minute where sleep is in milliseconds
    } //dead loop
  } //method ASRT
} //scope \_SB
#endif