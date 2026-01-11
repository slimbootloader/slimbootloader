/**@file
  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

// Values for Audio PLL 96MHz
#define V_HDA_SNDW_IP_CLOCK_PLL_96MHZ                             96000000 // 96 MHz
#define V_HDA_SNDW_CLOCK_FREQ_SUPPORTED_PLL_96MHZ_DYNAMIC         6000000, 12000000 // 6 MHz, 12 MHz
#define V_HDA_SNDW_CLOCK_FREQ_SUPPORTED_PLL_96MHZ_STATIC_6_MHZ    6000000 // 6 MHz
#define V_HDA_SNDW_CLOCK_FREQ_SUPPORTED_PLL_96MHZ_STATIC_12_MHZ   12000000 // 12 MHz
#define V_HDA_SNDW_FRAME_ROW_SIZE_PLL_96MHZ                       125
#define V_HDA_SNDW_FRAME_COL_SIZE_PLL_96MHZ                       2

// Values for XTAL 24MHz
#define V_HDA_SNDW_IP_CLOCK_XTAL24MHZ                       24000000 // 24 MHz
#define V_HDA_SNDW_CLOCK_FREQ_SUPPORTED_XTAL24MHZ           6000000  // 6 MHz
#define V_HDA_SNDW_FRAME_ROW_SIZE_XTAL24MHZ                 125
#define V_HDA_SNDW_FRAME_COL_SIZE_XTAL24MHZ                 2

// Values for XTAL 38.4MHz
#define V_HDA_SNDW_IP_CLOCK_XTAL38P4MHZ                           38400000 // 38.4 MHz
#define V_HDA_SNDW_CLOCK_FREQ_SUPPORTED_XTAL38P4MHZ_DYNAMIC       4800000, 9600000  // 4.8 MHz, 9.6 MHz
#define V_HDA_SNDW_CLOCK_FREQ_SUPPORTED_XTAL38P4MHZ_STATIC_9_6MHZ 9600000  // 9.6 MHz
#define V_HDA_SNDW_CLOCK_FREQ_SUPPORTED_XTAL38P4MHZ_STATIC_4_8MHZ 4800000  // 4.8 MHz
#define V_HDA_SNDW_FRAME_ROW_SIZE_XTAL38P4MHZ                     50
#define V_HDA_SNDW_FRAME_COL_SIZE_XTAL38P4MHZ                     4

// Values for XTAL 19.2MHz
#define V_HDA_SNDW_IP_CLOCK_XTAL19P2MHZ                     19200000 // 19.2 MHz
#define V_HDA_SNDW_CLOCK_FREQ_SUPPORTED_XTAL19P2MHZ         4800000  // 4.8 MHz
#define V_HDA_SNDW_FRAME_ROW_SIZE_XTAL19P2MHZ               50
#define V_HDA_SNDW_FRAME_COL_SIZE_XTAL19P2MHZ               4

#define V_HDA_SNDW_SW_INTERFACE_REVISION                    0x00010000

#define SDW_INTEL_QUIRK_MSK_INDEX                           0
#define SDW_INTEL_IP_CLK_INDEX                              1
#define SDW_INTEL_DATA_ON_ACTIVE_INTERVAL_SELECT_INDEX      2
#define SDW_INTEL_DATA_ON_ACTIVE_INTERVAL_EXT_SELECT_INDEX  3
#define SDW_INTEL_DATA_ON_ACTIVE_INTERVAL_EXT2_SELECT_INDEX 4
#define SDW_INTEL_DATA_ON_DELAY_SELECT                      5
#define SDW_INTEL_DATA_ON_DELAY_EXT_SELECT                  6
#define SDW_INTEL_DATA_ON_DELAY_EXT2_SELECT                 7
#define SNW_INTEL_CLOCK_LOOPBACK_DELAY_SELECT               8
#define SNW_INTEL_CLOCK_LOOPBACK_DELAY_ENABLE               9
#define SNW_INTEL_CLOCK_LOOPBACK_DELAY_SOURCE               10
#define SDW_INTEL_AUTONOMOUS_CLK_STOP_INDEX                 11
#define SDW_INTEL_SNDW_ENABLED_LANES_INDEX                  12
#define SDW_INTEL_SNDW_MLCS                                 13
#define SNW_INTEL_LAST_INDEX                                14
#define SDW_MIPI_CLK_STOP_MODE0_SUPPORT_INDEX               (SNW_INTEL_LAST_INDEX + 0)
#define SDW_MIPI_CLK_STOP_MODE1_SUPPORT_INDEX               (SNW_INTEL_LAST_INDEX + 1)
#define SDW_MIPI_CLK_FREQ_SUPPORTED_INDEX                   (SNW_INTEL_LAST_INDEX + 2)
#define SDW_MIPI_DEFAULT_FRAME_RATE_INDEX                   (SNW_INTEL_LAST_INDEX + 3)
#define SDW_MIPI_DEFAULT_FRAME_ROW_SIZE_INDEX               (SNW_INTEL_LAST_INDEX + 4)
#define SDW_MIPI_DEFAULT_FRAME_COL_SIZE_INDEX               (SNW_INTEL_LAST_INDEX + 5)
#define SDW_MIPI_DYNAMIC_FRAME_SHAPE_INDEX                  (SNW_INTEL_LAST_INDEX + 6)
#define SDW_MIPI_CMD_ERROR_THRESHOLD_INDEX                  (SNW_INTEL_LAST_INDEX + 7)

#define SDW_CLOCK_SOURCE_SELECT_XTAL                        0
#define SDW_CLOCK_SOURCE_SELECT_APLL                        1
#define SDW_MLCS_XTAL                                       0
#define SDW_MLCS_APLL                                       2
#define SDW_DYNAMIC_FRAME_SHAPE_DISABLED                    0

#define SDW_LNK_DESC_DATA(LinkDesc) DeRefOf(Index(LinkDesc, 1))
#define SDW_LNK_ENTRY(LinkDesc, EntryIndex) DeRefOf(Index(SDW_LNK_DESC_DATA(LinkDesc), EntryIndex))
#define SDW_LNK_ENTRY_DATA(LinkDesc, EntryIndex) Index (SDW_LNK_ENTRY(LinkDesc, EntryIndex), 1)

//
// SoundWire Link Controllers definition (up to 4 controllers can be defined)
//

//
// Address (_ADR) encoding:
// Bits 28-31 - Link Type
// 0 = HD-Audio, 1 = DSP, 2 = PDM, 3 = SSP, 4 = SoundWire
// Bits 0-3 - Device Instance ID (unique to virtual bus).
// SoundWire:
// 0-3 = Bus Instance 0 - 3
// For example:
// 1)  SWC0: _ADR (0x10000000) - LinkType[31:28] = 1 (DSP), Instance[3:0] = 0
// 2)  SWC1: _ADR (0x40000001) - LinkType[31:28] = 4 (SW),  Instance[3:0] = 1
// 3)  SWC2: _ADR (0x40000002) - LinkType[31:28] = 4 (SW),  Instance[3:0] = 2
//

//
// SoundWire Link Controller (Aggregated defintion for all 4 contoller instances - LinkType = SoundWire)
//
Device (SNDW) {
  Name (_ADR, 0x40000000)

  Name (_CID, Package () {
    // Precedence order does matter for evaluation of list
    "PRP00001",  // to indicate that we want to use DeviceTree-like "compatible" matching, Linux only
    "PNP0A05"    // generic container device always placed last, makes sure entry is ignored by Windows with no yellow bangs if there is no matching driver
  })

  Method (_STA, 0, NotSerialized) { // _STA: Status
    Return (0x0F)
  }

  //
  // Update Quirk mask based on enabled SoundWire interfaces and update autonomous clock stop based on PchPolicy
  // Arg0 - Link instance (LNK[N])
  // Arg1 - SoundWire Quirk Mask
  // Arg2 - Autonomous Clock Stop
  // Arg3 - Number of enabled lanes for the SoundWire link instance
  Method (QCFG, 4,  NotSerialized) {
    // Update LNK[N] object, "intel-quirk-mask" property value
    // in accordance with SoundWire Link [N] enable/disable state in PchPolicy
    Store (Arg1, SDW_LNK_ENTRY_DATA (Arg0, SDW_INTEL_QUIRK_MSK_INDEX))
    // Update LNK[N] object, "intel-autonomous-clock-stop" property value
    // in accordance with SoundWire Link [N] Autonomous Clock Stop enable/disable state in PchPolicy
    Store (Arg2, SDW_LNK_ENTRY_DATA (Arg0, SDW_INTEL_AUTONOMOUS_CLK_STOP_INDEX))
    // Update LNK[N] object, "intel-sdw-lane-mask" property value
    // in accordance with SoundWire Link [N] Multilane lanes enabled state in PchPolicy
    Store (Arg3, SDW_LNK_ENTRY_DATA (Arg0, SDW_INTEL_SNDW_ENABLED_LANES_INDEX))
  }

  //
  // Update Link properties (Clock, Supported frequency, Frame) based on selected clock (XTAL, APLL)
  // Arg0 - Link instance (LNK[N])
  // Arg1 - XTAL Clock Value
  // Arg2 - SoundWire Clock Source Select
  // Arg3 - SoundWire supported frequency pool select
  // Arg4 - SoundWire Dynamic Frame Shape
  // Arg5 - SoundWire Frame Row Size
  // Arg6 - SoundWire Frame Col Size
  Method (XCFG, 7,  NotSerialized) {
    Store (0x00, Local0)
    Store (Package () {0x00}, Local1)
    Store (0x00, Local2)
    Store (0x00, Local3)
    Store (0x00, Local4)

    Switch (ToInteger (Arg2)) {
      Case (SDW_CLOCK_SOURCE_SELECT_XTAL) {
        If (Arg1 == 24000000) {
          ADBG ("XTAL 24MHz")
          Store (V_HDA_SNDW_IP_CLOCK_XTAL24MHZ, Local0)
          Store (Package () {V_HDA_SNDW_CLOCK_FREQ_SUPPORTED_XTAL24MHZ}, Local1)
          Store (V_HDA_SNDW_FRAME_ROW_SIZE_XTAL24MHZ, Local2)
          Store (V_HDA_SNDW_FRAME_COL_SIZE_XTAL24MHZ, Local3)
        } ElseIf (Arg1 == 38400000) {
          ADBG ("XTAL 38.4MHz")
          Store (V_HDA_SNDW_IP_CLOCK_XTAL38P4MHZ, Local0)
          if (Arg3 == 1) {
            Store (Package () {V_HDA_SNDW_CLOCK_FREQ_SUPPORTED_XTAL38P4MHZ_STATIC_9_6MHZ}, Local1)
          } ElseIf (Arg3 == 2) {
            Store (Package () {V_HDA_SNDW_CLOCK_FREQ_SUPPORTED_XTAL38P4MHZ_STATIC_4_8MHZ}, Local1)
          } Else {
            Store (Package () {V_HDA_SNDW_CLOCK_FREQ_SUPPORTED_XTAL38P4MHZ_DYNAMIC}, Local1)
          }
          Store (V_HDA_SNDW_FRAME_ROW_SIZE_XTAL38P4MHZ, Local2)
          Store (V_HDA_SNDW_FRAME_COL_SIZE_XTAL38P4MHZ, Local3)
        } ElseIf (Arg1 == 19200000) {
          ADBG ("XTAL 19.2MHz")
          Store (V_HDA_SNDW_IP_CLOCK_XTAL19P2MHZ, Local0)
          Store (Package () {V_HDA_SNDW_CLOCK_FREQ_SUPPORTED_XTAL19P2MHZ}, Local1)
          Store (V_HDA_SNDW_FRAME_ROW_SIZE_XTAL19P2MHZ, Local2)
          Store (V_HDA_SNDW_FRAME_COL_SIZE_XTAL19P2MHZ, Local3)
        }

        Store (SDW_MLCS_XTAL, Local4)
      }
      Case (SDW_CLOCK_SOURCE_SELECT_APLL) {
        ADBG ("APLL 96MHz")
          if (Arg3 == 1) {
            Store (Package () {V_HDA_SNDW_CLOCK_FREQ_SUPPORTED_PLL_96MHZ_STATIC_12_MHZ}, Local1)
          } ElseIf (Arg3 == 2) {
            Store (Package () {V_HDA_SNDW_CLOCK_FREQ_SUPPORTED_PLL_96MHZ_STATIC_6_MHZ}, Local1)
          } Else {
            Store (Package () {V_HDA_SNDW_CLOCK_FREQ_SUPPORTED_PLL_96MHZ_DYNAMIC}, Local1)
          }
        Store (V_HDA_SNDW_IP_CLOCK_PLL_96MHZ, Local0)
        Store (V_HDA_SNDW_FRAME_ROW_SIZE_PLL_96MHZ, Local2)
        Store (V_HDA_SNDW_FRAME_COL_SIZE_PLL_96MHZ, Local3)
        Store (SDW_MLCS_APLL, Local4)
      }
      Default {
        ADBG ("SOURCE CLOCK UNSUPPORTED")
      }
    }

    // Overwrite the row and column size when the dynamic frame shape is disabled
    If (Arg4 == SDW_DYNAMIC_FRAME_SHAPE_DISABLED) {
      Store (Arg5, Local2)
      Store (Arg6, Local3)
    }

    // Update properties values: "intel-sdw-ip-clock", "mipi-sdw-clock-frequencies-supported",
    // "mipi-sdw-default-frame-row-size", "mipi-sdw-default-frame-col-size"
    // based on detected XTAL (0: 24MHz or 1: 38.4MHz; 2: Unsupported).
    // By default assign values corresponding to XTAL 24MHz.

    // LNK[N] (Arg0) - update property values for selected XTAL(Arg1)/APLL
    Store (Local0, SDW_LNK_ENTRY_DATA (Arg0, SDW_INTEL_IP_CLK_INDEX))
    Store (Local1, SDW_LNK_ENTRY_DATA (Arg0, SDW_MIPI_CLK_FREQ_SUPPORTED_INDEX))
    Store (Local2, SDW_LNK_ENTRY_DATA (Arg0, SDW_MIPI_DEFAULT_FRAME_ROW_SIZE_INDEX))
    Store (Local3, SDW_LNK_ENTRY_DATA (Arg0, SDW_MIPI_DEFAULT_FRAME_COL_SIZE_INDEX))

    // LNK[N] (Arg0) - "intel-sdw-mlcs" property value for selected clock
    Store (Local4, SDW_LNK_ENTRY_DATA (Arg0, SDW_INTEL_SNDW_MLCS))

    // LNK[N] (Arg0) - "mipi-sdw-dynamic-frame-shape" property value for selected frame shape (Arg4)
    Store (Arg4, SDW_LNK_ENTRY_DATA (Arg0, SDW_MIPI_DYNAMIC_FRAME_SHAPE_INDEX))
  }

  // Update SoundWire AC I/O Clock Loopback based on PchPolicy
  // Arg0 - Link instance (LNK[N])
  // Arg1 - Clock Loopback Delay Select (CLDS)
  // Arg2 - Clock Loopback Enable (CLDE)
  // Arg3 - Clock Loopback Source Select (CLSS)
  Method (CCFG, 4,  NotSerialized) {
    // Update LNK[N] object, "intel-sdw-clds" property value
    // in accordance with SoundWire Link [N] Clock Loopback Delay Select state in PchPolicy
    Store (Arg1, SDW_LNK_ENTRY_DATA (Arg0, SNW_INTEL_CLOCK_LOOPBACK_DELAY_SELECT))
    // Update LNK[N] object, "intel-sdw-clde" property value
    // in accordance with SoundWire Link [N] Clock Loopback Enable state in PchPolicy
    Store (Arg2, SDW_LNK_ENTRY_DATA (Arg0, SNW_INTEL_CLOCK_LOOPBACK_DELAY_ENABLE))
    // Update LNK[N] object, "intel-sdw-clss" property value
    // in accordance with SoundWire Link [N] SoundWire Clock Loopback Source Select state in PchPolicy
    Store (Arg3, SDW_LNK_ENTRY_DATA (Arg0, SNW_INTEL_CLOCK_LOOPBACK_DELAY_SOURCE))
  }

  // Update SoundWire AC I/O timing register based on PchPolicy
  // Arg0 - Link instance (LNK[N])
  // Arg1 - Data On Active Interval Select (DOAIS)
  // Arg2 - Data On Active Interval Select Extension (DOAISE)
  // Arg3 - Data On Active Interval Select Extension 2 (DOAISE2)
  // Arg4 - Data On Delay Select (DODS)
  // Arg5 - Data On Delay Select Extension (DODSE)
  // Arg6 - Data On Delay Select Extension 2 (DODSE2)
  Method (DCFG, 7,  NotSerialized) {
    // Update LNK[N] object, "intel-sdw-doais" property value
    // in accordance with SoundWire Link [N] Data On Active Interval Select state in PchPolicy
    Store (Arg1, SDW_LNK_ENTRY_DATA (Arg0, SDW_INTEL_DATA_ON_ACTIVE_INTERVAL_SELECT_INDEX))
    // Update LNK[N] object, "intel-sdw-doaise" property value
    // in accordance with SoundWire Link [N] Data On Active Interval Extension Select state in PchPolicy
    Store (Arg2, SDW_LNK_ENTRY_DATA (Arg0, SDW_INTEL_DATA_ON_ACTIVE_INTERVAL_EXT_SELECT_INDEX))
    // Update LNK[N] object, "intel-sdw-doaise2" property value
    // in accordance with SoundWire Link [N] Data On Active Interval Extension 2 Select state in PchPolicy
    Store (Arg3, SDW_LNK_ENTRY_DATA (Arg0, SDW_INTEL_DATA_ON_ACTIVE_INTERVAL_EXT2_SELECT_INDEX))
    // Update LNK[N] object, "intel-sdw-dods" property value
    // in accordance with SoundWire Link [N] Data On Delay Select state in PchPolicy
    Store (Arg4, SDW_LNK_ENTRY_DATA (Arg0, SDW_INTEL_DATA_ON_DELAY_SELECT))
    // Update LNK[N] object, "intel-sdw-dodse" property value
    // in accordance with SoundWire Link [N] Data On Delay Extension Select state in PchPolicy
    Store (Arg5, SDW_LNK_ENTRY_DATA (Arg0, SDW_INTEL_DATA_ON_DELAY_EXT_SELECT))
    // Update LNK[N] object, "intel-sdw-dodse2" property value
    // in accordance with SoundWire Link [N] Data On Delay Extension 2 Select state in PchPolicy
    Store (Arg6, SDW_LNK_ENTRY_DATA (Arg0, SDW_INTEL_DATA_ON_DELAY_EXT2_SELECT))
  }

  //
  // "mipi-sdw-master-count" update
  // Arg0 - SoundWire requester Count value
  Method (SDWU, 1,  NotSerialized) {
    Store (Arg0, Index (DeRefOf (Index (DeRefOf (Index (_DSD, 1)), 2)), 1))
  }


  Method (_INI) {
    ADBG ("SNDW _INI")

    // Update SoundWire requester Count
    SDWU (SWMC)

    // Update Link Enable property
    // SWQx - PCH NVS variables set in accordance with PchPolicy (AudioLinkSndwX)
    QCFG (LNK0, SWQ0, ACS0, SML0)
    QCFG (LNK1, SWQ1, ACS1, SML1)
    QCFG (LNK2, SWQ2, ACS2, SML2)
    QCFG (LNK3, SWQ3, ACS3, SML3)
    QCFG (LNK4, SWQ4, ACS4, SML4)

    // Update SoundWire clock source based properties
    // XTAL - PCH NVS variable with XTAL frequency (0 - 24MHz; 1 - 38.4MHz)
    // SWCS - PCH NVS variable with SoundWire Clock source value (0 - XTAL; 1 - Audio PLL)
    XCFG (LNK0, XTAL, SWCS, SSF0, DFS0, FRS0, FCS0)
    XCFG (LNK1, XTAL, SWCS, SSF1, DFS1, FRS1, FCS1)
    XCFG (LNK2, XTAL, SWCS, SSF2, DFS2, FRS2, FCS2)
    XCFG (LNK3, XTAL, SWCS, SSF3, DFS3, FRS3, FCS3)
    XCFG (LNK4, XTAL, SWCS, SSF4, DFS4, FRS4, FCS4)

    // Update DOAIS and DODS based proparties
    // PCH NVS variables set in accordance with PchPolicy
    DCFG (LNK0, DAI0, DAE0, DAX0, DOD0, DOE0, DOX0)
    DCFG (LNK1, DAI1, DAE1, DAX1, DOD1, DOE1, DOX1)
    DCFG (LNK2, DAI2, DAE2, DAX2, DOD2, DOE2, DOX2)
    DCFG (LNK3, DAI3, DAE3, DAX3, DOD3, DOE3, DOX3)
    DCFG (LNK4, DAI4, DAE4, DAX4, DOD4, DOE4, DOX4)

    // Update SoundWire Clock Loopback based proparties
    // PCH NVS variables set in accordance with PchPolicy
    CCFG (LNK0, CLT0,  CLE0, CLS0);
    CCFG (LNK1, CLT1,  CLE1, CLS1);
    CCFG (LNK2, CLT2,  CLE2, CLS2);
    CCFG (LNK3, CLT3,  CLE3, CLS3);
    CCFG (LNK4, CLT4,  CLE4, CLS4);
  }

  Name (_DSD, Package () {
    ToUUID ("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
    Package () {
      Package (2) {"mipi-sdw-sw-interface-revision", V_HDA_SNDW_SW_INTERFACE_REVISION},
      // mipi-sdw-manager-list deprectes mipi-sdw-master-count, in DisCo 2.0 r8
      Package (2) {"mipi-sdw-manager-list", 0x1F},
      Package (2) {"mipi-sdw-master-count", 4} // deprecated in DisCo 2.0 r8
      // Vendor specific parameters (optional)
    },
    // Properties for the SoundWire bus instances
    ToUUID ("dbb8e3e6-5886-4ba6-8795-1319f52a966b"),
    Package () {
      Package (2) {"mipi-sdw-link-0-subproperties", "LNK0"},
      Package (2) {"mipi-sdw-link-1-subproperties", "LNK1"},
      Package (2) {"mipi-sdw-link-2-subproperties", "LNK2"},
      Package (2) {"mipi-sdw-link-3-subproperties", "LNK3"},
      Package (2) {"mipi-sdw-link-4-subproperties", "LNK4"}
    }
    })

  Name (LNK0, Package () {
    ToUUID ("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
    Package () {
      //
      // Intel specific properties
      //
      Package (2) {"intel-quirk-mask", 0}, // Quirks: [BIT0] - static clock, [BIT1] - bus instance disable (0-No, 1-Yes)
      Package (2) {"intel-sdw-ip-clock", V_HDA_SNDW_IP_CLOCK_XTAL38P4MHZ}, //SoundWire clock value delivered to IP
      Package (2) {"intel-sdw-doais", 1}, //SoundWire Data On Active Interval Select (0 - 3 clock periods, 1 - 4 clock periods, 2 - 5 clock periods, 3 - 6 clock periods)
      Package (2) {"intel-sdw-doaise", 1}, //SoundWire Data On Active Interval Extension Select (0 - Disabled, 1 - Enabled)
      Package (2) {"intel-sdw-doaise2", 1}, //SoundWire Data On Active Interval Extension Select (0 - Disabled, 1 - Enabled)
      Package (2) {"intel-sdw-dods", 1}, //SoundWire Data On Delay Select (0 - 2 clock periods, 1 - 3 clock periods)
      Package (2) {"intel-sdw-dodse", 1}, //SoundWire Data On Delay Extension Select (0 - Disabled, 1 - Enabled)
      Package (2) {"intel-sdw-dodse2", 1}, //SoundWire Data On Delay Extension 2 Select (0 - Disabled, 1 - Enabled)
      Package (2) {"intel-sdw-clds", 1}, //SoundWire Clock Loopback Delay Select (0x0 - 3 clock periods, 0x1 - 4 clock periods, ... , 0xE - 17 clock periods)
      Package (2) {"intel-sdw-clde", 1}, //SoundWire Clock Loopback Enable (0 - Disable, 1 - Enable)
      Package (2) {"intel-sdw-clss", 1}, //SoundWire Clock Loopback Source Select (0 - External Loopback, 1 - Internal Loopback)
      Package (2) {"intel-autonomous-clock-stop", 0}, //SoundWire autonomous clock stop capability (0-Disabled, 1-Enabled)
      Package (2) {"intel-sdw-lane-mask", 1}, // Lane mask for multilane configuration, each bit corresponds to enabled data lane, 0b0001 - sndw0 data lane enabled
      Package (2) {"intel-sdw-mlcs", 0}, // SoundWire Clock Source Select (0 - XTAL, 2 - Audio PLL)
      //
      // Properties defined as per the MIPI software spec for Link controllers
      //
      Package (2) {"mipi-sdw-clock-stop-mode0-supported", 1}, // Integer/Boolean
      Package (2) {"mipi-sdw-clock-stop-mode1-supported", 1}, // Integer/Boolean
      Package (2) {"mipi-sdw-clock-frequencies-supported", Package () {V_HDA_SNDW_CLOCK_FREQ_SUPPORTED_XTAL38P4MHZ_DYNAMIC}}, // Package
      Package (2) {"mipi-sdw-default-frame-rate", 48000}, // Integer
      Package (2) {"mipi-sdw-default-frame-row-size", V_HDA_SNDW_FRAME_ROW_SIZE_XTAL38P4MHZ}, // Integer
      Package (2) {"mipi-sdw-default-frame-col-size", V_HDA_SNDW_FRAME_COL_SIZE_XTAL38P4MHZ}, // Integer
      Package (2) {"mipi-sdw-dynamic-frame-shape", 1}, // Integer/Boolean
      Package (2) {"mipi-sdw-command-error-threshold", 16}, // Integer
    }
  })

  Name (LNK1, Package () {
    ToUUID ("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
    Package () {
      //
      // Intel specific properties
      //
      Package (2) {"intel-quirk-mask", 0}, // Quirks: [BIT0] - static clock, [BIT1] - bus instance disable (0-No, 1-Yes)
      Package (2) {"intel-sdw-ip-clock", V_HDA_SNDW_IP_CLOCK_XTAL38P4MHZ}, //SoundWire clock value delivered to IP
      Package (2) {"intel-sdw-doais", 1}, //SoundWire Data On Active Interval Select (0 - 3 clock periods, 1 - 4 clock periods, 2 - 5 clock periods, 3 - 6 clock periods)
      Package (2) {"intel-sdw-doaise", 1}, //SoundWire Data On Active Interval Extension Select (0 - Disabled, 1 - Enabled)
      Package (2) {"intel-sdw-doaise2", 1}, //SoundWire Data On Active Interval Extension Select (0 - Disabled, 1 - Enabled)
      Package (2) {"intel-sdw-dods", 1}, //SoundWire Data On Delay Select (0 - 2 clock periods, 1 - 3 clock periods)
      Package (2) {"intel-sdw-dodse", 1}, //SoundWire Data On Delay Extension Select (0 - Disabled, 1 - Enabled)
      Package (2) {"intel-sdw-dodse2", 1}, //SoundWire Data On Delay Extension 2 Select (0 - Disabled, 1 - Enabled)
      Package (2) {"intel-sdw-clds", 1}, //SoundWire Clock Loopback Delay Select (0x0 - 3 clock periods, 0x1 - 4 clock periods, ... , 0xE - 17 clock periods)
      Package (2) {"intel-sdw-clde", 1}, //SoundWire Clock Loopback Enable (0 - Disable, 1 - Enable)
      Package (2) {"intel-sdw-clss", 1}, //SoundWire Clock Loopback Source Select (0 - External Loopback, 1 - Internal Loopback)
      Package (2) {"intel-autonomous-clock-stop", 0}, //SoundWire autonomous clock stop capability (0-Disabled, 1-Enabled)
      Package (2) {"intel-sdw-lane-mask", 1}, // Lane mask for multilane configuration, each bit corresponds to enabled data lane, 0b0001 - sndw0 data lane enabled
      Package (2) {"intel-sdw-mlcs", 0}, // SoundWire Clock Source Select (0 - XTAL, 2 - Audio PLL)
      //
      // Properties defined as per the MIPI software spec for Link controllers
      //
      Package (2) {"mipi-sdw-clock-stop-mode0-supported", 1}, // Integer/Boolean
      Package (2) {"mipi-sdw-clock-stop-mode1-supported", 1}, // Integer/Boolean
      Package (2) {"mipi-sdw-clock-frequencies-supported", Package () {V_HDA_SNDW_CLOCK_FREQ_SUPPORTED_XTAL38P4MHZ_DYNAMIC}}, // Package
      Package (2) {"mipi-sdw-default-frame-rate", 48000}, // Integer
      Package (2) {"mipi-sdw-default-frame-row-size", V_HDA_SNDW_FRAME_ROW_SIZE_XTAL38P4MHZ}, // Integer
      Package (2) {"mipi-sdw-default-frame-col-size", V_HDA_SNDW_FRAME_COL_SIZE_XTAL38P4MHZ}, // Integer
      Package (2) {"mipi-sdw-dynamic-frame-shape", 1}, // Integer/Boolean
      Package (2) {"mipi-sdw-command-error-threshold", 16}, // Integer
    }
  })

  Name (LNK2, Package () {
    ToUUID ("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
    Package () {
      //
      // Intel specific properties
      //
      Package (2) {"intel-quirk-mask", 0}, // Quirks: [BIT0] - static clock, [BIT1] - bus instance disable (0-No, 1-Yes)
      Package (2) {"intel-sdw-ip-clock", V_HDA_SNDW_IP_CLOCK_XTAL38P4MHZ}, //SoundWire clock value delivered to IP
      Package (2) {"intel-sdw-doais", 1}, //SoundWire Data On Active Interval Select (0 - 3 clock periods, 1 - 4 clock periods, 2 - 5 clock periods, 3 - 6 clock periods)
      Package (2) {"intel-sdw-doaise", 1}, //SoundWire Data On Active Interval Extension Select (0 - Disabled, 1 - Enabled)
      Package (2) {"intel-sdw-doaise2", 1}, //SoundWire Data On Active Interval Extension Select (0 - Disabled, 1 - Enabled)
      Package (2) {"intel-sdw-dods", 1}, //SoundWire Data On Delay Select (0 - 2 clock periods, 1 - 3 clock periods)
      Package (2) {"intel-sdw-dodse", 1}, //SoundWire Data On Delay Extension Select (0 - Disabled, 1 - Enabled)
      Package (2) {"intel-sdw-dodse2", 1}, //SoundWire Data On Delay Extension 2 Select (0 - Disabled, 1 - Enabled)
      Package (2) {"intel-sdw-clds", 1}, //SoundWire Clock Loopback Delay Select (0x0 - 3 clock periods, 0x1 - 4 clock periods, ... , 0xE - 17 clock periods)
      Package (2) {"intel-sdw-clde", 1}, //SoundWire Clock Loopback Enable (0 - Disable, 1 - Enable)
      Package (2) {"intel-sdw-clss", 1}, //SoundWire Clock Loopback Source Select (0 - External Loopback, 1 - Internal Loopback)
      Package (2) {"intel-autonomous-clock-stop", 0}, //SoundWire autonomous clock stop capability (0-Disabled, 1-Enabled)
      Package (2) {"intel-sdw-lane-mask", 1}, // Lane mask for multilane configuration, each bit corresponds to enabled data lane, 0b0001 - sndw0 data lane enabled
      Package (2) {"intel-sdw-mlcs", 0}, // SoundWire Clock Source Select (0 - XTAL, 2 - Audio PLL)
      //
      // Properties defined as per the MIPI software spec for Link controllers
      //
      Package (2) {"mipi-sdw-clock-stop-mode0-supported", 1}, // Integer/Boolean
      Package (2) {"mipi-sdw-clock-stop-mode1-supported", 1}, // Integer/Boolean
      Package (2) {"mipi-sdw-clock-frequencies-supported", Package () {V_HDA_SNDW_CLOCK_FREQ_SUPPORTED_XTAL38P4MHZ_DYNAMIC}}, // Package
      Package (2) {"mipi-sdw-default-frame-rate", 48000}, // Integer
      Package (2) {"mipi-sdw-default-frame-row-size", V_HDA_SNDW_FRAME_ROW_SIZE_XTAL38P4MHZ}, // Integer
      Package (2) {"mipi-sdw-default-frame-col-size", V_HDA_SNDW_FRAME_COL_SIZE_XTAL38P4MHZ}, // Integer
      Package (2) {"mipi-sdw-dynamic-frame-shape", 1}, // Integer/Boolean
      Package (2) {"mipi-sdw-command-error-threshold", 16}, // Integer
    }
  })

  Name (LNK3, Package () {
    ToUUID ("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
    Package () {
      //
      // Intel specific properties
      //
      Package (2) {"intel-quirk-mask", 0}, // Quirks: [BIT0] - static clock, [BIT1] - bus instance disable (0-No, 1-Yes)
      Package (2) {"intel-sdw-ip-clock", V_HDA_SNDW_IP_CLOCK_XTAL38P4MHZ}, //SoundWire clock value delivered to IP
      Package (2) {"intel-sdw-doais", 1}, //SoundWire Data On Active Interval Select (0 - 3 clock periods, 1 - 4 clock periods, 2 - 5 clock periods, 3 - 6 clock periods)
      Package (2) {"intel-sdw-doaise", 1}, //SoundWire Data On Active Interval Extension Select (0 - Disabled, 1 - Enabled)
      Package (2) {"intel-sdw-doaise2", 1}, //SoundWire Data On Active Interval Extension Select (0 - Disabled, 1 - Enabled)
      Package (2) {"intel-sdw-dods", 1}, //SoundWire Data On Delay Select (0 - 2 clock periods, 1 - 3 clock periods)
      Package (2) {"intel-sdw-dodse", 1}, //SoundWire Data On Delay Extension Select (0 - Disabled, 1 - Enabled)
      Package (2) {"intel-sdw-dodse2", 1}, //SoundWire Data On Delay Extension 2 Select (0 - Disabled, 1 - Enabled)
      Package (2) {"intel-sdw-clds", 1}, //SoundWire Clock Loopback Delay Select (0x0 - 3 clock periods, 0x1 - 4 clock periods, ... , 0xE - 17 clock periods)
      Package (2) {"intel-sdw-clde", 1}, //SoundWire Clock Loopback Enable (0 - Disable, 1 - Enable)
      Package (2) {"intel-sdw-clss", 1}, //SoundWire Clock Loopback Source Select (0 - External Loopback, 1 - Internal Loopback)
      Package (2) {"intel-autonomous-clock-stop", 0}, //SoundWire autonomous clock stop capability (0-Disabled, 1-Enabled)
      Package (2) {"intel-sdw-lane-mask", 1}, // Lane mask for multilane configuration, each bit corresponds to enabled data lane, 0b0001 - sndw0 data lane enabled
      Package (2) {"intel-sdw-mlcs", 0}, // SoundWire Clock Source Select (0 - XTAL, 2 - Audio PLL)
      //
      // Properties defined as per the MIPI software spec for Link controllers
      //
      Package (2) {"mipi-sdw-clock-stop-mode0-supported", 1}, // Integer/Boolean
      Package (2) {"mipi-sdw-clock-stop-mode1-supported", 1}, // Integer/Boolean
      Package (2) {"mipi-sdw-clock-frequencies-supported", Package () {V_HDA_SNDW_CLOCK_FREQ_SUPPORTED_XTAL38P4MHZ_DYNAMIC}}, // Package
      Package (2) {"mipi-sdw-default-frame-rate", 48000}, // Integer
      Package (2) {"mipi-sdw-default-frame-row-size", V_HDA_SNDW_FRAME_ROW_SIZE_XTAL38P4MHZ}, // Integer
      Package (2) {"mipi-sdw-default-frame-col-size", V_HDA_SNDW_FRAME_COL_SIZE_XTAL38P4MHZ}, // Integer
      Package (2) {"mipi-sdw-dynamic-frame-shape", 1}, // Integer/Boolean
      Package (2) {"mipi-sdw-command-error-threshold", 16}, // Integer
    }
  })

  Name (LNK4, Package () {
    ToUUID ("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
    Package () {
      //
      // Intel specific properties
      //
      Package (2) {"intel-quirk-mask", 0}, // Quirks: [BIT0] - static clock, [BIT1] - bus instance disable (0-No, 1-Yes)
      Package (2) {"intel-sdw-ip-clock", V_HDA_SNDW_IP_CLOCK_XTAL38P4MHZ}, //SoundWire clock value delivered to IP
      Package (2) {"intel-sdw-doais", 1}, //SoundWire Data On Active Interval Select (0 - 3 clock periods, 1 - 4 clock periods, 2 - 5 clock periods, 3 - 6 clock periods)
      Package (2) {"intel-sdw-doaise", 1}, //SoundWire Data On Active Interval Extension Select (0 - Disabled, 1 - Enabled)
      Package (2) {"intel-sdw-doaise2", 1}, //SoundWire Data On Active Interval Extension Select (0 - Disabled, 1 - Enabled)
      Package (2) {"intel-sdw-dods", 1}, //SoundWire Data On Delay Select (0 - 2 clock periods, 1 - 3 clock periods)
      Package (2) {"intel-sdw-dodse", 1}, //SoundWire Data On Delay Extension Select (0 - Disabled, 1 - Enabled)
      Package (2) {"intel-sdw-dodse2", 1}, //SoundWire Data On Delay Extension 2 Select (0 - Disabled, 1 - Enabled)
      Package (2) {"intel-sdw-clds", 1}, //SoundWire Clock Loopback Delay Select (0x0 - 3 clock periods, 0x1 - 4 clock periods, ... , 0xE - 17 clock periods)
      Package (2) {"intel-sdw-clde", 1}, //SoundWire Clock Loopback Enable (0 - Disable, 1 - Enable)
      Package (2) {"intel-sdw-clss", 1}, //SoundWire Clock Loopback Source Select (0 - External Loopback, 1 - Internal Loopback)
      Package (2) {"intel-autonomous-clock-stop", 0}, //SoundWire autonomous clock stop capability (0-Disabled, 1-Enabled)
      Package (2) {"intel-sdw-lane-mask", 1}, // Lane mask for multilane configuration, each bit corresponds to enabled data lane, 0b0001 - sndw0 data lane enabled
      Package (2) {"intel-sdw-mlcs", 0}, // SoundWire Clock Source Select (0 - XTAL, 2 - Audio PLL)
      //
      // Properties defined as per the MIPI software spec for Link controllers
      //
      Package (2) {"mipi-sdw-clock-stop-mode0-supported", 1}, // Integer/Boolean
      Package (2) {"mipi-sdw-clock-stop-mode1-supported", 1}, // Integer/Boolean
      Package (2) {"mipi-sdw-clock-frequencies-supported", Package () {V_HDA_SNDW_CLOCK_FREQ_SUPPORTED_XTAL38P4MHZ_DYNAMIC}}, // Package
      Package (2) {"mipi-sdw-default-frame-rate", 48000}, // Integer
      Package (2) {"mipi-sdw-default-frame-row-size", V_HDA_SNDW_FRAME_ROW_SIZE_XTAL38P4MHZ}, // Integer
      Package (2) {"mipi-sdw-default-frame-col-size", V_HDA_SNDW_FRAME_COL_SIZE_XTAL38P4MHZ}, // Integer
      Package (2) {"mipi-sdw-dynamic-frame-shape", 1}, // Integer/Boolean
      Package (2) {"mipi-sdw-command-error-threshold", 16}, // Integer
    }
  })

}
