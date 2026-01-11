/** @file
  SoundWire topology definitions

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

Name (_DSD, Package ()  // _DSD: Device-Specific Data
{
  ToUUID ("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),  //Device Properties for _DSD
  Package ()
  {
    Package (2) {"mipi-sdw-sw-interface-revision", 0x00020001},  // v2.1
    Package (2) {"mipi-sdca-control-list", 0xF000000301F2}, // Bitmap: 0x1, 0x4, 0x5, 0x6, 0x7, 0x8, 0x10, 0x11, 0x2C, 0x2D, 0x2E, 0x2F - Function level controls
    Package (2) {"mipi-sdca-entity-id-list", Package() {0x1} },
  },

  ToUUID("dbb8e3e6-5886-4ba6-8795-1319f52a966b"),
  Package ()
  {
    Package (2) {"mipi-sdca-control-0x1-subproperties", "C042"},//Commit_Group_Mask, Class, RW
    Package (2) {"mipi-sdca-control-0x4-subproperties", "C004"},//Function_SDCA_Version
    Package (2) {"mipi-sdca-control-0x5-subproperties", "C005"},//Function_Topology
    Package (2) {"mipi-sdca-control-0x6-subproperties", "C006"},//Function_ManufacturerId
    Package (2) {"mipi-sdca-control-0x7-subproperties", "C007"},//Function_ID
    Package (2) {"mipi-sdca-control-0x8-subproperties", "C008"},//Function_Version
    Package (2) {"mipi-sdca-control-0x10-subproperties", "C043"},//Function_Status, Class, RW1C
    Package (2) {"mipi-sdca-control-0x11-subproperties", "C044"},//Function_Action, Class, RW1S
    Package (2) {"mipi-sdca-control-0x2C-subproperties", "C02C"},//Device_Manufacturer_ID
    Package (2) {"mipi-sdca-control-0x2D-subproperties", "C02D"},//Device_Part_ID
    Package (2) {"mipi-sdca-control-0x2E-subproperties", "C02E"},//Device_Version
    Package (2) {"mipi-sdca-control-0x2F-subproperties", "C02F"},//Device_SDCA_Version
    Package (2) {"mipi-sdca-entity-id-0x1-subproperties", "E001"},
  },

  ToUUID("edb12dd0-363d-4085-a3d2-49522ca160c4"),
  Package()
  {
    Package (2) {"mipi-sdca-function-initialization-table", "BUF0"},
    Package (2) {"mipi-sdca-hid-descriptor", "BUF1"},
    Package (2) {"mipi-sdca-report-descriptor", "BUF2"},
  }
}) //End _DSD

Name(BUF0, Buffer()
{
  0x64, 0x20, 0x10, 0x06, 0xCF,  // 0x06102064, 0xCF
  0x64, 0x00, 0x10, 0x06, 0x00,  // 0x06100064, 0x00

  0x65, 0x20, 0x10, 0x06, 0x00,  // 0x06102065, 0x00
  0x65, 0x00, 0x10, 0x06, 0x0F,  // 0x06100065, 0x0F

  0x60, 0x20, 0x10, 0x06, 0x11,  // 0x06102060, 0x11
  0x60, 0x00, 0x10, 0x06, 0x00,  // 0x06100060, 0x00

  0x67, 0x20, 0x10, 0x06, 0x0C,  // 0x06102067, 0x0C
  0x67, 0x00, 0x10, 0x06, 0x12,  // 0x06100067, 0x12
}) //End BUF0

Name(BUF1, Buffer()
{
  0x09,   // size of decriptor
  0x21,   // HID descriptor type
  0x11,   // HID class spec release number BCD LO
  0x01,   // HID class spec release number BCD HI
  0x00,   // country code
  0x01,   // number of HID class descripotr to follow
  0x22,   // Report Type
  0x33,   // CLASS DESC[0] LENGTH Lo
  0x00,   // CLASS DESC[0] LENGTH Hi
}) //End BUF1

Name(BUF2, Buffer()
{
  0x05, 0x0C,       // USAGE_PAGE (Consumer devices)
  0x09, 0x01,       // USAGE (Consumer Control)
  0xa1, 0x01,       // COLLECTION (Application)
      0x85, 0x11,   // REPORT_ID

    0x09, 0xe9,   // USAGE (Volume up) 08
    0x15, 0x00,   // LOGICAL_MINIMUM (0)
    0x25, 0x01,   // LOGICAL_MAXIMUM (1)
    0x75, 0x01,   // REPORT_SIZE (1)
    0x95, 0x01,   // REPORT_COUNT (1)
    0x81, 0x02,   // INPUT (Data, Var, Abs)

    0x09, 0xea,   // USAGE (Volume down) 09
    0x15, 0x00,   // LOGICAL_MINIMUM (0)
    0x25, 0x01,   // LOGICAL_MAXIMUM (1)
    0x75, 0x01,   // REPORT_SIZE (1)
    0x95, 0x01,   // REPORT_COUNT (1)
    0x81, 0x02,   // INPUT (Data, Var, Abs)

    0x09, 0xcd,   // USAGE (play/pause) 10
    0x15, 0x00,   // LOGICAL_MINIMUM (0)
    0x25, 0x01,   // LOGICAL_MAXIMUM (1)
    0x75, 0x01,   // REPORT_SIZE (1)
    0x95, 0x01,   // REPORT_COUNT (1)
    0x81, 0x06,   // INPUT (Data, Var, Relative)

    0x75, 0x0D,   // REPORT_SIZE (1) 11~23
    0x95, 0x01,   // REPORT_COUNT (1)
    0x81, 0x03,   // INPUT (Cnst,Ary,Abs)
  0xC0,             // MAIN::END_COLLECTION

}) //End BUF2

Name(C042, Package()
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package()
  {
    //Commit_Group_Mask, Class, RW
    Package(2) {"mipi-sdca-control-access-layer", 0x4},
    Package(2) {"mipi-sdca-control-access-mode", 0},
  }
}) //End AF04.C042

Name(C043, Package()
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package()
  {
    //Function_Status, Class, RW1C
    Package(2) {"mipi-sdca-control-access-layer", 0x4},
    Package(2) {"mipi-sdca-control-access-mode", 2},
    Package(2) {"mipi-sdca-control-interrupt-position", 10}, //IntStat_SDCA_10
  }
}) //End AF04.C043

Name(C044, Package()
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package()
  {
    //Function_Action, Class, RW1S
    Package(2) {"mipi-sdca-control-access-layer", 0x4},
    Package(2) {"mipi-sdca-control-access-mode", 4},
  }
}) //End AF04.C044

Name(C02C, Package()
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package()
  {
    //Device_Manufacturer_ID = 0x025d, Class, DC
    Package(2) {"mipi-sdca-control-access-layer", 0x4},
    Package(2) {"mipi-sdca-control-access-mode", 5},
    Package(2) {"mipi-sdca-control-dc-value", 0x025D},
  }
}) //End AF04.C02C

Name(C02D, Package()
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package()
  {
    //Device_Part_ID = 0x722, Class, DC
    Package(2) {"mipi-sdca-control-access-layer", 0x4},
    Package(2) {"mipi-sdca-control-access-mode", 5},
    Package(2) {"mipi-sdca-control-dc-value", 0x0722},
  }
}) //End AF04.C02D

Name(C02E, Package()
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package()
  {
    //Device_Version, Class, DC
    Package(2) {"mipi-sdca-control-access-layer", 0x4},
    Package(2) {"mipi-sdca-control-access-mode", 5},
    Package(2) {"mipi-sdca-control-dc-value", 0x1},
  }
}) //End AF04.C02E

Name(C02F, Package()
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package()
  {
    //Device_SDCA_Version, Class, DC
    Package(2) {"mipi-sdca-control-access-layer", 0x4},
    Package(2) {"mipi-sdca-control-access-mode", 5},
    Package(2) {"mipi-sdca-control-dc-value", 0x10},
  }
}) //End AF04.C02F

Name(C004, Package()
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package()
  {
    // Function_SDCA_Version = 1.0, Class, DC
    Package(2) {"mipi-sdca-control-access-layer", 4},
    Package(2) {"mipi-sdca-control-access-mode", 5},
    Package(2) {"mipi-sdca-control-dc-value", 0x10},
  }
}) // End C004

Name(C005, Package()
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package()
  {
    // Function_Topology = 0xA HID, Class, DC
    Package(2) {"mipi-sdca-control-access-layer", 4},
    Package(2) {"mipi-sdca-control-access-mode", 5},
    Package(2) {"mipi-sdca-control-dc-value", 0xA},
  }
}) // End C005

Name(C006, Package()
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package()
  {
    Package(2) {"mipi-sdca-control-access-layer", 4},
    Package(2) {"mipi-sdca-control-access-mode", 5},
    Package(2) {"mipi-sdca-control-dc-value", 0x025d},
  }
}) // End C006

Name(C007, Package()
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package()
  {
    Package(2) {"mipi-sdca-control-access-layer", 4},
    Package(2) {"mipi-sdca-control-access-mode", 5},
    Package(2) {"mipi-sdca-control-dc-value", 0x0722},
  }
}) // End C007

Name(C008, Package()
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package()
  {
    // Function_Version = 0x1, Class, DC
    Package(2) {"mipi-sdca-control-access-layer", 4},
    Package(2) {"mipi-sdca-control-access-mode", 5},
    Package(2) {"mipi-sdca-control-dc-value", 0x1},
  }
}) // End C008

Name(E001, Package()
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package ()
  {
    Package (2) {"mipi-sdca-entity-type", 0x31},
    //Package (2) {"mipi-sdca-control-list", Package() {0x10, 0x11, 0x12, 0x13} },
    Package (2) {"mipi-sdca-control-list", 0x000D0000},  // Bitmap: 0x10, 0x11, 0x12, 0x13
    Package (2) {"mipi-sdca-hide-related-audio-function-list", Package() {0x1} },   // sending/receiving the messages in this HIDE to/from AF01(UAJ)
    Package (2) {"mipi-sdca-HIDTx-supported-report-ids", Package() {0x11} },   // HIDE 1 TX Routing Table
  },
  ToUUID("dbb8e3e6-5886-4ba6-8795-1319f52a966b"),
  Package ()
  {
    Package (2) {"mipi-sdca-control-0x10-subproperties", "CS10"},
    Package (2) {"mipi-sdca-control-0x12-subproperties", "CS12"},
    Package (2) {"mipi-sdca-control-0x13-subproperties", "CS13"},
  }
}) //End E001

Name(CS10, Package()
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package ()
  {  // HIDTx_CurrentOwner, Class, RW1S
    Package (2) {"mipi-sdca-control-interrupt-position", 8},// bit 8 in SDCA interrupt
    Package (2) {"mipi-sdca-control-access-layer", 4},
    Package (2) {"mipi-sdca-control-access-mode", 4},
  }
}) //End CS10

Name(CS12, Package()
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package ()
  {  // HIDTx_MessageOffset, Class, RO
    Package (2) {"mipi-sdca-control-access-layer", 4},
    Package (2) {"mipi-sdca-control-access-mode", 3},
  },

  ToUUID("edb12dd0-363d-4085-a3d2-49522ca160c4"),
  Package ()
  {
    Package (2) {"mipi-sdca-control-range", "BUF3"},
  }
}) //End CS12

Name(BUF3, Buffer()
{
  0x03, 0x00, // Range type 0x0003
  0x01, 0x00, // Count of ranges = 0x1
  0x00, 0x00, 0x03, 0x44, // Buffer Start = 0x44030000
  0x18, 0x00, 0x00, 0x00, // Buffer Length = 0x18
  0x00, 0x00, 0x00, 0x00, // UMP Type = 0 (Direct)
}) //End BUF3

Name(CS13, Package()
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package ()
  {
    // HIDTx_MessageLength, Class, RO
    Package (2) {"mipi-sdca-control-access-layer", 4},
    Package (2) {"mipi-sdca-control-access-mode", 3},
  }
}) //End CS13
