/** @file

  Copyright (c) 1999 - 2014, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

Module Name:
  IgdOpRegion.h
Abstract:
  This is part of the implementation of an Intel Graphics drivers OpRegion /
  Software SCI interface between system BIOS, ASL code, and Graphics drivers.
  Supporting Specification: OpRegion / Software SCI SPEC 0.70
  Acronyms:
    IGD:        Internal Graphics Device
    NVS:        ACPI Non Volatile Storage
    OpRegion:   ACPI Operational Region
    VBT:        Video BIOS Table (OEM customizable data)
**/

#ifndef _IGD_OPREGION_PROTOCOL_H_
#define _IGD_OPREGION_PROTOCOL_H_

//
// OpRegion / Software SCI protocol GUID
//
#define IGD_OPREGION_PROTOCOL_GUID \
  { \
    0xcdc5dddf, 0xe79d, 0x41ec, 0xa9, 0xb0, 0x65, 0x65, 0x49, 0xd, 0xb9, 0xd3 \
  }

//
// Extern the GUID for protocol users.
//
extern EFI_GUID gIgdOpRegionProtocolGuid;

//
// Forward reference for pure ANSI compatability
//
typedef struct _IGD_OPREGION_PROTOCOL IGD_OPREGION_PROTOCOL;

//
// Protocol data definitions
//

//
// OpRegion structures:
// Sub-structures define the different parts of the OpRegion followed by the
// main structure representing the entire OpRegion.
//
// Note: These structures are packed to 1 byte offsets because the exact
// data location is requred by the supporting design specification due to
// the fact that the data is used by ASL and Graphics driver code compiled
// separatly.
//
#pragma pack(1)
///
/// OpRegion header (mailbox 0) structure and defines.
///
typedef struct {
  CHAR8   Sign[0x10]; ///< Offset 0    OpRegion signature
  UINT32  Size;       ///< Offset 16   OpRegion size
  UINT32  Over;       ///< Offset 20   OpRegion structure version
  UINT8   Sver[0x20]; ///< Offset 24   System BIOS build version
  UINT8   Vver[0x10]; ///< Offset 56   Video BIOS build version
  UINT8   Gver[0x10]; ///< Offset 72   Graphic driver build version
  UINT32  MBox;       ///< Offset 88   Mailboxes supported
  UINT32  DMod;       ///< Offset 92   Driver Model
  UINT32  PCon;       ///< Offset 96   Platform Capabilities
  CHAR16  Dver[0x10]; ///< Offset 100  GOP Version
  UINT8   Rhd1[0x7C]; ///< Offset 132  Reserved
} OPREGION_HEADER;
#pragma pack()

#pragma pack(1)
///
/// OpRegion mailbox 1 (public ACPI Methods).
///
typedef struct {
  UINT32  Drdy;       ///< Offset 0    Driver readiness
  UINT32  Csts;       ///< Offset 4    Status
  UINT32  Cevt;       ///< Offset 8    Current event
  UINT8   Rm11[0x14]; ///< Offset 12   Reserved
  UINT32  Didl;       ///< Offset 32   Supported display device 1
  UINT32  Ddl2;       ///< Offset 36   Supported display device 2
  UINT32  Ddl3;       ///< Offset 40   Supported display device 3
  UINT32  Ddl4;       ///< Offset 44   Supported display device 4
  UINT32  Ddl5;       ///< Offset 48   Supported display device 5
  UINT32  Ddl6;       ///< Offset 52   Supported display device 6
  UINT32  Ddl7;       ///< Offset 56   Supported display device 7
  UINT32  Ddl8;       ///< Offset 60   Supported display device 8
  UINT32  Cpdl;       ///< Offset 64   Currently present display device 1
  UINT32  Cpl2;       ///< Offset 68   Currently present display device 2
  UINT32  Cpl3;       ///< Offset 72   Currently present display device 3
  UINT32  Cpl4;       ///< Offset 76   Currently present display device 4
  UINT32  Cpl5;       ///< Offset 80   Currently present display device 5
  UINT32  Cpl6;       ///< Offset 84   Currently present display device 6
  UINT32  Cpl7;       ///< Offset 88   Currently present display device 7
  UINT32  Cpl8;       ///< Offset 92   Currently present display device 8
  UINT32  Cadl;       ///< Offset 96   Currently active display device 1
  UINT32  Cal2;       ///< Offset 100  Currently active display device 2
  UINT32  Cal3;       ///< Offset 104  Currently active display device 3
  UINT32  Cal4;       ///< Offset 108  Currently active display device 4
  UINT32  Cal5;       ///< Offset 112  Currently active display device 5
  UINT32  Cal6;       ///< Offset 116  Currently active display device 6
  UINT32  Cal7;       ///< Offset 120  Currently active display device 7
  UINT32  Cal8;       ///< Offset 124  Currently active display device 8
  UINT32  Nadl;       ///< Offset 128  Next active device 1
  UINT32  Ndl2;       ///< Offset 132  Next active device 2
  UINT32  Ndl3;       ///< Offset 136  Next active device 3
  UINT32  Ndl4;       ///< Offset 140  Next active device 4
  UINT32  Ndl5;       ///< Offset 144  Next active device 5
  UINT32  Ndl6;       ///< Offset 148  Next active device 6
  UINT32  Ndl7;       ///< Offset 152  Next active device 7
  UINT32  Ndl8;       ///< Offset 156  Next active device 8
  UINT32  Aslp;       ///< Offset 160  ASL sleep timeout
  UINT32  Tidx;       ///< Offset 164  Toggle table index
  UINT32  Chpd;       ///< Offset 168  Current hot plug enable indicator
  UINT32  Clid;       ///< Offset 172  Current lid state indicator
  UINT32  Cdck;       ///< Offset 176  Current docking state indicator
  UINT32  Sxsw;       ///< Offset 180  Display Switch notification on Sx State resume
  UINT32  Evts;       ///< Offset 184  Events supported by ASL
  UINT32  Cnot;       ///< Offset 188  Current OS Notification
  UINT32  Nrdy;       ///< Offset 192  Reasons for DRDY = 0
  UINT32  Ddl9;       ///< Offset 196  Extended Supported display device 1
  UINT32  Dd10;       ///< Offset 200  Extended Supported display device 2
  UINT32  Dd11;       ///< Offset 204  Extended Supported display device 3
  UINT32  Dd12;       ///< Offset 208  Extended Supported display device 4
  UINT32  Dd13;       ///< Offset 212  Extended Supported display device 5
  UINT32  Dd14;       ///< Offset 216  Extended Supported display device 6
  UINT32  Dd15;       ///< Offset 220  Extended Supported display device 7
  UINT32  Cpl9;       ///< Offset 224  Extended Currently present device 1
  UINT32  Cp10;       ///< Offset 228  Extended Currently present device 2
  UINT32  Cp11;       ///< Offset 232  Extended Currently present device 3
  UINT32  Cp12;       ///< Offset 236  Extended Currently present device 4
  UINT32  Cp13;       ///< Offset 240  Extended Currently present device 5
  UINT32  Cp14;       ///< Offset 244  Extended Currently present device 6
  UINT32  Cp15;       ///< Offset 248  Extended Currently present device 7
  UINT8   Rm12[0x4];  ///< Offset 252  Reserved 4 bytes
} OPREGION_MBOX1;
#pragma pack()

#pragma pack(1)
///
/// OpRegion mailbox 2 (Software SCI Interface).
///
typedef struct {
  UINT32  Scic;       ///< Offset 0    Software SCI function number parameters
  UINT32  Parm;       ///< Offset 4    Software SCI additional parameters
  UINT32  Dslp;       ///< Offset 8    Driver sleep timeout
  UINT8   Rm21[0xF4]; ///< Offset 12   Reserved
} OPREGION_MBOX2;
#pragma pack()

#pragma pack(1)
///
/// OpRegion mailbox 3 (Power Conservation).
///
typedef struct {
  UINT32  Ardy;       ///< Offset 0    Driver readiness
  UINT32  Aslc;       ///< Offset 4    ASLE interrupt command / status
  UINT32  Tche;       ///< Offset 8    Technology enabled indicator
  UINT32  Alsi;       ///< Offset 12   Current ALS illuminance reading
  UINT32  Bclp;       ///< Offset 16   Backlight britness to set
  UINT32  Pfit;       ///< Offset 20   Panel fitting Request
  UINT32  Cblv;       ///< Offset 24   Brightness Current State
  UINT16  Bclm[0x14]; ///< Offset 28   Backlight Brightness Level Duty Cycle Mapping Table
  UINT32  Cpfm;       ///< Offset 68   Panel Fitting Current Mode
  UINT32  Epfm;       ///< Offset 72   Enabled Panel Fitting Modes
  UINT8   Plut[0x4A]; ///< Offset 76   Panel Look Up Table
  UINT32  Pfmb;       ///< Offset 150  PWM Frequency and Minimum Brightness
  UINT32  Ccdv;       ///< Offset 154  Color Correction Default Values
  UINT32  Pcft;       ///< Offset 158  Power Conservation Features
  UINT32  Srot;       ///< Offset 162  Supported Rotation angle
  UINT32  Iuer;       ///< Offset 166  Intel Ultrabook Event Register
  UINT64  Fdsp;       ///< Offset 170  FFS Display Physical address
  UINT32  Fdss;       ///< Offset 178  FFS Display Size
  UINT32  Stat;       ///< Offset 182  State Indicator
  UINT64  Rvda;       ///< Offset 186  (Igd opregion offset 0x3BAh) Physical address of Raw VBT data
  UINT32  Rvds;       ///< Offset 194  (Igd opregion offset 0x3C2h) Size of Raw VBT data
  UINT8   Rmem[0x3A]; ///< Offset 198  Reserved
} OPREGION_MBOX3;
#pragma pack()

#pragma pack(1)
///
/// OpRegion mailbox 4 (VBT).
///
typedef struct {
  UINT8   Gvd1[0x1800]; ///< Offset 1024 6K Reserved
} OPREGION_VBT;
#pragma pack ()

#pragma pack (1)
typedef struct {
  UINT32  PHed;         ///< Offset 7168 Panel Header
  UINT8   BDdc[0x100];  ///< Offset 7172 Panel EDID
  UINT8   Rm51[0x2FC];  ///< Offset 7428 764 bytes
} OPREGION_MBOX5;
#pragma pack ()
//
// Entire OpRegion
//
#pragma pack (1)
typedef struct {
  OPREGION_HEADER Header; ///< OpRegion header
  OPREGION_MBOX1  MBox1;  ///< Mailbox 1: Public ACPI Methods
  OPREGION_MBOX2  MBox2;  ///< Mailbox 2: Software SCI Inteface
  OPREGION_MBOX3  MBox3;  ///< Mailbox 3: Power Conservation
  OPREGION_VBT    Vbt;    ///< VBT: Video BIOS Table (OEM customizable data)
  OPREGION_MBOX5  MBox5;  ///< Mailbox 5: BIOS to Driver Notification Extension
} IGD_OPREGION_STRUC;
#pragma pack()
///
/// IGD OpRegion Protocol
///
struct _IGD_OPREGION_PROTOCOL {
  IGD_OPREGION_STRUC  *OpRegion; ///< IGD Operation Region Structure
};

#endif
