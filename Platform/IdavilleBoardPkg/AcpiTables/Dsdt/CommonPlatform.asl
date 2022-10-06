/** @file
  CommonPlatform.asl - Contains common objects for the platform

  Copyright (c) 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

  //
  // External declarations
  // HECI-1/HECI-2 are in 10nmServerPlatformPkg\Me\Sps\Acpi\SpsNm.asl
  //
  External(\_SB.PC00.HEC2.HPTS, MethodObj)
  External(\_SB.PC00.HEC2.HWAK, MethodObj)
  External(\_SB.OSPC, MethodObj)

  //
  // System Sleep States
  //
  Name (\_S0,Package (){0,0,0,0})
  Name (\_S3,Package (){5,0,0,0})   // Name changed to \DS3 if disabled in Setup
  Name (\_S4,Package (){6,0,0,0})   // Name changed to \DS4 if disabled in Setup
  Name (\_S5,Package (){7,0,0,0})

  //
  // Native OS hot plug support, 0->ACPI, 1->OS
  //
  Name (\OSHF, 0)

  Name (\HWPS, 0)   // CPC capability from platform _OSC
  //
  // OS flag
  //
  #include "Os.asi"

  //
  // for determing PIC mode
  //
  Name (\PICM,Zero)
  Method (\_PIC, 1, NotSerialized) {
    Store(Arg0,\PICM)
  }

  OperationRegion (DBG0, SystemIO, 0x80, 2)
  Field (DBG0, ByteAcc,NoLock,Preserve) {
    IO80, 8,
    IO81, 8
  }

  //
  // Access CMOS range
  //
  OperationRegion (ACMS, SystemIO, 0x72, 2)
  Field ( ACMS, ByteAcc, NoLock, Preserve) {
    INDX, 8,
    DATA, 8
  }

  //
  // SWGPE_CTRL
  //
  OperationRegion (GPCT, SystemIO, Add (PMBA, 0x42), 1)
  Field (GPCT, ByteAcc, NoLock, Preserve) {
          ,   1,
    SGPC  ,   1,
  }

  //
  // GPE0 Enable
  //
  OperationRegion (GPE0, SystemIO, Add (PMBA, 0x9C), 4)
  Field (GPE0, ByteAcc,NoLock,Preserve) {
    ,1,
    GPEH,1,
    ,7,
    PCIE,1,
    ,1,
    PMEE,1,
    ,1,
    PMB0,1,
    ,18,
  }

  //
  // GPE0 Status
  //
  OperationRegion (GPES, SystemIO, Add (PMBA, 0x8C), 4)
  Field (GPES, ByteAcc,NoLock,Preserve) {
    ,1,
    GPSH,1,
    SGPS,1,
    ,6,
    PEES,1,
    ,1,
    PMES,1,
    ,1,
    PMEB,1,
    ,18,
  }

  //
  // System sleep down
  //
  Method (_PTS, 1, NotSerialized)
  {
    Store (0x72, IO80)  // Sync with EfiPostCode.h

    //
    // Clear wake event status.
    //
    Store(1,PMES)
    Store(1,PMEB)

    //
    // Enable SCI and wake event sources.
    //
    Store(1,GPEH)
    Store(1,PCIE)
    Store(1,PMEE)
    Store(1,PMB0)

    //
    // If HECI-2 exist call its prepare-to-sleep handler.
    // The handler checks whether HECI-2 is enabled.
    //
    If (CondRefOf(\_SB.PC00.HEC2.HPTS))
    {
      \_SB.PC00.HEC2.HPTS()
    }
  }

  //
  // System Wake up
  //
  Method (_WAK, 1, Serialized)
  {
    Store (0x73, IO80)  // Sync with EfiPostCode.h

    //
    // If HECI-2 exist call its wake-up handler.
    // The handler checks whether HECI-2 is enabled.
    //
    If (CondRefOf(\_SB.PC00.HEC2.HWAK))
    {
      \_SB.PC00.HEC2.HWAK()
    }

    //
    // If waking from S3
    //
    If (LEqual(Arg0, 3)) {
    }

    Return(Package(){0, 0})
  }

  Scope(\_SB) {

    Name(OSCP, 0)

    // Information on CPU and Memory for hotplug SKUs
    #include "CpuMemHp.asi"

    OperationRegion (IOB2, SystemIO, 0xB2, 2)  //MKF_SMIPORT
    Field (IOB2, ByteAcc, NoLock, Preserve) {
      SMIC, 8,                               // SW-SMI ctrl port
      SMIS, 8,                               // SW-SMI status port
    }

    Method(_OSC , 4) {
      Store(Arg3,Local0)
      CreateDWordField(Local0,4,CPBF)
      If(LEqual(Arg0, ToUUID("0811B06E-4A27-44F9-8D60-3CBBC22E7B48"))) {
        Store(CPBF, OSCP)
        //Save Platform OSC HWPM capabilities
        If(LEqual(And(CPBF, 0x00000040), 0x00000040)) {
          Store(1,HWPS)
        }
      }
      If (CondRefOf (\_SB.OSPC)) {
        Return (\_SB.OSPC(Arg0, Arg1, Arg2, Arg3))
      }
      Return (Local0)
    }
    //
    // SGX
    //
    #include "Sgx.asi"
    //
    // NGN Dimm support
    //
    // ICX_TODO
    //#include "Ngn.asi"
  }   // end _SB scope

