/** @file

  Copyright (c) 2016 - 2024, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#if FixedPcdGetBool (PcdWclSupport) == 1
#include <Register/GpioAcpiDefinesWcl.h>
#else
#include <Register/GpioAcpiDefinesPtl.h>
#endif
