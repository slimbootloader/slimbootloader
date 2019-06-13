/** @file
  Header file for the Smbus Library.

  Copyright (c) 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _SMBUS_LIB_H_
#define _SMBUS_LIB_H_

#include <IndustryStandard/SmBus.h>

/**
  This function provides a standard way to execute Smbus protocols
  as defined in the SMBus Specification. The data can either be of
  the Length byte, word, or a block of data. The resulting transaction will be
  either the SMBus Slave Device accepts this transaction or this function
  returns with an error

  @param[in] SlaveAddress         Smbus Slave device the command is directed at
  @param[in] Command              Slave Device dependent
  @param[in] Operation            Which SMBus protocol will be used
  @param[in] PecCheck             Defines if Packet Error Code Checking is to be used
  @param[in, out] Length          How many bytes to read. Must be 0 <= Length <= 32 depending on Operation
                                  It will contain the actual number of bytes read/written.
  @param[in, out] Buffer          Contain the data read/written.

  @retval EFI_SUCCESS             The operation completed successfully.
  @exception EFI_UNSUPPORTED      The operation is unsupported.

  @retval EFI_INVALID_PARAMETER   Length or Buffer is NULL for any operation besides
                                  quick read or quick write.
  @retval EFI_TIMEOUT             The transaction did not complete within an internally
                                  specified timeout period, or the controller is not
                                  available for use.
  @retval EFI_DEVICE_ERROR        There was an Smbus error (NACK) during the operation.
                                  This could indicate the slave device is not present
                                  or is in a hung condition.
**/
EFI_STATUS
SmbusExec (
  IN      EFI_SMBUS_DEVICE_ADDRESS  SlaveAddress,
  IN      EFI_SMBUS_DEVICE_COMMAND  Command,
  IN      EFI_SMBUS_OPERATION       Operation,
  IN      BOOLEAN                   PecCheck,
  IN OUT  UINTN                     *Length,
  IN OUT  VOID                      *Buffer
  );

#endif
