/** @file

  Copyright (c) 2016 - 2019, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _BL_COMMON_H_

#define ROUNDED_DOWN(val, align)	((val) & ~((align) - 1))
#define ROUNDED_UP(  val, align)	ROUNDED_DOWN((val) + (align) - 1, (align))

#define KB_(n)	((unsigned) (n) * 1024)
#define MB_(n)	((unsigned) (n) * 1024 * 1024)
#define GB_(n)	((unsigned) (n) * 1024 * 1024 * 1024)

#define BIT_( n)     (1U << (n))
#define BITS_(n, m)  (((~0U << (n)) ^ (~0U << (m))) | BIT_(n) | BIT_(m))

#endif
