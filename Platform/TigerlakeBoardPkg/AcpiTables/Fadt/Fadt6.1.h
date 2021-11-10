/** @file
  This file describes the contents of the ACPI Fixed ACPI Description Table
  (FADT).  All changes to the FADT contents should be done in this file.

  Copyright (c) 2017 - 2018, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#ifndef _FADT_H_
#define _FADT_H_


#include <IndustryStandard/Acpi61.h>
#include "AcpiTablePlatform.h"
#include "Register/PmcRegs.h"

//
// FADT Definitions
//
#ifndef EFI_ACPI_OEM_FADT_REVISION
#define EFI_ACPI_OEM_FADT_REVISION  0x00000000
#endif

//
// For ACPI 1.0 support
//
#define EFI_ACPI_INT_MODEL            0x00
/*
1 Desktop
2 Mobile
3 Workstation
4 Enterprise Server
5 SOHO Server
6 Appliance PC
7 Performance Server
*/
#define EFI_ACPI_PREFERRED_PM_PROFILE 0x02
#define EFI_ACPI_SCI_INT              0x0009
#define EFI_ACPI_SMI_CMD              0x00001804
#define ACPI_ENABLE     0x01
#define ACPI_DISABLE    0x00
#define EFI_ACPI_S4_BIOS_REQ          0x00
#define EFI_ACPI_PSTATE_CNT           0x00
#define EFI_ACPI_GPE1_BASE            0x00
#define EFI_ACPI_CST_CNT              0x00
#define EFI_ACPI_P_LVL2_LAT           0x0064
#define EFI_ACPI_P_LVL3_LAT           0x01F4
#define EFI_ACPI_FLUSH_SIZE           0x0400
#define EFI_ACPI_FLUSH_STRIDE         0x0010

#define EFI_ACPI_DUTY_OFFSET          0x01
#define EFI_ACPI_DUTY_WIDTH           0x03
#define EFI_ACPI_DAY_ALRM             0x0D
#define EFI_ACPI_MON_ALRM             0x00
#define EFI_ACPI_CENTURY              0x32

//
// IA-PC Boot Architecture Flags, see ACPI 6.1 table specification and Acpi61.h
// We should not use EFI_ACPI_6_1_8042 for legacy free platform, or XP reset fail. No impact to mobile.
//
#define EFI_ACPI_IAPC_BOOT_ARCH (EFI_ACPI_6_1_LEGACY_DEVICES)

//
// Fixed Feature Flags, see ACPI 6.1 table specification and Acpi6_1.h
//
#define EFI_ACPI_FIXED_FEATURE_FLAGS (\
  EFI_ACPI_6_1_USE_PLATFORM_CLOCK | \
  EFI_ACPI_6_1_WBINVD | \
  EFI_ACPI_6_1_FIX_RTC | \
  EFI_ACPI_6_1_RTC_S4 | \
  EFI_ACPI_6_1_SLP_BUTTON | \
  EFI_ACPI_6_1_PROC_C1 | \
  EFI_ACPI_6_1_RESET_REG_SUP  \
  )


//
// PM1A Event Register Block Generic Address Information
//
#define EFI_ACPI_PM1A_EVT_BLK_ADDRESS_SPACE_ID  EFI_ACPI_6_1_SYSTEM_IO
#define EFI_ACPI_PM1A_EVT_BLK_BIT_WIDTH         0x20
#define EFI_ACPI_PM1A_EVT_BLK_BIT_OFFSET        0x00
#define EFI_ACPI_PM1A_EVT_BLK_ADDRESS           ACPI_BASE_ADDRESS

//
// PM1B Event Register Block Generic Address Information
//
#define EFI_ACPI_PM1B_EVT_BLK_ADDRESS_SPACE_ID  EFI_ACPI_6_1_SYSTEM_IO
#define EFI_ACPI_PM1B_EVT_BLK_BIT_WIDTH         0x00
#define EFI_ACPI_PM1B_EVT_BLK_BIT_OFFSET        0x00
#define EFI_ACPI_PM1B_EVT_BLK_ADDRESS           0x0000000000000000

//
// PM1A Control Register Block Generic Address Information
//
#define EFI_ACPI_PM1A_CNT_BLK_ADDRESS_SPACE_ID  EFI_ACPI_6_1_SYSTEM_IO
#define EFI_ACPI_PM1A_CNT_BLK_BIT_WIDTH         0x10
#define EFI_ACPI_PM1A_CNT_BLK_BIT_OFFSET        0x00
#define EFI_ACPI_PM1A_CNT_BLK_ADDRESS           (EFI_ACPI_PM1A_EVT_BLK_ADDRESS + 0x04)

//
// PM1B Control Register Block Generic Address Information
//
#define EFI_ACPI_PM1B_CNT_BLK_ADDRESS_SPACE_ID  EFI_ACPI_6_1_SYSTEM_IO
#define EFI_ACPI_PM1B_CNT_BLK_BIT_WIDTH         0x00
#define EFI_ACPI_PM1B_CNT_BLK_BIT_OFFSET        0x00
#define EFI_ACPI_PM1B_CNT_BLK_ADDRESS           0x0000000000000000

//
// PM2 Control Register Block Generic Address Information
//
#define EFI_ACPI_PM2_CNT_BLK_ADDRESS_SPACE_ID   EFI_ACPI_6_1_SYSTEM_IO
#define EFI_ACPI_PM2_CNT_BLK_BIT_WIDTH          0x08
#define EFI_ACPI_PM2_CNT_BLK_BIT_OFFSET         0x00
#define EFI_ACPI_PM2_CNT_BLK_ADDRESS            (EFI_ACPI_PM1A_EVT_BLK_ADDRESS + 0x50)

//
// Power Management Timer Control Register Block Generic Address
// Information
//
#define EFI_ACPI_PM_TMR_BLK_ADDRESS_SPACE_ID    EFI_ACPI_6_1_SYSTEM_IO
#define EFI_ACPI_PM_TMR_BLK_BIT_WIDTH           0x20
#define EFI_ACPI_PM_TMR_BLK_BIT_OFFSET          0x00
#define EFI_ACPI_PM_TMR_BLK_ADDRESS             (EFI_ACPI_PM1A_EVT_BLK_ADDRESS + 0x08)

//
// General Purpose Event 0 Register Block Generic Address
// Information
//
#define EFI_ACPI_GPE0_BLK_ADDRESS_SPACE_ID      EFI_ACPI_6_1_SYSTEM_IO
#define EFI_ACPI_GPE0_BLK_BIT_WIDTH             0x100
#define EFI_ACPI_GPE0_BLK_BIT_OFFSET            0x00
#define EFI_ACPI_GPE0_BLK_ADDRESS               0x00001860

//
// General Purpose Event 1 Register Block Generic Address
// Information
//
#define EFI_ACPI_GPE1_BLK_ADDRESS_SPACE_ID      EFI_ACPI_6_1_SYSTEM_IO
#define EFI_ACPI_GPE1_BLK_BIT_WIDTH             0x00
#define EFI_ACPI_GPE1_BLK_BIT_OFFSET            0x00
#define EFI_ACPI_GPE1_BLK_ADDRESS               0x0000000000000000

//
// Reset Register Generic Address Information
//
#define EFI_ACPI_RESET_REG_ADDRESS_SPACE_ID     EFI_ACPI_6_1_SYSTEM_IO
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
