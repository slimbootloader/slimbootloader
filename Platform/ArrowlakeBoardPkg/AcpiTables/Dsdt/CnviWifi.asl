/**@file
  CNVi Wi-Fi ACPI definitions

  Copyright (c) 2024, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

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
    // Define a Memory Region that will allow access to the CNVi WiFi PCI Configuration Space
    //
    OperationRegion (CWAR, SystemMemory, BASE (), 0x100)
    Field (CWAR, WordAcc, NoLock, Preserve) {
      VDID, 32, // 0x00, VID DID
      Offset (R_CNVI_CFG_WIFI_GIO_DEV_CAP),
          , 28,
      WFLR,  1, // Function Level Reset Capable
      Offset (R_CNVI_CFG_GIO_DEV),
          , 15,
      WIFR,  1, // Init Function Level Reset
    }
    Method (_S0W, 0x0, NotSerialized) {
      Return (0x3)
    }

    Method (_DSW, 3) {}

    // Define Platform-level device reset power resource
    PowerResource (WRST, 5, 0) {
      // Define the PowerResource for CNVi WiFi
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
          CFLR () // WiFi FLR
          Store (1, PRRS) // Set Core PLDR Completed successfully

          // Perform WiFi PLDR if RSTT = 1
          If (LAnd (CondRefOf (RSTT), LEqual (RSTT, 1))) {
            ADBG ("WiFi Product Reset")

            ADBG (Concatenate ("PID of CNVi = ", ToHexString (PCNV)))
            Store (P2SB_DEV.PCRR (PCNV, R_CNVI_ACPI_PLRB), Local5)
            ADBG (Concatenate ("Register PLDB Value = ", ToHexString (Local5)))

            // Query previous PLDR completed by checking PLDR_ABORT_REQUEST = 0
            If (LEqual (And (P2SB_DEV.PCRR (PCNV, R_CNVI_ACPI_PLRB), B_CNVI_PCR_CNVI_PLDR_ABORT_REQUEST), Zero)) {
              ADBG ("ABORT_REQUEST = 0, previous PLDR completed")

              // If BT RF-Kill deasserted, assert BT RF-Kill and wait for 160 ms
              If (LEqual (\_SB.GBTR (), 1)) {
                \_SB.BTRK (0x00) // assert W_DISABLE2#
                Sleep (160)
                Store (1, Local2)
              }

              // Enable CNVi PLDR request and wait for 10 ms
              P2SB_DEV.PCRO (PCNV, R_CNVI_ACPI_PLRB, Or (B_CNVI_PCR_CNVI_PLDR_ABORT_ENABLE, B_CNVI_PCR_CNVI_PLDR_ABORT_REQUEST))
              Sleep (10)
              Store (P2SB_DEV.PCRR (PCNV, R_CNVI_ACPI_PLRB), Local1)
              ADBG (Concatenate ("CNVI_PLDR_ABORT = ", ToHexString (Local1)))

              // Check PLDR request is successful if PLDR_ABORT_REQUEST = 0 & CNVB_CNVI_READY = 1
              If (LAnd (LEqual (And (Local1, B_CNVI_PCR_CNVI_PLDR_ABORT_REQUEST), 0), And (Local1, B_CNVI_PCR_SCU_CNVB_CNVI_READY))) {
                ADBG ("WiFi Product Reset Completed")
                Store (2, PRRS) // Set Product PLDR Completed successfully
                // Deassert BT RF-Kill and wait for 160 ms
                If (LEqual (Local2, 1)) {
                  \_SB.BTRK (0x01) // deassert W_DISABLE2#
                  Sleep (160)
                }
              } Else {
                // PLDR timeout
                Store (4, PRRS) // Set Product PLDR timeout
                \_SB.BTRK (0x01) // deassert W_DISABLE2#
              }
            } Else {
              // Previous PLDR not completed
              Store (3, PRRS) // Set Previous PLDR Not Completed
            }
          }

          ADBG (Concatenate ("WiFi PRRS = ", PRRS))
          Release (\CNMT)
        }
      }
    } // End WRST

    Name (_PRR, Package () {WRST})

    Method (CFLR) {
      ADBG (Concatenate ("WiFi Core Reset, WFLR = ", WFLR))
      If (LEqual (WFLR, 1)) {
        Store (1, WIFR)
      }
    }
