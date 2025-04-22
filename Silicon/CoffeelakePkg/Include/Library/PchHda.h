/** @file
  Header file for HD Audio configuration.

  Copyright (c) 2018 - 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _PCH_HDA_H_
#define _PCH_HDA_H_

enum PCH_HDAUDIO_VC_TYPE {
  PchHdaVc0 = 0,
  PchHdaVc1 = 1
};

enum PCH_HDAUDIO_SNDW_BUFFER_RCOMP_TOPOLOGY {
  PchHdaSndwNonActTopology = 0,
  PchHdaSndwActTopology    = 1
};

enum PCH_HDAUDIO_DMIC_TYPE {
  PchHdaDmicDisabled = 0,
  PchHdaDmic2chArray = 1,
  PchHdaDmic4chArray = 2,
  PchHdaDmic1chArray = 3
};

typedef enum {
  PchHdaLinkFreq6MHz  = 0,
  PchHdaLinkFreq12MHz = 1,
  PchHdaLinkFreq24MHz = 2,
  PchHdaLinkFreq48MHz = 3,
  PchHdaLinkFreq96MHz = 4,
  PchHdaLinkFreqInvalid
} PCH_HDAUDIO_LINK_FREQUENCY;

typedef enum  {
  PchHdaIDispMode2T  = 0,
  PchHdaIDispMode1T  = 1,
  PchHdaIDispMode4T  = 2,
  PchHdaIDispMode8T  = 3,
  PchHdaIDispMode16T = 4,
  PchHdaIDispTModeInvalid
} PCH_HDAUDIO_IDISP_TMODE;

typedef enum  {
  PchHdaLink      = 0,
  PchHdaIDispLink = 1,
  PchHdaDmic0     = 2,
  PchHdaDmic1     = 3,
  PchHdaSsp0      = 4,
  PchHdaSsp1      = 5,
  PchHdaSsp2      = 6,
  PchHdaSsp3      = 7,
  PchHdaSsp4      = 8,
  PchHdaSsp5      = 9,
  PchHdaSndw1     = 10,
  PchHdaSndw2     = 11,
  PchHdaSndw3     = 12,
  PchHdaSndw4     = 13,
  PchHdaLinkUnsupported
} PCH_HDAUDIO_LINK_TYPE;

#endif // _PCH_HDA_H_
