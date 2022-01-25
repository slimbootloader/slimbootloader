/** @file

  Copyright (c) 2021, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

Device(H8S2)
{
  Name(_HID,EISAID("PNP0A05"))

  Name(_UID, 5)

  Method(_STA,0,Serialized)
  {
    If(LEqual(PFLV,FlavorMobile))
    {
      Return(0x000F)
    }
    If(LEqual(HSIO,1))
    {
      Return(0x000F)
    }
    Return(0x0000)
  }

  Device(UAR9)  // Serial Port UART 9
  {
    Name(_HID, EISAID("PNP0501"))

    Name(_UID, 1)

    //
    // Status Method for UART 9.
    //
    Method(_STA, 0, Serialized)
    {
      If(LEqual(PFLV,FlavorMobile))
      {
        Return(0x000F)
      }
      If(LEqual(HSIO,1))
      {
        Return(0x000F)
      }
      Return(0x0000)
    }

    //
    // Disable Method for UART 9.
    //
    //
    // Current Resource Setting Method for UART 9.
    //
    Method(_CRS, 0, Serialized)
    {
      //
      // Create the Buffer that stores the resources to be returned.
      //
      Name(BUF0, ResourceTemplate()
      {
        IO(Decode16,0x03F8,0x03F8,0x01,0x08)
        IRQNoFlags(){4}
      })

      //
      // TO-DO: Need to implement the code updating BUF0 (I/O 0x3F8~0x3FF and IRQ 4 are the default setting)
      // according to the real resources assigned when the EC commands are available.
      //

      Return(BUF0)
    }


    //
    // Set Resource Setting Method for UART 9.
    //
    Method(_SRS,1,Serialized)
    {
      CreateByteField(Arg0, 0x02, IOLO)
      CreateByteField(Arg0, 0x03, IOHI)
      CreateWordField(Arg0, 0x09, IRQW)

      If(LEqual(IOHI, 0x03))
      {
        If(LEqual(IOLO, 0xF8))
        {
          If(LEqual(IRQW, 4))
          {

          }
        }
      }

      //
      // TO-DO: Need to implement the code setting the resources based on the arguments when the EC commands are available.
      //
    }

    //
    // D0 Method for COM Port.
    //
    Method(_PS0,0,Serialized)
    {
      //
      // TO-DO: Need to implement the code put the device on D0 state when the EC commands are available.
      //
    }

    //
    // D3 Method for COM Port.
    //
    Method(_PS3,0,Serialized)
    {
      //
      // TO-DO: Need to implement the code putting the device on D3 state when the EC commands are available.
      //
    }
  }
}


