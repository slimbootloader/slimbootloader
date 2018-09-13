/** @file

  Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#if !defined(_GS_MOD_METHOD_H)
#define _GS_MOD_METHOD_H

//#include "owndefs.h"
#include "owncp.h"

#include "pcpbnuimpl.h"
//#include "gsmodstuff.h"

typedef struct _gsModEngine gsEngine;

/* modular arith methods */
typedef BNU_CHUNK_T* (*mod_encode)(BNU_CHUNK_T* pR, const BNU_CHUNK_T* pA, gsEngine* pMA);
typedef BNU_CHUNK_T* (*mod_decode)(BNU_CHUNK_T* pR, const BNU_CHUNK_T* pA, gsEngine* pMA);
typedef BNU_CHUNK_T* (*mod_red)   (BNU_CHUNK_T* pR,       BNU_CHUNK_T* pA, gsEngine* pMA);
typedef BNU_CHUNK_T* (*mod_sqr)   (BNU_CHUNK_T* pR, const BNU_CHUNK_T* pA, gsEngine* pMA);
typedef BNU_CHUNK_T* (*mod_mul)   (BNU_CHUNK_T* pR, const BNU_CHUNK_T* pA, const BNU_CHUNK_T* pB, gsEngine* pMA);
typedef BNU_CHUNK_T* (*mod_add)   (BNU_CHUNK_T* pR, const BNU_CHUNK_T* pA, const BNU_CHUNK_T* pB, gsEngine* pMA);
typedef BNU_CHUNK_T* (*mod_sub)   (BNU_CHUNK_T* pR, const BNU_CHUNK_T* pA, const BNU_CHUNK_T* pB, gsEngine* pMA);
typedef BNU_CHUNK_T* (*mod_neg)   (BNU_CHUNK_T* pR, const BNU_CHUNK_T* pA, gsEngine* pMA);
typedef BNU_CHUNK_T* (*mod_div2)  (BNU_CHUNK_T* pR, const BNU_CHUNK_T* pA, gsEngine* pMA);
typedef BNU_CHUNK_T* (*mod_mul2)  (BNU_CHUNK_T* pR, const BNU_CHUNK_T* pA, gsEngine* pMA);
typedef BNU_CHUNK_T* (*mod_mul3)  (BNU_CHUNK_T* pR, const BNU_CHUNK_T* pA, gsEngine* pMA);

typedef struct _gsModMethod {
   mod_encode encode;
   mod_decode decode;
   mod_mul  mul;
   mod_sqr  sqr;
   mod_red  red;
   mod_add  add;
   mod_sub  sub;
   mod_neg  neg;
   mod_div2 div2;
   mod_mul2 mul2;
   mod_mul3 mul3;
} gsModMethod;

#if 0 // constant exevution time version
__INLINE BNU_CHUNK_T cpIsZero(BNU_CHUNK_T x)
{
   #if (BNU_CHUNK_BITS == BNU_CHUNK_64BIT)
   x |= x>>32;
   #endif
   x |= x>>16;
   x |= x>>8;
   x |= x>>4;
   x |= x>>2;
   x |= x>>1;
   return x&1;
}
#endif

__INLINE BNU_CHUNK_T cpIsZero(BNU_CHUNK_T x)
{  return x==0; }
__INLINE BNU_CHUNK_T cpIsNonZero(BNU_CHUNK_T x)
{  return x!=0; }
__INLINE BNU_CHUNK_T cpIsOdd(BNU_CHUNK_T x)
{  return x&1; }
__INLINE BNU_CHUNK_T cpIsEven(BNU_CHUNK_T x)
{  return 1-cpIsOdd(x); }

/* dst[] = (flag)? src[] : dst[] */
__INLINE void cpMaskMove_gs(BNU_CHUNK_T* dst, const BNU_CHUNK_T* src, int len, BNU_CHUNK_T moveFlag)
{
   BNU_CHUNK_T srcMask = 0-cpIsNonZero(moveFlag);
   BNU_CHUNK_T dstMask = ~srcMask;
   int n;
   for(n=0; n<len; n++)
      dst[n] = (src[n] & srcMask) ^  (dst[n] & dstMask);
}

/* common available pre-defined methos */
#define      gsModMethod OWNAPI(gsModMethod)
gsModMethod* gsModArith(void);

/* available pre-defined methos for RSA */
#define      gsModMethodRSA OWNAPI(gsModMethodRSA)
gsModMethod* gsModArithRSA(void);

/* available pre-defined methos for ippsMont* */
#define      gsModMethodMont OWNAPI(gsModMethodMont)
gsModMethod* gsModArithMont(void);

/* available pre-defined methos for DLP * */
#define      gsModMethodDLP OWNAPI(gsModMethodDLP)
gsModMethod* gsModArithDLP(void);

/* available pre-defined common methos for GF over prime * */
#define      gsArithGFpOWNAPI(gsArithGFp)
gsModMethod* gsArithGFp(void);

/* ... and etc ... */

#endif /* _GS_MOD_METHOD_H */

