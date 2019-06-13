/**@file
  SoundWire Controllers ACPI

  Copyright (c) 2018 - 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

// Values for XTAL 24MHz
#define V_HDA_SNDW_IP_CLOCK               24000000 // 24 MHz
#define V_HDA_SNDW_CLOCK_FREQ_SUPPORTED   6000000  // 6 MHz
#define V_HDA_SNDW_DEFAULT_FRAME_ROW_SIZE 125
#define V_HDA_SNDW_DEFAULT_FRAME_COL_SIZE 2

#define V_HDA_SNDW_SW_INTERFACE_REVISION 0x00010000

//
// SoundWire Link Controllers definition (up to 4 controllers can be defined)
//
Scope(HDAS)
{
  //
  // Address (_ADR) encoding:
  // Bits 28-31 - Link Type
  // 0 = HD-Audio, 1 = DSP, 2 = PDM, 3 = SSP, 4 = SoundWire
  // Bits 0-3 - Device Instance ID (unique to virtual bus).
  // SoundWire:
  // 0-3 = Bus Instance 0 - 3
  // For example:
  // 1)  SWC0: _ADR(0x10000000) - LinkType[31:28] = 1 (DSP), Instance[3:0] = 0
  // 2)  SWC1: _ADR(0x40000001) - LinkType[31:28] = 4 (SW),  Instance[3:0] = 1
  // 3)  SWC2: _ADR(0x40000002) - LinkType[31:28] = 4 (SW),  Instance[3:0] = 2
  //

  //
  // SoundWire Link Controller (Aggregated defintion for all 4 contoller instances - LinkType = SoundWire)
  //
  Device(SNDW) {
    Name(_ADR, 0x40000000)

    Name(_CID, Package() {
      // Precedence order does matter for evaluation of list
      "PRP00001",  // to indicate that we want to use DeviceTree-like "compatible" matching, Linux only
      "PNP0A05"    // generic container device always placed last, makes sure entry is ignored by Windows with no yellow bangs if there is no matching driver
    })

    Method(_STA, 0, NotSerialized) { // _STA: Status
      Return (0x0B)
    }

    //
    // Update Quirk mask based on enabled SoundWire interfaces
    // Arg0 - Link instance (LNK[N])
    // Arg1 - SoundWire Quirk Mask
    //
    Method(QCFG, 2,  NotSerialized) {
      // Update LNK[N] object, "intel-quirk-mask" property value
      // in accordance with SoundWire Link [N] enable/disable state in PchPolicy
      Store (Arg1, Index (DeRefOf(Index(DeRefOf(Index (Arg0, 1)), 0)), 1))
    }

    Method (_INI) {
      ADBG("SNDW _INI")

      // Update Link Enable property
      // SWQx - PCH NVS variables set in accordance with PchPolicy (AudioLinkSndwX)
      QCFG(LNK0, \SWQ0)
      QCFG(LNK1, \SWQ1)
      QCFG(LNK2, \SWQ2)
      QCFG(LNK3, \SWQ3)
    }

    Name(_DSD, Package () {
      ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
      Package () {
        Package (2) {"mipi-sdw-sw-interface-revision", V_HDA_SNDW_SW_INTERFACE_REVISION},
        Package (2) {"mipi-sdw-master-count", 4}
        // Vendor specific parameters (optional)
      },
      // Properties for the SoundWire bus instances
      ToUUID("dbb8e3e6-5886-4ba6-8795-1319f52a966b"),
      Package () {
        Package (2) {"mipi-sdw-link-0-subproperties", "LNK0"}, // Link Descriptor 0
        Package (2) {"mipi-sdw-link-1-subproperties", "LNK1"}, // Link Descriptor 1
        Package (2) {"mipi-sdw-link-2-subproperties", "LNK2"}, // Link Descriptor 2
        Package (2) {"mipi-sdw-link-3-subproperties", "LNK3"}  // Link Descriptor 3
      }
     }) // END Name(_DSD)

    Name(LNK0, Package() {
      ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
      Package () {
        //
        // Intel specific properties
        //
        Package (2) {"intel-quirk-mask", 0}, // Quirks: [BIT0] - static clock, [BIT1] - bus instance disable (0-No, 1-Yes)
        Package (2) {"intel-sdw-ip-clock", V_HDA_SNDW_IP_CLOCK}, //SoundWire clock value delivered to IP
        //
        // Properties defined as per the MIPI software spec for Link controllers
        //
        Package (2) {"mipi-sdw-clock-stop-mode0-supported", 1}, // Integer/Boolean
        Package (2) {"mipi-sdw-clock-stop-mode1-supported", 1}, // Integer/Boolean
        Package (2) {"mipi-sdw-clock-frequencies-supported", Package(){V_HDA_SNDW_CLOCK_FREQ_SUPPORTED}}, // Package
        Package (2) {"mipi-sdw-default-frame-rate", 48000}, // Integer
        Package (2) {"mipi-sdw-default-frame-row-size", V_HDA_SNDW_DEFAULT_FRAME_ROW_SIZE}, // Integer
        Package (2) {"mipi-sdw-default-frame-col-size", V_HDA_SNDW_DEFAULT_FRAME_COL_SIZE}, // Integer
        Package (2) {"mipi-sdw-dynamic-frame-shape", 1}, // Integer/Boolean
        Package (2) {"mipi-sdw-command-error-threshold", 16}, // Integer
      }
    }) // END Name(LNK0)

    Name(LNK1, Package() {
      ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
      Package () {
        //
        // Intel specific properties
        //
        Package (2) {"intel-quirk-mask", 0}, // Quirks: [BIT0] - static clock, [BIT1] - bus instance disable (0-No, 1-Yes)
        Package (2) {"intel-sdw-ip-clock", V_HDA_SNDW_IP_CLOCK}, //SoundWire clock value delivered to IP
        //
        // Properties defined as per the MIPI software spec for Link controllers
        //
        Package (2) {"mipi-sdw-clock-stop-mode0-supported", 1}, // Integer/Boolean
        Package (2) {"mipi-sdw-clock-stop-mode1-supported", 1}, // Integer/Boolean
        Package (2) {"mipi-sdw-clock-frequencies-supported", Package(){V_HDA_SNDW_CLOCK_FREQ_SUPPORTED}}, // Package
        Package (2) {"mipi-sdw-default-frame-rate", 48000}, // Integer
        Package (2) {"mipi-sdw-default-frame-row-size", V_HDA_SNDW_DEFAULT_FRAME_ROW_SIZE}, // Integer
        Package (2) {"mipi-sdw-default-frame-col-size", V_HDA_SNDW_DEFAULT_FRAME_COL_SIZE}, // Integer
        Package (2) {"mipi-sdw-dynamic-frame-shape", 1}, // Integer/Boolean
        Package (2) {"mipi-sdw-command-error-threshold", 16}, // Integer
      }
    }) // END Name(LNK1)

    Name(LNK2, Package() {
      ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
      Package () {
        //
        // Intel specific properties
        //
        Package (2) {"intel-quirk-mask", 0}, // Quirks: [BIT0] - static clock, [BIT1] - bus instance disable (0-No, 1-Yes)
        Package (2) {"intel-sdw-ip-clock", V_HDA_SNDW_IP_CLOCK}, //SoundWire clock value delivered to IP
        //
        // Properties defined as per the MIPI software spec for Link controllers
        //
        Package (2) {"mipi-sdw-clock-stop-mode0-supported", 1}, // Integer/Boolean
        Package (2) {"mipi-sdw-clock-stop-mode1-supported", 1}, // Integer/Boolean
        Package (2) {"mipi-sdw-clock-frequencies-supported", Package(){V_HDA_SNDW_CLOCK_FREQ_SUPPORTED}}, // Package
        Package (2) {"mipi-sdw-default-frame-rate", 48000}, // Integer
        Package (2) {"mipi-sdw-default-frame-row-size", V_HDA_SNDW_DEFAULT_FRAME_ROW_SIZE}, // Integer
        Package (2) {"mipi-sdw-default-frame-col-size", V_HDA_SNDW_DEFAULT_FRAME_COL_SIZE}, // Integer
        Package (2) {"mipi-sdw-dynamic-frame-shape", 1}, // Integer/Boolean
        Package (2) {"mipi-sdw-command-error-threshold", 16}, // Integer
      }
    }) // END Name(LNK2)

    Name(LNK3, Package() {
      ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
      Package () {
        //
        // Intel specific properties
        //
        Package (2) {"intel-quirk-mask", 0}, // Quirks: [BIT0] - static clock, [BIT1] - bus instance disable (0-No, 1-Yes)
        Package (2) {"intel-sdw-ip-clock", V_HDA_SNDW_IP_CLOCK}, //SoundWire clock value delivered to IP
        //
        // Properties defined as per the MIPI software spec for Link controllers
        //
        Package (2) {"mipi-sdw-clock-stop-mode0-supported", 1}, // Integer/Boolean
        Package (2) {"mipi-sdw-clock-stop-mode1-supported", 1}, // Integer/Boolean
        Package (2) {"mipi-sdw-clock-frequencies-supported", Package(){V_HDA_SNDW_CLOCK_FREQ_SUPPORTED}}, // Package
        Package (2) {"mipi-sdw-default-frame-rate", 48000}, // Integer
        Package (2) {"mipi-sdw-default-frame-row-size", V_HDA_SNDW_DEFAULT_FRAME_ROW_SIZE}, // Integer
        Package (2) {"mipi-sdw-default-frame-col-size", V_HDA_SNDW_DEFAULT_FRAME_COL_SIZE}, // Integer
        Package (2) {"mipi-sdw-dynamic-frame-shape", 1}, // Integer/Boolean
        Package (2) {"mipi-sdw-command-error-threshold", 16}, // Integer
      }
    }) // END Name(LNK3)

  } // END Device(SNDW)
} // END Scope(HDAS)
