/** @file
  SoundWire topology definitions

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

Name(_DSD, Package()
{
  // Function Descriptor
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package()
  {
    Package(2) {"mipi-sdw-sw-interface-revision", 0x00020001}, // v2.1
    Package (2) {"mipi-sdw-clockstopprepare-timeout", 0x4E5}, // 335ms + 54ms + 785ms*1.1
    Package(2) {"mipi-sdca-function-topology-features", 0x0020}, // Reference Stream (05)
    //Package(2) {"mipi-sdca-control-list", Package() {0x4, 0x5, 0x6, 0x7, 0x8} }, // Function level controls
    Package(2) {"mipi-sdca-control-list", 0xF000000301F2}, // Bitmap: 0x1, 0x4, 0x5, 0x6, 0x7, 0x8, 0x10, 0x11, 0x2C, 0x2D, 0x2E, 0x2F - Function level controls
    Package(2) {"mipi-sdca-entity-id-list", Package() {0x30, 0x2, 0x6, 0x4, 0x7, 0x4C, 0x51, 0x41, 0x42, 0x4D, 0x32, 0x31, 0x21, 0x4E} }, // List of entities in this function
    // Cluster Library
    Package (2) {"mipi-sdca-cluster-id-list", Package() {0x11, 0x21, 0x22, 0x23, 0x24, 0x31, 0x41, 0x42, 0x51} },
    // Behavior Set Library
    Package(2) {"mipi-sdca-behavior-set-id-list", Package() {0x1, 0x2} },
  },

  ToUUID("dbb8e3e6-5886-4ba6-8795-1319f52a966b"),
  Package()
  {
    Package(2) {"mipi-sdca-control-0x1-subproperties", "C042"},//Commit_Group_Mask, Class, RW
    Package(2) {"mipi-sdca-control-0x4-subproperties", "C004"},//Function_SDCA_Version
    Package(2) {"mipi-sdca-control-0x5-subproperties", "C005"},//Function_Topology
    Package(2) {"mipi-sdca-control-0x6-subproperties", "C006"},//Function_ManufacturerId
    Package(2) {"mipi-sdca-control-0x7-subproperties", "C007"},//Function_ID
    Package(2) {"mipi-sdca-control-0x8-subproperties", "C008"},//Function_Version
    Package(2) {"mipi-sdca-control-0x10-subproperties", "C043"},//Function_Status, Class, RW1C
    Package(2) {"mipi-sdca-control-0x11-subproperties", "C044"},//Function_Action, Class, RW1S
    Package(2) {"mipi-sdca-control-0x2C-subproperties", "C02C"},//Device_Manufacturer_ID
    Package(2) {"mipi-sdca-control-0x2D-subproperties", "C02D"},//Device_Part_ID
    Package(2) {"mipi-sdca-control-0x2E-subproperties", "C02E"},//Device_Version
    Package(2) {"mipi-sdca-control-0x2F-subproperties", "C02F"},//Device_SDCA_Version
    Package(2) {"mipi-sdca-entity-id-0x30-subproperties", "E030"},
    Package(2) {"mipi-sdca-entity-id-0x2-subproperties", "E002"},
    Package(2) {"mipi-sdca-entity-id-0x6-subproperties", "E006"},
    //Package(2) {"mipi-sdca-entity-id-0x56-subproperties", "E056"},
    Package(2) {"mipi-sdca-entity-id-0x4-subproperties", "E004"},
    Package(2) {"mipi-sdca-entity-id-0x7-subproperties", "E007"},
    Package(2) {"mipi-sdca-entity-id-0x4C-subproperties", "E04C"},
    Package(2) {"mipi-sdca-entity-id-0x51-subproperties", "E051"},
    Package(2) {"mipi-sdca-entity-id-0x41-subproperties", "E041"},
    Package(2) {"mipi-sdca-entity-id-0x42-subproperties", "E042"},
    Package(2) {"mipi-sdca-entity-id-0x4D-subproperties", "E04D"},
    Package(2) {"mipi-sdca-entity-id-0x32-subproperties", "E032"},
    Package(2) {"mipi-sdca-entity-id-0x31-subproperties", "E031"},
    Package(2) {"mipi-sdca-entity-id-0x21-subproperties", "E021"},
    Package(2) {"mipi-sdca-entity-id-0x4E-subproperties", "E04E"},
    //Package(2) {"mipi-sdca-entity-id-0x43-subproperties", "E043"},
    //Package(2) {"mipi-sdca-entity-id-0x44-subproperties", "E044"},
    //Package(2) {"mipi-sdca-entity-id-0x45-subproperties", "E045"},
    //Package(2) {"mipi-sdca-entity-id-0x46-subproperties", "E046"},
    Package(2) {"mipi-sdca-function-expansion-subproperties", "EXT0"},
    // Cluster ID subproperties
    Package (2) {"mipi-sdca-cluster-id-0x11-subproperties", "CL11"},
    Package (2) {"mipi-sdca-cluster-id-0x21-subproperties", "CL21"},
    Package (2) {"mipi-sdca-cluster-id-0x22-subproperties", "CL22"},
    Package (2) {"mipi-sdca-cluster-id-0x23-subproperties", "CL23"},
    Package (2) {"mipi-sdca-cluster-id-0x24-subproperties", "CL24"},
    Package (2) {"mipi-sdca-cluster-id-0x31-subproperties", "CL31"},
    Package (2) {"mipi-sdca-cluster-id-0x41-subproperties", "CL41"},
    Package (2) {"mipi-sdca-cluster-id-0x42-subproperties", "CL42"},
    Package (2) {"mipi-sdca-cluster-id-0x51-subproperties", "CL51"},
    // Behavior Set subproperties
    Package(2) {"mipi-sdca-behavior-set-id-0x1-subproperties", "BSP1"},
    Package(2) {"mipi-sdca-behavior-set-id-0x2-subproperties", "BSP2"},
  },

  ToUUID("edb12dd0-363d-4085-a3d2-49522ca160c4"),
  Package()
  {
    Package(2) {"mipi-sdca-function-initialization-table","BUF0"},
  }
}) //End _DSD

// By Programming guide, Application 2. Version: 0.05 (2022/12/21)
Name(BUF0, Buffer()
{
  0x3C, 0x20, 0x00, 0x02, 0xC2, // 0x0200203C, 0xC2
  0x3C, 0x00, 0x00, 0x02, 0x15, // 0x0200003C, 0x15

  0x00, 0x20, 0x81, 0x05, 0x70, // 0x05812000, 0x70
  0x00, 0x00, 0x81, 0x05, 0x2C, // 0x05810000, 0x2C

  0x00, 0x20, 0x81, 0x05, 0x70, // 0x05812000, 0x70
  0x00, 0x00, 0x81, 0x05, 0x2D, // 0x05810000, 0x2D

  0x29, 0x20, 0x10, 0x06, 0x41, // 0x06102029, 0x41
  0x29, 0x00, 0x10, 0x06, 0x41, // 0x06100029, 0x41

  0x55, 0x20, 0x10, 0x06, 0x00, // 0x06102055, 0x00
  0x55, 0x00, 0x10, 0x06, 0x02, // 0x06100055, 0x02

  0x38, 0x20, 0x00, 0x02, 0x89, // 0x02002038, 0x89     20241121 fine-tune speaker power to meet 2w/4ohm
  0x38, 0x00, 0x00, 0x02, 0x0A, // 0x02000038, 0x0A

  0x00, 0x81, 0x38, 0x41, 0x04, // 0x41388100, 0x04

  0x03, 0x2F, 0x00, 0x00, 0x06, // 0x00002F03, 0x06 (PHYtiming TDZ/TZD control)

  0x37, 0x20, 0x00, 0x02, 0xFC, // 0x02002037, 0xFC  silence detect to power down class-D
  0x37, 0x00, 0x00, 0x02, 0x15, // 0x02000037, 0x15

  0x00, 0x20, 0xD1, 0x05, 0x99, // 0x05D12000, 0x99    AGC limit to 0dB for non Dolby case
  0x00, 0x00, 0xD1, 0x05, 0xFF, // 0x05D10000, 0xFF

  0x02, 0x20, 0xD1, 0x05, 0x00, // 0x05D12002, 0x00
  0x02, 0x00, 0xD1, 0x05, 0x00, // 0x05D10002, 0x00

  0x03, 0x20, 0xD1, 0x05, 0x00, // 0x05D12003, 0x00
  0x03, 0x00, 0xD1, 0x05, 0x25, // 0x05D10003, 0x25    //Coefficient for low pass filter in ALC set 2^-9, move 3dB frequency to 200Hz

  0x04, 0x20, 0xD1, 0x05, 0x5F, // 0x05D12004, 0x5F
  0x04, 0x00, 0xD1, 0x05, 0x5F, // 0x05D10004, 0x5F

}) //End BUF0

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
    Package(2) {"mipi-sdca-control-interrupt-position", 27}, //IntStat_SDCA_27
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
    Package(2) {"mipi-sdca-control-access-layer", 0x4},
    Package(2) {"mipi-sdca-control-access-mode", 5},
    Package(2) {"mipi-sdca-control-dc-value", 0x10},
  }
}) //End C004

Name(C005, Package()
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package()
  {
    // Function_Topology = 0x1 Smart-Amp, Class, DC
    Package(2) {"mipi-sdca-control-access-layer", 0x4},
    Package(2) {"mipi-sdca-control-access-mode", 5},
    Package(2) {"mipi-sdca-control-dc-value", 0x1},
  }
}) //End C005

Name(C006, Package()
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package()
  {
    // Function_ManufacturerId = 0x025d, Class, DC
    Package(2) {"mipi-sdca-control-access-layer", 0x4},
    Package(2) {"mipi-sdca-control-access-mode", 5},
    Package(2) {"mipi-sdca-control-dc-value", 0x025d},
  }
}) //End C006

Name(C007, Package()
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package()
  {
    // Function_ID = 0x0722 (Device specific), Class, DC
    Package(2) {"mipi-sdca-control-access-layer", 0x4},
    Package(2) {"mipi-sdca-control-access-mode", 5},
    Package(2) {"mipi-sdca-control-dc-value", 0x0722},
  }
}) //End C007

Name(C008, Package()
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package()
  {
    // Function_Version = 0x1, Class, DC
    Package(2) {"mipi-sdca-control-access-layer", 0x4},
    Package(2) {"mipi-sdca-control-access-mode", 5},
    Package(2) {"mipi-sdca-control-dc-value", 0x1},
  }
}) //End C008


Name(E031, Package()
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package()
  {
    Package(2) {"mipi-sdca-entity-type", 0x0b},
    Package(2) {"mipi-sdca-entity-label", "CS 21"},
    Package(2) {"mipi-sdca-cs-type", 0},
    //Package(2) {"mipi-sdca-control-list", Package() {0x10} }, // Sample_Freq_Index
    Package(2) {"mipi-sdca-control-list", 0x00010000}, // Bitmap: 0x10 - Sample_Freq_Index
  },

  ToUUID("dbb8e3e6-5886-4ba6-8795-1319f52a966b"),
  Package()
  {
    Package(2) {"mipi-sdca-control-0x10-subproperties", "C311"},
  }
}) //End E031

Name(C311, Package()
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package()
  {
    // Sample_Freq_Index, Class, RW
    Package(2) {"mipi-sdca-control-access-layer", 0x4},
    Package(2) {"mipi-sdca-control-access-mode", 0},
  },

  ToUUID("edb12dd0-363d-4085-a3d2-49522ca160c4"),
  Package()
  {
    Package(2) {"mipi-sdca-control-range","BUF1"},
  }
}) //End C311

Name(BUF1, Buffer()
{
  0x02, 0x00, // Range type 0x0002
  0x03, 0x00, // Count of ranges = 0x3
  0x09, 0x00, 0x00, 0x00, 0x80, 0xBB, 0x00, 0x00, // 0x00000009, 0x0000BB80(48000)
  0x0B, 0x00, 0x00, 0x00, 0x00, 0x77, 0x01, 0x00, // 0x0000000B, 0x00017700(96000)
  0x0D, 0x00, 0x00, 0x00, 0x00, 0xEE, 0x02, 0x00, // 0x0000000D, 0x0002EE00(192000)
}) //End BUF1

Name(E030, Package()
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package()
  {
    Package(2) {"mipi-sdca-entity-type", 0x2},
    Package(2) {"mipi-sdca-entity-label", "IT 21"},
    Package(2) {"mipi-sdca-terminal-type", 0x0101}, // Generic Sink DataPort
    //Package(2) {"mipi-sdca-control-list", Package() {0x4, 0x8, 0x10, 0x11} }, // Usage, Latency, ClusterIndex, DataPortIndex
    Package(2) {"mipi-sdca-control-list", 0x00030110}, // Bitmap: 0x4 - Usage, 0x8 - Latency, 0x10 - ClusterIndex, 0x11 - DataPortIndex
  },

  ToUUID("dbb8e3e6-5886-4ba6-8795-1319f52a966b"),
  Package()
  {
    Package(2) {"mipi-sdca-control-0x4-subproperties", "C304"},
    Package(2) {"mipi-sdca-control-0x8-subproperties", "LC00"},
    Package(2) {"mipi-sdca-control-0x10-subproperties", "C300"},
    Package(2) {"mipi-sdca-control-0x11-subproperties", "C301"},
    Package(2) {"mipi-sdca-terminal-clock-connection", "E031"},
  },

  // ToUUID("edb12dd0-363d-4085-a3d2-49522ca160c4"),
  // Package()
  // {
  //   Package(2) {"mipi-sdca-terminal-dp-numbers","BUF2"},
  // }
}) //End E030

// Name(BUF2, Buffer()
// {
//   0x0, 0x3, // DP_Index: 0x0 DP_Num: 0x3
// }) //End BUF2

Name(C304, Package()
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package()
  { // Usage, Class, DC
    Package(2) { "mipi-sdca-control-access-layer", 0x4 },
    Package(2) {"mipi-sdca-control-access-mode", 5},
    Package(2) {"mipi-sdca-control-dc-value", 1},
  },
  ToUUID("edb12dd0-363d-4085-a3d2-49522ca160c4"),
  Package()
  {
    Package(2) {"mipi-sdca-control-range", "USG1"},
  }
}) //End C304

Name(USG1, Buffer()
{
  0x07, 0x00,  // Range type 0x0007 (Septuples)
  0x02, 0x00,  // Count of ranges = 0x2
  0x01, 0x00, 0x00, 0x00, // Usage Number
  0xae, 0x01, 0x00, 0x00, // Usage CBN, 430 (0x1ae) Full band and Near-ultrasound
  0x80, 0xbb, 0x00, 0x00, // Sample Rate, 48000Hz (0xBB80)
  0x18, 0x00, 0x00, 0x00, // Sample Width, 24-bits (0x18)
  0x00, 0x00, 0x00, 0x00, // Full Scale, 0 not report
  0x00, 0x00, 0x00, 0x00, // Noise Floor, 0 not report
  0x00, 0x00, 0x00, 0x00, // Usage Tag
  0x01, 0x00, 0x00, 0x00, // Usage Number
  0xae, 0x01, 0x00, 0x00, // Usage CBN, 430 (0x1ae) Full band and Near-ultrasound
  0x80, 0xbb, 0x00, 0x00, // Sample Rate, 48000Hz (0xBB80)
  0x10, 0x00, 0x00, 0x00, // Sample Width, 16-bits (0x10)
  0x00, 0x00, 0x00, 0x00, // Full Scale, 0 not report
  0x00, 0x00, 0x00, 0x00, // Noise Floor, 0 not report
  0x00, 0x00, 0x00, 0x00, // Usage Tag
}) //End USG1

Name(LC00, Package()
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package()
  {
    // Latency, Class, DC
    Package(2) {"mipi-sdca-control-access-layer", 0x4},
    Package(2) {"mipi-sdca-control-access-mode", 5},
    Package(2) {"mipi-sdca-control-dc-value", 0},
  }
}) //End LC00

Name(C300, Package()
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package()
  {
    // ClusterIndex, Class, DC
    Package(2) {"mipi-sdca-control-access-layer", 0x4},
    Package(2) {"mipi-sdca-control-access-mode", 5},
    Package(2) {"mipi-sdca-control-dc-value", 1},
  },
  ToUUID("edb12dd0-363d-4085-a3d2-49522ca160c4"),
  Package()
  {
    Package(2) {"mipi-sdca-control-range", "CLM1"},
  }
})

Name(CLM1, Buffer()
{
  0x02, 0x00,  // Range type 2 parameters
  0x01, 0x00,  // Count of ranges = 0x1
  0x01, 0x00, 0x00, 0x00, 0x11, 0x00, 0x00, 0x00, // Cluster Index = 0x1,  Cluster ID = 0x11
}) //End CLM1

Name(C301, Package()
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package()
  {
    // DataPortIndex, Class, DC
    Package(2) {"mipi-sdca-control-access-layer", 0x4},
    Package(2) {"mipi-sdca-control-access-mode", 5},
    Package(2) {"mipi-sdca-control-dc-value", 0x0001}, // DP_Index_D=0x0(0xFF, no DP), DP_Index_C=0x0(0xFF,no DP), DP_Index_B=0x0(0xFF, no DP), DP_Index_A=0x01(0x03, DP3)
  },
  ToUUID("edb12dd0-363d-4085-a3d2-49522ca160c4"),
  Package()
  {
    Package(2) {"mipi-sdca-control-range", "DPS1"},
  }
}) //End C301

Name(DPS1, Buffer()
{
  0x10, 0x00,  // Range type 0x0010
  0x04, 0x00,  // Count of ranges = 0x4
  0xFF, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, // DP_Index_A
  0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, // DP_Index_B
  0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, // DP_Index_C
  0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, // DP_Index_D
}) //End DPS1

Name(E002, Package()
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package()
  {
    Package(2) {"mipi-sdca-entity-type", 0x25},
    Package(2) {"mipi-sdca-entity-label", "PPU 21"},
    Package(2) {"mipi-sdca-control-list", 0x10000}, // Bitmap: 0x10 - PostureNumber
    Package(2) {"mipi-sdca-input-pin-list", 0x2}, // Input Pin 1 connected
  },

  ToUUID("dbb8e3e6-5886-4ba6-8795-1319f52a966b"),
  Package()
  {
    Package(2) {"mipi-sdca-input-pin-1", "E030"}, // Input Pin 1 connected to IT21
    Package(2) {"mipi-sdca-control-0x10-subproperties", "C210"},
  }
})

Name(C210, Package()
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package ()
  {  // PostureNumber, Class, DC
    Package (2) {"mipi-sdca-control-access-layer", 0x4},
    Package (2) {"mipi-sdca-control-access-mode", 5},
    Package (2) {"mipi-sdca-control-dc-value", 1},
    },
  ToUUID("edb12dd0-363d-4085-a3d2-49522ca160c4"),
    Package()
  {
    Package(2) {"mipi-sdca-control-range", "PSM1"},
  }
})  // End C210

Name(PSM1, Buffer()
{
  0x0B, 0x00,  // Range type 0x000B (Septuples)
  0x01, 0x00,  // Count of ranges = 0x1
  0x00, 0x00, 0x00, 0x00, 0x67, 0x01, 0x00, 0x00, // Pitch Min (0), Pitch Max (359)
  0x00, 0x00, 0x00, 0x00, 0x67, 0x01, 0x00, 0x00, // Roll Min (0), Roll Max (359)
  0x00, 0x00, 0x00, 0x00, 0x67, 0x01, 0x00, 0x00, // Yaw Min (0), Yaw Max (359)
  0x00, 0x00, 0x00, 0x00, 0x67, 0x01, 0x00, 0x00, // HingeAngle Min (0), HingeAngle Max (359)
  0x00, 0x00, 0x00, 0x00, // System Extension Value
  0x01, 0x00, 0x00, 0x00, // Posture Number
  0x21, 0x00, 0x00, 0x00, // Cluster ID
}) //End PSM1


Name(E006, Package()
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package()
  {
    Package(2) {"mipi-sdca-entity-type", 0x07},
    Package(2) {"mipi-sdca-entity-label", "FU 21"},
    //Package(2) {"mipi-sdca-control-list", Package() {0x1, 0x2, 0x10} }, // Mute, Volume, Latency
    Package(2) {"mipi-sdca-control-list", 0x00010006}, // Bitmap: 0x1 - Mute, 0x2 - Volume, 0x10 - Latency
    Package(2) {"mipi-sdca-input-pin-list", 0x2}, // Input Pin 1 connected
  },

  ToUUID("dbb8e3e6-5886-4ba6-8795-1319f52a966b"),
  Package()
  {
    Package(2) {"mipi-sdca-input-pin-1", "E002"}, // Input Pin 1 connected to PPU21
    Package(2) {"mipi-sdca-control-0x1-subproperties", "C601"},
    Package(2) {"mipi-sdca-control-0x2-subproperties", "C602"},
    Package(2) {"mipi-sdca-control-0x10-subproperties", "LC00"},
  }
}) //End E006

Name(C601, Package()
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package()
  {
    // Mute, User, RW
    Package(2) {"mipi-sdca-control-access-layer", 0x1},
    Package(2) {"mipi-sdca-control-access-mode", 0},
    Package(2) {"mipi-sdca-control-default-value", 0x0000}, // unmute
    Package(2) {"mipi-sdca-control-cn-list", 0x6 },
  }
}) //End C601

Name(C602, Package()
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package()
  {
    // Volume, User, RW
    Package(2) {"mipi-sdca-control-access-layer", 0x1},
    Package(2) {"mipi-sdca-control-access-mode", 0},
    Package(2) {"mipi-sdca-control-default-value", 0x0000}, // 0dB
    //Package(2) {"mipi-sdca-control-fixed-value", 0x0000},
    Package(2) {"mipi-sdca-control-cn-list", 0x6 },
  },

  ToUUID("edb12dd0-363d-4085-a3d2-49522ca160c4"),
  Package()
  {
    Package(2) {"mipi-sdca-control-range","BUF3"},
  }
}) //End C602

Name(BUF3, Buffer()
{
  0x03, 0x00, // Range type 0x0003 (Triples)
  0x01, 0x00, // Count of ranges = 0x1
  0xC0, 0xBE, 0x00, 0x00, // Min = 0x0000BEC0,  // -65.25dB
  0x00, 0x00, 0x00, 0x00, // Max = 0x00000000,  // 0dB
  0xC0, 0x00, 0x00, 0x00, // Step = 0x000000C0, // 0.75dB
}) //End BUF3


Name(E04E, Package()
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package ()
  {
    Package (2) {"mipi-sdca-entity-type", 0x22},
    Package (2) {"mipi-sdca-entity-label", "MFPU 21"},
    //Package (2) {"mipi-sdca-control-list", 0x00D90232}, // Bitmap: 0x1 - Bypass, 0x4 - Algorithm_Ready, 0x5 - Algorithm_Enable, 0x9 - Algorithm_Prepare, 0x10 - ClusterIndex, 0x13 - Algorithm_Number, 0x14 - Algorithm_CurrentOwner, 0x16 - AE_MessageOffset, 0x17 - AE_MessageLength
    Package (2) {"mipi-sdca-control-list", 0x00000002}, // Bitmap: 0x1 - Bypass
    Package (2) {"mipi-sdca-input-pin-list", 0x2}, // Input Pin 1 connected
  },

  ToUUID("dbb8e3e6-5886-4ba6-8795-1319f52a966b"),
  Package ()
  {
    Package (2) {"mipi-sdca-input-pin-1", "E006"}, // Input Pin 1 connected to FU21
    Package (2) {"mipi-sdca-control-0x1-subproperties", "C4E1"},
    // Package (2) {"mipi-sdca-control-0x4-subproperties", "C4E4"},
    // Package (2) {"mipi-sdca-control-0x5-subproperties", "C4E5"},
    // Package (2) {"mipi-sdca-control-0x9-subproperties", "C4E9"},
    // Package (2) {"mipi-sdca-control-0x10-subproperties", "CE10"},
    // Package (2) {"mipi-sdca-control-0x13-subproperties", "CE13"},
    // Package (2) {"mipi-sdca-control-0x14-subproperties", "CE14"},
    // Package (2) {"mipi-sdca-control-0x16-subproperties", "CE16"},
    // Package (2) {"mipi-sdca-control-0x17-subproperties", "CE17"},
  }
}) //End E04E

Name(C4E1, Package()
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package ()
  {
    // Bypass, Class, DC:1 (Null MFPU)
    Package (2) {"mipi-sdca-control-access-layer", 0x4},
    Package (2) {"mipi-sdca-control-access-mode", 5},
    Package (2) {"mipi-sdca-control-dc-value", 1},
  }
})//End C4E1

// Name(C4E1, Package()
// {
//   ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
//   Package ()
//   {
//     // Bypass, Class, DC:0
//     Package (2) {"mipi-sdca-control-access-layer", 0x4},
//     Package (2) {"mipi-sdca-control-access-mode", 5},
//     Package (2) {"mipi-sdca-control-dc-value", 0},
//   }
// })//End C4E1

// Name(C4E4, Package()
// {
//   ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
//   Package ()
//   {
//     // Algorithm_Ready, Class, RO
//     Package (2) {"mipi-sdca-control-access-layer", 0x4},
//     Package (2) {"mipi-sdca-control-access-mode", 3},
//   }
// })//End C4E4

// Name(C4E5, Package()
// {
//   ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
//   Package ()
//   {
//     // Algorithm_Enable, Class/Extension/Device, RW
//     Package (2) {"mipi-sdca-control-access-layer", 0x34},
//     Package (2) {"mipi-sdca-control-access-mode", 0},
//   }
// })//End C4E5

// Name(C4E9, Package()
// {
//   ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
//   Package ()
//   {
//     // Algorithm_Prepare, Class/Extension/Device, RW
//     Package (2) {"mipi-sdca-control-access-layer", 0x34},
//     Package (2) {"mipi-sdca-control-access-mode", 0},
//   }
// })//End C4E9

// Name(CE10, Package()
// {
//   ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
//   Package ()
//   {
//     // ClusterIndex, Class/Extension, DC
//     Package (2) {"mipi-sdca-control-access-layer", 0x24},
//     Package (2) {"mipi-sdca-control-access-mode", 5},
//     Package (2) {"mipi-sdca-control-dc-value", 0}, // output inherits the Cluster from the input
//   },
//   ToUUID("edb12dd0-363d-4085-a3d2-49522ca160c4"),
//   Package()
//   {
//     Package(2) {"mipi-sdca-control-range", "CLM2"},
//   }
// })//End CE10

// Name(CLM2, Buffer()
// {
//   0x02, 0x00,  // Range type 2 parameters
//   0x01, 0x00,  // Count of ranges = 0x1
//   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // Cluster Index = 0x0, Cluster ID = 0x0 means inherit the Cluster from Input Pin #1
// }) //End CLM2

// Name(CE13, Package()
// {
//   ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
//   Package ()
//   {
//     // Algorithm_Number, Class, RO
//     Package (2) {"mipi-sdca-control-interrupt-position", 26},// bit 26 in SDCA interrupt
//     Package (2) {"mipi-sdca-control-access-layer", 0x4},
//     Package (2) {"mipi-sdca-control-access-mode", 3},
//   }
// })//End CE13

// Name(CE14, Package()
// {
//   ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
//   Package ()
//   {
//     // Algorithm_CurrentOwner, Class, RW1S
//     Package (2) {"mipi-sdca-control-interrupt-position", 25},// bit 25 in SDCA interrupt
//     Package (2) {"mipi-sdca-control-access-layer", 0x4},
//     Package (2) {"mipi-sdca-control-access-mode", 4},
//   }
// })//End CE14

// Name(CE16, Package()
// {
//   ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
//   Package ()
//   {
//     // AE_MessageOffset, Class, RW
//     Package (2) {"mipi-sdca-control-access-layer", 0x4},
//     Package (2) {"mipi-sdca-control-access-mode", 0},
//   }
// })//End CE16

// Name(CE17, Package()
// {
//   ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
//   Package ()
//   {
//     // AE_MessageLength, Class, RW
//     Package (2) {"mipi-sdca-control-access-layer", 0x4},
//     Package (2) {"mipi-sdca-control-access-mode", 0},
//   }
// })//End CE17


// Access mode - DC for PDE22

Name(E056, Package()
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package()
  {
    Package(2) {"mipi-sdca-entity-type", 0x11},
    Package(2) {"mipi-sdca-entity-label", "PDE 22"},
    //Package(2) {"mipi-sdca-powerdomain-state-list", 0xF},
    Package(2) {"mipi-sdca-powerdomain-managed-list", Package() {0x04}},
    Package(2) {"mipi-sdca-powerdomain-transition-typical-delay",
      Package()
      {
        0x3, 0x0, 0,
        0x0, 0x3, 0,
        0x2, 0x0, 0,
        0x0, 0x2, 0,
        0x1, 0x0, 0,
        0x0, 0x1, 0,
        0x0, 0x0, 0,
      },
    },
    Package(2) {"mipi-sdca-powerdomain-transition-max-delay",
      Package()
      {
        0x3, 0x0, 0,
        0x0, 0x3, 0,
        0x2, 0x0, 0,
        0x0, 0x2, 0,
        0x1, 0x0, 0,
        0x0, 0x1, 0,
        0x0, 0x0, 0,
      },
    },
    //Package(2) {"mipi-sdca-control-list", Package() {0x1, 0x10} }, // Requested_PS, Actual_PS
    Package(2) {"mipi-sdca-control-list", 0x00010002}, // Bitmap: 0x1 - Requested_PS, 0x10 - Actual_PS
  },

  ToUUID("dbb8e3e6-5886-4ba6-8795-1319f52a966b"),
  Package()
  {
    Package(2) {"mipi-sdca-control-0x1-subproperties", "RP56"},
    Package(2) {"mipi-sdca-control-0x10-subproperties", "AP56"},
  }
}) //End E056

Name(RP56, Package()
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package()
  {
    // Requested_PS, Class, DC
    Package(2) {"mipi-sdca-control-access-layer", 0x4},
    Package(2) {"mipi-sdca-control-access-mode", 5},
    Package (2) {"mipi-sdca-control-dc-value", 0x0},
  },
  ToUUID("edb12dd0-363d-4085-a3d2-49522ca160c4"),
  Package ()
  {
    Package (2) {"mipi-sdca-control-range", "PDEC"},
  }
}) //End RP56

Name(PDEC, Buffer()
{
  0x01, 0x00, // Range type 0x0001
  0x01, 0x00, // Count of ranges = 0x1
  0x00, 0x00, 0x00, 0x00, // PS0
  //0x03, 0x00, 0x00, 0x00, // PS3
}) //End PDEC

Name(AP56, Package()
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package()
  {
    // Actual_PS, Class, DC
    Package(2) {"mipi-sdca-control-access-layer", 0x4},
    Package(2) {"mipi-sdca-control-access-mode", 5},
    Package (2) {"mipi-sdca-control-dc-value", 0x0},
  }
}) //End AP56

Name(E004, Package()
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package()
  {
    Package(2) {"mipi-sdca-entity-type", 0x0A},
    Package(2) {"mipi-sdca-entity-label", "XU 22"},
    //Package(2) {"mipi-sdca-control-list", Package() {0x1, 0x6} }, // Bypass, Latency
    Package(2) {"mipi-sdca-control-list", 0x0042}, // Bitmap: 0x1 - Bypass, 0x6 - Latency
    Package(2) {"mipi-sdca-input-pin-list", 0x2}, // Input Pin 1 & 2 connected
  },

  ToUUID("dbb8e3e6-5886-4ba6-8795-1319f52a966b"),
  Package()
  {
    //Package (2) {"mipi-sdca-powerdomain-entity", "E056"},
    Package(2) {"mipi-sdca-input-pin-1", "E04E"}, // Input Pin 1 connected to FU21
    Package(2) {"mipi-sdca-control-0x1-subproperties", "C401"},
    Package(2) {"mipi-sdca-control-0x6-subproperties", "LC00"},
  }
}) //End E004

Name(C401, Package()
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package()
  {
    // Bypass, Class, R/W
    Package(2) {"mipi-sdca-control-access-layer", 0x4},
    Package(2) {"mipi-sdca-control-access-mode", 0},
  }
}) //End C401

Name(E007, Package()
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package()
  {
    Package(2) {"mipi-sdca-entity-type", 0x24},
    Package(2) {"mipi-sdca-entity-label", "SAPU 29"},
    Package(2) {"mipi-sdca-input-pin-list", 0x2}, // Input Pin 1 connected
    //Package(2) {"mipi-sdca-control-list", Package() {0x5, 0x10, 0x11} }, // Latency, Protection_Mode, Protection_Status
    Package(2) {"mipi-sdca-control-list", 0x00030020}, // Bitmap: 0x5 - Latency, 0x10 - Protection_Mode, 0x11 - Protection_Status
  },

  ToUUID("dbb8e3e6-5886-4ba6-8795-1319f52a966b"),
  Package()
  {
    Package(2) {"mipi-sdca-input-pin-1", "E004" }, // Input Pin 1 connected to XU22
    //Package(2) {"mipi-sdca-input-pin-2", "E012"}, // Input Pin 2 connected to IT29
    Package(2) {"mipi-sdca-control-0x5-subproperties", "LC00"},
    Package(2) {"mipi-sdca-control-0x10-subproperties", "C710"},
    Package(2) {"mipi-sdca-control-0x11-subproperties", "C711"},
  }
}) //End E007

Name(C710, Package()
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package()
  {
    // Protection_Mode, Class, RO
    Package(2) {"mipi-sdca-control-access-layer", 0x4},
    Package(2) {"mipi-sdca-control-interrupt-position", 24}, // SDCA Interrupt Bit 24
    Package(2) {"mipi-sdca-control-access-mode", 3},
  }
}) //End C710

Name(C711, Package()
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package()
  {
    // Protection_Status, Class, RO
    Package(2) {"mipi-sdca-control-access-layer", 0x4},
    Package(2) {"mipi-sdca-control-access-mode", 3},
  },
  ToUUID("edb12dd0-363d-4085-a3d2-49522ca160c4"),
  Package()
  {
    Package(2) {"mipi-sdca-control-range", "BMT1"},
  }
}) //End C711

Name(BMT1, Buffer()
{
  0x03, 0x00,  // Range type 3 parameters
  0x02, 0x00,  // Count of ranges = 0x2
  0x00, 0x00, 0x00, 0x00, // BitNumber 0
  0x01, 0x00, 0x00, 0x00, // Behavior Set ID 0x1 (BSP1)
  0x00, 0x00, 0x00, 0x00, // String Number, Null string
  0x01, 0x00, 0x00, 0x00, // BitNumber 1
  0x02, 0x00, 0x00, 0x00, // Behavior Set ID 0x2 (BSP2)
  0x00, 0x00, 0x00, 0x00, // String Number, Null string
}) //End BMT1

Name(BSP1, Package()
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package()
  {
    Package(2) {"mipi-sdca-behavior-set-cbn-list", Package() { 1060 } }, // Internal Condition
  }
}) //End BSP1

Name(BSP2, Package()
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package()
  {
    Package(2) {"mipi-sdca-behavior-set-cbn-list", Package() { 1100 } }, // Catastrophic Fault
  }
}) //End BSP2

Name(E04C, Package()
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package()
  {
    Package(2) {"mipi-sdca-entity-type", 0x21},
    Package(2) {"mipi-sdca-entity-label", "UDMPU 23"},
    Package(2) {"mipi-sdca-input-pin-list", 0x2}, // Input Pin 1 connected
    //Package(2) {"mipi-sdca-control-list", Package() {0x6, 0x10} }, // Latency, ClusterIndex
    Package(2) {"mipi-sdca-control-list", 0x00010040}, // Bitmap: 0x6 - Latency, 0x10 - ClusterIndex
  },
  ToUUID("dbb8e3e6-5886-4ba6-8795-1319f52a966b"),
  Package()
  {
    Package(2) {"mipi-sdca-input-pin-1", "E007"},
    Package(2) {"mipi-sdca-control-0x6-subproperties", "LC00"},
    Package(2) {"mipi-sdca-control-0x10-subproperties", "C4C1"},
  }
}) //End E04C

Name(C4C1, Package()
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package()
  {
    // ClusterIndex, Class, DC
    Package(2) {"mipi-sdca-control-access-layer", 4},
    Package(2) {"mipi-sdca-control-access-mode", 5},
    Package(2) {"mipi-sdca-control-dc-value", 1},
  },
  ToUUID("edb12dd0-363d-4085-a3d2-49522ca160c4"),
  Package()
  {
    Package(2) {"mipi-sdca-control-range", "CLM3"},
  }
}) //End C4C1

Name(CLM3, Buffer()
{
  0x02, 0x00,  // Range type 2 parameters
  0x01, 0x00,  // Count of ranges = 0x1
  0x01, 0x00, 0x00, 0x00, 0x31, 0x00, 0x00, 0x00, // Cluster Index = 0x1,  Cluster ID = 0x31
}) //End CLM3

// FU 0x15 should be platform access only
Name(E051, Package()
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package ()
  {
    Package(2) {"mipi-sdca-entity-type", 0x07},
    Package(2) {"mipi-sdca-entity-label", "FU 23"},
    Package(2) {"mipi-sdca-control-list", 0x10000}, // Bitmap: 0x10 - Latency
    Package(2) {"mipi-sdca-input-pin-list", 0x2}, // Input Pin 1 connected
  },

  ToUUID("dbb8e3e6-5886-4ba6-8795-1319f52a966b"),
  Package ()
  {
    Package(2) {"mipi-sdca-input-pin-1", "E04C"}, // Input Pin 1 connected to UDMPU 23
    Package(2) {"mipi-sdca-control-0x10-subproperties", "LC00"},
  }
}) //End E015

Name(E041, Package()
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package()
  {
    Package(2) {"mipi-sdca-entity-type", 0x11},
    Package(2) {"mipi-sdca-entity-label", "PDE 23"},
    //Package(2) {"mipi-sdca-powerdomain-state-list", 0xF},
    Package(2) {"mipi-sdca-powerdomain-managed-list", Package() {0x42} },
    Package(2) {"mipi-sdca-powerdomain-transition-typical-delay",
      Package()
      {
        0x3, 0x0, 384000,
        0x0, 0x3, 384000,
        0x2, 0x0, 0,
        0x0, 0x2, 0,
        0x1, 0x0, 0,
        0x0, 0x1, 0,
        0x0, 0x0, 0,
      }
    },
    Package(2) {"mipi-sdca-powerdomain-transition-max-delay",
      Package()
      {
        0x3, 0x0, 500000,
        0x0, 0x3, 500000,
        0x2, 0x0, 0,
        0x0, 0x2, 0,
        0x1, 0x0, 0,
        0x0, 0x1, 0,
        0x0, 0x0, 0,
      }
    },
    //Package(2) {"mipi-sdca-control-list", Package() {0x1, 0x10} },
    Package(2) {"mipi-sdca-control-list", 0x00010002},  // Bitmap: 0x1, 0x10
  },

  ToUUID("dbb8e3e6-5886-4ba6-8795-1319f52a966b"),
  Package()
  {
    Package(2) {"mipi-sdca-control-0x1-subproperties", "RP41"},
    Package(2) {"mipi-sdca-control-0x10-subproperties", "AP41"},
  }
}) //End E041

Name(RP41, Package()
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
}) //End AF04.E041.RP41

Name(PDER, Buffer()
{
  0x01, 0x00, // Range type 0x0001
  0x02, 0x00, // Count of ranges = 0x2
  0x00, 0x00, 0x00, 0x00, // PS0
  0x03, 0x00, 0x00, 0x00, // PS3
}) //End PDER

Name(AP41, Package()
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package ()
  {
    // Actual_PS, Class, RO
    Package (2) {"mipi-sdca-control-access-layer", 0x4},
    Package (2) {"mipi-sdca-control-access-mode", 3},
  }
}) //End AF04.E041.AP41

Name(E042, Package()
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package()
  {
    Package(2) {"mipi-sdca-entity-type", 0x03},
    Package(2) {"mipi-sdca-entity-label", "OT 23"},
    Package(2) {"mipi-sdca-terminal-type", 0x0380}, // Primary Full Speaker
    Package(2) {"mipi-sdca-input-pin-list", 0x2},
    Package(2) {"mipi-sdca-control-list", 0x0110}, // Bitmap: 0x4 - Usage, 0x8 - Latency
    Package(2) {"mipi-sdca-terminal-transducer-count", 2},  // 2 speakers
  },

  ToUUID("dbb8e3e6-5886-4ba6-8795-1319f52a966b"),
  Package()
  {
    Package(2) {"mipi-sdca-input-pin-1", "E051"},
    //Package(2) {"mipi-sdca-powerdomain-entity", "E041"},
    Package(2) {"mipi-sdca-control-0x4-subproperties", "C424"},
    Package(2) {"mipi-sdca-control-0x8-subproperties", "LC00"},
  }
}) //End E042

Name(C424, Package()
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package()
  {   // Usage, Class, DC
    Package(2) {"mipi-sdca-control-access-layer", 0x4},
    Package(2) {"mipi-sdca-control-access-mode", 5},
    Package(2) {"mipi-sdca-control-dc-value", 1},
  },
  ToUUID("edb12dd0-363d-4085-a3d2-49522ca160c4"),
  Package()
  {
    Package(2) {"mipi-sdca-control-range", "USG2"},
  }
}) //End C424

Name(USG2, Buffer()
{
  0x07, 0x00,  // Range type 0x0007 (Septuples)
  0x01, 0x00,  // Count of ranges = 0x1
  0x01, 0x00, 0x00, 0x00, // Usage Number
  0xae, 0x01, 0x00, 0x00, // Usage CBN, 430 (0x1AE) Full band and Near-ultrasound
  0x00, 0x00, 0x00, 0x00, // Sample Rate, 0 not report
  0x00, 0x00, 0x00, 0x00, // Sample Width, 0 not report
  0x00, 0x80, 0x00, 0x00, // Full Scale, -128.0dB (0x8000) not report actual value
  0x00, 0x80, 0x00, 0x00, // Noise Floor, -128.0dB (0x8000) not report actual value
  0x00, 0x00, 0x00, 0x00, // Usage Tag
}) //End USG2

Name(E04D, Package()
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package()
  {
    Package(2) {"mipi-sdca-entity-type", 0x21},
    Package(2) {"mipi-sdca-entity-label", "UDMPU 25"},
    Package(2) {"mipi-sdca-input-pin-list", 0x2}, // Input Pin 1 connected
    Package(2) {"mipi-sdca-control-list", 0x00010040}, // Bitmap: 0x6 - Latency, 0x10 - ClusterIndex
  },

  ToUUID("dbb8e3e6-5886-4ba6-8795-1319f52a966b"),
  Package()
  {
    Package(2) {"mipi-sdca-input-pin-1", "E051"},
    Package(2) {"mipi-sdca-control-0x6-subproperties", "LC00"},
    Package(2) {"mipi-sdca-control-0x10-subproperties", "C4D1"},
  }
}) //End E04C

Name(C4D1, Package()
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package()
  {  // ClusterIndex, Class, DC
    Package(2) {"mipi-sdca-control-access-layer", 4 },
    Package(2) {"mipi-sdca-control-access-mode", 5},
    Package(2) {"mipi-sdca-control-dc-value", 1},
  },
  ToUUID("edb12dd0-363d-4085-a3d2-49522ca160c4"),
  Package()
  {
    Package(2) {"mipi-sdca-control-range", "CLM4"},
  }
}) //End C4D1

Name(CLM4, Buffer()
{
  0x02, 0x00,  // Range type 2 parameters
  0x01, 0x00,  // Count of ranges = 0x1
  0x01, 0x00, 0x00, 0x00, 0x51, 0x00, 0x00, 0x00, // Cluster Index = 0x1,  Cluster ID = 0x51
}) //End CLM4

Name(E032, Package()
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package()
  {
    Package(2) {"mipi-sdca-entity-type", 0x03},
    Package(2) {"mipi-sdca-entity-label", "OT 25"},
    Package(2) {"mipi-sdca-terminal-type", 0x0188},    // Reference Stream Source Data Port
    Package(2) {"mipi-sdca-input-pin-list", 0x2},
    //Package(2) {"mipi-sdca-control-list", Package() {0x4, 0x8, 0x11} }, // Latency, DataPortIndex
    Package(2) {"mipi-sdca-control-list", 0x00020110}, // Bitmap: 0x4 - Usage, 0x8 - Latency, 0x11 - DataPortIndex
  },

  ToUUID("dbb8e3e6-5886-4ba6-8795-1319f52a966b"),
  Package()
  {
    Package(2) {"mipi-sdca-input-pin-1", "E04D"},
    Package(2) {"mipi-sdca-terminal-clock-connection", "E031"},
    Package(2) {"mipi-sdca-control-0x4-subproperties", "C320"},
    Package(2) {"mipi-sdca-control-0x8-subproperties", "LC00"},
    Package(2) {"mipi-sdca-control-0x11-subproperties", "C321"},
  },

  // ToUUID("edb12dd0-363d-4085-a3d2-49522ca160c4"),
  // Package()
  // {
  //   Package(2) {"mipi-sdca-terminal-dp-numbers","BUF4"},
  // }
}) //End E032

// Name(BUF4, Buffer()
// {
//   0x0, 0xA, // DP_Index: 0x0 DP_Num: 0xA
// }) //End BUF4

Name(C320, Package()
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package()
  { // Usage, Class, DC
    Package(2) { "mipi-sdca-control-access-layer", 4 },
    Package(2) {"mipi-sdca-control-access-mode", 5},
    Package(2) {"mipi-sdca-control-dc-value", 1},
  },
  ToUUID("edb12dd0-363d-4085-a3d2-49522ca160c4"),
  Package()
  {
    Package(2) {"mipi-sdca-control-range", "USG3"},
  }
}) //End C320

Name(USG3, Buffer()
{
  0x07, 0x00,  // Range type 0x0007 (Septuples)
  0x01, 0x00,  // Count of ranges = 0x1
  0x01, 0x00, 0x00, 0x00, // Usage Number
  0xBC, 0x02, 0x00, 0x00, // Usage CBN, 700 (0x2BC) Reference Stream
  0x80, 0xbb, 0x00, 0x00, // Sample Rate, 48000Hz (0xBB80)
  0x18, 0x00, 0x00, 0x00, // Sample Width, 24-bits (0x18)
  0x00, 0x00, 0x00, 0x00, // Full Scale, 0 not report
  0x00, 0x00, 0x00, 0x00, // Noise Floor, 0 not report
  0x00, 0x00, 0x00, 0x00, // Usage Tag
}) //End USG3

Name(C321, Package()
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package()
  {
    // DataPortIndex, Class, DC
    Package(2) {"mipi-sdca-control-access-layer", 0x4},
    Package(2) {"mipi-sdca-control-access-mode", 5},
    Package(2) {"mipi-sdca-control-dc-value", 0x0001}, // DP_Index_D=0x0(0xFF, no DP), DP_Index_C=0x0(0xFF,no DP), DP_Index_B=0x0(0xFF, no DP), DP_Index_A=0x01(0x0A, DP10)
  },
  ToUUID("edb12dd0-363d-4085-a3d2-49522ca160c4"),
  Package()
  {
    Package(2) {"mipi-sdca-control-range", "DPS2"},
  }
}) //End C251

Name(DPS2, Buffer()
{
  0x10, 0x00,  // Range type 0x0010
  0x04, 0x00,  // Count of ranges = 0x4
  0xFF, 0x00, 0x00, 0x00, 0x0A, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, // DP_Index_A
  0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, // DP_Index_B
  0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, // DP_Index_C
  0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, // DP_Index_D
}) //End DPS2

Name(E021, Package()
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package()
  {
    Package(2) {"mipi-sdca-entity-type", 0x30},
    Package(2) {"mipi-sdca-entity-label", "TG 23"},
    //Package(2) {"mipi-sdca-control-list", Package() {0x10}  }, // Tone_Divider
    Package(2) {"mipi-sdca-control-list", 0x00010000}, // Bitmap: 0x10 - Tone_Divider
  },

  ToUUID("dbb8e3e6-5886-4ba6-8795-1319f52a966b"),
  Package()
  {
    Package(2) {"mipi-sdca-control-0x10-subproperties", "C211"},
    //Package(2) {"mipi-sdca-tone-nominal-frequency", 12000000},
  }
}) //End E021

Name(C211, Package()
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package()
  {
    // Tone_Divider, Platform, RW
    Package(2) {"mipi-sdca-control-access-layer", 0x8},
    Package(2) {"mipi-sdca-control-access-mode", 0},
  }
}) //End C211

// =================================== Below part is SPE function.
/*
Name(E043, Package()
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package ()
  {
    Package (2) {"mipi-sdca-entity-type", 0x2},
    Package (2) {"mipi-sdca-entity-label", "IT 199"}, // = IT 43 in ALC722 internal spec
    Package (2) {"mipi-sdca-terminal-type", 0x0741},
    //Package (2) {"mipi-sdca-control-list", Package() {0x8, 0x10, 0x11} }, // Latency, ClusterIndex, DataPortIndex
    Package (2) {"mipi-sdca-control-list", 0x00030100}, // Bitmap: 0x8 - Latency, 0x10 - ClusterIndex, 0x11 - DataPortIndex
  },

  ToUUID("dbb8e3e6-5886-4ba6-8795-1319f52a966b"),
  Package ()
  {
    // Latency, ClusterIndex, DataPort Selector
    Package (2) {"mipi-sdca-control-0x8-subproperties", "LC00"},
    Package (2) {"mipi-sdca-control-0x10-subproperties", "C430"},
    Package (2) {"mipi-sdca-control-0x11-subproperties", "C431"},
  },

}) //End AF01.E002

Name(C430, Package()
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package ()
  {
    // ClusterIndex, Class, DC
    Package (2) {"mipi-sdca-control-access-layer", 0x4},
    Package (2) {"mipi-sdca-control-access-mode", 5},
    Package (2) {"mipi-sdca-control-dc-value", 0},
  }
})

Name(C431, Package()
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package ()
  {
    // DataPort Selector, Class, DC
    Package (2) {"mipi-sdca-control-access-layer", 0x4},
    Package (2) {"mipi-sdca-control-access-mode", 5},
    Package (2) {"mipi-sdca-control-dc-value", 0},
  }
})

//=============================================================================

Name(E044, Package()
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package ()
  {
    Package (2) {"mipi-sdca-entity-type", 0x13},
    Package (2) {"mipi-sdca-entity-label", "PE 199"}, // = PE 44 in ALC722 internal spec
    Package (2) {"mipi-sdca-input-pin-list", 0x2}, // Input Pin 1
    //Package (2) {"mipi-sdca-control-list",  Package() {0x10} },  // Sample_Freq_Index
    Package (2) {"mipi-sdca-control-list",  0x001E}, // Bitmap: 0x01 - Private, 0x02 - Privacy_policy, 0x03 - Privacy_lockstate, 0x04 - Privacy_owner
  },

  ToUUID("dbb8e3e6-5886-4ba6-8795-1319f52a966b"),
  Package ()
  {
    Package (2) {"mipi-sdca-input-pin-1", "E043"}, // Input Pin 1 connected to IT 23
    Package (2) {"mipi-sdca-control-0x01-subproperties", "C441"},
    Package (2) {"mipi-sdca-control-0x02-subproperties", "C442"},
    Package (2) {"mipi-sdca-control-0x03-subproperties", "C443"},
    Package (2) {"mipi-sdca-control-0x04-subproperties", "C444"},
  }
}) //End AF01.E001

Name(C441, Package()
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package ()
  {
    // Private,
    Package (2) {"mipi-sdca-control-access-layer", 0x4},
    Package (2) {"mipi-sdca-control-interrupt-position", 29}, // SDCA Interrupt Bit 29
    Package (2) {"mipi-sdca-control-access-mode", 0},
  }
})

Name(C442, Package()
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package ()
  {
    // Privacy_Policy,
    Package (2) {"mipi-sdca-control-access-layer", 0x4},
    Package (2) {"mipi-sdca-control-access-mode", 0},
  }
})

Name(C443, Package()
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package ()
  {
    // Privacy_Lockstate,
    Package (2) {"mipi-sdca-control-access-layer", 0x4},
    Package (2) {"mipi-sdca-control-access-mode", 0},
  }
})

Name(C444, Package()
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

Name(E045, Package()
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package ()
  {
    Package (2) {"mipi-sdca-entity-type", 0x11},
    Package (2) {"mipi-sdca-entity-label", "PDE 199"}, // = PDE 45
    //Package (2) {"mipi-sdca-powerdomain-state-list", 0xF},
    //Package (2) {"mipi-sdca-powerdomain-managed-list", Package() {0x8, 0x9, 0x43}},
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
    Package (2) {"mipi-sdca-control-0x1-subproperties", "RP45"},
    Package (2) {"mipi-sdca-control-0x10-subproperties", "AP45"},
  }
})

Name(RP45, Package()
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
})

Name(AP45, Package()
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package ()
  {
    // Actual_PS, Class, RO
    Package (2) {"mipi-sdca-control-access-layer", 0x4},
    Package (2) {"mipi-sdca-control-access-mode", 3},
  }
})

//=============================================================================================================

Name(E046, Package()
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package ()
  {
    Package (2) {"mipi-sdca-entity-type", 0x03},
    Package (2) {"mipi-sdca-entity-label", "OT 199"}, // = OT 46 in ALC722 internal spec
    Package (2) {"mipi-sdca-terminal-type", 0x0747},
    Package (2) {"mipi-sdca-terminal-reference-number", 0x1},
    Package (2) {"mipi-sdca-input-pin-list", 0x2},
    //Package (2) {"mipi-sdca-control-list", Package() {0x4, 0x8} }, // Usage, Latency
    Package (2) {"mipi-sdca-control-list", 0x0110}, // 0x4 - Usage, 0x8 - Latency
  },

  ToUUID("dbb8e3e6-5886-4ba6-8795-1319f52a966b"),
  Package ()
  {
    Package (2) {"mipi-sdca-input-pin-1", "E044"},
    Package (2) {"mipi-sdca-powerdomain-entity", "E045"},
    Package (2) {"mipi-sdca-control-0x4-subproperties", "C464"},
    Package (2) {"mipi-sdca-control-0x8-subproperties", "LC00"},
  }
})

Name(C464, Package()
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package ()
  {
    // Usage, Class, DC
    Package (2) {"mipi-sdca-control-access-layer", 0x4},
    Package (2) {"mipi-sdca-control-access-mode", 5},
    Package (2) {"mipi-sdca-control-dc-value", 0x60}, // Full band
  }
})

*/

Name(EXT0, Package()
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package()
  {
    Package(2) {"mipi-sdca-function-expansion-subsystem-id", 0x1234},
    Package(2) {"mipi-sdca-function-expansion-subsystem-revision-id", 0x1234},
  }
}) //End EXT0

//==============================================================================================

// Cluster Library
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

Name(CL21, Package()  // Posture UDMPU 21 ClusterIndex 1, OT23 ch1=Left, OT23 ch2=right
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package ()
  {
    Package (2) {"mipi-sdca-channel-count", 2},          // 2 channels
  },
  ToUUID("dbb8e3e6-5886-4ba6-8795-1319f52a966b"),
  Package ()
  {
    Package (2) {"mipi-sdca-channel-1-subproperties", "CH21"},  // channel 1 User-Left, Generic Audio, Equipment-Left
    Package (2) {"mipi-sdca-channel-2-subproperties", "CH22"},  // channel 2 User-Right, Generic Audio, Equipment-Right
  }
}) //End CL21

Name(CL22, Package()  // Posture UDMPU 21 ClusterIndex 2, OT23 ch1=(left+right)/2, OT23 ch2=(left+right)/2
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package ()
  {
    Package (2) {"mipi-sdca-channel-count", 2},          // 2 channels
  },
  ToUUID("dbb8e3e6-5886-4ba6-8795-1319f52a966b"),
  Package ()
  {
    Package (2) {"mipi-sdca-channel-1-subproperties", "CH23"},  // channel 255 User-Single, Generic Audio, Equipment-Combined
    Package (2) {"mipi-sdca-channel-2-subproperties", "CH23"},  // channel 255 User-Single, Generic Audio, Equipment-Combined
  }
}) //End CL29

Name(CL23, Package()  // Posture UDMPU 21 ClusterIndex 3, OT23 ch1=right, OT23 ch2=left
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package ()
  {
    Package (2) {"mipi-sdca-channel-count", 2},          // 2 channels
  },
  ToUUID("dbb8e3e6-5886-4ba6-8795-1319f52a966b"),
  Package ()
  {
    Package (2) {"mipi-sdca-channel-1-subproperties", "CH22"},  // channel 2 User-Right, Generic Audio, Equipment-Right
    Package (2) {"mipi-sdca-channel-2-subproperties", "CH21"},  // channel 1 User-Left, Generic Audio, Equipment-Left
  }
}) //End CL23

Name(CL24, Package()  // Posture UDMPU 21 ClusterIndex 4, OT23 ch1=(left+right)/2, OT23 ch2=(left+right)/2
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package ()
  {
    Package (2) {"mipi-sdca-channel-count", 2},          // 2 channels
  },
  ToUUID("dbb8e3e6-5886-4ba6-8795-1319f52a966b"),
  Package ()
  {
    Package (2) {"mipi-sdca-channel-1-subproperties", "CH23"},  // channel 255 User-Single, Generic Audio, Equipment-Combined
    Package (2) {"mipi-sdca-channel-2-subproperties", "CH23"},  // channel 255 User-Single, Generic Audio, Equipment-Combined
  }
}) //End CL24

Name(CH21, Package()
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package ()
  {
    Package (2) {"mipi-sdca-cluster-channel-id", 1},        // channel 1 (User Left)
    Package (2) {"mipi-sdca-cluster-channel-purpose", 0x1},      // 0x01: Generic Audio
    Package (2) {"mipi-sdca-cluster-channel-relationship", 0x2},  // 0x03: Equipment-Left
  }
}) //End CH21

Name(CH22, Package()
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package ()
  {
    Package (2) {"mipi-sdca-cluster-channel-id", 2},        // channel 2 (User Right)
    Package (2) {"mipi-sdca-cluster-channel-purpose", 0x1},      // 0x01: Generic Audio
    Package (2) {"mipi-sdca-cluster-channel-relationship", 0x3},  // 0x03: Equipment-Right
  }
}) //End CH22

Name(CH23, Package()
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package ()
  {
    Package (2) {"mipi-sdca-cluster-channel-id", 255},        // channel 255 (User Single)
    Package (2) {"mipi-sdca-cluster-channel-purpose", 0x1},      // 0x01: Generic Audio
    Package (2) {"mipi-sdca-cluster-channel-relationship", 0x47},  // 0x03: Equipment-Combined
  }
}) //End CH23

Name(CL31, Package()  // Transducer UDMPU 23 ClusterIndex 0x1, Cluster ID 0x31
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package ()
  {
    Package (2) {"mipi-sdca-channel-count", 2},          // 2 channels
  },
  ToUUID("dbb8e3e6-5886-4ba6-8795-1319f52a966b"),
  Package ()
  {
    Package (2) {"mipi-sdca-channel-1-subproperties", "CH31"},  // channel 1 (Imp-Def), Generic Audio, Primary Transducer
    Package (2) {"mipi-sdca-channel-2-subproperties", "CH32"},  // channel 2 (Imp-Def), Generic Audio, Primary Transducer
  }
}) //End CL31

Name(CH31, Package()
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package ()
  {
    Package (2) {"mipi-sdca-cluster-channel-id", 1},        // channel 1 (Imp-Def)
    Package (2) {"mipi-sdca-cluster-channel-purpose", 0x1},      // 0x01: Generic Audio
    Package (2) {"mipi-sdca-cluster-channel-relationship", 0x50},  // 0x50: Primary Transducer
  }
}) //End CH31

Name(CH32, Package()
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package ()
  {
    Package (2) {"mipi-sdca-cluster-channel-id", 2},        // channel 2 (Imp-Def)
    Package (2) {"mipi-sdca-cluster-channel-purpose", 0x1},      // 0x01: Generic Audio
    Package (2) {"mipi-sdca-cluster-channel-relationship", 0x50},  // 0x50: Primary Transducer
  }
}) //End CH32

Name(CL41, Package()  // 2T, 2ch, Transducer Priority, LI/LV/RI/RV
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package ()
  {
    Package (2) {"mipi-sdca-channel-count", 2},          // 2 channels
  },
  ToUUID("dbb8e3e6-5886-4ba6-8795-1319f52a966b"),
  Package ()
  {
    Package (2) {"mipi-sdca-channel-1-subproperties", "CH41"},  // channel 1, SV1, T12i
    Package (2) {"mipi-sdca-channel-2-subproperties", "CH42"},  // channel 2, SV2, T12i
  }
}) //End CL41

Name(CL42, Package()  // 2T, 2ch, Variable Priority, LI/RI/LV/RV
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package ()
  {
    Package (2) {"mipi-sdca-channel-count", 2},          // 2 channels
  },
  ToUUID("dbb8e3e6-5886-4ba6-8795-1319f52a966b"),
  Package ()
  {
    Package (2) {"mipi-sdca-channel-1-subproperties", "CH43"},  // channel 1, SV12i, T1
    Package (2) {"mipi-sdca-channel-2-subproperties", "CH44"},  // channel 2, SV12i, T2
  }
}) //End CL42

Name(CH41, Package()
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package ()
  {
    Package (2) {"mipi-sdca-cluster-channel-id", 1},        // channel 1
    Package (2) {"mipi-sdca-cluster-channel-purpose", 0x09},    // 0x09: SV1
    Package (2) {"mipi-sdca-cluster-channel-relationship", 0x5A},  // 0x5A: T12i
  }
}) //End CH41

Name(CH42, Package()
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package ()
  {
    Package (2) {"mipi-sdca-cluster-channel-id", 2},        // channel 2
    Package (2) {"mipi-sdca-cluster-channel-purpose", 0x0A},    // 0x0A: SV2
    Package (2) {"mipi-sdca-cluster-channel-relationship", 0x5A},  // 0x5A: T12i
  }
}) //End CH42

Name(CH43, Package()
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package ()
  {
    Package (2) {"mipi-sdca-cluster-channel-id", 1},        // channel 1
    Package (2) {"mipi-sdca-cluster-channel-purpose", 0x10},    // 0x09: SV12i
    Package (2) {"mipi-sdca-cluster-channel-relationship", 0x58},  // 0x58: T1
  }
}) //End CH43

Name(CH44, Package()
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package ()
  {
    Package (2) {"mipi-sdca-cluster-channel-id", 2},        // channel 2
    Package (2) {"mipi-sdca-cluster-channel-purpose", 0x10},    // 0x0A: SV12i
    Package (2) {"mipi-sdca-cluster-channel-relationship", 0x59},  // 0x5A: T2
  }
}) //End CH44

Name(CL51, Package()  // 2-ch-ref-2-speaker-1/2
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package ()
  {
    Package (2) {"mipi-sdca-channel-count", 2},          // 2 channels
  },
  ToUUID("dbb8e3e6-5886-4ba6-8795-1319f52a966b"),
  Package ()
  {
    Package (2) {"mipi-sdca-channel-1-subproperties", "CH51"},  // channel 101, EchoRef-1, Reference
    Package (2) {"mipi-sdca-channel-2-subproperties", "CH52"},  // channel 102, EchoRef-2, Reference
  }
}) //End CL51

Name(CH51, Package()
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package ()
  {
    Package (2) {"mipi-sdca-cluster-channel-id", 101},        // channel 101
    Package (2) {"mipi-sdca-cluster-channel-purpose", 0x05},    // 0x05: Reference
    Package (2) {"mipi-sdca-cluster-channel-relationship", 0x71},  // 0x71: EchoRef-1
  }
}) //End CH51

Name(CH52, Package()
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package ()
  {
    Package (2) {"mipi-sdca-cluster-channel-id", 102},        // channel 102
    Package (2) {"mipi-sdca-cluster-channel-purpose", 0x05},    // 0x05: Reference
    Package (2) {"mipi-sdca-cluster-channel-relationship", 0x72},  // 0x72: EchoRef-2
  }
}) //End CH52
