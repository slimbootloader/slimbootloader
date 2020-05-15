## @file
#
#  Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
#  SPDX-License-Identifier: BSD-2-Clause-Patent
#
##

  # !BSF PAGES:{HDA:SIL:"HD Audio Config"}
  # !BSF PAGE:{HDA}
  # !BSF SUBT:{CFGHDR_TMPL:HDA_CFG_DATA:0:0}

  # !HDR EMBED:{HDA_CFG_DATA:TAG_303:START}

    # !BSF NAME:{HD Audio Support}
    # !BSF TYPE:{Combo}
    # !BSF OPTION:{0x1:Enable, 0x0:Disable}
    # !BSF HELP:{Enable/disable HDA Audio Feature}
    gCfgData.HdaEnable                  |      * | 0x01 | 1

    # !BSF NAME:{HD Audio DSP Support}
    # !BSF TYPE:{Combo}
    # !BSF OPTION:{0x1:Enable, 0x0:Disable}
    # !BSF HELP:{Enable/disable HDA Audio DSP Feature}
    # !BSF CONDITION:{$(COND_HDA_EN)}
    gCfgData.DspEnable                  |      * | 0x01 | 1

    # !BSF NAME:{Universal Audio Architecture compliance for DSP enabled system}
    # !BSF TYPE:{Combo}
    # !BSF OPTION:{0:Not-UAA Compliant, 1:UAA Compliant}
    # !BSF HELP:{Not-UAA Compliant (Intel SST driver supported only) or UAA Compliant (HDA Inbox driver or SST driver supported)}
    # !BSF CONDITION:{$(COND_HDA_DSP_EN)}
    gCfgData.HdAudioDspUaaCompliance    |      * | 0x01 | 1

    # !BSF NAME:{Azalia wake-on-ring}
    # !BSF TYPE:{Combo}
    # !BSF OPTION:{0x1:Enable, 0x0:Disable}
    # !BSF HELP:{Enable/disable Azalia wake-on-ring}
    # !BSF CONDITION:{$(COND_HDA_DSP_EN)}
    gCfgData.Pme                        |      * | 0x01 | 1

    # !BSF NAME:{HD-Audio I/O Buffer Ownership}
    # !BSF TYPE:{Combo}
    # !BSF OPTION:{0:HD-Audio link owns all the I/O buffers, 1:HD-Audio link owns 4 I/O buffers and I2S port owns 4 I/O buffers, 3:I2S port owns all the I/O buffers}
    # !BSF HELP:{Set HD-Audio I/O Buffer Ownership}
    # !BSF CONDITION:{$(COND_HDA_DSP_EN)}
    gCfgData.HdAudioIoBufferOwnership   |      * | 0x01 | 3

    # !BSF NAME:{HD-Audio Virtual Channel Type}
    # !BSF TYPE:{Combo}
    # !BSF OPTION:{0:VC0, 1:VC1}
    # !BSF HELP:{HD-Audio Virtual Channel Type Selectiton}
    # !BSF CONDITION:{$(COND_HDA_DSP_EN)}
    gCfgData.HdAudioVcType              |      * | 0x01 | 0

    # !BSF NAME:{HD-Audio Link Frequency}
    # !BSF TYPE:{Combo}
    # !BSF OPTION:{0:6MHz, 1:12MHz, 2:24MHz, 3:48MHz, 4:96MHz}
    # !BSF HELP:{HD-Audio Virtual Channel Type Selectiton}
    # !BSF CONDITION:{$(COND_HDA_DSP_EN)}
    gCfgData.HdAudioLinkFrequency       |      * | 0x01 | 2

    # !BSF NAME:{HD-Audio iDisp-Link Frequency}
    # !BSF TYPE:{Combo}
    # !BSF OPTION:{0:6MHz, 1:12MHz, 2:24MHz, 3:48MHz, 4:96MHz}
    # !BSF HELP:{HD-Audio iDisp-Link Frequency Selectiton}
    # !BSF CONDITION:{$(COND_HDA_DSP_EN)}
    gCfgData.HdAudioIDispLinkFrequency  |      * | 0x01 | 4

    # !BSF NAME:{HD-Audio iDisp-Link T-Mode}
    # !BSF TYPE:{Combo}
    # !BSF OPTION:{0:2T, 1:1T}
    # !BSF HELP:{HD-Audio iDisp-Link T-Mode Selectiton}
    # !BSF CONDITION:{$(COND_HDA_DSP_EN)}
    gCfgData.HdAudioIDispLinkTmode      |      * | 0x01 | 0

    # !BSF NAME:{HD-Audio Disp DMIC}
    # !BSF TYPE:{Combo}
    # !BSF OPTION:{0: Disable, 1: 2ch array, 2: 4ch array}
    # !BSF HELP:{HD-Audio Disp DMIC Selectiton}
    # !BSF CONDITION:{$(COND_HDA_DSP_EN)}
    gCfgData.DspEndpointDmic            |      * | 0x01 | 0

    # !BSF NAME:{HD-Audio Bluetooth}
    # !BSF TYPE:{Combo}
    # !BSF OPTION:{0x1:Enable, 0x0:Disable}
    # !BSF HELP:{Enable/Disable HD-Audio bluetooth}
    # !BSF CONDITION:{$(COND_HDA_DSP_EN)}
    gCfgData.DspEndpointBluetooth       |      * | 0x01 | 1

    # !BSF NAME:{HD-Audio I2S SPK}
    # !BSF TYPE:{Combo}
    # !BSF OPTION:{0x1:Enable, 0x0:Disable}
    # !BSF HELP:{Enable/Disable HD-Audio I2S SPK}
    # !BSF CONDITION:{$(COND_HDA_DSP_EN)}
    gCfgData.DspEndpointI2sSpk          |      * | 0x01 | 1

    # !BSF NAME:{HD-Audio I2S HP}
    # !BSF TYPE:{Combo}
    # !BSF OPTION:{0x1:Enable, 0x0:Disable}
    # !BSF HELP:{Enable/Disable HD-Audio I2S HP}
    # !BSF CONDITION:{$(COND_HDA_DSP_EN)}
    gCfgData.DspEndpointI2sHp           |      * | 0x01 | 1

    # !BSF NAME:{Bitmask of DSP Feature}
    # !BSF TYPE:{EditNum, HEX, (0x00, 0xFFFFFFFF)}
    # !BSF HELP:{[BIT0] - WoV, [BIT1] - BT Sideband, [BIT2] - Codec VAD, [BIT5] - BT Intel HFP, [BIT6] - BT Intel A2DP, [BIT7] - DSP based speech pre-processing disabled, [BIT8] - 0: Intel WoV, 1: Windows Voice Activation}
    # !BSF CONDITION:{$(COND_HDA_DSP_EN)}
    gCfgData.DspFeatureMask             |      * | 0x04 | 0x2A

    # !BSF NAME:{Bitmask of supported DSP Post-Processing Modules}
    # !BSF TYPE:{EditNum, HEX, (0x00, 0xFFFFFFFF)}
    # !BSF HELP:{[BIT0] - WoV, [BIT1] - BT Sideband, [BIT2] - Codec VAD, [BIT5] - BT Intel HFP, [BIT6] - BT Intel A2DP, [BIT7] - DSP based speech pre-processing disabled, [BIT8] - 0: Intel WoV, 1: Windows Voice Activation}
    # !BSF CONDITION:{$(COND_HDA_DSP_EN)}
    gCfgData.DspPpModuleMask            |      * | 0x04 | 0

    # !BSF NAME:{BT Render Virtual Bus ID}
    # !BSF TYPE:{Combo}
    # !BSF OPTION:{0:I2S1,1:I2S2,2:I2S3,3:I2S4,4:I25,5:I2S6,0xF:N/A}
    # !BSF HELP:{BT Render Virtual Bus ID Selection}
    # !BSF CONDITION:{$(COND_HDA_DSP_EN)}
    gCfgData.VirtualIdBtRender          |      * | 0x01 | 2

    # !BSF NAME:{BT Capture Virtual Bus ID}
    # !BSF TYPE:{Combo}
    # !BSF OPTION:{0:I2S1,1:I2S2,2:I2S3,3:I2S4,4:I25,5:I2S6,0xF:N/A}
    # !BSF HELP:{BT Capture Virtual Bus ID Selection}
    # !BSF CONDITION:{$(COND_HDA_DSP_EN)}
    gCfgData.VirtualIdBtCapture         |      * | 0x01 | 2

    # !BSF NAME:{I2S Render Speaker Virtual Bus ID}
    # !BSF TYPE:{Combo}
    # !BSF OPTION:{0:I2S1,1:I2S2,2:I2S3,3:I2S4,4:I25,5:I2S6,0xF:N/A}
    # !BSF HELP:{I2S Render Speaker Bus ID Selection}
    # !BSF CONDITION:{$(COND_HDA_DSP_EN)}
    gCfgData.VirtualIdI2sRenderSpk      |      * | 0x01 | 5

    # !BSF NAME:{I2S Render Headphone Virtual Bus ID}
    # !BSF TYPE:{Combo}
    # !BSF OPTION:{0:I2S1,1:I2S2,2:I2S3,3:I2S4,4:I25,5:I2S6,0xF:N/A}
    # !BSF HELP:{I2S Render Headphone Virtual Bus ID Selection}
    # !BSF CONDITION:{$(COND_HDA_DSP_EN)}
    gCfgData.VirtualIdI2sRenderHp       |      * | 0x01 | 5

    # !BSF NAME:{I2S Capture Virtual Bus ID}
    # !BSF TYPE:{Combo}
    # !BSF OPTION:{0:I2S1,1:I2S2,2:I2S3,3:I2S4,4:I25,5:I2S6,0xF:N/A}
    # !BSF HELP:{I2S Capture Virtual Bus ID Selection}
    # !BSF CONDITION:{$(COND_HDA_DSP_EN)}
    gCfgData.VirtualIdI2sCapture        |      * | 0x01 | 5

    # !BSF NAME:{Hda Reserved} TYPE:{Reserved}
    gCfgData.Reserved                   |      * | 0x02 | 0

  # !HDR EMBED:{HDA_CFG_DATA:TAG_303:END}
