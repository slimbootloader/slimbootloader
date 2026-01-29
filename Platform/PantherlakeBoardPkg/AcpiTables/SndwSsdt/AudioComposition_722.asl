/** @file
  SoundWire topology definitions

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

Name(_DSD, Package() {
    ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),    // Device Properties UUID
    Package () {
       Package (2) {"acpi-acd-interface-revision", 0x00000001},    // 0.1 Spec Version Bits 31-16: (Upper word) Major version number. Bits 15-0: (Lower word) Minor version number
       Package (2) {"acpi-acd-endpoint-count", 8},    // 8 Endpoints: MicrophoneArray, Headphones, LineOut, HeadsetOutput, Microphone, LineIn, HeadsetMic, Speaker
    },
    ToUUID("dbb8e3e6-5886-4ba6-8795-1319f52a966b"),    // Hierarchical Data Extension UUID
    Package () {
       Package (2) {"acpi-acd-endpoint-0-properties", "EP01"},     // Configuration for MicrophoneArray
       Package (2) {"acpi-acd-endpoint-1-properties", "EP02"},     // Configuration for Headphones
       Package (2) {"acpi-acd-endpoint-2-properties", "EP03"},     // Configuration for LineOut
       Package (2) {"acpi-acd-endpoint-3-properties", "EP04"},     // Configuration for HeadsetOutput
       Package (2) {"acpi-acd-endpoint-4-properties", "EP05"},     // Configuration for Microphone
       Package (2) {"acpi-acd-endpoint-5-properties", "EP06"},     // Configuration for LineIn
       Package (2) {"acpi-acd-endpoint-6-properties", "EP07"},     // Configuration for HeadsetMic
       Package (2) {"acpi-acd-endpoint-7-properties", "EP08"},     // Configuration for Speaker

    }
}) //End _DSD

Name(EP01, Package() {
   ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),    // Device Properties UUID
   Package () {
      Package (2) {"acpi-acd-endpoint-friendly-name", "MicrophoneArray"},
      Package (2) {"acpi-acd-endpoint-config-count", 1},    // Only one config, No alternate config
      Package (2) {"acpi-acd-endpoint-id", 2},    // Endpoint id
   },
   ToUUID("dbb8e3e6-5886-4ba6-8795-1319f52a966b"),    // Hierarchical Data Extension UUID
   Package () {
      Package (2) {"acpi-acd-endpoint-config-0-properties", "EC10"},
   }
}) //End EP01

Name(EC10, Package() {
   ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),    // Device Properties UUID
   Package () {
      Package (2) {"acpi-acd-config-priority", 1},
      Package (2) {"acpi-acd-config-friendly-name", "MicrophoneArray_With_DSP"},
      Package (2) {"acpi-acd-collection-type", 1},    // 0: Generic, 1: SoundWire
      Package (2) {"acpi-acd-collection-ordering", 0},    // Serial Connection
      Package (2) {"acpi-acd-collection-count", 2},    // DSP + Sdca Audio Function makes the endpoint
   },
   ToUUID("dbb8e3e6-5886-4ba6-8795-1319f52a966b"),    // Hierarchical Data Extension UUID
   Package () {
      Package (2) {"acpi-acd-collection-0-properties", "CC10"},    // DSP Configuration
      Package (2) {"acpi-acd-collection-1-properties", "CC11"},    // Sdca Audio Function Configuration
   }
}) //End EC10

Name(CC10, Package() {
   ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),    // Device Properties UUID
   Package () {
      Package (2) {"acpi-acd-device-namestring", "\\_SB.PC00.HDAS.IDA.ISSW"},
      Package (2) {"acpi-acd-device-type", 0},    // 0: Generic, 1: SoundWire
   },
   ToUUID("dbb8e3e6-5886-4ba6-8795-1319f52a966b"),    // Hierarchical Data Extension UUID
   Package () {
      Package (2) {"msft-acx-properties", "AC00"},    // Acx specific properties
      Package (2) {"acpi-acd-vendor-collection-properties", "VN01"},
   }
}) //End CC10

Name(AC00, Package() {    // This package is shared by all DSP devices in this composition table
    ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),    // Device Properties UUID
    Package () {
       Package (2) {"msft-acx-factory-circuit", One},    // Is an ACX Factory
       Package (2) {"msft-acx-core-circuit", Zero},    // Not an ACX Core circuit
    }
}) //End AC00

Name(VN01, Package() {    // Passed in as an AcxObjectBag during circuit creation. Contents of this package are proprietary.
   ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),    // Device Properties UUID
   Package () {
      Package (2) {"acpi-vendor-id", 0x1},
      Package (2) {"acpi-vendor-config-type", "Streaming_MicrophoneArray"},
      Package (2) {"acpi-vendor-sdca-terminal-type", 0x0201},
      Package (2) {"acpi-acd-sdca-terminal-type", 0x0201},
      Package (2) {"acpi-acd-connection-count", 1},
      Package (2) {"acpi-vendor-connection-0-dsp-pin", 0x0},
      Package (2) {"acpi-vendor-connection-0-stream-type", 0x0},
      Package (2) {"acpi-vendor-connection-0-peripheral-dp-number", 0x6},
      Package (2) {"acpi-acd-connection-0-properties",
                        Buffer()
                        {
                           0x02,
                           0x00, 0x03, 0x00, 0x00,
                           0x01, 0x22, 0x07, 0x5D, 0x02, 0x30, 0x03, 0x00,
                           0x06
                        }
                  },
   }
}) //End VN01

Name(CC11, Package() {
   ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),    // Device Properties UUID
   Package () {
      Package (2) {"acpi-acd-device-namestring", "\\_SB.PC00.HDAS.IDA.SNDW.SWD0.AF02"},
      Package (2) {"acpi-acd-device-type", 1},    // 0: Generic, 1: SoundWire
      Package (2) {"acpi-acd-sdca-terminal-id", 0x26},    // Entity id of the Analog terminal used for this endpoint
      Package (2) {"acpi-acd-sdca-terminal-type", 0x0201},    // Sdca Terminal Type based on Sdca Version implemented by Audio Function
   },
   ToUUID("dbb8e3e6-5886-4ba6-8795-1319f52a966b"),    // Hierarchical Data Extension UUID
   Package () {
      Package (2) {"msft-acx-properties", "AC02"},    // Acx specific properties
   }
}) //End CC11

Name(AC02, Package() {    // This package is shared by all Codec devices in this composition table
    ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),    // Device Properties UUID
    Package () {
       Package (2) {"msft-acx-factory-circuit", Zero},    // Not an ACX Factory
       Package (2) {"msft-acx-core-circuit", One},    // ACX Core circuit
    }
}) //End AC02

Name(EP02, Package() {
   ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),    // Device Properties UUID
   Package () {
      Package (2) {"acpi-acd-endpoint-friendly-name", "Headphones"},
      Package (2) {"acpi-acd-endpoint-config-count", 1},    // Only one config, No alternate config
      Package (2) {"acpi-acd-endpoint-id", 3},    // Endpoint id
   },
   ToUUID("dbb8e3e6-5886-4ba6-8795-1319f52a966b"),    // Hierarchical Data Extension UUID
   Package () {
      Package (2) {"acpi-acd-endpoint-config-0-properties", "EC20"},
   }
}) //End EP02

Name(EC20, Package() {
   ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),    // Device Properties UUID
   Package () {
      Package (2) {"acpi-acd-config-priority", 1},
      Package (2) {"acpi-acd-config-friendly-name", "Headphones_With_DSP"},
      Package (2) {"acpi-acd-collection-type", 1},    // 0: Generic, 1: SoundWire
      Package (2) {"acpi-acd-collection-ordering", 0},    // Serial Connection
      Package (2) {"acpi-acd-collection-count", 2},    // DSP + Sdca Audio Function makes the endpoint
   },
   ToUUID("dbb8e3e6-5886-4ba6-8795-1319f52a966b"),    // Hierarchical Data Extension UUID
   Package () {
      Package (2) {"acpi-acd-collection-0-properties", "CC20"},    // DSP Configuration
      Package (2) {"acpi-acd-collection-1-properties", "CC21"},    // Sdca Audio Function Configuration
   }
}) //End EC20

Name(CC20, Package() {
   ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),    // Device Properties UUID
   Package () {
      Package (2) {"acpi-acd-device-namestring", "\\_SB.PC00.HDAS.IDA.ISSW"},
      Package (2) {"acpi-acd-device-type", 0},    // 0: Generic, 1: SoundWire
   },
   ToUUID("dbb8e3e6-5886-4ba6-8795-1319f52a966b"),    // Hierarchical Data Extension UUID
   Package () {
      Package (2) {"msft-acx-properties", "AC00"},    // Acx specific properties
      Package (2) {"acpi-acd-vendor-collection-properties", "VN02"},
   }
}) //End CC20

Name(VN02, Package() {    // Passed in as an AcxObjectBag during circuit creation. Contents of this package are proprietary.
   ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),    // Device Properties UUID
   Package () {
      Package (2) {"acpi-vendor-id", 0x1},
      Package (2) {"acpi-vendor-config-type", "Streaming_Headphones"},
      Package (2) {"acpi-vendor-sdca-terminal-type", 0x06C0},
      Package (2) {"acpi-acd-sdca-terminal-type", 0x06C0},
      Package (2) {"acpi-acd-connection-count", 1},
      Package (2) {"acpi-vendor-connection-0-dsp-pin", 0x0},
      Package (2) {"acpi-vendor-connection-0-stream-type", 0x0},
      Package (2) {"acpi-vendor-connection-0-peripheral-dp-number", 0x1},
      Package (2) {"acpi-acd-connection-0-properties",
                        Buffer()
                        {
                           0x01,
                           0x00, 0x03, 0x00, 0x00,
                           0x01, 0x22, 0x07, 0x5D, 0x02, 0x30, 0x03, 0x00,
                           0x01
                        }
                  },
   }
}) //End VN02

Name(CC21, Package() {
   ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),    // Device Properties UUID
   Package () {
      Package (2) {"acpi-acd-device-namestring", "\\_SB.PC00.HDAS.IDA.SNDW.SWD0.AF01"},
      Package (2) {"acpi-acd-device-type", 1},    // 0: Generic, 1: SoundWire
      Package (2) {"acpi-acd-sdca-terminal-id", 0x6},    // Entity id of the Analog terminal used for this endpoint
      Package (2) {"acpi-acd-sdca-terminal-type", 0x06C0},    // Sdca Terminal Type based on Sdca Version implemented by Audio Function
   },
   ToUUID("dbb8e3e6-5886-4ba6-8795-1319f52a966b"),    // Hierarchical Data Extension UUID
   Package () {
      Package (2) {"msft-acx-properties", "AC02"},    // Acx specific properties
   }
}) //End CC21

Name(EP03, Package() {
   ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),    // Device Properties UUID
   Package () {
      Package (2) {"acpi-acd-endpoint-friendly-name", "LineOut"},
      Package (2) {"acpi-acd-endpoint-config-count", 1},    // Only one config, No alternate config
      Package (2) {"acpi-acd-endpoint-id", 3},    // Endpoint id
   },
   ToUUID("dbb8e3e6-5886-4ba6-8795-1319f52a966b"),    // Hierarchical Data Extension UUID
   Package () {
      Package (2) {"acpi-acd-endpoint-config-0-properties", "EC30"},
   }
}) //End EP03

Name(EC30, Package() {
   ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),    // Device Properties UUID
   Package () {
      Package (2) {"acpi-acd-config-priority", 1},
      Package (2) {"acpi-acd-config-friendly-name", "LineOut_With_DSP"},
      Package (2) {"acpi-acd-collection-type", 1},    // 0: Generic, 1: SoundWire
      Package (2) {"acpi-acd-collection-ordering", 0},    // Serial Connection
      Package (2) {"acpi-acd-collection-count", 2},    // DSP + Sdca Audio Function makes the endpoint
   },
   ToUUID("dbb8e3e6-5886-4ba6-8795-1319f52a966b"),    // Hierarchical Data Extension UUID
   Package () {
      Package (2) {"acpi-acd-collection-0-properties", "CC30"},    // DSP Configuration
      Package (2) {"acpi-acd-collection-1-properties", "CC31"},    // Sdca Audio Function Configuration
   }
}) //End EC30

Name(CC30, Package() {
   ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),    // Device Properties UUID
   Package () {
      Package (2) {"acpi-acd-device-namestring", "\\_SB.PC00.HDAS.IDA.ISSW"},
      Package (2) {"acpi-acd-device-type", 0},    // 0: Generic, 1: SoundWire
   },
   ToUUID("dbb8e3e6-5886-4ba6-8795-1319f52a966b"),    // Hierarchical Data Extension UUID
   Package () {
      Package (2) {"msft-acx-properties", "AC00"},    // Acx specific properties
      Package (2) {"acpi-acd-vendor-collection-properties", "VN03"},
   }
}) //End CC30

Name(VN03, Package() {    // Passed in as an AcxObjectBag during circuit creation. Contents of this package are proprietary.
   ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),    // Device Properties UUID
   Package () {
      Package (2) {"acpi-vendor-id", 0x1},
      Package (2) {"acpi-vendor-config-type", "Streaming_LineOut"},
      Package (2) {"acpi-vendor-sdca-terminal-type", 0x0690},
      Package (2) {"acpi-acd-sdca-terminal-type", 0x0690},
      Package (2) {"acpi-acd-connection-count", 1},
      Package (2) {"acpi-vendor-connection-0-dsp-pin", 0x0},
      Package (2) {"acpi-vendor-connection-0-stream-type", 0x0},
      Package (2) {"acpi-vendor-connection-0-peripheral-dp-number", 0x1},
      Package (2) {"acpi-acd-connection-0-properties",
                        Buffer()
                        {
                           0x01,
                           0x00, 0x03, 0x00, 0x00,
                           0x01, 0x22, 0x07, 0x5D, 0x02, 0x30, 0x03, 0x00,
                           0x01
                        }
                  },
   }
}) //End VN03

Name(CC31, Package() {
   ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),    // Device Properties UUID
   Package () {
      Package (2) {"acpi-acd-device-namestring", "\\_SB.PC00.HDAS.IDA.SNDW.SWD0.AF01"},
      Package (2) {"acpi-acd-device-type", 1},    // 0: Generic, 1: SoundWire
      Package (2) {"acpi-acd-sdca-terminal-id", 0x7},    // Entity id of the Analog terminal used for this endpoint
      Package (2) {"acpi-acd-sdca-terminal-type", 0x0690},    // Sdca Terminal Type based on Sdca Version implemented by Audio Function
   },
   ToUUID("dbb8e3e6-5886-4ba6-8795-1319f52a966b"),    // Hierarchical Data Extension UUID
   Package () {
      Package (2) {"msft-acx-properties", "AC02"},    // Acx specific properties
   }
}) //End CC31

Name(EP04, Package() {
   ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),    // Device Properties UUID
   Package () {
      Package (2) {"acpi-acd-endpoint-friendly-name", "HeadsetOutput"},
      Package (2) {"acpi-acd-endpoint-config-count", 1},    // Only one config, No alternate config
      Package (2) {"acpi-acd-endpoint-id", 3},    // Endpoint id
   },
   ToUUID("dbb8e3e6-5886-4ba6-8795-1319f52a966b"),    // Hierarchical Data Extension UUID
   Package () {
      Package (2) {"acpi-acd-endpoint-config-0-properties", "EC40"},
   }
}) //End EP04

Name(EC40, Package() {
   ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),    // Device Properties UUID
   Package () {
      Package (2) {"acpi-acd-config-priority", 1},
      Package (2) {"acpi-acd-config-friendly-name", "HeadsetOutput_With_DSP"},
      Package (2) {"acpi-acd-collection-type", 1},    // 0: Generic, 1: SoundWire
      Package (2) {"acpi-acd-collection-ordering", 0},    // Serial Connection
      Package (2) {"acpi-acd-collection-count", 2},    // DSP + Sdca Audio Function makes the endpoint
   },
   ToUUID("dbb8e3e6-5886-4ba6-8795-1319f52a966b"),    // Hierarchical Data Extension UUID
   Package () {
      Package (2) {"acpi-acd-collection-0-properties", "CC40"},    // DSP Configuration
      Package (2) {"acpi-acd-collection-1-properties", "CC41"},    // Sdca Audio Function Configuration
   }
}) //End EC40

Name(CC40, Package() {
   ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),    // Device Properties UUID
   Package () {
      Package (2) {"acpi-acd-device-namestring", "\\_SB.PC00.HDAS.IDA.ISSW"},
      Package (2) {"acpi-acd-device-type", 0},    // 0: Generic, 1: SoundWire
   },
   ToUUID("dbb8e3e6-5886-4ba6-8795-1319f52a966b"),    // Hierarchical Data Extension UUID
   Package () {
      Package (2) {"msft-acx-properties", "AC00"},    // Acx specific properties
      Package (2) {"acpi-acd-vendor-collection-properties", "VN04"},
   }
}) //End CC40

Name(VN04, Package() {    // Passed in as an AcxObjectBag during circuit creation. Contents of this package are proprietary.
   ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),    // Device Properties UUID
   Package () {
      Package (2) {"acpi-vendor-id", 0x1},
      Package (2) {"acpi-vendor-config-type", "Streaming_HeadsetOutput"},
      Package (2) {"acpi-vendor-sdca-terminal-type", 0x06D0},
      Package (2) {"acpi-acd-sdca-terminal-type", 0x06D0},
      Package (2) {"acpi-acd-connection-count", 1},
      Package (2) {"acpi-vendor-connection-0-dsp-pin", 0x0},
      Package (2) {"acpi-vendor-connection-0-stream-type", 0x0},
      Package (2) {"acpi-vendor-connection-0-peripheral-dp-number", 0x1},
      Package (2) {"acpi-acd-connection-0-properties",
                        Buffer()
                        {
                           0x01,
                           0x00, 0x03, 0x00, 0x00,
                           0x01, 0x22, 0x07, 0x5D, 0x02, 0x30, 0x03, 0x00,
                           0x01
                        }
                  },
   }
}) //End VN04

Name(CC41, Package() {
   ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),    // Device Properties UUID
   Package () {
      Package (2) {"acpi-acd-device-namestring", "\\_SB.PC00.HDAS.IDA.SNDW.SWD0.AF01"},
      Package (2) {"acpi-acd-device-type", 1},    // 0: Generic, 1: SoundWire
      Package (2) {"acpi-acd-sdca-terminal-id", 0x48},    // Entity id of the Analog terminal used for this endpoint
      Package (2) {"acpi-acd-sdca-terminal-type", 0x06D0},    // Sdca Terminal Type based on Sdca Version implemented by Audio Function
   },
   ToUUID("dbb8e3e6-5886-4ba6-8795-1319f52a966b"),    // Hierarchical Data Extension UUID
   Package () {
      Package (2) {"msft-acx-properties", "AC02"},    // Acx specific properties
   }
}) //End CC41

Name(EP05, Package() {
   ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),    // Device Properties UUID
   Package () {
      Package (2) {"acpi-acd-endpoint-friendly-name", "Microphone"},
      Package (2) {"acpi-acd-endpoint-config-count", 1},    // Only one config, No alternate config
      Package (2) {"acpi-acd-endpoint-id", 4},    // Endpoint id
   },
   ToUUID("dbb8e3e6-5886-4ba6-8795-1319f52a966b"),    // Hierarchical Data Extension UUID
   Package () {
      Package (2) {"acpi-acd-endpoint-config-0-properties", "EC50"},
   }
}) //End EP05

Name(EC50, Package() {
   ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),    // Device Properties UUID
   Package () {
      Package (2) {"acpi-acd-config-priority", 1},
      Package (2) {"acpi-acd-config-friendly-name", "Microphone_With_DSP"},
      Package (2) {"acpi-acd-collection-type", 1},    // 0: Generic, 1: SoundWire
      Package (2) {"acpi-acd-collection-ordering", 0},    // Serial Connection
      Package (2) {"acpi-acd-collection-count", 2},    // DSP + Sdca Audio Function makes the endpoint
   },
   ToUUID("dbb8e3e6-5886-4ba6-8795-1319f52a966b"),    // Hierarchical Data Extension UUID
   Package () {
      Package (2) {"acpi-acd-collection-0-properties", "CC50"},    // DSP Configuration
      Package (2) {"acpi-acd-collection-1-properties", "CC51"},    // Sdca Audio Function Configuration
   }
}) //End EC50

Name(CC50, Package() {
   ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),    // Device Properties UUID
   Package () {
      Package (2) {"acpi-acd-device-namestring", "\\_SB.PC00.HDAS.IDA.ISSW"},
      Package (2) {"acpi-acd-device-type", 0},    // 0: Generic, 1: SoundWire
   },
   ToUUID("dbb8e3e6-5886-4ba6-8795-1319f52a966b"),    // Hierarchical Data Extension UUID
   Package () {
      Package (2) {"msft-acx-properties", "AC00"},    // Acx specific properties
      Package (2) {"acpi-acd-vendor-collection-properties", "VN05"},
   }
}) //End CC50

Name(VN05, Package() {    // Passed in as an AcxObjectBag during circuit creation. Contents of this package are proprietary.
   ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),    // Device Properties UUID
   Package () {
      Package (2) {"acpi-vendor-id", 0x1},
      Package (2) {"acpi-vendor-config-type", "Streaming_Microphone"},
      Package (2) {"acpi-vendor-sdca-terminal-type", 0x06A0},
      Package (2) {"acpi-acd-sdca-terminal-type", 0x06A0},
      Package (2) {"acpi-acd-connection-count", 1},
      Package (2) {"acpi-vendor-connection-0-dsp-pin", 0x0},
      Package (2) {"acpi-vendor-connection-0-stream-type", 0x0},
      Package (2) {"acpi-vendor-connection-0-peripheral-dp-number", 0x2},
      Package (2) {"acpi-acd-connection-0-properties",
                        Buffer()
                        {
                           0x02,
                           0x00, 0x03, 0x00, 0x00,
                           0x01, 0x22, 0x07, 0x5D, 0x02, 0x30, 0x03, 0x00,
                           0x02
                        }
                  },
   }
}) //End VN05

Name(CC51, Package() {
   ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),    // Device Properties UUID
   Package () {
      Package (2) {"acpi-acd-device-namestring", "\\_SB.PC00.HDAS.IDA.SNDW.SWD0.AF01"},
      Package (2) {"acpi-acd-device-type", 1},    // 0: Generic, 1: SoundWire
      Package (2) {"acpi-acd-sdca-terminal-id", 0x8},    // Entity id of the Analog terminal used for this endpoint
      Package (2) {"acpi-acd-sdca-terminal-type", 0x06a0},    // Sdca Terminal Type based on Sdca Version implemented by Audio Function
   },
   ToUUID("dbb8e3e6-5886-4ba6-8795-1319f52a966b"),    // Hierarchical Data Extension UUID
   Package () {
      Package (2) {"msft-acx-properties", "AC02"},    // Acx specific properties
   }
}) //End CC51

Name(EP06, Package() {
   ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),    // Device Properties UUID
   Package () {
      Package (2) {"acpi-acd-endpoint-friendly-name", "LineIn"},
      Package (2) {"acpi-acd-endpoint-config-count", 1},    // Only one config, No alternate config
      Package (2) {"acpi-acd-endpoint-id", 4},    // Endpoint id
   },
   ToUUID("dbb8e3e6-5886-4ba6-8795-1319f52a966b"),    // Hierarchical Data Extension UUID
   Package () {
      Package (2) {"acpi-acd-endpoint-config-0-properties", "EC60"},
   }
}) //End EP06

Name(EC60, Package() {
   ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),    // Device Properties UUID
   Package () {
      Package (2) {"acpi-acd-config-priority", 1},
      Package (2) {"acpi-acd-config-friendly-name", "LineIn_With_DSP"},
      Package (2) {"acpi-acd-collection-type", 1},    // 0: Generic, 1: SoundWire
      Package (2) {"acpi-acd-collection-ordering", 0},    // Serial Connection
      Package (2) {"acpi-acd-collection-count", 2},    // DSP + Sdca Audio Function makes the endpoint
   },
   ToUUID("dbb8e3e6-5886-4ba6-8795-1319f52a966b"),    // Hierarchical Data Extension UUID
   Package () {
      Package (2) {"acpi-acd-collection-0-properties", "CC60"},    // DSP Configuration
      Package (2) {"acpi-acd-collection-1-properties", "CC61"},    // Sdca Audio Function Configuration
   }
}) //End EC60

Name(CC60, Package() {
   ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),    // Device Properties UUID
   Package () {
      Package (2) {"acpi-acd-device-namestring", "\\_SB.PC00.HDAS.IDA.ISSW"},
      Package (2) {"acpi-acd-device-type", 0},    // 0: Generic, 1: SoundWire
   },
   ToUUID("dbb8e3e6-5886-4ba6-8795-1319f52a966b"),    // Hierarchical Data Extension UUID
   Package () {
      Package (2) {"msft-acx-properties", "AC00"},    // Acx specific properties
      Package (2) {"acpi-acd-vendor-collection-properties", "VN06"},
   }
}) //End CC60

Name(VN06, Package() {    // Passed in as an AcxObjectBag during circuit creation. Contents of this package are proprietary.
   ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),    // Device Properties UUID
   Package () {
      Package (2) {"acpi-vendor-id", 0x1},
      Package (2) {"acpi-vendor-config-type", "Streaming_LineIn"},
      Package (2) {"acpi-vendor-sdca-terminal-type", 0x0680},
      Package (2) {"acpi-acd-sdca-terminal-type", 0x0680},
      Package (2) {"acpi-acd-connection-count", 1},
      Package (2) {"acpi-vendor-connection-0-dsp-pin", 0x0},
      Package (2) {"acpi-vendor-connection-0-stream-type", 0x0},
      Package (2) {"acpi-vendor-connection-0-peripheral-dp-number", 0x2},
      Package (2) {"acpi-acd-connection-0-properties",
                        Buffer()
                        {
                           0x02,
                           0x00, 0x03, 0x00, 0x00,
                           0x01, 0x22, 0x07, 0x5D, 0x02, 0x30, 0x03, 0x00,
                           0x02
                        }
                  },
   }
}) //End VN06

Name(CC61, Package() {
   ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),    // Device Properties UUID
   Package () {
      Package (2) {"acpi-acd-device-namestring", "\\_SB.PC00.HDAS.IDA.SNDW.SWD0.AF01"},
      Package (2) {"acpi-acd-device-type", 1},    // 0: Generic, 1: SoundWire
      Package (2) {"acpi-acd-sdca-terminal-id", 0x9},    // Entity id of the Analog terminal used for this endpoint
      Package (2) {"acpi-acd-sdca-terminal-type", 0x0680},    // Sdca Terminal Type based on Sdca Version implemented by Audio Function
   },
   ToUUID("dbb8e3e6-5886-4ba6-8795-1319f52a966b"),    // Hierarchical Data Extension UUID
   Package () {
      Package (2) {"msft-acx-properties", "AC02"},    // Acx specific properties
   }
}) //End CC61

Name(EP07, Package() {
   ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),    // Device Properties UUID
   Package () {
      Package (2) {"acpi-acd-endpoint-friendly-name", "HeadsetMic"},
      Package (2) {"acpi-acd-endpoint-config-count", 1},    // Only one config, No alternate config
      Package (2) {"acpi-acd-endpoint-id", 4},    // Endpoint id
   },
   ToUUID("dbb8e3e6-5886-4ba6-8795-1319f52a966b"),    // Hierarchical Data Extension UUID
   Package () {
      Package (2) {"acpi-acd-endpoint-config-0-properties", "EC70"},
   }
}) //End EP07

Name(EC70, Package() {
   ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),    // Device Properties UUID
   Package () {
      Package (2) {"acpi-acd-config-priority", 1},
      Package (2) {"acpi-acd-config-friendly-name", "HeadsetMic_With_DSP"},
      Package (2) {"acpi-acd-collection-type", 1},    // 0: Generic, 1: SoundWire
      Package (2) {"acpi-acd-collection-ordering", 0},    // Serial Connection
      Package (2) {"acpi-acd-collection-count", 2},    // DSP + Sdca Audio Function makes the endpoint
   },
   ToUUID("dbb8e3e6-5886-4ba6-8795-1319f52a966b"),    // Hierarchical Data Extension UUID
   Package () {
      Package (2) {"acpi-acd-collection-0-properties", "CC70"},    // DSP Configuration
      Package (2) {"acpi-acd-collection-1-properties", "CC71"},    // Sdca Audio Function Configuration
   }
}) //End EC70

Name(CC70, Package() {
   ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),    // Device Properties UUID
   Package () {
      Package (2) {"acpi-acd-device-namestring", "\\_SB.PC00.HDAS.IDA.ISSW"},
      Package (2) {"acpi-acd-device-type", 0},    // 0: Generic, 1: SoundWire
   },
   ToUUID("dbb8e3e6-5886-4ba6-8795-1319f52a966b"),    // Hierarchical Data Extension UUID
   Package () {
      Package (2) {"msft-acx-properties", "AC00"},    // Acx specific properties
      Package (2) {"acpi-acd-vendor-collection-properties", "VN07"},
   }
}) //End CC70

Name(VN07, Package() {    // Passed in as an AcxObjectBag during circuit creation. Contents of this package are proprietary.
   ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),    // Device Properties UUID
   Package () {
      Package (2) {"acpi-vendor-id", 0x1},
      Package (2) {"acpi-vendor-config-type", "Streaming_HeadsetMic"},
      Package (2) {"acpi-vendor-sdca-terminal-type", 0x06D0},
      Package (2) {"acpi-acd-sdca-terminal-type", 0x06D0},
      Package (2) {"acpi-acd-connection-count", 1},
      Package (2) {"acpi-vendor-connection-0-dsp-pin", 0x0},
      Package (2) {"acpi-vendor-connection-0-stream-type", 0x0},
      Package (2) {"acpi-vendor-connection-0-peripheral-dp-number", 0x2},
      Package (2) {"acpi-acd-connection-0-properties",
                        Buffer()
                        {
                           0x02,
                           0x00, 0x03, 0x00, 0x00,
                           0x01, 0x22, 0x07, 0x5D, 0x02, 0x30, 0x03, 0x00,
                           0x02
                        }
                  },
   }
}) //End VN07

Name(CC71, Package() {
   ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),    // Device Properties UUID
   Package () {
      Package (2) {"acpi-acd-device-namestring", "\\_SB.PC00.HDAS.IDA.SNDW.SWD0.AF01"},
      Package (2) {"acpi-acd-device-type", 1},    // 0: Generic, 1: SoundWire
      Package (2) {"acpi-acd-sdca-terminal-id", 0x43},    // Entity id of the Analog terminal used for this endpoint
      Package (2) {"acpi-acd-sdca-terminal-type", 0x06D0},    // Sdca Terminal Type based on Sdca Version implemented by Audio Function
   },
   ToUUID("dbb8e3e6-5886-4ba6-8795-1319f52a966b"),    // Hierarchical Data Extension UUID
   Package () {
      Package (2) {"msft-acx-properties", "AC02"},    // Acx specific properties
   }
}) //End CC71

Name(EP08, Package() {
   ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),    // Device Properties UUID
   Package () {
      Package (2) {"acpi-acd-endpoint-friendly-name", "Speaker"},
      Package (2) {"acpi-acd-endpoint-config-count", 1},    // Only one config, No alternate config
      Package (2) {"acpi-acd-endpoint-id", 1},    // Endpoint id
   },
   ToUUID("dbb8e3e6-5886-4ba6-8795-1319f52a966b"),    // Hierarchical Data Extension UUID
   Package () {
      Package (2) {"acpi-acd-endpoint-config-0-properties", "EC80"},
   }
}) //End EP08

Name(EC80, Package() {
   ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),    // Device Properties UUID
   Package () {
      Package (2) {"acpi-acd-config-priority", 1},
      Package (2) {"acpi-acd-config-friendly-name", "Speaker_With_DSP"},
      Package (2) {"acpi-acd-collection-type", 1},    // 0: Generic, 1: SoundWire
      Package (2) {"acpi-acd-collection-ordering", 0},    // Serial Connection
      Package (2) {"acpi-acd-collection-count", 2},    // DSP + Sdca Audio Function makes the endpoint
   },
   ToUUID("dbb8e3e6-5886-4ba6-8795-1319f52a966b"),    // Hierarchical Data Extension UUID
   Package () {
      Package (2) {"acpi-acd-collection-0-properties", "CC80"},    // DSP Configuration
      Package (2) {"acpi-acd-collection-1-properties", "CC81"},    // Sdca Audio Function Configuration
   }
}) //End EC80

Name(CC80, Package() {
   ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),    // Device Properties UUID
   Package () {
      Package (2) {"acpi-acd-device-namestring", "\\_SB.PC00.HDAS.IDA.ISSW"},
      Package (2) {"acpi-acd-device-type", 0},    // 0: Generic, 1: SoundWire
   },
   ToUUID("dbb8e3e6-5886-4ba6-8795-1319f52a966b"),    // Hierarchical Data Extension UUID
   Package () {
      Package (2) {"msft-acx-properties", "AC00"},    // Acx specific properties
      Package (2) {"acpi-acd-vendor-collection-properties", "VN08"},
   }
}) //End CC80

Name(VN08, Package() {    // Passed in as an AcxObjectBag during circuit creation. Contents of this package are proprietary.
   ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),    // Device Properties UUID
   Package () {
       Package (2) {"acpi-vendor-id", 0x1},
       Package (2) {"acpi-vendor-config-type", "Streaming_Speaker"},
       Package (2) {"acpi-vendor-sdca-terminal-type", 0x0380},
       Package (2) {"acpi-acd-sdca-terminal-type", 0x0380},
       Package (2) {"acpi-acd-connection-count", 1},
       // Speaker Render stream (raw)
       Package (2) {"acpi-vendor-connection-0-dsp-pin", 0x0},
       Package (2) {"acpi-vendor-connection-0-stream-type", 0x0},
       Package (2) {"acpi-vendor-connection-0-peripheral-dp-number", 0x3},
       // Speaker Render stream (offload)
       Package (2) {"acpi-vendor-connection-1-dsp-pin", 0x2},
       Package (2) {"acpi-vendor-connection-1-stream-type", 0x0},
       Package (2) {"acpi-vendor-connection-1-peripheral-dp-number", 0x3},
       Package (2) {"acpi-acd-connection-0-properties",
          Buffer()
          {
             0x01,
             0x00, 0x03, 0x00, 0x00,
             0x01, 0x22, 0x07, 0x5D, 0x02, 0x30, 0x03, 0x00,
             0x03
          }
       },
   }
}) //End VN08

Name(CC81, Package() {
   ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),    // Device Properties UUID
   Package () {
      Package (2) {"acpi-acd-device-namestring", "\\_SB.PC00.HDAS.IDA.SNDW.SWD0.AF04"},
      Package (2) {"acpi-acd-device-type", 1},    // 0: Generic, 1: SoundWire
      Package (2) {"acpi-acd-sdca-terminal-id", 0x42},    // Entity id of the Analog terminal used for this endpoint
      Package (2) {"acpi-acd-sdca-terminal-type", 0x0380},    // Sdca Terminal Type based on Sdca Version implemented by Audio Function
   },
   ToUUID("dbb8e3e6-5886-4ba6-8795-1319f52a966b"),    // Hierarchical Data Extension UUID
   Package () {
      Package (2) {"msft-acx-properties", "AC02"},    // Acx specific properties
   }
}) //End CC81
