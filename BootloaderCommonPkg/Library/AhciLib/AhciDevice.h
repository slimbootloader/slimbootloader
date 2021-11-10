/** @file
  This file provides AHCI SATA device specific structure.

  Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _AHCI_DEVICE_H_
#define _AHCI_DEVICE_H_

#include <IndustryStandard/Pci.h>
#include <IndustryStandard/Atapi.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BootloaderCommonLib.h>
#include <Library/TimerLib.h>
#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <Library/IoMmuLib.h>
#include "AhciMode.h"

#define  ATA_ATAPI_DEVICE_SIGNATURE     SIGNATURE_32 ('a', 'd', 'e', 'v')
#define  AHCI_CONTROLLER_SIGNATURE      SIGNATURE_32 ('a', 'h', 'c', 'i')

#define  EFI_ATA_DEVICE_FROM_LINK(a) \
           CR (a, EFI_ATA_DEVICE_INFO, Link, ATA_ATAPI_DEVICE_SIGNATURE)

#define  ATAPI_BLOCK_SIZE               0x800
#define  ATA_BLOCK_SIZE                 0x200
#define  ATAPI_INVALID_MAX_LBA_ADDRESS  0xFFFFFFFF

#define  AHCI_MAX_48_TRANSFER_SECTOR    65536
#define  AHCI_MAX_28_TRANSFER_SECTOR    256

#define  DEVICE_LBA_48_SUPPORT          BIT1
#define  DMA_WAIT_TIMEOUT_MS            500

//
// ATA device info
//
typedef struct {
  UINT32                            Signature;
  LIST_ENTRY                        Link;
  UINT16                            Port;
  UINT16                            PortMultiplier;
  EFI_ATA_DEVICE_TYPE               Type;
  UINT32                            BlockSize;
  UINT32                            DeviceFeature;
  EFI_LBA                           TotalBlockNumber;
  EFI_IDENTIFY_DATA                 IdentifyData;
  EFI_AHCI_CONTROLLER              *Controller;
} EFI_ATA_DEVICE_INFO;

/**
  Block read/write to ATA device.

  The function performs read or write operation to the ATA device.

  @param[in]  AtaDevice     ATA device instance.
  @param[in]  Read          Read or write.
  @param[in]  Lba           The starting logical block address (LBA) to read from
                            on the device.
  @param[in]  BufferSize    The size of the Buffer in bytes. This number must be
                            a multiple of the intrinsic block size of the device.
  @param[out] Buffer        A pointer to the destination buffer for the data.
                            The caller is responsible for the ownership of the
                            buffer.

  @retval EFI_INVALID_PARAMETER  Inpurt parameters are not valid.
  @retval EFI_DEVICE_ERROR       The DMA data transfer abort with error occurs.
  @retval EFI_TIMEOUT            The operation is time out.
  @retval EFI_UNSUPPORTED        The device is not ready for transfer.
  @retval EFI_SUCCESS            The DMA data transfer executes successfully.

**/
EFI_STATUS
AhciReadWriteBlock (
  IN     EFI_ATA_DEVICE_INFO  *AtaDevice,
  IN     BOOLEAN               Read,
  IN     EFI_LBA               Lba,
  IN     UINTN                 BufferSize,
  OUT    VOID                 *Buffer
  );

#endif
