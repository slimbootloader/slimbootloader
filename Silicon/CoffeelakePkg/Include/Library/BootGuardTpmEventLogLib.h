/** @file

  Copyright (c) 2018 - 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

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
