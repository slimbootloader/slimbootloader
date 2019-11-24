/** @file
  ACPI DSDT table


  Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

DefinitionBlock (
  "DSDT.aml",
  "DSDT",
  0x02,       // DSDT revision.
              // A Revision field value greater than or equal to 2 signifies that integers
              // Declared within the Definition Block are to be evaluated as 64-bit values
  "INTEL",    // OEM ID (6 byte string)
  "BXT-SOC",  // OEM table ID  (8 byte string)
  0x0         // OEM version of DSDT table (4 byte Integer)
)

// BEGIN OF ASL SCOPE
{
  // Miscellaneous services enabled in Project
  include ("Token.asl")
  include ("AmlUpd.asl")
  include ("GloblNvs.asl")
  include ("PciTree.asl")
  include ("Sc.asl")
  include ("LpcB.asl")
  include ("Bxt.asl")
  include ("Cpu.asl")
  include ("Platform.asl")
  include ("Thermal.asl")
  include ("PciDrc.asl")
  include ("Gpe.asl") //Need this for xHCI D3 wake flow.

  include ("Cpu0Cst.asl")
  include ("ApCst.asl")
  include ("CpuSsdt.asl")
  include ("Cpu0Ist.asl")
  include ("ApIst.asl")

  include ("Gfx.asl")

  #include "FwuWmi.asl"

// Sleep states supported by Chipset/Board.
//----------------------------------------------------------------------
// SSx - BIOS setup controlled enabled _Sx Sleep state status
// Values to be written to SLP_TYPE register are provided by SBACPI.SDL (South Bridge ACPI ModulePart)

  Name(\_S0, Package(4) {0x0,0x0,0,0}) // mandatory System state
  if(SS1) { Name(\_S1, Package(4){0x1,0x0,0,0})}
  if(SS3) { Name(\_S3, Package(4){0x5,0x0,0,0})}
  if(SS4) { Name(\_S4, Package(4){0x6,0x0,0,0})}
  Name(\_S5, Package(4) {0x7,0x0,0,0}) // mandatory System state

}// End of ASL File
