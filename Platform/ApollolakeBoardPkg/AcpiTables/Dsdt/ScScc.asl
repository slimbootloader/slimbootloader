/**@file

  Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#define SW_GPIO_177           0x00C005D0    //SDCARD_CD_B

Name(EMMS,0) // eMMC Host Max Speed

scope (\_SB.PCI0) {

  //
  // Access P2SB CFG Space
  //
  //  P2SB PCI MMCFG Space: 0xE0000000
  //  P2SB Device: 0xD, Function: 0x0
  //  P2SB MM CFG Interface Access Registers: 0xD0 - 0xDC
  //
  OperationRegion(P2CG, SystemMemory, OR( OR (0xE0000000, ShiftLeft(0xD, 15)), 0xD0), 0x20)  // P2SB PCI CFG SB Interface Registers
  Field(P2CG, DWordAcc, NoLock, Preserve)
  {
    Offset (0x00),  // SBI Addr (0xD0)
    SBAD, 32,
    Offset (0x04),  // SBI Data (0xD4)
    SBDA, 32,
    Offset (0x08),  // SBI Stat (0xD8)
    IRDY, 1,
    ,     6,
    POST, 1,
    OPCD, 8,
    Offset (0x0A),  // SBI RID  (0xDA)
    SBID, 16,
    Offset (0x0C),  // SBI Ext Addr (0xDC)
    SBEA, 32,
    Offset (0x10),  // P2SBC - P2SB Control (0xE0)
    ,     8,
    P2HD, 8         // P2SB HIDE bit [8:8] (use the byte which is reserved)
  }

  //
  // SBI Method:
  //   SBI message execution
  //
  // Arguments: (4)
  //   Arg0: Addr
  //   Arg1: WriteData
  //   Arg2: OpCode
  //   Arg3: RID
  //
  //  Return Value:
  //    ReadData
  //
  Method (SBIM, 4, Serialized)
  {
    Store (0x0, Local0)
    //
    // Acquire the ACPI Global Lock to Ensure Exclusive Access to the P2SB Interface
    //
    Store(Acquire (\_GL, 0x1F40), Local1)
    If (LEqual(Local1, 0)) {
      // Unhide P2SB CFG Space
      Store (0, P2HD)

      // Wait Until InitRdy == 0 (Transaction Complete)
      While (IRDY)
      {
        Sleep (1)
      }

      // SBIADDR
      And (SBAD, 0x00F00000, SBAD)
      Or (SBAD, Arg0, SBAD)

      // SBIEXTADDR
      Store (0, SBEA)

      // SBIDATA
      Store (Arg1, SBDA)

      // SBIRID
      And (SBID, 0x0800, SBID)
      Or (SBID, Arg3, SBID)

      // SBISTAT
      Store (0, POST)     // set non-posted access
      Store (Arg2, OPCD)
      Store (1, IRDY)

      // Wait Until InitRdy == 0 (Transaction Complete)
      While (IRDY)
      {
        Sleep (1)
      }
      Store (SBDA, local0)

      // Hide P2SB CFG Space
      Store (1, P2HD)
    }
    Release (\_GL)
    Return (local0)
  }

  OperationRegion (SBMM, SystemMemory, OR( OR (P2BA, ShiftLeft(0xD6, 16)), 0x0600), 0x18)
    Field (SBMM, DWordAcc, NoLock, Preserve)
    {
      Offset (0x00),
      GENR, 32,
      Offset (0x08),
          ,  5,
      GRR3,  1,
    }

  //
  // SCC power gate control method, this method must be serialized as multiple device will control the GENR register
  //
  // Arguments: (2)
  //  Arg0: 0-AND  1-OR
  //  Arg1: Value
  Method (SCPG, 2, Serialized)
  {
    Name (TMP, 0x0)
    if (LEqual(Arg0, 0x1)) {
      Store (\_SB.PCI0.GENR, TMP)
      Or    (TMP, Arg1, \_SB.PCI0.GENR)

    } ElseIf(LEqual(Arg0, 0x0)) {
      Store (\_SB.PCI0.GENR, TMP)
      And   (TMP, Arg1, \_SB.PCI0.GENR)
    }
  }

  //
  // eMMC
  //
  Device(SDHA) {
    Name (_ADR, 0x001C0000)
    Name (_DDN, "Intel(R) eMMC Controller - 80865ACC")
    Name (_UID, 1)

    Name (RBUF, ResourceTemplate ()
    {
    })

    OperationRegion (PMCS, PCI_Config, 0x84, 0x4)
    Field (PMCS, WordAcc, NoLock, Preserve) {
      PMSR, 32,   // 0x84, PMCSR - Power Management Control and Status
    }

    OperationRegion (SCPC, PCI_Config, 0xA0, 4)
    Field (SCPC, WordAcc, NoLock, Preserve)
    {
      Offset (0x00), // 0xA0 D0i3 Max Power Latency Powergating Config
      ,     17,
      I3EN, 1,
      DPGE, 1
    }

    Method (_CRS, 0, NotSerialized)  // _CRS: Current Resource Settings
    {
      Return (RBUF)
    }

    Method (_PS0, 0, NotSerialized)  // _PS0: Power State 0
    {
      //
      // Disable power gate
      //
      Store (0, \_SB.PCI0.SDHA.DPGE)
      Store (0, \_SB.PCI0.SDHA.I3EN)
      //
      // Clear clock gate
      //
      \_SB.PCI0.SCPG(0,0xFFFFFFBE) // Clear bit 6 and 0
      Sleep (2)                              // Sleep 2 ms

    }

    Method (_PS3, 0, NotSerialized)          // _PS3: Power State 3
    {
      Store (SBIM (0xD600003C, 0, 0x0, 0x30E0), local0)
      Store (SBIM (0xD6000834, 0, 0x0, 0x30E0), local1)
      Store (SBIM (0xD6000840, 0, 0x0, 0x30E0), local2)

      If (And (Local0, 0x00800000)) {
        Add (Multiply (And (local2, 0x1F), 2), ShiftRight(And (Local1, 0x3F00), 8), local3)
        Or (And (local1, 0xFFFFFF80), And (local3, 0x7F), local1)
        SBIM (0xD6000834, local1, 0x1, 0x30E0)
      }
      //
      // Enable power gate
      //
      Store (1, \_SB.PCI0.SDHA.DPGE)
      Store (1, \_SB.PCI0.SDHA.I3EN)
      //
      // Restore clock gate
      //
      \_SB.PCI0.SCPG(1,0x00000041) // restore bit 6 and 0

      //
      // Dummy read PMCSR
      //
      Store (PMSR, Local0)
      And (Local0, 1, Local0) // Dummy operation on Local0
    }

    Method(_DSM, 0x4, Serialized)
    {
        //check the UUID
        if(LEqual(Arg0, ToUUID("f6c13ea5-65cd-461f-ab7a-29f7e8d5bd61")))
        {
            //check the revision
            If(LEqual(Arg1, Zero))
            {
                //Switch statement based on the function index.
                switch(ToInteger(Arg2))
                {
                    //
                    // Function Index 0 the return value is a buffer containing
                    // one bit for each function index, starting with zero.
                    // Bit 0 - Indicates whether there is support for any functions other than function 0.
                    // Bit 1 - Indicates support to clear power control register
                    // Bit 2 - Indicates support to set power control register
                    // Bit 3 - Indicates support to set 1.8V signalling
                    // Bit 4 - Indicates support to set 3.3V signalling
                    // Bit 5 - Indicates support for HS200 mode
                    // Bit 6 - Indicates support for HS400 mode
                    //
                    // On APL, for eMMC we have to support functions for
                    // HS200 and HS400
                    //

                    case(0)
                    {
                       Return(Buffer() {0x61})
                    }

                    //
                    // Function index 5 - corresponds to HS200 mode
                    // Return value from this function is used to program
                    // the UHS Mode Select bits in Host Control 2.
                    // 011b - corresponds to SDR104 and according to the
                    // SD Host Controller Spec and this value is overloaded
                    // to program the controller to select HS200 mode for eMMC.
                    //

                    case(5)
                    {
                        Return(Buffer() {0x3})
                    }

                    //
                    // Function index 6 - corresponds to HS400 mode
                    // Return value from this function is used to program
                    // the UHS Mode Select bits in Host Control 2.
                    // 101b is a reserved value according to the SD Host
                    // Controller Spec and we use this value for HS400 mode
                    // selection.
                    //

                    case(6)
                    {
                        Return(Buffer() {0x5})
                    }
                } // End - Switch(Arg2)

            Return(Buffer() {0x0})
          } Else {
            Return(Buffer() {0x0})
          } //End - If else statement for revision check
        } Else {
          Return(Buffer() {0x0})
        } //End - If else statement for UUID check
    }

    Device (EMMD)
    {
      Name (_ADR, 0x00000008) // Slot 0, Function 8
      Method (_RMV, 0, NotSerialized)
      {
        Return (0x0)
      }
    }
  } // Device(SDHA)

  //
  // UFS
  //
  Device(UFSH) {
    Name (_ADR, 0x001D0000)
    Name (_DDN, "Intel(R) UFS Controller - 80865ACE")
    Name (_UID, 1)

    OperationRegion (PMCS, PCI_Config, 0x84, 0x4)
    Field (PMCS, WordAcc, NoLock, Preserve) {
      PMSR, 32,   // 0x84, PMCSR - Power Management Control and Status
    }

    Method (_PS0, 0, NotSerialized) {         // _PS0: Power State 0
    }

    Method (_PS3, 0, NotSerialized) {         // _PS3: Power State 3
      //
      // Dummy read PMCSR
      //
      Store (PMSR, Local0)
      And (Local0, 1, Local0) // Dummy operation on Local0
    }

    Name (RBUF, ResourceTemplate ()
    {
    })
    Method (_CRS, 0x0, NotSerialized)
    {
      Return (RBUF)
    }

    Device (UFSD)
    {
      Name (_ADR, 0x00000008) // Slot 0, Function 8
      Method (_RMV, 0, NotSerialized)
      {
        Return (0x0)
      }
    }
  } // Device(UFSH)

  //
  // SDIO
  //
  Device(SDIO) {
    Name (_ADR, 0x001E0000)
    Name (_DDN, "Intel(R) SDIO Controller - 80865AD0")
    Name (_UID, 1)
    Name (_S0W, 0x03)  // _S0W: S0 Device Wake State

    Name (RBUF, ResourceTemplate ()
    {
    })

    Name (PSTS, 0x0)

    OperationRegion (SCPC, PCI_Config, 0xA0, 4)
    Field (SCPC, WordAcc, NoLock, Preserve)
    {
      Offset (0x00), // 0xA0 D0i3 Max Power Latency Powergating Config
      ,     17,
      I3EN, 1,
      DPGE, 1
    }

    OperationRegion (PMCS, PCI_Config, 0x84, 0x4)
    Field (PMCS, WordAcc, NoLock, Preserve) {
      PMSR, 32,   // 0x84, PMCSR - Power Management Control and Status
    }

    Method (_CRS, 0, NotSerialized)  // _CRS: Current Resource Settings
    {
      Return (RBUF)
    }

    Method (_PS0, 0, NotSerialized)  // _PS0: Power State 0
    {
      If(LEqual(\_SB.PCI0.SDIO.PSTS,0x0))
      {
        //
        // Disable power gate
        //
        Store (0, \_SB.PCI0.SDIO.DPGE)
        Store (0, \_SB.PCI0.SDIO.I3EN)
        //
        // Enable clock gate
        //
        \_SB.PCI0.SCPG(0,0xFFFFFEFE) // Clear bit 8 and 0
        Sleep (2)                                // Sleep 2 ms

        Store( 0x1, \_SB.PCI0.SDIO.PSTS)  // Indicates that the device is powered ON
      }
    }

    Method (_PS3, 0, NotSerialized)            // _PS3: Power State 3
    {
      If(LEqual(\_SB.PCI0.SDIO.PSTS,0x1))
      {
        //
        // Enable power gate
        //
        Store (1, \_SB.PCI0.SDIO.DPGE)
        Store (1, \_SB.PCI0.SDIO.I3EN)

        //
        // Restore clock gate
        //
        \_SB.PCI0.SCPG(1,0x00000101) // restore bit 8 and 0

        //
        // Dummy read PMCSR
        //
        Store (PMSR, Local0)
        And (Local0, 1, Local0) // Dummy operation on Local0
        Store( 0x0, \_SB.PCI0.SDIO.PSTS)  // Indicates that the device is powered OFF
      }
    }

    Method (_DSW, 3, NotSerialized)  // _DSW: Device Sleep Wake
    {
    }
  } // Device(SDIO)

  //
  // SD Card
  //
  Device (SDC) {
    Name (_ADR, 0x001B0000)  // _ADR: Address
    Name (_DDN, "Intel(R) SD Card Controller - 80865ACA")  // _DDN: DOS Device Name
    Name (_S0W, 0x03)  // _S0W: S0 Device Wake State
    Name (GDW0, 0x00000000)
    Name (GDW1, 0x00000000)

    Method (_CRS, 0x0, Serialized)
    {
      Name (RBUF, ResourceTemplate ()
      {
        //
        // Don't align with BoardGpio setting. If do, system can't enter S3 with SD card in slot.
        //
        GpioInt(Edge, ActiveBoth, SharedAndWake, PullNone, 0, "\\_SB.GPO3", 0, ResourceConsumer, , ) {26}      // GPIO_177(SW pin 26):SDCARD_CD_B (SDMMC3_CD_N)
        GpioIo(Shared, PullDefault, 0, 0, IoRestrictionInputOnly, "\\_SB.GPO3", 0, ResourceConsumer, , ) {26}  // GPIO_177(SW pin 26):SDCARD_CD_B (SDMMC3_CD_N)
      })

      Return (RBUF)
    }

    Method (_STA, 0x0, NotSerialized)
    {
        Return(0xF)
    }

    OperationRegion (SCPC, PCI_Config, 0xA0, 4)
    Field (SCPC, WordAcc, NoLock, Preserve)
    {
      Offset (0x00),  // 0xA0 D0i3 Max Power Latency Powergating Config
      ,     17,
      I3EN, 1,
      DPGE, 1
    }

    OperationRegion(PCCS, PCI_Config, 0x84, 0x04)
    Field(PCCS, WordAcc, NoLock, Preserve) {
         PMSR,    32  // 84h:   Power Gating Control
    }



    OperationRegion (SCGI, SystemMemory, Or(GP3A, 0x100), 0x10)
    Field (SCGI, DWordAcc, NoLock, Preserve)
    {
      Offset (0x00),
      GPIS,   32,                     // GPI Interrupt Status
    }

    OperationRegion (SCGP, SystemMemory, Or(GP3A, 0x5D0), 0x2C)
    Field (SCGP, DWordAcc, NoLock, Preserve)
    {
      Offset (0x00),                 // SDCARD_CD (177-26) Pad Cfg DW0
      ,     1,
      RXST, 1,                       // GPIO RX State
      ,    23,
      RXEV, 2,                       // RX Level/Edge Configuration
      Offset (0x20),                 // SDCARD_PWR_DOWN_B (183-30) Pad Cfg DW0
      GPOV, 1                        // GPO Value
    }

    //
    // Update GPIO Interrupt status and value used by SD card.
    //
    Method (WAK, 0x0, Serialized) {
      //
      // Write clear GPIO_177 Interrupt status (bit26)
      //
      If (LEqual (\_SB.PCI0.SDC.GPIS, 0x04000000)) {
        Store (0x04000000, \_SB.PCI0.SDC.GPIS)
      }
      \_SB.SPC0 (SW_GPIO_177, GDW0)
      \_SB.SPC1 (SW_GPIO_177, GDW1)
    }

    Method(_INI, 0)
    {
      Not(\_SB.PCI0.SDC.RXST, \_SB.PCI0.GRR3)  // Set Card Presence in host controller to inverse of RX State in GPIO pad
    }

    Method (_PS0, 0, NotSerialized) // _PS0: Power State 0
    {
      //
      // Set Card Presence in Host Controller to Inverse of RX State in GPIO Pad
      //
      Not(\_SB.PCI0.SDC.RXST, \_SB.PCI0.GRR3)

      //
      // Disable power gate
      //
      Store (0, \_SB.PCI0.SDC.DPGE)
      Store (0, \_SB.PCI0.SDC.I3EN)

      //
      // Clear clock gate
      //
      \_SB.PCI0.SCPG(0, 0xFFFFFBFE) // Clear bit 10 and 0

      Sleep (2)                    // Sleep 2 ms

      //
      // Restore clock gate
      //
      \_SB.PCI0.SCPG(1, 0x00000401) // restore bit 10 and 0

      //
      // Turn Card Power On
      //
      // GPIO 183: Set GPIOTXState = 0
      //
      Store (0, \_SB.PCI0.SDC.GPOV)


      //
      // Save GPIO_177 DW0 and DW1
      //
      If (LAnd (LEqual (GDW0, 0), LEqual (\_SB.PCI0.SDC.RXEV, 0))) {
        Store (\_SB.GPC0 (SW_GPIO_177), GDW0)
        Store (\_SB.GPC1 (SW_GPIO_177), GDW1)
      }
    }

    Method (_PS3, 0, NotSerialized) // _PS3: Power State 3
    {
      //
      // Turn Card Power Off
      //
      // GPIO 183: Set GPIOTXState = 1
      //
      Store (1, \_SB.PCI0.SDC.GPOV)


      //
      // Clear Normal Interrupt Status Enable Bit 6 & 7
      // Clear Normal Interrupt Signal Enable Bit 6 & 7
      //
      // Note: SB Port = 0xD6 (SCC), Reg Offset = 0x34 and Reg Offset = 0x38, OpCode = MRd (0)

      // Only Perform the WA If the Card is Already Inserted
      If (LEqual(\_SB.PCI0.GRR3, 1))
      {
        SBIM (0xD6000034, 0, 0x1, 0x30D8)
        SBIM (0xD6000038, 0, 0x1, 0x30D8)
      }

      //
      // Clear Present Bit (S0ix Fix)
      //
      Store (0, \_SB.PCI0.GRR3)

      //
      // Restore clock gate
      //
      \_SB.PCI0.SCPG(1, 0x00000401) // restore bit 10 and 0

      //
      // Enable power gate
      //
      Store (1, \_SB.PCI0.SDC.DPGE)
      Store (1, \_SB.PCI0.SDC.I3EN)

      //
      // Dummy read PMCSR (SCC D3 WA)
      //
      Store (PMSR, Local0)
      And (Local0, 1, Local0) // Dummy operation on Local0
    }

    Method(_DSM, 0x4, Serialized)
    {
      //
      // Switch based on which unique function identifier was passed in Arg3.
      //
      If (LEqual(Arg0, ToUUID("f6c13ea5-65cd-461f-ab7a-29f7e8d5bd61")))
      {
        Switch (ToInteger(Arg2))
        {
          //
          // Function 0: Return supported functions.
          // Currently, we only support: Query, 1.8v & 3.3v switch support functions.
          //
          Case (0)
          {
            Switch (ToInteger(Arg1))
            {
              //
              // REVISION 0: DSM has 6 Supported SUB functions
              //
              Case (0) {Return (Buffer() {0x19})}
            }
            // Default Case: Report no functions supported
            Return (Buffer() {0x00})
          }
          //
          // Function 1: Clear Power Control Register
          //
          Case (1)
          {
            //Store (Zero, DL13)
            Return (Zero)
          }
          //
          // Function 2: Set power control register
          //
          Case (2)
          {
            //
            // Set Power Control Code
            //

            Return (Buffer(){0x00})
          }
          // Function 3: Set 1.8v Signalling
          Case (3)
          {
            //
            // 1.8v Signalling Code
            //
            // BXT-P: Nothing to be done here.
            //

            Return (Buffer(){0x00})
          }
          // Function 4: Set 3.3v Signalling
          Case (4)
          {
            //
            // 3.3v Signalling Code
            //
            //  Power Off SD Card:
            //    GPIO 183:
            //      Set gpiotxstate =1
            //

            Store (1, \_SB.PCI0.SDC.GPOV)

            // Sleep for 50ms Between Powering Off / Powering On
            Sleep (50)

            // Power On SD Card:
            //     GPIO 183:
            //       Set gpiotxstate = 0

            Store (0, \_SB.PCI0.SDC.GPOV)

            Return (Buffer(){0x00})
          }
          // Function 5: Enable HS200 capability
          Case (5)
          {
            Return (Buffer(){0x00})
          }
          // Function 6: Enable HS400 capability
          Case (6)
          {
            Return (Buffer(){0x00})
          }
          // Function 7 (Reserved): Enable Platform Speed Register
        }
        //
        // If not one of the function identifiers we recognize, then return a buffer
        // with bit 0 set to 1 indicating no functions supported.
        //
        Return (Buffer(){0x01})
      }
      Else
      {
        Return (Buffer(One){0x0})
      }
    }

  } // Device(SDC)

}

