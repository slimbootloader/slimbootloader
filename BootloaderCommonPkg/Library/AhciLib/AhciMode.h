/** @file
  Header file for AHCI mode of ATA host controller.

  Copyright (c) 2010 - 2017, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#ifndef __ATA_HC_AHCI_MODE_H__
#define __ATA_HC_AHCI_MODE_H__

#define EFI_AHCI_BAR_OFFSET                    0x24

#define EFI_AHCI_CAPABILITY_OFFSET             0x0000
#define   EFI_AHCI_CAP_SAM                     BIT18
#define   EFI_AHCI_CAP_SSS                     BIT27
#define   EFI_AHCI_CAP_S64A                    BIT31
#define EFI_AHCI_GHC_OFFSET                    0x0004
#define   EFI_AHCI_GHC_RESET                   BIT0
#define   EFI_AHCI_GHC_IE                      BIT1
#define   EFI_AHCI_GHC_ENABLE                  BIT31
#define EFI_AHCI_IS_OFFSET                     0x0008
#define EFI_AHCI_PI_OFFSET                     0x000C

#define EFI_AHCI_MAX_PORTS                     32

typedef struct {
  UINT32  Lower32;
  UINT32  Upper32;
} DATA_32;

typedef union {
  DATA_32   Uint32;
  UINT64    Uint64;
} DATA_64;

typedef enum {
  EfiIdeCdrom,                  /* ATAPI CDROM */
  EfiIdeHarddisk,               /* Hard Disk */
  EfiPortMultiplier,            /* Port Multiplier */
  EfiIdeUnknown
} EFI_ATA_DEVICE_TYPE;

//
// Refer SATA1.0a spec section 5.2, the Phy detection time should be less than 10ms.
// Add a bit of margin for robustness.
//
#define  EFI_AHCI_BUS_PHY_DETECT_TIMEOUT       15
//
// Refer SATA1.0a spec, the FIS enable time should be less than 500ms.
//
#define  EFI_AHCI_PORT_CMD_FR_CLEAR_TIMEOUT    EFI_TIMER_PERIOD_MILLISECONDS(500)
//
// Refer SATA1.0a spec, the bus reset time should be less than 1s.
//
#define  EFI_AHCI_BUS_RESET_TIMEOUT            EFI_TIMER_PERIOD_SECONDS(1)

#define  EFI_AHCI_ATAPI_DEVICE_SIG             0xEB140000
#define  EFI_AHCI_ATA_DEVICE_SIG               0x00000000
#define  EFI_AHCI_PORT_MULTIPLIER_SIG          0x96690000
#define  EFI_AHCI_ATAPI_SIG_MASK               0xFFFF0000

//
// Each PRDT entry can point to a memory block up to 4M byte
//
#define EFI_AHCI_MAX_DATA_PER_PRDT             0x400000

#define EFI_AHCI_FIS_REGISTER_H2D              0x27      //Register FIS - Host to Device
#define   EFI_AHCI_FIS_REGISTER_H2D_LENGTH     20
#define EFI_AHCI_FIS_REGISTER_D2H              0x34      //Register FIS - Device to Host
#define   EFI_AHCI_FIS_REGISTER_D2H_LENGTH     20
#define EFI_AHCI_FIS_DMA_ACTIVATE              0x39      //DMA Activate FIS - Device to Host
#define   EFI_AHCI_FIS_DMA_ACTIVATE_LENGTH     4
#define EFI_AHCI_FIS_DMA_SETUP                 0x41      //DMA Setup FIS - Bi-directional
#define   EFI_AHCI_FIS_DMA_SETUP_LENGTH        28
#define EFI_AHCI_FIS_DATA                      0x46      //Data FIS - Bi-directional
#define EFI_AHCI_FIS_BIST                      0x58      //BIST Activate FIS - Bi-directional
#define   EFI_AHCI_FIS_BIST_LENGTH             12
#define EFI_AHCI_FIS_PIO_SETUP                 0x5F      //PIO Setup FIS - Device to Host
#define   EFI_AHCI_FIS_PIO_SETUP_LENGTH        20
#define EFI_AHCI_FIS_SET_DEVICE                0xA1      //Set Device Bits FIS - Device to Host
#define   EFI_AHCI_FIS_SET_DEVICE_LENGTH       8

#define EFI_AHCI_D2H_FIS_OFFSET                0x40
#define EFI_AHCI_DMA_FIS_OFFSET                0x00
#define EFI_AHCI_PIO_FIS_OFFSET                0x20
#define EFI_AHCI_SDB_FIS_OFFSET                0x58
#define EFI_AHCI_FIS_TYPE_MASK                 0xFF
#define EFI_AHCI_U_FIS_OFFSET                  0x60

//
// Port register
//
#define EFI_AHCI_PORT_START                    0x0100
#define EFI_AHCI_PORT_REG_WIDTH                0x0080
#define EFI_AHCI_PORT_CLB                      0x0000
#define EFI_AHCI_PORT_CLBU                     0x0004
#define EFI_AHCI_PORT_FB                       0x0008
#define EFI_AHCI_PORT_FBU                      0x000C
#define EFI_AHCI_PORT_IS                       0x0010
#define   EFI_AHCI_PORT_IS_DHRS                BIT0
#define   EFI_AHCI_PORT_IS_PSS                 BIT1
#define   EFI_AHCI_PORT_IS_SSS                 BIT2
#define   EFI_AHCI_PORT_IS_SDBS                BIT3
#define   EFI_AHCI_PORT_IS_UFS                 BIT4
#define   EFI_AHCI_PORT_IS_DPS                 BIT5
#define   EFI_AHCI_PORT_IS_PCS                 BIT6
#define   EFI_AHCI_PORT_IS_DIS                 BIT7
#define   EFI_AHCI_PORT_IS_PRCS                BIT22
#define   EFI_AHCI_PORT_IS_IPMS                BIT23
#define   EFI_AHCI_PORT_IS_OFS                 BIT24
#define   EFI_AHCI_PORT_IS_INFS                BIT26
#define   EFI_AHCI_PORT_IS_IFS                 BIT27
#define   EFI_AHCI_PORT_IS_HBDS                BIT28
#define   EFI_AHCI_PORT_IS_HBFS                BIT29
#define   EFI_AHCI_PORT_IS_TFES                BIT30
#define   EFI_AHCI_PORT_IS_CPDS                BIT31
#define   EFI_AHCI_PORT_IS_CLEAR               0xFFFFFFFF
#define   EFI_AHCI_PORT_IS_FIS_CLEAR           0x0000000F

#define EFI_AHCI_PORT_IE                       0x0014
#define EFI_AHCI_PORT_CMD                      0x0018
#define   EFI_AHCI_PORT_CMD_ST_MASK            0xFFFFFFFE
#define   EFI_AHCI_PORT_CMD_ST                 BIT0
#define   EFI_AHCI_PORT_CMD_SUD                BIT1
#define   EFI_AHCI_PORT_CMD_POD                BIT2
#define   EFI_AHCI_PORT_CMD_CLO                BIT3
#define   EFI_AHCI_PORT_CMD_CR                 BIT15
#define   EFI_AHCI_PORT_CMD_FRE                BIT4
#define   EFI_AHCI_PORT_CMD_FR                 BIT14
#define   EFI_AHCI_PORT_CMD_MASK               ~(EFI_AHCI_PORT_CMD_ST | EFI_AHCI_PORT_CMD_FRE | EFI_AHCI_PORT_CMD_COL)
#define   EFI_AHCI_PORT_CMD_PMA                BIT17
#define   EFI_AHCI_PORT_CMD_HPCP               BIT18
#define   EFI_AHCI_PORT_CMD_MPSP               BIT19
#define   EFI_AHCI_PORT_CMD_CPD                BIT20
#define   EFI_AHCI_PORT_CMD_ESP                BIT21
#define   EFI_AHCI_PORT_CMD_ATAPI              BIT24
#define   EFI_AHCI_PORT_CMD_DLAE               BIT25
#define   EFI_AHCI_PORT_CMD_ALPE               BIT26
#define   EFI_AHCI_PORT_CMD_ASP                BIT27
#define   EFI_AHCI_PORT_CMD_ICC_MASK           (BIT28 | BIT29 | BIT30 | BIT31)
#define   EFI_AHCI_PORT_CMD_ACTIVE             (1 << 28 )
#define EFI_AHCI_PORT_TFD                      0x0020
#define   EFI_AHCI_PORT_TFD_MASK               (BIT7 | BIT3 | BIT0)
#define   EFI_AHCI_PORT_TFD_BSY                BIT7
#define   EFI_AHCI_PORT_TFD_DRQ                BIT3
#define   EFI_AHCI_PORT_TFD_ERR                BIT0
#define   EFI_AHCI_PORT_TFD_ERR_MASK           0x00FF00
#define EFI_AHCI_PORT_SIG                      0x0024
#define EFI_AHCI_PORT_SSTS                     0x0028
#define   EFI_AHCI_PORT_SSTS_DET_MASK          0x000F
#define   EFI_AHCI_PORT_SSTS_DET               0x0001
#define   EFI_AHCI_PORT_SSTS_DET_PCE           0x0003
#define   EFI_AHCI_PORT_SSTS_SPD_MASK          0x00F0
#define EFI_AHCI_PORT_SCTL                     0x002C
#define   EFI_AHCI_PORT_SCTL_DET_MASK          0x000F
#define   EFI_AHCI_PORT_SCTL_MASK              (~EFI_AHCI_PORT_SCTL_DET_MASK)
#define   EFI_AHCI_PORT_SCTL_DET_INIT          0x0001
#define   EFI_AHCI_PORT_SCTL_DET_PHYCOMM       0x0003
#define   EFI_AHCI_PORT_SCTL_SPD_MASK          0x00F0
#define   EFI_AHCI_PORT_SCTL_IPM_MASK          0x0F00
#define   EFI_AHCI_PORT_SCTL_IPM_INIT          0x0300
#define   EFI_AHCI_PORT_SCTL_IPM_PSD           0x0100
#define   EFI_AHCI_PORT_SCTL_IPM_SSD           0x0200
#define EFI_AHCI_PORT_SERR                     0x0030
#define   EFI_AHCI_PORT_SERR_RDIE              BIT0
#define   EFI_AHCI_PORT_SERR_RCE               BIT1
#define   EFI_AHCI_PORT_SERR_TDIE              BIT8
#define   EFI_AHCI_PORT_SERR_PCDIE             BIT9
#define   EFI_AHCI_PORT_SERR_PE                BIT10
#define   EFI_AHCI_PORT_SERR_IE                BIT11
#define   EFI_AHCI_PORT_SERR_PRC               BIT16
#define   EFI_AHCI_PORT_SERR_PIE               BIT17
#define   EFI_AHCI_PORT_SERR_CW                BIT18
#define   EFI_AHCI_PORT_SERR_BDE               BIT19
#define   EFI_AHCI_PORT_SERR_DE                BIT20
#define   EFI_AHCI_PORT_SERR_CRCE              BIT21
#define   EFI_AHCI_PORT_SERR_HE                BIT22
#define   EFI_AHCI_PORT_SERR_LSE               BIT23
#define   EFI_AHCI_PORT_SERR_TSTE              BIT24
#define   EFI_AHCI_PORT_SERR_UFT               BIT25
#define   EFI_AHCI_PORT_SERR_EX                BIT26
#define   EFI_AHCI_PORT_ERR_CLEAR              0xFFFFFFFF
#define EFI_AHCI_PORT_SACT                     0x0034
#define EFI_AHCI_PORT_CI                       0x0038
#define EFI_AHCI_PORT_SNTF                     0x003C



/**
  Macro that returns the number of 100 ns units for a specified number of microseconds.
  This is useful for managing EFI timer events.

  @param  Microseconds           The number of microseconds.

  @return The number of 100 ns units equivalent to the number of microseconds specified
          by Microseconds.

**/
#define  EFI_TIMER_PERIOD_MICROSECONDS(Microseconds) MultU64x32((UINT64)(Microseconds), 10)

/**
  Macro that returns the number of 100 ns units for a specified number of milliseconds.
  This is useful for managing EFI timer events.

  @param  Milliseconds           The number of milliseconds.

  @return The number of 100 ns units equivalent to the number of milliseconds specified
          by Milliseconds.

**/
#define  EFI_TIMER_PERIOD_MILLISECONDS(Milliseconds) MultU64x32((UINT64)(Milliseconds), 10000)

/**
  Macro that returns the number of 100 ns units for a specified number of seconds.
  This is useful for managing EFI timer events.

  @param  Seconds                The number of seconds.

  @return The number of 100 ns units equivalent to the number of seconds specified
          by Seconds.

**/
#define  EFI_TIMER_PERIOD_SECONDS(Seconds)           MultU64x32((UINT64)(Seconds), 10000000)


#define  ATA_ATAPI_TIMEOUT                     EFI_TIMER_PERIOD_SECONDS(3)


#define ATA_ID_WORD_88_VALID                        BIT2
#define LBA_48_BIT_ADDRESS_FEATURE_SET_SUPPORTED    BIT10

//
//*******************************************************
// EFI_ATA_IDENTIFY_DATA
//*******************************************************
//

#pragma pack(1)

typedef struct {
  UINT16  Config;             // General Configuration
  UINT16  Cylinders;          // Number of Cylinders
  UINT16  Reserved_2;
  UINT16  Heads;              //Number of logical heads
  UINT16  Vendor_data1;
  UINT16  Vendor_data2;
  UINT16  Sectors_per_track;
  UINT16  Vendor_specific_7_9[3];
  CHAR8   SerialNo[20];       // ASCII
  UINT16  Vendor_specific_20_21[2];
  UINT16  Ecc_bytes_available;
  CHAR8   FirmwareVer[8];     // ASCII
  CHAR8   ModelName[40];      // ASCII
  UINT16  Multi_sector_cmd_max_sct_cnt;
  UINT16  Reserved_48;
  UINT16  Capabilities;
  UINT16  Reserved_50;
  UINT16  Pio_cycle_timing;
  UINT16  Reserved_52;
  UINT16  Field_validity;
  UINT16  Current_cylinders;
  UINT16  Current_heads;
  UINT16  Current_sectors;
  UINT16  CurrentCapacityLsb;
  UINT16  CurrentCapacityMsb;
  UINT16  Reserved_59;
  UINT16  User_addressable_sectors_lo;
  UINT16  User_addressable_sectors_hi;
  UINT16  Reserved_62;
  UINT16  Multi_word_dma_mode;
  UINT16  Advanced_pio_modes;
  UINT16  Min_multi_word_dma_cycle_time;
  UINT16  Rec_multi_word_dma_cycle_time;
  UINT16  Min_pio_cycle_time_without_flow_control;
  UINT16  Min_pio_cycle_time_with_flow_control;
  UINT16  Reserved_69_79[11];
  UINT16  Major_version_no;
  UINT16  Minor_version_no;
  UINT16  Command_set_supported_82; // word 82
  UINT16  Command_set_supported_83; // word 83
  UINT16  Command_set_feature_extn; // word 84
  UINT16  Command_set_feature_enb_85; // word 85
  UINT16  Command_set_feature_enb_86; // word 86
  UINT16  Command_set_feature_default; // word 87
  UINT16  Ultra_dma_mode; // word 88
  UINT16  Reserved_89_105[17];
  UINT16  Phy_logic_sector_support; // word 106
  UINT16  Reserved_107_116[10];
  UINT16  Logic_sector_size_lo; // word 117
  UINT16  Logic_sector_size_hi; // word 118
  UINT16  Reserved_119_127[9];
  UINT16  Security_status;
  UINT16  Vendor_data_129_159[31];
  UINT16  Reserved_160_208[49];
  UINT16  Alignment_logic_in_phy_blocks; // word 209
  UINT16  Reserved_210_255[46];
} EFI_ATA_IDENTIFY_DATA;

//
//*******************************************************
// EFI_ATAPI_IDENTIFY_DATA
//*******************************************************
//
typedef struct {
  UINT16  Config;             // General Configuration
  UINT16  Obsolete_1;
  UINT16  SpecificConfig;
  UINT16  Obsolete_3;
  UINT16  Retired_4_5[2];
  UINT16  Obsolete_6;
  UINT16  CfaReserved_7_8[2];
  UINT16  Retired_9;
  CHAR8   SerialNo[20];       // ASCII
  UINT16  Retired_20_21[2];
  UINT16  Obsolete_22;
  CHAR8   FirmwareVer[8];     // ASCII
  CHAR8   ModelName[40];      // ASCII
  UINT16  Multi_sector_cmd_max_sct_cnt;
  UINT16  Reserved_48;
  UINT16  Capabilities_49;
  UINT16  Capabilities_50;
  UINT16  Obsolete_51_52[2];
  UINT16  Field_validity;
  UINT16  Obsolete_54_58[5];
  UINT16  Mutil_sector_setting;
  UINT16  User_addressable_sectors_lo;
  UINT16  User_addressable_sectors_hi;
  UINT16  Obsolete_62;
  UINT16  Multi_word_dma_mode;
  UINT16  Advanced_pio_modes;
  UINT16  Min_multi_word_dma_cycle_time;
  UINT16  Rec_multi_word_dma_cycle_time;
  UINT16  Min_pio_cycle_time_without_flow_control;
  UINT16  Min_pio_cycle_time_with_flow_control;
  UINT16  Reserved_69_74[6];
  UINT16  Queue_depth;
  UINT16  Reserved_76_79[4];
  UINT16  Major_version_no;
  UINT16  Minor_version_no;
  UINT16  Cmd_set_support_82;
  UINT16  Cmd_set_support_83;
  UINT16  Cmd_feature_support;
  UINT16  Cmd_feature_enable_85;
  UINT16  Cmd_feature_enable_86;
  UINT16  Cmd_feature_default;
  UINT16  Ultra_dma_select;
  UINT16  Time_required_for_sec_erase;
  UINT16  Time_required_for_enhanced_sec_erase;
  UINT16  Current_advanced_power_mgmt_value;
  UINT16  Master_pwd_revison_code;
  UINT16  Hardware_reset_result;
  UINT16  Current_auto_acoustic_mgmt_value;
  UINT16  Reserved_95_99[5];
  UINT16  Max_user_lba_for_48bit_addr[4];
  UINT16  Reserved_104_126[23];
  UINT16  Removable_media_status_notification_support;
  UINT16  Security_status;
  UINT16  Vendor_data_129_159[31];
  UINT16  Cfa_power_mode;
  UINT16  Cfa_reserved_161_175[15];
  UINT16  Current_media_serial_no[30];
  UINT16  Reserved_206_254[49];
  UINT16  Integrity_word;
} EFI_ATAPI_IDENTIFY_DATA;

typedef struct _EFI_ATA_COMMAND_BLOCK {
  UINT8 Reserved1[2];
  UINT8 AtaCommand;
  UINT8 AtaFeatures;
  UINT8 AtaSectorNumber;
  UINT8 AtaCylinderLow;
  UINT8 AtaCylinderHigh;
  UINT8 AtaDeviceHead;
  UINT8 AtaSectorNumberExp;
  UINT8 AtaCylinderLowExp;
  UINT8 AtaCylinderHighExp;
  UINT8 AtaFeaturesExp;
  UINT8 AtaSectorCount;
  UINT8 AtaSectorCountExp;
  UINT8 Reserved2[6];
} EFI_ATA_COMMAND_BLOCK;

typedef struct _EFI_ATA_STATUS_BLOCK {
  UINT8 Reserved1[2];
  UINT8 AtaStatus;
  UINT8 AtaError;
  UINT8 AtaSectorNumber;
  UINT8 AtaCylinderLow;
  UINT8 AtaCylinderHigh;
  UINT8 AtaDeviceHead;
  UINT8 AtaSectorNumberExp;
  UINT8 AtaCylinderLowExp;
  UINT8 AtaCylinderHighExp;
  UINT8 Reserved2;
  UINT8 AtaSectorCount;
  UINT8 AtaSectorCountExp;
  UINT8 Reserved3[6];
} EFI_ATA_STATUS_BLOCK;

//
// Command List structure includes total 32 entries.
// The entry data structure is listed at the following.
//
typedef struct {
  UINT32   AhciCmdCfl: 5;     //Command FIS Length
  UINT32   AhciCmdA: 1;       //ATAPI
  UINT32   AhciCmdW: 1;       //Write
  UINT32   AhciCmdP: 1;       //Prefetchable
  UINT32   AhciCmdR: 1;       //Reset
  UINT32   AhciCmdB: 1;       //BIST
  UINT32   AhciCmdC: 1;       //Clear Busy upon R_OK
  UINT32   AhciCmdRsvd: 1;
  UINT32   AhciCmdPmp: 4;     //Port Multiplier Port
  UINT32   AhciCmdPrdtl: 16;  //Physical Region Descriptor Table Length
  UINT32   AhciCmdPrdbc;      //Physical Region Descriptor Byte Count
  UINT32   AhciCmdCtba;       //Command Table Descriptor Base Address
  UINT32   AhciCmdCtbau;      //Command Table Descriptor Base Address Upper 32-BITs
  UINT32   AhciCmdRsvd1[4];
} EFI_AHCI_COMMAND_LIST;

//
// This is a software constructed FIS.
// For data transfer operations, this is the H2D Register FIS format as
// specified in the Serial ATA Revision 2.6 specification.
//
typedef struct {
  UINT8    AhciCFisType;
  UINT8    AhciCFisPmNum: 4;
  UINT8    AhciCFisRsvd: 1;
  UINT8    AhciCFisRsvd1: 1;
  UINT8    AhciCFisRsvd2: 1;
  UINT8    AhciCFisCmdInd: 1;
  UINT8    AhciCFisCmd;
  UINT8    AhciCFisFeature;
  UINT8    AhciCFisSecNum;
  UINT8    AhciCFisClyLow;
  UINT8    AhciCFisClyHigh;
  UINT8    AhciCFisDevHead;
  UINT8    AhciCFisSecNumExp;
  UINT8    AhciCFisClyLowExp;
  UINT8    AhciCFisClyHighExp;
  UINT8    AhciCFisFeatureExp;
  UINT8    AhciCFisSecCount;
  UINT8    AhciCFisSecCountExp;
  UINT8    AhciCFisRsvd3;
  UINT8    AhciCFisControl;
  UINT8    AhciCFisRsvd4[4];
  UINT8    AhciCFisRsvd5[44];
} EFI_AHCI_COMMAND_FIS;

//
// ACMD: ATAPI command (12 or 16 bytes)
//
typedef struct {
  UINT8    AtapiCmd[0x10];
} EFI_AHCI_ATAPI_COMMAND;

//
// Physical Region Descriptor Table includes up to 65535 entries
// The entry data structure is listed at the following.
// the actual entry number comes from the PRDTL field in the command
// list entry for this command slot.
//
typedef struct {
  UINT32   AhciPrdtDba;       //Data Base Address
  UINT32   AhciPrdtDbau;      //Data Base Address Upper 32-BITs
  UINT32   AhciPrdtRsvd;
  UINT32   AhciPrdtDbc: 22;   //Data Byte Count
  UINT32   AhciPrdtRsvd1: 9;
  UINT32   AhciPrdtIoc: 1;    //Interrupt on Completion
} EFI_AHCI_COMMAND_PRDT;

//
// Command table data strucute which is pointed to by the entry in the command list
//
typedef struct {
  EFI_AHCI_COMMAND_FIS      CommandFis;       // A software constructed FIS.
  EFI_AHCI_ATAPI_COMMAND    AtapiCmd;         // 12 or 16 bytes ATAPI cmd.
  UINT8                     Reserved[0x30];
  EFI_AHCI_COMMAND_PRDT     PrdtTable[65535];     // The scatter/gather list for data transfer
} EFI_AHCI_COMMAND_TABLE;

//
// Received FIS structure
//
typedef struct {
  UINT8    AhciDmaSetupFis[0x1C];         // Dma Setup Fis: offset 0x00
  UINT8    AhciDmaSetupFisRsvd[0x04];
  UINT8    AhciPioSetupFis[0x14];         // Pio Setup Fis: offset 0x20
  UINT8    AhciPioSetupFisRsvd[0x0C];
  UINT8    AhciD2HRegisterFis[0x14];      // D2H Register Fis: offset 0x40
  UINT8    AhciD2HRegisterFisRsvd[0x04];
  UINT64   AhciSetDeviceBitsFis;          // Set Device Bits Fix: offset 0x58
  UINT8    AhciUnknownFis[0x40];          // Unkonwn Fis: offset 0x60
  UINT8    AhciUnknownFisRsvd[0x60];
} EFI_AHCI_RECEIVED_FIS;

#pragma pack()

//
//*******************************************************
// EFI_IDENTIFY_DATA
//*******************************************************
//
typedef union {
  EFI_ATA_IDENTIFY_DATA       AtaData;
  EFI_ATAPI_IDENTIFY_DATA     AtapiData;
} EFI_IDENTIFY_DATA;

typedef struct {
  EFI_AHCI_RECEIVED_FIS     *AhciRFis;
  VOID                      *AhciRFisMap;
  EFI_AHCI_COMMAND_LIST     *AhciCmdList;
  VOID                      *AhciCmdListMap;
  EFI_AHCI_COMMAND_TABLE    *AhciCommandTable;
  VOID                      *AhciCommandTableMap;
  EFI_AHCI_RECEIVED_FIS     *AhciRFisPciAddr;
  EFI_AHCI_COMMAND_LIST     *AhciCmdListPciAddr;
  EFI_AHCI_COMMAND_TABLE    *AhciCommandTablePciAddr;
  UINT32                    MaxCommandListSize;
  UINT32                    MaxCommandTableSize;
  UINT32                    MaxReceiveFisSize;
} EFI_AHCI_REGISTERS;

typedef struct {
  UINT32                    Signature;
  UINT32                    AhciPciCfgAddr;
  UINT32                    AhciMemAddr;
  EFI_AHCI_REGISTERS        AhciRegisters;
  LIST_ENTRY                DeviceList;
} EFI_AHCI_CONTROLLER;

/**
  Start command for give slot on specific port.

  @param  AhciController     The AHCI controller instance.
  @param  Port               The number of port.
  @param  CommandSlot        The number of CommandSlot.
  @param  Timeout            The timeout value of start, uses 100ns as a unit.

  @retval EFI_DEVICE_ERROR   The command start unsuccessfully.
  @retval EFI_TIMEOUT        The operation is time out.
  @retval EFI_SUCCESS        The command start successfully.

**/
EFI_STATUS
EFIAPI
AhciStartCommand (
  IN  EFI_AHCI_CONTROLLER      *AhciController,
  IN  UINT8                     Port,
  IN  UINT8                     CommandSlot,
  IN  UINT64                    Timeout
  );

/**
  Stop command running for giving port

  @param  AhciController     The AHCI controller instance.
  @param  Port               The number of port.
  @param  Timeout            The timeout value of stop, uses 100ns as a unit.

  @retval EFI_DEVICE_ERROR   The command stop unsuccessfully.
  @retval EFI_TIMEOUT        The operation is time out.
  @retval EFI_SUCCESS        The command stop successfully.

**/
EFI_STATUS
EFIAPI
AhciStopCommand (
  IN  EFI_AHCI_CONTROLLER      *AhciController,
  IN  UINT8                     Port,
  IN  UINT64                    Timeout
  );


/**
  Initialize ATA host controller at AHCI mode.

  The function is designed to initialize ATA host controller.

  @param[in]  AhciController          A pointer to the AHCI controller instance.

**/
EFI_STATUS
EFIAPI
AhciModeInitialization (
  IN     EFI_AHCI_CONTROLLER    *AhciController
  );


/**
  Allocate device info data structure to contain device info.
  And insert the data structure to the tail of device list for tracing.

  @param[in]  AhciDeviceData      The AHCI controller instance.
  @param[in]  Port                The port number of the ATA device to send the command.
  @param[in]  PortMultiplierPort  The port multiplier port number of the ATA device to send the command.
                                  If there is no port multiplier, then specify 0xFFFF.
  @param[in]  DeviceType          The device type of the ATA device.
  @param[in]  IdentifyData        The data buffer to store the output of the IDENTIFY cmd.

  @retval EFI_SUCCESS             Successfully insert the ata device to the tail of device list.
  @retval EFI_OUT_OF_RESOURCES    Can not allocate enough resource for use.

**/
EFI_STATUS
EFIAPI
CreateNewDeviceInfo (
  IN  EFI_AHCI_CONTROLLER           *AhciDeviceData,
  IN  UINT16                         Port,
  IN  UINT16                         PortMultiplier,
  IN  EFI_ATA_DEVICE_TYPE            DeviceType,
  IN  EFI_IDENTIFY_DATA              *IdentifyData
  );


/**
  Start a DMA data transfer on specific port

  @param[in]       AhciController      The AHCI controller instance.
  @param[in]       AhciRegisters       The pointer to the EFI_AHCI_REGISTERS.
  @param[in]       Port                The number of port.
  @param[in]       PortMultiplier      The timeout value of stop.
  @param[in]       AtapiCommand        The atapi command will be used for the
                                       transfer.
  @param[in]       AtapiCommandLength  The length of the atapi command.
  @param[in]       Read                The transfer direction.
  @param[in]       AtaCommandBlock     The EFI_ATA_COMMAND_BLOCK data.
  @param[in, out]  AtaStatusBlock      The EFI_ATA_STATUS_BLOCK data.
  @param[in, out]  MemoryAddr          The pointer to the data buffer.
  @param[in]       DataCount           The data count to be transferred.
  @param[in]       Timeout             The timeout value of non data transfer, uses 100ns as a unit.

  @retval EFI_DEVICE_ERROR    The DMA data transfer abort with error occurs.
  @retval EFI_TIMEOUT         The operation is time out.
  @retval EFI_UNSUPPORTED     The device is not ready for transfer.
  @retval EFI_SUCCESS         The DMA data transfer executes successfully.

**/
EFI_STATUS
EFIAPI
AhciDmaTransfer (
  IN     EFI_AHCI_CONTROLLER        *AhciController,
  IN     EFI_AHCI_REGISTERS         *AhciRegisters,
  IN     UINT8                      Port,
  IN     UINT8                      PortMultiplier,
  IN     EFI_AHCI_ATAPI_COMMAND     *AtapiCommand OPTIONAL,
  IN     UINT8                      AtapiCommandLength,
  IN     BOOLEAN                    Read,
  IN     EFI_ATA_COMMAND_BLOCK      *AtaCommandBlock,
  IN OUT EFI_ATA_STATUS_BLOCK       *AtaStatusBlock,
  IN OUT VOID                       *MemoryAddr,
  IN     UINT32                     DataCount,
  IN     UINT64                     Timeout
  );

/**
  Do AHCI HBA reset.

  @param  AhciController              The AHCI controller protocol instance.
  @param  Timeout            The timeout value of reset, uses 100ns as a unit.

  @retval EFI_DEVICE_ERROR   AHCI controller is failed to complete hardware reset.
  @retval EFI_TIMEOUT        The reset operation is time out.
  @retval EFI_SUCCESS        AHCI controller is reset successfully.

**/
EFI_STATUS
EFIAPI
AhciReset (
  IN  EFI_AHCI_CONTROLLER       *AhciController,
  IN  UINT64                    Timeout
  );

#endif

