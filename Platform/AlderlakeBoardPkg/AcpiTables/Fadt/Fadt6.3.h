/** @file
  This file describes the contents of the ACPI Fixed ACPI Description Table
  (FADT).  All changes to the FADT contents should be done in this file.

  Copyright (c) 2021, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#ifndef _FADT_H_
#define _FADT_H_

#include "AcpiTablePlatform.h"
#include <IndustryStandard/Acpi63.h>
#include <Register/PmcRegs.h>


/*
1 Desktop
2 Mobile
3 Workstation
4 Enterprise Server
5 SOHO Server
6 Appliance PC
7 Performance Server
*/
#define EFI_ACPI_RESERVED_BYTE        0x00
#define EFI_ACPI_PREFERRED_PM_PROFILE 0x01
#define EFI_ACPI_SCI_INT              0x0009
#define EFI_ACPI_SMI_CMD              0x00001804
#define EFI_ACPI_ACPI_ENABLE          0x01
#define EFI_ACPI_ACPI_DISABLE         0x00
#define EFI_ACPI_S4_BIOS_REQ          0x00
#define EFI_ACPI_PSTATE_CNT           0x00
#define EFI_ACPI_GPE1_BASE            0x10
#define EFI_ACPI_CST_CNT              0x00
#define EFI_ACPI_P_LVL2_LAT           0x0065
#define EFI_ACPI_P_LVL3_LAT           0x03E9
#define EFI_ACPI_FLUSH_SIZE           0x0000
#define EFI_ACPI_FLUSH_STRIDE         0x0000
#define EFI_ACPI_DUTY_OFFSET          0x01
#define EFI_ACPI_DUTY_WIDTH           0x03
#define EFI_ACPI_DAY_ALRM             0x0D
#define EFI_ACPI_MON_ALRM             0x00
#define EFI_ACPI_CENTURY              0x32

//
// IA-PC Boot Architecture Flags, see ACPI 6.3 table specification and Acpi61.h
// We should not use EFI_ACPI_6_3_8042 for legacy free platform, or XP reset fail. No impact to mobile.
//
#define EFI_ACPI_IAPC_BOOT_ARCH (EFI_ACPI_6_3_LEGACY_DEVICES)

//
// Fixed Feature Flags, see ACPI 6.3 table specification and Acpi6_3.h
//
#define EFI_ACPI_FIXED_FEATURE_FLAGS (\
  EFI_ACPI_6_3_USE_PLATFORM_CLOCK | \
  EFI_ACPI_6_3_WBINVD | \
  EFI_ACPI_6_3_FIX_RTC | \
  EFI_ACPI_6_3_PCI_EXP_WAK | \
  EFI_ACPI_6_3_RTC_S4 | \
  EFI_ACPI_6_3_SLP_BUTTON | \
  EFI_ACPI_6_3_PROC_C1 | \
  EFI_ACPI_6_3_RESET_REG_SUP  \
  )

//
// PM1A Event Register Block Generic Address Information
//
#define EFI_ACPI_PM1A_EVT_BLK_ADDRESS_SPACE_ID  EFI_ACPI_6_3_SYSTEM_IO
#define EFI_ACPI_PM1A_EVT_BLK_BIT_WIDTH         0x20
#define EFI_ACPI_PM1A_EVT_BLK_BIT_OFFSET        0x00
#define EFI_ACPI_PM1A_EVT_BLK_ADDRESS           0x1800

//
// PM1B Event Register Block Generic Address Information
//
#define EFI_ACPI_PM1B_EVT_BLK_ADDRESS_SPACE_ID  EFI_ACPI_6_3_SYSTEM_IO
#define EFI_ACPI_PM1B_EVT_BLK_BIT_WIDTH         0x00
#define EFI_ACPI_PM1B_EVT_BLK_BIT_OFFSET        0x00
#define EFI_ACPI_PM1B_EVT_BLK_ADDRESS           0x0000000000000000

//
// PM1A Control Register Block Generic Address Information
//
#define EFI_ACPI_PM1A_CNT_BLK_ADDRESS_SPACE_ID  EFI_ACPI_6_3_SYSTEM_IO
#define EFI_ACPI_PM1A_CNT_BLK_BIT_WIDTH         0x10
#define EFI_ACPI_PM1A_CNT_BLK_BIT_OFFSET        0x00
#define EFI_ACPI_PM1A_CNT_BLK_ADDRESS           (EFI_ACPI_PM1A_EVT_BLK_ADDRESS + 0x04)

//
// PM1B Control Register Block Generic Address Information
//
#define EFI_ACPI_PM1B_CNT_BLK_ADDRESS_SPACE_ID  EFI_ACPI_6_3_SYSTEM_IO
#define EFI_ACPI_PM1B_CNT_BLK_BIT_WIDTH         0x00
#define EFI_ACPI_PM1B_CNT_BLK_BIT_OFFSET        0x00
#define EFI_ACPI_PM1B_CNT_BLK_ADDRESS           0x0000000000000000

//
// PM2 Control Register Block Generic Address Information
//
#define EFI_ACPI_PM2_CNT_BLK_ADDRESS_SPACE_ID   EFI_ACPI_6_3_SYSTEM_IO
#define EFI_ACPI_PM2_CNT_BLK_BIT_WIDTH          0x08
#define EFI_ACPI_PM2_CNT_BLK_BIT_OFFSET         0x00
#define EFI_ACPI_PM2_CNT_BLK_ADDRESS            (EFI_ACPI_PM1A_EVT_BLK_ADDRESS + 0x50)

//
// Power Management Timer Control Register Block Generic Address
// Information
//
#define EFI_ACPI_PM_TMR_BLK_ADDRESS_SPACE_ID    EFI_ACPI_6_3_SYSTEM_IO
#define EFI_ACPI_PM_TMR_BLK_BIT_WIDTH           0x20
#define EFI_ACPI_PM_TMR_BLK_BIT_OFFSET          0x00
#define EFI_ACPI_PM_TMR_BLK_ADDRESS             (EFI_ACPI_PM1A_EVT_BLK_ADDRESS + 0x08)

//
// General Purpose Event 0 Register Block Generic Address
// Information
//
#define EFI_ACPI_GPE0_BLK_ADDRESS_SPACE_ID      EFI_ACPI_6_3_SYSTEM_IO
#define EFI_ACPI_GPE0_BLK_BIT_WIDTH             0x100
#define EFI_ACPI_GPE0_BLK_BIT_OFFSET            0x00
#define EFI_ACPI_GPE0_BLK_ADDRESS               0x00001860

//
// General Purpose Event 1 Register Block Generic Address
// Information
//
#define EFI_ACPI_GPE1_BLK_ADDRESS_SPACE_ID      EFI_ACPI_6_3_SYSTEM_IO
#define EFI_ACPI_GPE1_BLK_BIT_WIDTH             0x00
#define EFI_ACPI_GPE1_BLK_BIT_OFFSET            0x00
#define EFI_ACPI_GPE1_BLK_ADDRESS               0x0000000000000000

//
// Sleep Control Register Block Generic Address Information
//
#define EFI_ACPI_SLP_CNT_BLK_ADDRESS_SPACE_ID   EFI_ACPI_6_3_SYSTEM_IO
#define EFI_ACPI_SLP_CNT_BLK_BIT_WIDTH          0x8
#define EFI_ACPI_SLP_CNT_BLK_BIT_OFFSET         0x0
#define EFI_ACPI_SLP_CNT_BLK_ADDRESS            0x1804

//
// Sleep Status Register Block Generic Address Information
//
#define EFI_ACPI_SLP_STS_BLK_ADDRESS_SPACE_ID   EFI_ACPI_6_3_SYSTEM_IO
#define EFI_ACPI_SLP_STS_BLK_BIT_WIDTH          0x8
#define EFI_ACPI_SLP_STS_BLK_BIT_OFFSET         0x0
#define EFI_ACPI_SLP_STS_BLK_ADDRESS            0x1800

//
// Reset Register Generic Address Information
//
#define EFI_ACPI_RESET_REG_ADDRESS_SPACE_ID     EFI_ACPI_6_3_SYSTEM_IO
#define EFI_ACPI_RESET_REG_BIT_WIDTH            0x08
#define EFI_ACPI_RESET_REG_BIT_OFFSET           0x00
#define EFI_ACPI_RESET_REG_ADDRESS              0x0000000000000CF9
#define EFI_ACPI_RESET_VALUE                    0x06

//
// Number of bytes decoded by PM1 event blocks (a and b)
//
#define EFI_ACPI_PM1_EVT_LEN ((EFI_ACPI_PM1A_EVT_BLK_BIT_WIDTH + EFI_ACPI_PM1B_EVT_BLK_BIT_WIDTH) / 8)

//
// Number of bytes decoded by PM1 control blocks (a and b)
//
#define EFI_ACPI_PM1_CNT_LEN ((EFI_ACPI_PM1A_CNT_BLK_BIT_WIDTH + EFI_ACPI_PM1B_CNT_BLK_BIT_WIDTH) / 8)

//
// Number of bytes decoded by PM2 control block
//
#define EFI_ACPI_PM2_CNT_LEN (EFI_ACPI_PM2_CNT_BLK_BIT_WIDTH / 8)

//
// Number of bytes decoded by PM timer block
//
#define EFI_ACPI_PM_TMR_LEN (EFI_ACPI_PM_TMR_BLK_BIT_WIDTH / 8)

//
// Number of bytes decoded by GPE0 block
//
#define EFI_ACPI_GPE0_BLK_LEN (EFI_ACPI_GPE0_BLK_BIT_WIDTH / 8)

//
// Number of bytes decoded by GPE1 block
//
#define EFI_ACPI_GPE1_BLK_LEN (EFI_ACPI_GPE1_BLK_BIT_WIDTH / 8)

#endif
