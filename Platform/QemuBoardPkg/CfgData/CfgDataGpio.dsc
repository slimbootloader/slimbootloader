## @file
#
#  Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
#  SPDX-License-Identifier: BSD-2-Clause-Patent
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

  # Bit start offset within each GPIO entry array to identify a GPIO pin uniquely. EX: GPIO pin id
  # Offset is 2nd DWORD BIT0 = 1 * 32 + 0 = 32
  gCfgData.GpioItemIdBitOff       |      * | 0x01  | 32
  # Bit length within each GPIO entry array to identify a GPIO pin uniquely.
  # Length is 2nd DWORD BIT0 to BIT15 = 16
  gCfgData.GpioItemIdBitLen       |      * | 0x01  | 16
  # Bit offset within each GPIO entry array to indicate SKIP a GPIO programming
  # Offset is 2nd DWORD BIT31 = 63
  gCfgData.GpioItemValidBitOff    |      * | 0x01  | 63
  gCfgData.GpioItemUnused         |      * | 0x01  | 0

  # Need 1 bit per GPIO. So this mask byte length needs to be at least (GpioNumber + 7) / 8
  # Padding can be added to let the whole length aligned at DWORD boundary
  gCfgData.GpioBaseTableBitMask   |      * |    2  | {0}

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