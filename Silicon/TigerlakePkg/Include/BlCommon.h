/** @file

  Copyright (c) 2016 Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _BL_COMMON_H_

#define ROUNDED_DOWN(val, align)  ((val) & ~((align) - 1))
#define ROUNDED_UP(  val, align)  ROUNDED_DOWN((val) + (align) - 1, (align))

#define KB_(n)  ((unsigned) (n) * 1024)
#define MB_(n)  ((unsigned) (n) * 1024 * 1024)
#define GB_(n)  ((unsigned) (n) * 1024 * 1024 * 1024)

#define BIT_( n)     (1U << (n))
#define BITS_(n, m)  (((~0U << (n)) ^ (~0U << (m))) | BIT_(n) | BIT_(m))

#endif
