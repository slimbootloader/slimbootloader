/** @file
  Register names for PCH SATA controllers

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

  Copyright (c) 1999 - 2017, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/
#ifndef _PCH_REGS_SATA_H_
#define _PCH_REGS_SATA_H_


#define PCI_FUNCTION_NUMBER_CDF_PCH_SATA_1  0

#define PCI_FUNCTION_NUMBER_CDF_PCH_SATA_2  0

#define PCI_FUNCTION_NUMBER_CDF_PCH_SATA_3  0


//
//  CNL PCH-LP SATA Device ID's
//
#define V_PCH_LP_CNL_SATA_DEVICE_ID_M_AHCI         0x9DD3  ///< SATA Controller (AHCI) - Mobile
#define V_PCH_LP_CNL_SATA_DEVICE_ID_M_RAID         0x9DD5  ///< SATA Controller (RAID 0/1/5/10) - NOT premium - Mobile
#define V_PCH_LP_CNL_SATA_DEVICE_ID_M_RAID_PREM    0x9DD7  ///< SATA Controller (RAID 0/1/5/10) - premium - Mobile
#define V_PCH_LP_CNL_SATA_DEVICE_ID_M_RAID_IBC     0x282A  ///< SATA Controller (RAID 0/1/5/10) - In-box compatible - Mobile

//
//  PCH-H SATA Device ID's
//
#define V_PCH_H_SATA_DEVICE_ID_D_RAID_ALTDIS   0x2822  ///< SATA Controller (RAID 0/1/5/10) - premium - Alternate ID
#define V_PCH_H_SATA_DEVICE_ID_D_RAID_RSTE     0x2826  ///< SATA Controller (RAID 0/1/5/10) - RSTe of Server SKU

//
//  CNL PCH-H SATA Device ID's
//
#define V_PCH_H_CNL_SATA_DEVICE_ID_D_AHCI         0xA352  ///< SATA Controller (AHCI)
#define V_PCH_H_CNL_SATA_DEVICE_ID_D_RAID         0xA354  ///< SATA Controller (RAID 0/1/5/10) - NOT premium
#define V_PCH_H_CNL_SATA_DEVICE_ID_D_RAID_PREM    0xA356  ///< SATA Controller (RAID 0/1/5/10) - premium
#define V_PCH_H_CNL_SATA_DEVICE_ID_D_RAID_IBC     0x2822  ///< SATA Controller (RAID 0/1/5/10) - In-box compatible
#define V_PCH_H_CNL_SATA_DEVICE_ID_D_RAID_IBC_RST 0x2826  ///< SATA Controller (RAID 0/1/5/10) - In-box compatible (RSTe)


//
//  SATA AHCI Device ID macros
//
#define IS_PCH_H_SATA_AHCI_DEVICE_ID(DeviceId) \
    ( \
      (DeviceId == V_PCH_H_CNL_SATA_DEVICE_ID_D_AHCI) \
    )

#define IS_PCH_LP_SATA_AHCI_DEVICE_ID(DeviceId) \
    ( \
      (DeviceId == V_PCH_LP_CNL_SATA_DEVICE_ID_M_AHCI) \
    )

#define IS_PCH_SATA_AHCI_DEVICE_ID(DeviceId) \
    ( \
      IS_PCH_H_SATA_AHCI_DEVICE_ID(DeviceId) || \
      IS_PCH_LP_SATA_AHCI_DEVICE_ID(DeviceId) \
    )

//
//  SATA RAID Device ID macros
//
#define IS_PCH_H_SATA_RAID_DEVICE_ID(DeviceId) \
    ( \
      (DeviceId == V_PCH_H_CNL_SATA_DEVICE_ID_D_RAID) || \
      (DeviceId == V_PCH_H_CNL_SATA_DEVICE_ID_D_RAID_PREM) || \
      (DeviceId == V_PCH_H_CNL_SATA_DEVICE_ID_D_RAID_IBC) || \
      (DeviceId == V_PCH_H_CNL_SATA_DEVICE_ID_D_RAID_IBC_RST) || \
      (DeviceId == V_PCH_H_SATA_DEVICE_ID_D_RAID_ALTDIS) || \
      (DeviceId == V_PCH_H_SATA_DEVICE_ID_D_RAID_RSTE) \
    )

#define IS_PCH_LP_SATA_RAID_DEVICE_ID(DeviceId) \
    ( \
      (DeviceId == V_PCH_LP_CNL_SATA_DEVICE_ID_M_RAID) || \
      (DeviceId == V_PCH_LP_CNL_SATA_DEVICE_ID_M_RAID_PREM) || \
      (DeviceId == V_PCH_LP_CNL_SATA_DEVICE_ID_M_RAID_IBC) \
    )

#define IS_PCH_SATA_RAID_DEVICE_ID(DeviceId) \
    ( \
      IS_PCH_H_SATA_RAID_DEVICE_ID(DeviceId) || \
      IS_PCH_LP_SATA_RAID_DEVICE_ID(DeviceId) \
    )

//
//  Combined SATA IDE/AHCI/RAID Device ID macros
//
#define IS_PCH_H_SATA_DEVICE_ID(DeviceId) \
    ( \
      IS_PCH_H_SATA_AHCI_DEVICE_ID(DeviceId) || \
      IS_PCH_H_SATA_RAID_DEVICE_ID(DeviceId) \
    )

#define IS_PCH_LP_SATA_DEVICE_ID(DeviceId) \
    ( \
      IS_PCH_LP_SATA_AHCI_DEVICE_ID(DeviceId) || \
      IS_PCH_LP_SATA_RAID_DEVICE_ID(DeviceId) \
    )
#define IS_PCH_SATA_DEVICE_ID(DeviceId) \
    ( \
      IS_PCH_H_SATA_DEVICE_ID(DeviceId) || \
      IS_PCH_LP_SATA_DEVICE_ID(DeviceId) \
    )

#endif
