/** @file
  RST RAID support in ACPI

  Copyright (c) 2015 - 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

//
// Include this file in SATA scope to enable RST RAID support
//

//
// NOTE: For platform implementers.
// IR3E method checks if RTD3 flow is allowed for given device
// RTD3 flow is enabled if it hasn't been globally locked or device isn't part of the RAID volume
// RST driver will globally lock RTD3 flows in following scenarios:
// 1. Volume creation
// 2. System shutdown
// 3. System transition to S3/S4
//
// To enable RAID functionality platform code should check in _ON/_OFF methods of the
// power resource associated with the device that can be a part of the RAID volume if RTD3 flow has not been blocked.
// If RTD3 flow has been blocked platform code should not execute D3Cold entry logic.
//
// To allow RAID volume to manage SATA port power implement SPON/SPOF methods.
// SPON - Method to turn SATA port on.
// SPOF - Method to turn SATA port off.
//

//
// SSDT RTD3 imports
//
External(\_SB.PC00.SAT0.PRT0.SPON, MethodObj)
External(\_SB.PC00.SAT0.PRT1.SPON, MethodObj)
External(\_SB.PC00.SAT0.PRT2.SPON, MethodObj)
External(\_SB.PC00.SAT0.PRT3.SPON, MethodObj)
External(\_SB.PC00.SAT0.PRT4.SPON, MethodObj)
External(\_SB.PC00.SAT0.PRT5.SPON, MethodObj)
External(\_SB.PC00.SAT0.PRT0.SPOF, MethodObj)
External(\_SB.PC00.SAT0.PRT1.SPOF, MethodObj)
External(\_SB.PC00.SAT0.PRT2.SPOF, MethodObj)
External(\_SB.PC00.SAT0.PRT3.SPOF, MethodObj)
External(\_SB.PC00.SAT0.PRT4.SPOF, MethodObj)
External(\_SB.PC00.SAT0.PRT5.SPOF, MethodObj)

#define RAID_VOL_PRT0_MASK 0x01
#define RAID_VOL_PRT1_MASK 0x02
#define RAID_VOL_PRT2_MASK 0x04
#define RAID_VOL_PRT3_MASK 0x08
#define RAID_VOL_PRT4_MASK 0x10
#define RAID_VOL_PRT5_MASK 0x20
#define RAID_VOL_NVM1_MASK 0x100
#define RAID_VOL_NVM2_MASK 0x200
#define RAID_VOL_NVM3_MASK 0x400

Name(VL0M, 0) //Raid Volume 0 Mask
Name(VL1M, 0) //Raid Volume 1 Mask
Name(VL2M, 0) //Raid Volume 2 Mask
Name(VR3A, 1) //Global RTD3 Allowed Flag

// Is RAID member method
// Arg0 - SATA port/ remapped port RAID mask value
Method(IRM, 1) {
  If(And(VL0M, Arg0)) {
    Return(1)
  } ElseIf(And(VL1M, Arg0)) {
    Return(1)
  } ElseIf(And(VL2M, Arg0)) {
    Return(1)
  } Else {
    Return(0)
  }
}

// Is D3Cold blocked on the port.
// Arg0 - SATA port / remapped port RAID mask value
Method(PD3C, 1) {
  Return(LNot(LOr(VR3A, LNot(IRM(Arg0)))))
}

Scope(PRT0) {
  Method(IR3D) { Return(PD3C(RAID_VOL_PRT0_MASK)) }
}

Scope(PRT1) {
  Method(IR3D) { Return(PD3C(RAID_VOL_PRT1_MASK)) }
}

Scope(PRT2) {
  Method(IR3D) { Return(PD3C(RAID_VOL_PRT2_MASK)) }
}

Scope(PRT3) {
  Method(IR3D) { Return(PD3C(RAID_VOL_PRT3_MASK)) }
}

Scope(PRT4) {
  Method(IR3D) { Return(PD3C(RAID_VOL_PRT4_MASK)) }
}

Scope(PRT5) {
  Method(IR3D) { Return(PD3C(RAID_VOL_PRT5_MASK)) }
}

Scope(NVM1) {
  Method(IR3D) { Return(PD3C(RAID_VOL_NVM1_MASK)) }
}

Scope(NVM2) {
  Method(IR3D) { Return(PD3C(RAID_VOL_NVM2_MASK)) }
}

Scope(NVM3) {
  Method(IR3D) { Return(LNot(RAID_VOL_NVM3_MASK)) }
}

// RSTD - _DSM method for Device RAID Volume
// Arg0 - UUID: E03E3431-E510-4fa2-ABC0-2D7E901245FE
// Arg1 - Revision ID: 0x01 (Integer)
// Arg2 - Function Index: 0x01 (Integer)
// Arg3 - Rtd3PortMask: A bit mask of SATA Ports representing each
//        of the RAID member disk in the RAID Volume to be saved for RTD3 handling (Integer)
//        The form of the bit mask for each RAID volume is:
//        b-00000000000000000000000000pppppp where each p represents a SATA Controller port.
//        If p in bit 0 is set to 1 then SATA Controller Port 0 is part of the RAID Volume.
//        A 0 in a bit position means the port is not part of the RAID Volume.
//        A mask with no port bits set is valid and means there are no devices that are part
//        of the RTD3 of this RAID Volume.
// Return - A package containing a single Integer signifying the Error value:
//          0: Success.
//          1: Invalid package UUID.
//          2: Invalid package size.
//          All Others: Reserved.
//
// Arg4 - PMAP Port Bitmap Status - output argument where port-per-volume bitmap will be stored
//
Method (RSTD, 5, Serialized,,, {BuffObj, IntObj, IntObj, PkgObj, FieldUnitObj}) {
  If (LEqual(Arg0, ToUUID ("E03E3431-E510-4fa2-ABC0-2D7E901245FE"))) {
    Switch(ToInteger(Arg2)) {
      Case (0) {
        // Standard query - A bitmask of functions supported
        Name(RBUF, Buffer(1){0x00})
        CreateByteField (RBUF, 0, SFUN)
        Store (0x3F, SFUN)
        Return (RBUF)
      }
      Case (1) {// Assign ports to this RAID volume - _SAVE_RTD3_PACKAGE
        Store(DeRefOf(Index(Arg3,0)), Arg4) // Assign port mapping to the volume's power resource
        Store(DeRefOf(Index(Arg3,0)), Local0) //workaround for ADBG crashing when given directly "Arg4" instead of Local0
        Return(Package(){0})
      }
      Case (2) {  // RST Driver access to turn ON a port.
        Store(DeRefOf(Index(Arg3,0)), Local0)
        \_SB.PC00.SAT0.CPON(Local0);
        Return(Package(){0})
      }
      Case (3) {  //RST Driver access to turn OFF a port.
        Store(DeRefOf(Index(Arg3,0)), Local0)
        \_SB.PC00.SAT0.CPOF(Local0)
        Return(Package(){0})
      }
      Case(4) { // RST Driver read RAID mask
        Name(GETM, Buffer(1){0x00})
        CreateByteField (GETM, 0, GMSK)
        Store (Arg4, GMSK)
        Return (GETM)
      }
      Case(5) { //Globally allow/disallow RTD3 for Raid Volumes
        Store(DeRefOf(Index(Arg3,0)), VR3A)
        Return (0)
      }
      Default {
        Return(Package(){2})
      }
    }
  } Else {
    Return(Buffer(){0})
  }
}

//
// Indicate to the RST driver whether it is required to support D3 on the SATA controller.
// This method is called via a _DSM method in SAT0 scope.
// Method GUID: {9CD9CDDD-8845-4AFD-8392-31C4EB87CABD}
// Argument:
// Arg0 - Revision id passed from _DSM : IntObj
// Arg1 - Function index : IntObj
// Arg2 - Package of arguments : PkgObj
//
Method(D3AS, 3, Serialized,,, {IntObj, IntObj, PkgObj}) {
  Switch(Arg1) {
    Case(0) {
      // This method supports function index 0 and 1
      Return(Buffer() {0x3})
    }
    Case(1) {
      // For now D3 support for adapter is required only if PS_ON is enabled.
      If(LEqual(PSON, 1)) {
        Return(Buffer() {1})
      } Else {
        Return(Buffer() {0})
      }
    }
  }
  Return(Buffer() {0})
}

// CPON  : Method to turn ON ports per PMAP bitmap status
// Arg0 : PMAP - Port Bitmap Status
Method(CPON,1,Serialized,,,{IntObj}) {
  If(LAnd(CondRefOf(\_SB.PC00.SAT0.PRT0.SPON),And(Arg0, RAID_VOL_PRT0_MASK))) { \_SB.PC00.SAT0.PRT0.SPON() }
  If(LAnd(CondRefOf(\_SB.PC00.SAT0.PRT1.SPON),And(Arg0, RAID_VOL_PRT1_MASK))) { \_SB.PC00.SAT0.PRT1.SPON() }
  If(LAnd(CondRefOf(\_SB.PC00.SAT0.PRT2.SPON),And(Arg0, RAID_VOL_PRT2_MASK))) { \_SB.PC00.SAT0.PRT2.SPON() }
  If(LAnd(CondRefOf(\_SB.PC00.SAT0.PRT3.SPON),And(Arg0, RAID_VOL_PRT3_MASK))) { \_SB.PC00.SAT0.PRT3.SPON() }
  If(LAnd(CondRefOf(\_SB.PC00.SAT0.PRT4.SPON),And(Arg0, RAID_VOL_PRT4_MASK))) { \_SB.PC00.SAT0.PRT4.SPON() }
  If(LAnd(CondRefOf(\_SB.PC00.SAT0.PRT5.SPON),And(Arg0, RAID_VOL_PRT5_MASK))) { \_SB.PC00.SAT0.PRT5.SPON() }
  // Remapped PCIe
  If(LAnd(CondRefOf(\_SB.PC00.SAT0.NVM1.RPON),And(Arg0, RAID_VOL_NVM1_MASK))) { \_SB.PC00.SAT0.NVM1.RPON() }
  If(LAnd(CondRefOf(\_SB.PC00.SAT0.NVM1.NVD0),And(Arg0, RAID_VOL_NVM1_MASK))) { \_SB.PC00.SAT0.NVM1.NVD0() }

  If(LAnd(CondRefOf(\_SB.PC00.SAT0.NVM2.RPON),And(Arg0, RAID_VOL_NVM2_MASK))) { \_SB.PC00.SAT0.NVM2.RPON() }
  If(LAnd(CondRefOf(\_SB.PC00.SAT0.NVM2.NVD0),And(Arg0, RAID_VOL_NVM2_MASK))) { \_SB.PC00.SAT0.NVM2.NVD0() }

  If(LAnd(CondRefOf(\_SB.PC00.SAT0.NVM3.RPON),And(Arg0, RAID_VOL_NVM3_MASK))) { \_SB.PC00.SAT0.NVM3.RPON() }
  If(LAnd(CondRefOf(\_SB.PC00.SAT0.NVM3.NVD0),And(Arg0, RAID_VOL_NVM3_MASK))) { \_SB.PC00.SAT0.NVM3.NVD0() }
}

// CPOF  : Method to turn OFF ports per PMAP bitmap status
// Arg0 : PMAP - Port Bitmap Status
Method(CPOF,1,Serialized,,,{IntObj}) {
  If(LAnd(CondRefOf(\_SB.PC00.SAT0.PRT0.SPOF),And(Arg0, RAID_VOL_PRT0_MASK))) { \_SB.PC00.SAT0.PRT0.SPOF() }
  If(LAnd(CondRefOf(\_SB.PC00.SAT0.PRT1.SPOF),And(Arg0, RAID_VOL_PRT1_MASK))) { \_SB.PC00.SAT0.PRT1.SPOF() }
  If(LAnd(CondRefOf(\_SB.PC00.SAT0.PRT2.SPOF),And(Arg0, RAID_VOL_PRT2_MASK))) { \_SB.PC00.SAT0.PRT2.SPOF() }
  If(LAnd(CondRefOf(\_SB.PC00.SAT0.PRT3.SPOF),And(Arg0, RAID_VOL_PRT3_MASK))) { \_SB.PC00.SAT0.PRT3.SPOF() }
  If(LAnd(CondRefOf(\_SB.PC00.SAT0.PRT4.SPOF),And(Arg0, RAID_VOL_PRT4_MASK))) { \_SB.PC00.SAT0.PRT4.SPOF() }
  If(LAnd(CondRefOf(\_SB.PC00.SAT0.PRT5.SPOF),And(Arg0, RAID_VOL_PRT5_MASK))) { \_SB.PC00.SAT0.PRT5.SPOF() }
  // Remapped PCIe
  If(LAnd(CondRefOf(\_SB.PC00.SAT0.NVM1.NVD3),And(Arg0, RAID_VOL_NVM1_MASK))) { \_SB.PC00.SAT0.NVM1.NVD3() }
  If(LAnd(CondRefOf(\_SB.PC00.SAT0.NVM1.RPOF),And(Arg0, RAID_VOL_NVM1_MASK))) { \_SB.PC00.SAT0.NVM1.RPOF() }

  If(LAnd(CondRefOf(\_SB.PC00.SAT0.NVM2.NVD3),And(Arg0, RAID_VOL_NVM2_MASK))) { \_SB.PC00.SAT0.NVM2.NVD3() }
  If(LAnd(CondRefOf(\_SB.PC00.SAT0.NVM2.RPOF),And(Arg0, RAID_VOL_NVM2_MASK))) { \_SB.PC00.SAT0.NVM2.RPOF() }

  If(LAnd(CondRefOf(\_SB.PC00.SAT0.NVM3.NVD3),And(Arg0, RAID_VOL_NVM3_MASK))) { \_SB.PC00.SAT0.NVM3.NVD3() }
  If(LAnd(CondRefOf(\_SB.PC00.SAT0.NVM3.RPOF),And(Arg0, RAID_VOL_NVM3_MASK))) { \_SB.PC00.SAT0.NVM3.RPOF() }
}

Device(VOL0) {
  Name(_ADR,0x0080FFFF)  // RAID Volume 0
  Name(_PR0, Package(){V0PR})
  Name(_PR3, Package(){V0PR})

  Method (_DSM, 4, Serialized, 0, UnknownObj, {BuffObj, IntObj, IntObj, PkgObj}) {
    Return(\_SB.PC00.SAT0.RSTD(Arg0,Arg1,Arg2,Arg3,RefOf(\_SB.PC00.SAT0.VL0M)))
  }

  PowerResource(V0PR, 0, 0) { // Power Resource Aggregate for RAID volume 0
    Name(_STA, 0x01)

    Method(_ON, 0) {
      If(VR3A) {
        \_SB.PC00.SAT0.CPON(VL0M)
        Store(0x1, _STA)
      } Else {
      }
    }

    Method(_OFF, 0) {
      If (VR3A) {
        \_SB.PC00.SAT0.CPOF(VL0M)
        Store(0x0, _STA)
      } Else {
      }
    }
  }
}

Device(VOL1) {
  Name(_ADR,0x0081FFFF)  // RAID Volume 1
  Name(_PR0, Package(){V1PR})
  Name(_PR3, Package(){V1PR})

  Method (_DSM, 4, Serialized, 0, UnknownObj, {BuffObj, IntObj, IntObj, PkgObj}) {
    Return(\_SB.PC00.SAT0.RSTD(Arg0,Arg1,Arg2,Arg3,RefOf(\_SB.PC00.SAT0.VL1M)))
  }

  PowerResource(V1PR, 0, 0) { // Power Resource Aggregate for RAID volume 1
    Name(_STA, 0x01)

    Method(_ON, 0)  {
      If(VR3A) {
        \_SB.PC00.SAT0.CPON(VL1M)
        Store(0x1, _STA)
      } Else {
      }
    }

    Method(_OFF, 0) {
      If (VR3A) {
        \_SB.PC00.SAT0.CPOF(VL1M)
        Store(0x0, _STA)
      } Else {
      }
    }
  }
}

Device(VOL2) {
  Name(_ADR,0x0082FFFF)  // RAID Volume 2
  Name(_PR0, Package(){V2PR})
  Name(_PR3, Package(){V2PR})

  Method (_DSM, 4, Serialized, 0, UnknownObj, {BuffObj, IntObj, IntObj, PkgObj}) {
    Return(\_SB.PC00.SAT0.RSTD(Arg0,Arg1,Arg2,Arg3,RefOf(\_SB.PC00.SAT0.VL2M)))
  }

  PowerResource(V2PR, 0, 0) { // Power Resource Aggregate for RAID volume 2
    Name(_STA, 0x01)

    Method(_ON, 0)  {
      If (VR3A) {
        \_SB.PC00.SAT0.CPON(VL2M)
        Store(0x1, _STA)
      } Else {
      }
    }

    Method(_OFF, 0) {
      If (VR3A) {
        \_SB.PC00.SAT0.CPOF(VL2M)
        Store(0x0, _STA)
      } Else {
      }
    }
  }
}

