/** @file
  Header file for the Boot Media Library.

@copyright
 Copyright (c) 2016 - 2019 Intel Corporation. All rights reserved
 This software and associated documentation (if any) is furnished
 under a license and may only be used or copied in accordance
 with the terms of the license. Except as permitted by the
 license, no part of this software or documentation may be
 reproduced, stored in a retrieval system, or transmitted in any
 form or by any means without the express written consent of
 Intel Corporation.
 This file contains an 'Intel Peripheral Driver' and is uniquely
 identified as "Intel Reference Module" and is licensed for Intel
 CPUs and chipsets under the terms of your license agreement with
 Intel or your vendor. This file may be modified by the user, subject
 to additional terms of the license agreement.

@par Specification
**/

#ifndef _BOOT_MEDIA_LIB_H_
#define _BOOT_MEDIA_LIB_H_

#include <Uefi/UefiBaseType.h>

typedef enum {
  BootMediaEmmc,
  BootMediaUfs,
  BootMediaSpi,
  BootMediaMax
} BOOT_MEDIA_TYPE;

typedef enum {
  BootPartition1,
  BootPartition2,
  BootPartitionMax
} BOOT_PARTITION_SELECT;

/**
  Determines the boot media that the platform firmware is booting from

  @param[out] BootMediaType   The Boot Media the platform is booting from

  @retval     EFI_SUCCESS     The operation completed successfully.
**/
EFI_STATUS
EFIAPI
GetBootMediaType (
  OUT BOOT_MEDIA_TYPE        *BootMediaType
  );

/**
  Determines the boot partition that the platform firmware is booting from

  @param[out] BootPartition   The Boot partition the platform is booting from

  @retval     EFI_SUCCESS     The operation completed successfully.
**/
EFI_STATUS
EFIAPI
GetBootPartition (
  OUT BOOT_PARTITION_SELECT  *BootPartition
  );

/**
  Determines if the platform firmware is booting from SPI or not

  @retval TRUE        Platform firmware is booting from SPI
  @retval FALSE       Platform firmware is booting from a non-SPI device (eMMC, UFS, etc.)
**/
BOOLEAN
EFIAPI
BootMediaIsSpi (
  VOID
  );

/**
  Determines if the platform firmware is booting from eMMC or not

  @retval TRUE        Platform firmware is booting from eMMC
  @retval FALSE       Platform firmware is booting from a non-eMMC device (SPI, UFS, etc.)
**/
BOOLEAN
EFIAPI
BootMediaIsEmmc (
  VOID
  );

/**
  Determines if the platform firmware is booting from UFS or not

  @retval TRUE        Platform firmware is booting from UFS
  @retval FALSE       Platform firmware is booting from a non-UFS device (eMMC, SPI, etc.)
**/
BOOLEAN
EFIAPI
BootMediaIsUfs (
  VOID
  );


#endif
