/** @file
  ACPI Sata PUIS Enablement

  Copyright (c) 2018 - 2019, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

Name (GTF0, Buffer (0x7) {0x00,0x00,0x00,0x00,0x00,0xA0,0x00})

//
// Set Device Data Method
// Arg0 - A Buffer containing an ATA drive identify block, contents described by the ATA
//
Method (_SDD, 0x1, NotSerialized) {
  CreateByteField(GTF0, 0x0, FEAT)
  CreateByteField(GTF0, 0x6, CMMD)
  CreateWordField(Arg0, 0xA6, W083)  // Identify Device Data Word 83
  CreateWordField(Arg0, 0xAC, W086)  // Identify Device Data Word 86

  If (LEqual(SizeOf(Arg0), 0x200)) {  // 256 WORD Data Block by OSPM to send 0xEC Command to Device
    // Check if PUIS feature is supported (Word 83 Bit5)
    If (LEqual (And (W083, 0x0020), 0x0020)) {
      // Check enable state (Word 86 Bit5)
      If (LEqual (And (W086, 0x0020), 0)) {
        // Issue Set Feature EFh, subcommand 06h to enable PUIS
        Store (0x06, FEAT)
        Store (0xEF, CMMD)
      }
    }
  }
}

//
// Get Task File Method
//
// @return 0 - A Buffer containing a byte stream of ATA commands for the drive
//
Method(_GTF, 0, NotSerialized) {
  return (GTF0)

}
