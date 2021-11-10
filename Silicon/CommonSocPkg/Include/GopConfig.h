/** @file
Header file for GOP Configuration Library

  Copyright (c) 2021, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#ifndef _GOP_CONFIG_H_
#define _GOP_CONFIG_H_

#pragma pack(1)

typedef struct {
  UINT16  Dclk;                         // DClk in 10 KHz
  UINT8   HActive;                      // HActive [7:0]
  UINT8   HBlank;                       // HBlank [7:0]
  UINT8   HA_HB_UpperNibble;            // Upper nibble = HActive [11:8]
  UINT8   VActive;                      // VActive [7:0]
  UINT8   VBlank;                       // VBlank [7:0]
  UINT8   VA_VB_UpperNibble;            // Upper nibble = VActive [11:8]
  UINT8   HSyncOffset;                  // HSync offset from blank start LSB
  UINT8   HPulseWidth;                  // HSync Pulse Width, LSB
  UINT8   VsyncOffset_VpulseWidth_LSB;  // Bits 7:4 = VSync offset [3:0]
  UINT8   HSO_HSPW_V_High;              // Bits 7:6 = HSync Offset [9:8]
  UINT8   HorImageSize;                 // Horizontal Image Size
  UINT8   VerImageSize;                 // Vertical Image Size
  UINT8   HIS_VIS_High;                 // UpperLmtH_V Upper limits of H. and V. image size
  UINT8   HBorder;                      // Horizontal Border
  UINT8   VBorder;                      // Vertical Border
  UINT8   Flags;                        // Flags
} DTD_STRUCTURE;                        // 18 Bytes

typedef struct {
  UINT16  XRes;
  UINT16  YRes;
  UINT32  SerialNo;
  UINT8   Week;
  UINT8   Year;
} PID_DATA;                             // 10 Bytes

//
// VBT Header
//
/**
  This structure defines the VBT Header.
**/
typedef struct {
  UINT8   Product_String[20]; ///< "$VBT_Cannonlake" is the product string
  UINT16  Version_Num;        ///< Defines the VBT Header version number.
  UINT16  Header_Size;        ///< Defines the size of VBT Header.
  UINT16  Table_Size;         ///< Defines the size of complete VBT.
  UINT8   Checksum;           ///< Defines the checksum of entire VBT
  UINT8   Reserved1;          ///< Reserved field 1 of 1 byte.
  UINT32  Bios_Data_Offset;   ///< Defines the offset of VBT Data block.
  UINT32  Aim_Data_Offset[4]; ///< 4 reserved pointers to VBT data blocks.
} VBT_HEADER;

/**
  This structure defines the VBT BIOS Data Block Header
**/
typedef struct {
  UINT8   BDB_Signature[16];  ///< Defines the Bios Data Block signature "BIOS_DATA_BLOCK".
  UINT16  BDB_Version;        ///< Defines the VBT (data) version.
  UINT16  BDB_Header_Size;    ///< Defines the size of VBT Bios data block header.
  UINT16  BDB_Size;           ///< Defines the size of Bios data block.
} VBT_BIOS_DATA_HEADER;

/**
  This structure defines the common block header
**/
typedef struct {
  UINT8   BlockId;            ///< Defines Block ID
  UINT16  BlockSize;          ///< Defines the size of the block.
} VBT_BLOCK_COMMON_HEADER;

/**
  This structure defines the BMP Signon Message and Copyright Message Structure
**/
typedef struct {
  UINT8   BlockId;            ///< Defines Block ID : 254
  UINT16  BlockSize;          ///< Defines the size of BMP Signon block.

  UINT16  Bmp_BIOS_Size;      ///< Defines the BIOS size 32k/48k/64k.
  UINT8   BIOS_Type;          ///< Defines the type of BIOS desktop or mobile.
  UINT8   RelStatus;          ///< Defines the release status of the current GOP driver.
  UINT8   BIOS_HW;            ///< Defines the Hardware i.e. Tigerlake.
  UINT8   INT_HW;             ///< Defines the integrated hardware supported eDP/HDMI/DP.
  UINT8   Build_Number[4];    ///< Defines the build number string.
  UINT8   SignOn[155];        ///< Defines the sign on message.
  UINT8   CopyRight[61];      ///< Defines the copyright message.
} BMP_STRUCTURE_SIGNON;

/**
  This structure defines the BMP General Bits
**/
typedef struct {
  UINT16  bmp_BIOS_CS;          ///< Defines the start of BIOS code segment
  UINT8   bmp_DOS_Boot_Mode;    ///< Defines the mode number to set when DOS is boot
  UINT8   bmp_BW_Percent;       ///< Set percentage of total memory BW
  UINT8   bmp_Popup_Mem_Size;   ///< Default Popup memory size in KB
  UINT8   bmp_Resize_PCI_BIOS;  ///< BIOS size granularity in 0.5 KB
  UINT8   Switch_CRT_To_DDC2;   ///< Obsolete field: Is the CRT already switched to DDC2
  UINT8   bmp_Allow_Config;     ///< Bit 1 : 1, Enable aspect ratio for DOS
                                ///< Bit 0 : 1, Allow boot to DVI even if it is not attached.
} BMPGEN;

/**
  This structure defines Block 254 (BMP structure)
**/
typedef struct {
  BMP_STRUCTURE_SIGNON    bmp_Signon_Message;   ///< Instance of signon and copyright message structure
  BMPGEN                  bmp_General_Bytes;    ///< Instance of BMP General Bits structure.
} BLOCK254_BMP_Structure;

/**
  This structure defines Block 1 (General Bytes Definitions)
**/
typedef struct {
  UINT8   BlockId;        ///< Defines the Block ID (1)
  UINT16  BlockSize;      ///< Defines the size of General bytes definition block.

  /**
  BMP General Bit Definitions 1\n
  Bit 7 = DVO A color flip bit
    = 0, No DVO A color flip
    = 1, Flip DVO A color
  Bits 6:4 = Clear screen (CLS) after Signon
      = 000, No CLS
      = 001, 0.5 sec pause and then CLS
      = 010, 1.0 sec pause and then CLS
      = 011, 1.5 sec pause and then CLS
      = 100, 2.0 sec pause and then CLS
      = 101, 2.5 sec pause and then CLS
      = 110, 3.0 sec pause and then CLS
      = 111, 3.5 sec pause and then CLS
  Bit 3 = 1  Enable Display Signon
  Bit 2 = 1  Enable Flex-aim Support
  Bits 1:0 = Flat panel fitting enabling
      = 00, Centering
      = 01, Reserved
      = 10, Aspect scaling
      = 11, Fullscreen
  **/
  union {
    UINT8  Value;
    struct {
      UINT8 PanelFitterEnabling:2;
      UINT8 FlexAimSupportEnable:1;
      UINT8 DisplaySignonEnable:1;
      UINT8 ClearScreenTime:3;
      UINT8 DvoAColorFlipEnable:1;
    } Bits;
  } bmp_Bits_1;

  /**
  BMP General Bit Definitions\n
  Bit 7 = Hot plug support
    = 0, Hot plug disabled
    = 1, Hot plug enabled
  Bit 6 = Dynamic CD clock feature
    = 0, Dynamic CD clock feature is disabled
    = 1, Dynamic CD clock feature is enabled
  Bit 5 = Underscan support for VGA timings
  Bit 4 = Disable SSC in Dual Display Twin Mode. (This field is obsolete now. Kept for VBIOS only.)
    = 0, No
    = 1, Yes
  Bit 3 = LFP power state on override by 5f64h,08h
    = 0, No Override
    = 1, Override
  Bit 2 = Internal LVDS SSC frequency. (This field is obsolete now. Kept for VBIOS only.)
    = 0, 96/120MHz
    = 1, 100MHz
  Bit 1 = internal LVDS SSC (Spread Spectrum Clock) (This field is obsolete now. Kept for VBIOS only.)
    = 0, Disabled
    = 1, Enabled
  Bit 0 = KvmrSessionEnable.
    = 0, Disabled
    = 1, Enabled
  **/
  union {
    UINT8 Value;
    struct {
      UINT8 KvmrSessionEnable:1;
      UINT8 Reserved_1:5;
      UINT8 DynamicCdClockEnable:1;
      UINT8 HotPlugEnable:1;
    } Bits;
  } bmp_Bits_2;

  /**
  BMP General Bit Definitions 3\n
  Bit 7 = Ignore strap status
      = 0 Do not ignore
      = 1 Ignore
  Bit 6 = Panel Timing algorithm
      = 0 Preferred timings
      = 1 Best fit timings
  Bit 5 Copy iLFP DTD to SDVO LVDS DTD
      = 0 Don't copy DTD
      = 1 Copy DTD to
  Bit 4 = VBIOS normal/extd. DT mode
      = 0 Normal mode
      = 1 DUAL mode
  Bit 3 = FDI RX Polarity
      = 0 Normal
      = 1 Inverted
  Bit 2 = Enable 180 Degree Rotation
      = 0  Disable
      = 1  Enable
  Bit 1 = Single DVI-I connector for CRT and DVI display: Obsolete field
      = 0 Disabled
      = 1 Enabled
  Bit 0 = Smooth Vision
      = 0  Disabled
      = 1  Enabled
  **/
  union {
    UINT8 Value;
    struct {
      UINT8 Reserved1:1;
      UINT8 SingleDviiCrtConnector:1;
      UINT8 Enable180DegRotation:1;
      UINT8 FdiRxPolarity:1;
      UINT8 Reserved2:4;
    } Bits;
  } bmp_Bits_3;

  UINT8   Reserved;     ///< Reserved field. It was Legacy_Monitor_Detect in previous platforms.

  /**
  Integrated display device support\n
  Bits 7:6 = Reserved
  Bit 5 = DP SSC Dongle Enable/Disable
  Bit 4 = DP SSC Frequency. (This field is obsolete now. Kept for VBIOS only.)
    = 0, 96 MHz
    = 1, 100 MHz
  Bit 3 = DP SSC Enable
    = 0, Disable
    = 1, Enable
  Bit 2 = Integrated EFP support
    = 0, Disable
    = 1, Enable
  Bit 1 = Integrated TV support. (This field is obsolete now. Kept for VBIOS only.)
    = 0, Disable
    = 1, Enable
  Bit 0 = Integrated CRT support: Obsolete field
    = 0, Disable
    = 1, Enable
  **/
  union {
    UINT8 Value;
    struct {
      UINT8 CrtSupported:1;
      UINT8 TvSupported:1;
      UINT8 EfpSupported:1;
      UINT8 DpSscEnable:1;
      UINT8 DpSscFrequency:1;
      UINT8 DpDongleSscEnable:1;
      UINT8 Reserved1:2;
    } Bits;
  } Int_Displays_Support;
} VBT_GENERAL1_INFO;

/**
  This defines the Structure of PRD Boot Table Entry
**/
typedef struct {

  UINT16 BootDev_PipeA;  ///< Bitmap representing the display to boot on Pipe A.
  UINT16 BootDev_PipeB;  ///< Bitmap representing the display to boot on Pipe B.
} PRD_TABLE;

/**
  This defines the structure of Block 254 (PRD Boot Table/Child Device List)
**/
typedef struct {
  UINT8     BlockId;       ///< Defines the Block ID (254)
  UINT16    BlockSize;     ///< Defines the size of Block 254
  UINT16    NumOfEntry;    ///< Defines the number of entries in the Child device list
  PRD_TABLE PRDTable[16];  ///< Defines the Child device list for enumerating child handles.
} PRD_BOOT_TABLE;

/**
  This structure defines Block 2 (General Bytes Definitions)
**/
typedef struct {
  UINT8         BlockId;          ///< Defines the Block ID : 2.
  UINT16        BlockSize;        ///< Defines the size of VBT General Info 2 Block.

  UINT8         bmp_CRT_DDC_GMBUS_Pin;  ///< Obsolete field: Selects the CRT DDC GMBUS pin pair.
  UINT8         bmp_DPMS_Bits;          ///< BMP DPMS Bit Definitions.
  UINT16        bmp_Boot_Dev_Bits;      ///< BMP Boot Device Bit Definitions.
  UINT8         SizeChild_Struct;       ///< Size of the ChildStruc structure.

  CHILD_STRUCT  Child_Struct[ChildStruct_MAX];  ///< This array defines all the supported child structures.
} VBT_GENERAL2_INFO;

/**
  This defines structure of a pointer table.
**/
typedef struct {
  UINT16  Offset;       ///< Defines the offset of the table from start of BIOS Data block.
  UINT16  Size;         ///< Defines the size of an entry of the table.
} BMP_TABLE_PTR;

/**
  This structure defines Block 252 (SBIOS Hooks and BMP Table Pointers).
**/
typedef struct {
  UINT8           BlockId;          ///< Defines the Block ID : 252.
  UINT16          BlockSize;        ///< Defines the size of SBIOS Hooks block.
  UINT8           SbiosHooks[18];   ///< This array defines a series of SBIOS hooks. Each entry represents one hook.
  BMP_TABLE_PTR   BmpTablePtr[26];  ///< This array defines pointers to all the important tables in the VBT.
} BLOCK252_SBIOS_Hook;

/**
  This structure defines the PSR feature table entry.
**/
typedef struct {
  UINT8   SRD_Enables;        ///< Defines PSR features such as full link enable/disable and whether aux is required to wake up.
  UINT8   SRD_WaitTimes;      ///< Defines lines to wait before link standby and idle frames to wait before SRD enable.
  UINT16  SRD_TP1_WakeupTime; ///< TP 1 wake up time in multiples of 100.
  UINT16  SRD_TP2_WakeupTime; ///< TP2/TP3 wake up time in multiples of 100
} PSR_FEATURE_TABLE;

/**
  This defines the structure of Block 9 (PSR Features Block)
**/
typedef struct {
  UINT8             BlockId;              ///< Defines the block ID : 9
  UINT16            BlockSize;            ///< Defines the size of PSR Feature block.
  PSR_FEATURE_TABLE PSRFeatureTable[16];  ///< Array containing the PSR Feature table.
} BLOCK09_PSR_FEATURE;

/**
  This structure defines an entry of Mode Removal table.
**/
typedef struct {
  UINT16  XRes;         ///< X resolution of the mode.
  UINT16  YRes;         ///< Y resolution of the mode.
  UINT8   Bpp;          ///< Bits per pixel of the mode.
  UINT16  RRate;        ///< Refresh rate of the mode.
  UINT8   RFlags;       ///< Flags specifying display type and functional area where the mode is to be removed.
  UINT16  PanelFlags;   ///< Applicable to LFP only. Indicates which LFP panels the mode is to be removed.
} MODE_REMOVAL_TABLE_ENTRY;

/**
  This defines the structure of Block 10 (Mode Removal Block)
**/
typedef struct {
  UINT8                     BlockId;              ///< Defines the Block ID : 10.
  UINT16                    BlockSize;            ///< Defines the size of Mode Removal table block.
  UINT8                     EntrySize;            ///< Defines the size of one entry of mode removal table.
  MODE_REMOVAL_TABLE_ENTRY  ModeRemovalTable[20]; ///< Array containing the mode removal table.
  UINT16                    Terminator;           ///< Special value indicating end of mode removal table (0xFFFF).
} BLOCK10_MODE_REMOVAL_TABLE;

/**
  This defines the structure of Block 12 (Driver Features Data Block)
**/
typedef struct {
  UINT8   BlockId;                  ///< Defines the unique Block ID : 12
  UINT16  BlockSize;                ///< Defines the size of Driver features block.

  /**
  This field defines the various driver related bits:\n
  Bit 7 = Use 00000110h ID for Primary LFP
        = 0, No
        = 1, Yes
  Bit 6 = Enable/Disable Sprite in Clone Mode
        = 0, Disable
        = 1, Enable
  Bit 5 = Driver INT 15h hook
        = 0, Disable
        = 1, Enable
  Bit 4 = Dual View Zoom
        = 0, Disable
        = 1, Enable
  Bit 3 = Hot Plug DVO
        = 0, Disable
        = 1, Enable
  Bit 2 = Allow display switching when in Full Screen DOS.
        = 0, Block Display Switching
        = 1, Allow Display Switching
  Bit 1 = Block display switching when DVD active
        = 0, No Block Display Switching
        = 1, Block Display Switching
  Bit 0 = Boot device algorithm
        = 0, OS Default
        = 1, Driver Default
  **/
  UINT8   bmp_Driver_Bits;
  UINT16  bmp_Driver_Boot_Mode_X;   ///< X resolution of driver boot mode.
  UINT16  bmp_Driver_Boot_Mode_Y;   ///< Y resolution of driver boot mode.
  UINT8   bmp_Driver_Boot_Mode_BPP; ///< Bits per pixel of driver boot mode.
  UINT8   bmp_Driver_Boot_Mode_RR;  ///< Refresh rate of driver boot mode.

  /**
  This field defines the extended driver bits 1.\n
  Bits [15:14] = Integrated HDMI configuration
              = 00b,  No Integrated HDMI
              = 01b,  Port-B Only
              = 10b,  Port-C Only
              = 11b,  Both Port-B and Port-C
  Bits 13 = TV Hotplug
  Bits [12:11]  = LFP configuration
                = 00b,  No LVDS
                = 01b,  Integrated LVDS
                = 10b,  SDVO LVDS
                = 11b,  eDP
  Bit 10 = Obsolete field: CRT hotplug
          = 0, Disabled
          = 1, Enabled (Default)
  Bit 9 = SDVO device power down
        = 0, Disabled (Default)
        = 1, Enabled
  Bit 8 = Preserve Aspect Ratio
        = 0, Disabled (Default)
        = 1, Enabled
  Bit 7 = Display "Maintain Aspect Scaling" via CUI
        = 0, No
        = 1, Yes (Default)
  Bit 6 = Sprite Display Assignment when Overlay is Active in Clone Mode:
        = 0, Secondary
        = 1, Primary
  Bit 5 = Default Power Scheme user interface
        = 0, CUI
        = 1, 3rd Party Application
  Bit 4 = NT 4.0 Dual Display Clone Support
        = 0, Disable
        = 1, Enable
  Bit 3 = Default Render Clock Frequency
        = 0, High Frequency
        = 1, Low Frequency
  Bit 2 = Dual-Frequency Graphics Technology
        = 0, No
        = 1, Yes
  Bit 1 = Selective Mode Pruning
        = 0, No
        = 1, Yes
  Bit 0 = Enable LFP as primary
        = 0, Disable
        = 1, Enable
**/
  UINT16  bmp_Ext_Driver_Bits;

  /**
  This defines the driver flags related to CUI Hot key.\n
  Bits [7:3] - Reserved
  Bit 2 = Display Subsystem Enable/Disable
        = 0, Enable (default Value)
        = 1, Disable
  Bit 1 = Embedded Platform
        = 0, False
        = 1, True
  Bit 0 = Define CUI HotK Displays Statically
        = 0, No
        = 1, Yes
  **/
  UINT8   bmp_Display_Detect_CUIHotK;

  UINT16  bmp_Legacy_CRT_Max_X;         ///< Obsolete field: Defines the legacy CRT X resolution for driver boot mode.
  UINT16  bmp_Legacy_CRT_Max_Y;         ///< Obsolete field: Defines the legacy CRT Y resolution for driver boot mode.
  UINT8   bmp_Legacy_CRT_Max_RR;        ///< Obsolete field: Defines the legacy CRT refresh rate for driver boot mode.

  /**
  This field defines the extended driver bits 2.\n
  Bits [7:1] - Reserved
  Bit 0 = Enable Internal Source Termination for HDMI
        = 0, External Termination
        = 1, Internal Termination
  **/
  UINT8   bmp_Ext2_Driver_Bits;

  UINT8   bmp_VBT_Customization_Version;  ///< Defines the customized VBT version number.

  /**
  This field defines all the driver feature flags.\n
  Bit 15 = PC Features Field's Validity
         = 0, Invalid
         = 1, Valid
  Bit 14 = Hpd_wake - HPD events are routed to display driver when system is in S0ix/DC9
         = 0, Disable
         = 1, Enable
  Bit 13 = Assertive Display Technology (ADT)
         = 0, Disable
         = 1, Enable
  Bit 12 = Dynamic Media Refresh Rate Switching (DMRRS)
         = 0, Disable
         = 1, Enable
  Bit 11 = Dynamic Frames Per Second (DFPS)
         = 0, Disable
         = 1, Enable
  Bit 10 = Intermediate Pixel Storage (IPS)
         = 0, Disable
         = 1, Enable
  Bit 9 = Panel Self Refresh (PSR)
        = 0, Disable
        = 1, Enable
  Bit 8 = Intel Turbo Boost Technology
        = 0, Disable
        = 1, Enable
  Bit 7 = Graphics Power Modulation Technology (GPMT)
        = 0, Disable
        = 1, Enable
  Bit 6 = Graphics Render Standby (RS)
        = 0, Disable
        = 1, Enable
  Bit 5 = Intel Display Refresh Rate Switching (DRRS)
        = 0, Disable
        = 1, Enable
  Bit 4 = Intel Automatic Display Brightness (ADB)
        = 0, Disable
        = 1, Enable
  Bit 3 = DxgkDDI Backlight Control (DxgkDdiBLC)
        = 0, Disable
        = 1, Enable
  Bit 2 = Intel Display Power Saving Technology (DPST)
        = 0, Disable
        = 1, Enable
  Bit 1 = Intel Smart 2D Display Technology (S2DDT)
        = 0, Disable
        = 1, Enable
  Bit 0 = Intel Rapid Memory Power Management (RMPM)
        = 0, Disable
        = 1, Enable
  **/
  UINT16  bmp_Driver_Feature_Flags;
} BLOCK12_DRIVER_FEATURES;

/**
  This structure defines an entry of OEM mode table.
**/
typedef struct {
  /**
  Mode Flags:
    Bits[7:3] = Reserved
    Bit 2 = Enable/disable this OEM mode in GOP driver.
    Bit 1 = Enable/disable this mode in Driver
    Bit 0 = Enable/disable this mode in VBIOS
  **/
  UINT8   ModeFlags;

  /**
  Display Device Flags:
    Bit 7 = LFP2
    Bit 6 = EFP2
    Bit 5 = EFP3
    Bit 4 = EFP4
    Bit 3 = LFP
    Bit 2 = EFP
    Bit 1 = Rsvd
    Bit 0 = Rsvd
  **/
  UINT8   DisplayFlags;
  UINT16  XRes;         ///< Defines the X resolution of the mode.
  UINT16  YRes;         ///< Defines the Y resolution of the mode.

  /**
  Defines the bits per pixel of the mode.
    Bit 7:3 = Reserved
    Bit 2 = 32 BPP
    Bit 1 = 16 BPP
    Bit 0 = 8 BPP
  **/
  UINT8   Bpp;
  UINT8   RRate;        ///< Defines the refresh rate of the mode.
  DTD_STRUCTURE Dtd;    ///< Defines the 18 byte timing config for the mode.
  /**
  Display Device Flags: This field should be used only from Gen11 HP platforms
    Bit 15:10 : Reserved
    Bit 9 :     EFP8
    Bit 8       EFP7
    Bit 7 :     LFP2
    Bit 6 :     EFP2
    Bit 5 :     EFP3
    Bit 4 :     EFP4
    Bit 3 :     LFP1
    Bit 2 :     EFP1
    Bit 1 :     EFP5
    Bit 0 :     EFP6
  **/
  UINT16  DisplayFlags2;
} OEM_MODE_ENTRY;

/**
  This defines the structure of Block 20 (OEM Mode Customization Block)
**/
typedef struct {
  UINT8           BlockId;          ///< Defines the unique block ID : 20
  UINT16          BlockSize;        ///< Defines the size of OEM customization block.
  UINT8           NumOfEntry;       ///< Defines the number of entries in OEM Mode table.
  UINT8           EntrySize;        ///< Defines the size of one entry of OEM Mode table.
  OEM_MODE_ENTRY  OemModeTable[6];  ///< Array defining the OEM mode table.
} BLOCK20_OEM_CUSTOMIZATION;

/**
  This structure defines the eDP panel power sequencing parameters.
**/
typedef struct {
  UINT16  T3;         ///< Panel Power-Up Delay.
  UINT16  T8;         ///< Panel Power-On to backlight Enable Delay.
  UINT16  T9;         ///< Backlight-Off to Power-Down Delay.
  UINT16  T10;        ///< Power-Down Delay.
  UINT16  T12;        ///< Power Cycle Delay.
} EDP_PWR_SEQ;

/**
  This structure defines the PWM<-->Backlight delays for a single eDP panel.
**/
typedef struct {
  UINT16  PwmOnToBacklightEnableDelay;      ///< PWM on to backight enable delay.
  UINT16  BacklightDisableToPwmOffDelay;    ///< Backlight disable to PWM off delay.
} EDP_PWM_BACKLIGHT_DELAYS;

/**
  This defines FLT parameters for a single eDP panel.
  Bits[15:12] : VSwing level
            0 : 0.4V (default)
            1 : 0.6V
            2 : 0.8V
            3 : 1.2V
       Others : Reserved
  Bits[11:8]  : Pre-emphasis level
            0 : no pre-emphasis (default)
            1 : 3.5dB
            2 : 6dB
            3 : 9.5dB
       Others : Reserved
  Bits[7:4]   : Lane count (port width)
            0 : x1 mode (default)
            1 : x2 mode
            2 : Reserved
            3 : x4 mode
       Others : Reserved
  Bits[3:0]   : data rate
            0 : 1.62 Gbps
            1 : 2.7 Gbps
            2 : 5.4 Gbps
       Others : Reserved
**/
typedef union {
  UINT16 Value;
  struct {
    UINT16 DataRateObsolete:4;
    UINT16 LaneCount:4;
    UINT16 PreEmphasisLevel:4;
    UINT16 VSwingLevel:4;
  } Bits;
} EDP_FAST_LINK_TRAINING_PARAMS;

/**
  This defines Full link training parameters for a single eDP panel.
  Bits[7:4] : VSwing level
          0 : 0.4V (default)
          1 : 0.6V
          2 : 0.8V
          3 : 1.2V
     Others : Reserved
  Bits[3:0] : Pre-emphasis level
          0 : no pre-emphasis (default)
          1 : 3.5dB
          2 : 6dB
          3 : 9.5dB
     Others : Reserved
**/
typedef union {
  UINT8   Value;
  struct {
    UINT8   PreEmphasisLevel:4;
    UINT8   VSwingLevel:4;
  } Bits;
} EDP_FULL_LINK_TRAINING_PARAMS;

/**
  This defines the structure of Apical Parameters for a single eDP panel.
**/
typedef struct {
  UINT32      PanelOui;             ///< Apical IP specific field for Panel OUI
  UINT32      DPCDBaseAddress;      ///< Apical IP specific field for DPCD Base address
  UINT32      DPCDIrdidixControl0;  ///< Apical IP specific field for DPCD Idridix Control 0
  UINT32      DPCDOptionSelect;     ///< Apical IP specific field for DPCD option select
  UINT32      DPCDBacklight;        ///< Apical IP specific field for DPCD backlight
  UINT32      AmbientLight;         ///< Apical IP specific field for Ambient light
  UINT32      BacklightScale;       ///< Apical IP specific field for backlight scale
} EDP_APICAL_PARAMS;

/**
  This defines the structure of Block 27 (eDP Display Block)
**/
typedef struct {
  UINT8       BlockId;            ///< Defines the unique Block ID : 27
  UINT16      BlockSize;          ///< Defines the size of eDP display VBT block.

  EDP_PWR_SEQ eDP_PWR_SEQ[16];    ///< Array defining the panel power sequencing for all 16 eDP panels.

  /**
  Defines the panel color depth in bits per pixel. 2 Bits for each Panel.
    Bits[1:0] Panel color depth for Panel #1
      = 00, 18bpp
      = 01, 24bpp
      = 10, 30bpp
      = 11, 36bpp
  **/
  UINT32      eDP_Panel_Color_Depth;

  /**
    Array containing the FLT parameters of 16 eDP panels.
  **/
  EDP_FAST_LINK_TRAINING_PARAMS      eDP_Fast_Link_Training_Params[16];

  /**
  This field defines the eDP sDRRS MSA Timing Delay for all 16 eDP panels. 2 Bits for Each Panel.
  Bits[1:0] for Panel #1
    = 00, Line 1
    = 01, Line 2
    = 10, Line 3
    = 11, Line 4
  **/
  UINT32      eDP_sDRRS_MSA_Delay;

  /**
  Defines the S3D feature enable/disable for all 16 eDP panels. 1 Bit for Each Panel.
  Bits[0] for Panel #1
    = 0, S3D disabled for this panel
    = 1, S3D enabled for this panel
  **/
  UINT16      eDP_S3D_Feature;

  /**
  Defines the T3 optimization enable/disable for all 16 panels. 1 Bit for each panel.
  Bits[0] = Panel #1
    = 0, T3 optimization disabled for this panel
    = 1, T3 optimization enabled for this panel
  **/
  UINT16      eDP_T3_Optmization;

  /**
  Defines the Edp vswing and pre-emphasis for all 16 panels. 4 Bits for Each Panel
  Bits[3:0] = Panel #1
    = 0, Use table 1 for this panel.
    = 1, Use table 2 for this panel.
  **/
  UINT64       VswingPreEmphasisTableNum;

  /**
  Defines the Edp fast link training support on all 16 panels. 1 Bit for Each Panel
  Bits[0] = Panel #1
    = 0, FastLinkTraining feature is disabled for this panel
    = 1, FastLinkTraining feature is enabled for this panel
  **/
  UINT16     EdpFastLinkTrainingSupportOnPanel;

  /**
  Defines whether the Set power state at DPCD 600h is to be done in eDP enable/disable sequence.
  Bits[0] = Panel #1
    = 0, Set power state at DPCD 600h feature is disabled for this panel
    = 1, Set power state at DPCD 600h feature is enabled for this panel
  **/
  UINT16     SetPowerStateAtDPCD600h; //This is not used currently

  /**
    Array defining the PWM <--> Backlight related delays for 16 panels.
  **/
  EDP_PWM_BACKLIGHT_DELAYS eDP_Pwm_BackLight_Delays[16];

  /**
  Defines the Edp full link training support on all 16 panels. 1 Bit for Each Panel.
  \verbatim
  Bits[0] : Panel #1
        0 : Initial vswing and pre-emphasis levels are not provided for Full link training
        1 : Initial vswing and pre-emphasis levels are provided for Full link training
  Bits 1 to 15 are for panel # 2 to 16.
  \endverbatim
  **/
  UINT16     InitialFullLinkTrainingParamsProvidedInVbt;

  /**
    Array containing the initial Vswing and Pre-emphasis parameters for Full link training.
  **/
  EDP_FULL_LINK_TRAINING_PARAMS    eDP_Full_Link_Training_Params[16];

  /**
  Defines the Edp Apical assertive display IP support on all 16 panels. 1 Bit for Each Panel.
  Bit 0   : Panel #1
        0 : Apical assertive display IP is disabled for this panel.
        1 : Apical assertive display IP is enabled for this panel.
  Bits 1 to 15 are for panel # 2 to 16.
  **/
  UINT16                           IsApicalAssertiveDisplayIpEnable;

  /**
    Array containing the Apical parameters for all 16 panels
  **/
  EDP_APICAL_PARAMS                eDP_Apcial_Params[16];

  /**
    This field specifies Data Rate to be used for Fast Link Training in unit of 200KHz
  **/
  UINT16                           eDP_Fast_Link_Training_Data_Rate[16];
} BLOCK27_EDP_FEATURES;

/**
  This defines the structure of Block 28 (Edidless EFP support DTD timings)
**/
typedef struct {
  UINT8                 BlockId;                    ///< Defines the unique Block ID : 28
  UINT16                BlockSize;                  ///< Defines the size of Edidless EFP support block.
  DTD_STRUCTURE         Edidless_EFP_DTD_Struc[8];  ///< Array defining the DTD timing for 8 EFP devices.
} BLOCK28_EDIDLESS_EFP;

/**
  This defines the Local Flat panel basic details such as resolution and the various registers.
**/
typedef struct {
  UINT16  XRes;                   ///< X resolution of the panel.
  UINT16  YRes;                   ///< Y resolution of the panel.
  UINT32  LVDSDigDisReg;          ///< MMIO offset of LFP digital display port register.
  UINT32  LVDSDigDisVal;          ///< Value of LFP digital display port register.
  UINT32  OnSeqDelayReg;          ///< MMIO offset of Panel power on sequencing delay register.
  UINT32  OnSeqDelayVal;          ///< Value of Panel power on sequencing delay register.
  UINT32  OffSeqDelayReg;         ///< MMIO offset of Panel power off sequencing delay register.
  UINT32  OffSeqDelayVal;         ///< Value of Panel power off sequencing delay register.
  UINT32  CycleDelay_RefDivReg;   ///< MMIO offset of Panel power cycle delay and reference divider register.
  UINT32  CycleDelay_RefDivVal;   ///< Value of Panel power cycle delay and reference divider register.
  UINT16  Terminate;              ///< Special value 0xFFFF indicating end of data.
} FP_DATA;

/**
  This defines the structure consisting of all details for a single Local Flat panel.
**/
typedef struct {
  FP_DATA       FP_Data;      ///< Instance of ::FP_DATA structure.
  DTD_STRUCTURE DTD_Data;     ///< Instance of ::DTD_STRUCTURE which contains the DTD timings for the panel.
  PID_DATA      PID_Data;     ///< Instance of ::PID_DATA structure which contains panel related information used by driver.
} LVDS_FP_TABLE;

/**
  This structure defines all the details regarding Backlight control for LFP.
**/
typedef struct {
  /**
  Defines the backlight features for the panel.
  Bits 7:6  = GMBus Speed:
            = 00, 100 KHz
            = 01, 50 KHz
            = 10, 400 KHz
            = 11, 1 MHz
  Bits 5:3  = Inverter GPIO Pins
            = 0, None
            = 1, I2C GPIO pins
            = 2, Analog CRT DDC pins
            = 3, DVI/LVDS DDC GPIO pins
            = 5, sDVO I2C GPIO pins
  Bit 2     = Inverter Polarity (i2c & PWM)
            = 0, Normal (0 = Minimum brightness)
            = 1, Inverted (0 = Maximum brightness)
  Bits 1:0  = BLC Inverter Type
            = 00, None/External
            = 01, i2c
            = 10, PWM
            = 11, Reserved
  **/
  UINT8   BLC_Ftr;

  UINT16  PWM_Freq;       ///< PWM inverter frequency in KHz
  UINT8   Min_Brightness; ///< Minimum brightness in the range 0-255
  UINT8   I2C_Add;        ///< I2C Inverter Slave Address
  UINT8   I2C_Command;    ///< I2C Inverter command code
} BLC;

/**
  This defines the structure of Block 40 (LFP Features)
**/
typedef struct {
  UINT8   BlockId;          ///< Defines the unique Block ID : 40
  UINT16  BlockSize;        ///< Defines the size of LFP Features block.

  UINT8   bmp_Panel_type;   ///< Defines the panel type of LFP.
  UINT8   bmp_Panel_type2;  ///< Defines the panel type of LFP2.

  /**
  Capabilities byte:
  Bit 15:7  = SW Workaround bits
  Bit 6     = Panel EDID support
            = 0, Disable
            = 1, Enable
  Bit 5     = Pixel dithering
            = 0, Disable
            = 1, Enable
  Bit 4     = Panel Fitting ratio calc.
            = 0 - Manual
            = 1 - Automatic
  Bit 3     = Panel Fitting Graphics mode
            = 0, Bilinear
            = 1, Enhanced
  Bit 2     = Panel Fitting Text mode
            = 0, Bilinear
            = 1, Enhanced
  Bit 1:0   = Panel Fitting Support
            = 00, No panel fitting
            = 01, Text panel fitting
            = 10, GFX panel fitting
            = 11, Text+GFX panel fitting
  **/
  UINT16  bmp_LVDS_Capabilities;

  /**
  Defines the channel type of LFP. 2 Bits for each Panel.
  Bits [0:1] for Panel #1
    = 00, Automatic (algorithm)
    = 01, Single Channel
    = 10, Dual Channel
    = 11, Reserved
  **/
  UINT32  INT_LVDS_Panel_Channel_Bits;

  UINT16  Enable_SSC_Bit;         ///< LVDS Spread Spectrum Clock
  UINT16  SSC_Freq_Bit;           ///< LVDS Spread Spectrum Clock Frequency
  UINT16  Disable_SSC_DDT_Bit;    ///< Disable SSC in Dual Display Twin

  /**
  Defines the panel color depth. 1 Bits for each Panel.
  Bits[0] for Panel #01
    = 0, 18bpp
    = 1, 24bpp
  **/
  UINT16  INT_Panel_Color_Depth;

  /**
  Defines the Panel type. 2 Bits for each Panel.
  Bits [0:1] for Panel #1
    = 00, Static DRRS
    = 01, D2PO
    = 10, Seamless
    = 11, Reserved
  **/
  UINT32  DPS_Panel_Type_Bits;

  /**
  Defines the type of backlight control for the LFP. 2 bits for each Panel.
  Bits [0:1] for Panel #1
    = 00, Default
    = 01, CCFL backlight
    = 10, LED backlight
    = 11, Reserved
  **/
  UINT32  BLT_Control_Type_Bits;

  /**
  Defines the LFP power enable flag in S0 state for all 16 panels. 1 Bit for Each Panel.
  Bits[0] : Panel #1
        0 : Do not keep LCDVCC on during S0 state.
        1 : Keep LCDVCC on during S0 state.
  Bits 1 to 15 are for panel # 2 to 16.
  **/
  UINT16     LcdvccOnDuringS0State;
} BLOCK40_LVDS_FEATURES;

/**
  This structure defines the second type of BMP table pointers.
  This is used to store pointers to LFP Flat panel data, DTD and PID information.
**/
typedef struct {
  UINT16  Offset;       ///< Offset of the table.
  UINT8   Size;         ///< Size of the table.
} BMP_TABLE_TYPE2_PTR;

/**
  This structure defines a set of 3 pointers for LFP display.
  These pointers point to FP data, DTD and PID information respectively.
**/
typedef struct {
  BMP_TABLE_TYPE2_PTR   FpTablePtr;   ///< Pointer to FP Data of the LFP panel.
  BMP_TABLE_TYPE2_PTR   DtdTablePtr;  ///< Pointer to DTD of the LFP panel.
  BMP_TABLE_TYPE2_PTR   PidTablePtr;  ///< Pointer to the PID data of the LFP panel.
} LFP_TABLE_POINTERS;

/**
  This defines the structure of Block 41 (LFP Table Pointers for FPDATA, DTD and PID)
**/
typedef struct {
  UINT8               BlockId;                  ///< Defines the unique Block ID=41
  UINT16              BlockSize;                ///< Defines the size of LFP Table Pointer Block.
  UINT8               NumOfEntries;             ///< Defines the number of entries in the Table.
  LFP_TABLE_POINTERS  LfpTablePointers[16];     ///< Array of ::LFP_TABLE_POINTERS for all 16 panels.
  UINT16              LfpPanelNameTableOffset;  ///< Offset of LFP panel names table.
  UINT8               LfpPanelNameLength;       ///< Length of a single LFP panel's name.
} BLOCK41_LFP_TABLE_POINTERS;

/**
  This defines the structure of Block 42 (Complete LFP Panel Information)
**/
typedef struct {
  UINT8         BlockId;                ///< Defines the unique block ID : 42
  UINT16        BlockSize;              ///< Defines the size of Complete LFP panel information for all 16 panels.
  LVDS_FP_TABLE LVDS_FP_Table[16];      ///< Array of ::LVDS_FP_TABLE containing data of 16 panels.
  UINT8         LFP_PANEL_NAMES[16][13];///< Array defining the panel names for all 16 panels.

  /**
  1 Bit for Each Panel
  Bit0  = Scaling feature for panel 1.
        = 0, Scaling feature is disabled for this panel.
        = 1, Scaling feature is enabled for this panel.
  **/
  UINT16        EnableScaling; //This is not used currently

  /**
    Array defining DRRS minimum refresh rate. 1 Byte for Each Panel.
  **/
  UINT8         Seamless_DRRS_Min_RR[16];

  /**
    Array defining Pixel Overlap Count. 1 Byte for Each Panel.
  **/
  UINT8         PixelOverlapCount[16];
} BLOCK42_LVDS_PANEL_INFO;

typedef union {
  /**
  Backlight control parameters.\n
  Bits 7:4  : PWM Controller Selection
          0 : Controller 0
          1 : Controller 1
          2 : Controller 2
          3 : Controller 3
     Others : Reserved.
  Bits 3:0  : PWM Source Selection
          0 : PMIC PWM
          1 : LPSS PWM
          2 : DISPLAY PWM
          3 : CABC PWM
     Others : Reserved.
  **/
  UINT8 Value;
  struct {
    UINT8 PwmSourceSelection:4;
    UINT8 PwmPpsControllerSelection:4;
  } Bits;
} BKLT_CTRL_PARAMS;

/**
  This defines the structure of Block 43 (LFP Brightness Control)
**/
typedef struct {
  UINT8             BlockId;                ///< Defines the unique Block ID : 43
  UINT16            BlockSize;              ///< Defines the size of Brightness control block.

  UINT8             SIZE_BLCStruc;          ///< Defines the size of single entry in Backlight control table for LFP.
  BLC               BLC_Struct[16];         ///< Array defining the backlight control for 16 LFP panels.
  UINT8             Post_Brightness[16];    ///< Array defining the initial brightness for all 16 panels.
  BKLT_CTRL_PARAMS  Brightness_Control[16]; ///< Array defining the brightness control method for all 16 panels
} BLOCK43_LVDS_BLC;

/**
  This defines the structure of Block 44 (LFP Power Conservation Features)
**/
typedef struct {
  UINT8   BlockId;        ///< Defines the unique block ID : 44
  UINT16  BlockSize;      ///< Defines the size of LFP Power Conservation Features block.
  union {
  /**
  Bit[7]        : ALS Enable/Disable
               0 - Disable
               1 - Enable
  Bit[6]        : Display LACE support
               0 - Not supported
               1 - Supported
  Bit[5]        : Default Display LACE enabled status
               0 - Disabled
               1 - Enabled
  Bit[4]        : Reserved
  Bit[3:1]      : Power conservation preference level.
                 4 is default in a range of 1 to 6.
  Bit[0]        : Reserved
  **/
    UINT8  Value;
    struct {
      UINT8 Reserved:1;
      UINT8 PwrConservation:3;
      UINT8 Reserved_1:1;
      UINT8 DefalutDisplayLaceEnable:1;
      UINT8 DisplayLaceSupport:1;
      UINT8 AlsEnable:1;
    } Bits;
  } LfpFeatureBits;

  UINT16  AlsData[10];    ///< Defines the main ALS data.

  union {
  /**
  Bit[7:3]      : Reserved
  Bit[2:0]      : Aggressiveness Level Profile.
            000 - Minimum
            001 - Moderate
            010 - High
  **/
    UINT8  Value;
    struct {
      UINT8 AggressionProfileLevel:3;
      UINT8 Reserved:5;
    } Bits;
  } LaceAggressivenessProfile; ///< Defines the LACE Aggressiveness Profile
} BLOCK44_ALS;


/**
  This defines the structure of Black Frame Insertion table entry.
**/
typedef struct {
  /**
  BFI Features\n
  Bit[7-2]  : Reserved\n
  Bit[1]    : Enable Brightness control in CUI\n
  Bit[0]    : Enable BFI in driver
  **/
  UINT8         EnableBits;
  UINT8         BrightnessNonBFI;   ///< Brightness percentage in non BFI mode
} BFI;

/**
  This defines the structure of Block 45 (Black Frame insertion Support for LFP)
**/
typedef struct {
  UINT8              BlockId;         ///< Defines the unique Block ID : 45
  UINT16             BlockSize;       ///< Defines the size of Black frame insertion support block.
  UINT8              SIZE_BFIStruc;   ///< Defines the size of 1 entry of black frame data.
  BFI                BFI_Struct[16];  ///< Array defining the data of black frame insertion for all 16 panels.
} BLOCK45_BFI_SUPPORT;

/**
  This structure defines the chromaticity information for a single LFP panel.
**/
typedef struct {
  /**
  Defines the chromaticity feature enable bits
  Bits 7:2  = Reserved
  Bit 1     = Override EDID values for chromaticity if enabled, Instead Use VBT values
            = 0, Disable, Use the EDID values
            = 1, Enable, Use the values from the VBT
  Bit 0     = Enable chromaticity feature. EDID values will be used when this feature is enabled.
            = 0, Disable
            = 1, Enable
  **/
  UINT8        EnableBits;

  UINT8        Red_Green_1;   ///< Red/green chormaticity coordinates at EDID offset 19h
  UINT8        Blue_White_1;  ///< Blue/white chromatiity coordinates at EDID offset 1Ah
  UINT8        Red_X1;        ///< Red x coordinate at EDID offset 1Bh
  UINT8        Red_Y1;        ///< Red x coordinate at EDID offset 1Ch
  UINT8        Green_X1;      ///< Green x coordinate at EDID offset 1Dh
  UINT8        Green_Y1;      ///< Green x coordinate at EDID offset 1Eh
  UINT8        Blue_X1;       ///< Blue x coordinate at EDID offset 1Fh
  UINT8        Blue_Y1;       ///< Blue x coordinate at EDID offset 20h
  UINT8        White_X1;      ///< White x coordinate at EDID offset 21h
  UINT8        White_Y1;      ///< White x coordinate at EDID offset 22h
} CHROMATICITY;

/**
  This structure defines the Luminance information for a single LFP panel.
**/
typedef struct {
  /**
  Defines the chromaticity feature enable bits
  Bits 7:2  : Reserved
  Bit 1     : Enable Gamma feature.
            : if enabled, use gamma values from this block.
          0 : Disable
          1 : Enable
  Bit 0     : Enable Luminance feature.
            : if enabled, use values from this block.
          0 : Disable
          1 : Enable
  **/
  UINT8        EnableBits;
  /**
    Luminance info (refer DisplayID 2.0)
    2 byte value, encoded in IEEE 754 half-precision binary floating point format
  **/
  UINT16      MinLuminance;           ///< Native minimum luminance
  UINT16      MaxFullFrameLuminance;  ///< Native maximum luminance (Full Frame)
  UINT16      MaxLuminance;           ///< Native Maximum Luminance (1% Rectangular Coverage)
  /**
    Gamma EOTF
   Gamma values range from 00h through FFh which will come from VBT.
    Value shall define the gamma range, from 1.00 to 3.54.

  Field Value = (Gamma (value from VBT) + 100) / 100

    FFh = No gamma information shall be provided
  **/
  UINT8 Gamma;

}LUMINANCE_AND_GAMMA;
/**
  This defines the structure of Block 46 (Chromaticity Support)
**/
typedef struct {
  UINT8                BlockId;                 ///< Defines the unique Block ID : 46
  UINT16               BlockSize;               ///< Defines the size of Chromaticity Block.
  CHROMATICITY         Chromaticity_Struct[16]; ///< Defines the chromaticity information for all 16 panels.
  LUMINANCE_AND_GAMMA  Luminance_Gamma_Struct[16];    ///< Defines the lumianance information for all 16 panels.
} BLOCK46_CHROMATICITY_SUPPORT;

/**
  This defines the structure of Block 51 (Fixed Mode Set)
**/
typedef struct{
  UINT8       BlockId;        ///< Defines the unique block ID : 51.
  UINT16      BlockSize;      ///< Defines the size of Fixed mode set feature block.
  UINT8       FeatureEnable;  ///< Whether the fixed mode set feature is enabled/disabled.
  UINT32      XRes;           ///< X resolution of the fixed mode.
  UINT32      YRes;           ///< Y resolution of the fixed mode.
} BLOCK51_FIXED_MODE_SET;

//
// MIPI Configuration Block 52
//
typedef struct{
  UINT16  PanelIdentifier;
  union {
  /**
  Bit [31:17]  : Reserved
  Bit[16]  : Disable Video BTA
         0 - Sending BTA at the last blanking line of VFP is enabled
         1 - Sending BTA at the last blanking line of VFP is disabled
  Bit[15:14]  : Panel Rotation
         00 - 0 degree
         01 - 90 degree
         10 - 180 degree
         11 - 270 degree
  Bit[13:10]    : Color Format In Video Mode
            0001 - RGB565
            0010 - RGB666
            0011 - RGB 666(Loosely Packed Format)
            0100 - RGB888
  Bit[9]      : Select the following lines are coming either from PMIC or SoC
                    PANEL_EN, BACKLIGHT_EN, PWM
           0 - From PMIC (default)
           1 - From SoC
  Bit[8]      : CABC Support
           0 - CABC is not supported
           1 - CABC is supported
  Bit[7:6]   : Packet Sequence For Video Mode(Preferred by OEM)
            00 - Reserved
            01 - Non-burst with sync pulse
            10 - Non-burst with sync events
            11 - Burst mode
  Bit[5]      : Video/Command Mode
          0 - Video Mode
          1 - Command Mode
  Bit[4:3]   : MIPI panel architecture type
            00 - Type 1
            01 - Type 2
            10 - Type 3
            11 - Type 4
  Bit[2]      : Panel Type(Hide it for VLV)
            0 - Native MIPI Panel
            1 - MIPI to LVDS Bridge
  Bit[1]      : Reserved
  Bit[0]      : Dithering(Hide it for VLV)
            0 - Dithering is done in Panel Controller
            1 - Dithering is done in Display Controller
  **/
  UINT32  Value;
  struct {
    UINT32 Dithering:1;
    UINT32 Reserved_0:1;
    UINT32 PanelType:1;
    UINT32 MipiPanelArchType:2;
    UINT32 VideoCommandMode:1;
    UINT32 PacketSequenceForVideoMode:2;
    UINT32 CabcSupport:1;
    UINT32 PmicSocSelection:1;
    UINT32 ColourFormatInVideoMode:4;
    UINT32 PanelRotation:2;
    UINT32 BtaDisable:1;
    UINT32 Reserved_1:15;
    } Bits;
  } GeneralMipiParams;

  union {
  /**
  Bit [15:12] : Reserved
  Bit [11:10] : Panel PWM Bklt Controller On/Off commands to be sent to below ports in case of DualLink MIPI panels
            : 00 - MIPI Port-A
            : 01 - MIPI Port-C
            : 02 - Both MIPI PortA and PortC
  Bit [9:8]:  CABC On/Off commands to be sent to below ports in case of DualLink MIPI panels
          : 00 - MIPI Port-A
          : 01 - MIPI Port-C
          : 02 - Both MIPI PortA and PortC
  Bit [7]:  RGB Flip Bit, to indicate RGB/BGR Panel Selection
            0 - RGB type Panel
            1 - BGR type Panel
  Bit [6:4] : Pixel Overlap Count
  Bit [3:2] : Number of Data Lanes
              Value = Lane count -1
  Bit [1:0]  : Dual Link(Hide it for VLV)
          00 - Dual Link not supported
          01 - Dual Link Front Back Mode
          10 - Dual Link Pixel Alternative Mode
          11 - Reserved
  **/
  UINT16  Value;
  struct {
    UINT16 DualLink:2;
    UINT16 NumberOfLanes:2;
    UINT16 PixelOverlapCount:3;
    UINT16 RGBFlip:1;
    UINT16 CabcCmdsPort:2;
    UINT16 PanelPwmCmdsPort:2;
    UINT16 PortSyncSupport:1;
    UINT16 Reserved:3;
  } Bits;
  } PortDesc;

  /**
  Bit[15:1]   : Reserved
  Bit[0]      : TE Usage(Hide it for VLV)
           0 - Using DSI PHY
           1 - Using CMOS Line
  This is for future use, where the params fixed for a platform can be
  mentioned i.e, configuration settings for BandGap, TE Usage and DSI AFE
  **/
  UINT16  DsiControllerParams;

  UINT8   Reserved0;
  UINT32  RequiredBurstModeRate;
  //
  // MIPI DSI DDR Clock in KHz. If 0, driver will calculate the clock from panel timings
  //
  UINT32  DsiDDRClock;
  //
  // Bridge Reference Clock in KHz(Hide it for VLV)
  //
  UINT32  BridgeRefClock;
  /**
  Low power clock equivalence in terms of byte clock.
  Bit[0:1]           : 00: 20 Mhz
                          01: 10 Mhz
                          10: 5 Mhz
                          11: Undefined.
  **/
  UINT8  EscapeClk;

  union {
  /**
  Bit[15:3]  : Reserved
  Bit[2] : To enable or disable clock stopping feature during BLLP
           timing in a DPI transfer in dual channel mode or during DPI only mode and also
           when there is no traffic in the DBI interface in DBI only enabled mode
           0 - clock stopping disabled
           1 - clock stopping enabled
  Bit[1] : Enable or disable EoT short packet transmission
           0 - Enable EoTp Transmission
           1 - Disable EoTp Transmission
  Bit[0] : DphyParamFlag
           0 -Dphy Param Flag is not valid
           1 -Dphy Param Flag is valid (so that we use the hardcoded blocks if needed in future)
  **/
  UINT16  Value;
  struct {
    UINT16 DphyParamFlag:1;
    UINT16 EotpDisabled:1;
    UINT16 ClockStop:1;
    UINT16 BlankingDuringBllp:1;
    UINT16 LpClockDurignLpm:1;
    UINT16 Reserved:11;
  } Bits;
  } DphyFlags;

  UINT32  HsTxTimeOut;
  UINT32  LpRxTimeOut;
  UINT32  TurnAroundTimeOut;
  UINT32  DeviceResetTimer;
  UINT32  MasterInitTimer;
  UINT32  DbiBwTimer;

  UINT32  LpByteClkRegValue;

  union {
    /**
    Bit[31:30]    : Reserved
    Bit[29:24]    : Exit zero count
    Bit[23:21]    : Reserved
    Bit[20:16]    : Trail count
    Bit[15:8]     : Clk zero count
    Bit[7:6]      : Reserved
    Bit[5:0]      : Prepare Count
    **/
    UINT32  Value;
    struct {
      UINT32 PrepareCount:8;
      UINT32 ClkZeroCount:8;
      UINT32 TrailCount:8;
      UINT32 ExitZeroCount:8;
    } Bits;
  } DphyParamReg;

  UINT32  ClkLaneSwitchingTimeCount;
  UINT32  HighLowSwitchCount;

  UINT32  Reserved1;
  UINT32  Reserved2;
  UINT32  Reserved3;
  UINT32  Reserved4;
  UINT32  Reserved5;
  UINT32  Reserved6;

  UINT8   TClkMiss;
  UINT8   TClkPost;
  UINT8   Reserved7;
  UINT8   TClkPre;
  UINT8   TClkPrepare;
  UINT8   TClkSettle;
  UINT8   TClkTermEn;
  UINT8   TClkTrail;
  UINT16  TClkPrepareTClkZero;
  UINT8   Reserved8;
  UINT8   TDTermEn;
  UINT8   TEot;
  UINT8   THsExit;
  UINT8   THsPrepare;
  UINT16  THsPrepareTHsZero;
  UINT8   Reserved9;
  UINT8   THsSettle;
  UINT8   THsSkip;
  UINT8   THsTrail;
  UINT8   TInit;
  UINT8   TLpx;
  UINT8   Reserved10;
  UINT16  Reserved11;

  UINT8 PanelEnable;
  UINT8 BkltEnable;
  UINT8 PWMEnable;
  UINT8 RstRN;
  UINT8 PwrDownR;
  UINT8 StbyRN;
} MIPI_CONF;

//
// MIPI DSI Panel Power On/Off Sequence delays (in 100uS)
//
typedef struct {
  UINT16 MipiDsiPanelPowerUpDelay;
  UINT16 MipiDsiDataTurnOnToPanelBacklightEnableDelay;
  UINT16 MipiDsiPanelBacklightOffToDataTurnOffDelay;
  UINT16 MipiDsiPanelPowerDownDelay;
  UINT16 MipiDsiPanelPowerCycleDelay;
} MIPI_PPS;

typedef struct {
  UINT16 PwmOnToBacklightEnableDelay;
  UINT16 BacklightDisableToPwmOffDelay;
} MIPI_PWM_BACKLIGHT_DELAYS;

typedef struct {
  UINT8                     BlockId;
  UINT16                    BlockSize;
  MIPI_CONF                 MipiConf[6];
  MIPI_PPS                  MipiPps[6];
  MIPI_PWM_BACKLIGHT_DELAYS MipiPwmBackLightDelays[6];
  UINT8                     PmicI2cBusNo[6];
} BLOCK52_MIPI_CONF;

/**
  This defines the structure of Block 53 (MIPI Sequence block) for panel initialisation.
**/
typedef struct {
  UINT8      BlockId;         ///< Defines the unique Block ID: 53
  UINT16     BlockSize;       ///< Defines the size of MIPI sequence Block
  UINT8      Version;         ///< Defines the version of MIPI sequence.
  UINT8*     MipiSequence;    ///< Pointer to the actual MIPI sequence structure.
} BLOCK53_MIPI_SEQ;

/**
  This defines the structure of a single RGB Palette entry block
  for programming Gamma, Brightness, Contrast.
**/
typedef struct {
  UINT8     Red[256];         ///< Red component.
  UINT8     Green[256];       ///< Green component.
  UINT8     Blue[256];        ///< Blue component.
} RGB_PALETTE_TABLE_ENTRY;

/**
  This defines the structure of Block 54 (RGB Palette Table)
**/
typedef struct {
  UINT8                   BlockId;        ///< Defines the unique Block ID: 54
  UINT16                  BlockSize;      ///< Defines the size of RGB Palette Table block.
  UINT8                   IsEnabled;      ///< Feature enable flag.
  RGB_PALETTE_TABLE_ENTRY PaletteTable;   ///< Structure of RGB palette entries.
} BLOCK54_RGB_PALETTE_TABLE;

/**
  This defines a single structure of compression parameters.
  For each compression engine, we need to have one structure.
**/
typedef struct {
  /**
  DSC Version field
  Bits 7 - 4 : Minor Version
  Bits 3 - 0 : Major Version
  **/
  union {
    UINT8 Value;
    struct {
      UINT8   MajorVersion : 4;
      UINT8   MinorVersion : 4;
    } Bits;
  } DscVersion;

  /**
  Bits 7 - 2 : Reserved
  Bits 1 - 0 : RC Buffer block size
          00 : 1KB
          01 : 4KB
          10 : 16KB
          11 : 64KB
  **/
  union {
    UINT8 Value;
    struct {
      UINT8   RcBlockSize : 2;
      UINT8   Reserved : 6;
    } Bits;
  } DscBufBlkSize;

  /**
  Each rate buffer size in units of blocks.
  Buffer size (in blocks) = value + 1
  **/
  UINT8    DscRcBufSize;

  /**
  DSC Slices per line.
  For end user this is a bitmap selection field.
  Each bit represents whether the specific slices per line is supported or not.
  Bit = 0 means not supported and bit = 1 means supported

  Bit   Number of slices per line
  0   = 1 slices per line
  1   = 2 slices per line
  2   = 4 slices per line
  3   = 6 slices per line
  4   = 8 slices per line
  5   = 10 slices per line
  6   = 12 slices per line
  7   = 16 slices per line
  8   = 20 slices per line
  9   = 24 slices per line
  **/
  UINT32    DscSlicesPerLine;

  /**
  This field specifies the line buffer bit depth used by the sink device
  to store the reconstructed pixels within the frame buffer.
  Bits 7 - 4 : Reserved
  Bits 3 - 0 : Line buffer depth
           0 = 8 bits
           1 = 9 bits
           2 = 10 bits
           3 = 11 bits
           4 = 12 bits
           5 = 13 bits
           6 = 14 bits
           7 = 15 bits
           8 = 16 bits
  **/
  union {
    UINT8 Value;
    struct {
      UINT8   LineBufferDepth:4;
      UINT8   Reserved:4;
    } Bits;
  } DscLineBufDepth;

  /**
  DSC Flag_Bits_1
  Contains various flag bits.
  Bits 7 - 1 : Reserved
  Bit  0     : Block prediction enable
           0 : BP is not used
           1 : Decoder must select between BP and MMAP
  **/
  union {
    UINT8 Value;
    struct {
      UINT8   BlkPredictionEn:1;
      UINT8   Reserved:7;
    } Bits;
  } DscFlagBits1;

  /**
  DSC Maximum Bits per pixel.
  This field specifies the maximum bits per pixel
  that the sink device can support for compressed output.
  Value         BPP
     0        = 6BPP
     1        = 8BPP
     2        = 10BPP
     3        = 12BPP
  **/
  UINT8    DscMaxBpp;

  /**
  DSC Color depth capabilities.
  This field specifies the color depths supported by the sink.
  Bits 7-4 : Reserved
  Bit    3 : 12 Bits per Color supported
         0 = 12 bpc is not supported
         1 = 12 bpc is supported
  Bit    2 : 10 Bits per Color supported
         0 = 10 bpc is not supported
         1 = 10 bpc is supported
  Bit    1 : 8  Bits per Color supported
         0 = 8  bpc is not supported
         1 = 8 bpc is supported
  **/

  union {
    UINT8 Value;
    struct {
      UINT8   Reserved:1;
      UINT8   BpcSupported8:1;
      UINT8   BpcSupported10:1;
      UINT8   BpcSupported12:1;
      UINT8   Reserved1:4;
    } Bits;
  } DscColorDepthCap;


  /**
  DSC Slice Height
  This field specifies the slice height supported by the sink side.
  **/
  UINT16    DscSliceHeight;
} COMPRESSION_PARAMETERS_STRUCT;

/**
  This defines the structure of Block 56 (Compression parameters)
**/
typedef struct {
  UINT8                           BlockId;                  ///< Defines the unique Block ID: 56
  UINT16                          BlockSize;                ///< Defines the size of Compression Block.
  UINT16                          EntrySize;                ///< Size of single compression parameter structure.
  COMPRESSION_PARAMETERS_STRUCT   CompressionParameters[CompressionStruct_MAX]; ///< Array defining the compression parameters.
} BLOCK56_COMPRESSION_PARAMETERS;

/**
  This defines the Complete VBT Structure for generation purpose
**/
typedef struct {
  VBT_HEADER                                VbtHeader;
  VBT_BIOS_DATA_HEADER                      VbtBdbHeader;
  BLOCK254_BMP_Structure                    Block254BMPStructure;
  VBT_GENERAL1_INFO                         VbtGen1Info;
  PRD_BOOT_TABLE                            PrdBootTable;
  VBT_GENERAL2_INFO                         VbtGen2Info;
  BLOCK252_SBIOS_Hook                       Block252SbiosHook;
  BLOCK09_PSR_FEATURE                       Block09PsrFeature;
  BLOCK10_MODE_REMOVAL_TABLE                Block10ModeRemovalTable;
  BLOCK12_DRIVER_FEATURES                   Block12DriverFeatures;
  BLOCK20_OEM_CUSTOMIZATION                 Block20OemCustomization;
  BLOCK27_EDP_FEATURES                      Block27EDPFeatures;
  BLOCK28_EDIDLESS_EFP                      Block28EdidlessEFP;
  BLOCK40_LVDS_FEATURES                     Block40LVDSFeatures;
  BLOCK41_LFP_TABLE_POINTERS                Block41LfpTablePointers;
  BLOCK42_LVDS_PANEL_INFO                   Block42LvdsPanelInfo;
  BLOCK43_LVDS_BLC                          Block43LVDSBlc;
  BLOCK44_ALS                               Block44Als;
  BLOCK46_CHROMATICITY_SUPPORT              Block46ChromaticitySupport;
  BLOCK51_FIXED_MODE_SET                    Block51FixedModeSet;
  BLOCK52_MIPI_CONF                         Block52MipiConf;
  BLOCK56_COMPRESSION_PARAMETERS            Block56CompressionParameters;
} VBT_TABLE_DATA;

#pragma pack()

#endif
