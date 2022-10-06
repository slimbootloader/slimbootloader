/** @file
  This file contains a structure definition for the ACPI 5.0 Firmware ACPI
  Control Structure (FACS).  The contents of this file should only be modified
  for bug fixes, no porting is required.

  Copyright (c) 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __ACPI_FACP_H__
#define __ACPI_FACP_H__

//
// FADT Definitions
//
#define EFI_ACPI_OEM_FADT_REVISION    0x00000000

#define EFI_ACPI_PREFERRED_PM_PROFILE 0x04

#define EFI_ACPI_SCI_INT              0x0009
#define EFI_ACPI_SMI_CMD              0x0504

#define EFI_ACPI_ACPI_ENABLE          0x01
#define EFI_ACPI_ACPI_DISABLE         0x00
#define EFI_ACPI_S4_BIOS_REQ          0x00
#define EFI_ACPI_CST_CNT              0x00

#define EFI_ACPI_PSTATE_CNT           0x00
#define EFI_ACPI_GPE1_BASE            (EFI_ACPI_GPE1_BLK_BIT_WIDTH / 2)
#define EFI_ACPI_P_LVL2_LAT           0x0065
#define EFI_ACPI_P_LVL3_LAT           0x03E9

#define EFI_ACPI_FLUSH_SIZE           0x0000
#define EFI_ACPI_FLUSH_STRIDE         0x0000

#define EFI_ACPI_DUTY_OFFSET          0x01
#define EFI_ACPI_DUTY_WIDTH           0x00
#define EFI_ACPI_DAY_ALRM             0x0D
#define EFI_ACPI_MON_ALRM             0x00
#define EFI_ACPI_CENTURY              0x32

#define EFI_ACPI_IAPC_BOOT_ARCH       EFI_ACPI_5_0_8042

//
// Fixed Feature Flags
//
#define EFI_ACPI_FIXED_FEATURE_FLAGS  (EFI_ACPI_5_0_RESET_REG_SUP | \
                                      EFI_ACPI_5_0_RTC_S4 | \
                                      EFI_ACPI_5_0_SLP_BUTTON | \
                                      EFI_ACPI_5_0_WBINVD | \
                                      EFI_ACPI_5_0_PROC_C1)

//
// PM1A Event Register Block Generic Address Information
//
#define EFI_ACPI_PM1A_EVT_BLK_ADDRESS_SPACE_ID  EFI_ACPI_5_0_SYSTEM_IO
#define EFI_ACPI_PM1A_EVT_BLK_BIT_WIDTH         0x20
#define EFI_ACPI_PM1A_EVT_BLK_BIT_OFFSET        0x00
#define EFI_ACPI_PM1A_EVT_BLK_ADDRESS           0x00000500

//
// PM1B Event Register Block Generic Address Information
//
#define EFI_ACPI_PM1B_EVT_BLK_ADDRESS_SPACE_ID  EFI_ACPI_5_0_SYSTEM_IO
#define EFI_ACPI_PM1B_EVT_BLK_BIT_WIDTH         0x00
#define EFI_ACPI_PM1B_EVT_BLK_BIT_OFFSET        0x00
#define EFI_ACPI_PM1B_EVT_BLK_ADDRESS           0x00000000

//
// PM1A Control Register Block Generic Address Information
//
#define EFI_ACPI_PM1A_CNT_BLK_ADDRESS_SPACE_ID  EFI_ACPI_5_0_SYSTEM_IO
#define EFI_ACPI_PM1A_CNT_BLK_BIT_WIDTH         0x10
#define EFI_ACPI_PM1A_CNT_BLK_BIT_OFFSET        0x00
#define EFI_ACPI_PM1A_CNT_BLK_ADDRESS           (EFI_ACPI_PM1A_EVT_BLK_ADDRESS + 0x04)

//
// PM1B Control Register Block Generic Address Information
//
#define EFI_ACPI_PM1B_CNT_BLK_ADDRESS_SPACE_ID  EFI_ACPI_5_0_SYSTEM_IO
#define EFI_ACPI_PM1B_CNT_BLK_BIT_WIDTH         0x00
#define EFI_ACPI_PM1B_CNT_BLK_BIT_OFFSET        0x00
#define EFI_ACPI_PM1B_CNT_BLK_ADDRESS           0x00

//
// PM2 Control Register Block Generic Address Information
//
#define EFI_ACPI_PM2_CNT_BLK_ADDRESS_SPACE_ID   EFI_ACPI_5_0_SYSTEM_IO
#define EFI_ACPI_PM2_CNT_BLK_BIT_WIDTH          0x08
#define EFI_ACPI_PM2_CNT_BLK_BIT_OFFSET         0x00
#define EFI_ACPI_PM2_CNT_BLK_ADDRESS            (EFI_ACPI_PM1A_EVT_BLK_ADDRESS + 0x50)

//
// Power Management Timer Control Register Block Generic Address
// Information
//
#define EFI_ACPI_PM_TMR_BLK_ADDRESS_SPACE_ID    EFI_ACPI_5_0_SYSTEM_IO
#define EFI_ACPI_PM_TMR_BLK_BIT_WIDTH           0x20
#define EFI_ACPI_PM_TMR_BLK_BIT_OFFSET          0x00
#define EFI_ACPI_PM_TMR_BLK_ADDRESS             (EFI_ACPI_PM1A_EVT_BLK_ADDRESS + 0x08)

//
// General Purpose Event 0 Register Block Generic Address
// Information
//
#define EFI_ACPI_GPE0_BLK_ADDRESS_SPACE_ID      EFI_ACPI_5_0_SYSTEM_IO
#define EFI_ACPI_GPE0_BLK_BIT_WIDTH             0x80
#define EFI_ACPI_GPE0_BLK_BIT_OFFSET            0x00
#define EFI_ACPI_GPE0_BLK_ADDRESS               (EFI_ACPI_PM1A_EVT_BLK_ADDRESS + 0x60)

//
// General Purpose Event 1 Register Block Generic Address
// Information
//
#define EFI_ACPI_GPE1_BLK_ADDRESS_SPACE_ID      EFI_ACPI_5_0_SYSTEM_IO
#define EFI_ACPI_GPE1_BLK_BIT_WIDTH             0x0
#define EFI_ACPI_GPE1_BLK_BIT_OFFSET            0x0
#define EFI_ACPI_GPE1_BLK_ADDRESS               0x0

//
// Sleep Control Register Block Generic Address Information
//
#define EFI_ACPI_SLP_CNT_BLK_ADDRESS_SPACE_ID   EFI_ACPI_5_0_SYSTEM_IO
#define EFI_ACPI_SLP_CNT_BLK_BIT_WIDTH          0x0
#define EFI_ACPI_SLP_CNT_BLK_BIT_OFFSET         0x0
#define EFI_ACPI_SLP_CNT_BLK_ADDRESS            0x0

//
// Sleep Status Register Block Generic Address Information
//
#define EFI_ACPI_SLP_STS_BLK_ADDRESS_SPACE_ID   EFI_ACPI_5_0_SYSTEM_IO
#define EFI_ACPI_SLP_STS_BLK_BIT_WIDTH          0x0
#define EFI_ACPI_SLP_STS_BLK_BIT_OFFSET         0x0
#define EFI_ACPI_SLP_STS_BLK_ADDRESS            0x0

//
// Reset Register Generic Address Information
//
#define EFI_ACPI_RESET_REG_ADDRESS_SPACE_ID     EFI_ACPI_5_0_SYSTEM_IO
#define EFI_ACPI_RESET_REG_BIT_WIDTH            0x08
#define EFI_ACPI_RESET_REG_BIT_OFFSET           0x00
#define EFI_ACPI_RESET_REG_ADDRESS              0x00000CF9
#define EFI_ACPI_RESET_VALUE                    0x06

//
// Number of bytes decoded by PM1 event blocks (a and b)
//
#define EFI_ACPI_PM1_EVT_LEN  ((EFI_ACPI_PM1A_EVT_BLK_BIT_WIDTH + EFI_ACPI_PM1B_EVT_BLK_BIT_WIDTH) / 8)

//
// Number of bytes decoded by PM1 control blocks (a and b)
//
#define EFI_ACPI_PM1_CNT_LEN  ((EFI_ACPI_PM1A_CNT_BLK_BIT_WIDTH + EFI_ACPI_PM1B_CNT_BLK_BIT_WIDTH) / 8)

//
// Number of bytes decoded by PM2 control block
//
#define EFI_ACPI_PM2_CNT_LEN  (EFI_ACPI_PM2_CNT_BLK_BIT_WIDTH / 8)

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

#endif /* __ACPI_FACP_H__ */
