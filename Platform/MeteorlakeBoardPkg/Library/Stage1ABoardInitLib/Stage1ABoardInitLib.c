/** @file

  Copyright (c) 2020 - 2023, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/


#include <PiPei.h>
#include <Library/BaseLib.h>
#include <Library/BoardInitLib.h>
#include <Library/SerialPortLib.h>
#include <Library/PlatformHookLib.h>
#include <Library/BootloaderCoreLib.h>
#include <PchAccess.h>
#include <FsptUpd.h>
#include <PlatformData.h>
#include <Library/GpioLib.h>
#include <GpioConfig.h>
#include <GpioPinsVer4S.h>
#include <GpioPinsVer2Lp.h>
#include <GpioV2PinsMtlSoc.h>
#include <Library/ConfigDataLib.h>
#include <Library/PchInfoLib.h>
#include <Library/MtlSocInfoLib.h>
#include <Register/GpioRegsVer6.h>
#include <Register/GpioV2ChipsetId.h>
#include <Include/GpioV2ControllerInterface.h>
#include <Register/PmcRegsVer6.h>
#include <Include/GpioV2Pad.h>
#include <Include/P2SbSidebandAccessLib.h>
#include <Include/RegisterAccess.h>
#include <Include/P2SbController.h>
#include <Library/MtlSocGpioTopologyLib.h>
#include <Library/GpioNativePads.h>

#define UCODE_REGION_BASE   FixedPcdGet32(PcdUcodeBase)
#define UCODE_REGION_SIZE   FixedPcdGet32(PcdUcodeSize)
#define SG1B_REDB_BASE      (UINT32) ((2 * FixedPcdGet32(PcdTopSwapRegionSize)) + FixedPcdGet32(PcdRedundantRegionSize) + FixedPcdGet32(PcdStage1BSize))
#define CODE_REGION_SIZE    ALIGN_UP (SG1B_REDB_BASE, SIZE_1MB)
#define CODE_REGION_BASE    (UINT32) (BASE_4GB - CODE_REGION_SIZE)

#define MTL_MAX_SERIALIO_UART_CONTROLLERS     3

CONST
FSPT_UPD TempRamInitParams = {
  .FspUpdHeader = {
    .Signature = FSPT_UPD_SIGNATURE,
    .Revision  = 2,
    .Reserved  = {0},
  },
  .FsptArchUpd = {
    .Revision = 1,
    .Length = 0x20,
    .Reserved  = {0},
  },
  .FsptCoreUpd = {
    .MicrocodeRegionBase    = UCODE_REGION_BASE,
    .MicrocodeRegionSize    = UCODE_REGION_SIZE,
    .CodeRegionBase         = CODE_REGION_BASE,
    .CodeRegionSize         = CODE_REGION_SIZE,
    .Reserved               = {0},
  },
  .FsptConfig = {
    .PcdSerialIoUartDebugEnable = 1,
    .PcdSerialIoUartNumber      = FixedPcdGet32 (PcdDebugPortNumber),
    .PcdSerialIoUartMode        = 4,
    .PcdSerialIoUartBaudRate    = 115200,
    .PcdPciExpressBaseAddress   = FixedPcdGet32 (PcdPciMmcfgBase),
    .PcdPciExpressRegionLength  = 0x10000000,
    .PcdSerialIoUartParity      = 1,
    .PcdSerialIoUartDataBits    = 8,
    .PcdSerialIoUartStopBits    = 1,
    .PcdSerialIoUartAutoFlow    = 0,
    .PcdSerialIoUartRxPinMux    = 0,
    .PcdSerialIoUartTxPinMux    = 0,
    .PcdSerialIoUartRtsPinMux   = 0,
    .PcdSerialIoUartCtsPinMux   = 0,
    .PcdLpcUartDebugEnable      = 1,
  },
  .UpdTerminator = 0x55AA,
};

CONST GPIO_INIT_CONFIG mMtlPUartGpioTable[] = {
  {0xA050008 ,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//SERIALIO_UART0_RXD
  {0xA050009 ,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//SERIALIO_UART0_TXD
};

STATIC UINT32 mMtlSocMGpioMapping[] = {
  GPIO_FUNCTION_SERIAL_IO_UART_RX(0),    GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_SOC_M_GPP_H8, 0x1),
  GPIO_FUNCTION_SERIAL_IO_UART_RX(2),    GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_SOC_M_GPP_F1, 0x2),
  GPIO_FUNCTION_SERIAL_IO_UART_TX(0),    GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_SOC_M_GPP_H9, 0x1),
  GPIO_FUNCTION_SERIAL_IO_UART_TX(2),    GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_SOC_M_GPP_F2, 0x2),
  GPIO_FUNCTION_SERIAL_IO_UART_RTS(0),   GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_SOC_M_GPP_H10,0x1),
  GPIO_FUNCTION_SERIAL_IO_UART_RTS(2),   GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_SOC_M_GPP_F0, 0x2),
  GPIO_FUNCTION_SERIAL_IO_UART_CTS(0),   GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_SOC_M_GPP_H11, 0x1),
  GPIO_FUNCTION_SERIAL_IO_UART_CTS(2),   GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_SOC_M_GPP_F3,  0x2),
  GPIO_FUNCTION_SERIAL_IO_SPI_MOSI (1),  GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_SOC_M_GPP_F12, 0x5),
  GPIO_FUNCTION_SERIAL_IO_SPI_MISO (1),  GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_SOC_M_GPP_F13, 0x5),
  GPIO_FUNCTION_SERIAL_IO_SPI_CLK (1),   GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_SOC_M_GPP_F11, 0x5),
  GPIO_FUNCTION_SERIAL_IO_SPI_CS (1, 0), GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_SOC_M_GPP_F17, 0x5),
  GPIO_FUNCTION_SERIAL_IO_I2C_SCL(0),    GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_SOC_M_GPP_H20, 0x1),
  GPIO_FUNCTION_SERIAL_IO_I2C_SCL(1),    GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_SOC_M_GPP_H22, 0x1),
  GPIO_FUNCTION_SERIAL_IO_I2C_SDA(0),    GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_SOC_M_GPP_H19, 0x1),
  GPIO_FUNCTION_SERIAL_IO_I2C_SDA(1),    GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_SOC_M_GPP_H21, 0x1),
  GPIO_FUNCTION_SERIAL_IO_I3C_SCL(0),    GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_SOC_M_GPP_H20, 0x2),
  GPIO_FUNCTION_SERIAL_IO_I3C_SDA(0),    GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_SOC_M_GPP_H19, 0x2),
  GPIO_FUNCTION_SERIAL_IO_I3C_SCL_FB(0), GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_SOC_M_LPI3C0_CLK_LOOPBK, 0x2),
  GPIO_FUNCTION_ISH_GP(0),               GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_SOC_M_GPP_B5,  0x4),
  GPIO_FUNCTION_ISH_GP(1),               GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_SOC_M_GPP_B6,  0x4),
  GPIO_FUNCTION_ISH_GP(2),               GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_SOC_M_GPP_B7,  0x4),
  GPIO_FUNCTION_ISH_GP(3),               GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_SOC_M_GPP_B8,  0x4),
  GPIO_FUNCTION_ISH_GP(4),               GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_SOC_M_GPP_B4,  0x4),
  GPIO_FUNCTION_ISH_GP(7),               GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_SOC_M_GPP_E5,  0x8),
  GPIO_FUNCTION_ISH_UART_RX(0),          GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_SOC_M_GPP_D5,  0x2),
  GPIO_FUNCTION_ISH_UART_TX(0),          GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_SOC_M_GPP_D6,  0x2),
  GPIO_FUNCTION_ISH_UART_RTS(0),         GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_SOC_M_GPP_D7,  0x2),
  GPIO_FUNCTION_ISH_UART_CTS(0),         GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_SOC_M_GPP_D8,  0x2),
  GPIO_FUNCTION_ISH_I2C_SCL(0),          GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_SOC_M_GPP_B3,  0x3),
  GPIO_FUNCTION_ISH_I2C_SCL(1),          GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_SOC_M_GPP_H15, 0x3),
  GPIO_FUNCTION_ISH_I2C_SDA(0),          GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_SOC_M_GPP_B2,  0x3),
  GPIO_FUNCTION_ISH_I2C_SDA(1),          GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_SOC_M_GPP_H14, 0x3),
  GPIO_FUNCTION_ISH_I3C_SCL(0),          GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_SOC_M_GPP_B3,  0x4),
  GPIO_FUNCTION_ISH_I3C_SDA(0),          GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_SOC_M_GPP_B2,  0x4),
  GPIO_FUNCTION_HDA_BCLK,                GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_SOC_M_GPP_D10, 0x1),
  GPIO_FUNCTION_HDA_RSTB,                GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_SOC_M_GPP_D17, 0x1),
  GPIO_FUNCTION_HDA_SYNC,                GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_SOC_M_GPP_D11, 0x1),
  GPIO_FUNCTION_HDA_SDO,                 GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_SOC_M_GPP_D12, 0x1),
  GPIO_FUNCTION_HDA_SDI_0,               GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_SOC_M_GPP_D13, 0x1),
  GPIO_FUNCTION_HDA_SDI_1,               GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_SOC_M_GPP_D16, 0x1),
  GPIO_FUNCTION_DDSP_HPD(1),             GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_SOC_M_GPP_B9,  0x2),
  GPIO_FUNCTION_DDSP_HPD(2),             GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_SOC_M_GPP_B10, 0x2),
  GPIO_FUNCTION_DDSP_HPD(3),             GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_SOC_M_GPP_B11, 0x2),
  GPIO_FUNCTION_DDSP_HPD(4),             GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_SOC_M_GPP_B14, 0x2),
  GPIO_FUNCTION_DDSP_HPD('A'),           GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_SOC_M_GPP_E14, 0x1),
  GPIO_FUNCTION_DDSP_HPD('B'),           GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_SOC_M_GPP_B16, 0x2),
  GPIO_FUNCTION_PANEL_AVDD_EN(0),        GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_SOC_M_VDDEN,   0x1),
  GPIO_FUNCTION_PANEL_BKLTEN(0),         GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_SOC_M_BKLTEN,  0x1),
  GPIO_FUNCTION_PANEL_BKLTCTL(0),        GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_SOC_M_BKLTCTL, 0x1),
  GPIO_FUNCTION_PANEL_AVDD_EN(1),        GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_SOC_M_GPP_B17, 0x2),
  GPIO_FUNCTION_PANEL_BKLTEN(1),         GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_SOC_M_GPP_D1, 0x2),
  GPIO_FUNCTION_PANEL_BKLTCTL(1),        GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_SOC_M_GPP_D2, 0x2),
  GPIO_FUNCTION_CNVI_RF_RESET,           GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_SOC_M_GPP_F4,  0x1),
  GPIO_FUNCTION_CNVI_CLKREQ,             GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_SOC_M_GPP_F5,  0x3),
  GPIO_FUNCTION_DMIC_CLKA(0),            GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_SOC_M_GPP_S2, 0x3),
  GPIO_FUNCTION_DMIC_CLKA(1),            GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_SOC_M_GPP_S6, 0x3),
  GPIO_FUNCTION_DMIC_CLKB(0),            GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_SOC_M_GPP_S4, 0x3),
  GPIO_FUNCTION_DMIC_CLKB(1),            GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_SOC_M_GPP_S5, 0x3),
  GPIO_FUNCTION_DMIC_DATA(0),            GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_SOC_M_GPP_S3, 0x3),
  GPIO_FUNCTION_DMIC_DATA(1),            GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_SOC_M_GPP_S7, 0x3),
  GPIO_FUNCTION_THC_SPI_INT(0),          GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_SOC_M_GPP_E17, 0x2),
  GPIO_FUNCTION_THC_SPI_INT(1),          GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_SOC_M_GPP_F18, 0x3),
  GPIO_FUNCTION_THC_CLK_LOOPBACK(0),     GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_SOC_M_THC0_GPSI0_CLK_LOOPBK, 0x2),
  GPIO_FUNCTION_THC_CLK_LOOPBACK(1),     GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_SOC_M_THC1_GSPI1_CLK_LOOPBK, 0x3),
  GPIO_FUNCTION_THC_WOT(0),              GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_SOC_M_THC_0, 0x0),
  GPIO_FUNCTION_THC_WOT(1),              GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_SOC_M_THC_1, 0x0),
  GPIO_FUNCTION_SATA_DEVSLP(0,0),        GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_SOC_M_GPP_E4,  0x1),
  GPIO_FUNCTION_SATA_DEVSLP(0,1),        GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_SOC_M_GPP_E5,  0x1),
  GPIO_FUNCTION_PCIE_CLKREQ(0),          GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_SOC_M_GPP_C9,  0x1),
  GPIO_FUNCTION_PCIE_CLKREQ(1),          GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_SOC_M_GPP_C10, 0x1),
  GPIO_FUNCTION_PCIE_CLKREQ(2),          GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_SOC_M_GPP_C11, 0x1),
  GPIO_FUNCTION_PCIE_CLKREQ(3),          GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_SOC_M_GPP_C12, 0x1),
  GPIO_FUNCTION_PCIE_CLKREQ(4),          GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_SOC_M_GPP_C13, 0x1),
  GPIO_FUNCTION_PCIE_CLKREQ(5),          GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_SOC_M_GPP_D21, 0x2),
  GPIO_FUNCTION_PCIE_CLKREQ(6),          GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_SOC_M_GPP_D18, 0x1),
  GPIO_FUNCTION_PCIE_CLKREQ(7),          GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_SOC_M_GPP_D19, 0x1),
  GPIO_FUNCTION_PCIE_CLKREQ(8),          GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_SOC_M_GPP_D20, 0x1)
};

static UINT32 mMtlGpioMapping[] = {
  GPIO_FUNCTION_PCIE_CLKREQ(0),           GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_SOC_S_GPP_B7,  1),
  GPIO_FUNCTION_PCIE_CLKREQ(1),           GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_SOC_S_GPP_B14, 1),
  GPIO_FUNCTION_PCIE_CLKREQ(2),           GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_SOC_S_GPP_B0,  3),
  GPIO_FUNCTION_PCIE_CLKREQ(3),           GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_SOC_S_GPP_D7,  1),
  GPIO_FUNCTION_PCIE_CLKREQ(4),           GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_SOC_S_GPP_B8,  1),
  GPIO_FUNCTION_PCIE_CLKREQ(5),           GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_SOC_S_GPP_B9,  1),
  GPIO_FUNCTION_PANEL_AVDD_EN(1),         GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_SOC_S_GPP_B6,  1),
  GPIO_FUNCTION_PANEL_BKLTEN(1),          GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_SOC_S_GPP_B10, 1),
  GPIO_FUNCTION_PANEL_BKLTCTL(1),         GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_SOC_S_GPP_B11, 1)
};

/**
  Stitching process might pass some specific platform data to be
  consumed pretty early. This will be used to guide the platform initialization
  even before CFGDATA is available.

**/
VOID
EarlyPlatformDataCheck (
  VOID
)
{
  STITCH_DATA          *StitchData;

  // Stitching process might pass some platform specific data.
  StitchData = (STITCH_DATA *)(UINTN)(0xFFFFFFF4);

  if (StitchData->Marker != 0xAA) {
    // set default  as Debug UART
    // PlatformID will be deferred to be detected
    SetDebugPort (PcdGet8 (PcdDebugPortNumber));
  } else {
    SetDebugPort  (StitchData->DebugUart);
    if ((StitchData->PlatformId > 0) && (StitchData->PlatformId < 32)) {
      SetPlatformId (StitchData->PlatformId);
    }
  }
}

/**
  This function provides GPIO Native Pad for a given native function
  @param[in]  GpioServices           Gpio Services
  @param[in]  PadFunction            PadFunction for a specific native signal. Please refer to GpioNativePads.h

  @retval     GpioPad                GPIO pad with encoded native function
**/
GPIOV2_PAD
MtlSocGpioGetNativePadByFunction (
  IN  UINT32  PadFunction
  )
{
  UINT32    *MappingTable = NULL;
  UINT32    MapIndex;
  UINT32    MapCount = 0;

  if (MtlIsSocM ()) {
    MappingTable = mMtlSocMGpioMapping;
    MapCount = sizeof(mMtlSocMGpioMapping)/sizeof(UINT32);
  } else {
    MappingTable = mMtlGpioMapping;
    MapCount = sizeof(mMtlGpioMapping)/sizeof(UINT32);
  }

  for (MapIndex = 0 ; MapIndex < MapCount ; MapIndex+=2) {
    if (MappingTable[MapIndex] ==  PadFunction) {
      return MappingTable[MapIndex+1];
    }
  }
  return GPIOV2_PAD_NONE;
}

/**
  This procedure will get GPIO native pad based on provided native function and platform muxing selection (if needed).

  @param[in]  PadFunction         PadMode for a specific native signal. Please refer to GpioNativePads.h
  @param[in]  PinMux              GPIO Native pin mux platform config.
                                  This argument is optional and needs to be
                                  provided only if feature can be enabled
                                  on multiple pads

  @retval  NativePad              GPIO pad with encoded native function
**/
GPIO_NATIVE_PAD
MtlSocGpioGetNativePadByFunctionAndPinMux (
  IN UINT32  PadFunction,
  IN UINT32  PinMux
  )
{
  //
  //  To do : Need to decide how Pin Muxing will be supported once confirmed there will be
  //          Change in following code
  //

  GPIOV2_PAD GpioPad;

  GpioPad = MtlSocGpioGetNativePadByFunction (PadFunction);

  if (GpioPad != GPIOV2_PAD_NONE) {
    //
    // For situations where native signal can be enabled only on
    // a single pad then PinMux argument should be set to 0.
    //
    ASSERT(PinMux == GPIOV2_PAD_NONE);
  } else {
     //
     // If native signal can be enabled on multiple pads
     // then PinMux argument needs to be configured to platform specific value
     GpioPad = PinMux;
  }

  return GpioPad;
}

/**
  This procedure does minimum Gpio Configuration required for Serial Io Devices
  in SEC phase.

  @param[in] GpioPad             GPIO pad
  @param[in] PadModeValue        GPIO pad mode value, please refer to GpioV2Pad.h
  @param[in] TerminationConfig   Termination configuration, please refer to GpioV2Pad.h
  @param[in] InputInversion      GpioV2InputInversionEnable or GpioV2InputInversionDisable, please refer to GpioV2Pad.h

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_NOT_FOUND           Required structure not found.
**/
EFI_STATUS
EFIAPI
SecSerialIoGpioConfigure (
  IN  GPIOV2_PAD                  GpioPad,
  IN  GPIOV2_PAD_MODE             PadMode,
  IN  GPIOV2_PAD_INPUT_INVERSION  InputInversion,
  IN  GPIOV2_TERMINATION_CONFIG   TerminationConfig
  )
{
  P2SB_SIDEBAND_REGISTER_ACCESS   CommunityAccess;
  UINT32                          CommunityIndex;
  EFI_STATUS                      Status;
  UINT32                          RegisterOffset;
  UINT32                          AndValue;
  UINT32                          OrValue;
  P2SB_CONTROLLER                 P2SbController;

  CommunityIndex = GPIOV2_PAD_GET_COMMUNITY_INDEX (GpioPad);

  ZeroMem (&CommunityAccess, sizeof (P2SB_SIDEBAND_REGISTER_ACCESS));

  Status = MtlSocGetP2SbController (&(P2SbController));
  if (EFI_ERROR (Status)) {
    return EFI_NOT_FOUND;
  }

  BuildP2SbSidebandAccess (
      &(P2SbController),
      (P2SB_PID)(MTL_SOC_PID_GPIOCOM0 + CommunityIndex),
      0,
      P2SbMemory,
      P2SbMmioAccess,
      TRUE,
      &CommunityAccess
      );

  //
  //
  //  Get DW0 Register Offset to program Pad Mode and Input Inversion
  //
  Status = MtlSocGpioGetRegOffset(GpioPad, GpioV2Dw0Reg, &RegisterOffset);
  if (EFI_ERROR(Status)) {
    return Status;
  }

  //
  //  Program Pad Mode
  //
  if (PadMode != GpioV2PadModeHardwareDefault) {
    AndValue = (UINT32)~(GPIOV2_PAD_MODE_MASK << GPIOV2_PAD_MODE_DW0_POS);
    OrValue = ((PadMode >> 1) & GPIOV2_PAD_MODE_MASK) << GPIOV2_PAD_MODE_DW0_POS;

    CommunityAccess.Access.AndThenOr32(
      &(CommunityAccess.Access),
      RegisterOffset,
      AndValue,
      OrValue
    );
  }

  //
  //  Program Input Inversion
  //
  if (InputInversion != GpioV2InputInversionHardwareDefault) {

    AndValue = (UINT32)~(GPIOV2_PAD_INPUT_INVERSION_MASK << GPIOV2_PAD_INPUT_INVERSION_DW0_POS);
    OrValue  = ((InputInversion >> 1) & GPIOV2_PAD_INPUT_INVERSION_MASK) << GPIOV2_PAD_INPUT_INVERSION_DW0_POS;

    CommunityAccess.Access.AndThenOr32 (
      &(CommunityAccess.Access),
      RegisterOffset,
      AndValue,
      OrValue
    );
  }

  //
  //  Program Termination config
  //
  if (TerminationConfig != GpioV2TermDefault) {
    //
    //  Get DW1 Register Offset to program Pad Termination
    //
    Status = MtlSocGpioGetRegOffset (GpioPad, GpioV2Dw1Reg, &RegisterOffset);
    if (EFI_ERROR(Status)) {
      return Status;
    }

    AndValue = (UINT32)~(GPIOV2_PAD_TERMINATION_CONFIG_MASK << GPIOV2_PAD_TERMINATION_CONFIG_DW1_POS);
    OrValue  = ((TerminationConfig >> 1) & GPIOV2_PAD_TERMINATION_CONFIG_MASK) << GPIOV2_PAD_TERMINATION_CONFIG_DW1_POS;

    CommunityAccess.Access.AndThenOr32 (
      &(CommunityAccess.Access),
      RegisterOffset,
      AndValue,
      OrValue
    );
  }
  return EFI_SUCCESS;
}


/**
  Board specific hook points.

  Implement board specific initialization during the boot flow.

  @param[in] InitPhase             Current phase in the boot flow.

**/
VOID
BoardInit (
  IN  BOARD_INIT_PHASE  InitPhase
  )
{
  UINT8             DebugPort;
  UINT8             UartNumber;
  GPIOV2_PAD        GpioPad;

  UartNumber = 0;

  switch (InitPhase) {
  case PostTempRamInit:
    DisableWatchDogTimer ();
    EarlyPlatformDataCheck ();

    DebugPort = GetDebugPort ();
    if ((DebugPort != 0xFF) && (DebugPort < MTL_MAX_SERIALIO_UART_CONTROLLERS)) {
      // RX
      GpioPad =  MtlSocGpioGetNativePadByFunctionAndPinMux (GPIO_FUNCTION_SERIAL_IO_UART_RX (UartNumber), TempRamInitParams.FsptConfig.PcdSerialIoUartRxPinMux);
      SecSerialIoGpioConfigure (GpioPad, GPIOV2_PAD_GET_PAD_MODE (GpioPad), GpioV2InputInversionDisable, GpioV2TermDefault);

      // TX
      GpioPad =  MtlSocGpioGetNativePadByFunctionAndPinMux (GPIO_FUNCTION_SERIAL_IO_UART_TX (UartNumber), TempRamInitParams.FsptConfig.PcdSerialIoUartTxPinMux);
      SecSerialIoGpioConfigure (GpioPad, GPIOV2_PAD_GET_PAD_MODE (GpioPad), GpioV2InputInversionDisable, GpioV2TermDefault);
    }

    PlatformHookSerialPortInitialize ();
    SerialPortInitialize ();
    break;
  default:
    break;
  }
}

/**
  Get size of Platform Specific Data

  @param[in] none

  @retval    UINT32     Size of Platform Specific Data

**/
UINT32
EFIAPI
GetPlatformDataSize (
  IN  VOID
  )
{
  return sizeof (PLATFORM_DATA);
}
