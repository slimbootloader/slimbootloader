/** @file
  NvmExpressDxe driver is used to manage non-volatile memory subsystem which follows
  NVM Express specification.

  Copyright (c) 2013 - 2016, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Uefi/UefiBaseType.h>
#include <Library/BaseLib.h>
#include <Library/IoLib.h>
#include "NvmExpress.h"


/**
  Read/Write specified NVM host controller mmio register.

  @param[in]      Address      Host controller mmio base address.
  @param[in]      Offset       The offset within the selected BAR to start the
                               memory operation.
  @param[in]      Read         A boolean to indicate it's read or write operation.
  @param[in]      Count        The width of the mmio register in bytes.
                               Must be 1, 2 , 4 or 8 bytes.
  @param[in, out] Data         For read operations, the destination buffer to store
                               the results. For write operations, the source buffer
                               to write data from. The caller is responsible for
                               having ownership of the data buffer and ensuring its
                               size not less than Count bytes.

  @retval EFI_INVALID_PARAMETER The Data is NULL or the Count is not valid.
  @retval EFI_SUCCESS           The read/write operation succeeds.
  @retval Others                The read/write operation fails.

**/
EFI_STATUS
EFIAPI
NvmHcRwMmio (
  IN     UINT32                Address,
  IN     UINT32                Offset,
  IN     BOOLEAN               Read,
  IN     UINT8                 Count,
  IN OUT VOID                  *Data
  )
{
  if ((Address == 0x0) || (Data == NULL))  {
    return EFI_INVALID_PARAMETER;
  }

  if ((Count != 1) && (Count != 2) && (Count != 4) && (Count != 8)) {
    return EFI_INVALID_PARAMETER;
  }

  switch (Count) {
  case 1:
    if (Read) {
      * (UINT8 *)Data = MmioRead8 (Address + Offset);
    } else {
      MmioWrite8 (Address + Offset, * (UINT8 *)Data);
    }
    break;
  case 2:
    if (Read) {
      * (UINT16 *)Data = MmioRead16 (Address + Offset);
    } else {
      MmioWrite16 (Address + Offset, * (UINT16 *)Data);
    }
    break;
  case 4:
    if (Read) {
      * (UINT32 *)Data = MmioRead32 (Address + Offset);
    } else {
      MmioWrite32 (Address + Offset, * (UINT32 *)Data);
    }
    break;
  case 8:
    if (Read) {
      * (UINT64 *)Data = MmioRead64 (Address + Offset);
    } else {
      MmioWrite64 (Address + Offset, * (UINT64 *)Data);
    }
    break;
  default:
    ASSERT (FALSE);
    return EFI_INVALID_PARAMETER;
  }

  return EFI_SUCCESS;
}

/**
  Read Nvm Express controller capability register.

  @param  Address          PCI base address to the Nvme controller.
  @param  Capability       The buffer used to store capability register content.

  @return EFI_SUCCESS      Successfully read the controller capability register content.
  @return EFI_DEVICE_ERROR Fail to read the controller capability register.

**/
EFI_STATUS
ReadNvmeControllerCapabilities (
  IN UINT32                           Address,
  IN NVME_CAP                         *Capability
  )
{
  EFI_STATUS            Status;
  UINT64                Cap;

  Status = NvmHcRwMmio (Address, NVME_CAP_OFFSET, TRUE, sizeof (Cap), &Cap);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  CopyMem (Capability, &Cap, sizeof (Cap));

  return EFI_SUCCESS;
}

/**
  Read Nvm Express controller configuration register.

  @param  Address          PCI base address to the Nvme controller.
  @param  Cc               The buffer used to store configuration register content.

  @return EFI_SUCCESS      Successfully read the controller configuration register content.
  @return EFI_DEVICE_ERROR Fail to read the controller configuration register.

**/
EFI_STATUS
ReadNvmeControllerConfiguration (
  IN UINT32                           Address,
  IN NVME_CC                          *Cc
  )
{
  EFI_STATUS            Status;
  UINT32                Data;

  Status = NvmHcRwMmio (Address, NVME_CC_OFFSET, TRUE, sizeof (Data), &Data);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  CopyMem (Cc, &Data, sizeof (Data));
  return EFI_SUCCESS;
}

/**
  Write Nvm Express controller configuration register.

  @param  Address          PCI base address to the Nvme controller.
  @param  Cc               The buffer used to store the content to be written into configuration register.

  @return EFI_SUCCESS      Successfully write data into the controller configuration register.
  @return EFI_DEVICE_ERROR Fail to write data into the controller configuration register.

**/
EFI_STATUS
WriteNvmeControllerConfiguration (
  IN UINT32                           Address,
  IN NVME_CC                          *Cc
  )
{
  EFI_STATUS            Status;
  UINT32                Data;

  Data   = ReadUnaligned32 ((UINT32 *)Cc);

  Status = NvmHcRwMmio (Address, NVME_CC_OFFSET, FALSE, sizeof (Data), &Data);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  DEBUG ((DEBUG_VERBOSE, "Cc.En: %d\n", Cc->En));
  DEBUG ((DEBUG_VERBOSE, "Cc.Css: %d\n", Cc->Css));
  DEBUG ((DEBUG_VERBOSE, "Cc.Mps: %d\n", Cc->Mps));
  DEBUG ((DEBUG_VERBOSE, "Cc.Ams: %d\n", Cc->Ams));
  DEBUG ((DEBUG_VERBOSE, "Cc.Shn: %d\n", Cc->Shn));
  DEBUG ((DEBUG_VERBOSE, "Cc.Iosqes: %d\n", Cc->Iosqes));
  DEBUG ((DEBUG_VERBOSE, "Cc.Iocqes: %d\n", Cc->Iocqes));

  return EFI_SUCCESS;
}

/**
  Read Nvm Express controller status register.

  @param  Address          PCI base address to the Nvme controller.
  @param  Csts             The buffer used to store status register content.

  @return EFI_SUCCESS      Successfully read the controller status register content.
  @return EFI_DEVICE_ERROR Fail to read the controller status register.

**/
EFI_STATUS
ReadNvmeControllerStatus (
  IN UINT32                           Address,
  IN NVME_CSTS                        *Csts
  )
{
  EFI_STATUS            Status;
  UINT32                Data;

  Status = NvmHcRwMmio (Address, NVME_CSTS_OFFSET, TRUE, sizeof (Data), &Data);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  CopyMem (Csts, &Data, sizeof (Data));
  return EFI_SUCCESS;
}

/**
  Read Nvm Express admin queue attributes register.

  @param  Address          PCI base address to the Nvme controller.
  @param  Aqa              The buffer used to store admin queue attributes register content.

  @return EFI_SUCCESS      Successfully read the admin queue attributes register content.
  @return EFI_DEVICE_ERROR Fail to read the admin queue attributes register.

**/
EFI_STATUS
ReadNvmeAdminQueueAttributes (
  IN UINT32                           Address,
  IN NVME_AQA                         *Aqa
  )
{
  EFI_STATUS            Status;
  UINT32                Data;

  Status = NvmHcRwMmio (Address, NVME_AQA_OFFSET, TRUE, sizeof (Data), &Data);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  CopyMem (Aqa, &Data, sizeof (Data));
  return EFI_SUCCESS;
}

/**
  Write Nvm Express admin queue attributes register.

  @param  Address          PCI base address to the Nvme controller.
  @param  Aqa              The buffer used to store the content to be written into admin queue attributes register.

  @return EFI_SUCCESS      Successfully write data into the admin queue attributes register.
  @return EFI_DEVICE_ERROR Fail to write data into the admin queue attributes register.

**/
EFI_STATUS
WriteNvmeAdminQueueAttributes (
  IN UINT32                           Address,
  IN NVME_AQA                         *Aqa
  )
{
  EFI_STATUS            Status;
  UINT32                Data;

  Data   = ReadUnaligned32 ((UINT32 *)Aqa);
  Status = NvmHcRwMmio (Address, NVME_AQA_OFFSET, FALSE, sizeof (Data), &Data);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  DEBUG ((DEBUG_VERBOSE, "Aqa.Asqs: %d\n", Aqa->Asqs));
  DEBUG ((DEBUG_VERBOSE, "Aqa.Acqs: %d\n", Aqa->Acqs));

  return EFI_SUCCESS;
}

/**
  Read Nvm Express admin submission queue base address register.

  @param  Address          PCI base address to the Nvme controller.
  @param  Asq              The buffer used to store admin submission queue base address register content.

  @return EFI_SUCCESS      Successfully read the admin submission queue base address register content.
  @return EFI_DEVICE_ERROR Fail to read the admin submission queue base address register.

**/
EFI_STATUS
ReadNvmeAdminSubmissionQueueBaseAddress (
  IN UINT32                           Address,
  IN NVME_ASQ                         *Asq
  )
{
  EFI_STATUS            Status;
  UINT64                Data;

  Status = NvmHcRwMmio (Address, NVME_ASQ_OFFSET, TRUE, sizeof (Data), &Data);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  CopyMem (Asq, &Data, sizeof (Data));
  return EFI_SUCCESS;
}

/**
  Write Nvm Express admin submission queue base address register.

  @param  Address          PCI base address to the Nvme controller.
  @param  Asq              The buffer used to store the content to be written into admin submission queue base address register.

  @return EFI_SUCCESS      Successfully write data into the admin submission queue base address register.
  @return EFI_DEVICE_ERROR Fail to write data into the admin submission queue base address register.

**/
EFI_STATUS
WriteNvmeAdminSubmissionQueueBaseAddress (
  IN UINT32                           Address,
  IN NVME_ASQ                         *Asq
  )
{
  EFI_STATUS            Status;
  UINT64                Data;

  Data   = ReadUnaligned64 ((UINT64 *)Asq);

  Status = NvmHcRwMmio (Address, NVME_ASQ_OFFSET, FALSE, sizeof (Data), &Data);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  DEBUG ((DEBUG_VERBOSE, "Asq: %lx\n", *Asq));

  return EFI_SUCCESS;
}

/**
  Read Nvm Express admin completion queue base address register.

  @param  Address          PCI base address to the Nvme controller.
  @param  Acq              The buffer used to store admin completion queue base address register content.

  @return EFI_SUCCESS      Successfully read the admin completion queue base address register content.
  @return EFI_DEVICE_ERROR Fail to read the admin completion queue base address register.

**/
EFI_STATUS
ReadNvmeAdminCompletionQueueBaseAddress (
  IN UINT32                           Address,
  IN NVME_ACQ                         *Acq
  )
{
  EFI_STATUS            Status;
  UINT64                Data;

  Status = NvmHcRwMmio (Address, NVME_ACQ_OFFSET, TRUE, sizeof (Data), &Data);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  CopyMem (Acq, &Data, sizeof (Data));
  return EFI_SUCCESS;
}

/**
  Write Nvm Express admin completion queue base address register.

  @param  Address          PCI base address to the Nvme controller.
  @param  Acq              The buffer used to store the content to be written into admin completion queue base address register.

  @return EFI_SUCCESS      Successfully write data into the admin completion queue base address register.
  @return EFI_DEVICE_ERROR Fail to write data into the admin completion queue base address register.

**/
EFI_STATUS
WriteNvmeAdminCompletionQueueBaseAddress (
  IN UINT32                           Address,
  IN NVME_ACQ                         *Acq
  )
{
  EFI_STATUS            Status;
  UINT64                Data;

  Data   = ReadUnaligned64 ((UINT64 *)Acq);

  Status = NvmHcRwMmio (Address, NVME_ACQ_OFFSET, FALSE, sizeof (Data), &Data);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  DEBUG ((DEBUG_VERBOSE, "Acq: %lxh\n", *Acq));

  return EFI_SUCCESS;
}

/**
  Disable the Nvm Express controller.

  @param  Private          The pointer to the NVME_CONTROLLER_PRIVATE_DATA data structure.

  @return EFI_SUCCESS      Successfully disable the controller.
  @return EFI_DEVICE_ERROR Fail to disable the controller.

**/
EFI_STATUS
NvmeDisableController (
  IN NVME_CONTROLLER_PRIVATE_DATA     *Private
  )
{
  NVME_CC                Cc;
  NVME_CSTS              Csts;
  EFI_STATUS             Status;
  UINT32                 Index;
  UINT8                  Timeout;
  UINT32                 NvmeHCBase;

  //NVME PCI base address
  NvmeHCBase = Private->NvmeHCBase;

  //
  // Read Controller Configuration Register.
  //
  Status = ReadNvmeControllerConfiguration (NvmeHCBase, &Cc);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Cc.En = 0;

  //
  // Disable the controller.
  //
  Status = WriteNvmeControllerConfiguration (NvmeHCBase, &Cc);

  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Cap.To specifies max delay time in 500ms increments for Csts.Rdy to transition from 1 to 0 after
  // Cc.Enable transition from 1 to 0. Loop produces a 1 millisecond delay per itteration, up to 500 * Cap.To.
  //
  if (Private->Cap.To == 0) {
    Timeout = 1;
  } else {
    Timeout = Private->Cap.To;
  }

  for (Index = (Timeout * 500); Index != 0; --Index) {

    NanoSecondDelay (1000 * 1000);

    //
    // Check if the controller is initialized
    //
    Status = ReadNvmeControllerStatus (NvmeHCBase, &Csts);

    if (EFI_ERROR (Status)) {
      return Status;
    }

    if (Csts.Rdy == 0) {
      break;
    }
  }

  if (Index == 0) {
    Status = EFI_DEVICE_ERROR;
  }

  DEBUG ((DEBUG_VERBOSE, "NVMe controller is disabled with status [%r].\n", Status));
  return Status;
}

/**
  Enable the Nvm Express controller.

  @param  Private          The pointer to the NVME_CONTROLLER_PRIVATE_DATA data structure.

  @return EFI_SUCCESS      Successfully enable the controller.
  @return EFI_DEVICE_ERROR Fail to enable the controller.
  @return EFI_TIMEOUT      Fail to enable the controller in given time slot.

**/
EFI_STATUS
NvmeEnableController (
  IN NVME_CONTROLLER_PRIVATE_DATA     *Private
  )
{
  NVME_CC                Cc;
  NVME_CSTS              Csts;
  EFI_STATUS             Status;
  UINT32                 Index;
  UINT8                  Timeout;
  UINT32                 NvmeHCBase;

  //NVME PCI base address
  NvmeHCBase = Private->NvmeHCBase;

  //
  // Enable the controller.
  // CC.AMS, CC.MPS and CC.CSS are all set to 0.
  //
  ZeroMem (&Cc, sizeof (NVME_CC));
  Cc.En     = 1;
  Cc.Iosqes = 6;
  Cc.Iocqes = 4;

  Status = WriteNvmeControllerConfiguration (NvmeHCBase, &Cc);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Cap.To specifies max delay time in 500ms increments for Csts.Rdy to set after
  // Cc.Enable. Loop produces a 1 millisecond delay per itteration, up to 500 * Cap.To.
  //
  if (Private->Cap.To == 0) {
    Timeout = 1;
  } else {
    Timeout = Private->Cap.To;
  }

  for (Index = (Timeout * 500); Index != 0; --Index) {

    NanoSecondDelay (1000 * 1000);

    //
    // Check if the controller is initialized
    //
    Status = ReadNvmeControllerStatus (NvmeHCBase, &Csts);

    if (EFI_ERROR (Status)) {
      return Status;
    }

    if (Csts.Rdy) {
      break;
    }
  }

  if (Index == 0) {
    Status = EFI_TIMEOUT;
  }

  DEBUG ((DEBUG_VERBOSE, "NVMe controller is enabled with status [%r].\n", Status));
  return Status;
}

/**
  Get identify controller data.

  @param  Private          The pointer to the NVME_CONTROLLER_PRIVATE_DATA data structure.
  @param  Buffer           The buffer used to store the identify controller data.

  @return EFI_SUCCESS      Successfully get the identify controller data.
  @return EFI_DEVICE_ERROR Fail to get the identify controller data.

**/
EFI_STATUS
NvmeIdentifyController (
  IN NVME_CONTROLLER_PRIVATE_DATA       *Private,
  IN VOID                               *Buffer
  )
{
  EFI_NVM_EXPRESS_PASS_THRU_COMMAND_PACKET CommandPacket;
  EFI_NVM_EXPRESS_COMMAND                  Command;
  EFI_NVM_EXPRESS_COMPLETION               Completion;
  EFI_STATUS                               Status;

  ZeroMem (&CommandPacket, sizeof (EFI_NVM_EXPRESS_PASS_THRU_COMMAND_PACKET));
  ZeroMem (&Command, sizeof (EFI_NVM_EXPRESS_COMMAND));
  ZeroMem (&Completion, sizeof (EFI_NVM_EXPRESS_COMPLETION));

  Command.Cdw0.Opcode = NVME_ADMIN_IDENTIFY_CMD;
  //
  // According to Nvm Express 1.1 spec Figure 38, When not used, the field shall be cleared to 0h.
  // For the Identify command, the Namespace Identifier is only used for the Namespace data structure.
  //
  Command.Nsid        = 0;

  CommandPacket.NvmeCmd        = &Command;
  CommandPacket.NvmeCompletion = &Completion;
  CommandPacket.TransferBuffer = Buffer;
  CommandPacket.TransferLength = sizeof (NVME_ADMIN_CONTROLLER_DATA);
  CommandPacket.CommandTimeout = NVME_GENERIC_TIMEOUT;
  CommandPacket.QueueType      = NVME_ADMIN_QUEUE;
  //
  // Set bit 0 (Cns bit) to 1 to identify a controller
  //
  Command.Cdw10                = 1;
  Command.Flags                = CDW10_VALID;

  Status = Private->Passthru.PassThru (
             &Private->Passthru,
             NVME_CONTROLLER_ID,
             &CommandPacket,
             NULL
             );

  return Status;
}

/**
  Get specified identify namespace data.

  @param  Private          The pointer to the NVME_CONTROLLER_PRIVATE_DATA data structure.
  @param  NamespaceId      The specified namespace identifier.
  @param  Buffer           The buffer used to store the identify namespace data.

  @return EFI_SUCCESS      Successfully get the identify namespace data.
  @return EFI_DEVICE_ERROR Fail to get the identify namespace data.

**/
EFI_STATUS
NvmeIdentifyNamespace (
  IN NVME_CONTROLLER_PRIVATE_DATA      *Private,
  IN UINT32                            NamespaceId,
  IN VOID                              *Buffer
  )
{
  EFI_NVM_EXPRESS_PASS_THRU_COMMAND_PACKET CommandPacket;
  EFI_NVM_EXPRESS_COMMAND                  Command;
  EFI_NVM_EXPRESS_COMPLETION               Completion;
  EFI_STATUS                               Status;

  ZeroMem (&CommandPacket, sizeof (EFI_NVM_EXPRESS_PASS_THRU_COMMAND_PACKET));
  ZeroMem (&Command, sizeof (EFI_NVM_EXPRESS_COMMAND));
  ZeroMem (&Completion, sizeof (EFI_NVM_EXPRESS_COMPLETION));

  CommandPacket.NvmeCmd        = &Command;
  CommandPacket.NvmeCompletion = &Completion;

  Command.Cdw0.Opcode = NVME_ADMIN_IDENTIFY_CMD;
  Command.Nsid        = NamespaceId;
  CommandPacket.TransferBuffer = Buffer;
  CommandPacket.TransferLength = sizeof (NVME_ADMIN_NAMESPACE_DATA);
  CommandPacket.CommandTimeout = NVME_GENERIC_TIMEOUT;
  CommandPacket.QueueType      = NVME_ADMIN_QUEUE;
  //
  // Set bit 0 (Cns bit) to 1 to identify a namespace
  //
  CommandPacket.NvmeCmd->Cdw10 = 0;
  CommandPacket.NvmeCmd->Flags = CDW10_VALID;

  Status = Private->Passthru.PassThru (
             &Private->Passthru,
             NamespaceId,
             &CommandPacket,
             NULL
             );

  return Status;
}

/**
  Create io completion queue.

  @param  Private          The pointer to the NVME_CONTROLLER_PRIVATE_DATA data structure.

  @return EFI_SUCCESS      Successfully create io completion queue.
  @return EFI_DEVICE_ERROR Fail to create io completion queue.

**/
EFI_STATUS
NvmeCreateIoCompletionQueue (
  IN NVME_CONTROLLER_PRIVATE_DATA      *Private
  )
{
  EFI_NVM_EXPRESS_PASS_THRU_COMMAND_PACKET CommandPacket;
  EFI_NVM_EXPRESS_COMMAND                  Command;
  EFI_NVM_EXPRESS_COMPLETION               Completion;
  EFI_STATUS                               Status;
  NVME_ADMIN_CRIOCQ                        CrIoCq;
  UINT32                                   Index;
  UINT16                                   QueueSize;

  Status = EFI_SUCCESS;

  for (Index = 1; Index < NVME_MAX_QUEUES; Index++) {
    ZeroMem (&CommandPacket, sizeof (EFI_NVM_EXPRESS_PASS_THRU_COMMAND_PACKET));
    ZeroMem (&Command, sizeof (EFI_NVM_EXPRESS_COMMAND));
    ZeroMem (&Completion, sizeof (EFI_NVM_EXPRESS_COMPLETION));
    ZeroMem (&CrIoCq, sizeof (NVME_ADMIN_CRIOCQ));

    CommandPacket.NvmeCmd        = &Command;
    CommandPacket.NvmeCompletion = &Completion;

    Command.Cdw0.Opcode = NVME_ADMIN_CRIOCQ_CMD;
    CommandPacket.TransferBuffer = Private->CqBuffer[Index];
    CommandPacket.TransferLength = EFI_PAGE_SIZE;
    CommandPacket.CommandTimeout = NVME_GENERIC_TIMEOUT;
    CommandPacket.QueueType      = NVME_ADMIN_QUEUE;

    if (Index == 1) {
      QueueSize = NVME_CCQ_SIZE;
    } else {
      if (Private->Cap.Mqes > NVME_ASYNC_CCQ_SIZE) {
        QueueSize = NVME_ASYNC_CCQ_SIZE;
      } else {
        QueueSize = Private->Cap.Mqes;
      }
    }

    CrIoCq.Qid   = Index;
    CrIoCq.Qsize = QueueSize;
    CrIoCq.Pc    = 1;
    CopyMem (&CommandPacket.NvmeCmd->Cdw10, &CrIoCq, sizeof (NVME_ADMIN_CRIOCQ));
    CommandPacket.NvmeCmd->Flags = CDW10_VALID | CDW11_VALID;

    Status = Private->Passthru.PassThru (
               &Private->Passthru,
               0,
               &CommandPacket,
               NULL
               );
    if (EFI_ERROR (Status)) {
      break;
    }
  }

  return Status;
}

/**
  Create io submission queue.

  @param  Private          The pointer to the NVME_CONTROLLER_PRIVATE_DATA data structure.

  @return EFI_SUCCESS      Successfully create io submission queue.
  @return EFI_DEVICE_ERROR Fail to create io submission queue.

**/
EFI_STATUS
NvmeCreateIoSubmissionQueue (
  IN NVME_CONTROLLER_PRIVATE_DATA      *Private
  )
{
  EFI_NVM_EXPRESS_PASS_THRU_COMMAND_PACKET CommandPacket;
  EFI_NVM_EXPRESS_COMMAND                  Command;
  EFI_NVM_EXPRESS_COMPLETION               Completion;
  EFI_STATUS                               Status;
  NVME_ADMIN_CRIOSQ                        CrIoSq;
  UINT32                                   Index;
  UINT16                                   QueueSize;

  Status = EFI_SUCCESS;

  for (Index = 1; Index < NVME_MAX_QUEUES; Index++) {
    ZeroMem (&CommandPacket, sizeof (EFI_NVM_EXPRESS_PASS_THRU_COMMAND_PACKET));
    ZeroMem (&Command, sizeof (EFI_NVM_EXPRESS_COMMAND));
    ZeroMem (&Completion, sizeof (EFI_NVM_EXPRESS_COMPLETION));
    ZeroMem (&CrIoSq, sizeof (NVME_ADMIN_CRIOSQ));

    CommandPacket.NvmeCmd        = &Command;
    CommandPacket.NvmeCompletion = &Completion;

    Command.Cdw0.Opcode = NVME_ADMIN_CRIOSQ_CMD;
    CommandPacket.TransferBuffer = Private->SqBuffer[Index];
    CommandPacket.TransferLength = EFI_PAGE_SIZE;
    CommandPacket.CommandTimeout = NVME_GENERIC_TIMEOUT;
    CommandPacket.QueueType      = NVME_ADMIN_QUEUE;

    if (Index == 1) {
      QueueSize = NVME_CSQ_SIZE;
    } else {
      if (Private->Cap.Mqes > NVME_ASYNC_CSQ_SIZE) {
        QueueSize = NVME_ASYNC_CSQ_SIZE;
      } else {
        QueueSize = Private->Cap.Mqes;
      }
    }

    CrIoSq.Qid   = Index;
    CrIoSq.Qsize = QueueSize;
    CrIoSq.Pc    = 1;
    CrIoSq.Cqid  = Index;
    CrIoSq.Qprio = 0;
    CopyMem (&CommandPacket.NvmeCmd->Cdw10, &CrIoSq, sizeof (NVME_ADMIN_CRIOSQ));
    CommandPacket.NvmeCmd->Flags = CDW10_VALID | CDW11_VALID;

    Status = Private->Passthru.PassThru (
               &Private->Passthru,
               0,
               &CommandPacket,
               NULL
               );
    if (EFI_ERROR (Status)) {
      break;
    }
  }

  return Status;
}

/**
  Initialize the Nvm Express controller.

  @param[in] Private                 The pointer to the NVME_CONTROLLER_PRIVATE_DATA data structure.

  @retval EFI_SUCCESS                The NVM Express Controller is initialized successfully.
  @retval Others                     A device error occurred while initializing the controller.

**/
EFI_STATUS
NvmeControllerInit (
  IN NVME_CONTROLLER_PRIVATE_DATA    *Private
  )
{
  EFI_STATUS                      Status;
  NVME_AQA                        Aqa;
  NVME_ASQ                        Asq;
  NVME_ACQ                        Acq;
  UINT8                           Sn[21];
  UINT8                           Mn[41];
  UINT32                          NvmeHCBase;

  //NVME PCI base address
  NvmeHCBase = Private->NvmeHCBase;

  //
  // Read the Controller Capabilities register and verify that the NVM command set is supported
  //
  Status = ReadNvmeControllerCapabilities (NvmeHCBase, &Private->Cap);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  if (Private->Cap.Css != 0x01) {
    DEBUG ((DEBUG_VERBOSE, "NvmeControllerInit: the controller doesn't support NVMe command set\n"));
    return EFI_UNSUPPORTED;
  }

  //
  // Currently the driver only supports 4k page size.
  //
  ASSERT ((Private->Cap.Mpsmin + 12) <= EFI_PAGE_SHIFT);

  Private->Cid[0] = 0;
  Private->Cid[1] = 0;
  Private->Cid[2] = 0;
  Private->Pt[0]  = 0;
  Private->Pt[1]  = 0;
  Private->Pt[2]  = 0;
  Private->SqTdbl[0].Sqt = 0;
  Private->SqTdbl[1].Sqt = 0;
  Private->SqTdbl[2].Sqt = 0;
  Private->CqHdbl[0].Cqh = 0;
  Private->CqHdbl[1].Cqh = 0;
  Private->CqHdbl[2].Cqh = 0;
  Private->AsyncSqHead   = 0;

  Status = NvmeDisableController (Private);

  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // set number of entries admin submission & completion queues.
  //
  Aqa.Asqs  = NVME_ASQ_SIZE;
  Aqa.Rsvd1 = 0;
  Aqa.Acqs  = NVME_ACQ_SIZE;
  Aqa.Rsvd2 = 0;

  //
  // Address of admin submission queue.
  //
  Asq = (UINT64) (UINTN) (Private->Buffer) & ~0xFFF;

  //
  // Address of admin completion queue.
  //
  Acq = (UINT64) (UINTN) (Private->Buffer + EFI_PAGE_SIZE) & ~0xFFF;

  //
  // Address of I/O submission & completion queue.
  //
  ZeroMem (Private->Buffer, EFI_PAGES_TO_SIZE (6));
  Private->SqBuffer[0]        = (NVME_SQ *) (UINTN) (Private->Buffer);
  Private->CqBuffer[0]        = (NVME_CQ *) (UINTN) (Private->Buffer + 1 * EFI_PAGE_SIZE);
  Private->SqBuffer[1]        = (NVME_SQ *) (UINTN) (Private->Buffer + 2 * EFI_PAGE_SIZE);
  Private->CqBuffer[1]        = (NVME_CQ *) (UINTN) (Private->Buffer + 3 * EFI_PAGE_SIZE);
  Private->SqBuffer[2]        = (NVME_SQ *) (UINTN) (Private->Buffer + 4 * EFI_PAGE_SIZE);
  Private->CqBuffer[2]        = (NVME_CQ *) (UINTN) (Private->Buffer + 5 * EFI_PAGE_SIZE);

  DEBUG ((DEBUG_VERBOSE, "Private->Buffer = [%016X]\n", (UINT64) (UINTN)Private->Buffer));
  DEBUG ((DEBUG_VERBOSE, "Admin     Submission Queue size (Aqa.Asqs) = [%08X]\n", Aqa.Asqs));
  DEBUG ((DEBUG_VERBOSE, "Admin     Completion Queue size (Aqa.Acqs) = [%08X]\n", Aqa.Acqs));
  DEBUG ((DEBUG_VERBOSE, "Admin     Submission Queue (SqBuffer[0]) = [%016X]\n", Private->SqBuffer[0]));
  DEBUG ((DEBUG_VERBOSE, "Admin     Completion Queue (CqBuffer[0]) = [%016X]\n", Private->CqBuffer[0]));
  DEBUG ((DEBUG_VERBOSE, "Sync  I/O Submission Queue (SqBuffer[1]) = [%016X]\n", Private->SqBuffer[1]));
  DEBUG ((DEBUG_VERBOSE, "Sync  I/O Completion Queue (CqBuffer[1]) = [%016X]\n", Private->CqBuffer[1]));
  DEBUG ((DEBUG_VERBOSE, "Async I/O Submission Queue (SqBuffer[2]) = [%016X]\n", Private->SqBuffer[2]));
  DEBUG ((DEBUG_VERBOSE, "Async I/O Completion Queue (CqBuffer[2]) = [%016X]\n", Private->CqBuffer[2]));

  //
  // Program admin queue attributes.
  //
  Status = WriteNvmeAdminQueueAttributes (NvmeHCBase, &Aqa);

  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Program admin submission queue address.
  //
  Status = WriteNvmeAdminSubmissionQueueBaseAddress (NvmeHCBase, &Asq);

  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Program admin completion queue address.
  //
  Status = WriteNvmeAdminCompletionQueueBaseAddress (NvmeHCBase, &Acq);

  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = NvmeEnableController (Private);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Allocate buffer for Identify Controller data
  //
  if (Private->ControllerData == NULL) {
    Private->ControllerData = (NVME_ADMIN_CONTROLLER_DATA *)AllocateZeroPool (sizeof (NVME_ADMIN_CONTROLLER_DATA));

    if (Private->ControllerData == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }
  }

  //
  // Get current Identify Controller Data
  //
  Status = NvmeIdentifyController (Private, Private->ControllerData);

  if (EFI_ERROR (Status)) {
    FreePool (Private->ControllerData);
    Private->ControllerData = NULL;
    return EFI_NOT_FOUND;
  }

  //
  // Dump NvmExpress Identify Controller Data
  //
  CopyMem (Sn, Private->ControllerData->Sn, sizeof (Private->ControllerData->Sn));
  Sn[20] = 0;
  CopyMem (Mn, Private->ControllerData->Mn, sizeof (Private->ControllerData->Mn));
  Mn[40] = 0;
  DEBUG ((DEBUG_VERBOSE, " == NVME IDENTIFY CONTROLLER DATA ==\n"));
  DEBUG ((DEBUG_VERBOSE, "    PCI VID   : 0x%x\n", Private->ControllerData->Vid));
  DEBUG ((DEBUG_VERBOSE, "    PCI SSVID : 0x%x\n", Private->ControllerData->Ssvid));
  DEBUG ((DEBUG_VERBOSE, "    SN        : %a\n",   Sn));
  DEBUG ((DEBUG_VERBOSE, "    MN        : %a\n",   Mn));
  DEBUG ((DEBUG_VERBOSE, "    FR        : 0x%x\n", * ((UINT64 *)Private->ControllerData->Fr)));
  DEBUG ((DEBUG_VERBOSE, "    RAB       : 0x%x\n", Private->ControllerData->Rab));
  DEBUG ((DEBUG_VERBOSE, "    IEEE      : 0x%x\n", * (UINT32 *)Private->ControllerData->Ieee_oui));
  DEBUG ((DEBUG_VERBOSE, "    AERL      : 0x%x\n", Private->ControllerData->Aerl));
  DEBUG ((DEBUG_VERBOSE, "    SQES      : 0x%x\n", Private->ControllerData->Sqes));
  DEBUG ((DEBUG_VERBOSE, "    CQES      : 0x%x\n", Private->ControllerData->Cqes));
  DEBUG ((DEBUG_VERBOSE, "    NN        : 0x%x\n", Private->ControllerData->Nn));

  //
  // Create two I/O completion queues.
  // One for blocking I/O, one for non-blocking I/O.
  //
  Status = NvmeCreateIoCompletionQueue (Private);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Create two I/O Submission queues.
  // One for blocking I/O, one for non-blocking I/O.
  //
  Status = NvmeCreateIoSubmissionQueue (Private);

  return Status;
}

