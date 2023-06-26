/** @file
  Header file for HostBridge IP specific Registers.

  Copyright (c) 2021 - 2023, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

@par Specification Reference:
**/
#ifndef _HOSTBRIDGE_DEFINES_H_
#define _HOSTBRIDGE_DEFINES_H_

#define SA_SEG_NUM              0x00
#define SA_MC_BUS               0x00
#define SA_MC_DEV               0x00
#define SA_MC_FUN               0x00


//
// Description of TMBAR (15:38)
// - This field corresponds to bits 38 to 15 of the base address PCI TMBAR configuration space.
// - SA PCI Device to Access TMBAR register
//
/**
 Description of GGCLCK (0:0)
 - When set to 1b, this bit will lock all bits in this register.
**/


#define R_SA_MC_SIM_RECOGNITION_OFFSET              0xFC
#define V_SA_MC_SIM_RECOGNITION_PLATFORM_SIMICS     0x04
#define V_SA_MC_SIM_RECOGNITION_PLATFORM_HFPGA      0x05


///
/// This register is used as interface between BIOS and MemSS PMA
///
typedef union {
  struct {
    UINT32 mc0_en : 1; /**< MC0 Enable */
                            /* Bits[0:0], Access Type=RW/V, default=0x00000000*/
                            /* BIOS Enable for MC0 */
    UINT32 mc1_en : 1; /**< MC1 Enable */
                            /* Bits[1:1], Access Type=RW/V, default=0x00000000*/
                            /* BIOS Enable for MC1 */
    UINT32 ibecc0_en : 1; /**< IBECC0 Enable */
                            /* Bits[2:2], Access Type=RW/V, default=0x00000000*/
                            /* BIOS Enable for IBECC0 */
    UINT32 ibecc1_en : 1; /**< IBECC1 Enable */
                            /* Bits[3:3], Access Type=RW/V, default=0x00000000*/
                            /* BIOS Enable for IBECC1 */
    UINT32 cce0_en : 1; /**< CCE0 Enable */
                            /* Bits[4:4], Access Type=RW/V, default=0x00000000*/
                            /* BIOS Enable for CCE0 */
    UINT32 cce1_en : 1; /**< CCE1 Enable */
                            /* Bits[5:5], Access Type=RW/V, default=0x00000000*/
                            /* BIOS Enable for CCE1 */
    UINT32 ddr_type : 3; /**< DDR Type */
                            /* Bits[8:6], Access Type=RW/V, default=0x00000000*/
                            /*
                               DDR Type  000 - Reserved,  001 - DDR5,  010
                               - LPDDR5,  011 - LPDDR4,  1xx - reserved
                            */
    UINT32 reserved : 23; /**< Reserved for BitField */
                            /* Bits[31:9], Access Type=RO, default=0x00000000*/
                            /* Reserved */
  }     Bits;
  UINT32 Data;
  UINT16 Data16[2];
  UINT8  Data8[4];
} R_SA_MCHBAR_MEMSS_PMA_CR_MEM_CONFIG_STRUCT;


#define  V_ADM_CONFIG_1   BIT0
#define  V_ADM_CONFIG_2   (BIT0 | BIT1)
#endif /* _HOSTBRIDGE_DEFINES_H_ */
