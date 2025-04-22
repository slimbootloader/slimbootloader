/** @file
  Chipset definition for ME Devices.

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

  Copyright (c) 2011 - 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/
#ifndef _ME_CHIPSET_H_
#define _ME_CHIPSET_H_

#define ME_SEGMENT            0
#define ME_BUS                0
#define ME_DEVICE_NUMBER      22
#define HECI_MIN_FUNC         0
#define HECI_MAX_FUNC         5

#define HECI_FUNCTION_NUMBER  0x00
#define HECI2_FUNCTION_NUMBER 0x01
#define IDER_FUNCTION_NUMBER  0x02
#define SOL_FUNCTION_NUMBER   0x03
#define HECI3_FUNCTION_NUMBER 0x04
#define HECI4_FUNCTION_NUMBER 0x05

#define IDER_BUS_NUMBER       ME_BUS
#define IDER_DEVICE_NUMBER    ME_DEVICE_NUMBER
#define SOL_BUS_NUMBER        ME_BUS
#define SOL_DEVICE_NUMBER     ME_DEVICE_NUMBER
#define HECI_DEV_FCN          ((ME_DEVICE_NUMBER) << 3 | (HECI_FUNCTION_NUMBER))
#define HECI2_DEV_FCN         ((ME_DEVICE_NUMBER) << 3 | (HECI2_FUNCTION_NUMBER))
#define HECI3_DEV_FCN         ((ME_DEVICE_NUMBER) << 3 | (HECI3_FUNCTION_NUMBER))


///
/// Convert to HECI# defined in BWG from Fun#
///
#define HECI_NAME_MAP(a) ((a < 2) ? (a + 1) : (a - 1))

///
/// Default Vendor ID and Device ID
///
#define V_INTEL_VENDOR_ID             0x8086

#define V_ME_HECI_VENDOR_ID           V_INTEL_VENDOR_ID
#define V_ME_SOL_VENDOR_ID            V_INTEL_VENDOR_ID

#define V_ME_PCH_LP_HECI_DEVICE_ID    0x9DE0
#define V_ME_PCH_LP_HECI2_DEVICE_ID   0x9DE1
#define V_ME_PCH_LP_HECI3_DEVICE_ID   0x9DE4
#define V_ME_PCH_LP_HECI4_DEVICE_ID   0x9DE5
#define V_ME_PCH_LP_SOL_DEVICE_ID     0x9DE3

#define V_ME_PCH_H_HECI_DEVICE_ID     0xA360
#define V_ME_PCH_H_HECI2_DEVICE_ID    0xA361
#define V_ME_PCH_H_HECI3_DEVICE_ID    0xA364
#define V_ME_PCH_H_HECI4_DEVICE_ID    0xA365
#define V_ME_PCH_H_SOL_DEVICE_ID      0xA363

#define R_ME_HFS                      0x40
#define R_ME_HFS_2                    0x48
#define R_ME_PMCSR                    0x54
#define V_ME_PMCSR                    0x03
#define R_ME_HFS_3                    0x60
#define R_ME_HFS_4                    0x64
#define R_ME_HFS_5                    0x68
#define R_ME_HFS_6                    0x6C
#define B_BOOT_GUARD_ENF_MASK         0x0200
#define B_TPM_DISCONNECT              0x1000
#define B_TPM1_2_DEACTIVATED          0x0100

#define R_ME_H_GS                     0x4C
#define R_ME_H_GS2                    0x70
#define B_ME_DID_RAPID_START_BIT      BIT23
#define B_ME_DID_TYPE_MASK            BIT28
#define R_ME_H_GS2                    0x70
#define B_ME_MBP_GIVE_UP              BIT0
#define R_ME_HIDM                     0xA0
#define V_ME_HIDM_MSI                 0
#define V_ME_HIDM_SCI                 1
#define V_ME_HIDM_SMI                 2
#define R_ME_HERS                     0xBC
#define B_ME_EXTEND_REG_ALGORITHM     (BIT3 | BIT2 | BIT1 | BIT0)   // Bits3:0 = 0x0 - SHA1; 0x2 - SHA256; 0x4 - SHA384
#define B_ME_EXTEND_COMPLETE          BIT29
#define B_ME_EXTEND_FEAT_PRESENT      BIT30
#define B_ME_EXTEND_REG_VALID         BIT31
#define V_ME_SHA_1                    0x00
#define V_ME_SHA_256                  0x02
#define V_ME_SHA_384                  0x04
#define R_ME_HER1                     0xC0
#define R_ME_HER2                     0xC4
#define R_ME_HER3                     0xC8
#define R_ME_HER4                     0xCC
#define R_ME_HER5                     0xD0
#define R_ME_HER6                     0xD4
#define R_ME_HER7                     0xD8
#define R_ME_HER8                     0xDC

#define V_ME_PCH_LP_USBR1_PORT_NUMBER 0xA
#define V_ME_PCH_LP_USBR2_PORT_NUMBER 0xB
#define V_ME_PCH_H_USBR1_PORT_NUMBER  0xE
#define V_ME_PCH_H_USBR2_PORT_NUMBER  0xF

///
/// SOL Private CR Space Definitions
///
#define V_ME_SOL_FID    0xB3
/*
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
/// HECI Device Id Definitions
///
 #define IS_PCH_H_HECI_DEVICE_ID(DeviceId) \
     (  \
       (DeviceId == V_ME_PCH_H_HECI_DEVICE_ID) \
     )

 #define IS_PCH_LP_HECI_DEVICE_ID(DeviceId) \
     (  \
       (DeviceId == V_ME_PCH_LP_HECI_DEVICE_ID) \
     )

 #define IS_HECI_DEVICE_ID(DeviceId) \
     ( \
       IS_PCH_H_HECI_DEVICE_ID(DeviceId) || \
       IS_PCH_LP_HECI_DEVICE_ID(DeviceId) \
     )

///
/// HECI2 Device Id Definitions
///
#define IS_PCH_H_HECI2_DEVICE_ID(DeviceId) \
     (  \
       (DeviceId == V_ME_PCH_H_HECI2_DEVICE_ID) \
     )

#define IS_PCH_LP_HECI2_DEVICE_ID(DeviceId) \
     (  \
       (DeviceId == V_ME_PCH_LP_HECI2_DEVICE_ID) \
     )

#define IS_HECI2_DEVICE_ID(DeviceId) \
     ( \
       IS_PCH_H_HECI2_DEVICE_ID(DeviceId) || \
       IS_PCH_LP_HECI2_DEVICE_ID(DeviceId) \
     )

///
/// HECI3 Device Id Definitions
///
#define IS_PCH_H_HECI3_DEVICE_ID(DeviceId) \
    (  \
      (DeviceId == V_ME_PCH_H_HECI3_DEVICE_ID) \
    )

#define IS_PCH_LP_HECI3_DEVICE_ID(DeviceId) \
    (  \
      (DeviceId == V_ME_PCH_LP_HECI3_DEVICE_ID) \
    )

#define IS_HECI3_DEVICE_ID(DeviceId) \
    ( \
      IS_PCH_H_HECI3_DEVICE_ID(DeviceId) || \
      IS_PCH_LP_HECI3_DEVICE_ID(DeviceId) \
    )

///
/// HECI4 Device Id Definitions
///
#define IS_PCH_H_HECI4_DEVICE_ID(DeviceId) \
    (  \
      (DeviceId == V_ME_PCH_H_HECI4_DEVICE_ID) \
    )

#define IS_PCH_LP_HECI4_DEVICE_ID(DeviceId) \
    (  \
      (DeviceId == V_ME_PCH_LP_HECI4_DEVICE_ID) \
    )

#define IS_HECI4_DEVICE_ID(DeviceId) \
    ( \
      IS_PCH_H_HECI4_DEVICE_ID(DeviceId) || \
      IS_PCH_LP_HECI4_DEVICE_ID(DeviceId) \
    )

///
/// SoL Device Id Definitions
///
#define IS_PCH_H_SOL_DEVICE_ID(DeviceId) \
    (  \
      (DeviceId == V_ME_PCH_H_SOL_DEVICE_ID) \
    )

#define IS_PCH_LP_SOL_DEVICE_ID(DeviceId) \
    (  \
      (DeviceId == V_ME_PCH_LP_SOL_DEVICE_ID) \
    )

#define IS_PCH_SOL_DEVICE_ID(DeviceId) \
    ( \
      IS_PCH_H_SOL_DEVICE_ID(DeviceId) || \
      IS_PCH_LP_SOL_DEVICE_ID(DeviceId) \
    )
*/

#endif
