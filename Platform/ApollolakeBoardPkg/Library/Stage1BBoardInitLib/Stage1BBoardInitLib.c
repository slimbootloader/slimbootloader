/** @file

  Copyright (c) 2017 - 2018, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <PiPei.h>
#include <Library/BaseLib.h>
#include <Library/IoLib.h>
#include <Library/PcdLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/GpioLib.h>
#include <Library/DebugLib.h>
#include <Library/CryptoLib.h>
#include <Library/BoardInitLib.h>
#include <Library/SerialPortLib.h>
#include <Library/PlatformHookLib.h>
#include <Library/ConfigDataLib.h>
#include <Library/BootloaderCoreLib.h>
#include <Library/SpiFlashLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/MmcAccessLib.h>
#include <IndustryStandard/Pci30.h>
#include <Library/DebugPrintErrorLevelLib.h>
#include <Library/SocInfoLib.h>
#include <Library/IocIpcLib.h>
#include <Library/RleCompressLib.h>
#include <PlatformBase.h>
#include <RegAccess.h>
#include <Library/BootGuardLib20.h>
#include <Library/TpmLib.h>
#include <Library/LoaderLib.h>
#include <Library/HeciLib.h>
#include <Library/BootloaderCommonLib.h>
#include <Library/BoardSupportLib.h>
#include <Library/SocInitLib.h>
#include <FspmUpd.h>
#include <GpioDefines.h>
#include <PlatformBase.h>
#include "ScRegs/RegsPmc.h"
#include <Pi/PiBootMode.h>
#include <FwuBootMode.h>
#include <PlatformHookSupport.h>
#include <ConfigDataDefs.h>
#include <ConfigDataCommonStruct.h>
#include <ConfigDataStruct.h>
#include <PlatformData.h>
#include <Register/RegsSpi.h>
#include <Library/TimerLib.h>
#include <Library/SideBandLib.h>
#include <Library/InternalIpcLib.h>
#include "ScRegs/SscRegs.h"
#include <MeBiosPayloadData.h>
#include <Library/GpioLibApl.h>

#define APL_FSP_STACK_TOP       0xFEF40000
#define MRC_PARAMS_BYTE_OFFSET_MRC_VERSION 14

CONST PLT_DEVICE  mPlatformDevices[]= {
  {{0x00001200}, OsBootDeviceSata  , 0 },
  {{0x00001B00}, OsBootDeviceSd    , 0 },
  {{0x00001C00}, OsBootDeviceEmmc  , 0 },
  {{0x00001D00}, OsBootDeviceUfs   , 0 },
  {{0x00000D02}, OsBootDeviceSpi   , 0 },
  {{0x00001500}, OsBootDeviceUsb   , 0 },
  {{0x01000000}, OsBootDeviceMemory, 0 },
  {{0x00000200}, PlatformDeviceGraphics, 0},
};

CONST CHAR16 *BootDeviceType[] = { L"eMMC", L"UFS", L"SPI" };

CONST
UINT8 mCommIdxTbl[] = {
  SOUTHWEST,
  NA,
  NORTHWEST,
  NORTH,
  WEST
};

const
BXT_GPIO_PAD_INIT  mGpioInitTbl[] = {
  /*                  Group Pin#:  pad_name,    PMode,GPIO_Config,HostSw,GPO_STATE,INT_Trigger,  Wake_Enabled ,Term_H_L,Inverted, GPI_ROUT, IOSstae, IOSTerm,MMIO_Offset,Community */
  BXT_GPIO_PAD_CONF (L"GPIO_134 LPSS_I2C5_SDA",   M2,    NA, NA,   NA,   NA,  Wake_Disabled, P_20K_H,   NA,    NA, D0RxDRx0I,  EnPu,  GPIO_PADBAR + 0x0050,  WEST),
  BXT_GPIO_PAD_CONF (L"GPIO_135 LPSS_I2C5_SCL",   M2,    NA, NA,   NA,   NA,  Wake_Disabled, P_20K_H,   NA,    NA, D0RxDRx0I,  EnPu,  GPIO_PADBAR + 0x0058,  WEST),
  BXT_GPIO_PAD_CONF (L"GPIO_136 LPSS_I2C6_SDA",   M2,    NA, NA,   NA,   NA,  Wake_Disabled, P_20K_H,   NA,    NA, D0RxDRx0I,  EnPu,  GPIO_PADBAR + 0x0060,  WEST),
  BXT_GPIO_PAD_CONF (L"GPIO_137 LPSS_I2C6_SCL",   M2,    NA, NA,   NA,   NA,  Wake_Disabled, P_20K_H,   NA,    NA, D0RxDRx0I,  EnPu,  GPIO_PADBAR + 0x0068,  WEST),

  BXT_GPIO_PAD_CONF (L"GPIO_0",                   M1,    NA,  NA,  NA,   NA, Wake_Disabled, P_20K_L,   NA,    NA,   HizRx0I,   SAME,    0x0400,  NORTH),
  BXT_GPIO_PAD_CONF (L"GPIO_1",                   M1,    NA,  NA,  NA,   NA, Wake_Disabled, P_20K_L,   NA,    NA,   HizRx0I,   SAME,    0x0408,  NORTH),
  BXT_GPIO_PAD_CONF (L"GPIO_2",                   M1,    NA,  NA,  NA,   NA, Wake_Disabled, P_20K_L,   NA,    NA,   HizRx0I,   SAME,    0x0410,  NORTH),
  BXT_GPIO_PAD_CONF (L"GPIO_3",                   M1,    NA,  NA,  NA,   NA, Wake_Disabled, P_20K_L,   NA,    NA,   HizRx0I,   SAME,    0x0418,  NORTH),
  BXT_GPIO_PAD_CONF (L"GPIO_4",                   M1,    NA,  NA,  NA,   NA, Wake_Disabled, P_20K_L,   NA,    NA,   HizRx0I,   SAME,    0x0420,  NORTH),
  BXT_GPIO_PAD_CONF (L"GPIO_5",                   M1,    NA,  NA,  NA,   NA, Wake_Disabled, P_20K_L,   NA,    NA,   HizRx0I,   SAME,    0x0428,  NORTH),                        //Mux with CSE_PG based on the SW3 switch
  BXT_GPIO_PAD_CONF (L"GPIO_6",                   M1,    NA,  NA,  NA,   NA, Wake_Disabled, P_20K_L,   NA,    NA,   HizRx0I,   SAME,    0x0430,  NORTH),                        //Mux with DISP1_RST_N based on the SW3 switch
  BXT_GPIO_PAD_CONF (L"GPIO_7",                   M1,    NA,  NA,  NA,   NA, Wake_Disabled, P_20K_L,   NA,    NA,   HizRx0I,   SAME,    0x0438,  NORTH),                        //Mux with DISP1_TOUCH_INT_N based on the SW3 switch
  BXT_GPIO_PAD_CONF (L"GPIO_8",                   M1,    NA,  NA,  NA,   NA, Wake_Disabled, P_20K_L,   NA,    NA,   HizRx0I,   SAME,    0x0440,  NORTH),                        //Mux with DISP1_TOUCH_RST_N based on the SW3 switch

  BXT_GPIO_PAD_CONF (L"GPIO_136 LPSS_I2C6_SDA",   M1,    NA, NA,   NA,   NA, Wake_Disabled,  P_1K_H,    NA,    NA, D1RxDRx1I,  EnPu,     0x0460,  WEST),                       //EXP_I2C_SDA and I2C_PSS_SDA and I2C_2_SDA_IOEXP
  BXT_GPIO_PAD_CONF (L"GPIO_137 LPSS_I2C6_SCL",   M1,    NA, NA,   NA,   NA, Wake_Disabled,  P_1K_H,    NA,    NA, D1RxDRx1I,  EnPu,     0x0468,  WEST),                       //EXP_I2C_SCL and I2C_PSS_SCL and I2C_2_SCL_IOEXP

  BXT_GPIO_PAD_CONF (L"GPIO_152 ISH_GPIO_6",      M0,    GPO, GPIO_D,  LO,   NA, Wake_Disabled, P_20K_L,   NA,    NA,     NA,     NA, GPIO_PADBAR + 0x00B0,  WEST),                       // PERST#
  BXT_GPIO_PAD_CONF (L"GPIO_19",                  M0,    GPO, GPIO_D,  HI,   NA, Wake_Disabled, P_20K_H,   NA,    NA,     NA,     NA, GPIO_PADBAR + 0x0098,  NORTH),                      // PFET

  BXT_GPIO_PAD_CONF (L"GPIO_13",                  M0,    GPO, GPIO_D,  LO,   NA, Wake_Disabled, P_20K_L,   NA,    NA,     NA,     NA, GPIO_PADBAR + 0x0068,  NORTH),                      // PERST#
  BXT_GPIO_PAD_CONF (L"GPIO_17",                  M0,    GPO, GPIO_D,  HI,   NA, Wake_Disabled, P_20K_H,   NA,    NA,     NA,     NA, GPIO_PADBAR + 0x0088,  NORTH),                      // PFET

  BXT_GPIO_PAD_CONF (L"GPIO_15",                  M0,    GPO, GPIO_D,  LO,   NA, Wake_Disabled, P_20K_L,   NA,    NA,     NA,     NA, GPIO_PADBAR + 0x0078,  NORTH),                      // PERST#

  BXT_GPIO_PAD_CONF (L"GPIO_210 PCIE_CLKREQ1_B",  M1,    NA, NA,  NA,   NA, Wake_Disabled, P_20K_L,   NA,    NA,     NA,     NA, GPIO_PADBAR + 0x00D8,  WEST),                            // CLKREQ#

  BXT_GPIO_PAD_CONF (L"SMB_CLK",                    M1,     NA,   NA,  NA,   NA, Wake_Disabled, P_20K_H,    NA,    NA, IOS_Masked, SAME, GPIO_PADBAR + 0x0100,  SOUTHWEST),
  BXT_GPIO_PAD_CONF (L"SMB_DATA",                   M1,     NA,   NA,  NA,   NA, Wake_Disabled, P_20K_H,    NA,    NA, IOS_Masked, SAME, GPIO_PADBAR + 0x0108,  SOUTHWEST),
  BXT_GPIO_PAD_CONF (L"LPC_ILB_SERIRQ",             M1,     NA,   NA,  NA,   NA, Wake_Disabled, P_20K_H,    NA,    NA, IOS_Masked, SAME, GPIO_PADBAR + 0x0110,  SOUTHWEST),
  BXT_GPIO_PAD_CONF (L"LPC_CLKOUT0",                M1,     NA,   NA,  NA,   NA, Wake_Disabled, P_NONE,     NA,    NA, IOS_Masked, SAME, GPIO_PADBAR + 0x0118,  SOUTHWEST),
  BXT_GPIO_PAD_CONF (L"LPC_CLKOUT1",                M1,     NA,   NA,  NA,   NA, Wake_Disabled, P_NONE,     NA,    NA, IOS_Masked, SAME, GPIO_PADBAR + 0x0120,  SOUTHWEST),
  BXT_GPIO_PAD_CONF (L"LPC_AD0",                    M1,     NA,   NA,  NA,   NA, Wake_Disabled, P_20K_H,    NA,    NA, IOS_Masked, SAME, GPIO_PADBAR + 0x0128,  SOUTHWEST),
  BXT_GPIO_PAD_CONF (L"LPC_AD1",                    M1,     NA,   NA,  NA,   NA, Wake_Disabled, P_20K_H,    NA,    NA, IOS_Masked, SAME, GPIO_PADBAR + 0x0130,  SOUTHWEST),
  BXT_GPIO_PAD_CONF (L"LPC_AD2",                    M1,     NA,   NA,  NA,   NA, Wake_Disabled, P_20K_H,    NA,    NA, IOS_Masked, SAME, GPIO_PADBAR + 0x0138,  SOUTHWEST),
  BXT_GPIO_PAD_CONF (L"LPC_AD3",                    M1,     NA,   NA,  NA,   NA, Wake_Disabled, P_20K_H,    NA,    NA, IOS_Masked, SAME, GPIO_PADBAR + 0x0140,  SOUTHWEST),
  BXT_GPIO_PAD_CONF (L"LPC_CLKRUNB",                M1,     NA,   NA,  NA,   NA, Wake_Disabled, P_20K_H,    NA,    NA, IOS_Masked, SAME, GPIO_PADBAR + 0x0148,  SOUTHWEST),
  BXT_GPIO_PAD_CONF (L"LPC_FRAMEB",                 M1,     NA,   NA,  NA,   NA, Wake_Disabled, P_20K_H,    NA,    NA, IOS_Masked, SAME, GPIO_PADBAR + 0x0150,  SOUTHWEST),
};

const
BXT_GPIO_PAD_INIT  mGpioInitTblEMMC[] = {

  BXT_GPIO_PAD_CONF(L"GPIO_156 EMMC0_CLK",         M1,     NA    ,   NA   ,  NA     ,   NA       ,Wake_Disabled, P_20K_L,    NA   ,    NA  ,HizRx0I  ,EnPd  ,   GPIO_PADBAR+0x0020 ,  SOUTHWEST),
  BXT_GPIO_PAD_CONF(L"GPIO_157 EMMC0_D0",          M1,     NA    ,   NA   ,  NA     ,   NA       ,Wake_Disabled, P_20K_H,    NA   ,    NA  ,HizRx0I  ,EnPd  ,   GPIO_PADBAR+0x0028 ,  SOUTHWEST),
  BXT_GPIO_PAD_CONF(L"GPIO_158 EMMC0_D1",          M1,     NA    ,   NA   ,  NA     ,   NA       ,Wake_Disabled, P_20K_H,    NA   ,    NA  ,HizRx0I  ,EnPd  ,   GPIO_PADBAR+0x0030 ,  SOUTHWEST),
  BXT_GPIO_PAD_CONF(L"GPIO_159 EMMC0_D2",          M1,     NA    ,   NA   ,  NA     ,   NA       ,Wake_Disabled, P_20K_H,    NA   ,    NA  ,HizRx0I  ,EnPd  ,   GPIO_PADBAR+0x0038 ,  SOUTHWEST),
  BXT_GPIO_PAD_CONF(L"GPIO_160 EMMC0_D3",          M1,     NA    ,   NA   ,  NA     ,   NA       ,Wake_Disabled, P_20K_H,    NA   ,    NA  ,HizRx0I  ,EnPd  ,   GPIO_PADBAR+0x0040 ,  SOUTHWEST),
  BXT_GPIO_PAD_CONF(L"GPIO_161 EMMC0_D4",          M1,     NA    ,   NA   ,  NA     ,   NA       ,Wake_Disabled, P_20K_H,    NA   ,    NA  ,HizRx0I  ,EnPd  ,   GPIO_PADBAR+0x0048 ,  SOUTHWEST),
  BXT_GPIO_PAD_CONF(L"GPIO_162 EMMC0_D5",          M1,     NA    ,   NA   ,  NA     ,   NA       ,Wake_Disabled, P_20K_H,    NA   ,    NA  ,HizRx0I  ,EnPd  ,   GPIO_PADBAR+0x0050 ,  SOUTHWEST),
  BXT_GPIO_PAD_CONF(L"GPIO_163 EMMC0_D6",          M1,     NA    ,   NA   ,  NA     ,   NA       ,Wake_Disabled, P_20K_H,    NA   ,    NA  ,HizRx0I  ,EnPd  ,   GPIO_PADBAR+0x0058 ,  SOUTHWEST),
  BXT_GPIO_PAD_CONF(L"GPIO_164 EMMC0_D7",          M1,     NA    ,   NA   ,  NA     ,   NA       ,Wake_Disabled, P_20K_H,    NA   ,    NA  ,HizRx0I  ,EnPd  ,   GPIO_PADBAR+0x0060 ,  SOUTHWEST),
  BXT_GPIO_PAD_CONF(L"GPIO_165 EMMC0_CMD",         M1,     NA    ,   NA   ,  NA     ,   NA       ,Wake_Disabled, P_20K_H,    NA   ,    NA  ,HizRx0I  ,EnPd  ,   GPIO_PADBAR+0x0068 ,  SOUTHWEST),
};


CONST
BXT_GPIO_PAD_INIT mGpioInitWifiTbl[] = {
  BXT_GPIO_PAD_CONF (L"GPIO_26",                  M0   ,    GPO   ,  NA   ,   HI    ,   NA      , Wake_Disabled,  P_NONE ,   NA    ,    NA,NA        ,  NA  ,    GPIO_PADBAR+0x00D0,  NORTH),
  BXT_GPIO_PAD_CONF (L"GPIO_27",                  M0   ,    GPO   ,  NA   ,   HI    ,   NA      , Wake_Disabled,  P_NONE ,   NA    ,    NA,NA        ,  NA  ,    GPIO_PADBAR+0x00D8,  NORTH),
};

typedef struct {
  UINT8             NumCores;
  UINT8             Ratio;
  PLATFORM_SKU_ID   PlatformSkuId;
} PLATFORM_SKU_INFO;

CONST PLATFORM_SKU_INFO mGpMrbSkuInfo[PlatformSkuMax] = {
  { 4,  24,   PlatformSkuPremium  },
  { 4,  20,   PlatformSkuHigh     },
  { 4,  18,   PlatformSkuMid      },
  { 2,  18,   PlatformSkuLow      }
};

CONST UINT32 mBaudRate[9] = {
  9600, 19200, 38400, 57600, 115200, 921600, 1500000, 3686400, 4000000
};

CONST UINT32 mInputHertz[9] = {
  1843200, 1843200, 1843200, 1843200, 1843200, 58982400, 58982400, 58982400, 64000000
};

CONST UINT16 mGpMrbModuleIdOffset[] = {
  0x01B8, 0x01C0, 0x01C8, 0x01D0, 0x01D8, 0x01E0, 0x01E8
};

CONST UINT16 mUp2ModuleIdOffset[] = {
  0x00D8, 0x00E0, 0x00F0
};

typedef struct {
  UINT8 RankEnable;
  UINT8 DeviceWidth;
  UINT8 DramDensity;
  UINT8 Option;
  UINT8 OdtConfig;
  UINT8 TristateClk1;
  UINT8 Mode2N;
  UINT8 OdtLevels;
} DRP_DRAM_POLICY;

typedef struct {
  UINT8             MemSkuId;
  UINT8             Profile;
  UINT8             NumOfChan;
  DRP_DRAM_POLICY   Policy;
} MEMORY_SKU_CONFIG;

CONST MEMORY_SKU_CONFIG mMemorySkuConfig[] = {
  {
    .MemSkuId   = MemorySku4x16Gb,
    .Profile    = 0,
    .NumOfChan  = 4,
    .Policy     = { 3, 1, 2, 3, 0, 0, 0, 0 }
  },
  {
    .MemSkuId   = MemorySku4x8Gb,
    .Profile    = 0,
    .NumOfChan  = 4,
    .Policy     = { 1, 1, 2, 3, 0, 0, 0, 0 }
  },
  {
    .MemSkuId   = MemorySku2x8Gb,
    .Profile    = 9,
    .NumOfChan  = 2,
    .Policy     = { 1, 1, 2, 3, 0, 0, 0, 0 }
  },
  {
    .MemSkuId   = MemorySku4x12Gb,
    .Profile    = 0,
    .NumOfChan  = 4,
    .Policy     = { 1, 1, 3, 3, 0, 0, 0, 0 }
  },
  {
    .MemSkuId   = MemorySku4x4Gb,
    .Profile    = 0,
    .NumOfChan  = 4,
    .Policy     = { 1, 1, 0, 3, 0, 0, 0, 0 }
  },
  {
    .MemSkuId   = MemorySku2x4Gb,
    .Profile    = 0,
    .NumOfChan  = 2,
    .Policy     = { 1, 1, 0, 3, 0, 0, 0, 0 }
  },
  {
    .MemSkuId   = MemorySku1x4Gb,
    .Profile    = 0,
    .NumOfChan  = 1,
    .Policy     = { 1, 1, 0, 3, 0, 0, 0, 0 }
  }
};



/**
  Read the board Id from the GPIO pins.

**/
STATIC
UINT8
GetEmbeddedBoardId (
  VOID
  )
{
  BXT_CONF_PAD0   PadConfg0;
  BXT_CONF_PAD1   PadConfg1;
  UINT8           BrdId;

  DEBUG ((DEBUG_INFO, "Get Embedded BoardId\n"));

  //Board_ID0: PMIC_STDBY
  PadConfg0.padCnf0 = GpioRead (NORTHWEST, GPIO_PADBAR + 0xF0 + BXT_GPIO_PAD_CONF0_OFFSET);
  PadConfg0.r.PMode = 0;
  PadConfg0.r.GPIORxTxDis = 0x1;
  GpioWrite (NORTHWEST, GPIO_PADBAR + 0xF0 + BXT_GPIO_PAD_CONF0_OFFSET, PadConfg0.padCnf0);
  PadConfg1.padCnf1 = GpioRead (NORTHWEST, GPIO_PADBAR + 0xF0 + BXT_GPIO_PAD_CONF1_OFFSET);
  //Set to Pull Up 20K
  PadConfg1.r.Term = 0xC;
  GpioWrite (NORTHWEST, GPIO_PADBAR + 0xF0 + BXT_GPIO_PAD_CONF1_OFFSET, PadConfg1.padCnf1);
  //Board_ID1: PMIC_SDWN_B
  PadConfg0.padCnf0 = GpioRead (NORTHWEST, GPIO_PADBAR + 0xD0 + BXT_GPIO_PAD_CONF0_OFFSET);
  PadConfg0.r.PMode = 0;
  PadConfg0.r.GPIORxTxDis = 0x1;
  GpioWrite (NORTHWEST, GPIO_PADBAR + 0xD0 + BXT_GPIO_PAD_CONF0_OFFSET, PadConfg0.padCnf0);
  //Board_ID2: PMIC_RESET_B
  PadConfg0.padCnf0 = GpioRead (NORTHWEST, GPIO_PADBAR + 0xC8 + BXT_GPIO_PAD_CONF0_OFFSET);
  PadConfg0.r.PMode = 0;
  PadConfg0.r.GPIORxTxDis = 0x1;
  GpioWrite (NORTHWEST, GPIO_PADBAR + 0xC8 + BXT_GPIO_PAD_CONF0_OFFSET, PadConfg0.padCnf0);
  //Board_ID3: PMIC_PWRGOOD
  PadConfg0.padCnf0 = GpioRead (NORTHWEST, GPIO_PADBAR + 0xC0 + BXT_GPIO_PAD_CONF0_OFFSET);
  PadConfg0.r.PMode = 0;
  PadConfg0.r.GPIORxTxDis = 0x1;
  GpioWrite (NORTHWEST, GPIO_PADBAR + 0xC0 + BXT_GPIO_PAD_CONF0_OFFSET, PadConfg0.padCnf0);

  BrdId = (UINT8)   (((GpioRead (NORTHWEST, GPIO_PADBAR + 0x00F0 + BXT_GPIO_PAD_CONF0_OFFSET) & BIT1) >> 1) | \
                     (((GpioRead (NORTHWEST, GPIO_PADBAR + 0x00D0 + BXT_GPIO_PAD_CONF0_OFFSET) & BIT1) >> 1) << 1) | \
                     (((GpioRead (NORTHWEST, GPIO_PADBAR + 0x00C8 + BXT_GPIO_PAD_CONF0_OFFSET) & BIT1) >> 1) << 2));

  return BrdId;
}

/**
  Read the board Id from the GPIO pins' Cfg Data.

**/
STATIC
UINT8
GetBoardIdFromGpioPins (
  VOID
  )
{
  BXT_CONF_PAD0       PadConfg0;
  BXT_CONF_PAD1       PadConfg1;
  UINT8               BrdId = 0x0;
  PID_GPIO_CFG_DATA   *PidGpioCfgData;
  PID_GPIO_PIN_DATA   *PidGpioPinData;
  UINT8               Index;
  UINT8               Community;
  UINT16              PadNum;
  UINT8               PinCount;

  BrdId = 0xFF;
  PidGpioCfgData = (PID_GPIO_CFG_DATA *)FindConfigDataByTag (CDATA_PID_GPIO_TAG);
  if (PidGpioCfgData != NULL) {
    PinCount = (UINT8)ARRAY_SIZE (PidGpioCfgData->PidPin0);
    PidGpioPinData = PidGpioCfgData->PidPin0;
    for (Index = 0; Index < PinCount; Index++) {
      if (PidGpioPinData->Enable == 0) {
        continue;
      } else if (BrdId == 0xFF) {
        BrdId = 0;
      }
      Community = (UINT8) (PidGpioPinData->PidPadInfo & 0xFF);
      PadNum    = (UINT16)(PidGpioPinData->PidPadInfo >> 8);
      if (PadNum < 0x300) {
        PadConfg0.padCnf0 = GpioRead (Community, GPIO_PADBAR + PadNum + BXT_GPIO_PAD_CONF0_OFFSET);
        PadConfg0.r.PMode = 0;
        PadConfg0.r.GPIORxTxDis = 0x1;
        GpioWrite (Community, GPIO_PADBAR + PadNum + BXT_GPIO_PAD_CONF0_OFFSET, PadConfg0.padCnf0);
        PadConfg1.padCnf1 = GpioRead (Community, GPIO_PADBAR + PadNum + BXT_GPIO_PAD_CONF1_OFFSET);
        PadConfg1.r.IOSTerm = 0x3; //Enable Pullup
        PadConfg1.r.Term    = 0xC & 0x0F; //Default 20K set to Pull Up
        GpioWrite (Community, GPIO_PADBAR + PadNum + BXT_GPIO_PAD_CONF1_OFFSET, PadConfg1.padCnf1);
        BrdId |= (UINT8) (((GpioRead (Community, GPIO_PADBAR + PadNum + BXT_GPIO_PAD_CONF0_OFFSET) & BIT1) >> 1) << Index);
      }
      PidGpioPinData++;
    }
  }

  if (BrdId == 0xFF) {
    DEBUG ((DEBUG_INFO, "GPIO based PID detection is disabled\n"));
  }
  return BrdId;
}

/**
  Read the IVI board Id from the GPIO pins.

**/
STATIC
UINT8
GetIVIBoardId (
  VOID
  )
{
  BXT_CONF_PAD0   PadConfg0;
  BXT_CONF_PAD1   PadConfg1;
  UINT8           BrdId;

  DEBUG ((DEBUG_INFO, "Get IVI BoardId\n"));

  //Board_ID0: GPIO_62
  PadConfg0.padCnf0 = GpioRead (NORTH, GPIO_PADBAR + 0x190 + BXT_GPIO_PAD_CONF0_OFFSET);
  PadConfg0.r.PMode = 0;
  PadConfg0.r.GPIORxTxDis = 0x1;
  GpioWrite (NORTH, GPIO_PADBAR + 0x190 + BXT_GPIO_PAD_CONF0_OFFSET, PadConfg0.padCnf0);
  PadConfg1.padCnf1 = GpioRead (NORTH, GPIO_PADBAR + 0x190 + BXT_GPIO_PAD_CONF1_OFFSET);
  PadConfg1.r.IOSTerm = 0x3; //Enable Pullup
  PadConfg1.r.Term = 0xC;    //20k wpu
  GpioWrite (NORTH, GPIO_PADBAR + 0x190 + BXT_GPIO_PAD_CONF1_OFFSET, PadConfg1.padCnf1);
  //Board_ID1: GPIO_63
  PadConfg0.padCnf0 = GpioRead (NORTH, GPIO_PADBAR + 0x198 + BXT_GPIO_PAD_CONF0_OFFSET);
  PadConfg0.r.PMode = 0;
  PadConfg0.r.GPIORxTxDis = 0x1;
  GpioWrite (NORTH, GPIO_PADBAR + 0x198 + BXT_GPIO_PAD_CONF0_OFFSET, PadConfg0.padCnf0);
  PadConfg1.padCnf1 = GpioRead (NORTH, GPIO_PADBAR + 0x198 + BXT_GPIO_PAD_CONF1_OFFSET);
  PadConfg1.r.IOSTerm = 0x3; //Enable Pullup
  PadConfg1.r.Term = 0xC;    //20k wpu
  GpioWrite (NORTH, GPIO_PADBAR + 0x198 + BXT_GPIO_PAD_CONF1_OFFSET, PadConfg1.padCnf1);
  //Board_ID2: GPIO_64
  PadConfg0.padCnf0 = GpioRead (NORTH, GPIO_PADBAR + 0x1A0 + BXT_GPIO_PAD_CONF0_OFFSET);
  PadConfg0.r.PMode = 0;
  PadConfg0.r.GPIORxTxDis = 0x1;
  GpioWrite (NORTH, GPIO_PADBAR + 0x1A0 + BXT_GPIO_PAD_CONF0_OFFSET, PadConfg0.padCnf0);
  PadConfg1.padCnf1 = GpioRead (NORTH, GPIO_PADBAR + 0x1A0 + BXT_GPIO_PAD_CONF1_OFFSET);
  PadConfg1.r.IOSTerm = 0x3; //Enable Pullup
  PadConfg1.r.Term = 0xC;    //20k wpu
  GpioWrite (NORTH, GPIO_PADBAR + 0x1A0 + BXT_GPIO_PAD_CONF1_OFFSET, PadConfg1.padCnf1);
  //Board_ID3: GPIO_65
  PadConfg0.padCnf0 = GpioRead (NORTH, GPIO_PADBAR + 0x1A8 + BXT_GPIO_PAD_CONF0_OFFSET);
  PadConfg0.r.PMode = 0;
  PadConfg0.r.GPIORxTxDis = 0x1;
  GpioWrite (NORTH, GPIO_PADBAR + 0x1A8 + BXT_GPIO_PAD_CONF0_OFFSET, PadConfg0.padCnf0);
  PadConfg1.padCnf1 = GpioRead (NORTH, GPIO_PADBAR + 0x1A8 + BXT_GPIO_PAD_CONF1_OFFSET);
  PadConfg1.r.IOSTerm = 0x3; //Enable Pullup
  PadConfg1.r.Term = 0xC;    //20k wpu
  GpioWrite (NORTH, GPIO_PADBAR + 0x1A8 + BXT_GPIO_PAD_CONF1_OFFSET, PadConfg1.padCnf1);

  BrdId = (UINT8)   (((GpioRead (NORTH, GPIO_PADBAR + 0x0190 + BXT_GPIO_PAD_CONF0_OFFSET) & BIT1) >> 1) | \
                     (((GpioRead (NORTH, GPIO_PADBAR + 0x0198 + BXT_GPIO_PAD_CONF0_OFFSET) & BIT1) >> 1) << 1) | \
                     (((GpioRead (NORTH, GPIO_PADBAR + 0x01A0 + BXT_GPIO_PAD_CONF0_OFFSET) & BIT1) >> 1) << 2) | \
                     (((GpioRead (NORTH, GPIO_PADBAR + 0x01A8 + BXT_GPIO_PAD_CONF0_OFFSET) & BIT1) >> 1) << 3));

  return BrdId;
}

/**
  Initialize module id based on platform id.

  @retval  EFI_SUCCESS     Initialized module id successfully
  @retval  EFI_UNSUPPORTED Unsupport
**/
STATIC
EFI_STATUS
ModuleIdInitialize (
  VOID
  )
{
  UINT16          PlatformId;
  UINT16          ModuleId;
  BXT_CONF_PAD0   PadConfg0;
  BXT_CONF_PAD1   PadConfg1;
  UINT16          Offset;
  UINT16          Index;
  PLATFORM_DATA   *PlatformData;
  UINT32          CpuNumCores;
  UINT32          TurboRatio;

  PlatformId = GetPlatformId ();
  switch (PlatformId) {
  //
  // Will be configured by CDATA
  //
  case PLATFORM_ID_GPMRB:
    ModuleId = 0;
    for (Index = 0; Index < ARRAY_SIZE (mGpMrbModuleIdOffset); Index ++) {
      Offset = GPIO_PADBAR + mGpMrbModuleIdOffset[Index];
      PadConfg0.padCnf0 = GpioRead (NORTH, Offset + BXT_GPIO_PAD_CONF0_OFFSET);
      PadConfg0.r.PMode = 0;
      PadConfg0.r.GPIORxTxDis = 0x1;
      GpioWrite (NORTH, Offset + BXT_GPIO_PAD_CONF0_OFFSET, PadConfg0.padCnf0);
      PadConfg1.padCnf1 = GpioRead (NORTH, Offset + BXT_GPIO_PAD_CONF1_OFFSET);
      PadConfg1.r.IOSTerm = 0x3;
      PadConfg1.r.Term = 0xC;
      GpioWrite (NORTH, Offset + BXT_GPIO_PAD_CONF1_OFFSET, PadConfg1.padCnf1);

      PadConfg0.padCnf0 = GpioRead (NORTH, Offset + BXT_GPIO_PAD_CONF0_OFFSET);
      ModuleId |= (UINT16) (PadConfg0.r.GPIORxState << Index);
    }
    PlatformData = (PLATFORM_DATA *)GetPlatformDataPtr ();
    PlatformData->ModuleIdInfo.ModuleId = ModuleId & 0x7F;

    PlatformData->ModuleIdInfo.Bits.PlatformSkuId = PlatformSkuMax;
    CpuNumCores = GetCpuNumCores();
    TurboRatio  = GetCpuTurboRatio();
    for (Index = 0; Index < ARRAY_SIZE (mGpMrbSkuInfo); Index ++) {
      if (mGpMrbSkuInfo[Index].NumCores == CpuNumCores && mGpMrbSkuInfo[Index].Ratio <= TurboRatio) {
        PlatformData->ModuleIdInfo.Bits.PlatformSkuId = Index;
        break;
      }
    }

    break;

  case PLATFORM_ID_UP2:
    ModuleId = 0;
    for (Index = 0; Index < ARRAY_SIZE (mUp2ModuleIdOffset); Index ++) {
      Offset = GPIO_PADBAR + mUp2ModuleIdOffset[Index];
      PadConfg0.padCnf0 = GpioRead (NORTHWEST, Offset + BXT_GPIO_PAD_CONF0_OFFSET);
      PadConfg0.r.PMode = 0;
      PadConfg0.r.GPIORxTxDis = 0x1;
      GpioWrite (NORTHWEST, Offset + BXT_GPIO_PAD_CONF0_OFFSET, PadConfg0.padCnf0);
      PadConfg1.padCnf1 = GpioRead (NORTHWEST, Offset + BXT_GPIO_PAD_CONF1_OFFSET);
      PadConfg1.r.IOSTerm = 0x3;
      PadConfg1.r.Term = 0xC;
      GpioWrite (NORTHWEST, Offset + BXT_GPIO_PAD_CONF1_OFFSET, PadConfg1.padCnf1);

      PadConfg0.padCnf0 = GpioRead (NORTHWEST, Offset + BXT_GPIO_PAD_CONF0_OFFSET);
      ModuleId |= (UINT16) (PadConfg0.r.GPIORxState << Index);
    }
    PlatformData = (PLATFORM_DATA *)GetPlatformDataPtr ();
    PlatformData->ModuleIdInfo.ModuleId = ModuleId & 0x7;

    break;

  default:
    return EFI_UNSUPPORTED;
    break;
  }

  DEBUG ((DEBUG_INFO, "MemSkuId 0x%02X, FabId 0x%02X\n", \
          PlatformData->ModuleIdInfo.Bits.MemSkuId, \
          PlatformData->ModuleIdInfo.Bits.FabId));

  return EFI_SUCCESS;
}

/**
  Set the debug print error level fron CFG data.

**/
VOID *
EFIAPI
SetDebugLevelFromCfgData (
  VOID
  )
{
  GEN_CFG_DATA   *GenCfgData;

  GenCfgData = (GEN_CFG_DATA *)FindConfigDataByTag (CDATA_GEN_TAG);
  if (GenCfgData != NULL) {
    DEBUG ((DEBUG_INFO, "Load general parameter from CfgData.\n"));
    SetDebugPrintErrorLevel (GenCfgData->DebugPrintLevel);
  }
  return 0;
}

/**
  Update FSP-M UPD config data.

  @param  FspmUpdPtr            The pointer to the FSP-M UPD to be updated.

**/
VOID
EFIAPI
UpdateFspConfig (
  VOID     *FspmUpdPtr
  )
{
  FSPM_UPD         *FspmUpd;
  FSP_M_CONFIG     *Fspmcfg;
  MEMORY_CFG_DATA  *MemCfgData;
  GPU_CFG_DATA     *GpuCfgData;
  PLATFORM_DATA    *PlatformData;
  UINTN             PolicyAddress;
  DRP_DRAM_POLICY  *Drp;
  MEMORY_SKU_ID     MemSkuId;
  FAB_ID            FabId;
  UINT16            Index;
  UINT16            IndexOfChan;

  FspmUpd = (FSPM_UPD *)FspmUpdPtr;
  Fspmcfg = &FspmUpd->FspmConfig;

  MemCfgData = (MEMORY_CFG_DATA *)FindConfigDataByTag (CDATA_MEMORY_TAG);
  if (MemCfgData != NULL) {
    DEBUG ((DEBUG_INFO, "Load memory parameters from CfgData.\n"));
    Fspmcfg->MrcFastBoot               = MemCfgData->MrcFastBoot;
    Fspmcfg->Package                   = MemCfgData->Package;
    Fspmcfg->Profile                   = MemCfgData->Profile;
    Fspmcfg->MemoryDown                = MemCfgData->MemoryDown;
    Fspmcfg->DDR3LPageSize             = MemCfgData->DDR3LPageSize;
    Fspmcfg->DDR3LASR                  = MemCfgData->DDR3LASR;
    Fspmcfg->ScramblerSupport          = MemCfgData->ScramblerSupport;
    Fspmcfg->ChannelHashMask           = MemCfgData->ChannelHashMask;
    Fspmcfg->SliceHashMask             = MemCfgData->SliceHashMask;
    Fspmcfg->InterleavedMode           = MemCfgData->InterleavedMode;
    Fspmcfg->ChannelsSlicesEnable      = MemCfgData->ChannelsSlicesEnable;
    Fspmcfg->MinRefRate2xEnable        = MemCfgData->MinRefRate2xEnable;
    Fspmcfg->DualRankSupportEnable     = MemCfgData->DualRankSupportEnable;
    Fspmcfg->RmtMode                   = MemCfgData->RmtMode;
    Fspmcfg->MemorySizeLimit           = MemCfgData->MemorySizeLimit;
    Fspmcfg->LowMemoryMaxValue         = MemCfgData->LowMemoryMaxValue;
    Fspmcfg->DisableFastBoot           = MemCfgData->DisableFastBoot;
    Fspmcfg->HighMemoryMaxValue        = MemCfgData->HighMemoryMaxValue;
    Fspmcfg->DIMM0SPDAddress           = MemCfgData->DIMM0SPDAddress;
    Fspmcfg->DIMM1SPDAddress           = MemCfgData->DIMM1SPDAddress;
    Fspmcfg->Ch0_RankEnable            = MemCfgData->Ch0_RankEnable;
    Fspmcfg->Ch0_DeviceWidth           = MemCfgData->Ch0_DeviceWidth;
    Fspmcfg->Ch0_DramDensity           = MemCfgData->Ch0_DramDensity;
    Fspmcfg->Ch0_Option                = MemCfgData->Ch0_Option;
    Fspmcfg->Ch0_OdtConfig             = MemCfgData->Ch0_OdtConfig;
    Fspmcfg->Ch0_TristateClk1          = MemCfgData->Ch0_TristateClk1;
    Fspmcfg->Ch0_Mode2N                = MemCfgData->Ch0_Mode2N;
    Fspmcfg->Ch0_OdtLevels             = MemCfgData->Ch0_OdtLevels;
    Fspmcfg->Ch1_RankEnable            = MemCfgData->Ch1_RankEnable;
    Fspmcfg->Ch1_DeviceWidth           = MemCfgData->Ch1_DeviceWidth;
    Fspmcfg->Ch1_DramDensity           = MemCfgData->Ch1_DramDensity;
    Fspmcfg->Ch1_Option                = MemCfgData->Ch1_Option;
    Fspmcfg->Ch1_OdtConfig             = MemCfgData->Ch1_OdtConfig;
    Fspmcfg->Ch1_TristateClk1          = MemCfgData->Ch1_TristateClk1;
    Fspmcfg->Ch1_Mode2N                = MemCfgData->Ch1_Mode2N;
    Fspmcfg->Ch1_OdtLevels             = MemCfgData->Ch1_OdtLevels;
    Fspmcfg->Ch2_RankEnable            = MemCfgData->Ch2_RankEnable;
    Fspmcfg->Ch2_DeviceWidth           = MemCfgData->Ch2_DeviceWidth;
    Fspmcfg->Ch2_DramDensity           = MemCfgData->Ch2_DramDensity;
    Fspmcfg->Ch2_Option                = MemCfgData->Ch2_Option;
    Fspmcfg->Ch2_OdtConfig             = MemCfgData->Ch2_OdtConfig;
    Fspmcfg->Ch2_TristateClk1          = MemCfgData->Ch2_TristateClk1;
    Fspmcfg->Ch2_Mode2N                = MemCfgData->Ch2_Mode2N;
    Fspmcfg->Ch2_OdtLevels             = MemCfgData->Ch2_OdtLevels;
    Fspmcfg->Ch3_RankEnable            = MemCfgData->Ch3_RankEnable;
    Fspmcfg->Ch3_DeviceWidth           = MemCfgData->Ch3_DeviceWidth;
    Fspmcfg->Ch3_DramDensity           = MemCfgData->Ch3_DramDensity;
    Fspmcfg->Ch3_Option                = MemCfgData->Ch3_Option;
    Fspmcfg->Ch3_OdtConfig             = MemCfgData->Ch3_OdtConfig;
    Fspmcfg->Ch3_TristateClk1          = MemCfgData->Ch3_TristateClk1;
    Fspmcfg->Ch3_Mode2N                = MemCfgData->Ch3_Mode2N;
    Fspmcfg->Ch3_OdtLevels             = MemCfgData->Ch3_OdtLevels;
    Fspmcfg->RmtCheckRun               = MemCfgData->RmtCheckRun;
    Fspmcfg->RmtMarginCheckScaleHighThreshold    = MemCfgData->RmtMarginCheckScaleHighThreshold;
    Fspmcfg->MsgLevelMask              = MemCfgData->MsgLevelMask;
    Fspmcfg->EnhancePort8xhDecoding    = MemCfgData->EnhancePort8xhDecoding;
    Fspmcfg->SpdWriteEnable            = MemCfgData->SpdWriteEnable;
    CopyMem (Fspmcfg->Ch0_Bit_swizzling, MemCfgData->Ch0_Bit_swizzling, sizeof (MemCfgData->Ch0_Bit_swizzling));
    CopyMem (Fspmcfg->Ch1_Bit_swizzling, MemCfgData->Ch1_Bit_swizzling, sizeof (MemCfgData->Ch1_Bit_swizzling));
    CopyMem (Fspmcfg->Ch2_Bit_swizzling, MemCfgData->Ch2_Bit_swizzling, sizeof (MemCfgData->Ch2_Bit_swizzling));
    CopyMem (Fspmcfg->Ch3_Bit_swizzling, MemCfgData->Ch3_Bit_swizzling, sizeof (MemCfgData->Ch3_Bit_swizzling));

    Fspmcfg->SerialDebugPortType        = 2;
    Fspmcfg->SerialDebugPortDevice      = GetDebugPort ();
    Fspmcfg->SerialDebugPortStrideSize  = 2;
    Fspmcfg->SerialDebugPortAddress     = 0;
    Fspmcfg->PreMemGpioTableEntryNum    = 0;
    Fspmcfg->eMMCTraceLen               = 0;
    SetMem (Fspmcfg->PreMemGpioTablePinNum, sizeof (Fspmcfg->PreMemGpioTablePinNum), 0);
    Fspmcfg->NpkEn                      = MemCfgData->NpkEn;
  } else {
    CpuHalt ("Failed to find memory CFGDATA!");
  }
  Fspmcfg->MrcDataSaving             = 0;

  //
  // The NVS buffer will be loaded to PcdStage1BLoadBase FindNvsData()
  // The PcdStage1BLoadBase is not used any more after Stage1B is loaded, so reuse it to save CAR space.
  //
  Fspmcfg->VariableNvsBufferPtr      = (VOID *)(UINTN)APL_FSP_STACK_TOP;

  //
  // This will be done by configuration data
  //
  if (GetPlatformId () == PLATFORM_ID_UP2) {
    PlatformData  = (PLATFORM_DATA *)GetPlatformDataPtr ();

    DEBUG ((DEBUG_INFO, "UP2 memory SKU ID is 0x%x\n", PlatformData->ModuleIdInfo.Bits.MemSkuId));
    switch (PlatformData->ModuleIdInfo.Bits.MemSkuId) {
      case 5: /* 4GB */
        Fspmcfg->Ch0_RankEnable        = 1;
        Fspmcfg->Ch0_DramDensity       = 2;
        Fspmcfg->Ch1_RankEnable        = 1;
        Fspmcfg->Ch1_DramDensity       = 2;
        Fspmcfg->Ch2_RankEnable        = 1;
        Fspmcfg->Ch2_DramDensity       = 2;
        Fspmcfg->Ch3_RankEnable        = 1;
        Fspmcfg->Ch3_DramDensity       = 2;
        break;
      case 6: /* 8GB */
        Fspmcfg->Ch0_RankEnable        = 3;
        Fspmcfg->Ch0_DramDensity       = 2;
        Fspmcfg->Ch1_RankEnable        = 3;
        Fspmcfg->Ch1_DramDensity       = 2;
        Fspmcfg->Ch2_RankEnable        = 3;
        Fspmcfg->Ch2_DramDensity       = 2;
        Fspmcfg->Ch3_RankEnable        = 3;
        Fspmcfg->Ch3_DramDensity       = 2;
        break;

      case 7: /* 8GB */
        Fspmcfg->Ch0_RankEnable        = 1;
        Fspmcfg->Ch0_DramDensity       = 4;
        Fspmcfg->Ch1_RankEnable        = 1;
        Fspmcfg->Ch1_DramDensity       = 4;
        Fspmcfg->Ch2_RankEnable        = 1;
        Fspmcfg->Ch2_DramDensity       = 4;
        Fspmcfg->Ch3_RankEnable        = 1;
        Fspmcfg->Ch3_DramDensity       = 4;
        break;

      default: /* 2GB */
        Fspmcfg->Ch0_RankEnable        = 1;
        Fspmcfg->Ch0_DramDensity       = 2;
        Fspmcfg->Ch1_RankEnable        = 1;
        Fspmcfg->Ch1_DramDensity       = 2;
        Fspmcfg->Ch2_RankEnable        = 0;
        Fspmcfg->Ch3_RankEnable        = 0;
        break;
    };

  } else if (GetPlatformId () == PLATFORM_ID_GPMRB) {
    PlatformData  = (PLATFORM_DATA *)GetPlatformDataPtr ();
    MemSkuId      = PlatformData->ModuleIdInfo.Bits.MemSkuId;
    FabId         = PlatformData->ModuleIdInfo.Bits.FabId;

    for (Index = 0; Index < ARRAY_SIZE (mMemorySkuConfig); Index ++) {
      if (MemSkuId == mMemorySkuConfig[Index].MemSkuId) {
        DEBUG ((DEBUG_INFO, "Found memory sku(id %d) configuration!\n", mMemorySkuConfig[Index].MemSkuId));
        break;
      }
    }

    if (Index >= ARRAY_SIZE (mMemorySkuConfig)) {
      DEBUG ((DEBUG_ERROR, "Failed to find memory sku configuration!\n"));
    } else {
      PolicyAddress = (UINTN) (VOID *)&Fspmcfg->Ch0_RankEnable;
      ZeroMem ((VOID *)PolicyAddress, sizeof (DRP_DRAM_POLICY) * 4);
      for (IndexOfChan = 0; IndexOfChan < mMemorySkuConfig[Index].NumOfChan; IndexOfChan ++) {
        Drp               = (DRP_DRAM_POLICY *)PolicyAddress;
        Drp->RankEnable   = mMemorySkuConfig[Index].Policy.RankEnable;
        Drp->DeviceWidth  = mMemorySkuConfig[Index].Policy.DeviceWidth;
        Drp->DramDensity  = mMemorySkuConfig[Index].Policy.DramDensity;
        Drp->Option       = mMemorySkuConfig[Index].Policy.Option;
        Drp->OdtConfig    = mMemorySkuConfig[Index].Policy.OdtConfig;
        Drp->TristateClk1 = mMemorySkuConfig[Index].Policy.TristateClk1;
        Drp->Mode2N       = mMemorySkuConfig[Index].Policy.Mode2N;
        Drp->OdtLevels    = mMemorySkuConfig[Index].Policy.OdtLevels;

        if (FabId > FabIdD) {
          Drp->OdtConfig = 0x00;
        } else {
          Drp->OdtConfig = 0x02;
        }

        PolicyAddress += sizeof (DRP_DRAM_POLICY);
      }

      if (mMemorySkuConfig[Index].Profile == 0) {
        switch (PlatformData->ModuleIdInfo.Bits.PlatformSkuId) {
        case PlatformSkuPremium:
        case PlatformSkuHigh:
          Fspmcfg->Profile  = 9;
          break;
        case PlatformSkuMid:
        case PlatformSkuLow:
          Fspmcfg->Profile  = 10;
          break;
        }
      }
    }

    if (FabId > FabIdD) {
      Fspmcfg->ScramblerSupport |= 2;
    } else if ((FabId == FabIdD) /*&& (need to check cpu version < BXTP_B1(0x52))*/) {
      Fspmcfg->ScramblerSupport |= 2;
    }
  }

  //
  // Set to a default value so that we don't need wait 100ms in FSP.
  // Ticks = Time x Frequency / 1,000,000,000
  Fspmcfg->StartTimerTickerOfPfetAssert = 20000;

  /* Stage2 will be loaded by Stage1 instead of FSP */
  Fspmcfg->OemLoadingBase = PcdGet32 (PcdStage2LoadBase);
  if (PcdGetBool (PcdLoadImageUseFsp)) {
    CopyMem (Fspmcfg->OemFileName, "OBB", 4);
  } else {
    Fspmcfg->OemFileName[0] = 0;
  }
  SetDebugLevelFromCfgData ();

  GpuCfgData = (GPU_CFG_DATA *) FindConfigDataByTag (CDATA_GPU_TAG);
  if (GpuCfgData != NULL) {
    Fspmcfg->IgdApertureSize           = GpuCfgData->ApertureSize & 0x1F;
    Fspmcfg->Igd                       = GpuCfgData->Igd;
    Fspmcfg->IgdDvmt50PreAlloc         = GpuCfgData->IgdDvmt50PreAlloc;
    Fspmcfg->GttSize                   = GpuCfgData->GttSize;
    Fspmcfg->PrimaryVideoAdaptor       = GpuCfgData->PrimaryVideoAdaptor;
  }
}

/**
  Read the current platform state from PM status reg.

  @retval  Bootmode current power state.
**/
UINT8
GetPlatformPowerState (
  VOID
  )
{
  UINT32        Pm1EnSts;
  UINT32        Pm1Cnt;
  UINT8         BootMode;

  BootMode = BOOT_WITH_FULL_CONFIGURATION;

  Pm1EnSts = IoRead32 (ACPI_BASE_ADDRESS + R_ACPI_PM1_STS);
  Pm1Cnt = IoRead32 (ACPI_BASE_ADDRESS + R_ACPI_PM1_CNT);
  if ((Pm1EnSts & B_ACPI_PM1_STS_WAK) == B_ACPI_PM1_STS_WAK) {
    switch (Pm1Cnt & B_ACPI_PM1_CNT_SLP_TYP) {
    case V_ACPI_PM1_CNT_S3:
      BootMode = BOOT_ON_S3_RESUME;
      IoWrite32 (ACPI_BASE_ADDRESS + R_ACPI_PM1_CNT, Pm1Cnt & ~B_ACPI_PM1_CNT_SLP_TYP);
      break;
    default:
      break;
    }
  }

  return BootMode;
}


/**
  Initialize EMMC in early stage before MemoryInit to achieve better performance number

   @retval  EFI_SUCCESS           The operation completed successfully.
   @retval  others                There is error happening.

**/

VOID
EarlyBootDeviceInit (
  VOID
)
{
  EFI_STATUS  Status        = EFI_SUCCESS;
  UINTN       EmmcHcPciBase;

  EmmcHcPciBase = GetDeviceAddr (OsBootDeviceEmmc, 0);
  EmmcHcPciBase = TO_MM_PCI_ADDRESS (EmmcHcPciBase);

  UINT32      Base          = 0xFE700000; // temporary PCI MMIO resource for eMMC device, non-conflicting MMIO address


  /* Configure EMMC GPIO Pad */
  GpioConfigurePads (ARRAY_SIZE(mGpioInitTblEMMC), (GPIO_INIT_CONFIG *) (UINTN) mGpioInitTblEMMC);
  DEBUG ((DEBUG_INFO, "Early GpioInit for EMMC\n"));

  MmioWrite32 (EmmcHcPciBase + PCI_BASE_ADDRESSREG_OFFSET, Base);
  MmioWrite32 (EmmcHcPciBase + PCI_BASE_ADDRESSREG_OFFSET + 4, 0x00000000);
  MmioOr32    (EmmcHcPciBase + PCI_COMMAND_OFFSET, EFI_PCI_COMMAND_MEMORY_SPACE);

  /* Configure initial DLLs for early emmc init before SiliconInit
   * values are from ABL's default values  */
  MmioWrite32(Base + R_SCC_MEM_TX_CMD_DLL_CNTL,       0x00000505);
  MmioWrite32(Base + R_SCC_MEM_TX_DATA_DLL_CNTL1,     0x00000C11);
  MmioWrite32(Base + R_SCC_MEM_TX_DATA_DLL_CNTL2,     0x28282927);
  MmioWrite32(Base + R_SCC_MEM_RX_CMD_DATA_DLL_CNTL1, 0x0000000A);
  MmioWrite32(Base + R_SCC_MEM_RX_STROBE_DLL_CNTL,    0x201D162F);
  MmioWrite32(Base + R_SCC_MEM_RX_CMD_DATA_DLL_CNTL2, 0x0001000A);

  Status = MmcInitialize(EmmcHcPciBase, DevInitOnlyPhase1);

  MmioAnd32   (EmmcHcPciBase + PCI_COMMAND_OFFSET, (UINT32)~EFI_PCI_COMMAND_MEMORY_SPACE);
  MmioWrite32 (EmmcHcPciBase + PCI_BASE_ADDRESSREG_OFFSET, 0x00000000);

  if (EFI_ERROR(Status)) {
    DEBUG ((DEBUG_ERROR, "Early MMC Init Fails @ 0x%08X - Status(%r)\n", Base, Status));
  }
}


/**
  Read CMOS to determine the FWU.

  @retval BOOLEAN  true if in FWU false if not.

**/
BOOLEAN
IsFirmwareUpdate ()
{
  EFI_STATUS        Status;
  UINT16            FwuReq;
  FLASH_MAP        *FlashMapPtr;
  FW_UPDATE_STATUS  FwUpdStatus;
  UINT32            OffsetInBiosRgn;
  UINT32            RsvdBase;

  // Check if state machine is set to capsule processing mode.
  FlashMapPtr = GetFlashMapPtr ();
  Status = GetComponentInfoByPartition (FLASH_MAP_SIG_BLRESERVED, FALSE, &RsvdBase, NULL);
  if (!EFI_ERROR (Status) && (FlashMapPtr != NULL)) {
    OffsetInBiosRgn = FlashMapPtr->RomSize + RsvdBase;
    ZeroMem (&FwUpdStatus, sizeof(FwUpdStatus));
    SpiFlashRead (FlashRegionBios, OffsetInBiosRgn, sizeof(FwUpdStatus), (VOID *)&FwUpdStatus);
    if (CheckStateMachine (&FwUpdStatus) == EFI_SUCCESS) {
      return TRUE;
    }
  }

  // Check if platform firmware update trigger is set.
  FwuReq = (MmioRead32 (PMC_BASE_ADDRESS + R_PMC_BIOS_SCRATCHPAD) >> 16) & 0xFF;
  if ((FwuReq & 0x0F) != 0) {
    return TRUE;
  }

  return FALSE;
}

/**
  Init spi controller.

**/
VOID
SpiControllerInitialize (
  VOID
  )
{
  UINTN                           SpiBaseAddress;

  // Allow SPI write in non-SMM mode
  SpiBaseAddress = GetDeviceAddr (OsBootDeviceSpi, 0);
  SpiBaseAddress = TO_MM_PCI_ADDRESS (SpiBaseAddress);

  MmioWrite32 (SpiBaseAddress + R_LPSS_IO_BAR, SPI_TEMP_MEM_BASE_ADDRESS);
  MmioAnd8    (SpiBaseAddress + R_SPI_BCR, (UINT8)~B_SPI_BCR_EISS);

  SpiConstructor ();
}


/**
  Initialize TPM.
**/
VOID
TpmInitialize (
  VOID
  )
{
  EFI_STATUS                   Status;
  UINT8                        BootMode;
  PLATFORM_DATA               *PlatformData;

  BootMode = GetBootMode();
  PlatformData = (PLATFORM_DATA *)GetPlatformDataPtr ();

  if ((PlatformData != NULL) && (PlatformData->BtGuardInfo.Bpm.Mb)) {
    Status = TpmInit (0, BootMode);

    if (EFI_SUCCESS == Status) {
      if (BootMode != BOOT_ON_S3_RESUME) {
        // Create and add BootGuard Event logs in TCG Event log
        ExtendBtGuardEvents (& (PlatformData->BtGuardInfo));
      }
    } else {
      CpuHalt ("Tpm Initialization failed !!\n");
    }
  } else {
    DisableTpm();
  }
}

/**
  Load the configuration data blob from media into destination buffer.

  @param[in]    Dst        Destination address to load configuration data blob.
  @param[in]    Src        Source address to load configuration data blob.
  @param[in]    Len        The destination address buffer size.

  @retval  EFI_SUCCESS             Configuration data blob was loaded successfully.
  @retval  EFI_NOT_FOUND           Configuration data blob cannot be found.
  @retval  EFI_OUT_OF_RESOURCES    Destination buffer is too small to hold the
                                   configuration data blob.
  @retval  Others                  Failed to load configuration data blob.

**/
EFI_STATUS
EFIAPI
LoadExternalConfigData (
  IN UINT32  Dst,
  IN UINT32  Src,
  IN UINT32  Len
  )
{


  return SpiLoadExternalConfigData (Dst, Src, Len);

}

/**
  Get the reset reason from the PMC registers.
**/
VOID
EFIAPI
UpdateResetReason (
  VOID
  )
{
  RESET_CAUSE              RstCause;
  UINT32                   PmConf1;
  UINT32                   Bar;
  UINT32                   TcoStatus;

  RstCause = ResetUnknown;

  // In BIOS, it checks S3 at first.
  if (GetBootMode() == BOOT_ON_S3_RESUME) {
    RstCause = ResetWakeS3;
  }

  Bar     = MmioRead32 (MM_PCI_ADDRESS (0, PCI_DEVICE_NUMBER_PMC, PCI_FUNCTION_NUMBER_PMC, R_PMC_BASE)) & ~0x0F;
  PmConf1 = MmioRead32 (Bar + R_PMC_GEN_PMCON_1);

  TcoStatus = IoRead32(ACPI_BASE_ADDRESS + R_TCO_STS);

  //DetermineBootMode
  if (TcoStatus & B_TCO_STS_SECOND_TO) {
    RstCause = ResetTcoWdt;
  } else if ((PmConf1 & 0x08000000) != 0) {                                            // .cold_boot_sts=1
    RstCause = ResetPowerOn;
  } else if ((PmConf1 & 0x04000000) != 0 && (RstCause != ResetWakeS3)) {      // .cold_reset_sts=1
    RstCause = ResetCold;
  } else if ((PmConf1 & 0x02000000) != 0 && (RstCause != ResetWakeS3)) {      // .warm_reset_sts=1
    RstCause = ResetWarm;
  } else if ((PmConf1 & 0x01000000) != 0) {                                     // .global_reset_sts=1
    RstCause = ResetGlobal;
  }

  SetResetReason (RstCause);

}

typedef struct {
  UINT16  Community   : 4;
  UINT16  PadNum      : 8;
  UINT16  Term        : 4;
} BOOT_MODE_GPIO_PIN_IN;

typedef struct {
  UINT16  Community   : 4;
  UINT16  PadNum      : 8;
  UINT16  GpioTxState : 1;
  UINT16  Rsvd        : 3;
} BOOT_MODE_GPIO_PIN_OUT;

typedef union {
  BOOT_MODE_GPIO_PIN_IN   GpioPinIn;
  BOOT_MODE_GPIO_PIN_OUT  GpioPinOut;
  UINT16                  Uint16;
} BOOT_MODE_GPIO_PIN;

typedef struct {
  UINT16              Invert;
  UINT16              Mask;
  UINT16              Alignment;
  BOOT_MODE_GPIO_PIN  Out;
  BOOT_MODE_GPIO_PIN  In[0];
} BOOT_MODE_GPIO;

STATIC
UINT16
DecodeBits (
  IN  UINT16  Mask,
  IN  UINT16  Bits
  )
{
  UINT16 M;
  UINT16 B;
  UINT16 Data16;

  for (M = 1, B = 1, Data16 = 0; Mask; Mask &= ~M, M <<= 1) {
    if ((Mask & M) == 0) {
      continue;
    }

    if ((Bits & B) != 0) {
      Data16 |= M;
    }
    B <<= 1;
  }

  return Data16;
}

/**
  Get boot mode value from config data

  @param[in]  Cdata             Config data address
  @param[in]  CdataLen          The length of config data
  @param[out] BootModeOption    Parsed boot mode value

  @retval RETURN_SUCCESS        Getting boot mode value succeeded.
  @retval RETURN_DEVICE_ERROR   Getting boot mode value could not be completed.

**/
RETURN_STATUS
EFIAPI
GetBootModeOption (
  IN  VOID    *Cdata,
  IN  UINT32   CdataLen,
  OUT UINT16  *BootModeOption
  )
{
  BOOT_MODE_GPIO          *BootModeGpio;
  UINT32                  NumOfPins;
  INT32                   Index;
  UINT16                  Data16;
  UINT8                   Community;
  UINT16                  Offset;
  BXT_CONF_PAD0           PadConfig0;
  BXT_CONF_PAD1           PadConfig1;

  BootModeGpio = (BOOT_MODE_GPIO *)Cdata;
  NumOfPins = (CdataLen - sizeof (BOOT_MODE_GPIO)) / sizeof (BOOT_MODE_GPIO_PIN);

  DEBUG ((DEBUG_VERBOSE, "BOOT_MODE_GPIO\n Invert=0x%04X\n Mask=0x%04X\n Alignment=0x%04X\n NumOfPins=%d\n", \
    BootModeGpio->Invert, \
    BootModeGpio->Mask, \
    BootModeGpio->Alignment,
    NumOfPins));

  Index = (INT32) (NumOfPins - 1);
  Data16 = 0;
  do {
    Community = mCommIdxTbl[BootModeGpio->In[Index].GpioPinIn.Community];
    Offset = (GPIO_PADBAR + BootModeGpio->In[Index].GpioPinIn.PadNum * 8);

    DEBUG ((DEBUG_VERBOSE, " GpioPinIn[%d]=0x%04X\n Community=%d\n Offset=0x%04X Term=0x%X\n", \
      Index, BootModeGpio->In[Index].Uint16, Community, Offset, \
      BootModeGpio->In[Index].GpioPinIn.Term));

    PadConfig0.padCnf0 = GpioRead (Community, Offset + BXT_GPIO_PAD_CONF0_OFFSET);
    PadConfig0.r.PMode = M0;
    PadConfig0.r.GPIORxTxDis = 1;
    GpioWrite (Community, Offset + BXT_GPIO_PAD_CONF0_OFFSET, PadConfig0.padCnf0);

    if (BootModeGpio->In[Index].GpioPinIn.Term) {
      PadConfig1.padCnf1 = GpioRead (Community, Offset + BXT_GPIO_PAD_CONF1_OFFSET);
      PadConfig1.r.IOSTerm = 3;
      PadConfig1.r.Term = BootModeGpio->In[Index].GpioPinIn.Term;
      GpioWrite (Community, Offset + BXT_GPIO_PAD_CONF1_OFFSET, PadConfig1.padCnf1);

      (VOID)GpioRead (Community, Offset + BXT_GPIO_PAD_CONF1_OFFSET);
    }

    PadConfig0.padCnf0 = GpioRead (Community, Offset + BXT_GPIO_PAD_CONF0_OFFSET);
    Data16 = (Data16 << 1) | (UINT16)PadConfig0.r.GPIORxState;

    Index--;
  } while (Index >= 0);

  Data16 = DecodeBits (BootModeGpio->Mask, Data16);
  *BootModeOption = (Data16 ^ BootModeGpio->Invert);

  Community = mCommIdxTbl[BootModeGpio->Out.GpioPinOut.Community];
  Offset = (GPIO_PADBAR + BootModeGpio->Out.GpioPinOut.PadNum * 8);
  DEBUG ((DEBUG_VERBOSE, " GpioPinOut=0x%04X\n Community=%d\n Offset=0x%04X GpioTxState=0x%X\n", \
      BootModeGpio->Out.Uint16, Community, Offset, BootModeGpio->Out.GpioPinOut.GpioTxState));

  PadConfig0.padCnf0 = GpioRead (Community, Offset + BXT_GPIO_PAD_CONF0_OFFSET);
  PadConfig0.r.PMode = M0;
  PadConfig0.r.GPIORxTxDis = 2;
  PadConfig0.r.GPIOTxState = (BootModeGpio->Out.GpioPinOut.GpioTxState & BIT0);
  GpioWrite (Community, Offset + BXT_GPIO_PAD_CONF0_OFFSET, PadConfig0.padCnf0);

  return RETURN_SUCCESS;
}

/**
  IOC initialized and get configuration data.

  @retval  EFI_SUCCESS     Configuration data was loaded successfully.
  @retval  Others          Failed to get configuration data blob.
**/
EFI_STATUS
EFIAPI
IocInitialize (
  IN  VOID
  )
{
  EFI_STATUS              Status;
  IOC_UART_CFG_DATA       *IocUartData;
  IOC_IPC_CONFIG_DATA     IocIpcConfig;
  UINT32                  Data32;
  CHAR8                   Buffer[IOC_IPC_PACKET_LENGTH_MAX];
  UINT32                  Length;
  CDATA_HEADER            *CdataHdr;
  VOID                    *Cdata;
  UINT16                  BootModeOption;
  PLATFORM_DATA           *PlatformData;
  UINT32                  PciBar;

  if (GetPlatformId () != PLATFORM_ID_GPMRB) {
    return EFI_UNSUPPORTED;
  }

  DEBUG ((DEBUG_INFO, "IocInitialize\n"));
  IocUartData = (IOC_UART_CFG_DATA *)FindConfigDataByTag (CDATA_IOC_UART_TAG);
  if (IocUartData == NULL) {
    DEBUG ((DEBUG_WARN, "CDATA_IOC_UART_TAG Not Found\n"));
    return EFI_UNSUPPORTED;
  }

  DEBUG ((DEBUG_INFO, "IOC_UART_CFG_DATA DeviceIndex=%d BaudRate=%d Retries=%d " \
          "TimeoutInitial=%d TimeoutXmit=%d\n", \
          IocUartData->DeviceIndex, \
          IocUartData->BaudRate, \
          IocUartData->Retries, \
          IocUartData->TimeoutInitial, \
          IocUartData->TimeoutXmit));

  ASSERT (IocUartData->DeviceIndex < 4);   // uart num max
  ASSERT (IocUartData->DeviceIndex != 2);  // debug serial port
  ASSERT (IocUartData->BaudRate < 7);      // supports up to 7 baud rate in cfgblob

  //
  // Prepare IPC device
  //
  Status = UartPortInitialize (IocUartData->DeviceIndex);
  ASSERT_EFI_ERROR (Status);

  PciBar = (UINT32)GetUartBaseAddress (IocUartData->DeviceIndex);
  ASSERT (PciBar != 0xFFFFFFFF);

  Status = UartGpioInitialize (IocUartData->DeviceIndex);
  ASSERT_EFI_ERROR (Status);

  //
  // Prepare IPC driver
  //
  IocIpcConfig.IpcDeviceType                      = IocIpcDeviceTypeUart;
  IocIpcConfig.IpcDevice.IpcDeviceUart.BaseAddr   = PciBar;
  IocIpcConfig.IpcDevice.IpcDeviceUart.BaudRate   = mBaudRate[IocUartData->BaudRate];
  IocIpcConfig.IpcDevice.IpcDeviceUart.InputHertz = mInputHertz[IocUartData->BaudRate];
  IocIpcConfig.IpcDevice.IpcDeviceUart.RegWidth   = 4;
  IocIpcConfig.IpcMisc.Retries                    = IocUartData->Retries;
  IocIpcConfig.IpcMisc.TimeoutInitial             = IocUartData->TimeoutInitial;
  IocIpcConfig.IpcMisc.TimeoutXmit                = IocUartData->TimeoutXmit;

  ZeroMem (Buffer, sizeof (Buffer));
  Length = QueryConfigDataFromIoc (&IocIpcConfig, (VOID *)Buffer);

  // By default CLOCK register will be reset to 0 in cold/S3 boot flow.
  // Here clear CLOCK register to avoid OS issue in S3 path.
  MmioWrite32 (PciBar + R_LPSS_IO_MEM_PCP, 0);
  MmioRead32  (PciBar + R_LPSS_IO_MEM_PCP);

  if (Length < 6) {
    return EFI_TIMEOUT;
  }
  DumpHex (2, 0, Length, (VOID *)Buffer);

  CdataHdr = (CDATA_HEADER *)&Buffer[2];
  DEBUG ((DEBUG_VERBOSE, "Config Data from IOC\n" \
          "  Tag=0x%04X ConditionNum=0x%04X Length=0x%04X Flags=0x%04X Version=0x%04X\n", \
          CdataHdr->Tag, \
          CdataHdr->ConditionNum, \
          CdataHdr->Length * sizeof (UINT32), \
          CdataHdr->Flags, \
          CdataHdr->Version));

  Data32  = sizeof (CDATA_HEADER) + sizeof (CDATA_COND) * CdataHdr->ConditionNum;
  Cdata   = (VOID *) ((UINT8 *)CdataHdr + Data32);
  Length  = CdataHdr->Length * sizeof (UINT32) - Data32;
  DumpHex (2, 0, Length, (VOID *)Cdata);

  if (CdataHdr->Tag == ABL_CDATA_BOOT_MODE_GPIO_TAG) {
    Status = GetBootModeOption (Cdata, Length, &BootModeOption);
    ASSERT_EFI_ERROR (Status);

    PlatformData = (PLATFORM_DATA *)GetPlatformDataPtr ();
    ASSERT (PlatformData != NULL);

    PlatformData->BootModeOption.Uint16 = BootModeOption;
    DEBUG ((DEBUG_INFO, "BootModeOption 0x%04X\n", PlatformData->BootModeOption.Uint16));
  }

  return RETURN_SUCCESS;
}

/**
  Get cpu sku name.

  @retval   cpu sku
**/
CONST CHAR8 *
EFIAPI
GetCpuSkuName (
  VOID
  )
{
  PLATFORM_DATA   *PlatformData;

  PlatformData = (PLATFORM_DATA *)GetPlatformDataPtr ();
  switch (PlatformData->ModuleIdInfo.Bits.PlatformSkuId) {
  case PlatformSkuPremium:
    return "premium";
    break;
  case PlatformSkuHigh:
    return "high";
    break;
  case PlatformSkuMid:
    return "mid";
    break;
  case PlatformSkuLow:
    return "low";
    break;
  default:
    break;
  }

  return "unknown";
}

/**
  Print Board Information.
**/
VOID
EFIAPI
PrintBoardInfo (
  VOID
  )
{
  UINT32 CpuNumCores;
  UINT32 CpuFreq;
  UINT32 UcodeRev;
  CONST CHAR8    *SkuName;
  CONST CHAR8    *DeviceName;
  CONST CHAR8    *SteppingName;

  UcodeRev      = GetCpuUCodeRev ();
  CpuNumCores   = GetCpuNumCores ();
  CpuFreq       = GetCpuMaxNbFrequency ();
  SkuName       = GetCpuSkuName ();
  DeviceName    = GetPchDeviceName ();
  SteppingName  = GetPchSteppingName ();

  DEBUG ((DEBUG_INIT, "CPU : %a-%a [%dC @ %dMHz: %a SKU], uCode rev.%x\n",
          DeviceName, SteppingName, CpuNumCores, CpuFreq, SkuName, UcodeRev));
}

/**
  Print MRC Information.
**/
VOID
EFIAPI
PrintMrcInfo (
  VOID
  )
{
  VOID                     *FspHobList;
  UINT8                    *MrcParamsData;
  UINT32                    MrcVersion;

  MrcParamsData = NULL;
  FspHobList = GetFspHobListPtr ();
  if (FspHobList != NULL) {
    MrcParamsData = GetGuidHobData (FspHobList, NULL, &gFspNonVolatileStorageHobGuid);
  }
  if (MrcParamsData == NULL) {
    DEBUG((DEBUG_ERROR, "MRC: MRC Save Restore Params HOB not valid!\n"));
    return;
  }

  MrcVersion = *(UINT32 *)(MrcParamsData + MRC_PARAMS_BYTE_OFFSET_MRC_VERSION);
  DEBUG ((DEBUG_INIT, "MRC: [v%d.%d.%d.%d]\n",
         (MrcVersion >> 24) & 0xFF,
         (MrcVersion >> 16) & 0xFF,
         (MrcVersion >>  8) & 0xFF,
         (MrcVersion      ) & 0xFF));
}

/**
  Detect board and configure PlatformID.

  @retval  EFI_SUCCESS     Configuration data was loaded successfully.
  @retval  Others          Failed to get configuration data blob.
**/
EFI_STATUS
EFIAPI
PlatformIdInitialize (
  IN  VOID
  )
{
  UINT16     PlatformId;

  PlatformId = (UINT16)GetBoardIdFromGpioPins ();

  if (PlatformId != 0xFF) {
    PlatformId += 0x10; // Customer board identified, assign Platform Ids from 16 to 31
  } else {
    PlatformId = (UINT16)GetEmbeddedBoardId ();
    //Platform ID from GPIOs are read as 0 for Juniper hills due to GPIO pins
    //on the board reduced from 4 to 3 (hardware change) hence translating here
    //in the code.
    if (PlatformId == 0){
        DEBUG ((DEBUG_INFO, "GPIO returned platformID 0 translating to 8(JNH)\n"));
        PlatformId = 0x8;
    }
    if ((PlatformId != PLATFORM_ID_OXH) && (PlatformId != PLATFORM_ID_LFH) && (PlatformId != PLATFORM_ID_JNH)) {
      PlatformId = (UINT16)GetIVIBoardId ();
      if (PlatformId != PLATFORM_ID_GPMRB) {
        DEBUG ((DEBUG_ERROR, "BOARD NOT SUPPORTED: 0x%04X\n", PlatformId));
        CpuDeadLoop ();
      }
    }
  }

  SetPlatformId (PlatformId);
  return EFI_SUCCESS;
}

/**
  Wifi module requires more clock stabilization time on PcieRootPort.
  We have options
    1. clock stabilization in FSP-S
    2. clock stabliization in Stage2 before FSP-S
    3. clock stabilization as soon as pissible (this one)
    ...

    for better perf number, let's do it as soon as possible
**/
VOID
EarlyPcieLinkUp (
  VOID
  )
{
  UINT32              Address;
  UINT8               Data8;
  UINT8               PortIndex;
  UINT8               ClkReqNum;

  if (GetPlatformId () == PLATFORM_ID_GPMRB) {
    GpioConfigurePads (ARRAY_SIZE (mGpioInitWifiTbl), (GPIO_INIT_CONFIG *) (UINTN) mGpioInitWifiTbl);

    // WiFi module specific
    PortIndex = 5;
    ClkReqNum = 0xF;

    Address = MmioRead32 (MM_PCI_ADDRESS (0, PCI_DEVICE_NUMBER_P2SB, PCI_FUNCTION_NUMBER_P2SB, R_P2SB_BASE)) & B_PCH_P2SB_SBREG_RBA;
    Address |= (0xB0 << 16);                                        // PID_FIA: 0xB0
    Address |= (0x100 + (SC_PCIE_ROOT_PORT_FUNC (PortIndex) / 2)); // R_SC_PCR_FIA_DRCRM1: 0x100
    Data8   = MmioRead8 (Address);
    Data8   &= 0x0F;
    Data8   |= (ClkReqNum << 4);
    MmioWrite8 (Address, Data8);
    DEBUG ((DEBUG_INFO, "Address = 0x%08X Value = 0x%X\n", Address, Data8));

    Address = (UINT32)MM_PCI_ADDRESS (0, SC_PCIE_ROOT_PORT_BUS (PortIndex), SC_PCIE_ROOT_PORT_FUNC (PortIndex), 0);
    DEBUG ((DEBUG_INFO, "RpBase = 0x%08X\n", Address));
    MmioOr16 (Address + R_PCH_PCIE_XCAP, B_PCIE_XCAP_SI);
  }
}

/**

**/
VOID
ProcessMbpData (
  VOID
  )
{
  MBP_CMD_RESP_DATA        *MBPHeader;
  MBP_ITEM_HEADER          *MBPItem;
  MBP_CURRENT_BOOT_MEDIA   *MBPCurrentMedia;
  MBP_FW_VERSION_NAME      *MBPVersion;
  PLATFORM_DATA            *PlatformData;
  CDATA_BLOB               *UserCfgData;
  EFI_STATUS               Status;

  MBPHeader = (MBP_CMD_RESP_DATA *)MeGetMeBiosPayloadHob ();
  if (MBPHeader == NULL) {
    DEBUG((DEBUG_ERROR, "CSE: MBP Data HOB not valid!\n"));
    return;
  }

  MBPItem      = (MBP_ITEM_HEADER *)(MBPHeader + 1);
  PlatformData = (PLATFORM_DATA *)GetPlatformDataPtr ();

  while ((UINT32*)MBPItem < (UINT32*)MBPHeader + MBPHeader->Length) {
    if ((MBPItem->Fields.AppId == MBP_APP_ID_KERNEL) && (MBPItem->Fields.ItemId == MBP_ITEM_ID_FW_VER_NAME)) {
      MBPVersion = (MBP_FW_VERSION_NAME *)(MBPItem + 1);
      DEBUG ((DEBUG_INIT, "CSE: FW %d.%d.%d.%d, VB: %d, MB: %d\n",
              MBPVersion->MajorVersion,
              MBPVersion->MinorVersion,
              MBPVersion->HotfixVersion,
              MBPVersion->BuildVersion,
              PlatformData->BtGuardInfo.Bpm.Vb,
              PlatformData->BtGuardInfo.Bpm.Mb));
    }
    if ((MBPItem->Fields.AppId == MBP_APP_ID_NVM) && (MBPItem->Fields.ItemId == MBP_ITEM_ID_CURRENT_BOOT_MEDIA)) {
      MBPCurrentMedia = (MBP_CURRENT_BOOT_MEDIA *)(MBPItem + 1);
      DEBUG((DEBUG_INFO, "CSE: boot dev #%d: %s\n", MBPCurrentMedia->PhysicalData, BootDeviceType[MBPCurrentMedia->PhysicalData]));
    }
    if ((MBPItem->Fields.AppId == MBP_APP_ABL_SIG) && (MBPItem->Fields.ItemId == MBP_ITEM_ID_IAFW_IBB_SIG)) {
      UserCfgData = (CDATA_BLOB *)(MBPItem + 1);
      DumpHex (2, 0, 128, (VOID *)UserCfgData);
      if (UserCfgData->Signature == CFG_DATA_SIGNATURE) {
        Status = AddConfigData ((UINT8 *)UserCfgData);
        if (EFI_ERROR (Status)) {
          DEBUG ((DEBUG_INFO, "Append User CFG Data ... %r\n", Status));
        } else {
          if (FindConfigDataByTag(CDATA_CAPSULE_TAG) != NULL) {
            SetBootMode(BOOT_ON_FLASH_UPDATE);
          }
        }
      }
    }

    MBPItem = (MBP_ITEM_HEADER*)((UINT32*)MBPItem + MBPItem->Fields.Length);

    //
    // Prevent faulty items that could run this loop infinitely
    //
    if (MBPItem->Fields.Length == 0)
      break;

  }
}

/**
  Configure the GPIO output ports

  @param[in] Community             Community of the pad.
  @param[in] PadNum                Pad Numnber.
  @param[in] OutportDrive          Drive value for the output port.
  @param[in] PullUp                If GPIO needs to be pulled up.

  @retval    None.
**/
VOID
ConfigureGpioOutPad (
  UINT32      Community,
  UINT32      PadNum,
  UINT32      OutportDrive,
  UINT8       PullUp
  )
{
  BXT_CONF_PAD0       PadConfg0;
  BXT_CONF_PAD1       PadConfg1;

  if (Community == 0xFF) {
    return;
  }

  PadConfg0.padCnf0 = GpioRead ((UINT8)Community, GPIO_PADBAR + (UINT16)PadNum + BXT_GPIO_PAD_CONF0_OFFSET);
  PadConfg0.r.PMode = M0;
  PadConfg0.r.GPIORxTxDis = GPO;
  PadConfg0.r.GPIOTxState = (PadConfg0.r.GPIOTxState & ~BIT0) | (OutportDrive & 0x1);
  GpioWrite ((UINT8)Community, GPIO_PADBAR + (UINT16)PadNum + BXT_GPIO_PAD_CONF0_OFFSET, PadConfg0.padCnf0);
  if (PullUp != 0) {
    PadConfg1.padCnf1 = GpioRead ((UINT8)Community, GPIO_PADBAR + (UINT16)PadNum + BXT_GPIO_PAD_CONF1_OFFSET);
    PadConfg1.r.IOSTerm = EnPu;
    PadConfg1.r.Term    = PullUp;
    GpioWrite ((UINT8)Community, GPIO_PADBAR + (UINT16)PadNum + BXT_GPIO_PAD_CONF1_OFFSET, PadConfg1.padCnf1);
  }
}

/**
  Initial PCIE RP init for configuring power sequence

  @retval    None.
**/
VOID
PcieRpPwrRstInit (
  VOID
  )
{
  PCIE_RP_CFG_DATA            *PcieRpConfigData;
  PCIE_RP_PIN_CTRL            *PowerResetData;
  UINT8                       Idx1;

  PcieRpConfigData  = (PCIE_RP_CFG_DATA *)FindConfigDataByTag (CDATA_PCIE_RP_TAG);
  if (PcieRpConfigData == NULL) {
    return;
  }

  PowerResetData    = (PCIE_RP_PIN_CTRL *)&PcieRpConfigData->PcieRpReset0;
  for (Idx1 = 0; Idx1 < PCIE_MAX_ROOT_PORTS; Idx1++) {
    if (!PowerResetData->PcieRpReset0.Skip) {
      ConfigureGpioOutPad(PowerResetData->PcieRpReset0.Community, PowerResetData->PcieRpReset0.PadNum, PowerResetData->PcieRpReset0.Drive ^ 1, 0);
    }
    if (!PowerResetData->PcieRpPower0.Skip) {
      ConfigureGpioOutPad(PowerResetData->PcieRpPower0.Community, PowerResetData->PcieRpPower0.PadNum, PowerResetData->PcieRpPower0.Drive ^ 0, 0);
    }
    PowerResetData++;
  }
}


/**
  Read RTC content through its registers.

  @param  Address  Address offset of RTC.

  @return The data of UINT8 type read from RTC.
**/
UINT8
RtcRead (
  IN  UINT8 Address
  )
{
  IoWrite8 (R_RTC_INDEX, (UINT8) (Address | (UINT8) (IoRead8 (R_RTC_INDEX) & 0x80)));
  return IoRead8 (R_RTC_TARGET);
}

/**
  Write RTC through its registers.

  @param  Address  Address offset of RTC.
  @param  Data     The content you want to write into RTC.

**/
VOID
RtcWrite (
  IN  UINT8   Address,
  IN  UINT8   Data
  )
{
  IoWrite8 (R_RTC_INDEX, (UINT8) (Address | (UINT8) (IoRead8 (R_RTC_INDEX) & 0x80)));
  IoWrite8 (R_RTC_TARGET, Data);
}

/**
  Initialize RTC.

  @retval    None.

**/
VOID
RtcInit (
  VOID
  )
{
  UINT32  Bar;
  UINT8   PmConf1;
  UINT8   Data8;

  Bar     = MmioRead32 (MM_PCI_ADDRESS (0, PCI_DEVICE_NUMBER_PMC, PCI_FUNCTION_NUMBER_PMC, R_PMC_BASE)) & ~0x0F;
  PmConf1 = MmioRead8 (Bar + R_PMC_GEN_PMCON_1);

  if ((PmConf1 & B_PMC_GEN_PMCON_RTC_PWR_STS) != 0) {
    DEBUG ((DEBUG_INFO, "Initialize RTC with default values\n"));
    RtcWrite (R_RTC_REGISTERA, 0x66);

    Data8 = RtcRead (R_RTC_REGISTERB);
    Data8 &= ~(BIT2);
    Data8 |= (BIT7 | BIT1);
    RtcWrite (R_RTC_REGISTERB, Data8);

    RtcWrite (R_RTC_REGISTERA, 0x26);

    Data8 = RtcRead (R_RTC_REGISTERB);
    Data8 &= ~(BIT7);
    RtcWrite (R_RTC_REGISTERB, Data8);

    RtcWrite (0x0F, 0x00);

    RtcWrite (R_RTC_REGISTERD, 0x00);
    RtcRead (R_RTC_REGISTERD);

    RtcWrite (R_RTC_HOURSALARM, 0x00);
    RtcWrite (R_RTC_MINUTESALARM, 0x00);
    RtcWrite (R_RTC_SECONDSALARM, 0x00);

    // 11/11/2011 11:11:11
    RtcWrite (R_RTC_SECONDS, 0x11);
    RtcWrite (R_RTC_MINUTES, 0x11);
    RtcWrite (R_RTC_HOURS, 0x11);
    RtcWrite (R_RTC_DAYOFMONTH, 0x11);
    RtcWrite (R_RTC_MONTH, 0x11);
    RtcWrite (R_RTC_YEAR, 0x11);
    RtcWrite (B_RTC_CENTURY, 0x20);

    PmConf1 &= ~(B_PMC_GEN_PMCON_RTC_PWR_STS);
    MmioWrite8 (Bar + R_PMC_GEN_PMCON_1, PmConf1);
  }
}

/**
    Read the Platform Features from the config data
**/
VOID
PlatformFeaturesInit (
  VOID
  )
{
  FEATURES_CFG_DATA           *FeaturesCfgData;
  PLAT_FEATURES               *PlatformFeatures;
  DYNAMIC_CFG_DATA            *DynamicCfgData;
  PLATFORM_DATA               *PlatformData;
  UINT32                       LdrFeatures;

  // Set common features
  LdrFeatures  = GetFeatureCfg ();
  LdrFeatures |= FeaturePcdGet (PcdAcpiEnabled)?FEATURE_ACPI:0;
  LdrFeatures |= FeaturePcdGet (PcdVerifiedBootEnabled)?FEATURE_VERIFIED_BOOT:0;
  LdrFeatures |= FeaturePcdGet (PcdMeasuredBootEnabled)?FEATURE_MEASURED_BOOT:0;

  // Update feature by configuration data.
  FeaturesCfgData = (FEATURES_CFG_DATA *) FindConfigDataByTag(CDATA_FEATURES_TAG);
  if (FeaturesCfgData != NULL) {
    if (FeaturesCfgData->Features.Acpi == 0) {
      LdrFeatures &= ~FEATURE_ACPI;
    }

    if (FeaturesCfgData->Features.MeasuredBoot == 0) {
      LdrFeatures &= ~FEATURE_MEASURED_BOOT;
    }

    if (FeaturesCfgData->Features.eMMCTuning != 0) {
      LdrFeatures |= FEATURE_MMC_TUNING;
    }

    // Update platform specific feature from configuration data.
    PlatformFeatures           = &((PLATFORM_DATA *)GetPlatformDataPtr ())->PlatformFeatures;
    PlatformFeatures->Vt       = FeaturesCfgData->Features.Vt;
    PlatformFeatures->DciDebug = FeaturesCfgData->Features.DciDebug;
  } else {
    DEBUG ((DEBUG_INFO, "FEATURES CFG DATA NOT FOUND!\n"));
  }

  // Update features by boot guard profile
  PlatformData  = (PLATFORM_DATA *)GetPlatformDataPtr ();
  if (PlatformData != NULL) {
    if (PlatformData->BtGuardInfo.Bpm.Mb == 0) {
      LdrFeatures &= ~FEATURE_MEASURED_BOOT;
    }
    if (PlatformData->BtGuardInfo.Bpm.Vb == 0) {
      LdrFeatures &= ~FEATURE_VERIFIED_BOOT;
    }
  }

  // Update features by dynamic configuration data
  DynamicCfgData = (DYNAMIC_CFG_DATA *) FindConfigDataByTag (CDATA_DYNAMIC_TAG);
  if ((DynamicCfgData != NULL) && (DynamicCfgData->EmmcTuningEnforcement != 0)) {
    LdrFeatures |= FEATURE_MMC_FORCE_TUNING;
  }

  SetFeatureCfg (LdrFeatures);
}

/**
    USB3, PCie, SATA, eDP, DP, eMMC, SD and SDIO SSC
    Partially implemented PeiHighSpeedSerialInterfaceSSCInit and PeiDDRSSCInit
    to disable SSC in LCPLL_CTRL_1 and LJ1PLL_CTRL
**/
STATIC
VOID
EFIAPI
SetSscDisable (
  VOID
  )
{
  EFI_STATUS                        Status;
  LCPLL_CR_RW_CONTROL_1             LCPLL_CTRL_1;
  LCPLL_CR_RW_CONTROL_2             LCPLL_CTRL_2;
  UINT32                            BufferSize = 0;
  SSC_IPC_BUFFER                    WBuf;

  //
  // static table for the SSC settings (corresponding with the SSC settings 0~-0.5%, 0.1% stepping)
  // Modulation Freq = 32KHz
  //
  SSC_SETTING                     SSC_Select_Table[] = {{No_SSC, 0x12B, 0},
                                                        {M01_SSC, 0x12B, 0x1062},
                                                        {M02_SSC, 0x12B, 0x2BB0},
                                                        {M03_SSC, 0x12B, 0x46FF},
                                                        {M04_SSC, 0x12B, 0x624D},
                                                        {M05_SSC, 0x12B, 0x7D9C}};

  //
  //static table for the clock bending settings (corresponding with the clock bending settings 1.3%, 0.6%, 0, -0.9%)
  //
  CLOCK_BENDING_SETTING           CLK_Bending_Table[] = {{Clk_Bending_13, 0xA00000, 0x7E},
                                                         {Clk_Bending_06, 0xC00000, 0x7D},
                                                         {No_Clk_Bending, 0x0, 0x7D},
                                                         {Clk_Bending_M09, 0xDB6C20, 0x7B}};

  DEBUG ((DEBUG_INFO, "SetSscDisable()\n"));
  //
  // default value of the 4 SSC setting registers
  //
  WBuf.LJ1PLL_CTRL_1.Data = 0x00;
  WBuf.LJ1PLL_CTRL_2.Data = 0x0888812B;
  WBuf.LJ1PLL_CTRL_3 = 0x7D000000;
  WBuf.LJ1PLL_CTRL_5.Data = 0x7D000000;
  BufferSize = sizeof (UINT32) * 4;

  //
  // Set default value of SSC
  //
  WBuf.LJ1PLL_CTRL_2.Fields.ssc_cyc_to_peak_m1 = SSC_Select_Table[SSC_DEFAULT_SETTING].Ssc_Cyc_To_Peak;
  WBuf.LJ1PLL_CTRL_2.Fields.ssc_frac_step = SSC_Select_Table[SSC_DEFAULT_SETTING].Ffs_Frac_Step;
  //
  // Set default value of Clock bending
  //
  WBuf.LJ1PLL_CTRL_5.Fields.pll_ratio_frac = CLK_Bending_Table[CLK_BENDING_DEFAULT_SETTING].Pll_Ratio_Frac;
  WBuf.LJ1PLL_CTRL_5.Fields.pll_ratio_int = CLK_Bending_Table[CLK_BENDING_DEFAULT_SETTING].Pll_Ratio_Int;

  //
  // send the IPC command for SSC
  //
  IpcSendCommandEx (IPC_CMD_ID_EMI_RFI_SUPPORT, IPC_SUBCMD_ID_SSC_APPLY_NOW, &WBuf, BufferSize);

  //
  // Delay for 1ms to avoid the SSC doesn't set correctly sometimes
  //
  MicroSecondDelay (1000);

  //
  // set the ssc_en to Disable!
  //
  WBuf.LJ1PLL_CTRL_1.Fields.ssc_en = SSC_DISABLE;
  WBuf.LJ1PLL_CTRL_1.Fields.ssc_en_ovr = SSC_DISABLE;
  Status = IpcSendCommandEx (IPC_CMD_ID_EMI_RFI_SUPPORT, IPC_SUBCMD_ID_SSC_APPLY_NOW, &WBuf, BufferSize);
  if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "\nFailed to disable LJ1PLL_CTRL SSC \n\r"));
  }

  //
  // Setting LCPLL_CTRL
  //
  LCPLL_CTRL_1.Data = SideBandRead32 (0x99, 0x9910);
  LCPLL_CTRL_2.Data = SideBandRead32 (0x99, 0x9914);
  DEBUG ((DEBUG_INFO, "LCPLL_CTRL_1 register: 0x%02X\n", LCPLL_CTRL_1.Data));
  DEBUG ((DEBUG_INFO, "LCPLL_CTRL_2 register: 0x%02X\n", LCPLL_CTRL_2.Data));

  // disable SSC
  LCPLL_CTRL_1.Fields.ssc_en = SSC_DISABLE;
  LCPLL_CTRL_1.Fields.ssc_en_ovr = SSC_DISABLE;
  DEBUG ((DEBUG_INFO, "LCPLL_CTRL_1 write data: 0x%02X\n", LCPLL_CTRL_1.Data));
  SideBandWrite32 (0x99, 0x9910, LCPLL_CTRL_1.Data);
}

/**
    Control SSC enable function
**/
VOID
SetPlatformSsc (
  IN  BOOLEAN  Enable )
{
  if (!Enable) {
    SetSscDisable();
  }
}

/**
  Board specific hook points.

  Implement board specific initialization during the boot flow.

  @param[in] InitPhase             Current phase in the boot flow.

**/
VOID
EFIAPI
BoardInit (
  IN  BOARD_INIT_PHASE  InitPhase
  )
{
  PLATFORM_DATA            *PlatformData;
  PLT_DEVICE_TABLE         *PltDeviceTable;

  switch (InitPhase) {
  case PreConfigInit:
    PltDeviceTable = (PLT_DEVICE_TABLE *)AllocatePool (sizeof (PLT_DEVICE_TABLE) + sizeof (mPlatformDevices));
    PltDeviceTable->DeviceNumber = sizeof (mPlatformDevices) /sizeof (PLT_DEVICE);
    CopyMem (PltDeviceTable->Device, mPlatformDevices, sizeof (mPlatformDevices));
    SetDeviceTable (PltDeviceTable);
    PrintBoardInfo ();
    EarlyBootDeviceInit ();
    SpiControllerInitialize ();
    break;
  case PostConfigInit:
    if (GetPlatformId () == 0) {
      // Platform ID has not been initialzied yet
      PlatformIdInitialize ();
    }
    PlatformNameInit ();
    ModuleIdInitialize ();
    SetBootMode (IsFirmwareUpdate() ? BOOT_ON_FLASH_UPDATE : GetPlatformPowerState());
    UpdateResetReason ();
    IocInitialize ();
    PcieRpPwrRstInit ();
    RtcInit ();
    PlatformFeaturesInit ();
    SetPlatformSsc (TRUE);

    break;
  case PreMemoryInit:
    PlatformData = (PLATFORM_DATA *)GetPlatformDataPtr ();
    if (PlatformData != NULL) {
      FetchPostRBPData (& (PlatformData->BtGuardInfo));
      DEBUG ((DEBUG_INFO, "BootPolicy : 0x%08X\n", PlatformData->BtGuardInfo.Bpm));
    }
    GpioConfigurePads (sizeof (mGpioInitTbl) / sizeof (mGpioInitTbl[0]), (GPIO_INIT_CONFIG *) (UINTN) mGpioInitTbl);
    EarlyPcieLinkUp ();
    break;
  case PostMemoryInit:
    UpdateMemoryInfo ();
    break;
  case PreTempRamExit:
    break;
  case PostTempRamExit:
    if (MEASURED_BOOT_ENABLED() ) {
      TpmInitialize();
    }
    ProcessMbpData ();
    PrintMrcInfo ();
    break;
  default:
    break;
  }
}

/**
  Search for the saved MrcParam to initialize Memory for fastboot.

  @retval Found MrcParam or NULL
**/
VOID *
EFIAPI
FindNvsData (
  VOID
  )
{
  UINT8                          *CompressedData;
  VOID                           *MemPool;
  VOID                           *MrcVarData;
  VOID                           *MrcParamData;
  MRC_VAR_HDR                    *MrcVarHdr;
  MRC_PARAM_HDR                   MrcParamHdr;
  UINT32                          RegionSize;
  UINT32                          DataSize;
  UINT32                          MrcNvDataOffset;
  UINT32                          MrcParamsOffset;
  PLATFORM_DATA                  *PlatformData;
  EFI_STATUS                      Status;
  UINT32                          ActIdx;
  UINT32                          Idx;
  UINT8                           Data8;
  UINT32                          MrcDataBase;
  UINT32                          MrcDataSize;
  UINT32                          Offset;
  DYNAMIC_CFG_DATA               *DynCfgData;

  DynCfgData = (DYNAMIC_CFG_DATA *)FindConfigDataByTag (CDATA_DYNAMIC_TAG);
  if (DynCfgData != NULL) {
    if (DynCfgData->MrcTrainingEnforcement) {
      DEBUG ((DEBUG_INFO, "MRC Training Enforcement enabled, providing NULL pointer for NVS Data!\n"));
      return NULL;
    }
  }

  // Read current MRC DATA CRC
  RegionSize      = SIZE_4KB;
  MrcNvDataOffset = 0;
  MrcParamsOffset = MrcNvDataOffset + RegionSize;

  // All MRC NVS data should be less than 64KB
  MrcVarData   = (VOID *)(UINTN)APL_FSP_STACK_TOP;
  MrcParamData = (UINT8 *)MrcVarData + SIZE_1KB;
  MemPool      = (UINT8 *)MrcVarData + SIZE_64KB;

  DEBUG ((DEBUG_INFO, "Looking for MRC saving data\n"));
  do {
    Status = GetComponentInfo (FLASH_MAP_SIG_MRCDATA, &MrcDataBase, &MrcDataSize);
    if (EFI_ERROR(Status)) {
      Status = EFI_NOT_FOUND;
      break;
    }

    CopyMem (&MrcParamHdr,  (UINT8 *)(UINTN)MrcDataBase + MrcParamsOffset, sizeof (MRC_PARAM_HDR));
    if (MrcParamHdr.Signature != MRC_PARAM_SIGNATURE) {
      Status = EFI_NOT_FOUND;
      break;
    }

    CompressedData = (UINT8 *)MemPool + sizeof (MRC_PARAM_HDR);
    DataSize       = MrcParamHdr.Length - sizeof (MRC_PARAM_HDR);

    DEBUG ((DEBUG_INFO, "Read MRC ParamData at 0x%X\n", MrcDataBase + MrcParamsOffset));
    CopyMem (CompressedData, (UINT8 *)(UINTN)MrcDataBase + MrcParamsOffset + sizeof (MRC_PARAM_HDR), DataSize);

    DEBUG ((DEBUG_INFO, "Decompress ParamData\n"));
    DataSize = (UINT32)RleDecompressData (CompressedData, DataSize, MrcParamData);

    DEBUG ((DEBUG_INFO, "Read MRC VarData at 0x%X\n", MrcDataBase + MrcNvDataOffset));
    MrcVarHdr = (MRC_VAR_HDR *)MemPool;
    CopyMem ((UINT8 *)MrcVarHdr, (UINT8 *)(UINTN)MrcDataBase + MrcNvDataOffset,  sizeof (MRC_VAR_HDR));

    ActIdx = 0xFF;
    if (MrcVarHdr->Signature == MRC_VAR_SIGNATURE) {
      if (MrcVarHdr->SlotNum > 0) {
        // Set default slot to be the last entry
        ActIdx = MrcVarHdr->SlotNum;
      }
      for (Idx = 0; Idx < MrcVarHdr->SlotNum >> 3; Idx++) {
        Data8 = MrcVarHdr->SlotMap[Idx];
        if (Data8 != 0) {
          ActIdx = Idx * 8;
          while ((Data8 & BIT0) == 0) {
            ActIdx++;
            Data8 >>= 1;
          }
          break;
        }
      }
    }
    if (ActIdx == 0xFF) {
      Status = EFI_NOT_FOUND;
      break;
    }

    // Adjust index to be 0 based
    ActIdx--;
    DEBUG ((DEBUG_INFO, "Use slot %d\n", ActIdx));

    // Read NV data from the slot
    Offset = MrcNvDataOffset + sizeof (MRC_VAR_HDR) + ActIdx * MRC_VAR_SLOT_LENGTH;
    CopyMem ((UINT8 *)MrcVarData, (UINT8 *)(UINTN)MrcDataBase + Offset, MRC_VAR_LENGTH);

  } while (EFI_ERROR (Status));

  PlatformData = (PLATFORM_DATA *)GetPlatformDataPtr ();
  if (EFI_ERROR (Status)) {
    // Mark MRC NVS data as invalid
    PlatformData->MrcParamHdr.Signature = 0;
    ZeroMem (PlatformData->MrcVarData,  sizeof (PlatformData->MrcVarData));
    DEBUG ((DEBUG_INFO, "MRC data NOT found\n"));
    return NULL;
  } else {
    // Remember the MRC NVS data CRC in platform data
    // Stage2 will use it to determine the data is the same or not
    CopyMem (&PlatformData->MrcParamHdr, &MrcParamHdr, sizeof (PlatformData->MrcParamHdr));
    CopyMem (PlatformData->MrcVarData,   MrcVarData,   sizeof (PlatformData->MrcVarData));
    DEBUG ((DEBUG_INFO, "MRC data was found @ 0x%08X(Param) and 0x%08X(Var)\n", MrcParamData, MrcVarData));
    return MrcParamData;
  }
}
