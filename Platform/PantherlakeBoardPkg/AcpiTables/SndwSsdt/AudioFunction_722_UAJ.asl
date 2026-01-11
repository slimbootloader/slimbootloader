/** @file
  SoundWire topology definitions

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

Name(_DSD, Package()
{  // Function Descriptor
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package ()
  {
    Package (2) {"mipi-sdw-sw-interface-revision", 0x00020001}, // v2.1
    //Package (2) {"mipi-sdca-function-topology-features", 0x0080000000000000}, // Security/Privacy(bit-55)
    Package (2) {"mipi-sdca-function-topology-features", 0x0000000000000000}, // No option
    Package (2) {"mipi-sdw-clockstopprepare-timeout", 0x4E5}, // 335ms + 54ms + 785ms*1.1
    //Package (2) {"mipi-sdca-control-list",  Package() {0x4, 0x5, 0x6, 0x7, 0x8} }, // Function level controls
    Package (2) {"mipi-sdca-control-list", 0xF000000301F2}, // Bitmap: 0x1, 0x4, 0x5, 0x6, 0x7, 0x8, 0x10, 0x11, 0x2C, 0x2D, 0x2E, 0x2F - Function level controls
    //Package (2) {"mipi-sdca-entity-id-list",
    //  Package() {0x1, 0x2, 0x3, 0x42, 0x5, 0x45, 0x46, 0x47, 0x40, 0x6, 0x7, 0x48,
    //  0x41, 0x49, 0x12, 0x8, 0x9, 0x43, 0xA, 0xB, 0x44, 0xC, 0xD, 0xF, 0x10, 0x11, 0x50, 0x51, 0x20, 0x21, 0x22, 0x23} }, // List of entities in this function (with PDE 42/36/199, and Security/Privacy)
    //Package (2) {"mipi-sdca-entity-id-list",
    //  Package() {0x1, 0x2, 0x3, 0x42, 0x5, 0x45, 0x46, 0x47, 0x40, 0x6, 0x7, 0x48,
    //  0x41, 0x49, 0x12, 0x8, 0x9, 0x43, 0xA, 0xB, 0x44, 0xC, 0xD, 0xF, 0x10, 0x11, 0x20, 0x21, 0x23} }, // List of entities in this function (with Security/Privacy)
    Package (2) {"mipi-sdca-entity-id-list",
      Package() {0x1, 0x2, 0x3, 0x42, 0x5, 0x45, 0x46, 0x47, 0x40, 0x6, 0x7, 0x48,
      0x41, 0x49, 0x12, 0x8, 0x9, 0x43, 0xA, 0xB, 0x44, 0xC, 0xD, 0xF, 0x10, 0x11} }, // List of entities in this function
    // Cluster Library
    Package (2) {"mipi-sdca-cluster-id-list", Package() {0x11, 0x12} },
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
    Package (2) {"mipi-sdca-entity-id-0x2-subproperties", "E002"},
    Package (2) {"mipi-sdca-entity-id-0x3-subproperties", "E003"},
    Package (2) {"mipi-sdca-entity-id-0x42-subproperties", "E042"},
    //Package (2) {"mipi-sdca-entity-id-0x50-subproperties", "E050"},
    Package (2) {"mipi-sdca-entity-id-0x5-subproperties", "E005"},
    Package (2) {"mipi-sdca-entity-id-0x45-subproperties", "E045"},
    Package (2) {"mipi-sdca-entity-id-0x46-subproperties", "E046"},
    Package (2) {"mipi-sdca-entity-id-0x47-subproperties", "E047"},
    Package (2) {"mipi-sdca-entity-id-0x40-subproperties", "E040"},
    Package (2) {"mipi-sdca-entity-id-0x6-subproperties", "E006"},
    Package (2) {"mipi-sdca-entity-id-0x7-subproperties", "E007"},
    Package (2) {"mipi-sdca-entity-id-0x48-subproperties", "E048"},
    Package (2) {"mipi-sdca-entity-id-0x41-subproperties", "E041"},
    Package (2) {"mipi-sdca-entity-id-0x49-subproperties", "E049"},
    Package (2) {"mipi-sdca-entity-id-0x12-subproperties", "E012"},
    Package (2) {"mipi-sdca-entity-id-0x8-subproperties", "E008"},
    Package (2) {"mipi-sdca-entity-id-0x9-subproperties", "E009"},
    Package (2) {"mipi-sdca-entity-id-0x43-subproperties", "E043"},
    Package (2) {"mipi-sdca-entity-id-0xA-subproperties", "E00A"},
    Package (2) {"mipi-sdca-entity-id-0xB-subproperties", "E00B"},
    Package (2) {"mipi-sdca-entity-id-0x44-subproperties", "E044"},
    Package (2) {"mipi-sdca-entity-id-0xC-subproperties", "E00C"},
    Package (2) {"mipi-sdca-entity-id-0xD-subproperties", "E00D"},
    //Package (2) {"mipi-sdca-entity-id-0x51-subproperties", "E051"},
    Package (2) {"mipi-sdca-entity-id-0xF-subproperties", "E00F"},
    Package (2) {"mipi-sdca-entity-id-0x10-subproperties", "E010"},
    Package (2) {"mipi-sdca-entity-id-0x11-subproperties", "E011"},
    //Package (2) {"mipi-sdca-entity-id-0x20-subproperties", "E020"},
    //Package (2) {"mipi-sdca-entity-id-0x21-subproperties", "E021"},
    //Package (2) {"mipi-sdca-entity-id-0x22-subproperties", "E022"},
    //Package (2) {"mipi-sdca-entity-id-0x23-subproperties", "E023"},
    Package (2) {"mipi-sdca-function-expansion-subproperties", "EXT0"},
    // Cluster ID subproperties
    Package (2) {"mipi-sdca-cluster-id-0x11-subproperties", "CL11"},
    Package (2) {"mipi-sdca-cluster-id-0x12-subproperties", "CL12"},
  },
  ToUUID("edb12dd0-363d-4085-a3d2-49522ca160c4"),
  Package()
  {
    Package (2) {"mipi-sdca-function-initialization-table", "BUF0"},
  }
}) //End AF01.DSD

// By Programming guide, Application 3. Version: 0.08 (2023/07/13)
Name(BUF0, Buffer()
{
  0x00, 0x20, 0xC1, 0x05, 0x48, // 0x05C12000, 0x48  //E-fuse setting 20241129 to fix headphone pop
  0x00, 0x00, 0xC1, 0x05, 0x08, // 0x05C10000, 0x08  //E-fuse setting

  //0x30, 0x00, 0x81, 0x05, 0xBD, // 0x05810030, 0xBD  //test 0314
  //0x30, 0x00, 0x81, 0x05, 0x00, // 0x05810030, 0x00

  0x00, 0x20, 0x81, 0x05, 0xF0, // 0x05812000, 0xF0  //calibration headphone and class-D
  0x00, 0x00, 0x81, 0x05, 0x2D, // 0x05810000, 0x2D

  0x04, 0x20, 0x00, 0x02, 0xA0, // 0x02002004, 0xA0
  0x04, 0x00, 0x00, 0x02, 0x01, // 0x02000004, 0x01

  0x46, 0x20, 0x10, 0x06, 0xA0, // 0x06102046, 0xA0
  0x46, 0x00, 0x10, 0x06, 0x09, // 0x06100046, 0x09

  0x64, 0x20, 0x10, 0x06, 0xCF, // 0x06102064, 0xCF
  0x64, 0x00, 0x10, 0x06, 0x00, // 0x06100064, 0x00

  0x65, 0x20, 0x10, 0x06, 0x00, // 0x06102065, 0x00
  0x65, 0x00, 0x10, 0x06, 0x0F, // 0x06100065, 0x0F

  0x60, 0x20, 0x10, 0x06, 0x11, // 0x06102060, 0x11
  0x60, 0x00, 0x10, 0x06, 0x00, // 0x06100060, 0x00

  0x67, 0x20, 0x10, 0x06, 0x0C, // 0x06102067, 0x0C
  0x67, 0x00, 0x10, 0x06, 0x12, // 0x06100067, 0x12

  0x09, 0x20, 0x00, 0x02, 0x70, // 0x02002009, 0x70
  0x09, 0x00, 0x00, 0x02, 0x02, // 0x02000009, 0x02

  0x0A, 0x20, 0x00, 0x02, 0x77, // 0x0200200A, 0x77
  0x0A, 0x00, 0x00, 0x02, 0x70, // 0x0200000A, 0x70

  0x3C, 0x20, 0x00, 0x02, 0xC2, // 0x0200203C, 0xC2
  0x3C, 0x00, 0x00, 0x02, 0x15, // 0x0200003C, 0x15

  0x67, 0x20, 0x00, 0x02, 0x41, // 0x02002067, 0x41
  0x67, 0x00, 0x00, 0x02, 0x00, // 0x02000067, 0x00

  //0x03, 0x20, 0x80, 0x05, 0x00, // 0x05802003, 0x00
  //0x03, 0x00, 0x80, 0x05, 0x8D, // 0x05800003, 0x8D   // 20241203 cancel this part for headphone pop fix

  0x4A, 0x20, 0x00, 0x02, 0x00, // 0x0200204A, 0x00
  0x4A, 0x00, 0x00, 0x02, 0x10, // 0x0200004A, 0x10

  0x25, 0x20, 0x10, 0x06, 0x2A, // 0x06102025, 0x2A
  0x25, 0x00, 0x10, 0x06, 0x12, // 0x06100025, 0x12

  0x23, 0x20, 0x10, 0x06, 0x34, // 0x06102023, 0x34
  0x23, 0x00, 0x10, 0x06, 0x29, // 0x06100023, 0x29

  0x24, 0x20, 0x10, 0x06, 0x41, // 0x06102024, 0x41
  0x24, 0x00, 0x10, 0x06, 0x12, // 0x06100024, 0x12

  0x22, 0x20, 0x10, 0x06, 0x40, // 0x06102022, 0x40
  0x22, 0x00, 0x10, 0x06, 0x40, // 0x06100022, 0x40

  0x76, 0x20, 0x10, 0x06, 0x41, // 0x06102076, 0x41
  0x76, 0x00, 0x10, 0x06, 0x41, // 0x06100076, 0x41

  0x70, 0x20, 0x10, 0x06, 0x01, // 0x06102070, 0x01
  0x70, 0x00, 0x10, 0x06, 0x01, // 0x06100070, 0x01

  0x03, 0x2F, 0x00, 0x00, 0x06, // 0x00002F03, 0x06 (PHYtiming TDZ/TZD control)

  0x45, 0x20, 0x00, 0x02, 0x52, // 0x02002045, 0x52 (Use two VrefO)
  0x45, 0x00, 0x00, 0x02, 0x81, // 0x02000045, 0x81

  0x63, 0x20, 0x00, 0x02, 0xC0, // 0x02002063, 0xC0
  0x63, 0x00, 0x00, 0x02, 0x3C, // 0x02000063, 0x3C

  0x58, 0x2F, 0x00, 0x00, 0x05, // 0x00002F58, 0x05   //Entity 28 power change to 131ms
}) //End AF01.BUF0

//==============================================================================================

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
    Package(2) {"mipi-sdca-control-interrupt-position", 3}, //IntStat_SDCA_3
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
  Package ()
  {
    // Function_SDCA_Version = 1.0, Class, DC
    Package (2) {"mipi-sdca-control-access-layer", 0x4},
    Package (2) {"mipi-sdca-control-access-mode", 5},
    Package (2) {"mipi-sdca-control-dc-value", 0x10},
  }
}) //End AF01.C004

Name(C005, Package()
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package ()
  {
    // Function_Topology = 0x6 UAJ, Class, DC
    Package (2) {"mipi-sdca-control-access-layer", 0x4},
    Package (2) {"mipi-sdca-control-access-mode", 5},
    Package (2) {"mipi-sdca-control-dc-value", 0x6},
  }
}) //End AF01.C005

Name(C006, Package()
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package ()
  {
    // Function_ManufacturerId = 0x025d, Class, DC
    Package (2) {"mipi-sdca-control-access-layer", 0x4},
    Package (2) {"mipi-sdca-control-access-mode", 5},
    Package (2) {"mipi-sdca-control-dc-value", 0x025d},
  }
}) //End AF01.C006

Name(C007, Package()
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package ()
  {
    // Function_ID = 0x0722 (Device specific), Class, DC
    Package (2) {"mipi-sdca-control-access-layer", 0x4},
    Package (2) {"mipi-sdca-control-access-mode", 5},
    Package (2) {"mipi-sdca-control-dc-value", 0x0722},
  }
}) //End AF01.C007

Name(C008, Package()
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package ()
  {
    // Function_Version = 0x1, Class, DC
    Package (2) {"mipi-sdca-control-access-layer", 0x4},
    Package (2) {"mipi-sdca-control-access-mode", 5},
    Package (2) {"mipi-sdca-control-dc-value", 0x1},
  }
}) //End AF01.C008

//==============================================================================================

Name(E001, Package()
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package ()
  {
    Package (2) {"mipi-sdca-entity-type", 0x0b},
    Package (2) {"mipi-sdca-entity-label", "CS 41"},
    Package (2) {"mipi-sdca-cs-type", 0},
    //Package (2) {"mipi-sdca-control-list",  Package() {0x10} },  // Sample_Freq_Index
    Package (2) {"mipi-sdca-control-list",  0x00010000}, // Bitmap: 0x10 - Sample_Freq_Index
  },

  ToUUID("dbb8e3e6-5886-4ba6-8795-1319f52a966b"),
  Package ()
  {
    Package (2) {"mipi-sdca-control-0x10-subproperties", "C110"},
  }
}) //End AF01.E001

Name(C110, Package()
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package ()
  {
    // Sample_Freq_Index, Class, RW
    Package (2) {"mipi-sdca-control-access-layer", 0x4},
    Package (2) {"mipi-sdca-control-access-mode", 0},
  },

  ToUUID("edb12dd0-363d-4085-a3d2-49522ca160c4"),
  Package()
  {
    Package(2) {"mipi-sdca-control-range", "BUF1"},
  }
}) //End AF01.E001.C110

Name(BUF1, Buffer()
{
  0x02, 0x00,  // Range type 0x0002
  0x03, 0x00,  // Count of ranges = 0x3
  0x09, 0x00, 0x00, 0x00, 0x80, 0xBB, 0x00,0x00, // 0x00000009, 0x0000BB80(48000)
  0x0b, 0x00, 0x00, 0x00, 0x00, 0x77, 0x01,0x00, // 0x0000000b, 0x00017700(96000)
  0x0d, 0x00, 0x00, 0x00, 0x00, 0xEE, 0x02,0x00, // 0x0000000d, 0x0002EE00(192000)
}) //End AF01.E001.C110.BUF1

//==============================================================================================

Name(E002, Package()
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package ()
  {
    Package (2) {"mipi-sdca-entity-type", 0x2},
    Package (2) {"mipi-sdca-entity-label", "IT 41"}, // = IT 02 in ALC722 internal spec
    Package (2) {"mipi-sdca-terminal-type", 0x0101},
    //Package (2) {"mipi-sdca-control-list", Package() {0x4, 0x8, 0x10, 0x11} }, // Usage, Latency, ClusterIndex, DataPortIndex
    Package (2) {"mipi-sdca-control-list", 0x00030110}, // Bitmap: 0x4 - Usage, 0x8 - Latency, 0x10 - ClusterIndex, 0x11 - DataPortIndex
  },

  ToUUID("dbb8e3e6-5886-4ba6-8795-1319f52a966b"),
  Package ()
  {
    // Usage, Latency, ClusterIndex, DataPort Selector
    Package (2) {"mipi-sdca-control-0x4-subproperties", "C204"},
    Package (2) {"mipi-sdca-control-0x8-subproperties", "LC00"},
    Package (2) {"mipi-sdca-control-0x10-subproperties", "C210"},
    Package (2) {"mipi-sdca-control-0x11-subproperties", "C211"},
    Package (2) {"mipi-sdca-terminal-clock-connection", "E001"},
  },

  // ToUUID("edb12dd0-363d-4085-a3d2-49522ca160c4"),
  // Package()
  // {
  //   Package(2) {"mipi-sdca-terminal-dp-numbers", "BUF2"},
  // }
}) //End AF01.E002

// Name(BUF2, Buffer()
// {
//   0x0, 0x1, // DP_Index, DP_Num
// }) //End AF01.E002.BUF2

Name(LC00, Package()
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package ()
  {
    // Latency, Class, DC
    Package (2) {"mipi-sdca-control-access-layer", 0x4},
    Package (2) {"mipi-sdca-control-access-mode", 5},
    Package (2) {"mipi-sdca-control-dc-value", 0},
  }
}) //End AF01.LC00

Name(C204, Package()
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package ()
  {
    // Usage, Class, DC
    Package (2) {"mipi-sdca-control-access-layer", 0x4},
    Package (2) {"mipi-sdca-control-access-mode", 0x5},
    Package (2) {"mipi-sdca-control-dc-value", 1},
  },
  ToUUID("edb12dd0-363d-4085-a3d2-49522ca160c4"),
  Package()
  {
    Package(2) {"mipi-sdca-control-range", "USG1"},
  }
}) //End C204

Name(USG1, Buffer()
{
  0x07, 0x00,  // Range type 0x0007 (Septuples)
  0x06, 0x00,  // Count of ranges = 0x6
  0x01, 0x00, 0x00, 0x00, // Usage Number
  0x9a, 0x01, 0x00, 0x00, // Usage CBN, 410 (0x19a) Full band
  0x80, 0xbb, 0x00, 0x00, // Sample Rate, 48000Hz (0xBB80)
  0x18, 0x00, 0x00, 0x00, // Sample Width, 24-bits (0x18)
  0x00, 0x00, 0x00, 0x00, // Full Scale, 0 not report
  0x00, 0x00, 0x00, 0x00, // Noise Floor, 0 not report
  0x00, 0x00, 0x00, 0x00, // Usage Tag
  0x01, 0x00, 0x00, 0x00, // Usage Number
  0x9a, 0x01, 0x00, 0x00, // Usage CBN, 410 (0x19a) Full band
  0x80, 0xbb, 0x00, 0x00, // Sample Rate, 48000Hz (0xBB80)
  0x10, 0x00, 0x00, 0x00, // Sample Width, 16-bits (0x10)
  0x00, 0x00, 0x00, 0x00, // Full Scale, 0 not report
  0x00, 0x00, 0x00, 0x00, // Noise Floor, 0 not report
  0x00, 0x00, 0x00, 0x00, // Usage Tag
  0x01, 0x00, 0x00, 0x00, // Usage Number
  0xcc, 0x01, 0x00, 0x00, // Usage CBN, 460 (0x1cc) HiFi Full band
  0x00, 0x77, 0x01, 0x00, // Sample Rate, 96000Hz (0x17700)
  0x18, 0x00, 0x00, 0x00, // Sample Width, 24-bits (0x18)
  0x00, 0x00, 0x00, 0x00, // Full Scale, 0 not report
  0x00, 0x00, 0x00, 0x00, // Noise Floor, 0 not report
  0x00, 0x00, 0x00, 0x00, // Usage Tag
  0x01, 0x00, 0x00, 0x00, // Usage Number
  0xcc, 0x01, 0x00, 0x00, // Usage CBN, 460 (0x1cc) HiFi Full band
  0x00, 0x77, 0x01, 0x00, // Sample Rate, 96000Hz (0x17700)
  0x10, 0x00, 0x00, 0x00, // Sample Width, 16-bits (0x10)
  0x00, 0x00, 0x00, 0x00, // Full Scale, 0 not report
  0x00, 0x00, 0x00, 0x00, // Noise Floor, 0 not report
  0x00, 0x00, 0x00, 0x00, // Usage Tag
  0x01, 0x00, 0x00, 0x00, // Usage Number
  0xf4, 0x01, 0x00, 0x00, // Usage CBN, 500 (0x1f4) HiFi Ultra Full band
  0x00, 0xee, 0x02, 0x00, // Sample Rate, 192000Hz (0x2EE00)
  0x18, 0x00, 0x00, 0x00, // Sample Width, 24-bits (0x18)
  0x00, 0x00, 0x00, 0x00, // Full Scale, 0 not report
  0x00, 0x00, 0x00, 0x00, // Noise Floor, 0 not report
  0x00, 0x00, 0x00, 0x00, // Usage Tag
  0x01, 0x00, 0x00, 0x00, // Usage Number
  0xf4, 0x01, 0x00, 0x00, // Usage CBN, 500 (0x1f4) HiFi Ultra Full band
  0x00, 0xee, 0x02, 0x00, // Sample Rate, 192000Hz (0x2EE00)
  0x10, 0x00, 0x00, 0x00, // Sample Width, 16-bits (0x10)
  0x00, 0x00, 0x00, 0x00, // Full Scale, 0 not report
  0x00, 0x00, 0x00, 0x00, // Noise Floor, 0 not report
  0x00, 0x00, 0x00, 0x00, // Usage Tag
}) //End USG1

Name(C210, Package()
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package ()
  {
    // ClusterIndex, Class, DC
    Package (2) {"mipi-sdca-control-access-layer", 0x4},
    Package (2) {"mipi-sdca-control-access-mode", 5},
    Package (2) {"mipi-sdca-control-dc-value", 1},
  },
  ToUUID("edb12dd0-363d-4085-a3d2-49522ca160c4"),
  Package()
  {
    Package(2) {"mipi-sdca-control-range", "CLM1"},
  }
}) //End AF01.E002.C210

Name(CLM1, Buffer()
{
  0x02, 0x00,  // Range type 2 parameters
  0x01, 0x00,  // Count of ranges = 0x1
  0x01, 0x00, 0x00, 0x00, 0x11, 0x00, 0x00, 0x00, // Cluster Index = 0x1,  Cluster ID = 0x11
}) //End CLM1

Name(C211, Package()
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package ()
  {
    // DataPort Selector, Class, DC
    Package (2) {"mipi-sdca-control-access-layer", 0x4},
    Package (2) {"mipi-sdca-control-access-mode", 5},
    Package (2) {"mipi-sdca-control-dc-value", 0x0001}, // DP_Index_D=0x0(0xFF, no DP), DP_Index_C=0x0(0xFF,no DP), DP_Index_B=0x0(0xFF, no DP), DP_Index_A=0x01(0x01, DP1)
  },
  ToUUID("edb12dd0-363d-4085-a3d2-49522ca160c4"),
  Package()
  {
    Package(2) {"mipi-sdca-control-range", "DPS1"},
  }
}) //End AF01.E002.C211

Name(DPS1, Buffer()
{
  0x10, 0x00,  // Range type 0x0010
  0x04, 0x00,  // Count of ranges = 0x4
  0xFF, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, // DP_Index_A
  0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, // DP_Index_B
  0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, // DP_Index_C
  0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, // DP_Index_D
}) //End DPS1

//==============================================================================================

Name(E003, Package()
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package ()
  {
    Package (2) {"mipi-sdca-entity-type", 0x0A},
    Package (2) {"mipi-sdca-entity-label", "XU 42"},
    Package (2) {"mipi-sdca-control-interrupt-position", 6}, // bit 6 in SDCA interrupt
    //Package (2) {"mipi-sdca-control-list", Package() {0x1, 0x6} }, // Bypass, latency
    Package (2) {"mipi-sdca-control-list", 0x0042}, // Bitmap: 0x01 - Bypass, 0x06 - latency
    Package (2) {"mipi-sdca-input-pin-list", 0x2}, // Input Pin 1
  },
  ToUUID("dbb8e3e6-5886-4ba6-8795-1319f52a966b"),
  Package ()
  {
    //Package (2) {"mipi-sdca-powerdomain-entity", "E050"},
    Package (2) {"mipi-sdca-input-pin-1", "E042"}, // Input Pin 1 connected to MU 35
    Package (2) {"mipi-sdca-control-0x1-subproperties", "C301"},
    Package (2) {"mipi-sdca-control-0x6-subproperties", "LC00"},
  }
}) //End AF01.E003

Name(C301, Package()
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package ()
  {
    // Bypass, Class, R/W
    Package (2) {"mipi-sdca-control-access-layer", 0x4},
    Package (2) {"mipi-sdca-control-access-mode", 0},
  }
}) //End AF01.E003.C301

//==============================================================================================

Name(E042, Package()
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package ()
  {
    Package (2) {"mipi-sdca-entity-type", 0x05},
    Package (2) {"mipi-sdca-entity-label", "MU 35"},
    //Package (2) {"mipi-sdca-control-list",  Package() {0x1,0x6} }, // mixer, latency
    Package (2) {"mipi-sdca-control-list",  0x0042}, // Bitmap: 0x1 - mixer, 0x6 - latency
    Package (2) {"mipi-sdca-input-pin-list", 0x6}, // Input Pin 1 and 2 connected
  },
  ToUUID("dbb8e3e6-5886-4ba6-8795-1319f52a966b"),
  Package ()
  {
    Package (2) {"mipi-sdca-input-pin-1", "E002"}, // Input Pin 1 connected
    Package (2) {"mipi-sdca-input-pin-2", "E041"}, // Input Pin 2 connected
    Package (2) {"mipi-sdca-control-0x1-subproperties", "C421"},
    Package (2) {"mipi-sdca-control-0x6-subproperties", "LC00"},
  }
}) //End AF01.E042

Name(C421, Package()
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package ()
  {
    // Mixer, Class, DC, The Q7.8dB format used for MU Controls represents the gain range -128 dB to +127.996 dB
    Package(2) {"mipi-sdca-control-access-layer", 0x4},
    Package(2) {"mipi-sdca-control-access-mode", 5},
    Package(2) {"mipi-sdca-control-cn-0-dc-value", 0x0000},  // Input Pin 1, Ch 1 => Output Pin 1, Ch 1 => 0dB unmute
    Package(2) {"mipi-sdca-control-cn-1-dc-value", 0x8000},  // Input Pin 1, Ch 1 => Output Pin 1, Ch 2 => -128dB mute
    Package(2) {"mipi-sdca-control-cn-2-dc-value", 0x8000},  // Input Pin 1, Ch 2 => Output Pin 1, Ch 1 => -128dB mute
    Package(2) {"mipi-sdca-control-cn-3-dc-value", 0x0000},  // Input Pin 1, Ch 2 => Output Pin 1, Ch 2 => 0dB unmute
    Package(2) {"mipi-sdca-control-cn-4-dc-value", 0x0000},  // Input Pin 2, Ch 1 => Output Pin 1, Ch 1 => 0dB unmute
    Package(2) {"mipi-sdca-control-cn-5-dc-value", 0x8000},  // Input Pin 2, Ch 1 => Output Pin 1, Ch 2 => -128dB mute
    Package(2) {"mipi-sdca-control-cn-6-dc-value", 0x8000},  // Input Pin 2, Ch 2 => Output Pin 1, Ch 1 => -128dB mute
    Package(2) {"mipi-sdca-control-cn-7-dc-value", 0x0000},  // Input Pin 2, Ch 2 => Output Pin 1, Ch 2 => 0dB unmute
  },
  ToUUID("edb12dd0-363d-4085-a3d2-49522ca160c4"),
  Package()
  {
    Package(2) {"mipi-sdca-control-range", "MUR1"},
  }
}) //End AF01.E042.C421

Name(MUR1, Buffer()
{
  0x03, 0x00,  // Range type 0x0003 (Triples)
  0x01, 0x00,  // Count of ranges = 0x1
  0x00, 0x80, 0x00, 0x00,  // Min =0x00008000, -128dB
  0x00, 0x00, 0x00, 0x00,  // Max =0x00000000, 0dB
  0x00, 0x01, 0x00, 0x00,  // Step =0x00000100, 1dB/step
}) //End AF04.MUR1

//==============================================================================================
// Access mode - DC for PDE42

Name(E050, Package()
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package ()
  {
    Package (2) {"mipi-sdca-entity-type", 0x11},
    Package (2) {"mipi-sdca-entity-label", "PDE 42"},
    //Package (2) {"mipi-sdca-powerdomain-state-list", 0xF},
    Package (2) {"mipi-sdca-powerdomain-managed-list", Package() {0x3} },
    Package (2) {"mipi-sdca-powerdomain-transition-typical-delay",
      Package()
      {
        0x3, 0x0, 0x0,
        0x0, 0x3, 0x0,
        0x2, 0x0, 0x0,
        0x0, 0x2, 0x0,
        0x1, 0x0, 0x0,
        0x0, 0x1, 0x0,
        0x0, 0x0, 0x0,
      }
    },
    Package (2) {"mipi-sdca-powerdomain-transition-max-delay",
      Package()
      {
        0x3, 0x0, 0x0,
        0x0, 0x3, 0x0,
        0x2, 0x0, 0x0,
        0x0, 0x2, 0x0,
        0x1, 0x0, 0x0,
        0x0, 0x1, 0x0,
        0x0, 0x0, 0x0,
      }
    },
    //Package (2) {"mipi-sdca-control-list", Package() {0x1, 0x10} },  // Requested_PS, Actual_PS
    Package (2) {"mipi-sdca-control-list", 0x00010002},  // Bitmap: 0x1 - Requested_PS, 0x10 - Actual_PS
  },
  ToUUID("dbb8e3e6-5886-4ba6-8795-1319f52a966b"),
  Package ()
  {
    Package (2) {"mipi-sdca-control-0x1-subproperties", "RP50"},
    Package (2) {"mipi-sdca-control-0x10-subproperties", "AP50"},
  }
}) //End AF01.E050

Name(RP50, Package()
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package ()
  {
    // Requested_PS, Class, DC
    Package (2) {"mipi-sdca-control-access-layer", 0x4},
    Package (2) {"mipi-sdca-control-access-mode", 5},
    Package (2) {"mipi-sdca-control-dc-value", 0x0},
  },
  ToUUID("edb12dd0-363d-4085-a3d2-49522ca160c4"),
  Package ()
  {
    Package (2) {"mipi-sdca-control-range", "PDEC"},
  }
}) //End AF01.E050.RP50

Name(PDEC, Buffer()
{
  0x01, 0x00, // Range type 0x0001
  0x01, 0x00, // Count of ranges = 0x1
  0x00, 0x00, 0x00, 0x00, // PS0
  //0x03, 0x00, 0x00, 0x00, // PS3
}) //End PDEC

Name(AP50, Package()
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package ()
  {
    // Actual_PS, Class, DC
    Package (2) {"mipi-sdca-control-access-layer", 0x4},
    Package (2) {"mipi-sdca-control-access-mode", 5},
    Package (2) {"mipi-sdca-control-dc-value", 0x0},
}
}) //End AF01.E050.AP50

//==============================================================================================

Name(E005, Package()
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package ()
  {
    Package (2) {"mipi-sdca-entity-type", 0x07},
    Package (2) {"mipi-sdca-entity-label", "FU 41"},
    //Package (2) {"mipi-sdca-control-list", Package() {0x1, 0x2, 0x10} }, // Mute, Volume, Latency
    Package (2) {"mipi-sdca-control-list", 0x00010006}, // Bitmap: 0x1 - Mute, 0x2 - Volume, 0x10 - Latency
    Package (2) {"mipi-sdca-input-pin-list", 0x2}, // Input Pin 1 connected
  },
  ToUUID("dbb8e3e6-5886-4ba6-8795-1319f52a966b"),
  Package ()
  {
    Package (2) {"mipi-sdca-input-pin-1", "E003"}, // Input Pin 1 connected to XU42
    Package (2) {"mipi-sdca-control-0x1-subproperties", "C501"},
    Package (2) {"mipi-sdca-control-0x2-subproperties", "C502"},
    Package (2) {"mipi-sdca-control-0x10-subproperties", "LC00"},
  }
}) //End AF01.E005

Name(C501, Package()
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package ()
  {
    // Mute, User, RW
    Package (2) {"mipi-sdca-control-access-layer", 0x1},
    Package (2) {"mipi-sdca-control-access-mode", 0},
    Package (2) {"mipi-sdca-control-default-value", 0}, // unmute
    //Package (2) {"mipi-sdca-control-cn-list", Package() {0x1, 0x2} }, // channel 1 and 2
    Package (2) {"mipi-sdca-control-cn-list", 0x6 }, // channel 1 and 2
  }
}) //End AF01.E005.C501

Name(C502, Package()
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package ()
  {
    // Volume, User, RW
    Package (2) {"mipi-sdca-control-access-layer", 0x1},
    Package (2) {"mipi-sdca-control-access-mode", 0},
    //Package (2) {"mipi-sdca-control-default-value", 0xFA00}, // -6dB   67%
    Package (2) {"mipi-sdca-control-default-value", 0}, // 0dB
    //Package (2) {"mipi-sdca-control-cn-list", Package() {0x1, 0x2} },
    Package (2) {"mipi-sdca-control-cn-list", 0x6 },
  },
  ToUUID("edb12dd0-363d-4085-a3d2-49522ca160c4"),
  Package()
  {
    Package(2) {"mipi-sdca-control-range", "BUF3"},
  }
}) //End AF01.E005.C502

Name(BUF3, Buffer()
{
  0x03, 0x00, // Range type 0x0003
  0x01, 0x00, // Count of ranges = 0x1
  0xC0, 0xBE, 0x00, 0x00, // Min = 0x0000BEC0,  // Min = -65.25dB
  0x00, 0x00, 0x00, 0x00, // Max = 0x00000000,  // Max = 0dB
  0xC0, 0x00, 0x00, 0x00, // Step = 0x000000C0, // Step = 0.75dB
}) //End AF01.E005.BUF3

//==============================================================================================

Name(E045, Package()
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package ()
  {
    Package (2) {"mipi-sdca-entity-type", 0x06},
    Package (2) {"mipi-sdca-entity-label", "SU 43"},
    //Package (2) {"mipi-sdca-control-list",  Package() {0x1, 0x2}}, // selector, latency
    Package (2) {"mipi-sdca-control-list",  0x06}, // Bitmap: 0x1 - selector, 0x2 - latency
    Package (2) {"mipi-sdca-input-pin-list", 0x2}, // Input Pin 1 connected
  },
  ToUUID("dbb8e3e6-5886-4ba6-8795-1319f52a966b"),
  Package ()
  {
    Package (2) {"mipi-sdca-input-pin-1", "E005"}, // Input Pin 1 connected to FU42
    Package (2) {"mipi-sdca-control-0x1-subproperties", "C451"},
    Package (2) {"mipi-sdca-control-0x2-subproperties", "LC00"},
  }
}) //End AF01.E045

Name(C451, Package()
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package ()
  {
    // selector, Device, RO (SU controlled by GE)
    Package (2) {"mipi-sdca-control-access-layer", 0x10},
    Package (2) {"mipi-sdca-control-access-mode", 3},
    //Package (2) {"mipi-sdca-control-cn-list", Package() {0x1, 0x2} }, // channel 1 and 2
    Package (2) {"mipi-sdca-control-cn-list", 0x6 }, // channel 1 and 2
  },
  ToUUID("edb12dd0-363d-4085-a3d2-49522ca160c4"),
  Package()
  {
    Package(2) {"mipi-sdca-control-range", "SUP1"},
  }
}) //End AF01.E045.C451

Name(SUP1, Buffer()
{
  0x01, 0x00, // Range type 0x0001
  0x02, 0x00, // Count of ranges = 0x2
  0x00, 0x00, 0x00, 0x00, // Input Pin 0 can be selected
  0x01, 0x00, 0x00, 0x00, // Input Pin 1 can be selected
}) //End SUP1

//==============================================================================================

Name(E046, Package()
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package ()
  {
    Package (2) {"mipi-sdca-entity-type", 0x06},
    Package (2) {"mipi-sdca-entity-label", "SU 44"},
    //Package (2) {"mipi-sdca-control-list", Package() {0x1, 0x2} }, // selector, latency
    Package (2) {"mipi-sdca-control-list", 0x06}, // Bitmap: 0x1 - selector, 0x2 - latency
    Package (2) {"mipi-sdca-input-pin-list", 0x2}, // Input Pin 1 connected
  },
  ToUUID("dbb8e3e6-5886-4ba6-8795-1319f52a966b"),
  Package ()
  {
    Package (2) {"mipi-sdca-input-pin-1", "E005"}, // Input Pin 1 connected to FU42
    Package (2) {"mipi-sdca-control-0x1-subproperties", "C461"},
    Package (2) {"mipi-sdca-control-0x2-subproperties", "LC00"},
  }
}) //End AF01.E046

Name(C461, Package()
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package ()
  {
    // selector, Device, RO (SU controlled by GE)
    Package (2) {"mipi-sdca-control-access-layer", 0x10},
    Package (2) {"mipi-sdca-control-access-mode", 3},
    //Package (2) {"mipi-sdca-control-cn-list", Package() {0x1, 0x2} }, // channel 1 and 2
    Package (2) {"mipi-sdca-control-cn-list", 0x6 }, // channel 1 and 2
  },
  ToUUID("edb12dd0-363d-4085-a3d2-49522ca160c4"),
  Package()
  {
    Package(2) {"mipi-sdca-control-range", "SUP1"},
  }
}) //End AF01.E046.C461

//==============================================================================================

Name(E047, Package()
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package ()
  {
    Package (2) {"mipi-sdca-entity-type", 0x06},
    Package (2) {"mipi-sdca-entity-label", "SU 45"},
    //Package (2) {"mipi-sdca-control-list",  Package() {0x1, 0x2} }, // selector, latency
    Package (2) {"mipi-sdca-control-list",  0x06}, // Bitmap: 0x1 - selector, 0x2 - latency
    Package (2) {"mipi-sdca-input-pin-list", 0x2}, // Input Pin 1 connected
  },
  ToUUID("dbb8e3e6-5886-4ba6-8795-1319f52a966b"),
  Package ()
  {
    Package (2) {"mipi-sdca-input-pin-1", "E005"}, // Input Pin 1 connected to FU42
    Package (2) {"mipi-sdca-control-0x1-subproperties", "C471"},
    Package (2) {"mipi-sdca-control-0x2-subproperties", "LC00"},
  }
}) //End AF01.E047

Name(C471, Package()
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package ()
  {
    // selector, Device, RO (SU controlled by GE)
    Package (2) {"mipi-sdca-control-access-layer", 0x10},
    Package (2) {"mipi-sdca-control-access-mode", 3},
    //Package (2) {"mipi-sdca-control-cn-list", Package() {0x1, 0x2} }, // channel 1 and 2
    Package (2) {"mipi-sdca-control-cn-list", 0x6 }, // channel 1 and 2
  },
  ToUUID("edb12dd0-363d-4085-a3d2-49522ca160c4"),
  Package()
  {
    Package(2) {"mipi-sdca-control-range", "SUP1"},
  }
}) //End AF01.E047.C471

//==============================================================================================

Name(E040, Package()
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package ()
  {
    Package (2) {"mipi-sdca-entity-type", 0x11},
    Package (2) {"mipi-sdca-entity-label", "PDE 47"}, // = PDE 47 Entity ID 40
    //Package (2) {"mipi-sdca-powerdomain-state-list", 0xF},
    Package (2) {"mipi-sdca-powerdomain-managed-list", Package() {0x6, 0x7, 0x48} },
    Package (2) {"mipi-sdca-powerdomain-transition-typical-delay",
      Package()
      {
        0x3, 0x0, 384000,
        0x0, 0x3, 384000,
        0x2, 0x0, 0x0,
        0x0, 0x2, 0x0,
        0x1, 0x0, 0x0,
        0x0, 0x1, 0x0,
        0x0, 0x0, 0x0,
      }
    },
    Package (2) {"mipi-sdca-powerdomain-transition-max-delay",
      Package()
      {
        0x3, 0x0, 500000,
        0x0, 0x3, 500000,
        0x2, 0x0, 0x0,
        0x0, 0x2, 0x0,
        0x1, 0x0, 0x0,
        0x0, 0x1, 0x0,
        0x0, 0x0, 0x0,
      }
    },
    //Package (2) {"mipi-sdca-control-list", Package() {0x1, 0x10} }, // Requested_PS, Actual_PS
    Package (2) {"mipi-sdca-control-list", 0x00010002}, // 0x1 - Requested_PS, 0x10 - Actual_PS
  },
  ToUUID("dbb8e3e6-5886-4ba6-8795-1319f52a966b"),
  Package ()
  {
    Package (2) {"mipi-sdca-control-0x1-subproperties", "RP40"},
    Package (2) {"mipi-sdca-control-0x10-subproperties", "AP40"},
  }
}) //End AF01.E040

Name(RP40, Package()
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package ()
  {
    // Requested_PS, Class, R/W
    Package (2) {"mipi-sdca-control-access-layer", 0x4},
    Package (2) {"mipi-sdca-control-access-mode", 0},
  },
  ToUUID("edb12dd0-363d-4085-a3d2-49522ca160c4"),
  Package ()
  {
    Package (2) {"mipi-sdca-control-range", "PDER"},
  }
}) //End AF01.E040.RP40

Name(PDER, Buffer()
{
  0x01, 0x00, // Range type 0x0001
  0x02, 0x00, // Count of ranges = 0x2
  0x00, 0x00, 0x00, 0x00, // PS0
  0x03, 0x00, 0x00, 0x00, // PS3
}) //End PDER

Name(AP40, Package()
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package ()
  {
    // Actual_PS, Class, RO
    Package (2) {"mipi-sdca-control-access-layer", 0x4},
    Package (2) {"mipi-sdca-control-access-mode", 3},
  }
}) //End AF01.E040.AP40

//==============================================================================================

Name(E006, Package()
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package ()
  {
    Package (2) {"mipi-sdca-entity-type", 0x03},
    Package (2) {"mipi-sdca-entity-label", "OT 43"},
    Package (2) {"mipi-sdca-terminal-type", 0x06C0},
    Package (2) {"mipi-sdca-terminal-reference-number", 0x1},
    Package (2) {"mipi-sdca-input-pin-list", 0x2},
    //Package (2) {"mipi-sdca-control-list", Package() {0x4, 0x8} },  // Usage, Latency
    Package (2) {"mipi-sdca-control-list", 0x0110},  // 0x4 - Usage, 0x8 - Latency
    Package (2) {"mipi-sdca-terminal-transducer-count", 2},  // 2 speakers
    Package (2) {"mipi-sdca-terminal-connector-type", 0x02},  // 3.5 mm Jack
  },
  ToUUID("dbb8e3e6-5886-4ba6-8795-1319f52a966b"),
  Package ()
  {
    Package (2) {"mipi-sdca-input-pin-1", "E045"},
    //Package (2) {"mipi-sdca-powerdomain-entity", "E040"},
    Package (2) {"mipi-sdca-control-0x4-subproperties", "C604"},
    Package (2) {"mipi-sdca-control-0x8-subproperties", "LC00"},
  },
  ToUUID("edb12dd0-363d-4085-a3d2-49522ca160c4"),
  Package ()
  {
    Package (2) {"mipi-sdca-terminal-reference-description", "TRD1"},
  }
}) //End AF01.E006

Name(TRD1, Buffer()
{
  0x00, 0x01, // Version
  0x00, 0x00, 0x00, 0x00, // Color: Black
  0x03, 0x00, // Gen Location: Left
  0x00, 0x00, // Geo Location: On Primary Chassis
}) //End TRD1

Name(C604, Package()
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package ()
  {
    // Usage, Class, DC
    Package (2) {"mipi-sdca-control-access-layer", 0x4},
    Package (2) {"mipi-sdca-control-access-mode", 5},
    Package (2) {"mipi-sdca-control-dc-value", 1},
  },
  ToUUID("edb12dd0-363d-4085-a3d2-49522ca160c4"),
  Package()
  {
    Package(2) {"mipi-sdca-control-range", "USG3"},
  }
}) //End C604

Name(USG3, Buffer()
{
  0x07, 0x00,  // Range type 0x0007 (Septuples)
  0x01, 0x00,  // Count of ranges = 0x1
  0x01, 0x00, 0x00, 0x00, // Usage Number
  0x9a, 0x01, 0x00, 0x00, // Usage CBN, 410 (0x19A) Full band
  0x00, 0x00, 0x00, 0x00, // Sample Rate, 0 not report
  0x00, 0x00, 0x00, 0x00, // Sample Width, 0 not report
  0x00, 0x80, 0x00, 0x00, // Full Scale, -128.0dB (0x8000) not report actual value
  0x00, 0x80, 0x00, 0x00, // Noise Floor, -128.0dB (0x8000) not report actual value
  0x00, 0x00, 0x00, 0x00, // Usage Tag
}) //End USG3

//==============================================================================================

Name(E007, Package()
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package ()
  {
    Package (2) {"mipi-sdca-entity-type", 0x03},
    Package (2) {"mipi-sdca-entity-label", "OT 44"},
    Package (2) {"mipi-sdca-terminal-type", 0x0690},
    Package (2) {"mipi-sdca-terminal-reference-number", 0x1},
    Package (2) {"mipi-sdca-input-pin-list", 0x2},
    //Package (2) {"mipi-sdca-control-list", Package() {0x4, 0x8} }, // Usage, Latency
    Package (2) {"mipi-sdca-control-list", 0x0110}, // 0x4 - Usage, 0x8 - Latency
    Package (2) {"mipi-sdca-terminal-transducer-count", 2},  // 2 speakers
    Package (2) {"mipi-sdca-terminal-connector-type", 0x02},  // 3.5 mm Jack
  },
  ToUUID("dbb8e3e6-5886-4ba6-8795-1319f52a966b"),
  Package ()
  {
    Package (2) {"mipi-sdca-input-pin-1", "E046"},
    //Package (2) {"mipi-sdca-powerdomain-entity", "E040"},
    Package (2) {"mipi-sdca-control-0x4-subproperties", "C704"},
    Package (2) {"mipi-sdca-control-0x8-subproperties", "LC00"},
  },
  ToUUID("edb12dd0-363d-4085-a3d2-49522ca160c4"),
  Package ()
  {
    Package (2) {"mipi-sdca-terminal-reference-description", "TRD1"},
  }
}) //End AF01.E007

Name(C704, Package()
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package ()
  {
    // Usage, Class, DC
    Package (2) {"mipi-sdca-control-access-layer", 0x4},
    Package (2) {"mipi-sdca-control-access-mode", 5},
    Package (2) {"mipi-sdca-control-dc-value", 1},
  },
  ToUUID("edb12dd0-363d-4085-a3d2-49522ca160c4"),
  Package()
  {
    Package(2) {"mipi-sdca-control-range", "USG3"},
  }
}) //End C704

//==============================================================================================

Name(E048, Package()
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package ()
  {
    Package (2) {"mipi-sdca-entity-type", 0x03},
    Package (2) {"mipi-sdca-entity-label", "OT 45"},
    Package (2) {"mipi-sdca-terminal-type", 0x06D0},
    Package (2) {"mipi-sdca-terminal-reference-number", 0x1},
    Package (2) {"mipi-sdca-input-pin-list", 0x2},
    //Package (2) {"mipi-sdca-control-list", Package() {0x4, 0x8} },  // Usage, Latency
    Package (2) {"mipi-sdca-control-list", 0x0110},  // 0x4 - Usage, 0x8 - Latency
    Package (2) {"mipi-sdca-terminal-transducer-count", 2},  // 2 speakers
    Package (2) {"mipi-sdca-terminal-connector-type", 0x02},  // 3.5 mm Jack
  },
  ToUUID("dbb8e3e6-5886-4ba6-8795-1319f52a966b"),
  Package ()
  {
    Package (2) {"mipi-sdca-input-pin-1", "E047"},
    //Package (2) {"mipi-sdca-powerdomain-entity", "E040"},
    Package (2) {"mipi-sdca-control-0x4-subproperties", "C484"},
    Package (2) {"mipi-sdca-control-0x8-subproperties", "LC00"},
  },
  ToUUID("edb12dd0-363d-4085-a3d2-49522ca160c4"),
  Package ()
  {
    Package (2) {"mipi-sdca-terminal-reference-description", "TRD1"},
  }
}) //End AF01.E048

Name(C484, Package()
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package ()
  {
    // Usage, Class, DC
    Package (2) {"mipi-sdca-control-access-layer", 0x4},
    Package (2) {"mipi-sdca-control-access-mode", 5},
    Package (2) {"mipi-sdca-control-dc-value", 1},
  },
  ToUUID("edb12dd0-363d-4085-a3d2-49522ca160c4"),
  Package()
  {
    Package(2) {"mipi-sdca-control-range", "USG3"},
  }
}) //End C484

//==============================================================================================

Name(E049, Package()
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package ()
  {
    Package (2) {"mipi-sdca-entity-type", 0x12},
    Package (2) {"mipi-sdca-entity-label", "GE 35"},
    Package (2) {"mipi-sdca-ge-managed-terminal-reference-number", 0x1},
    Package (2) {"mipi-sdca-ge-default-selectedmode", 3}, // This is HP
    // Package (2) {"msft-ge-mode-terminaltype-list",
    //   Package()
    //   {
    //     0x3, 0x06C0, // Mode 3 maps to Headphone
    //     0x4, 0x0690, // Mode 4 maps to Lineout stereo
    //     0x5, 0x06D0, // Mode 5 maps to Headset
    //     0x6, 0x06A0, // Mode 6 maps to Mic
    //     0x7, 0x0680, // Mode 7 maps to LineIn stereo
    //   }
    // },
    // Package (2) {"msft-ge-managed-list",
    //   Package()
    //   {
    //     0x6, // Entity Id of Headphone terminal
    //     0x7, // Entity Id of Lineout stereo terminal
    //     0x48, // Entity Id of Headset Speaker terminal
    //     0x8, // Entity Id of to Mic terminal
    //     0x9, // Entity Id of LineIn stereo terminal
    //     0x43, // Entity Id of Headset Mic terminal
    //   }
    // },
    Package (2) {"mipi-sdca-control-list", 0x06},  // 0x1 - selected_mode, 0x2 - detected_mode
  },
  ToUUID("dbb8e3e6-5886-4ba6-8795-1319f52a966b"),
  Package ()
  {
    Package (2) {"mipi-sdca-control-0x1-subproperties", "GE01"},
    Package (2) {"mipi-sdca-control-0x2-subproperties", "GE02"}
  },
  ToUUID("edb12dd0-363d-4085-a3d2-49522ca160c4"),
  Package()
  {
    Package (2) {"mipi-sdca-ge-selectedmode-controls-affected", "BUF4"},
  }
}) //End AF01.E049

Name(BUF4, Buffer()
{
  0x08, // Mode count

  0x00, // Mode 0, Unplugged
    0x04,  // Control count
      0xC, 0x1, 0x0, 0x0, // entity id, control selector, control number, value
      0x45, 0x1, 0x0, 0x0,
      0x46, 0x1, 0x0, 0x0,
      0x47, 0x1, 0x0, 0x0,

  0x01, // Mode 1, Unknown
    0x04,  // Control count
      0xC, 0x1, 0x0, 0x0,
      0x45, 0x1, 0x0, 0x0,
      0x46, 0x1, 0x0, 0x0,
      0x47, 0x1, 0x0, 0x0,

  0x02, // Mode 2, Undefined
    0x04,  // Control count
      0xC, 0x1, 0x0, 0x0,
      0x45, 0x1, 0x0, 0x0,
      0x46, 0x1, 0x0, 0x0,
      0x47, 0x1, 0x0, 0x0,

  0x03, // Mode 3, Headphone
    0x04,  // Control count
      0xC, 0x1, 0x0, 0x0,
      0x45, 0x1, 0x0, 0x1,
      0x46, 0x1, 0x0, 0x0,
      0x47, 0x1, 0x0, 0x0,

  0x04, // Mode 4, Line-Out
    0x04,  // Control count
      0xC, 0x1, 0x0, 0x0,
      0x45, 0x1, 0x0, 0x0,
      0x46, 0x1, 0x0, 0x1,
      0x47, 0x1, 0x0, 0x0,

  0x05, // Mode 5, Headset
    0x04,  // Control count
      0xC, 0x1, 0x0, 0x3,
      0x45, 0x1, 0x0, 0x0,
      0x46, 0x1, 0x0, 0x0,
      0x47, 0x1, 0x0, 0x1,

  0x06, // Mode 6, MIC-IN
    0x04,  // Control count
      0xC, 0x1, 0x0, 0x1,
      0x45, 0x1, 0x0, 0x0,
      0x46, 0x1, 0x0, 0x0,
      0x47, 0x1, 0x0, 0x0,

  0x07, // Mode 7, LINE-IN
    0x04,  // Control count
      0xC, 0x1, 0x0, 0x2,
      0x45, 0x1, 0x0, 0x0,
      0x46, 0x1, 0x0, 0x0,
      0x47, 0x1, 0x0, 0x0,
}) //End AF01.E049.BUF4

Name(GE01, Package()
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package ()
  {
    // selected_mode, Class, RW
    Package (2) {"mipi-sdca-control-access-layer", 0x4},
    Package (2) {"mipi-sdca-control-access-mode", 0}
  },
  ToUUID("edb12dd0-363d-4085-a3d2-49522ca160c4"),
  Package()
  {
    Package(2) {"mipi-sdca-control-range", "GES1"},
  }
}) //End AF01.GE01

Name(GES1, Buffer()
{
  0x02, 0x00,  // Range type 0x0002
  0x05, 0x00,  // Count of ranges = 0x5
  0x03, 0x00, 0x00, 0x00, 0xC0, 0x06, 0x00, 0x00, // Mode 3 maps to Headphone
  0x04, 0x00, 0x00, 0x00, 0x90, 0x06, 0x00, 0x00, // Mode 4 maps to Lineout stereo
  0x05, 0x00, 0x00, 0x00, 0xD0, 0x06, 0x00, 0x00, // Mode 5 maps to Headset
  0x06, 0x00, 0x00, 0x00, 0xA0, 0x06, 0x00, 0x00, // Mode 6 maps to Mic
  0x07, 0x00, 0x00, 0x00, 0x80, 0x06, 0x00, 0x00, // Mode 7 maps to LineIn stereo
}) //End GES1

Name(GE02, Package()
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package ()
  {
    // detected_mode, Class, RO
    Package (2) {"mipi-sdca-control-access-layer", 0x4},
    Package (2) {"mipi-sdca-control-interrupt-position", 0}, // bit 0 in SDCA interrupt
    Package (2) {"mipi-sdca-control-access-mode", 3}
  }
}) //End AF01.GE02

//==============================================================================================

Name(E041, Package()
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package ()
  {
    Package (2) {"mipi-sdca-entity-type", 0x07},
    Package (2) {"mipi-sdca-entity-label", "FU 35"},
    //Package (2) {"mipi-sdca-control-list", Package() {0x1, 0x2, 0x10} }, // Mute, Volume, Latency
    Package (2) {"mipi-sdca-control-list", 0x00010006}, // 0x1 - Mute, 0x2 - Volume, 0x10 - Latency
    Package (2) {"mipi-sdca-input-pin-list", 0x2}, // Input Pin 1 connected
  },
  ToUUID("dbb8e3e6-5886-4ba6-8795-1319f52a966b"),
  Package ()
  {
    Package (2) {"mipi-sdca-input-pin-1", "E00C"}, // Input Pin 1 connected to
    Package (2) {"mipi-sdca-control-0x1-subproperties", "C411"},
    Package (2) {"mipi-sdca-control-0x2-subproperties", "C412"},
    Package (2) {"mipi-sdca-control-0x10-subproperties", "LC00"}, // Latency
  }
}) //End AF01.E041

Name(C411, Package()
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package ()
  {
    // Mute, User, RW
    Package (2) {"mipi-sdca-control-access-layer", 0x1},
    Package (2) {"mipi-sdca-control-access-mode", 0},
    Package (2) {"mipi-sdca-control-fixed-value", 1}, // mute sideband
    //Package (2) {"mipi-sdca-control-cn-list", Package() {0x1, 0x2} }, // channel 1 and 2
    Package (2) {"mipi-sdca-control-cn-list", 0x6 }, // channel 1 and 2
  }
}) //End AF01.E041.C411

Name(C412, Package()
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package ()
  {
    // Volume, User, RW
    Package (2) {"mipi-sdca-control-access-layer", 0x1},
    Package (2) {"mipi-sdca-control-access-mode", 0},
    Package (2) {"mipi-sdca-control-fixed-value", 0}, // 0dB
    //Package (2) {"mipi-sdca-control-cn-list", Package() {0x1, 0x2} },
    Package (2) {"mipi-sdca-control-cn-list", 0x6 },
  },
  ToUUID("edb12dd0-363d-4085-a3d2-49522ca160c4"),
  Package()
  {
    Package (2) {"mipi-sdca-control-range", "BUF5"},
  }
}) //End AF01.E041.C412

Name(BUF5, Buffer()
{
  0x03, 0x00, // Range type 0x0003
  0x01, 0x00, // Count of ranges = 0x1
  0xC0, 0xEE, 0x00, 0x00, // Min = 0x0000EEC0,  // Min = -17.25dB
  0x00, 0x1E, 0x00, 0x00, // Max = 0x00001E00,  // Max = 30dB
  0xC0, 0x00, 0x00, 0x00, // Step = 0x000000C0, // Step = 0.75dB
}) //End AF02.BUF5

//==============================================================================================

Name(E012, Package()
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package ()
  {
    Package (2) {"mipi-sdca-entity-type", 0x11},
    Package (2) {"mipi-sdca-entity-label", "PDE 34"},
    //Package (2) {"mipi-sdca-powerdomain-state-list", 0xF},
    Package (2) {"mipi-sdca-powerdomain-managed-list", Package() {0x8, 0x9, 0x43}},
    Package (2) {"mipi-sdca-powerdomain-transition-typical-delay",
      Package()
      {
        0x3, 0x0, 384000,
        0x0, 0x3, 384000,
        0x2, 0x0, 0x0,
        0x0, 0x2, 0x0,
        0x1, 0x0, 0x0,
        0x0, 0x1, 0x0,
        0x0, 0x0, 0x0,
      }
    },
    Package (2) {"mipi-sdca-powerdomain-transition-max-delay",
      Package()
      {
        0x3, 0x0, 500000,
        0x0, 0x3, 500000,
        0x2, 0x0, 0x0,
        0x0, 0x2, 0x0,
        0x1, 0x0, 0x0,
        0x0, 0x1, 0x0,
        0x0, 0x0, 0x0,
      }
    },
    //Package (2) {"mipi-sdca-control-list", Package() {0x1, 0x10}}, // Requested_PS, Actual_PS
    Package (2) {"mipi-sdca-control-list", 0x00010002}, // 0x1 - Requested_PS, 0x10 - Actual_PS
  },
  ToUUID("dbb8e3e6-5886-4ba6-8795-1319f52a966b"),
  Package ()
  {
    Package (2) {"mipi-sdca-control-0x1-subproperties", "RP12"},
    Package (2) {"mipi-sdca-control-0x10-subproperties", "AP12"},
  }
}) //End AF01.E012

Name(RP12, Package()
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package ()
  {
    // Requested_PS, Class, R/W
    Package (2) {"mipi-sdca-control-access-layer", 0x4},
    Package (2) {"mipi-sdca-control-access-mode", 0},
  },
  ToUUID("edb12dd0-363d-4085-a3d2-49522ca160c4"),
  Package ()
  {
    Package (2) {"mipi-sdca-control-range", "PDER"},
  }
}) //End AF01.E012.RP12

Name(AP12, Package()
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package ()
  {
    // Actual_PS, Class, RO
    Package (2) {"mipi-sdca-control-access-layer", 0x4},
    Package (2) {"mipi-sdca-control-access-mode", 3},
  }
}) //End AF01.E012.AP12

//==============================================================================================

Name(E008, Package()
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package ()
  {
    Package (2) {"mipi-sdca-entity-type", 0x02},
    Package (2) {"mipi-sdca-entity-label", "IT 31"},
    Package (2) {"mipi-sdca-terminal-type", 0x06A0},
    Package (2) {"mipi-sdca-terminal-reference-number", 0x1},
    //Package (2) {"mipi-sdca-control-list",  Package() {0x4, 0x8, 0x10}},  // Usage, Latency, ClusterIndex
    Package (2) {"mipi-sdca-control-list",  0x00010110},  // Bitmap: 0x4 - Usage, 0x8 - Latency, 0x10 - ClusterIndex
    Package (2) {"mipi-sdca-terminal-transducer-count", 2},  // 2 channels
    Package (2) {"mipi-sdca-terminal-connector-type", 0x02},  // 3.5 mm Jack
  },
  ToUUID("dbb8e3e6-5886-4ba6-8795-1319f52a966b"),
  Package ()
  {
    //Package (2) {"mipi-sdca-powerdomain-entity", "E012"},
    Package (2) {"mipi-sdca-control-0x4-subproperties", "C804"},
    Package (2) {"mipi-sdca-control-0x8-subproperties", "LC00"},
    Package (2) {"mipi-sdca-control-0x10-subproperties", "C810"},
  },
  ToUUID("edb12dd0-363d-4085-a3d2-49522ca160c4"),
  Package ()
  {
    Package (2) {"mipi-sdca-terminal-reference-description", "TRD1"},
  }
}) //End AF01.E008

Name(C804, Package()
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package ()
  {
    // Usage, class, DC
    Package (2) {"mipi-sdca-control-access-layer", 0x4},
    Package (2) {"mipi-sdca-control-access-mode", 5},
    Package (2) {"mipi-sdca-control-dc-value", 1},
  },
  ToUUID("edb12dd0-363d-4085-a3d2-49522ca160c4"),
  Package()
  {
    Package(2) {"mipi-sdca-control-range", "USG3"},
  }
}) //End C804

Name(C810, Package()
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package ()
  {
    //cluster, class, DC
    Package (2) {"mipi-sdca-control-access-layer", 0x4},
    Package (2) {"mipi-sdca-control-access-mode", 5},
    Package (2) {"mipi-sdca-control-dc-value", 1},
  },
  ToUUID("edb12dd0-363d-4085-a3d2-49522ca160c4"),
  Package()
  {
    Package(2) {"mipi-sdca-control-range", "CLM1"},
  }
}) //End AF01.E008.C810

//==============================================================================================

Name(E009, Package()
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package ()
  {
    Package (2) {"mipi-sdca-entity-type", 0x02},
    Package (2) {"mipi-sdca-entity-label", "IT 32"},
    Package (2) {"mipi-sdca-terminal-type", 0x0680},
    Package (2) {"mipi-sdca-terminal-reference-number", 0x1},
    //Package (2) {"mipi-sdca-control-list", Package() {0x4, 0x8, 0x10}},  // Usage, Latency, ClusterIndex
    Package (2) {"mipi-sdca-control-list", 0x00010110},  // Bitmap: 0x4 - Usage, 0x8 - Latency, 0x10 - ClusterIndex
    Package (2) {"mipi-sdca-terminal-transducer-count", 2},  // 2 channels
    Package (2) {"mipi-sdca-terminal-connector-type", 0x02},  // 3.5 mm Jack
  },
  ToUUID("dbb8e3e6-5886-4ba6-8795-1319f52a966b"),
  Package ()
  {
    //Package (2) {"mipi-sdca-powerdomain-entity", "E012"},
    Package (2) {"mipi-sdca-control-0x4-subproperties", "C904"},
    Package (2) {"mipi-sdca-control-0x8-subproperties", "LC00"},
    Package (2) {"mipi-sdca-control-0x10-subproperties", "C910"},
  },
  ToUUID("edb12dd0-363d-4085-a3d2-49522ca160c4"),
  Package ()
  {
    Package (2) {"mipi-sdca-terminal-reference-description", "TRD1"},
  }
}) //End AF01.E009

Name(C904, Package()
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package ()
  {
    // Usage, Class, DC
    Package (2) {"mipi-sdca-control-access-layer", 0x4},
    Package (2) {"mipi-sdca-control-access-mode", 5},
    Package (2) {"mipi-sdca-control-dc-value", 1},
  },
  ToUUID("edb12dd0-363d-4085-a3d2-49522ca160c4"),
  Package()
  {
    Package(2) {"mipi-sdca-control-range", "USG3"},
  }
}) //End C904

Name(C910, Package()
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package ()
  {
    //cluster, class, DC
    Package (2) {"mipi-sdca-control-access-layer", 0x4},
    Package (2) {"mipi-sdca-control-access-mode", 5},
    Package (2) {"mipi-sdca-control-dc-value", 1},
  },
  ToUUID("edb12dd0-363d-4085-a3d2-49522ca160c4"),
  Package()
  {
    Package(2) {"mipi-sdca-control-range", "CLM1"},
  }
}) //End AF01.E009.C910

//==============================================================================================

Name(E043, Package()
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package ()
  {
    Package (2) {"mipi-sdca-entity-type", 0x02},
    Package (2) {"mipi-sdca-entity-label", "IT 33"},
    Package (2) {"mipi-sdca-terminal-type", 0x06D0},
    Package (2) {"mipi-sdca-terminal-reference-number", 0x1},
    //Package (2) {"mipi-sdca-control-list", Package() {0x4, 0x8, 0x10}},  // Usage, Latency, ClusterIndex
    Package (2) {"mipi-sdca-control-list", 0x00010110},  // Bitmap: 0x4 - Usage, 0x8 - Latency, 0x10 - ClusterIndex
    Package (2) {"mipi-sdca-terminal-transducer-count", 2},  // 2 channels
    Package (2) {"mipi-sdca-terminal-connector-type", 0x02},  // 3.5 mm Jack
  },
  ToUUID("dbb8e3e6-5886-4ba6-8795-1319f52a966b"),
  Package ()
  {
    //Package (2) {"mipi-sdca-powerdomain-entity", "E012"},
    Package (2) {"mipi-sdca-control-0x4-subproperties", "C430"},
    Package (2) {"mipi-sdca-control-0x8-subproperties", "LC00"},
    Package (2) {"mipi-sdca-control-0x10-subproperties", "C431"},
  },
  ToUUID("edb12dd0-363d-4085-a3d2-49522ca160c4"),
  Package ()
  {
    Package (2) {"mipi-sdca-terminal-reference-description", "TRD1"},
  }
}) //End AF01.E043

Name(C430, Package()
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package ()
  {
    // Usage, Class, DC
    Package (2) {"mipi-sdca-control-access-layer", 0x4},
    Package (2) {"mipi-sdca-control-access-mode", 5},
    Package (2) {"mipi-sdca-control-dc-value", 1},
  },
  ToUUID("edb12dd0-363d-4085-a3d2-49522ca160c4"),
  Package()
  {
    Package(2) {"mipi-sdca-control-range", "USG3"},
  }
}) //End C430

Name(C431, Package()
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package ()
  {
    //cluster, class, DC
    Package (2) {"mipi-sdca-control-access-layer", 0x4},
    Package (2) {"mipi-sdca-control-access-mode", 5},
    Package (2) {"mipi-sdca-control-dc-value", 1},
  },
  ToUUID("edb12dd0-363d-4085-a3d2-49522ca160c4"),
  Package()
  {
    Package(2) {"mipi-sdca-control-range", "CLM2"},
  }
}) //End AF01.E043.C431

Name(CLM2, Buffer()
{
  0x02, 0x00,  // Range type 2 parameters
  0x01, 0x00,  // Count of ranges = 0x1
  0x01, 0x00, 0x00, 0x00, 0x12, 0x00, 0x00, 0x00, // Cluster Index = 0x1, Cluster ID = 0x12
}) //End CLM2

//==============================================================================================

Name(E00A, Package()
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package ()
  {
    Package (2) {"mipi-sdca-entity-type", 0x07},
    Package (2) {"mipi-sdca-entity-label", "FU 31"},
    //Package (2) {"mipi-sdca-control-list", Package() {0xB, 0x10} }, // boost gain, Latency
    Package (2) {"mipi-sdca-control-list", 0x00010800}, // 0xB - boost gain, 0x10 - Latency
    Package (2) {"mipi-sdca-input-pin-list", 0x2}, // Input Pin 1 connected
  },
  ToUUID("dbb8e3e6-5886-4ba6-8795-1319f52a966b"),
  Package ()
  {
    Package (2) {"mipi-sdca-input-pin-1", "E008"},
    Package (2) {"mipi-sdca-control-0xB-subproperties", "CA0B"},
    Package (2) {"mipi-sdca-control-0x10-subproperties", "LC00"}, // Latency
  }
}) //End AF01.E00A

Name(CA0B, Package()
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package ()
  {
    // boost gain, platform, RW
    Package (2) {"mipi-sdca-control-access-layer", 0x8},
    Package (2) {"mipi-sdca-control-access-mode", 0},
    //Package (2) {"mipi-sdca-control-cn-list", Package() {0x1, 0x2} },
    Package (2) {"mipi-sdca-control-cn-list", 0x6 },
  }
}) //End AF01.CA0B

//==============================================================================================

Name(E00B, Package()
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package ()
  {
    Package (2) {"mipi-sdca-entity-type", 0x07},
    Package (2) {"mipi-sdca-entity-label", "FU 32"},
    //Package (2) {"mipi-sdca-control-list", Package() { 0xB, 0x10} }, // boost gain, Latency
    Package (2) {"mipi-sdca-control-list", 0x00010800}, // Bitmap: 0xB - boost gain, 0x10 - Latency
    Package (2) {"mipi-sdca-input-pin-list", 0x2}, // Input Pin 1 connected
  },
  ToUUID("dbb8e3e6-5886-4ba6-8795-1319f52a966b"),
  Package ()
  {
    Package (2) {"mipi-sdca-input-pin-1", "E009"}, // Input Pin 1 connected to FU 0B
    Package (2) {"mipi-sdca-control-0xB-subproperties", "CB0B"},
    Package (2) {"mipi-sdca-control-0x10-subproperties", "LC00"}, // Latency
  }
}) //End AF01.E00B

Name(CB0B, Package()
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package ()
  {
    // boost gain, platform, RW
    Package (2) {"mipi-sdca-control-access-layer", 0x8},
    Package (2) {"mipi-sdca-control-access-mode", 0},
    //Package (2) {"mipi-sdca-control-cn-list", Package() {0x1, 0x2} },
    Package (2) {"mipi-sdca-control-cn-list", 0x6 },
  }
}) //End AF01.CB0B

//==============================================================================================

Name(E044, Package()
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package ()
  {
    Package (2) {"mipi-sdca-entity-type", 0x07},
    Package (2) {"mipi-sdca-entity-label", "FU 33"},
    //Package (2) {"mipi-sdca-control-list",  Package() {0xB, 0x10} }, // boost gain, Latency
    Package (2) {"mipi-sdca-control-list",  0x00010800}, // 0xB - boost gain, 0x10 - Latency
    Package (2) {"mipi-sdca-input-pin-list", 0x2}, // Input Pin 1 connected
  },
  ToUUID("dbb8e3e6-5886-4ba6-8795-1319f52a966b"),
  Package ()
  {
    Package (2) {"mipi-sdca-input-pin-1", "E043"}, // Input Pin 1 connected to FU44
    Package (2) {"mipi-sdca-control-0xB-subproperties", "C44B"},
    Package (2) {"mipi-sdca-control-0x10-subproperties", "LC00"}, // Latency
  }
}) //End AF01.E044

Name(C44B, Package()
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package ()
  {
    // gain, platform, RW
    Package (2) {"mipi-sdca-control-access-layer", 0x8},
    Package (2) {"mipi-sdca-control-access-mode", 0},
    //Package (2) {"mipi-sdca-control-cn-list", Package() {0x1, 0x2} },
    Package (2) {"mipi-sdca-control-cn-list", 0x6 },
  }
}) //End AF01.C44B

//==============================================================================================

Name(E00C, Package()
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package ()
  {
    Package (2) {"mipi-sdca-entity-type", 0x06},
    Package (2) {"mipi-sdca-entity-label", "SU 35"},
    //Package (2) {"mipi-sdca-control-list", Package() {0x1, 0x2} }, // selector, latency
    Package (2) {"mipi-sdca-control-list", 0x06}, // Bitmap: 0x1 - selector, 0x2 - latency
    Package (2) {"mipi-sdca-input-pin-list", 0xE}, // Input Pin 1/2/3 connected
  },
  ToUUID("dbb8e3e6-5886-4ba6-8795-1319f52a966b"),
  Package ()
  {
    Package (2) {"mipi-sdca-input-pin-1", "E00A"}, // Input Pin 1 connected to FU31
    Package (2) {"mipi-sdca-input-pin-2", "E00B"}, // Input Pin 1 connected to FU32
    Package (2) {"mipi-sdca-input-pin-3", "E044"}, // Input Pin 1 connected to FU33
    Package (2) {"mipi-sdca-control-0x1-subproperties", "CC01"},
    Package (2) {"mipi-sdca-control-0x2-subproperties", "LC00"},
  }
}) //End AF01.E00C

Name(CC01, Package()
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package ()
  {
    // selector, Device, RO (SU controlled by GE)
    Package (2) {"mipi-sdca-control-access-layer", 0x10},
    Package (2) {"mipi-sdca-control-access-mode", 3},
    //Package (2) {"mipi-sdca-control-cn-list", Package() {0x1, 0x2} }, // channel 1 and 2
    Package (2) {"mipi-sdca-control-cn-list", 0x6 }, // channel 1 and 2
  },
  ToUUID("edb12dd0-363d-4085-a3d2-49522ca160c4"),
  Package()
  {
    Package(2) {"mipi-sdca-control-range", "SUP3"},
  }
}) //End AF01.CC01

Name(SUP3, Buffer()
{
  0x01, 0x00, // Range type 0x0001
  0x04, 0x00, // Count of ranges = 0x4
  0x00, 0x00, 0x00, 0x00, // Input Pin 0 can be selected
  0x01, 0x00, 0x00, 0x00, // Input Pin 1 can be selected
  0x02, 0x00, 0x00, 0x00, // Input Pin 2 can be selected
  0x03, 0x00, 0x00, 0x00, // Input Pin 3 can be selected
}) //End SUP3

//==============================================================================================

Name(E00D, Package()
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package ()
  {
    Package (2) {"mipi-sdca-entity-type", 0x0A},
    Package (2) {"mipi-sdca-entity-label", "XU 36"},
    //Package (2) {"mipi-sdca-control-list", Package() {0x1, 0x06} }, // Bypass, Latency
    Package (2) {"mipi-sdca-control-list", 0x42}, // 0x1 - Bypass, 0x6 - Latency
    Package (2) {"mipi-sdca-input-pin-list", 0x2}, // Input Pin 1
  },
  ToUUID("dbb8e3e6-5886-4ba6-8795-1319f52a966b"),
  Package ()
  {
    //Package (2) {"mipi-sdca-powerdomain-entity", "E051"},
    Package (2) {"mipi-sdca-input-pin-1", "E00C"}, // Input Pin 1 connected to SU 0C
    Package (2) {"mipi-sdca-control-0x1-subproperties", "CD01"},
    Package (2) {"mipi-sdca-control-0x6-subproperties", "LC00"},
  }
}) //End AF01.E00D

Name(CD01, Package()
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package ()
  {
    // Bypass, Class, R/W
    Package (2) {"mipi-sdca-control-access-layer", 0x4},
    Package (2) {"mipi-sdca-control-access-mode", 0},
  }
}) //End AF01.CD01

//==============================================================================================
// Access mode - DC for PDE36

Name(E051, Package()
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package ()
  {
    Package (2) {"mipi-sdca-entity-type", 0x11},
    Package (2) {"mipi-sdca-entity-label", "PDE 36"},
    //Package (2) {"mipi-sdca-powerdomain-state-list", 0xF},
    Package (2) {"mipi-sdca-powerdomain-managed-list", Package() {0xd}},
    Package (2) {"mipi-sdca-powerdomain-transition-typical-delay",
      Package()
      {
        0x3, 0x0, 0x0,
        0x0, 0x3, 0x0,
        0x2, 0x0, 0x0,
        0x0, 0x2, 0x0,
        0x1, 0x0, 0x0,
        0x0, 0x1, 0x0,
        0x0, 0x0, 0x0,
      }
    },
    Package (2) {"mipi-sdca-powerdomain-transition-max-delay",
      Package()
      {
        0x3, 0x0, 0x0,
        0x0, 0x3, 0x0,
        0x2, 0x0, 0x0,
        0x0, 0x2, 0x0,
        0x1, 0x0, 0x0,
        0x0, 0x1, 0x0,
        0x0, 0x0, 0x0,
      }
    },
    //Package (2) {"mipi-sdca-control-list", Package() {0x1, 0x10}}, // Requested_PS, Actual_PS
    Package (2) {"mipi-sdca-control-list", 0x00010002}, // Bitmap: 0x1 - Requested_PS, 0x10 - Actual_PS
  },
  ToUUID("dbb8e3e6-5886-4ba6-8795-1319f52a966b"),
  Package ()
  {
    Package (2) {"mipi-sdca-control-0x1-subproperties", "RP51"},
    Package (2) {"mipi-sdca-control-0x10-subproperties", "AP51"},
  }
}) //End AF01.E051

Name(RP51, Package()
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package ()
  {
    // Requested_PS, Class, DC
    Package (2) {"mipi-sdca-control-access-layer", 0x4},
    Package (2) {"mipi-sdca-control-access-mode", 5},
    Package (2) {"mipi-sdca-control-dc-value", 0x0},
  },
  ToUUID("edb12dd0-363d-4085-a3d2-49522ca160c4"),
  Package ()
  {
    Package (2) {"mipi-sdca-control-range", "PDEC"},
  }
}) //End AF01.E051.RP51

Name(AP51, Package()
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package ()
  {
    // Actual_PS, Class, DC
    Package (2) {"mipi-sdca-control-access-layer", 0x4},
    Package (2) {"mipi-sdca-control-access-mode", 5},
    Package (2) {"mipi-sdca-control-dc-value", 0x0},
}
}) //End AF01.E051.AP51

//==============================================================================================

Name(E00F, Package()
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package ()
  {
    Package (2) {"mipi-sdca-entity-type", 0x07},
    Package (2) {"mipi-sdca-entity-label", "FU 36"},
    //Package (2) {"mipi-sdca-control-list", Package() {0x1, 0x2, 0x10} }, // Mute, Volume, Latency
    Package (2) {"mipi-sdca-control-list", 0x00010006}, // Bitmap: 0x1 - Mute, 0x2 - Volume, 0x10 - Latency
    Package (2) {"mipi-sdca-input-pin-list", 0x2}, // Input Pin 1 connected
  },
  ToUUID("dbb8e3e6-5886-4ba6-8795-1319f52a966b"),
  Package ()
  {
    Package (2) {"mipi-sdca-input-pin-1", "E00D"}, // Input Pin 1 connected to XU 0D
    Package (2) {"mipi-sdca-control-0x1-subproperties", "CF01"},
    Package (2) {"mipi-sdca-control-0x2-subproperties", "CF02"},
    Package (2) {"mipi-sdca-control-0x10-subproperties", "LC00"}, // Latency
  }
}) //End AF01.E00F

Name(CF01, Package()
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package ()
  {
    // Mute, User, RW
    Package (2) {"mipi-sdca-control-access-layer", 0x1},
    Package (2) {"mipi-sdca-control-access-mode", 0},
    Package (2) {"mipi-sdca-control-default-value", 0}, // unmute
    //Package (2) {"mipi-sdca-control-cn-list", Package() {0x1, 0x2} }, // channel 1 and 2
    Package (2) {"mipi-sdca-control-cn-list", 0x6 }, // channel 1 and 2
  }
}) //End AF01.CF01

Name(CF02, Package()
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package ()
  {
    // Volume, User, RW
    Package (2) {"mipi-sdca-control-access-layer", 0x1},
    Package (2) {"mipi-sdca-control-access-mode", 0},
    Package (2) {"mipi-sdca-control-default-value", 0x1466}, // 20.4dB  92%
    //Package (2) {"mipi-sdca-control-cn-list", Package() {0x1, 0x2} },
    Package (2) {"mipi-sdca-control-cn-list", 0x6 },
  },
  ToUUID("edb12dd0-363d-4085-a3d2-49522ca160c4"),
  Package()
  {
    Package (2) {"mipi-sdca-control-range", "BUF5"},
  }
}) //End AF01.CF02

//==============================================================================================

Name(E010, Package()
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package ()
  {
    Package (2) {"mipi-sdca-entity-type", 0x03},
    Package (2) {"mipi-sdca-entity-label", "OT 36"}, // = OT 10 in ALC722 internal spec
    Package (2) {"mipi-sdca-terminal-type", 0x0101},
    Package (2) {"mipi-sdca-input-pin-list", 0x2},
    //Package (2) {"mipi-sdca-control-list", Package()  {0x4, 0x8, 0x11}}, // Usage, Latency, DataPortIndex
    Package (2) {"mipi-sdca-control-list", 0x00020110}, // Bitmap: 0x4 - Usage, 0x8 - Latency, 0x11 - DataPortIndex
  },
  ToUUID("dbb8e3e6-5886-4ba6-8795-1319f52a966b"),
  Package ()
  {
    Package (2) {"mipi-sdca-input-pin-1", "E00F"},
    Package (2) {"mipi-sdca-terminal-clock-connection", "E011"},
    Package (2) {"mipi-sdca-control-0x4-subproperties", "C100"},
    Package (2) {"mipi-sdca-control-0x8-subproperties", "LC00"},
    Package (2) {"mipi-sdca-control-0x11-subproperties", "C101"},
  },
  // ToUUID("edb12dd0-363d-4085-a3d2-49522ca160c4"),
  // Package()
  // {
  //   Package(2) {"mipi-sdca-terminal-dp-numbers", "BUF6"},
  // }
}) //End AF01.E010

// Name(BUF6, Buffer()
// {
//   0x0, 0x2, // DP_Index, DP_Num
// }) //End AF01.E010.BUF6

Name(C100, Package() {
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package ()
  {
    // Usage, class, DC
    Package (2) {"mipi-sdca-control-access-layer", 0x4},
    Package (2) {"mipi-sdca-control-access-mode", 0x5},
    Package (2) {"mipi-sdca-control-dc-value", 1},
  },
  ToUUID("edb12dd0-363d-4085-a3d2-49522ca160c4"),
  Package()
  {
    Package(2) {"mipi-sdca-control-range", "USG2"},
  }
}) //End C100

Name(USG2, Buffer()
{
  0x07, 0x00,  // Range type 0x0007 (Septuples)
  0x01, 0x00,  // Count of ranges = 0x1
  0x01, 0x00, 0x00, 0x00, // Usage Number
  0x9a, 0x01, 0x00, 0x00, // Usage CBN, 410 (0x19a) Full band
  0x80, 0xbb, 0x00, 0x00, // Sample Rate, 48000Hz (0xBB80)
  0x18, 0x00, 0x00, 0x00, // Sample Width, 24-bits (0x18)
  0x00, 0x00, 0x00, 0x00, // Full Scale, 0 not report
  0x00, 0x00, 0x00, 0x00, // Noise Floor, 0 not report
  0x00, 0x00, 0x00, 0x00, // Usage Tag
}) //End USG2

Name(C101, Package()
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package ()
  {
    // DataPortIndex, Class, DC
    Package (2) {"mipi-sdca-control-access-layer", 0x4},
    Package (2) {"mipi-sdca-control-access-mode", 5},
    Package (2) {"mipi-sdca-control-dc-value", 0x0001}, // DP_Index_D=0x0(0xFF, no DP), DP_Index_C=0x0(0xFF,no DP), DP_Index_B=0x0(0xFF, no DP), DP_Index_A=0x01(0x02, DP2)
  },
  ToUUID("edb12dd0-363d-4085-a3d2-49522ca160c4"),
  Package()
  {
    Package(2) {"mipi-sdca-control-range", "DPS2"},
  }
}) //End AF01.E010.C101

Name(DPS2, Buffer()
{
  0x10, 0x00,  // Range type 0x0010
  0x04, 0x00,  // Count of ranges = 0x4
  0xFF, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, // DP_Index_A
  0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, // DP_Index_B
  0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, // DP_Index_C
  0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, // DP_Index_D
}) //End DPS2

//==============================================================================================

Name(E011, Package()
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package ()
  {
    Package (2) {"mipi-sdca-entity-type", 0x0B},
    Package (2) {"mipi-sdca-entity-label", "CS 36"},
    Package (2) {"mipi-sdca-cs-type", 0},
    //Package (2) {"mipi-sdca-control-list", Package() {0x10} }, // Sample_Freq_Index
    Package (2) {"mipi-sdca-control-list", 0x00010000}, // Bitmap: 0x10 - Sample_Freq_Index
  },
  ToUUID("dbb8e3e6-5886-4ba6-8795-1319f52a966b"),
  Package ()
  {
    Package (2) {"mipi-sdca-control-0x10-subproperties", "C111"},
  }
}) //End AF01.E011

Name(C111, Package()
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package ()
  {
    // Sample_Freq_Index, Class, RW
    Package (2) {"mipi-sdca-control-access-layer", 0x4},
    Package (2) {"mipi-sdca-control-access-mode", 0},
  },

  ToUUID("edb12dd0-363d-4085-a3d2-49522ca160c4"),
  Package()
  {
    Package(2) {"mipi-sdca-control-range", "BUF7"},
  }
}) //End AF01.E011.C111

Name(BUF7, Buffer()
{
  0x02, 0x00, // Range type 0x0002
  0x03, 0x00, // Count of ranges = 0x3
  0x09, 0x00, 0x00, 0x00, 0x80, 0xBB, 0x00, 0x00, // 0x00000009, 0x0000BB80(48000)
  0x0B, 0x00, 0x00, 0x00, 0x00, 0x77, 0x01, 0x00, // 0x0000000B, 0x00017700(96000)
  0x0D, 0x00, 0x00, 0x00, 0x00, 0xEE, 0x02, 0x00, // 0x0000000D, 0x0002EE00(192000)
}) //End AF01.E011.C111.BUF7

//==============================================================================================
// =================================== Below part is SPE function. =============================
/*

Name(E020, Package()
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package ()
  {
    Package (2) {"mipi-sdca-entity-type", 0x2},
    Package (2) {"mipi-sdca-entity-label", "IT 199"},
    Package (2) {"mipi-sdca-terminal-type", 0x0741},
    //Package (2) {"mipi-sdca-control-list", Package() {0x30} }, // imp-def control 0x30
    Package (2) {"mipi-sdca-control-list", 0x0001000000000000}, // Bitmap: 0x30 - imp-def control 0x30
  },
  ToUUID("dbb8e3e6-5886-4ba6-8795-1319f52a966b"),
  Package ()
  {
    // Latency, ClusterIndex, DataPort Selector
    Package (2) {"mipi-sdca-control-0x30-subproperties", "C200"},
  },

}) //End AF01.E002

Name(C200, Package()
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package ()
  {
    // imp-def control 0x30, Extension, DC
    Package (2) {"mipi-sdca-control-access-layer", 0x20},
    Package (2) {"mipi-sdca-control-access-mode", 5},
    Package (2) {"mipi-sdca-control-dc-value", 0},
  }
})

//=============================================================================

Name(E021, Package()
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package ()
  {
    Package (2) {"mipi-sdca-entity-type", 0x13},
    Package (2) {"mipi-sdca-entity-label", "PE 199"}, // = PE 21 in ALC722 internal spec
    Package (2) {"mipi-sdca-input-pin-list", 0x2}, // Input Pin 1
    //Package (2) {"mipi-sdca-control-list",  Package() {0x10} },  // Sample_Freq_Index
    Package (2) {"mipi-sdca-control-list",  0x001E}, // Bitmap: 0x01 - Private, 0x02 - Privacy_policy, 0x03 - Privacy_lockstate, 0x04 - Privacy_owner
  },
  ToUUID("dbb8e3e6-5886-4ba6-8795-1319f52a966b"),
  Package ()
  {
    Package (2) {"mipi-sdca-input-pin-1", "E020"}, // Input Pin 1 connected to IT 20
    Package (2) {"mipi-sdca-control-0x01-subproperties", "C221"},
    Package (2) {"mipi-sdca-control-0x02-subproperties", "C212"},
    Package (2) {"mipi-sdca-control-0x03-subproperties", "C213"},
    Package (2) {"mipi-sdca-control-0x04-subproperties", "C214"},
  }
}) //End AF01.E001

Name(C221, Package()
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package ()
  {
    // Private,
    Package (2) {"mipi-sdca-control-access-layer", 0x4},
    Package (2) {"mipi-sdca-control-interrupt-position", 5}, // SDCA Interrupt Bit 5
    Package (2) {"mipi-sdca-control-access-mode", 0},
  }
})

Name(C212, Package()
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package ()
  {
    // Privacy_Policy,
    Package (2) {"mipi-sdca-control-access-layer", 0x4},
    Package (2) {"mipi-sdca-control-access-mode", 0},
  }
})

Name(C213, Package()
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package ()
  {
    // Privacy_Lockstate,
    Package (2) {"mipi-sdca-control-access-layer", 0x4},
    Package (2) {"mipi-sdca-control-access-mode", 0},
  }
})

Name(C214, Package()
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package ()
  {
    // Privacy_owner,
    Package (2) {"mipi-sdca-control-access-layer", 0x4},
    Package (2) {"mipi-sdca-control-access-mode", 0},

  }
})

//=============================================================================

Name(E022, Package()
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package ()
  {
    Package (2) {"mipi-sdca-entity-type", 0x11},
    Package (2) {"mipi-sdca-entity-label", "PDE 199"},
    //Package (2) {"mipi-sdca-powerdomain-state-list", 0xF},
    //Package (2) {"mipi-sdca-powerdomain-managed-list", Package() {0x23}},
    Package (2) {"mipi-sdca-powerdomain-transition-typical-delay",
      Package()
      {
        0x3, 0x0, 0x0,
        0x0, 0x3, 0x0,
        0x2, 0x0, 0x0,
        0x0, 0x2, 0x0,
        0x1, 0x0, 0x0,
        0x0, 0x1, 0x0,
        0x0, 0x0, 0x0,
      }
    },
    Package (2) {"mipi-sdca-powerdomain-transition-max-delay",
      Package()
      {
        0x3, 0x0, 0x0,
        0x0, 0x3, 0x0,
        0x2, 0x0, 0x0,
        0x0, 0x2, 0x0,
        0x1, 0x0, 0x0,
        0x0, 0x1, 0x0,
        0x0, 0x0, 0x0,
      }
    },
    //Package (2) {"mipi-sdca-control-list", Package() {0x1, 0x10}}, // Requested_PS, Actual_PS
    Package (2) {"mipi-sdca-control-list", 0x00010002}, // 0x1 - Requested_PS, 0x10 - Actual_PS
  },
  ToUUID("dbb8e3e6-5886-4ba6-8795-1319f52a966b"),
  Package ()
  {
    Package (2) {"mipi-sdca-control-0x1-subproperties", "RP22"},
    Package (2) {"mipi-sdca-control-0x10-subproperties", "AP22"},
  }
})

Name(RP22, Package()
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package ()
  {
    // Requested_PS, Class, DC
    Package (2) {"mipi-sdca-control-access-layer", 0x4},
    Package (2) {"mipi-sdca-control-access-mode", 5},
    Package (2) {"mipi-sdca-control-dc-value", 0x0},
  },
  ToUUID("edb12dd0-363d-4085-a3d2-49522ca160c4"),
  Package ()
  {
    Package (2) {"mipi-sdca-control-range", "PDEC"},
  }
})

Name(AP22, Package()
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package ()
  {
    // Actual_PS, Class, DC
    Package (2) {"mipi-sdca-control-access-layer", 0x4},
    Package (2) {"mipi-sdca-control-access-mode", 5},
    Package (2) {"mipi-sdca-control-dc-value", 0x0},
  }
})

//=============================================================================================================

Name(E023, Package()
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package ()
  {
    Package (2) {"mipi-sdca-entity-type", 0x03},
    Package (2) {"mipi-sdca-entity-label", "OT 199"},
    Package (2) {"mipi-sdca-terminal-type", 0x0747},
    Package (2) {"mipi-sdca-input-pin-list", 0x2},
    //Package (2) {"mipi-sdca-control-list", Package() {0x30} }, // imp-def control 0x30
    Package (2) {"mipi-sdca-control-list", 0x0001000000000000}, // Bitmap: 0x30 - imp-def control 0x30
  },
  ToUUID("dbb8e3e6-5886-4ba6-8795-1319f52a966b"),
  Package ()
  {
    Package (2) {"mipi-sdca-input-pin-1", "E021"},
    //Package (2) {"mipi-sdca-powerdomain-entity", "E022"},
    Package (2) {"mipi-sdca-control-0x30-subproperties", "C234"},
  }
})

Name(C234, Package()
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package ()
  {
    // imp-def control 0x30, Extension, DC
    Package (2) {"mipi-sdca-control-access-layer", 0x20},
    Package (2) {"mipi-sdca-control-access-mode", 5},
    Package (2) {"mipi-sdca-control-dc-value", 0},
  }
})

//=============================================================================================================

*/

Name(EXT0, Package()
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package()
  {
    Package(2) {"mipi-sdca-function-expansion-subsystem-id", 0x1234 },
    Package(2) {"mipi-sdca-function-expansion-subsystem-revision-id", 0x1234},
    // both render and capture are supported
    //Package(2) {"realtek-ge-supported-terminals", Package() {0x680, 0x6A0, 0x6D0, 0x690, 0x6C0} }, // LineIn Stereo, Mic on Jack, Headset, LineOut Stereo, Headphone on Jack
    Package(2) {"realtek-ge-supported-terminals", Package() {0x000, 0x6A0, 0x6D0, 0x000, 0x6C0} }, // -------------, Mic on Jack, Headset, --------------, Headphone on Jack
    // capture is disabled
    //Package(2) {"realtek-ge-supported-terminals", Package() {0x690, 0x6C0} }, // LineOut Stereo, Headphone on Jack
    // render is disabled
    //Package(2) {"realtek-ge-supported-terminals", Package() {0x680, 0x6A0} }, // LineIn Stereo, Mic on Jack
  }
}) //End EXT0

//==============================================================================================

Name(CL11, Package()  // Stream-LR-Spatial
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package ()
  {
    Package (2) {"mipi-sdca-channel-count", 2},          // 2 channels
  },
  ToUUID("dbb8e3e6-5886-4ba6-8795-1319f52a966b"),
  Package ()
  {
    Package (2) {"mipi-sdca-channel-1-subproperties", "CH11"},  // channel 1, Generic Audio, Generic-Left
    Package (2) {"mipi-sdca-channel-2-subproperties", "CH12"},  // channel 2, Generic Audio, Generic-Right
  }
}) //End CL11

Name(CL12, Package()  // Stream-Dup-Mono
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package ()
  {
    Package (2) {"mipi-sdca-channel-count", 2},          // 2 channels
  },
  ToUUID("dbb8e3e6-5886-4ba6-8795-1319f52a966b"),
  Package ()
  {
    Package (2) {"mipi-sdca-channel-1-subproperties", "CH13"},  // channel 1, Generic Audio, Generic-Mono
    Package (2) {"mipi-sdca-channel-2-subproperties", "CH13"},  // channel 1, Generic Audio, Generic-Mono
  }
}) //End CL11

Name(CH11, Package()
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package ()
  {
    Package (2) {"mipi-sdca-cluster-channel-id", 1},      // channel 1
    Package (2) {"mipi-sdca-cluster-channel-purpose", 1},    // 0x01: Generic Audio
    Package (2) {"mipi-sdca-cluster-channel-relationship", 2},  // 0x02: Generic-Left
  }
}) //End CH11

Name(CH12, Package()
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package ()
  {
    Package (2) {"mipi-sdca-cluster-channel-id", 2},      // channel 2
    Package (2) {"mipi-sdca-cluster-channel-purpose", 1},    // 0x01: Generic Audio
    Package (2) {"mipi-sdca-cluster-channel-relationship", 3},  // 0x03: Generic-Right
  }
}) //End CH12

Name(CH13, Package()
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package ()
  {
    Package (2) {"mipi-sdca-cluster-channel-id", 1},      // channel 1
    Package (2) {"mipi-sdca-cluster-channel-purpose", 1},    // 0x01: Generic Audio
    Package (2) {"mipi-sdca-cluster-channel-relationship", 1},  // 0x03: Generic-Mono
  }
}) //End CH13
