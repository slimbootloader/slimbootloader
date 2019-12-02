/** @file

  Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

/*
//               Intel(R) Integrated Performance Primitives
//                   Cryptographic Primitives (ippcp)
//
*/
#if !defined(_CP_MONTRED_H)
#define _CP_MONTRED_H

#include "owndefs.h"
#include "owncp.h"

#include "pcpbnuimpl.h"
/*
// Montgomery reduction
*/
#define cpMontRedAdc_BNU OWNAPI(cpMontRedAdc_BNU)
void    cpMontRedAdc_BNU(BNU_CHUNK_T* pR,
                        BNU_CHUNK_T* pProduct,
                  const BNU_CHUNK_T* pModulus, cpSize nsM, BNU_CHUNK_T m0);

#define cpMontRedAdx_BNU OWNAPI(cpMontRedAdx_BNU)
void    cpMontRedAdx_BNU(BNU_CHUNK_T* pR,
                        BNU_CHUNK_T* pProduct,
                  const BNU_CHUNK_T* pModulus, cpSize nsM, BNU_CHUNK_T m0);

__INLINE void cpMontRed_BNU_opt(BNU_CHUNK_T* pR,
                                BNU_CHUNK_T* pProduct,
                          const BNU_CHUNK_T* pModulus, cpSize nsM, BNU_CHUNK_T m0)
{
#if(_ADCOX_NI_ENABLING_==_FEATURE_ON_)
   cpMontRedAdx_BNU(pR, pProduct, pModulus, nsM, m0);
#elif(_ADCOX_NI_ENABLING_==_FEATURE_TICKTOCK_)
   IsFeatureEnabled(ADCOX_ENABLED)? cpMontRedAdx_BNU(pR, pProduct, pModulus, nsM, m0)
                                  : cpMontRedAdc_BNU(pR, pProduct, pModulus, nsM, m0);
#else
   cpMontRedAdc_BNU(pR, pProduct, pModulus, nsM, m0);
#endif
}

#endif /* _CP_MONTRED_H */
