/** @file
  ACPI DSDT table

  Copyright (c) 2022, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/


Device(ITE8)
{
  Name(_HID,EISAID("PNP0A05"))

  Name(_UID,6)

  // Status Method for the ITE SIO.

  Method(_STA,0,Serialized)
  {
    // Only report resources to the OS if the SIO Device is
    // present on the Docking Board.

    If(LEqual(ITSI,1))
    {
      Return(0x000F)
    }

    Return(0x0000)
  }

  // Define the SIO Index/Data Registers as an ACPI Operating
  // Region.  These registers will be used communicate to the SIO.

  OperationRegion(ITE8, SystemIO, 0x04E, 0x02)
  Field(ITE8, ByteAcc, Lock, Preserve)
  {
    INDX, 8,
    DATA, 8
  }

  OperationRegion(ITT8, SystemIO, 0x295, 0x02)
  Field(ITT8, ByteAcc, Lock, Preserve)
  {
    INDT, 8,
    DATT, 8
  }

  // Use ACPI Defined IndexField so consecutive Index/Data I/Os are
  // assured to be uninterrupted.

  IndexField(INDX, DATA, ByteAcc, Lock, Preserve)
  {
    Offset(0x07), // Logical Device Number.
    IT07, 8,
    Offset(0x20), // SIO Configuration and ID.
    IT20, 8,
    IT21, 8,
    IT22, 8,
    IT23, 8,
    IT24, 8,
    IT25, 8,
    IT26, 8,
    IT27, 8,
    IT28, 8,
    IT29, 8,
    IT2A, 8,
    Offset(0x30), // Logical Device Activate.
    IT30, 8,
    Offset(0x60), // I/O Space Configuration.
    IT60, 8,
    IT61, 8,
    Offset(0x70), // Interrupt Configuration.
    IT70, 8,
    IT71, 8,
    Offset(0x74), // DMA Configuration.
    IT74, 8,
    IT75, 8,
    Offset(0xF0), // Special Logical Device Configuration.
    ITF0, 8,
    ITF1, 8
  }

  //------------------------------------------------------------------------
  // Enter Config Mode, Select LDN
  // Arg0 : Logical Device number
  //------------------------------------------------------------------------
  Method(ENFG, 1)
  {
    Store(0x87, INDX)
    Store(0x01, INDX)
    Store(0x55, INDX)
    Store(0xAA, INDX)
    Store(Arg0, IT07) //Select LDN
  }

  //------------------------------------------------------------------------
  // Exit Config Mode
  //------------------------------------------------------------------------
  Method(EXFG, 0)
  {
    Store(0x02, INDX)
    Store(0x02, DATA)
  }

  // Set External Temperature Sensor Host Control Reg
  Method(SETP, 1)
  {
    Store(0x8E, INDT)
    Store(Arg0, DATT)
  }

  // Set Environment Controller Configuration Reg
  Method(SETS, 1)
  {
    Store(0x00, INDT)
    Store(Arg0, DATT)
  }
  If (LEqual(\ECON,0))
  {
    Include ("IT8659EHwMon.asl")
    if(LEqual(DPTF,1))
    {
      Include ("ItEHwMonFan2.asl")
      Include ("ItEHwMon.asl")
      Include ("Sen1Particip.asl")
      Include ("Sen2Particip.asl")
      Include ("Sen3Particip.asl")
    }  Else {
      Include ("IteHwMonAutoFan2.asl")
      Include ("IteHwMonAuto.asl")
    }
    Include ("IteCom.asl")
  }




}
