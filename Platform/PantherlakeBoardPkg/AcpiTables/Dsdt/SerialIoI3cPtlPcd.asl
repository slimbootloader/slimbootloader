/**@file

  Serial IO I3C ACPI definitions

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include <Register/SerialIoI3cRegs.h>

External (\_SB.PC00.PC2M, MethodObj)

Scope (\_SB.PC00) {
  Device (I3C0) {

    OperationRegion (PXC0, SystemMemory, \_SB.PC00.PC2M (0x00110000, 0), 0x100)
    Field (PXC0,AnyAcc, NoLock, Preserve)
    {
      Offset (0),
      VDID, 32,
      Offset (0x84), // CFG_PM_CAP_1 0x84
      PSTE, 2,       // 1:0, PM_STATE
      , 6,
      PMEN, 1,       // 8, PME_EN
      , 6,
      PSTS, 1,       // 15, PME_STATUS
    }

    Name (I3R0, Package () {
      // I3C #0 Register                               // Value                                 // Index
      R_SERIAL_IO_I3C0_MEM_QUEUE_THLD_CTRL,               0,                                    // 1
      R_SERIAL_IO_I3C0_MEM_DATA_BUFFER_THLD_CTRL,         0,                                    // 3
      R_SERIAL_IO_I3C0_MEM_SCL_I3C_OD_TIMING,             0,                                    // 5
      R_SERIAL_IO_I3C0_MEM_SCL_I3C_PP_TIMING,             0,                                    // 7
      R_SERIAL_IO_I3C0_MEM_SCL_I2C_FM_TIMING,             0,                                    // 9
      R_SERIAL_IO_I3C0_MEM_SCL_I2C_FMP_TIMING,            0,                                    // 11
      R_SERIAL_IO_I3C0_MEM_SCL_I2C_SS_TIMING,             0,                                    // 13
      R_SERIAL_IO_I3C0_MEM_SCL_EXT_LCNT_TIMING,           0,                                    // 15
      R_SERIAL_IO_I3C0_MEM_SCL_EXT_TERMN_LCNT_TIMING,     0,                                    // 17
      R_SERIAL_IO_I3C0_MEM_SDA_HOLD_SWITCH_DLY_TIMING,    0,                                    // 19
      R_SERIAL_IO_I3C0_MEM_BUS_FREE_TIMING,               0,                                    // 21
    })

    Name (_ADR, 0x00110000)
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

    Method (_DSM, 4, Serialized, 0, UnknownObj, {BuffObj, IntObj, IntObj, PkgObj}) {
      // Compare passed in UUID to supported UUID.
      If (PCIC (Arg0)) { Return (PCID (Arg0,Arg1,Arg2,Arg3)) }
      If (LEqual (Arg0, ToUUID ("E59152DE-BD93-4164-8A5E-2828FBD8B959"))) {
        Switch (ToInteger (Arg2)) {           // Switch to Function Index.
          // Function 0, Query of supported functions.
          Case (0) {
            Return (ToBuffer (0x1F)) // Supports Function 1.
          }
          // Function 1, Return I3C settings.
          Case (1) {
            Store (QTH0, Index (I3R0, 1))
            Store (BTH0, Index (I3R0, 3))
            Store (ODT0, Index (I3R0, 5))
            Store (PPT0, Index (I3R0, 7))
            Store (FMT0, Index (I3R0, 9))
            Store (FPT0, Index (I3R0, 11))
            Store (SST0, Index (I3R0, 13))
            Store (ELC0, Index (I3R0, 15))
            Store (ETL0, Index (I3R0, 17))
            Store (SWD0, Index (I3R0, 19))
            Store (BDL0, Index (I3R0, 21))

            Return (I3R0)
          }
        } // End Switch statement
        Return (ToBuffer (0x0))
      } // End UUID check
      Return (ToBuffer (0x0))

    } // End _DSM Method
  } // end Device I3C0

  Device (I3C2) {

    OperationRegion (PXC2, SystemMemory, \_SB.PC00.PC2M (0x00110001, 0), 0x100)
    Field (PXC2,AnyAcc, NoLock, Preserve)
    {
      Offset (0),
      VDID, 32,
      Offset (0x84), // CFG_PM_CAP_1 0x84
      PSTE, 2,       // 1:0, PM_STATE
      , 6,
      PMEN, 1,       // 8, PME_EN
      , 6,
      PSTS, 1,       // 15, PME_STATUS
    }
    Name (I3R2, Package () {
      // I3C #2 Register                               // Value                                    // Index
      R_SERIAL_IO_I3C2_MEM_QUEUE_THLD_CTRL,               0,                                     // 1
      R_SERIAL_IO_I3C2_MEM_DATA_BUFFER_THLD_CTRL,         0,                                     // 3
      R_SERIAL_IO_I3C2_MEM_SCL_I3C_OD_TIMING,             0,                                     // 5
      R_SERIAL_IO_I3C2_MEM_SCL_I3C_PP_TIMING,             0,                                     // 7
      R_SERIAL_IO_I3C2_MEM_SCL_I2C_FM_TIMING,             0,                                     // 9
      R_SERIAL_IO_I3C2_MEM_SCL_I2C_FMP_TIMING,            0,                                     // 11
      R_SERIAL_IO_I3C2_MEM_SCL_I2C_SS_TIMING,             0,                                     // 13
      R_SERIAL_IO_I3C2_MEM_SCL_EXT_LCNT_TIMING,           0,                                     // 15
      R_SERIAL_IO_I3C2_MEM_SCL_EXT_TERMN_LCNT_TIMING,     0,                                     // 17
      R_SERIAL_IO_I3C2_MEM_SDA_HOLD_SWITCH_DLY_TIMING,    0,                                     // 19
      R_SERIAL_IO_I3C2_MEM_BUS_FREE_TIMING,               0                                      // 21
    })

    Name (_ADR, 0x00110001)
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

    Method (_DSM, 4, Serialized, 0, UnknownObj, {BuffObj, IntObj, IntObj, PkgObj}) {
      // Compare passed in UUID to supported UUID.
      If (PCIC (Arg0)) { Return (PCID (Arg0,Arg1,Arg2,Arg3)) }
      If (LEqual (Arg0, ToUUID ("E59152DE-BD93-4164-8A5E-2828FBD8B959"))) {
        Switch (ToInteger (Arg2)) {           // Switch to Function Index.
          // Function 0, Query of supported functions.
          Case (0) {
            Return (ToBuffer (0x1F)) // Supports Function 1.
          }
          // Function 1, Return I3C settings.
          Case (1) {
            Store (QTH2, Index (I3R2, 1))
            Store (BTH2, Index (I3R2, 3))
            Store (ODT2, Index (I3R2, 5))
            Store (PPT2, Index (I3R2, 7))
            Store (FMT2, Index (I3R2, 9))
            Store (FPT2, Index (I3R2, 11))
            Store (SST2, Index (I3R2, 13))
            Store (ELC2, Index (I3R2, 15))
            Store (ETL2, Index (I3R2, 17))
            Store (SWD2, Index (I3R2, 19))
            Store (BDL2, Index (I3R2, 21))

            Return (I3R2)
          }
        } // End Switch statement
        Return (ToBuffer (0x0))
      } // End UUID check
      Return (ToBuffer (0x0))
    } // End _DSM Method

  } // end Device I3C2

} //scope

