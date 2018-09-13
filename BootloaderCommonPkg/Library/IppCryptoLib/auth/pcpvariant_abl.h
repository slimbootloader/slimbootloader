/** @file

  Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#if !defined(_CP_VARIANT_ABL_H)
#define _CP_VARIANT_ABL_H

#define _DISABLE_ALG_SHA224_
#define _DISABLE_ALG_SHA384_
#define _DISABLE_ALG_SHA512_224_
#define _DISABLE_ALG_SHA512_256_
#define _DISABLE_ALG_MD5_
#define _DISABLE_ALG_SM3_

#if defined(_SLIMBOOT_OPT)
  #define _SHA_NI_ENABLING_ _FEATURE_OFF_
  #define _DISABLE_ALG_SHA1_
  #define _DISABLE_ALG_SHA512_
  #define _ALG_SHA256_COMPACT_
#else
  #define _SHA_NI_ENABLING_ _FEATURE_ON_
#endif

#define _ALG_SHA512_COMPACT_

#endif /* _CP_VARIANT_ABL_H */
