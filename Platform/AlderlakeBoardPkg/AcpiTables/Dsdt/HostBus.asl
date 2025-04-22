/** @file

  Copyright (c) 2021 - 2022, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

External(M64B)
External(M64L)
External(M32B)
External(M32L)
//External(MPGN)
External(ITSP)
External(SWAK, MethodObj)
External (\_SB.PC00.PEG0.CPRA, MethodObj)
External (\_SB.PC00.PEG1.CPRA, MethodObj)
External (\_SB.PC00.PEG2.CPRA, MethodObj)
External (\_SB.PC00.PEG3.CPRA, MethodObj)

Device (MC)
{
  Name(_ADR,0)
  //
  // Define various System Agent (SA) PCI Configuration Space
  // registers which will be used to dynamically produce all
  // resources in the Host Bus _CRS.
  //
  OperationRegion (HBUS, PCI_Config, 0x00, 0x100)
  Field (HBUS, DWordAcc, NoLock, Preserve)
  {
    Offset(0x40),   // EPBAR (0:0:0:40)
    EPEN, 1,        // Enable
        , 11,
    EPBR, 20,       // EPBAR [31:12]

    Offset(0x48),   // MCHBAR (0:0:0:48)
    MHEN, 1,        // Enable
        , 14,
    MHBR, 17,       // MCHBAR [31:15]

    Offset(0x50),   // GGC (0:0:0:50)
    GCLK, 1,        // GGCLCK

    Offset(0x54),   // DEVEN (0:0:0:54)
    D0EN, 1,        // DEV0 Enable
    D1F2, 1,        // DEV1 FUN2 Enable
    D1F1, 1,        // DEV1 FUN1 Enable
    D1F0, 1,        // DEV1 FUN0 Enable
        , 9,
    D6F0, 1,        // DEV6 FUN0 Enable

    Offset(0x60),   // PCIEXBAR (0:0:0:60)
    PXEN, 1,        // Enable
    PXSZ, 3,        // PCI Express Size
        , 22,
    PXBR, 6,        // PCI Express BAR [31:26]

    Offset(0x68),   // DMIBAR (0:0:0:68)
    DIEN, 1,        // Enable
        , 11,
    DIBR, 20,       // DMIBAR [31:12]

    Offset(0x70),   // MESEG_BASE (0:0:0:70)
        , 20,
    MEBR, 12,       // MESEG_BASE [31:20]

    Offset(0x80),   // PAM0 Register (0:0:0:80)
        , 4,
    PM0H, 2,        // PAM 0, High Nibble
        , 2,

    Offset(0x81),   // PAM1 Register (0:0:0:81)
    PM1L, 2,        // PAM1, Low  Nibble
        , 2,
    PM1H, 2,        // PAM1, High Nibble
        , 2,

    Offset(0x82),   // PAM2 Register (0:0:0:82)
    PM2L, 2,        // PAM2, Low  Nibble
        , 2,
    PM2H, 2,        // PAM2, High Nibble
        , 2,

    Offset(0x83),   // PAM3 Register (0:0:0:83)
    PM3L, 2,        // PAM3, Low  Nibble
        , 2,
    PM3H, 2,        // PAM3, High Nibble
        , 2,

    Offset(0x84),   // PAM4 Register (0:0:0:84)
    PM4L, 2,        // PAM4, Low  Nibble
        , 2,
    PM4H, 2,        // PAM4, High Nibble
        , 2,

    Offset(0x85),   // PAM5 Register (0:0:0:85)
    PM5L, 2,        // PAM5, Low  Nibble
        , 2,
    PM5H, 2,        // PAM5, High Nibble
        , 2,

    Offset(0x86),   // PAM6 Register (0:0:0:86)
    PM6L, 2,        // PAM6, Low  Nibble
        , 2,
    PM6H, 2,        // PAM6, High Nibble
        , 2,

    Offset(0xA8),   // Top of Upper Usable DRAM Register (0:0:0:A8)
        , 20,
    TUUD, 19,       // TOUUD [38:20]

    Offset(0xBC),   // Top of Lower Usable DRAM Register (0:0:0:BC)
        , 20,
    TLUD, 12,       // TOLUD [31:20]

    Offset(0xC8),   // ERRSTS register (0:0:0:C8)
        , 7,
    HTSE, 1         // Host Thermal Sensor Event for SMI/SCI/SERR
  }
} // End MC
//
// Define a buffer that will store all the bus, memory, and IO information
// relating to the Host Bus.  This buffer will be dynamically altered in
// the _CRS and passed back to the OS.
//
Name(BUF0,ResourceTemplate()
{
  //
  // Bus Number Allocation: Bus 0 to 0xFF
  //
  WORDBusNumber(ResourceProducer,MinFixed,MaxFixed,PosDecode,0x00,
    0x0000,0x00FF,0x00,0x0100,,,PB00)

  //
  // I/O Region Allocation 0 ( 0x0000 - 0x0CF7 )
  //
  DWordIo(ResourceProducer,MinFixed,MaxFixed,PosDecode,EntireRange,
    0x00,0x0000,0x0CF7,0x00,0x0CF8,,,PI00)

  //
  // PCI Configuration Registers ( 0x0CF8 - 0x0CFF )
  //
  Io(Decode16,0x0CF8,0x0CF8,1,0x08)

  //
  // I/O Region Allocation 1 ( 0x0D00 - 0xFFFF )
  //
  DWordIo(ResourceProducer,MinFixed,MaxFixed,PosDecode,EntireRange,
    0x00,0x0D00,0xFFFF,0x00,0xF300,,,PI01)

  //
  // Video Buffer Area ( 0xA0000 - 0xBFFFF )
  //
  DWordMemory(ResourceProducer,PosDecode,MinFixed,MaxFixed,Cacheable,
    ReadWrite,0x00,0xA0000,0xBFFFF,0x00,0x20000,,,A000)

  //
  // ISA Add-on BIOS Area ( 0xC0000 - 0xC3FFF )
  //
  DWordMemory(ResourceProducer,PosDecode,MinFixed,MaxFixed,Cacheable,
    ReadWrite,0x00,0xC0000,0xC3FFF,0x00,0x4000,,,C000)

  //
  // ISA Add-on BIOS Area ( 0xC4000 - 0xC7FFF )
  //
  DWordMemory(ResourceProducer,PosDecode,MinFixed,MaxFixed,Cacheable,
    ReadWrite,0x00,0xC4000,0xC7FFF,0x00,0x4000,,,C400)

  //
  // ISA Add-on BIOS Area ( 0xC8000 - 0xCBFFF )
  //
  DWordMemory(ResourceProducer,PosDecode,MinFixed,MaxFixed,Cacheable,
    ReadWrite,0x00,0xC8000,0xCBFFF,0x00,0x4000,,,C800)

  //
  // ISA Add-on BIOS Area ( 0xCC000 - 0xCFFFF )
  //
  DWordMemory(ResourceProducer,PosDecode,MinFixed,MaxFixed,Cacheable,
    ReadWrite,0x00,0xCC000,0xCFFFF,0x00,0x4000,,,CC00)

  //
  // ISA Add-on BIOS Area ( 0xD0000 - 0xD3FFF )
  //
  DWordMemory(ResourceProducer,PosDecode,MinFixed,MaxFixed,Cacheable,
    ReadWrite,0x00,0xD0000,0xD3FFF,0x00,0x4000,,,D000)

  //
  // ISA Add-on BIOS Area ( 0xD4000 - 0xD7FFF )
  //
  DWordMemory(ResourceProducer,PosDecode,MinFixed,MaxFixed,Cacheable,
    ReadWrite,0x00,0xD4000,0xD7FFF,0x00,0x4000,,,D400)

  //
  // ISA Add-on BIOS Area ( 0xD8000 - 0xDBFFF )
  //
  DWordMemory(ResourceProducer,PosDecode,MinFixed,MaxFixed,Cacheable,
    ReadWrite,0x00,0xD8000,0xDBFFF,0x00,0x4000,,,D800)

  //
  // ISA Add-on BIOS Area ( 0xDC000 - 0xDFFFF )
  //
  DWordMemory(ResourceProducer,PosDecode,MinFixed,MaxFixed,Cacheable,
    ReadWrite,0x00,0xDC000,0xDFFFF,0x00,0x4000,,,DC00)

  //
  // BIOS Extension Area ( 0xE0000 - 0xE3FFF )
  //
  DWordMemory(ResourceProducer,PosDecode,MinFixed,MaxFixed,Cacheable,
    ReadWrite,0x00,0xE0000,0xE3FFF,0x00,0x4000,,,E000)

  //
  // BIOS Extension Area ( 0xE4000 - 0xE7FFF )
  //
  DWordMemory(ResourceProducer,PosDecode,MinFixed,MaxFixed,Cacheable,
    ReadWrite,0x00,0xE4000,0xE7FFF,0x00,0x4000,,,E400)

  //
  // BIOS Extension Area ( 0xE8000 - 0xEBFFF )
  //
  DWordMemory(ResourceProducer,PosDecode,MinFixed,MaxFixed,Cacheable,
    ReadWrite,0x00,0xE8000,0xEBFFF,0x00,0x4000,,,E800)

  //
  // BIOS Extension Area ( 0xEC000 - 0xEFFFF )
  //
  DWordMemory(ResourceProducer,PosDecode,MinFixed,MaxFixed,Cacheable,
    ReadWrite,0x00,0xEC000,0xEFFFF,0x00,0x4000,,,EC00)

  //
  // BIOS Area ( 0xF0000 - 0xFFFFF )
  //
  DWordMemory(ResourceProducer,PosDecode,MinFixed,MaxFixed,Cacheable,
    ReadWrite,0x00,0xF0000,0xFFFFF,0x00,0x10000,,,F000)

//  //
//  // Memory Hole Region ( 0xF00000 - 0xFFFFFF )
//  //
//  DWordMemory(ResourceProducer,PosDecode,MinFixed,MaxFixed,Cacheable,
//    ReadWrite,0x00,0xF00000,0xFFFFFF,0x00,0x100000,,,HOLE)

  //
  // PCI Memory Region ( TOLUD - 0xDFFFFFFF )
  //
  DWordMemory(ResourceProducer,PosDecode,MinFixed,MaxFixed,NonCacheable,
    ReadWrite,0x00,0x00000000,0xDFFFFFFF,0x00,0xE0000000,,,PM01)

  //
  // PCI Memory Region ( TOUUD - (TOUUD + ABOVE_4G_MMIO_SIZE) )
  // (This is dummy range for OS compatibility, will patch it in _CRS)
  //
  QWordMemory(ResourceProducer,PosDecode,MinFixed,MaxFixed,NonCacheable,
    ReadWrite,0x00,0x10000,0x1FFFF,0x00,0x10000,,,PM02)

})

Name(EP_B, 0) // to store EP BAR
Name(MH_B, 0) // to store MCH BAR
Name(PC_B, 0) // to store PCIe BAR
Name(PC_L, 0) // to store PCIe BAR Length
Name(DM_B, 0) // to store DMI BAR

//
// Get EP BAR
//
Method(GEPB,0,Serialized)
{
  if(LEqual(EP_B,0))
  {
    ShiftLeft(\_SB.PC00.MC.EPBR,12,EP_B)
  }
  Return(EP_B)
}

//
// Get MCH BAR
//
Method(GMHB,0,Serialized)
{
  if(LEqual(MH_B,0))
  {
    ShiftLeft(\_SB.PC00.MC.MHBR,15,MH_B)
  }
  Return(MH_B)
}

//
// Get PCIe BAR
//
Method(GPCB,0,Serialized)
{
  if(LEqual(PC_B,0))
  {
    ShiftLeft(\_SB.PC00.MC.PXBR,26,PC_B)
  }
  Return(PC_B)
}

//
// PCIe root port Pci Cfg space to MMIO address
// Converts PCI device address from _ADR method to physical memory address
// Arg0 - _ADR object of the device that will be accessed
// _ADR returns Integer containing the address of the device
// High word Device Number, Low word Function Number
// (for example, device 3, function 2 is 0x00030002). To refer to all the functions on a device #, use a function number of FFFF).
// Host Memory Space address:
// Bus Number      [8 bits] [27:20]
// Device Number   [5 bits] [19:15]
// Function Number [3 bits] [14:12]
// Register Number [10 bits][ 9:0 ]
//
Method (PC2M, 1, Serialized) {     // Convert _ADR to PCI MMIO address.
  Store (\_SB.PC00.GPCB(), Local0) // MMIO Base address
  Add (Local0, ShiftRight(And(Arg0, 0x001F0000), 1), Local0) // Device no [19:15]
  Add (Local0, ShiftLeft(And(Arg0, 0x00000007), 12), Local0) // Function no [14:12]
  Return (Local0)
}

//
// Get PCIe Length
//
Method(GPCL,0,Serialized)
{
  if(LEqual(PC_L,0)) {
    if(LLess (\_SB.PC00.MC.PXSZ, 3)) {
      ShiftRight(0x10000000, \_SB.PC00.MC.PXSZ,PC_L)
    } Else {
      Store(0x10000000, PC_L) // it's for calculating Bus Number of the Segment, so the MAX value is 256MB for 256 Buses
    }
  }
  Return(PC_L)
}

//
// Get DMI BAR
//
Method(GDMB,0,Serialized)
{
  if(LEqual(DM_B,0))
  {
    ShiftLeft(\_SB.PC00.MC.DIBR,12,DM_B)
  }
  Return(DM_B)
}


Method(_CRS,0,Serialized)
{
  //
  // Fix up Max Bus Number and Length
  //
  Store(\_SB.PC00.GPCL(),Local0)
  CreateWordField(BUF0, ^PB00._MAX, PBMX)
  Store(Subtract(ShiftRight(Local0,20),2), PBMX)
  CreateWordField(BUF0, ^PB00._LEN, PBLN)
  Store(Subtract(ShiftRight(Local0,20),1), PBLN)
  //
  // Fix up all of the Option ROM areas from 0xC0000-0xFFFFF.
  //
  If(^MC.PM1L)  // \_SB.PC00
  {
    // PAMx != 0.  Set length = 0.

    CreateDwordField(BUF0, ^C000._LEN,C0LN)
    Store(Zero,C0LN)
  }

  If(LEqual(^MC.PM1L,1))
  {
    CreateBitField(BUF0, ^C000._RW,C0RW)
    Store(Zero,C0RW)
  }

  If(^MC.PM1H)
  {
    CreateDwordField(BUF0, ^C400._LEN,C4LN)
    Store(Zero,C4LN)
  }

  If(LEqual(^MC.PM1H,1))
  {
    CreateBitField(BUF0, ^C400._RW,C4RW)
    Store(Zero,C4RW)
  }

  If(^MC.PM2L)
  {
    CreateDwordField(BUF0, ^C800._LEN,C8LN)
    Store(Zero,C8LN)
  }

  If(LEqual(^MC.PM2L,1))
  {
    CreateBitField(BUF0, ^C800._RW,C8RW)
    Store(Zero,C8RW)
  }

  If(^MC.PM2H)
  {
    CreateDwordField(BUF0, ^CC00._LEN,CCLN)
    Store(Zero,CCLN)
  }

  If(LEqual(^MC.PM2H,1))
  {
    CreateBitField(BUF0, ^CC00._RW,CCRW)
    Store(Zero,CCRW)
  }

  If(^MC.PM3L)
  {
    CreateDwordField(BUF0, ^D000._LEN,D0LN)
    Store(Zero,D0LN)
  }

  If(LEqual(^MC.PM3L,1))
  {
    CreateBitField(BUF0, ^D000._RW,D0RW)
    Store(Zero,D0RW)
  }

  If(^MC.PM3H)
  {
    CreateDwordField(BUF0, ^D400._LEN,D4LN)
    Store(Zero,D4LN)
  }

  If(LEqual(^MC.PM3H,1))
  {
    CreateBitField(BUF0, ^D400._RW,D4RW)
    Store(Zero,D4RW)
  }

  If(^MC.PM4L)
  {
    CreateDwordField(BUF0, ^D800._LEN,D8LN)
    Store(Zero,D8LN)
  }

  If(LEqual(^MC.PM4L,1))
  {
    CreateBitField(BUF0, ^D800._RW,D8RW)
    Store(Zero,D8RW)
  }

  If(^MC.PM4H)
  {
    CreateDwordField(BUF0, ^DC00._LEN,DCLN)
    Store(Zero,DCLN)
  }

  If(LEqual(^MC.PM4H,1))
  {
    CreateBitField(BUF0, ^DC00._RW,DCRW)
    Store(Zero,DCRW)
  }

  If(^MC.PM5L)
  {
    CreateDwordField(BUF0, ^E000._LEN,E0LN)
    Store(Zero,E0LN)
  }

  If(LEqual(^MC.PM5L,1))
  {
    CreateBitField(BUF0, ^E000._RW,E0RW)
    Store(Zero,E0RW)
  }

  If(^MC.PM5H)
  {
    CreateDwordField(BUF0, ^E400._LEN,E4LN)
    Store(Zero,E4LN)
  }

  If(LEqual(^MC.PM5H,1))
  {
    CreateBitField(BUF0, ^E400._RW,E4RW)
    Store(Zero,E4RW)
  }

  If(^MC.PM6L)
  {
    CreateDwordField(BUF0, ^E800._LEN,E8LN)
    Store(Zero,E8LN)
  }

  If(LEqual(^MC.PM6L,1))
  {
    CreateBitField(BUF0, ^E800._RW,E8RW)
    Store(Zero,E8RW)
  }

  If(^MC.PM6H)
  {
    CreateDwordField(BUF0, ^EC00._LEN,ECLN)
    Store(Zero,ECLN)
  }

  If(LEqual(^MC.PM6H,1))
  {
    CreateBitField(BUF0, ^EC00._RW,ECRW)
    Store(Zero,ECRW)
  }

  If(^MC.PM0H)
  {
    CreateDwordField(BUF0, ^F000._LEN,F0LN)
    Store(Zero,F0LN)
  }

  If(LEqual(^MC.PM0H,1))
  {
    CreateBitField(BUF0, ^F000._RW,F0RW)
    Store(Zero,F0RW)
  }

  // Enable the 1MB region between 15-16MB if HENA = 1.
  //
  // If( MCHC.HENA)
  // {
  // CreateDwordField(BUF0, HOLE._LEN,H0LN)
  // Store(0x100000,H0LN)
  // }

  //
  // Create pointers to Memory Sizing values.
  //
  CreateDwordField(BUF0, ^PM01._MIN,M1MN)
  CreateDwordField(BUF0, ^PM01._MAX,M1MX)
  CreateDwordField(BUF0, ^PM01._LEN,M1LN)

  //
  // Set Memory Size Values. TLUD represents bits 31:20 of phyical
  // TOM, so shift these bits into the correct position and fix up
  // the Memory Region available to PCI.
  //
  Store (M32L, M1LN)
  Store (M32B, M1MN)
  Subtract (Add (M1MN, M1LN), 1, M1MX)

  //
  // Create pointers to Memory Sizing values.
  // Patch PM02 range basing on memory size and OS type
  //
  If (LEqual(M64L, 0)) {
    CreateQwordField(BUF0, ^PM02._LEN,MSLN)
    //
    // Set resource length to 0
    //
    Store (0, MSLN)
  }
  Else {
    CreateQwordField(BUF0, ^PM02._LEN,M2LN)
    CreateQwordField(BUF0, ^PM02._MIN,M2MN)
    CreateQwordField(BUF0, ^PM02._MAX,M2MX)
    //
    // Set 64bit MMIO resource Base and Length
    //
    Store (M64L, M2LN)
    Store (M64B, M2MN)
    Subtract (Add (M2MN, M2LN), 1, M2MX)
  }
  Return(BUF0)
}

//
//Name(GUID,UUID("33DB4D5B-1FF7-401C-9657-7441C03DD766"))
//
Name(GUID,Buffer(){0x5b, 0x4d, 0xdb, 0x33,
          0xf7, 0x1f,
          0x1c, 0x40,
          0x96, 0x57,
          0x74, 0x41, 0xc0, 0x3d, 0xd7, 0x66})


Name(SUPP,0)  // PCI _OSC Support Field value
Name(CTRL,0)  // PCI _OSC Control Field value
Name(XCNT, 0) // Variable used in _OSC for counting

Method(_OSC,4,Serialized)
{
  //
  // Check for proper UUID
  // Save the capabilities buffer
  //
  Store(Arg3,Local0)

  //
  // Create DWord-adressable fields from the Capabilties Buffer
  //
  CreateDWordField(Local0,0,CDW1)
  CreateDWordField(Local0,4,CDW2)
  CreateDWordField(Local0,8,CDW3)

  //
  // Check for proper UUID
  //
  If(LEqual(Arg0,GUID))
  {
    // Save Capabilities DWord2 & 3
    Store(CDW2,SUPP)
    Store(CDW3,CTRL)

    //
    // You can clear bits in CTRL here if you don't want OS to take
    // control
    //
    If(LNot(NEXP))
    {
      And(CTRL, 0xFFFFFFF8, CTRL)       // disable Native hot plug, PME
    }

    If(CondRefOf(ITSP)) {
      If(ITSP) {
        // \_OSC disallow only Advanced Error Reporting control
        And(CTRL, 0xFFFFFFF7, CTRL)
      }
    }

    If(Not(And(CDW1,1)))  // Query flag clear?
    { // Disable GPEs for features granted native control.
      Store(CTRL,OSCC)
      If (CondRefOf(SWAK)) {
        SWAK (0xFFFF, 0x0) //Reserved wake
      }
    }

    If(LNotEqual(Arg1,One))
    {
      //
      // Unknown revision
      //
      Or(CDW1,0x08,CDW1)
    }

    If(LNotEqual(CDW3,CTRL))
    {
      //
      // Capabilities bits were masked
      //
      Or(CDW1,0x10,CDW1)
    }
    //
    // Update DWORD3 in the buffer
    //
    Store(CTRL,CDW3)
    Store(CTRL,OSCC)
    Return(Local0)
  } Else {
    Or(CDW1,4,CDW1)   // Unrecognized UUID
    Return(Local0)
  }
} // End _OSC

//
// Added code for Dual IRQ support. Two set of ACPI IRQ tables were generated.
// Code has been added to select the appropriate IRQ table by checking the CPUID.
//
Scope(\_SB.PC00)
{
  Method(AR00) {
    Return(\_SB.AR00)
  }

  Method(PD00) {
    Return(\_SB.PD00)
  }

  Method(AR04) {
    Return(\_SB.AR04)
  }

  Method(PD04) {
    Return(\_SB.PD04)
  }

  Method(AR05) {
    Return(\_SB.AR05)
  }

  Method(PD05) {
    Return(\_SB.PD05)
  }

  Method(AR06) {
    Return(\_SB.AR06)
  }

  Method(PD06) {
    Return(\_SB.PD06)
  }

  Method(AR07) {
    Return(\_SB.AR07)
  }

  Method(PD07) {
    Return(\_SB.PD07)
  }

  Method(AR08) {
    Return(\_SB.AR08)
  }

  Method(PD08) {
    Return(\_SB.PD08)
  }

  Method(AR09) {
    Return(\_SB.AR09)
  }

  Method(PD09) {
    Return(\_SB.PD09)
  }

  //
  // Add device scope definition for System Agent
  // P.E.G. Root Port D6F0
  //
  Device(PEG0) {
    Method (_ADR, 0) {
      Store (0x00060000, local0) // Initialize the Address with hardcoded value
      If (CondRefOf (\_SB.PC00.PEG0.CPRA)) {
        Store (\_SB.PC00.PEG0.CPRA (), local0)
      }
      Return (local0)
    }
    Device(PEGP) { // P.E.G. Port Slot x4
      Name(_ADR, 0x00000000)
    }
  }
  //
  // P.E.G. Root Port D1F0
  //
  Device(PEG1) {
    Method (_ADR, 0) {
      Store (0x00010000, local0) // Initialize the Address with hardcoded value
      If (CondRefOf (\_SB.PC00.PEG1.CPRA)) {
        Store (\_SB.PC00.PEG1.CPRA (), local0)
      }
      Return (local0)
    }
    Device(PEGP) { // P.E.G. Port Slot x16
      Name(_ADR, 0x00000000)
    }
  }
  //
  // P.E.G. Root Port D1F1
  //
  Device(PEG2) {
    Method (_ADR, 0) {
      Store (0x00060002, local0) // Initialize the Address with hardcoded value
      If (CondRefOf (\_SB.PC00.PEG2.CPRA)) {
        Store (\_SB.PC00.PEG2.CPRA (), local0)
      }
      Return (local0)
    }
    Device(PEGP) { // P.E.G. Port Slot x8
      Name(_ADR, 0x00000000)
    }
  }
  //If (LGreaterEqual(MPGN, 4)) {
    //
    // P.E.G. Root Port D1F2
    //
    Device(PEG3) {
      Method (_ADR, 0) {
        Store (0x00010002, local0) // Initialize the Address with hardcoded value
        If (CondRefOf (\_SB.PC00.PEG3.CPRA)) {
          Store (\_SB.PC00.PEG3.CPRA (), local0)
        }
        Return (local0)
      }
      Device(PEGP) { // P.E.G. Port Slot x4
        Name(_ADR, 0x00000000)
      }
    }
  //}
  //
  // I.G.D
  //
  Device(GFX0) {
    Name(_ADR, 0x00020000)
  }
  //
  // SA Thermal Device
  //
  Device(TCPU) {
    Method(_DSM,4,serialized){if(PCIC(Arg0)) { return(PCID(Arg0,Arg1,Arg2,Arg3)) }; Return(Buffer() {0})}
    Name(_ADR, 0x00040000)
  }
  //
  // Device IPU0 is the IPU PCI device
  //
  Device(IPU0) {
    Name(_ADR, 0x00050000)
  }
  //
  // Device GNA D8F0
  //
  Device(GNA0) {
    Name(_ADR, 0x00080000)
  }
}

Scope(\)
{
  //
  //
  //  Intel ACPI Debug utility wrapper method to communicate ACPI debug messages.
  //  Method by default will output to Kernel debugger. OEM can implement their own
  //  version on ODBG wrapper call.
  //  Arguments:
  //    Arg0:  any string or value that needs to be passed to debugger agent.
  //
  //  Return Value:
  //    Error if string cannot be processed
  //
  //
  Method(ADBG,1,Serialized)
  {
    //SDBG(Arg0)
    Return(0)
  }
}
