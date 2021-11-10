/** @file
  Definitions for HECI common

  Copyright (c) 2018 - 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#ifndef _HECI_HELPER_H_
#define _HECI_HELPER_H_


/**
  Calculate if the circular buffer has overflowed.
  Corresponds to HECI HPS (part of) section 4.2.1

  @param[in] ReadPointer          Location of the read pointer.
  @param[in] WritePointer         Location of the write pointer.

  @retval UINT8                   Number of filled slots.
**/
UINT8
FilledSlots (
  IN UINT32 ReadPointer,
  IN UINT32 WritePointer
  );

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
  );

#endif // _HECI_HELPER_H_
