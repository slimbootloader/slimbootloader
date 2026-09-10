/** @file
  SoundWire topology definitions

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

Name(_DSD, Package()
{ // Peripheral Descriptor
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package ()
  {
    Package (2) {"mipi-sdw-sw-interface-revision", 0x00020001}, // Integer: v 2.1

        // Peripheral Device configuration
    Package (2) {"mipi-sdw-sdca-interface-revision", 0x1000}, // Integer: SDCA 1.0r00

        Package(2) {"mipi-sdw-sdca-interrupt-register-list", 0xF},            // Bitmap
        Package(2) {"mipi-sdw-commit-register-supported", 0},                 // Boolean

    Package (2) {"mipi-sdw-wake-up-unavailable", 0}, // support wakeup
    Package (2) {"mipi-sdw-test-mode-supported", 1}, // Integer/Boolean
    Package (2) {"mipi-sdw-clock-stop-mode1-supported", 1}, // Integer/Boolean
    Package (2) {"mipi-sdw-simplified-clockstopprepare-sm-supported", 0}, //Integer/Boolean
    Package (2) {"mipi-sdw-clockstopprepare-timeout", 1500}, // Integer
    Package (2) {"mipi-sdw-peripheral-channelprepare-timeout", 150}, // Integer
    Package (2) {"mipi-sdw-clockstopprepare-hard-reset-behavior", 0}, // Integer
    Package (2) {"mipi-sdw-highPHY-capable", 0}, // Integer/Boolean
    Package (2) {"mipi-sdw-paging-supported", 1}, // Integer/Boolean
    Package (2) {"mipi-sdw-bank-delay-supported", 0}, // Integer/Boolean
    Package (2) {"mipi-sdw-port15-read-behavior", 1}, // Integer
    Package (2) {"mipi-sdw-manager-list", 0x0}, // Integer

    // Lane mapping
    Package (2) {"mipi-sdw-lane-1-mapping", "mipi-sdw-manager-lane-1"},
    Package (2) {"mipi-sdw-lane-2-mapping", "mipi-sdw-manager-lane-2"},

        // Data Port configuration
    Package (2) {"mipi-sdw-source-port-list", 0x454}, // Bitmap, DP 2,4,6,10
    Package (2) {"mipi-sdw-sink-port-list", 0x2A}, // Bitmap, DP 1,3,5
    Package (2) {"mipi-sdw-dp-0-supported", 0}, // Boolean
    },
    ToUUID("dbb8e3e6-5886-4ba6-8795-1319f52a966b"),
    Package()
    {
        //Package (2) {"mipi-sdw-dp-0-subproperties", "DP0S"},
        Package (2) {"mipi-sdw-dp-1-sink-subproperties", "DP1S"},
    Package (2) {"mipi-sdw-dp-2-source-subproperties", "DP2S"},
    Package (2) {"mipi-sdw-dp-3-sink-subproperties", "DP3S"},
    Package (2) {"mipi-sdw-dp-4-source-subproperties", "DP4S"},
    Package (2) {"mipi-sdw-dp-5-sink-subproperties", "DP5S"},
    Package (2) {"mipi-sdw-dp-6-source-subproperties", "DP6S"},
    Package (2) {"mipi-sdw-dp-10-source-subproperties", "DP10"},

  },
})  // End _DSD

Name(DP1S, Package()
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package ()
  {
    //Package (2) {"mipi-sdw-port-max-wordlength", 24},
    //Package (2) {"mipi-sdw-port-min-wordlength", 16},
    Package (2) {"mipi-sdw-port-wordlength-configs", Package() {16, 20, 24} },
    Package (2) {"mipi-sdw-data-port-type", 0},
    Package (2) {"mipi-sdw-max-grouping-supported", 0},
    Package (2) {"mipi-sdw-simplified-channelprepare-sm", 1},
    Package (2) {"mipi-sdw-min-channel-number", 0x0},
    Package (2) {"mipi-sdw-max-channel-number", 0x1},
    Package (2) {"mipi-sdw-channel-combination-list", Package() {0x3}},
    Package (2) {"mipi-sdw-modes-supported", 0x0F},
    Package (2) {"mipi-sdw-max-async-buffer", 0x08},
    Package (2) {"mipi-sdw-port-encoding-type", 0x01},
    Package (2) {"mipi-sdw-lane-list", Package() {0, 1, 2}},
  }
}) // End DP1S

Name(DP2S, Package()
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package ()
  {
    //Package (2) {"mipi-sdw-port-max-wordlength", 24},
    //Package (2) {"mipi-sdw-port-min-wordlength", 8},
    Package (2) {"mipi-sdw-port-wordlength-configs", Package() {16, 20, 24} },
    Package (2) {"mipi-sdw-data-port-type", 0},
    Package (2) {"mipi-sdw-max-grouping-supported", 0},
    Package (2) {"mipi-sdw-simplified-channelprepare-sm", 1},
    Package (2) {"mipi-sdw-min-channel-number", 0x0},
    Package (2) {"mipi-sdw-max-channel-number", 0x1},
    Package (2) {"mipi-sdw-channel-combination-list", Package() {0x3}},
    Package (2) {"mipi-sdw-modes-supported", 0x0F},
    Package (2) {"mipi-sdw-max-async-buffer", 0x08},
    Package (2) {"mipi-sdw-port-encoding-type", 0x01},
    Package (2) {"mipi-sdw-lane-list", Package() {0, 1, 2}},
  }
}) // End DP2S

Name(DP3S, Package()
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package ()
  {
    //Package (2) {"mipi-sdw-port-max-wordlength", 24},
    //Package (2) {"mipi-sdw-port-min-wordlength", 8},
    Package (2) {"mipi-sdw-port-wordlength-configs", Package() {16, 20, 24} },
    Package (2) {"mipi-sdw-data-port-type", 0},
    Package (2) {"mipi-sdw-max-grouping-supported", 0},
    Package (2) {"mipi-sdw-simplified-channelprepare-sm", 1},
    Package (2) {"mipi-sdw-min-channel-number", 0x0},
    Package (2) {"mipi-sdw-max-channel-number", 0x1},
    Package (2) {"mipi-sdw-channel-combination-list", Package() {0x3}},
    Package (2) {"mipi-sdw-modes-supported", 0x0F},
    Package (2) {"mipi-sdw-max-async-buffer", 0x08},
    Package (2) {"mipi-sdw-port-encoding-type", 0x01},
    Package (2) {"mipi-sdw-lane-list", Package() {1, 2, 0}},
  }
}) // End DP3S

Name(DP4S, Package()
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package ()
  {
    //Package (2) {"mipi-sdw-port-max-wordlength", 24},
    //Package (2) {"mipi-sdw-port-min-wordlength", 8},
    Package (2) {"mipi-sdw-port-wordlength-configs", Package() {16, 20, 24} },
    Package (2) {"mipi-sdw-data-port-type", 0},
    Package (2) {"mipi-sdw-max-grouping-supported", 0},
    Package (2) {"mipi-sdw-simplified-channelprepare-sm", 1},
    Package (2) {"mipi-sdw-min-channel-number", 0x0},
    Package (2) {"mipi-sdw-max-channel-number", 0x1},
    Package (2) {"mipi-sdw-channel-combination-list", Package() {0x3}},
    Package (2) {"mipi-sdw-modes-supported", 0x0F},
    Package (2) {"mipi-sdw-max-async-buffer", 0x08},
    Package (2) {"mipi-sdw-port-encoding-type", 0x01},
    Package (2) {"mipi-sdw-lane-list", Package() {2, 0, 1}},
  }
}) // End DP4S

Name(DP5S, Package()
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package ()
  {
    //Package (2) {"mipi-sdw-port-max-wordlength", 24},
    //Package (2) {"mipi-sdw-port-min-wordlength", 8},
    Package (2) {"mipi-sdw-port-wordlength-configs", Package() {16, 20, 24} },
    Package (2) {"mipi-sdw-data-port-type", 0},
    Package (2) {"mipi-sdw-max-grouping-supported", 0},
    Package (2) {"mipi-sdw-simplified-channelprepare-sm", 1},
    Package (2) {"mipi-sdw-min-channel-number", 0x0},
    Package (2) {"mipi-sdw-max-channel-number", 0x1},
    Package (2) {"mipi-sdw-channel-combination-list", Package() {0x3}},
    Package (2) {"mipi-sdw-modes-supported", 0x0F},
    Package (2) {"mipi-sdw-max-async-buffer", 0x08},
    Package (2) {"mipi-sdw-port-encoding-type", 0x01},
    Package (2) {"mipi-sdw-lane-list", Package() {2, 0, 1}},
  }
}) // End DP5S

Name(DP6S, Package()
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package ()
  {
    //Package (2) {"mipi-sdw-port-max-wordlength", 24},
    //Package (2) {"mipi-sdw-port-min-wordlength", 8},
    Package (2) {"mipi-sdw-port-wordlength-configs", Package() {16, 20, 24} },
    Package (2) {"mipi-sdw-data-port-type", 0},
    Package (2) {"mipi-sdw-max-grouping-supported", 0},
    Package (2) {"mipi-sdw-simplified-channelprepare-sm", 1},
    Package (2) {"mipi-sdw-min-channel-number", 0x0},
    Package (2) {"mipi-sdw-max-channel-number", 0x3},
    Package (2) {"mipi-sdw-channel-combination-list", Package() {0x3, 0xF}},
    Package (2) {"mipi-sdw-modes-supported", 0x0F},
    Package (2) {"mipi-sdw-max-async-buffer", 0x08},
    Package (2) {"mipi-sdw-port-encoding-type", 0x01},
    Package (2) {"mipi-sdw-lane-list", Package() {2, 0, 1}},
  }
}) // End DP6S

Name(DP10, Package()
{
  ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  Package ()
  {
    //Package (2) {"mipi-sdw-port-max-wordlength", 24},
    //Package (2) {"mipi-sdw-port-min-wordlength", 8},
    Package (2) {"mipi-sdw-port-wordlength-configs", Package() {16, 20, 24} },
    Package (2) {"mipi-sdw-data-port-type", 0},
    Package (2) {"mipi-sdw-max-grouping-supported", 0},
    Package (2) {"mipi-sdw-simplified-channelprepare-sm", 1},
    Package (2) {"mipi-sdw-min-channel-number", 0x0},
    Package (2) {"mipi-sdw-max-channel-number", 0x1},
    Package (2) {"mipi-sdw-channel-combination-list", Package() {0x3}},
    Package (2) {"mipi-sdw-modes-supported", 0x0F},
    Package (2) {"mipi-sdw-max-async-buffer", 0x08},
    Package (2) {"mipi-sdw-port-encoding-type", 0x01},
    Package (2) {"mipi-sdw-lane-list", Package() {1, 2, 0}},
  }
}) // End DP10

Device(AF01)
{
  Name (_ADR, 0x1)
  Include ("AudioFunction_722_UAJ.asl")
}

Device(AF02)
{
  Name (_ADR, 0x2)
  Include ("AudioFunction_722_MIC.asl")
}

Device(AF03)
{
  Name (_ADR, 0x3)
  Include ("AudioFunction_722_HID.asl")
}

Device(AF04)
{
  Name (_ADR, 0x4)
  Include ("AudioFunction_722_AMP.asl")
}

