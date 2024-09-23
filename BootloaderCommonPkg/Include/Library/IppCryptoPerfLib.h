/** @file

  Copyright (c) 2017 - 2020, Intel Corporation. All rights reserved.<BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#ifndef _IPP_CRYPTO_PERF_LIB_H_
#define _IPP_CRYPTO_PERF_LIB_H_

#include <Library/CryptoLib.h>

/* function to Measure crypto performance data */
VOID
EFIAPI
CryptoPerfTest (
);

/* function to print performance data */
VOID
EFIAPI
CryptoPerfTestPrintResult (
);

#endif
