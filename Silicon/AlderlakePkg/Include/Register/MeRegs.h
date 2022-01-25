/** @file
  Register definition for ME Devices.

  Conventions:

  - Prefixes:
    - Definitions beginning with "R_" are registers
    - Definitions beginning with "B_" are bits within registers
    - Definitions beginning with "V_" are meaningful values of bits within the registers
    - Definitions beginning with "S_" are register sizes
    - Definitions beginning with "N_" are the bit position
  - Registers / bits that are different between PCH generations are denoted by
    "_ME_[generation_name]_" in register/bit names.
  - Registers / bits that are specific to PCH-H denoted by "PCH_H_" in register/bit names.
    Registers / bits that are specific to PCH-LP denoted by "PCH_LP_" in register/bit names.
    e.g., "_ME_PCH_H_", "_ME_PCH_LP_"
    Registers / bits names without _PCH_H_ or _PCH_LP_ apply for both H and LP.
  - Registers / bits that are different between SKUs are denoted by "_[SKU_name]"
    at the end of the register/bit names
  - Registers / bits of new devices introduced in a PCH generation will be just named
    as "_ME_" without [generation_name] inserted.

  Copyright (c) 2021, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/
#ifndef _ME_REGS_H_
#define _ME_REGS_H_

#include <Library/MeChipsetLib.h>


///
/// Convert to HECI# defined in BWG from Fun#
///
#define HECI_NAME_MAP(a) ((a < 2) ? (a + 1) : (a - 1))

///
/// Default Vendor ID and Device ID
///
#define V_INTEL_VENDOR_ID             0x8086


///
/// Ver1 PCH-N ME Device IDs
///
#define V_ME_VER1_PCH_N_SOL_DEVICE_ID      0x4DE3

///
/// Ver2 PCH-H/LP ME Device IDs
///
#define V_ME_VER2_PCH_H_SOL_DEVICE_ID      0x43E3

#define V_ME_VER2_PCH_LP_SOL_DEVICE_ID     0xA0E3

///
/// Ver3 PCH ME Device IDs
///
#define V_ME_VER3_PCH_SOL_DEVICE_ID        0x1BE3

///
/// Ver4 PCH-S/P/M ME Device IDs
///
#define V_ME_VER4_PCH_S_SOL_DEVICE_ID      0x7AEB

#define V_ME_VER4_PCH_P_SOL_DEVICE_ID      0x51E3

#define V_ME_VER4_PCH_M_SOL_DEVICE_ID      0x54E3


#define R_ME_HERS                     0xBC
#define B_ME_EXTEND_REG_ALGORITHM     (BIT3 | BIT2 | BIT1 | BIT0)   // Bits3:0 = 0x0 - SHA1; 0x2 - SHA256; 0x4 - SHA384
#define B_ME_EXTEND_COMPLETE          BIT29
#define B_ME_EXTEND_FEAT_PRESENT      BIT30
#define B_ME_EXTEND_REG_VALID         BIT31
#define V_ME_SHA_256                  0x02
#define V_ME_SHA_384                  0x04
#define R_ME_HER1                     0xC0

///
/// ME-related Chipset Definition
///
#define HeciEnable()    MeDeviceControl (HECI1, Enabled);
#define Heci2Enable()   MeDeviceControl (HECI2, Enabled);
#define Heci3Enable()   MeDeviceControl (HECI3, Enabled);
#define Heci4Enable()   MeDeviceControl (HECI4, Enabled);
#define IderEnable()    MeDeviceControl (IDER, Enabled);
#define SolEnable()     MeDeviceControl (SOL, Enabled);

#define HeciDisable()   MeDeviceControl (HECI1, Disabled);
#define Heci2Disable()  MeDeviceControl (HECI2, Disabled);
#define Heci3Disable()  MeDeviceControl (HECI3, Disabled);
#define IderDisable()   MeDeviceControl (IDER, Disabled);
#define SolDisable()    MeDeviceControl (SOL, Disabled);

#define DisableAllMeDevices() \
  HeciDisable (); \
  Heci2Disable (); \
  Heci3Disable (); \
  IderDisable (); \
  SolDisable ();

///
/// SoL Device Id Definitions
///
#define IS_VER1_PCH_SOL_DEVICE_ID(DeviceId) \
  ( \
    (DeviceId == V_ME_VER1_PCH_N_SOL_DEVICE_ID) \
  )

#define IS_VER2_PCH_SOL_DEVICE_ID(DeviceId) \
  ( \
    (DeviceId == V_ME_VER2_PCH_H_SOL_DEVICE_ID) || \
    (DeviceId == V_ME_VER2_PCH_LP_SOL_DEVICE_ID) \
  )

#define IS_VER3_PCH_SOL_DEVICE_ID(DeviceId) \
  ( \
    (DeviceId == V_ME_VER3_PCH_SOL_DEVICE_ID) \
  )

#define IS_VER4_PCH_SOL_DEVICE_ID(DeviceId) \
  ( \
    (DeviceId == V_ME_VER4_PCH_S_SOL_DEVICE_ID) || \
    (DeviceId == V_ME_VER4_PCH_P_SOL_DEVICE_ID) || \
    (DeviceId == V_ME_VER4_PCH_M_SOL_DEVICE_ID) \
  )

#define IS_PCH_SOL_DEVICE_ID(DeviceId) \
  ( \
    IS_VER1_PCH_SOL_DEVICE_ID(DeviceId) || \
    IS_VER2_PCH_SOL_DEVICE_ID(DeviceId) || \
    IS_VER3_PCH_SOL_DEVICE_ID(DeviceId) || \
    IS_VER4_PCH_SOL_DEVICE_ID(DeviceId) \
  )

#endif
