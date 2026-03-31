/** @file
  Include file to support building the Intel IPPCrypto library.

Copyright (c) 2024 - 2026, Intel Corporation. All rights reserved.<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent

**/

// Stub assert.h for UEFI/EDK2 build environment.
// assert() is used only for internal debug checks in ipp-crypto (e.g. pcpsha3ca.c).
// Map to EDK2 BaseLib ASSERT() so debug invariants trigger a halt in DEBUG builds
// and are compiled out in RELEASE builds.

#ifndef _IPPCP_ASSERT_H_STUB_
#define _IPPCP_ASSERT_H_STUB_

#include <Library/DebugLib.h>

#define assert(expr) ASSERT((expr))

#endif /* _IPPCP_ASSERT_H_STUB_ */
