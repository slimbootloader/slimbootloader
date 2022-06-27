/** @file
  Header file for GpioConfig structure used by GPIO library.

  Copyright (c) 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _GPIO_LIB_CONFIG_H_
#define _GPIO_LIB_CONFIG_H_

#pragma pack(push, 1)

///
/// For any GpioPad usage in code use GPIO_PAD type
///
typedef UINT32 GPIO_PAD;

///
/// GpioPad with additional native function information.
/// This type is used to represent signal muxing alternatives. Platform will provide such value to
/// identify muxing selection for given signal on a specific SOC.
/// Please refer to the board layout
///
typedef UINT32 GPIO_NATIVE_PAD;

///
/// For any GpioGroup usage in code use GPIO_GROUP type
///
typedef UINT32 GPIO_GROUP;

/**
  GPIO configuration structure used for pin programming.
  Structure contains fields that can be used to configure pad.
**/
typedef struct {
  /**
  Pad Mode
  Pad can be set as GPIO or one of its native functions.
  When in native mode setting Direction (except Inversion), OutputState,
  InterruptConfig, Host Software Pad Ownership and OutputStateLock are unnecessary.
  Refer to definition of GPIO_PAD_MODE.
  Refer to EDS for each native mode according to the pad.
  **/
  UINT32 PadMode            : 5;
  /**
  Host Software Pad Ownership
  Set pad to ACPI mode or GPIO Driver Mode.
  Refer to definition of GPIO_HOSTSW_OWN.
  **/
  UINT32 HostSoftPadOwn     : 2;
  /**
  GPIO Direction
  Can choose between In, In with inversion, Out, both In and Out, both In with inversion and out or disabling both.
  Refer to definition of GPIO_DIRECTION for supported settings.
  **/
  UINT32 Direction           : 6;
  /**
  Output State
  Set Pad output value.
  Refer to definition of GPIO_OUTPUT_STATE for supported settings.
  This setting takes place when output is enabled.
  **/
  UINT32 OutputState         : 2;
  /**
  GPIO Interrupt Configuration
  Set Pad to cause one of interrupts (IOxAPIC/SCI/SMI/NMI).
  This setting is applicable only if GPIO is in GpioMode with input enabled.
  Refer to definition of GPIO_INT_CONFIG for supported settings.
  **/
  UINT32 InterruptConfig     : 9;
  /**
  GPIO Power Configuration.
  This setting controls Pad Reset Configuration.
  Refer to definition of GPIO_RESET_CONFIG for supported settings.
  **/
  UINT32 PowerConfig        : 8;
  /**
  GPIO Electrical Configuration
  This setting controls pads termination.
  Refer to definition of GPIO_ELECTRICAL_CONFIG for supported settings.
  **/
  UINT32 ElectricalConfig   : 9;
  /**
  GPIO Lock Configuration
  This setting controls pads lock.
  Refer to definition of GPIO_LOCK_CONFIG for supported settings.
  **/
  UINT32 LockConfig         : 4;
  /**
  Additional GPIO configuration
  Refer to definition of GPIO_OTHER_CONFIG for supported settings.
  **/
  UINT32 OtherSettings     :  9;
  UINT32 RsvdBits          : 10;    ///< Reserved bits for future extension
} GPIO_CONFIG;

typedef enum {
  GpioHardwareDefault        = 0x0    ///< Leave setting unmodified
} GPIO_HARDWARE_DEFAULT;

/**
  GPIO Pad Mode
  Refer to GPIO documentation on native functions available for certain pad.
  If GPIO is set to one of NativeX modes then following settings are not applicable
  and can be skipped:
  - Interrupt related settings
  - Host Software Ownership
  - Output/Input enabling/disabling
  - Output lock
**/
typedef enum {
  GpioPadModeGpio     = 0x1,
  GpioPadModeNative1  = 0x3,
  GpioPadModeNative2  = 0x5,
  GpioPadModeNative3  = 0x7,
  GpioPadModeNative4  = 0x9,
  GpioPadModeNative5  = 0xB,
  GpioPadModeNative6  = 0xD,
  GpioPadModeNative7  = 0xF,
  GpioPadModeNative8  = 0x11
} GPIO_PAD_MODE;

/**
  Host Software Pad Ownership modes
  This setting affects GPIO interrupt status registers. Depending on chosen ownership
  some GPIO Interrupt status register get updated and other masked.
  Please refer to EDS for HOSTSW_OWN register description.
**/
typedef enum {
  GpioHostOwnDefault = 0x0,   ///< Leave ownership value unmodified
  /**
  Set HOST ownership to ACPI.
  Use this setting if pad is not going to be used by GPIO OS driver.
  If GPIO is configured to generate SCI/SMI/NMI then this setting must be
  used for interrupts to work
  **/
  GpioHostOwnAcpi    = 0x1,
  /**
  Set HOST ownership to GPIO Driver mode.
  Use this setting only if GPIO pad should be controlled by GPIO OS Driver.
  GPIO OS Driver will be able to control the pad if appropriate entry in
  ACPI exists (refer to ACPI specification for GpioIo and GpioInt descriptors)
  **/
  GpioHostOwnGpio    = 0x3
} GPIO_HOSTSW_OWN;

///
/// GPIO Direction
///
typedef enum {
  GpioDirDefault         = 0x0,                ///< Leave pad direction setting unmodified
  GpioDirInOut           = (0x1 | (0x1 << 3)), ///< Set pad for both output and input
  GpioDirInInvOut        = (0x1 | (0x3 << 3)), ///< Set pad for both output and input with inversion
  GpioDirIn              = (0x3 | (0x1 << 3)), ///< Set pad for input only
  GpioDirInInv           = (0x3 | (0x3 << 3)), ///< Set pad for input with inversion
  GpioDirOut             = 0x5,                ///< Set pad for output only
  GpioDirNone            = 0x7                 ///< Disable both output and input
} GPIO_DIRECTION;

/**
  GPIO Output State
  This field is relevant only if output is enabled
**/
typedef enum {
  GpioOutDefault         = 0x0,  ///< Leave output value unmodified
  GpioOutLow             = 0x1,  ///< Set output to low
  GpioOutHigh            = 0x3   ///< Set output to high
} GPIO_OUTPUT_STATE;

/**
  GPIO interrupt configuration
  This setting is applicable only if pad is in GPIO mode and has input enabled.
  GPIO_INT_CONFIG allows to choose which interrupt is generated (IOxAPIC/SCI/SMI/NMI)
  and how it is triggered (edge or level). Refer to PADCFG_DW0 register description in
  EDS for details on this settings.
  Field from GpioIntNmi to GpioIntApic can be OR'ed with GpioIntLevel to GpioIntBothEdge
  to describe an interrupt e.g. GpioIntApic | GpioIntLevel
  If GPIO is set to cause an SCI then also GPI_GPE_EN is enabled for this pad.
  If GPIO is set to cause an NMI then also GPI_NMI_EN is enabled for this pad.
  Not all GPIO are capable of generating an SMI or NMI interrupt.
  When routing GPIO to cause an IOxAPIC interrupt care must be taken, as this
  interrupt cannot be shared and its IRQn number is not configurable.
  Refer to EDS for GPIO pads IRQ numbers (PADCFG_DW1.IntSel)
  If GPIO is under GPIO OS driver control and appropriate ACPI GpioInt descriptor
  exist then use only trigger type setting (from GpioIntLevel to GpioIntBothEdge).
  This type of GPIO Driver interrupt doesn't have any additional routing setting
  required to be set by BIOS. Interrupt is handled by GPIO OS Driver.
**/

typedef enum {
  GpioIntDefault           = 0x0,  ///< Leave value of interrupt routing unmodified
  GpioIntDis               = 0x1,  ///< Disable IOxAPIC/SCI/SMI/NMI interrupt generation
  GpioIntNmi               = 0x3,  ///< Enable NMI interrupt only
  GpioIntSmi               = 0x5,  ///< Enable SMI interrupt only
  GpioIntSci               = 0x9,  ///< Enable SCI interrupt only
  GpioIntApic              = 0x11, ///< Enable IOxAPIC interrupt only
  GpioIntLevel       = (0x1 << 5), ///< Set interrupt as level triggered
  GpioIntEdge        = (0x3 << 5), ///< Set interrupt as edge triggered (type of edge depends on input inversion)
  GpioIntLvlEdgDis   = (0x5 << 5), ///< Disable interrupt trigger
  GpioIntBothEdge    = (0x7 << 5)  ///< Set interrupt as both edge triggered
} GPIO_INT_CONFIG;

#define B_GPIO_INT_CONFIG_INT_SOURCE_MASK  0x1F ///< Mask for GPIO_INT_CONFIG for interrupt source
#define B_GPIO_INT_CONFIG_INT_TYPE_MASK    0xE0 ///< Mask for GPIO_INT_CONFIG for interrupt type

/**
  GPIO Power Configuration
  GPIO_RESET_CONFIG allows to set GPIO Reset type (PADCFG_DW0.PadRstCfg) which will
  be used to reset certain GPIO settings.
  Refer to EDS for settings that are controllable by PadRstCfg.
**/
typedef enum {
  GpioResetDefault   = 0x00,        ///< Leave value of pad reset unmodified
  ///
  /// Deprecated settings. Maintained only for compatibility.
  ///
  GpioResetPwrGood   = 0x09,        ///< GPP: RSMRST; GPD: DSW_PWROK; (PadRstCfg = 00b = "Powergood")
  GpioResetDeep      = 0x0B,        ///< Deep GPIO Reset (PadRstCfg = 01b = "Deep GPIO Reset")
  GpioResetNormal    = 0x0D,        ///< GPIO Reset (PadRstCfg = 10b = "GPIO Reset" )
  GpioResetResume    = 0x0F,        ///< GPP: Reserved; GPD: RSMRST; (PadRstCfg = 11b = "Resume Reset" )

  ///
  /// New GPIO reset configuration options
  ///
  /**
  Resume Reset (RSMRST)
    GPP: PadRstCfg = 00b = "Powergood"
    GPD: PadRstCfg = 11b = "Resume Reset"
  Pad setting will reset on:
  - DeepSx transition
  - G3
  Pad settings will not reset on:
  - S3/S4/S5 transition
  - Warm/Cold/Global reset
  **/
  GpioResumeReset      = 0x01,
  /**
  Host Deep Reset
    PadRstCfg = 01b = "Deep GPIO Reset"
  Pad settings will reset on:
  - Warm/Cold/Global reset
  - DeepSx transition
  - G3
  Pad settings will not reset on:
  - S3/S4/S5 transition
  **/
  GpioHostDeepReset    = 0x03,
  /**
  Platform Reset (PLTRST)
    PadRstCfg = 10b = "GPIO Reset"
  Pad settings will reset on:
  - S3/S4/S5 transition
  - Warm/Cold/Global reset
  - DeepSx transition
  - G3
  **/
  GpioPlatformReset    = 0x05,
  /**
  Deep Sleep Well Reset (DSW_PWROK)
    GPP: not applicable
    GPD: PadRstCfg = 00b = "Powergood"
  Pad settings will reset on:
  - G3
  Pad settings will not reset on:
  - S3/S4/S5 transition
  - Warm/Cold/Global reset
  - DeepSx transition
  **/
  GpioDswReset         = 0x07
} GPIO_RESET_CONFIG;

/**
  GPIO Electrical Configuration
  Configuration options for GPIO termination setting
**/
typedef enum {
  GpioTermDefault          = 0x0,  ///< Leave termination setting unmodified
  GpioTermNone             = 0x1,  ///< none
  GpioTermWpd5K            = 0x5,  ///< 5kOhm weak pull-down
  GpioTermWpd20K           = 0x9,  ///< 20kOhm weak pull-down
  GpioTermWpu1K            = 0x13, ///< 1kOhm weak pull-up
  GpioTermWpu2K            = 0x17, ///< 2kOhm weak pull-up
  GpioTermWpu5K            = 0x15, ///< 5kOhm weak pull-up
  GpioTermWpu20K           = 0x19, ///< 20kOhm weak pull-up
  GpioTermWpu1K2K          = 0x1B, ///< 1kOhm & 2kOhm weak pull-up
  /**
  Native function controls pads termination
  This setting is applicable only to some native modes.
  Please check EDS to determine which native functionality
  can control pads termination
  **/
  GpioTermNative     = 0x1F,
  GpioNoTolerance1v8 = (0x1 << 5),   ///< Disable 1.8V pad tolerance
  GpioTolerance1v8   = (0x3 << 5)    ///< Enable 1.8V pad tolerance
} GPIO_ELECTRICAL_CONFIG;

#define B_GPIO_ELECTRICAL_CONFIG_TERMINATION_MASK    0x1F   ///< Mask for GPIO_ELECTRICAL_CONFIG for termination value

/**
  GPIO LockConfiguration
  Set GPIO configuration lock and output state lock.
  GpioPadConfigUnlock/Lock and GpioOutputStateUnlock can be OR'ed.
  By default GPIO pads will be locked unless GPIO lib is explicitly
  informed that certain pad is to be left unlocked.
  Lock settings reset is in Powergood domain. Care must be taken when using this setting
  as fields it locks may be reset by a different signal and can be controlled
  by what is in GPIO_RESET_CONFIG (PADCFG_DW0.PadRstCfg). GPIO library provides
  functions which allow to unlock a GPIO pad. If possible each GPIO lib function will try to unlock
  an already locked pad upon request for reconfiguration
**/
typedef enum {
  /**
  Perform default action
   - if pad is an GPO, lock configuration but leave output unlocked
   - if pad is an GPI, lock everything
   - if pad is in native, lock everything
**/
  GpioLockDefault       = 0x0,
  GpioPadConfigUnlock   = 0x3,  ///< Leave Pad configuration unlocked
  GpioPadConfigLock     = 0x1,  ///< Lock Pad configuration
  GpioOutputStateUnlock = 0xC,  ///< Leave Pad output control unlocked
  GpioPadUnlock         = 0xF,  ///< Leave both Pad configuration and output control unlocked
  GpioPadLock           = 0x5   ///< Lock both Pad configuration and output control
} GPIO_LOCK_CONFIG;

#define B_GPIO_LOCK_CONFIG_PAD_CONF_LOCK_MASK  0x3  ///< Mask for GPIO_LOCK_CONFIG for Pad Configuration Lock
#define B_GPIO_LOCK_CONFIG_OUTPUT_LOCK_MASK    0xC  ///< Mask for GPIO_LOCK_CONFIG for Pad Output Lock

/**
  Other GPIO Configuration
  GPIO_OTHER_CONFIG is used for less often settings and for future extensions
  Supported settings:
   - RX raw override to '1' - allows to override input value to '1'
      This setting is applicable only if in input mode (both in GPIO and native usage).
      The override takes place at the internal pad state directly from buffer and before the RXINV.
**/
typedef enum {
  GpioRxRaw1Default           = 0x0,  ///< Use default input override value
  GpioRxRaw1Dis               = 0x1,  ///< Don't override input
  GpioRxRaw1En                = 0x3   ///< Override input to '1'
} GPIO_OTHER_CONFIG;

#define B_GPIO_OTHER_CONFIG_RXRAW_MASK           0x3   ///< Mask for GPIO_OTHER_CONFIG for RxRaw1 setting

typedef UINT8    GPIO_PCH_SBI_PID;

/**
  PCH SBI opcode definitions
**/
typedef enum {
  GpioMemoryRead             = 0x0,
  GpioMemoryWrite            = 0x1,
  GpioPciConfigRead          = 0x4,
  GpioPciConfigWrite         = 0x5,
  GpioPrivateControlRead     = 0x6,
  GpioPrivateControlWrite    = 0x7,
  GpioLibGpioLockUnlock      = 0x13
} GPIO_PCH_SBI_OPCODE;

typedef struct {
  GPIO_PAD           GpioPad;
  GPIO_CONFIG        GpioConfig;
} GPIO_INIT_CONFIG;

//
// Structure for native pin data
//
typedef struct {
  GPIO_PAD       Pad;
  GPIO_PAD_MODE  Mode;
} GPIO_PAD_NATIVE_FUNCTION;

//
// Below defines are based on GPIO_CONFIG structure fields
//
#define B_GPIO_PAD_MODE_MASK                            0xF
#define N_GPIO_PAD_MODE_BIT_POS                         0
#define B_GPIO_HOSTSW_OWN_MASK                          0x3
#define N_GPIO_HOSTSW_OWN_BIT_POS                       0
#define B_GPIO_DIRECTION_MASK                           0x1F
#define B_GPIO_DIRECTION_DIR_MASK                       0x7
#define N_GPIO_DIRECTION_DIR_BIT_POS                    0
#define B_GPIO_DIRECTION_INV_MASK                       0x18
#define N_GPIO_DIRECTION_INV_BIT_POS                    3
#define B_GPIO_OUTPUT_MASK                              0x3
#define N_GPIO_OUTPUT_BIT_POS                           0
#define N_GPIO_INT_CONFIG_INT_SOURCE_BIT_POS            0
#define N_GPIO_INT_CONFIG_INT_TYPE_BIT_POS              5
#define B_GPIO_RESET_CONFIG_RESET_MASK                  0x3F
#define N_GPIO_RESET_CONFIG_OLD_RESET_TYPE              BIT1
#define B_GPIO_RESET_CONFIG_OLD_RESET_MASK              0xF
#define N_GPIO_RESET_CONFIG_RESET_BIT_POS               0
#define B_GPIO_RESET_CONFIG_GPD_RESET_MASK              (BIT5 | BIT4)
#define B_GPIO_RESET_CONFIG_GPP_RESET_MASK              (BIT3 | BIT2)
#define N_GPIO_ELECTRICAL_CONFIG_TERMINATION_BIT_POS    0
#define N_GPIO_OTHER_CONFIG_RXRAW_BIT_POS               0

//#define GPIO_PCR_PADCFG               0x10
#define GPIO_PCR_PADCFG                 GpioGetPcrPadCfgOffset()

#define B_GPIO_PCR_RST_CONF             (BIT31 | BIT30)
#define N_GPIO_PCR_RST_CONF             30
#define V_GPIO_PCR_RST_CONF_POW_GOOD    0x00
#define V_GPIO_PCR_RST_CONF_DEEP_RST    0x01
#define V_GPIO_PCR_RST_CONF_GPIO_RST    0x02
#define V_GPIO_PCR_RST_CONF_RESUME_RST  0x03
#define B_GPIO_PCR_RX_RAW1              BIT28
#define N_GPIO_PCR_RX_RAW1              28
#define B_GPIO_PCR_RX_LVL_EDG           (BIT26 | BIT25)
#define N_GPIO_PCR_RX_LVL_EDG           25
#define B_GPIO_PCR_RXINV                BIT23
#define N_GPIO_PCR_RXINV                23
#define B_GPIO_PCR_RX_APIC_ROUTE        BIT20
#define B_GPIO_PCR_RX_SCI_ROUTE         BIT19
#define B_GPIO_PCR_RX_SMI_ROUTE         BIT18
#define B_GPIO_PCR_RX_NMI_ROUTE         BIT17
#define N_GPIO_PCR_RX_NMI_ROUTE         17
#define B_GPIO_PCR_PINSTRAPVAL          BIT11
#define N_GPIO_PCR_PINSTRAPVAL          11
#define B_GPIO_PCR_TERM                 (BIT13 | BIT12 | BIT11 | BIT10)
#define N_GPIO_PCR_TERM                 10
#define B_GPIO_PCR_PAD_MODE             (BIT12 | BIT11 | BIT10)
#define N_GPIO_PCR_PAD_MODE             10
#define B_GPIO_PCR_RXDIS                BIT9
#define B_GPIO_PCR_TXDIS                BIT8
#define N_GPIO_PCR_TXDIS                8
#define B_GPIO_PCR_INTSEL               0x7F
#define N_GPIO_PCR_INTSEL               0
#define B_GPIO_PCR_RX_STATE             BIT1
#define N_GPIO_PCR_RX_STATE             1
#define B_GPIO_PCR_TX_STATE             BIT0
#define N_GPIO_PCR_TX_STATE             0

///
/// Groups mapped to 2-tier General Purpose Event will all be under
/// one master GPE_111 (0x6F)
///
#define PCH_GPIO_2_TIER_MASTER_GPE_NUMBER  0x6F

//
// Structure for storing information about registers offset, community,
// maximal pad number for available groups
//
typedef struct {
  GPIO_PCH_SBI_PID Community;
  UINT16       PadOwnOffset;
  UINT16       HostOwnOffset;
  UINT16       GpiIsOffset;
  UINT16       GpiIeOffset;
  UINT16       GpiGpeStsOffset;
  UINT16       GpiGpeEnOffset;
  UINT16       SmiStsOffset;
  UINT16       SmiEnOffset;
  UINT16       NmiStsOffset;
  UINT16       NmiEnOffset;
  UINT16       PadCfgLockOffset;
  UINT16       PadCfgLockTxOffset;
  UINT16       PadCfgOffset;
  UINT16       PadPerGroup;
} GPIO_GROUP_INFO;

//
// If in GPIO_GROUP_INFO structure certain register doesn't exist
// it will have value equal to NO_REGISTER_FOR_PROPERTY
//
#define NO_REGISTER_FOR_PROPERTY 0xFFFF

#define GPIO_GET_PAD_POSITION(PadNumber)       ((PadNumber) % 32)
#define GPIO_GET_DW_NUM(PadNumber)             ((PadNumber) / 32u)

//
// Structure which stores information needed to map GPIO Group
// to 1-Tier GPE. Configuration is needed both in PMC and GPIO IP.
// Because GPE_DWx can handle only 32 pins only single double word can
// be mapped at a time. Each DW for a group has different configuration in PMC and GPIO
//
typedef struct {
  GPIO_GROUP  Group;
  UINT8       GroupDw;
  UINT8       PmcGpeDwxVal;
  UINT8       GpioGpeDwxVal;
} GPIO_GROUP_TO_GPE_MAPPING;

///
/// Possible values of Pad Ownership
/// If Pad is not under Host ownership then GPIO registers
/// are not accessible by host (e.g. BIOS) and reading them
/// will return 0xFFs.
///
typedef enum {
  GpioPadOwnHost = 0x0,
  GpioPadOwnCsme = 0x1,
  GpioPadOwnIsh  = 0x2,
} GPIO_PAD_OWN;

//
// Access necessary info from GPIO CFG DATA
//
typedef struct {
  UINT8       BaseTableId;
  UINT16      ItemSize;
  UINT16      ItemCount;
  UINT8      *BaseTableBitMask;
  UINT8      *TableData;
} GPIO_CFG_HDR_INFO;

#pragma pack(pop)

#endif //_GPIO_LIB_CONFIG_H_

