/**@file
 VER3 GPIO data for use with GPIO ASL lib

 Copyright (c) 2018 - 2019, Intel Corporation. All rights reserved.<BR>
 SPDX-License-Identifier: BSD-2-Clause-Patent
**/
#include "Register/GpioRegs.h"
#include "Register/GpioRegsVer3.h"

//
// VER3-PCH GPIO Community address
//
#define PCH_GPIO_COM0 0x006e0000 // PID_GPIOCOM0 = 0x6E
#define PCH_GPIO_COM1 0x006d0000 // PID_GPIOCOM1 = 0x6D
#define PCH_GPIO_COM2 0x006c0000 // PID_GPIOCOM2 = 0x6C
#define PCH_GPIO_COM3 0x006b0000 // PID_GPIOCOM3 = 0x6B
#define PCH_GPIO_COM4 0x006a0000 // PID_GPIOCOM4 = 0x6A
#define PCH_GPIO_COM5 0x00690000 // PID_GPIOCOM5 = 0x69

//
// If in GPIO_GROUP_INFO structure certain register doesn't exist
// it will have value equal to NO_REGISTER_FOR_PROPERTY
//
#define NO_REGISTER_FOR_PROPERTY 0xFFFF

//
// If in GPIO_GROUP_INFO structure certain group should not be used
// by GPIO OS driver then "Gpio base number" field should be set to below value
//
#define GPIO_OS_DRV_NOT_SUPPORTED 0xFFFF

//
// GPIO Library objects
//
Scope(\_SB)
{
  //
  // GPIO information data structure
  //
  // PCH-LP: GPCL
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
  //            where GPIO OS Driver banks are (for PCH-LP):
  //              GpioHwBank[] ={"GPP_B","GPP_T","GPP_G","GPP_R","GPP_V","GPP_H","GPP_D","GPP_U","vGPIO","GPP_C","GPP_F","GPP_E","GPP_S","GPP_A"}
  //
  //
  // GPIO information data structure for PCH-LP
  //
  Name(GPCL, Package(){
    Package(){ // GPP_B
      PCH_GPIO_COM0,
      GPIO_VER3_PCH_GPIO_GPP_B_PAD_MAX,
      R_GPIO_VER3_PCH_GPIO_PCR_GPP_B_PADCFG_OFFSET,
      R_GPIO_VER3_PCH_GPIO_PCR_GPP_B_HOSTSW_OWN,
      R_GPIO_VER3_PCH_GPIO_PCR_GPP_B_PAD_OWN,
      R_GPIO_VER3_PCH_GPIO_PCR_GPP_B_GPI_GPE_STS,
      0
    },
    Package(){ // GPP_T
      PCH_GPIO_COM0,
      GPIO_VER3_PCH_GPIO_GPP_T_PAD_MAX,
      R_GPIO_VER3_PCH_GPIO_PCR_GPP_T_PADCFG_OFFSET,
      R_GPIO_VER3_PCH_GPIO_PCR_GPP_T_HOSTSW_OWN,
      R_GPIO_VER3_PCH_GPIO_PCR_GPP_T_PAD_OWN,
      R_GPIO_VER3_PCH_GPIO_PCR_GPP_T_GPI_GPE_STS,
      32
    },
    Package(){ // GPP_G
      PCH_GPIO_COM0,
      GPIO_VER3_PCH_GPIO_GPP_G_PAD_MAX,
      R_GPIO_VER3_PCH_GPIO_PCR_GPP_G_PADCFG_OFFSET,
      R_GPIO_VER3_PCH_GPIO_PCR_GPP_G_HOSTSW_OWN,
      R_GPIO_VER3_PCH_GPIO_PCR_GPP_G_PAD_OWN,
      R_GPIO_VER3_PCH_GPIO_PCR_GPP_G_GPI_GPE_STS,
      64
    },
    Package(){ // GPP_V
      PCH_GPIO_COM1,
      GPIO_VER3_PCH_GPIO_GPP_V_PAD_MAX,
      R_GPIO_VER3_PCH_GPIO_PCR_GPP_V_PADCFG_OFFSET,
      R_GPIO_VER3_PCH_GPIO_PCR_GPP_V_HOSTSW_OWN,
      R_GPIO_VER3_PCH_GPIO_PCR_GPP_V_PAD_OWN,
      R_GPIO_VER3_PCH_GPIO_PCR_GPP_V_GPI_GPE_STS,
      96
    },
    Package(){ // GPP_H
      PCH_GPIO_COM1,
      GPIO_VER3_PCH_GPIO_GPP_H_PAD_MAX,
      R_GPIO_VER3_PCH_GPIO_PCR_GPP_H_PADCFG_OFFSET,
      R_GPIO_VER3_PCH_GPIO_PCR_GPP_H_HOSTSW_OWN,
      R_GPIO_VER3_PCH_GPIO_PCR_GPP_H_PAD_OWN,
      R_GPIO_VER3_PCH_GPIO_PCR_GPP_H_GPI_GPE_STS,
      128
    },
    Package(){ // GPP_D
      PCH_GPIO_COM1,
      GPIO_VER3_PCH_GPIO_GPP_D_PAD_MAX,
      R_GPIO_VER3_PCH_GPIO_PCR_GPP_D_PADCFG_OFFSET,
      R_GPIO_VER3_PCH_GPIO_PCR_GPP_D_HOSTSW_OWN,
      R_GPIO_VER3_PCH_GPIO_PCR_GPP_D_PAD_OWN,
      R_GPIO_VER3_PCH_GPIO_PCR_GPP_D_GPI_GPE_STS,
      160
    },
    Package(){ // GPP_U
      PCH_GPIO_COM1,
      GPIO_VER3_PCH_GPIO_GPP_U_PAD_MAX,
      R_GPIO_VER3_PCH_GPIO_PCR_GPP_U_PADCFG_OFFSET,
      R_GPIO_VER3_PCH_GPIO_PCR_GPP_U_HOSTSW_OWN,
      R_GPIO_VER3_PCH_GPIO_PCR_GPP_U_PAD_OWN,
      R_GPIO_VER3_PCH_GPIO_PCR_GPP_U_GPI_GPE_STS,
      192
    },
    Package(){ // VGPIO
      PCH_GPIO_COM1,
      GPIO_VER3_PCH_GPIO_VGPIO_PAD_MAX,
      R_GPIO_VER3_PCH_GPIO_PCR_VGPIO_PADCFG_OFFSET,
      R_GPIO_VER3_PCH_GPIO_PCR_VGPIO_HOSTSW_OWN,
      R_GPIO_VER3_PCH_GPIO_PCR_VGPIO_PAD_OWN,
      R_GPIO_VER3_PCH_GPIO_PCR_VGPIO_GPI_GPE_STS,
      224
    },
    Package(){ // GPD
      PCH_GPIO_COM2,
      GPIO_VER3_PCH_GPIO_GPD_PAD_MAX,
      R_GPIO_VER3_PCH_GPIO_PCR_GPD_PADCFG_OFFSET,
      R_GPIO_VER3_PCH_GPIO_PCR_GPD_HOSTSW_OWN,
      R_GPIO_VER3_PCH_GPIO_PCR_GPD_PAD_OWN,
      R_GPIO_VER3_PCH_GPIO_PCR_GPD_GPI_GPE_STS,
      GPIO_OS_DRV_NOT_SUPPORTED
    },
    Package(){ // CPU
      PCH_GPIO_COM3,
      GPIO_VER3_PCH_GPIO_CPU_PAD_MAX,
      R_GPIO_VER3_PCH_GPIO_PCR_CPU_PADCFG_OFFSET,
      R_GPIO_VER3_PCH_GPIO_PCR_CPU_HOSTSW_OWN,
      R_GPIO_VER3_PCH_GPIO_PCR_CPU_PAD_OWN,
      NO_REGISTER_FOR_PROPERTY,
      GPIO_OS_DRV_NOT_SUPPORTED
    },
    Package(){ // GPP_S
      PCH_GPIO_COM3,
      GPIO_VER3_PCH_GPIO_GPP_S_PAD_MAX,
      R_GPIO_VER3_PCH_GPIO_PCR_GPP_S_PADCFG_OFFSET,
      R_GPIO_VER3_PCH_GPIO_PCR_GPP_S_HOSTSW_OWN,
      R_GPIO_VER3_PCH_GPIO_PCR_GPP_S_PAD_OWN,
      NO_REGISTER_FOR_PROPERTY,
      256
    },
    Package(){ // GPP_A
      PCH_GPIO_COM3,
      GPIO_VER3_PCH_GPIO_GPP_A_PAD_MAX,
      R_GPIO_VER3_PCH_GPIO_PCR_GPP_A_PADCFG_OFFSET,
      R_GPIO_VER3_PCH_GPIO_PCR_GPP_A_HOSTSW_OWN,
      R_GPIO_VER3_PCH_GPIO_PCR_GPP_A_PAD_OWN,
      R_GPIO_VER3_PCH_GPIO_PCR_GPP_A_GPI_GPE_STS,
      288
    },
    Package(){ // VGPIO_3
      PCH_GPIO_COM3,
      GPIO_VER3_PCH_GPIO_VGPIO_3_PAD_MAX,
      R_GPIO_VER3_PCH_GPIO_PCR_VGPIO_3_PADCFG_OFFSET,
      R_GPIO_VER3_PCH_GPIO_PCR_VGPIO_3_HOSTSW_OWN,
      R_GPIO_VER3_PCH_GPIO_PCR_VGPIO_3_PAD_OWN,
      NO_REGISTER_FOR_PROPERTY,
      320
    },
    Package(){ // GPP_C
      PCH_GPIO_COM4,
      GPIO_VER3_PCH_GPIO_GPP_C_PAD_MAX,
      R_GPIO_VER3_PCH_GPIO_PCR_GPP_C_PADCFG_OFFSET,
      R_GPIO_VER3_PCH_GPIO_PCR_GPP_C_HOSTSW_OWN,
      R_GPIO_VER3_PCH_GPIO_PCR_GPP_C_PAD_OWN,
      R_GPIO_VER3_PCH_GPIO_PCR_GPP_C_GPI_GPE_STS,
      352
    },
    Package(){ // GPP_F
      PCH_GPIO_COM4,
      GPIO_VER3_PCH_GPIO_GPP_F_PAD_MAX,
      R_GPIO_VER3_PCH_GPIO_PCR_GPP_F_PADCFG_OFFSET,
      R_GPIO_VER3_PCH_GPIO_PCR_GPP_F_HOSTSW_OWN,
      R_GPIO_VER3_PCH_GPIO_PCR_GPP_F_PAD_OWN,
      R_GPIO_VER3_PCH_GPIO_PCR_GPP_F_GPI_GPE_STS,
      384
    },
    Package(){ // HVCMOS
      PCH_GPIO_COM4,
      GPIO_VER3_PCH_GPIO_HVCMOS_PAD_MAX,
      R_GPIO_VER3_PCH_GPIO_PCR_HVCMOS_PADCFG_OFFSET,
      R_GPIO_VER3_PCH_GPIO_PCR_HVCMOS_HOSTSW_OWN,
      R_GPIO_VER3_PCH_GPIO_PCR_HVCMOS_PAD_OWN,
      NO_REGISTER_FOR_PROPERTY,
      GPIO_OS_DRV_NOT_SUPPORTED
    },
    Package(){ // GPP_E
      PCH_GPIO_COM4,
      GPIO_VER3_PCH_GPIO_GPP_E_PAD_MAX,
      R_GPIO_VER3_PCH_GPIO_PCR_GPP_E_PADCFG_OFFSET,
      R_GPIO_VER3_PCH_GPIO_PCR_GPP_E_HOSTSW_OWN,
      R_GPIO_VER3_PCH_GPIO_PCR_GPP_E_PAD_OWN,
      R_GPIO_VER3_PCH_GPIO_PCR_GPP_E_GPI_GPE_STS,
      416
    },
    Package(){ // JTAG
      PCH_GPIO_COM4,
      GPIO_VER3_PCH_GPIO_JTAG_PAD_MAX,
      R_GPIO_VER3_PCH_GPIO_PCR_JTAG_PADCFG_OFFSET,
      R_GPIO_VER3_PCH_GPIO_PCR_JTAG_HOSTSW_OWN,
      R_GPIO_VER3_PCH_GPIO_PCR_JTAG_PAD_OWN,
      NO_REGISTER_FOR_PROPERTY,
      GPIO_OS_DRV_NOT_SUPPORTED
    },
    Package(){ // GPP_R
      PCH_GPIO_COM5,
      GPIO_VER3_PCH_GPIO_GPP_R_PAD_MAX,
      R_GPIO_VER3_PCH_GPIO_PCR_GPP_R_PADCFG_OFFSET,
      R_GPIO_VER3_PCH_GPIO_PCR_GPP_R_HOSTSW_OWN,
      R_GPIO_VER3_PCH_GPIO_PCR_GPP_R_PAD_OWN,
      R_GPIO_VER3_PCH_GPIO_PCR_GPP_R_GPI_GPE_STS,
      448
    },
    Package(){ // SPI
      PCH_GPIO_COM5,
      GPIO_VER3_PCH_GPIO_SPI_PAD_MAX,
      R_GPIO_VER3_PCH_GPIO_PCR_SPI_PADCFG_OFFSET,
      R_GPIO_VER3_PCH_GPIO_PCR_SPI_HOSTSW_OWN,
      R_GPIO_VER3_PCH_GPIO_PCR_SPI_PAD_OWN,
      NO_REGISTER_FOR_PROPERTY,
      GPIO_OS_DRV_NOT_SUPPORTED
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
    Buffer(GPIO_VER3_PCH_GPIO_GPP_B_PAD_MAX){},  // PCH: GPP_B
    Buffer(GPIO_VER3_PCH_GPIO_GPP_T_PAD_MAX){},  // PCH: GPP_T
    Buffer(GPIO_VER3_PCH_GPIO_GPP_G_PAD_MAX){},  // PCH: GPP_G
    Buffer(GPIO_VER3_PCH_GPIO_GPP_V_PAD_MAX){},  // PCH: GPP_V
    Buffer(GPIO_VER3_PCH_GPIO_GPP_H_PAD_MAX){},  // PCH: GPP_H
    Buffer(GPIO_VER3_PCH_GPIO_GPP_D_PAD_MAX){},  // PCH: GPP_D
    Buffer(GPIO_VER3_PCH_GPIO_GPP_U_PAD_MAX){},  // PCH: GPP_U
    Buffer(GPIO_VER3_PCH_GPIO_VGPIO_PAD_MAX){},  // PCH: VGPIO
    Buffer(GPIO_VER3_PCH_GPIO_GPD_PAD_MAX){},    // PCH: GPD
    Buffer(GPIO_VER3_PCH_GPIO_CPU_PAD_MAX){},    // PCH: CPU
    Buffer(GPIO_VER3_PCH_GPIO_GPP_S_PAD_MAX){},  // PCH: GPP_S
    Buffer(GPIO_VER3_PCH_GPIO_GPP_A_PAD_MAX){},  // PCH: GPP_A
    Buffer(GPIO_VER3_PCH_GPIO_VGPIO_3_PAD_MAX){},// PCH: VGPIO3
    Buffer(GPIO_VER3_PCH_GPIO_GPP_C_PAD_MAX){},  // PCH: GPP_C
    Buffer(GPIO_VER3_PCH_GPIO_GPP_F_PAD_MAX){},  // PCH: GPP_F
    Buffer(GPIO_VER3_PCH_GPIO_HVCMOS_PAD_MAX){}, // PCH: HVCMOS
    Buffer(GPIO_VER3_PCH_GPIO_GPP_E_PAD_MAX){},  // PCH: GPP_E
    Buffer(GPIO_VER3_PCH_GPIO_JTAG_PAD_MAX){},   // PCH: JTAG
    Buffer(GPIO_VER3_PCH_GPIO_GPP_R_PAD_MAX){},  // PCH: GPP_R
    Buffer(GPIO_VER3_PCH_GPIO_SPI_PAD_MAX){},    // PCH: SPI
  })
}
