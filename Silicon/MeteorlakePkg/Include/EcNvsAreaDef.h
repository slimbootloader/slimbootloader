/**@file


  Copyright (c) 2021 - 2023, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _EC_NVS_AREA_DEF_H_
#define _EC_NVS_AREA_DEF_H_

#pragma pack (push,1)
typedef struct {
  UINT8    IuerButtonEnable;                        ///< Offset 0       IUER Button Enable
  UINT8    IuerConvertibleEnable;                   ///< Offset 1       IUER Convertible Enable
  UINT8    IuerDockEnable;                          ///< Offset 2       IUER Dock Enable
  UINT8    CSNotifyEC;                              ///< Offset 3       EC Notification of Low Power S0 Idle State
  UINT8    EcLowPowerMode;                          ///< Offset 4       EC Low Power Mode: 1 - Enabled, 0 - Disabled
  UINT8    PcdBatterySupport;                       ///< Offset 5       Battery Support - Bit0: Real Battery is supported on this platform. Bit1: Virtual Battery is supported on this platform.
  UINT8    PcdEcHotKeyF3Support;                    ///< Offset 6       Ec Hotkey F3 Support
  UINT8    PcdEcHotKeyF4Support;                    ///< Offset 7       Ec Hotkey F4 Support
  UINT8    PcdEcHotKeyF5Support;                    ///< Offset 8       Ec Hotkey F5 Support
  UINT8    PcdEcHotKeyF6Support;                    ///< Offset 9       Ec Hotkey F6 Support
  UINT8    PcdEcHotKeyF7Support;                    ///< Offset 10      Ec Hotkey F7 Support
  UINT8    PcdEcHotKeyF8Support;                    ///< Offset 11      Ec Hotkey F8 Support
  UINT8    PcdVirtualButtonVolumeUpSupport;         ///< Offset 12      Virtual Button Volume Up Support
  UINT8    PcdVirtualButtonVolumeDownSupport;       ///< Offset 13      Virtual Button Volume Down Support
  UINT8    PcdVirtualButtonHomeButtonSupport;       ///< Offset 14      Virtual Button Home Button Support
  UINT8    PcdVirtualButtonRotationLockSupport;     ///< Offset 15      Virtual Button Rotation Lock Support
  UINT8    PcdSlateModeSwitchSupport;               ///< Offset 16      Slate Mode Switch Support
  UINT8    PcdAcDcAutoSwitchSupport;                ///< Offset 17      Ac Dc Auto Switch Support
  UINT32   PcdPmPowerButtonGpioPin;                 ///< Offset 18      Pm Power Button Gpio Pin
  UINT32   EcdtGpeNumber;                           ///< Offset 22      Ecdt GPE bit value
  UINT32   LidSwitchWakeGpio;                       ///< Offset 26      Lid Switch Wake Gpio
  UINT8    PseudoG3StateCounter;                    ///< Offset 30      Pseudo G3 counter Enable/Disable
  UINT8    EnableBTP;                               ///< Offset 31      Enable battery _BTP support
} EC_NVS_AREA;

#pragma pack(pop)
#endif
