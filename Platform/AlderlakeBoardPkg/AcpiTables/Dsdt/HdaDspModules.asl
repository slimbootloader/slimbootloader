/** @file

  Copyright (c) 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef  BIT0
#define  BIT0     0x00000001
#endif
#ifndef  BIT1
#define  BIT1     0x00000002
#endif
#ifndef  BIT2
#define  BIT2     0x00000004
#endif
#ifndef  BIT3
#define  BIT3     0x00000008
#endif
#ifndef  BIT4
#define  BIT4     0x00000010
#endif
#ifndef  BIT5
#define  BIT5     0x00000020
#endif
#ifndef  BIT6
#define  BIT6     0x00000040
#endif
#ifndef  BIT7
#define  BIT7     0x00000080
#endif
#ifndef  BIT8
#define  BIT8     0x00000100
#endif
#ifndef  BIT9
#define  BIT9     0x00000200
#endif
#ifndef  BIT10
#define  BIT10    0x00000400
#endif
#ifndef  BIT11
#define  BIT11    0x00000800
#endif
#ifndef  BIT12
#define  BIT12    0x00001000
#endif
#ifndef  BIT13
#define  BIT13    0x00002000
#endif
#ifndef  BIT14
#define  BIT14    0x00004000
#endif
#ifndef  BIT15
#define  BIT15    0x00008000
#endif
#ifndef  BIT16
#define  BIT16    0x00010000
#endif
#ifndef  BIT17
#define  BIT17    0x00020000
#endif
#ifndef  BIT18
#define  BIT18    0x00040000
#endif
#ifndef  BIT19
#define  BIT19    0x00080000
#endif
#ifndef  BIT20
#define  BIT20    0x00100000
#endif
#ifndef  BIT21
#define  BIT21    0x00200000
#endif
#ifndef  BIT22
#define  BIT22    0x00400000
#endif
#ifndef  BIT23
#define  BIT23    0x00800000
#endif
#ifndef  BIT24
#define  BIT24    0x01000000
#endif
#ifndef  BIT25
#define  BIT25    0x02000000
#endif
#ifndef  BIT26
#define  BIT26    0x04000000
#endif
#ifndef  BIT27
#define  BIT27    0x08000000
#endif
#ifndef  BIT28
#define  BIT28    0x10000000
#endif
#ifndef  BIT29
#define  BIT29    0x20000000
#endif
#ifndef  BIT30
#define  BIT30    0x40000000
#endif
#ifndef  BIT31
#define  BIT31    0x80000000
#endif

Scope(\_SB.PC00.HDAS)
{
  //
  // Pre/Post processing modules (3rd Party IP) support for Intel Smart Sound Technology
  //
  Method(PPMS, 1, Serialized)
  {
    // Helper platform method for HDAS._DSM Function 3 - Query Pre/Post Processing Module Support
    // Arg0 (equals Arg3 passed to _DSM) - UUID: Specifies the UUID of the 3rd Party IP module to check (Buffer)
    // Return - TRUE if PP Module supported, else FALSE.

    // ADPM - Platform NVS AudioDSP Post-Processing Module Bit Mask:
    //  [BIT0] - Waves; [BIT1] - DTS; [BIT2] - Intel SST Speech; [BIT3] - Dolby
    //  [BIT4] - Icepower; [BIT5] - Audyssey; [BIT6] - [Unused]
    //  [BIT7] - ForteMedia SAMSoft; [BIT8] - Intel WoV; [BIT9] - Sound Research IP
    //  [BIT10] - Conexant Pre-Process; [BIT11] - Conexant Smart Amp
    //  [BIT12] - Realtek Post-Process; [BIT13] - Realtek Smart Amp
    //  [BIT14] - PP Module Icepower IP MFX sub module; [BIT15] - PP Module Icepower IP EFX sub module
    //  [BIT16] - PP Module Voice Preprocessing; [BIT17] - PP Module Voice Preprocessing
    //  [BIT31:29] - Custom 3rd party IP modules (GUIDs provided from BIOS Setup)

    //
    // Example (to be updated with real GUIDs of supported 3rd party IP):
    //
    // 3rd Party DSP Processing Module placeholder
    // Check support for 3rd Party IP module with GUID AABBCCDD-EEFF-1122-3344-556677889900
    // If (LEqual(Arg0, ToUUID ("AABBCCDD-EEFF-1122-3344-556677889900"))){
    //   Return (1)) // GUID passed to _DSM Function 3 as Arg3 matches - module supported, return true.
    // }
    //
    // Implement for all supported PP modules
    //

    // PP Module Intel SST Speech
    If (LEqual(Arg0, ToUUID ("7C708106-3AFF-40FE-88BE-8C999B3F7445"))){
      Return(And(ADPM, BIT2)) // DSDT NVS ADPM[BIT2] set - supported (return true)
    }

    // PP Module Intel WoV
    If (LEqual(Arg0, ToUUID ("EC774FA9-28D3-424A-90E4-69F984F1EEB7"))){
      Return(And(ADPM, BIT8)) // ADPM[BIT8] set - supported (return true)
    }

    // PP Module 'Alpha' Custom GUID
    // Check PP module with GUID - entered in Setup menu
    If (LEqual(Arg0, ACCG(AG1L, AG1H))){
      Return(And(ADPM, BIT29)) // ADPM[BIT29] set - supported (return true)
    }

    // PP Module 'Beta' Custom GUID
    // Check PP module with GUID - entered in Setup menu
    If (LEqual(Arg0, ACCG(AG2L, AG2H))){
      Return(And(ADPM, BIT30)) // ADPM[BIT30] set - supported (return true)
    }

    // PP Module 'Gamma' Custom GUID
    // Check PP module with GUID - entered in Setup menu
    If (LEqual(Arg0, ACCG(AG3L, AG3H))){
      Return(And(ADPM, BIT31)) // ADPM[BIT31] set - supported (return true)
    }

    Return (0) // GUID not found - module not supported
  }

  Method (ACCG, 2, NotSerialized)
  {
    // ACCG - AudioDSP custom Pre/Post-Processing module GUID create
    // Auxiliary method to be used inside _DSM Function 3 / PPMS() (PP Module support)
    // Argument values passed from BIOS Setup as two 64bit variables
    // Arg0 - First half of GUID  - bits 0-63
    // Arg1 - Second half of GUID - bits 64-127
    // Return - 16 byte buffer containing GUID with the same byte order as ToUUID function
    Name(GBUF, Buffer(16) {})
    Concatenate(Arg0, Arg1, GBUF)
    Return (GBUF)
  }
}
