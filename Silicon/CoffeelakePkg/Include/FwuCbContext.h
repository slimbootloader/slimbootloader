/** @file

  Copyright (c) 2017 - 2019, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef __FWUCBCONTEXT_H__
#define __FWUCBCONTEXT_H__

#include <RegAccess.h>

// prototypes takens from MdePkg\Include\Library\MemoryAllocationLib.h
typedef VOID *(*FWU_ALLOCATE_POOL_CB)(UINTN AllocationSize);
typedef VOID *(*FWU_ALLOCATE_ZERO_POOL_CB)(UINTN AllocationSize);
typedef VOID (*FWU_FREE_POOL_CB)(VOID *Buffer);

// prototypes takens from MdePkg\Include\Library\BaseMemoryLib.h
typedef VOID *(*FWU_COPY_MEM_CB)(VOID *Destination, CONST VOID *Source, UINTN Length);
typedef VOID *(*FWU_SET_MEM_CB)(VOID *Buffer, UINTN Length, UINT8 Value); // different arguments order than memset()
typedef INTN (*FWU_COMPARE_MEM_CB)(CONST VOID *DestinationBuffer, CONST VOID *SourceBuffer, UINTN Length);

// prototype takens from MdePkg\Include\Uefi\UefiSpec.h
typedef EFI_STATUS(*FWU_STALL_CB)(UINTN Microseconds);

// #define NON_BLOCKING                  0
// #define BLOCKING                      1 FW Update Lib uses BLOCKING

/**
  Read the HECI Message from Intel ME with size in Length into
  buffer MessageBody. Set Blocking to BLOCKING and code will
  wait until one message packet is received. When set to
  NON_BLOCKING, if the circular buffer is empty at the time, the
  code will not wait for the message packet.

  @param[in] Blocking             Used to determine if the read is BLOCKING or NON_BLOCKING.
  @param[in] *MessageBody         Pointer to a buffer used to receive a message.
  @param[in, out] *Length         Pointer to the length of the buffer on input and the length of the message on return. (in bytes)

  @retval EFI_SUCCESS             One message packet read.
  @retval EFI_DEVICE_ERROR        Failed to initialize HECI or zero-length message packet read
  @retval EFI_TIMEOUT             HECI is not ready for communication
  @retval EFI_BUFFER_TOO_SMALL    The caller's buffer was not large enough
**/
typedef
EFI_STATUS
(*FWU_HECI_READ_MESSAGE_CB) (
  IN      HECI_DEVICE             HeciDev,
  IN      UINT32                  Blocking,
  IN      UINT32                  *MessageBody,
  IN OUT  UINT32                  *Length
  );

/**
  Function sends one message (of any length) through the HECI circular buffer.

  @param[in] *Message             Pointer to the message data to be sent.
  @param[in] Length               Length of the message in bytes.
  @param[in] HostAddress          The address of the host processor.
  @param[in] MEAddress            Address of the ME subsystem the message is being sent to.

  @retval EFI_SUCCESS             One message packet sent.
  @retval EFI_DEVICE_ERROR        Failed to initialize HECI
  @retval EFI_TIMEOUT             HECI is not ready for communication
  @exception EFI_UNSUPPORTED      Current ME mode doesn't support send message through HECI
**/
typedef
EFI_STATUS
(*FWU_HECI_SEND_MESSAGE_CB) (
  IN      HECI_DEVICE             HeciDev,
  IN      UINT32                  *Message,
  IN      UINT32                  Length,
  IN      UINT8                   HostAddress,
  IN      UINT8                   MEAddress
  );

/**
  Reset the HECI Controller with algorithm defined in the RS -
  Intel(R) Management Engine - Host Embedded Controller
  Interface Hardware Programming Specification (HPS)

  @retval EFI_TIMEOUT             HECI does not return the buffer before timeout
  @retval EFI_SUCCESS             Interface reset
**/
typedef
EFI_STATUS
(*FWU_HECI_RESET_CB) (
  IN      HECI_DEVICE            HeciDev
  );

typedef
EFI_STATUS
(*FWU_PCI_READ_CB) (
  IN     UINT64         Address,
  IN     UINTN          Count,
  IN OUT VOID           *Buffer
  );


typedef struct
{
  FWU_ALLOCATE_POOL_CB          AllocatePool;
  FWU_ALLOCATE_ZERO_POOL_CB     AllocateZeroPool;
  FWU_FREE_POOL_CB              FreePool;
  FWU_COPY_MEM_CB               CopyMem;
  FWU_SET_MEM_CB                SetMem;
  FWU_COMPARE_MEM_CB            CompareMem;
  FWU_STALL_CB                  Stall;
  FWU_HECI_READ_MESSAGE_CB      HeciReadMessage;
  FWU_HECI_SEND_MESSAGE_CB      HeciSendMessage;
  FWU_HECI_RESET_CB             HeciReset;
  FWU_PCI_READ_CB               PciRead;
} FWU_CB_CONTEXT;

extern FWU_CB_CONTEXT g_FwuCbCtxt;

#endif
