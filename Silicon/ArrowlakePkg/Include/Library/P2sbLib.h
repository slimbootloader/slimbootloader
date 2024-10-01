/** @file
  Header for P2sbLib

  Copyright (c) 2024, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _P2SB_LIB_H_
#define _P2SB_LIB_H_

#include <Uefi/UefiBaseType.h>
#include <Library/PchSbiAccessLib.h>

typedef UINT8          P2SB_PID;

/**
  Execute P2SB SBI message
  Take care of that there is no lock protection when using SBI programming in both POST time and SMI.
  It will clash with POST time SBI programming when SMI happen.
  Programmer MUST do the save and restore opration while using the PchSbiExecution inside SMI
  to prevent from racing condition.
  This function will reveal P2SB and hide P2SB if it's originally hidden. If more than one SBI access
  needed, it's better to unhide the P2SB before calling and hide it back after done.

  When the return value is "EFI_SUCCESS", the "Response" do not need to be checked as it would have been
  SBI_SUCCESS. If the return value is "EFI_DEVICE_ERROR", then this would provide additional information
  when needed.

  @param[in] P2sbBase                   P2SB PCI address (PCI_LIB_ADDRESS(bus, device, fun, 0)
  @param[in] Pid                        Port ID of the SBI message
  @param[in] Fid                        Function ID
  @param[in] Offset                     Offset of the SBI message
  @param[in] Opcode                     Opcode
  @param[in] Posted                     Posted message
  @param[in, out] Data32                Read/Write data
  @param[out] Response                  Response

  @retval EFI_SUCCESS                   Successfully completed.
  @retval EFI_DEVICE_ERROR              Transaction fail
  @retval EFI_INVALID_PARAMETER         Invalid parameter
  @retval EFI_TIMEOUT                   Timeout while waiting for response
**/
EFI_STATUS
EFIAPI
P2SbSbiExecution (
  IN     UINT64          P2sbBase,
  IN     P2SB_PID        Pid,
  IN     UINT16          Fid,
  IN     UINT64          Offset,
  IN     PCH_SBI_OPCODE  Opcode,
  IN     BOOLEAN         Posted,
  IN OUT UINT32          *Data32,
  OUT    UINT8           *Response
  );

/**
  Read a register from P2SB

  @param[in] P2sbBase    P2SB PCI address (PCI_LIB_ADDRESS(bus, device, fun, 0) or SBREG_BAR address
  @param[in] Pid         Port ID of the SBI message
  @param[in] Fid         Function ID
  @param[in] Offset      Offset of the SBI message

  @retval EFI_SUCCESS    Successfully completed.
**/
UINT32
EFIAPI
P2sbRead32 (
  UINT64    P2sbBase,
  P2SB_PID  Pid,
  UINT16    Fid,
  UINT64    Offset
  );

/**
  Write a register from P2SB

  @param[in] P2sbBase    P2SB PCI address (PCI_LIB_ADDRESS(bus, device, fun, 0) or SBREG_BAR address
  @param[in] Pid         Port ID of the SBI message
  @param[in] Fid         Function ID
  @param[in] Offset      Offset of the SBI message
  @param[in] Data32      The data to write to register

  @retval EFI_SUCCESS    Successfully completed.
**/
UINT32
EFIAPI
P2sbWrite32 (
  UINT64    P2sbBase,
  P2SB_PID  Pid,
  UINT16    Fid,
  UINT64    Offset,
  UINT32    Data32
  );


/**
  Performs a 32-bit and then or on the register

  @param[in] P2sbBase    P2SB PCI address (PCI_LIB_ADDRESS(bus, device, fun, 0) or SBREG_BAR address
  @param[in] Pid         Port ID of the SBI message
  @param[in] Offset      Offset of the register in the register bank
  @param[in] AndData     Data with which register should be AND-ed
  @param[in] OrData      Data with which register should be OR-ed

  @return The 32-bit register value written to register
**/
UINT32
EFIAPI
P2SbAndThenOr32 (
  IN UINT64           P2sbBase,
  IN P2SB_PID         Pid,
  IN UINT16           Fid,
  IN UINT32           Offset,
  IN UINT32           AndData,
  IN UINT32           OrData
  );

#endif
