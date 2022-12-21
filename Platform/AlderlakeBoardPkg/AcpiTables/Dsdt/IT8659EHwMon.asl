/**@file IOTG SIO

  Copyright (c) 2022, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/


// This ASL file will support the F81804U HW monitor.

Device(IHWM)  // Hw monitor.
{
  Name(_HID,EISAID("PNP0A05"))
  Name(_UID,0)

  // Status Method.
  Method(_STA,0,Serialized)
  {
    // Only report resources to the OS if the SIO Device is present.
    // Only AlderIsland board has ITE8659-E SIO.

    If(LEqual(ITHW,1))
    {
      Return(0x0B)
    }

    Return(0x00)
  }

  // Current Resource.

  Method(_CRS,0,Serialized)
  {
    Name(HMIO, ResourceTemplate()
    {
      IO(Decode16,0x0295,0x0296,1,2)
    })

    Return(HMIO)
  }
}
