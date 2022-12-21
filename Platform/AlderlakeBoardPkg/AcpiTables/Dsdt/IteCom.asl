/** @file
  ACPI DSDT table

  Copyright (c) 2022, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/


Device(UAR1)  // Serial Port A
{
  Name(_HID, EISAID("PNP0501"))

  Name(_UID,11)

  // Status Method for COM A.

  Method(_STA,0,Serialized)
  {
    // Only report resources to the OS if the SIO Device is
    // not set to Disabled in BIOS Setup.

    If(LEqual(ITCM,1))
    {
      // Set the SIO to COM A.
      ENFG(0x01)

      // Display COMA and return status.

      If(IT30)
      {
        EXFG()
        Return(0x000F)
      }
      EXFG()
      Return(0x000D)
    }
    Return(0x0000)
  }

  // Disable Method for COM A.

  Method(_DIS,0,Serialized)
  {
    ENFG(0x01)
    Store(0,IT30)
    EXFG()
  }

  // Current Resource Setting Method for COM A.

  Method(_CRS,0,Serialized)
  {
    // Create the Buffer that stores the Resources to
    // be returned.

    Name(BUF0,ResourceTemplate()
    {
      IO(Decode16,0x03F8,0x03F8,0x01,0x08)
      IRQNoFlags(){4}
    })

    // Set SIO to COM A.
    ENFG(0x01)
    // Skip this sequence if the COMA Port is Disabled
    // in BIOS Setup.

    If(LEqual(ITCM,1))
    {
      // Create pointers to the specific byte.

      CreateByteField(BUF0,0x02,IOL0)
      CreateByteField(BUF0,0x03,IOH0)
      CreateByteField(BUF0,0x04,IOL1)
      CreateByteField(BUF0,0x05,IOH1)
      CreateByteField(BUF0,0x07,LEN0)
      CreateWordField(BUF0,0x09,IRQW)

      // Write IO and Length values into the Buffer.

      Store(IT60,IOH0)
      Store(IT61,IOL0)
      Store(IT60,IOH1)
      Store(IT61,IOL1)
      Store(8,LEN0)

      // Write the IRQ value into the Buffer.

      And(IT70,0x0F,Local0)
      If(Local0)
      {
        ShiftLeft(One,Local0,IRQW)
      }
      Else
      {
        Store(Zero,IRQW)
      }
    }

    EXFG()
    Return(BUF0)
  }

  // Possible Resource Setting Method for COM A.

    // Build a Buffer with all valid COM Port Resources.

    Name(_PRS,ResourceTemplate()
    {
      StartDependentFn(0,2)
      {
        IO(Decode16,0x3F8,0x3F8,0x01,0x08)
        IRQNoFlags(){4}
      }

      StartDependentFn(0,2)
      {
        IO(Decode16,0x2F8,0x2F8,0x01,0x08)
        IRQNoFlags(){3}
      }

      StartDependentFn(0,2)
      {
        IO(Decode16,0x3E8,0x3E8,0x01,0x08)
        IRQNoFlags(){4}
      }

      StartDependentFn(0,2)
      {
        IO(Decode16,0x2E8,0x2E8,0x01,0x08)
        IRQNoFlags(){3}
      }

      EndDependentFn()
    })


  // Set Resource Setting Method for COM A.

  Method(_SRS,1,Serialized)
  {
    // Point to the specific information in the passed
    // in Buffer.

    CreateByteField(Arg0,0x02,IOLO)
    CreateByteField(Arg0,0x03,IOHI)
    CreateWordField(Arg0,0x09,IRQW)

    // Set the SIO to COM A.
    ENFG(0x01)

    // Disable the device.

    Store(0,IT30)

    // Set the Base IO Address.

    Store(IOLO,IT61)
    Store(IOHI,IT60)

    // Set the IRQ.

    FindSetRightBit(IRQW,Local0)
    If(LNotEqual(IRQW,Zero))
    {
      Decrement(Local0)
    }
    Store(Local0,IT70)

    // Set the Decode Range so COM A works on ICH7 and
    // future platforms.

    AND(IOD0,0xF8,IOD0)     // Clear all bits.

    If(LEqual(IOHI,0x03))     // Address = 0x3xx
    {
      If(LEqual(IOLO,0xF8))   // Address = 0x3F8
      {
        Or(IOD0,0x00,IOD0)
      }
      Else        // Address = 0x3E8
      {
        Or(IOD0,0x07,IOD0)
      }
    }
    Else          // Address = 0x2xx
    {
      If(LEqual(IOLO,0xF8))   // Address = 0x2F8
      {
        Or(IOD0,0x01,IOD0)
      }
      Else        // Address = 0x2E8
      {
        Or(IOD0,0x05,IOD0)
      }
    }

    // Enable the device.

    Store(1,IT30)
    EXFG()
  }

  // D0 Method for COM Port.

  Method(_PS0,0,Serialized)
  {
    ENFG(0x01)
    Store(1,IT30)
    EXFG()
  }

  // D3 Method for COM Port.

  Method(_PS3,0,Serialized)
  {
    ENFG(0x01)
    Store(0,IT30)
    EXFG()
  }
}
#if 0
Device(UAR2)  // Serial Port B
{
  Name(_HID, EISAID("PNP0501"))

  Name(_UID,12)

  // Status Method for COM B.

  Method(_STA,0,Serialized)
  {
    // Only report resources to the OS if the SIO Device is
    // not set to Disabled in BIOS Setup.

    If(LEqual(ITCM,1))
    {
      // Set the SIO to COM B.
      ENFG(0x02)

      // Display COM B and return status.

      If(IT30)
      {
        EXFG()
        Return(0x000F)
      }
      EXFG()
      Return(0x000D)
    }
    Return(0x0000)
  }

  // Disable Method for COM B.

  Method(_DIS,0,Serialized)
  {
    ENFG(0x02)
    Store(0,IT30)
    EXFG()
  }

  // Current Resource Setting Method for COM B.

  Method(_CRS,0,Serialized)
  {
    // Create the Buffer that stores the Resources to
    // be returned.

    Name(BUF0,ResourceTemplate()
    {
      IO(Decode16,0x02F8,0x02F8,0x01,0x08)
      IRQNoFlags(){3}
    })

    // Set SIO to COM B.

    ENFG(0x02)

    // Skip this sequence if the COM B Port is Disabled
    // in BIOS Setup.

    If(LEqual(ITCM,1))
    {
      // Create pointers to the specific byte.

      CreateByteField(BUF0,0x02,IOL0)
      CreateByteField(BUF0,0x03,IOH0)
      CreateByteField(BUF0,0x04,IOL1)
      CreateByteField(BUF0,0x05,IOH1)
      CreateByteField(BUF0,0x07,LEN0)
      CreateWordField(BUF0,0x09,IRQW)

      // Write IO and Length values into the Buffer.

      Store(IT60,IOH0)
      Store(IT61,IOL0)
      Store(IT60,IOH1)
      Store(IT61,IOL1)
      Store(8,LEN0)

      // Write the IRQ value into the Buffer.

      And(IT70,0x0F,Local0)
      If(Local0)
      {
        ShiftLeft(One,Local0,IRQW)
      }
      Else
      {
        Store(Zero,IRQW)
      }
    }
    EXFG()
    Return(BUF0)
  }

  // Possible Resource Setting Method for COM B.

    // Build a Buffer with all valid COM B Port Resources.

    Name(_PRS,ResourceTemplate()
    {
      StartDependentFn(0,2)
      {
        IO(Decode16,0x2E8,0x2E8,0x01,0x08)
        IRQNoFlags(){3}
      }

      StartDependentFn(0,2)
      {
        IO(Decode16,0x2F8,0x2F8,0x01,0x08)
        IRQNoFlags(){3}
      }

      StartDependentFn(0,2)
      {
        IO(Decode16,0x3E8,0x3E8,0x01,0x08)
        IRQNoFlags(){3}
      }

      StartDependentFn(0,2)
      {
        IO(Decode16,0x3F8,0x3F8,0x01,0x08)
        IRQNoFlags(){3}
      }

      StartDependentFn(0,2)
      {
        IO(Decode16,0x2E8,0x2E8,0x01,0x08)
        IRQNoFlags(){4}
      }

      StartDependentFn(0,2)
      {
        IO(Decode16,0x2F8,0x2F8,0x01,0x08)
        IRQNoFlags(){4}
      }

      StartDependentFn(0,2)
      {
        IO(Decode16,0x3E8,0x3E8,0x01,0x08)
        IRQNoFlags(){4}
      }

      StartDependentFn(0,2)
      {
        IO(Decode16,0x3F8,0x3F8,0x01,0x08)
        IRQNoFlags(){4}
      }

      EndDependentFn()
    })


  // Set Resource Setting Method for COM B.

  Method(_SRS,1,Serialized)
  {
    // Point to the specific information in the passed
    // in Buffer.

    CreateByteField(Arg0,0x02,IOLO)
    CreateByteField(Arg0,0x03,IOHI)
    CreateWordField(Arg0,0x09,IRQW)

    // Set the SIO to COM B.

    ENFG(0x02)

    // Disable the device.

    Store(0,IT30)

    // Set the Base IO Address.

    Store(IOLO,IT61)
    Store(IOHI,IT60)

    // Set the IRQ.

    FindSetRightBit(IRQW,Local0)
    If(LNotEqual(IRQW,Zero))
    {
      Decrement(Local0)
    }
    Store(Local0,IT70)

    // Set the Decode Range so COM B works on ICH7 and
    // future platforms.

    AND(IOD0,0x8F,IOD0)     // Clear all bits.

    If(LEqual(IOHI,0x03))     // Address = 0x3xx
    {
      If(LEqual(IOLO,0xF8))   // Address = 0x3F8
      {
        Or(IOD0,0x00,IOD0)
      }
      Else        // Address = 0x3E8
      {
        Or(IOD0,0x70,IOD0)
      }
    }
    Else          // Address = 0x2xx
    {
      If(LEqual(IOLO,0xF8))   // Address = 0x2F8
      {
        Or(IOD0,0x10,IOD0)
      }
      Else        // Address = 0x2E8
      {
        Or(IOD0,0x50,IOD0)
      }
    }

    // Enable the device.

    Store(1,IT30)
    EXFG()
  }

  // D0 Method for COM B Port.

  Method(_PS0,0,Serialized)
  {
    ENFG(0x02)
    Store(1,IT30)
    EXFG()
  }

  // D3 Method for COM B Port.

  Method(_PS3,0,Serialized)
  {
    ENFG(0x02)
    Store(0,IT30)
    EXFG()
  }
}
#endif
