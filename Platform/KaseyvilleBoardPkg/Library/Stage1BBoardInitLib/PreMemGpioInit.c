/** @file

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Uefi/UefiBaseType.h>
#include <Library/BaseLib.h>
#include <Library/IoLib.h>
#include <Library/DebugLib.h>
#include <Library/BootloaderCommonLib.h>
#include <GpioV2PinsIbl.h>
#include <Library/ConfigDataLib.h>
#include <Library/GpioV2Lib.h>
#include <Include/IblSbPortIds.h>

#define MAX_GPIO_PINS             100

static GPIOV2_INIT_CONFIG mGpioTableKaseyvilleInternalRp [] =
{
   //GPIO_PIN           ,PAD_MODE_DEFAULT            ,HOSTSW_OWN            ,GPIO_PIN_DIRECTION,OUTPUT_STATE       ,INTERRUPT_CONFIG|INTERRUPT_TRIGGER   ,  PAD_RESET      ,ELECTRICAL_CONFIG ,PAD_CONFIG_LOCK,   LOCK_Tx                , OTHER_SETTINGS     ,vGPIO_OUT_CS   , RB,//CCA_SIGNAL_NAME
  {GPIOV2_IBL_GPP_C1    ,{GpioV2PadModeGpio          ,GpioV2HostOwnDefault  ,GpioV2DirOut      ,GpioV2StateLow     ,GpioV2IntDis                         ,GpioV2ResetHost  ,GpioV2TermDefault ,GpioV2Lock     ,GpioV2LockHardwareDefault ,GpioV2RxRaw1Default ,             0 ,0}},//[O] RST_SMB_HOST_MUX_N
  {GPIOV2_IBL_GPP_C2    ,{GpioV2PadModeGpio          ,GpioV2HostOwnAcpi     ,GpioV2DirIn       ,GpioV2StateDefault ,GpioV2IntApic|GpioV2IntLevel         ,GpioV2ResetHost  ,GpioV2TermDefault ,GpioV2Lock     ,GpioV2LockHardwareDefault ,GpioV2RxRaw1Dis     ,             0 ,0}},//[I] IRQ_CPU0_TPM_N
  {GPIOV2_IBL_GPP_C3    ,{GpioV2PadModeGpio          ,GpioV2HostOwnDefault  ,GpioV2DirIn       ,GpioV2StateDefault ,GpioV2IntDis                         ,GpioV2ResetHost  ,GpioV2TermDefault ,GpioV2Lock     ,GpioV2LockHardwareDefault ,GpioV2RxRaw1Dis     ,             0 ,0}},//[I] FM_MFG_MODE
  {GPIOV2_IBL_GPP_C4    ,{GpioV2PadModeGpio          ,GpioV2HostOwnDefault  ,GpioV2DirIn       ,GpioV2StateDefault ,GpioV2IntDis                         ,GpioV2ResetHost  ,GpioV2TermDefault ,GpioV2Unlock   ,GpioV2Unlock              ,GpioV2RxRaw1Dis     ,             0 ,0}},//[I]  FM_CPU0_GPIO4_EAF_BMC_SAFS_SEL_LVC1
  {GPIOV2_IBL_GPP_C5    ,{GpioV2PadModeGpio          ,GpioV2HostOwnDefault  ,GpioV2DirIn       ,GpioV2StateDefault ,GpioV2IntDis                         ,GpioV2ResetHost  ,GpioV2TermDefault ,GpioV2Lock     ,GpioV2LockHardwareDefault ,GpioV2RxRaw1Dis     ,             0 ,0}},//[I]  IRQ_PLL_INT_LVC3_N
  {GPIOV2_IBL_GPP_C6    ,{GpioV2PadModeGpio          ,GpioV2HostOwnDefault  ,GpioV2DirOut      ,GpioV2StateHigh    ,GpioV2IntDis                         ,GpioV2ResetHost  ,GpioV2TermDefault ,GpioV2Lock     ,GpioV2LockHardwareDefault ,GpioV2RxRaw1Default ,             0 ,0}},//[O] FM_NIC1_DISABLE_N
  {GPIOV2_IBL_GPP_C7    ,{GpioV2PadModeGpio          ,GpioV2HostOwnDefault  ,GpioV2DirOut      ,GpioV2StateHigh    ,GpioV2IntDis                         ,GpioV2ResetHost  ,GpioV2TermDefault ,GpioV2Lock     ,GpioV2LockHardwareDefault ,GpioV2RxRaw1Default ,             0 ,0}},//[O] FM_TPM_EN_PULSE
  {GPIOV2_IBL_GPP_C8    ,{GpioV2PadModeGpio          ,GpioV2HostOwnDefault  ,GpioV2DirOut      ,GpioV2StateHigh    ,GpioV2IntDis                         ,GpioV2ResetHost  ,GpioV2TermDefault ,GpioV2Lock     ,GpioV2LockHardwareDefault ,GpioV2RxRaw1Default ,             0 ,0}},//[O] RST_BMC_SMB_HSBP_MUX_N
  {GPIOV2_IBL_GPP_C9    ,{GpioV2PadModeGpio          ,GpioV2HostOwnDefault  ,GpioV2DirIn       ,GpioV2StateDefault ,GpioV2IntDis                         ,GpioV2ResetHost  ,GpioV2TermDefault ,GpioV2Lock     ,GpioV2LockHardwareDefault ,GpioV2RxRaw1Dis     ,             0 ,0}},//[I] FM_M2_ALERT_LVC18_R_N
  {GPIOV2_IBL_GPP_C12   ,{GpioV2PadModeGpio          ,GpioV2HostOwnDefault  ,GpioV2DirOut      ,GpioV2StateLow     ,GpioV2IntDis                         ,GpioV2ResetHost  ,GpioV2TermDefault ,GpioV2Lock     ,GpioV2LockHardwareDefault ,GpioV2RxRaw1Default ,             0 ,0}},//[O] FM_GPIO12_ADR_MODE_SEL1_R
  {GPIOV2_IBL_GPP_C13   ,{GpioV2PadModeGpio          ,GpioV2HostOwnDefault  ,GpioV2DirOut      ,GpioV2StateLow     ,GpioV2IntDis                         ,GpioV2ResetHost  ,GpioV2TermDefault ,GpioV2Lock     ,GpioV2LockHardwareDefault ,GpioV2RxRaw1Default ,             0 ,0}},//[O] FM_GPIO13_ADR_MODE_SEL0_R
  {GPIOV2_IBL_GPP_C15   ,{GpioV2PadModeGpio          ,GpioV2HostOwnDefault  ,GpioV2DirOut      ,GpioV2StateHigh    ,GpioV2IntDis                         ,GpioV2ResetHost  ,GpioV2TermDefault ,GpioV2Lock     ,GpioV2LockHardwareDefault ,GpioV2RxRaw1Default ,             0 ,0}},//[O] FM_I3C_SEL_HUB_GATE_LVC18_R
  {GPIOV2_IBL_GPP_C16   ,{GpioV2PadModeGpio          ,GpioV2HostOwnDefault  ,GpioV2DirIn       ,GpioV2StateDefault ,GpioV2IntDis                         ,GpioV2ResetHost  ,GpioV2TermDefault ,GpioV2Lock     ,GpioV2LockHardwareDefault ,GpioV2RxRaw1Dis     ,             0 ,0}},//[I] M_INTRUDER_HDR_BMC_LVC18_R_N
  {GPIOV2_IBL_GPP_C17   ,{GpioV2PadModeGpio          ,GpioV2HostOwnDefault  ,GpioV2DirIn       ,GpioV2StateDefault ,GpioV2IntDis                         ,GpioV2ResetHost  ,GpioV2TermDefault ,GpioV2Lock     ,GpioV2LockHardwareDefault ,GpioV2RxRaw1Dis     ,             0 ,0}},//[I] FM_1200VA_OC_LVC18_R
  {GPIOV2_IBL_GPP_C18   ,{GpioV2PadModeGpio          ,GpioV2HostOwnDefault  ,GpioV2DirIn       ,GpioV2StateDefault ,GpioV2IntDis                         ,GpioV2ResetHost  ,GpioV2TermDefault ,GpioV2Lock     ,GpioV2LockHardwareDefault ,GpioV2RxRaw1Dis     ,             0 ,0}},//[I] UART_ESPI_USB_GPIO_LVC18_R
  {GPIOV2_IBL_GPP_C19   ,{GpioV2PadModeGpio          ,GpioV2HostOwnDefault  ,GpioV2DirIn       ,GpioV2StateDefault ,GpioV2IntDis                         ,GpioV2ResetHost  ,GpioV2TermDefault ,GpioV2Lock     ,GpioV2LockHardwareDefault ,GpioV2RxRaw1Dis     ,             0 ,0}},//[I] FM_PMBUS_ALERT_LVC18_R_N
  {GPIOV2_IBL_GPP_C20   ,{GpioV2PadModeGpio          ,GpioV2HostOwnDefault  ,GpioV2DirOut      ,GpioV2StateLow     ,GpioV2IntDis                         ,GpioV2ResetHost  ,GpioV2TermDefault ,GpioV2Lock     ,GpioV2LockHardwareDefault ,GpioV2RxRaw1Default ,             0 ,0}},//[O] FM_PMBUS_ALERT_LVC18_R_EN
  {GPIOV2_IBL_GPP_D0    ,{GpioV2PadModeGpio          ,GpioV2HostOwnDefault  ,GpioV2DirIn       ,GpioV2StateDefault ,GpioV2IntDis                         ,GpioV2ResetHost  ,GpioV2TermDefault ,GpioV2Lock     ,GpioV2LockHardwareDefault ,GpioV2RxRaw1Dis     ,             0 ,0}},//[I] GPP_BIOS_UNCORE_TREE_LVC18
  {GPIOV2_IBL_GPP_D1    ,{GpioV2PadModeGpio          ,GpioV2HostOwnDefault  ,GpioV2DirOut      ,GpioV2StateHigh    ,GpioV2IntDis                         ,GpioV2ResetHost  ,GpioV2TermDefault ,GpioV2Lock     ,GpioV2LockHardwareDefault ,GpioV2RxRaw1Default ,             0 ,0}},//[O] FM_NM_THROTTLE_LVC18_N
  {GPIOV2_IBL_GPP_D4    ,{GpioV2PadModeGpio          ,GpioV2HostOwnDefault  ,GpioV2DirIn       ,GpioV2StateDefault ,GpioV2IntDis                         ,GpioV2ResetHost  ,GpioV2TermDefault ,GpioV2Unlock   ,GpioV2LockHardwareDefault ,GpioV2RxRaw1Default ,             0 ,0}},// H_CPU0_NMI_LCV18
  {GPIOV2_IBL_GPP_D11   ,{GpioV2PadModeGpio          ,GpioV2HostOwnDefault  ,GpioV2DirIn       ,GpioV2StateDefault ,GpioV2IntDis                         ,GpioV2ResetHost  ,GpioV2TermDefault ,GpioV2Lock     ,GpioV2LockHardwareDefault ,GpioV2RxRaw1Dis     ,             0 ,0}},// MEMORY_INTERPOSER_ID0
  {GPIOV2_IBL_GPP_D12   ,{GpioV2PadModeGpio          ,GpioV2HostOwnDefault  ,GpioV2DirIn       ,GpioV2StateDefault ,GpioV2IntDis                         ,GpioV2ResetHost  ,GpioV2TermDefault ,GpioV2Lock     ,GpioV2LockHardwareDefault ,GpioV2RxRaw1Dis     ,             0 ,0}},// MEMORY_INTERPOSER_ID1
  {GPIOV2_IBL_GPP_D13   ,{GpioV2PadModeGpio          ,GpioV2HostOwnDefault  ,GpioV2DirIn       ,GpioV2StateDefault ,GpioV2IntDis                         ,GpioV2ResetHost  ,GpioV2TermDefault ,GpioV2Lock     ,GpioV2LockHardwareDefault ,GpioV2RxRaw1Dis     ,             0 ,0}},// MEMORY_INTERPOSER_ID2
  {0} // terminator
};


/**
  Count the number of GPIO settings in the Table.

  @param[in]  GpioTable   The pointer of GPIO config table
  @param[out] GpioCount   The number of GPIO config entries
**/
VOID
GetGpioTableSize (
  GPIOV2_INIT_CONFIG   *GpioTable,
  OUT UINT16           *GpioCount
  )
{
  *GpioCount = 0;
  if(GpioTable != NULL) {
    while (GpioTable[*GpioCount].GpioPad != 0 && *GpioCount < MAX_GPIO_PINS) {
      DEBUG ((DEBUG_VERBOSE, "GpioTable[%d]->GpioPad = %x \n", *GpioCount, GpioTable[*GpioCount].GpioPad));
      (*GpioCount) ++;
    }
  } else {
    DEBUG ((DEBUG_VERBOSE, "GpioTable is NULL\n"));
  }
  DEBUG ((DEBUG_VERBOSE, "GetGpioTableSize() GpioCount = %d\n", *GpioCount));
}

/**
  Program Gpio Pins required for Pre Mem Init

  @param None
**/
VOID
PreMemGpioInit (
  VOID
)
{
  UINT16                          GpioCount;

  if (sizeof(mGpioTableKaseyvilleInternalRp)/sizeof(GPIOV2_INIT_CONFIG) != 0) {
    GpioCount = 0;
    GetGpioTableSize (mGpioTableKaseyvilleInternalRp, &GpioCount);
    if (GpioCount != 0) {
      ConfigureGpioV2 (CDATA_NO_TAG, (VOID *) mGpioTableKaseyvilleInternalRp, (UINTN) GpioCount);
    }
  }
}
