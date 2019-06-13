/**@file

  Copyright (c) 2018 - 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

External(\_SB.PCI0.HDAS.PS0X, MethodObj)
External(\_SB.PCI0.HDAS.PS3X, MethodObj)
External(\_SB.PCI0.HDAS.PPMS, MethodObj)
External(\_SB.VMON, MethodObj)
External(\_SB.VMOF, MethodObj)
External(HIDW, MethodObj)
External(HIWC, MethodObj)

//
// High Definition Audio Controller - Device 31, Function 3
//
Device(HDAS)
{
  Name(_ADR, 0x001F0003)

  //
  // Define a Memory Region that will allow access to the HDA PCI Configuration Space
  //
  OperationRegion(HDAR, PCI_Config, 0x00, 0x100)
  Field(HDAR,WordAcc,NoLock,Preserve) {
    VDID,32       // 0x00, VID DID
  }

  Name(_S0W, 3) // Device can wake itself from D3 in S0

  Method(_DSW, 3) {}

  Method(_PRW, 0) {
    Return(GPRW(0x6D, 4)) // can wakeup from S4 state
  }

  // D0 Method for HD-A Controller
  Method(_PS0, 0, Serialized)
  {
    //
    // Check if Runtime Volate Margining is enabled
    //
    If(VMRC) {
      If(CondRefOf(\_SB.VMOF)) {
        // Disable voltage margining on entering D0
        \_SB.VMOF()
        Store(1,XSQD)
      }
    }
    //
    // Call platform HDAS PS0 method if present
    //
    If(CondRefOf(\_SB.PCI0.HDAS.PS0X)) {
      \_SB.PCI0.HDAS.PS0X()
    }
  }

  // D3 Method for HD-A Controller
  Method(_PS3, 0, Serialized)
  {
    //
    // Check if Runtime Volate Margining is enabled
    //
    If(VMRC) {
      If(CondRefOf(\_SB.VMON)) {
        // Enable voltage margining on entering D3
        \_SB.VMON()
        Store(0,XSQD)
      }
    }

    //
    // Call platform HDAS PS3 method if present
    //
    If(CondRefOf(\_SB.PCI0.HDAS.PS3X)) {
      \_SB.PCI0.HDAS.PS3X()
    }
  }

  // NHLT Table memory descriptor, returned from _DSM
  Name(NBUF, ResourceTemplate () {
    // NHLT table address (_MIN = NHLT 64bit pointer, _MAX = _MIN + _LEN - 1) and length (_LEN)
    QWordMemory (ResourceConsumer, , MinNotFixed, MaxNotFixed, NonCacheable, ReadOnly,
                0x1,                // AddressGranularity
                0x0000000000000000, // AddressMinimum _MIN
                0x0000000000000000, // AddressMaximum _MAX
                0x0,
                0x0,                // RangeLength _LEN
                , , NHLT, AddressRangeACPI,)
  })

#ifdef CPU_CFL
  // DSP FW stolen memory descriptor
  Name (DBUF, ResourceTemplate () {
    Memory32Fixed (ReadWrite, 0x00000000, 0x00400000, DFSM)
  })
#endif

  Method(_INI) {
    // Update resource according to NVS
    ADBG("HDAS _INI")

    // Set NHLT base address and length
    CreateQWordField(NBUF, ^NHLT._MIN, NBAS)
    CreateQWordField(NBUF, ^NHLT._MAX, NMAS)
    CreateQWordField(NBUF, ^NHLT._LEN, NLEN)
    Store(NHLA, NBAS)
    Add(NHLA, Subtract(NHLL, 1), NMAS)
    Store(NHLL, NLEN)

#ifdef CPU_CFL
    // Set DSP stolen memory base address
    CreateDWordField(DBUF, ^DFSM._BAS, HDBA)
    Store(DSPM, HDBA)
#endif
  }

  Name(_DSD, Package () {
    ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
    // Properties related to HDAS
    Package () {
      Package (2) {"nhlt-version","1.8-0"}
    }
  })

  Method(_DSM, 0x4, NotSerialized, 0, UnknownObj, {BuffObj, IntObj, IntObj, PkgObj}) {
    // Arg0 - UUID: A69F886E-6CEB-4594-A41F-7B5DCE24C553 (Buffer)
    // Arg1 - Revision ID: 0x01 (Integer)
    // Arg2 - Function Index: 0x0 - 0x3 (Integer) - See below for details.
    // Arg3 - Depends on Function Index - See below for details.
    // Return - Depends on Function Index - See below for details.

    ADBG("HDAS _DSM")

    if(PCIC(Arg0)) { return(PCID(Arg0,Arg1,Arg2,Arg3)) }

    // Verify UUID
    If (LEqual(Arg0, ToUUID ("A69F886E-6CEB-4594-A41F-7B5DCE24C553"))) {

      Switch(ToInteger(Arg2)) {

        // Function 0: Function Support Query
        // Arg2 - Function Index: 0x00 (Integer)
        // Arg3: Unused
        // Return: Bitmask of functions supported. (Buffer)
        Case(0) {
#ifdef CPU_CFL
          If(LEqual(PCHS, PCHL) && LLess(PSTP, PCH_B0)) { // Apply until CNL-LP B0 stepping (B0 RevID = 0x10)
            // Supports Function 0 - 4
            Return(Buffer(One) { 0x1F })
          }
#endif
          // Supports function 0 - 3
          Return(Buffer(One) { 0x0F })
        }

        // Function 1: Query Non HD Audio Descriptor Table
        //                    Used by the Intel Offload Engine Driver to discover the
        //                    non HD Audio devices supported by the Audio DSP.
        // Arg2 - Function Index: 0x01 (Integer)
        // Arg3 - Unused
        // Return - ACPI Table describing the non HD Audio links and devices supported by the ADSP (ResourceBuffer)
        Case(1) {
          ADBG("_DSM Fun 1 NHLT")
          // NBUF - Memory Resource Descriptor buffer with address and length of NHLT
          Return(NBUF)
        }

        // Function 2: Query Feature Mask
        //                    Used by the Intel Offload Engine Driver to retrieve a bitmask
        //                    of features allowable on this platform.
        // Arg2 - Function Index: 0x02 (Integer)
        // Arg3: Unused
        // Return: Bitmask of supported features.
        Case (2) {
          ADBG("_DSM Fun 2 FMSK")
          // Bit 0 == '1', WoV is supported, Bit 0 == '0', WoV not supported
          // Bit 1 == '1', BT Sideband is supported, Bit 1 == '0', BT not supported
          // Bit 2 == '1', codec based VAD support allowable
          // Bit 3 - 4 Reserved
          // Bit 5 == '1', BT Intel HFP SCO is supported
          // Bit 6 == '1', BT Intel A2DP is supported
          // Bit 7 == '1', DSP based speech pre-processing disabled
          // Bit 8 == '1', Windows Voice Activation, Bit 8 == '0', Intel Wake on Voice
          // Bit 9 - 31 Reserved, shall be set to '0'
          // ADFM - NVS AudioDSP Feature Bit Mask updated from PchPolicy
          Return(ADFM)
        }

        // Function 3: Query Pre/Post Processing Module Support
        //                    Used by the Intel Offload Engine Driver to determine if a
        //                    specified PP Module is allowed to be supported on this platform
        // Arg2 - Function Index: 0x03 (Integer)
        // Arg3 - UUID: Specifies the UUID of the PP module to check (Buffer)
        // Return - TRUE if PP Module supported, else FALSE.
        Case (3) {
          ADBG("_DSM Fun 3 PPMS")

          If(CondRefOf(\_SB.PCI0.HDAS.PPMS)) {
            // Call platform method PPMS to check if 3rd Party IP module with given GUID (Arg3) is supported
            Return(\_SB.PCI0.HDAS.PPMS (Arg3))
          }

          Return(0) // Is not supported
        }
#ifdef CPU_CFL
        Case(4) {
          ADBG("_DSM Fun 4 DBUF")
          // DBUF - Memory Resource Descriptor buffer with address and length of DBUF
          Return(DBUF)
        }
#endif
        Default {
          // Function not supported (Arg2)
          ADBG("_DSM Fun NOK")
          Return(Buffer(One) { 0x00 })
        }
      } // Switch(Arg2) End
    } // If(Arg0, UUID) End

    //-------------------------------------------
    //  HID Wake up Event solution
    //-------------------------------------------
    If(CondRefOf(HIWC)) {
      If(HIWC(Arg0)) {
        If(CondRefOf(HIDW)) {
          Return (HIDW(Arg0, Arg1, Arg2, Arg3))
        }
      }
    }

    // UUID not supported (Arg0)
    ADBG("_DSM UUID NOK")
    Return(Buffer() {0})
  } // _DSM End
} // end "High Definition Audio Controller"
