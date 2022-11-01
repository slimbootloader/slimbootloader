/** @file

  Copyright (c) 2017 - 2022, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

/*
 * MmcAccessLibPrivate.h
 */
#ifndef _MMC_ACCESS_LIB_PRIVATE_H_
#define _MMC_ACCESS_LIB_PRIVATE_H_

#define  SD_OCR_33      20
#define  SD_OCR_30      18
#define  SD_OCR_LOW     7

//
// SD card voltage check retry times
//
#define  SD_VOLTAGE_CHECK_MAX_RETRY    5

/**
  Get a pointer to the SD_MMC_HC_PRIVATE_DATA instance.

  @retval Private      A pointer to the SD_MMC_HC_PRIVATE_DATA instance.
**/
SD_MMC_HC_PRIVATE_DATA *
MmcGetHcPrivateData (
  VOID
  );

/**
  Send command SEND_OP_COND to the EMMC device to get the data of the OCR register.

  Refer to EMMC Electrical Standard Spec 5.1 Section 6.4 for details.

  @param[in]  Private       A pointer to the SD_MMC_HC_PRIVATE_DATA instance.
  @param[in, out] Argument  On input, the argument of SEND_OP_COND is to send to the device.
                            On output, the argument is the value of OCR register.

  @retval EFI_SUCCESS       The operation is done correctly.
  @retval Others            The operation fails.

**/
EFI_STATUS
MmcGetOcr (
  IN     SD_MMC_HC_PRIVATE_DATA         *Private,
  IN OUT UINT32                         *Argument
  );

/**
  This function initializes EMMC device

  @param[in]  Private           A pointer to the SD_MMC_HC_PRIVATE_DATA instance.

  @retval EFI_SUCCESS           The request is executed successfully.
  @retval EFI_OUT_OF_RESOURCES  The request could not be executed due to a lack of resources.
  @retval Others                The request could not be executed successfully.

**/
EFI_STATUS
ContinueMmcInitialize (
  IN  SD_MMC_HC_PRIVATE_DATA     *Private
  );


/**
  Send command SEND_IF_COND to the device to inquiry the SD Memory Card interface
  condition.

  @param[in]  Private       A pointer to the SD_MMC_HC_PRIVATE_DATA instance.

  @retval EFI_SUCCESS       The operation is done correctly.
  @retval Others            The operation fails.

**/
EFI_STATUS
SdCardVoltageCheck (
  IN  SD_MMC_HC_PRIVATE_DATA            *Private
  );

/**
  Send command SD_SEND_OP_COND to the device to see whether it is SDIO device.

  @param[in]  Private        A pointer to the SD_MMC_HC_PRIVATE_DATA instance.
  @param[in, out]  Argument       Command argument for input.
                             Command response 0 for output.
  @retval EFI_SUCCESS        The operation is done correctly.
  @retval Others             The operation fails.

**/
EFI_STATUS
SdCardSendOpCond (
  IN  SD_MMC_HC_PRIVATE_DATA            *Private,
  IN OUT UINT32                         *Argument
  );

#endif /* _MMC_ACCESS_LIB_PRIVATE_H_ */
