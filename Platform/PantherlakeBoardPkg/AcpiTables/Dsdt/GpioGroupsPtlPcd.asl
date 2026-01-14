/**@file
  GPIO data for use with GPIO ASL lib

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include <Register/GpioRegs.h>
#include <Register/PchRegsGpioVer2.h>
#include <Register/GpioAcpiDefines.h>

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
Scope (\_SB)
{
  //
  // GPIO information data structure
  //
  // GPCx[GroupIndex][Field]
  //   Field1 - Maximum Pad number
  //   Field2 - PADCFG register offset
  //   Field3 - HOSTSW_OWN register offset
  //   Field4 - PAD_OWN register offset
  //   Field5 - GPI_GPE_STS register offset
  //   Field6 - PAD_CFG_LOCK register offset
  //   Field7 - PAD_CFG_LOCK_TX register offset
  //   Field8 - Starting Gpio Interrupt Index for given group within community
  //            Gpio base number for GPIO OS Driver usage
  //            GpioIo and GpioInt number is created in following way:
  //            Pad Io/Int Index = Field8 value + Pad Index within given group
  //   Field9 - Community Index
  //
  //
  Name(GPCS, Package (){
    Package () { // COM0
      PTL_PCD_GPIOV2_COM0,
      Package (){
        Package (){  // Group: GPP_V
          GPIOV2_PTL_PCD_GPP_V_PAD_MAX,
          R_GPIOV2_PTL_PCD_PCR_GPP_V_PAD_CFG_DW0,
          R_GPIOV2_PTL_PCD_PCR_GPP_V_HOSTSW_OWN,
          R_GPIOV2_PTL_PCD_PCR_GPP_V_PAD_OWN,
          R_GPIOV2_PTL_PCD_PCR_GPP_V_GPI_GPE_STS,
          R_GPIOV2_PTL_PCD_PCR_GPP_V_PADCFGLOCK,
          R_GPIOV2_PTL_PCD_PCR_GPP_V_PADCFGLOCKTX,
          0,
          0
        },
        Package (){  // Group: GPP_C
          GPIOV2_PTL_PCD_GPP_C_PAD_MAX,
          R_GPIOV2_PTL_PCD_PCR_GPP_C_PAD_CFG_DW0,
          R_GPIOV2_PTL_PCD_PCR_GPP_C_HOSTSW_OWN,
          R_GPIOV2_PTL_PCD_PCR_GPP_C_PAD_OWN,
          R_GPIOV2_PTL_PCD_PCR_GPP_C_GPI_GPE_STS,
          R_GPIOV2_PTL_PCD_PCR_GPP_C_PADCFGLOCK,
          R_GPIOV2_PTL_PCD_PCR_GPP_C_PADCFGLOCKTX,
          24,
          1
        }
      }
    },
    Package (){ // COM1
      PTL_PCD_GPIOV2_COM1,
      Package (){
        Package (){  // Group: GPP_F
          GPIOV2_PTL_PCD_GPP_F_PAD_MAX,
          R_GPIOV2_PTL_PCD_PCR_GPP_F_PAD_CFG_DW0,
          R_GPIOV2_PTL_PCD_PCR_GPP_F_HOSTSW_OWN,
          R_GPIOV2_PTL_PCD_PCR_GPP_F_PAD_OWN,
          R_GPIOV2_PTL_PCD_PCR_GPP_F_GPI_GPE_STS,
          R_GPIOV2_PTL_PCD_PCR_GPP_F_PADCFGLOCK,
          R_GPIOV2_PTL_PCD_PCR_GPP_F_PADCFGLOCKTX,
          0,
          2
        },
        Package (){  // Group: GPP_E
          GPIOV2_PTL_PCD_GPP_E_PAD_MAX,
          R_GPIOV2_PTL_PCD_PCR_GPP_E_PAD_CFG_DW0,
          R_GPIOV2_PTL_PCD_PCR_GPP_E_HOSTSW_OWN,
          R_GPIOV2_PTL_PCD_PCR_GPP_E_PAD_OWN,
          R_GPIOV2_PTL_PCD_PCR_GPP_E_GPI_GPE_STS,
          R_GPIOV2_PTL_PCD_PCR_GPP_E_PADCFGLOCK,
          R_GPIOV2_PTL_PCD_PCR_GPP_E_PADCFGLOCKTX,
          26,
          3
        }
      }
    },
    Package() { // COM3
      PTL_PCD_GPIOV2_COM3,
      Package (){
        Package (){  // Group: CPUJTAG
          GPIOV2_PTL_PCD_CPUJTAG_PAD_MAX,
          R_GPIOV2_PTL_PCD_PCR_CPUJTAG_PAD_CFG_DW0,
          R_GPIOV2_PTL_PCD_PCR_CPUJTAG_HOSTSW_OWN,
          R_GPIOV2_PTL_PCD_PCR_CPUJTAG_PAD_OWN,
          R_GPIOV2_PTL_PCD_PCR_CPUJTAG_GPI_GPE_STS,
          R_GPIOV2_PTL_PCD_PCR_CPUJTAG_PADCFGLOCK,
          R_GPIOV2_PTL_PCD_PCR_CPUJTAG_PADCFGLOCKTX,
          0,
          4
        },
        Package (){  // Group: GPP_H
          GPIOV2_PTL_PCD_GPP_H_PAD_MAX,
          R_GPIOV2_PTL_PCD_PCR_GPP_H_PAD_CFG_DW0,
          R_GPIOV2_PTL_PCD_PCR_GPP_H_HOSTSW_OWN,
          R_GPIOV2_PTL_PCD_PCR_GPP_H_PAD_OWN,
          R_GPIOV2_PTL_PCD_PCR_GPP_H_GPI_GPE_STS,
          R_GPIOV2_PTL_PCD_PCR_GPP_H_PADCFGLOCK,
          R_GPIOV2_PTL_PCD_PCR_GPP_H_PADCFGLOCKTX,
          15,
          5
        },
        Package (){  // Group: GPPASPI0
          GPIOV2_PTL_PCD_GPPASPI0_PAD_MAX,
          R_GPIOV2_PTL_PCD_PCR_GPPASPI0_PAD_CFG_DW0,
          R_GPIOV2_PTL_PCD_PCR_GPPASPI0_HOSTSW_OWN,
          R_GPIOV2_PTL_PCD_PCR_GPPASPI0_PAD_OWN,
          R_GPIOV2_PTL_PCD_PCR_GPPASPI0_GPI_GPE_STS,
          R_GPIOV2_PTL_PCD_PCR_GPPASPI0_PADCFGLOCK,
          R_GPIOV2_PTL_PCD_PCR_GPPASPI0_PADCFGLOCKTX,
          43,
          6
        },
        Package (){  // Group: vGPIO_3
          GPIOV2_PTL_PCD_VGPIO_3_PAD_MAX,
          R_GPIOV2_PTL_PCD_PCR_VGPIO_3_PAD_CFG_DW0,
          R_GPIOV2_PTL_PCD_PCR_VGPIO_3_HOSTSW_OWN,
          R_GPIOV2_PTL_PCD_PCR_VGPIO_3_PAD_OWN,
          R_GPIOV2_PTL_PCD_PCR_VGPIO_3_GPI_GPE_STS,
          R_GPIOV2_PTL_PCD_PCR_VGPIO_3_PADCFGLOCK,
          R_GPIOV2_PTL_PCD_PCR_VGPIO_3_PADCFGLOCKTX,
          71,
          7
        }
      }
    },
    Package() { // COM4
      PTL_PCD_GPIOV2_COM4,
      Package (){
        Package (){  // Group: GPP_S
          GPIOV2_PTL_PCD_GPP_S_PAD_MAX,
          R_GPIOV2_PTL_PCD_PCR_GPP_S_PAD_CFG_DW0,
          R_GPIOV2_PTL_PCD_PCR_GPP_S_HOSTSW_OWN,
          R_GPIOV2_PTL_PCD_PCR_GPP_S_PAD_OWN,
          R_GPIOV2_PTL_PCD_PCR_GPP_S_GPI_GPE_STS,
          R_GPIOV2_PTL_PCD_PCR_GPP_S_PADCFGLOCK,
          R_GPIOV2_PTL_PCD_PCR_GPP_S_PADCFGLOCKTX,
          0,
          8
        }
      }
    },
    Package() { // COM5
      PTL_PCD_GPIOV2_COM5,
      Package (){
        Package (){  // Group: GPP_B
          GPIOV2_PTL_PCD_GPP_B_PAD_MAX,
          R_GPIOV2_PTL_PCD_PCR_GPP_B_PAD_CFG_DW0,
          R_GPIOV2_PTL_PCD_PCR_GPP_B_HOSTSW_OWN,
          R_GPIOV2_PTL_PCD_PCR_GPP_B_PAD_OWN,
          R_GPIOV2_PTL_PCD_PCR_GPP_B_GPI_GPE_STS,
          R_GPIOV2_PTL_PCD_PCR_GPP_B_PADCFGLOCK,
          R_GPIOV2_PTL_PCD_PCR_GPP_B_PADCFGLOCKTX,
          0,
          9
        },
        Package (){  // Group: GPP_D
          GPIOV2_PTL_PCD_GPP_D_PAD_MAX,
          R_GPIOV2_PTL_PCD_PCR_GPP_D_PAD_CFG_DW0,
          R_GPIOV2_PTL_PCD_PCR_GPP_D_HOSTSW_OWN,
          R_GPIOV2_PTL_PCD_PCR_GPP_D_PAD_OWN,
          R_GPIOV2_PTL_PCD_PCR_GPP_D_GPI_GPE_STS,
          R_GPIOV2_PTL_PCD_PCR_GPP_D_PADCFGLOCK,
          R_GPIOV2_PTL_PCD_PCR_GPP_D_PADCFGLOCKTX,
          27,
          10
        },
        Package (){  // Group: vGPIO
          GPIOV2_PTL_PCD_VGPIO_PAD_MAX,
          R_GPIOV2_PTL_PCD_PCR_VGPIO_PAD_CFG_DW0,
          R_GPIOV2_PTL_PCD_PCR_VGPIO_HOSTSW_OWN,
          R_GPIOV2_PTL_PCD_PCR_VGPIO_PAD_OWN,
          R_GPIOV2_PTL_PCD_PCR_VGPIO_GPI_GPE_STS,
          R_GPIOV2_PTL_PCD_PCR_VGPIO_PADCFGLOCK,
          R_GPIOV2_PTL_PCD_PCR_VGPIO_PADCFGLOCKTX,
          54,
          11
        }
      }
    }
  })
}
