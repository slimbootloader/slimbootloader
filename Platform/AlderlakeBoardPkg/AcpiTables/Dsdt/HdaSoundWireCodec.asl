/** @file

  Copyright (c) 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

///
/// SoundWire Codec ACPI definition body
///

//
//Device(SWDC){
//
  Name (_DSD, Package() {
    ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
    Package () {
      Package (2) {"intel-endpoints-num", 2},
      Package (2) {"intel-autonomous-clock-stop", 0}, //SoundWire autonomous clock stop capability (0-Disabled, 1-Enabled)
      // Descriptors based on MIPI spec for SoundWire device
      Package (2) {"mipi-sdw-sw-interface-revision", V_HDA_SNDW_SW_INTERFACE_REVISION}, // Integer
      Package (2) {"mipi-sdw-wake-up-unavailable", 0}, // Integer/Boolean
      Package (2) {"mipi-sdw-test-mode-supported", 1}, // Integer/Boolean
      Package (2) {"mipi-sdw-clock-stop-mode1-supported", 1}, // Integer/Boolean
      Package (2) {"mipi-sdw-simplified-clockstopprepare-sm-supported", 1}, // Integer/Boolean
      Package (2) {"mipi-sdw-clockstopprepare-hard-reset-behavior", 0}, // Integer
      Package (2) {"mipi-sdw-highPHY-capable", 0}, // Integer/Boolean
      Package (2) {"mipi-sdw-paging-support", 0}, // Integer/Boolean
      Package (2) {"mipi-sdw-bank-delay-support", 0}, // Integer/Boolean
      Package (2) {"mipi-sdw-port15-read-behavior", 1}, // Integer
      Package (2) {"mipi-sdw-master-count", 0x4}, // Integer
      Package (2) {"mipi-sdw-source-port-list", 4}, // Integer
      Package (2) {"mipi-sdw-sink-port-list", 4}, // Integer
    },
    ToUUID("dbb8e3e6-5886-4ba6-8795-1319f52a966b"),
    Package () {
      // Intel defined properties for Slave
      Package (2) {"intel-endpoint-descriptor-0", "EPD0"},
      Package (2) {"intel-endpoint-descriptor-1", "EPD1"},
      //Package (2) {"intel-endpoint-descriptor-(N-1)", "EPD(N-1)"},
      // MIPI Defined properties for Slave
      Package (2) {"mipi-sdw-link-0-subproperties", "LNK0"}, // Link Descriptor 0
      Package (2) {"mipi-sdw-link-1-subproperties", "LNK1"}, // Link Descriptor 1
      Package (2) {"mipi-sdw-link-2-subproperties", "LNK2"}, // Link Descriptor 2
      Package (2) {"mipi-sdw-link-3-subproperties", "LNK3"}, // Link Descriptor 3
      Package (2) {"mipi-sdw-dp-0-subproperties", "DP0"},
      Package (2) {"mipi-sdw-dp-1-source-subproperties", "DPN"}, // DP1 ADC output
      Package (2) {"mipi-sdw-dp-2-sink-subproperties", "DPN"}, // DP2 DAC
      Package (2) {"mipi-sdw-dp-1-sink-subproperties", "DPN"}, // DP1 DAC
      Package (2) {"mipi-sdw-dp-2-source-subproperties", "DPN"}, // DP2 ADC output
      Package (2) {"mipi-sdw-dp-3-sink-subproperties", "DPN"}, // DP3 DAC
      Package (2) {"mipi-sdw-dp-4-source-subproperties", "DPN"}, // DP4 ADC output
      Package (2) {"mipi-sdw-dp-5-sink-subproperties", "DPN"}, // DP5 DAC
      Package (2) {"mipi-sdw-dp-6-source-subproperties", "DPN"}, // DP6 ADC output
      Package (2) {"mipi-sdw-dp-7-sink-subproperties", "DPN"}, // DP7 DAC
      Package (2) {"mipi-sdw-dp-8-source-subproperties", "DPN"} // DP8 ADC output
     }
  })

  // Endpoint Descriptor for EP 0 to N
  // Endpoint Descriptor #0
  Name(EPD0, Package() {
    ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
    Package () {
      // Type of the endpoint
      Package (2) {"intel-endpoint-type", 1}, // Byte, range 0-1: PDM or PCM
      //UUID for aggregated endpoints
      Package (2) {"intel-endpoint-group-id", ToUUID("00000000-0000-0000-0000-000000000000")},
      Package (2) {"intel-endpoint-group-mask", 0},
      //Position for aggregated endpoints
      Package (2) {"intel-endpoint-group-position", 0}, // Byte
      Package (2) {"intel-endpoint-direction", 1}, //0-In, 1-Out
      Package (2) {"intel-endpoint-pin-descr", "EPN0"} // Reference to EPN0 Buffer
    },
    ToUUID("dbb8e3e6-5886-4ba6-8795-1319f52a966b"),
    Package () {
      Package (2) {"intel-endpoint-formats", "EFM0"},
      Package (2) {"intel-endpoint-config", "ECF0"}
    }
  })

  //Ch2_48kHz24bitFormat
  Name(WFX0, Package() {
    Package () // WAVEFORMATEX
    {
      0xFFFE, // wFormatTag
      2, // nChannels
      48000, // nSamplesPerSec
      384000, // nAvgBytesPerSec
      8, // nBlockAlign
      32, // wBitsPerSample
      0x16 // cbSize
    },
    24, // Samples
    0x3, // dwChannelMask
    ToUUID("00000001-0000-0010-8000-00AA00389B71") // SubFormat
  })

  //Ch2_96kHz24bitFormat
  Name(WFX1, Package() {
    Package () // WAVEFORMATEX
    {
      0xFFFE, // wFormatTag
      2, // nChannels
      96000, // nSamplesPerSec
      768000, // nAvgBytesPerSec
      8, // nBlockAlign
      32, // wBitsPerSample
      0x16 // cbSize
    },
    24, // Samples
    0x3, // dwChannelMask
    ToUUID("00000001-0000-0010-8000-00AA00389B71") // SubFormat
  })

  //Ch2_48kHz16bitFormat
  Name(WFX2, Package() {
    Package () // WAVEFORMATEX
    {
      0xFFFE, // wFormatTag
      2, // nChannels
      48000, // nSamplesPerSec
      192000, // nAvgBytesPerSec
      4, // nBlockAlign
      16, // wBitsPerSample
      0x16 // cbSize
    },
    16, // Samples
    0x3, // dwChannelMask
    ToUUID("00000001-0000-0010-8000-00AA00389B71") // SubFormat
  })

  //Ch2_96kHz16bitFormat
  Name(WFX3, Package() {
    Package () // WAVEFORMATEX
    {
      0xFFFE, // wFormatTag
      2, // nChannels
      96000, // nSamplesPerSec
      384000, // nAvgBytesPerSec
      4, // nBlockAlign
      16, // wBitsPerSample
      0x16 // cbSize
    },
    16, // Samples
    0x3, // dwChannelMask
    ToUUID("00000001-0000-0010-8000-00AA00389B71") // SubFormat
  })

  // Endpoint #0 Format
  Name(EFM0, Package() {
    ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
    Package () {
    Package (2) {"wfx-0", "WFX0"},
    Package (2) {"wfx-1", "WFX1"},
    Package (2) {"wfx-2", "WFX2"},
    Package (2) {"wfx-3", "WFX3"}
    }
  })

  // Endpoint #0 Configuration
  Name(ECF0, Package() {
    ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
    Package () {
    }
  })

  // Endpoint #0 Pin Descriptor
  Name(EPN0, Buffer() {
             // IntcPinDescription 2ch Headphones JackDetect
             0xE2, 0x1C, 0xF2, 0xDF, 0x0F, 0xF7, 0xD0, 0x11,
             0xB9, 0x17, 0x00, 0xA0, 0xC9, 0x22, 0x31, 0x96,
             0x01, 0x03, 0x00, 0x00, 0x00, 0x8C, 0xC9, 0xB3,
             0x00, 0x07, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00,
             0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00,
             0x00, 0x02, 0x00, 0x00, 0x00, 0x01, 0x00
             })

  // Endpoint Descriptor #1
  Name(EPD1, Package() {
    ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
    Package () {
      // Type of the endpoint
      Package (2) {"intel-endpoint-type", 1}, // Byte, range 0-1: PDM or PCM
      //UUID for aggregated endpoints
      Package (2) {"intel-endpoint-group-id", ToUUID("00000000-0000-0000-0000-000000000000")},
      Package (2) {"intel-endpoint-group-mask", 0},
      //Position for aggregated endpoints
      Package (2) {"intel-endpoint-group-position", 0}, // Byte
      Package (2) {"intel-endpoint-direction", 0}, //0-In, 1-Out
      Package (2) {"intel-endpoint-pin-descr", "EPN1"} // Reference to EPN1 Buffer
    },
    ToUUID("dbb8e3e6-5886-4ba6-8795-1319f52a966b"),
    Package () {
      Package (2) {"intel-endpoint-formats", "EFM1"},
      Package (2) {"intel-endpoint-config", "ECF1"}
    }
  })

  //Ch1_48kHz24bitFormat
  Name(WFX4, Package() {
    Package() // WAVEFORMATEX
    {
      0xFFFE, // wFormatTag
      1, // nChannels
      48000, // nSamplesPerSec
      192000, // nAvgBytesPerSec
      4, // nBlockAlign
      32, // wBitsPerSample
      0x16 // cbSize
    },
    24, // Samples
    0x1, // dwChannelMask
    ToUUID("00000001-0000-0010-8000-00AA00389B71") // SubFormat
  })

  //Ch1_96kHz24bitFormat
  Name(WFX5, Package() {
    Package() // WAVEFORMATEX
    {
      0xFFFE, // wFormatTag
      1, // nChannels
      96000, // nSamplesPerSec
      384000, // nAvgBytesPerSec
      4, // nBlockAlign
      32, // wBitsPerSample
      0x16 // cbSize
    },
    24, // Samples
    0x1, // dwChannelMask
    ToUUID("00000001-0000-0010-8000-00AA00389B71") // SubFormat
  })

  //Ch1_48kHz16bitFormat
  Name(WFX6, Package() {
    Package() // WAVEFORMATEX
    {
      0xFFFE, // wFormatTag
      1, // nChannels
      48000, // nSamplesPerSec
      96000, // nAvgBytesPerSec
      2, // nBlockAlign
      16, // wBitsPerSample
      0x16 // cbSize
    },
    16, // Samples
    0x1, // dwChannelMask
    ToUUID("00000001-0000-0010-8000-00AA00389B71") // SubFormat
  })

  //Ch1_96kHz16bitFormat
  Name(WFX7, Package() {
    Package() // WAVEFORMATEX
    {
      0xFFFE, // wFormatTag
      1, // nChannels
      96000, // nSamplesPerSec
      192000, // nAvgBytesPerSec
      2, // nBlockAlign
      16, // wBitsPerSample
      0x16 // cbSize
    },
    16, // Samples
    0x1, // dwChannelMask
    ToUUID("00000001-0000-0010-8000-00AA00389B71") // SubFormat
  })

  // Endpoint #1 Formats
  Name(EFM1, Package() {
    ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
    Package () {
    Package (2) {"wfx-0", "WFX4"},
    Package (2) {"wfx-1", "WFX5"},
    Package (2) {"wfx-2", "WFX6"},
    Package (2) {"wfx-3", "WFX7"}
    }
  })

  // Endpoint #1 Configuration
  Name(ECF1, Package() {
    ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
    Package () {
    }
  })

  // Endpoint #1 Pin Descriptor
  Name(EPN1, Buffer() {
             // IntcPinDescription 1ch Microphone JackDetect
             0xE1, 0x1B, 0xF2, 0xDF, 0x0F, 0xF7, 0xD0, 0x11,
             0xB9, 0x17, 0x00, 0xA0, 0xC9, 0x22, 0x31, 0x96,
             0x01, 0x01, 0x00, 0x00, 0x00, 0x99, 0x8C, 0xE8,
             0x00, 0x07, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00,
             0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00,
             0x00, 0x0A, 0x00, 0x00, 0x00, 0x01, 0x00
             })

  // DP0 Properties
  Name(DP0, Package() {
    ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
    Package () {
      Package (2) {"mipi-sdw-port-max-wordlength", 64}, // Integer
      Package (2) {"mipi-sdw-port-min-wordlength", 1},  // Integer
      Package (2) {"mipi-sdw-port-wordlength-configs", Package(){0}},  // Package
      Package (2) {"mipi-sdw-bra-flow-controlled", 0}, // Integer/Boolean
      Package (2) {"mipi-sdw-bra-imp-def-response-supported", 0}, // Integer/Boolean
      Package (2) {"mipi-sdw-simplified-channel-prepare-sm", 1}, // Integer/Boolean
      Package (2) {"mipi-sdw-imp-def-dp0-interrupts-supported", 0}, // Integer/Boolean
      Package (2) {"mipi-sdw-imp-def-bpt-supported", 1} // Integer/Boolean
    },
    ToUUID("dbb8e3e6-5886-4ba6-8795-1319f52a966b"),
    Package () {
      Package (2) {"mipi-sdw-port-bra-mode", "BRA0"}
    }
  })

  // DP0 BRA mode properties (DP0MODE0)
  Name(BRA0, Package() {
    ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
    Package () {
      Package (2) {"mipi-sdw-bra-mode-max-bus-frequency", 24000000}, // Integer
      Package (2) {"bra-mode-min-bus-frequency", 2400000}, // Integer
      Package (2) {"mipi-sdw-bra-mode-bus-frequency-configs", Package(){0}}, // Package
      Package (2) {"mipi-sdw-bra-mode-max-data-per-frame", 470}, // Integer
      Package (2) {"mipi-sdw-bra-mode-min-us-between-transactions", 0} // Integer
    }
  })

  // DPn properties
  Name(DPN, Package() {
    ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
    Package () {
      Package (2) {"mipi-sdw-port-max-wordlength", 64}, // Integer
      Package (2) {"mipi-sdw-port-min-wordlength", 1},  // Integer
      Package (2) {"mipi-sdw-port-wordlength-configs", Package(){0}},  // Package
      Package (2) {"mipi-sdw-data-port-type", 0}, // Integer
      Package (2) {"mipi-sdw-max-grouping-supported", 0}, // Integer
      Package (2) {"mipi-sdw-simplified-channel-prepare-sm", 1},  // Integer/Boolean
      Package (2) {"mipi-sdw-imp-def-dpn-interrupts-supported", 0},  // Integer
      Package (2) {"mipi-sdw-min-channel-number", 2},  // Integer
      Package (2) {"mipi-sdw-max-channel-number", 2},  // Integer
      Package (2) {"mipi-sdw-channel-number-list", Package(){0}},  // Package
      Package (2) {"mipi-sdw-modes-supported", 1},  // Integer
      Package (2) {"mipi-sdw-block-packing-mode", 1},  // Integer/Boolean
      Package (2) {"mipi-sdw-port-encoding-type", 0}  // Integer
    },
    ToUUID("dbb8e3e6-5886-4ba6-8795-1319f52a966b"),
    Package () {
      Package (2) {"mipi-sdw-port-audio-mode-0", "AM0"}
    }
  })

  // Audio Mode properties (DPNMODE0)
  Name(AM0, Package() {
    ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
    Package () {
      Package (2) {"mipi-sdw-audio-mode-max-bus-frequency", 24000000}, // Integer
      Package (2) {"mipi-sdw-audio-mode-min-bus-frequency", 2400000}, // Integer
      Package (2) {"mipi-sdw-audio-mode-bus-frequency-configs", Package(){0}}, // Package
      Package (2) {"mipi-sdw-audio-mode-max-sampling-frequency", 192000}, // Integer
      Package (2) {"mipi-sdw-audio-mode-min-sampling-frequency", 8000}, // Integer
      Package (2) {"mipi-sdw-audio-mode-sampling-frequency-configs", Package(){0}}, // Package
      Package (2) {"mipi-sdw-audio-mode-prepare-channel-behavior", 0}, // Integer
      Package (2) {"mipi-sdw-audio-mode-glitchless-transitions", 0} // Integer
    }
  })
//
//} // END Device(SWDC)
//
