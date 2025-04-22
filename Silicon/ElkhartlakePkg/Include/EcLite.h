/** @file
Defines EcLite offsets used by EcLite driver

Copyright (c) 2017 - 2021, Intel Corporation. All rights reserved.<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#ifndef _ECLITE_FEATURE_H_
#define _ECLITE_FEATURE_H_

//
// FG1 - BAT1
//
#define ECLITE_B1ST_OFFSET 0   // 000: Battery State
#define ECLITE_B1PR_OFFSET 2   // 002: Battery Present Rate
#define ECLITE_B1RC_OFFSET 4   // 004: Battery Remaining Capacity
#define ECLITE_B1PV_OFFSET 6   // 006: Battery present voltage
#define ECLITE_B1DC_OFFSET 8   // 008: Design Capacity
#define ECLITE_B1FC_OFFSET 10  // 010: Full charge capacity
#define ECLITE_B1DV_OFFSET 12  // 012: Design Voltage
#define ECLITE_B1CC_OFFSET 14  // 014: Cycle count
#define ECLITE_B1CW_OFFSET 16  // 016: Design Capacity of warning
#define ECLITE_B1LW_OFFSET 18  // 018: Design Capacity of Low
#define ECLITE_B1T0_OFFSET 20  // 020: Battery Trip Point[0]
#define ECLITE_B1T1_OFFSET 22  // 022: Battery Trip Point[1]
#define ECLITE_B1DR_OFFSET 24  // 024: Battery discharge Rate[0]
#define ECLITE_B1WL_OFFSET 26  // 026: Battery Warning Level
#define ECLITE_B1LL_OFFSET 28  // 028: Battery Low Level
                               // 030-037: Reserved

//
// FG1 - BAT2
//
#define ECLITE_B2ST_OFFSET 38  // 038: Battery State
#define ECLITE_B2PR_OFFSET 40  // 040: Battery Present Rate
#define ECLITE_B2RC_OFFSET 42  // 042: Battery Remaining Capacity
#define ECLITE_B2PV_OFFSET 44  // 044: Battery present voltage
#define ECLITE_B2DC_OFFSET 46  // 046: Design Capacity
#define ECLITE_B2FC_OFFSET 48  // 048: Full charge capacity
#define ECLITE_B2DV_OFFSET 50  // 050: Design Voltage
#define ECLITE_B2CC_OFFSET 52  // 052: Cycle count
#define ECLITE_B2CW_OFFSET 54  // 054: Design Capacity of warning
#define ECLITE_B2LW_OFFSET 56  // 056: Design Capacity of low
#define ECLITE_B2T0_OFFSET 58  // 058: Battery Trip Point[0]
#define ECLITE_B2T1_OFFSET 60  // 060: Battery Trip Point[1]
#define ECLITE_B2DR_OFFSET 62  // 062: Battery discharge Rate[0]
#define ECLITE_B2WL_OFFSET 64  // 064: Battery Warning Level
#define ECLITE_B2LL_OFFSET 66  // 066: Battery Low Level
                               // 068-075: Reserved

//
// DPTF
//
#define ECLITE_S0T1_OFFSET 76  // 076: System0 Temperature 1
#define ECLITE_S1T1_OFFSET 78  // 078: System1 Temperature 1
#define ECLITE_S2T1_OFFSET 80  // 080: System2 Temperature 1
#define ECLITE_S3T1_OFFSET 82  // 082: System3 Temperature
                               // 084-099: Reserved

#define ECLITE_S0A3_OFFSET 100 // 100: Sys0_ALERT3
#define ECLITE_S0A1_OFFSET 102 // 102: Sys0_ALERT1
#define ECLITE_S1A3_OFFSET 104 // 104: Sys1_ALERT3
#define ECLITE_S1A1_OFFSET 106 // 106: Sys1_ALERT1
#define ECLITE_S2A3_OFFSET 108 // 108: Sys2_ALERT3
#define ECLITE_S2A1_OFFSET 110 // 110: Sys2_ALERT1
#define ECLITE_S3A3_OFFSET 112 // 112: Sys3_ALERT3
#define ECLITE_S3A1_OFFSET 114 // 114: Sys3_ALERT1
                               // 116-147: Reserved

#define ECLITE_S0CT_OFFSET 148 // 148: SYS0_CRIT_TEMP_THR
#define ECLITE_S1CT_OFFSET 150 // 150: SYS1_CRIT_TEMP_THR
#define ECLITE_S2CT_OFFSET 152 // 152: SYS2_CRIT_TEMP_THR
#define ECLITE_S3CT_OFFSET 154 // 154: BATT0_CRIT_TEMP_THR
                               // 156-173 Reserved

#define ECLITE_CTCT_OFFSET 174 // 174: CPU Temp High Threshold
#define ECLITE_CTHT_OFFSET 176 // 176: CPU Therm Temp

//
// CHARGER 1
//
#define ECLITE_C1CC_OFFSET 178 // 178: Charger 1 CC
#define ECLITE_C1CV_OFFSET 180 // 180: Charger 1 CV
#define ECLITE_C1IN_OFFSET 182 // 182: Charger 1 INLIM

//
// CHARGER 2
//
#define ECLITE_C2CC_OFFSET 184 // 184: Charger 2 CC
#define ECLITE_C2CV_OFFSET 186 // 186: Charger 2 CV
#define ECLITE_C2IN_OFFSET 188 // 188: Charger 2 INLIM
                               // 190-207: Reserved
//
// POWER BOSS
//
                               // 208: Reserved
#define ECLITE_PSRC_OFFSET 210 // 210: Current power source
                               // 212-223: Reserved

//
// PROCESSOR
//
                               // 224-226: Reserved

//
//BATTERY 1
//
                               // 228 - 243: Reserved

//
//BATTERY 2
//
                               // 244 - 259: Reserved

//
// UCSI
//
#define ECLITE_MGO1_OFFSET 260 // 260: MessageOut1
#define ECLITE_MGO2_OFFSET 264 // 264: MessageOut2
#define ECLITE_MGO3_OFFSET 268 // 268: MessageOut3
#define ECLITE_MGO4_OFFSET 272 // 272: MessageOut4
#define ECLITE_CTL1_OFFSET 276 // 272: Control1
#define ECLITE_CTL2_OFFSET 280 // 280: Control2
#define ECLITE_MGI1_OFFSET 284 // 284: MessageIn1
#define ECLITE_MGI2_OFFSET 288 // 288: MessageIn2
#define ECLITE_MGI3_OFFSET 292 // 292: MessageIn3
#define ECLITE_MGI4_OFFSET 296 // 296: MessageIn4
#define ECLITE_CCI_OFFSET  300 // 300: CCI
#define ECLITE_UCRV_OFFSET 304 // 304: UCSI Revision
                               // 260-321: Reserved

//
// USB Mode
//
#define ECLITE_USBM_OFFSET 322 // 322: Host/Device mode
                               // 323: Reserved

//
// Haptics
//
                               // 324-325: Reserved

//
// BIOS Mode
//
                               // 326-327: Reserved

//
// FAN Control
//
#define ECLITE_FPWM_OFFSET                     328 // 328: PWM control for FAN
#define ECLITE_TACO_OFFSET                     330 // 330: TACHO control for FAN
                                                   // 332-332: Reserved

//
// ECLite Event Notify Config
//
#define ECLITE_ENCG_OFFSET                     333 // 333: ECLite Event Notify Config
                                                   // 334-384: Reserved

//
// ECLite region commands
//
// Read command: This command is issued from bios to ECLite driver to initiate the Batch read operation from ISH to ECLite opregion
// Write command: This command is issued from bios to ECLite driver to initiate the Batch write operation from ECLite opregion to ISH
//
#define ECLITE_READ_COMMAND                    1
#define ECLITE_WRITE_COMMAND                   2
#define ECLITE_MAX_COMMAND                     255

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
#define  ECLITE_DEVICE_CONNECT_EVENT          11
#define  ECLITE_UCSI_UPDATE_EVENT             12
#define  ECLITE_DEVICE_DISCONNECT_EVENT       13
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
#define  ECLITE_UCSI_UPDATE                    10
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
