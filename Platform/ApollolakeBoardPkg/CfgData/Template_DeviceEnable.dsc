## @file
#
#  Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
#  This program and the accompanying materials
#  are licensed and made available under the terms and conditions of the BSD License
#  which accompanies this distribution.  The full text of the license may be found at
#  http://opensource.org/licenses/bsd-license.php
#
#  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
#  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
#
##

  #
  # ========================= DEV_EN_TMPL ==========================
  #


  # !BSF DEFT:{DEV_EN_TMPL:START}

    # !BSF PAGES:{CTRL0:DEV_EN_DIS:"Control 0"}
    # !BSF PAGE:{CTRL0}
    # !BSF NAME:{DEV ENABLE/DISABLE CTRL 0}
    # !BSF HELP:{Device Enable/Disable Control 0}
    # !HDR STRUCT:{DEV_EN_CTRL_0}
    gCfgData.DevEnControl0                  |      * | 0x04 | $(1)

      # !BSF NAME:{HD Audio Support} TYPE:{Combo} OPTION:{$EN_DIS}
      # !BSF HELP:{Enable/disable HDA Audio Feature. 0:Disable, 1:Enable(Default).}
      # !BSF FIELD:{HdaEnable:1b}

      # !BSF NAME:{HD Audio DSP Support} TYPE:{Combo} OPTION:{$EN_DIS}
      # !BSF HELP:{Enable/disable HDA Audio DSP Feature. 0:Disable, 1:Enable(Default).}
      # !BSF FIELD:{DspEnable:1b}

      # !BSF NAME:{I2C Device 0} TYPE:{Combo} OPTION:{0: Disabled, 1: PCI Mode, 2: ACPI Mode}
      # !BSF HELP:{Enable/Disable I2C Device 0. 0:Disabled, 1:PCI Mode(Default), 2:ACPI Mode.}
      # !BSF FIELD:{I2C0Enable:2b}

      # !BSF NAME:{I2C Device 1} TYPE:{Combo} OPTION:{0: Disabled, 1: PCI Mode, 2: ACPI Mode}
      # !BSF HELP:{Enable/Disable I2C Device 1. 0:Disabled, 1:PCI Mode(Default), 2:ACPI Mode.}
      # !BSF FIELD:{I2C1Enable:2b}

      # !BSF NAME:{I2C Device 2} TYPE:{Combo} OPTION:{0: Disabled, 1: PCI Mode, 2: ACPI Mode}
      # !BSF HELP:{Enable/Disable I2C Device 2. 0:Disabled, 1:PCI Mode(Default), 2:ACPI Mode.}
      # !BSF FIELD:{I2C2Enable:2b}

      # !BSF NAME:{I2C Device 3} TYPE:{Combo} OPTION:{0: Disabled, 1: PCI Mode, 2: ACPI Mode}
      # !BSF HELP:{Enable/Disable I2C Device 3. 0:Disabled, 1:PCI Mode(Default), 2:ACPI Mode.}
      # !BSF FIELD:{I2C3Enable:2b}

      # !BSF NAME:{I2C Device 4} TYPE:{Combo} OPTION:{0: Disabled, 1: PCI Mode, 2: ACPI Mode}
      # !BSF HELP:{Enable/Disable I2C Device 4. 0:Disabled, 1:PCI Mode(Default), 2:ACPI Mode.}
      # !BSF FIELD:{I2C4Enable:2b}

      # !BSF NAME:{I2C Device 5} TYPE:{Combo} OPTION:{0: Disabled, 1: PCI Mode, 2: ACPI Mode}
      # !BSF HELP:{Enable/Disable I2C Device 5. 0:Disabled, 1:PCI Mode(Default), 2:ACPI Mode.}
      # !BSF FIELD:{I2C5Enable:2b}

      # !BSF NAME:{I2C Device 6} TYPE:{Combo} OPTION:{0: Disabled, 1: PCI Mode, 2: ACPI Mode}
      # !BSF HELP:{Enable/Disable I2C Device 6. 0:Disabled, 1:PCI Mode(Default), 2:ACPI Mode.}
      # !BSF FIELD:{I2C6Enable:2b}

      # !BSF NAME:{I2C Device 7} TYPE:{Combo} OPTION:{0: Disabled, 1: PCI Mode, 2: ACPI Mode}
      # !BSF HELP:{Enable/Disable I2C Device 7. 0:Disabled, 1:PCI Mode(Default), 2:ACPI Mode.}
      # !BSF FIELD:{I2C7Enable:2b}

      # !BSF NAME:{UART Device 0} TYPE:{Combo} OPTION:{0: Disabled, 1: PCI Mode, 2: ACPI Mode}
      # !BSF HELP:{Enable/Disable UART Device 0. 0:Disabled, 1:PCI Mode(Default), 2:ACPI Mode.}
      # !BSF FIELD:{Hsuart0Enable:2b}

      # !BSF NAME:{UART Device 1} TYPE:{Combo} OPTION:{0: Disabled, 1: PCI Mode, 2: ACPI Mode}
      # !BSF HELP:{Enable/Disable UART Device 1. 0:Disabled, 1:PCI Mode(Default), 2:ACPI Mode.}
      # !BSF FIELD:{Hsuart1Enable:2b}

      # !BSF NAME:{UART Device 2} TYPE:{Combo} OPTION:{0: Disabled, 1: PCI Mode, 2: ACPI Mode}
      # !BSF HELP:{Enable/Disable UART Device 2. 0:Disabled, 1:PCI Mode(Default), 2:ACPI Mode.}
      # !BSF FIELD:{Hsuart2Enable:2b}

      # !BSF NAME:{UART Device 3} TYPE:{Combo} OPTION:{0: Disabled, 1: PCI Mode, 2: ACPI Mode}
      # !BSF HELP:{Enable/Disable UART Device 3. 0:Disabled, 1:PCI Mode(Default), 2:ACPI Mode.}
      # !BSF FIELD:{Hsuart3Enable:2b}

      # !BSF NAME:{SPI UART Device 0} TYPE:{Combo} OPTION:{0: Disabled, 1: PCI Mode, 2: ACPI Mode}
      # !BSF HELP:{Enable/Disable SPI Device 0. 0:Disabled, 1:PCI Mode(Default), 2:ACPI Mode.}
      # !BSF FIELD:{Spi0Enable:2b}

      # !BSF NAME:{SPI UART Device 1} TYPE:{Combo} OPTION:{0: Disabled, 1: PCI Mode, 2: ACPI Mode}
      # !BSF HELP:{Enable/Disable SPI Device 1. 0:Disabled, 1:PCI Mode(Default), 2:ACPI Mode.}
      # !BSF FIELD:{Spi1Enable:2b}

      # !BSF NAME:{SPI UART Device 2} TYPE:{Combo} OPTION:{0: Disabled, 1: PCI Mode, 2: ACPI Mode}
      # !BSF HELP:{Enable/Disable SPI Device 2. 0:Disabled, 1:PCI Mode(Default), 2:ACPI Mode.}
      # !BSF FIELD:{Spi2Enable:2b}


    # !BSF PAGES:{CTRL1:DEV_EN_DIS:"Control 1"}
    # !BSF PAGE:{CTRL1}
    # !BSF NAME:{DEV ENABLE/DISABLE CTRL 1}
    # !BSF HELP:{Device Enable/Disable Control 1}
    # !HDR STRUCT:{DEV_EN_CTRL_1}
    gCfgData.DevEnControl1                  |      * | 0x02 | $(2)

      # !BSF NAME:{ISH Controller} TYPE:{Combo} OPTION:{$EN_DIS}
      # !BSF HELP:{Enable/Disable ISH Controller. 0:Disable, 1:Enable(Default).}
      # !BSF FIELD:{IshEnable:1bW}

      # !BSF NAME:{Chipset SATA} TYPE:{Combo} OPTION:{$EN_DIS}
      # !BSF HELP:{Enables or Disables the Chipset SATA Controller. The Chipset SATA controller supports the 2 black internal SATA ports (up to 3Gb/s supported per port). 0:Disable, 1:Enable(Default).}
      # !BSF FIELD:{SataEnable:1bW}

      # !BSF NAME:{SATA Port 0} TYPE:{Combo} OPTION:{$EN_DIS}
      # !BSF HELP:{Enable or Disable SATA Port 0.  0:Disable, 1:Enable(Default).}
      # !BSF FIELD:{SataPort0Enable:1bW}

      # !BSF NAME:{SATA Port 1} TYPE:{Combo} OPTION:{$EN_DIS}
      # !BSF HELP:{Enable or Disable SATA Port 1.  0:Disable, 1:Enable(Default).}
      # !BSF FIELD:{SataPort1Enable:1bW}

      # !BSF NAME:{SD Card Support (D27:F0)} TYPE:{Combo} OPTION:{$EN_DIS}
      # !BSF HELP:{Enable/Disable SD Card Support. 0:Disable, 1:Enable(Default).}
      # !BSF FIELD:{SdcardEnable:1bW}

      # !BSF NAME:{SeMMC Support (D28:F0)} TYPE:{Combo} OPTION:{$EN_DIS}
      # !BSF HELP:{Enable/Disable eMMC Support. 0:Disable, 1:Enable(Default).}
      # !BSF FIELD:{eMMCEnable:1bW}

      # !BSF NAME:{UFS Support (D29:F0)} TYPE:{Combo} OPTION:{$EN_DIS}
      # !BSF HELP:{Enable/Disable SDIO Support. 0:Disable, 1:Enable(Default).}
      # !BSF FIELD:{UfsEnable:1bW}

      # !BSF NAME:{SDIO Support (D30:F0)} TYPE:{Combo} OPTION:{$EN_DIS}
      # !BSF HELP:{Enable/Disable SDIO Support. 0:Disable, 1:Enable(Default).}
      # !BSF FIELD:{SdioEnable:1bW}

      # !BSF NAME:{Enable SMBus} TYPE:{Combo} OPTION:{$EN_DIS}
      # !BSF HELP:{Enable/disable SMBus controller. 0:Disable, 1:Enable(Default).}
      # !BSF FIELD:{SmbusEnable:1bW}

      # !BSF NAME:{IPU Enable/Disable} TYPE:{Combo} OPTION:{$EN_DIS}
      # !BSF HELP:{Enable/Disable IPU Device. 0:Disable, 1:Enable(Default).}
      # !BSF FIELD:{IpuEnable:1bW}

      # !BSF NAME:{Reserved} TYPE:{Reserved}
      # !BSF HELP:{Control 1 reserved}
      # !BSF FIELD:{Rsvd:6bW}


    # !BSF PAGES:{CTRL2:DEV_EN_DIS:"Control 2"}
    # !BSF PAGE:{CTRL2}
    # !BSF NAME:{DEV ENABLE/DISABLE CTRL 2}
    # !BSF HELP:{Device Enable/Disable Control 2}
    # !HDR STRUCT:{DEV_EN_CTRL_2}
    gCfgData.DevEnControl2                  |      * | 0x02 | $(3)

      # !BSF NAME:{Enable USB2 port 0} TYPE:{Combo} OPTION:{0: Disable, 1: Enable}
      # !BSF HELP:{Enable/disable USB2 port 0. 0x01(Default).}
      # !BSF FIELD:{Usb20Port0Enable:1bW}

      # !BSF NAME:{Enable USB2 port 1} TYPE:{Combo} OPTION:{0: Disable, 1: Enable}
      # !BSF HELP:{Enable/disable USB2 port 1. 0x01(Default).}
      # !BSF FIELD:{Usb20Port1Enable:1bW}

      # !BSF NAME:{Enable USB2 port 2} TYPE:{Combo} OPTION:{0: Disable, 1: Enable}
      # !BSF HELP:{Enable/disable USB2 port 2. 0x01(Default).}
      # !BSF FIELD:{Usb20Port2Enable:1bW}

      # !BSF NAME:{Enable USB2 port 3} TYPE:{Combo} OPTION:{0: Disable, 1: Enable}
      # !BSF HELP:{Enable/disable USB2 port 3. 0x01(Default).}
      # !BSF FIELD:{Usb20Port3Enable:1bW}

      # !BSF NAME:{Enable USB2 port 4} TYPE:{Combo} OPTION:{0: Disable, 1: Enable}
      # !BSF HELP:{Enable/disable USB2 port 4. 0x01(Default).}
      # !BSF FIELD:{Usb20Port4Enable:1bW}

      # !BSF NAME:{Enable USB2 port 5} TYPE:{Combo} OPTION:{0: Disable, 1: Enable}
      # !BSF HELP:{Enable/disable USB2 port 5. 0x01(Default).}
      # !BSF FIELD:{Usb20Port5Enable:1bW}

      # !BSF NAME:{Enable USB2 port 6} TYPE:{Combo} OPTION:{0: Disable, 1: Enable}
      # !BSF HELP:{Enable/disable USB2 port 6. 0x01(Default).}
      # !BSF FIELD:{Usb20Port6Enable:1bW}

      # !BSF NAME:{Enable USB2 port 7} TYPE:{Combo} OPTION:{0: Disable, 1: Enable}
      # !BSF HELP:{Enable/disable USB2 port 7. 0x01(Default).}
      # !BSF FIELD:{Usb20Port7Enable:1bW}

      # !BSF NAME:{Enable USB3 port 0} TYPE:{Combo} OPTION:{0: Disable, 1: Enable}
      # !BSF HELP:{Enable/disable USB3 port 0. 0x01(Default).}
      # !BSF FIELD:{Usb30Port0Enable:1bW}

      # !BSF NAME:{Enable USB3 port 1} TYPE:{Combo} OPTION:{0: Disable, 1: Enable}
      # !BSF HELP:{Enable/disable USB3 port 1. 0x01(Default).}
      # !BSF FIELD:{Usb30Port1Enable:1bW}

      # !BSF NAME:{Enable USB3 port 2} TYPE:{Combo} OPTION:{0: Disable, 1: Enable}
      # !BSF HELP:{Enable/disable USB3 port 2. 0x01(Default).}
      # !BSF FIELD:{Usb30Port2Enable:1bW}

      # !BSF NAME:{Enable USB3 port 3} TYPE:{Combo} OPTION:{0: Disable, 1: Enable}
      # !BSF HELP:{Enable/disable USB3 port 3. 0x01(Default).}
      # !BSF FIELD:{Usb30Port3Enable:1bW}

      # !BSF NAME:{Enable USB3 port 4} TYPE:{Combo} OPTION:{0: Disable, 1: Enable}
      # !BSF HELP:{Enable/disable USB3 port 4. 0x01(Default).}
      # !BSF FIELD:{Usb30Port4Enable:1bW}

      # !BSF NAME:{Enable USB3 port 5} TYPE:{Combo} OPTION:{0: Disable, 1: Enable}
      # !BSF HELP:{Enable/disable USB3 port 5. 0x01(Default).}
      # !BSF FIELD:{Usb30Port5Enable:1bW}

      # !BSF NAME:{Reserved} TYPE:{Reserved}
      # !BSF HELP:{Control 2 reserved}
      # !BSF FIELD:{Rsvd:2b}

  # !BSF DEFT:{DEV_EN_TMPL:END}
