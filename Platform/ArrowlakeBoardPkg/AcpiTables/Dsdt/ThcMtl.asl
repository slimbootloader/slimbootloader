/** @file

  Copyright (c) 2024, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#define THC0_ADR                           0x00100000
#define THC0_MODE                          TMD0
#define THC0_WAKE_INT                      TIN0
#define THC0_RST_PAD                       T010
#define THC0_RST_TRIGGER                   T020
#define THC0_CONNECTION_SPEED              T030
#define THC0_INPUT_REPORT_HEADER_ADDRESS   T040
#define THC0_INPUT_REPORT_BODY_ADDRESS     T050
#define THC0_OUTPUT_REPORT_ADDRESS         T060
#define THC0_READ_OPCODE                   T070
#define THC0_WRITE_OPCODE                  T080
#define THC0_FLAGS                         T090
#define THC0_ACTIVE_LTR                    T0A0
#define THC0_IDLE_LTR                      T0B0
#define THC0_LIMIT_PACKET_SIZE             T0C0
#define THC0_PERFORMANCE_LIMITATION        T0D0
#define THC0_DISPLAY_FRAME_SYNC_PERIOD     0x0
#define THC0_RESET_DELAY                   T0F0

#define THC1_ADR                           0x00100001
#define THC1_MODE                          TMD1
#define THC1_WAKE_INT                      TIN1
#define THC1_RST_PAD                       T011
#define THC1_RST_TRIGGER                   T021
#define THC1_CONNECTION_SPEED              T031
#define THC1_INPUT_REPORT_HEADER_ADDRESS   T041
#define THC1_INPUT_REPORT_BODY_ADDRESS     T051
#define THC1_OUTPUT_REPORT_ADDRESS         T061
#define THC1_READ_OPCODE                   T071
#define THC1_WRITE_OPCODE                  T081
#define THC1_FLAGS                         T091
#define THC1_ACTIVE_LTR                    T0A1
#define THC1_IDLE_LTR                      T0B1
#define THC1_LIMIT_PACKET_SIZE             T0C1
#define THC1_PERFORMANCE_LIMITATION        T0D1
#define THC1_DISPLAY_FRAME_SYNC_PERIOD     0x0
#define THC1_RESET_DELAY                   T0F1

#define THC_HIDI2C_DEVICE_ADDRESS             0x0
#define THC_HIDI2C_CONNECTION_SPEED           0x0
#define THC_HIDI2C_ADDRESSING_MODE            0x0
#define THC_HIDI2C_SM_SCL_HIGH_PERIOD         0x0
#define THC_HIDI2C_SM_SCL_LOW_PERIOD          0x0
#define THC_HIDI2C_SM_SDA_HOLD_TX_PERIOD      0x0
#define THC_HIDI2C_SM_SDA_HOLD_RX_PERIOD      0x0
#define THC_HIDI2C_FM_SCL_HIGH_PERIOD         0x0
#define THC_HIDI2C_FM_SCL_LOW_PERIOD          0x0
#define THC_HIDI2C_FM_SDA_HOLD_TX_PERIOD      0x0
#define THC_HIDI2C_FM_SDA_HOLD_RX_PERIOD      0x0
#define THC_HIDI2C_SUPPRESSED_SPIKES_S_F_FP   0x0
#define THC_HIDI2C_FMP_SCL_HIGH_PERIOD        0x0
#define THC_HIDI2C_FMP_SCL_LOW_PERIOD         0x0
#define THC_HIDI2C_FMP_SDA_HOLD_TX_PERIOD     0x0
#define THC_HIDI2C_FMP_SDA_HOLD_RX_PERIOD     0x0
#define THC_HIDI2C_HM_SCL_HIGH_PERIOD         0x0
#define THC_HIDI2C_HM_SCL_LOW_PERIOD          0x0
#define THC_HIDI2C_HM_SDA_HOLD_TX_PERIOD      0x0
#define THC_HIDI2C_HM_SDA_HOLD_RX_PERIOD      0x0
#define THC_HIDI2C_SUPPRESSED_SPIKES_H        0x0

Scope (\_SB.PC00) {
  //
  // THC Helper function that produces ACPI _CRS resource when Wake On Touch is enabled
  //
  // Arg0 - Wake On Touch Int Number
  //
  // retval THC Interrupt resource (_CRS)
  //
  Method (TINT, 1, Serialized) {
    Name (TINR, ResourceTemplate () {
      GpioInt (Level, ActiveLow, ExclusiveAndWake, PullUp, 0x0000, "\\_SB.GPI0", 0x00, ResourceConsumer, INTG ) { 0 }
    })
    CreateWordField (TINR,INTG._PIN,INT1)
    Store (GNUM (Arg0), INT1)
    Return (TINR)
  }

#define THC_ADR                         THC0_ADR
#define THC_MODE                        THC0_MODE
#define THC_WAKE_INT                    THC0_WAKE_INT
#define THC_RST_PAD                     THC0_RST_PAD
#define THC_RST_TRIGGER                 THC0_RST_TRIGGER
#define THC_CONNECTION_SPEED            THC0_CONNECTION_SPEED
#define THC_LIMIT_PACKET_SIZE           THC0_LIMIT_PACKET_SIZE
#define THC_PERFORMANCE_LIMITATION      THC0_PERFORMANCE_LIMITATION
#define THC_INPUT_REPORT_HEADER_ADDRESS THC0_INPUT_REPORT_HEADER_ADDRESS
#define THC_INPUT_REPORT_BODY_ADDRESS   THC0_INPUT_REPORT_BODY_ADDRESS
#define THC_OUTPUT_REPORT_ADDRESS       THC0_OUTPUT_REPORT_ADDRESS
#define THC_READ_OPCODE                 THC0_READ_OPCODE
#define THC_WRITE_OPCODE                THC0_WRITE_OPCODE
#define THC_FLAGS                       THC0_FLAGS
#define THC_ACTIVE_LTR                  THC0_ACTIVE_LTR
#define THC_IDLE_LTR                    THC0_IDLE_LTR
#define THC_DISPLAY_FRAME_SYNC_PERIOD   THC0_DISPLAY_FRAME_SYNC_PERIOD
#define THC_RESET_DELAY                 THC0_RESET_DELAY
#define GPIO_CONTROLLER_PATH            \_SB
Device (THC0) {
Include ("ThcController.asl")
}
#undef THC_ADR
#undef THC_MODE
#undef THC_WAKE_INT
#undef THC_RST_PAD
#undef THC_RST_TRIGGER
#undef THC_CONNECTION_SPEED
#undef THC_LIMIT_PACKET_SIZE
#undef THC_PERFORMANCE_LIMITATION
#undef THC_INPUT_REPORT_HEADER_ADDRESS
#undef THC_INPUT_REPORT_BODY_ADDRESS
#undef THC_OUTPUT_REPORT_ADDRESS
#undef THC_READ_OPCODE
#undef THC_WRITE_OPCODE
#undef THC_FLAGS
#undef THC_ACTIVE_LTR
#undef THC_IDLE_LTR
#undef THC_DISPLAY_FRAME_SYNC_PERIOD
#undef THC_RESET_DELAY
#undef GPIO_CONTROLLER_PATH

#define THC_ADR                         THC1_ADR
#define THC_MODE                        THC1_MODE
#define THC_WAKE_INT                    THC1_WAKE_INT
#define THC_RST_PAD                     THC1_RST_PAD
#define THC_RST_TRIGGER                 THC1_RST_TRIGGER
#define THC_CONNECTION_SPEED            THC1_CONNECTION_SPEED
#define THC_LIMIT_PACKET_SIZE           THC1_LIMIT_PACKET_SIZE
#define THC_PERFORMANCE_LIMITATION      THC1_PERFORMANCE_LIMITATION
#define THC_INPUT_REPORT_HEADER_ADDRESS THC1_INPUT_REPORT_HEADER_ADDRESS
#define THC_INPUT_REPORT_BODY_ADDRESS   THC1_INPUT_REPORT_BODY_ADDRESS
#define THC_OUTPUT_REPORT_ADDRESS       THC1_OUTPUT_REPORT_ADDRESS
#define THC_READ_OPCODE                 THC1_READ_OPCODE
#define THC_WRITE_OPCODE                THC1_WRITE_OPCODE
#define THC_FLAGS                       THC1_FLAGS
#define THC_ACTIVE_LTR                  THC1_ACTIVE_LTR
#define THC_IDLE_LTR                    THC1_IDLE_LTR
#define THC_DISPLAY_FRAME_SYNC_PERIOD   THC1_DISPLAY_FRAME_SYNC_PERIOD
#define THC_RESET_DELAY                 THC1_RESET_DELAY
#define GPIO_CONTROLLER_PATH            \_SB
Device (THC1) {
Include ("ThcController.asl")
}
#undef THC_ADR
#undef THC_MODE
#undef THC_WAKE_INT
#undef THC_RST_PAD
#undef THC_RST_TRIGGER
#undef THC_CONNECTION_SPEED
#undef THC_LIMIT_PACKET_SIZE
#undef THC_PERFORMANCE_LIMITATION
#undef THC_INPUT_REPORT_HEADER_ADDRESS
#undef THC_INPUT_REPORT_BODY_ADDRESS
#undef THC_OUTPUT_REPORT_ADDRESS
#undef THC_READ_OPCODE
#undef THC_WRITE_OPCODE
#undef THC_FLAGS
#undef THC_ACTIVE_LTR
#undef THC_IDLE_LTR
#undef THC_DISPLAY_FRAME_SYNC_PERIOD
#undef THC_RESET_DELAY
#undef GPIO_CONTROLLER_PATH

} //End scope

#undef THC_HIDI2C_DEVICE_ADDRESS
#undef THC_HIDI2C_CONNECTION_SPEED
#undef THC_HIDI2C_ADDRESSING_MODE
#undef THC_HIDI2C_SM_SCL_HIGH_PERIOD
#undef THC_HIDI2C_SM_SCL_LOW_PERIOD
#undef THC_HIDI2C_SM_SDA_HOLD_TX_PERIOD
#undef THC_HIDI2C_SM_SDA_HOLD_RX_PERIOD
#undef THC_HIDI2C_FM_SCL_HIGH_PERIOD
#undef THC_HIDI2C_FM_SCL_LOW_PERIOD
#undef THC_HIDI2C_FM_SDA_HOLD_TX_PERIOD
#undef THC_HIDI2C_FM_SDA_HOLD_RX_PERIOD
#undef THC_HIDI2C_SUPPRESSED_SPIKES_S_F_FP
#undef THC_HIDI2C_FMP_SCL_HIGH_PERIOD
#undef THC_HIDI2C_FMP_SCL_LOW_PERIOD
#undef THC_HIDI2C_FMP_SDA_HOLD_TX_PERIOD
#undef THC_HIDI2C_FMP_SDA_HOLD_RX_PERIOD
#undef THC_HIDI2C_HM_SCL_HIGH_PERIOD
#undef THC_HIDI2C_HM_SCL_LOW_PERIOD
#undef THC_HIDI2C_HM_SDA_HOLD_TX_PERIOD
#undef THC_HIDI2C_HM_SDA_HOLD_RX_PERIOD
#undef THC_HIDI2C_SUPPRESSED_SPIKES_H

#undef THC0_DISPLAY_FRAME_SYNC_PERIOD
#undef THC1_DISPLAY_FRAME_SYNC_PERIOD
