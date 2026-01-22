/** @file
  Default CSME Performance ID to String Mapping Library

  This library provides the default/common CSME performance event descriptions.
  Platform-specific implementations can override this by providing their own version.

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Library/BaseLib.h>

/**
  Provide description string for CSME performance event ID.

  This is the default implementation. Platforms can override by providing
  their own library with the same LIBRARY_CLASS.

  @param[in]  Id  CSME Performance Event ID

  @retval Description string or NULL if not found
**/
CHAR8 *
EFIAPI
CsmePerfIdToStr (
  IN UINT32 Id
  )
{
  switch (Id) {
  default:
    return "Reserved";
  }
}
