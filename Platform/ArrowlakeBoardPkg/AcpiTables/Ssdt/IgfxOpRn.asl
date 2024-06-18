/** @file
  IGD OpRegion/Software SCI Reference Code.
  This file contains the interrupt handler code for the Integrated
  Graphics Device (IGD) OpRegion/Software SCI mechanism.
  It defines OperationRegions to cover the IGD PCI configuration space
  as described in the IGD OpRegion specification.

  Copyright (c) 2024, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/
//
//NOTES:
//
// (1) The code contained in this file inherits the scope in which it
//     was included.  So BIOS developers must be sure to include this
//     file in the scope associated with the graphics device
//     (ex. \_SB.PC00.GFX0).
// (2) The MCHP operation region assumes that _ADR and _BBN names
//     corresponding to bus 0, device0, function 0 have been declared
//     under the PC00 scope.
// (3) Before the first execution of any IGD Opregion related methods,
//     the base address of the IGD OpRegion must be programmed where the
//     driver can access it. A 32bit scratch register at 0xFC in the IGD PCI
//     configuration space (B0/D2/F0/R0FCh) is used for this purpose.

//
//  Define an OperationRegion to cover the IGD PCI configuration space as
//  described in the IGD OpRegion specificiation.
//
OperationRegion (IGDP, PCI_Config, 0x40, 0xC0)
Field (IGDP, AnyAcc, NoLock, Preserve)
{
  Offset (0xA4),
  ASLE, 8,       // Reg 0xE4, ASLE interrupt register
      , 24,      // Only use first byte of ASLE reg
  Offset (0xBC),
  ASLS, 32,      // Reg 0xFC, Address of the IGD OpRegion
}
//
//  Define an OperationRegion to cover the IGD OpRegion layout.
//
OperationRegion (IGDM, SystemMemory, ASLB, 0x2000)
Field (IGDM, AnyAcc, NoLock, Preserve)
{
  //
  // OpRegion Header
  //
  SIGN, 128,     // Signature-"IntelGraphicsMem"
  SIZE, 32,      // OpRegion Size
  OVER, 32,      // OpRegion Version
  SVER, 256,     // System BIOS Version
  VVER, 128,     // VBIOS Version
  GVER, 128,     // Driver version
  MBOX, 32,      // Mailboxes supported
  DMOD, 32,      // Driver Model
  PCON, 32,      // Platform Configuration
  DVER, 64,      // GOP Version
  //
  // OpRegion Mailbox 1 (Public ACPI Methods)
  // Note: Mailbox 1 is normally reserved for desktop platforms.
  //
  Offset (0x100),
  DRDY, 32,      // Driver readiness (ACPI notification)
  CSTS, 32,      // Notification status
  CEVT, 32,      // Current event
  Offset (0x120),
  DIDL, 32,      // Supported display device ID list
  DDL2, 32,      // Allows for 8 devices
  DDL3, 32,
  DDL4, 32,
  DDL5, 32,
  DDL6, 32,
  DDL7, 32,
  DDL8, 32,
  CPDL, 32,      // Obsolete : Currently present display list
  CPL2, 32,      // Obsolete : Allows for 8 devices
  CPL3, 32,      // Obsolete
  CPL4, 32,      // Obsolete
  CPL5, 32,      // Obsolete
  CPL6, 32,      // Obsolete
  CPL7, 32,      // Obsolete
  CPL8, 32,      // Obsolete
  CADL, 32,      // Obsolete : Currently active display list
  CAL2, 32,      // Obsolete : Allows for 8 devices
  CAL3, 32,      // Obsolete
  CAL4, 32,      // Obsolete
  CAL5, 32,      // Obsolete
  CAL6, 32,      // Obsolete
  CAL7, 32,      // Obsolete
  CAL8, 32,      // Obsolete
  NADL, 32,      // Obsolete : Next active display list
  NDL2, 32,      // Obsolete : Allows for 8 devices
  NDL3, 32,      // Obsolete
  NDL4, 32,      // Obsolete
  NDL5, 32,      // Obsolete
  NDL6, 32,      // Obsolete
  NDL7, 32,      // Obsolete
  NDL8, 32,      // Obsolete
  ASLP, 32,      // ASL sleep timeout
  TIDX, 32,      // Obsolete : Toggle table index
  CHPD, 32,      // Obsolete : Current hot plug enable indicator
  CLID, 32,      // Current lid state indicator
  CDCK, 32,      // Current docking state indicator
  SXSW, 32,      // Obsolete : Display switch notify on resume
  EVTS, 32,      // Obsolete : Events supported by ASL (diag only)
  CNOT, 32,      // Obsolete : Current OS notifications (diag only)
  NRDY, 32,
  //
  //Extended DIDL list
  //
  DDL9, 32,
  DD10, 32,
  DD11, 32,
  DD12, 32,
  DD13, 32,
  DD14, 32,
  DD15, 32,
  //
  // OpRegion Mailbox 2 (Backlight communication)
  //
  Offset (0x200),
  BCL1, 32,
  BCL2, 32,
  CBL1, 32,
  CBL2, 32,
  BCM1, 960,
  BCM2, 960,
  //
  // OpRegion Mailbox 3 (BIOS to Driver Notification)
  // Note: Mailbox 3 is normally reserved for desktop platforms.
  //
  Offset (0x300),
  ARDY, 32,      // Obsolete : Driver readiness (power conservation)
  ASLC, 32,      // Obsolete : ASLE interrupt command/status
  TCHE, 32,      // Obsolete : Technology enabled indicator
  ALSI, 32,      // Obsolete : Current ALS illuminance reading
  BCLP, 32,      // Obsolete : Backlight brightness
  PFIT, 32,      // Obsolete : Panel fitting state or request
  CBLV, 32,      // Obsolete : Current brightness level
  BCLM, 320,     // Obsolete : Backlight brightness level duty cycle mapping table
  CPFM, 32,      // Obsolete : Current panel fitting mode
  EPFM, 32,      // Obsolete : Enabled panel fitting modes
  PLUT, 592,     // Obsolete : Optional. 74-byte Panel LUT Table
  PFMB, 32,      // Obsolete : Optional. PWM Frequency and Minimum Brightness
  CCDV, 32,      // Obsolete : Optional. Gamma, Brightness, Contrast values.
  PCFT, 32,      // Obsolete : Optional. Power Conservation Features
  SROT, 32,      // Obsolete : Supported rotation angle.
  IUER, 32,      // Obsolete : Optional. Intel Ultrabook Event Register.
  FDSS, 64,      // Obsolete : Optional. FFS Display Physical address
  FDSP, 32,      // Obsolete : Optional. FFS Display Size
  STAT, 32,      // Obsolete : State Indicator
  RVDA, 64,      // Physical address of Raw VBT data
  RVDS, 32,      // Size of Raw VBT data
  //
  // OpRegion Mailbox 4 (VBT)
  //
  Offset (0x400),
  RVBT, 0xC000,  // 6K bytes maximum VBT image
  //
  // OpRegion Mailbox 5 (BIOS to Driver Notification Extension)
  //
  Offset (0x1C00),
  PHED, 32,      // Panel Header
  BDDC, 2048,    // Panel EDID (Max 256 bytes)

}

Include ("IgfxCommon.asl")    // IGD SCI mobile features
