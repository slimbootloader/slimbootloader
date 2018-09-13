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
//  Purpose:
//     Intel(R) Integrated Performance Primitives.
//     Internal BNU32 arithmetic
// 
// 
*/

#if !defined(_CP_BNU32_ARITH_H)
#define _CP_BNU32_ARITH_H

#define cpAdd_BNU32 OWNAPI(cpAdd_BNU32)
Ipp32u cpAdd_BNU32(Ipp32u* pR, const Ipp32u* pA, const Ipp32u* pB, int ns);
#define cpSub_BNU32 OWNAPI(cpSub_BNU32)
Ipp32u cpSub_BNU32(Ipp32u* pR, const Ipp32u* pA, const Ipp32u* pB, int ns);
#define cpInc_BNU32 OWNAPI(cpInc_BNU32)
Ipp32u cpInc_BNU32(Ipp32u* pR, const Ipp32u* pA, cpSize ns, Ipp32u val);
#define cpDec_BNU32 OWNAPI(cpDec_BNU32)
Ipp32u cpDec_BNU32(Ipp32u* pR, const Ipp32u* pA, cpSize ns, Ipp32u val);

#define cpMulDgt_BNU32 OWNAPI(cpMulDgt_BNU32)
Ipp32u cpMulDgt_BNU32(Ipp32u* pR, const Ipp32u* pA, int ns, Ipp32u val);
#define cpSubMulDgt_BNU32 OWNAPI(cpSubMulDgt_BNU32)
Ipp32u cpSubMulDgt_BNU32(Ipp32u* pR, const Ipp32u* pA, int nsA, Ipp32u val);

#define cpDiv_BNU32 OWNAPI(cpDiv_BNU32)
int cpDiv_BNU32(Ipp32u* pQ, int* nsQ, Ipp32u* pX, int nsX, Ipp32u* pY, int nsY);
#define cpMod_BNU32(pX,sizeX, pM,sizeM) cpDiv_BNU32(NULL,NULL, (pX),(sizeX), (pM),(sizeM))

#define cpFromOS_BNU32 OWNAPI(cpFromOS_BNU32)
int cpFromOS_BNU32(Ipp32u* pBNU, const Ipp8u* pOctStr, int strLen);
#define cpToOS_BNU32 OWNAPI(cpToOS_BNU32)
int cpToOS_BNU32(Ipp8u* pStr, int strLen, const Ipp32u* pBNU, int bnuSize);

#endif /* _CP_BNU32_ARITH_H */
