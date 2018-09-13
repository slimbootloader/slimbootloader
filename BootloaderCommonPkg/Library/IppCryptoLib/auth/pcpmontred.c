/** @file

  Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

/* 
//               Intel(R) Integrated Performance Primitives
//                   Cryptographic Primitives (ippcp)
// 
*/

#include "owncp.h"
#include "pcpbnuarith.h"


#if defined(_USE_C_cpMontRedAdc_BNU_)
#pragma message ("C version of cpMontRedAdc_BNU: ON")
#else
#pragma message ("C version of cpMontRedAdc_BNU: OFF")
#endif

#if !((_IPP==_IPP_W7) || \
      (_IPP==_IPP_T7) || \
      (_IPP==_IPP_V8) || \
      (_IPP==_IPP_P8) || \
      (_IPP>=_IPP_G9) || \
      (_IPP==_IPP_S8) || \
      (_IPP32E==_IPP32E_M7) || \
      (_IPP32E==_IPP32E_U8) || \
      (_IPP32E==_IPP32E_Y8) || \
      (_IPP32E>=_IPP32E_E9) || \
      (_IPP32E==_IPP32E_N8)) || \
      defined(_USE_C_cpMontRedAdc_BNU_)
#define cpMontRedAdc_BNU OWNAPI(cpMontRedAdc_BNU)
void cpMontRedAdc_BNU(BNU_CHUNK_T* pR,
                      BNU_CHUNK_T* pProduct,
                const BNU_CHUNK_T* pModulus, cpSize nsM, BNU_CHUNK_T m0)
{
   BNU_CHUNK_T carry;
   BNU_CHUNK_T extension;

   cpSize n;
   for(n=0, carry = 0; n<(nsM-1); n++) {
      BNU_CHUNK_T u = pProduct[n]*m0;
      BNU_CHUNK_T t = pProduct[nsM +n +1] + carry;

      extension = cpAddMulDgt_BNU(pProduct+n, pModulus, nsM, u);
      ADD_AB(carry, pProduct[nsM+n], pProduct[nsM+n], extension);
      t += carry;

      carry = t<pProduct[nsM+n+1];
      pProduct[nsM+n+1] = t;
   }

   m0 *= pProduct[nsM-1];
   extension = cpAddMulDgt_BNU(pProduct+nsM-1, pModulus, nsM, m0);
   ADD_AB(extension, pProduct[2*nsM-1], pProduct[2*nsM-1], extension);

   carry |= extension;
   carry -= cpSub_BNU(pR, pProduct+nsM, pModulus, nsM);
   /* condition copy: R = carry? Product+mSize : R */
   MASKED_COPY_BNU(pR, carry, pProduct+nsM, pR, nsM);
}
#endif
