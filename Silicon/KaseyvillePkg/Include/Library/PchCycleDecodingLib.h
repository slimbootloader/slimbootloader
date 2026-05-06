/** @file

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#ifndef _PCH_CYCLE_DECODING_LIB_H_
#define _PCH_CYCLE_DECODING_LIB_H_

///
/// structure of LPC general IO range register
/// It contains base address, address mask, and enable status.
///
typedef struct {
  UINT32                                BaseAddr :16;
  UINT32                                Length   :15;
  UINT32                                Enable   : 1;
} PCH_LPC_GEN_IO_RANGE;

#define PCH_LPC_GEN_IO_RANGE_MAX        4
#define ESPI_CS1_GEN_IO_RANGE_MAX       1

///
/// structure of LPC general IO range register list
/// It lists all LPC general IO ran registers supported by PCH.
///
typedef struct {
  PCH_LPC_GEN_IO_RANGE                  Range[PCH_LPC_GEN_IO_RANGE_MAX];
} PCH_LPC_GEN_IO_RANGE_LIST;

typedef enum {
  DeviceEspiCS0,
  DeviceEspiCS1,
  DeviceIdMax
} DEVICE_ID_INDEX;

/**
  Set IBL eSPI IO decode ranges.
  Program eSPI I/O Decode Ranges in eSPI PCI offset 80h.
  Please check EDS for detail of eSPI IO decode ranges bit definition.
  Bit  12: FDD range
  Bit 9:8: LPT range
  Bit 6:4: ComB range
  Bit 2:0: ComA range

  @param[in] LpcIoDecodeRanges          eSPI IO decode ranges bit settings.

  @retval EFI_SUCCESS                   Successfully completed.
**/
EFI_STATUS
EFIAPI
PchLpcIoDecodeRangesSet (
  IN  UINT16                            LpcIoDecodeRanges
  );

/**
  Set IBL eSPI and eSPI CS1# IO enable decoding.
  Setup I/O Enables in eSPI PCI offset 82h (eSPI CS0#) or A0h (eSPI CS1#).
  Note: Bit[15:10] of the source decode register is Read-Only. The IO range indicated by the Enables field
  in eSPI PCI offset 82h[13:10] or A0h[13:10] is always forwarded to subtractive agent for handling.
  Please check EDS for detail of Lpc/eSPI IO decode ranges bit definition.

  @param[in] IoEnableDecoding           eSPI IO enable decoding bit settings.
  @param[in] DeviceId                    Device ID (refer to DEVICE_ID_INDEX)
  @param[in] EspiPcrAccess              eSPI Sideband register access protocol

  @retval EFI_SUCCESS                   Successfully completed.
**/
EFI_STATUS
EFIAPI
EspiIoEnableDecodingSetHelper (
  IN  UINT16                            IoEnableDecoding,
  IN  DEVICE_ID_INDEX                   DeviceId
  );

/**
  Set IBL LPC and eSPI CS0# IO enable decoding.
  Setup I/O Enables in eSPI PCI offset 82h.
  Note: Bit[15:10] of the source decode register is Read-Only. The IO range indicated by the Enables field
  in eSPI PCI offset 82h[13:10] is always forwarded to subtractive agent for handling.
  Please check EDS for detail of eSPI IO decode ranges bit definition.

  @param[in] LpcIoEnableDecoding        LPC IO enable decoding bit settings.

  @retval EFI_SUCCESS                   Successfully completed.
**/
EFI_STATUS
EFIAPI
PchLpcIoEnableDecodingSet (
  IN  UINT16                            IoEnableDecoding
  );

/**
  Returns LPC device PCI base address.

  @retval                   LPC PCI base address.
**/
UINT64
LpcPciCfgBase (
  VOID
  );
#endif // _PCH_CYCLE_DECODING_LIB_H_
