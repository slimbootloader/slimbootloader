/** @file

  Copyright (c) 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

/*
//
//  Purpose:
//     Cryptography Primitive.
//     SHA512 message digest
//
//  Contents:
//     SHA512 stuff
//
//
*/

#include "owndefs.h"
#include "owncp.h"
#include "pcphash.h"
#include "pcphash_rmf.h"
#include "pcptool.h"


/* SHA-512, SHA-384, SHA512-224, SHA512 constants */
static const Ipp64u sha512_iv[] = {
   CONST_64(0x6A09E667F3BCC908), CONST_64(0xBB67AE8584CAA73B),
   CONST_64(0x3C6EF372FE94F82B), CONST_64(0xA54FF53A5F1D36F1),
   CONST_64(0x510E527FADE682D1), CONST_64(0x9B05688C2B3E6C1F),
   CONST_64(0x1F83D9ABFB41BD6B), CONST_64(0x5BE0CD19137E2179)};
static const Ipp64u sha512_384_iv[] = {
   CONST_64(0xCBBB9D5DC1059ED8), CONST_64(0x629A292A367CD507),
   CONST_64(0x9159015A3070DD17), CONST_64(0x152FECD8F70E5939),
   CONST_64(0x67332667FFC00B31), CONST_64(0x8EB44A8768581511),
   CONST_64(0xDB0C2E0D64F98FA7), CONST_64(0x47B5481DBEFA4FA4)};

static __ALIGN16 const Ipp64u sha512_cnt[] = {
   CONST_64(0x428A2F98D728AE22), CONST_64(0x7137449123EF65CD), CONST_64(0xB5C0FBCFEC4D3B2F), CONST_64(0xE9B5DBA58189DBBC),
   CONST_64(0x3956C25BF348B538), CONST_64(0x59F111F1B605D019), CONST_64(0x923F82A4AF194F9B), CONST_64(0xAB1C5ED5DA6D8118),
   CONST_64(0xD807AA98A3030242), CONST_64(0x12835B0145706FBE), CONST_64(0x243185BE4EE4B28C), CONST_64(0x550C7DC3D5FFB4E2),
   CONST_64(0x72BE5D74F27B896F), CONST_64(0x80DEB1FE3B1696B1), CONST_64(0x9BDC06A725C71235), CONST_64(0xC19BF174CF692694),
   CONST_64(0xE49B69C19EF14AD2), CONST_64(0xEFBE4786384F25E3), CONST_64(0x0FC19DC68B8CD5B5), CONST_64(0x240CA1CC77AC9C65),
   CONST_64(0x2DE92C6F592B0275), CONST_64(0x4A7484AA6EA6E483), CONST_64(0x5CB0A9DCBD41FBD4), CONST_64(0x76F988DA831153B5),
   CONST_64(0x983E5152EE66DFAB), CONST_64(0xA831C66D2DB43210), CONST_64(0xB00327C898FB213F), CONST_64(0xBF597FC7BEEF0EE4),
   CONST_64(0xC6E00BF33DA88FC2), CONST_64(0xD5A79147930AA725), CONST_64(0x06CA6351E003826F), CONST_64(0x142929670A0E6E70),
   CONST_64(0x27B70A8546D22FFC), CONST_64(0x2E1B21385C26C926), CONST_64(0x4D2C6DFC5AC42AED), CONST_64(0x53380D139D95B3DF),
   CONST_64(0x650A73548BAF63DE), CONST_64(0x766A0ABB3C77B2A8), CONST_64(0x81C2C92E47EDAEE6), CONST_64(0x92722C851482353B),
   CONST_64(0xA2BFE8A14CF10364), CONST_64(0xA81A664BBC423001), CONST_64(0xC24B8B70D0F89791), CONST_64(0xC76C51A30654BE30),
   CONST_64(0xD192E819D6EF5218), CONST_64(0xD69906245565A910), CONST_64(0xF40E35855771202A), CONST_64(0x106AA07032BBD1B8),
   CONST_64(0x19A4C116B8D2D0C8), CONST_64(0x1E376C085141AB53), CONST_64(0x2748774CDF8EEB99), CONST_64(0x34B0BCB5E19B48A8),
   CONST_64(0x391C0CB3C5C95A63), CONST_64(0x4ED8AA4AE3418ACB), CONST_64(0x5B9CCA4F7763E373), CONST_64(0x682E6FF3D6B2B8A3),
   CONST_64(0x748F82EE5DEFB2FC), CONST_64(0x78A5636F43172F60), CONST_64(0x84C87814A1F0AB72), CONST_64(0x8CC702081A6439EC),
   CONST_64(0x90BEFFFA23631E28), CONST_64(0xA4506CEBDE82BDE9), CONST_64(0xBEF9A3F7B2C67915), CONST_64(0xC67178F2E372532B),
   CONST_64(0xCA273ECEEA26619C), CONST_64(0xD186B8C721C0C207), CONST_64(0xEADA7DD6CDE0EB1E), CONST_64(0xF57D4F7FEE6ED178),
   CONST_64(0x06F067AA72176FBA), CONST_64(0x0A637DC5A2C898A6), CONST_64(0x113F9804BEF90DAE), CONST_64(0x1B710B35131C471B),
   CONST_64(0x28DB77F523047D84), CONST_64(0x32CAAB7B40C72493), CONST_64(0x3C9EBE0A15C9BEBC), CONST_64(0x431D67C49C100D4C),
   CONST_64(0x4CC5D4BECB3E42B6), CONST_64(0x597F299CFC657E2A), CONST_64(0x5FCB6FAB3AD6FAEC), CONST_64(0x6C44198C4A475817)
};

/* setup init hash value */
__INLINE void hashInit(Ipp64u* pHash, const Ipp64u* iv)
{
   pHash[0] = iv[0];
   pHash[1] = iv[1];
   pHash[2] = iv[2];
   pHash[3] = iv[3];
   pHash[4] = iv[4];
   pHash[5] = iv[5];
   pHash[6] = iv[6];
   pHash[7] = iv[7];
}
static void sha512_hashInit(void* pHash)
{
   hashInit((Ipp64u*)pHash, sha512_iv);
}
static void sha512_384_hashInit(void* pHash)
{
   hashInit((Ipp64u*)pHash, sha512_384_iv);
}

static void sha512_hashUpdate(void* pHash, const Ipp8u* pMsg, int msgLen)
{
   UpdateSHA512 (pHash, pMsg, msgLen, sha512_cnt);
}

/* convert hash into big endian */
static void sha512_hashOctString(Ipp8u* pMD, void* pHashVal)
{
   ((Ipp64u*)pMD)[0] = ENDIANNESS64(((Ipp64u*)pHashVal)[0]);
   ((Ipp64u*)pMD)[1] = ENDIANNESS64(((Ipp64u*)pHashVal)[1]);
   ((Ipp64u*)pMD)[2] = ENDIANNESS64(((Ipp64u*)pHashVal)[2]);
   ((Ipp64u*)pMD)[3] = ENDIANNESS64(((Ipp64u*)pHashVal)[3]);
   ((Ipp64u*)pMD)[4] = ENDIANNESS64(((Ipp64u*)pHashVal)[4]);
   ((Ipp64u*)pMD)[5] = ENDIANNESS64(((Ipp64u*)pHashVal)[5]);
   ((Ipp64u*)pMD)[6] = ENDIANNESS64(((Ipp64u*)pHashVal)[6]);
   ((Ipp64u*)pMD)[7] = ENDIANNESS64(((Ipp64u*)pHashVal)[7]);
}
static void sha512_384_hashOctString(Ipp8u* pMD, void* pHashVal)
{
   ((Ipp64u*)pMD)[0] = ENDIANNESS64(((Ipp64u*)pHashVal)[0]);
   ((Ipp64u*)pMD)[1] = ENDIANNESS64(((Ipp64u*)pHashVal)[1]);
   ((Ipp64u*)pMD)[2] = ENDIANNESS64(((Ipp64u*)pHashVal)[2]);
   ((Ipp64u*)pMD)[3] = ENDIANNESS64(((Ipp64u*)pHashVal)[3]);
   ((Ipp64u*)pMD)[4] = ENDIANNESS64(((Ipp64u*)pHashVal)[4]);
   ((Ipp64u*)pMD)[5] = ENDIANNESS64(((Ipp64u*)pHashVal)[5]);
}

static void sha512_msgRep(Ipp8u* pDst, Ipp64u lenLo, Ipp64u lenHi)
{
   lenHi = LSL64(lenHi,3) | LSR64(lenLo,63-3);
   lenLo = LSL64(lenLo,3);
   ((Ipp64u*)(pDst))[0] = ENDIANNESS64(lenHi);
   ((Ipp64u*)(pDst))[1] = ENDIANNESS64(lenLo);
}

//#define   cpFinalizeSHA512       OWNAPI(cpFinalizeSHA512)
//void      cpFinalizeSHA512(DigestSHA512 pHash, const Ipp8u* inpBuffer, int inpLen, Ipp64u lenLo, Ipp64u lenHi);
#define   cpSHA512MessageDigest  OWNAPI(cpSHA512MessageDigest)
IppStatus cpSHA512MessageDigest(DigestSHA512 hash, const Ipp8u* pMsg, int msgLen, const DigestSHA512 IV);
#define   InitSHA512             OWNAPI(InitSHA512)
IppStatus InitSHA512(IppsSHA512State* pState, const DigestSHA512 IV);

static void cpFinalizeSHA512(DigestSHA512 pHash,
                       const Ipp8u* inpBuffer, int inpLen,
                             Ipp64u lenLo, Ipp64u lenHi)
{
   /* local buffer and it length */
   Ipp8u buffer[MBS_SHA512*2];
   int bufferLen = inpLen < (MBS_SHA512-(int)MLR_SHA512)? MBS_SHA512 : MBS_SHA512*2;

   /* copy rest of message into internal buffer */
   CopyBlock(inpBuffer, buffer, inpLen);

   /* padd message */
   buffer[inpLen++] = 0x80;
   PadBlock(0, buffer+inpLen, bufferLen-inpLen-MLR_SHA512);

   /* message length representation */
   lenHi = LSL64(lenHi,3) | LSR64(lenLo,63-3);
   lenLo = LSL64(lenLo,3);
   ((Ipp64u*)(buffer+bufferLen))[-2] = ENDIANNESS64(lenHi);
   ((Ipp64u*)(buffer+bufferLen))[-1] = ENDIANNESS64(lenLo);

   /* copmplete hash computation */
   UpdateSHA512 (pHash, buffer, bufferLen, sha512_cnt);
}

// #endif /* #if !defined(_PCP_SHA512_STUFF_H) */



IppStatus InitSHA512(IppsSHA512State* pState, const DigestSHA512 IV)
{
   /* test state pointer */
   IPP_BAD_PTR1_RET(pState);
   pState = (IppsSHA512State*)( IPP_ALIGNED_PTR(pState, SHA512_ALIGNMENT) );

   /* set state ID */
   HASH_CTX_ID(pState) = idCtxSHA512;
   /* zeros message length */
   HASH_LENLO(pState) = 0;
   HASH_LENHI(pState) = 0;
   /* message buffer is free */
   HAHS_BUFFIDX(pState) = 0;
   /* setup initial digest */
   hashInit(HASH_VALUE(pState), IV);

   return ippStsNoErr;
}


/*F*
//    Name: ippsSHA384Final
//
// Purpose: Stop message digesting and return digest.
//
// Returns:                Reason:
//    ippStsNullPtrErr        pMD == NULL
//                            pState == NULL
//    ippStsContextMatchErr   pState->idCtx != idCtxSHA512
//    ippStsNoErr             no errors
//
// Parameters:
//    pMD         address of the output digest
//    pState      pointer to the SHA384 state
//
*F*/

IPPFUN(IppStatus, ippsSHA384Final,(Ipp8u* pMD, IppsSHA384State* pState))
{
   /* test state pointer and ID */
   IPP_BAD_PTR1_RET(pState);
   pState = (IppsSHA512State*)( IPP_ALIGNED_PTR(pState, SHA512_ALIGNMENT) );
   IPP_BADARG_RET(idCtxSHA512 !=HASH_CTX_ID(pState), ippStsContextMatchErr);

   /* test digest pointer */
   IPP_BAD_PTR1_RET(pMD);

   cpFinalizeSHA512(HASH_VALUE(pState),
                    HASH_BUFF(pState), HAHS_BUFFIDX(pState),
                    HASH_LENLO(pState), HASH_LENHI(pState));
   /* convert hash into big endian */
   ((Ipp64u*)pMD)[0] = ENDIANNESS64(HASH_VALUE(pState)[0]);
   ((Ipp64u*)pMD)[1] = ENDIANNESS64(HASH_VALUE(pState)[1]);
   ((Ipp64u*)pMD)[2] = ENDIANNESS64(HASH_VALUE(pState)[2]);
   ((Ipp64u*)pMD)[3] = ENDIANNESS64(HASH_VALUE(pState)[3]);
   ((Ipp64u*)pMD)[4] = ENDIANNESS64(HASH_VALUE(pState)[4]);
   ((Ipp64u*)pMD)[5] = ENDIANNESS64(HASH_VALUE(pState)[5]);

   /* re-init hash value */
   HAHS_BUFFIDX(pState) = 0;
   HASH_LENLO(pState) = 0;
   HASH_LENHI(pState) = 0;
   sha512_384_hashInit(HASH_VALUE(pState));

   return ippStsNoErr;
}


/*F*
//    Name: ippsSHA512Update
//
// Purpose: Updates intermadiate digest based on input stream.
//
// Returns:                Reason:
//    ippStsNullPtrErr        pSrc == NULL
//                            pState == NULL
//    ippStsContextMatchErr   pState->idCtx != idCtxSHA512
//    ippStsLengthErr         len <0
//    ippStsNoErr             no errors
//
// Parameters:
//    pSrc        pointer to the input stream
//    len         input stream length
//    pState      pointer to the SHA512 state
//
*F*/
IPPFUN(IppStatus, ippsSHA512Update,(const Ipp8u* pSrc, int len, IppsSHA512State* pState))
{
   /* test state pointer and ID */
   IPP_BAD_PTR1_RET(pState);
   /* use aligned context */
   pState = (IppsSHA512State*)( IPP_ALIGNED_PTR(pState, SHA512_ALIGNMENT) );
   IPP_BADARG_RET(idCtxSHA512 !=HASH_CTX_ID(pState), ippStsContextMatchErr);

   /* test input length */
   IPP_BADARG_RET((len<0), ippStsLengthErr);
   /* test source pointer */
   IPP_BADARG_RET((len && !pSrc), ippStsNullPtrErr);

   /*
   // handle non empty message
   */
   if(len) {
      int procLen;

      int idx = HAHS_BUFFIDX(pState);
      Ipp8u* pBuffer = HASH_BUFF(pState);
      Ipp64u lenLo = HASH_LENLO(pState) +len;
      Ipp64u lenHi = HASH_LENHI(pState);
      if(lenLo < HASH_LENLO(pState)) lenHi++;

      /* if non empty internal buffer filling */
      if(idx) {
         /* copy from input stream to the internal buffer as match as possible */
         procLen = IPP_MIN(len, (MBS_SHA512-idx));
         CopyBlock(pSrc, pBuffer+idx, procLen);

         /* update message pointer and length */
         pSrc += procLen;
         len  -= procLen;
         idx  += procLen;

         /* update digest if buffer full */
         if(MBS_SHA512 == idx) {
            UpdateSHA512(HASH_VALUE(pState), pBuffer, MBS_SHA512, sha512_cnt);
            idx = 0;
         }
      }

      /* main message part processing */
      procLen = len & ~(MBS_SHA512-1);
      if(procLen) {
         UpdateSHA512(HASH_VALUE(pState), pSrc, procLen, sha512_cnt);
         pSrc += procLen;
         len  -= procLen;
      }

      /* store rest of message into the internal buffer */
      if(len) {
         CopyBlock(pSrc, pBuffer, len);
         idx += len;
      }

      /* update length of processed message */
      HASH_LENLO(pState) = lenLo;
      HASH_LENHI(pState) = lenHi;
      HAHS_BUFFIDX(pState) = idx;
   }

   return ippStsNoErr;
}

/*F*
//    Name: ippsSHA384Init
//
// Purpose: Init SHA384
//
// Returns:                Reason:
//    ippStsNullPtrErr        pState == NULL
//    ippStsNoErr             no errors
//
// Parameters:
//    pState      pointer to the SHA384 state
//
*F*/

IPPFUN(IppStatus, ippsSHA384Init,(IppsSHA384State* pState))
{
   return InitSHA512(pState, sha512_384_iv);
}

/*F*
//    Name: ippsSHA384Update
//
// Purpose: Updates intermadiate digest based on input stream.
//
// Returns:                Reason:
//    ippStsNullPtrErr        pSrc == NULL
//                            pState == NULL
//    ippStsContextMatchErr   pState->idCtx != idCtxSHA512
//    ippStsLengthErr         len <0
//    ippStsNoErr             no errors
//
// Parameters:
//    pSrc        pointer to the input stream
//    len         input stream length
//    pState      pointer to the SHA384 state
//
*F*/

IPPFUN(IppStatus, ippsSHA384Update,(const Ipp8u* pSrc, int len, IppsSHA384State* pState))
{
   return ippsSHA512Update(pSrc, len, pState);
}


/*
// available SHA384 methods
*/
IPPFUN( const IppsHashMethod*, ippsHashMethod_SHA384, (void) )
{
   static IppsHashMethod method = {
      ippHashAlg_SHA384,
      IPP_SHA384_DIGEST_BITSIZE/8,
      MBS_SHA384,
      MLR_SHA384,
      sha512_384_hashInit,
      sha512_hashUpdate,
      sha512_384_hashOctString,
      sha512_msgRep
   };
   return &method;
}

/*
// available SHA512 methods
*/
IPPFUN( const IppsHashMethod*, ippsHashMethod_SHA512, (void) )
{
   static IppsHashMethod method = {
      ippHashAlg_SHA512,
      IPP_SHA384_DIGEST_BITSIZE/8,
      MBS_SHA512,
      MLR_SHA512,
      sha512_hashInit,
      sha512_hashUpdate,
      sha512_hashOctString,
      sha512_msgRep
   };
   return &method;
}

#pragma message("IPP_ALG_HASH_SHA512 enabled")

#if !((_IPP==_IPP_W7) || (_IPP==_IPP_T7) || \
      (_IPP==_IPP_V8) || (_IPP==_IPP_P8) || \
      (_IPP==_IPP_S8) || (_IPP>=_IPP_G9) || \
      (_IPP32E==_IPP32E_M7) || \
      (_IPP32E==_IPP32E_U8) || (_IPP32E==_IPP32E_Y8) || \
      (_IPP32E==_IPP32E_N8) || (_IPP32E>=_IPP32E_E9))
/*
// SHA512 Specific Macros (reference proposal 256-384-512)
//
// Note: All operations act on DWORDs (64-bits)
*/
#define CH(x,y,z)    (((x) & (y)) ^ (~(x) & (z)))
#define MAJ(x,y,z)   (((x) & (y)) ^ ((x) & (z)) ^ ((y) & (z)))

#define SUM0(x)   (ROR64((x),28) ^ ROR64((x),34) ^ ROR64((x),39))
#define SUM1(x)   (ROR64((x),14) ^ ROR64((x),18) ^ ROR64((x),41))

#define SIG0(x)   (ROR64((x), 1) ^ ROR64((x), 8) ^ LSR64((x), 7))
#define SIG1(x)   (ROR64((x),19) ^ ROR64((x),61) ^ LSR64((x), 6))

#define SHA512_UPDATE(i) \
   wdat[i&15] += SIG1(wdat[(i+14)&15]) + wdat[(i+9)&15] + SIG0(wdat[(i+1)&15])

#define SHA512_STEP(i,j)  \
    v[(7-i)&7] += (j ? SHA512_UPDATE(i) : wdat[i&15])    \
               + SHA512_cnt_loc[i+j]                         \
               + SUM1(v[(4-i)&7])                        \
               + CH(v[(4-i)&7], v[(5-i)&7], v[(6-i)&7]); \
    v[(3-i)&7] += v[(7-i)&7];                            \
    v[(7-i)&7] += SUM0(v[(0-i)&7]) + MAJ(v[(0-i)&7], v[(1-i)&7], v[(2-i)&7])

#define COMPACT_SHA512_STEP(A,B,C,D,E,F,G,H, W,K, r) { \
   Ipp64u _T1 = (H) + SUM1((E)) + CH((E),(F),(G)) + (W)[(r)] + (K)[(r)]; \
   Ipp64u _T2 = SUM0((A)) + MAJ((A),(B),(C)); \
   (H) = (G); \
   (G) = (F); \
   (F) = (E); \
   (E) = (D)+_T1; \
   (D) = (C); \
   (C) = (B); \
   (B) = (A); \
   (A) = _T1+_T2; \
}


#if defined(_ALG_SHA512_COMPACT_)
#pragma message("SHA512 compact")

void UpdateSHA512Compact(void* uniHash, const Ipp8u* mblk, int mlen, const void* uniPraram)
{
   Ipp32u* data = (Ipp32u*)mblk;

   Ipp64u* digest = (Ipp64u*)uniHash;
   Ipp64u* SHA512_cnt_loc = (Ipp64u*)uniPraram;

   for(; mlen>=MBS_SHA512; data += MBS_SHA512/sizeof(Ipp32u), mlen -= MBS_SHA512) {
      int t;
      Ipp64u W[80];

      /*
      // expand message block
      */
      /* initialize the first 16 words in the array W (remember about endian) */
      for(t=0; t<16; t++) {
         Ipp32u hiX = data[2*t];
         Ipp32u loX = data[2*t+1];
         #if (IPP_ENDIAN == IPP_BIG_ENDIAN)
         W[t] = IPP_MAKEDWORD(loX, hiX);
         #else
         W[t] = IPP_MAKEDWORD( ENDIANNESS(loX), ENDIANNESS(hiX) );
         #endif
      }
      for(; t<80; t++)
         W[t] = SIG1(W[t-2]) + W[t-7] + SIG0(W[t-15]) + W[t-16];

      /*
      // update hash
      */
      {
         /* init A, B, C, D, E, F, G, H by the input hash */
         Ipp64u A = digest[0];
         Ipp64u B = digest[1];
         Ipp64u C = digest[2];
         Ipp64u D = digest[3];
         Ipp64u E = digest[4];
         Ipp64u F = digest[5];
         Ipp64u G = digest[6];
         Ipp64u H = digest[7];

         for(t=0; t<80; t++)
            COMPACT_SHA512_STEP(A,B,C,D,E,F,G,H, W,SHA512_cnt_loc, t);

         /* update hash*/
         digest[0] += A;
         digest[1] += B;
         digest[2] += C;
         digest[3] += D;
         digest[4] += E;
         digest[5] += F;
         digest[6] += G;
         digest[7] += H;
      }
   }
}
#endif

void UpdateSHA512Normal(void* uniHash, const Ipp8u* mblk, int mlen, const void* uniPraram)
{
   Ipp32u* data = (Ipp32u*)mblk;

   Ipp64u* digest = (Ipp64u*)uniHash;
   Ipp64u* SHA512_cnt_loc = (Ipp64u*)uniPraram;

   for(; mlen>=MBS_SHA512; data += MBS_SHA512/sizeof(Ipp32u), mlen -= MBS_SHA512) {
      Ipp64u wdat[16];
      int j;

      Ipp64u v[8];

      /* initialize the first 16 words in the array W (remember about endian) */
      for(j=0; j<16; j++) {
         Ipp32u hiX = data[2*j];
         Ipp32u loX = data[2*j+1];
         #if (IPP_ENDIAN == IPP_BIG_ENDIAN)
         wdat[j] = IPP_MAKEDWORD(loX, hiX);
         #else
         wdat[j] = IPP_MAKEDWORD( ENDIANNESS(loX), ENDIANNESS(hiX) );
         #endif
      }

      /* copy digest */
      CopyBlock(digest, v, IPP_SHA512_DIGEST_BITSIZE/BYTESIZE);

      for(j=0; j<80; j+=16) {
         SHA512_STEP( 0, j);
         SHA512_STEP( 1, j);
         SHA512_STEP( 2, j);
         SHA512_STEP( 3, j);
         SHA512_STEP( 4, j);
         SHA512_STEP( 5, j);
         SHA512_STEP( 6, j);
         SHA512_STEP( 7, j);
         SHA512_STEP( 8, j);
         SHA512_STEP( 9, j);
         SHA512_STEP(10, j);
         SHA512_STEP(11, j);
         SHA512_STEP(12, j);
         SHA512_STEP(13, j);
         SHA512_STEP(14, j);
         SHA512_STEP(15, j);
      }

      /* update digest */
      digest[0] += v[0];
      digest[1] += v[1];
      digest[2] += v[2];
      digest[3] += v[3];
      digest[4] += v[4];
      digest[5] += v[5];
      digest[6] += v[6];
      digest[7] += v[7];
   }
}

/*F*
//    Name: UpdateSHA512
//
// Purpose: Update internal hash according to input message stream.
//
// Parameters:
//    uniHash  pointer to in/out hash
//    mblk     pointer to message stream
//    mlen     message stream length (multiple by message block size)
//    uniParam pointer to the optional parameter
//
*F*/

void UpdateSHA512(void* uniHash, const Ipp8u* mblk, int mlen, const void* uniPraram)
{
#if defined(_SLIMBOOT_OPT)
   #if (FixedPcdGet32 (PcdCryptoShaOptMask) & IPP_CRYPTO_SHA384_G9)
      UpdateSHA512G9 (uniHash, mblk, mlen, uniPraram);
   #elif (FixedPcdGet32 (PcdCryptoShaOptMask) & IPP_CRYPTO_SHA384_W7)
      UpdateSHA512W7 (uniHash, mblk, mlen, uniPraram);
   #else
      UpdateSHA512Compact (uniHash, mblk, mlen, uniPraram);
   #endif
#else

#if  defined(_ALG_SHA512_COMPACT_)
  UpdateSHA512Compact (uniHash, mblk, mlen, uniPraram);
#else
  UpdateSHA512Normal (uniHash, mblk, mlen, uniPraram);
#endif

#endif //_SLIMBOOT_OPT
}

#endif
