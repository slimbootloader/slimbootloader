/** @file

  Copyright (c) 2021, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#define CPU_DEVICE(INDEX, INDEX_VALUE) \
  Device(PR##INDEX){ \
    Name(_HID, "ACPI0007") \
    Name(_UID, INDEX_VALUE) \
  }

Scope(\_SB)
{
  CPU_DEVICE(00, 0)
  CPU_DEVICE(01, 1)
  CPU_DEVICE(02, 2)
  CPU_DEVICE(03, 3)
  CPU_DEVICE(04, 4)
  CPU_DEVICE(05, 5)
  CPU_DEVICE(06, 6)
  CPU_DEVICE(07, 7)
  CPU_DEVICE(08, 8)
  CPU_DEVICE(09, 9)
  CPU_DEVICE(10, 10)
  CPU_DEVICE(11, 11)
  CPU_DEVICE(12, 12)
  CPU_DEVICE(13, 13)
  CPU_DEVICE(14, 14)
  CPU_DEVICE(15, 15)
  CPU_DEVICE(16, 16)
  CPU_DEVICE(17, 17)
  CPU_DEVICE(18, 18)
  CPU_DEVICE(19, 19)
  CPU_DEVICE(20, 20)
  CPU_DEVICE(21, 21)
  CPU_DEVICE(22, 22)
  CPU_DEVICE(23, 23)
  CPU_DEVICE(24, 24)
  CPU_DEVICE(25, 25)
  CPU_DEVICE(26, 26)
  CPU_DEVICE(27, 27)
  CPU_DEVICE(28, 28)
  CPU_DEVICE(29, 29)
  CPU_DEVICE(30, 30)
  CPU_DEVICE(31, 31)
  CPU_DEVICE(32, 32)
  CPU_DEVICE(33, 33)
  CPU_DEVICE(34, 34)
  CPU_DEVICE(35, 35)
  CPU_DEVICE(36, 36)
  CPU_DEVICE(37, 37)
  CPU_DEVICE(38, 38)
  CPU_DEVICE(39, 39)
  CPU_DEVICE(40, 40)
  CPU_DEVICE(41, 41)
  CPU_DEVICE(42, 42)
  CPU_DEVICE(43, 43)
  CPU_DEVICE(44, 44)
  CPU_DEVICE(45, 45)
  CPU_DEVICE(46, 46)
  CPU_DEVICE(47, 47)
  CPU_DEVICE(48, 48)
  CPU_DEVICE(49, 49)
  CPU_DEVICE(50, 50)
  CPU_DEVICE(51, 51)
  CPU_DEVICE(52, 52)
  CPU_DEVICE(53, 53)
  CPU_DEVICE(54, 54)
  CPU_DEVICE(55, 55)
  CPU_DEVICE(56, 56)
  CPU_DEVICE(57, 57)
  CPU_DEVICE(58, 58)
  CPU_DEVICE(59, 59)
  CPU_DEVICE(60, 60)
  CPU_DEVICE(61, 61)
  CPU_DEVICE(62, 62)
  CPU_DEVICE(63, 63)
} // End Scope(\_SB)

#undef CPU_DEVICE


#ifndef SPS_SUPPORT // SPS is using Processor Aggregator Device different way
Scope(\_SB)
{
  // The Processor Aggregator Device provides a control point that enables the platform to perform
  // specific processor configuration and control that applies to all processors in the platform.
  Device (PAGD)
  {
    Name (_HID, "ACPI000C") // Processor Aggregator Device

    // _STA (Status)
    //
    // This object returns the current status of a device.
    //
    // Arguments: (0)
    //   None
    // Return Value:
    //   An Integer containing a device status bitmap:
    //    Bit 0 - Set if the device is present.
    //    Bit 1 - Set if the device is enabled and decoding its resources.
    //    Bit 2 - Set if the device should be shown in the UI.
    //    Bit 3 - Set if the device is functioning properly (cleared if device failed its diagnostics).
    //    Bit 4 - Set if the battery is present.
    //    Bits 5-31 - Reserved (must be cleared).
    //
    Method(_STA)
    {
      If(\_OSI("Processor Aggregator Device")){
        Return (0x0F) // Processor Aggregator Device is supported by this OS.
      } Else {
        Return (0)    // No support in this OS.
      }
    }


    // _PUR (Processor Utilization Request)
    //
    // The _PUR object is an optional object that may be declared under the Processor Aggregator Device
    // and provides a means for the platform to indicate to OSPM the number of logical processors
    // to be idled. OSPM evaluates the _PUR object as a result of the processing of a Notify event
    // on the Processor Aggregator device object of type 0x80.
    //
    // Arguments: (0)
    //   None
    // Return Value:
    //   Package
    //
    Name (_PUR, Package() // Requests a number of logical processors to be placed in an idle state.
    {
      1,                           // RevisionID, Integer: Current value is 1
      0                            // NumProcessors, Integer
    })

  } // end Device(PAGD)
}// end Scope(\_SB)
#endif // ndef SPS_SUPPORT

