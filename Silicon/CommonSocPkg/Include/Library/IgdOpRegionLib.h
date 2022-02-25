/** @file
  This is part of the implementation of an Intel Graphics drivers OpRegion /
  Software SCI interface between system Bootloader, ASL code, and Graphics drivers.
  The code in this file will load the driver and initialize the interface
  Supporting Specification: OpRegion / Software SCI SPEC 0.70

  Acronyms:
    IGD:        Internal Graphics Device
    NVS:        ACPI Non Volatile Storage
    OpRegion:   ACPI Operational Region
    VBT:        Video BIOS Table (OEM customizable data)
    IPU:        Image Processing Unit

  Copyright (c) 2021 - 2022, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _IGD_OP_REGION_LIB_H_
#define _IGD_OP_REGION_LIB_H_

#define VBT_BLK_ID_GENERAL2_INFO              2

#pragma pack(1)

#define ChildStruct_MAX                       8         ///< Maximum number of child structures in VBT
#define CompressionStruct_MAX                 2         ///< Maximum number of compression parameter structures in VBT.

#define NO_DEVICE                             0x00      ///< Defines a null display class.
#define DISPLAY_PORT_HDMI_DVI_COMPATIBLE      0x60D6    ///< Defines a display class of Integrated DisplayPort with HDMI/DVI Compatible
#define HDMI_DVI                              0x60D2    ///< Defines a display class of Integrated HDMI/DVI
#define eDP_ONLY                              0x1806    ///< Defines a display class of eDP only
#define AUX_CHANNEL_B                         0x10
#define HDMI_B                                0x01      ///< Defines a output port HDMI-B
#define HDMI_D                                0x03      ///< Defines a output port HDMI-D
#define DISPLAY_PORT_B                        0x07      ///< Defines a output port DisplayPort B

/**
  This defines the Structure for a CHILD_STRUCT (used for all the displays).
**/
typedef struct {
  UINT16  DeviceHandle;         ///< Unique ID indicating the group of display device (LFP/EFP1/EFP2/EFP3/EFP4).
  UINT16  DeviceClass;          ///< Indicates the class of display device.
  UINT8   I2CSpeed;             ///< Defines the I2C speed to be used for I2C transaction.
  /**
  Defines the DP on board redriver configuration.
  BIT[7]    : Reserved
  BIT[6]    : Is On Board DP Redriver Present
          0 : No
          1 : Yes
  BIT[5:3]  : On Board Redriver VSwing Level
          0 : Level 0
          1 : Level 1
          2 : Level 2
          3 : Level 3
     Others : Reserved
  BIT[2:0]  : On Board Redriver PreEmph Level
          0 : Level 0
          1 : Level 1
          2 : Level 2
          3 : Level 3
  Others    : Reserved
  **/
  union{
  UINT8 Value;
  struct {
    UINT8 OnBoardPreEmphLevel:3;
    UINT8 OnBoardVSwingLevel:3;
    UINT8 OnBoardRedriverPresent:1;
    UINT8 Reserved:1;
    } Bits;
  } DpOnBoardRedriver;

  /**
  Defines the DP on dock redriver configuration.
  BIT[7]    : Reserved
  BIT[6]    : Is On Dock DP Redriver Present
          0 : No
          1 : Yes
  BIT[5:3]  : On Dock Redriver VSwing Level
          0 : Level 0
          1 : Level 1
          2 : Level 2
          3 : Level 3
     Others : Reserved
  BIT[2:0]  : On Dock Redriver PreEmph Level
          0 : Level 0
          1 : Level 1
          2 : Level 2
          3 : Level 3
  Others    : Reserved
  **/
  union {
  UINT8 Value;
  struct {
    UINT8 OnDockPreEmphLevel:3;
    UINT8 OnDockVSwingLevel:3;
    UINT8 OnDockRedriverPresent:1;
    UINT8 Reserved:1;
    } Bits;
  } DpOnDockRedriver;

  /**

  Defines the HDMI level shifter configuration.
  BIT[7:5]  : Hdmi Maximum data rate
  BIT[4:0]  : Hdmi Level shifter value

  **/
  union{
  UINT8 Value;
  struct {
    UINT8 HdmiLevelShifterValue:5;
    UINT8 HdmiMaxDataRateBits:3;
    } Bits;
  } HdmiLevelShifterConfig;

  UINT16  EFPDTDBufferPointer;  ///< Pointer to the DTD timing to be used in case of edidless EFP.

  /**
  Defines the first set of flags.
  BIT[7-4]  : Reserved
  BIT[3]    : Dual pipe ganged display support
          0 : Display uses a single pipe/port
  BIT[2]    : Compression Method Select
          0 : Compression using picture parameter set (PPS)
          1 : Compression using Capability parameter set (CPS)
  BIT[1]    : Compression enable/disable for this display.
          0 : Disabled
          1 : Enabled
  BIT[0]    : EDID less EFP Enable
          0 : Enable support for EDID less EFP.
          1 : Disable support for EDID less EFP.
  **/
  union {
  UINT8 Value;
  struct {
    UINT8 EdidlessEfpEnable:1;
    UINT8 CompressionEnable:1;
    UINT8 CompressionMethod:1;
    UINT8 IsDualPortEnabled:1;
    UINT8 Reserved:4;
    } Bits;
  } Flags0;

  /**
  Defines the compression index field for the display.
  BITS[7-4]  : Reserved
  BITS[3-0]  : Compression Structure index in the block 55.
        0x0  : Index 0 in block 55
        0x1  : Index 1 in block 55
        0xF  : Not Applicable.
      Others : Reserved
  **/
  union {
  UINT8 Value;
  struct {
    UINT8 IndexInBlock55:4;
    UINT8 Reserved:4;
    } Bits;
  } CompressionStructureIndex;

  UINT8   SlaveDdiPort;         ///< The DVO port number of slave DDI to be used in case Flags0[3] = 1.

  UINT8   Reserved_1;           ///< Reserved and might be used in other platforms.
  UINT16  AddInOffset;          ///< Obsolete field.
  UINT8   DVOPort;              ///< Specifies the port number of the display device represented in the device class.
  UINT8   I2CBus;               ///< Specifies the GMBUS or I2C pin pair for add in card.
  UINT8   SlaveAddr;            ///< Specifies the I2C address of the add in card.
  UINT8   DDCBus;               ///< Specifies the GMBUS pin pair for EDID read.
  UINT16  TimingInfoPtr;        ///< Pointer to the buffer where VBIOS stores the EDID of device.
  UINT8   DVOCfg;               ///< Obsolete field.

  /**
  Flags 1\n
  Bits 7:6  : Reserved
  Bit 3     : Override Phy values from Vbt
          0 : Override is not enabled
          1 : Override is enabled
  Bit 4     : HPD Sense Invert
          0 : Invert not required (default)
          1 : Invert required
  Bit 3     : IBoost feature enable/disable.
          0 : IBoost feature is disabled.
          1 : IBoost feature is enabled.
  Bit 2     : Hdmi 2.0 Motherboard Downsuppotred options
          0 : Motherboard Down chip not supported
          1 : Motherboard Down Chip Supported on the Board
  Bit 1     : Lane Reversal feature.
          0 : Disable
          1 : Enable
  Bit 0     : DP/HDMI routed to dock.
          0 : Disable
          1 : Enable
  **/
  union {
  UINT8 Value;
  struct {
    UINT8 DockablePort:1;
    UINT8 EnableLaneReversal:1;
    UINT8 OnBoardLsPconDonglePresent:1;
    UINT8 IBoostEnable:1;
    UINT8 IsHpdInverted:1;
    UINT8 OverridePhyTable:1;
    UINT8 Reserved:2;
    } Bits;
  } Flags_1;

  UINT8   Compatibility;        ///< Compatibility is used in VBIOS only. It was used before device class was defined.
  UINT8   AUX_Channel;          ///< Specifies the aux channel to be used for display port devices.
  UINT8   Dongle_Detect;        ///< Indicates whether dongle detect is enabled or not.
  UINT8   Capabilities;         ///< Bits 1-0 indicate pipe capabilities whether display can be used on one pipe or both the pipes.
  UINT8   DVOWiring;            ///< Obsolete field.
  UINT8   MipiBridgeType;       ///< MIPI bridge type
  UINT16  DeviceClassExtension; ///< Obsolete.
  UINT8   DVOFunction;          ///< Obsolete.

  /**
  Flags 2
  Bits 7:4  : DP Port trace length from silicon to output port on the board
          0 : Default RVP length
          1 : Short trace length
          2 : Long trace length
  Bits 3:2  : Reserved
  Bit 1     : Indicates whether this port is Thunderbolt port or not for Gen11+ platforms.
              This field implicitly means that the port is also a type C connector represented by bit 0.
          0 : No
          1 : Yes
  Bit 0     : DP 2 lane RCR# 1024829: USB type C to enable 2 lane DP display for platforms till Gen10.
              This field is also used from Gen11+ to indicate whether the display port is a type C connector.
          0 : Disable
          1 : Enable
  **/
  union {
    UINT8   Value;
    struct {
      UINT8   DpAltModeOverTypeCEnabled:1;      ///< Indicates whether this port supports DP alt mode over type C.
      UINT8   TbtAltModeOverTypeCEnabled:1;     ///< Indicates whether this port supports TBT alt mode over type C.
      UINT8   Reserved:2;                       ///< Reserved for future use.
      UINT8   DpPortTraceLength:4;              ///< Dp port trace length from silicon to port.
    } Bits;
  } Flags_2;
  UINT8   DP2XGpioIndex;        ///< GPIO index number for the USB type C.
  UINT16  DP2XGpioNumber;       ///< GPIO number for USB type C.

  /**
  IBoost magnitude field.
  Bits 7:4  : DP Boost magnitude
          0 : 1
          1 : 3
          2 : 7
     Others : Reserved for SKL.
  Bits 3:0  : HDMI Boost magnitude
          0 : 1
          1 : 3
          2 : 7
  Others : Reserved.
  **/
  union {
    UINT8   Value;
    struct {
      UINT8   DpEdpBoostMagnitude:4;
      UINT8   HdmiBoostMagnitude:4;
    } Bits;
  } BoostMagnitude;

  /**
  Defines the DP Maximum link rate supported by platform.
  BIT[7:2]  : Reserved.
  BIT[1:0]  : DP max link rate supported by platform.
  **/
  union{
    UINT8 Value;
    struct {
      UINT8 DpMaxLinkRate:2;
      UINT8 Reserved:6;
    } Bits;
  } DpPortMaxLinkRate;

} CHILD_STRUCT;


#pragma pack()

typedef
VOID
(EFIAPI *GOP_VBT_UPDATE_CALLBACK) (
  IN CHILD_STRUCT **ChildStructPtr
  );

typedef struct {
  UINT8                  TurboIMON;
  GOP_VBT_UPDATE_CALLBACK callback;
} IGD_OP_PLATFORM_INFO;

/**

  Graphics OpRegion / Software SCI driver installation function.

  @param[in] GopVbtUpdateCallback   A callback routine for platform to update VBT.

  @retval EFI_SUCCESS     The driver installed without error.
  @retval EFI_ABORTED     The driver encountered an error and could not complete
                          installation of the ACPI tables.
**/
EFI_STATUS
EFIAPI
IgdOpRegionInit (
  IN IGD_OP_PLATFORM_INFO *PlatformInfo
  );

/**
  Locate a VBT block within VBT bianry by its block ID.

  @param[in] VbtBuf     VBT binary buffer pointer.
  @param[in] BlockId    A VBT block ID to locate.

  @retval    NULL       Not found.
             Others     The pointer to the block header.
**/
UINT8 *
EFIAPI
LocateVbtBlockById (
  IN  UINT8     *VbtBuf,
  IN  UINT8      BlockId
  );

#endif
