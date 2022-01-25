/** @file

  Copyright (c) 2021, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/


Scope (\_SB.PC00){

  Device(PDRC)
  {
    //
    // PCI Device Resource Consumption
    //
    Name(_HID,EISAID("PNP0C02"))

    Name(_UID,1)

    Name(BUF0,ResourceTemplate()
    {
      //
      // MCH BAR _BAS will be updated in _CRS below according to B0:D0:F0:Reg.48h
      //
      Memory32Fixed(ReadWrite,0,0x08000,MCHB)
      //
      // DMI BAR _BAS will be updated in _CRS below according to B0:D0:F0:Reg.68h
      //
      Memory32Fixed(ReadWrite,0,0x01000,DMIB)
      //
      // EP BAR _BAS will be updated in _CRS below according to B0:D0:F0:Reg.40h
      //
      Memory32Fixed(ReadWrite,0,0x01000,EGPB)
      //
      // PCI Express BAR _BAS and _LEN will be updated in _CRS below according to B0:D0:F0:Reg.60h
      //
      Memory32Fixed(ReadWrite,0,0,PCIX)

      //
      // PCH TXT space (Private / Public / TPM / DbC TraceHub / other)
      // Check if the hard code meets the real configuration.
      // If not, dynamically update it like the _CRS method below.
      //
      Memory32Fixed(ReadWrite,0xFED20000,0x60000)

      //
      // VTD engine memory range. Check if the hard code meets the real configuration.
      // If not, dynamically update it like the _CRS method below.
      //
      Memory32Fixed(ReadOnly, 0xFED90000, 0x00004000)

      //
      // MISC ICH. Check if the hard code meets the real configuration.
      // If not, dynamically update it like the _CRS method below.
      //
      Memory32Fixed(ReadWrite,0xFED45000,0x4B000,TPMM)

      //
      // Local APIC range(0xFEE0_0000 to 0xFEEF_FFFF)
      //
      Memory32Fixed (ReadOnly, 0xFEE00000, 0x100000, LIOH)

      //
      // Sx handler reserved MMIO
      //
      Memory32Fixed (ReadWrite, 0, 0, SXRE)

      //
      // Reserve HPET address decode range
      //
      Memory32Fixed (ReadWrite, 0, 0, HPET)
    })


    Method(_CRS,0,Serialized)
    {
      CreateDwordField(BUF0,^MCHB._BAS,MBR0)
      Store(\_SB.PC00.GMHB(), MBR0)

      CreateDwordField(BUF0,^DMIB._BAS,DBR0)
      Store(\_SB.PC00.GDMB(), DBR0)

      CreateDwordField(BUF0,^EGPB._BAS,EBR0)
      Store(\_SB.PC00.GEPB(), EBR0)

      CreateDwordField(BUF0,^PCIX._BAS,XBR0)
      Store(\_SB.PC00.GPCB(), XBR0)

      CreateDwordField(BUF0,^PCIX._LEN,XSZ0)
      Store(\_SB.PC00.GPCL(), XSZ0)

      // HPET device claims the resource in LpcDev.asl.
      If(LNOT(HPTE)){
        CreateDwordField(BUF0,^HPET._BAS,HBAS)
        CreateDwordField(BUF0,^HPET._LEN,HLEN)
        Store(HPTB, HBAS)
        Store(0x400, HLEN)
      }

      Return(BUF0)
    }
  } //end of PDRC
} // end of SB
