/** @file

  Copyright (c) 2014 - 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Library/HdaEndpoints.h>

GLOBAL_REMOVE_IF_UNREFERENCED
CONST WAVEFORMATEXTENSIBLE Ch2_48kHz16bitFormat =
{
  {
    WAVE_FORMAT_EXTENSIBLE,
    2,
    48000,
    192000,
    4,
    16,
    sizeof (WAVEFORMATEXTENSIBLE) - sizeof (WAVEFORMATEX)
  },
  { 16 },
  KSAUDIO_SPEAKER_STEREO,
  KSDATAFORMAT_SUBTYPE_PCM
};

GLOBAL_REMOVE_IF_UNREFERENCED
CONST WAVEFORMATEXTENSIBLE Ch2_48kHz24bitFormat =
{
  {
    WAVE_FORMAT_EXTENSIBLE,
    2,
    48000,
    384000,
    8,
    32,
    sizeof (WAVEFORMATEXTENSIBLE) - sizeof (WAVEFORMATEX)
  },
  { 24 },
  KSAUDIO_SPEAKER_STEREO,
  KSDATAFORMAT_SUBTYPE_PCM
};

GLOBAL_REMOVE_IF_UNREFERENCED
CONST WAVEFORMATEXTENSIBLE Ch2_48kHz32bitFormat =
{
  {
    WAVE_FORMAT_EXTENSIBLE,
    2,
    48000,
    384000,
    8,
    32,
    sizeof (WAVEFORMATEXTENSIBLE) - sizeof (WAVEFORMATEX)
  },
  { 32 },
  KSAUDIO_SPEAKER_STEREO,
  KSDATAFORMAT_SUBTYPE_PCM
};

GLOBAL_REMOVE_IF_UNREFERENCED
CONST WAVEFORMATEXTENSIBLE Ch4_48kHz16bitFormat =
{
  {
    WAVE_FORMAT_EXTENSIBLE,
    4,
    48000,
    384000,
    8,
    16,
    sizeof (WAVEFORMATEXTENSIBLE) - sizeof (WAVEFORMATEX)
  },
  { 16 },
  KSAUDIO_SPEAKER_QUAD,
  KSDATAFORMAT_SUBTYPE_PCM
};

GLOBAL_REMOVE_IF_UNREFERENCED
CONST WAVEFORMATEXTENSIBLE Ch4_48kHz32bitFormat =
{
  {
    WAVE_FORMAT_EXTENSIBLE,
    4,
    48000,
    384000,
    8,
    32,
    sizeof (WAVEFORMATEXTENSIBLE) - sizeof (WAVEFORMATEX)
  },
  { 32 },
  KSAUDIO_SPEAKER_QUAD,
  KSDATAFORMAT_SUBTYPE_PCM
};

GLOBAL_REMOVE_IF_UNREFERENCED
CONST WAVEFORMATEXTENSIBLE NarrowbandFormat =
{
  {
    WAVE_FORMAT_EXTENSIBLE,
    1,
    8000,
    16000,
    2,
    16,
    sizeof (WAVEFORMATEXTENSIBLE) - sizeof (WAVEFORMATEX)
  },
  { 16 },
  KSAUDIO_SPEAKER_MONO,
  KSDATAFORMAT_SUBTYPE_PCM
};

GLOBAL_REMOVE_IF_UNREFERENCED
CONST WAVEFORMATEXTENSIBLE WidebandFormat =
{
  {
    WAVE_FORMAT_EXTENSIBLE,
    1,
    16000,
    32000,
    2,
    16,
    sizeof (WAVEFORMATEXTENSIBLE) - sizeof (WAVEFORMATEX)
  },
  { 16 },
  KSAUDIO_SPEAKER_MONO,
  KSDATAFORMAT_SUBTYPE_PCM
};

GLOBAL_REMOVE_IF_UNREFERENCED
CONST WAVEFORMATEXTENSIBLE A2dpFormat =
{
  {
    WAVE_FORMAT_EXTENSIBLE,
    2,
    48000,
    384000,
    8,
    32,
    sizeof (WAVEFORMATEXTENSIBLE) - sizeof (WAVEFORMATEX)
  },
  { 24 },
  KSAUDIO_SPEAKER_STEREO,
  KSDATAFORMAT_SUBTYPE_PCM
};

GLOBAL_REMOVE_IF_UNREFERENCED
CONST UINT32 DmicStereo16BitFormatConfig[] =
{
  0x00000001,
  0xFFFF3210,
  0xFFFFFF10,
  0xFFFFFF32,
  0xFFFFFFFF,
  0x00000003,
  0x00000003,
  0x00300003,
  0x00300003
};

GLOBAL_REMOVE_IF_UNREFERENCED
CONST UINT32 DmicStereo32BitFormatConfig[] =
{
  0x00000001,
  0xFFFF3210,
  0xFFFFFF10,
  0xFFFFFF32,
  0xFFFFFFFF,
  0x00000003,
  0x00000003,
  0x00380003,
  0x00380003
};

GLOBAL_REMOVE_IF_UNREFERENCED
CONST UINT32 DmicQuad16BitFormatConfig[] =
{
  0x00000001,
  0xFFFF3210,
  0xFFFFFF10,
  0xFFFFFF32,
  0xFFFFFFFF,
  0x00000003,
  0x00000003,
  0x00320003,
  0x00320003
};

GLOBAL_REMOVE_IF_UNREFERENCED
CONST UINT32 DmicQuad32BitFormatConfig[] =
{
  0x00000001,
  0xFFFF3210,
  0xFFFFFF10,
  0xFFFFFF32,
  0xFFFFFFFF,
  0x00000003,
  0x00000003,
  0x003A0003,
  0x003A0003
};

GLOBAL_REMOVE_IF_UNREFERENCED CONST UINT32 DmicFormatConfigSize = sizeof (DmicStereo16BitFormatConfig);

GLOBAL_REMOVE_IF_UNREFERENCED
CONST UINT32 DmicCommonFormatConfig[] =
{
  0x00000003,
  //
  // PDM_SETTINGS
  //
  0x0001, 0x08000F00, 0x0, 0x0303, 0, 0, 0, 0,
  0x11, 0x401a0, 0, 0, 0, 0, 0, 0,
  0x11, 0xe03b1, 0, 0, 0, 0, 0, 0,
  //
  // FIR A coefficients
  //
  0x00006, 0xfffc2, 0xfff4a, 0xffe76, 0xffd3d, 0xffbb1, 0xffa06, 0xff891, 0xff7b5, 0xff7d0, 0xff91a, 0xffb89, 0xffec7, 0x00232, 0x00501, 0x00674, 0x0060f, 0x003c8, 0x0001f, 0xffc0e,
  0xff8c8, 0xff76b, 0xff89b, 0xffc48, 0x00192, 0x006fd, 0x00ad0, 0x00ba2, 0x008d3, 0x002db, 0xffb45, 0xff451, 0xff04f, 0xff0de, 0xff654, 0xfff79, 0x009c4, 0x01204, 0x01559, 0x0122f,
  0x008e0, 0xffbcc, 0xfeec8, 0xfe607, 0xfe4c4, 0xfec1d, 0xffa84, 0x00bfe, 0x01b34, 0x0230f, 0x02062, 0x0131d, 0xffe9a, 0xfe8d0, 0xfd8a1, 0xfd3b3, 0xfdc86, 0xff162, 0x00c8d, 0x025d6,
  0x03504, 0x03479, 0x02332, 0x00574, 0xfe3d0, 0xfc8b1, 0xfbd30, 0xfc60d, 0xfe1e3, 0x0092c, 0x0303b, 0x04ab2, 0x04f5b, 0x03b53, 0x01350, 0xfe2a8, 0xfb81f, 0xfa174, 0xfa6ff, 0xfc8be,
  0xffdd5, 0x036d4, 0x0622f, 0x07195, 0x05e9d, 0x02d32, 0xfeae9, 0xfab52, 0xf8238, 0xf7d85, 0xfa0ad, 0xfe310, 0x03207, 0x0761d, 0x099fa, 0x09101, 0x05b91, 0x00783, 0xfaca0, 0xf65ea,
  0xf499f, 0xf6261, 0xfab80, 0x011b3, 0x0782e, 0x0c0b9, 0x0d485, 0x0ab24, 0x04d99, 0xfd470, 0xf6145, 0xf157b, 0xf08ad, 0xf419d, 0xfb3aa, 0x041a3, 0x0c550, 0x119b3, 0x1253d, 0x0e125,
  0x05bdc, 0xfb5dc, 0xf1998, 0xeb0a6, 0xe98ed, 0xedca7, 0xf6f50, 0x0303a, 0x0f22e, 0x18623, 0x1c671, 0x19fbb, 0x1158d, 0x041b8, 0xf4ea6, 0xe6e38, 0xdcf55, 0xd9485, 0xdcd8e, 0xe7549,
  0xf73b4, 0x0a398, 0x1da21, 0x2eec7, 0x3c1e2, 0x440c0, 0x466ea, 0x43ca0, 0x3d36f, 0x341bc, 0x29e6b, 0x1fd4d, 0x16cef, 0x0f5cd, 0x09ac4, 0x05a55, 0x03040, 0x0171e, 0x00999, 0x0033a,
  0x000be,
  //
  // FIR B coefficients
  //
  0xffff5, 0x007c7, 0x00726, 0x00a22, 0x00db5, 0x011d6, 0x01676, 0x01b80, 0x020d4, 0x02648, 0x02baa, 0x030c4, 0x03557, 0x03924, 0x03be9, 0x03d69, 0x03d6c, 0x03bc5, 0x03854, 0x03309,
  0x02be4, 0x022fe, 0x01882, 0x00cb5, 0xfffec, 0xff294, 0xfe526, 0xfd82b, 0xfcc2f, 0xfc1c0, 0xfb967, 0xfb3a1, 0xfb0d7, 0xfb158, 0xfb556, 0xfbcdd, 0xfc7d4, 0xfd5f7, 0xfe6dc, 0xff9f0,
  0x00e7f, 0x023b7, 0x038af, 0x04c77, 0x05e19, 0x06ca8, 0x07750, 0x07d58, 0x07e34, 0x0798a, 0x06f3a, 0x05f66, 0x04a6d, 0x030f5, 0x013db, 0xff438, 0xfd34d, 0xfb280, 0xf9346, 0xf771b,
  0xf5f64, 0xf4d73, 0xf425a, 0xf3efb, 0xf43de, 0xf513f, 0xf66f2, 0xf8476, 0xfa8dd, 0xfd2ef, 0x00113, 0x03183, 0x06234, 0x09112, 0x0bbf5, 0x0e0d9, 0x0fdd1, 0x11149, 0x119ee, 0x116eb,
  0x107cf, 0x0ecbd, 0x0c64d, 0x095a8, 0x05c67, 0x01c9d, 0xfd8b2, 0xf935b, 0xf4f70, 0xf0fe0, 0xed781, 0xea8fe, 0xe86ae, 0xe7280, 0xe6de0, 0xe799f, 0xe95f0, 0xec252, 0xefda0, 0xf4606,
  0xf9922, 0xff404, 0x0535e, 0x0b38a, 0x110c8, 0x16749, 0x1b36d, 0x1f1d1, 0x21f8e, 0x23a37, 0x24019, 0x2302a, 0x20a3e, 0x1cee6, 0x17f93, 0x11e6e, 0x0ae61, 0x032e8, 0xfb00a, 0xf2a20,
  0xea5c4, 0xe2795, 0xdb421, 0xd4faa, 0xcfe13, 0xcc2ad, 0xca02a, 0xc9877, 0xcacbc, 0xcdd42, 0xd2982, 0xd901f, 0xe0efb, 0xea344, 0xf4996, 0xffe0c, 0x0bc6f, 0x1804f, 0x24528, 0x3068b,
  0x3c039, 0x46e45, 0x50d2f, 0x599f7, 0x61231, 0x67412, 0x6be71, 0x6f0ca, 0x70b3a, 0x70e72, 0x6fbac, 0x6d497, 0x69b45, 0x65213, 0x5fb93, 0x59a75, 0x53172, 0x4c336, 0x4524e, 0x3e116,
  0x371b3, 0x305ff, 0x29f8c, 0x23f9b, 0x1e71b, 0x196b1, 0x14eb4, 0x10f3e, 0x0d828, 0x0a91e, 0x081a1, 0x06117, 0x046cd, 0x03207, 0x02204, 0x01605, 0x00d56, 0x009c9,
  //
  // PDM_SETTINGS
  //
  0x0001, 0x08000F00, 0x0, 0x0303, 0, 0, 0, 0,
  0x11, 0x401a0, 0, 0, 0, 0, 0, 0,
  0x11, 0xe03b1, 0, 0, 0, 0, 0, 0,
  //
  // FIR A coefficients
  //
  0x00006, 0xfffc2, 0xfff4a, 0xffe76, 0xffd3d, 0xffbb1, 0xffa06, 0xff891, 0xff7b5, 0xff7d0, 0xff91a, 0xffb89, 0xffec7, 0x00232, 0x00501, 0x00674, 0x0060f, 0x003c8, 0x0001f, 0xffc0e,
  0xff8c8, 0xff76b, 0xff89b, 0xffc48, 0x00192, 0x006fd, 0x00ad0, 0x00ba2, 0x008d3, 0x002db, 0xffb45, 0xff451, 0xff04f, 0xff0de, 0xff654, 0xfff79, 0x009c4, 0x01204, 0x01559, 0x0122f,
  0x008e0, 0xffbcc, 0xfeec8, 0xfe607, 0xfe4c4, 0xfec1d, 0xffa84, 0x00bfe, 0x01b34, 0x0230f, 0x02062, 0x0131d, 0xffe9a, 0xfe8d0, 0xfd8a1, 0xfd3b3, 0xfdc86, 0xff162, 0x00c8d, 0x025d6,
  0x03504, 0x03479, 0x02332, 0x00574, 0xfe3d0, 0xfc8b1, 0xfbd30, 0xfc60d, 0xfe1e3, 0x0092c, 0x0303b, 0x04ab2, 0x04f5b, 0x03b53, 0x01350, 0xfe2a8, 0xfb81f, 0xfa174, 0xfa6ff, 0xfc8be,
  0xffdd5, 0x036d4, 0x0622f, 0x07195, 0x05e9d, 0x02d32, 0xfeae9, 0xfab52, 0xf8238, 0xf7d85, 0xfa0ad, 0xfe310, 0x03207, 0x0761d, 0x099fa, 0x09101, 0x05b91, 0x00783, 0xfaca0, 0xf65ea,
  0xf499f, 0xf6261, 0xfab80, 0x011b3, 0x0782e, 0x0c0b9, 0x0d485, 0x0ab24, 0x04d99, 0xfd470, 0xf6145, 0xf157b, 0xf08ad, 0xf419d, 0xfb3aa, 0x041a3, 0x0c550, 0x119b3, 0x1253d, 0x0e125,
  0x05bdc, 0xfb5dc, 0xf1998, 0xeb0a6, 0xe98ed, 0xedca7, 0xf6f50, 0x0303a, 0x0f22e, 0x18623, 0x1c671, 0x19fbb, 0x1158d, 0x041b8, 0xf4ea6, 0xe6e38, 0xdcf55, 0xd9485, 0xdcd8e, 0xe7549,
  0xf73b4, 0x0a398, 0x1da21, 0x2eec7, 0x3c1e2, 0x440c0, 0x466ea, 0x43ca0, 0x3d36f, 0x341bc, 0x29e6b, 0x1fd4d, 0x16cef, 0x0f5cd, 0x09ac4, 0x05a55, 0x03040, 0x0171e, 0x00999, 0x0033a,
  0x000be,
  //
  // FIR B coefficients
  //
  0xffff5, 0x007c7, 0x00726, 0x00a22, 0x00db5, 0x011d6, 0x01676, 0x01b80, 0x020d4, 0x02648, 0x02baa, 0x030c4, 0x03557, 0x03924, 0x03be9, 0x03d69, 0x03d6c, 0x03bc5, 0x03854, 0x03309,
  0x02be4, 0x022fe, 0x01882, 0x00cb5, 0xfffec, 0xff294, 0xfe526, 0xfd82b, 0xfcc2f, 0xfc1c0, 0xfb967, 0xfb3a1, 0xfb0d7, 0xfb158, 0xfb556, 0xfbcdd, 0xfc7d4, 0xfd5f7, 0xfe6dc, 0xff9f0,
  0x00e7f, 0x023b7, 0x038af, 0x04c77, 0x05e19, 0x06ca8, 0x07750, 0x07d58, 0x07e34, 0x0798a, 0x06f3a, 0x05f66, 0x04a6d, 0x030f5, 0x013db, 0xff438, 0xfd34d, 0xfb280, 0xf9346, 0xf771b,
  0xf5f64, 0xf4d73, 0xf425a, 0xf3efb, 0xf43de, 0xf513f, 0xf66f2, 0xf8476, 0xfa8dd, 0xfd2ef, 0x00113, 0x03183, 0x06234, 0x09112, 0x0bbf5, 0x0e0d9, 0x0fdd1, 0x11149, 0x119ee, 0x116eb,
  0x107cf, 0x0ecbd, 0x0c64d, 0x095a8, 0x05c67, 0x01c9d, 0xfd8b2, 0xf935b, 0xf4f70, 0xf0fe0, 0xed781, 0xea8fe, 0xe86ae, 0xe7280, 0xe6de0, 0xe799f, 0xe95f0, 0xec252, 0xefda0, 0xf4606,
  0xf9922, 0xff404, 0x0535e, 0x0b38a, 0x110c8, 0x16749, 0x1b36d, 0x1f1d1, 0x21f8e, 0x23a37, 0x24019, 0x2302a, 0x20a3e, 0x1cee6, 0x17f93, 0x11e6e, 0x0ae61, 0x032e8, 0xfb00a, 0xf2a20,
  0xea5c4, 0xe2795, 0xdb421, 0xd4faa, 0xcfe13, 0xcc2ad, 0xca02a, 0xc9877, 0xcacbc, 0xcdd42, 0xd2982, 0xd901f, 0xe0efb, 0xea344, 0xf4996, 0xffe0c, 0x0bc6f, 0x1804f, 0x24528, 0x3068b,
  0x3c039, 0x46e45, 0x50d2f, 0x599f7, 0x61231, 0x67412, 0x6be71, 0x6f0ca, 0x70b3a, 0x70e72, 0x6fbac, 0x6d497, 0x69b45, 0x65213, 0x5fb93, 0x59a75, 0x53172, 0x4c336, 0x4524e, 0x3e116,
  0x371b3, 0x305ff, 0x29f8c, 0x23f9b, 0x1e71b, 0x196b1, 0x14eb4, 0x10f3e, 0x0d828, 0x0a91e, 0x081a1, 0x06117, 0x046cd, 0x03207, 0x02204, 0x01605, 0x00d56, 0x009c9
};
GLOBAL_REMOVE_IF_UNREFERENCED CONST UINT32 DmicCommonFormatConfigSize = sizeof (DmicCommonFormatConfig);

GLOBAL_REMOVE_IF_UNREFERENCED
CONST UINT32 I2sFormatConfigRender[] =  { 0x0, 0xffffff10, 0xffffff32, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x83d00337, 0xc0700000, 0x0, 0x2010004, 0xf, 0xf, 0x4002, 0x4, 0x7070f00, 0x20, 0x220002, 0x2 }; ///<config_Master TDM 24bit 48Khz 4 slot;
GLOBAL_REMOVE_IF_UNREFERENCED CONST UINT32 I2sFormatConfigRenderSize = sizeof (I2sFormatConfigRender);

GLOBAL_REMOVE_IF_UNREFERENCED
CONST UINT32 I2sFormatConfigCapture[] = { 0x0, 0xffffff10, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x83d00337, 0xc0700000, 0x0, 0x2010004, 0xf, 0xf, 0x4002, 0x4, 0x7070f00, 0x20, 0x220002, 0x2 }; ///<config_Master TDM 24bit 48Khz 4 slot;
GLOBAL_REMOVE_IF_UNREFERENCED CONST UINT32 I2sFormatConfigCaptureSize = sizeof (I2sFormatConfigCapture);

GLOBAL_REMOVE_IF_UNREFERENCED
CONST UINT32 BtFormatConfig[] = {0xfffffff0, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x80c095bf, 0xc0401dc0, 0x00000000, 0x02010005, 0x00000001, 0x00000001,  0x00004002, 0x00000000, 0x07020000, 0x00000030, 0x00000001, 0x00000000};

GLOBAL_REMOVE_IF_UNREFERENCED CONST UINT32 BtFormatConfigSize = sizeof (BtFormatConfig);

GLOBAL_REMOVE_IF_UNREFERENCED
ENDPOINT_DESCRIPTOR  HdaEndpointDmicX2 = {
  0,                 ///< EndpointDescriptorLength
  HdaNhltLinkDmic,   ///< LinkType
  0,                 ///< InstanceId
  0x8086,           ///< HwVendorId
  0xae20,           ///< HwDeviceId
  1,                 ///< HwRevisionId
  1,                 ///< HwSubsystemId
  HdaNhltDeviceDmic, ///< DeviceType
  1,                 ///< Direction
  0,                 ///< VirtualBusId
  { 0 },             ///< EndpointConfig
  { 0 }              ///< FormatsConfig
};

GLOBAL_REMOVE_IF_UNREFERENCED
ENDPOINT_DESCRIPTOR  HdaEndpointDmicX4 = {
  0,                 ///< EndpointDescriptorLength
  HdaNhltLinkDmic,   ///< LinkType
  0,                 ///< InstanceId
  0x8086,           ///< HwVendorId
  0xae20,           ///< HwDeviceId
  1,                 ///< HwRevisionId
  1,                 ///< HwSubsystemId
  HdaNhltDeviceDmic, ///< DeviceType
  1,                 ///< Direction
  0,                 ///< VirtualBusId
  { 0 },             ///< EndpointConfig
  { 0 }              ///< FormatsConfig
};

GLOBAL_REMOVE_IF_UNREFERENCED
ENDPOINT_DESCRIPTOR  HdaEndpointBtRender = {
  0,                 ///< EndpointDescriptorLength
  HdaNhltLinkSsp,    ///< LinkType
  0,                 ///< InstanceId
  0x8086,           ///< HwVendorId
  0xae30,           ///< HwDeviceId
  1,                 ///< HwRevisionId
  1,                 ///< HwSubsystemId
  HdaNhltDeviceBt,   ///< DeviceType
  0,                 ///< Direction
  1,                 ///< VirtualBusId
  { 0 },             ///< EndpointConfig
  { 0 }              ///< FormatsConfig
};

GLOBAL_REMOVE_IF_UNREFERENCED
ENDPOINT_DESCRIPTOR  HdaEndpointBtCapture = {
  0,                 ///< EndpointDescriptorLength
  HdaNhltLinkSsp,    ///< LinkType
  0,                 ///< InstanceId
  0x8086,           ///< HwVendorId
  0xae30,           ///< HwDeviceId
  1,                 ///< HwRevisionId
  1,                 ///< HwSubsystemId
  HdaNhltDeviceBt,   ///< DeviceType
  1,                 ///< Direction
  1,                 ///< VirtualBusId
  { 0 },             ///< EndpointConfig
  { 0 }              ///< FormatsConfig
};

ENDPOINT_DESCRIPTOR  HdaEndpointI2sRenderSKP = {
   0,                 ///< EndpointDescriptorLength
   HdaNhltLinkSsp,    ///< LinkType
   1,                 ///< InstanceId
   0x8086,            ///< HwVendorId
   0xae34,            ///< HwDeviceId
   1,                 ///< HwRevisionId
   1,                 ///< HwSubsystemId
   HdaNhltDeviceI2s,  ///< DeviceType
   0,                 ///< Direction
   0,                 ///< VirtualBusId
   { 0 },       ///< EndpointConfig
   { 0 }        ///< FormatsConfig
};

ENDPOINT_DESCRIPTOR  HdaEndpointI2sRenderHP = {
   0,                 ///< EndpointDescriptorLength
   HdaNhltLinkSsp,    ///< LinkType
   1,                 ///< InstanceId
   0x8086,            ///< HwVendorId
   0xae34,            ///< HwDeviceId
   1,                 ///< HwRevisionId
   1,                 ///< HwSubsystemId
   HdaNhltDeviceI2s,  ///< DeviceType
   0,                 ///< Direction
   0,                 ///< VirtualBusId
   { 0 },       ///< EndpointConfig
   { 0 }        ///< FormatsConfig
};

GLOBAL_REMOVE_IF_UNREFERENCED
ENDPOINT_DESCRIPTOR  HdaEndpointI2sCapture = {
  0,                 ///< EndpointDescriptorLength
  HdaNhltLinkSsp,    ///< LinkType
  1,                 ///< InstanceId
  0x8086,           ///< HwVendorId
  0xae34,           ///< HwDeviceId
  1,                 ///< HwRevisionId
  1,                 ///< HwSubsystemId
  HdaNhltDeviceI2s,  ///< DeviceType
  1,                 ///< Direction
  0,                 ///< VirtualBusId
  { 0 },             ///< EndpointConfig
  { 0 }              ///< FormatsConfig
};

GLOBAL_REMOVE_IF_UNREFERENCED
ENDPOINT_DESCRIPTOR  HdaEndpointModem1Render = {
  0,                 ///< EndpointDescriptorLength
  HdaNhltLinkSsp,    ///< LinkType
  1,                 ///< InstanceId
  0x8086,           ///< HwVendorId
  0x7260,           ///< HwDeviceId
  1,                 ///< HwRevisionId
  1,                 ///< HwSubsystemId
  HdaNhltDeviceI2s,  ///< DeviceType
  0,                 ///< Direction
  0,                 ///< VirtualBusId
  { 0 },             ///< EndpointConfig
  { 0 }              ///< FormatsConfig
};

GLOBAL_REMOVE_IF_UNREFERENCED
ENDPOINT_DESCRIPTOR  HdaEndpointModem1Capture = {
  0,                 ///< EndpointDescriptorLength
  HdaNhltLinkSsp,    ///< LinkType
  1,                 ///< InstanceId
  0x8086,           ///< HwVendorId
  0x7360,           ///< HwDeviceId
  1,                 ///< HwRevisionId
  1,                 ///< HwSubsystemId
  HdaNhltDeviceI2s,  ///< DeviceType
  1,                 ///< Direction
  0,                 ///< VirtualBusId
  { 0 },             ///< EndpointConfig
  { 0 }              ///< FormatsConfig
};

GLOBAL_REMOVE_IF_UNREFERENCED
ENDPOINT_DESCRIPTOR  HdaEndpointModem2Render = {
  0,                 ///< EndpointDescriptorLength
  HdaNhltLinkSsp,    ///< LinkType
  1,                 ///< InstanceId
  0x8086,           ///< HwVendorId
  0xae34,           ///< HwDeviceId
  1,                 ///< HwRevisionId
  1,                 ///< HwSubsystemId
  HdaNhltDeviceI2s,  ///< DeviceType
  0,                 ///< Direction
  0,                 ///< VirtualBusId
  { 0 },             ///< EndpointConfig
  { 0 }              ///< FormatsConfig
};

GLOBAL_REMOVE_IF_UNREFERENCED
ENDPOINT_DESCRIPTOR  HdaEndpointModem2Capture = {
  0,                 ///< EndpointDescriptorLength
  HdaNhltLinkSsp,    ///< LinkType
  1,                 ///< InstanceId
  0x8086,           ///< HwVendorId
  0xae34,           ///< HwDeviceId
  1,                 ///< HwRevisionId
  1,                 ///< HwSubsystemId
  HdaNhltDeviceI2s,  ///< DeviceType
  1,                 ///< Direction
  0,                 ///< VirtualBusId
  { 0 },             ///< EndpointConfig
  { 0 }              ///< FormatsConfig
};

GLOBAL_REMOVE_IF_UNREFERENCED CONST UINT8  DmicX2Config[] = { 0x00, 0x01, 0x0A };
GLOBAL_REMOVE_IF_UNREFERENCED CONST UINT32 DmicX2ConfigSize = sizeof (DmicX2Config);

GLOBAL_REMOVE_IF_UNREFERENCED CONST UINT8  DmicX4Config[] = { 0x00, 0x01, 0x0D };
GLOBAL_REMOVE_IF_UNREFERENCED CONST UINT32 DmicX4ConfigSize = sizeof (DmicX4Config);

GLOBAL_REMOVE_IF_UNREFERENCED CONST UINT8  BtConfig[] = {0};
GLOBAL_REMOVE_IF_UNREFERENCED CONST UINT32 BtConfigSize = 0;

GLOBAL_REMOVE_IF_UNREFERENCED CONST UINT8  I2sConfig[] = {0};
GLOBAL_REMOVE_IF_UNREFERENCED CONST UINT32 I2sConfigSize = 0;

GLOBAL_REMOVE_IF_UNREFERENCED CONST UINT8  I2sConfigSKP[] = {1};
GLOBAL_REMOVE_IF_UNREFERENCED CONST UINT32 I2sConfigSKPSize = 1;

GLOBAL_REMOVE_IF_UNREFERENCED CONST UINT8  Modem1Config[] = {0};
GLOBAL_REMOVE_IF_UNREFERENCED CONST UINT32 Modem1ConfigSize = 0;

GLOBAL_REMOVE_IF_UNREFERENCED CONST UINT8  Modem2Config[] = {0};
GLOBAL_REMOVE_IF_UNREFERENCED CONST UINT32 Modem2ConfigSize = 0;

GLOBAL_REMOVE_IF_UNREFERENCED CONST UINT32 NhltConfiguration[] = { 0xEFBEADDE };
GLOBAL_REMOVE_IF_UNREFERENCED CONST UINT32 NhltConfigurationSize = sizeof (NhltConfiguration);
