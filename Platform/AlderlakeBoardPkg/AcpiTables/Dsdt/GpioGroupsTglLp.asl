/**@file
 VER2 GPIO data for use with GPIO ASL lib

  Copyright (c) 2021, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include <GpioAcpiDefines.h>
#include <GpioAcpiDefinesTgl.h>

#include <Register/PchRegsGpio.h>
#include <Register/PchRegsGpioVer2.h>

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
  //
  //
  // GPIO information data structure for PCH-LP
  //
  Name(GPCL, Package(){
    Package(){ // GPP_B
      PCH_GPIO_COM0,
      GPIO_VER2_PCH_LP_GPIO_GPP_B_PAD_MAX,
      R_GPIO_VER2_PCH_LP_GPIO_PCR_GPP_B_PADCFG_OFFSET,
      R_GPIO_VER2_PCH_LP_GPIO_PCR_GPP_B_HOSTSW_OWN,
      R_GPIO_VER2_PCH_LP_GPIO_PCR_GPP_B_PAD_OWN,
      R_GPIO_VER2_PCH_LP_GPIO_PCR_GPP_B_GPI_GPE_STS,
      0
    },
    Package(){ // GPP_T
      PCH_GPIO_COM0,
      GPIO_VER2_PCH_LP_GPIO_GPP_T_PAD_MAX,
      R_GPIO_VER2_PCH_LP_GPIO_PCR_GPP_T_PADCFG_OFFSET,
      R_GPIO_VER2_PCH_LP_GPIO_PCR_GPP_T_HOSTSW_OWN,
      R_GPIO_VER2_PCH_LP_GPIO_PCR_GPP_T_PAD_OWN,
      R_GPIO_VER2_PCH_LP_GPIO_PCR_GPP_T_GPI_GPE_STS,
      32
    },
    Package(){ // GPP_A
      PCH_GPIO_COM0,
      GPIO_VER2_PCH_LP_GPIO_GPP_A_PAD_MAX,
      R_GPIO_VER2_PCH_LP_GPIO_PCR_GPP_A_PADCFG_OFFSET,
      R_GPIO_VER2_PCH_LP_GPIO_PCR_GPP_A_HOSTSW_OWN,
      R_GPIO_VER2_PCH_LP_GPIO_PCR_GPP_A_PAD_OWN,
      R_GPIO_VER2_PCH_LP_GPIO_PCR_GPP_A_GPI_GPE_STS,
      64
    },
    Package(){ // GPP_R
      PCH_GPIO_COM5,
      GPIO_VER2_PCH_LP_GPIO_GPP_R_PAD_MAX,
      R_GPIO_VER2_PCH_LP_GPIO_PCR_GPP_R_PADCFG_OFFSET,
      R_GPIO_VER2_PCH_LP_GPIO_PCR_GPP_R_HOSTSW_OWN,
      R_GPIO_VER2_PCH_LP_GPIO_PCR_GPP_R_PAD_OWN,
      R_GPIO_VER2_PCH_LP_GPIO_PCR_GPP_R_GPI_GPE_STS,
      352
    },
    Package(){ // SPI
      PCH_GPIO_COM5,
      GPIO_VER2_PCH_LP_GPIO_SPI_PAD_MAX,
      R_GPIO_VER2_PCH_LP_GPIO_PCR_SPI_PADCFG_OFFSET,
      R_GPIO_VER2_PCH_LP_GPIO_PCR_SPI_HOSTSW_OWN,
      R_GPIO_VER2_PCH_LP_GPIO_PCR_SPI_PAD_OWN,
      NO_REGISTER_FOR_PROPERTY,
      GPIO_OS_DRV_NOT_SUPPORTED
    },
    Package(){ // GPD
      PCH_GPIO_COM2,
      GPIO_VER2_PCH_LP_GPIO_GPD_PAD_MAX,
      R_GPIO_VER2_PCH_LP_GPIO_PCR_GPD_PADCFG_OFFSET,
      R_GPIO_VER2_PCH_LP_GPIO_PCR_GPD_HOSTSW_OWN,
      R_GPIO_VER2_PCH_LP_GPIO_PCR_GPD_PAD_OWN,
      R_GPIO_VER2_PCH_LP_GPIO_PCR_GPD_GPI_GPE_STS,
      GPIO_OS_DRV_NOT_SUPPORTED
    },
    Package(){ // GPP_S
      PCH_GPIO_COM1,
      GPIO_VER2_PCH_LP_GPIO_GPP_S_PAD_MAX,
      R_GPIO_VER2_PCH_LP_GPIO_PCR_GPP_S_PADCFG_OFFSET,
      R_GPIO_VER2_PCH_LP_GPIO_PCR_GPP_S_HOSTSW_OWN,
      R_GPIO_VER2_PCH_LP_GPIO_PCR_GPP_S_PAD_OWN,
      R_GPIO_VER2_PCH_LP_GPIO_PCR_GPP_S_GPI_GPE_STS,
      96
    },
    Package(){ // GPP_H
      PCH_GPIO_COM1,
      GPIO_VER2_PCH_LP_GPIO_GPP_H_PAD_MAX,
      R_GPIO_VER2_PCH_LP_GPIO_PCR_GPP_H_PADCFG_OFFSET,
      R_GPIO_VER2_PCH_LP_GPIO_PCR_GPP_H_HOSTSW_OWN,
      R_GPIO_VER2_PCH_LP_GPIO_PCR_GPP_H_PAD_OWN,
      R_GPIO_VER2_PCH_LP_GPIO_PCR_GPP_H_GPI_GPE_STS,
      128
    },
    Package(){ // GPP_D
      PCH_GPIO_COM1,
      GPIO_VER2_PCH_LP_GPIO_GPP_D_PAD_MAX,
      R_GPIO_VER2_PCH_LP_GPIO_PCR_GPP_D_PADCFG_OFFSET,
      R_GPIO_VER2_PCH_LP_GPIO_PCR_GPP_D_HOSTSW_OWN,
      R_GPIO_VER2_PCH_LP_GPIO_PCR_GPP_D_PAD_OWN,
      R_GPIO_VER2_PCH_LP_GPIO_PCR_GPP_D_GPI_GPE_STS,
      160
    },
    Package(){ // GPP_U
      PCH_GPIO_COM1,
      GPIO_VER2_PCH_LP_GPIO_GPP_U_PAD_MAX,
      R_GPIO_VER2_PCH_LP_GPIO_PCR_GPP_U_PADCFG_OFFSET,
      R_GPIO_VER2_PCH_LP_GPIO_PCR_GPP_U_HOSTSW_OWN,
      R_GPIO_VER2_PCH_LP_GPIO_PCR_GPP_U_PAD_OWN,
      R_GPIO_VER2_PCH_LP_GPIO_PCR_GPP_U_GPI_GPE_STS,
      192
    },
    Package(){ // VGPIO
      PCH_GPIO_COM1,
      GPIO_VER2_PCH_LP_GPIO_VGPIO_PAD_MAX,
      R_GPIO_VER2_PCH_LP_GPIO_PCR_VGPIO_PADCFG_OFFSET,
      R_GPIO_VER2_PCH_LP_GPIO_PCR_VGPIO_HOSTSW_OWN,
      R_GPIO_VER2_PCH_LP_GPIO_PCR_VGPIO_PAD_OWN,
      R_GPIO_VER2_PCH_LP_GPIO_PCR_VGPIO_GPI_GPE_STS,
      224
    },
    Package(){ // GPP_C
      PCH_GPIO_COM4,
      GPIO_VER2_PCH_LP_GPIO_GPP_C_PAD_MAX,
      R_GPIO_VER2_PCH_LP_GPIO_PCR_GPP_C_PADCFG_OFFSET,
      R_GPIO_VER2_PCH_LP_GPIO_PCR_GPP_C_HOSTSW_OWN,
      R_GPIO_VER2_PCH_LP_GPIO_PCR_GPP_C_PAD_OWN,
      R_GPIO_VER2_PCH_LP_GPIO_PCR_GPP_C_GPI_GPE_STS,
      256
    },
    Package(){ // GPP_F
      PCH_GPIO_COM4,
      GPIO_VER2_PCH_LP_GPIO_GPP_F_PAD_MAX,
      R_GPIO_VER2_PCH_LP_GPIO_PCR_GPP_F_PADCFG_OFFSET,
      R_GPIO_VER2_PCH_LP_GPIO_PCR_GPP_F_HOSTSW_OWN,
      R_GPIO_VER2_PCH_LP_GPIO_PCR_GPP_F_PAD_OWN,
      R_GPIO_VER2_PCH_LP_GPIO_PCR_GPP_F_GPI_GPE_STS,
      288
    },
    Package(){ // HVCMOS
      PCH_GPIO_COM4,
      GPIO_VER2_PCH_LP_GPIO_HVCMOS_PAD_MAX,
      R_GPIO_VER2_PCH_LP_GPIO_PCR_HVCMOS_PADCFG_OFFSET,
      R_GPIO_VER2_PCH_LP_GPIO_PCR_HVCMOS_HOSTSW_OWN,
      R_GPIO_VER2_PCH_LP_GPIO_PCR_HVCMOS_PAD_OWN,
      NO_REGISTER_FOR_PROPERTY,
      GPIO_OS_DRV_NOT_SUPPORTED
    },
    Package(){ // GPP_E
      PCH_GPIO_COM4,
      GPIO_VER2_PCH_LP_GPIO_GPP_E_PAD_MAX,
      R_GPIO_VER2_PCH_LP_GPIO_PCR_GPP_E_PADCFG_OFFSET,
      R_GPIO_VER2_PCH_LP_GPIO_PCR_GPP_E_HOSTSW_OWN,
      R_GPIO_VER2_PCH_LP_GPIO_PCR_GPP_E_PAD_OWN,
      R_GPIO_VER2_PCH_LP_GPIO_PCR_GPP_E_GPI_GPE_STS,
      320
    },
    Package(){ // JTAG
      PCH_GPIO_COM4,
      GPIO_VER2_PCH_LP_GPIO_JTAG_PAD_MAX,
      R_GPIO_VER2_PCH_LP_GPIO_PCR_JTAG_PADCFG_OFFSET,
      R_GPIO_VER2_PCH_LP_GPIO_PCR_JTAG_HOSTSW_OWN,
      R_GPIO_VER2_PCH_LP_GPIO_PCR_JTAG_PAD_OWN,
      NO_REGISTER_FOR_PROPERTY,
      GPIO_OS_DRV_NOT_SUPPORTED
    },
    Package(){ // CPU
      PCH_GPIO_COM3,
      GPIO_VER2_PCH_LP_GPIO_CPU_PAD_MAX,
      R_GPIO_VER2_PCH_LP_GPIO_PCR_CPU_PADCFG_OFFSET,
      R_GPIO_VER2_PCH_LP_GPIO_PCR_CPU_HOSTSW_OWN,
      R_GPIO_VER2_PCH_LP_GPIO_PCR_CPU_PAD_OWN,
      NO_REGISTER_FOR_PROPERTY,
      GPIO_OS_DRV_NOT_SUPPORTED
    },
    Package(){ // VGPIO_3
      PCH_GPIO_COM3,
      GPIO_VER2_PCH_LP_GPIO_VGPIO_3_PAD_MAX,
      R_GPIO_VER2_PCH_LP_GPIO_PCR_VGPIO_3_PADCFG_OFFSET,
      R_GPIO_VER2_PCH_LP_GPIO_PCR_VGPIO_3_HOSTSW_OWN,
      R_GPIO_VER2_PCH_LP_GPIO_PCR_VGPIO_3_PAD_OWN,
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
    Buffer(GPIO_VER2_PCH_LP_GPIO_GPP_B_PAD_MAX){},  // PCH-LP: GPP_B
    Buffer(GPIO_VER2_PCH_LP_GPIO_GPP_T_PAD_MAX){},  // PCH-LP: GPP_T
    Buffer(GPIO_VER2_PCH_LP_GPIO_GPP_A_PAD_MAX){},  // PCH-LP: GPP_A
    Buffer(GPIO_VER2_PCH_LP_GPIO_GPP_R_PAD_MAX){},  // PCH-LP: GPP_R
    Buffer(GPIO_VER2_PCH_LP_GPIO_SPI_PAD_MAX){},    // PCH-LP: SPI
    Buffer(GPIO_VER2_PCH_LP_GPIO_GPD_PAD_MAX){},    // PCH-LP: GPD
    Buffer(GPIO_VER2_PCH_LP_GPIO_GPP_S_PAD_MAX){},  // PCH-LP: GPP_S
    Buffer(GPIO_VER2_PCH_LP_GPIO_GPP_H_PAD_MAX){},  // PCH-LP: GPP_H
    Buffer(GPIO_VER2_PCH_LP_GPIO_GPP_D_PAD_MAX){},  // PCH-LP: GPP_D
    Buffer(GPIO_VER2_PCH_LP_GPIO_GPP_U_PAD_MAX){},  // PCH-LP: GPP_U
    Buffer(GPIO_VER2_PCH_LP_GPIO_VGPIO_PAD_MAX){},  // PCH-LP: VGPIO
    Buffer(GPIO_VER2_PCH_LP_GPIO_GPP_C_PAD_MAX){},  // PCH-LP: GPP_C
    Buffer(GPIO_VER2_PCH_LP_GPIO_GPP_F_PAD_MAX){},  // PCH-LP: GPP_F
    Buffer(GPIO_VER2_PCH_LP_GPIO_HVCMOS_PAD_MAX){}, // PCH-LP: HVCMOS
    Buffer(GPIO_VER2_PCH_LP_GPIO_GPP_E_PAD_MAX){},  // PCH-LP: GPP_E
    Buffer(GPIO_VER2_PCH_LP_GPIO_JTAG_PAD_MAX){},   // PCH-LP: JTAG
    Buffer(GPIO_VER2_PCH_LP_GPIO_CPU_PAD_MAX){},    // PCH-LP: CPU
    Buffer(GPIO_VER2_PCH_LP_GPIO_VGPIO_3_PAD_MAX){} // PCH-LP: VGPIO3
  })
}
