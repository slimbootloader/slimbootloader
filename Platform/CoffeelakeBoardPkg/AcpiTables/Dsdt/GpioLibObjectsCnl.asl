/**@file

  Copyright (c) 2018 - 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#include "Register/PchRegsGpio.h"
#include "Register/PchRegsGpioCnl.h"

//
// CNL-PCH GPIO Community address
//
#define PCH_GPIO_COM0 0x006e0000 // PID_GPIOCOM0 = 0x6E
#define PCH_GPIO_COM1 0x006d0000 // PID_GPIOCOM1 = 0x6D
#define PCH_GPIO_COM2 0x006c0000 // PID_GPIOCOM2 = 0x6C
#define PCH_GPIO_COM3 0x006b0000 // PID_GPIOCOM3 = 0x6B
#define PCH_GPIO_COM4 0x006a0000 // PID_GPIOCOM4 = 0x6A

//
// If in GPIO_GROUP_INFO structure certain register doesn't exist
// it will have value equal to NO_REGISTER_FOR_PROPERTY
//
#define NO_REGISTER_FOR_PROPERTY 0xFFFF

//
// GPIO Library objects
//
Scope(\_SB)
{
  //
  // GPIO information data structure
  //
  // PCH-LP: GPCL
  // PCH-H:  GPCH
  //
  // GPCx[GroupIndex][Field]
  //   Field0 - Community
  //   Field1 - Pad number
  //   Field2 - PADCFG register offset
  //   Field3 - HOSTSW_OWN register offset
  //   Field4 - PAD_OWN register offset
  //   Field5 - GPI_GPE_STS register offset
  //   Field6 - Gpio base number for GPIO OS Driver usage
  //            GpioIo and GpioInt number is created in following way:
  //              GpioAcpiPinNumber = GpioHwBankIndex * 32 + GpioPadNumber
  //            where GPIO OS Driver banks are:
  //            PCH-LP:
  //              GpioHwBank[] ={"GPP_A","GPP_B","GPP_G","GPP_D","GPP_F","GPP_H","vGPIO_0","vGPIO_1","GPP_C","GPP_E"}
  //            PCH-H:
  //              GpioHwBank[] ={"GPP_A","GPP_B","GPP_C","GPP_D","GPP_G","vGPIO_0","GPP_K","GPP_H","GPP_E","GPP_F","GPP_I","GPP_J"}
  //
  // GPIO information data structure for PCH-LP
  //
  Name(GPCL, Package(){
    Package(){ // GPP_A
      PCH_GPIO_COM0,
      CNL_PCH_LP_GPIO_GPP_A_PAD_MAX,
      R_CNL_PCH_LP_GPIO_PCR_GPP_A_PADCFG_OFFSET,
      R_CNL_PCH_LP_GPIO_PCR_GPP_A_HOSTSW_OWN,
      R_CNL_PCH_LP_GPIO_PCR_GPP_A_PAD_OWN,
      R_CNL_PCH_LP_GPIO_PCR_GPP_A_GPI_GPE_STS,
      0
    },
    Package(){ // GPP_B
      PCH_GPIO_COM0,
      CNL_PCH_LP_GPIO_GPP_B_PAD_MAX,
      R_CNL_PCH_LP_GPIO_PCR_GPP_B_PADCFG_OFFSET,
      R_CNL_PCH_LP_GPIO_PCR_GPP_B_HOSTSW_OWN,
      R_CNL_PCH_LP_GPIO_PCR_GPP_B_PAD_OWN,
      R_CNL_PCH_LP_GPIO_PCR_GPP_B_GPI_GPE_STS,
      32
    },
    Package(){ // GPP_C
      PCH_GPIO_COM4,
      CNL_PCH_LP_GPIO_GPP_C_PAD_MAX,
      R_CNL_PCH_LP_GPIO_PCR_GPP_C_PADCFG_OFFSET,
      R_CNL_PCH_LP_GPIO_PCR_GPP_C_HOSTSW_OWN,
      R_CNL_PCH_LP_GPIO_PCR_GPP_C_PAD_OWN,
      R_CNL_PCH_LP_GPIO_PCR_GPP_C_GPI_GPE_STS,
      256
    },
    Package(){ // GPP_D
      PCH_GPIO_COM1,
      CNL_PCH_LP_GPIO_GPP_D_PAD_MAX,
      R_CNL_PCH_LP_GPIO_PCR_GPP_D_PADCFG_OFFSET,
      R_CNL_PCH_LP_GPIO_PCR_GPP_D_HOSTSW_OWN,
      R_CNL_PCH_LP_GPIO_PCR_GPP_D_PAD_OWN,
      R_CNL_PCH_LP_GPIO_PCR_GPP_D_GPI_GPE_STS,
      96
    },
    Package(){ // GPP_E
      PCH_GPIO_COM4,
      CNL_PCH_LP_GPIO_GPP_E_PAD_MAX,
      R_CNL_PCH_LP_GPIO_PCR_GPP_E_PADCFG_OFFSET,
      R_CNL_PCH_LP_GPIO_PCR_GPP_E_HOSTSW_OWN,
      R_CNL_PCH_LP_GPIO_PCR_GPP_E_PAD_OWN,
      R_CNL_PCH_LP_GPIO_PCR_GPP_E_GPI_GPE_STS,
      288,
    },
    Package(){ // GPP_F
      PCH_GPIO_COM1,
      CNL_PCH_LP_GPIO_GPP_F_PAD_MAX,
      R_CNL_PCH_LP_GPIO_PCR_GPP_F_PADCFG_OFFSET,
      R_CNL_PCH_LP_GPIO_PCR_GPP_F_HOSTSW_OWN,
      R_CNL_PCH_LP_GPIO_PCR_GPP_F_PAD_OWN,
      R_CNL_PCH_LP_GPIO_PCR_GPP_F_GPI_GPE_STS,
      128
    },
    Package(){ // GPP_G
      PCH_GPIO_COM0,
      CNL_PCH_LP_GPIO_GPP_G_PAD_MAX,
      R_CNL_PCH_LP_GPIO_PCR_GPP_G_PADCFG_OFFSET,
      R_CNL_PCH_LP_GPIO_PCR_GPP_G_HOSTSW_OWN,
      R_CNL_PCH_LP_GPIO_PCR_GPP_G_PAD_OWN,
      R_CNL_PCH_LP_GPIO_PCR_GPP_G_GPI_GPE_STS,
      64
    },
    Package(){ // GPP_H
      PCH_GPIO_COM1,
      CNL_PCH_LP_GPIO_GPP_H_PAD_MAX,
      R_CNL_PCH_LP_GPIO_PCR_GPP_H_PADCFG_OFFSET,
      R_CNL_PCH_LP_GPIO_PCR_GPP_H_HOSTSW_OWN,
      R_CNL_PCH_LP_GPIO_PCR_GPP_H_PAD_OWN,
      R_CNL_PCH_LP_GPIO_PCR_GPP_H_GPI_GPE_STS,
      160
    },
    Package(){ // GPD
      PCH_GPIO_COM2,
      CNL_PCH_LP_GPIO_GPD_PAD_MAX,
      R_CNL_PCH_LP_GPIO_PCR_GPD_PADCFG_OFFSET,
      R_CNL_PCH_LP_GPIO_PCR_GPD_HOSTSW_OWN,
      R_CNL_PCH_LP_GPIO_PCR_GPD_PAD_OWN,
      R_CNL_PCH_LP_GPIO_PCR_GPD_GPI_GPE_STS,
      0xFFFF, // Group not supported by GPIO OS Driver
    },
    Package(){ // VGPIO
      PCH_GPIO_COM1,
      CNL_PCH_LP_GPIO_VGPIO_PAD_MAX,
      R_CNL_PCH_LP_GPIO_PCR_VGPIO_PADCFG_OFFSET,
      R_CNL_PCH_LP_GPIO_PCR_VGPIO_HOSTSW_OWN,
      R_CNL_PCH_LP_GPIO_PCR_VGPIO_PAD_OWN,
      R_CNL_PCH_LP_GPIO_PCR_VGPIO_GPI_GPE_STS,
      192
    },
    Package(){ // SPI
      PCH_GPIO_COM0,
      CNL_PCH_LP_GPIO_SPI_PAD_MAX,
      R_CNL_PCH_LP_GPIO_PCR_SPI_PADCFG_OFFSET,
      R_CNL_PCH_LP_GPIO_PCR_SPI_HOSTSW_OWN,
      R_CNL_PCH_LP_GPIO_PCR_SPI_PAD_OWN,
      R_CNL_PCH_LP_GPIO_PCR_SPI_GPI_GPE_STS,
      0xFFFF, // Group not supported by GPIO OS Driver
    },
    Package(){ // AZA
      PCH_GPIO_COM3,
      CNL_PCH_LP_GPIO_AZA_PAD_MAX,
      R_CNL_PCH_LP_GPIO_PCR_AZA_PADCFG_OFFSET,
      R_CNL_PCH_LP_GPIO_PCR_AZA_HOSTSW_OWN,
      R_CNL_PCH_LP_GPIO_PCR_AZA_PAD_OWN,
      R_CNL_PCH_LP_GPIO_PCR_AZA_GPI_GPE_STS,
      0xFFFF, // Group not supported by GPIO OS Driver
    },
    Package(){ // CPU
      PCH_GPIO_COM3,
      CNL_PCH_LP_GPIO_CPU_PAD_MAX,
      R_CNL_PCH_LP_GPIO_PCR_CPU_PADCFG_OFFSET,
      R_CNL_PCH_LP_GPIO_PCR_CPU_HOSTSW_OWN,
      R_CNL_PCH_LP_GPIO_PCR_CPU_PAD_OWN,
      NO_REGISTER_FOR_PROPERTY,
      0xFFFF, // Group not supported by GPIO OS Driver
    },
    Package(){ // JTAG
      PCH_GPIO_COM4,
      CNL_PCH_LP_GPIO_JTAG_PAD_MAX,
      R_CNL_PCH_LP_GPIO_PCR_JTAG_PADCFG_OFFSET,
      R_CNL_PCH_LP_GPIO_PCR_JTAG_HOSTSW_OWN,
      R_CNL_PCH_LP_GPIO_PCR_JTAG_PAD_OWN,
      NO_REGISTER_FOR_PROPERTY,
      0xFFFF, // Group not supported by GPIO OS Driver
    },
    Package(){ // HVMOS
      PCH_GPIO_COM4,
      CNL_PCH_LP_GPIO_HVMOS_PAD_MAX,
      R_CNL_PCH_LP_GPIO_PCR_HVMOS_PADCFG_OFFSET,
      R_CNL_PCH_LP_GPIO_PCR_HVMOS_HOSTSW_OWN,
      R_CNL_PCH_LP_GPIO_PCR_HVMOS_PAD_OWN,
      R_CNL_PCH_LP_GPIO_PCR_HVMOS_GPI_GPE_STS,
      0xFFFF, // Group not supported by GPIO OS Driver
    }
  })

  //
  // GPIO information data structure for PCH-H
  //
  Name(GPCH, Package(){
    Package(){ // GPP_A
      PCH_GPIO_COM0,
      CNL_PCH_H_GPIO_GPP_A_PAD_MAX,
      R_CNL_PCH_H_GPIO_PCR_GPP_A_PADCFG_OFFSET,
      R_CNL_PCH_H_GPIO_PCR_GPP_A_HOSTSW_OWN,
      R_CNL_PCH_H_GPIO_PCR_GPP_A_PAD_OWN,
      R_CNL_PCH_H_GPIO_PCR_GPP_A_GPI_GPE_STS,
      0
    },
    Package(){ // GPP_B
      PCH_GPIO_COM0,
      CNL_PCH_H_GPIO_GPP_B_PAD_MAX,
      R_CNL_PCH_H_GPIO_PCR_GPP_B_PADCFG_OFFSET,
      R_CNL_PCH_H_GPIO_PCR_GPP_B_HOSTSW_OWN,
      R_CNL_PCH_H_GPIO_PCR_GPP_B_PAD_OWN,
      R_CNL_PCH_H_GPIO_PCR_GPP_B_GPI_GPE_STS,
      32
    },
    Package(){ // GPP_C
      PCH_GPIO_COM1,
      CNL_PCH_H_GPIO_GPP_C_PAD_MAX,
      R_CNL_PCH_H_GPIO_PCR_GPP_C_PADCFG_OFFSET,
      R_CNL_PCH_H_GPIO_PCR_GPP_C_HOSTSW_OWN,
      R_CNL_PCH_H_GPIO_PCR_GPP_C_PAD_OWN,
      R_CNL_PCH_H_GPIO_PCR_GPP_C_GPI_GPE_STS,
      64
    },
    Package(){ // GPP_D
      PCH_GPIO_COM1,
      CNL_PCH_H_GPIO_GPP_D_PAD_MAX,
      R_CNL_PCH_H_GPIO_PCR_GPP_D_PADCFG_OFFSET,
      R_CNL_PCH_H_GPIO_PCR_GPP_D_HOSTSW_OWN,
      R_CNL_PCH_H_GPIO_PCR_GPP_D_PAD_OWN,
      R_CNL_PCH_H_GPIO_PCR_GPP_D_GPI_GPE_STS,
      96
    },
    Package(){ // GPP_E
      PCH_GPIO_COM3,
      CNL_PCH_H_GPIO_GPP_E_PAD_MAX,
      R_CNL_PCH_H_GPIO_PCR_GPP_E_PADCFG_OFFSET,
      R_CNL_PCH_H_GPIO_PCR_GPP_E_HOSTSW_OWN,
      R_CNL_PCH_H_GPIO_PCR_GPP_E_PAD_OWN,
      R_CNL_PCH_H_GPIO_PCR_GPP_E_GPI_GPE_STS,
      256
    },
    Package(){ // GPP_F
      PCH_GPIO_COM3,
      CNL_PCH_H_GPIO_GPP_F_PAD_MAX,
      R_CNL_PCH_H_GPIO_PCR_GPP_F_PADCFG_OFFSET,
      R_CNL_PCH_H_GPIO_PCR_GPP_F_HOSTSW_OWN,
      R_CNL_PCH_H_GPIO_PCR_GPP_F_PAD_OWN,
      R_CNL_PCH_H_GPIO_PCR_GPP_F_GPI_GPE_STS,
      288
    },
    Package(){ // GPP_G
      PCH_GPIO_COM1,
      CNL_PCH_H_GPIO_GPP_G_PAD_MAX,
      R_CNL_PCH_H_GPIO_PCR_GPP_G_PADCFG_OFFSET,
      R_CNL_PCH_H_GPIO_PCR_GPP_G_HOSTSW_OWN,
      R_CNL_PCH_H_GPIO_PCR_GPP_G_PAD_OWN,
      R_CNL_PCH_H_GPIO_PCR_GPP_G_GPI_GPE_STS,
      128
    },
    Package(){ // GPP_H
      PCH_GPIO_COM3,
      CNL_PCH_H_GPIO_GPP_H_PAD_MAX,
      R_CNL_PCH_H_GPIO_PCR_GPP_H_PADCFG_OFFSET,
      R_CNL_PCH_H_GPIO_PCR_GPP_H_HOSTSW_OWN,
      R_CNL_PCH_H_GPIO_PCR_GPP_H_PAD_OWN,
      R_CNL_PCH_H_GPIO_PCR_GPP_H_GPI_GPE_STS,
      224
    },
    Package(){ // GPP_I
      PCH_GPIO_COM4,
      CNL_PCH_H_GPIO_GPP_I_PAD_MAX,
      R_CNL_PCH_H_GPIO_PCR_GPP_I_PADCFG_OFFSET,
      R_CNL_PCH_H_GPIO_PCR_GPP_I_HOSTSW_OWN,
      R_CNL_PCH_H_GPIO_PCR_GPP_I_PAD_OWN,
      R_CNL_PCH_H_GPIO_PCR_GPP_I_GPI_GPE_STS,
      320
    },
    Package(){ // GPP_J
      PCH_GPIO_COM4,
      CNL_PCH_H_GPIO_GPP_J_PAD_MAX,
      R_CNL_PCH_H_GPIO_PCR_GPP_J_PADCFG_OFFSET,
      R_CNL_PCH_H_GPIO_PCR_GPP_J_HOSTSW_OWN,
      R_CNL_PCH_H_GPIO_PCR_GPP_J_PAD_OWN,
      R_CNL_PCH_H_GPIO_PCR_GPP_J_GPI_GPE_STS,
      352,
    },
    Package(){ // GPP_K
      PCH_GPIO_COM3,
      CNL_PCH_H_GPIO_GPP_K_PAD_MAX,
      R_CNL_PCH_H_GPIO_PCR_GPP_K_PADCFG_OFFSET,
      R_CNL_PCH_H_GPIO_PCR_GPP_K_HOSTSW_OWN,
      R_CNL_PCH_H_GPIO_PCR_GPP_K_PAD_OWN,
      R_CNL_PCH_H_GPIO_PCR_GPP_K_GPI_GPE_STS,
      192,
    },
    Package(){ // GPD
      PCH_GPIO_COM2,
      CNL_PCH_H_GPIO_GPD_PAD_MAX,
      R_CNL_PCH_H_GPIO_PCR_GPD_PADCFG_OFFSET,
      R_CNL_PCH_H_GPIO_PCR_GPD_HOSTSW_OWN,
      R_CNL_PCH_H_GPIO_PCR_GPD_PAD_OWN,
      R_CNL_PCH_H_GPIO_PCR_GPD_GPI_GPE_STS,
      0xFFFF, // Group not supported by GPIO OS Driver
    },
    Package(){ // VGPIO
      PCH_GPIO_COM1,
      CNL_PCH_H_GPIO_VGPIO_PAD_MAX,
      R_CNL_PCH_H_GPIO_PCR_VGPIO_PADCFG_OFFSET,
      R_CNL_PCH_H_GPIO_PCR_VGPIO_HOSTSW_OWN,
      R_CNL_PCH_H_GPIO_PCR_VGPIO_PAD_OWN,
      NO_REGISTER_FOR_PROPERTY,
      160,
    },
    Package(){ // SPI
      PCH_GPIO_COM3,
      CNL_PCH_H_GPIO_SPI_PAD_MAX,
      R_CNL_PCH_H_GPIO_PCR_SPI_PADCFG_OFFSET,
      R_CNL_PCH_H_GPIO_PCR_SPI_HOSTSW_OWN,
      R_CNL_PCH_H_GPIO_PCR_SPI_PAD_OWN,
      NO_REGISTER_FOR_PROPERTY,
      0xFFFF, // Group not supported by GPIO OS Driver
    },
    Package(){ // AZA
      PCH_GPIO_COM1,
      CNL_PCH_H_GPIO_AZA_PAD_MAX,
      R_CNL_PCH_H_GPIO_PCR_AZA_PADCFG_OFFSET,
      R_CNL_PCH_H_GPIO_PCR_AZA_HOSTSW_OWN,
      R_CNL_PCH_H_GPIO_PCR_AZA_PAD_OWN,
      NO_REGISTER_FOR_PROPERTY,
      0xFFFF, // Group not supported by GPIO OS Driver
    },
    Package(){ // CPU
      PCH_GPIO_COM4,
      CNL_PCH_H_GPIO_CPU_PAD_MAX,
      R_CNL_PCH_H_GPIO_PCR_CPU_PADCFG_OFFSET,
      R_CNL_PCH_H_GPIO_PCR_CPU_HOSTSW_OWN,
      R_CNL_PCH_H_GPIO_PCR_CPU_PAD_OWN,
      NO_REGISTER_FOR_PROPERTY,
      0xFFFF, // Group not supported by GPIO OS Driver
    },
    Package(){ // JTAG
      PCH_GPIO_COM4,
      CNL_PCH_H_GPIO_JTAG_PAD_MAX,
      R_CNL_PCH_H_GPIO_PCR_JTAG_PADCFG_OFFSET,
      R_CNL_PCH_H_GPIO_PCR_JTAG_HOSTSW_OWN,
      R_CNL_PCH_H_GPIO_PCR_JTAG_PAD_OWN,
      NO_REGISTER_FOR_PROPERTY,
      0xFFFF, // Group not supported by GPIO OS Driver
    }
  })

  //
  // Object for storing RX Level/Edge Configuration for all pads.
  // One fields contains data for one pad.
  //   00b = Level
  //   01b = Edge (RxInv=0 for rising edge; 1 for falling edge)
  //   10b = Disabled
  //   11b = Either rising edge or falling edge
  //
  // Each row has data for one group. Buffer size needs
  // to equal to number of pads
  Name(RXEV, Package(){
    Buffer(CNL_PCH_H_GPIO_GPP_A_PAD_MAX){}, // GPP_A
    Buffer(CNL_PCH_H_GPIO_GPP_B_PAD_MAX){}, // GPP_B
    Buffer(CNL_PCH_H_GPIO_GPP_C_PAD_MAX){}, // GPP_C
    Buffer(CNL_PCH_H_GPIO_GPP_D_PAD_MAX){}, // GPP_D
    Buffer(CNL_PCH_LP_GPIO_GPP_E_PAD_MAX){},// GPP_E
    Buffer(CNL_PCH_H_GPIO_GPP_F_PAD_MAX){}, // GPP_F
    Buffer(CNL_PCH_H_GPIO_GPP_G_PAD_MAX){}, // GPP_G
    Buffer(CNL_PCH_H_GPIO_GPP_H_PAD_MAX){}, // GPP_H
    Buffer(CNL_PCH_H_GPIO_GPP_I_PAD_MAX){}, // PCH-H GPP_I, PCH-LP GPD
    Buffer(CNL_PCH_LP_GPIO_VGPIO_PAD_MAX){},// PCH-H GPP_J, PCH-LP VGPIO
    Buffer(CNL_PCH_H_GPIO_GPP_K_PAD_MAX){}, // PCH-H GPP_K, PCH-LP not used
    Buffer(CNL_PCH_H_GPIO_GPD_PAD_MAX){},   // PCH-H GPD,   PCH-LP not used
    Buffer(CNL_PCH_H_GPIO_VGPIO_PAD_MAX){}  // PCH-H VGPIO, PCH-LP not used
  })
}
