/** @file
  Register names for IBL Virtual GPIO V2

Conventions:

  - Register definition format:
    Prefix_[GenerationName]_RegisterSpace_RegisterName
  - Prefix:
    Definitions beginning with "B_" are bits within registers
    Definitions beginning with "V_" are meaningful values within the bits
    Definitions beginning with "N_" are the bit position
  - [GenerationName]:
    Register name without GenerationName applies to all generations.
  - RegisterSpace:
    PCR - Private configuration register of subsystem.
  - RegisterName:
    Full register name.

  Copyright (c) 2023, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#ifndef IBL_GPIO_REGS_H
#define IBL_GPIO_REGS_H

//
// Pad Configuration Register DW0
//

//Virtual GPIO Reset Config
#define B_VGPIO_PCR_PAD_RESET_CONFIG    (BIT31 | BIT30)
#define N_VGPIO_PCR_PAD_RESET_CONFIG    30

//Virtual GPIO Ownership
#define B_VGPIO_PCR_RX_PAD_OWNERSHIP    (BIT29 | BIT28)
#define N_VGPIO_PCR_RX_PAD_OWNERSHIP    28

//Virtual GPIO HostSW Ownership
#define B_VGPIO_PCR_RX_PAD_HOSTSW_OWNERSHIP BIT27
#define N_VGPIO_PCR_RX_PAD_HOSTSW_OWNERSHIP 27

//Virtual GPIO interrupts routing
#define B_VGPIO_PCR_INT_TYPE            (BIT26 | BIT25 | BIT24)
#define N_VGPIO_PCR_INT_TYPE            24
#define V_VGPIO_PCR_INT_TYPE_IRQ        0x0
#define V_VGPIO_PCR_INT_TYPE_SCI        0x1
#define V_VGPIO_PCR_INT_TYPE_SMI        0x2
#define V_VGPIO_PCR_INT_TYPE_NMI        0x4
#define V_VGPIO_PCR_INT_TYPE_DIS        0x7

//RX Level/Edge Configuration for Virtual GPIO
#define B_VGPIO_PCR_RX_LVL_EDG          (BIT23 | BIT22)
#define N_VGPIO_PCR_RX_LVL_EDG          22
#define V_VGPIO_PCR_RX_LVL_EDG_LVL      0x00
#define V_VGPIO_PCR_RX_LVL_EDG_0        0x02

//Interrupt number
#define B_VGPIO_PCR_INTSEL              (BIT21 | BIT20 | BIT19 | BIT18 | BIT17 | BIT16 | BIT15 | BIT14)
#define N_VGPIO_PCR_INTSEL              14

// Virtual GPIO Chipselect
#define B_VGPIO_PCR_OUT_CS              (BIT6 | BIT5)
#define N_VGPIO_PCR_OUT_CS              5
#define V_VGPIO_PCR_OUT_CS_0            0
#define V_VGPIO_PCR_OUT_CS_1            1

//GPIO RX Disable for Virtual GPIO
#define B_VGPIO_PCR_RX_DISABLE          BIT4
#define N_VGPIO_PCR_RX_DISABLE          4

//GPIO TX Disable for Virtual GPIO
#define B_VGPIO_PCR_TX_DISABLE          BIT3
#define N_VGPIO_PCR_TX_DISABLE          3

//GPIO RX State for Virtual GPIO
#define B_VGPIO_PCR_RX_STATE            BIT1
#define N_VGPIO_PCR_RX_STATE            1

//GPIO TX State for Virtual GPIO
#define B_VGPIO_PCR_TX_STATE            BIT0
#define N_VGPIO_PCR_TX_STATE            0

#endif // IBL_GPIO_REGS_H