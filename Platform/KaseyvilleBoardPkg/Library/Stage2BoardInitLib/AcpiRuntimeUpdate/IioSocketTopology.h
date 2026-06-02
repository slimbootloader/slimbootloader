/** @file
  Prototypes for IIO subsystem information API functions

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "IioUniversalDataHob.h"

#ifndef _IIO_SOCKET_TOPOLOGY_LIB_H_
#define _IIO_SOCKET_TOPOLOGY_LIB_H_

#define LOOK_FOR_STACK_BASED_ON_PE  TRUE
#define LOOK_FOR_PE_BASED_ON_STACK  FALSE

#define LOOK_FOR_STACK_BASED_ON_IOAT  TRUE
#define LOOK_FOR_IOAT_BASED_ON_STACK  FALSE

typedef enum {
  TYPE_UBOX = 0,              // Stack/Tile with only Ubox device
  TYPE_UBOX_IIO,              // DMI/PCIe Stack (Stack0) that hosts both IIO and Ubox devices
  TYPE_IIO,                   // PCIe/CXL/DMI stack
  TYPE_MCP,
  TYPE_HFI,
  TYPE_NAC,
  TYPE_GRAPHICS,
  TYPE_DINO,
  TYPE_TSNO_DICH,
  TYPE_DICH,
  TYPE_TIP,
  TYPE_RESERVED,
  TYPE_DISABLED,              // This item must be prior to stack specific disable types
  TYPE_UBOX_IIO_DIS,
  TYPE_IIO_DIS,
  TYPE_MCP_DIS,
  TYPE_HFI_DIS,
  TYPE_NAC_DIS,
  TYPE_GRAPHICS_DIS,
  TYPE_DINO_DIS,
  TYPE_RESERVED_DIS,
  TYPE_TSNO_DICH_DIS,
  TYPE_DICH_DIS,
  TYPE_TIP_DIS,
  TYPE_NONE
} STACK_TYPE;

typedef enum {
  IioSocket0 = 0,
  IioSocket1 = 1,
  IioSocket2 = 2,
  IioSocket3 = 3,
  IioSocket4 = 4,
  IioSocket5 = 5,
  IioSocket6 = 6,
  IioSocket7 = 7
} IIO_SOCKET;
typedef enum {
  IioPortA = 0,
  IioPortB = 1,
  IioPortC = 2,
  IioPortD = 3,
  IioPortE = 4,
  IioPortF = 5,
  IioPortG = 6,
  IioPortH = 7
} IIO_PORT;

typedef enum {
  PE0 = 0,
  PE1,
  PE2,
  PE3,
  PE4,
  PE5,
  PEa,
  PEb,
  PEc,
  PEd,
  PE_MAX,
  PE_ = 0xFF   // temporary unknown value
} IIO_PACKAGE_PE;

typedef enum {
  IIO_GEN3 = 3,
  IIO_GEN4 = 4,
  IIO_GEN5 = 5,
  IIO_GEN6 = 6,
  IIO_GEN_MAX
} IIO_PORT_GEN;

//
//Public stack defines
//
typedef enum {
  IIO_STACK_0 = 0,
  IIO_STACK_1,
  IIO_STACK_2,
  IIO_STACK_3,
  IIO_STACK_4,
  IIO_STACK_5,
  IIO_STACK_6,
  IIO_STACK_7,
  IIO_STACK_8,
  IIO_STACK_9,
  IIO_STACK_10,
  IIO_STACK_11,
  IIO_STACK_12,
  IIO_STACK_13,
  IIO_STACK_14,
  IIO_STACK_15,
  IIO_STACK_16,
  IIO_STACK_17,
  IIO_STACK_ID_MAX,
  IIO_STACK_INVALID = 0xFF
} IIO_STACK_ID;

typedef struct _STACK_TO_PE {
  UINT8   StackId;
  UINT8   PackagePE;
} STACK_TO_PE;

/**
  Return stack index assigned to given PE on board.
  This can be used mostly to load board config.

  @param[in] Socket   Socket index
  @param[in] PeIndex  PCI Express index in socket pinout

  @return IIO_STACK  Right stack assigned to given PCI Express
 */
UINT8
IioPe2StackIndex (
  IN  UINT8   Socket,
  IN  UINT8   PeIndex
  );

/**
  Return stack index assigned to accelerator.
  This can be used mostly to load board config.

  @param[in] Socket             Socket index
  @param[in] IoatIndexSearched  Accelerator index in socket pinout

  @return IIO_STACK  Right stack assigned to given accelerator
 */
UINT8
IioIoat2StackIndex (
  IN  UINT8   Socket,
  IN  UINT8   IoatIndexSearched
  );

/**
  Return Package PE value assigned to given stack on board.
  This can be used mostly to load board config.

  @param[in] Socket  Socket index
  @param[in] Stack   The stack index inside socket

  @return IIO_PACKAGE_PE  Right package PE assigned to given stack
 */
UINT8
IioStack2PeIndex (
  IN  UINT8   Socket,
  IN  UINT8   Stack
  );

/**
  Return accelerator index assigned to stack.

  @param[in] Socket  Socket index
  @param[in] Stack   The stack index inside socket

  @return            Accelarator index assigned to given stack
 */
UINT8
IioStack2IoatIndex (
  IN  UINT8   Socket,
  IN  UINT8   Stack
  );

#endif // _IIO_SOCKET_TOPOLOGY_LIB_H_
