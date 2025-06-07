/** @file
  Heci ME Extended Measured boot library.

  Copyright (c) 2024, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#ifndef _HECI_ME_LIB_H_
#define _HECI_ME_LIB_H_
#include <Uefi/UefiBaseType.h>
#include <Library/BaseLib.h>
#include <Library/PciLib.h>
#include <MkhiMsgs.h>

#define HeciPciRead16(func, reg)     PciRead16 (PCI_LIB_ADDRESS(ME_BUS, ME_DEVICE_NUMBER, func, reg))
#define HeciPciOr16(func, reg, val)  PciOr16 (PCI_LIB_ADDRESS(ME_BUS, ME_DEVICE_NUMBER, func, reg), val)
#define HeciPciAnd8(func, reg, val)  PciAnd8 (PCI_LIB_ADDRESS(ME_BUS, ME_DEVICE_NUMBER, func, reg), val)

/**
  Reads a 32-bit value from the HECI PCI configuration space.

  @param[in]  Function    The function number of the HECI device.
  @param[in]  Regoffset   The offset of the register to read.

  @return The 32-bit value read from the HECI PCI configuration space.
**/
UINT32
EFIAPI
HeciPciRead32 (
  IN UINT8   Function,
  IN UINT32  Regoffset
 );

/**
  Writes an 8-bit value to the HECI PCI configuration space by performing a bitwise OR operation.

  @param[in]  Function    The function number of the HECI device.
  @param[in]  Regoffset   The offset of the register to write.
  @param[in]  OrValue     The 8-bit value read from the HECI PCI configuration space after the logical OR operation.

  @return The 8-bit value read from the HECI PCI configuration space.
**/
UINT8
EFIAPI
HeciPciOr8 (
  IN UINT8   Function,
  IN UINT32  Regoffset,
  IN UINT8   OrValue
);

/**
  This function sends a command to CSME to get Log information of Measurements provided in
  CSME Extend Registers for BIOS to extend to TPM-PCR registers.

  @param[out]     EventLogBuffer    Pointer to received Event log buffer
  @param[in]      RemainingBufSize  Size of available buffer to collect event log
  @param[out]     ReceivedLogSize   Size of received event log
  @param[in, out] LogEntryIndex     On input - Index of first / starting log entry in the buffer. On output - Index of last log entry returned
  @param[out]     RemainingEntries  Pointer to Remaining Entries (0 - no more entries in a log; 1 - additional iteration(s) is(are) required)

  @retval EFI_SUCCESS             Log received successfully
  @retval EFI_UNSUPPORTED         Current ME mode doesn't support this function
  @retval EFI_BUFFER_TOO_SMALL    Message Buffer is too small for the Acknowledge
  @retval EFI_DEVICE_ERROR        HECI Device error, command aborts abnormally
  @retval EFI_TIMEOUT             HECI does not return the buffer before timeout or transition failed
  within allowed time limit
**/
EFI_STATUS
EFIAPI
HeciGetErLog (
  OUT UINT8        *EventLogBuffer,
  IN  UINT32       RemainingBufSize,
  IN OUT UINT32    *ReceivedLogSize,
  IN OUT UINT8     *LogEntryIndex,
  OUT UINT8        *RemainingEntries
  );

/**
  Send Get Current CSME Measured Boot State

  @param[out] GetMeasuredBootState    Pointer to get CSME Measured boot status.

  @retval EFI_UNSUPPORTED         Current CSME mode/version doesn't support this function
  @retval EFI_SUCCESS             Command succeeded
  @retval EFI_DEVICE_ERROR        HECI Device error, command aborts abnormally
  @retval EFI_TIMEOUT             HECI does not return the buffer before timeout
  @retval EFI_BUFFER_TOO_SMALL    Message Buffer is too small for the Acknowledge
**/
EFI_STATUS
EFIAPI
HeciGetMeasuredBootState (
  OUT UINT8  *GetMeasuredBootState
  );

/**
  Send Set Measured Boot State to Enable or Disable CSME Measurement

  @param[in]  SetMeasuredBootState  Set Measured boot state to Enable/Disable CSME Extended Measured boot

  @retval EFI_UNSUPPORTED         Current CSME mode doesn't support this function
  @retval EFI_SUCCESS             Command succeeded
  @retval EFI_DEVICE_ERROR        HECI Device error, command aborts abnormally
  @retval EFI_TIMEOUT             HECI does not return the buffer before timeout
  @retval EFI_BUFFER_TOO_SMALL    Message Buffer is too small for the Acknowledge
**/
EFI_STATUS
EFIAPI
HeciSetMeasuredBootState (
  IN UINT8 SetMeasuredBootState
  );

/**
  This function gets sha256 or sha384 CSME Measured data from HECI1 MMIO space (HECI1_HER1 - HECI1_HER12).

  @param[in]      Her             - Pointer to receive HER Data buffer of 12 DWORDS
  @param[in]      HerOffset       - Offset of first HER register in MMIO Space
  @param[in]      HerMaxRegCount  - Max Register count to read HER registers

  @retval EFI_SUCCESS             Event data read successfully
  @retval EFI_INVALID_PARAMETER   Input parameters are not valid
  @retval EFI_DEVICE_ERROR        HECI Device error, command aborts abnormally
  @retval EFI_NOT_READY           Device is not ready
**/
EFI_STATUS
EFIAPI
HeciGetHerHashData (
  IN UINT32    *Her,
  IN UINT32    HerOffset,
  IN UINT8     HerMaxRegCount
  );

/**
  Send Set FIPS Mode to Enabled or Disabled

  @param[in] FipsMode             Enable or disable FIPS Mode

  @retval EFI_UNSUPPORTED         Current ME mode doesn't support this function
  @retval EFI_SUCCESS             Command succeeded
  @retval EFI_DEVICE_ERROR        HECI Device error, command aborts abnormally
  @retval EFI_TIMEOUT             HECI does not return the buffer before timeout
  @retval EFI_BUFFER_TOO_SMALL    Message Buffer is too small for the Acknowledge
**/
EFI_STATUS
EFIAPI
HeciSetFipsMode (
  IN UINT32 FipsMode
  );

/**
  Send Get Current FIPS Mode and Crypto Driver version

  @param[out] GetFipsModeData     Pointer to FIPS Mode data

  @retval EFI_UNSUPPORTED         Current ME mode doesn't support this function
  @retval EFI_SUCCESS             Command succeeded
  @retval EFI_DEVICE_ERROR        HECI Device error, command aborts abnormally
  @retval EFI_TIMEOUT             HECI does not return the buffer before timeout
  @retval EFI_BUFFER_TOO_SMALL    Message Buffer is too small for the Acknowledge
**/
EFI_STATUS
EFIAPI
HeciGetFipsMode (
  OUT GET_FIPS_MODE_DATA  *GetFipsModeData
  );

#endif
