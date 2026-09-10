/** @file
  EC Private command and definitions.

  This private header file provides EC command sample interface.

  There may be different EC command for using different Embedded Controller.
  Make sure the Silicon and design the command on your own.

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#ifndef _EC_COMMANDS_H_
#define _EC_COMMANDS_H_

//
// Timeout if EC command/data fails
//
#define EC_TIME_OUT                  0x20000

//
// The EC implements an embedded controller interface at ports 0x60/0x64 and a ACPI compliant
// system management controller at ports 0x62/0x66. Port 0x66 is the command and status port,
// port 0x62 is the data port.
//
#define EC_D_PORT        0x62
#define EC_C_PORT        0x66

//
// Status Port 0x62
//
#define EC_S_OVR_TMP     0x80    // Current CPU temperature exceeds the threshold
#define EC_S_SMI_EVT     0x40    // SMI event is pending
#define EC_S_SCI_EVT     0x20    // SCI event is pending
#define EC_S_BURST       0x10    // EC is in burst mode or normal mode
#define EC_S_CMD         0x08    // Byte in data register is command/data
#define EC_S_IGN         0x04    // Ignored
#define EC_S_IBF         0x02    // Input buffer is full/empty
#define EC_S_OBF         0x01    // Output buffer is full/empty

//
// EC commands that are issued to the EC through the command port (0x66).
// New commands and command parameters should only be written by the host when IBF=0.
// Data read from the EC data port is valid only when OBF=1.
//
#define EC_C_STC_CONFIG                           0x05    // Set the Skin Thermal Control Config.
                                                          // Send:
                                                          // 5 bytes: byte 0: Sensor ID.
                                                          //          byte 1: Target Temperature
                                                          //          byte 2: bit0    - Enable the control.
                                                          //                  bit1-3  - Gain: Set the aggressiveness of the control loop.
                                                          //                            0 - graceful.
                                                          //                            7 - aggressive.
                                                          //                  bit4-7  - Reserved.
                                                          //          byte 3: Minium Performance Level.
                                                          //          byte 4: Reserved.
#define EC_C_SMC_GET_MODE                         0x09    // Return a key to indentify SNC & State info.
                                                          // Receive:
                                                          // 4 bytes: byte 0-2: "KSC"
                                                          //          byte 3: bit7    - 1 = Geyserville support enabled
                                                          //                  bit6    - 1 = Thermal states locked
                                                          //                  bit5    - 1 = Extended thermal sensor supported
                                                          //                  bit4    - 1 = MAF mode
                                                          //                  bit3    - 1 = SAF Mode
                                                          //                  bit2    - reserved for future use.
                                                          //                  bit1    - 1 = PECI access mode
                                                          //                  bit0    - 1 = ACPI mode
#define EC_C_SWITCH_STATUS                        0x0A    // Get switch status, return 1 byte data.
                                                          // Receive:
                                                          // 1 byte:  byte 0: bit7-6  - Reserved
                                                          //                  bit5    - Docking status
                                                          //                  bit4    - Actual power source AC/DC
                                                          //                  bit3    - Home button status
                                                          //                  bit2    - NMI jumper remove
                                                          //                  bit1    - SW2 or virtual battery status
                                                          //                  bit0    - Lid switch status, 1 = closed
#define GET_HW_PERIPHERALS_STS                    0x0B    // Get Hardware Peripheral status
                                                          // Receive:
                                                          // 2 bytes: byte 0: Fan present
                                                          //          byte 1: Thermal sensors present
                                                          // Each bit in individual bytes indicate
                                                          // presnece of respective indexed peripheral.
                                                          //
                                                          // For instance, if Bit0 & Bit2 in byte 0 are set,
                                                          // then, fan#1 & fan#3 are present.
#define EC_C_LAST_SHUTDOWN_STATUS                 0x0C    // Get the reason of the last shutdown
                                                          // Receive:
                                                          // 1 byte:  byte 0: bit0    - last shutdown was due to critical temperature (0 = False, 1 = True)
#define EC_C_BOARD_INFO                           0x0D    // Get the board info in the lower 3 bits
                                                          // Receive:
                                                          // 2 bytes: board ID, fab ID, Bom ID ... read from board
#define EC_C_RESTORATION_PL4_VALUES               0x0E    // Notify EC to restore of PL4 offset values.
#define EC_C_PLN_ENABLE                           0x0F    // Enable or disable PLN
                                                          // Send:
                                                          // 1 byte
#define EC_C_SET_SYS_CHANGING_CFG                 0x12    //
#define EC_C_SET_CHARGING_METHOD                  0x13    // Command to specify charging method. (Normal charging or Fast charging (DFCT))
                                                          // Send:
                                                          // 1 byte:  0x01  - Fast charging enable
                                                          //          0x00  - Fast charging disable
#define EC_C_PWR_BOSS_OK                          0x15
#define EC_C_UPDATE_UVTH                          0x17
#define EC_C_TDP_WATTS                            0x18    // Pass TDP value in Watts to EC. EC will update UCSI bit according to charging rate when USBC adapter plug-in.
#define EC_C_UPDATE_RPM_TRIP_THRESHOLDS           0x19
#define EC_C_UPDATE_PWM                           0x1A    // Update the FAN Speed
                                                          // Ec Ram data: PWM port select: 0x41, 0x42
                                                          //              PWM Iinitial value: 0x43
                                                          //              PWM ending value: 0x44
                                                          //              PWM ms delay between steps: 0x45
                                                          // Update the PWM to the percent value in PWM ending value to the select port.
#define EC_C_SET_FAN_POLICY_MODE                  0x1C
#define EC_C_SET_PD_FW_UPDATE_MODE                0x1D    // Commands EC to enter or exit Retimer FW Update Mode for ALL PD controller
#define EC_C_GET_PD_FW_UPDATE_MODE                0x1E    // Commands EC to get Status of ALL PD controller Retimer FW Update Mode
#define EC_C_USBC_RETIMER                         0x1F    // notify EC of Retimer compliance mode enablement status
#define EC_C_ENABLE_POWER_BTN                     0x23    // Disable power button functionality
#define EC_C_DISABLE_POWER_BTN                    0x24    // Enable power button functionality
#define EC_C_CS_LOW_POWER_ENABLE                  0x27    // Enter Low Power Mode
                                                          // Send:
                                                          // 1 byte:  1 - enable CS low power mode for EC
#define EC_C_SET_DEEP_SLEEP                       0x28    // Enable deep sleep.
                                                          // Send:
                                                          // 1 byte:  Data to indicate the deep sleep type.
#define EC_C_SET_DSW_MODE                         0x29    // Set DSW mode with data temp input parameter
                                                          // Send:
                                                          // 1 byte:  0x01  - DSW enabled in S5/DC only
                                                          //          0x02  - DSW enabled in SA/AC-DC
                                                          //          0x03  - DSW enabled in S4S5/DC only
                                                          //          0x04  - DSW enabled in S4S5/AC-DC
                                                          //          0x05  - DSW enabled in S3S5/DC
                                                          //          0x06  - FF-DSW enabled in S4S5/DC only
                                                          //          0x07  - FF-DSW enabled in S3S5/DC
#define EC_C_GET_DSW_MODE                         0x2A    // Get DSW configuration data
                                                          // Receive:
                                                          // 1 byte:  0x01  - DSW enabled in S5/DC only
                                                          //          0x02  - DSW enabled in SA/AC-DC
                                                          //          0x03  - DSW enabled in S4S5/DC only
                                                          //          0x04  - DSW enabled in S4S5/AC-DC
                                                          //          0x05  - DSW enabled in S3S5/DC
                                                          //          0x06  - FF-DSW enabled in S4S5/DC only
                                                          //          0x07  - FF-DSW enabled in S3S5/DC
#define EC_C_LOW_POWER_ENTRY                      0x2C    // CS entry command (low power enable)
#define EC_C_LOW_POWER_EXIT                       0x2D    // Exit Low Power Mode
#define EC_C_P_G3_SET_MODE                        0x2E    // Enable/disable PseudoG3.
                                                          // Send:
                                                          // 1 byte: PG3 state value.
#define EC_C_P_G3_PROGRAM_COUNTER                 0x2F    // Pseudo G3 program counter.
                                                          // InfoLess: input, output info loss
#define EC_C_SET_BAT_THRESHOLD                    0x34    // Set battery threshold
                                                          // Send:
                                                          // 1 byte:  battery threshold
#define EC_C_GET_WAKE_STATUS                      0x35    // Get wake status
                                                          // Receive:
                                                          // 1 byte:  wake reason from S3 (Note: supported base on platform
                                                          //          requirement and hardware support)
                                                          //          bit0: low battery wake flag
                                                          //          bit1: LID wake flag
                                                          //          bit2: keyboard/mouse wake flag (PS2/Scan matrix)
                                                          //          bit3: timer wake flag
                                                          //          bit4: PCIE wake flag
                                                          //          bit5: LAN wake flag
                                                          //          bit6: Sensor hub wake flag
#define EC_C_CLEAR_WAKE_STATUS                    0x36    // Clear wake status (Legacy)
#define EC_C_CHARGE_RATE_CHANGE_NOTIFY            0x37    // Notify change in battery charge rate. DPTF req. to change battery
                                                          // charge rate.
#define EC_C_BTN_SCI_CTL                          0x38    // Update the Windows * 5 button notification control for: Power
                                                          // Windows*, Vol up, Vol down, Rot lock
                                                          // Send:
                                                          // 1 byte:  bit0  - power button bit
                                                          //          bit1  - home button
                                                          //          bit2  - volume up
                                                          //          bit3  - volume down
                                                          //          bit4  - rotation lock
#define EC_C_FAIL_SAFE_FAN_CTRL                   0x39    // Set Fail safe FAN speed for given Cpu temp
#define EC_C_SET_PECI_MODE                        0x3C    // Switch PECI mode between Legacy and eSPI mode
                                                          // Send:
                                                          // 1 byte:  0 - Legacy mode
                                                          //          1 - ESPI mode
#define EC_C_SET_TMP_THRESHOLD                    0x4A    // Set temperature threshold for DPTF notification. Value get updated
                                                          // from acpi name space
#define EC_C_DIS_PCH_DTS_READ                     0x4E    // Disable PCH DTS reading
#define EC_C_SET_CRITICAL_TEMP                    0x58    // Set the critical shutdown temperature
                                                          // Send:
                                                          // 1 byte:  value of the critical shutdown temperature
#define EC_C_START_PECI_TEST                      0x65    // Start PECI test
#define EC_C_STOP_PECI_TEST                       0x66    // Stop PECI test
#define EC_C_ACPI_PORT_WRT_PECI_DATA              0x67    // ACPI port - write PECI data
#define EC_C_ACPI_PORT_RD_PECI_DATA               0x68    // ACPI port - read PECI data
#define EC_C_ACPI_PORT_GET_ERR_COUNT              0x69    // ACPI port - get error count
#define EC_C_ACPI_PORT_RD_CMD_STATUS              0x6A    // ACPI port - read command status
                                                          // Receive:
                                                          // 1 byte
#define EC_C_SMI_QUERY                            0x70    // *PlaceHolder The host reads the data port to retrieve the notifications
#define EC_C_EN_PWR_BTN_NOTIFY                    0x73    // Enable power button notification
#define EC_C_DIS_PWR_BTN_NOTIFY                   0x74    // Disable power button notification
#define EC_C_READ_MEM                             0x80    // Read the EC memory
                                                          // Send:
                                                          // 1 byte:  ACPI offset (EC RAM offset)
                                                          // Receive:
                                                          // 1 byte:  data read
#define EC_C_WRITE_MEM                            0x81    // Write the EC memory
                                                          // Send:
                                                          // 2 bytes: byte 0: the ACPI address (EC RAM offset)
                                                          //          byte 1: data to write
#define EC_C_ACPI_TURST_CMD                       0x82    // ACPI burst command
#define EC_C_ACPI_NORMAL_EC                       0x83    // ACPI normal command
#define EC_C_ACPI_QUERY_EC                        0x84    // ACPI query command
#define EC_C_EC_REVISION                          0x90    // Get the revision for the EC
                                                          // Receive:
                                                          // 2 bytes: EC version
#define EC_C_ACPI_ENABLE                          0xAA    // Enable ACPI mode
#define EC_C_ACPI_DISABLE                         0xAB    // Disable ACPI mode
#define EC_C_SX_ENTER                             0xAC
#define EC_C_SX_EXIT                              0xAD
#define EC_C_SMI_DISABLE                          0xBC    // *PlaceHolder SMI generation by the EC is disabled.
#define EC_C_SMI_ENABLE                           0xBD    // *PlaceHolder SMI generation by the EC is enabled.
#define EC_C_MODULAR_IO_INFO_CMD                  0xDE    // Get Modular USBC IO Config data
                                                          // Receive:
                                                          // 8 bytes: TCPx port config data
                                                          // [15:0]  - TCP0
                                                          // [31:16] - TCP1
                                                          // [47:32] - TCP2
                                                          // [63:48] - TCP3
#define EC_C_PD_VERSION_INFO_CMD                  0xDF    // Get PD information data
                                                          // Send:
                                                          // 1 byte: PD index
                                                          // Receive:
                                                          // 4 bytes: PD info data
#define EC_C_PD_VERSION_INFO_CMD_4BYTE            0xDF
#define EC_C_UCSI_CMD_HNDLR                       0xE0    // UCSI command interrupt
#define EC_C_UCSI_READ_VERSION                    0xE1    // USCI spec version
                                                          // Receive:
                                                          // 2 bytes: UCSI revision
#define EC_C_USBC_HOST_FLAGS                      0xE2    // Passing USBC host flags to EC.
#define EC_C_USBC_SX_EXIT_WAIT                    0xE3    // Send this command during boot and wait for response
#define EC_C_WDT_WRITE                            0xE4    // write command to EC WDT through 0x66 EC_C_PORT
#define EC_C_WDT_READ                             0xE5    // read command to EC WDT through 0x66 EC_C_PORT
#define EC_C_READ_ACPI_SPACE                      0xEA    // @Need to move to PVT port Read ACPI Space
#define EC_C_WRITE_ACPI_SPACE                     0xEB    // @Need to move to PVT port Write ACPI Space
#define EC_C_BATTERY_SHIP_MODE                    0xF2    // @EC test command Battery ship Mode
#define EC_C_SET_G3TOS5                           0xF3    // *PlaceHolder Set KSC Misc Flags BIT0(G3ToS5).
                                                          // Send:
                                                          // 1 byte: state after G3
#define EC_C_SET_PROGRAMMABLE_POWER_SUPPLY_MODE   0xF8
#define EC_C_BIOS_FAN_CONTROL                     0xFE
#define EC_C_RESET_KSC                            0xFF    // Reset EC.
                                                          // Send:
                                                          // 1 byte:  0x5A to do the platform reset

//
//  EC_C_SET_CHARGING_METHOD (0x13).
//
#define EC_D_NORMAL_CHARGING           0x01    // Select normal charging method
#define EC_D_FAST_CHARGING             0x02    // Select fast charging (DFCT) method

//
// Definition for EC WDT timeout. EC_C_WDT_WRITE (0xE4).
//
#define EC_WDT_ENABLE         0x02    // 10(b) = ARM WDT
#define EC_WDT_DISABLE        0x01    // 01(b) = Disarm WDT


#endif
