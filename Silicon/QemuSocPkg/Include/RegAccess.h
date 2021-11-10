/** @file

  Copyright (c) 2016 - 2021, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _REG_ACCESS_H_

#include <Library/IoLib.h>
#include <Library/PciLib.h>
#include <PlatformBase.h>
#include <BlCommon.h>
#include <IndustryStandard/Pci.h>

#define POWER_MGMT_REGISTER_Q35(Offset)    PCI_LIB_ADDRESS (0, 0x1f, 0, (Offset))

#define Q35_HOST_BRIDGE_PAM0            0x90

#define MCH_SMRAM             0x9D
#define MCH_SMRAM_D_LCK         BIT4
#define MCH_SMRAM_D_CLOSE       BIT5
#define MCH_SMRAM_D_OPEN        BIT6

#define MCH_ESMRAMC           0x9E
#define MCH_ESMRAMC_T_EN        BIT0

#define ICH9_PMBASE_OFS_SMI_EN 0x30

#define ICH9_PMBASE           0x40
#define ICH9_PMBASE_MASK      (BIT15 | BIT14 | BIT13 | BIT12 | BIT11 | \
                               BIT10 | BIT9  | BIT8  | BIT7)

#define ICH9_GEN_PMCON_1          0xA0
#define ICH9_GEN_PMCON_1_SMI_LOCK   BIT4

#define ICH9_RCBA                 0xF0
#define ICH9_RCBA_EN                BIT0

#endif
