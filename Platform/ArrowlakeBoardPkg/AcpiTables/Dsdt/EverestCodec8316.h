/** @file
Header file for Everest Information

  Copyright (c) 2024, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
///0x00
#define ONBOARD_MICRPHONE_TYPE                           0x00
#define ONBOARD_MICRPHONE_TYPE_MIC_2CH                   0x55
//0x01
#define HEADPHONE_MICROPHONE_LOCATION                    0x01
#define HEADPHONE_MICROPHONE_LOCATION_LIN2RIN2_DFLT      0xCC
//0x02
#define SPEAKER_TYPE                                     0x02
#define SPEAKER_TYPE_STEREO_SPEAKER_DFLT                 0x02
//0x03
#define POLARITY_TYPE                                    0x03
#define POLARITY_TYPE_NORMAL                             0x00
#define POLARITY_TYPE_INVERTED_DFLT                      0x01
//0x04
#define FORMAT_TYPE                                      0x04
#define FORMAT_TYPE_DSP_PCMA_DFLT                        0x03
//0x0E
#define EVREST_CODEC_TYPE                               0x0E
#define EVREST_CODEC_TYPE_8316_8336_DFLT                0x10
#define EVREST_CODEC_TYPE_8326                          0x1A
//0x0f
#define EVEREST_I2S_BUS_NUMBER                          0x0F
#define EVEREST_MTL_I2S1_BUS_NUMBER                     0x01
//0x10
#define HEADSET_MPHONE_BOOST_GAIN                       0x10
#define HEADSET_MPHONE_BOOST_GAIN_DFLT                  0x06
//0x11
#define HEADSET_MPHONE_15DB_GAIN                        0x11
#define HEADSET_MPHONE_15DB_GAIN_DFLT                   0x00
#define HEADSET_MPHONE_15DB_GAIN_8326                   0x01

//0x20
#define ONBOARD_MPHONE_BOOST_GAIN                       0x20
#define ONBOARD_MPHONE_BOOST_GAIN_DFLT                  0x08


typedef enum {
    OnBoardMicrophonetype = 0x00,
    HeadphoneMicrophoneLocation,
    SpeakerType,
    HeadsetDetectPolarity,
    PcmFormat,
    Reserved=5, //0x05 Reserved for Microphone Array
    CodecType= 0x0E,
    I2sBuNumberUsed,//0x0f
    HeadSetMicrophoneBoostGain=0x10,//Headset Microphone Boost Gain
    HeadSetMicrophone15dbGain,
    HeadSetMicrophoneVolume,
    HeadSetMicrophoneAlcEnable,
    HeadsetMicrophoneAlcParameters14 =0x14,//0x14-0x1b Headset Microphone ALC Parameters
    HeadsetMicrophoneAlcParameters15,
    HeadsetMicrophoneAlcParameters16,
    HeadsetMicrophoneAlcParameters17,
    HeadsetMicrophoneAlcParameters18,
    HeadsetMicrophoneAlcParameters19,
    HeadsetMicrophoneAlcParameters1A,
    HeadsetMicrophoneAlcParameters1B,
    OnBoardMicrophoneBoostGain=0x20,//Onboard Microphone Boost Gain
    OnBoardMicrophone15bbGain,
    OnBoardMicrophoneVolume,
    OnBoardMicrophoneAicEnable,
    OnboardMicrophoneAlcParameters24 = 0x24,//0x24-0x2b Headset Microphone ALC Parameters
    OnboardMicrophoneAlcParameters25,
    OnboardMicrophoneAlcParameters26,
    OnboardMicrophoneAlcParameters27,
    OnboardMicrophoneAlcParameters28,
    OnboardMicrophoneAlcParameters29,
    OnboardMicrophoneAlcParameters2A,
    OnboardMicrophoneAlcParameters2B,
    MicrophoneGainStep = 0x2c,
    MicrophoneGainRange = 0x2d,
    //0x2E - 0x3F Not used
    HeadphoneModeDacMixerHiGain =0x40,
    HeadphoneModeDacMixerVolume = 0x41,
    HeadphoneDriverVolume = 0x42,
    HeadphoneModeDacSourceSelect = 0x43,
    HeadphoneModeLeftDacVolume=0x44,
    HeadphoneModeRightDacVolume = 0x45,
    HeadphoneAutoMuteEnable = 0x46,
    HeadphoneSoftrampMode,
    //0x48 - 0x4A    Not used
    HeadphoneGpioPolarity=0x4b,
    //0x4C    Not Used
    //0x4D, 0x4E, 0x4F Not Used
    SpeakerModeDacMixerHiGain = 0x50,
    SpeakerModeDacMixerVolume,
    SpeakerDriverVolume,
    SpeakerModeDacSourceSelect,
    SpeakerModeLeftDacVolume = 0x54,
    SpeakerModeRightDacVolume = 0x55,
    SpeakerAutoMuteEnable = 0x56,
    SpeakerSoftrampMode=0x57,
    SpeakerModeDacHiGain,
    //0x58 - 0x5A    Not used
    PaEnableGpioPolarity = 0x5B,
    //0x5c,0x5D, 0x5E, 0x5F, 0x61    Not Used
    //0x62 - 0x84    Not used
    I2sMclkBclkDefinition=0x85,
    PcType,
    //0x87-0x8D    Not Used
    AudioPathType=0x8E,
    RenderCaptureEndpointNumbers,
    RenderAudioPath1Description,
    RenderAudioPath2Description,
    RenderAudioPath3Description,
    RenderAudioPath4Description,
    CaptureAudioPath1Description=0x94,
    CaptureAudioPath2Description,
    CaptureAudioPath3Description,
    CaptureAudioPath4Description,
    RenderPath1TimeSlotAndDetectCap=0x98,
    RenderPath2TimeSlotAndDetectCap,
    RenderPath3TimeSlotAndDetectCap,
    RenderPath4TimeSlotAndDetectCap,
    CapturePath1TimeSlotAndDetectCap = 0x9c,
    CapturePath2TimeSlotAndDetectCap,
    CapturePath3TimeSlotAndDetectCap,
    CapturePath4TimeSlotAndDetectCap,
    RenAudioPath1ConnectionTypeandportConnection=0xA0,
    RenAudioPath2ConnectionTypeandportConnection,
    RenAudioPath3ConnectionTypeandportConnection,
    RenAudioPath4ConnectionTypeandportConnection,
    CaptureAudioPath1ConnectionTypeandportConnection = 0xA4,
    CaptureAudioPath2ConnectionTypeandportConnection,
    CaptureAudioPath3ConnectionTypeandportConnection,
    CaptureAudioPath4ConnectionTypeandportConnection,
    RenAudGeoAndGenLocationPath1=0xA8,
    RenAudGeoAndGenLocationPath2 ,
    RenAudGeoAndGenLocationPath3 ,
    RenAudGeoAndGenLocationPath4 ,
    CaptureAudGeoAndGenLocationPath1 = 0xAC,
    CaptureAudGeoAndGenLocationPath2,
    CaptureAudGeoAndGenLocationPath3,
    CaptureAudGeoAndGenLocationPath4,
    RenAudDefDeviceAndChnMapPath1=0xB0,
    RenAudDefDeviceAndChnMapPath2,
    RenAudDefDeviceAndChnMapPath3,
    RenAudDefDeviceAndChnMapPath4,
    CapAudDefDeviceAndChnMapPath1 = 0xB4,
    CapAudDefDeviceAndChnMapPath2,
    CapAudDefDeviceAndChnMapPath3,
    CapAudDefDeviceAndChnMapPath4,
    RenAudKsCategoryPath1=0xB8,
    RenAudKsCategoryPath2,
    RenAudKsCategoryPath3,
    RenAudKsCategoryPath4,
    CapAudKsCategoryPath1 = 0xBC,
    CapAudKsCategoryPath2,
    CapAudKsCategoryPath3,
    CapAudKsCategoryPath4,
    RenAudPth1Jak1ColorLsb = 0xC0,
    RenAudPth1Jak1ColorMsb = 0xC3,
    RenAudPth2Jak2ColorLsb = 0xC4,
    RenAudPth2Jak2ColorMsb = 0xC7,
    RenAudPth3Jak3ColorLsb = 0xC8,
    RenAudPth3Jak3ColorMsb = 0xCB,
    RenAudPth4Jak4ColorLsb = 0xCC,
    RenAudPth4Jak4ColorMsb = 0xCF,
    CaptAudPth1Jak1ColorLsb = 0xD0,
    CaptAudPth1Jak1ColorMsb = 0xD3,
    CaptAudPth2Jak2ColorLsb = 0xD4,
    CaptAudPth2Jak2ColorMsb = 0xD7,
    CaptAudPth3Jak3ColorLsb = 0xD8,
    CaptAudPth3Jak3ColorMsb = 0xDB,
    CaptAudPth4Jak4ColorLsb = 0xDC,
    CaptAudPth4Jak4ColorMsb = 0xDF,
} EVEREST_DSM_ITEMS_OFFSET;
