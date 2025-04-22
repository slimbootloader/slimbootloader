/** @file

  Copyright (c) 2021, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

// Values for XTAL 24MHz
#define V_HDA_SNDW_IP_CLOCK_XTAL24MHZ                 24000000 // 24 MHz
#define V_HDA_SNDW_CLOCK_FREQ_SUPPORTED_XTAL24MHZ     6000000  // 6 MHz
#define V_HDA_SNDW_FRAME_ROW_SIZE_XTAL24MHZ           125
#define V_HDA_SNDW_FRAME_COL_SIZE_XTAL24MHZ           2

// Values for XTAL 38.4MHz
#define V_HDA_SNDW_IP_CLOCK_XTAL38P4MHZ               38400000 // 38.4 MHz
#define V_HDA_SNDW_CLOCK_FREQ_SUPPORTED_XTAL38P4MHZ   4800000  // 4.8 MHz
#define V_HDA_SNDW_FRAME_ROW_SIZE_XTAL38P4MHZ         50
#define V_HDA_SNDW_FRAME_COL_SIZE_XTAL38P4MHZ         4

// Values for XTAL 19.2MHz
#define V_HDA_SNDW_IP_CLOCK_XTAL19P2MHZ               19200000 // 19.2 MHz
#define V_HDA_SNDW_CLOCK_FREQ_SUPPORTED_XTAL19P2MHZ   4800000  // 4.8 MHz
#define V_HDA_SNDW_FRAME_ROW_SIZE_XTAL19P2MHZ         50
#define V_HDA_SNDW_FRAME_COL_SIZE_XTAL19P2MHZ         4

#define V_HDA_SNDW_SW_INTERFACE_REVISION 0x00010000

#define SDW_INTEL_QUIRK_MSK_INDEX                      0
#define SDW_INTEL_IP_CLK_INDEX                         1
#define SDW_INTEL_DATA_ON_ACTIVE_INTERVAL_SELECT_INDEX 2
#define SDW_INTEL_DATA_ON_DELAY_SELECT                 3
#define SDW_INTEL_AUTONOMOUS_CLK_STOP_INDEX            4
#define SDW_MIPI_CLK_STOP_MODE0_SUPPORT_INDEX          5
#define SDW_MIPI_CLK_STOP_MODE1_SUPPORT_INDEX          6
#define SDW_MIPI_CLK_FREQ_SUPPORTED_INDEX              7
#define SDW_MIPI_DEFAULT_FRAME_RATE_INDEX              8
#define SDW_MIPI_DEFAULT_FRAME_ROW_SIZE_INDEX          9
#define SDW_MIPI_DEFAULT_FRAME_COL_SIZE_INDEX         10
#define SDW_MIPI_DYNAMIC_FRAME_SHAPE_INDEX            11
#define SDW_MIPI_CMD_ERROR_THRESHOLD_INDEX            12

#define SDW_LNK_DESC_GUID_INDEX 0
#define SDW_LNK_DESC_DATA_INDEX 1

#define SDW_LNK_DESC_DATA(LinkDesc) DeRefOf(Index(LinkDesc, SDW_LNK_DESC_DATA_INDEX))
#define SDW_LNK_ENTRY(LinkDesc, EntryIndex) DeRefOf(Index(SDW_LNK_DESC_DATA(LinkDesc), EntryIndex))
#define SDW_LNK_ENTRY_DATA(LinkDesc, EntryIndex) Index (SDW_LNK_ENTRY(LinkDesc, EntryIndex), 1)

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
    // Update Quirk mask based on enabled SoundWire interfaces and update autonomous clock stop based on PchPolicy
    // Arg0 - Link instance (LNK[N])
    // Arg1 - SoundWire Quirk Mask
    // Arg2 - Autonomous Clock Stop
    Method(QCFG, 3,  NotSerialized) {
      // Update LNK[N] object, "intel-quirk-mask" property value
      // in accordance with SoundWire Link [N] enable/disable state in PchPolicy
      Store (Arg1, SDW_LNK_ENTRY_DATA(Arg0, SDW_INTEL_QUIRK_MSK_INDEX))
      // Update LNK[N] object, "intel-autonomous-clock-stop" property value
      // in accordance with SoundWire Link [N] Autonomous Clock Stop enable/disable state in PchPolicy
      Store (Arg2, SDW_LNK_ENTRY_DATA(Arg0, SDW_INTEL_AUTONOMOUS_CLK_STOP_INDEX))
    }

    //
    // Update Link properties (Clock, Supported frequency, Frame) based on selected XTAL
    // Arg0 - Link instance (LNK[N])
    // Arg1 - XTAL
    //
    Method(XCFG, 2,  NotSerialized) {
      If (Arg1 == 24000000) {
        Store (V_HDA_SNDW_IP_CLOCK_XTAL24MHZ, Local0)
        Store (V_HDA_SNDW_CLOCK_FREQ_SUPPORTED_XTAL24MHZ, Local1)
        Store (V_HDA_SNDW_FRAME_ROW_SIZE_XTAL24MHZ, Local2)
        Store (V_HDA_SNDW_FRAME_COL_SIZE_XTAL24MHZ, Local3)
      }
      ElseIf (Arg1 == 38400000) {
        Store (V_HDA_SNDW_IP_CLOCK_XTAL38P4MHZ, Local0)
        Store (V_HDA_SNDW_CLOCK_FREQ_SUPPORTED_XTAL38P4MHZ, Local1)
        Store (V_HDA_SNDW_FRAME_ROW_SIZE_XTAL38P4MHZ, Local2)
        Store (V_HDA_SNDW_FRAME_COL_SIZE_XTAL38P4MHZ, Local3)
      }
      ElseIf (Arg1 == 19200000) {
        Store (V_HDA_SNDW_IP_CLOCK_XTAL19P2MHZ, Local0)
        Store (V_HDA_SNDW_CLOCK_FREQ_SUPPORTED_XTAL19P2MHZ, Local1)
        Store (V_HDA_SNDW_FRAME_ROW_SIZE_XTAL19P2MHZ, Local2)
        Store (V_HDA_SNDW_FRAME_COL_SIZE_XTAL19P2MHZ, Local3)
      }
      Else {
        Store (0x00, Local0)
        Store (0x00, Local1)
        Store (0x00, Local2)
        Store (0x00, Local3)
      }

      // Update properties values: "intel-sdw-ip-clock", "mipi-sdw-clock-frequencies-supported",
      // "mipi-sdw-default-frame-row-size", "mipi-sdw-default-frame-col-size"
      // based on detected XTAL (0: 24MHz or 1: 38.4MHz; 2: Unsupported).
      // By default assign values corresponding to XTAL 24MHz.

      // LNK[N] (Arg0) - update property values for selected XTAL (Arg1)
      Store (Local0, SDW_LNK_ENTRY_DATA(Arg0, SDW_INTEL_IP_CLK_INDEX))
      Store (Local1, Index (DeRefOf(SDW_LNK_ENTRY_DATA(Arg0, SDW_MIPI_CLK_FREQ_SUPPORTED_INDEX)), 0))
      Store (Local2, SDW_LNK_ENTRY_DATA(Arg0, SDW_MIPI_DEFAULT_FRAME_ROW_SIZE_INDEX))
      Store (Local3, SDW_LNK_ENTRY_DATA(Arg0, SDW_MIPI_DEFAULT_FRAME_COL_SIZE_INDEX))
    }

    //
    // Update  based on PchPolicy
    // Arg0 - Link instance (LNK[N])
    // Arg1 - Data On Active Interval Select (DOAIS)
    // Arg2 - Data On Delay Select (DODS)
    Method(DCFG, 3,  NotSerialized) {
      // Update LNK[N] object, "intel-sdw-doais" property value
      // in accordance with SoundWire Link [N] Data On Active Interval Select state in PchPolicy
      Store (Arg1, SDW_LNK_ENTRY_DATA(Arg0, SDW_INTEL_DATA_ON_ACTIVE_INTERVAL_SELECT_INDEX))
      // Update LNK[N] object, "intel-sdw-dods" property value
      // in accordance with SoundWire Link [N] Data On Delay Select state in PchPolicy
      Store (Arg2, SDW_LNK_ENTRY_DATA(Arg0, SDW_INTEL_DATA_ON_DELAY_SELECT))
    }

    //
    // "mipi-sdw-master-count" update
    // Arg0 - SoundWire requester Count value
    Method(SDWU, 1,  NotSerialized) {
      Store (Arg0, Index(DeRefOf(Index(DeRefOf(Index(_DSD, 1)), 1)), 1))
    }


    Method (_INI) {

      // Update SoundWire requester Count
      SDWU(\SWMC)

      // Update Link Enable property
      // SWQx - PCH NVS variables set in accordance with PchPolicy (AudioLinkSndwX)
      QCFG(LNK0, \SWQ0, \ACS0)
      QCFG(LNK1, \SWQ1, \ACS1)
      QCFG(LNK2, \SWQ2, \ACS2)
      QCFG(LNK3, \SWQ3, \ACS3)

      // Update XTAL based properties
      // XTAL - PCH NVS variable with XTAL frequency (0 - 24MHz; 1 - 38.4MHz)
      XCFG(LNK0, \XTAL)
      XCFG(LNK1, \XTAL)
      XCFG(LNK2, \XTAL)
      XCFG(LNK3, \XTAL)

      // Update DOAIS and DODS based proparties
      // PCH NVS variables set in accordance with PchPolicy
      DCFG(LNK0, \DAI0, \DOD0)
      DCFG(LNK1, \DAI1, \DOD1)
      DCFG(LNK2, \DAI2, \DOD2)
      DCFG(LNK3, \DAI3, \DOD3)
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
        Package (2) {"mipi-sdw-link-0-subproperties", "LNK0"},
        Package (2) {"mipi-sdw-link-1-subproperties", "LNK1"},
        Package (2) {"mipi-sdw-link-2-subproperties", "LNK2"},
        Package (2) {"mipi-sdw-link-3-subproperties", "LNK3"}
      }
     })

    Name(LNK0, Package() {
      ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
      Package () {
        //
        // Intel specific properties
        //
        Package (2) {"intel-quirk-mask", 0}, // Quirks: [BIT0] - static clock, [BIT1] - bus instance disable (0-No, 1-Yes)
        Package (2) {"intel-sdw-ip-clock", V_HDA_SNDW_IP_CLOCK_XTAL38P4MHZ}, //SoundWire clock value delivered to IP
        Package (2) {"intel-sdw-doais", 1}, //SoundWire Data On Active Interval Select (0 - 3 clock periods, 1 - 4 clock periods, 2 - 5 clock periods, 3 - 6 clock periods)
        Package (2) {"intel-sdw-dods", 1}, //SoundWire Data On Delay Select (0 - 2 clock periods, 1 - 3 clock periods)
        Package (2) {"intel-autonomous-clock-stop", 0}, //SoundWire autonomous clock stop capability (0-Disabled, 1-Enabled)
        //
        // Properties defined as per the MIPI software spec for Link controllers
        //
        Package (2) {"mipi-sdw-clock-stop-mode0-supported", 1}, // Integer/Boolean
        Package (2) {"mipi-sdw-clock-stop-mode1-supported", 1}, // Integer/Boolean
        Package (2) {"mipi-sdw-clock-frequencies-supported", Package(){V_HDA_SNDW_CLOCK_FREQ_SUPPORTED_XTAL38P4MHZ}}, // Package
        Package (2) {"mipi-sdw-default-frame-rate", 48000}, // Integer
        Package (2) {"mipi-sdw-default-frame-row-size", V_HDA_SNDW_FRAME_ROW_SIZE_XTAL38P4MHZ}, // Integer
        Package (2) {"mipi-sdw-default-frame-col-size", V_HDA_SNDW_FRAME_COL_SIZE_XTAL38P4MHZ}, // Integer
        Package (2) {"mipi-sdw-dynamic-frame-shape", 1}, // Integer/Boolean
        Package (2) {"mipi-sdw-command-error-threshold", 16}, // Integer
      }
    })

    Name(LNK1, Package() {
      ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
      Package () {
        //
        // Intel specific properties
        //
        Package (2) {"intel-quirk-mask", 0}, // Quirks: [BIT0] - static clock, [BIT1] - bus instance disable (0-No, 1-Yes)
        Package (2) {"intel-sdw-ip-clock", V_HDA_SNDW_IP_CLOCK_XTAL38P4MHZ}, //SoundWire clock value delivered to IP
        Package (2) {"intel-sdw-doais", 1}, //SoundWire Data On Active Interval Select (0 - 3 clock periods, 1 - 4 clock periods, 2 - 5 clock periods, 3 - 6 clock periods)
        Package (2) {"intel-sdw-dods", 1}, //SoundWire Data On Delay Select (0 - 2 clock periods, 1 - 3 clock periods)
        Package (2) {"intel-autonomous-clock-stop", 0}, //SoundWire autonomous clock stop capability (0-Disabled, 1-Enabled)
        //
        // Properties defined as per the MIPI software spec for Link controllers
        //
        Package (2) {"mipi-sdw-clock-stop-mode0-supported", 1}, // Integer/Boolean
        Package (2) {"mipi-sdw-clock-stop-mode1-supported", 1}, // Integer/Boolean
        Package (2) {"mipi-sdw-clock-frequencies-supported", Package(){V_HDA_SNDW_CLOCK_FREQ_SUPPORTED_XTAL38P4MHZ}}, // Package
        Package (2) {"mipi-sdw-default-frame-rate", 48000}, // Integer
        Package (2) {"mipi-sdw-default-frame-row-size", V_HDA_SNDW_FRAME_ROW_SIZE_XTAL38P4MHZ}, // Integer
        Package (2) {"mipi-sdw-default-frame-col-size", V_HDA_SNDW_FRAME_COL_SIZE_XTAL38P4MHZ}, // Integer
        Package (2) {"mipi-sdw-dynamic-frame-shape", 1}, // Integer/Boolean
        Package (2) {"mipi-sdw-command-error-threshold", 16}, // Integer
      }
    })

    Name(LNK2, Package() {
      ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
      Package () {
        //
        // Intel specific properties
        //
        Package (2) {"intel-quirk-mask", 0}, // Quirks: [BIT0] - static clock, [BIT1] - bus instance disable (0-No, 1-Yes)
        Package (2) {"intel-sdw-ip-clock", V_HDA_SNDW_IP_CLOCK_XTAL38P4MHZ}, //SoundWire clock value delivered to IP
        Package (2) {"intel-sdw-doais", 1}, //SoundWire Data On Active Interval Select (0 - 3 clock periods, 1 - 4 clock periods, 2 - 5 clock periods, 3 - 6 clock periods)
        Package (2) {"intel-sdw-dods", 1}, //SoundWire Data On Delay Select (0 - 2 clock periods, 1 - 3 clock periods)
        Package (2) {"intel-autonomous-clock-stop", 0}, //SoundWire autonomous clock stop capability (0-Disabled, 1-Enabled)
        //
        // Properties defined as per the MIPI software spec for Link controllers
        //
        Package (2) {"mipi-sdw-clock-stop-mode0-supported", 1}, // Integer/Boolean
        Package (2) {"mipi-sdw-clock-stop-mode1-supported", 1}, // Integer/Boolean
        Package (2) {"mipi-sdw-clock-frequencies-supported", Package(){V_HDA_SNDW_CLOCK_FREQ_SUPPORTED_XTAL38P4MHZ}}, // Package
        Package (2) {"mipi-sdw-default-frame-rate", 48000}, // Integer
        Package (2) {"mipi-sdw-default-frame-row-size", V_HDA_SNDW_FRAME_ROW_SIZE_XTAL38P4MHZ}, // Integer
        Package (2) {"mipi-sdw-default-frame-col-size", V_HDA_SNDW_FRAME_COL_SIZE_XTAL38P4MHZ}, // Integer
        Package (2) {"mipi-sdw-dynamic-frame-shape", 1}, // Integer/Boolean
        Package (2) {"mipi-sdw-command-error-threshold", 16}, // Integer
      }
    })

    Name(LNK3, Package() {
      ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
      Package () {
        //
        // Intel specific properties
        //
        Package (2) {"intel-quirk-mask", 0}, // Quirks: [BIT0] - static clock, [BIT1] - bus instance disable (0-No, 1-Yes)
        Package (2) {"intel-sdw-ip-clock", V_HDA_SNDW_IP_CLOCK_XTAL38P4MHZ}, //SoundWire clock value delivered to IP
        Package (2) {"intel-sdw-doais", 1}, //SoundWire Data On Active Interval Select (0 - 3 clock periods, 1 - 4 clock periods, 2 - 5 clock periods, 3 - 6 clock periods)
        Package (2) {"intel-sdw-dods", 1}, //SoundWire Data On Delay Select (0 - 2 clock periods, 1 - 3 clock periods)
        Package (2) {"intel-autonomous-clock-stop", 0}, //SoundWire autonomous clock stop capability (0-Disabled, 1-Enabled)
        //
        // Properties defined as per the MIPI software spec for Link controllers
        //
        Package (2) {"mipi-sdw-clock-stop-mode0-supported", 1}, // Integer/Boolean
        Package (2) {"mipi-sdw-clock-stop-mode1-supported", 1}, // Integer/Boolean
        Package (2) {"mipi-sdw-clock-frequencies-supported", Package(){V_HDA_SNDW_CLOCK_FREQ_SUPPORTED_XTAL38P4MHZ}}, // Package
        Package (2) {"mipi-sdw-default-frame-rate", 48000}, // Integer
        Package (2) {"mipi-sdw-default-frame-row-size", V_HDA_SNDW_FRAME_ROW_SIZE_XTAL38P4MHZ}, // Integer
        Package (2) {"mipi-sdw-default-frame-col-size", V_HDA_SNDW_FRAME_COL_SIZE_XTAL38P4MHZ}, // Integer
        Package (2) {"mipi-sdw-dynamic-frame-shape", 1}, // Integer/Boolean
        Package (2) {"mipi-sdw-command-error-threshold", 16}, // Integer
      }
    })

  }
}
