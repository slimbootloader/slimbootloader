/** @file
  This file contains the TCSS DMA ASL code.
  It defines a Power Management for TCSS DMA

  Copyright (c) 2021, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/
  //
  // For devices that support both D3hot and D3 exposed to OSPM via _PR3, device software/drivers
  // must always assume OSPM will target D3 and must assume all device context will be lost and the
  // device will no longer be enumerable. The possible way to access an OpRegion could be when the
  // access type configured as SystemMemory.
  //
  OperationRegion(DPME, SystemMemory, BASE(), 0x100)
  Field(DPME,ByteAcc,NoLock,Preserve)
  {
    Offset(0),
    VDID, 32,
    , 1,
    MEMS, 1,
    , 29,
    Offset(R_HOST_DMA_CFG_PM_CAP_1),    // 0x84
    PMST, 2,       // 1:0, PM_STATE
    , 6,
    PMEE, 1,       // 8, PME_EN
    , 6,
    PMES, 1,       // 15, PME_STATUS
    Offset(R_HOST_DMA_RTD3_FORCEPOWER), // 0xFC
    DD3E, 1,       // 0:0 DMA RTD3 Enable
    DFPE, 1,       // 1:1 DMA Force Power
    , 22,
    DMAD, 8        // 31:24 DMA Active Delay
  }

  Name (STAT, 0x1)  // Variable to save power state 1 - D0, 0 - D3C
  Name (ALCT, 0x0)  // Connect-Topology cmd can be sent or not 1 - yes, 0 - no
  Name (WACT, 0x0)  // Wait Connect-Topology cmd done, 2 - wait in progress, 1 - need to wait, 0 - no need to wait
  Name (SCME, 0x0)  // Variable to determine current SW CM mode is enabled or not after _OSC query. 1 - SW CM, 0 - FW CM

  //
  // This Control Method is used to put the specific device into its D0 state. The PMSS is not yet
  // changed (staying in previous state, could be D0 or D3H) when this method is called.
  //
  Method(_PS0,0,Serialized)
  {
  }

  //
  // This control method is used to put the specific device into its D3 state. The PMSS is in D3H typically
  // when this method is called, so this control method can only access Operation Regions that are either always
  // available while in the system working state (S0) or that are available when the Power Resources referenced
  // by the _PR0, _PR1 and PR2 objects are all ON. For devices that support both D3hot and D3 exposed to OSPM via
  // _PR3, device software/drivers must always assume OSPM will target D3 and must assume all device context will
  // be lost and the device will no longer be enumerable. The possible way to access an OpRegion could be when the
  // access type configured as SystemMemory.
  //
  Method(_PS3,0,Serialized)
  {
  }

  //
  // The deepest D-state supported by this device in the S0 system sleeping state where the device can wake itself,
  // "4" represents D3cold.
  //
  Method(_S0W, 0x0)
  {
    ADBG("TDMA S0W")
      Return(0x3)
  }
  //
  // RTD3 Exit Method to bring TBT controller out of RTD3 mode.
  //
  Method(D3CX, 0, Serialized)  // RTD3 Exit
  {
    ADBG("Disable DMA RTD3 bit")
    ADBG(Concatenate("DUID -", ToHexString(DUID)))
    Store(0, DD3E)                                 // Disable DMA RTD3
    Store(0x1, STAT)
  }

  //
  // RTD3 Entry method to enable TBT controller RTD3 mode.
  //
  Method(D3CE, 0, Serialized)  // RTD3 Entry
  {
    ADBG("Enable DMA RTD3 bit")
    ADBG(Concatenate("DUID -", ToHexString(DUID)))
    Store(1, DD3E)                                 // Enable DMA RTD3
    Store(0x0, STAT)
    If (LEqual (SCME, 0)) {
      //
      // The system is running in FW CM mode now
      //
      Store (0x1, ALCT)                               // Allow to send Connect-Topology cmd
      //
      // End of FW CM mode execution
      //
    }
  }

  //
  // Variable to skip TCSS/TBT D3 cold; 1+: Skip D3CE, 0 - Enable D3CE
  // TCSS D3 Cold and TBT RTD3 is only available when system power state is in S0.
  //
  Name(SD3C, 0)

  Method(_DSW, 3) {
    ADBG(Concatenate("TCSS DMA _DSW DUID -", ToHexString(DUID)))
    ADBG(Concatenate("Arg0 -", ToHexString(Arg0)))
    ADBG(Concatenate("Arg1 -", ToHexString(Arg1)))
    Store(Arg1, SD3C)                                 // If entering Sx (Arg1 > 1), need to skip TCSS D3Cold & TBT RTD3/D3Cold.
    ADBG(Concatenate("SD3C -", ToHexString(SD3C)))
  }

  //
  // Additionally if the device is capable of waking up
  // the platform from a low power System state (S3 or S4), there is a _S0W and _PRW
  // associated with the device as well.
  //
  Method(_PRW, 0) {
    //
    // Report the deepest wakeable state is S4 always in order to supporting TCSS RTD3 capability is some OS builds. The actual Sx wakeable state is mapped and managed by
    // VCCST's preference to IOM for remaining or removing VNNAON rail in S4 state. BIOS is no longer required to report _PRW with S3 wake capability to OS even if
    // the platform wants to limit USBC Sx wake capability to S4 only, or when VCCST support is disabled.
    // The actual S4 wake capability will be limited by VCST setting instead of OS managment. Platform shall set VCCST setting to OFF when not supporting S4 wake over USBC.
    //
    Return (GPRW (0x6D, 4)) // can wakeup from S4 state
  }

  Method (_DSD, 0) {
    ADBG(Concatenate("TBT _DSD WAKE_SUPPORTED =", ToHexString(TIVS)))

      If (TIVS) {
        Return (
          Package ()
          {
            ToUUID("C44D002F-69F9-4E7D-A904-A7BAABDF43F7"),
            Package ()
            {
              Package (2) {"IMR_VALID", 0}
            },
            ToUUID("6C501103-C189-4296-BA72-9BF5A26EBE5D"),
            Package ()
            {
              Package (2) {"WAKE_SUPPORTED", 1}
            }
          }
        ) // End of Return ()
      } Else {
        Return (
          Package ()
          {
            ToUUID("C44D002F-69F9-4E7D-A904-A7BAABDF43F7"),
            Package ()
            {
              Package (2) {"IMR_VALID", 0}
            },
            ToUUID("6C501103-C189-4296-BA72-9BF5A26EBE5D"),
            Package ()
            {
              Package (2) {"WAKE_SUPPORTED", 0}
            }
          }
        ) // End of Return ()
      }
  }

  Method (_DSM, 4, Serialized) {  // _DSM: Device-Specific Method
    //
    // _DSM Definition for TBT DMA functions
    // Arguments:
    // Arg0: UUID: 197DB5E0-F095-4F33-B915-71DD70833E55
    // Arg1: Revision ID: 1
    // Arg2: Function Index: 0, 1
    // Arg3: Additional Inputs Bits[31:0] Arg3 {Byte0, Byte1, Byte2, Byte3}
    //
    // Switch by function index
    //
      If (LEqual (Arg0,ToUUID("197DB5E0-F095-4F33-B915-71DD70833E55"))) { // TBT DMA
        If (LEqual (Arg2, 0)) {
        //
        // Function Index: 0
        // Standard query - A bitmask of functions supported
        //
        // Return: A bitmask of functions supported
        //
          If (LEqual (Arg1, 1)) {
            Return (Buffer() {0x07})  // Functions 1 and 2 supported for revision 1
          }
          Else { Return (Buffer() {0x01}) } // revisions higher than 1 are not supported
        }
        If (LEqual (Arg2, 1)) {
          //
          // Function Index: 1
          // Standard query - A bitmask of ITBT PCIe RootPort supported
          //
          // Return: A bitmask of ITBT PCIe RootPort supported
          //         below is the PCIe RP map
          //         00b: both ports disabled
          //         01b: first port is enable
          //         10b: second port is enabled
          //         11b: both ports enabled.
          Name(PCPN, Buffer(1){0})
          CreateBitField(PCPN, 0, PCP0)
          CreateBitField(PCPN, 1, PCP1)
          If (LEqual (DUID, 0)) {
            If(LEqual(TRE0, 1)) {
              Store (1, PCP0)
            }
            If(LEqual(TRE1, 1)) {
              Store (1, PCP1)
            }
          } Else {
            If(LEqual(TRE2, 1)) {
              Store (1, PCP0)
            }
            If(LEqual(TRE3, 1)) {
              Store (1, PCP1)
            }
          }

          Return (PCPN)
        }
        If (LEqual (Arg2, 2)) {
          //
          // Function Index: 2
          // Standard query - A bitmask of ITBT DMA capability information
          //
          // Return: A bitmask of ITBT DMA capability information
          //         Bit0: IMR_VALID
          //         Bit1: WAKE_SUPPORTED
          //         Bit2: vPro_Dock Supported
          //
          Name(TCAP, Buffer(1){0})
          CreateBitField(TCAP, 0, IMRV)
          CreateBitField(TCAP, 1, WAKE)
          CreateBitField(TCAP, 2, TVDS)
          If (TIVS) {
            Store (1, WAKE)
          }

          Return (TCAP)
        }
      } ElseIf (LEqual (Arg0, ToUUID ("1B425232-154C-4F34-7B9E-B65C946494A3"))) {
        Switch (ToInteger (Arg2)) {
          //
          // Function Index 0: Return supported functions, based on revision
          // one bit for each function index, starting with zero.
          // Bit 0 - Indicates whether there is support for any functions other than function 0 for the specified UUID and Revision ID.
          // Bit 1 - Indicates support to get DMA buffer address(size: 4MB) for TCSS DMA0/1
          //
          case (0) {
            ADBG(Concatenate("TBT DMA _DSM DUID:", ToHexString(DUID)))
            switch (ToInteger(Arg1)) {
              // revision 1: functions 1 are supported
              case (1) {
                return (Buffer() {0x3})
              }
            }
            // no functions are supported other than revision 1
            ADBG("return buffer 0x00")
            Return (Buffer() {0})
          }

          //
          // Function Index 1: Return 512KB DMA buffer address for WiAMT
          //
          Case(1) {
            ADBG(Concatenate("TBT DMA _DSM Func 1 DUID:", ToHexString(DUID)))
            If (Lequal (DUID, 1)) {
              ADBG(Concatenate("ITM1:", ToHexString(ITM1)))
              Return (ITM1)
            } Else {
              ADBG(Concatenate("ITM0:", ToHexString(ITM0)))
              Return (ITM0)
            }
          }
        } // End - Switch (ToInteger (Arg2))
      }
      Return (Buffer() {0})
    }

  //
  // Name: STCM
  // Description: Set CM mode of USB4 host router to OS preferred CM mode
  // Arguments:
  // Arg0 - OS USB4 native support state, the CM mode to be applied (0 = FW CM, 1 = SW CM)
  // Arg1 - CM mode information in BIOS phase (Bit 7 - Valid, Bit 3:0 - CM mode in BIOS phase)
  // Return : CM mode applied for OS phase (0 = FW CM, 1 = SW CM, 0xFF = Failure)
  //
  // Local0 - Pre-boot CM mode value (0 = FW CM, 1 = SW CM)
  // Local1 - Indicate CM mode switch is required or not
  //
  Method (STCM, 2, Serialized)
  {
    ADBG (Concatenate ("STCM DUID = ", ToHexString (DUID)))
    ADBG (Concatenate ("Requested CM mode = ", Arg0))
    ADBG (Concatenate ("Pre-boot CM Info  = ", ToHexString (Arg1)))

    //
    // Lower 3 bits of Arg1 indicate pre-boot CM mode value
    //
    And (Arg1, 0x07, Local0)

    //
    // Switch CM mode by default
    // It is not required only if pre-boot CM mode aligns with OS preferred CM mode
    //
    Store (1, Local1)

    //
    // U4CM Bit Definition
    // Bit 7   - Valid bit
    // Bit 6:4 - Setup CM value (0 = FW CM, 1 = SW CM, 2 = OS, 3 = Pass Through)
    // Bit 3   - Reserved
    // Bit 2:0 - Pre-boot CM mode (0 = FW CM, 1 = SW CM)
    //
    If (LNotEqual (And (Arg1, 0x80), 0x80))
    {
      ADBG ("U4CM valid bit is not set!")
      Return (0xFF)
    }
    Else
    {
      //
      // CM mode switch is not required if pre-boot CM mode aligns with OS preferred CM mode
      //
      Store (Local0, SCME)  // Save pre-boot CM setting to SCME
      If (LEqual (Arg0, Local0))
      {
        ADBG ("Pre-boot CM mode aligns with OS preferred CM mode")
        Store (0, Local1)
      }
    }
    ADBG(Concatenate("SCME - ", ToHexString(SCME)))
    Return (SCME)
  }
