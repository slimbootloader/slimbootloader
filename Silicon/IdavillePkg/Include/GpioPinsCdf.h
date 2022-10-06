/** @file
  GPIO pins for CDF-PCH-H,

  Copyright (c) 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _GPIO_PINS_CDF_
#define _GPIO_PINS_CDF_

///
/// Unique ID used in GpioPad defines
///
#define GPIO_CDF_H_CHIPSET_ID       0x3

#define GPIO_CDF_GROUP_GPP_A        0x0300
#define GPIO_CDF_GPP_A0             0x03000000 //GBE_SDP_TIMESYNC0
#define GPIO_CDF_GPP_A1             0x03000001 //GBE_SDP_TIMESYNC1
#define GPIO_CDF_GPP_A2             0x03000002 //GBE_SDP_TIMESYNC2
#define GPIO_CDF_GPP_A3             0x03000003 //GBE_SDP_TIMESYNC3

#define GPIO_CDF_GPP_B20            0x03010014 //UART0_RXD
#define GPIO_CDF_GPP_B21            0x03010015 //UART0_TXD
#define GPIO_CDF_GPP_B22            0x03010016 //UART1_RXD
#define GPIO_CDF_GPP_B23            0x03010017 //UART1_TXD
#define GPIO_CDF_GPP_D0             0x03030000 //IE_UART_RXD
#define GPIO_CDF_GPP_D1             0x03030001 //IE_UART_TXD

#define GPIO_CDF_GPP_C0             0x03020000 //CPU_GP_0
#define GPIO_CDF_GPP_C1             0x03020001 //CPU_GP_1
#define GPIO_CDF_GPP_C4             0x03020004 //FAN_PWM_0
#define GPIO_CDF_GPP_C5             0x03020005 //FAN_PWM_1
#define GPIO_CDF_GPP_C6             0x03020006 //FAN_PWM_2
#define GPIO_CDF_GPP_C7             0x03020007 //FAN_PWM_3
#define GPIO_CDF_GPP_C8             0x03020008 //FAN_TACH_0
#define GPIO_CDF_GPP_C9             0x03020009 //FAN_TACH_1
#define GPIO_CDF_GPP_C10            0x0302000a //FAN_TACH_2
#define GPIO_CDF_GPP_C11            0x0302000b //FAN_TACH_3
#define GPIO_CDF_GPP_C12            0x0302000c //ME_SMB0_CLK
#define GPIO_CDF_GPP_C13            0x0302000d //ME_SMB0_DATA
#define GPIO_CDF_GPP_C14            0x0302000e //ME_SMB0_ALRT_N

#define GPIO_CDF_GPP_D9             0x03030009 //PCIE_CLKREQ4_N
#define GPIO_CDF_GPP_D10            0x0303000a //PCIE_CLKREQ5_N
#define GPIO_CDF_GPP_D11            0x0303000b //PCIE_CLKREQ6_N
#define GPIO_CDF_GPP_D12            0x0303000c //PCIE_CLKREQ7_N
#define GPIO_CDF_GPP_D13            0x0303000d //PCIE_CLKREQ8_N
#define GPIO_CDF_GPP_D14            0x0303000e //PCIE_CLKREQ9_N

#define GPIO_CDF_GPP_H8             0x03070008 //DBG_SPARE0
#define GPIO_CDF_GPP_H9             0x03070009 //DBG_SPARE1
#define GPIO_CDF_GPP_H10            0x0307000a //DBG_SPARE2
#define GPIO_CDF_GPP_H11            0x0307000b //DBG_SPARE3

#define GPIO_CDF_GPP_L5             0x030b0005 //GPIO_4
#define GPIO_CDF_GPP_L9             0x030b0009 //GPIO_8
#define GPIO_CDF_GPP_L10            0x030b000a //GPIO_9
#define GPIO_CDF_GPP_L11            0x030b000b //GPIO_10
#define GPIO_CDF_GPP_L12            0x030b000c //GPIO_11
#define GPIO_CDF_GPP_L13            0x030b000d //GPIO_12
#define GPIO_CDF_GPP_L14            0x030b000e //PECI_SMB_DATA
#define GPIO_CDF_GPP_L15            0x030b000f //SATA0_LED_N
#define GPIO_CDF_GPP_L17            0x030b0011 //SATA_PDETECT0
#define GPIO_CDF_GPP_L19            0x030b0013 //SATA0_SDOUT
#define GPIO_CDF_GPP_L21            0x030b0015 //SATA2_LED_N
#define GPIO_CDF_GPP_L22            0x030b0016 //SATA_PDETECT2
#define GPIO_CDF_GPP_L23            0x030b0017 //SATA2_SDOUT

#define GPIO_CDF_GPP_N3             0x030d0003 //SPI_MISO_IO1
#define GPIO_CDF_GPP_N10            0x030d000a //ADR_COMPLETE
#define GPIO_CDF_GPP_N19            0x030d0013 //PMU_I2C_CLK
#define GPIO_CDF_GPP_N20            0x030d0014 //PMU_I2C_DATA
#define GPIO_CDF_GPP_N21            0x030d0015 //PECI_SMB_CLK
#define GPIO_CDF_GPP_N22            0x030d0016 //PECI_SMB_ALRT_N

#define GPIO_CDF_GPP_O0             0x030e0000 //EMMC_CMD
#define GPIO_CDF_GPP_O1             0x030e0001 //EMMC_STROBE
#define GPIO_CDF_GPP_O2             0x030e0002 //EMMC_CLK
#define GPIO_CDF_GPP_O3             0x030e0003 //EMMC_D0
#define GPIO_CDF_GPP_O4             0x030e0004 //EMMC_D1
#define GPIO_CDF_GPP_O5             0x030e0005 //EMMC_D2
#define GPIO_CDF_GPP_O6             0x030e0006 //EMMC_D3
#define GPIO_CDF_GPP_O7             0x030e0007 //EMMC_D4
#define GPIO_CDF_GPP_O8             0x030e0008 //EMMC_D5
#define GPIO_CDF_GPP_O9             0x030e0009 //EMMC_D6
#define GPIO_CDF_GPP_O10            0x030e000a //EMMC_D7

#endif //_GPIO_PINS_CDF_
