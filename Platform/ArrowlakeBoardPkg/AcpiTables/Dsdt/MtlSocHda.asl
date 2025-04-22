/** @file

  Copyright (c) 2024, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#define UAOE_NVS_PATH                  \UAOE
#define NHLT_ADDRESS_NVS_PATH          \NHAA
#define NHLT_LENGTH_NVS_PATH           \NHLL
#define AUDIO_FEATURE_MASK_NVS_PATH    \ADFM
#define XTAL_NVS_PATH                  \XTAL
#define SNWD0_QUIRK_MASK_NVS_PATH      \SWQ0
#define SNWD1_QUIRK_MASK_NVS_PATH      \SWQ1
#define SNWD2_QUIRK_MASK_NVS_PATH      \SWQ2
#define SNWD3_QUIRK_MASK_NVS_PATH      \SWQ3
#define SNWD0_ACS_NVS_PATH             \ACS0
#define SNWD1_ACS_NVS_PATH             \ACS1
#define SNWD2_ACS_NVS_PATH             \ACS2
#define SNWD3_ACS_NVS_PATH             \ACS3
#define SNWD0_DAI_NVS_PATH             \DAI0
#define SNWD1_DAI_NVS_PATH             \DAI1
#define SNWD2_DAI_NVS_PATH             \DAI2
#define SNWD3_DAI_NVS_PATH             \DAI3
#define SNWD0_DOD_NVS_PATH             \DOD0
#define SNWD1_DOD_NVS_PATH             \DOD1
#define SNWD2_DOD_NVS_PATH             \DOD2
#define SNWD3_DOD_NVS_PATH             \DOD3
#define SNDW_MASTER_COUNT_NVS_PATH     \SWMC
#define HDAS_PATH                      PC00.HDAS

//
// High Definition Audio Controller - Device 31, Function 3
//
Device (HDAS)
{
    Name (_ADR, 0x001F0003)

    // D0 Method for HD-A Controller
    Method (_PS0, 0, Serialized)
    {
    ADBG ("HD-A Ctrlr D0")

    \_SB.S023 (0, 1)
    //
    // Call platform HDAS PS0 method if present
    //
    If (CondRefOf (\_SB.PC00.HDAS.PS0X)) {
        \_SB.PC00.HDAS.PS0X ()
    }
    }

    // D3 Method for HD-A Controller
    Method (_PS3, 0, Serialized)
    {
    ADBG ("HD-A Ctrlr D3")

    \_SB.S023 (0, 0)
    //
    // Call platform HDAS PS3 method if present
    //
    If (CondRefOf (\_SB.PC00.HDAS.PS3X)) {
        \_SB.PC00.HDAS.PS3X ()
    }
    }

    Method (_PRW, 0) {
    If (CondRefOf (GP1E)){
        If (LNotEqual (GP1E, 0)) {
        Return (GPRW (GPE1_ACE_PME_B0, 4))
        }
    }
    Return (GPRW (0x6D, 4)) // can wakeup from S4 state
    }

    include ("Hda.asl")
    include ("HdaIda.asl")
}

Scope (HDAS.IDA) {
    //
    // High Definition Audio - SoundWire Controller
    //
    include ("HdaSoundWireCtrl.asl")

    If (LNotEqual (UAOE,0)) {
    //
    // High Definition Audio - USB Audio Offload
    //
    include ("HdaUsbAudioOffload.asl")
    }
} // END Device (HDAS)

#undef UAOE_NVS_PATH
#undef NHLT_ADDRESS_NVS_PATH
#undef NHLT_LENGTH_NVS_PATH
#undef AUDIO_FEATURE_MASK_NVS_PATH
#undef XTAL_NVS_PATH
#undef SNWD0_QUIRK_MASK_NVS_PATH
#undef SNWD1_QUIRK_MASK_NVS_PATH
#undef SNWD2_QUIRK_MASK_NVS_PATH
#undef SNWD3_QUIRK_MASK_NVS_PATH
#undef SNWD0_ACS_NVS_PATH
#undef SNWD1_ACS_NVS_PATH
#undef SNWD2_ACS_NVS_PATH
#undef SNWD3_ACS_NVS_PATH
#undef SNWD0_DAI_NVS_PATH
#undef SNWD1_DAI_NVS_PATH
#undef SNWD2_DAI_NVS_PATH
#undef SNWD3_DAI_NVS_PATH
#undef SNWD0_DOD_NVS_PATH
#undef SNWD1_DOD_NVS_PATH
#undef SNWD2_DOD_NVS_PATH
#undef SNWD3_DOD_NVS_PATH
#undef SNDW_MASTER_COUNT_NVS_PATH
#undef HDAS_PATH
