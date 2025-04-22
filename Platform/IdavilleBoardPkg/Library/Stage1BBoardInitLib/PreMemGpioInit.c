/** @file

  Copyright (c) 2021, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Uefi/UefiBaseType.h>
#include <Library/BaseLib.h>
#include <Library/IoLib.h>
#include <Library/DebugLib.h>
#include <Library/BootloaderCommonLib.h>
#include <GpioPinsCdf.h>
#include <GpioConfig.h>
#include <Library/UncoreLib.h>
#include <Library/ConfigDataLib.h>
#include <Library/GpioLib.h>
#include "Stage1BBoardInitLib.h"

// Based on sheet BrightonCity_RP_GPIO_sheet_BMC_mode_PO_ABCDformat_v0p72.xlsx
static GPIO_INIT_CONFIG mGpioBrightonCityRp[] = {
    //CODE_DEFINE    ,PAD_MODE           ,HOST_SOFTPAD_OWN  ,DIRECTION     ,OUTPUT_STATE  ,INTERRUPT_CONFIG|INT_TRIGGER,POWER_CONFIG    ,ELECTRICAL_CONFIG,LOCK_CONFIG       ,OTHER_SETTINGS     ,//GPIO_NAME|BOARD_NAME
    {GPIO_CDF_GPP_A0 ,{GpioPadModeNative1,GpioHostOwnDefault,GpioDirDefault,GpioOutDefault,GpioIntDefault              ,GpioResetDefault,GpioTermDefault  ,GpioLockDefault   ,GpioRxRaw1Default}},//GBE_SDP_TIMESYNC0|GBE_SDP_TIMESYNC0_S2N
    {GPIO_CDF_GPP_A1 ,{GpioPadModeNative1,GpioHostOwnDefault,GpioDirDefault,GpioOutDefault,GpioIntDefault              ,GpioResetDefault,GpioTermDefault  ,GpioLockDefault   ,GpioRxRaw1Default}},//GBE_SDP_TIMESYNC1|GBE_SDP_TIMESYNC1_S2N
    {GPIO_CDF_GPP_A2 ,{GpioPadModeNative1,GpioHostOwnDefault,GpioDirDefault,GpioOutDefault,GpioIntDefault              ,GpioResetDefault,GpioTermDefault  ,GpioLockDefault   ,GpioRxRaw1Default}},//GBE_SDP_TIMESYNC2|GBE_SDP_TIMESYNC2_S2N
    {GPIO_CDF_GPP_A3 ,{GpioPadModeNative1,GpioHostOwnDefault,GpioDirDefault,GpioOutDefault,GpioIntDefault              ,GpioResetDefault,GpioTermDefault  ,GpioLockDefault   ,GpioRxRaw1Default}},//GBE_SDP_TIMESYNC3|GBE_SDP_TIMESYNC3_S2N
    {GPIO_CDF_GPP_C0 ,{GpioPadModeGpio   ,GpioHostOwnGpio   ,GpioDirIn     ,GpioOutDefault,GpioIntDefault              ,GpioResetDefault,GpioTermDefault  ,GpioLockDefault   ,GpioRxRaw1Default}},//CPU_GP_0|IRQ_BMC_PCH_NMI
    //{GPIO_CDF_GPP_C1 ,{GpioPadModeGpio   ,GpioHostOwnDefault,GpioDirDefault,GpioOutDefault,GpioIntDefault              ,GpioResetDefault,GpioTermDefault  ,GpioLockDefault   ,GpioRxRaw1Default}},//CPU_GP_1|FM_NM _THROTTLE_N
    {GPIO_CDF_GPP_C10,{GpioPadModeGpio   ,GpioHostOwnGpio   ,GpioDirIn     ,GpioOutDefault,GpioIntDefault              ,GpioResetDefault,GpioTermDefault  ,GpioPadLock       ,GpioRxRaw1Default}},//FAN_TACH_2|Spare
    {GPIO_CDF_GPP_C11,{GpioPadModeGpio   ,GpioHostOwnGpio   ,GpioDirIn     ,GpioOutDefault,GpioIntDefault              ,GpioResetDefault,GpioTermDefault  ,GpioLockDefault   ,GpioRxRaw1Default}},//FAN_TACH_3|FM_BIOS_ADV_FUNCTION
    {GPIO_CDF_GPP_C12,{GpioPadModeNative3,GpioHostOwnDefault,GpioDirDefault,GpioOutDefault,GpioIntDefault              ,GpioResetDefault,GpioTermDefault  ,GpioPadConfigLock ,GpioRxRaw1Default}},//ME_SMB0_CLK
    {GPIO_CDF_GPP_C13,{GpioPadModeNative3,GpioHostOwnDefault,GpioDirDefault,GpioOutDefault,GpioIntDefault              ,GpioResetDefault,GpioTermDefault  ,GpioPadConfigLock ,GpioRxRaw1Default}},//ME_SMB0_DATA
    {GPIO_CDF_GPP_C14,{GpioPadModeNative3,GpioHostOwnDefault,GpioDirDefault,GpioOutDefault,GpioIntDefault              ,GpioResetDefault,GpioTermDefault  ,GpioLockDefault   ,GpioRxRaw1Default}},//ME_SMB0_ALRT_N
    {GPIO_CDF_GPP_C4 ,{GpioPadModeGpio   ,GpioHostOwnGpio   ,GpioDirOut    ,GpioOutHigh   ,GpioIntDefault              ,GpioResetDefault,GpioTermDefault  ,GpioPadLock       ,GpioRxRaw1Default}},//FAN_PWM_0|FM_MC_INIT
    {GPIO_CDF_GPP_C5 ,{GpioPadModeGpio   ,GpioHostOwnGpio   ,GpioDirOut    ,GpioOutHigh   ,GpioIntDefault              ,GpioResetDefault,GpioTermDefault  ,GpioPadLock       ,GpioRxRaw1Default}},//FAN_PWM_1|FM_ADR_SMI_GPIO_N
    {GPIO_CDF_GPP_C6 ,{GpioPadModeGpio   ,GpioHostOwnGpio   ,GpioDirInOut  ,GpioOutHigh   ,GpioIntDefault              ,GpioResumeReset ,GpioTermDefault  ,GpioPadConfigLock ,GpioRxRaw1Default}},//FAN_PWM_2|FM_ADR_EN_DATAOUT
    {GPIO_CDF_GPP_C7 ,{GpioPadModeGpio   ,GpioHostOwnGpio   ,GpioDirOut    ,GpioOutHigh   ,GpioIntDefault              ,GpioResumeReset ,GpioTermDefault  ,GpioPadLock       ,GpioRxRaw1Default}},//FAN_PWM_3|FM_ADR_EN_CLK
    {GPIO_CDF_GPP_C8 ,{GpioPadModeGpio   ,GpioHostOwnGpio   ,GpioDirIn     ,GpioOutHigh   ,GpioIntDefault              ,GpioResetDefault,GpioTermDefault  ,GpioLockDefault   ,GpioRxRaw1Default}},//FAN_TACH_0|Spare
    {GPIO_CDF_GPP_C9 ,{GpioPadModeGpio   ,GpioHostOwnGpio   ,GpioDirIn     ,GpioOutHigh   ,GpioIntDefault              ,GpioResetDefault,GpioTermDefault  ,GpioLockDefault   ,GpioRxRaw1Default}},//FAN_TACH_1|Spare
    {GPIO_CDF_GPP_D10,{GpioPadModeGpio   ,GpioHostOwnGpio   ,GpioDirIn     ,GpioOutDefault,GpioIntDefault              ,GpioResetDefault,GpioTermDefault  ,GpioLockDefault   ,GpioRxRaw1Default}},//PCIE_CLKREQ5_N|NET_PLL_INTR_N
    {GPIO_CDF_GPP_D11,{GpioPadModeGpio   ,GpioHostOwnGpio   ,GpioDirOut    ,GpioOutHigh   ,GpioIntDefault              ,GpioResetDefault,GpioTermDefault  ,GpioLockDefault   ,GpioRxRaw1Default}},//PCIE_CLKREQ6_N| FM_ME_VLN_1
    {GPIO_CDF_GPP_D12,{GpioPadModeGpio   ,GpioHostOwnGpio   ,GpioDirOut    ,GpioOutHigh   ,GpioIntDefault              ,GpioResetDefault,GpioTermDefault  ,GpioLockDefault   ,GpioRxRaw1Default}},//PCIE_CLKREQ7_N| FM_ME_VLN_2
    {GPIO_CDF_GPP_D13,{GpioPadModeGpio   ,GpioHostOwnGpio   ,GpioDirOut    ,GpioOutHigh   ,GpioIntDefault              ,GpioResetDefault,GpioTermDefault  ,GpioPadLock       ,GpioRxRaw1Default}},//PCIE_CLKREQ8_N|Spare
    {GPIO_CDF_GPP_D14,{GpioPadModeGpio   ,GpioHostOwnGpio   ,GpioDirInOut  ,GpioOutDefault,GpioIntDefault              ,GpioResetDefault,GpioTermDefault  ,GpioPadUnlock     ,GpioRxRaw1Default}},//PCIE_CLKREQ9_N|SATA_RAID_KEY
    {GPIO_CDF_GPP_D9 ,{GpioPadModeGpio   ,GpioHostOwnGpio   ,GpioDirOut    ,GpioOutHigh   ,GpioIntDefault              ,GpioResetDefault,GpioTermDefault  ,GpioPadLock       ,GpioRxRaw1Default}},//PCIE_CLKREQ4_N|FM_NIC1_DISABLE_N
    {GPIO_CDF_GPP_H10,{GpioPadModeGpio   ,GpioHostOwnGpio   ,GpioDirIn     ,GpioOutDefault,GpioIntDefault              ,GpioResetDefault,GpioTermDefault  ,GpioLockDefault   ,GpioRxRaw1Default}},//DBG_SPARE2|FM_BIOS_IMAGE_SWAP_2
    //{GPIO_CDF_GPP_H11,{GpioPadModeGpio   ,GpioHostOwnGpio   ,GpioDirIn     ,GpioOutDefault,GpioIntSci|GpioIntLevel     ,GpioResetDefault,GpioTermDefault  ,GpioPadConfigLock ,GpioRxRaw1Default}},//DBG_SPARE3|IRQ_PCH_SCI_WHEA_N
    {GPIO_CDF_GPP_H8 ,{GpioPadModeGpio   ,GpioHostOwnGpio   ,GpioDirIn     ,GpioOutDefault,GpioIntDefault              ,GpioResetDefault,GpioTermDefault  ,GpioLockDefault   ,GpioRxRaw1Default}},//DBG_SPARE0|FM_PASSWORD_CLEAR_N
    {GPIO_CDF_GPP_H9 ,{GpioPadModeGpio   ,GpioHostOwnGpio   ,GpioDirIn     ,GpioOutDefault,GpioIntDefault              ,GpioResetDefault,GpioTermDefault  ,GpioLockDefault   ,GpioRxRaw1Default}},//DBG_SPARE1|FM_BIOS_IMAGE_SWAP_1
    {GPIO_CDF_GPP_L10,{GpioPadModeGpio   ,GpioHostOwnGpio   ,GpioDirOut    ,GpioOutHigh   ,GpioIntDefault              ,GpioResetDefault,GpioTermDefault  ,GpioLockDefault   ,GpioRxRaw1Default}},//GPIO_9|FM_BIOS_POST_CMPLT_N
    {GPIO_CDF_GPP_L11,{GpioPadModeGpio   ,GpioHostOwnGpio   ,GpioDirOut    ,GpioOutHigh   ,GpioIntDefault              ,GpioResetDefault,GpioTermDefault  ,GpioPadLock       ,GpioRxRaw1Default}},//GPIO_10|IRQ_SMI_ACTIVE_N
    {GPIO_CDF_GPP_L12,{GpioPadModeGpio   ,GpioHostOwnGpio   ,GpioDirIn     ,GpioOutDefault,GpioIntApic|GpioIntLevel    ,GpioResetDefault,GpioTermDefault  ,GpioLockDefault   ,GpioRxRaw1Default}},//GPIO_11|IRQ_TPM_SPI_N
    {GPIO_CDF_GPP_L13,{GpioPadModeGpio   ,GpioHostOwnGpio   ,GpioDirIn     ,GpioOutDefault,GpioIntDefault              ,GpioResetDefault,GpioTermDefault  ,GpioLockDefault   ,GpioRxRaw1Default}},//GPIO_12|PLD_PROG_N
    {GPIO_CDF_GPP_L14,{GpioPadModeGpio   ,GpioHostOwnGpio   ,GpioDirOut    ,GpioOutHigh   ,GpioIntDefault              ,GpioResetDefault,GpioTermDefault  ,GpioPadLock       ,GpioRxRaw1Default}},//PECI_SMB_DATA|I2C_Buffer_Enable
    {GPIO_CDF_GPP_L15,{GpioPadModeNative2,GpioHostOwnDefault,GpioDirDefault,GpioOutDefault,GpioIntDefault              ,GpioResetDefault,GpioTermDefault  ,GpioLockDefault   ,GpioRxRaw1Default}},//SATA0_LED_N|spare
    {GPIO_CDF_GPP_L17,{GpioPadModeNative2,GpioHostOwnDefault,GpioDirDefault,GpioOutDefault,GpioIntDefault              ,GpioResetDefault,GpioTermDefault  ,GpioLockDefault   ,GpioRxRaw1Default}},//SATA_PDETECT0|Spare
    {GPIO_CDF_GPP_L19,{GpioPadModeNative2,GpioHostOwnDefault,GpioDirDefault,GpioOutDefault,GpioIntDefault              ,GpioResetDefault,GpioTermDefault  ,GpioLockDefault   ,GpioRxRaw1Default}},//SATA0_SDOUT|Spare
    {GPIO_CDF_GPP_L21,{GpioPadModeGpio   ,GpioHostOwnGpio   ,GpioDirOut    ,GpioOutHigh   ,GpioIntDefault              ,GpioResetDefault,GpioTermDefault  ,GpioPadLock       ,GpioRxRaw1Default}},//SATA2_LED_N|spare
    {GPIO_CDF_GPP_L22,{GpioPadModeGpio   ,GpioHostOwnGpio   ,GpioDirIn     ,GpioOutDefault,GpioIntDefault              ,GpioResetDefault,GpioTermDefault  ,GpioLockDefault   ,GpioRxRaw1Default}},//SATA_PDETECT2|FM_MFG_MODE
    {GPIO_CDF_GPP_L23,{GpioPadModeGpio   ,GpioHostOwnGpio   ,GpioDirIn     ,GpioOutHigh   ,GpioIntDefault              ,GpioResetDefault,GpioTermDefault  ,GpioLockDefault   ,GpioRxRaw1Default}},//SATA2_SDOUT|Spare
    {GPIO_CDF_GPP_L5 ,{GpioPadModeNative1,GpioHostOwnDefault,GpioDirDefault,GpioOutDefault,GpioIntDefault              ,GpioResetDefault,GpioTermDefault  ,GpioLockDefault   ,GpioRxRaw1Default}},//GPIO_4
    {GPIO_CDF_GPP_L9 ,{GpioPadModeNative1,GpioHostOwnDefault,GpioDirDefault,GpioOutDefault,GpioIntDefault              ,GpioResetDefault,GpioTermDefault  ,GpioLockDefault   ,GpioRxRaw1Default}},//GPIO_8|SPI_TPM_CS_N
    {GPIO_CDF_GPP_N10,{GpioPadModeNative1,GpioHostOwnDefault,GpioDirDefault,GpioOutDefault,GpioIntDefault              ,GpioResetDefault,GpioTermDefault  ,GpioPadLock       ,GpioRxRaw1Default}},//ADR_COMPLETE|Spare
    //{GPIO_CDF_GPP_N19,{GpioPadModeGpio   ,GpioHostOwnDefault,GpioDirDefault,GpioOutDefault,GpioIntDefault              ,GpioResetDefault,GpioTermDefault  ,GpioLockDefault   ,GpioRxRaw1Default}},//PMU_I2C_CLK|PMBUS_ALERT_EN
    //{GPIO_CDF_GPP_N20,{GpioPadModeGpio   ,GpioHostOwnGpio   ,GpioDirIn     ,GpioOutDefault,GpioIntDefault              ,GpioResetDefault,GpioTermDefault  ,GpioLockDefault   ,GpioRxRaw1Default}},//PMU_I2C_DATA|PMBUS_ALERT_N
    {GPIO_CDF_GPP_N21,{GpioPadModeGpio   ,GpioHostOwnGpio   ,GpioDirIn     ,GpioOutDefault,GpioIntDefault              ,GpioResetDefault,GpioTermDefault  ,GpioLockDefault   ,GpioRxRaw1Default}},//PECI_SMB_CLK|FM_SSATA_PCIE_M2_1_SEL_1
    //{GPIO_CDF_GPP_N22,{GpioPadModeGpio   ,GpioHostOwnGpio   ,GpioDirIn     ,GpioOutDefault,GpioIntDefault              ,GpioResetDefault,GpioTermDefault  ,GpioLockDefault   ,GpioRxRaw1Default}},//PECI_SMB_ALRT_N|FM_ME_RCVR_N
    {GPIO_CDF_GPP_N3 ,{GpioPadModeNative1,GpioHostOwnDefault,GpioDirDefault,GpioOutDefault,GpioIntDefault              ,GpioResetDefault,GpioTermDefault  ,GpioLockDefault   ,GpioRxRaw1Default}},//SPI_MISO_IO1|SPI_MISO_IO1
    {GPIO_CDF_GPP_O0 ,{GpioPadModeNative1,GpioHostOwnDefault,GpioDirDefault,GpioOutDefault,GpioIntDefault              ,GpioResetDefault,GpioTermWpu20K   ,GpioLockDefault   ,GpioRxRaw1Default}},//EMMC_CMD
    {GPIO_CDF_GPP_O1 ,{GpioPadModeNative1,GpioHostOwnDefault,GpioDirDefault,GpioOutDefault,GpioIntDefault              ,GpioResetDefault,GpioTermWpd20K   ,GpioLockDefault   ,GpioRxRaw1Default}},//EMMC_STROBE
    {GPIO_CDF_GPP_O10,{GpioPadModeNative1,GpioHostOwnDefault,GpioDirDefault,GpioOutDefault,GpioIntDefault              ,GpioResetDefault,GpioTermWpu20K   ,GpioLockDefault   ,GpioRxRaw1Default}},//EMMC_D7
    {GPIO_CDF_GPP_O2 ,{GpioPadModeNative1,GpioHostOwnDefault,GpioDirDefault,GpioOutDefault,GpioIntDefault              ,GpioResetDefault,GpioTermWpu20K   ,GpioLockDefault   ,GpioRxRaw1Default}},//EMMC_CLK
    {GPIO_CDF_GPP_O3 ,{GpioPadModeNative1,GpioHostOwnDefault,GpioDirDefault,GpioOutDefault,GpioIntDefault              ,GpioResetDefault,GpioTermWpu20K   ,GpioLockDefault   ,GpioRxRaw1Default}},//EMMC_D0
    {GPIO_CDF_GPP_O4 ,{GpioPadModeNative1,GpioHostOwnDefault,GpioDirDefault,GpioOutDefault,GpioIntDefault              ,GpioResetDefault,GpioTermWpu20K   ,GpioLockDefault   ,GpioRxRaw1Default}},//EMMC_D1
    {GPIO_CDF_GPP_O5 ,{GpioPadModeNative1,GpioHostOwnDefault,GpioDirDefault,GpioOutDefault,GpioIntDefault              ,GpioResetDefault,GpioTermWpu20K   ,GpioLockDefault   ,GpioRxRaw1Default}},//EMMC_D2
    {GPIO_CDF_GPP_O6 ,{GpioPadModeNative1,GpioHostOwnDefault,GpioDirDefault,GpioOutDefault,GpioIntDefault              ,GpioResetDefault,GpioTermWpu20K   ,GpioLockDefault   ,GpioRxRaw1Default}},//EMMC_D3
    {GPIO_CDF_GPP_O7 ,{GpioPadModeNative1,GpioHostOwnDefault,GpioDirDefault,GpioOutDefault,GpioIntDefault              ,GpioResetDefault,GpioTermWpu20K   ,GpioLockDefault   ,GpioRxRaw1Default}},//EMMC_D4
    {GPIO_CDF_GPP_O8 ,{GpioPadModeNative1,GpioHostOwnDefault,GpioDirDefault,GpioOutDefault,GpioIntDefault              ,GpioResetDefault,GpioTermWpu20K   ,GpioLockDefault   ,GpioRxRaw1Default}},//EMMC_D5
    {GPIO_CDF_GPP_O9 ,{GpioPadModeNative1,GpioHostOwnDefault,GpioDirDefault,GpioOutDefault,GpioIntDefault              ,GpioResetDefault,GpioTermWpu20K   ,GpioLockDefault   ,GpioRxRaw1Default}},//EMMC_D6
};

static GPIO_INIT_CONFIG mGpioMoroCityRP[] = {
    //CODE_DEFINE    ,PAD_MODE           ,HOST_SOFTPAD_OWN  ,DIRECTION     ,OUTPUT_STATE  ,INTERRUPT_CONFIG|INT_TRIGGER,POWER_CONFIG    ,ELECTRICAL_CONFIG,LOCK_CONFIG      ,OTHER_SETTINGS     ,//GPIO_NAME|BOARD_NAME
    {GPIO_CDF_GPP_A0 ,{GpioPadModeNative1,GpioHostOwnDefault,GpioDirDefault,GpioOutDefault,GpioIntDefault              ,GpioResetDefault,GpioTermDefault  ,GpioLockDefault  ,GpioRxRaw1Default}},//GBE_SDP_TIMESYNC0|GBE_SDP_TIMESYNC0_S2N
    {GPIO_CDF_GPP_A1 ,{GpioPadModeNative1,GpioHostOwnDefault,GpioDirDefault,GpioOutDefault,GpioIntDefault              ,GpioResetDefault,GpioTermDefault  ,GpioLockDefault  ,GpioRxRaw1Default}},//GBE_SDP_TIMESYNC1|GBE_SDP_TIMESYNC1_S2N
    {GPIO_CDF_GPP_A2 ,{GpioPadModeNative1,GpioHostOwnDefault,GpioDirDefault,GpioOutDefault,GpioIntDefault              ,GpioResetDefault,GpioTermDefault  ,GpioLockDefault  ,GpioRxRaw1Default}},//GBE_SDP_TIMESYNC2|GBE_SDP_TIMESYNC2_S2N
    {GPIO_CDF_GPP_A3 ,{GpioPadModeNative1,GpioHostOwnDefault,GpioDirDefault,GpioOutDefault,GpioIntDefault              ,GpioResetDefault,GpioTermDefault  ,GpioLockDefault  ,GpioRxRaw1Default}},//GBE_SDP_TIMESYNC3|GBE_SDP_TIMESYNC3_S2N
    {GPIO_CDF_GPP_C0 ,{GpioPadModeGpio   ,GpioHostOwnGpio   ,GpioDirIn     ,GpioOutDefault,GpioIntDefault              ,GpioResetDefault,GpioTermDefault  ,GpioLockDefault  ,GpioRxRaw1Default}},//CPU_GP_0|IRQ_BMC_PCH_NMI
    //{GPIO_CDF_GPP_C1 ,{GpioPadModeGpio   ,GpioHostOwnDefault,GpioDirDefault,GpioOutDefault,GpioIntDefault              ,GpioResetDefault,GpioTermDefault  ,GpioLockDefault  ,GpioRxRaw1Default}},//CPU_GP_1|FM_NM _THROTTLE_N
    {GPIO_CDF_GPP_C10,{GpioPadModeGpio   ,GpioHostOwnGpio   ,GpioDirIn     ,GpioOutDefault,GpioIntDefault              ,GpioResetDefault,GpioTermDefault  ,GpioPadLock      ,GpioRxRaw1Default}},//FAN_TACH_2|Spare
    {GPIO_CDF_GPP_C11,{GpioPadModeGpio   ,GpioHostOwnGpio   ,GpioDirIn     ,GpioOutDefault,GpioIntDefault              ,GpioResetDefault,GpioTermDefault  ,GpioLockDefault  ,GpioRxRaw1Default}},//FAN_TACH_3|FM_BIOS_ADV_FUNCTION
    {GPIO_CDF_GPP_C12,{GpioPadModeNative3,GpioHostOwnDefault,GpioDirDefault,GpioOutDefault,GpioIntDefault              ,GpioResetDefault,GpioTermDefault  ,GpioPadConfigLock,GpioRxRaw1Default}},//ME_SMB0_CLK
    {GPIO_CDF_GPP_C13,{GpioPadModeNative3,GpioHostOwnDefault,GpioDirDefault,GpioOutDefault,GpioIntDefault              ,GpioResetDefault,GpioTermDefault  ,GpioPadConfigLock,GpioRxRaw1Default}},//ME_SMB0_DATA
    {GPIO_CDF_GPP_C14,{GpioPadModeNative3,GpioHostOwnDefault,GpioDirDefault,GpioOutDefault,GpioIntDefault              ,GpioResetDefault,GpioTermDefault  ,GpioLockDefault  ,GpioRxRaw1Default}},//ME_SMB0_ALRT_N
    {GPIO_CDF_GPP_C4 ,{GpioPadModeGpio   ,GpioHostOwnGpio   ,GpioDirOut    ,GpioOutHigh   ,GpioIntDefault              ,GpioResetDefault,GpioTermDefault  ,GpioPadLock      ,GpioRxRaw1Default}},//FAN_PWM_0|FM_MC_INIT
    {GPIO_CDF_GPP_C5 ,{GpioPadModeGpio   ,GpioHostOwnGpio   ,GpioDirOut    ,GpioOutHigh   ,GpioIntDefault              ,GpioResetDefault,GpioTermDefault  ,GpioPadLock      ,GpioRxRaw1Default}},//FAN_PWM_1|FM_ADR_SMI_GPIO_N
    {GPIO_CDF_GPP_C6 ,{GpioPadModeGpio   ,GpioHostOwnGpio   ,GpioDirInOut  ,GpioOutHigh   ,GpioIntDefault              ,GpioResumeReset ,GpioTermDefault  ,GpioPadConfigLock,GpioRxRaw1Default}},//FAN_PWM_2|FM_ADR_EN_DATAOUT
    {GPIO_CDF_GPP_C7 ,{GpioPadModeGpio   ,GpioHostOwnGpio   ,GpioDirOut    ,GpioOutHigh   ,GpioIntDefault              ,GpioResumeReset ,GpioTermDefault  ,GpioPadLock      ,GpioRxRaw1Default}},//FAN_PWM_3|FM_ADR_EN_CLK
    {GPIO_CDF_GPP_C8 ,{GpioPadModeGpio   ,GpioHostOwnGpio   ,GpioDirIn     ,GpioOutHigh   ,GpioIntDefault              ,GpioResetDefault,GpioTermDefault  ,GpioLockDefault  ,GpioRxRaw1Default}},//FAN_TACH_0|Spare
    {GPIO_CDF_GPP_C9 ,{GpioPadModeGpio   ,GpioHostOwnGpio   ,GpioDirIn     ,GpioOutHigh   ,GpioIntDefault              ,GpioResetDefault,GpioTermDefault  ,GpioLockDefault  ,GpioRxRaw1Default}},//FAN_TACH_1|Spare
    {GPIO_CDF_GPP_D10,{GpioPadModeGpio   ,GpioHostOwnGpio   ,GpioDirIn     ,GpioOutDefault,GpioIntDefault              ,GpioResetDefault,GpioTermDefault  ,GpioLockDefault  ,GpioRxRaw1Default}},//PCIE_CLKREQ5_N|NET_PLL_INTR_N
    {GPIO_CDF_GPP_D11,{GpioPadModeGpio   ,GpioHostOwnGpio   ,GpioDirOut    ,GpioOutHigh   ,GpioIntDefault              ,GpioResetDefault,GpioTermDefault  ,GpioLockDefault  ,GpioRxRaw1Default}},//PCIE_CLKREQ6_N| FM_ME_VLN_1
    {GPIO_CDF_GPP_D12,{GpioPadModeGpio   ,GpioHostOwnGpio   ,GpioDirOut    ,GpioOutHigh   ,GpioIntDefault              ,GpioResetDefault,GpioTermDefault  ,GpioLockDefault  ,GpioRxRaw1Default}},//PCIE_CLKREQ7_N| FM_ME_VLN_2
    {GPIO_CDF_GPP_D13,{GpioPadModeGpio   ,GpioHostOwnGpio   ,GpioDirOut    ,GpioOutHigh   ,GpioIntDefault              ,GpioResetDefault,GpioTermDefault  ,GpioPadLock      ,GpioRxRaw1Default}},//PCIE_CLKREQ8_N|Spare
    {GPIO_CDF_GPP_D14,{GpioPadModeGpio   ,GpioHostOwnGpio   ,GpioDirInOut  ,GpioOutDefault,GpioIntDefault              ,GpioResetDefault,GpioTermDefault  ,GpioPadUnlock    ,GpioRxRaw1Default}},//PCIE_CLKREQ9_N|SATA_RAID_KEY
    {GPIO_CDF_GPP_D9 ,{GpioPadModeGpio   ,GpioHostOwnGpio   ,GpioDirOut    ,GpioOutHigh   ,GpioIntDefault              ,GpioResetDefault,GpioTermDefault  ,GpioPadLock      ,GpioRxRaw1Default}},//PCIE_CLKREQ4_N|FM_NIC1_DISABLE_N
    {GPIO_CDF_GPP_H10,{GpioPadModeGpio   ,GpioHostOwnGpio   ,GpioDirIn     ,GpioOutDefault,GpioIntDefault              ,GpioResetDefault,GpioTermDefault  ,GpioLockDefault  ,GpioRxRaw1Default}},//DBG_SPARE2|FM_BIOS_IMAGE_SWAP_2
    //{GPIO_CDF_GPP_H11,{GpioPadModeGpio   ,GpioHostOwnGpio   ,GpioDirIn     ,GpioOutDefault,GpioIntSci|GpioIntLevel     ,GpioResetDefault,GpioTermDefault  ,GpioPadConfigLock,GpioRxRaw1Default}},//DBG_SPARE3|IRQ_PCH_SCI_WHEA_N
    {GPIO_CDF_GPP_H8 ,{GpioPadModeGpio   ,GpioHostOwnGpio   ,GpioDirIn     ,GpioOutDefault,GpioIntDefault              ,GpioResetDefault,GpioTermDefault  ,GpioLockDefault  ,GpioRxRaw1Default}},//DBG_SPARE0|FM_PASSWORD_CLEAR_N
    {GPIO_CDF_GPP_H9 ,{GpioPadModeGpio   ,GpioHostOwnGpio   ,GpioDirIn     ,GpioOutDefault,GpioIntDefault              ,GpioResetDefault,GpioTermDefault  ,GpioLockDefault  ,GpioRxRaw1Default}},//DBG_SPARE1|FM_BIOS_IMAGE_SWAP_1
    {GPIO_CDF_GPP_L10,{GpioPadModeGpio   ,GpioHostOwnGpio   ,GpioDirOut    ,GpioOutHigh   ,GpioIntDefault              ,GpioResetDefault,GpioTermDefault  ,GpioLockDefault  ,GpioRxRaw1Default}},//GPIO_9|FM_BIOS_POST_CMPLT_N
    {GPIO_CDF_GPP_L11,{GpioPadModeGpio   ,GpioHostOwnGpio   ,GpioDirOut    ,GpioOutHigh   ,GpioIntDefault              ,GpioResetDefault,GpioTermDefault  ,GpioPadLock      ,GpioRxRaw1Default}},//GPIO_10|IRQ_SMI_ACTIVE_N
    {GPIO_CDF_GPP_L12,{GpioPadModeGpio   ,GpioHostOwnGpio   ,GpioDirIn     ,GpioOutDefault,GpioIntApic|GpioIntLevel    ,GpioResetDefault,GpioTermDefault  ,GpioLockDefault  ,GpioRxRaw1Default}},//GPIO_11|IRQ_TPM_SPI_N
    {GPIO_CDF_GPP_L13,{GpioPadModeGpio   ,GpioHostOwnGpio   ,GpioDirIn     ,GpioOutDefault,GpioIntDefault              ,GpioResetDefault,GpioTermDefault  ,GpioLockDefault  ,GpioRxRaw1Default}},//GPIO_12|PLD_PROG_N
    {GPIO_CDF_GPP_L14,{GpioPadModeGpio   ,GpioHostOwnGpio   ,GpioDirOut    ,GpioOutHigh   ,GpioIntDefault              ,GpioResetDefault,GpioTermDefault  ,GpioPadLock      ,GpioRxRaw1Default}},//PECI_SMB_DATA|I2C_Buffer_Enable
    {GPIO_CDF_GPP_L15,{GpioPadModeNative2,GpioHostOwnDefault,GpioDirDefault,GpioOutDefault,GpioIntDefault              ,GpioResetDefault,GpioTermDefault  ,GpioLockDefault  ,GpioRxRaw1Default}},//SATA0_LED_N|spare
    {GPIO_CDF_GPP_L17,{GpioPadModeNative2,GpioHostOwnDefault,GpioDirDefault,GpioOutDefault,GpioIntDefault              ,GpioResetDefault,GpioTermDefault  ,GpioLockDefault  ,GpioRxRaw1Default}},//SATA_PDETECT0|Spare
    {GPIO_CDF_GPP_L19,{GpioPadModeNative2,GpioHostOwnDefault,GpioDirDefault,GpioOutDefault,GpioIntDefault              ,GpioResetDefault,GpioTermDefault  ,GpioLockDefault  ,GpioRxRaw1Default}},//SATA0_SDOUT|Spare
    {GPIO_CDF_GPP_L21,{GpioPadModeGpio   ,GpioHostOwnGpio   ,GpioDirOut    ,GpioOutHigh   ,GpioIntDefault              ,GpioResetDefault,GpioTermDefault  ,GpioPadLock      ,GpioRxRaw1Default}},//SATA2_LED_N|spare
    {GPIO_CDF_GPP_L22,{GpioPadModeGpio   ,GpioHostOwnGpio   ,GpioDirIn     ,GpioOutDefault,GpioIntDefault              ,GpioResetDefault,GpioTermDefault  ,GpioLockDefault  ,GpioRxRaw1Default}},//SATA_PDETECT2|FM_MFG_MODE
    {GPIO_CDF_GPP_L23,{GpioPadModeGpio   ,GpioHostOwnGpio   ,GpioDirIn     ,GpioOutHigh   ,GpioIntDefault              ,GpioResetDefault,GpioTermDefault  ,GpioLockDefault  ,GpioRxRaw1Default}},//SATA2_SDOUT|Spare
    {GPIO_CDF_GPP_L5 ,{GpioPadModeNative1,GpioHostOwnDefault,GpioDirDefault,GpioOutDefault,GpioIntDefault              ,GpioResetDefault,GpioTermWpu20K   ,GpioLockDefault  ,GpioRxRaw1Default}},//GPIO_4
    {GPIO_CDF_GPP_L9 ,{GpioPadModeNative1,GpioHostOwnDefault,GpioDirDefault,GpioOutDefault,GpioIntDefault              ,GpioResetDefault,GpioTermDefault  ,GpioLockDefault  ,GpioRxRaw1Default}},//GPIO_8|SPI_TPM_CS_N
    {GPIO_CDF_GPP_N10,{GpioPadModeNative1,GpioHostOwnDefault,GpioDirDefault,GpioOutDefault,GpioIntDefault              ,GpioResetDefault,GpioTermDefault  ,GpioPadLock      ,GpioRxRaw1Default}},//ADR_COMPLETE|Spare
    //{GPIO_CDF_GPP_N19,{GpioPadModeGpio   ,GpioHostOwnDefault,GpioDirDefault,GpioOutDefault,GpioIntDefault              ,GpioResetDefault,GpioTermDefault  ,GpioLockDefault  ,GpioRxRaw1Default}},//PMU_I2C_CLK|PMBUS_ALERT_EN
    //{GPIO_CDF_GPP_N20,{GpioPadModeGpio   ,GpioHostOwnGpio   ,GpioDirIn     ,GpioOutDefault,GpioIntDefault              ,GpioResetDefault,GpioTermDefault  ,GpioLockDefault  ,GpioRxRaw1Default}},//PMU_I2C_DATA|PMBUS_ALERT_N
    {GPIO_CDF_GPP_N21,{GpioPadModeGpio   ,GpioHostOwnGpio   ,GpioDirIn     ,GpioOutDefault,GpioIntDefault              ,GpioResetDefault,GpioTermDefault  ,GpioLockDefault  ,GpioRxRaw1Default}},//PECI_SMB_CLK|FM_SSATA_PCIE_M2_1_SEL_1
    //{GPIO_CDF_GPP_N22,{GpioPadModeGpio   ,GpioHostOwnGpio   ,GpioDirIn     ,GpioOutDefault,GpioIntDefault              ,GpioResetDefault,GpioTermDefault  ,GpioLockDefault  ,GpioRxRaw1Default}},//PECI_SMB_ALRT_N|FM_ME_RCVR_N
    {GPIO_CDF_GPP_N3 ,{GpioPadModeNative1,GpioHostOwnDefault,GpioDirDefault,GpioOutDefault,GpioIntDefault              ,GpioResetDefault,GpioTermDefault  ,GpioLockDefault  ,GpioRxRaw1Default}},//SPI_MISO_IO1|SPI_MISO_IO1
    {GPIO_CDF_GPP_O0 ,{GpioPadModeNative1,GpioHostOwnDefault,GpioDirDefault,GpioOutDefault,GpioIntDefault              ,GpioResetDefault,GpioTermWpu20K   ,GpioLockDefault  ,GpioRxRaw1Default}},//EMMC_CMD
    {GPIO_CDF_GPP_O1 ,{GpioPadModeNative1,GpioHostOwnDefault,GpioDirDefault,GpioOutDefault,GpioIntDefault              ,GpioResetDefault,GpioTermWpd20K   ,GpioLockDefault  ,GpioRxRaw1Default}},//EMMC_STROBE
    {GPIO_CDF_GPP_O10,{GpioPadModeNative1,GpioHostOwnDefault,GpioDirDefault,GpioOutDefault,GpioIntDefault              ,GpioResetDefault,GpioTermWpu20K   ,GpioLockDefault  ,GpioRxRaw1Default}},//EMMC_D7
    {GPIO_CDF_GPP_O2 ,{GpioPadModeNative1,GpioHostOwnDefault,GpioDirDefault,GpioOutDefault,GpioIntDefault              ,GpioResetDefault,GpioTermWpu20K   ,GpioLockDefault  ,GpioRxRaw1Default}},//EMMC_CLK
    {GPIO_CDF_GPP_O3 ,{GpioPadModeNative1,GpioHostOwnDefault,GpioDirDefault,GpioOutDefault,GpioIntDefault              ,GpioResetDefault,GpioTermWpu20K   ,GpioLockDefault  ,GpioRxRaw1Default}},//EMMC_D0
    {GPIO_CDF_GPP_O4 ,{GpioPadModeNative1,GpioHostOwnDefault,GpioDirDefault,GpioOutDefault,GpioIntDefault              ,GpioResetDefault,GpioTermWpu20K   ,GpioLockDefault  ,GpioRxRaw1Default}},//EMMC_D1
    {GPIO_CDF_GPP_O5 ,{GpioPadModeNative1,GpioHostOwnDefault,GpioDirDefault,GpioOutDefault,GpioIntDefault              ,GpioResetDefault,GpioTermWpu20K   ,GpioLockDefault  ,GpioRxRaw1Default}},//EMMC_D2
    {GPIO_CDF_GPP_O6 ,{GpioPadModeNative1,GpioHostOwnDefault,GpioDirDefault,GpioOutDefault,GpioIntDefault              ,GpioResetDefault,GpioTermWpu20K   ,GpioLockDefault  ,GpioRxRaw1Default}},//EMMC_D3
    {GPIO_CDF_GPP_O7 ,{GpioPadModeNative1,GpioHostOwnDefault,GpioDirDefault,GpioOutDefault,GpioIntDefault              ,GpioResetDefault,GpioTermWpu20K   ,GpioLockDefault  ,GpioRxRaw1Default}},//EMMC_D4
    {GPIO_CDF_GPP_O8 ,{GpioPadModeNative1,GpioHostOwnDefault,GpioDirDefault,GpioOutDefault,GpioIntDefault              ,GpioResetDefault,GpioTermWpu20K   ,GpioLockDefault  ,GpioRxRaw1Default}},//EMMC_D5
    {GPIO_CDF_GPP_O9 ,{GpioPadModeNative1,GpioHostOwnDefault,GpioDirDefault,GpioOutDefault,GpioIntDefault              ,GpioResetDefault,GpioTermWpu20K   ,GpioLockDefault  ,GpioRxRaw1Default}},//EMMC_D6
};

/**
  Program Gpio Pins required for Pre Mem Init

  @param None
**/
VOID
PreMemGpioInit (
  VOID
)
{
  if (GetChopType (0) == TypeLcc) {
    ConfigureGpio (CDATA_NO_TAG, ARRAY_SIZE(mGpioBrightonCityRp), (UINT8*)mGpioBrightonCityRp);
  } else {
    ConfigureGpio (CDATA_NO_TAG, ARRAY_SIZE(mGpioMoroCityRP), (UINT8*)mGpioMoroCityRP);
  }
}
