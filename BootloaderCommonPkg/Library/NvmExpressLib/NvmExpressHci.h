/** @file
  NvmExpressDxe driver is used to manage non-volatile memory subsystem which follows
  NVM Express specification.

  (C) Copyright 2016 Hewlett Packard Enterprise Development LP<BR>
  Copyright (c) 2013 - 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _NVME_HCI_H_
#define _NVME_HCI_H_

#define NVME_BAR                 0

//
// Offset from the beginning of private data queue buffer
//
#define NVME_ASQ_BUF_OFFSET                  EFI_PAGE_SIZE


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
  );

/**
  Initialize the Nvm Express controller.

  @param[in] Private                 The pointer to the NVME_CONTROLLER_PRIVATE_DATA data structure.

  @retval EFI_SUCCESS                The NVM Express Controller is initialized successfully.
  @retval Others                     A device error occurred while initializing the controller.

**/
EFI_STATUS
NvmeControllerInit (
  IN NVME_CONTROLLER_PRIVATE_DATA    *Private
  );

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
  );

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
  );

/**
  Disable the Nvm Express controller.

  @param  Private          The pointer to the NVME_CONTROLLER_PRIVATE_DATA data structure.

  @return EFI_SUCCESS      Successfully disable the controller.
  @return EFI_DEVICE_ERROR Fail to disable the controller.

**/
EFI_STATUS
NvmeDisableController (
  IN NVME_CONTROLLER_PRIVATE_DATA     *Private
  );

#endif

