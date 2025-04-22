/** @file

  Copyright (c) 2016 - 2017, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _SPI_COMMON_LIB_H_
#define _SPI_COMMON_LIB_H_

///
/// Private data structure definitions for the driver
///
#define PCH_SPI_PRIVATE_DATA_SIGNATURE  SIGNATURE_32 ('P', 'S', 'P', 'I')

typedef struct {
  UINT32                Signature;
  EFI_HANDLE            Handle;
  UINT32                StoreBase;
  UINT32                TotalFlashSize;
  UINT32                BlockSize;
} SPI_INSTANCE;


#endif
