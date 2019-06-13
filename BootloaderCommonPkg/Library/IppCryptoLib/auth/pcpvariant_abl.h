/** @file

  Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

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
