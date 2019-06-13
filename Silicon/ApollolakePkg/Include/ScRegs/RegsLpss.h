/** @file
  Register names for Low Power Sub System (LPSS) module.

  Conventions:

  - Prefixes:
    Definitions beginning with "R_" are registers
    Definitions beginning with "B_" are bits within registers
    Definitions beginning with "V_" are meaningful values of bits within the registers
    Definitions beginning with "S_" are register sizes
    Definitions beginning with "N_" are the bit position
  - In general, SC registers are denoted by "_PCH_" in register names
  - Registers / bits that are different between SC generations are denoted by
    "_PCH_<generation_name>_" in register/bit names. e.g., "_PCH_CHV_"
  - Registers / bits that are different between SKUs are denoted by "_<SKU_name>"
    at the end of the register/bit names
  - Registers / bits of new devices introduced in a SC generation will be just named
    as "_PCH_" without <generation_name> inserted.

  Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#ifndef _REGS_LPSS_H_
#define _REGS_LPSS_H_


///
///Low Power Input Output (LPSS) Module Registers
///
///Define LPSS IO Devices Generic PCI Registers
///

#define R_LPSS_IO_DEVVENDID                  0x00 ///< Device ID & Vendor ID
#define B_LPSS_IO_DEVVENDID_DID              0xFFFF0000 ///< Device ID
#define B_LPSS_IO_DEVVENDID_VID              0x0000FFFF ///< Vendor ID

#define R_LPSS_IO_STSCMD                     0x04  ///< Status & Command
#define B_LPSS_IO_STSCMD_SSE                 BIT30 ///< Signaled System Error
#define B_LPSS_IO_STSCMD_RMA                 BIT29 ///< Received Master Abort
#define B_LPSS_IO_STSCMD_RTA                 BIT28 ///< Received Target Abort
#define B_LPSS_IO_STSCMD_STA                 BIT27 ///< Signaled Target Abort
#define B_LPSS_IO_STSCMD_CAPLIST             BIT20 ///< Capability List
#define B_LPSS_IO_STSCMD_INTRSTS             BIT19 ///< Interrupt Status
#define B_LPSS_IO_STSCMD_INTRDIS             BIT10 ///< Interrupt Disable
#define B_LPSS_IO_STSCMD_SERREN              BIT8  ///< SERR# Enable
#define B_LPSS_IO_STSCMD_BME                 BIT2  ///< Bus Master Enable
#define B_LPSS_IO_STSCMD_MSE                 BIT1  ///< Memory Space Enable

#define R_LPSS_IO_REVCC                      0x08  ///< Revision ID & Class Code
#define B_LPSS_IO_REVCC_CC                   0xFFFFFF00 ///< Class Code
#define B_LPSS_IO_REVCC_RID                  0x000000FF ///< Revision ID

#define R_LPSS_IO_CLHB                       0x0C
#define B_LPSS_IO_CLHB_MULFNDEV              BIT23 ///< Multi Function Device
#define B_LPSS_IO_CLHB_HT                    0x007F0000 ///< Header Type
#define B_LPSS_IO_CLHB_LT                    0x0000FF00 ///< Latency Timer
#define B_LPSS_IO_CLHB_CLS                   0x000000FF ///< Cache Line Size

#define R_LPSS_IO_BAR                        0x10  ///< BAR0 Low
#define R_LPSS_IO_BAR_HIGH                   0x14  ///< BAR0 High
#define B_LPSS_IO_BAR_BA                     0xFFFFF000 ///< Base Address
#define V_LPSS_IO_BAR_SIZE                   0x1000
#define N_LPSS_IO_BAR_ALIGNMENT              12
#define B_LPSS_IO_BAR_SI                     0x00000FF0 ///< Size Indicator
#define B_LPSS_IO_BAR_PF                     BIT3  ///< Prefetchable
#define B_LPSS_IO_BAR_TYPE                   (BIT2 | BIT1) ///< Type
#define B_LPSS_IO_BAR_MS                     BIT0  ///< Message Space

#define R_LPSS_IO_BAR1                       0x18  ///< BAR1 Low
#define R_LPSS_IO_BAR1_HIGH                  0x1C  ///< BAR1 High
#define B_LPSS_IO_BAR1_BA                    0xFFFFF000 ///< Base Address
#define V_LPSS_IO_BAR1_SIZE                  0x1000
#define B_LPSS_IO_BAR1_SI                    0x00000FF0 ///< Size Indicator
#define B_LPSS_IO_BAR1_PF                    BIT3  ///< Prefetchable
#define B_LPSS_IO_BAR1_TYPE                  (BIT2 | BIT1) ///< Type
#define B_LPSS_IO_BAR1_MS                    BIT0  ///< Message Space

#define R_LPSS_IO_SSID                       0x2C  ///< Sub System ID
#define B_LPSS_IO_SSID_SID                   0xFFFF0000 ///< Sub System ID
#define B_LPSS_IO_SSID_SVID                  0x0000FFFF ///< Sub System Vendor ID

#define R_LPSS_IO_ERBAR                      0x30  ///< Expansion ROM BAR
#define B_LPSS_IO_ERBAR_BA                   0xFFFFFFFF ///< Expansion ROM Base Address

#define R_LPSS_IO_CAPPTR                     0x34  ///< Capability Pointer
#define B_LPSS_IO_CAPPTR_CPPWR               0xFF  ///< Capability Pointer Power

#define R_LPSS_IO_INTR                       0x3C  ///< Interrupt
#define B_LPSS_IO_INTR_ML                    0xFF000000 ///< Max Latency
#define B_LPSS_IO_INTR_MG                    0x00FF0000
#define B_LPSS_IO_INTR_IP                    0x00000F00 ///< Interrupt Pin
#define B_LPSS_IO_INTR_IL                    0x000000FF ///< Interrupt Line

#define R_LPSS_IO_PCAPID                     0x80  ///< Power Capability ID
#define B_LPSS_IO_PCAPID_PS                  0xF8000000 ///< PME Support
#define B_LPSS_IO_PCAPID_VS                  0x00070000 ///< Version
#define B_LPSS_IO_PCAPID_NC                  0x0000FF00 ///< Next Capability
#define B_LPSS_IO_PCAPID_PC                  0x000000FF ///< Power Capability

#define R_LPSS_IO_PCS                        0x84  ///< PME Control Status
#define B_LPSS_IO_PCS_PMESTS                 BIT15 ///< PME Status
#define B_LPSS_IO_PCS_PMEEN                  BIT8  ///< PME Enable
#define B_LPSS_IO_PCS_NSS                    BIT3  ///< No Soft Reset
#define B_LPSS_IO_PCS_PS                     (BIT1 | BIT0) ///< Power State

#define R_LPSS_IO_MANID                      0xF8  ///< Manufacturer ID
#define B_LPSS_IO_MANID_MANID                0xFFFFFFFF ///< Manufacturer ID

#define R_LPSS_IO_D0I3MAXDEVPG               0x0A0 ///< D0i3 Max Power On Latency and Device PG config


///
///LPSS IO Device Generic MMIO Register
///MMIO Registers (BAR0 + Offset)
///
#define R_LPSS_IO_MEM_PCP                 0x200        ///< Private Clock Parameters
#define B_LPSS_IO_MEM_PCP_CLK_UPDATE      BIT31        ///< Clock Divider Update
#define B_LPSS_IO_MEM_PCP_N_VAL           0x7FFF0000   ///< N value for the M over N divider
#define B_LPSS_IO_MEM_PCP_M_VAL           0x0000FFFE   ///< M value for the M over N divider
#define B_LPSS_IO_MEM_PCP_CLK_EN          BIT0         ///< Clock Enable
#define V_LPSS_IO_PPR_CLK_M_DIV           602
#define V_LPSS_IO_PPR_CLK_N_DIV           32767

#define R_LPSS_IO_MEM_RESETS              0x204        ///< Software Reset
#define B_LPSS_IO_MEM_HC_RESET_REL        (BIT0|BIT1)  ///< LPSS IO Host Controller Reset Release
#define B_LPSS_IO_MEM_iDMA_RESET_REL      BIT2         ///< iDMA Reset Release

#define R_LPSS_IO_ACTIVELTR               0x210
 #define B_LPSS_IO_ACTIVELTR_LATENCY      BIT11        ///< When the LTR registers are under Platform/HW default control, the value need set to 0x800

#define R_LPSS_IO_IDLELTR         0x214
 #define B_LPSS_IO_IDLELTR_LATENCY        BIT11        ///< When the LTR registers are under Platform/HW default control, the value need set to 0x800

#define R_LPSS_IO_REMAP_ADDRESS_LOW       0x240        ///< Low 32 bits of BAR address read by SW from BAR Low CFG Offset 0x10
#define R_LPSS_IO_REMAP_ADDRESS_HI        0x244        ///< High 32 bits of BAR address read by SW from BAR High CFG Offset 0x14

#define R_LPSS_IO_DEV_IDLE_CTRL           0x24C        ///< DevIdle Control per LPSS slice

///
/// LPSS I2C Module
/// PCI Config Space Registers
///
#define PCI_DEVICE_NUMBER_LPSS_I2C0             22
  #define PCI_FUNCTION_NUMBER_LPSS_I2C0         0
  #define PCI_FUNCTION_NUMBER_LPSS_I2C1         1
  #define PCI_FUNCTION_NUMBER_LPSS_I2C2         2
  #define PCI_FUNCTION_NUMBER_LPSS_I2C3         3

#define PCI_DEVICE_NUMBER_LPSS_I2C1             23
  #define PCI_FUNCTION_NUMBER_LPSS_I2C4         0
  #define PCI_FUNCTION_NUMBER_LPSS_I2C5         1
  #define PCI_FUNCTION_NUMBER_LPSS_I2C6         2
  #define PCI_FUNCTION_NUMBER_LPSS_I2C7         3

#define LPSS_I2C_TMP_BAR0_DELTA         0x10000   ///< Increasement for each I2C device BAR0
#define LPSS_I2C_TMP_BAR1_OFFSET        0x8000    ///< Offset from I2C BAR0 to BAR1

#define R_LPSS_I2C_IC_ENABLE            0x6C
 #define B_LPSS_I2C_IC_ENABLE_ENABLE    BIT0

///
/// LPSS HSUART Modules
/// PCI Config Space Registers
///
#define PCI_DEVICE_NUMBER_LPSS_HSUART         24
#define PCI_FUNCTION_NUMBER_LPSS_HSUART0      0
#define PCI_FUNCTION_NUMBER_LPSS_HSUART1      1
#define PCI_FUNCTION_NUMBER_LPSS_HSUART2      2
#define PCI_FUNCTION_NUMBER_LPSS_HSUART3      3

///
/// LPSS SPI Module
/// PCI Config Space Registers
///
#define PCI_DEVICE_NUMBER_LPSS_SPI            25
#define PCI_FUNCTION_NUMBER_LPSS_SPI0         0
#define PCI_FUNCTION_NUMBER_LPSS_SPI1         1
#define PCI_FUNCTION_NUMBER_LPSS_SPI2         2

#define R_LPSS_SPI_MEM_SSP_CONTROL0             0x000
 #define B_LPSS_SPI_MEM_SSP_CONTROL0_SSE        BIT7

#endif
