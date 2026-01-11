/**@file
  ACPI definition for GPIO controller on PTL_PCD

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include <Register/GpioRegs.h>
#include <Register/GpioAcpiDefines.h>
#include <Register/PchRegsGpioVer2.h>

Scope (\_SB)
{
  //
  // GPIO _HID
  // Returns Generation specific GPIO Hardware ID
  //
  Method (GHID) {
    // Return motherboard reserved resources HID when GPIO is hidden
    if (LEqual (GPHD, 1)) {
      Return ("PNP0C02")
    }
    If (LEqual (PCHS, PCHH)) {
      Return (GPIO_HID_PTL_PCD_H)
    } Else {
      Return (GPIO_HID_PTL_PCD_P)
    }
  }

  //
  // GPIO _CRS
  // Returns Interrupt and memory resources required for GPIO controller
  //
  // Arg0 - GPIO Port ID Offset for given community
  //
  Method (GCRS, 0x1, Serialized) {
    Name (RBFL,ResourceTemplate(){
      Interrupt (ResourceConsumer, Level, ActiveLow, Shared, , , IRQL) { 14 } //Interrupt IRQ_EN
        QWordMemory (
          ResourceConsumer, ,
          MinFixed,
          MaxFixed,
          NonCacheable,
          ReadWrite,
          0x0,                // AddressGranularity
          0x0000000000000000, // AddressMinimum _MIN
          0x000000000000FFFF, // AddressMaximum _MAX
          0x0,                // AddressTranslation
          0x10000,            // RangeLength _LEN
          , ,
          RBL0,
          AddressRangeMemory,
          )
    })
    CreateDWordField (RBFL, IRQL._INT, INTL)
    Store (SGIR, INTL)
    CreateQWordField (RBFL, RBL0._MIN, CML0)
    CreateQWordField (RBFL, RBL0._MAX, CMX0)
    Store (Add (SBRG, Arg0), CML0)
    Store (Add (CML0, 0x0000FFFF), CMX0)
    Return (RBFL)
  }

  //
  // GPIO _STA
  // Returns GPIO Status
  //
  Method (GSTA) {
    If (LEqual (GPHD, 1)) { // Hide GPIO ACPI device
      Return (0x8)
    }
    Return (0xF)
  }

  //  Groups for Community 0
  Device (GPI0) {
    Method (_HID) { Return (\_SB.GHID ()) }
    Name (_CID, "INTC105F") // Generic GPIO Controller
    Method (_STA) { Return (\_SB.GSTA ()) }
    Name (_UID, 0)          // numeric value, GPIO Community
    //
    // GPIO resource designated for a single Community
    //
    Method (_CRS, 0x0, NotSerialized) { Return (\_SB.GCRS (PTL_PCD_GPIOV2_COM0)) }
    //
    //  GPIO Group Definitions
    //
    Name(GPPV, Package(){ // Data-only subnode of "GPP_V"
      ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
      Package () {
        Package () {"intc-gpio-group-name", "GPP_V"},
        Package () {"intc-gpio-pad-count", 24},
        Package () {"intc-gpio-group-offset", R_GPIOV2_PTL_PCD_PCR_GPP_V_PAD_CFG_DW0}
      }
    })
    Name(GPPC, Package(){ // Data-only subnode of "GPP_C"
      ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
      Package () {
        Package () {"intc-gpio-group-name", "GPP_C"},
        Package () {"intc-gpio-pad-count", 24},
        Package () {"intc-gpio-group-offset", R_GPIOV2_PTL_PCD_PCR_GPP_C_PAD_CFG_DW0}
      }
    })

    Name (_DSD, Package() {
      ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"), // Device Properties UUID
      Package () {
        Package () {"intc-gpio-sw-revision", 0x00010000}, // 1.0
        Package () {"intc-gpio-community-name", "Community0"},
        Package () {"intc-gpio-group-count", 2}, // tells how many extensions
        Package () {"intc-gpio-group-stride", 16}, // Offset (in Bytes) between config registers for each pad
        Package () {"intc-gpio-pad-ownership-offset", R_GPIOV2_PTL_PCD_PCR_GPP_V_PAD_OWN},
        Package () {"intc-gpio-pad-configuration-lock-offset", R_GPIOV2_PTL_PCD_PCR_GPP_V_PADCFGLOCK},
        Package () {"intc-gpio-host-software-pad-ownership-offset", R_GPIOV2_PTL_PCD_PCR_GPP_V_HOSTSW_OWN},
        Package () {"intc-gpio-gpi-interrupt-status-offset", R_GPIOV2_PTL_PCD_PCR_GPP_V_GPI_IS },
        Package () {"intc-gpio-gpi-interrupt-enable-offset", R_GPIOV2_PTL_PCD_PCR_GPP_V_GPI_IE }
      },
      ToUUID("dbb8e3e6-5886-4ba6-8795-1319f52a966b"),
      Package () {
        Package () {"intc-gpio-group-0-subproperties", "GPPV"},
        Package () {"intc-gpio-group-1-subproperties", "GPPC"}
      }
    })

  }
  //  Groups for Community 1
  Device (GPI1) {
    Method (_HID) { Return (\_SB.GHID ()) }
    Name (_CID, "INTC105F") // Generic GPIO Controller
    Method (_STA) { Return (\_SB.GSTA ()) }
    Name (_UID, 1)          // numeric value, GPIO Community
    //
    // GPIO resource designated for a single Community
    //
    Method (_CRS, 0x0, NotSerialized) { Return (\_SB.GCRS (PTL_PCD_GPIOV2_COM1)) }
    Name (_DSD, Package() {
      ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"), // Device Properties UUID
      Package () {
        Package () {"intc-gpio-sw-revision", 0x00010000}, // 1.0
        Package () {"intc-gpio-community-name", "Community1"},
        Package () {"intc-gpio-group-count", 2}, // tells how many extensions
        Package () {"intc-gpio-group-stride", 16}, // Offset (in Bytes) between config registers for each pad
        Package () {"intc-gpio-pad-ownership-offset", R_GPIOV2_PTL_PCD_PCR_GPP_F_PAD_OWN},
        Package () {"intc-gpio-pad-configuration-lock-offset", R_GPIOV2_PTL_PCD_PCR_GPP_F_PADCFGLOCK},
        Package () {"intc-gpio-host-software-pad-ownership-offset", R_GPIOV2_PTL_PCD_PCR_GPP_F_HOSTSW_OWN},
        Package () {"intc-gpio-gpi-interrupt-status-offset", R_GPIOV2_PTL_PCD_PCR_GPP_F_GPI_IS },
        Package () {"intc-gpio-gpi-interrupt-enable-offset", R_GPIOV2_PTL_PCD_PCR_GPP_F_GPI_IE }
      },
      ToUUID("dbb8e3e6-5886-4ba6-8795-1319f52a966b"),
      Package () {
        Package () {"intc-gpio-group-0-subproperties", "GPPF"},
        Package () {"intc-gpio-group-1-subproperties", "GPPE"}
      }
    })
    //
    //  GPIO Group Definitions
    //
    Name(GPPF, Package(){ // Data-only subnode of "GPP_F"
      ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
      Package () {
        Package () {"intc-gpio-group-name", "GPP_F"},
        Package () {"intc-gpio-pad-count", 26},
        Package () {"intc-gpio-group-offset", R_GPIOV2_PTL_PCD_PCR_GPP_F_PAD_CFG_DW0}
      }
    })
    Name(GPPE, Package(){ // Data-only subnode of "GPP_E"
      ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
      Package () {
        Package () {"intc-gpio-group-name", "GPP_E"},
        Package () {"intc-gpio-pad-count", 25},
        Package () {"intc-gpio-group-offset", R_GPIOV2_PTL_PCD_PCR_GPP_E_PAD_CFG_DW0}
      }
    })
  }
  //  Groups for Community 3
  Device (GPI3) {
    Method (_HID) { Return (\_SB.GHID ()) }
    Name (_CID, "INTC105F") // Generic GPIO Controller
    Method (_STA) { Return (\_SB.GSTA ()) }
    Name (_UID, 3)          // numeric value, GPIO Community
    //
    // GPIO resource designated for a single Community
    //
    Method (_CRS, 0x0, NotSerialized) { Return (\_SB.GCRS (PTL_PCD_GPIOV2_COM3)) }
    Name (_DSD, Package() {
      ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"), // Device Properties UUID
      Package () {
        Package () {"intc-gpio-sw-revision", 0x00010000}, // 1.0
        Package () {"intc-gpio-community-name", "Community3"},
        Package () {"intc-gpio-group-count", 4}, // tells how many extensions
        Package () {"intc-gpio-group-stride", 16}, // Offset (in Bytes) between config registers for each pad
        Package () {"intc-gpio-pad-ownership-offset", R_GPIOV2_PTL_PCD_PCR_CPUJTAG_PAD_OWN},
        Package () {"intc-gpio-pad-configuration-lock-offset", R_GPIOV2_PTL_PCD_PCR_CPUJTAG_PADCFGLOCK},
        Package () {"intc-gpio-host-software-pad-ownership-offset", R_GPIOV2_PTL_PCD_PCR_CPUJTAG_HOSTSW_OWN},
        Package () {"intc-gpio-gpi-interrupt-status-offset", R_GPIOV2_PTL_PCD_PCR_CPUJTAG_GPI_IS },
        Package () {"intc-gpio-gpi-interrupt-enable-offset", R_GPIOV2_PTL_PCD_PCR_CPUJTAG_GPI_IE }
      },
      ToUUID("dbb8e3e6-5886-4ba6-8795-1319f52a966b"),
      Package () {
        Package () {"intc-gpio-group-0-subproperties", "JTAG"},
        Package () {"intc-gpio-group-1-subproperties", "GPPH"},
        Package () {"intc-gpio-group-2-subproperties", "SPI0"},
        Package () {"intc-gpio-group-3-subproperties", "PIO3"}
      }
    })
    //
    //  GPIO Group Definitions
    //
    Name(JTAG, Package(){ // Data-only subnode of "CPUJTAG"
      ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
      Package () {
        Package () {"intc-gpio-group-name", "CPUJTAG"},
        Package () {"intc-gpio-pad-count", 15},
        Package () {"intc-gpio-group-offset", R_GPIOV2_PTL_PCD_PCR_CPUJTAG_PAD_CFG_DW0}
      }
    })
    Name(GPPH, Package(){ // Data-only subnode of "GPP_H"
      ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
      Package () {
        Package () {"intc-gpio-group-name", "GPP_H"},
        Package () {"intc-gpio-pad-count", 28},
        Package () {"intc-gpio-group-offset", R_GPIOV2_PTL_PCD_PCR_GPP_H_PAD_CFG_DW0}
      }
    })
    Name(SPI0, Package(){ // Data-only subnode of "GPPASPI0"
      ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
      Package () {
        Package () {"intc-gpio-group-name", "GPPASPI0"},
        Package () {"intc-gpio-pad-count", 28},
        Package () {"intc-gpio-group-offset", R_GPIOV2_PTL_PCD_PCR_GPPASPI0_PAD_CFG_DW0}
      }
    })
    Name(PIO3, Package(){ // Data-only subnode of "vGPIO_3"
      ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
      Package () {
        Package () {"intc-gpio-group-name", "vGPIO_3"},
        Package () {"intc-gpio-pad-count", 14},
        Package () {"intc-gpio-group-offset", R_GPIOV2_PTL_PCD_PCR_VGPIO_3_PAD_CFG_DW0}
      }
    })
  }
  //  Groups for Community 4
  Device (GPI4) {
    Method (_HID) { Return (\_SB.GHID ()) }
    Name (_CID, "INTC105F") // Generic GPIO Controller
    Method (_STA) { Return (\_SB.GSTA ()) }
    Name (_UID, 4)          // numeric value, GPIO Community
    //
    // GPIO resource designated for a single Community
    //
    Method (_CRS, 0x0, NotSerialized) { Return (\_SB.GCRS (PTL_PCD_GPIOV2_COM4)) }
    Name (_DSD, Package() {
      ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"), // Device Properties UUID
      Package () {
        Package () {"intc-gpio-sw-revision", 0x00010000}, // 1.0
        Package () {"intc-gpio-community-name", "Community4"},
        Package () {"intc-gpio-group-count", 1}, // tells how many extensions
        Package () {"intc-gpio-group-stride", 16}, // Offset (in Bytes) between config registers for each pad
        Package () {"intc-gpio-pad-ownership-offset", R_GPIOV2_PTL_PCD_PCR_GPP_S_PAD_OWN},
        Package () {"intc-gpio-pad-configuration-lock-offset", R_GPIOV2_PTL_PCD_PCR_GPP_S_PADCFGLOCK},
        Package () {"intc-gpio-host-software-pad-ownership-offset", R_GPIOV2_PTL_PCD_PCR_GPP_S_HOSTSW_OWN},
        Package () {"intc-gpio-gpi-interrupt-status-offset", R_GPIOV2_PTL_PCD_PCR_GPP_S_GPI_IS },
        Package () {"intc-gpio-gpi-interrupt-enable-offset", R_GPIOV2_PTL_PCD_PCR_GPP_S_GPI_IE }
      },
      ToUUID("dbb8e3e6-5886-4ba6-8795-1319f52a966b"),
      Package () {
        Package () {"intc-gpio-group-0-subproperties", "GPPS"}
      }
    })
    //
    //  GPIO Group Definitions
    //
    Name(GPPS, Package(){ // Data-only subnode of "GPP_S"
      ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
      Package () {
        Package () {"intc-gpio-group-name", "GPP_S"},
        Package () {"intc-gpio-pad-count", 8},
        Package () {"intc-gpio-group-offset", R_GPIOV2_PTL_PCD_PCR_GPP_S_PAD_CFG_DW0}
      }
    })
  }
  //  Groups for Community 5
  Device (GPI5) {
    Method (_HID) { Return (\_SB.GHID ()) }
    Name (_CID, "INTC105F") // Generic GPIO Controller
    Method (_STA) { Return (\_SB.GSTA ()) }
    Name (_UID, 5)          // numeric value, GPIO Community
    //
    // GPIO resource designated for a single Community
    //
    Method (_CRS, 0x0, NotSerialized) { Return (\_SB.GCRS (PTL_PCD_GPIOV2_COM5)) }
    Name (_DSD, Package() {
      ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"), // Device Properties UUID
      Package () {
        Package () {"intc-gpio-sw-revision", 0x00010000}, // 1.0
        Package () {"intc-gpio-community-name", "Community5"},
        Package () {"intc-gpio-group-count", 3}, // tells how many extensions
        Package () {"intc-gpio-group-stride", 16}, // Offset (in Bytes) between config registers for each pad
        Package () {"intc-gpio-pad-ownership-offset", R_GPIOV2_PTL_PCD_PCR_GPP_B_PAD_OWN},
        Package () {"intc-gpio-pad-configuration-lock-offset", R_GPIOV2_PTL_PCD_PCR_GPP_B_PADCFGLOCK},
        Package () {"intc-gpio-host-software-pad-ownership-offset", R_GPIOV2_PTL_PCD_PCR_GPP_B_HOSTSW_OWN},
        Package () {"intc-gpio-gpi-interrupt-status-offset", R_GPIOV2_PTL_PCD_PCR_GPP_B_GPI_IS },
        Package () {"intc-gpio-gpi-interrupt-enable-offset", R_GPIOV2_PTL_PCD_PCR_GPP_B_GPI_IE }
      },
      ToUUID("dbb8e3e6-5886-4ba6-8795-1319f52a966b"),
      Package () {
        Package () {"intc-gpio-group-0-subproperties", "GPPB"},
        Package () {"intc-gpio-group-1-subproperties", "GPPD"},
        Package () {"intc-gpio-group-2-subproperties", "GPIO"}
      }
    })
    //
    //  GPIO Group Definitions
    //
    Name(GPPB, Package(){ // Data-only subnode of "GPP_B"
      ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
      Package () {
        Package () {"intc-gpio-group-name", "GPP_B"},
        Package () {"intc-gpio-pad-count", 27},
        Package () {"intc-gpio-group-offset", R_GPIOV2_PTL_PCD_PCR_GPP_B_PAD_CFG_DW0}
      }
    })
    Name(GPPD, Package(){ // Data-only subnode of "GPP_D"
      ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
      Package () {
        Package () {"intc-gpio-group-name", "GPP_D"},
        Package () {"intc-gpio-pad-count", 27},
        Package () {"intc-gpio-group-offset", R_GPIOV2_PTL_PCD_PCR_GPP_D_PAD_CFG_DW0}
      }
    })
    Name(GPIO, Package(){ // Data-only subnode of "vGPIO"
      ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
      Package () {
        Package () {"intc-gpio-group-name", "vGPIO"},
        Package () {"intc-gpio-pad-count", 18},
        Package () {"intc-gpio-group-offset", R_GPIOV2_PTL_PCD_PCR_VGPIO_PAD_CFG_DW0}
      }
    })
  }
}
