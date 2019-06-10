/** @file

  Copyright (c) 2018 - 2019, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _BOOT_GUARD_TPM_EVENT_LOG_LIB_H_
#define _BOOT_GUARD_TPM_EVENT_LOG_LIB_H_

#include <Library/BootGuardLib.h>

/**
  Create Boot Guard TPM event log

  @param[in] TpmType - Which type of TPM is available on system.
**/
VOID
CreateTpmEventLog (
  IN TPM_TYPE TpmType
  );

#endif
