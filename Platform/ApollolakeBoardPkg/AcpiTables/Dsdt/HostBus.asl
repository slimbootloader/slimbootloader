/**@file

  Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

Device(VLVC)
{
  Name(_ADR, 0x00000000)    // Device 0, Function 0

  // Define various MCH Controller PCI Configuration Space
  // registers which will be used to dynamically produce all
  // resources in the Host Bus _CRS.

  OperationRegion(HBUS, PCI_Config, 0x00, 0xFF)
  Field(HBUS, DWordAcc, NoLock, Preserve)
  {
    Offset(0x48),           // MCHBAR (0:0:0:48)
    MHEN, 1,                // Enable
        , 14,
    MHBR, 24,               // MCHBAR [38:15]
    Offset(0xB0),
    BDSM,   32,             // Base of Data Stolen Memory
    BGSM,   32,             // Base of Graphics Stolen Memory
    Offset(0xBC),
    TOLD,   32,             // Top of Low Useable DRAM
  }
}

//
// BUS, I/O, and MMIO resources
//
Method(_CRS,0,Serialized) {
  //
  // Create pointers to PCI MMIO values
  //
  CreateDwordField(RES0, ^PM01._MIN, M1MN)
  CreateDwordField(RES0, ^PM01._MAX, M1MX)
  CreateDwordField(RES0, ^PM01._LEN, M1LN)

  //
  // Set Memory Size Values. TLUD represents bits 31:20 of phyical
  // TOM, so shift these bits into the correct position and fix up
  // the Memory Region available to PCI.
  //
  Store (M32L, M1LN)
  Store (M32B, M1MN)
  Subtract (Add (M1MN, M1LN), 1, M1MX)

  //
  // Create pointers to Base of Data Stolen Memory Values
  //
  CreateDwordField(RES0, ^DSM1._MIN, GSMN)
  CreateDwordField(RES0, ^DSM1._MAX, GSMX)
  CreateDwordField(RES0, ^DSM1._LEN, GSLN)

  // Read C-Unit PCI CFG Reg. 0xB0 for BDSM
  Store(^VLVC.BDSM, GSMN)

  // Read C-Unit PCI CFG Reg. 0xBC for TOLUD
  And(^VLVC.TOLD, 0xFFFFF000, GSMX)
  Decrement(GSMX)

  Add(Subtract(GSMX, GSMN), 1, GSLN)

  //
  // Create pointers to Base of Graphics Stolen Memory Values
  //
  CreateDwordField(RES0, ^GSM1._MIN, GDMN)
  CreateDwordField(RES0, ^GSM1._MAX, GDMX)
  CreateDwordField(RES0, ^GSM1._LEN, GDLN)

  // Read C-Unit PCI CFG Reg. 0xB4 for BGSM
  Store(^VLVC.BGSM, GDMN)

  // Read C-Unit PCI CFG Reg. 0xBC for TOLUD
  And(^VLVC.BDSM, 0xFFFFF000, GDMX)
  Decrement(GDMX)

  Add(Subtract(GDMX, GDMN), 1, GDLN)

  Return(RES0)
}

Name( RES0,ResourceTemplate() {
  WORDBusNumber (          // Bus number resource (0); the bridge produces bus numbers for its subsequent buses
          ResourceProducer,      // bit 0 of general flags is 1
          MinFixed,              // Range is fixed
          MaxFixed,              // Range is fixed
          PosDecode,             // PosDecode
          0x0000,                // Granularity
          0x0000,                // Min
          0x00FF,                // Max
          0x0000,                // Translation
          0x0100                 // Range Length = Max-Min+1
          )

  IO (Decode16, 0x70, 0x77, 0x01, 0x08)         //Consumed resource (0xCF8-0xCFF)
  IO (Decode16, 0xCF8, 0xCF8, 0x01, 0x08)       //Consumed resource (0xCF8-0xCFF)

  WORDIO (                 // Consumed-and-produced resource (all I/O below CF8)
          ResourceProducer,      // bit 0 of general flags is 0
          MinFixed,              // Range is fixed
          MaxFixed,              // Range is fixed
          PosDecode,
          EntireRange,
          0x0000,                // Granularity
          0x0000,                // Min
          0x006F,                // Max
          0x0000,                // Translation
          0x0070                 // Range Length
          )

  WORDIO (                 // Consumed-and-produced resource
          ResourceProducer,      // bit 0 of general flags is 0
          MinFixed,              // Range is fixed
          MaxFixed,              // Range is fixed
          PosDecode,
          EntireRange,
          0x0000,                // Granularity
          0x0078,                // Min
          0x0CF7,                // Max
          0x0000,                // Translation
          0x0C80                 // Range Length
          )

  WORDIO (                 // Consumed-and-produced resource (all I/O above CFF)
          ResourceProducer,      // bit 0 of general flags is 0
          MinFixed,              // Range is fixed
          MaxFixed,              // Range is fixed
          PosDecode,
          EntireRange,
          0x0000,                // Granularity
          0x0D00,                // Min
          0xFFFF,                // Max
          0x0000,                // Translation
          0xF300                 // Range Length
          )

  DWORDMEMORY (            // Descriptor for legacy VGA video RAM
          ResourceProducer,      // bit 0 of general flags is 0
          PosDecode,
          MinFixed,              // Range is fixed
          MaxFixed,              // Range is fixed
          Cacheable,
          ReadWrite,
          0x00000000,            // Granularity
          0x000A0000,            // Min
          0x000BFFFF,            // Max
          0x00000000,            // Translation
          0x00020000             // Range Length
          ,,,
          LGB1
          )

  DWORDMEMORY (            // Descriptor for legacy OptionRom
          ResourceProducer,      // bit 0 of general flags is 0
          PosDecode,
          MinFixed,              // Range is fixed
          MaxFixed,              // Range is fixed
          Cacheable,
          ReadWrite,
          0x00000000,            // Granularity
          0x000C0000,            // Min
          0x000DFFFF,            // Max
          0x00000000,            // Translation
          0x00020000             // Range Length
          ,,,
          LGB2
          )

  DWORDMEMORY (                  // Descriptor for BIOS Area
          ResourceProducer,      // bit 0 of general flags is 0
          PosDecode,
          MinFixed,              // Range is fixed
          MaxFixed,              // Range is fixed
          Cacheable,
          ReadWrite,
          0x00000000,            // Granularity
          0x000E0000,            // Min
          0x000FFFFF,            // Max
          0x00000000,            // Translation
          0x00020000             // Range Length
          ,,,
          LGB3
          )

  DWORDMEMORY (                  // Descriptor for Base of Data Stolen Memory
          ResourceProducer,      // bit 0 of general flags is 0
          PosDecode,
          MinFixed,              // Range is fixed
          MaxFixed,              // Range is fixed
          Cacheable,
          ReadWrite,
          0x00000000,            // Granularity
          0x3be00000,            // Min
          0x3FFFFFFF,            // Max
          0x00000000,            // Translation
          0x04200000             // Range Length
          ,,,
          DSM1
          )

  DWORDMEMORY (                  // Descriptor for Base of Graphics Stolen Memory
          ResourceProducer,      // bit 0 of general flags is 0
          PosDecode,
          MinFixed,              // Range is fixed
          MaxFixed,              // Range is fixed
          Cacheable,
          ReadWrite,
          0x00000000,            // Granularity
          0x3be00000,            // Min
          0x3FFFFFFF,            // Max
          0x00000000,            // Translation
          0x04200000             // Range Length
          ,,,
          GSM1
          )

  DWORDMEMORY (                  // Descriptor for PCI MMIO
          ResourceProducer,      // bit 0 of general flags is 0
          PosDecode,
          MinFixed,              // Range is fixed
          MaxFixed,              // Range is fixed
          NonCacheable,
          ReadWrite,
          0x00000000,            // Granularity
          0x80000000,            // Min
          0xBFFFFFFF,            // Max
          0x00000000,            // Translation
          0x40000000             // Range Length
          ,,,
          PM01
          )

  DWORDMEMORY (                  // Descriptor for PCI CFG Space
          ResourceProducer,      // bit 0 of general flags is 0
          PosDecode,
          MinFixed,              // Range is fixed
          MaxFixed,              // Range is fixed
          NonCacheable,
          ReadWrite,
          0x00000000,            // Granularity
          0xE0000000,            // Min
          0xEFFFFFFF,            // Max
          0x00000000,            // Translation
          0x10000000             // Range Length
          ,,,
          PM02
          )

  QWORDMEMORY (                  // Descriptor for PCI CFG Space
          ResourceProducer,      // bit 0 of general flags is 0
          PosDecode,
          MinFixed,              // Range is fixed
          MaxFixed,              // Range is fixed
          NonCacheable,
          ReadWrite,
          0x00000000,            // Granularity
          0x400000000,            // Min
          0x7FFFFFFFF,            // Max
          0x000000000,            // Translation
          0x400000000             // Range Length
          )

})

Name(GUID,Buffer(){0x5b, 0x4d, 0xdb, 0x33,
          0xf7, 0x1f,
          0x1c, 0x40,
          0x96, 0x57,
          0x74, 0x41, 0xc0, 0x3d, 0xd7, 0x66})


Name(SUPP,0)  // PCI _OSC Support Field value
Name(CTRL,0)  // PCI _OSC Control Field value

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
  If (LAnd(LEqual(Arg0,GUID),NEXP)) {
    // Save Capabilities DWord2 & 3
    Store(CDW2,SUPP)
    Store(CDW3,CTRL)

    If (Not(And(CDW1,1))) {
      // Query flag clear?
      // Disable GPEs for features granted native control.
      If (And(CTRL,0x01)) {
        NHPG()
      }
      If (And(CTRL,0x04)) {
        // PME control granted?
        NPME()
      }
    }

    If (LNotEqual(Arg1,One)) {
      //
      // Unknown revision
      //
      Or(CDW1,0x08,CDW1)
    }

    If (LNotEqual(CDW3,CTRL)) {
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
