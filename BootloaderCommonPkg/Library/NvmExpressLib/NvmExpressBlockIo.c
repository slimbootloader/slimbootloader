/** @file
  NvmExpressDxe driver is used to manage non-volatile memory subsystem which follows
  NVM Express specification.

  Copyright (c) 2013 - 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "NvmExpress.h"

/**
  Get max transfer block number.

  @param[in]  Private       The pointer to the NVME_CONTROLLER_PRIVATE_DATA data structure.
  @param[in]  BlockSize     The media block size.

  @retval Max transfer block.

**/
STATIC
UINT32
GetMaxTransferBlockNumber (
  IN  NVME_CONTROLLER_PRIVATE_DATA   *Private,
  IN  UINT32                          BlockSize
)
{
  UINT32  MaxTransferBlocks;
  UINT32  MaxDmaTransferBlocks;

  if (Private->ControllerData->Mdts != 0) {
    MaxTransferBlocks = (1 << (Private->ControllerData->Mdts)) * (1 << (Private->Cap.Mpsmin + 12)) / BlockSize;
  } else {
    MaxTransferBlocks = 1024;
  }

  MaxDmaTransferBlocks = (PcdGet32 (PcdDmaBufferSize) >> 1) / BlockSize;
  if (MaxDmaTransferBlocks < MaxTransferBlocks) {
    MaxTransferBlocks = MaxDmaTransferBlocks;
  }
  return MaxTransferBlocks;
}


/**
  Read some sectors from the device.

  @param  Device                 The pointer to the NVME_DEVICE_PRIVATE_DATA data structure.
  @param  Buffer                 The buffer used to store the data read from the device.
  @param  Lba                    The start block number.
  @param  Blocks                 Total block number to be read.

  @retval EFI_SUCCESS            Datum are read from the device.
  @retval Others                 Fail to read all the datum.

**/
EFI_STATUS
ReadSectors (
  IN NVME_DEVICE_PRIVATE_DATA           *Device,
  IN UINT64                             Buffer,
  IN UINT64                             Lba,
  IN UINT32                             Blocks
  )
{
  NVME_CONTROLLER_PRIVATE_DATA             *Private;
  UINT32                                   Bytes;
  EFI_NVM_EXPRESS_PASS_THRU_COMMAND_PACKET CommandPacket;
  EFI_NVM_EXPRESS_COMMAND                  Command;
  EFI_NVM_EXPRESS_COMPLETION               Completion;
  EFI_STATUS                               Status;
  UINT32                                   BlockSize;

  Private    = Device->Controller;
  BlockSize  = Device->Media.BlockSize;
  Bytes      = Blocks * BlockSize;

  ZeroMem (&CommandPacket, sizeof (EFI_NVM_EXPRESS_PASS_THRU_COMMAND_PACKET));
  ZeroMem (&Command, sizeof (EFI_NVM_EXPRESS_COMMAND));
  ZeroMem (&Completion, sizeof (EFI_NVM_EXPRESS_COMPLETION));

  CommandPacket.NvmeCmd        = &Command;
  CommandPacket.NvmeCompletion = &Completion;

  CommandPacket.NvmeCmd->Cdw0.Opcode = NVME_IO_READ_OPC;
  CommandPacket.NvmeCmd->Nsid        = Device->NamespaceId;
  CommandPacket.TransferBuffer       = (VOID *) (UINTN)Buffer;

  CommandPacket.TransferLength = Bytes;
  CommandPacket.CommandTimeout = NVME_GENERIC_TIMEOUT;
  CommandPacket.QueueType      = NVME_IO_QUEUE;

  CommandPacket.NvmeCmd->Cdw10 = (UINT32)Lba;
  CommandPacket.NvmeCmd->Cdw11 = (UINT32)RShiftU64 (Lba, 32);
  CommandPacket.NvmeCmd->Cdw12 = (Blocks - 1) & 0xFFFF;

  CommandPacket.NvmeCmd->Flags = CDW10_VALID | CDW11_VALID | CDW12_VALID;

  Status = Private->Passthru.PassThru (
             &Private->Passthru,
             Device->NamespaceId,
             &CommandPacket,
             NULL
             );

  return Status;
}

/**
  Write some sectors to the device.

  @param  Device                 The pointer to the NVME_DEVICE_PRIVATE_DATA data structure.
  @param  Buffer                 The buffer to be written into the device.
  @param  Lba                    The start block number.
  @param  Blocks                 Total block number to be written.

  @retval EFI_SUCCESS            Datum are written into the buffer.
  @retval Others                 Fail to write all the datum.

**/
EFI_STATUS
WriteSectors (
  IN NVME_DEVICE_PRIVATE_DATA      *Device,
  IN UINT64                        Buffer,
  IN UINT64                        Lba,
  IN UINT32                        Blocks
  )
{
  NVME_CONTROLLER_PRIVATE_DATA             *Private;
  EFI_NVM_EXPRESS_PASS_THRU_COMMAND_PACKET CommandPacket;
  EFI_NVM_EXPRESS_COMMAND                  Command;
  EFI_NVM_EXPRESS_COMPLETION               Completion;
  EFI_STATUS                               Status;
  UINT32                                   Bytes;
  UINT32                                   BlockSize;

  Private    = Device->Controller;
  BlockSize  = Device->Media.BlockSize;
  Bytes      = Blocks * BlockSize;

  ZeroMem (&CommandPacket, sizeof (EFI_NVM_EXPRESS_PASS_THRU_COMMAND_PACKET));
  ZeroMem (&Command, sizeof (EFI_NVM_EXPRESS_COMMAND));
  ZeroMem (&Completion, sizeof (EFI_NVM_EXPRESS_COMPLETION));

  CommandPacket.NvmeCmd        = &Command;
  CommandPacket.NvmeCompletion = &Completion;

  CommandPacket.NvmeCmd->Cdw0.Opcode = NVME_IO_WRITE_OPC;
  CommandPacket.NvmeCmd->Nsid  = Device->NamespaceId;
  CommandPacket.TransferBuffer = (VOID *) (UINTN)Buffer;

  CommandPacket.TransferLength = Bytes;
  CommandPacket.CommandTimeout = NVME_GENERIC_TIMEOUT;
  CommandPacket.QueueType      = NVME_IO_QUEUE;

  CommandPacket.NvmeCmd->Cdw10 = (UINT32)Lba;
  CommandPacket.NvmeCmd->Cdw11 = (UINT32)RShiftU64 (Lba, 32);
  //
  // Set Force Unit Access bit (bit 30) to use write-through behaviour
  //
  CommandPacket.NvmeCmd->Cdw12 = ((Blocks - 1) & 0xFFFF) | BIT30;

  CommandPacket.MetadataBuffer = NULL;
  CommandPacket.MetadataLength = 0;

  CommandPacket.NvmeCmd->Flags = CDW10_VALID | CDW11_VALID | CDW12_VALID;

  Status = Private->Passthru.PassThru (
             &Private->Passthru,
             Device->NamespaceId,
             &CommandPacket,
             NULL
             );

  return Status;
}

/**
  Read some blocks from the device.

  @param  Device                 The pointer to the NVME_DEVICE_PRIVATE_DATA data structure.
  @param  Buffer                 The buffer used to store the data read from the device.
  @param  Lba                    The start block number.
  @param  Blocks                 Total block number to be read.

  @retval EFI_SUCCESS            Datum are read from the device.
  @retval Others                 Fail to read all the datum.

**/
EFI_STATUS
NvmeRead (
  IN     NVME_DEVICE_PRIVATE_DATA       *Device,
  OUT VOID                              *Buffer,
  IN     UINT64                         Lba,
  IN     UINTN                          Blocks
  )
{
  EFI_STATUS                       Status;
  UINT32                           BlockSize;
  NVME_CONTROLLER_PRIVATE_DATA     *Private;
  UINT32                           MaxTransferBlocks;
  UINTN                            OrginalBlocks;
  BOOLEAN                          IsEmpty;

  //
  // Wait for the device's asynchronous I/O queue to become empty.
  //
  while (TRUE) {
    IsEmpty = IsListEmpty (&Device->AsyncQueue);

    if (IsEmpty) {
      break;
    }

    NanoSecondDelay (100 * 1000);
  }

  Status        = EFI_SUCCESS;
  Private       = Device->Controller;
  BlockSize     = Device->Media.BlockSize;
  OrginalBlocks = Blocks;

  MaxTransferBlocks = GetMaxTransferBlockNumber (Private, BlockSize);
  while (Blocks > 0) {
    if (Blocks > MaxTransferBlocks) {
      Status = ReadSectors (Device, (UINT64) (UINTN)Buffer, Lba, MaxTransferBlocks);

      Blocks -= MaxTransferBlocks;
      Buffer  = (VOID *) (UINTN) ((UINT64) (UINTN)Buffer + MaxTransferBlocks * BlockSize);
      Lba    += MaxTransferBlocks;
    } else {
      Status = ReadSectors (Device, (UINT64) (UINTN)Buffer, Lba, (UINT32)Blocks);
      Blocks = 0;
    }

    if (EFI_ERROR (Status)) {
      break;
    }
  }

  DEBUG ((DEBUG_VERBOSE, "%a: Lba = 0x%08Lx, Original = 0x%08Lx, "
          "Remaining = 0x%08Lx, BlockSize = 0x%x, Status = %r\n", __FUNCTION__, Lba,
          (UINT64)OrginalBlocks, (UINT64)Blocks, BlockSize, Status));

  return Status;
}

/**
  Write some blocks to the device.

  @param  Device                 The pointer to the NVME_DEVICE_PRIVATE_DATA data structure.
  @param  Buffer                 The buffer to be written into the device.
  @param  Lba                    The start block number.
  @param  Blocks                 Total block number to be written.

  @retval EFI_SUCCESS            Datum are written into the buffer.
  @retval Others                 Fail to write all the datum.

**/
EFI_STATUS
NvmeWrite (
  IN NVME_DEVICE_PRIVATE_DATA           *Device,
  IN VOID                               *Buffer,
  IN UINT64                             Lba,
  IN UINTN                              Blocks
  )
{
  EFI_STATUS                       Status;
  UINT32                           BlockSize;
  NVME_CONTROLLER_PRIVATE_DATA     *Private;
  UINT32                           MaxTransferBlocks;
  UINTN                            OrginalBlocks;
  BOOLEAN                          IsEmpty;

  //
  // Wait for the device's asynchronous I/O queue to become empty.
  //
  while (TRUE) {
    IsEmpty = IsListEmpty (&Device->AsyncQueue);

    if (IsEmpty) {
      break;
    }

    NanoSecondDelay (100 * 1000);
  }

  Status        = EFI_SUCCESS;
  Private       = Device->Controller;
  BlockSize     = Device->Media.BlockSize;
  OrginalBlocks = Blocks;

  MaxTransferBlocks = GetMaxTransferBlockNumber (Private, BlockSize);
  while (Blocks > 0) {
    if (Blocks > MaxTransferBlocks) {
      Status = WriteSectors (Device, (UINT64) (UINTN)Buffer, Lba, MaxTransferBlocks);

      Blocks -= MaxTransferBlocks;
      Buffer  = (VOID *) (UINTN) ((UINT64) (UINTN)Buffer + MaxTransferBlocks * BlockSize);
      Lba    += MaxTransferBlocks;
    } else {
      Status = WriteSectors (Device, (UINT64) (UINTN)Buffer, Lba, (UINT32)Blocks);
      Blocks = 0;
    }

    if (EFI_ERROR (Status)) {
      break;
    }
  }

  DEBUG ((DEBUG_VERBOSE, "%a: Lba = 0x%08Lx, Original = 0x%08Lx, "
          "Remaining = 0x%08Lx, BlockSize = 0x%x, Status = %r\n", __FUNCTION__, Lba,
          (UINT64)OrginalBlocks, (UINT64)Blocks, BlockSize, Status));

  return Status;
}

/**
  Flushes all modified data to the device.

  @param  Device                 The pointer to the NVME_DEVICE_PRIVATE_DATA data structure.

  @retval EFI_SUCCESS            Datum are written into the buffer.
  @retval Others                 Fail to write all the datum.

**/
EFI_STATUS
NvmeFlush (
  IN NVME_DEVICE_PRIVATE_DATA      *Device
  )
{
  NVME_CONTROLLER_PRIVATE_DATA             *Private;
  EFI_NVM_EXPRESS_PASS_THRU_COMMAND_PACKET CommandPacket;
  EFI_NVM_EXPRESS_COMMAND                  Command;
  EFI_NVM_EXPRESS_COMPLETION               Completion;
  EFI_STATUS                               Status;

  Private = Device->Controller;

  ZeroMem (&CommandPacket, sizeof (EFI_NVM_EXPRESS_PASS_THRU_COMMAND_PACKET));
  ZeroMem (&Command, sizeof (EFI_NVM_EXPRESS_COMMAND));
  ZeroMem (&Completion, sizeof (EFI_NVM_EXPRESS_COMPLETION));

  CommandPacket.NvmeCmd        = &Command;
  CommandPacket.NvmeCompletion = &Completion;

  CommandPacket.NvmeCmd->Cdw0.Opcode = NVME_IO_FLUSH_OPC;
  CommandPacket.NvmeCmd->Nsid  = Device->NamespaceId;
  CommandPacket.CommandTimeout = NVME_GENERIC_TIMEOUT;
  CommandPacket.QueueType      = NVME_IO_QUEUE;

  Status = Private->Passthru.PassThru (
             &Private->Passthru,
             Device->NamespaceId,
             &CommandPacket,
             NULL
             );

  return Status;
}

/**
  Reset the Block Device.

  @param  This                 Indicates a pointer to the calling context.
  @param  ExtendedVerification Driver may perform diagnostics on reset.

  @retval EFI_SUCCESS          The device was reset.
  @retval EFI_DEVICE_ERROR     The device is not functioning properly and could
                               not be reset.

**/
EFI_STATUS
EFIAPI
NvmeBlockIoReset (
  IN  EFI_BLOCK_IO_PROTOCOL   *This,
  IN  BOOLEAN                 ExtendedVerification
  )
{
  NVME_CONTROLLER_PRIVATE_DATA    *Private;
  NVME_DEVICE_PRIVATE_DATA        *Device;
  EFI_STATUS                      Status;

  if (This == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // For Nvm Express subsystem, reset block device means reset controller.
  //

  Device  = NVME_DEVICE_PRIVATE_DATA_FROM_BLOCK_IO (This);

  Private = Device->Controller;

  Status  = NvmeControllerInit (Private);

  if (EFI_ERROR (Status)) {
    Status = EFI_DEVICE_ERROR;
  }

  return Status;
}

/**
  Read BufferSize bytes from Lba into Buffer.

  @param  This       Indicates a pointer to the calling context.
  @param  MediaId    Id of the media, changes every time the media is replaced.
  @param  Lba        The starting Logical Block Address to read from.
  @param  BufferSize Size of Buffer, must be a multiple of device block size.
  @param  Buffer     A pointer to the destination buffer for the data. The caller is
                     responsible for either having implicit or explicit ownership of the buffer.

  @retval EFI_SUCCESS           The data was read correctly from the device.
  @retval EFI_DEVICE_ERROR      The device reported an error while performing the read.
  @retval EFI_NO_MEDIA          There is no media in the device.
  @retval EFI_MEDIA_CHANGED     The MediaId does not matched the current device.
  @retval EFI_BAD_BUFFER_SIZE   The Buffer was not a multiple of the block size of the device.
  @retval EFI_INVALID_PARAMETER The read request contains LBAs that are not valid,
                                or the buffer is not on proper alignment.

**/
EFI_STATUS
EFIAPI
NvmeBlockIoReadBlocks (
  IN  EFI_BLOCK_IO_PROTOCOL   *This,
  IN  UINT32                  MediaId,
  IN  EFI_LBA                 Lba,
  IN  UINTN                   BufferSize,
  OUT VOID                    *Buffer
  )
{
  NVME_DEVICE_PRIVATE_DATA          *Device;
  EFI_STATUS                        Status;
  EFI_BLOCK_IO_MEDIA                *Media;
  UINTN                             BlockSize;
  UINTN                             NumberOfBlocks;
  UINTN                             IoAlign;

  //
  // Check parameters.
  //
  if (This == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Media = This->Media;

  if (MediaId != Media->MediaId) {
    return EFI_MEDIA_CHANGED;
  }

  if (Buffer == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (BufferSize == 0) {
    return EFI_SUCCESS;
  }

  BlockSize = Media->BlockSize;
  if ((BufferSize % BlockSize) != 0) {
    return EFI_BAD_BUFFER_SIZE;
  }

  NumberOfBlocks  = BufferSize / BlockSize;
  if ((Lba + NumberOfBlocks - 1) > Media->LastBlock) {
    return EFI_INVALID_PARAMETER;
  }

  IoAlign = Media->IoAlign;
  if (IoAlign > 0 && (((UINTN) Buffer & (IoAlign - 1)) != 0)) {
    return EFI_INVALID_PARAMETER;
  }

  Device = NVME_DEVICE_PRIVATE_DATA_FROM_BLOCK_IO (This);

  Status = NvmeRead (Device, Buffer, Lba, NumberOfBlocks);

  return Status;
}

/**
  Write BufferSize bytes from Lba into Buffer.

  @param  This       Indicates a pointer to the calling context.
  @param  MediaId    The media ID that the write request is for.
  @param  Lba        The starting logical block address to be written. The caller is
                     responsible for writing to only legitimate locations.
  @param  BufferSize Size of Buffer, must be a multiple of device block size.
  @param  Buffer     A pointer to the source buffer for the data.

  @retval EFI_SUCCESS           The data was written correctly to the device.
  @retval EFI_WRITE_PROTECTED   The device can not be written to.
  @retval EFI_DEVICE_ERROR      The device reported an error while performing the write.
  @retval EFI_NO_MEDIA          There is no media in the device.
  @retval EFI_MEDIA_CHNAGED     The MediaId does not matched the current device.
  @retval EFI_BAD_BUFFER_SIZE   The Buffer was not a multiple of the block size of the device.
  @retval EFI_INVALID_PARAMETER The write request contains LBAs that are not valid,
                                or the buffer is not on proper alignment.

**/
EFI_STATUS
EFIAPI
NvmeBlockIoWriteBlocks (
  IN  EFI_BLOCK_IO_PROTOCOL   *This,
  IN  UINT32                  MediaId,
  IN  EFI_LBA                 Lba,
  IN  UINTN                   BufferSize,
  IN  VOID                    *Buffer
  )
{
  NVME_DEVICE_PRIVATE_DATA          *Device;
  EFI_STATUS                        Status;
  EFI_BLOCK_IO_MEDIA                *Media;
  UINTN                             BlockSize;
  UINTN                             NumberOfBlocks;
  UINTN                             IoAlign;

  //
  // Check parameters.
  //
  if (This == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Media = This->Media;

  if (MediaId != Media->MediaId) {
    return EFI_MEDIA_CHANGED;
  }

  if (Buffer == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (BufferSize == 0) {
    return EFI_SUCCESS;
  }

  BlockSize = Media->BlockSize;
  if ((BufferSize % BlockSize) != 0) {
    return EFI_BAD_BUFFER_SIZE;
  }

  NumberOfBlocks  = BufferSize / BlockSize;
  if ((Lba + NumberOfBlocks - 1) > Media->LastBlock) {
    return EFI_INVALID_PARAMETER;
  }

  IoAlign = Media->IoAlign;
  if (IoAlign > 0 && (((UINTN) Buffer & (IoAlign - 1)) != 0)) {
    return EFI_INVALID_PARAMETER;
  }

  Device = NVME_DEVICE_PRIVATE_DATA_FROM_BLOCK_IO (This);

  Status = NvmeWrite (Device, Buffer, Lba, NumberOfBlocks);

  return Status;
}

/**
  Flush the Block Device.

  @param  This              Indicates a pointer to the calling context.

  @retval EFI_SUCCESS       All outstanding data was written to the device.
  @retval EFI_DEVICE_ERROR  The device reported an error while writing back the data.
  @retval EFI_NO_MEDIA      There is no media in the device.

**/
EFI_STATUS
EFIAPI
NvmeBlockIoFlushBlocks (
  IN  EFI_BLOCK_IO_PROTOCOL   *This
  )
{
  NVME_DEVICE_PRIVATE_DATA          *Device;
  EFI_STATUS                        Status;

  //
  // Check parameters.
  //
  if (This == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Device = NVME_DEVICE_PRIVATE_DATA_FROM_BLOCK_IO (This);

  Status = NvmeFlush (Device);

  return Status;
}

/**
  Trust transfer data from/to NVMe device.

  This function performs one NVMe transaction to do a trust transfer from/to NVMe device.

  @param  Private                      The pointer to the NVME_CONTROLLER_PRIVATE_DATA data structure.
  @param  Buffer                       The pointer to the current transaction buffer.
  @param  SecurityProtocolId           The value of the "Security Protocol" parameter of
                                       the security protocol command to be sent.
  @param  SecurityProtocolSpecificData The value of the "Security Protocol Specific" parameter
                                       of the security protocol command to be sent.
  @param  TransferLength               The block number or sector count of the transfer.
  @param  IsTrustSend                  Indicates whether it is a trust send operation or not.
  @param  Timeout                      The timeout, in 100ns units, to use for the execution
                                       of the security protocol command. A Timeout value of 0
                                       means that this function will wait indefinitely for the
                                       security protocol command to execute. If Timeout is greater
                                       than zero, then this function will return EFI_TIMEOUT
                                       if the time required to execute the receive data command
                                       is greater than Timeout.
  @param  TransferLengthOut            A pointer to a buffer to store the size in bytes of the data
                                       written to the buffer. Ignore it when IsTrustSend is TRUE.

  @retval EFI_SUCCESS       The data transfer is complete successfully.
  @return others            Some error occurs when transferring data.

**/
EFI_STATUS
TrustTransferNvmeDevice (
  IN OUT NVME_CONTROLLER_PRIVATE_DATA      *Private,
  IN OUT VOID                              *Buffer,
  IN UINT8                                 SecurityProtocolId,
  IN UINT16                                SecurityProtocolSpecificData,
  IN UINTN                                 TransferLength,
  IN BOOLEAN                               IsTrustSend,
  IN UINT64                                Timeout,
  OUT UINTN                                *TransferLengthOut
  )
{
  EFI_NVM_EXPRESS_PASS_THRU_COMMAND_PACKET CommandPacket;
  EFI_NVM_EXPRESS_COMMAND                  Command;
  EFI_NVM_EXPRESS_COMPLETION               Completion;
  EFI_STATUS                               Status;
  UINT16                                   SpecificData;

  ZeroMem (&CommandPacket, sizeof (EFI_NVM_EXPRESS_PASS_THRU_COMMAND_PACKET));
  ZeroMem (&Command, sizeof (EFI_NVM_EXPRESS_COMMAND));
  ZeroMem (&Completion, sizeof (EFI_NVM_EXPRESS_COMPLETION));

  CommandPacket.NvmeCmd        = &Command;
  CommandPacket.NvmeCompletion = &Completion;

  //
  // Change Endianness of SecurityProtocolSpecificData
  //
  SpecificData = (((SecurityProtocolSpecificData << 8) & 0xFF00) | (SecurityProtocolSpecificData >> 8));

  if (IsTrustSend) {
    Command.Cdw0.Opcode          = NVME_ADMIN_SECURITY_SEND_CMD;
    CommandPacket.TransferBuffer = Buffer;
    CommandPacket.TransferLength = (UINT32)TransferLength;
    CommandPacket.NvmeCmd->Cdw10 = (UINT32) ((SecurityProtocolId << 24) | (SpecificData << 8));
    CommandPacket.NvmeCmd->Cdw11 = (UINT32)TransferLength;
  } else {
    Command.Cdw0.Opcode          = NVME_ADMIN_SECURITY_RECEIVE_CMD;
    CommandPacket.TransferBuffer = Buffer;
    CommandPacket.TransferLength = (UINT32)TransferLength;
    CommandPacket.NvmeCmd->Cdw10 = (UINT32) ((SecurityProtocolId << 24) | (SpecificData << 8));
    CommandPacket.NvmeCmd->Cdw11 = (UINT32)TransferLength;
  }

  CommandPacket.NvmeCmd->Flags = CDW10_VALID | CDW11_VALID;
  CommandPacket.NvmeCmd->Nsid  = NVME_CONTROLLER_ID;
  CommandPacket.CommandTimeout = Timeout;
  CommandPacket.QueueType      = NVME_ADMIN_QUEUE;

  Status = Private->Passthru.PassThru (
             &Private->Passthru,
             NVME_CONTROLLER_ID,
             &CommandPacket,
             NULL
             );

  if (!IsTrustSend) {
    if (EFI_ERROR (Status))  {
      *TransferLengthOut = 0;
    } else {
      *TransferLengthOut = (UINTN) TransferLength;
    }
  }

  return Status;
}

/**
  Send a security protocol command to a device that receives data and/or the result
  of one or more commands sent by SendData.

  The ReceiveData function sends a security protocol command to the given MediaId.
  The security protocol command sent is defined by SecurityProtocolId and contains
  the security protocol specific data SecurityProtocolSpecificData. The function
  returns the data from the security protocol command in PayloadBuffer.

  For devices supporting the SCSI command set, the security protocol command is sent
  using the SECURITY PROTOCOL IN command defined in SPC-4.

  For devices supporting the ATA command set, the security protocol command is sent
  using one of the TRUSTED RECEIVE commands defined in ATA8-ACS if PayloadBufferSize
  is non-zero.

  If the PayloadBufferSize is zero, the security protocol command is sent using the
  Trusted Non-Data command defined in ATA8-ACS.

  If PayloadBufferSize is too small to store the available data from the security
  protocol command, the function shall copy PayloadBufferSize bytes into the
  PayloadBuffer and return EFI_WARN_BUFFER_TOO_SMALL.

  If PayloadBuffer or PayloadTransferSize is NULL and PayloadBufferSize is non-zero,
  the function shall return EFI_INVALID_PARAMETER.

  If the given MediaId does not support security protocol commands, the function shall
  return EFI_UNSUPPORTED. If there is no media in the device, the function returns
  EFI_NO_MEDIA. If the MediaId is not the ID for the current media in the device,
  the function returns EFI_MEDIA_CHANGED.

  If the security protocol fails to complete within the Timeout period, the function
  shall return EFI_TIMEOUT.

  If the security protocol command completes without an error, the function shall
  return EFI_SUCCESS. If the security protocol command completes with an error, the
  function shall return EFI_DEVICE_ERROR.

  @param  This                         Indicates a pointer to the calling context.
  @param  MediaId                      ID of the medium to receive data from.
  @param  Timeout                      The timeout, in 100ns units, to use for the execution
                                       of the security protocol command. A Timeout value of 0
                                       means that this function will wait indefinitely for the
                                       security protocol command to execute. If Timeout is greater
                                       than zero, then this function will return EFI_TIMEOUT
                                       if the time required to execute the receive data command
                                       is greater than Timeout.
  @param  SecurityProtocolId           The value of the "Security Protocol" parameter of
                                       the security protocol command to be sent.
  @param  SecurityProtocolSpecificData The value of the "Security Protocol Specific" parameter
                                       of the security protocol command to be sent.
  @param  PayloadBufferSize            Size in bytes of the payload data buffer.
  @param  PayloadBuffer                A pointer to a destination buffer to store the security
                                       protocol command specific payload data for the security
                                       protocol command. The caller is responsible for having
                                       either implicit or explicit ownership of the buffer.
  @param  PayloadTransferSize          A pointer to a buffer to store the size in bytes of the
                                       data written to the payload data buffer.

  @retval EFI_SUCCESS                  The security protocol command completed successfully.
  @retval EFI_WARN_BUFFER_TOO_SMALL    The PayloadBufferSize was too small to store the available
                                       data from the device. The PayloadBuffer contains the truncated data.
  @retval EFI_UNSUPPORTED              The given MediaId does not support security protocol commands.
  @retval EFI_DEVICE_ERROR             The security protocol command completed with an error.
  @retval EFI_NO_MEDIA                 There is no media in the device.
  @retval EFI_MEDIA_CHANGED            The MediaId is not for the current media.
  @retval EFI_INVALID_PARAMETER        The PayloadBuffer or PayloadTransferSize is NULL and
                                       PayloadBufferSize is non-zero.
  @retval EFI_TIMEOUT                  A timeout occurred while waiting for the security
                                       protocol command to execute.

**/
EFI_STATUS
EFIAPI
NvmeStorageSecurityReceiveData (
  IN  EFI_STORAGE_SECURITY_COMMAND_PROTOCOL    *This,
  IN  UINT32                                   MediaId,
  IN  UINT64                                   Timeout,
  IN  UINT8                                    SecurityProtocolId,
  IN  UINT16                                   SecurityProtocolSpecificData,
  IN  UINTN                                    PayloadBufferSize,
  OUT VOID                                     *PayloadBuffer,
  OUT UINTN                                    *PayloadTransferSize
  )
{
  EFI_STATUS                       Status;
  NVME_DEVICE_PRIVATE_DATA         *Device;

  Status  = EFI_SUCCESS;

  if ((PayloadBuffer == NULL) || (PayloadTransferSize == NULL) || (PayloadBufferSize == 0)) {
    return EFI_INVALID_PARAMETER;
  }

  Device = NVME_DEVICE_PRIVATE_DATA_FROM_STORAGE_SECURITY (This);

  if (MediaId != Device->BlockIo.Media->MediaId) {
    return EFI_MEDIA_CHANGED;
  }

  if (!Device->BlockIo.Media->MediaPresent) {
    return EFI_NO_MEDIA;
  }

  Status = TrustTransferNvmeDevice (
             Device->Controller,
             PayloadBuffer,
             SecurityProtocolId,
             SecurityProtocolSpecificData,
             PayloadBufferSize,
             FALSE,
             Timeout,
             PayloadTransferSize
             );

  return Status;
}

/**
  Send a security protocol command to a device.

  The SendData function sends a security protocol command containing the payload
  PayloadBuffer to the given MediaId. The security protocol command sent is
  defined by SecurityProtocolId and contains the security protocol specific data
  SecurityProtocolSpecificData. If the underlying protocol command requires a
  specific padding for the command payload, the SendData function shall add padding
  bytes to the command payload to satisfy the padding requirements.

  For devices supporting the SCSI command set, the security protocol command is sent
  using the SECURITY PROTOCOL OUT command defined in SPC-4.

  For devices supporting the ATA command set, the security protocol command is sent
  using one of the TRUSTED SEND commands defined in ATA8-ACS if PayloadBufferSize
  is non-zero. If the PayloadBufferSize is zero, the security protocol command is
  sent using the Trusted Non-Data command defined in ATA8-ACS.

  If PayloadBuffer is NULL and PayloadBufferSize is non-zero, the function shall
  return EFI_INVALID_PARAMETER.

  If the given MediaId does not support security protocol commands, the function
  shall return EFI_UNSUPPORTED. If there is no media in the device, the function
  returns EFI_NO_MEDIA. If the MediaId is not the ID for the current media in the
  device, the function returns EFI_MEDIA_CHANGED.

  If the security protocol fails to complete within the Timeout period, the function
  shall return EFI_TIMEOUT.

  If the security protocol command completes without an error, the function shall return
  EFI_SUCCESS. If the security protocol command completes with an error, the function
  shall return EFI_DEVICE_ERROR.

  @param  This                         Indicates a pointer to the calling context.
  @param  MediaId                      ID of the medium to receive data from.
  @param  Timeout                      The timeout, in 100ns units, to use for the execution
                                       of the security protocol command. A Timeout value of 0
                                       means that this function will wait indefinitely for the
                                       security protocol command to execute. If Timeout is greater
                                       than zero, then this function will return EFI_TIMEOUT
                                       if the time required to execute the send data command
                                       is greater than Timeout.
  @param  SecurityProtocolId           The value of the "Security Protocol" parameter of
                                       the security protocol command to be sent.
  @param  SecurityProtocolSpecificData The value of the "Security Protocol Specific" parameter
                                       of the security protocol command to be sent.
  @param  PayloadBufferSize            Size in bytes of the payload data buffer.
  @param  PayloadBuffer                A pointer to a destination buffer to store the security
                                       protocol command specific payload data for the security
                                       protocol command.

  @retval EFI_SUCCESS                  The security protocol command completed successfully.
  @retval EFI_UNSUPPORTED              The given MediaId does not support security protocol commands.
  @retval EFI_DEVICE_ERROR             The security protocol command completed with an error.
  @retval EFI_NO_MEDIA                 There is no media in the device.
  @retval EFI_MEDIA_CHANGED            The MediaId is not for the current media.
  @retval EFI_INVALID_PARAMETER        The PayloadBuffer is NULL and PayloadBufferSize is non-zero.
  @retval EFI_TIMEOUT                  A timeout occurred while waiting for the security
                                       protocol command to execute.

**/
EFI_STATUS
EFIAPI
NvmeStorageSecuritySendData (
  IN EFI_STORAGE_SECURITY_COMMAND_PROTOCOL    *This,
  IN UINT32                                   MediaId,
  IN UINT64                                   Timeout,
  IN UINT8                                    SecurityProtocolId,
  IN UINT16                                   SecurityProtocolSpecificData,
  IN UINTN                                    PayloadBufferSize,
  IN VOID                                     *PayloadBuffer
  )
{
  EFI_STATUS                       Status;
  NVME_DEVICE_PRIVATE_DATA         *Device;

  Status  = EFI_SUCCESS;

  if ((PayloadBuffer == NULL) && (PayloadBufferSize != 0)) {
    return EFI_INVALID_PARAMETER;
  }

  Device = NVME_DEVICE_PRIVATE_DATA_FROM_STORAGE_SECURITY (This);

  if (MediaId != Device->BlockIo.Media->MediaId) {
    return EFI_MEDIA_CHANGED;
  }

  if (!Device->BlockIo.Media->MediaPresent) {
    return EFI_NO_MEDIA;
  }

  Status = TrustTransferNvmeDevice (
             Device->Controller,
             PayloadBuffer,
             SecurityProtocolId,
             SecurityProtocolSpecificData,
             PayloadBufferSize,
             TRUE,
             Timeout,
             NULL
             );

  return Status;
}




