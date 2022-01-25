/** @file

 Copyright (c) 2021, Intel Corporation. All rights reserved.<BR>
 SPDX-License-Identifier: BSD-2-Clause-Patent
**/


#ifndef _LPIT_H_
#define _LPIT_H_


#include <IndustryStandard/LowPowerIdleTable.h>

#define EFI_ACPI_LOW_POWER_IDLE_TABLE_REVISION 0x1

#define ACPI_OEM_LPIT_REVISION               0x00000000

// LPI state disable default, will enable per BIOS setup selection
#define ACPI_LOW_POWER_IDLE_DEFAULT_FLAG        {{ 0x1, 0x0, 0x0 }}
#define ACPI_LOW_POWER_IDLE_RES_FREQ_TSC        0x0

//
// LPI state count
//
#define ACPI_LPI_STATE_COUNT            3

//
// LPI TRIGGER (HW C10)
//
#define ACPI_LPI_TRIGGER {0x7F,0x1,0x2,0x0,0x60}

//
// LPI residency counter (HW C10)
//
#define  ACPI_LPI_RES_C10_COUNTER    {0x7F,64,0x0,0x0,0x632}

//
// LPI residency counter (SLP S0)
//
#define ACPI_LPI_RES_SLP_S0_COUNTER {0x0,32,0x0,0x3,0xFF}

//
// LPI residency counter (PS_ON)
//
#define ACPI_LPI_RES_PS_ON_COUNTER {0x0,32,0x0,0x3,0xFF}

//
// LPI DUMMY COUNTER
//
#define  ACPI_DUMMY_RES_COUNTER   {0x0,0,0x0,0x0,0x0}

//
// LPI break-even residency in us (HW C10)
//
#define  ACPI_LPI_MIN_RES   30000

//
// LPI latency in us (HW C10)
//
#define  ACPI_LPI_LATENCY   3000

//
// LPI ID (HW C10)
//
#define  ACPI_LPI_C10_ID   0

//
// LPI ID (HW SLP S0)
//
#define  ACPI_LPI_SLP_S0_ID   1
//
// LPI ID (HW PS ON)
//
#define  ACPI_LPI_PS_ON_ID   2

//
// LPI Residency Counter Valid Status
//
// The table refect which counters are valid to OS
// e.g. if C10 bit value is 1, it means C10 counter is valid
//
typedef union {
  struct {
    UINT8 C10      : 1;  //bit0
    UINT8 Slp_S0   : 1;  //bit1
    UINT8 Ps_On    : 1;  //bit2
    UINT8 Reserved : 5;
  } Bits;
  UINT8 Data8;
} LPI_COUNTER_VALID_STS;

//
//  LPI ACPI table header
//
#pragma pack(1)

typedef struct _ACPI_LOW_POWER_IDLE_TABLE {
  EFI_ACPI_DESCRIPTION_HEADER         Header;
  ACPI_LPI_NATIVE_CSTATE_DESCRIPTOR   LpiStates[ACPI_LPI_STATE_COUNT];
} ACPI_LOW_POWER_IDLE_TABLE;

#pragma pack()


#endif //_LPIT_H_
