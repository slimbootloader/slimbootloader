/** @file
  ME HeciHelper implementation.

  Copyright (c) 2018 - 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Base.h>
#include <PiPei.h>
#include "HeciHelper.h"

/**
  Calculate if the circular buffer has overflowed.
  Corresponds to HECI HPS (part of) section 4.2.1

  @param[in] ReadPointer          Location of the read pointer.
  @param[in] WritePointer         Location of the write pointer.

  @retval                         Number of filled slots.
**/
UINT8
FilledSlots (
  IN UINT32 ReadPointer,
  IN UINT32 WritePointer
  )
{
  UINT8 FilledSlots;

  ///
  /// Calculation documented in HECI HPS 0.68 section 4.2.1
  ///
  FilledSlots = (((INT8) WritePointer) - ((INT8) ReadPointer));

  return FilledSlots;
}

/**
  Calculate if the circular buffer has overflowed
  Corresponds to HECI HPS (part of) section 4.2.1

  @param[in] ReadPointer          Value read from host/me read pointer
  @param[in] WritePointer         Value read from host/me write pointer
  @param[in] BufferDepth          Value read from buffer depth register

  @retval EFI_DEVICE_ERROR        The circular buffer has overflowed
  @retval EFI_SUCCESS             The circular buffer does not overflowed
**/
EFI_STATUS
OverflowCB (
  IN UINT32 ReadPointer,
  IN UINT32 WritePointer,
  IN UINT32 BufferDepth
  )
{
  UINT8 FilledSlots;

  ///
  /// Calculation documented in HECI HPS 0.68 section 4.2.1
  ///
  FilledSlots = (((INT8) WritePointer) - ((INT8) ReadPointer));

  ///
  /// test for overflow
  ///
  if (FilledSlots > ((UINT8) BufferDepth)) {
    return EFI_DEVICE_ERROR;
  }

  return EFI_SUCCESS;
}
