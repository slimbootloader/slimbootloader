/** @file
  Register names for CNVi

  Conventions:

  - Prefixes:
    Definitions beginning with "R_" are registers
    Definitions beginning with "B_" are bits within registers
    Definitions beginning with "V_" are meaningful values within the bits
    Definitions beginning with "S_" are register sizes
    Definitions beginning with "N_" are the bit position
  - In general, PCH registers are denoted by "_PCH_" in register names
  - Registers / bits that are different between PCH generations are denoted by
    "_PCH_[generation_name]_" in register/bit names.
  - Registers / bits that are specific to PCH-H denoted by "_H_" in register/bit names.
    Registers / bits that are specific to PCH-LP denoted by "_LP_" in register/bit names.
    e.g., "_PCH_H_", "_PCH_LP_"
    Registers / bits names without _H_ or _LP_ apply for both H and LP.
  - Registers / bits that are different between SKUs are denoted by "_[SKU_name]"
    at the end of the register/bit names
  - Registers / bits of new devices introduced in a PCH generation will be just named
    as "_PCH_" without [generation_name] inserted.

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/
#ifndef _CNVI_REGS_H_
#define _CNVI_REGS_H_

//
// CNVi WiFi Cfg Registers (D20:F3) and CNVi Bluetooth Cfg Registers (D20:F7)
//
#define R_CNVI_CFG_GIO_DEV_CAP                0x00000044U      ///< GIO Device Capability
                                                               /// Register default value: 0x10000ec0
                                                               /* The Device Capabilities register identifies
                                                                * PCI Express device specific capabilities.(Offset
                                                                * 0x44 )
                                                                */
#define R_CNVI_CFG_GIO_DEV                    0x00000048U      ///< GIO Device
                                                               /// Register default value: 0x00100c10
                                                               /* Device Control Register. (Offset 0x48 )
                                                                */
#define B_CNVI_CFG_GIO_DEV_INIT_FNC_LV_RS     BIT15            ///< INIT FNC LV RS
                                                               /* Initiate Function Level Reset -
                                                                * A write of 1b initiates Function
                                                                * (init FLR) this bit is RW after
                                                                * gf_cfg_dev_cap_flr_sup is set from
                                                                * OTP
                                                                */
#define R_CNVI_CFG_GIO_DEV_CAP_2              0x00000064U      ///< GIO Device Capability [2]
                                                               /// Register default value: 0x00080812
                                                               /* device control 2 (and status) register.
                                                                * (Offset 0x64 )
                                                                * new at Wilkins2
                                                                */
#define B_CNVI_CFG_GIO_DEV_CAP_2_LTR_MEC_SUP  BIT11            ///< LTR MEC SUP
                                                               /* LTR Mechanism Supported - a value
                                                                * of 1b indicates support for LTR.
                                                                */
#define R_CNVI_CFG_GIO_DEV_2                  0x00000068U      ///< GIO Device [2]
                                                               /// Register default value: 0x00000005
                                                               /* device control (and status) register.  (Offset
                                                                * 0x68 )
                                                                * spec section:7.8.16   new at Wilkins2
                                                                */
#define B_CNVI_CFG_GIO_DEV_2_LTR_MEC_EN       BIT10            ///< LTR MEC Enable
                                                               /* LTR Mechanism Enable - When Set
                                                                * enables the LTR.
                                                                */
#define R_CNVI_CFG_PMCSR                      0x000000ccU      ///< PMCSR
                                                               /// Register default value: 0x0d000008
                                                               /* Power Management Status and Control (Offset
                                                                * 0xCC )
                                                                * (in Kedron MAS PMSC)
                                                                */
#define B_CNVI_CFG_PMCSR_PWR_STATE            ( BIT1 | BIT0 )  ///< Power STATE
                                                               /* Power State
                                                                */
//
// CNVi WiFi Private registers description
//
#define R_CNVI_PCR_CNVI_PLDR_ABORT_SOC_M      0x00000080U      ///< XTAL request timeers and enable
#define R_CNVI_PCR_CNVI_PLDR_ABORT_SOC_S      0x00000044U      /// Register default value: 0x00000000
                                                               /* XTAL request timeers and enable.
                                                                */
#define B_CNVI_PCR_CNVI_PLDR_ABORT_CNVI_PLDR_ABORT_ENABLE BIT0             ///< CNVI_PLDR_ABORT_ENABLE
                                                               /* CNVI_PLDR_ABORT_ENABLE
                                                                */
#define B_CNVI_PCR_CNVI_PLDR_ABORT_CNVI_PLDR_ABORT_REQUEST BIT1             ///< CNVI_PLDR_ABORT_REQUEST
                                                               /* CNVI_PLDR_ABORT_REQUEST
                                                                */
#define B_CNVI_PCR_CNVI_PLDR_ABORT_SCU_CNVB_CNVI_READY BIT2             ///< SCU_CNVB_CNVI_READY
                                                               /* SCU_CNVB_CNVI_READY
                                                                */
#define R_CNVI_PCR_STAT                       0x00008100U      ///< Status Register
                                                               /// Register default value: 0x00000000
#define R_CNVI_PCR_WIDGET_CONTROL             0x00008120U      ///< Widget Control Register
                                                               /// Register default value: 0x00000000
                                                               /* This is a general register for controlling
                                                                * different IOSF-SB Widgets in the
                                                                * Connectity Buttress.
                                                                */
#define B_CNVI_PCR_WIDGET_CONTROL_MEW_INT     ( BIT7 | BIT6 | BIT5 | BIT4 | BIT3 | BIT2 | BIT1 | BIT0 ) ///< Message Encode Widget Interrupt Line
                                                               /* This field defines the IRQ number
                                                                * that is sent with the
                                                                * Assert_IRQn and Deassert_IRQn message
                                                                * that is sent when triggered from
                                                                * CNVi
                                                                * WiFi core.
                                                                */

//
// CNVi Bluetooth Private registers description
//

#define R_CNVI_PCR_BT_WIDGET_CONTROL          0x00008124U      ///< BT Widget Control Register
                                                               /// Register default value: 0x00000000
                                                               /* This is a general register for controlling
                                                                * different IOSF-SB Widgets in the
                                                                * Connectity Buttress.
                                                                */
#define B_CNVI_PCR_BT_WIDGET_CONTROL_BT_MEW_INT ( BIT7 | BIT6 | BIT5 | BIT4 | BIT3 | BIT2 | BIT1 | BIT0 ) ///< Message Encode Widget Interrupt Line
                                                               /* This field defines the IRQ number
                                                                * that is sent with the
                                                                * Assert_IRQn and Deassert_IRQn message
                                                                * that is sent when triggered from
                                                                * CNVi
                                                                * BT core.
                                                                */
#define R_CNVI_PCR_BT_PCIE_IF                 0x00008250U      ///< BT_PCIE_IF
                                                               /// Register default value: 0x00000000
#define B_CNVI_PCR_BT_PCIE_IF_BT_PCIE_IF_SEL  BIT0             ///< BT_PCIE_IF
                                                               /* BT_PCIE_IF.
                                                                */
#define R_CNVI_PCR_BT_OTP_RD_DONE             0x00008254U      ///< BT_OTP_RD_DONE
                                                               /// Register default value: 0x00000000
#define B_CNVI_PCR_BT_OTP_RD_DONE_BT_OTP_READ_DONE BIT0        ///< BT_OTP_RD_DONE
                                                               /* BT_OTP_RD_DONE.
                                                                */
#endif // _CNVI_REGS_H_
