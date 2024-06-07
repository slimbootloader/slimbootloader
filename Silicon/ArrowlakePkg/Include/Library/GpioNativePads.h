/** @file
  Header file for GPIO Native pads support

  Copyright (c) 2024, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

@par Specification Reference:
**/
#ifndef _GPIO_NATIVE_PADS_H_
#define _GPIO_NATIVE_PADS_H_

//
// GpioPad can contain additional information used to provide data on
// native functions. Please refer to description of GPIO_NATIVE_PAD
//
// FFFF CCCC TTTG GGGG TTTT TTTP PPPP PPPP
//
// F - 2^4 = 16,  native function number
// C - 2^4 = 16,  chipset ID
// T - 2^10 = 1024 , abstract type representing native mode of a pad (e.g. SERIALIO_UART2_TX)
// G - 2^5 = 32,  group
// P - 2^9 = 512, pad number
//
// F & T contain additional optional settings used for native pads
//
#define GPIO_NATIVE_PAD_DEF(GpioNativePad, NativeMode, NativeFunction) \
  (GpioNativePad | (NativeMode << 28) | ((NativeFunction & 0x7F) << 9) | ((NativeFunction & 0x380) << 14))

#define GPIO_NATIVE_GET_FUNCTION(GpioNativePad) ((((GpioNativePad) & 0xFE00) >> 9) | (((GpioNativePad) & 0xE00000) >> 14))
#define GPIO_NATIVE_GET_PAD_FN(GpioNativePad)   (((GpioNativePad) >> 28) & 0xF)
#define GPIO_NATIVE_GET_PAD_MODE(GpioNativePad) ((GPIO_NATIVE_GET_PAD_FN(GpioNativePad) << 1) | 1)
#define GPIO_NATIVE_TO_GPIO_PAD(GpioNativePad)  (GpioNativePad & 0xF1F01FF)


//
// Macro used to define GPIO native function.
// <Min,Max> defines range that can be used to encode given native signal.
// Numbering must be unique and cannot overlap.
// If there are many instances of similar signal (e.g. per controller) the lower
// word will store value for a given instance in the form: Min + Instance
// Upper word (Max) is left untouched and later used for verification
//
#define GPIO_NATIVE_FUNCTION_DEF(Min, Max)              (((Max) << 16) + (Min))
#define GPIO_NATIVE_FUNCTION_GET_MAX(NativeFunction)    (((NativeFunction) >> 16) & 0xFFFF)
#define GPIO_NATIVE_FUNCTION_GET_VALUE(NativeFunction)  ((NativeFunction) & 0xFFFF)

//
// Macro GPIO_NATIVE_FUNCTION_GET_SIGNAL is created as synonym to macro GPIO_NATIVE_FUNCTION_GET_MAX
// GPIO_NATIVE_FUNCTION_GET_SIGNAL used with below defines is more descriptive and easier to read
// ex.
//  - GPIO_NATIVE_FUNCTION_GET_SIGNAL(GPIO_SERIAL_IO_UART_RX)
//  - GPIO_NATIVE_FUNCTION_GET_SIGNAL(GPIO_ISH_GP)
//  - ...
//
#define GPIO_NATIVE_FUNCTION_GET_SIGNAL(NativeFunction) (GPIO_NATIVE_FUNCTION_GET_MAX(NativeFunction))

//
// GPIO native modes
// Those defines are internal to this header.
// GPIO_FUNCTION_<IP>_<signal>(index) defines should be used by other modules instead.
//
#define GPIO_SERIAL_IO_UART_RX     GPIO_NATIVE_FUNCTION_DEF(1,8)
#define GPIO_SERIAL_IO_UART_TX     GPIO_NATIVE_FUNCTION_DEF(9,16)
#define GPIO_SERIAL_IO_UART_RTS    GPIO_NATIVE_FUNCTION_DEF(17,24)
#define GPIO_SERIAL_IO_UART_CTS    GPIO_NATIVE_FUNCTION_DEF(25,31)
#define GPIO_SERIAL_IO_SPI_MOSI    GPIO_NATIVE_FUNCTION_DEF(32,39)
#define GPIO_SERIAL_IO_SPI_MISO    GPIO_NATIVE_FUNCTION_DEF(40,47)
#define GPIO_SERIAL_IO_SPI_CLK     GPIO_NATIVE_FUNCTION_DEF(48,55)
#define GPIO_SERIAL_IO_SPI_CS      GPIO_NATIVE_FUNCTION_DEF(56,71)
#define GPIO_ISH_GP                GPIO_NATIVE_FUNCTION_DEF(80,143)
#define GPIO_ISH_UART_RX           GPIO_NATIVE_FUNCTION_DEF(144,151)
#define GPIO_ISH_UART_TX           GPIO_NATIVE_FUNCTION_DEF(152,159)
#define GPIO_ISH_UART_RTS          GPIO_NATIVE_FUNCTION_DEF(160,167)
#define GPIO_ISH_UART_CTS          GPIO_NATIVE_FUNCTION_DEF(168,175)
#define GPIO_ISH_SPI_MOSI          GPIO_NATIVE_FUNCTION_DEF(184,191)
#define GPIO_ISH_SPI_MISO          GPIO_NATIVE_FUNCTION_DEF(192,199)
#define GPIO_ISH_SPI_CLK           GPIO_NATIVE_FUNCTION_DEF(200,207)
#define GPIO_ISH_SPI_CS            GPIO_NATIVE_FUNCTION_DEF(208,223)
#define GPIO_ISH_I2C_SCL           GPIO_NATIVE_FUNCTION_DEF(232,239)
#define GPIO_ISH_I2C_SDA           GPIO_NATIVE_FUNCTION_DEF(240,247)
#define GPIO_THC_SPI_INT           GPIO_NATIVE_FUNCTION_DEF(248,251)
#define GPIO_HDA_BCLK              GPIO_NATIVE_FUNCTION_DEF(252,252)
#define GPIO_HDA_RSTB              GPIO_NATIVE_FUNCTION_DEF(253,253)
#define GPIO_HDA_SYNC              GPIO_NATIVE_FUNCTION_DEF(254,254)
#define GPIO_HDA_SDO               GPIO_NATIVE_FUNCTION_DEF(255,255)
#define GPIO_HDA_SDI_0             GPIO_NATIVE_FUNCTION_DEF(256,256)
#define GPIO_HDA_SDI_1             GPIO_NATIVE_FUNCTION_DEF(257,257)
#define GPIO_DMIC_DATA             GPIO_NATIVE_FUNCTION_DEF(258,261)
#define GPIO_DMIC_CLKA             GPIO_NATIVE_FUNCTION_DEF(262,265)
#define GPIO_DMIC_CLKB             GPIO_NATIVE_FUNCTION_DEF(266,269)
#define GPIO_DDSP_HPD0             GPIO_NATIVE_FUNCTION_DEF(270,285)
#define GPIO_PANEL_AVDD_EN         GPIO_NATIVE_FUNCTION_DEF(286,289)
#define GPIO_PANEL_BKLTEN          GPIO_NATIVE_FUNCTION_DEF(290,293)
#define GPIO_PANEL_BKLTCTL         GPIO_NATIVE_FUNCTION_DEF(294,297)
#define GPIO_PANEL_RESET           GPIO_NATIVE_FUNCTION_DEF(298,301)
#define GPIO_PANEL_AVEE_EN         GPIO_NATIVE_FUNCTION_DEF(302,305)
#define GPIO_PANEL_VIO_EN          GPIO_NATIVE_FUNCTION_DEF(306,309)
#define GPIO_PANEL_HPD             GPIO_NATIVE_FUNCTION_DEF(310,313)
#define GPIO_PANEL_TE_EN           GPIO_NATIVE_FUNCTION_DEF(314,317)
#define GPIO_HDMI_GMBUS_SCL        GPIO_NATIVE_FUNCTION_DEF(318,325)
#define GPIO_HDMI_GMBUS_SDA        GPIO_NATIVE_FUNCTION_DEF(326,333)
#define GPIO_ISH_I3C_SCL           GPIO_NATIVE_FUNCTION_DEF(334,335)
#define GPIO_ISH_I3C_SDA           GPIO_NATIVE_FUNCTION_DEF(336,337)
#define GPIO_SERIAL_IO_I2C_SCL     GPIO_NATIVE_FUNCTION_DEF(338,353)
#define GPIO_SERIAL_IO_I2C_SDA     GPIO_NATIVE_FUNCTION_DEF(354,369)
#define GPIO_CNVI_RF_RESET         GPIO_NATIVE_FUNCTION_DEF(370,370)
#define GPIO_CNVI_CLKREQ           GPIO_NATIVE_FUNCTION_DEF(371,371)
#define GPIO_SD_DATA               GPIO_NATIVE_FUNCTION_DEF(374,377)
#define GPIO_EMMC_DATA             GPIO_NATIVE_FUNCTION_DEF(384,391)
#define GPIO_THC_CLK_LOOPBACK      GPIO_NATIVE_FUNCTION_DEF(395,396)
#define GPIO_SERIAL_IO_I3C_SCL     GPIO_NATIVE_FUNCTION_DEF(397,399)
#define GPIO_SERIAL_IO_I3C_SDA     GPIO_NATIVE_FUNCTION_DEF(400,402)
#define GPIO_MIPI_PANEL_RESET      GPIO_NATIVE_FUNCTION_DEF(403,406)
#define GPIO_MIPI_SEC_POW_EN_AVEE  GPIO_NATIVE_FUNCTION_DEF(407,410)
#define GPIO_MIPI_SEC_POW_EN_AVDD  GPIO_NATIVE_FUNCTION_DEF(411,414)
#define GPIO_THC_WOT               GPIO_NATIVE_FUNCTION_DEF(415,416)
#define GPIO_SATA_DEVSLP           GPIO_NATIVE_FUNCTION_DEF(417,448)
#define GPIO_PCIE_CLKREQ           GPIO_NATIVE_FUNCTION_DEF(449,480)
#define GPIO_SERIAL_IO_I3C_SCL_FB  GPIO_NATIVE_FUNCTION_DEF(481,483)


#define GPIO_FUNCTION_SERIAL_IO_UART_RX(UartDev)  (GPIO_SERIAL_IO_UART_RX + ((UINT32)UartDev))
#define GPIO_FUNCTION_SERIAL_IO_UART_TX(UartDev)  (GPIO_SERIAL_IO_UART_TX + ((UINT32)UartDev))
#define GPIO_FUNCTION_SERIAL_IO_UART_RTS(UartDev) (GPIO_SERIAL_IO_UART_RTS + ((UINT32)UartDev))
#define GPIO_FUNCTION_SERIAL_IO_UART_CTS(UartDev) (GPIO_SERIAL_IO_UART_CTS + ((UINT32)UartDev))

//
// Serial IO SPI
//
#define GPIO_SERIAL_IO_SPI_RANGE  8  // Number of SerialIo SPIx controllers supported in GPIO_NATIVE_PAD encoding

#define GPIO_FUNCTION_SERIAL_IO_SPI_MOSI(SpiDev)      (GPIO_SERIAL_IO_SPI_MOSI + ((UINT32)SpiDev))
#define GPIO_FUNCTION_SERIAL_IO_SPI_MISO(SpiDev)      (GPIO_SERIAL_IO_SPI_MISO + ((UINT32)SpiDev))
#define GPIO_FUNCTION_SERIAL_IO_SPI_CLK(SpiDev)       (GPIO_SERIAL_IO_SPI_CLK + ((UINT32)SpiDev))
#define GPIO_FUNCTION_SERIAL_IO_SPI_CS(SpiDev, CsNum) (GPIO_SERIAL_IO_SPI_CS + ((UINT32)SpiDev) + ((UINT32)CsNum) * GPIO_SERIAL_IO_SPI_RANGE)


#define GPIO_FUNCTION_SERIAL_IO_I2C_SCL(I2cDev) (GPIO_SERIAL_IO_I2C_SCL + ((UINT32)I2cDev))
#define GPIO_FUNCTION_SERIAL_IO_I2C_SDA(I2cDev) (GPIO_SERIAL_IO_I2C_SDA + ((UINT32)I2cDev))


#define GPIO_FUNCTION_ISH_GP(GpNum) (GPIO_ISH_GP + ((UINT32)GpNum))


#define GPIO_FUNCTION_ISH_UART_RX(UartDev)  (GPIO_ISH_UART_RX + ((UINT32)UartDev))
#define GPIO_FUNCTION_ISH_UART_TX(UartDev)  (GPIO_ISH_UART_TX + ((UINT32)UartDev))
#define GPIO_FUNCTION_ISH_UART_RTS(UartDev) (GPIO_ISH_UART_RTS + ((UINT32)UartDev))
#define GPIO_FUNCTION_ISH_UART_CTS(UartDev) (GPIO_ISH_UART_CTS + ((UINT32)UartDev))

//
// ISH SPI
//
#define GPIO_ISH_SPI_RANGE  8  // Number of ISH SPI controllers supported in GPIO_NATIVE_PAD encoding

#define GPIO_FUNCTION_ISH_SPI_MOSI(SpiDev)      (GPIO_ISH_SPI_MOSI + ((UINT32)SpiDev))
#define GPIO_FUNCTION_ISH_SPI_MISO(SpiDev)      (GPIO_ISH_SPI_MISO + ((UINT32)SpiDev))
#define GPIO_FUNCTION_ISH_SPI_CLK(SpiDev)       (GPIO_ISH_SPI_CLK + ((UINT32)SpiDev))
#define GPIO_FUNCTION_ISH_SPI_CS(SpiDev, CsNum) (GPIO_ISH_SPI_CS + ((UINT32)SpiDev) + ((UINT32)CsNum) * GPIO_ISH_SPI_RANGE)


#define GPIO_FUNCTION_ISH_I2C_SCL(I2cDev) (GPIO_ISH_I2C_SCL + ((UINT32)I2cDev))
#define GPIO_FUNCTION_ISH_I2C_SDA(I2cDev) (GPIO_ISH_I2C_SDA + ((UINT32)I2cDev))


#define GPIO_FUNCTION_ISH_I3C_SCL(I3cDev) (GPIO_ISH_I3C_SCL + ((UINT32)I3cDev))
#define GPIO_FUNCTION_ISH_I3C_SDA(I3cDev) (GPIO_ISH_I3C_SDA + ((UINT32)I3cDev))


#define GPIO_FUNCTION_SERIAL_IO_I3C_SCL(I3cDev) (GPIO_SERIAL_IO_I3C_SCL + ((UINT32)I3cDev))
#define GPIO_FUNCTION_SERIAL_IO_I3C_SDA(I3cDev) (GPIO_SERIAL_IO_I3C_SDA + ((UINT32)I3cDev))
#define GPIO_FUNCTION_SERIAL_IO_I3C_SCL_FB(I3cDev) (GPIO_SERIAL_IO_I3C_SCL_FB + ((UINT32)I3cDev))

//
// SD Card
//
#define GPIO_FUNCTION_SD_DATA(Index)        (GPIO_SD_DATA + ((UINT32)Index))

//
// EMMC
//
#define GPIO_FUNCTION_EMMC_DATA(Index)      (GPIO_EMMC_DATA + ((UINT32)Index))


#define GPIO_FUNCTION_THC_SPI_INT(SpiDev) (GPIO_THC_SPI_INT + ((UINT32)SpiDev))
#define GPIO_FUNCTION_THC_CLK_LOOPBACK(SpiDev) (GPIO_THC_CLK_LOOPBACK + ((UINT32)SpiDev))
#define GPIO_FUNCTION_THC_WOT(SpiDev) (GPIO_THC_WOT + ((UINT32)SpiDev))

//
// HDA
//
#define GPIO_FUNCTION_HDA_BCLK      GPIO_HDA_BCLK
#define GPIO_FUNCTION_HDA_RSTB      GPIO_HDA_RSTB
#define GPIO_FUNCTION_HDA_SYNC      GPIO_HDA_SYNC
#define GPIO_FUNCTION_HDA_SDO       GPIO_HDA_SDO
#define GPIO_FUNCTION_HDA_SDI_0     GPIO_HDA_SDI_0
#define GPIO_FUNCTION_HDA_SDI_1     GPIO_HDA_SDI_1


#define GPIO_FUNCTION_DMIC_DATA(DmicDev) (GPIO_DMIC_DATA  + ((UINT32)DmicDev))
#define GPIO_FUNCTION_DMIC_CLKA(DmicDev) (GPIO_DMIC_CLKA  + ((UINT32)DmicDev))
#define GPIO_FUNCTION_DMIC_CLKB(DmicDev) (GPIO_DMIC_CLKB  + ((UINT32)DmicDev))
#define GPIO_FUNCTION_DMIC_CLK(DmicDev)  (GPIO_DMIC_CLKA  + ((UINT32)DmicDev)) // If there is no split between channel A/B use A range for such Clocks


#define GPIO_FUNCTION_DDSP_HPD(HpdIndex) \
  (HpdIndex > 7) ? GPIO_DDSP_HPD0 + 8 + (HpdIndex) - 'A' : GPIO_DDSP_HPD0 + HpdIndex


#define GPIO_FUNCTION_HDMI_SCL(DdiPort) (GPIO_HDMI_GMBUS_SCL + ((UINT32)DdiPort))
#define GPIO_FUNCTION_HDMI_SDA(DdiPort) (GPIO_HDMI_GMBUS_SDA + ((UINT32)DdiPort))


#define GPIO_FUNCTION_PANEL_AVDD_EN(PanelDev)         (GPIO_PANEL_AVDD_EN + ((UINT32)PanelDev))
#define GPIO_FUNCTION_PANEL_BKLTEN(PanelDev)          (GPIO_PANEL_BKLTEN + ((UINT32)PanelDev))
#define GPIO_FUNCTION_PANEL_BKLTCTL(PanelDev)         (GPIO_PANEL_BKLTCTL + ((UINT32)PanelDev))
#define GPIO_FUNCTION_PANEL_RESET(PanelDev)           (GPIO_PANEL_RESET + ((UINT32)PanelDev))
#define GPIO_FUNCTION_PANEL_AVEE_EN(PanelDev)         (GPIO_PANEL_AVEE_EN + ((UINT32)PanelDev))
#define GPIO_FUNCTION_PANEL_VIO_EN(PanelDev)          (GPIO_PANEL_VIO_EN + ((UINT32)PanelDev))
#define GPIO_FUNCTION_PANEL_HPD(PanelDev)             (GPIO_PANEL_HPD + ((UINT32)PanelDev))
#define GPIO_FUNCTION_PANEL_TE_EN(PanelDev)           (GPIO_PANEL_TE_EN + ((UINT32)PanelDev))

//
// MIPI
//
#define GPIO_FUNCTION_MIPI_PANEL_RESET(PanelDev)      (GPIO_MIPI_PANEL_RESET + ((UINT32)PanelDev))
#define GPIO_FUNCTION_MIPI_SEC_POW_EN_AVEE(PanelDev)  (GPIO_MIPI_SEC_POW_EN_AVEE + ((UINT32)PanelDev))
#define GPIO_FUNCTION_MIPI_SEC_POW_EN_AVDD(PanelDev)  (GPIO_MIPI_SEC_POW_EN_AVDD + ((UINT32)PanelDev))


#define GPIO_FUNCTION_CNVI_RF_RESET GPIO_CNVI_RF_RESET


#define GPIO_FUNCTION_CNVI_CLKREQ GPIO_CNVI_CLKREQ

//
// SATA DevSlp
//
#define GPIO_SATA_DEVSLP_RANGE 32   // Number of SATA DevSlp instances per controller supported in GPIO_NATIVE_PAD encoding

#define GPIO_FUNCTION_SATA_DEVSLP(CsNum, SataDevSlpIndex)      (GPIO_SATA_DEVSLP + ((UINT32)SataDevSlpIndex) + ((UINT32)CsNum) * GPIO_SATA_DEVSLP_RANGE)


#define GPIO_FUNCTION_PCIE_CLKREQ(ClkReqIndex)      (GPIO_PCIE_CLKREQ + ((UINT32)ClkReqIndex))

#endif // _GPIO_NATIVE_PADS_H_
