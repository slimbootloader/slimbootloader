/** @file
  IppCrypto support functions for IA32 architecture.

  CPU feature detection, shift helpers, and BNU32 arithmetic optimized for
  IA32. These functions support ipp-crypto v2.0.0 on 32-bit systems.

  Copyright (c) 2024, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include <Base.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>

#define IPP_CRYPTO_SUPPORT_IMPLEMENTATION
#include "IppCryptoSupport.h"

// Tell MSVC not to treat memset/memcpy as intrinsics in this file so that
// the definitions below are accepted. Without this, MSVC emits C2169.
#ifdef _MSC_VER
#pragma function(memset, memcpy)
// In ipp-crypto v2.0.0 the C interface was split: PurgeBlock() became an inline
// volatile-pointer wrapper that calls PurgeBlockInternal() (the real symbol).
// The NASM file (pcppurgeblkw7as.nasm) still exports the old name 'PurgeBlock'.
// In SBL non-merged builds OWNAPI() is the identity macro, so the linker
// needs bare 'PurgeBlockInternal'.  Map it to the existing NASM export.
#pragma comment(linker, "/alternatename:PurgeBlockInternal=PurgeBlock")
#endif

void *
memset (
  void   *dest,
  int    ch,
  size_t count
  )
{
  return SetMem(dest, (UINTN)(count),(UINT8)(ch));
}

//
// CPU feature detection functions required by cpinit.c.
// Real CPUID/XCR0 implementations; cp_issue_avx512_instruction
// returns 0 (requires inline asm, conservative for boot firmware).
// Functions match IPP_OWN_DECL convention: __cdecl, no mangling.
//
void
__cdecl
cpGetReg (
  int  *buf,
  int  valEAX,
  int  valECX
  )
{
  UINT32  Eax;
  UINT32  Ebx;
  UINT32  Ecx;
  UINT32  Edx;

  AsmCpuidEx ((UINT32)valEAX, (UINT32)valECX, &Eax, &Ebx, &Ecx, &Edx);
  buf[0] = (int)Eax;
  buf[1] = (int)Ebx;
  buf[2] = (int)Ecx;
  buf[3] = (int)Edx;
}

int
__cdecl
cp_is_avx_extension (
  void
  )
{
  UINT32  Eax;
  UINT32  Ebx;
  UINT32  Ecx;
  UINT32  Edx;
  UINT64  Xcr0;

  AsmCpuid (1, &Eax, &Ebx, &Ecx, &Edx);
  if ((Ecx & BIT27) == 0 || (Ecx & BIT28) == 0) {
    return 0;
  }

  Xcr0 = AsmXGetBv (0);
  return ((Xcr0 & 0x6) == 0x6) ? 1 : 0;
}

int
__cdecl
cp_is_avx512_extension (
  void
  )
{
  UINT64  Xcr0;

  if (cp_is_avx_extension () == 0) {
    return 0;
  }

  Xcr0 = AsmXGetBv (0);
  return ((Xcr0 & 0xE6) == 0xE6) ? 1 : 0;
}

int
__cdecl
cp_issue_avx512_instruction (
  void
  )
{
  return 0;
}

UINT64
__cdecl
cp_get_pentium_counter (
  void
  )
{
  return AsmReadTsc ();
}

#ifdef _MSC_VER
//
// MSVC IA32 64-bit shift helpers.
//
// IMPORTANT: _allshl, _aullshl, _allshr, _aullshr use a NON-STANDARD calling
// convention on IA32: the 64-bit operand is passed in EDX:EAX (NOT on the stack)
// and the shift count is in CL (NOT on the stack).  A normal __cdecl C function
// would read garbage from the stack instead.  These MUST be __declspec(naked)
// with inline asm so the compiler does not generate a standard prologue and the
// register values are used as-is.  For _aullshr specifically, keep this aligned
// with Download/QemuSocPkg/QemuFsp/CryptoPkg/Library/IntrinsicLib/Ia32/MathRShiftU64.c.
//
__declspec(naked)
void
__cdecl
_allshl (void)
{
  __asm {
    cmp   cl, 64
    jae   allshl_exit
    cmp   cl, 32
    jae   allshl_more32
    shld  edx, eax, cl
    shl   eax, cl
    ret
allshl_more32:
    mov   edx, eax
    xor   eax, eax
    and   cl, 31
    shl   edx, cl
    ret
allshl_exit:
    xor   eax, eax
    xor   edx, edx
    ret
  }
}

__declspec(naked)
void
__cdecl
_aullshl (void)
{
  __asm {
    cmp   cl, 64
    jae   aullshl_exit
    cmp   cl, 32
    jae   aullshl_more32
    shld  edx, eax, cl
    shl   eax, cl
    ret
aullshl_more32:
    mov   edx, eax
    xor   eax, eax
    and   cl, 31
    shl   edx, cl
    ret
aullshl_exit:
    xor   eax, eax
    xor   edx, edx
    ret
  }
}

__declspec(naked)
void
__cdecl
_allshr (void)
{
  __asm {
    cmp   cl, 64
    jae   allshr_exit
    cmp   cl, 32
    jae   allshr_more32
    shrd  eax, edx, cl
    sar   edx, cl
    ret
allshr_more32:
    mov   eax, edx
    sar   edx, 31
    and   cl, 31
    sar   eax, cl
    ret
allshr_exit:
    sar   edx, 31
    mov   eax, edx
    ret
  }
}

__declspec(naked)
void
__cdecl
_aullshr (void)
{
  __asm {
    cmp   cl, 64
    jae   aullshr_exit
    cmp   cl, 32
    jae   aullshr_more32
    shrd  eax, edx, cl
    shr   edx, cl
    ret
aullshr_more32:
    mov   eax, edx
    xor   edx, edx
    and   cl, 31
    shr   eax, cl
    ret
aullshr_exit:
    xor   eax, eax
    xor   edx, edx
    ret
  }
}
#endif /* _MSC_VER */

void *
memcpy (
  void        *dest_str,
  const void  *src_str,
  size_t      n
  )
{
  return CopyMem(dest_str,src_str, (UINTN)(n));
}

#if defined (MDE_CPU_IA32)

//
// IA32 SBL-safe replacements for BNU32 64-bit arithmetic functions.
// Replaces pcpbnu32_arith_muldigit.c, pcpbnu32_arith_submuldigit.c,
// pcpbnu32_arith_div.c from ipp-crypto.  Those files use native C 64-bit
// operators which generate __allmul/__aulldiv calls not present in Stage1.
// Uses MultU64x32 / DivU64x32 (EDK2 BaseLib inline asm) instead.
//
#include "owncp.h"
#include "pcpbnumisc.h"
#include "pcpbnu32misc.h"
#include "pcpbnu32arith.h"

IPP_OWN_DEFN (Ipp32u, cpMulDgt_BNU32, (Ipp32u* pR, const Ipp32u* pA, cpSize nsA, Ipp32u val))
{
   Ipp32u carry = 0;
   cpSize i;
   for (i = 0; i < nsA; i++) {
      Ipp64u t = MultU64x32 ((UINT64)pA[i], (UINT32)val) + carry;
      pR[i] = IPP_LODWORD (t);
      carry  = IPP_HIDWORD (t);
   }
   return carry;
}

#if !((_IPP32E==_IPP32E_M7) || (_IPP32E==_IPP32E_U8) || \
      (_IPP32E==_IPP32E_Y8) || (_IPP32E>=_IPP32E_E9) || (_IPP32E==_IPP32E_N8))
IPP_OWN_DEFN (Ipp32u, cpSubMulDgt_BNU32, (Ipp32u* pR, const Ipp32u* pA, cpSize nsA, Ipp32u val))
{
   Ipp32u carry = 0;
   for (; nsA > 0; nsA--) {
      Ipp64u r = (Ipp64u)*pR - MultU64x32 ((UINT64)*pA++, (UINT32)val) - carry;
      *pR++ = IPP_LODWORD (r);
      carry  = 0 - IPP_HIDWORD (r);
   }
   return carry;
}

IPP_OWN_DEFN (int, cpDiv_BNU32, (Ipp32u* pQ, cpSize* sizeQ, Ipp32u* pX, cpSize sizeX, Ipp32u* pY, cpSize sizeY))
{
   FIX_BNU32 (pY, sizeY);
   FIX_BNU32 (pX, sizeX);

   if (sizeX < sizeY) {
      if (pQ) { pQ[0] = 0; *sizeQ = 1; }
      return sizeX;
   }

   if (1 == sizeY) {
      int i;
      Ipp32u r = 0;
      for (i = (int)sizeX - 1; i >= 0; i--) {
         Ipp64u tmp = IPP_MAKEDWORD (pX[i], r);
         Ipp32u q   = IPP_LODWORD (DivU64x32 (tmp, (UINT32)pY[0]));
         r          = IPP_LODWORD (tmp - MultU64x32 ((UINT64)pY[0], (UINT32)q));
         if (pQ) pQ[i] = q;
      }
      pX[0] = r;
      if (pQ) { FIX_BNU32 (pQ, sizeX); *sizeQ = sizeX; }
      return 1;
   }

   {
      cpSize qs  = sizeX - sizeY + 1;
      cpSize nlz = cpNLZ_BNU32 (pY[sizeY - 1]);
      pX[sizeX] = 0;
      if (nlz) {
         cpSize ni;
         pX[sizeX] = pX[sizeX - 1] >> (32 - nlz);
         for (ni = sizeX - 1; ni > 0; ni--)
            pX[ni] = (pX[ni] << nlz) | (pX[ni - 1] >> (32 - nlz));
         pX[0] <<= nlz;
         for (ni = sizeY - 1; ni > 0; ni--)
            pY[ni] = (pY[ni] << nlz) | (pY[ni - 1] >> (32 - nlz));
         pY[0] <<= nlz;
      }
      {
         Ipp32u yHi = pY[sizeY - 1];
         int i;
         for (i = (int)qs - 1; i >= 0; i--) {
            Ipp32u extend;
            Ipp64u tmp = IPP_MAKEDWORD (pX[i + sizeY - 1], pX[i + sizeY]);
            Ipp64u q   = DivU64x32 (tmp, (UINT32)yHi);
            Ipp64u r   = tmp - MultU64x32 (q, (UINT32)yHi);
            for (; IPP_HIDWORD (q) ||
                   (Ipp64u)MultU64x32 (q, (UINT32)pY[sizeY - 2]) > IPP_MAKEDWORD (pX[i + sizeY - 2], r); ) {
               q -= 1; r += yHi;
               if (IPP_HIDWORD (r)) break;
            }
            extend = cpSubMulDgt_BNU32 (pX + i, pY, sizeY, (Ipp32u)q);
            extend = (pX[i + sizeY] -= extend);
            if (extend) {
               q -= 1;
               extend = cpAdd_BNU32 (pX + i, pY, pX + i, sizeY);
               pX[i + sizeY] += extend;
            }
            if (pQ) pQ[i] = IPP_LODWORD (q);
         }
      }
      if (nlz) {
         cpSize ni;
         for (ni = 0; ni < sizeX; ni++)
            pX[ni] = (pX[ni] >> nlz) | (pX[ni + 1] << (32 - nlz));
         for (ni = 0; ni < sizeY - 1; ni++)
            pY[ni] = (pY[ni] >> nlz) | (pY[ni + 1] << (32 - nlz));
         pY[sizeY - 1] >>= nlz;
      }
      FIX_BNU32 (pX, sizeX);
      if (pQ) { FIX_BNU32 (pQ, qs); *sizeQ = qs; }
      return sizeX;
   }
}
#endif /* !X64 levels */

//
// MSVC IA32 emits __aulldiv / __aullrem / __allmul for 64-bit arithmetic.
// These wrappers MUST NOT call DivU64x64Remainder: on MSVC IA32, Math64.c
// implements that with C's '/' and '%' operators, which re-emit __aulldiv,
// causing infinite recursion and a triple-fault.
//
// Fast path uses DivU64x32 (inline asm DIV instruction — divisor is always
// 32-bit in cpDiv_BNU32). General fallback uses a shift loop with only
// LShiftU64/RShiftU64 (inline asm) so no CRT helpers are ever called.
//

INT64
__cdecl
_allmul (
  IN INT64  Multiplicand,
  IN INT64  Multiplier
  )
{
  return (INT64)MultU64x64 ((UINT64)Multiplicand, (UINT64)Multiplier);
}

UINT64
__cdecl
_aulldiv (
  IN UINT64  Dividend,
  IN UINT64  Divisor
  )
{
  if (((UINT32 *)&Divisor)[1] == 0) {
    return DivU64x32 (Dividend, ((UINT32 *)&Divisor)[0]);
  }

  {
    UINT64  Quotient  = 0;
    UINT64  Remainder = 0;
    INTN    i;

    for (i = 63; i >= 0; i--) {
      Remainder = LShiftU64 (Remainder, 1);
      if (RShiftU64 (Dividend, (UINTN)i) & 1) {
        ((UINT32 *)&Remainder)[0] |= 1;
      }
      if (Remainder >= Divisor) {
        Remainder -= Divisor;
        Quotient  |= LShiftU64 (1ULL, (UINTN)i);
      }
    }
    return Quotient;
  }
}

UINT64
__cdecl
_aullrem (
  IN UINT64  Dividend,
  IN UINT64  Divisor
  )
{
  if (((UINT32 *)&Divisor)[1] == 0) {
    UINT32  Remainder;

    DivU64x32Remainder (Dividend, ((UINT32 *)&Divisor)[0], &Remainder);
    return (UINT64)Remainder;
  }

  {
    UINT64  Remainder = 0;
    INTN    i;

    for (i = 63; i >= 0; i--) {
      Remainder = LShiftU64 (Remainder, 1);
      if (RShiftU64 (Dividend, (UINTN)i) & 1) {
        ((UINT32 *)&Remainder)[0] |= 1;
      }
      if (Remainder >= Divisor) {
        Remainder -= Divisor;
      }
    }
    return Remainder;
  }
}

#endif /* MDE_CPU_IA32 */
