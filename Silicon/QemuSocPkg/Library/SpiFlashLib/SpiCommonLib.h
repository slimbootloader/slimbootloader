/** @file

  Copyright (c) 2016 - 2017, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

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
