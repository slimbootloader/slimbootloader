/** @file
  This library abstract how to access TPM2 hardware device.

  Copyright (c) 2013, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _TPM2_DEVICE_LIB_H_
#define _TPM2_DEVICE_LIB_H_

#include <Uefi/UefiBaseType.h>
#include <IndustryStandard/Tpm2Acpi.h>

/**
  This service enables the sending of commands to the TPM2.

  @param[in]      InputParameterBlockSize  Size of the TPM2 input parameter block.
  @param[in]      InputParameterBlock      Pointer to the TPM2 input parameter block.
  @param[in,out]  OutputParameterBlockSize Size of the TPM2 output parameter block.
  @param[in]      OutputParameterBlock     Pointer to the TPM2 output parameter block.

  @retval EFI_SUCCESS            The command byte stream was successfully sent to the device and a response was successfully received.
  @retval EFI_DEVICE_ERROR       The command was not successfully sent to the device or a response was not successfully received from the device.
  @retval EFI_BUFFER_TOO_SMALL   The output parameter block is too small.
**/
EFI_STATUS
EFIAPI
Tpm2SubmitCommand (
  IN UINT32            InputParameterBlockSize,
  IN UINT8             *InputParameterBlock,
  IN OUT UINT32        *OutputParameterBlockSize,
  IN UINT8             *OutputParameterBlock
  );

/**
  This service requests use TPM2.

  @retval EFI_SUCCESS      Get the control of TPM2 chip.
  @retval EFI_NOT_FOUND    TPM2 not found.
  @retval EFI_DEVICE_ERROR Unexpected device behavior.
**/
EFI_STATUS
EFIAPI
Tpm2RequestUseTpm (
  VOID
  );

/**
  This service checks for TPM device.

  @retval EFI_SUCCESS      Supported TPM2 is present.
  @retval EFI_NOT_FOUND    TPM2 not found.
**/
EFI_STATUS
EFIAPI
IsSupportedTpmPresent (
  VOID
  );

/**
  Update TPM ACPI table with interface information.

  @param[in]      Tpm2Acpi           Pointer to Tpm2 ACPI table.

  @retval RETURN_SUCCESS             Tpm2 table updated successfully.
  @retval EFI_NOT_FOUND              The request could not be executed successfully.

**/
EFI_STATUS
EFIAPI
UpdateAcpiInterfaceInfo (
  IN EFI_TPM2_ACPI_TABLE *Tpm2Acpi
  );

#endif
