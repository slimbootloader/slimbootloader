/** @file

  Copyright (c) 2021, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __DTS_H__
#define __DTS_H__

/**
  Read PCH DTS

**/
VOID
EFIAPI
ReadPchDts (
  VOID
  );

/**
  Read CPU DTS

**/
VOID
EFIAPI
ReadCpuDts (
  VOID
  );

/**
  Append Boot DTS to Smbios

  @retval EFI_SUCCESS             Initialized necessary information successfully
  @retval EFI_OUT_OF_RESOURCES    Failed to allocate memory
  @retval other                   other errors

**/
EFI_STATUS
EFIAPI
AppendSmbiosBootDts (
  VOID
  );


#endif /* __DTS_H__ */
