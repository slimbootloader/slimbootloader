/** @file
  NvmExpressDxe driver is used to manage non-volatile memory subsystem which follows
  NVM Express specification.

  (C) Copyright 2016 Hewlett Packard Enterprise Development LP<BR>
  Copyright (c) 2013 - 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _EFI_NVM_EXPRESS_H_
#define _EFI_NVM_EXPRESS_H_

#include <PiPei.h>

#include <IndustryStandard/Pci.h>
#include <IndustryStandard/Nvme.h>
#include <Protocol/BlockIo.h>
#include <Protocol/BlockIo2.h>
#include <Protocol/DiskInfo.h>
#include <Protocol/StorageSecurityCommand.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/PrintLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PciNvmCtrlLib.h>
#include <Library/TimerLib.h>
#include <Library/PciExpressLib.h>
#include <Library/IoMmuLib.h>

typedef struct _NVME_CONTROLLER_PRIVATE_DATA NVME_CONTROLLER_PRIVATE_DATA;
typedef struct _NVME_DEVICE_PRIVATE_DATA     NVME_DEVICE_PRIVATE_DATA;
typedef struct _NVME_BLKIO2_SUBTASK          NVME_BLKIO2_SUBTASK;

#include <Protocol/NvmExpressPassthru.h>
#include "NvmExpressBlockIo.h"
#include "NvmExpressDiskInfo.h"
#include "NvmExpressHci.h"


#define EFI_TIMER_PERIOD_MILLISECONDS(Milliseconds) MultU64x32((UINT64)(Milliseconds), 10000)
#define EFI_TIMER_PERIOD_SECONDS(Seconds)           MultU64x32((UINT64)(Seconds), 10000000)

#define PCI_CLASS_MASS_STORAGE_NVM                0x08  // mass storage sub-class non-volatile memory.
#define PCI_IF_NVMHCI                             0x02  // mass storage programming interface NVMHCI.

#define NVME_ASQ_SIZE                             1     // Number of admin submission queue entries, which is 0-based
#define NVME_ACQ_SIZE                             1     // Number of admin completion queue entries, which is 0-based

#define NVME_CSQ_SIZE                             1     // Number of I/O submission queue entries, which is 0-based
#define NVME_CCQ_SIZE                             1     // Number of I/O completion queue entries, which is 0-based

//
// Number of asynchronous I/O submission queue entries, which is 0-based.
// The asynchronous I/O submission queue size is 4kB in total.
//
#define NVME_ASYNC_CSQ_SIZE                       63
//
// Number of asynchronous I/O completion queue entries, which is 0-based.
// The asynchronous I/O completion queue size is 4kB in total.
//
#define NVME_ASYNC_CCQ_SIZE                       255

#define NVME_MAX_QUEUES                           3     // Number of queues supported by the driver

#define NVME_CONTROLLER_ID                        0

//
// Time out value for Nvme transaction execution
//
#define NVME_GENERIC_TIMEOUT                      EFI_TIMER_PERIOD_SECONDS (5)

//
// Nvme async transfer timer interval, set by experience.
//
#define NVME_HC_ASYNC_TIMER                       EFI_TIMER_PERIOD_MILLISECONDS (1)

//
// Unique signature for private data structure.
//
#define NVME_CONTROLLER_PRIVATE_DATA_SIGNATURE    SIGNATURE_32 ('N','V','M','E')

//
// Nvme private data structure.
//
struct _NVME_CONTROLLER_PRIVATE_DATA {
  UINT32                              Signature;
  UINT32                              NvmeHCBase;
  UINT64                              PciAttributes;
  UINTN                               PciBaseAddr;
  EFI_NVM_EXPRESS_PASS_THRU_MODE      PassThruMode;
  EFI_NVM_EXPRESS_PASS_THRU_PROTOCOL  Passthru;

  //
  // pointer to identify controller data
  //
  NVME_ADMIN_CONTROLLER_DATA          *ControllerData;

  //
  // 6 x 4kB aligned buffers will be carved out of this buffer.
  // 1st 4kB boundary is the start of the admin submission queue.
  // 2nd 4kB boundary is the start of the admin completion queue.
  // 3rd 4kB boundary is the start of I/O submission queue #1.
  // 4th 4kB boundary is the start of I/O completion queue #1.
  // 5th 4kB boundary is the start of I/O submission queue #2.
  // 6th 4kB boundary is the start of I/O completion queue #2.
  //
  UINT8                               *Buffer;
  UINT8                               *BufferPciAddr;

  //
  // Pointers to 4kB aligned submission & completion queues.
  //
  NVME_SQ                             *SqBuffer[NVME_MAX_QUEUES];
  NVME_CQ                             *CqBuffer[NVME_MAX_QUEUES];

  //
  // Submission and completion queue indices.
  //
  NVME_SQTDBL                         SqTdbl[NVME_MAX_QUEUES];
  NVME_CQHDBL                         CqHdbl[NVME_MAX_QUEUES];
  UINT16                              AsyncSqHead;

  UINT8                               Pt[NVME_MAX_QUEUES];
  UINT16                              Cid[NVME_MAX_QUEUES];

  //
  // Nvme controller capabilities
  //
  NVME_CAP                            Cap;

  VOID                                *Mapping;

  //
  // For Non-blocking operations.
  //
  EFI_EVENT                           TimerEvent;
  LIST_ENTRY                          AsyncPassThruQueue;
  LIST_ENTRY                          UnsubmittedSubtasks;
};

#define NVME_CONTROLLER_PRIVATE_DATA_FROM_PASS_THRU(a) \
  CR (a, \
      NVME_CONTROLLER_PRIVATE_DATA, \
      Passthru, \
      NVME_CONTROLLER_PRIVATE_DATA_SIGNATURE \
      )

//
// Unique signature for private data structure.
//
#define NVME_DEVICE_PRIVATE_DATA_SIGNATURE     SIGNATURE_32 ('X','S','S','D')

//
// Nvme device private data structure
//
struct _NVME_DEVICE_PRIVATE_DATA {
  UINT32                                   Signature;
  UINT32                                   NamespaceId;
  UINT64                                   NamespaceUuid;

  EFI_BLOCK_IO_MEDIA                       Media;
  EFI_BLOCK_IO_PROTOCOL                    BlockIo;
  EFI_DISK_INFO_PROTOCOL                   DiskInfo;
  EFI_STORAGE_SECURITY_COMMAND_PROTOCOL    StorageSecurity;

  LIST_ENTRY                               AsyncQueue;

  EFI_LBA                                  NumBlocks;

  CHAR16                                   ModelName[80];
  NVME_ADMIN_NAMESPACE_DATA                NamespaceData;

  NVME_CONTROLLER_PRIVATE_DATA             *Controller;

};

//
// Statments to retrieve the private data from produced protocols.
//
#define NVME_DEVICE_PRIVATE_DATA_FROM_BLOCK_IO(a) \
  CR (a, \
      NVME_DEVICE_PRIVATE_DATA, \
      BlockIo, \
      NVME_DEVICE_PRIVATE_DATA_SIGNATURE \
      )

#define NVME_DEVICE_PRIVATE_DATA_FROM_BLOCK_IO2(a) \
  CR (a, \
      NVME_DEVICE_PRIVATE_DATA, \
      BlockIo2, \
      NVME_DEVICE_PRIVATE_DATA_SIGNATURE \
      )

#define NVME_DEVICE_PRIVATE_DATA_FROM_DISK_INFO(a) \
  CR (a, \
      NVME_DEVICE_PRIVATE_DATA, \
      DiskInfo, \
      NVME_DEVICE_PRIVATE_DATA_SIGNATURE \
      )

#define NVME_DEVICE_PRIVATE_DATA_FROM_STORAGE_SECURITY(a)\
  CR (a,                                                 \
      NVME_DEVICE_PRIVATE_DATA,                          \
      StorageSecurity,                                   \
      NVME_DEVICE_PRIVATE_DATA_SIGNATURE                 \
      )

//
// Nvme block I/O 2 request.
//
#define NVME_BLKIO2_REQUEST_SIGNATURE      SIGNATURE_32 ('N', 'B', '2', 'R')

typedef struct {
  UINT32                                   Signature;
  LIST_ENTRY                               Link;

  EFI_BLOCK_IO2_TOKEN                      *Token;
  UINTN                                    UnsubmittedSubtaskNum;
  BOOLEAN                                  LastSubtaskSubmitted;
  //
  // The queue for Nvme read/write sub-tasks of a BlockIo2 request.
  //
  LIST_ENTRY                               SubtasksQueue;
} NVME_BLKIO2_REQUEST;

#define NVME_BLKIO2_REQUEST_FROM_LINK(a) \
  CR (a, NVME_BLKIO2_REQUEST, Link, NVME_BLKIO2_REQUEST_SIGNATURE)

#define NVME_BLKIO2_SUBTASK_SIGNATURE      SIGNATURE_32 ('N', 'B', '2', 'S')


struct _NVME_BLKIO2_SUBTASK {
  UINT32                                   Signature;
  LIST_ENTRY                               Link;

  BOOLEAN                                  IsLast;
  UINT32                                   NamespaceId;
  EFI_EVENT                                Event;
  EFI_NVM_EXPRESS_PASS_THRU_COMMAND_PACKET *CommandPacket;
  //
  // The BlockIo2 request this subtask belongs to
  //
  NVME_BLKIO2_REQUEST                      *BlockIo2Request;
};

#define NVME_BLKIO2_SUBTASK_FROM_LINK(a) \
  CR (a, NVME_BLKIO2_SUBTASK, Link, NVME_BLKIO2_SUBTASK_SIGNATURE)

#define NVME_BLKIO2_SUBTASK_FROM_EVENT(a) \
  CR(a, NVME_BLKIO2_SUBTASK, Event, NVME_BLKIO2_SUBTASK_SIGNATURE)

//
// Nvme asynchronous passthru request.
//
#define NVME_PASS_THRU_ASYNC_REQ_SIG       SIGNATURE_32 ('N', 'P', 'A', 'R')

typedef struct {
  UINT32                                   Signature;
  LIST_ENTRY                               Link;

  EFI_NVM_EXPRESS_PASS_THRU_COMMAND_PACKET *Packet;
  UINT16                                   CommandId;
  VOID                                     *MapPrpList;
  UINTN                                    PrpListNo;
  VOID                                     *PrpListHost;
  VOID                                     *MapData;
  VOID                                     *MapMeta;
  EFI_EVENT                                CallerEvent;
} NVME_PASS_THRU_ASYNC_REQ;

#define NVME_PASS_THRU_ASYNC_REQ_FROM_THIS(a) \
  CR (a,                                                 \
      NVME_PASS_THRU_ASYNC_REQ,                          \
      Link,                                              \
      NVME_PASS_THRU_ASYNC_REQ_SIG                       \
      )

/**
  Sends an NVM Express Command Packet to an NVM Express controller or namespace. This function supports
  both blocking I/O and nonblocking I/O. The blocking I/O functionality is required, and the nonblocking
  I/O functionality is optional.

  @param[in]     This                A pointer to the EFI_NVM_EXPRESS_PASS_THRU_PROTOCOL instance.
  @param[in]     NamespaceId         Is a 32 bit Namespace ID to which the Express HCI command packet will be sent.
                                     A value of 0 denotes the NVM Express controller, a value of all 0FFh in the namespace
                                     ID specifies that the command packet should be sent to all valid namespaces.
  @param[in,out] Packet              A pointer to the NVM Express HCI Command Packet to send to the NVMe namespace specified
                                     by NamespaceId.
  @param[in]     Event               If non-blocking I/O is not supported then Event is ignored, and blocking I/O is performed.
                                     If Event is NULL, then blocking I/O is performed. If Event is not NULL and non-blocking I/O
                                     is supported, then non-blocking I/O is performed, and Event will be signaled when the NVM
                                     Express Command Packet completes.


  @retval EFI_SUCCESS                The NVM Express Command Packet was sent by the host. TransferLength bytes were transferred
                                     to, or from DataBuffer.
  @retval EFI_BAD_BUFFER_SIZE        The NVM Express Command Packet was not executed. The number of bytes that could be transferred
                                     is returned in TransferLength.
  @retval EFI_NOT_READY              The NVM Express Command Packet could not be sent because the controller is not ready. The caller
                                     may retry again later.
  @retval EFI_DEVICE_ERROR           A device error occurred while attempting to send the NVM Express Command Packet.
  @retval EFI_INVALID_PARAMETER      Namespace, or the contents of EFI_NVM_EXPRESS_PASS_THRU_COMMAND_PACKET are invalid. The NVM
                                     Express Command Packet was not sent, so no additional status information is available.
  @retval EFI_UNSUPPORTED            The command described by the NVM Express Command Packet is not supported by the host adapter.
                                     The NVM Express Command Packet was not sent, so no additional status information is available.
  @retval EFI_TIMEOUT                A timeout occurred while waiting for the NVM Express Command Packet to execute.

**/
EFI_STATUS
EFIAPI
NvmExpressPassThru (
  IN     EFI_NVM_EXPRESS_PASS_THRU_PROTOCOL          *This,
  IN     UINT32                                      NamespaceId,
  IN OUT EFI_NVM_EXPRESS_PASS_THRU_COMMAND_PACKET    *Packet,
  IN     EFI_EVENT                                   Event OPTIONAL
  );

/**
  Used to retrieve the next namespace ID for this NVM Express controller.

  The EFI_NVM_EXPRESS_PASS_THRU_PROTOCOL.GetNextNamespace() function retrieves the next valid
  namespace ID on this NVM Express controller.

  If on input the value pointed to by NamespaceId is 0xFFFFFFFF, then the first valid namespace
  ID defined on the NVM Express controller is returned in the location pointed to by NamespaceId
  and a status of EFI_SUCCESS is returned.

  If on input the value pointed to by NamespaceId is an invalid namespace ID other than 0xFFFFFFFF,
  then EFI_INVALID_PARAMETER is returned.

  If on input the value pointed to by NamespaceId is a valid namespace ID, then the next valid
  namespace ID on the NVM Express controller is returned in the location pointed to by NamespaceId,
  and EFI_SUCCESS is returned.

  If the value pointed to by NamespaceId is the namespace ID of the last namespace on the NVM
  Express controller, then EFI_NOT_FOUND is returned.

  @param[in]     This           A pointer to the EFI_NVM_EXPRESS_PASS_THRU_PROTOCOL instance.
  @param[in,out] NamespaceId    On input, a pointer to a legal NamespaceId for an NVM Express
                                namespace present on the NVM Express controller. On output, a
                                pointer to the next NamespaceId of an NVM Express namespace on
                                an NVM Express controller. An input value of 0xFFFFFFFF retrieves
                                the first NamespaceId for an NVM Express namespace present on an
                                NVM Express controller.

  @retval EFI_SUCCESS           The Namespace ID of the next Namespace was returned.
  @retval EFI_NOT_FOUND         There are no more namespaces defined on this controller.
  @retval EFI_INVALID_PARAMETER NamespaceId is an invalid value other than 0xFFFFFFFF.

**/
EFI_STATUS
EFIAPI
NvmExpressGetNextNamespace (
  IN     EFI_NVM_EXPRESS_PASS_THRU_PROTOCOL          *This,
  IN OUT UINT32                                      *NamespaceId
  );

/**
  Dump the execution status from a given completion queue entry.

  @param[in]     Cq               A pointer to the NVME_CQ item.

**/
VOID
NvmeDumpStatus (
  IN NVME_CQ             *Cq
  );


#endif
