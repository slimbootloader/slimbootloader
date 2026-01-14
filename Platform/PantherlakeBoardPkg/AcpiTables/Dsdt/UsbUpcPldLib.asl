/** @file

   Platform reference PLD Form Factor definition

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#define GPOS_BIT_OFFSET 87
#define USERVIS_BIT_OFFSET 64
//
// Platform reference PLD Form Factor definition
//
Scope (\_SB) {
  Name (PLDA, Package (1) {
    Buffer (0x14) {
      /* 0000 */   0x82, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      /* 0008 */   0x30, 0x1C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      /* 0010 */   0xFF, 0xFF, 0xFF, 0xFF
      }
  })
  Name (UPCA, Package (4) { 0x00, 0x00, 0x00000000, 0x00000000 } )

  //
  // Method for creating USB _UPC buffers
  //
  // Arg0: Connectable - Integer (BYTE)
  // Non-Zero value: Connectable
  //
  // Arg1: Type - Integer (BYTE)
  // 0x00: Type A connector
  // 0x01: Mini-AB connector
  // 0x02: ExpressCard
  // 0x03: USB 3 Standard-A connector
  // 0x04: USB 3 Standard-B connector
  // 0x05: USB 3 Micro-B connector
  // 0x06: USB 3 Micro-AB connector
  // 0x07: USB 3 Power-B connector
  // 0x08: Type C connector - USB2-only
  // 0x09: Type C connector - USB2 and SS with Switch
  // 0x0A: Type C connector - USB2 and SS without Switch
  // 0x0B - 0xFE: Reserved
  // 0xFF: Proprietary connector
  //
  // Arg2: USB-C Port Capabilities - Integer
  // Fields in this entry are valid only for a USB-C port (values 0x08, 0x09, or
  // 0x0A) described by the the host connector Type (above):
  // - Bits [1:0]: Retimer Count- number of retimer devices present on the board
  // between the Host Router and this Port (connector) The maximum value is 2
  // (10b). If present, the retimer devices apply to USB4, USB 3, and any Alternate
  // Modes supported.
  // - Bit [2]: PCI Express Tunneling supported. This bit is valid only for ports that
  // support USB4 or TBT3. If PCI Express Tunneling for USB4 is not supported
  // via the USB4 _OSC mechanism, then the value of this bit is indeterminate.
  // - Bit [3]: DisplayPort Alternative Mode (DP Alt Mode) supported. This bit is
  // required to be set for ports that support USB4 or Thunderbolt 3.
  // - Bit [4]: USB4 Supported.
  // - Bit [5] Thunderbolt 3 Alternate Mode (TBT3) supported.
  // All other bits Reserved and set to zero (0)
  //
  Method (TUPC, 3, Serialized) {
    // PCKG[3]: Reserved1 - Integer
    // This value is reserved and must be zero.
    Store (UPCA, Local0)
    Store (Arg0, Index (Local0, 0))
    Store (Arg1, Index (Local0, 1))
    Store (Arg2, Index (Local0, 2))
    return (Local0)
  }

  //
  // Method for creating USB _PLD buffers
  // _PLD contains lots of data, but for purpose of WHLK validation we care only about
  // ports visibility and pairing (this requires group position)
  // so these are the only 2 configurable parameters
  //
  // Arg0:  Visible
  // Arg1:  Group position
  //
  Method (TPLD, 2, Serialized) {
    Store (PLDA, Local0)

    CreateField (DerefOf (Index (Local0, 0)), USERVIS_BIT_OFFSET, 1, VISI)
    Store (Arg0, VISI)
    CreateField (DerefOf (Index (Local0, 0)), GPOS_BIT_OFFSET, 8, GPOS)
    Store (Arg1, GPOS)
    return (Local0)
  }
} // end Scope (\)
