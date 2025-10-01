/** @file
  Copyright (c) 2025, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#ifndef _TXT_LIB_H_
#define _TXT_LIB_H_

#define R_IOPORT_CMOS_STANDARD_INDEX            0x70
#define R_IOPORT_CMOS_STANDARD_DATA             0x71
#define TXT_CMOS_STATUS_REG                     0x2A

/**
  Determines whether or not the platform requires initialization for TXT use.

  @retval TRUE          - If the the platoform should be configured for TXT.
  @retval FALSE         - If TXT is not to be used.
**/
BOOLEAN
EFIAPI
IsTxtEnabled ();

/*
  Initialize Intel TXT
*/
EFI_STATUS
EFIAPI
InitTxt();

#endif
