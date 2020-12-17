/** @file
Defines EcLite offsets used by EcLite driver

Copyright (c) 2017 - 2020, Intel Corporation. All rights reserved.<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#ifndef _ECLITE_FEATURE_H_
#define _ECLITE_FEATURE_H_

//
// FG1 - BAT1
//
#define ECLITE_B1ST_OFFSET 0   // 000: Battery State
#define ECLITE_B1PV_OFFSET 6   // 006: Battery present voltage
#define ECLITE_B1DC_OFFSET 8   // 008: Design Capacity
#define ECLITE_B1DV_OFFSET 12  // 012: Design Voltage
#define ECLITE_B1CC_OFFSET 14  // 014: Cycle count
#define ECLITE_B1T0_OFFSET 20  // 020: Battery Trip Point[0]
#define ECLITE_B1T1_OFFSET 22  // 022: Battery Trip Point[1]
#define ECLITE_B1WL_OFFSET 26  // 026: Battery Warning Level
#define ECLITE_B1LL_OFFSET 28  // 028: Battery Low Level

//
// FG1 - BAT2
//
#define ECLITE_B2ST_OFFSET 38  // 038: Battery State


//
// CHARGER 2
//
                               // 190-207: Reserved
//
// POWER BOSS
//
                               // 208: Reserved
#define ECLITE_PSRC_OFFSET 210 // 210: Current power source


//
// ECLite region commands
//
// Read command: This command is issued from bios to ECLite driver to initiate the Batch read operation from ISH to ECLite opregion
// Write command: This command is issued from bios to ECLite driver to initiate the Batch write operation from ECLite opregion to ISH
//
#define ECLITE_READ_COMMAND                    1
#define ECLITE_WRITE_COMMAND                   2

// ECLite Header Revision
#define ECLITE_HEADER_REVISION 1

//
// Size definition
//
#define  ECLITE_BYTES_COUNT_0   0
#define  ECLITE_BYTES_COUNT_1   1
#define  ECLITE_BYTES_COUNT_2   2
#define  ECLITE_BYTES_COUNT_3   3
#define  ECLITE_BYTES_COUNT_4   4
#define  ECLITE_BYTES_COUNT_5   5
#define  ECLITE_BYTES_COUNT_6   6
#define  ECLITE_BYTES_COUNT_7   7
#define  ECLITE_BYTES_COUNT_8   8
#define  ECLITE_BYTES_COUNT_9   9
#define  ECLITE_BYTES_COUNT_10  10
#define  ECLITE_BYTES_COUNT_11  11
#define  ECLITE_BYTES_COUNT_12  12
#define  ECLITE_BYTES_COUNT_13  13
#define  ECLITE_BYTES_COUNT_14  14
#define  ECLITE_BYTES_COUNT_15  15
#define  ECLITE_BYTES_COUNT_16  16
#define  ECLITE_BYTES_COUNT_17  17
#define  ECLITE_BYTES_COUNT_18  18
#define  ECLITE_BYTES_COUNT_19  19
#define  ECLITE_BYTES_COUNT_20  20
#define  ECLITE_BYTES_COUNT_21  21
#define  ECLITE_BYTES_COUNT_22  22
#define  ECLITE_BYTES_COUNT_23  23
#define  ECLITE_BYTES_COUNT_24  24
#define  ECLITE_BYTES_COUNT_25  25
#define  ECLITE_BYTES_COUNT_26  26
#define  ECLITE_BYTES_COUNT_27  27
#define  ECLITE_BYTES_COUNT_28  28
#define  ECLITE_BYTES_COUNT_29  29
#define  ECLITE_BYTES_COUNT_30  30
#define  ECLITE_BYTES_COUNT_31  31
#define  ECLITE_BYTES_COUNT_32  32
#define  ECLITE_BYTES_COUNT_256 256
#define  ECLITE_MAX_BYTES_SUPPORTED 384
//
// ISH to BIOS Notifications
//
#define  ECLITE_CHARGER_CONNECT_EVENT         1
#define  ECLITE_CHARGER_DISCONNECT_EVENT      2
#define  ECLITE_BATTERY_EVENT                 3
#define  ECLITE_SYSTEM0_TEMP_ALERT            4
#define  ECLITE_SYSTEM1_TEMP_ALERT            5
#define  ECLITE_SYSTEM2_TEMP_ALERT            6
#define  ECLITE_BATTERY1_TEMP_ALERT           7
#define  ECLITE_BATTERY2_TEMP_ALERT           8
#define  ECLITE_PMIC_TEMP_ALERT               9
#define  ECLITE_SYSTEM3_TEMP_ALERT            10
#define  ECLITE_BATTERY_CYCLE_COUNT_EVENT     14
#define  ECLITE_CPU_TEMPERATURE_UPDATE        15
#define  ECLITE_PSRC_CHANGE_EVENT             17

//
// BIOS to ISH Notifications
//
#define  ECLITE_DEFAULT_UPDATE                 0 // ISH Firmware Ignores the Command completion notfication if it is 0
#define  ECLITE_BATTERY_TRIP_POINT_UPDATE      1
#define  ECLITE_WARNING_BATTERY_LEVEL_UPDATE   2
#define  ECLITE_LOW_BATTERY_LEVEL_UPDATE       3
#define  ECLITE_CRITICAL_TEMP_CONFIG_UPDATE    4
#define  ECLITE_THERMAL_ALERT_THERHSOLD_UPDATE 6
#define  ECLITE_CHARGER_UPDATE                 12
#define  ECLITE_OS_EVENT_PWM_UPDATE            15

//
// Device specific macros
//
#define  ECLITE_PSRC_BIT_MASK                    7 // Currently only one Source  Type C charger (no AC Brick)
#define  ECLITE_AC_PRESENT                       1
#define  ECLITE_DC_PRESENT                       0

//
// Boot time BIOS to ISH Support
//
typedef union {
 UINT32  Header;
 struct {
   UINT32  HeaderVersion       : 2;  //< 0:1   - Header version
   UINT32  DataType            : 2;  //< 2:3   - Data Type 0x01: Data , 0x02: Event Notification
   UINT32  DataReadWrite       : 2;  //< 4:5   - Data Read/Write 0x01: Read 0x02: Write
   UINT32  DataReadWriteOffset : 9;  //< 6:14  - Data Read/write Offset
   UINT32  DataLength          : 9;  //< 15:23 - Data payload Size in Bytes
   UINT32  CompletionCode      : 8;  //< 24:31 - Completion code
 } Fields;
} HECI_ECLITE_HEADER;

typedef struct {
  HECI_ECLITE_HEADER EcLiteHeader;
  UINT8              *Data; //< Data buffer to be sent/updated based on the datalength
} HECI_BIOS_ISH_MSG;

#endif // _ECLITE_FEATURE_H_
