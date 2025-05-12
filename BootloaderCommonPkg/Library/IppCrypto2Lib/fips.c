/** @file

  Copyright (c) 2025, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#include "owndefs.h"
#include "owncp.h"

#include "pcpngrsa.h"
#include "pcphash.h"
#include "pcptool.h"
#include "ippcp/fips_cert.h"

#include <Library/CryptoLib.h>
#include <Library/BlMemoryAllocationLib.h>
#include <Library/DebugLib.h>


RETURN_STATUS
EFIAPI
IppCrypto2LibFipsSelftestRsaVerifyPkcsv15 ()
{
    DEBUG ((DEBUG_INFO, "FipsSelftestRsaVerifyPkcsv15\n"));
    //------ FIPS-required part
    // 1. check that the function is FIPS-approved:
    if(!ippcp_is_fips_approved_func(RSAVerify_PKCS1v15_rmf)) {
        return RETURN_UNSUPPORTED; // cannot use this function in FIPS mode.
    }

    // 2. Run the Selftest
    fips_test_status selftest_status = IPPCP_ALGO_SELFTEST_OK;

    // Query buffer size for the test and allocate it (it can be done on Intel® Cryptography Primitives Library side with IPPCP_SELFTEST_USE_MALLOC=on)
    int KeysBufferSize = 0;
    selftest_status += fips_selftest_ippsRSASignVerify_PKCS1v15_rmf_get_size_keys(&KeysBufferSize);

    Ipp8u *pKeysBuffer = AllocateTemporaryMemory(KeysBufferSize);
    if (pKeysBuffer == NULL) {
        return RETURN_OUT_OF_RESOURCES;
    }
    int BufferSize = 0;
    selftest_status += fips_selftest_ippsRSASignVerify_PKCS1v15_rmf_get_size(&BufferSize, pKeysBuffer);

    Ipp8u *pBuffer = AllocateTemporaryMemory(BufferSize);
    if (pBuffer == NULL) {
        FreeTemporaryMemory(pKeysBuffer);
        return RETURN_OUT_OF_RESOURCES;
    }

    // Run the test
    selftest_status += fips_selftest_ippsRSAVerify_PKCS1v15_rmf(pBuffer, pKeysBuffer);

    // Check selftest status
    if (IPPCP_ALGO_SELFTEST_OK != selftest_status) {
        return RETURN_UNSUPPORTED; // selftest is not successful -> cannot use this function in FIPS mode.
    }
    //------ FIPS-required part ends (only needed before the first use of algorithm)

    FreeTemporaryMemory(pKeysBuffer);
    FreeTemporaryMemory(pBuffer);
    return RETURN_SUCCESS;
}

RETURN_STATUS
EFIAPI
IppCrypto2LibFipsSelftestSHA256 ()
{
    DEBUG ((DEBUG_INFO, "FipsSelftestSHA256\n"));
    // 1. check that the function is FIPS-approved:
    if(!ippcp_is_fips_approved_func(HashMessage_rmf)) {
      return RETURN_UNSUPPORTED; // cannot use this function in FIPS mode.
    }

    // 2. Run the Selftest
    fips_test_status selftest_status = IPPCP_ALGO_SELFTEST_OK;

    // Query buffer size for the test and allocate it (it can be done on Intel® Cryptography Primitives Library side with IPPCP_SELFTEST_USE_MALLOC=on)
    selftest_status += fips_selftest_ippsHashMessage_rmf(ippHashAlg_SHA256);

    // Check selftest status
    if (IPPCP_ALGO_SELFTEST_OK != selftest_status) {
      return RETURN_UNSUPPORTED; // selftest is not successful -> cannot use this function in FIPS mode.
    }
    //------ FIPS-required part ends (only needed before the first use of algorithm)

    return RETURN_SUCCESS;
}

RETURN_STATUS
EFIAPI
IppCrypto2LibFipsSelftestSHA384 ()
{
    DEBUG ((DEBUG_INFO, "FipsSelftestSHA384\n"));
    // 1. check that the function is FIPS-approved:
    if(!ippcp_is_fips_approved_func(HashMessage_rmf)) {
      return RETURN_UNSUPPORTED; // cannot use this function in FIPS mode.
    }

    // 2. Run the Selftest
    fips_test_status selftest_status = IPPCP_ALGO_SELFTEST_OK;

    // Query buffer size for the test and allocate it (it can be done on Intel® Cryptography Primitives Library side with IPPCP_SELFTEST_USE_MALLOC=on)
    selftest_status += fips_selftest_ippsHashMessage_rmf(ippHashAlg_SHA384);

    // Check selftest status
    if (IPPCP_ALGO_SELFTEST_OK != selftest_status) {
      return RETURN_UNSUPPORTED; // selftest is not successful -> cannot use this function in FIPS mode.
    }
    //------ FIPS-required part ends (only needed before the first use of algorithm)

    return RETURN_SUCCESS;
}

/* SM3 is not supported by FIPS.
 *  Here It is used for negative testcase execution
 */
RETURN_STATUS
EFIAPI
IppCrypto2LibFipsSelftestSM3 ()
{
    DEBUG ((DEBUG_INFO, "FipsSelftestSM3\n"));
    // 1. check that the function is FIPS-approved:
    if(!ippcp_is_fips_approved_func(HashMessage_rmf)) {
      return RETURN_UNSUPPORTED; // cannot use this function in FIPS mode.
    }

    // 2. Run the Selftest
    fips_test_status selftest_status = IPPCP_ALGO_SELFTEST_OK;

    // Query buffer size for the test and allocate it (it can be done on Intel® Cryptography Primitives Library side with IPPCP_SELFTEST_USE_MALLOC=on)
    selftest_status += fips_selftest_ippsHashMessage_rmf(ippHashAlg_SM3);

    // Check selftest status
    if (IPPCP_ALGO_SELFTEST_OK != selftest_status) {
      return RETURN_UNSUPPORTED; // selftest is not successful -> cannot use this function in FIPS mode.
    }
    //------ FIPS-required part ends (only needed before the first use of algorithm)

    return RETURN_SUCCESS;
}

RETURN_STATUS
EFIAPI
IppCrypto2LibFipsSelftestSM3Update ()
{
    DEBUG ((DEBUG_INFO, "SelftestSM3Update\n"));
    //------ FIPS-required part
    // 1. check that the function is FIPS-approved:
    if(!ippcp_is_fips_approved_func(HashUpdate_rmf)) {
        return RETURN_UNSUPPORTED; // cannot use this function in FIPS mode.
    }

    // 2. Run the Selftest
    fips_test_status selftest_status = IPPCP_ALGO_SELFTEST_OK;

    // Query buffer size for the test and allocate it (it can be done on Intel® Cryptography Primitives Library side with IPPCP_SELFTEST_USE_MALLOC=on)
    int BufferSize = 0;
    selftest_status += fips_selftest_ippsHash_rmf_get_size(&BufferSize);

    Ipp8u *pBuffer = AllocateTemporaryMemory(BufferSize);
    if (pBuffer == NULL) {
        return RETURN_OUT_OF_RESOURCES;
    }

    // Run the test
    selftest_status += fips_selftest_ippsHashUpdate_rmf(ippHashAlg_SM3, pBuffer);

    // Check selftest status
    if (IPPCP_ALGO_SELFTEST_OK != selftest_status) {
        return RETURN_UNSUPPORTED; // selftest is not successful -> cannot use this function in FIPS mode.
    }
    //------ FIPS-required part ends (only needed before the first use of algorithm)

    FreeTemporaryMemory(pBuffer);
    return RETURN_SUCCESS;
}


RETURN_STATUS
EFIAPI
IppCrypto2LibFipsSelftestSHA256Update ()
{
    DEBUG ((DEBUG_INFO, "FipsSelftestSHA256Update\n"));
    //------ FIPS-required part
    // 1. check that the function is FIPS-approved:
    if(!ippcp_is_fips_approved_func(HashUpdate_rmf)) {
        return RETURN_UNSUPPORTED; // cannot use this function in FIPS mode.
    }

    // 2. Run the Selftest
    fips_test_status selftest_status = IPPCP_ALGO_SELFTEST_OK;

    // Query buffer size for the test and allocate it (it can be done on Intel® Cryptography Primitives Library side with IPPCP_SELFTEST_USE_MALLOC=on)
    int BufferSize = 0;
    selftest_status += fips_selftest_ippsHash_rmf_get_size(&BufferSize);

    Ipp8u *pBuffer = AllocateTemporaryMemory(BufferSize);
    if (pBuffer == NULL) {
        return RETURN_OUT_OF_RESOURCES;
    }

    // Run the test
    selftest_status += fips_selftest_ippsHashUpdate_rmf(ippHashAlg_SHA256, pBuffer);

    // Check selftest status
    if (IPPCP_ALGO_SELFTEST_OK != selftest_status) {
        return RETURN_UNSUPPORTED; // selftest is not successful -> cannot use this function in FIPS mode.
    }
    //------ FIPS-required part ends (only needed before the first use of algorithm)

    FreeTemporaryMemory(pBuffer);
    return RETURN_SUCCESS;
}

RETURN_STATUS
EFIAPI
IppCrypto2LibFipsSelftestSHA384Update ()
{
    DEBUG ((DEBUG_INFO, "FipsSelftestSHA384Update\n"));
    //------ FIPS-required part
    // 1. check that the function is FIPS-approved:
    if(!ippcp_is_fips_approved_func(HashUpdate_rmf)) {
        return RETURN_UNSUPPORTED; // cannot use this function in FIPS mode.
    }

    // 2. Run the Selftest
    fips_test_status selftest_status = IPPCP_ALGO_SELFTEST_OK;

    // Query buffer size for the test and allocate it (it can be done on Intel® Cryptography Primitives Library side with IPPCP_SELFTEST_USE_MALLOC=on)
    int BufferSize = 0;
    selftest_status += fips_selftest_ippsHash_rmf_get_size(&BufferSize);

    Ipp8u *pBuffer = AllocateTemporaryMemory(BufferSize);
    if (pBuffer == NULL) {
        return RETURN_OUT_OF_RESOURCES;
    }

    // Run the test
    selftest_status += fips_selftest_ippsHashUpdate_rmf(ippHashAlg_SHA384, pBuffer);

    // Check selftest status
    if (IPPCP_ALGO_SELFTEST_OK != selftest_status) {
        return RETURN_UNSUPPORTED; // selftest is not successful -> cannot use this function in FIPS mode.
    }
    //------ FIPS-required part ends (only needed before the first use of algorithm)

    FreeTemporaryMemory(pBuffer);
    return RETURN_SUCCESS;
}

#if 0
RETURN_STATUS
EFIAPI
IppCrypto2LibFipsSelftestHMAC ()
{
    // 1. check that the function is FIPS-approved:
    if(!ippcp_is_fips_approved_func(HMACMessage_rmf)) {
      return RETURN_UNSUPPORTED; // cannot use this function in FIPS mode.
    }

    // 2. Run the Selftest
    fips_test_status selftest_status = IPPCP_ALGO_SELFTEST_OK;

    // Query buffer size for the test and allocate it (it can be done on Intel® Cryptography Primitives Library side with IPPCP_SELFTEST_USE_MALLOC=on)
    selftest_status += fips_selftest_ippsHMACMessage_rmf();

    // Check selftest status
    if (IPPCP_ALGO_SELFTEST_OK != selftest_status) {
      return RETURN_UNSUPPORTED; // selftest is not successful -> cannot use this function in FIPS mode.
    }
    //------ FIPS-required part ends (only needed before the first use of algorithm)

    return RETURN_SUCCESS;
}

RETURN_STATUS
EFIAPI
IppCrypto2LibFipsSelftestHMACUpdate ()
{
    //------ FIPS-required part
    // 1. check that the function is FIPS-approved:
    if(!ippcp_is_fips_approved_func(HMACUpdate_rmf)) {
        return RETURN_UNSUPPORTED; // cannot use this function in FIPS mode.
    }

    // 2. Run the Selftest
    fips_test_status selftest_status = IPPCP_ALGO_SELFTEST_OK;

    // Query buffer size for the test and allocate it (it can be done on Intel® Cryptography Primitives Library side with IPPCP_SELFTEST_USE_MALLOC=on)
    int BufferSize = 0;
    selftest_status += fips_selftest_ippsHMAC_rmf_get_size(&BufferSize);

    Ipp8u *pBuffer = AllocateTemporaryMemory(BufferSize);
    if (pBuffer == NULL) {
        return RETURN_OUT_OF_RESOURCES;
    }

    // Run the test
    selftest_status += fips_selftest_ippsHMACUpdate_rmf(pBuffer);

    // Check selftest status
    if (IPPCP_ALGO_SELFTEST_OK != selftest_status) {
        return RETURN_UNSUPPORTED; // selftest is not successful -> cannot use this function in FIPS mode.
    }
    //------ FIPS-required part ends (only needed before the first use of algorithm)

    FreeTemporaryMemory(pBuffer);
    return RETURN_SUCCESS;
}
#endif

RETURN_STATUS
EFIAPI
IppCrypto2LibFipsSelftestRsaVerifyPSS ()
{
    DEBUG ((DEBUG_INFO, "FipsSelftestRsaVerifyPSS\n"));
    //------ FIPS-required part
    // 1. check that the function is FIPS-approved:
    if(!ippcp_is_fips_approved_func(RSAVerify_PSS_rmf)) {
        return RETURN_UNSUPPORTED; // cannot use this function in FIPS mode.
    }

    // 2. Run the Selftest
    fips_test_status selftest_status = IPPCP_ALGO_SELFTEST_OK;

    // Query buffer size for the test and allocate it (it can be done on Intel® Cryptography Primitives Library side with IPPCP_SELFTEST_USE_MALLOC=on)
    int KeysBufferSize = 0;
    selftest_status += fips_selftest_ippsRSASignVerify_PSS_rmf_get_size_keys(&KeysBufferSize);

    Ipp8u *pKeysBuffer = AllocateTemporaryMemory(KeysBufferSize);
    if (pKeysBuffer == NULL) {
        return RETURN_OUT_OF_RESOURCES;
    }

    int BufferSize = 0;
    selftest_status += fips_selftest_ippsRSASignVerify_PSS_rmf_get_size(&BufferSize, pKeysBuffer);

    Ipp8u *pBuffer = AllocateTemporaryMemory(BufferSize);
    if (pBuffer == NULL) {
        FreeTemporaryMemory(pKeysBuffer);
        return RETURN_OUT_OF_RESOURCES;
    }

    // Run the test
    selftest_status += fips_selftest_ippsRSAVerify_PSS_rmf(pBuffer, pKeysBuffer);

    // Check selftest status
    if (IPPCP_ALGO_SELFTEST_OK != selftest_status) {
        return RETURN_UNSUPPORTED; // selftest is not successful -> cannot use this function in FIPS mode.
    }
    //------ FIPS-required part ends (only needed before the first use of algorithm)

    FreeTemporaryMemory(pKeysBuffer);
    FreeTemporaryMemory(pBuffer);
    return RETURN_SUCCESS;
}

RETURN_STATUS
EFIAPI
RunFipsSelftests ()
{
    DEBUG ((DEBUG_INFO, "Running FIPS selftests\n"));
    // Run the selftests for SHA256
    if(FixedPcdGet8(PcdCompSignHashAlg) == HASH_TYPE_SHA256) {
        if (EFI_ERROR(IppCrypto2LibFipsSelftestSHA256())) {
            return RETURN_UNSUPPORTED;
        }
        if (EFI_ERROR(IppCrypto2LibFipsSelftestSHA256Update())) {
            return RETURN_UNSUPPORTED;
        }
    }

    // Run the selftests for SHA384
    if(FixedPcdGet8(PcdCompSignHashAlg) == HASH_TYPE_SHA384) {
        if (EFI_ERROR(IppCrypto2LibFipsSelftestSHA384())) {
            return RETURN_UNSUPPORTED;
        }
        if (EFI_ERROR(IppCrypto2LibFipsSelftestSHA384Update())) {
            return RETURN_UNSUPPORTED;
        }
    }

/* SM3 is not supported by FIPS.
 * Here It is used for negative testcase execution.
 */
    // Run the selftests for SM3
    if (FixedPcdGet8(PcdIppHashLibSupportedMask) & IPP_HASHLIB_SM3_256) {
        if (EFI_ERROR(IppCrypto2LibFipsSelftestSM3())) {
            return RETURN_UNSUPPORTED;
        }
        if (EFI_ERROR(IppCrypto2LibFipsSelftestSM3Update())) {
            return RETURN_UNSUPPORTED;
        }
    }

/* HMAC is not being used yet
    // Run the selftests for HMAC
    if (EFI_ERROR(IppCrypto2LibFipsSelftestHMAC())) {
        return RETURN_UNSUPPORTED;
    }
    if (EFI_ERROR(IppCrypto2LibFipsSelftestHMACUpdate())) {
        return RETURN_UNSUPPORTED;
    }
*/
    // Run the selftests for RSA PKCSv15 Verify
    if ((FixedPcdGet8(PcdCompSignType) & IPP_RSALIB_PKCS_1_5) != 0) {
        if (EFI_ERROR(IppCrypto2LibFipsSelftestRsaVerifyPkcsv15())) {
            return RETURN_UNSUPPORTED;
        }
    }

    // Run the selftests for RSA PSS Verify
    if ((FixedPcdGet8(PcdCompSignType) & IPP_RSALIB_PSS) != 0) {
        if (EFI_ERROR(IppCrypto2LibFipsSelftestRsaVerifyPSS())) {
            return RETURN_UNSUPPORTED;
        }
    }
    return RETURN_SUCCESS;
}