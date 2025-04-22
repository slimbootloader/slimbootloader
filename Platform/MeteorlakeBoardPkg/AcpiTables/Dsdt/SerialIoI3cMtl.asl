/**@file

  Serial IO I3C ACPI definitions

  Copyright (c) 2023, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include <Register/SerialIoI3cRegs.h>

Scope (\_SB.PC00) {
  Device (I3C0) {
    Name (I3CR, Package () {
      // I3C3
      R_SERIAL_IO_I3C0_MEM_QUEUE_THLD_CTRL,               V_SERIAL_IO_I3C_MEM_QUEUE_THLD_CTRL,
      R_SERIAL_IO_I3C0_MEM_DATA_BUFFER_THLD_CTRL,         V_SERIAL_IO_I3C_MEM_DATA_BUFFER_THLD_CTRL,
      R_SERIAL_IO_I3C0_MEM_SCL_I3C_OD_TIMING,             V_SERIAL_IO_I3C_MEM_SCL_I3C_OD_TIMING,
      R_SERIAL_IO_I3C0_MEM_SCL_I3C_PP_TIMING,             V_SERIAL_IO_I3C_MEM_SCL_I3C_PP_TIMING,
      R_SERIAL_IO_I3C0_MEM_SCL_I2C_FM_TIMING,             V_SERIAL_IO_I3C_MEM_SCL_I2C_FM_TIMING,
      R_SERIAL_IO_I3C0_MEM_SCL_I2C_FMP_TIMING,            V_SERIAL_IO_I3C_MEM_SCL_I2C_FMP_TIMING,
      R_SERIAL_IO_I3C0_MEM_SCL_I2C_SS_TIMING,             V_SERIAL_IO_I3C_MEM_SCL_I2C_SS_TIMING,
      R_SERIAL_IO_I3C0_MEM_SCL_EXT_LCNT_TIMING,           V_SERIAL_IO_I3C_MEM_SCL_EXT_LCNT_TIMING,
      R_SERIAL_IO_I3C0_MEM_SCL_EXT_TERMN_LCNT_TIMING,     V_SERIAL_IO_I3C_MEM_SCL_EXT_TERMN_LCNT_TIMING,
      R_SERIAL_IO_I3C0_MEM_SDA_HOLD_SWITCH_DLY_TIMING,    V_SERIAL_IO_I3C_MEM_SDA_HOLD_SWITCH_DLY_TIMING,
      R_SERIAL_IO_I3C0_MEM_BUS_FREE_TIMING,               V_SERIAL_IO_I3C_MEM_BUS_FREE_TIMING,

      // I3C
      R_SERIAL_IO_I3C_MEM_ACTIVELTR,                      V_SERIAL_IO_I3C_MEM_ACTIVELTR,
      R_SERIAL_IO_I3C_MEM_IDLELTR,                        V_SERIAL_IO_I3C_MEM_IDLELTR,
      R_SERIAL_IO_I3C_MEM_CLOCK_GATE,                     V_SERIAL_IO_I3C_MEM_CLOCK_GATE,
      R_SERIAL_IO_I3C_MEM_DEVIDLE_CONTROL,                V_SERIAL_IO_I3C_MEM_DEVIDLE_CONTROL,
      R_SERIAL_IO_I3C_MEM_HCI_COMP_VER_OVRD,              V_SERIAL_IO_I3C_MEM_HCI_COMP_VER_OVRD,
      R_SERIAL_IO_I3C_MEM_I3C_THRESHOLD,                  V_SERIAL_IO_I3C_MEM_I3C_THRESHOLD,

      // I3C1
      R_SERIAL_IO_I3C1_MEM_QUEUE_THLD_CTRL,               V_SERIAL_IO_I3C_MEM_QUEUE_THLD_CTRL,
      R_SERIAL_IO_I3C1_MEM_DATA_BUFFER_THLD_CTRL,         V_SERIAL_IO_I3C_MEM_DATA_BUFFER_THLD_CTRL,
      R_SERIAL_IO_I3C1_MEM_SCL_I3C_OD_TIMING,             V_SERIAL_IO_I3C_MEM_SCL_I3C_OD_TIMING,
      R_SERIAL_IO_I3C1_MEM_SCL_I3C_PP_TIMING,             V_SERIAL_IO_I3C_MEM_SCL_I3C_PP_TIMING,
      R_SERIAL_IO_I3C1_MEM_SCL_I2C_FM_TIMING,             V_SERIAL_IO_I3C_MEM_SCL_I2C_FM_TIMING,
      R_SERIAL_IO_I3C1_MEM_SCL_I2C_FMP_TIMING,            V_SERIAL_IO_I3C_MEM_SCL_I2C_FMP_TIMING,
      R_SERIAL_IO_I3C1_MEM_SCL_I2C_SS_TIMING,             V_SERIAL_IO_I3C_MEM_SCL_I2C_SS_TIMING,
      R_SERIAL_IO_I3C1_MEM_SCL_EXT_LCNT_TIMING,           V_SERIAL_IO_I3C_MEM_SCL_EXT_LCNT_TIMING,
      R_SERIAL_IO_I3C1_MEM_SCL_EXT_TERMN_LCNT_TIMING,     V_SERIAL_IO_I3C_MEM_SCL_EXT_TERMN_LCNT_TIMING,
      R_SERIAL_IO_I3C1_MEM_SDA_HOLD_SWITCH_DLY_TIMING,    V_SERIAL_IO_I3C_MEM_SDA_HOLD_SWITCH_DLY_TIMING,
      R_SERIAL_IO_I3C1_MEM_BUS_FREE_TIMING,               V_SERIAL_IO_I3C_MEM_BUS_FREE_TIMING
    })

    Name (_ADR, 0x00150004)

    // _DSM : Device Specific Method for the I3C.
    // Arg0: UUID Unique function identifier
    // Arg1: Integer Revision Level
    // Arg2: Integer Function Index
    //   - 0 Check how many functions are supported
    //   - 1 Return the I3C settings
    // Arg3: Package Parameters
    // Return:
    //   Function 0: Supported Functions
    //   Function 1: Package with settings

    Method (_DSM, 4, Serialized, 0, UnknownObj, {BuffObj, IntObj, IntObj, PkgObj}) {
        // Compare passed in UUID to supported UUID.
        If (PCIC(Arg0)) { Return(PCID(Arg0,Arg1,Arg2,Arg3)) }
        If (LEqual(Arg0, ToUUID ("E59152DE-BD93-4164-8A5E-2828FBD8B959")))
        {
          Switch (ToInteger(Arg2)) {           // Switch to Function Index.
            // Function 0, Query of supported functions.
            Case (0) {
              Return (ToBuffer(0x1F)) // Supports Function 1.
            }
            // Function 1, Return I3C settings.
            Case (1) {
              Return (I3CR)
            }
          } // End Switch statement
          Return (ToBuffer(0x0))
        } // End UUID check
        Return (ToBuffer(0x0))
    } // End _DSM Method

    Method (_S0W, 0) {
      // PMEs can be generated from D3(hot)
      Return (3)
    }

    // IBI Wake Support
    Method (_PRW, 0) {
      If (CondRefOf (GP1E)){
        If (LNotEqual (GP1E, 0)) {
          Return (GPRW (GPE1_LPSS_PME_B0_EN, 4))
        }
      }
      Return (GPRW (0x6D, 4)) // can wakeup from S4 state
    }
  } // end Device I3C0
} //scope

