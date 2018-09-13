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
// 
//  Purpose:
//     Cryptography Primitive.
//     BN Multiplication (Karatsuba method) Definitions & Function Prototypes
// 
//  Contents:
//     cpKaratsubaBufferSize()
//     cpMul_BNU_karatuba()
//     cpSqr_BNU_karatuba()
//     cpKAdd_BNU()
//     cpKSub_BNU()
// 
// 
*/

#if !defined(_KARATSUBA_MUL_)
#define _KARATSUBA_MUL_

#if defined(_USE_KARATSUBA_)

#if((_IPP==_IPP_W7) || \
    (_IPP==_IPP_T7))
   #define CP_KARATSUBA_MUL_THRESHOLD 16
   #define CP_KARATSUBA_SQR_THRESHOLD 32
#elif ((_IPP==_IPP_V8) || \
       (_IPP==_IPP_P8) || \
       (_IPP==_IPP_G9) || \
       (_IPPLP32==_IPPLP32_S8))
   #define CP_KARATSUBA_MUL_THRESHOLD 32
   #define CP_KARATSUBA_SQR_THRESHOLD 32
#elif ((_IPP>=_IPP_H9))
   #define CP_KARATSUBA_MUL_THRESHOLD 32
   #define CP_KARATSUBA_SQR_THRESHOLD 32

#elif ((_IPP32E==_IPP32E_M7) || \
       (_IPP32E==_IPP32E_U8) || \
       (_IPP32E==_IPP32E_Y8) || \
       (_IPP32E==_IPP32E_E9) || \
       (_IPPLP64==_IPPLP64_N8))
   #define CP_KARATSUBA_MUL_THRESHOLD 16
   #define CP_KARATSUBA_SQR_THRESHOLD 40
#elif ((_IPP32E>=_IPP32E_L9))
   #define CP_KARATSUBA_MUL_THRESHOLD 20
   #define CP_KARATSUBA_SQR_THRESHOLD 48

#else
   #define CP_KARATSUBA_MUL_THRESHOLD 12
   #define CP_KARATSUBA_SQR_THRESHOLD 16
#endif


cpSize cpKaratsubaBufferSize(cpSize len);

BNU_CHUNK_T cpMul_BNU_karatsuba(BNU_CHUNK_T* pR,
                          const BNU_CHUNK_T* pX, const BNU_CHUNK_T* pY, cpSize ns,
                                BNU_CHUNK_T* pBuffer);
BNU_CHUNK_T cpSqr_BNU_karatsuba(BNU_CHUNK_T* pR,
                          const BNU_CHUNK_T* pX, cpSize ns,
                                BNU_CHUNK_T* pBuffer);


#else
   #define CP_KARATSUBA_MUL_THRESHOLD 0
   #define CP_KARATSUBA_SQR_THRESHOLD 0
#endif /* _USE_KARATSUBA_ */

#endif /* _KARATSUBA_MUL_ */
