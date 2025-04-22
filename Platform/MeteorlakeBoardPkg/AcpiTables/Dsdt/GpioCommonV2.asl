/**@file
  GPIO Common file with defines for ACPI GpioLibCommon and GpioXXX.asl (XXX - platform TGL, MTL, ...)

  Copyright (c) 2023, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#define GPIO_DESC_GET_COM_PID(GpioComIndex, GpioDataStructure) \
  ( \
    DeRefOf(Index(DeRefOf(Index(GpioDataStructure, GpioComIndex)), 0)) \
  )

#define GPIO_DESC_GET_GROUP_DESC(GpioComIndex, GpioDataStructure) \
  ( \
    DeRefOf(Index(DeRefOf(Index(GpioDataStructure, GpioComIndex)), 1)) \
  )

//
// Macro to retrieve data from GPIO information data structure (GpioLibObjects*.asl)
//
#define GPIO_DESC_GET_FIELD(GpioComIndex, GpioGroupIndex,GpioFieldIndex,GpioDataStructure) \
  ( \
    DeRefOf(Index(DeRefOf(Index(GPIO_DESC_GET_GROUP_DESC(GpioComIndex, GpioDataStructure), GpioGroupIndex)), GpioFieldIndex)) \
  )

#define IS_GPIO_GROUP_MAPPED_TO_GPE_DWX(GroupIndex, GroupDw, GpeDwXGroupIndex, GpeDwXGroupDw) \
  ( \
    LAnd( \
      LEqual(GpeDwXGroupIndex, GroupIndex), \
      LEqual(GpeDwXGroupDw, GroupDw) \
    ) \
  )

//
// Check if group belongs to 2-Tier GPE
//
#define IS_GPIO_GROUP_IN_2_TIER_GPE(GroupIndex, GroupDw) \
  ( \
    LNot( \
      Or( \
        Or( \
          IS_GPIO_GROUP_MAPPED_TO_GPE_DWX(GroupIndex, GroupDw, GEI0, GED0), \
          IS_GPIO_GROUP_MAPPED_TO_GPE_DWX(GroupIndex, GroupDw, GEI1, GED1)), \
        IS_GPIO_GROUP_MAPPED_TO_GPE_DWX(GroupIndex, GroupDw, GEI2, GED2))) \
  )

//
// Get PADCFG_DW0 GpioPad register address
//
#define GET_GPIO_PAD_PADCFG_REG_ADDRESS(GpioPad) \
  ( \
    Add( \
      GET_PADCFG_REG_ADDRESS(GpioPad), \
      Multiply( \
        GNMB(GpioPad), \
        S_GPIO_PCR_PADCFG) \
      ) \
  )

//
// Get GPIO register address
// If group has more DW registers for certain feature
// below macro will return address of the first one.
//
#define GET_PADCFG_REG_ADDRESS(GpioPad)      GADR(GpioPad,GPIO_PADCFG_REG_FIELD)
#define GET_HOSTSW_OWN_REG_ADDRESS(GpioPad)  GADR(GpioPad,GPIO_HOSTOWN_REG_FIELD)
#define GET_PAD_OWN_REG_ADDRESS(GpioPad)     GADR(GpioPad,GPIO_PADOWN_REG_FIELD)
#define GET_GPE_STS_REG_ADDRESS(GpioPad)     GADR(GpioPad,GPIO_GPE_STS_REG_FIELD)
#define GET_LOCK_CONFIG_REG_ADDRESS(GpioPad) GADR(GpioPad,GPIO_LOCK_CONFIG_REG_FIELD)
#define GET_LOCK_TX_REG_ADDRESS(GpioPad)     GADR(GpioPad,GPIO_LOCK_TX_REG_FIELD)
#define GET_GPIO_GROUP_INDEX(GpioPad)        GADR(GpioPad,GPIO_GROUP_INDEX)

//
// Arg1 possible values for internal GINF function
//
#define GPIO_PAD_NUM_FIELD           0
#define GPIO_PADCFG_REG_FIELD        1
#define GPIO_HOSTOWN_REG_FIELD       2
#define GPIO_PADOWN_REG_FIELD        3
#define GPIO_GPE_STS_REG_FIELD       4
#define GPIO_LOCK_CONFIG_REG_FIELD   5
#define GPIO_LOCK_TX_REG_FIELD       6
#define GPIO_DRIVER_PIN_BASE_NUMBER  7
#define GPIO_GROUP_INDEX             8