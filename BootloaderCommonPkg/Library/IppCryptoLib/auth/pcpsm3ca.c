/*******************************************************************************
* Copyright 2013-2020 Intel Corporation
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*******************************************************************************/

/*
//
//  Purpose:
//     Cryptography Primitive.
//     Digesting message according to SM3
//
//  Contents:
//     ippsSM3Init()
//     ippsSM3Update()
//     ippsSM3Final()
//
//
*/

#include "owndefs.h"
#include "owncp.h"
#include "pcphash.h"
#include "pcphash_rmf.h"
#include "pcptool.h"


/* SM3 constants */
static const Ipp32u sm3_iv[] = {
   0x7380166F, 0x4914B2B9, 0x172442D7, 0xDA8A0600,
   0xA96F30BC, 0x163138AA, 0xE38DEE4D, 0xB0FB0E4E};

static __ALIGN16 const Ipp32u sm3_cnt[] = {
   0x79CC4519,0xF3988A32,0xE7311465,0xCE6228CB,0x9CC45197,0x3988A32F,0x7311465E,0xE6228CBC,
   0xCC451979,0x988A32F3,0x311465E7,0x6228CBCE,0xC451979C,0x88A32F39,0x11465E73,0x228CBCE6,
   0x9D8A7A87,0x3B14F50F,0x7629EA1E,0xEC53D43C,0xD8A7A879,0xB14F50F3,0x629EA1E7,0xC53D43CE,
   0x8A7A879D,0x14F50F3B,0x29EA1E76,0x53D43CEC,0xA7A879D8,0x4F50F3B1,0x9EA1E762,0x3D43CEC5,
   0x7A879D8A,0xF50F3B14,0xEA1E7629,0xD43CEC53,0xA879D8A7,0x50F3B14F,0xA1E7629E,0x43CEC53D,
   0x879D8A7A,0x0F3B14F5,0x1E7629EA,0x3CEC53D4,0x79D8A7A8,0xF3B14F50,0xE7629EA1,0xCEC53D43,
   0x9D8A7A87,0x3B14F50F,0x7629EA1E,0xEC53D43C,0xD8A7A879,0xB14F50F3,0x629EA1E7,0xC53D43CE,
   0x8A7A879D,0x14F50F3B,0x29EA1E76,0x53D43CEC,0xA7A879D8,0x4F50F3B1,0x9EA1E762,0x3D43CEC5
};

static void sm3_hashInit(void* pHash)
{
   /* setup initial digest */
   ((Ipp32u*)pHash)[0] = sm3_iv[0];
   ((Ipp32u*)pHash)[1] = sm3_iv[1];
   ((Ipp32u*)pHash)[2] = sm3_iv[2];
   ((Ipp32u*)pHash)[3] = sm3_iv[3];
   ((Ipp32u*)pHash)[4] = sm3_iv[4];
   ((Ipp32u*)pHash)[5] = sm3_iv[5];
   ((Ipp32u*)pHash)[6] = sm3_iv[6];
   ((Ipp32u*)pHash)[7] = sm3_iv[7];
}

static void sm3_hashUpdate(void* pHash, const Ipp8u* pMsg, int msgLen)
{
   UpdateSM3(pHash, pMsg, msgLen, sm3_cnt);
}

static void sm3_hashOctString(Ipp8u* pMD, void* pHashVal)
{
   /* convert hash into big endian */
   ((Ipp32u*)pMD)[0] = ENDIANNESS32(((Ipp32u*)pHashVal)[0]);
   ((Ipp32u*)pMD)[1] = ENDIANNESS32(((Ipp32u*)pHashVal)[1]);
   ((Ipp32u*)pMD)[2] = ENDIANNESS32(((Ipp32u*)pHashVal)[2]);
   ((Ipp32u*)pMD)[3] = ENDIANNESS32(((Ipp32u*)pHashVal)[3]);
   ((Ipp32u*)pMD)[4] = ENDIANNESS32(((Ipp32u*)pHashVal)[4]);
   ((Ipp32u*)pMD)[5] = ENDIANNESS32(((Ipp32u*)pHashVal)[5]);
   ((Ipp32u*)pMD)[6] = ENDIANNESS32(((Ipp32u*)pHashVal)[6]);
   ((Ipp32u*)pMD)[7] = ENDIANNESS32(((Ipp32u*)pHashVal)[7]);
}

static void sm3_msgRep(Ipp8u* pDst, Ipp64u lenLo, Ipp64u lenHi)
{
   IPP_UNREFERENCED_PARAMETER(lenHi);
#ifdef _SLIMBOOT_OPT
   lenLo = ENDIANNESS64(LShiftU64 (lenLo, 3));
#else
   lenLo = ENDIANNESS64(lenLo<<3);
#endif
   ((Ipp64u*)(pDst))[0] = lenLo;
}

#define cpFinalizeSM3 OWNAPI(cpFinalizeSM3)
void cpFinalizeSM3(DigestSHA1 pHash, const Ipp8u* inpBuffer, int inpLen, Ipp64u processedMsgLen);

//#endif /* #if !defined _PCP_SM3_STUFF_H */



void cpFinalizeSM3(DigestSHA1 pHash, const Ipp8u* inpBuffer, int inpLen, Ipp64u processedMsgLen)
{
   /* local buffer and it length */
   Ipp8u buffer[MBS_SM3*2];
   int bufferLen = inpLen < (MBS_SM3-(int)MLR_SM3)? MBS_SM3 : MBS_SM3*2;

   /* copy rest of message into internal buffer */
   CopyBlock(inpBuffer, buffer, inpLen);

   /* padd message */
   buffer[inpLen++] = 0x80;
   PadBlock(0, buffer+inpLen, (cpSize)(bufferLen-inpLen-(int)MLR_SM3));

   /* put processed message length in bits */
#ifdef _SLIMBOOT_OPT
   processedMsgLen = ENDIANNESS64(LShiftU64 (processedMsgLen, 3));
#else
   processedMsgLen = ENDIANNESS64(processedMsgLen<<3);
#endif
   ((Ipp64u*)(buffer+bufferLen))[-1] = processedMsgLen;

   /* copmplete hash computation */
   UpdateSM3(pHash, buffer, bufferLen, sm3_cnt);
}


/*F*
//    Name: ippsSM3Init
//
// Purpose: Init SM3
//
// Returns:                Reason:
//    ippStsNullPtrErr        pState == NULL
//    ippStsNoErr             no errors
//
// Parameters:
//    pState      pointer to the SHA512 state
//
*F*/
IPPFUN(IppStatus, ippsSM3Init,(IppsSM3State* pState))
{
   /* test state pointer */
   IPP_BAD_PTR1_RET(pState);
   pState = (IppsSM3State*)( IPP_ALIGNED_PTR(pState, SM3_ALIGNMENT) );

   PadBlock(0, pState, sizeof(IppsSM3State));
   HASH_CTX_ID(pState) = idCtxSM3;
   sm3_hashInit(HASH_VALUE(pState));
   return ippStsNoErr;
}

/*F*
//    Name: ippsSM3Update
//
// Purpose: Updates intermediate digest based on input stream.
//
// Returns:                Reason:
//    ippStsNullPtrErr        pSrc == NULL
//                            pState == NULL
//    ippStsContextMatchErr   pState->idCtx != idCtxSM3
//    ippStsLengthErr         len <0
//    ippStsNoErr             no errors
//
// Parameters:
//    pSrc        pointer to the input stream
//    len         input stream length
//    pState      pointer to the SM3 state
//
*F*/
IPPFUN(IppStatus, ippsSM3Update,(const Ipp8u* pSrc, int len, IppsSM3State* pState))
{
   /* test state pointer and ID */
   IPP_BAD_PTR1_RET(pState);
   pState = (IppsSM3State*)( IPP_ALIGNED_PTR(pState, SM3_ALIGNMENT) );
   IPP_BADARG_RET(idCtxSM3 !=HASH_CTX_ID(pState), ippStsContextMatchErr);

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

      /* if non empty internal buffer filling */
      if(idx) {
         /* copy from input stream to the internal buffer as match as possible */
         procLen = IPP_MIN(len, (MBS_SM3-idx));
         CopyBlock(pSrc, pBuffer+idx, procLen);

         /* update message pointer and length */
         idx  += procLen;
         pSrc += procLen;
         len  -= procLen;

         /* update digest if buffer full */
         if( MBS_SM3 == idx) {
            UpdateSM3(HASH_VALUE(pState), pBuffer, MBS_SM3, sm3_cnt);
            idx = 0;
         }
      }

      /* main message part processing */
      procLen = len & ~(MBS_SM3-1);
      if(procLen) {
         UpdateSM3(HASH_VALUE(pState), pSrc, procLen, sm3_cnt);
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
      HAHS_BUFFIDX(pState) = idx;
   }

   return ippStsNoErr;
}

/*F*
//    Name: ippsSM3Final
//
// Purpose: Stop message digesting and return digest.
//
// Returns:                Reason:
//    ippStsNullPtrErr        pState == NULL
//                            pMD == NULL
//    ippStsContextMatchErr   pState->idCtx != idCtxSM3
//    ippStsNoErr             no errors
//
// Parameters:
//    pMD         address of the output digest
//    pState      pointer to the SM3 state
//
*F*/
IPPFUN(IppStatus, ippsSM3Final,(Ipp8u* pMD, IppsSM3State* pState))
{
   /* test state pointer and ID */
   IPP_BAD_PTR1_RET(pState);
   pState = (IppsSM3State*)( IPP_ALIGNED_PTR(pState, SM3_ALIGNMENT) );
   IPP_BADARG_RET(idCtxSM3 !=HASH_CTX_ID(pState), ippStsContextMatchErr);

   /* test digest pointer */
   IPP_BAD_PTR1_RET(pMD);

   cpFinalizeSM3(HASH_VALUE(pState), HASH_BUFF(pState), HAHS_BUFFIDX(pState), HASH_LENLO(pState));
   /* convert hash into big endian */
   ((Ipp32u*)pMD)[0] = ENDIANNESS32(HASH_VALUE(pState)[0]);
   ((Ipp32u*)pMD)[1] = ENDIANNESS32(HASH_VALUE(pState)[1]);
   ((Ipp32u*)pMD)[2] = ENDIANNESS32(HASH_VALUE(pState)[2]);
   ((Ipp32u*)pMD)[3] = ENDIANNESS32(HASH_VALUE(pState)[3]);
   ((Ipp32u*)pMD)[4] = ENDIANNESS32(HASH_VALUE(pState)[4]);
   ((Ipp32u*)pMD)[5] = ENDIANNESS32(HASH_VALUE(pState)[5]);
   ((Ipp32u*)pMD)[6] = ENDIANNESS32(HASH_VALUE(pState)[6]);
   ((Ipp32u*)pMD)[7] = ENDIANNESS32(HASH_VALUE(pState)[7]);

   /* re-init hash value */
   HAHS_BUFFIDX(pState) = 0;
   HASH_LENLO(pState) = 0;
   sm3_hashInit(HASH_VALUE(pState));

   return ippStsNoErr;
}

/*
// available SM3 methods
*/
IPPFUN( const IppsHashMethod*, ippsHashMethod_SM3, (void) )
{
   static IppsHashMethod method = {
      ippHashAlg_SM3,
      IPP_SM3_DIGEST_BITSIZE/8,
      MBS_SM3,
      MLR_SM3,
      sm3_hashInit,
      sm3_hashUpdate,
      sm3_hashOctString,
      sm3_msgRep
   };
   return &method;
}



#pragma message("IPP_ALG_HASH_SM3 enabled")

#if !((_IPP32E>=_IPP32E_U8) || (_IPP32E==_IPP32E_N8) )

/*
// SM3 Specific Macros
// (reference SM3 Cryptographic Hash Algorithm,
//  Chinese Commercial Cryptography Administration Office, 2010.12)
*/

/* T1 and T2 are base for additive const generation */
#define T1  (0x79CC4519)
#define T2  (0x7A879D8A)

// boolean functions (0<=nr<16)
#define FF1(x,y,z) ((x)^(y)^(z))
#define GG1(x,y,z) ((x)^(y)^(z))
// boolean functions (16<=nr<64)
#define FF2(x,y,z) (((x)&(y)) | ((x)&(z)) | ((y)&(z)))
#define GG2(x,y,z) (((x)&(y)) | (~(x)&(z)))

// P0 permutation:
#define P0(x)  ((x) ^ ROL32((x),9) ^ ROL32((x),17))
// P1 permutation:
#define P1(x)  ((x) ^ ROL32((x),15) ^ ROL32((x),23))

// update W
#define WUPDATE(nr, W) (P1(W[((nr)-16)&15] ^ W[((nr)-9)&15] ^ ROL32(W[((nr)-3)&15],15)) ^ ROL32(W[((nr)-13)&15],7) ^ W[((nr)-6)&15])

// SM3 steps
#define SM3_STEP1(nr, A,B,C,D,E,F,G,H, Tj, W)  { \
   TT1 = FF1(A,B,C) + D + (W[nr&15] ^ W[(nr+4)&15]); \
   TT2 = GG1(E,F,G) + H + W[nr&15]; \
   H = ROL32(A,12); \
   D = ROL32(H + E +Tj, 7); \
   H ^= D; \
   D += TT2; \
   H += TT1; \
   B = ROL32(B, 9); \
   D = P0(D); \
   F = ROL32(F, 19); \
   /*Tj = ROL32(Tj, 1);*/ \
   W[(nr)&15] = WUPDATE(nr, W); \
}

#define SM3_STEP2(nr, A,B,C,D,E,F,G,H, Tj, W)  { \
   TT1 = FF2(A,B,C) + D + (W[nr&15] ^ W[(nr+4)&15]); \
   TT2 = GG2(E,F,G) + H + W[nr&15]; \
   H = ROL32(A,12); \
   D = ROL32(H + E +Tj, 7); \
   H ^= D; \
   D += TT2; \
   H += TT1; \
   B = ROL32(B, 9); \
   D = P0(D); \
   F = ROL32(F, 19); \
   /*Tj = ROL32(Tj, 1);*/ \
   W[(nr)&15] = WUPDATE(nr, W); \
}

#define SM3_STEP3(nr, A,B,C,D,E,F,G,H, Tj, W)  { \
   TT1 = FF2(A,B,C) + D + (W[nr&15] ^ W[(nr+4)&15]); \
   TT2 = GG2(E,F,G) + H + W[nr&15]; \
   H = ROL32(A,12); \
   D = ROL32(H + E +Tj, 7); \
   H ^= D; \
   D += TT2; \
   H += TT1; \
   B = ROL32(B, 9); \
   D = P0(D); \
   F = ROL32(F, 19); \
   /*Tj = ROL32(Tj, 1);*/ \
}

#define COMPACT_SM3_STEP(A,B,C,D,E,F,G,H, FF, GG, W,Tj, r)  { \
   TT1 = FF((r)&0x30, A,B,C) + D + (W[(r)] ^ W[(r)+4]); \
   TT2 = GG((r)&0x30, E,F,G) + H + W[(r)]; \
   \
   _H = ROL32(A,12); \
   _D = ROL32(_H + E +Tj[(r)], 7); \
   _H ^= _D;   \
   _D += TT2;  \
   _H += TT1;  \
   _D = P0(_D);\
   \
   H = G; \
   G = ROL32(F,19); \
   F = E; \
   E =_D; \
   D = C; \
   C = ROL32(B, 9); \
   B = A; \
   A =_H; \
}


/*F*
//    Name: UpdateSM3
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
#if defined(_ALG_SM3_COMPACT_)
#pragma message("SM3 compact")

__INLINE Ipp32u MagicFF(int s, Ipp32u a, Ipp32u b, Ipp32u c)
{
   switch(s) {
      case 0: return FF1(a,b,c);
      default:return FF2(a,b,c);
   }
}
__INLINE Ipp32u MagicGG(int s, Ipp32u e, Ipp32u f, Ipp32u g)
{
   switch(s) {
      case 0: return GG1(e,f,g);
      default:return GG2(e,f,g);
   }
}

void UpdateSM3(void* uniHash, const Ipp8u* mblk, int mlen, const void* uniParam)
{
   Ipp32u* data = (Ipp32u*)mblk;

   Ipp32u* hash = (Ipp32u*)uniHash;
   Ipp32u* SM3_cnt_loc = (Ipp32u*)uniParam;

   for(; mlen>=MBS_SM3; data += MBS_SM3/sizeof(Ipp32u), mlen -= MBS_SM3) {
      int r;

      /*
      // expand message block
      */
      Ipp32u W[68];
      /* initialize the first 16 words in the array W (remember about endian) */
      for(r=0; r<16; r++) {
         #if (IPP_ENDIAN == IPP_BIG_ENDIAN)
         W[r] = data[r];
         #else
         W[r] = ENDIANNESS( data[r] );
         #endif
      }
      for(; r<68; r++)
         W[r] = P1(W[r-16] ^ W[r-9] ^ ROL32(W[r-3],15)) ^ ROL32(W[r-13],7) ^ W[r-6];

      /*
      // update hash
      */
      {
         /* init A, B, C, D, E, F, G, H by the input hash */
         Ipp32u A = hash[0];
         Ipp32u B = hash[1];
         Ipp32u C = hash[2];
         Ipp32u D = hash[3];
         Ipp32u E = hash[4];
         Ipp32u F = hash[5];
         Ipp32u G = hash[6];
         Ipp32u H = hash[7];

         Ipp32u TT1, TT2, _H, _D;
         for(r=0; r<64; r++)
            COMPACT_SM3_STEP(A,B,C,D,E,F,G,H, MagicFF,MagicGG, W, SM3_cnt_loc, r);

         /* update hash */
         hash[0] ^= A;
         hash[1] ^= B;
         hash[2] ^= C;
         hash[3] ^= D;
         hash[4] ^= E;
         hash[5] ^= F;
         hash[6] ^= G;
         hash[7] ^= H;
      }
   }
}

#else
void UpdateSM3(void* uniHash, const Ipp8u* mblk, int mlen, const void* uniParam)
{
   Ipp32u* data = (Ipp32u*)mblk;

   Ipp32u* hash = (Ipp32u*)uniHash;
   Ipp32u* SM3_cnt_loc = (Ipp32u*)uniParam;

   for(; mlen>=MBS_SM3; data += MBS_SM3/sizeof(Ipp32u), mlen -= MBS_SM3) {

      /* copy input hash */
      Ipp32u A = hash[0];
      Ipp32u B = hash[1];
      Ipp32u C = hash[2];
      Ipp32u D = hash[3];
      Ipp32u E = hash[4];
      Ipp32u F = hash[5];
      Ipp32u G = hash[6];
      Ipp32u H = hash[7];

      Ipp32u W[16];
      int j;


      /* initialize the first 16 words in the array W (remember about endian) */
      for(j=0; j<16; j++) {
         #if (IPP_ENDIAN == IPP_BIG_ENDIAN)
         W[j] = data[j];
         #else
         W[j] = ENDIANNESS( data[j] );
         #endif
      }

      /* apply compression function */
      {
         Ipp32u TT1, TT2;
         SM3_STEP1( 0,  A,B,C,D,E,F,G,H, SM3_cnt_loc[0], W);
         SM3_STEP1( 1,  H,A,B,C,D,E,F,G, SM3_cnt_loc[1], W);
         SM3_STEP1( 2,  G,H,A,B,C,D,E,F, SM3_cnt_loc[2], W);
         SM3_STEP1( 3,  F,G,H,A,B,C,D,E, SM3_cnt_loc[3], W);
         SM3_STEP1( 4,  E,F,G,H,A,B,C,D, SM3_cnt_loc[4], W);
         SM3_STEP1( 5,  D,E,F,G,H,A,B,C, SM3_cnt_loc[5], W);
         SM3_STEP1( 6,  C,D,E,F,G,H,A,B, SM3_cnt_loc[6], W);
         SM3_STEP1( 7,  B,C,D,E,F,G,H,A, SM3_cnt_loc[7], W);

         SM3_STEP1( 8,  A,B,C,D,E,F,G,H, SM3_cnt_loc[ 8], W);
         SM3_STEP1( 9,  H,A,B,C,D,E,F,G, SM3_cnt_loc[ 9], W);
         SM3_STEP1(10,  G,H,A,B,C,D,E,F, SM3_cnt_loc[10], W);
         SM3_STEP1(11,  F,G,H,A,B,C,D,E, SM3_cnt_loc[11], W);
         SM3_STEP1(12,  E,F,G,H,A,B,C,D, SM3_cnt_loc[12], W);
         SM3_STEP1(13,  D,E,F,G,H,A,B,C, SM3_cnt_loc[13], W);
         SM3_STEP1(14,  C,D,E,F,G,H,A,B, SM3_cnt_loc[14], W);
         SM3_STEP1(15,  B,C,D,E,F,G,H,A, SM3_cnt_loc[15], W);

         SM3_STEP2(16,  A,B,C,D,E,F,G,H, SM3_cnt_loc[16], W);
         SM3_STEP2(17,  H,A,B,C,D,E,F,G, SM3_cnt_loc[17], W);
         SM3_STEP2(18,  G,H,A,B,C,D,E,F, SM3_cnt_loc[18], W);
         SM3_STEP2(19,  F,G,H,A,B,C,D,E, SM3_cnt_loc[19], W);
         SM3_STEP2(20,  E,F,G,H,A,B,C,D, SM3_cnt_loc[20], W);
         SM3_STEP2(21,  D,E,F,G,H,A,B,C, SM3_cnt_loc[21], W);
         SM3_STEP2(22,  C,D,E,F,G,H,A,B, SM3_cnt_loc[22], W);
         SM3_STEP2(23,  B,C,D,E,F,G,H,A, SM3_cnt_loc[23], W);

         SM3_STEP2(24,  A,B,C,D,E,F,G,H, SM3_cnt_loc[24], W);
         SM3_STEP2(25,  H,A,B,C,D,E,F,G, SM3_cnt_loc[25], W);
         SM3_STEP2(26,  G,H,A,B,C,D,E,F, SM3_cnt_loc[26], W);
         SM3_STEP2(27,  F,G,H,A,B,C,D,E, SM3_cnt_loc[27], W);
         SM3_STEP2(28,  E,F,G,H,A,B,C,D, SM3_cnt_loc[28], W);
         SM3_STEP2(29,  D,E,F,G,H,A,B,C, SM3_cnt_loc[29], W);
         SM3_STEP2(30,  C,D,E,F,G,H,A,B, SM3_cnt_loc[30], W);
         SM3_STEP2(31,  B,C,D,E,F,G,H,A, SM3_cnt_loc[31], W);

         SM3_STEP2(32,  A,B,C,D,E,F,G,H, SM3_cnt_loc[32], W);
         SM3_STEP2(33,  H,A,B,C,D,E,F,G, SM3_cnt_loc[33], W);
         SM3_STEP2(34,  G,H,A,B,C,D,E,F, SM3_cnt_loc[34], W);
         SM3_STEP2(35,  F,G,H,A,B,C,D,E, SM3_cnt_loc[35], W);
         SM3_STEP2(36,  E,F,G,H,A,B,C,D, SM3_cnt_loc[36], W);
         SM3_STEP2(37,  D,E,F,G,H,A,B,C, SM3_cnt_loc[37], W);
         SM3_STEP2(38,  C,D,E,F,G,H,A,B, SM3_cnt_loc[38], W);
         SM3_STEP2(39,  B,C,D,E,F,G,H,A, SM3_cnt_loc[39], W);

         SM3_STEP2(40,  A,B,C,D,E,F,G,H, SM3_cnt_loc[40], W);
         SM3_STEP2(41,  H,A,B,C,D,E,F,G, SM3_cnt_loc[41], W);
         SM3_STEP2(42,  G,H,A,B,C,D,E,F, SM3_cnt_loc[42], W);
         SM3_STEP2(43,  F,G,H,A,B,C,D,E, SM3_cnt_loc[43], W);
         SM3_STEP2(44,  E,F,G,H,A,B,C,D, SM3_cnt_loc[44], W);
         SM3_STEP2(45,  D,E,F,G,H,A,B,C, SM3_cnt_loc[45], W);
         SM3_STEP2(46,  C,D,E,F,G,H,A,B, SM3_cnt_loc[46], W);
         SM3_STEP2(47,  B,C,D,E,F,G,H,A, SM3_cnt_loc[47], W);

         SM3_STEP2(48,  A,B,C,D,E,F,G,H, SM3_cnt_loc[48], W);
         SM3_STEP2(49,  H,A,B,C,D,E,F,G, SM3_cnt_loc[49], W);
         SM3_STEP2(50,  G,H,A,B,C,D,E,F, SM3_cnt_loc[50], W);
         SM3_STEP2(51,  F,G,H,A,B,C,D,E, SM3_cnt_loc[51], W);
         SM3_STEP3(52,  E,F,G,H,A,B,C,D, SM3_cnt_loc[52], W);
         SM3_STEP3(53,  D,E,F,G,H,A,B,C, SM3_cnt_loc[53], W);
         SM3_STEP3(54,  C,D,E,F,G,H,A,B, SM3_cnt_loc[54], W);
         SM3_STEP3(55,  B,C,D,E,F,G,H,A, SM3_cnt_loc[55], W);

         SM3_STEP3(56,  A,B,C,D,E,F,G,H, SM3_cnt_loc[56], W);
         SM3_STEP3(57,  H,A,B,C,D,E,F,G, SM3_cnt_loc[57], W);
         SM3_STEP3(58,  G,H,A,B,C,D,E,F, SM3_cnt_loc[58], W);
         SM3_STEP3(59,  F,G,H,A,B,C,D,E, SM3_cnt_loc[59], W);
         SM3_STEP3(60,  E,F,G,H,A,B,C,D, SM3_cnt_loc[60], W);
         SM3_STEP3(61,  D,E,F,G,H,A,B,C, SM3_cnt_loc[61], W);
         SM3_STEP3(62,  C,D,E,F,G,H,A,B, SM3_cnt_loc[62], W);
         SM3_STEP3(63,  B,C,D,E,F,G,H,A, SM3_cnt_loc[63], W);
      }
      /* update hash */
      hash[0] ^= A;
      hash[1] ^= B;
      hash[2] ^= C;
      hash[3] ^= D;
      hash[4] ^= E;
      hash[5] ^= F;
      hash[6] ^= G;
      hash[7] ^= H;
   }
}
#endif

#endif /* _PX/_W7/_T7, _MX/_M7 versions */
