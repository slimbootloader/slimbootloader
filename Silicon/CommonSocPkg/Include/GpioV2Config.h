/** @file
  Header file for Gpio Config structure used by GPIO library.

  Copyright (c) 2024, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#ifndef _GPIOV2_CONFIG_H_
#define _GPIOV2_CONFIG_H_

typedef UINT32  GPIOV2_PAD;
typedef UINT32  GPIOV2_NATIVE_PAD;
typedef UINT32  GPIOV2_PAD_GROUP;

#define GPIOV2_PAD_NONE       0
#define GPIOV2_PAD_GROUP_NONE 0

// If Bit 0 is 0 then Library ignores remaining value
// If Bit 0 is 1 then Library programes remaining value.
#define GPIO_ASSIGN_VALUE(Val) (Val << 1) | 0x01

#define GPIO_STATE(Val)  (UINT32) ((Val >> 1) & 0x01)

#define GPIOV2_PAD_MASK_CHIPSETID              (0xFF)
#define GPIOV2_PAD_POS_CHIPSETID               (16)
#define GPIOV2_PAD_MASK_COMMUNITY_INDEX        (0xF)
#define GPIOV2_PAD_POS_COMMUNITY_INDEX         (12)
#define GPIOV2_PAD_MASK_GROUP_INDEX            (0xF)
#define GPIOV2_PAD_POS_GROUP_INDEX             (8)
#define GPIOV2_PAD_MASK_PAD_INDEX              (0xFF)
#define GPIOV2_PAD_POS_PAD_INDEX               (0)

#define GPIOV2_PAD_ID(Functionality, ChipsetId, NativeFunction, CommunityIndex, GroupIndex, PadIndex) \
        ( ((ChipsetId & GPIOV2_PAD_MASK_CHIPSETID) << GPIOV2_PAD_POS_CHIPSETID) |\
          ((CommunityIndex & GPIOV2_PAD_MASK_COMMUNITY_INDEX) << GPIOV2_PAD_POS_COMMUNITY_INDEX) |\
          ((GroupIndex & GPIOV2_PAD_MASK_GROUP_INDEX) << GPIOV2_PAD_POS_GROUP_INDEX) |\
          ((PadIndex & GPIOV2_PAD_MASK_PAD_INDEX) << GPIOV2_PAD_POS_PAD_INDEX) \
        )

#define GPIOV2_PAD_GET_CHIPSETID(PadDefinition) \
        ( (PadDefinition >> GPIOV2_PAD_POS_CHIPSETID) & GPIOV2_PAD_MASK_CHIPSETID )

#define GPIOV2_PAD_GET_COMMUNITY_INDEX(PadDefinition) \
        ( (PadDefinition >> GPIOV2_PAD_POS_COMMUNITY_INDEX) & GPIOV2_PAD_MASK_COMMUNITY_INDEX )

#define GPIOV2_PAD_GET_GROUP_INDEX(PadDefinition) \
        ( (PadDefinition >> GPIOV2_PAD_POS_GROUP_INDEX) & GPIOV2_PAD_MASK_GROUP_INDEX )

#define GPIOV2_PAD_GET_PAD_INDEX(PadDefinition) \
        ( (PadDefinition >> GPIOV2_PAD_POS_PAD_INDEX) & GPIOV2_PAD_MASK_PAD_INDEX )


typedef enum {
  GpioV2PadModeHardwareDefault  = 0x0,
  GpioV2PadModeGpio             = GPIO_ASSIGN_VALUE(0x0),
  GpioV2PadModeNative1          = GPIO_ASSIGN_VALUE(0x1),
  GpioV2PadModeNative2          = GPIO_ASSIGN_VALUE(0x2),
  GpioV2PadModeNative3          = GPIO_ASSIGN_VALUE(0x3),
  GpioV2PadModeNative4          = GPIO_ASSIGN_VALUE(0x4),
  GpioV2PadModeNative5          = GPIO_ASSIGN_VALUE(0x5),
  GpioV2PadModeNative6          = GPIO_ASSIGN_VALUE(0x6),
  GpioV2PadModeNative7          = GPIO_ASSIGN_VALUE(0x7),
  GpioV2PadModeNative8          = GPIO_ASSIGN_VALUE(0x8),
  GpioV2PadModeNative9          = GPIO_ASSIGN_VALUE(0x9),
  GpioV2PadModeNative10         = GPIO_ASSIGN_VALUE(0xA),
  GpioV2PadModeNative11         = GPIO_ASSIGN_VALUE(0xB),
  GpioV2PadModeNative12         = GPIO_ASSIGN_VALUE(0xC),
  GpioV2PadModeNative13         = GPIO_ASSIGN_VALUE(0xD),
  GpioV2PadModeNative14         = GPIO_ASSIGN_VALUE(0xE),
  GpioV2PadModeNative15         = GPIO_ASSIGN_VALUE(0xF)
} GPIOV2_PAD_MODE;

#define GPIOV2_PAD_MODE_MASK       (0xF)
#define GPIOV2_PAD_MODE_DW0_POS    (10)

typedef enum {
  GpioV2InputInversionHardwareDefault = 0x0,
  GpioV2InputInversionDisable         = GPIO_ASSIGN_VALUE(0x0),
  GpioV2InputInversionEnable          = GPIO_ASSIGN_VALUE(0x1)
} GPIOV2_PAD_INPUT_INVERSION;

#define GPIOV2_PAD_INPUT_INVERSION_MASK        (0x1)
#define GPIOV2_PAD_INPUT_INVERSION_DW0_POS     (23)

typedef enum {
  GpioV2LockHardwareDefault   = 0x0,
  GpioV2Unlock                = GPIO_ASSIGN_VALUE(0x0),  ///< Leave Pad configuration unlocked
  GpioV2Lock                  = GPIO_ASSIGN_VALUE(0x1)   ///< Lock Pad configuration
} GPIOV2_PAD_LOCK;

#define GPIOV2_PAD_LOCK_MASK        (0x1)

typedef enum {
  GpioV2StateDefault     = 0x0,
  GpioV2StateLow         = GPIO_ASSIGN_VALUE(0x0),
  GpioV2StateHigh        = GPIO_ASSIGN_VALUE(0x1)
} GPIOV2_PAD_STATE;

#define GPIOV2_PAD_OUTPUT_STATE_MASK        (0x1)
#define GPIOV2_PAD_OUTPUT_STATE_DW0_POS     (0)

#define GPIOV2_PAD_INPUT_STATE_MASK         (0x1)
#define GPIOV2_PAD_INPUT_STATE_DW0_POS      (1)

#define GPIOV2_PAD_TX_DISABLE_MASK          (0x1)
#define GPIOV2_PAD_TX_DISABLE_DW0_POS       (8)

#define GPIOV2_PAD_RX_DISABLE_MASK          (0x1)
#define GPIOV2_PAD_RX_DISABLE_DW0_POS       (9)

typedef enum {
  GpioV2ResetDefault     = 0x00,        ///< Leave value of pad reset unmodified
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
  GpioV2ResetResume      = GPIO_ASSIGN_VALUE(0x0),
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
  GpioV2ResetHostDeep    = GPIO_ASSIGN_VALUE(0x1),
  /**
  Platform Reset (PLTRST)
    PadRstCfg = 10b = "GPIO Reset"
  Pad settings will reset on:
  - S3/S4/S5 transition
  - Warm/Cold/Global reset
  - DeepSx transition
  - G3
  **/
  GpioV2ResetHost        = GPIO_ASSIGN_VALUE(0x2),
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
  GpioV2DswReset          = GPIO_ASSIGN_VALUE(0x3),
  /**
    Global reset. Server chipsets only.
      PadRstCfg = 11b = "Global reset"
    Pad settings will reset on:
    - Global reset
    - G3
    Pad settings will not reset on:
    - S3/S4/S5 transition
    - Warm/Cold reset
    - DeepSx transition
  **/
  GpioV2ResetGlobal      = GPIO_ASSIGN_VALUE(0x4)
} GPIOV2_RESET_CONFIG;


#define GPIOV2_PAD_RESET_CONFIG_MASK        (0x3)
#define GPIOV2_PAD_RESET_CONFIG_DW0_POS     (30)

typedef enum {
  GpioV2TermDefault          = 0x0,  ///< Leave termination setting unmodified
  GpioV2TermNone             = 0x1,  ///< none
  GpioV2TermWpd5K            = 0x5,  ///< 5kOhm weak pull-down
  GpioV2TermWpd20K           = 0x9,  ///< 20kOhm weak pull-down
  GpioV2TermWpu1K            = 0x13, ///< 1kOhm weak pull-up
  GpioV2TermWpu2K            = 0x17, ///< 2kOhm weak pull-up
  GpioV2TermWpu5K            = 0x15, ///< 5kOhm weak pull-up
  GpioV2TermWpu20K           = 0x19, ///< 20kOhm weak pull-up
  GpioV2TermWpu1K2K          = 0x1B, ///< 1kOhm & 2kOhm weak pull-up
  /**
  Native function controls pads termination
  This setting is applicable only to some native modes.
  Please check EDS to determine which native functionality
  can control pads termination
  **/
  GpioV2TermNative           = 0x1F
} GPIOV2_TERMINATION_CONFIG;

#define GPIOV2_PAD_TERMINATION_CONFIG_MASK        (0xF)
#define GPIOV2_PAD_TERMINATION_CONFIG_DW1_POS     (10)

typedef enum {
  GpioV2IosStateDefault         = 0x0,
  GpioV2IosStateLatchLastValue  = GPIO_ASSIGN_VALUE(0x0),  ///< Latch last value driven on TX, TX Enable and RX Enable
  GpioV2IosStateTx0Rx0RxDis     = GPIO_ASSIGN_VALUE(0x1),  ///< TX: 0, RX: 0 (internally), RX disabled
  GpioV2IosStateTx0Rx1RxDis     = GPIO_ASSIGN_VALUE(0x2),  ///< TX: 0, RX: 1 (internally), RX disabled
  GpioV2IosStateTx1Rx0RxDis     = GPIO_ASSIGN_VALUE(0x3),  ///< TX: 1, RX: 0 (internally), RX disabled
  GpioV2IosStateTx1Rx1RxDis     = GPIO_ASSIGN_VALUE(0x4),  ///< TX: 1, RX: 1 (internally), RX disabled
  GpioV2IosStateTx0RxEn         = GPIO_ASSIGN_VALUE(0x5),  ///< TX: 0, RX enabled
  GpioV2IosStateTx1RxEn         = GPIO_ASSIGN_VALUE(0x6),  ///< TX: 1, RX enabled
  GpioV2IosStateHizRx0          = GPIO_ASSIGN_VALUE(0x7),  ///< Hi-Z, RX: 0 (internally)
  GpioV2IosStateHizRx1          = GPIO_ASSIGN_VALUE(0x8),  ///< Hi-Z, RX: 1 (internally)
  GpioV2IosStateTxDisRxEn       = GPIO_ASSIGN_VALUE(0x9),  ///< TX Disabled and RX Enabled (i.e. wake or interrupt)
  GpioV2IosStateMasked          = GPIO_ASSIGN_VALUE(0xF)   ///< IO Standby signal is masked for this pad. In this mode, a pad operates as if IOStandby has not been asserted.
} GPIOV2_IOSTANDBY_STATE;


/**
  GPIO Standby Term configuration
  Standby Termination options for GPIO Pads
**/
typedef enum {
  GpioV2IosTermDefault         = 0x00,
  GpioV2IosTermSame            = GPIO_ASSIGN_VALUE(0x0), ///< Same as state specified in Term
  GpioV2IosTermPuDisPdDis      = GPIO_ASSIGN_VALUE(0x1), ///< Disable Pullup and Pulldown
  GpioV2IosTermPuDisPdEn       = GPIO_ASSIGN_VALUE(0x2), ///< Enable Pulldown
  GpioV2IosTermPuEnPdDis       = GPIO_ASSIGN_VALUE(0x3)  ///< Enable Pullup
} GPIOV2_IOSTANDBY_TERM;


#define GPIOV2_PAD_OWNERSHIP_HOST      (0x00)
#define GPIOV2_PAD_OWNERSHIP_CSME      (0x01)
#define GPIOV2_PAD_OWNERSHIP_ISH       (0x02)
#define GPIOV2_PAD_OWNERSHIP_IE        (0x03)
typedef enum {
  GpioV2PadOwnHost = GPIO_ASSIGN_VALUE(GPIOV2_PAD_OWNERSHIP_HOST),
  GpioV2PadOwnCsme = GPIO_ASSIGN_VALUE(GPIOV2_PAD_OWNERSHIP_CSME),
  GpioV2PadOwnIsh  = GPIO_ASSIGN_VALUE(GPIOV2_PAD_OWNERSHIP_ISH),
  GpioV2PadOwnIe   = GPIO_ASSIGN_VALUE(GPIOV2_PAD_OWNERSHIP_IE),
} GPIOV2_PAD_OWN;


/**
  Host Software Pad Ownership modes
  This setting affects GPIO interrupt status registers. Depending on chosen ownership
  some GPIO Interrupt status register get updated and other masked.
  Please refer to EDS for HOSTSW_OWN register description.
**/
typedef enum {
  GpioV2HostOwnDefault = 0x0,   ///< Leave ownership value unmodified
  /**
  Set HOST ownership to ACPI.
  Use this setting if pad is not going to be used by GPIO OS driver.
  If GPIO is configured to generate SCI/SMI/NMI then this setting must be
  used for interrupts to work
  **/
  GpioV2HostOwnAcpi    = GPIO_ASSIGN_VALUE(0x0),
  /**
  Set HOST ownership to GPIO Driver mode.
  Use this setting only if GPIO pad should be controlled by GPIO OS Driver.
  GPIO OS Driver will be able to control the pad if appropriate entry in
  ACPI exists (refer to ACPI specification for GpioIo and GpioInt descriptors)
  **/
  GpioV2HostOwnGpio    = GPIO_ASSIGN_VALUE(0x1)
} GPIOV2_HOSTSW_OWN;

#define GPIOV2_PAD_HOST_OWNERSHIP_MASK        (0x1)
#define GPIOV2_PAD_GPI_IE_MASK                (0x1)
#define GPIOV2_PAD_GPI_GPE_EN_MASK            (0x1)
#define GPIOV2_PAD_NMI_EN_MASK                (0x1)
#define GPIOV2_PAD_SMI_EN_MASK                (0x1)


typedef enum {
  GpioV2IntRxEvCfgDefault     = 0x00,
  GpioV2IntRxEvCfgLevel       = GPIO_ASSIGN_VALUE(0x0),
  GpioV2IntRxEvCfgEdge        = GPIO_ASSIGN_VALUE(0x1),
  GpioV2IntRxEvCfgDisable     = GPIO_ASSIGN_VALUE(0x2),
  GpioV2IntRxEvCfgLevelEdge   = GPIO_ASSIGN_VALUE(0x3),
} GPIOV2_RXEVCFG;

#define GPIOV2_PAD_RXEV_MASK                  (0x3)
#define GPIOV2_PAD_RXEV_DW0_POS               (25)

///
/// GPIO Direction
///
typedef enum {
  GpioV2DirDefault         = 0x0,                     ///< Leave pad direction setting unmodified
  GpioV2DirInOut           = GPIO_ASSIGN_VALUE(0x1),  ///< Set pad for both output and input
  GpioV2DirInInvOut        = GPIO_ASSIGN_VALUE(0x2),  ///< Set pad for both output and input with inversion
  GpioV2DirIn              = GPIO_ASSIGN_VALUE(0x3),  ///< Set pad for input only
  GpioV2DirInInv           = GPIO_ASSIGN_VALUE(0x4),  ///< Set pad for input with inversion
  GpioV2DirOut             = GPIO_ASSIGN_VALUE(0x5),  ///< Set pad for output only
  GpioV2DirNone            = GPIO_ASSIGN_VALUE(0x6)   ///< Disable both output and input
} GPIOV2_DIRECTION;

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
  GpioV2IntDefault            = 0x0,                     ///< Leave value of interrupt routing unmodified
  GpioV2IntDis                = 0x1,                     ///< Disable IOxAPIC/SCI/SMI/NMI interrupt generation
  GpioV2IntNmi                = GPIO_ASSIGN_VALUE(0x1),  ///< Enable NMI interrupt only
  GpioV2IntSmi                = GPIO_ASSIGN_VALUE(0x2),  ///< Enable SMI interrupt only
  GpioV2IntSci                = GPIO_ASSIGN_VALUE(0x4),  ///< Enable SCI interrupt only
  GpioV2IntApic               = GPIO_ASSIGN_VALUE(0x8),  ///< Enable IOxAPIC interrupt only
  GpioV2IntLevel              = GPIO_ASSIGN_VALUE(0x10), ///< Set interrupt as level triggered
  GpioV2IntEdge               = GPIO_ASSIGN_VALUE(0x20), ///< Set interrupt as edge triggered (type of edge depends on input inversion)
  GpioV2IntLvlEdgDis          = GPIO_ASSIGN_VALUE(0x40), ///< Disable interrupt trigger
  GpioV2IntBothEdge           = GPIO_ASSIGN_VALUE(0x80), ///< Set interrupt as both edge triggered
} GPIOV2_INT_CONFIG;

#define GPIOV2_PAD_GPIROUTNMI_DW0_POS       (17)
#define GPIOV2_PAD_GPIROUTNMI_DW0_MASK      (1)

#define GPIOV2_PAD_GPIROUTSMI_DW0_POS       (18)
#define GPIOV2_PAD_GPIROUTSMI_DW0_MASK      (1)

#define GPIOV2_PAD_GPIROUTSCI_DW0_POS       (19)
#define GPIOV2_PAD_GPIROUTSCI_DW0_MASK      (1)

#define GPIOV2_PAD_GPIROUTIOXAPIC_DW0_POS   (20)
#define GPIOV2_PAD_GPIROUTIOXAPIC_DW0_MASK  (1)

/**
  GPIO RX Raw override to '1'
  This bit determines if the selected pad state is being overridden to '1'.
  This setting is applicable only if in input mode (both in GPIO and native usage).
  The override takes place at the internal pad state directly from buffer and before the RXINV.
**/
typedef enum {
  GpioV2RxRaw1Default           = 0x0,  ///< Use default input override value
  GpioV2RxRaw1Dis               = 0x1,  ///< No Override input
  GpioV2RxRaw1En                = 0x3   ///< Override input to '1'
} GPIOV2_RX_RAW_OVERRIDE;

//
// Structure for native pin data
//
typedef struct {
  GPIOV2_PAD              Pad;
  GPIOV2_PAD_MODE         Mode;
  GPIOV2_IOSTANDBY_STATE  IosState;
  GPIOV2_IOSTANDBY_TERM   IosTerm;
} GPIOV2_PAD_NATIVE_FUNCTION;

typedef enum {
  // Ownership related registers
  GpioV2PadOwnReg,
  GpioV2PadHostOwnReg,

  // Lock related registers
  GpioV2PadCfgLockReg,
  GpioV2PadCfgLockTxReg,

  // Interrupts related registers
  GpioV2GpiIsReg,
  GpioV2GpiIeReg,
  GpioV2GpiGpeStsReg,
  GpioV2GpiGpeEnReg,
  GpioV2SmiStsReg,
  GpioV2SmiEnReg,
  GpioV2NmiStsReg,
  GpioV2NmiEnReg,

  // Configuration registers
  GpioV2Dw0Reg,
  GpioV2Dw1Reg,
  GpioV2Dw2Reg,

  // Community registers
  GpioV2MiscCfg,
  GpioV2AcReg

} GPIOV2_REGISTER;

typedef struct {
  UINT16  PadOwn;
  UINT16  PadCfgLock;
  UINT16  PadCfgLockTx;
  UINT16  HostOwn;
  UINT16  GpiIs;
  UINT16  GpiIe;
  UINT16  GpiGpeSts;
  UINT16  GpiGpeEn;
  UINT16  SmiSts;
  UINT16  SmiEn;
  UINT16  NmiSts;
  UINT16  NmiEn;
  UINT16  Dw0;
} GPIOV2_GROUP_REGISTERS_OFFSETS;

//
// Community Registers Offsets
//
typedef struct {
  UINT16  MiscCfg;
} GPIOV2_COMMUNITY_REGISTERS_OFFSETS;

#define GPIOV2_PAD_MISCCFG_GPE0_DW2_MASK     (BIT19 | BIT18 | BIT17 | BIT16)
#define GPIOV2_PAD_MISCCFG_GPE0_DW2_POS      (16)
#define GPIOV2_PAD_MISCCFG_GPE0_DW1_MASK     (BIT15 | BIT14 | BIT13 | BIT12)
#define GPIOV2_PAD_MISCCFG_GPE0_DW1_POS      (12)
#define GPIOV2_PAD_MISCCFG_GPE0_DW0_MASK     (BIT11 | BIT10 | BIT9 | BIT8)
#define GPIOV2_PAD_MISCCFG_GPE0_DW0_POS      (8)

#endif
