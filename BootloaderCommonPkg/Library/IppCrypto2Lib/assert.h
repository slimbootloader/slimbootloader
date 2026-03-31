/** @file
  Include file to support building the Intel IPPCrypto library.

Copyright (c) 2024, Intel Corporation. All rights reserved.<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent

**/

// Stub assert.h for UEFI/EDK2 build environment.
// assert() is used only for internal debug checks in ipp-crypto (e.g. pcpsha3ca.c).
// In a firmware build there is no CRT, so define it as a no-op.

#ifndef _IPPCP_ASSERT_H_STUB_
#define _IPPCP_ASSERT_H_STUB_

#define assert(expr) ((void)0)

#endif /* _IPPCP_ASSERT_H_STUB_ */
