/** @file
  CNVi Bluetooth ACPI definitions

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
     //BT PLDR request delay of 50ms
    Name (BTDL, 50)

    // Reset Type
    //   0: Core Reset
    //   1: Product Reset
    Name (RSTT, 0)

    // Last _PRR Status
    //   0: Non-PLDR executed [Default]
    //   1: Core PLDR Completed successfully
    //   2: Product PLDR Completed successfully
    //   3: Previous PLDR Not Completed
    //   4: Product PLDR timeout
    Name (PRRS, 0)

    //
    // Define a Memory Region that will allow access to the CNVi Bluetooth PCI Configuration Space
    //
    OperationRegion (CBTR, SystemMemory, BASE (), 0x100)
    Field (CBTR, WordAcc, NoLock, Preserve) {
      VDID, 32, // 0x00, VID DID
      Offset(R_CNVI_CFG_GIO_DEV_CAP),
          , 28,
      BFLR,  1, // Function Level Reset Capable
      Offset(R_CNVI_CFG_GIO_DEV),
          , 15,
      BIFR,  1, // Init Function Level Reset
      Offset (R_CNVI_CFG_GIO_DEV_CAP_2),
          , 11,
      BLTS,  1, // LTR Mechanism Supported
      Offset (R_CNVI_CFG_GIO_DEV_2),
          , 10,
      BLTE,  1, // LTR Mechanism Enable
    }

    Method (_S0W, 0x0, NotSerialized) {
      Return (0x3)
    }

    Method (_DSW, 3) {}

    // Define Platform-level device reset power resource
    PowerResource (WRST, 5, 0) {
      // Define the PowerResource for CNVi Bluetooth
      // PowerResource expects to have _STA, _ON and _OFF Method per ACPI Spec. Not having one of them will cause BSOD

      // Method: Dummy _STA () to comply with ACPI Spec
      Method (_STA) {
        Return (0x01)
      }

      // Method: Dummy _ON () to comply with ACPI Spec
      Method (_ON, 0) {
      }

      // Method: Dummy _OFF () to comply with ACPI Spec
      Method (_OFF, 0) {
      }

      Method (_RST, 0, NotSerialized) {
      Store (Acquire (\CNMT, 1000), Local0) // save Acquire result so we can check for Mutex acquired
      If (LEqual (Local0, Zero)) // check for Mutex acquired
      {
        // BT FLR
        CFLR ()

        // Perform BT PLDR if RSTT = 1
        If (LAnd (CondRefOf (RSTT), LEqual (RSTT, 1))) {
          ADBG ("BT Product Reset")

          // Query previous PLDR completed by checking PLDR_ABORT_REQUEST = 0
          If (LEqual (And (PCRR (PCNV, R_CNVI_ACPI_PLRB), B_CNVI_PCR_CNVI_PLDR_ABORT_CNVI_PLDR_ABORT_REQUEST), Zero)) {
            ADBG ("ABORT_REQUEST = 0, previous PLDR completed")

           // Enable CNVi PLDR request and wait
            PCRO (PCNV, R_CNVI_ACPI_PLRB, Or (B_CNVI_PCR_CNVI_PLDR_ABORT_CNVI_PLDR_ABORT_ENABLE, B_CNVI_PCR_CNVI_PLDR_ABORT_CNVI_PLDR_ABORT_REQUEST))
            ADBG (Concatenate ("BT PLDR Timeout wait(ms)  = ", ToHexString (BTDL)))
            Sleep (BTDL)
            Store (PCRR (PCNV, R_CNVI_ACPI_PLRB), Local1)
            ADBG (Concatenate ("CNVI_PLDR_ABORT = ", ToHexString (Local1)))

            // Check PLDR request is successful if PLDR_ABORT_REQUEST = 0 & CNVB_CNVI_READY = 1
            If (LAnd (LEqual (And (Local1, B_CNVI_PCR_CNVI_PLDR_ABORT_CNVI_PLDR_ABORT_REQUEST), 0), And (Local1, B_CNVI_PCR_CNVI_PLDR_ABORT_SCU_CNVB_CNVI_READY))) {
              ADBG ("BT Product Reset Completed")
              Store (2, PRRS) // Set Product PLDR Completed successfully
            } Else {
              // PLDR timeout
              Store (4, PRRS) // Set Product PLDR timeout
            }
          } Else {
            // Previous PLDR not completed
            Store (3, PRRS) // Set Previous PLDR Not Completed
          }
        }

        //
        // Restore LTR Mechanism Enable, if LTR Mechanism Supported
        //
        If (LAnd (LEqual (BLTS, 1), LEqual (BLTE, 0))) {
          Store (1, BLTE)
        }

        ADBG (Concatenate ("BT PRRS = ", PRRS))
        Release (\CNMT)
        }
      }
    } // End WRST

    Name (_PRR, Package () {WRST})

    Method (CFLR) {
      ADBG (Concatenate ("BT Core Reset, BFLR = ", BFLR))
      If (LEqual (BFLR, 1)) {
        Store (1, BIFR)
        Store (1, PRRS) // Set Core PLDR Completed successfully
      }
    }
