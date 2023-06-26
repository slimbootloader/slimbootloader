/** @file
  Header for P2SbSidebandAccessLib

  Copyright (c) 2020 - 2023, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _P2SB_SIDEBAND_ACCESS_LIB_H_
#define _P2SB_SIDEBAND_ACCESS_LIB_H_

#include "RegisterAccess.h"
#include <Library/PchSbiAccessLib.h>
#include "P2SbController.h"
#include "PcrDefine.h"
#include <Uefi/UefiBaseType.h>

#define P2SB_REGISTER_ACCESS_INIT  {P2SbSidebandRead8, P2SbSidebandWrite8, P2SbSidebandOr8, P2SbSidebandAnd8, P2SbSidebandAndThenOr8, \
                                    P2SbSidebandRead16, P2SbSidebandWrite16, P2SbSidebandOr16, P2SbSidebandAnd16, P2SbSidebandAndThenOr16,\
                                    P2SbSidebandRead32, P2SbSidebandWrite32, P2SbSidebandOr32, P2SbSidebandAnd32, P2SbSidebandAndThenOr32}

typedef enum {
  P2SbMemory = 0,
  P2SbPciConfig,
  P2SbPrivateConfig
} P2SB_REGISTER_SPACE;

typedef enum {
  P2SbMmioAccess = 0,
  P2SbMsgAccess
} P2SB_SIDEBAND_ACCESS_METHOD;


/**
  REGISTER_ACCESS for P2SB device to support access to sideband registers.
  Be sure to keep first member of this structure as REGISTER_ACCESS to allow
  for correct casting between caller who sees this structure as REGISTER_ACCESS
  and calle who will cast it to P2SB_SIDEBAND_REGISTER_ACCESS.
**/
typedef struct {
  REGISTER_ACCESS              Access;
  P2SB_SIDEBAND_ACCESS_METHOD  AccessMethod;
  P2SB_PID                     P2SbPid;
  UINT16                       Fid;
  P2SB_REGISTER_SPACE          RegisterSpace;
  BOOLEAN                      PostedWrites;
  P2SB_CONTROLLER              *P2SbCtrl;
  UINT32                       Offset;
} P2SB_SIDEBAND_REGISTER_ACCESS;

/**
  Reads an 8-bit register

  @param[in] This    Pointer to REGISTER_ACCESS
  @param[in] Offset  Offset of the register in the register bank

  @return The 8-bit register value specified by Offset
**/
UINT8
P2SbSidebandRead8 (
  IN REGISTER_ACCESS  *This,
  IN UINT32           Offset
  );

/**
  Writes an 8-bit register

  @param[in] This    Pointer to REGISTER_ACCESS
  @param[in] Offset  Offset of the register in the register bank
  @param[in] Value   Value to write to register

  @return The 8-bit register value written to register
**/
UINT8
P2SbSidebandWrite8 (
  IN REGISTER_ACCESS  *This,
  IN UINT32           Offset,
  IN UINT8            Value
  );

/**
  Performs an 8-bit or on the register

  @param[in] This    Pointer to REGISTER_ACCESS
  @param[in] Offset  Offset of the register in the register bank
  @param[in] OrData  Data with which register should be OR-ed

  @return The 8-bit register value written to register
**/
UINT8
P2SbSidebandOr8 (
  IN REGISTER_ACCESS  *This,
  IN UINT32           Offset,
  IN UINT8            OrData
  );

/**
  Performs an 8-bit and on the register

  @param[in] This     Pointer to REGISTER_ACCESS
  @param[in] Offset   Offset of the register in the register bank
  @param[in] AndData  Data with which register should be AND-ed

  @return The 8-bit register value written to register
**/
UINT8
P2SbSidebandAnd8 (
  IN REGISTER_ACCESS  *This,
  IN UINT32           Offset,
  IN UINT8            AndData
  );

/**
  Performs an 8-bit and then or on the register

  @param[in] This     Pointer to REGISTER_ACCESS
  @param[in] Offset   Offset of the register in the register bank
  @param[in] AndData  Data with which register should be AND-ed
  @param[in] OrData   Data with which register should be OR-ed

  @return The 8-bit register value written to register
**/
UINT8
P2SbSidebandAndThenOr8 (
  IN REGISTER_ACCESS  *This,
  IN UINT32           Offset,
  IN UINT8            AndData,
  IN UINT8            OrData
  );

/**
  Reads a 16-bit register

  @param[in] This    Pointer to REGISTER_ACCESS
  @param[in] Offset  Offset of the register in the register bank

  @return The 16-bit register value specified by Offset
**/
UINT16
P2SbSidebandRead16 (
  IN REGISTER_ACCESS  *This,
  IN UINT32           Offset
  );

/**
  Writes a 16-bit register

  @param[in] This    Pointer to REGISTER_ACCESS
  @param[in] Offset  Offset of the register in the register bank
  @param[in] Value   Value to write to register

  @return The 16-bit register value written to register
**/
UINT16
P2SbSidebandWrite16 (
  IN REGISTER_ACCESS  *This,
  IN UINT32           Offset,
  IN UINT16            Value
  );

/**
  Performs a 16-bit or on the register

  @param[in] This    Pointer to REGISTER_ACCESS
  @param[in] Offset  Offset of the register in the register bank
  @param[in] OrData  Data with which register should be OR-ed

  @return The 16-bit register value written to register
**/
UINT16
P2SbSidebandOr16 (
  IN REGISTER_ACCESS  *This,
  IN UINT32           Offset,
  IN UINT16           OrData
  );

/**
  Performs a 16-bit and on the register

  @param[in] This     Pointer to REGISTER_ACCESS
  @param[in] Offset   Offset of the register in the register bank
  @param[in] AndData  Data with which register should be AND-ed

  @return The 16-bit register value written to register
**/
UINT16
P2SbSidebandAnd16 (
  IN REGISTER_ACCESS  *This,
  IN UINT32           Offset,
  IN UINT16           AndData
  );

/**
  Performs a 16-bit and then or on the register

  @param[in] This     Pointer to REGISTER_ACCESS
  @param[in] Offset   Offset of the register in the register bank
  @param[in] AndData  Data with which register should be AND-ed
  @param[in] OrData   Data with which register should be OR-ed

  @return The 16-bit register value written to register
**/
UINT16
P2SbSidebandAndThenOr16 (
  IN REGISTER_ACCESS  *This,
  IN UINT32           Offset,
  IN UINT16           AndData,
  IN UINT16           OrData
  );

/**
  Reads a 32-bit register

  @param[in] This    Pointer to REGISTER_ACCESS
  @param[in] Offset  Offset of the register in the register bank

  @return The 32-bit register value specified by Offset
**/
UINT32
P2SbSidebandRead32 (
  IN REGISTER_ACCESS  *This,
  IN UINT32           Offset
  );

/**
  Writes a 32-bit register

  @param[in] This    Pointer to REGISTER_ACCESS
  @param[in] Offset  Offset of the register in the register bank
  @param[in] Value   Value to write to register

  @return The 32-bit register value written to register
**/
UINT32
P2SbSidebandWrite32 (
  IN REGISTER_ACCESS  *This,
  IN UINT32           Offset,
  IN UINT32            Value
  );

/**
  Performs a 32-bit or on the register

  @param[in] This    Pointer to REGISTER_ACCESS
  @param[in] Offset  Offset of the register in the register bank
  @param[in] OrData  Data with which register should be OR-ed

  @return The 32-bit register value written to register
**/
UINT32
P2SbSidebandOr32 (
  IN REGISTER_ACCESS  *This,
  IN UINT32           Offset,
  IN UINT32           OrData
  );

/**
  Performs a 32-bit and on the register

  @param[in] This     Pointer to REGISTER_ACCESS
  @param[in] Offset   Offset of the register in the register bank
  @param[in] AndData  Data with which register should be AND-ed

  @return The 32-bit register value written to register
**/
UINT32
P2SbSidebandAnd32 (
  IN REGISTER_ACCESS  *This,
  IN UINT32           Offset,
  IN UINT32           AndData
  );

/**
  Performs a 32-bit and then or on the register

  @param[in] This     Pointer to REGISTER_ACCESS
  @param[in] Offset   Offset of the register in the register bank
  @param[in] AndData  Data with which register should be AND-ed
  @param[in] OrData   Data with which register should be OR-ed

  @return The 32-bit register value written to register
**/
UINT32
P2SbSidebandAndThenOr32 (
  IN REGISTER_ACCESS  *This,
  IN UINT32           Offset,
  IN UINT32           AndData,
  IN UINT32           OrData
  );

/**
  Full function for executing P2SB SBI message
  Take care of that there is no lock protection when using SBI programming in both POST time and SMI.
  It will clash with POST time SBI programming when SMI happen.
  Programmer MUST do the save and restore opration while using the PchSbiExecution inside SMI
  to prevent from racing condition.
  This function will reveal P2SB and hide P2SB if it's originally hidden. If more than one SBI access
  needed, it's better to unhide the P2SB before calling and hide it back after done.

  When the return value is "EFI_SUCCESS", the "Response" do not need to be checked as it would have been
  SBI_SUCCESS. If the return value is "EFI_DEVICE_ERROR", then this would provide additional information
  when needed.

  @param[in] P2sbBase                   P2SB PCI config base
  @param[in] Pid                        Port ID of the SBI message
  @param[in] Offset                     Offset of the SBI message
  @param[in] Opcode                     Opcode
  @param[in] Posted                     Posted message
  @param[in] Fbe                        First byte enable
  @param[in] Bar                        Bar
  @param[in] Fid                        Function ID
  @param[in, out] Data32                Read/Write data
  @param[out] Response                  Response

  @retval EFI_SUCCESS                   Successfully completed.
  @retval EFI_DEVICE_ERROR              Transaction fail
  @retval EFI_INVALID_PARAMETER         Invalid parameter
  @retval EFI_TIMEOUT                   Timeout while waiting for response
**/
EFI_STATUS
P2SbSbiExecutionEx (
  IN     UINT64           P2sbBase,
  IN     P2SB_PID         Pid,
  IN     UINT64           Offset,
  IN     PCH_SBI_OPCODE  Opcode,
  IN     BOOLEAN          Posted,
  IN     UINT16           Fbe,
  IN     UINT16           Bar,
  IN     UINT16           Fid,
  IN OUT UINT32           *Data32,
  OUT    UINT8            *Response
  );

/**
  Builds P2SB sideband access.

  @param[in]  P2SbController      Pointer to P2SB controller implementing the sideband
  @param[in]  P2SbPid             Port id
  @param[in]  Fid                 Function id
  @param[in]  RegisterSpace       Target register space
  @param[in]  AccessMethod        Access method
  @param[in]  PostedWrites        If TRUE writes sent through sideband msg will be posted
  @param[out] P2SbSidebandAccess  On output an initialized sideband access descriptor

  @retval TRUE   Access initialized successfuly
  @retval FALSE  Failed to initialize access
**/
BOOLEAN
BuildP2SbSidebandAccess (
  IN P2SB_CONTROLLER                 *P2sbController,
  IN P2SB_PID                        P2SbPid,
  IN UINT16                          Fid,
  IN P2SB_REGISTER_SPACE             RegisterSpace,
  IN P2SB_SIDEBAND_ACCESS_METHOD     AccessMethod,
  IN BOOLEAN                         PostedWrites,
  OUT P2SB_SIDEBAND_REGISTER_ACCESS  *P2SbSidebandAccess
  );

/**
  Builds P2SB sideband access using offset to config space.
  This offset will be added to every register offset, when access callbacks are used.
  Some IPs, like for example DMI, implement multiple register banks in the same register space.
  For instance sideband register space can contain both PCR registers,
  starting from the beginning of the register space,
  and PCI config registers starting from the offset in the same register space.

  @param[in]  P2SbController        Pointer to P2SB controller implementing the sideband
  @param[in]  P2SbPid               Port id
  @param[in]  Fid                   Function id
  @param[in]  RegisterSpace         Target register space
  @param[in]  AccessMethod          Access method
  @param[in]  PostedWrites          If TRUE writes sent through sideband msg will be posted
  @param[in]  Offset                Register space offset that will be added to register offset
  @param[out] P2SbSidebandAccess    An initialized sideband access descriptor containing register space offset

  @retval TRUE   Access initialized successfuly
  @retval FALSE  Failed to initialize access
**/
BOOLEAN
BuildP2SbSidebandAccessEx (
  IN P2SB_CONTROLLER                    *P2SbController,
  IN P2SB_PID                           P2SbPid,
  IN UINT16                             Fid,
  IN P2SB_REGISTER_SPACE                RegisterSpace,
  IN P2SB_SIDEBAND_ACCESS_METHOD        AccessMethod,
  IN BOOLEAN                            PostedWrites,
  IN UINT32                             Offset,
  OUT P2SB_SIDEBAND_REGISTER_ACCESS     *P2SbSidebandAccess
  );

#endif
