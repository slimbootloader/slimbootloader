## @file
#
#  Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
#  This program and the accompanying materials
#  are licensed and made available under the terms and conditions of the BSD License
#  which accompanies this distribution.  The full text of the license may be found at
#  http://opensource.org/licenses/bsd-license.php
#
#  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
#  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
#
##

  # ---------------------------------------------------------------------------------------
  # !BSF PAGE:{GIO}
  # !BSF SUBT:{CFGHDR_TMPL:GPIO_CFG_DATA:1:0}  //Flags - bit 12 (Reuse config from base platform ID) is set to 1

  # !HDR HEADER:{OFF}
  # !HDR EMBED:{GPIO_CFG_DATA:TAG_400:START}


  # !HDR HEADER:{ON}
  # !HDR EMBED:{GPIO_CFG_HDR:GpioCfgHdr:START}

  # !BSF NAME:{ } HELP:{Size of structure GPIO_CFG_HDR} TYPE:{Reserved}
  gCfgData.GpioHeaderSize         |      * | 0x01  | _LENGTH_GPIO_CFG_HDR_
  # !BSF NAME:{ } HELP:{GPIO base table platform ID} TYPE:{Reserved}
  gCfgData.GpioBaseTableId        |      * | 0x01  | 0xFF
  # !BSF NAME:{ } HELP:{GPIO data size in byte for each GPIO} TYPE:{Reserved}
  gCfgData.GpioItemSize           |      * | 0x02  | (_OFFSET_GPIO_DATA_GPP_A1_ - _OFFSET_GPIO_DATA_GPP_A0_)
  # !BSF NAME:{ } HELP:{GPIO count in the table} TYPE:{Reserved}
  gCfgData.GpioItemCount          |      * | 0x02  | (_LENGTH_GPIO_CFG_DATA_ - _LENGTH_GPIO_CFG_HDR_) / (_OFFSET_GPIO_DATA_GPP_A1_ - _OFFSET_GPIO_DATA_GPP_A0_)

  # Initial value is GpioValidByteOffset (bit 31 in PAD Cfg DW2) and GpioValidByteMask (0x80) in GpioBaseTableBitMask[0] and [1]
  # CfgDataTool will check each GPIO table data for:
  #    *(UINT8 *)(GpioEntryData + GpioValidByteOffset) & GpioValidByteMask
  #    Ex: (GPIO DW1 >> 24) & 0x80  == if set, skip it. if 0, include this GPIO
  # If the value is non-zero, a GPIO programming should be skipped.
  # The CfgDataTool tool will mark a bit in GpioBaseTableBitMask array to indicate a particular GPIO should be skipped or not.
  #   Length  = Need 1 bit per GPIO entry. There are 8 GPIOs in the below list.
  #   So 8/8 = 1 bytes needed. Minimum it needs 2 bytes. So reserve 2.
  # !BSF NAME:{ } HELP:{GPIO bit mask for base table} TYPE:{Reserved}
  gCfgData.GpioBaseTableBitMask   |      * |    2  | {7, 0x80}  #7 is byte offset 7 within the each GPIO bytes array.  0x80 is the byte mask to indicate if the GPIO pin is used or not.

  gCfgData.GpioTableData          |      * |    0  | 0
  # !HDR EMBED:{GPIO_CFG_HDR:GpioCfgHdr:END}

  # !HDR HEADER:{OFF}
  #                                    DW0   :     DW1

  # !BSF SUBT:{GPIO_TMPL:GPP_A0  : 0x00000000: 0x00000010}
  # !BSF SUBT:{GPIO_TMPL:GPP_A1  : 0x40000001: 0x00000011}
  # !BSF SUBT:{GPIO_TMPL:GPP_A2  : 0x80000002: 0x00000012}
  # !BSF SUBT:{GPIO_TMPL:GPP_A3  : 0xC0000003: 0x00000013}
  # !BSF SUBT:{GPIO_TMPL:GPP_A4  : 0x01000004: 0x00000014}
  # !BSF SUBT:{GPIO_TMPL:GPP_A5  : 0x41000005: 0x00000015}
  # !BSF SUBT:{GPIO_TMPL:GPP_A6  : 0x81000006: 0x00000016}
  # !BSF SUBT:{GPIO_TMPL:GPP_A7  : 0xC1000007: 0x00000017}

  # !HDR EMBED:{GPIO_CFG_DATA:TAG_400:END}

  # !HDR HEADER:{ON}