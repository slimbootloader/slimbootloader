/**@file

  Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#define BIT0    0x00000001
#define BIT1    0x00000002
#define BIT2    0x00000004
#define BIT3    0x00000008
#define BIT4    0x00000010
#define BIT5    0x00000020
#define BIT6    0x00000040
#define BIT7    0x00000080
#define BIT8    0x00000100
#define BIT9    0x00000200
#define BIT10   0x00000400
#define BIT11   0x00000800

#define BIT29   0x20000000
#define BIT30   0x40000000
#define BIT31   0x80000000

Name(ADFM,0x2a)
Name(ADPM,0)
Name(AG1L,0)
Name(AG1H,0)
Name(AG2L,0)
Name(AG2H,0)
Name(AG3L,0)
Name(AG3H,0)

  //
  // High Definition Audio - Device 14, Function 0
  //
  Device(HDAS) {
    Name(_ADR, 0x000E0000)

    OperationRegion(HDAR, PCI_Config, 0x00,0x100)
    Field(HDAR,ByteAcc,NoLock,Preserve) {
      VDID,    32,  // 0x00, VID DID
      Offset(0x48), // 0x48, CGCTL - Clock Gating Control
          ,    6,
      MBCG,    1,   // MISCBDCGE [BIT6]
      Offset(0x54), // 0x54, Power Management Control and Status Register
          ,    8,
      PMEE,    1,   // PME_EN
          ,    6,
      PMES,    1    // PME Status
    }

    // NHLT Table memory descriptor, returned from _DSM
    Name(NBUF, ResourceTemplate () {
    // NHLT table address (_MIN = NHLT 64bit pointer, _MAX = _MIN + _LEN - 1) and length (_LEN)
    // Move to 64 bit mode from 32 bit
    QWordMemory (ResourceConsumer, , MinNotFixed, MaxNotFixed, NonCacheable, ReadOnly,
                0x1,                // AddressGranularity
                0x0000000000000000, // AddressMinimum _MIN
                0x0000000000000000, // AddressMaximum _MAX
                0x0,
                0x0,                // RangeLength _LEN
                , , NHLT, AddressRangeACPI,)
    })

    Name(_S0W, 3) // Device can wake itself from D3 in S0

    Method(_DSW, 3) { Store(Arg0, PMEE) } // Device wake enable

    Name (_PRW, Package() {0x0E, 3})  // can wakeup from S3 state

    Method(_PS0,0,Serialized)     /// D0 Method for HD-A Controller
    {
    }
    Method(_PS3,0,Serialized)     /// D3 Method for HD-A Controller(Dummy routine to support D3 state)
    {
    }

    Method(_INI, 0, NotSerialized)  // _INI: Initialize
    {
      // Update resource according to NVS

      // Set NHLT base address and length
      CreateQWordField(NBUF, ^NHLT._MIN, NBAS)
      CreateQWordField(NBUF, ^NHLT._MAX, NMAS)
      CreateQWordField(NBUF, ^NHLT._LEN, NLEN)
      Store(NHLA, NBAS)
      Add(NHLA, Subtract(NHLL, 1), NMAS)
      Store(NHLL, NLEN)
    }

    Method(_DSM, 0x4, Serialized, 0, UnknownObj, {BuffObj, IntObj, IntObj, PkgObj}) {
      // Arg0 - UUID: A69F886E-6CEB-4594-A41F-7B5DCE24C553 (Buffer)
      // Arg1 - Revision ID: 0x01 (Integer)
      // Arg2 - Function Index: 0x0 - 0x3 (Integer) - See below for details.
      // Arg3 - Depends on Function Index - See below for details.
      // Return - Depends on Function Index - See below for details.
      // Verify UUID
      If (LEqual(Arg0, ToUUID ("A69F886E-6CEB-4594-A41F-7B5DCE24C553"))){

        Switch(ToInteger(Arg2)) {

          // Function 0: Function Support Query
          // Arg2 - Function Index: 0x00 (Integer)
          // Arg3: Unused
          // Return: Bitmask of functions supported. (Buffer)
          Case(0) {
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
            //
            // Bit 0 == '1', WoV is supported, Bit 0 == '0', WoV not supported
            // Bit 1 == '1', BT Sideband is supported, Bit 1 == '0', BT not supported
            // Bit 2 == '1', codec based VAD support allowable
            // Bit 3 == '1', SramReclaim Feature enabled
            // Bit 4 Reserved
            // Bit 5 == '1', BT Intel HFP SCO is supported
            // Bit 6 == '1', BT Intel A2DP is supported
            // Bit 7 == '1', DSP based speech pre-processing disabled
            // Bit 8 == '1', Windows Voice Activation, Bit 8 == '0', Intel Wake on Voice
            // Bit 9 == '1'  Context Aware Feature enabled
            // Bit 10 - 31 Reserved, shall be set to '0'
            // ADFM - NVS AudioDSP Feature Bit Mask updated from PchPolicy
            //
            Return(ADFM)
          }
          // Function 3: Query Pre/Post Processing Module Support
          //                    Used by the Intel Offload Engine Driver to determine if a
          //                    specified PP Module is allowed to be supported on this platform
          // Arg2 - Function Index: 0x03 (Integer)
          // Arg3 - UUID: Specifies the UUID of the PP module to check (Buffer)
          // Return - TRUE if PP Module supported, else FALSE.
          Case (3) {
              // ADPM - NVS AudioDSP Post-Processing Module Bit Mask updated from PchPolicy: HdaConfig->DspPpModuleMask

              //
              // Example (to be updated with real GUIDs of supported 3rd party IP):
              //
              // 3rd Party DSP Processing Module 1 placeholder (enabled by policy HdaConfig->DspPpModuleMask |= BIT0)
              // Check PP module with GUID AABBCCDD-EEFF-1122-3344-556677889900
              // If (LEqual(Arg3, ToUUID ("AABBCCDD-EEFF-1122-3344-556677889900"))){
              //   Return(And(ADPM, 0x1)) // DspPpModuleMask[BIT0] / ADPM[BIT0] set - supported 3rd Party Processing Module 1(return true)
              // }
              //
              // 3rd Party DSP Processing Module 5 placeholder (enabled by policy HdaConfig->DspPpModuleMask |= BIT5)
              // Check PP module with GUID 11111111-2222-3333-4444-AABBCCDDEEFF
              // If (LEqual(Arg3, ToUUID ("11111111-2222-3333-4444-AABBCCDDEEFF"))){
              //   Return(And(ADPM, 0x20)) // DspPpModuleMask[BIT5] / ADPM[BIT5] set - supported 3rd Party Processing Module 5(return true)
              // }
              //
              // Implement for all supported PP modules
              //

            // PP Module Waves (module XAMAXXAU)
            // Check PP module with GUID B489C2DE-0F96-42E1-8A2D-C25B5091EE49
            If (LEqual(Arg3, ToUUID ("B489C2DE-0F96-42E1-8A2D-C25B5091EE49"))){
              Return(And(ADPM, BIT0)) // ADPM[BIT0] set - supported (return true)
            }

            // PP Module DTS (module PRMSND)
            // Check PP module with GUID E1284052-8664-4FE4-A353-3878F72704C3
            If (LEqual(Arg3, ToUUID ("E1284052-8664-4FE4-A353-3878F72704C3"))){
              Return(And(ADPM, BIT1)) // ADPM[BIT1] set - supported (return true)
            }

              // PP Module IntelSst Speech
            // Check PP module with GUID 7C708106-3AFF-40FE-88BE-8C999B3F7445
            If (LEqual(Arg3, ToUUID ("7C708106-3AFF-40FE-88BE-8C999B3F7445"))){
              Return(And(ADPM, BIT2)) // ADPM[BIT2] set - supported (return true)
            }

            // PP Module Dolby
            // Check PP module with GUID E0E018A8-3550-4B54-A8D0-A8E05D0FCBA2
            If (LEqual(Arg3, ToUUID ("E0E018A8-3550-4B54-A8D0-A8E05D0FCBA2"))){
              Return(And(ADPM, BIT3)) // ADPM[BIT3] set - supported (return true)
            }

            // PP Module Samsung SoundAlive
            // Check PP module with GUID 202BADB5-8870-4290-B536-F2380C63F55D
            If (LEqual(Arg3, ToUUID ("202BADB5-8870-4290-B536-F2380C63F55D"))){
              Return(And(ADPM, BIT4)) // ADPM[BIT4] set - supported (return true)
            }

            // PP Module Samsung SoundBooster
            // Check PP module with GUID EB3FEA76-394B-495D-A14D-8425092D5CB7
            If (LEqual(Arg3, ToUUID ("EB3FEA76-394B-495D-A14D-8425092D5CB7"))){
              Return(And(ADPM, BIT5)) // ADPM[BIT5] set - supported (return true)
            }

            // PP Module Samsung EQ/DRC
            // Check PP module with GUID F1C69181-329A-45F0-8EEF-D8BDDF81E036
            If (LEqual(Arg3, ToUUID ("F1C69181-329A-45F0-8EEF-D8BDDF81E036"))){
              Return(And(ADPM, BIT6)) // ADPM[BIT6] set - supported (return true)
            }

            // PP Module ForteMedia SAMSoft
            // Check PP module with GUID B3573EFF-6441-4A75-91F7-4281EEC4597D
            If (LEqual(Arg3, ToUUID ("B3573EFF-6441-4A75-91F7-4281EEC4597D"))){
              Return(And(ADPM, BIT7)) // ADPM[BIT7] set - supported (return true)
            }

            // PP Module WoV Intel
            // Check PP module with GUID EC774FA9-28D3-424A-90E4-69F984F1EEB7
            If (LEqual(Arg3, ToUUID ("EC774FA9-28D3-424A-90E4-69F984F1EEB7"))){
              Return(And(ADPM, BIT8)) // ADPM[BIT8] set - supported (return true)
            }

            // PP Module WoV Sensory
            // Check PP module with GUID F101FEF0-FF5A-4AD4-8710-43592A6F7948
            If (LEqual(Arg3, ToUUID ("F101FEF0-FF5A-4AD4-8710-43592A6F7948"))){
              Return(And(ADPM, BIT9)) // ADPM[BIT9] set - supported (return true)
            }

            // PP Module Conexant
            // Check PP module with GUID F3578986-4400-4ADF-AE7E-CD433CD3F26E
            If (LEqual(Arg3, ToUUID ("F3578986-4400-4ADF-AE7E-CD433CD3F26E"))){
              Return(And(ADPM, BIT10)) // ADPM[BIT10] set - supported (return true)
            }

            // PP Module Aware
            // Check PP module with GUID 13B5E4D7-A91A-4059-8290-605B01CCB650
            If (LEqual(Arg3, ToUUID ("13B5E4D7-A91A-4059-8290-605B01CCB650"))){
              Return(And(ADPM, BIT11)) // ADPM[BIT11] set - supported (return true)
            }
          }
          Default {
            // Function not supported (Arg2)
            Return(Buffer(One) { 0x00 })
          }
        } // Switch(Arg2) End
      } // If(Arg0, UUID) End
      // UUID not supported (Arg0)
      Return(Buffer(One) { 0x00 }) //Will never hit this but compiler is not happy without it
    } // _DSM End
} // end "High Definition Audio Controller"
