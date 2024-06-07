/** @file

  Copyright (c) 2024, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Library/DebugLib.h>
#include <Library/BaseLib.h>
#include <Library/CryptoLib.h>
#include <Library/TpmLib.h>
#include <Library/BootGuardLib.h>
#include <Library/SecureBootLib.h>
#include <Library/TpmLib/Tpm2CommandLib.h>
#include <Library/IoLib.h>
#include <Library/PrintLib.h>
#include <Library/BaseMemoryLib.h>
#include <Register/BootGuardRegister.h>
#include <Library/BootloaderCommonLib.h>

//
// Data structure definition
//
#pragma pack (1)

#define  BASE_4GB  0x0000000100000000ULL

//
// FIT definition
//
#define FIT_TABLE_TYPE_HEADER                0x0
#define FIT_TABLE_TYPE_MICROCODE             0x1
#define FIT_TABLE_TYPE_STARTUP_ACM           0x2
#define FIT_TABLE_TYPE_BIOS_MODULE           0x7
#define FIT_TABLE_TYPE_KEY_MANIFEST          0xB
#define FIT_TABLE_TYPE_BOOT_POLICY_MANIFEST  0xC

typedef struct {
  UINT64 Address;
  UINT8  Size[3];
  UINT8  Reserved;
  UINT16 Version;
  UINT8  Type : 7;
  UINT8  Cv   : 1;
  UINT8  Chksum;
} FIRMWARE_INTERFACE_TABLE_ENTRY;

//
// ACM definition
//
#define ACM_MODULE_TYPE_CHIPSET_ACM                     2
#define ACM_MODULE_SUBTYPE_CAPABLE_OF_EXECUTE_AT_RESET  0x1
#define ACM_MODULE_SUBTYPE_ANC_MODULE                   0x2
#define ACM_HEADER_FLAG_DEBUG_SIGNED                    BIT15
#define ACM_NPW_SVN                                     0x2

#define MMIO_ACM_POLICY_STATUS  (TXT_PUBLIC_BASE + R_CPU_ACM_POLICY_STATUS)  // 0xFED30378
#define SCRTM_VERSION_UTF16_LENGTH                      13

typedef struct {
  UINT16 ModuleType;
  UINT16 ModuleSubType;
  UINT32 HeaderLen;
  UINT32 HeaderVersion;
  UINT16 ModuleID;
  UINT16 ModuleFlags;
  UINT32 ModuleVendor;
  UINT32 Revision;
  UINT32 Sizeb;
  UINT16 AcmSvn;
  UINT16 SeSvn;
  UINT32 CodeControl;
  UINT32 ErrorEntryPoint;
  UINT32 GDTSize;
  UINT32 GDTBasePtr;
  UINT32 SegSel;
  UINT32 EntryPoint;
  UINT8  Reserved2[64];
  UINT32 KeySize;     // 96 DWORDS in the Key
  UINT32 ScratchSize; // 208 DWORDS = 832 BYTES Sractch Size
  UINT8  RSA3072PubKey[384];
  UINT8  RSA3072Sig[384];
  UINT8 XMSSPubKey[64];
  UINT8 XMSSSig[2692];
  UINT8 Reserved3[60];
  UINT8 scratch[3584];
} ACM_HEADER_4;

//
// BPM Policy:
//   FIT record type 12 points to Boot Policy Manifest.
//   FIT record type 11 points to Key Manifest.
//

#define RSA_KEY_SIZE_1K                     SIZE_1KB
#define RSA_KEY_SIZE_2K                     SIZE_2KB
#define RSA_KEY_SIZE_3K                     (SIZE_1KB + SIZE_2KB)
#define PKCS_1_5_RSA_SHA1_SIGNATURE_SIZE    (RSA_KEY_SIZE_1K / 8)
#define PKCS_1_5_RSA_SHA256_SIGNATURE_SIZE  (RSA_KEY_SIZE_2K / 8)
#define PKCS_1_5_RSA_SHA384_SIGNATURE_SIZE  (RSA_KEY_SIZE_3K / 8)
#define SHA1_DIGEST_SIZE                    20
#define SHA256_DIGEST_SIZE                  32
#define SHA384_DIGEST_SIZE                  48
#define SM3_256_DIGEST_SIZE                 32

#define MAX_SUPPORTED_SIGNATURE_SIZE        RSA_KEY_SIZE_3K/8

typedef union {
    UINT8   digest8[SHA1_DIGEST_SIZE];
    UINT32  digest32[SHA1_DIGEST_SIZE / 4];
} BtgSha1Digest_u;

typedef union {
    UINT8   digest8[SHA256_DIGEST_SIZE];
    UINT32  digest32[SHA256_DIGEST_SIZE / 4];
    UINT64  digest64[SHA256_DIGEST_SIZE / 8];
} BtgSha256Digest_u;

typedef union {
    UINT8   digest8[SHA384_DIGEST_SIZE];
    UINT32  digest32[SHA384_DIGEST_SIZE / 4];
    UINT64  digest64[SHA384_DIGEST_SIZE / 8];
} BtgSha384Digest_u;

//
// Hash structure
//
typedef struct {
  UINT16               HashAlg;
  UINT16               Size;
  BtgSha1Digest_u      HashBuffer;
} SHA1_HASH_STRUCTURE;

typedef struct {
  UINT16               HashAlg;
  UINT16               Size;
  BtgSha256Digest_u    HashBuffer;
} SHA256_HASH_STRUCTURE;

typedef struct {
  UINT16               HashAlg;
  UINT16               Size;
  BtgSha384Digest_u    HashBuffer;
} SHA384_HASH_STRUCTURE;

typedef struct {
  UINT16               HashAlg;
  UINT16               Size;
  UINT8                HashBuffer[];
} SHAX_HASH_STRUCTURE;

typedef struct {
  UINT16               Size;           // Total number of bytes of HASH_LIST structure
  UINT16               Count;          // Number of Digest elements
} HASH_LIST;

typedef struct {
  UINT16                 Size;         // Total number of bytes of HASH_LIST structure
  UINT16                 Count;        // Number of Digest elements
  SHA1_HASH_STRUCTURE    Sha1Digest;   // Array of digests  {AlgID, Size, HashValue; ...}
  SHA256_HASH_STRUCTURE  Sha256Digest; // Array of digests  {AlgID, Size, HashValue; ...}
  SHA256_HASH_STRUCTURE  ShaSm3Digest; // Array of digests  {AlgID, Size, HashValue; ...}
  SHA384_HASH_STRUCTURE  Sha384Digest; // Array of digests  {AlgID, Size, HashValue; ...}
} MAX_HASH_LIST;

struct {
  UINT64               Usage;          // Bit mask of usages
  SHAX_HASH_STRUCTURE  Digest;         // Standard BtG hash structure primitive
} SHAX_KMHASH_STRUCT;


#define ALG_RSA             0x1
#define ALG_RSASSA          0x14

#define RSA_PUBLIC_KEY_STRUCT_VERSION_1_0  0x10
#define RSA_PUBLIC_KEY_STRUCT_KEY_EXPONENT_DEFAULT  0x11 // NOT 0x10001

typedef struct {
  UINT8    Version;                    // 0x10
  UINT16   KeySizeBits;                // 1024 or 2048 or 3072 bits
} KEY_STRUCT_HEADER;

typedef struct {
  UINT8    Version;                    // 0x10
  UINT16   KeySizeBits;                // Number of bits in the modulus.
  UINT32   Exponent;                   // The public exponent (must be RSA_KEY_EXPONENT_VALUE)
  UINT8    Modulus[RSA_KEY_SIZE_2K/8]; // The modulus in LSB format (256 bytes)
} RSA2K_PUBLIC_KEY_STRUCT;

typedef struct {
  UINT8    Version;                    // 0x10
  UINT16   KeySizeBits;                // Number of bits in the modulus.
  UINT32   Exponent;                   // The public exponent (must be RSA_KEY_EXPONENT_VALUE)
  UINT8    Modulus[RSA_KEY_SIZE_3K/8]; // The modulus in LSB format (384 bytes)
} RSA3K_PUBLIC_KEY_STRUCT;

typedef struct {
  UINT8    Version;                    // 0x10
  UINT16   KeySizeBits;                // 1024 or 2048 or 3072 bits
  UINT32   Exponent;                   // The public exponent
  UINT8    Modulus[1];                 // The modulus in LSB format
} RSA_PUBLIC_KEY_STRUCT;

#define ECC_PUBLIC_KEY_STRUCT_VERSION_1_0  0x10
#define ECC_PUBLIC_KEY_STRUCT_KEY_SIZE_DEFAULT  256
#define ECC_PUBLIC_KEY_STRUCT_KEY_LEN_DEFAULT   (ECC_PUBLIC_KEY_STRUCT_KEY_SIZE_DEFAULT/8)
typedef struct {
  UINT8  Version;                      // 0x10
  UINT16 KeySizeBits;                  // 256 - Number of bits in key. Fixed for SM2
  UINT8  Qx[ECC_PUBLIC_KEY_STRUCT_KEY_LEN_DEFAULT];  // X component. Fixed size for SM2
  UINT8  Qy[ECC_PUBLIC_KEY_STRUCT_KEY_LEN_DEFAULT];  // Y component. Fixed size for SM2
} ECC_PUBLIC_KEY_STRUCT;

#define RSASSA_SIGNATURE_STRUCT_VERSION_1_0  0x10

typedef struct {
  UINT8    Version;
  UINT16   SigSizeBits;                // 2048 or 3072 bits
} SIGNATURE_STRUCT_HEADER;

typedef struct {
  UINT8    Version;
  UINT16   KeySizeBits;                // 2048 or 3072 bits
  UINT16   HashAlg;
  UINT8    Signature[];
} RSASSA_SIGNATURE_STRUCT;

#define ECC_SIGNATURE_STRUCT_VERSION_1_0  0x10
typedef struct {
  UINT8    Version;
  UINT16   KeySizeBits;                // 256 or 384 bits
  UINT16   HashAlg;
} ECC_SIGNATURE_STRUCT;

#define KEY_SIGNATURE_STRUCT_VERSION_1_0  0x10
typedef struct {
  UINT8                      Version;
  UINT16                     KeyAlg;       // TPM_ALG_RSA=0x1 or TPM_ALG_ECC=0x23
} KEY_AND_SIGNATURE_STRUCT_HEADER;
typedef struct {
  UINT8                      Version;
  UINT16                     KeyAlg;       // TPM_ALG_RSA=0x1 or TPM_ALG_ECC=0x23
  union {                                  // Based on KeyAlg
    RSA_PUBLIC_KEY_STRUCT    RsaKey;
    ECC_PUBLIC_KEY_STRUCT    EccKey;
  } Key;
  UINT16                     SigScheme;    // TPM_ALG_RSASSA=0x14 or TPM_ALG_RSAPSS=0x15 or TPM_ALG_SM2=0x1B
  union {                                  // Based on KeyAlg
    RSASSA_SIGNATURE_STRUCT  SignatureRsa;
    ECC_SIGNATURE_STRUCT     SignatureEcc;
  } Sig;
} KEY_AND_SIGNATURE_STRUCT;

#define BP_KEY_TYPE_BOOT_POLICY_MANIFEST  0
#define BP_KEY_TYPE_KEY_MANIFEST          1

#define BOOT_POLICY_MANIFEST_HEADER_STRUCTURE_ID  (*(UINT64 *)"__ACBP__")
#define BOOT_POLICY_MANIFEST_HEADER_VERSION_2_1          0x21
typedef struct {
  UINT8              StructureId[8];
  UINT8              StructVersion;        // 0x21
  UINT8              HdrStructVersion;     // 0x20
  UINT16             HdrSize;              // total number of bytes in Header (i.e., offset to first element)
  UINT16             KeySignatureOffset;   // Offset from start of Bpm to KeySignature field of Signature Element
  UINT8              BpmRevision;
  UINT8              BpmRevocation;
  UINT8              AcmRevocation;
  UINT8              Reserved;
  UINT16             NemPages;
} BOOT_POLICY_MANIFEST_HEADER;

#define IBB_SEGMENT_FLAG_IBB     0x0
#define IBB_SEGMENT_FLAG_NON_IBB 0x1
typedef struct {
  UINT16             Reserved;         // Alignment
  UINT16             Flags;            // Control flags
  UINT32             Base;             // Segment base
  UINT32             Size;             // Segment size
} IBB_SEGMENT;

#define BPM_CNBS_ELEMENT_STRUCTURE_ID (*(UINT64 *)"__CNBS__")
#define BPM_CNBS_ELEMENT_VERSION 0x10
typedef struct {
  UINT8               StructureID[8];
  UINT8               StructVersion; // 0x10
  UINT16              SizeOfData;
  UINT8               Reserved;
  IBB_SEGMENT         BufferData;
} CNBS_SEGMENT;

#define BOOT_POLICY_MANIFEST_IBB_ELEMENT_STRUCTURE_ID  (*(UINT64 *)"__IBBS__")
#define BOOT_POLICY_MANIFEST_IBB_ELEMENT_DIGEST_ID     (*(UINT64 *)"__DIGE__")

#define BOOT_POLICY_MANIFEST_IBB_ELEMENT_VERSION_2_0       0x20
#define IBB_FLAG_INITIAL_MEASURE_LOC3  0x2
#define IBB_FLAG_AUTHORITY_MEASURE     0x4
//
// BIT7 will be used to control the stability of PCR[0] by masking out mutable policies from
// ACM_BIOS_POLICY_STS (0xfed30378) which is part of extend value into PCR[0]
//
#define IBB_FLAG_SRTM_AC               0x80 //Bit 7
#define SRTM_AC_MASK                   0x20FFF

typedef struct {
  UINT8               StructureId[8];
  UINT8               StructVersion;   // 0x20
  UINT8               Reserved0;
  UINT16              ElementSize;     // Total number of bytes in the element
  UINT8               Reserved1;
  UINT8               SetType;
  UINT8               Reserved;
  UINT8               PbetValue;
  UINT32              Flags;
  UINT64              IbbMchBar;
  UINT64              VtdBar;
  UINT32              DmaProtBase0;
  UINT32              DmaProtLimit0;
  UINT64              DmaProtBase1;
  UINT64              DmaProtLimit1;
  SHAX_HASH_STRUCTURE PostIbbHash;
} IBB_ELEMENT;

#define BOOT_POLICY_MANIFEST_TXT_ELEMENT_STRUCTURE_ID  (*(UINT64 *)"__TXTS__")
#define BOOT_POLICY_MANIFEST_TXT_ELEMENT_VERSION_2_0       0x20
typedef struct {
  UINT8               StructureId[8];
  UINT8               StructVersion;   // 0x20
  UINT8               Reserved0;
  UINT16              ElementSize;     // Total number of bytes in the element
  UINT8               Reserved1;
  UINT8               SetType;
  UINT16              Reserved;
  UINT32              Flags;
  UINT16              PwrDownInterval;
  UINT8               PttCmosOffset0;
  UINT8               PttCmosOffset1;
  UINT16              AcpiBaseOffset;
  UINT16              Reserved2;
  UINT32              PrwmBaseOffset;
  HASH_LIST           DigestList;
  UINT8               Reserved3[3];
  UINT8               SegmentCount;
  IBB_SEGMENT*        TxtSegment;      // TxtSegment[SegmentCount]
} TXT_ELEMENT;

#define BOOT_POLICY_MANIFEST_PLATFORM_CONFIG_DATA_ELEMENT_STRUCTURE_ID  (*(UINT64 *)"__PCDS__")
#define BOOT_POLICY_MANIFEST_PLATFORM_CONFIG_DATA_ELEMENT_VERSION_2_0       0x20
typedef struct {
  UINT8               StructureId[8];
  UINT8               StructVersion;   // 0x20
  UINT8               Reserved0;
  UINT16              ElementSize;     // Total number of bytes in the element
  UINT16              Reserved1;
  UINT16              SizeOfData;
  UINT8*              Data;            //Data[SizeofData]  // Any data but starts from PDRS
} PLATFORM_CONFIG_DATA_ELEMENT;

#define BOOT_POLICY_MANIFEST_PLATFORM_MANUFACTURER_ELEMENT_STRUCTURE_ID  (*(UINT64 *)"__PMDA__")
#define BOOT_POLICY_MANIFEST_PLATFORM_MANUFACTURER_ELEMENT_VERSION_2_0       0x20
typedef struct {
  UINT8               StructureId[8];
  UINT8               StructVersion;       // 0x20
  UINT8               Reserved0;
  UINT16              ElementSize;         // Total number of bytes in the element
  UINT16              Reserved1;
  UINT16              PmDataSize;          // required to be 4 byte multiple
  UINT8*              PmData;              // PmData[PmDataSize]
} PLATFORM_MANUFACTURER_ELEMENT;

#define BOOT_POLICY_MANIFEST_SIGNATURE_ELEMENT_STRUCTURE_ID  (*(UINT64 *)"__PMSG__")
#define BOOT_POLICY_MANIFEST_SIGNATURE_ELEMENT_VERSION_1_0       0x10
typedef struct {
  UINT8               StructureId[8];
  UINT8               StructVersion;       // 0x20
  UINT8               Reserved[3];         // KeySignature must be DWORD aligned
  KEY_AND_SIGNATURE_STRUCT  KeySignature;  // this is now a variable Size
} BOOT_POLICY_MANIFEST_SIGNATURE_ELEMENT;

#define KEY_MANIFEST_STRUCTURE_ID  (*(UINT64 *)"__KEYM__")
#define KEY_MANIFEST_STRUCTURE_VERSION_2_1     0x21
typedef struct {
  UINT8               StructureId[8];
  UINT8               StructVersion;       // 0x21
  UINT8               reserved[3];         // 3 bytes to make KeySignatureOffset same offset as for BPM
  UINT16              KeySignatureOffset;  // Offset from start of KM to KeyManifestSignature
  UINT8               Reserved2[3];        // Alignment
  UINT8               KeyManifestRevision;
  UINT8               KmSvn;
  UINT8               KeyManifestId;
  UINT16              KmPubKeyHashAlg;
  UINT16              KeyCount;
} KEY_MANIFEST_STRUCTURE;

//
// ACM Policy data
//
typedef struct {
  UINT64 AcmPolicyStatus;
  UINT8  ManifestKeyHashes[];
} ACM_POLICY_DATA;

typedef struct {
  UINT64 AcmPolicyStatus;
  UINT8  KmKeyHash[MAX_SUPPORTED_SIGNATURE_SIZE];
  UINT8  BpmKeyHashFromKm[MAX_SUPPORTED_SIGNATURE_SIZE];
} MAX_ACM_POLICY_DATA;

//
// Efi Startup Locality Event Data
//
typedef struct {
  UINT8   Signature[16];
  UINT8   StartupLocality;
} TCG_EFI_STARTUP_LOCALITY_EVENT;

typedef union {
  struct {
    UINT64 KmId               : 4;      // 0-3   Key Manifest ID used for verified Key Manifest
    UINT64 MeasuredBoot       : 1;      // 4     perform measured boot
    UINT64 VerifiedBoot       : 1;      // 5     perform verified boot
    UINT64 HAP                : 1;      // 6     high assurance platform
    UINT64 TxtSupported       : 1;      // 7     txt supported
    UINT64 BootMedia          : 1;      // 8     Boot media
    UINT64 DCD                : 1;      // 9     disable CPU debug
    UINT64 DBI                : 1;      // 10    disable BSP init
    UINT64 PBE                : 1;      // 11    protect BIOS environment
    UINT64 BBP                : 1;      // 12    bypass boot policy - fast S3 resume
    UINT64 TpmType            : 2;      // 13-14 TPM Type
    UINT64 TpmSuccess         : 1;      // 15    TPM Success
    UINT64 Reserved1          : 1;      // 16
    UINT64 BootPolicies       : 1;      // 17    PFR supported
    UINT64 BackupActions      : 2;      // 18-19 Backup actions
    UINT64 TxtProfile         : 5;      // 20-24 TXT profile selection
    UINT64 MemScrubPolicy     : 2;      // 25-26 Memory scrubbing policy
    UINT64 Reserved2          : 2;      // 27-28
    UINT64 DmaProtection      : 1;      // 29    DMA Protection
    UINT64 Reserved3          : 2;      // 30-31
    UINT64 SCrtmStatus        : 3;      // 32-34 S-CRTM status
    UINT64 Cosign             : 1;      // 35    CPU co-signing
    UINT64 TpmStartupLocality : 1;      // 36    TPM startup locality.
    UINT64 Reserved           :27;      // 37-63
  } Bits;
  UINT64 Data;
} ACM_BIOS_POLICY;

STATIC TPMI_ALG_HASH SortedAlgIds[] = {
  TPM_ALG_SHA1,
  TPM_ALG_SHA256,
  TPM_ALG_SHA384,
  TPM_ALG_SHA512,
  TPM_ALG_SM3_256
};

#pragma pack ()

/**
   Compute hash of data for the selected PCR, strictly following
   the ascending order of the TCG algorithm IDs:
   SHA1 -> SHA256 -> SHA384 -> SHA512 -> SM3


   @param[in]  Data   data
   @param[in]  Size   data size
   @param[in]  SelectedPcrs Mask of selected PCRs
   @param[out] DigestList   Digest list to store the computed digest values.
**/
VOID
HashDataForSelectedPcrs (
  IN  UINT8              *Data,
  IN  UINT32             Size,
  IN  UINT32             SelectedPcrs,
  OUT TPML_DIGEST_VALUES *DigestList
  )
{
  UINT8  Sha256[SHA256_DIGEST_SIZE];
  UINT8  Sha384[SHA384_DIGEST_SIZE];
  UINT8  Sha512[SHA512_DIGEST_SIZE];
  UINT8  Sm3[SM3_256_DIGEST_SIZE];

  ZeroMem (DigestList, sizeof (TPML_DIGEST_VALUES));

  if (((SelectedPcrs & HASH_ALG_SHA1) != 0) &&
      (DigestList->count < HASH_COUNT)) {
    DEBUG ((DEBUG_ERROR, "Unsupported hashing algorithm: SHA1\n"));
  }

  if (((SelectedPcrs & HASH_ALG_SHA256) != 0) &&
      (DigestList->count < HASH_COUNT)) {
    CalculateHash((UINT8*)Data, Size, HASH_TYPE_SHA256, (UINT8 *)Sha256);
    DigestList->digests[DigestList->count].hashAlg = TPM_ALG_SHA256;
    CopyMem (DigestList->digests[DigestList->count].digest.sha256, Sha256, SHA256_DIGEST_SIZE);
    DigestList->count ++;
  }

  if (((SelectedPcrs & HASH_ALG_SHA384) != 0) &&
      (DigestList->count < HASH_COUNT)) {
    CalculateHash((UINT8*)Data, Size, HASH_TYPE_SHA384, (UINT8 *)Sha384);
    DigestList->digests[DigestList->count].hashAlg = TPM_ALG_SHA384;
    CopyMem (DigestList->digests[DigestList->count].digest.sha384, Sha384, SHA384_DIGEST_SIZE);
    DigestList->count ++;
  }

  if (((SelectedPcrs & HASH_ALG_SHA512) != 0) &&
      (DigestList->count < HASH_COUNT)) {
    CalculateHash((UINT8*)Data, Size, HASH_TYPE_SHA512, (UINT8 *)Sha512);
    DigestList->digests[DigestList->count].hashAlg = TPM_ALG_SHA512;
    CopyMem (DigestList->digests[DigestList->count].digest.sha512, Sha512, SHA512_DIGEST_SIZE);
    DigestList->count ++;
  }

  if (((SelectedPcrs & HASH_ALG_SM3_256) != 0) &&
      (DigestList->count < HASH_COUNT)) {
    CalculateHash((UINT8*)Data, Size, HASH_TYPE_SM3, (UINT8 *)Sm3);
    DigestList->digests[DigestList->count].hashAlg = TPM_ALG_SM3_256;
    CopyMem (DigestList->digests[DigestList->count].digest.sm3_256, Sm3, SM3_256_DIGEST_SIZE);
    DigestList->count ++;
  }
}

/**
  Check if buffer is all zero.

  @param[in] Buffer      Buffer to be checked.
  @param[in] BufferSize  Size of buffer to be checked.

  @retval TRUE  Buffer is all zero.
  @retval FALSE Buffer is not all zero.
**/
BOOLEAN
IsZeroBufferLocal (
  IN VOID  *Buffer,
  IN UINTN BufferSize
  )
{
  UINT8 *BufferData;
  UINTN Index;

  BufferData = Buffer;
  for (Index = 0; Index < BufferSize; Index++) {
    if (BufferData[Index] != 0) {
      return FALSE;
    }
  }
  return TRUE;
}

/**
  Find FIT Entry address data by type.

  @param[in] Type  FIT Entry type

  @return  FIT entry address
**/
VOID *
FindFitEntryData (
  IN UINT8 Type
  )
{
  FIRMWARE_INTERFACE_TABLE_ENTRY *FitEntry;
  UINT32                         EntryNum;
  UINT64                         FitTableOffset;
  UINT32                         Index;

  FitTableOffset = *(UINT64 *)(UINTN)(BASE_4GB - 0x40);
  // Fit table is located in Top Swap region.
  // Adding check to validate Fit entry location
  if ((FitTableOffset < (BASE_4GB - PcdGet32 (PcdTopSwapRegionSize)))  ||  (FitTableOffset >  BASE_4GB)) {
    DEBUG ((DEBUG_INFO, "FitTableOffset 0x%x is not valid \n", FitTableOffset));
    return NULL;
  }

  FitEntry = (FIRMWARE_INTERFACE_TABLE_ENTRY *)(UINTN)FitTableOffset;

  if (FitEntry != NULL) {
    if (FitEntry[0].Address != *(UINT64 *)"_FIT_   ") {
      return NULL;
    }
    if (FitEntry[0].Type != FIT_TABLE_TYPE_HEADER) {
      return NULL;
    }
    EntryNum = *(UINT32 *)(&FitEntry[0].Size[0]) & 0xFFFFFF;
    for (Index = 0; Index < EntryNum; Index++) {
      if (FitEntry[Index].Type == Type) {
        return (VOID *)(UINTN)FitEntry[Index].Address;
      }
    }
  }

  return NULL;
}

/**
  Find the address of ACM.

  @return  A pointer to ACM.
**/
VOID *
FindAcm (
  VOID
  )
{
  return FindFitEntryData (FIT_TABLE_TYPE_STARTUP_ACM);
}

/**
  Find the address of Boot Policy Manifest.

  @return  A pointer to Key Manifest data structure.
**/
VOID *
FindBpm (
  VOID
  )
{
  return FindFitEntryData (FIT_TABLE_TYPE_BOOT_POLICY_MANIFEST);
}

/**
  Find the address of Key Manifest.

  @return  A pointer to Key Manifest data structure.
**/
VOID *
FindKm (
  VOID
  )
{
  return FindFitEntryData (FIT_TABLE_TYPE_KEY_MANIFEST);
}

/**
  Find BPM element by structureID

  @param[in] Bpm          A pointer to BPM data structure.
  @param[in] StructureId  BPM element StructureID

  @return  A pointer to BPM element data structure.
**/
VOID *
FindBpmElement (
  IN BOOT_POLICY_MANIFEST_HEADER *Bpm,
  IN UINT64                      StructureId
  )
{
  BOOT_POLICY_MANIFEST_HEADER             *BpmHeader;
  IBB_ELEMENT                             *IbbElement;
  TXT_ELEMENT                             *TxtElement;
  PLATFORM_CONFIG_DATA_ELEMENT            *PcdsElement;
  PLATFORM_MANUFACTURER_ELEMENT           *PmElement;
  BOOT_POLICY_MANIFEST_SIGNATURE_ELEMENT  *BpmSignatureElement;
  UINT8                                   *Buffer;

  Buffer = (UINT8 *)Bpm;

  BpmHeader = (BOOT_POLICY_MANIFEST_HEADER *)Buffer;
  if (*(UINT64 *)BpmHeader->StructureId != BOOT_POLICY_MANIFEST_HEADER_STRUCTURE_ID) {
    return NULL;
  }


  if (StructureId == BOOT_POLICY_MANIFEST_HEADER_STRUCTURE_ID) {
    return Buffer;
  }
  Buffer += sizeof(BOOT_POLICY_MANIFEST_HEADER);

  IbbElement = (IBB_ELEMENT *)Buffer;
  if (*(UINT64 *)IbbElement->StructureId != BOOT_POLICY_MANIFEST_IBB_ELEMENT_STRUCTURE_ID) {
    return NULL;
  }
  if (StructureId == BOOT_POLICY_MANIFEST_IBB_ELEMENT_STRUCTURE_ID) {
    return Buffer;
  }

  // Advance to end of IBB_ELEMENT structure
  Buffer = (UINT8*) &(IbbElement->PostIbbHash);
  Buffer += sizeof(UINT32) + IbbElement->PostIbbHash.Size;
  Buffer += sizeof(UINT32);  //entrypoint
  if (StructureId == BOOT_POLICY_MANIFEST_IBB_ELEMENT_DIGEST_ID) {
    return Buffer;
  }
  Buffer += sizeof(UINT32) + ((SHAX_HASH_STRUCTURE*)Buffer)->Size;  //digest
  Buffer += sizeof(UINT8) + (sizeof(IBB_SEGMENT) * (*Buffer));  //size * segmentcount

  // Do we have TXT element in BPM?
  // If so, advance to end of TXT_ELEMENT structure
  TxtElement = (TXT_ELEMENT *)Buffer;
  if (*(UINT64 *)TxtElement->StructureId == BOOT_POLICY_MANIFEST_TXT_ELEMENT_STRUCTURE_ID)
  {
    Buffer = (UINT8*) &(TxtElement->DigestList);
    Buffer += sizeof(UINT32) + TxtElement->DigestList.Size;
    Buffer += sizeof(UINT8) + (sizeof(IBB_SEGMENT) * (*Buffer));  //size * segmentcount
  }

  // Do we have Platform Config Data element in BPM?
  // If so, advance to end of PLATFORM_CONFIG_DATA_ELEMENT structure
  PcdsElement = (PLATFORM_CONFIG_DATA_ELEMENT *)Buffer;
  if (*(UINT64 *)PcdsElement->StructureId == BOOT_POLICY_MANIFEST_PLATFORM_CONFIG_DATA_ELEMENT_STRUCTURE_ID)
  {
    Buffer += 11 + PcdsElement->SizeOfData;
  }

  // Do we have Platform Manufacturer element in BPM?
  // If so, advance to end of PLATFORM_MANUFACTURER_ELEMENT structure
  PmElement = (PLATFORM_MANUFACTURER_ELEMENT *)Buffer;
  while (*(UINT64 *)PmElement->StructureId == BOOT_POLICY_MANIFEST_PLATFORM_MANUFACTURER_ELEMENT_STRUCTURE_ID) {
    if (StructureId == BOOT_POLICY_MANIFEST_PLATFORM_MANUFACTURER_ELEMENT_STRUCTURE_ID) {
      return Buffer;
    }
    Buffer += sizeof(PLATFORM_MANUFACTURER_ELEMENT) + PmElement->PmDataSize;
    PmElement = (PLATFORM_MANUFACTURER_ELEMENT *)Buffer;
  }

  BpmSignatureElement = (BOOT_POLICY_MANIFEST_SIGNATURE_ELEMENT *)Buffer;
  if (*(UINT64 *)BpmSignatureElement->StructureId != BOOT_POLICY_MANIFEST_SIGNATURE_ELEMENT_STRUCTURE_ID) {
    return NULL;
  }
  if (StructureId == BOOT_POLICY_MANIFEST_SIGNATURE_ELEMENT_STRUCTURE_ID) {
    return Buffer;
  }
  return NULL;
}

/**
  Find BPM IBB element.

  @param[in] Bpm  A pointer to BPM data structure.

  @return  A pointer to BPM IBB element data structure.
**/
VOID *
FindBpmIbb (
  IN BOOT_POLICY_MANIFEST_HEADER *Bpm
  )
{
  return FindBpmElement (Bpm, BOOT_POLICY_MANIFEST_IBB_ELEMENT_STRUCTURE_ID);
}

/**
  Check if Boot Guard verifies the IBB.

  @retval TRUE   It is VerifiedBoot
  @retval FALSE  It is NOT VerifiedBoot
**/
BOOLEAN
IsVerifiedBoot (
  VOID
  )
{
  if ((AsmReadMsr64 (MSR_BOOT_GUARD_SACM_INFO) & B_BOOT_GUARD_SACM_INFO_VERIFIED_BOOT) != 0) {
    return TRUE;
  } else {
    return FALSE;
  }
}

/**
  Check if Boot Guard measures IBB into TPM's PCRs.

  @retval TRUE   It is MeasuredBoot
  @retval FALSE  It is NOT MeasuredBoot
**/
BOOLEAN
IsMeasuredBoot (
  VOID
  )
{
  if ((AsmReadMsr64 (MSR_BOOT_GUARD_SACM_INFO) & B_BOOT_GUARD_SACM_INFO_MEASURED_BOOT) != 0) {
    return TRUE;
  } else {
    return FALSE;
  }
}

/**
  Get ACM Policy Status.

  @return ACM Policy Status
**/
UINT64
GetAcmPolicySts (
  VOID
  )
{
  return MmioRead64 (MMIO_ACM_POLICY_STATUS);
}

/**
  Check if we need AuthorityPCR measurement.

  @retval TRUE   Need AuthorityPCR measurement
  @retval FALSE  Do NOT need AuthorityPCR measurement
**/
BOOLEAN
NeedAuthorityMeasure (
  VOID
  )
{
  BOOT_POLICY_MANIFEST_HEADER *Bpm;
  IBB_ELEMENT                 *BpmIbb;

  Bpm = FindBpm ();
  ASSERT (Bpm != NULL);
  if (Bpm == NULL) return FALSE;

  BpmIbb = FindBpmIbb (Bpm);
  ASSERT (BpmIbb != NULL);
  if (BpmIbb == NULL) return FALSE;

  if ((BpmIbb->Flags & IBB_FLAG_AUTHORITY_MEASURE) != 0) {
    return TRUE;
  } else {
    return FALSE;
  }
}

/**
  Hash and log ACM extended event entry into the TCG Event Log.

  @param[in] DigestList    Pointer to a TPML_DIGEST_VALUES structure.
  @param[in] NewEventHdr   Pointer to a TCG_PCR_EVENT_HDR data structure.
  @param[in] NewEventData  Pointer to the new event data.

  @retval EFI_SUCCESS           The new event log entry was added.
  @retval EFI_OUT_OF_RESOURCES  No enough memory to log the new event.
**/
EFI_STATUS
LogAcmPcrExtendedEvent (
  IN      TPML_DIGEST_VALUES        *DigestList,
  IN      TCG_PCR_EVENT2_HDR        *NewEventHdr,
  IN      UINT8                     *NewEventData
  )
{
  UINT8                             *DigestBuffer;

  DigestBuffer = (UINT8 *)&NewEventHdr->Digests;
  DigestBuffer = CopyDigestListToBuffer (DigestBuffer, DigestList, PcdGet32(PcdMeasuredBootHashMask));
  CopyMem (DigestBuffer, &NewEventHdr->EventSize, sizeof (NewEventHdr->EventSize));
  DigestBuffer = DigestBuffer + sizeof (NewEventHdr->EventSize);
  CopyMem (DigestBuffer, NewEventData, NewEventHdr->EventSize);
  TpmLogEvent (NewEventHdr, (UINT8 *) NewEventData);

  return EFI_SUCCESS;
}

/**
   BIOS SHALL measure KM, BPM, and ACM_POLICY_STATUS register from locality 0.

   @param[in] ActivePcrBanks Active PCR Banks
   @param[in] AcmPolicyStatus ACM Policy status data
   @param[in] Km Pointer to the Key Manifest
   @param[in] Bpm Pointer to the Boot Policy Manifest
   @param[in] IbbHashPtr Pointer to the IBB hash list

   @retval     EFI_SUCCESS     The function completes successfully
   @retval     EFI_INVALID_PARAMETER Input pointers are invalid
   @retval     EFI_UNSUPPORTED Unable to parse unsupported hashing algorithm or signing scheme
**/
EFI_STATUS
CreatePolicyDataMeasurementEvent (
  IN UINT32  ActivePcrBanks,
  IN UINT64  AcmPolicyStatus,
  IN KEY_MANIFEST_STRUCTURE           *Km,
  IN BOOT_POLICY_MANIFEST_HEADER      *Bpm,
  IN IBB_ELEMENT                      *BpmIbb
  )
{

  EFI_STATUS                       Status = EFI_SUCCESS;
  UINT8                            *CurrPos;
  UINT32                           BpmSigSize = 0;
  UINT32                           KmSigSize = 0;
  UINT16                           KeyModulusSize;
  UINT16                           KeyStructSize;
  UINT32                           AcmPolicyDataSize = 0;
  MAX_ACM_POLICY_DATA              MaxAcmPolicyData;
  UINT8                            *AcmPolicyDataPtr;
  STATIC CONST CHAR8               PolicyDataMeasurementEventDataString[] = "BIOS Measured Boot Guard Policy\0";

  ASSERT (Km != NULL);
  if (Km == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  ASSERT (Bpm != NULL);
  if (Bpm == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  ASSERT (BpmIbb != NULL);
  if (BpmIbb == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  ZeroMem (&MaxAcmPolicyData, sizeof (MAX_ACM_POLICY_DATA));
  AcmPolicyDataPtr = (UINT8*) &MaxAcmPolicyData;

  ((ACM_POLICY_DATA *) AcmPolicyDataPtr)->AcmPolicyStatus = AcmPolicyStatus;
  DEBUG ((DEBUG_INFO, "AcmPolicySts  - 0x%04lx\n", ((ACM_POLICY_DATA *) AcmPolicyDataPtr)->AcmPolicyStatus));

  //
  // Mask out mutable policies from ACM_POLICY_STS
  //
  if ((BpmIbb->Flags & IBB_FLAG_SRTM_AC ) != 0) {
    ((ACM_POLICY_DATA *) AcmPolicyDataPtr)->AcmPolicyStatus &= SRTM_AC_MASK;
    DEBUG ((DEBUG_WARN, "ACM Policy Data Measurement: SRTM_AC_MASK is applied for AcmPolicySts.\n"));
    DEBUG ((DEBUG_WARN, "AcmPolicySts (MASKED)  - 0x%04lx\n", ((ACM_POLICY_DATA *) AcmPolicyDataPtr)->AcmPolicyStatus));
  }

  // Advance data pointers
  AcmPolicyDataSize = sizeof (((ACM_POLICY_DATA *) AcmPolicyDataPtr)->AcmPolicyStatus);
  AcmPolicyDataPtr += AcmPolicyDataSize;

  CurrPos = ((UINT8*) Km + Km->KeySignatureOffset);
  DEBUG ((DEBUG_INFO, "KmSignature offset:0x%04x\n", Km->KeySignatureOffset));

  if (((KEY_AND_SIGNATURE_STRUCT*) CurrPos)->KeyAlg == TPM_ALG_RSA) {
    // Advance the pointer to version (1 byte) and Key_alg (2 byte)
    CurrPos = (UINT8 *) (CurrPos + sizeof (UINT8) + sizeof (UINT16));
    KeyModulusSize = (((KEY_STRUCT_HEADER *) (UINT8*) CurrPos)->KeySizeBits)/8; // Modulus size in bytes
    KeyStructSize = sizeof (UINT8) + sizeof (UINT16) + sizeof (UINT32) + KeyModulusSize;
    CurrPos = CurrPos + KeyStructSize;

    // Here we have reached up to SigScheme
    if((*(UINT16*) CurrPos == TPM_ALG_RSASSA) || (*(UINT16*)CurrPos == TPM_ALG_RSAPSS)) {
      CurrPos += sizeof (UINT16);
      KmSigSize = (((SIGNATURE_STRUCT_HEADER*)CurrPos)->SigSizeBits)/8;
      CurrPos += sizeof (UINT8) + sizeof (UINT16) + sizeof (UINT16); // Sig version (1 byte) + sigsize (2 bytes) + hash_alg (2 bytes)

      // We have reached here to the signature data
      DEBUG ((DEBUG_INFO, "KmSignatureSize: 0x%x\n", KmSigSize));
      DEBUG ((DEBUG_INFO, "KmSignature:\n"));
      DumpHex (2, 0, KmSigSize, (VOID *)CurrPos);

      if ((AcmPolicyDataSize + KmSigSize) > sizeof (MAX_ACM_POLICY_DATA)) {
        DEBUG ((DEBUG_ERROR, "KM signature measurement: Error! Buffer Too Small.\n"));
        Status = EFI_BUFFER_TOO_SMALL;
        ASSERT_EFI_ERROR (Status);
        return Status;
      }

      // Copy Key manifest signature
      CopyMem (
        AcmPolicyDataPtr,
        CurrPos,
        KmSigSize);
    }

  } else {
    DEBUG ((DEBUG_ERROR, "Unsupported KM KeyAlg\n"));
    Status = EFI_UNSUPPORTED;
    ASSERT_EFI_ERROR (Status);
    return Status;
  }

  AcmPolicyDataSize += KmSigSize;
  AcmPolicyDataPtr += KmSigSize;

  CurrPos = ((UINT8*) Bpm + Bpm->KeySignatureOffset);
  DEBUG ((DEBUG_INFO, "BpmSignature offset:0x%04x\n", Bpm->KeySignatureOffset));

  if (((KEY_AND_SIGNATURE_STRUCT*) CurrPos)->KeyAlg == TPM_ALG_RSA) {

    // advance the pointer to version (1 byte) and Key_alg (2 byte)
    CurrPos = (UINT8 *) (CurrPos + sizeof (UINT8) + sizeof (UINT16));
    KeyModulusSize = (((KEY_STRUCT_HEADER *) (UINT8*) CurrPos)-> KeySizeBits)/8; //modulus size in bytes
    KeyStructSize = sizeof (UINT8) + sizeof (UINT16) + sizeof (UINT32) + KeyModulusSize;
    CurrPos = CurrPos + KeyStructSize;

    // Here we have reached up to SigScheme
    if ((*(UINT16 *) CurrPos == TPM_ALG_RSASSA) || (*(UINT16 *) CurrPos == TPM_ALG_RSAPSS)) {
      CurrPos += sizeof (UINT16);
      BpmSigSize = (((SIGNATURE_STRUCT_HEADER *) CurrPos)->SigSizeBits)/8;
      CurrPos += sizeof (UINT8) + sizeof (UINT16) + sizeof (UINT16) ; // Sig version (1 byte) + sigsize (2 bytes) + hash_alg (2 bytes)

      // We have reached here to the signature data
      DEBUG ((DEBUG_INFO, "BpmSignatureSize: 0x%x\n", BpmSigSize));
      DEBUG ((DEBUG_INFO, "BpmSignature:\n"));
      DumpHex (2, 0, BpmSigSize, (VOID *)CurrPos);


      if ((AcmPolicyDataSize + BpmSigSize) > sizeof (MAX_ACM_POLICY_DATA)) {
        DEBUG ((DEBUG_ERROR, "Bpm signature measurement: Error! Buffer Too Small.\n"));
        Status = EFI_BUFFER_TOO_SMALL;
        ASSERT_EFI_ERROR (Status);
        return Status;
      }

      CopyMem (
        AcmPolicyDataPtr,
        CurrPos,
        BpmSigSize);
    }

  } else {
    DEBUG ((DEBUG_ERROR, "Unsupported BPM KeyAlg\n"));
    Status = EFI_UNSUPPORTED;
    ASSERT_EFI_ERROR (Status);
    return Status;
  }

  AcmPolicyDataSize += BpmSigSize;

  //
  // Dump ACM Policy Data to be measured
  //
  DEBUG ((DEBUG_INFO, "ACM Policy Data blob:\n"));
  DumpHex (2, 0, AcmPolicyDataSize, (VOID *)&MaxAcmPolicyData);

  Status = TpmHashAndExtendPcrEventLog (
    0,
    (UINT8 *) &MaxAcmPolicyData,
    AcmPolicyDataSize,
    EV_POST_CODE,
    (UINT32) AsciiStrSize (PolicyDataMeasurementEventDataString),
    (UINT8 *) PolicyDataMeasurementEventDataString
    );

  return Status;
}

/**
   Create Locality Startup event entry

   The Startup Locality event should be placed in the log before any event which extends
   PCR[0]. This allows software which needs to parse the TCG Event Log to initialize its
   internal PCR[0] state correctly.

   @param[in] StartupLocality TPM Startup Locality
   @param[in] ActivePcrBanks Active PCR Banks

   @retval     EFI_SUCCESS  The function completes successfully
   @retval     EFI_INVALID_PARAMETER Input pointers are invalid
**/
EFI_STATUS
CreateLocalityStartupEvent (
  IN UINT8              StartupLocality,
  IN UINT32             ActivePcrBanks
  )
{

  EFI_STATUS                       Status = EFI_SUCCESS;
  TCG_PCR_EVENT2_HDR               NewEventHdr;
  TPML_DIGEST_VALUES               DigestList;
  TCG_EFI_STARTUP_LOCALITY_EVENT   LocalityEventData;
  STATIC CONST CHAR8               LocalityString[] = "StartupLocality\0";

  ZeroMem (&DigestList, sizeof (TPML_DIGEST_VALUES));

  if (((ActivePcrBanks & HASH_ALG_SHA1) != 0) &&
      (DigestList.count < HASH_COUNT)) {
    DEBUG ((DEBUG_ERROR, "Unsupported hashing algorithm: SHA1\n"));
  }

  if (((ActivePcrBanks & HASH_ALG_SHA256) != 0) &&
      (DigestList.count < HASH_COUNT)) {
    DigestList.digests[DigestList.count].hashAlg = TPM_ALG_SHA256;
    DigestList.count ++;
  }

  if (((ActivePcrBanks & HASH_ALG_SHA384) != 0) &&
      (DigestList.count < HASH_COUNT)) {
    DigestList.digests[DigestList.count].hashAlg = TPM_ALG_SHA384;
    DigestList.count ++;
  }

  if (((ActivePcrBanks & HASH_ALG_SHA512) != 0) &&
      (DigestList.count < HASH_COUNT)) {
    DigestList.digests[DigestList.count].hashAlg = TPM_ALG_SHA512;
    DigestList.count ++;
  }

  if (((ActivePcrBanks & HASH_ALG_SM3_256) != 0) &&
      (DigestList.count < HASH_COUNT)) {
    DigestList.digests[DigestList.count].hashAlg = TPM_ALG_SM3_256;
    DigestList.count ++;
  }

  NewEventHdr.PCRIndex  = 0;
  NewEventHdr.EventType = EV_NO_ACTION;
  NewEventHdr.EventSize = sizeof (TCG_EFI_STARTUP_LOCALITY_EVENT);

  CopyMem (LocalityEventData.Signature, LocalityString, AsciiStrSize (LocalityString));
  LocalityEventData.StartupLocality = StartupLocality;

  Status = LogAcmPcrExtendedEvent (&DigestList, &NewEventHdr, (UINT8 *) &LocalityEventData);
  return Status;
}

/**
   Recreate SCRTM Version extend event according to the requirements for EV_S_CRTM_VERSION event per
   PC Client PFP specification.

   S-ACM will perform a PCR0 extend operation from locality 3 containing the the ACM version.
   To disambiguate this extend and allow BIOS to reconstruct it, the order of entries in
   TPML_DIGEST_VALUES structure must be strictly specified in the ascending order of the TCG algorithm IDs:
   TCG algorithm IDs: SHA1 -> SHA256 -> SHA384 -> SHA512 -> SM3

   ACM version number:  NULL terminated UTF16 bit string. ACM revision (offset 20, 4 bytes) concatenated
   with ACM_SVN (offset 28, 2 bytes) converted to the UTF16 format

   @param[in] ActivePcrBanks Active PCR Banks
   @param[in] AcmHdr Pointer to the ACM header

   @retval     EFI_SUCCESS  The function completes successfully
   @retval     EFI_INVALID_PARAMETER Input pointers are invalid
**/
EFI_STATUS
CreateScrtmVersionEvent (
  IN UINT32                       ActivePcrBanks,
  IN ACM_HEADER_4                 *AcmHdr
  )
{
  EFI_STATUS Status = EFI_SUCCESS;

  UINT8   ByteCounter;
  UINT8   UnicodeCounter;

  UINT8   AcmHeaderDataBuffer[6] = {0};
  CHAR16  ConvertBuffer[3] = {'\0'};

  CHAR16  ScrtmUtf16String[SCRTM_VERSION_UTF16_LENGTH] = {'\0'};
  CHAR16  *UnicodeCharIterator = ScrtmUtf16String;

  TPML_DIGEST_VALUES  DigestList;
  TCG_PCR_EVENT2_HDR  NewEventHdr;

  ASSERT (AcmHdr != NULL);
  if (AcmHdr == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Read ACM revision (build date) - 4 bytes
  //
  CopyMem (
    &AcmHeaderDataBuffer[0],
    &((ACM_HEADER_4 *) AcmHdr)->Revision,
    4
    );

  //
  // Read ACM SVN - 2 bytes
  //
  CopyMem (
    &AcmHeaderDataBuffer[4],
    &((ACM_HEADER_4 *) AcmHdr)->AcmSvn,
    2
    );

  DumpHex (2, 0, 6, (VOID *)&AcmHeaderDataBuffer);

  for (ByteCounter = 0; ByteCounter < 6; ByteCounter++) {
    //
    // Fill convertion buffer with hex strings (with leading zeroes) per byte input.
    //
    UnicodeSPrint (
      ConvertBuffer,
      sizeof (CHAR16) * 3,
      L"%02X",
      AcmHeaderDataBuffer[ByteCounter]
      );

    if (UnicodeCharIterator >= (ScrtmUtf16String + SCRTM_VERSION_UTF16_LENGTH)) {
      DEBUG ((DEBUG_ERROR, "ERROR parsing ACM version string! %s\n", ScrtmUtf16String));
      ASSERT (UnicodeCharIterator < (ScrtmUtf16String + SCRTM_VERSION_UTF16_LENGTH));
      break;
    }
    //
    //  Keep 2 UTF-16 characters and trim NULL character from resulting hex strings
    //
    for (UnicodeCounter = 0; UnicodeCounter < 2; UnicodeCounter++) {
      *UnicodeCharIterator = (CHAR16) ConvertBuffer[UnicodeCounter];
      UnicodeCharIterator++;
    }
  }
  DEBUG ((DEBUG_INFO, "SCRTM version UTF-16 string: %s\n", ScrtmUtf16String));

  //
  // Hash digest value
  //
  ZeroMem (&DigestList, sizeof (TPML_DIGEST_VALUES));

  HashDataForSelectedPcrs (
    (UINT8 *) ScrtmUtf16String,
    sizeof (UINT16) * SCRTM_VERSION_UTF16_LENGTH,
    ActivePcrBanks,
    &DigestList);

  //
  // Create log event
  //
  NewEventHdr.PCRIndex  = 0;
  NewEventHdr.EventType = EV_S_CRTM_VERSION;
  NewEventHdr.EventSize = sizeof (UINT16) * SCRTM_VERSION_UTF16_LENGTH;
  Status = LogAcmPcrExtendedEvent (&DigestList, &NewEventHdr, (UINT8 *) ScrtmUtf16String);
  return Status;
}

/**
   Sort the input DigestList into the output SortedDigestList

   To disambiguate any extension performed by ACM and allow BIOS
   to reconstruct the event, order of entries in TPML_DIGEST_VALUES
   structure must be strictly specified in the ascending order of the
   TCG algorithm IDs: SHA1 -> SHA256 -> SHA384 -> SHA512 -> SM3

   @param[in]  DigestList        Input Digest list to sort
   @param[out] SortedDigestList  Output sorted Digest list
**/
VOID
SortDigestList (
  IN  TPML_DIGEST_VALUES *DigestList,
  OUT TPML_DIGEST_VALUES *SortedDigestList
  )
{
  UINT8 SortedAlgIdsSize = sizeof (SortedAlgIds) / sizeof (TPMI_ALG_HASH);
  UINT8 AlgIterator = 0;
  UINT8 SortedDigestCurrentPosition = 0;
  UINT8 InputIterator = 0;
  UINT8 MatchCount = 0;
  UINT32 DigestListSize = 0;

  if ((DigestList == NULL) || (SortedDigestList == NULL)) {
    return;
  }

  ZeroMem (SortedDigestList, sizeof (TPML_DIGEST_VALUES));

  DigestListSize = SortedDigestList->count = DigestList->count;

  //
  // Check that DigestListSize never exceeds the maximum supported digest count
  //
  if (DigestListSize > HASH_COUNT) {
    DEBUG ((DEBUG_ERROR, "DigestListSize exceeds maximum digest count supported!\n"));
    ASSERT (DigestListSize <= HASH_COUNT);
    return;
  }

  //
  // Iterate over TCG algorithms in specified ascending order.
  //
  for (AlgIterator = 0; ((AlgIterator < SortedAlgIdsSize) && (MatchCount < DigestListSize)); AlgIterator++) {
    //
    // Iterate over input digest list searching for a matching algorithm entry.
    //
    for (InputIterator = 0; InputIterator < DigestListSize; InputIterator++) {

      if (DigestList->digests[InputIterator].hashAlg != SortedAlgIds[AlgIterator]) {
        continue;
      }

      SortedDigestList->digests[SortedDigestCurrentPosition].hashAlg = DigestList->digests[InputIterator].hashAlg;
      CopyMem (
        &(SortedDigestList->digests[SortedDigestCurrentPosition].digest),
        &(DigestList->digests[InputIterator].digest),
        sizeof (TPMU_HA));

      SortedDigestCurrentPosition++;
      MatchCount++;
      break;
    }
  }
}


/**
  Create IBB Measurement event log

  All digests must be retrieved from BPM and placed into TPML_DIGEST_VALUES using the ascending
  order of the TCG algorithm IDs: SHA1 -> SHA256 -> SHA384 -> SHA512 -> SM3

  @param[in] ActivePcrBanks Active PCR Banks
  @param[in] Bpm Pointer to the Boot Policy Manifest
  @param[in] IbbHashPtr Pointer to the IBB hash list

  @retval     EFI_SUCCESS  The function completes successfully
  @retval     EFI_INVALID_PARAMETER Input pointers are invalid
**/
EFI_STATUS
CreateIbbMeasurementEvent (
  IN UINT32                       ActivePcrBanks,
  IN BOOT_POLICY_MANIFEST_HEADER  *Bpm,
  IN HASH_LIST                    *IbbHashPtr
  )
{
  EFI_STATUS                   Status = EFI_SUCCESS;
  UINT16                        Idx;
  UINT8                        *CurrPos;
  UINT16                       CurrHashAlg;
  UINT32                       CurrPcrBank;
  TPMI_ALG_HASH                CurrDigestAlg;
  TCG_PCR_EVENT2_HDR           NewEventHdr;
  UINT16                       IbbDigestSize;
  UINT16                       IbbDigestCount;
  TPML_DIGEST_VALUES           DigestList;
  TPML_DIGEST_VALUES           SortedDigestList;
  STATIC CONST CHAR8           IbbMeasurementEventDataString[] = "Boot Guard Measured IBB\0";

  CurrPcrBank = 0;
  CurrHashAlg =  0;
  ZeroMem (&DigestList, sizeof (TPML_DIGEST_VALUES));
  ZeroMem (&SortedDigestList, sizeof (TPML_DIGEST_VALUES));

  ASSERT (Bpm != NULL);
  if (Bpm == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  ASSERT (IbbHashPtr != NULL);
  if (IbbHashPtr == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  CurrPos = (UINT8 *)IbbHashPtr;
  IbbDigestCount = ((HASH_LIST*)CurrPos)->Count;
  DEBUG ((DEBUG_INFO, "IbbDigestCount = 0x%04x\n", IbbDigestCount));

  //
  // Advance past size and count field
  //
  CurrPos += sizeof (UINT16) + sizeof (UINT16);

  DEBUG ((DEBUG_INFO, "ActivePcrBanks = 0x%x\n", ActivePcrBanks));

  for (Idx = 0; Idx < IbbDigestCount ; Idx++) {
    CurrDigestAlg = ((SHAX_HASH_STRUCTURE *)CurrPos)->HashAlg;
    IbbDigestSize = ((SHAX_HASH_STRUCTURE *)CurrPos)->Size;

    DEBUG ((DEBUG_INFO, "Idx       = 0x%04x\n", Idx));
    DEBUG ((DEBUG_INFO, "Hash Alg  = 0x%04x\n", CurrDigestAlg));
    DEBUG ((DEBUG_INFO, "Hash Size = 0x%04x\n", IbbDigestSize));

    //
    // Advance to digest
    //
    CurrPos += sizeof (UINT16) + sizeof (UINT16);

    switch (CurrDigestAlg) {
      case TPM_ALG_SHA256:
        CurrPcrBank = HASH_ALG_SHA256;
        CurrHashAlg = TPM_ALG_SHA256;
        break;
      case TPM_ALG_SHA384:
        CurrPcrBank = HASH_ALG_SHA384;
        CurrHashAlg = TPM_ALG_SHA384;
        break;
      case TPM_ALG_SHA512:
        CurrPcrBank = HASH_ALG_SHA512;
        CurrHashAlg = TPM_ALG_SHA512;
        break;
      case TPM_ALG_SM3_256:
        CurrPcrBank = HASH_ALG_SM3_256;
        CurrHashAlg = TPM_ALG_SM3_256;
        break;
      default:
        DEBUG ((DEBUG_ERROR, "ERROR! Unsupported hashing algorithm (0x%04x) found in the BPM digests for an active PCR bank.\n", CurrDigestAlg));
        break;
    }

    if ((ActivePcrBanks & CurrPcrBank) != 0) {
      DigestList.digests[DigestList.count].hashAlg = CurrHashAlg;

      CopyMem (
        (UINT8 *)&(DigestList.digests[DigestList.count].digest),
        CurrPos,
        IbbDigestSize);

      DumpHex (2, 0, IbbDigestSize, (VOID *)CurrPos);
      DigestList.count++;
    }
    CurrPos += IbbDigestSize;
  }

  //
  // Sort Digest list to ensure ascending order of the TCG algorithm IDs
  //
  SortDigestList (
    &DigestList,
    &SortedDigestList
    );

  //
  // Create log event
  //
  NewEventHdr.PCRIndex  = 0;
  NewEventHdr.EventType = EV_POST_CODE;
  NewEventHdr.EventSize = (UINT32) AsciiStrSize (IbbMeasurementEventDataString);

  Status = LogAcmPcrExtendedEvent (&SortedDigestList, &NewEventHdr, (UINT8 *) IbbMeasurementEventDataString);
  return Status;
}


/**
  This function will handle all of the TCG event logging that
  depends on the Boot Guard configuration.

  It will call the functions required to recreate the ACM event measurements into
  the TCG log and for BIOS to extend the ACM policy data measurement to PCR[0].

  If the ACM policy data can't be retrieved for measurement, this function will
  attempt to measure an EV_SEPARATOR event to PCR[0].
**/
VOID
CreateTpmEventLog (
  IN TPM_TYPE TpmType
  )
{
  EFI_STATUS                    Status                  = EFI_SUCCESS;
  UINT8                         TpmStartupLocality      = LOCALITY_0_INDICATOR;

  UINT32                        ActivePcrBanks          = 0;
  UINT32                        TpmHashAlgorithmBitmap  = 0;
  UINT32                        EventData               = 0;

  ACM_HEADER_4                  *AcmHdr                 = NULL;
  KEY_MANIFEST_STRUCTURE        *Km                     = NULL;
  BOOT_POLICY_MANIFEST_HEADER   *Bpm                    = NULL;
  IBB_ELEMENT                   *BpmIbb                 = NULL;
  HASH_LIST                     *IbbHashPtr             = NULL;

  TCG_PCR_EVENT2_HDR            TcgEvent;
  ACM_BIOS_POLICY               AcmPolicySts;

  Status = Tpm2GetCapabilitySupportedAndActivePcrs (&TpmHashAlgorithmBitmap, &ActivePcrBanks);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Retrieval of PCRs failed!\n"));
  }
  AcmPolicySts.Data = GetAcmPolicySts ();

  if (AcmPolicySts.Bits.SCrtmStatus == 0) {
    DEBUG ((DEBUG_ERROR, "ACM SCRTM status is 0!\n"));
    return;
  }

  AcmHdr = FindAcm ();
  if (AcmHdr == NULL) {
    DEBUG ((DEBUG_ERROR, "ACM header not found!\n"));
    ASSERT (AcmHdr != NULL);
    Status = EFI_NOT_FOUND;
  }

  Km = FindKm ();
  if (Km == NULL) {
    DEBUG ((DEBUG_ERROR, "Key Manifest not found!\n"));
    ASSERT (Km != NULL);
    Status = EFI_NOT_FOUND;
  }

  Bpm = FindBpm ();
  if (Bpm == NULL) {
    DEBUG ((DEBUG_ERROR, "Boot Policy Manifest not found!\n"));
    ASSERT (Bpm != NULL);
    Status = EFI_NOT_FOUND;
  } else {
    BpmIbb = FindBpmIbb (Bpm);
    if (BpmIbb == NULL) {
      DEBUG ((DEBUG_ERROR, "IBB Element not found!\n"));
      ASSERT (BpmIbb != NULL);
      Status = EFI_NOT_FOUND;
    }

    IbbHashPtr = (HASH_LIST*) FindBpmElement (Bpm, BOOT_POLICY_MANIFEST_IBB_ELEMENT_DIGEST_ID);
    if (IbbHashPtr == NULL) {
      DEBUG ((DEBUG_ERROR, "IBB Hash not found!\n"));
      ASSERT (IbbHashPtr != NULL);
      Status = EFI_NOT_FOUND;
    }
  }

  if (AcmPolicySts.Bits.TpmStartupLocality == 0) {
    TpmStartupLocality = LOCALITY_3_INDICATOR;
  }

  Status = CreateLocalityStartupEvent (TpmStartupLocality, ActivePcrBanks);
  if(EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Creation of locality startup event failed!\n"));
  }

  Status = CreateScrtmVersionEvent (ActivePcrBanks, AcmHdr);
  if(EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Creation of SCRTM version event failed!\n"));
  }

  Status = CreateIbbMeasurementEvent (ActivePcrBanks, Bpm, IbbHashPtr);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Creation of IBB measurements event failed!\n"));

    EventData = 0x1;
    TcgEvent.PCRIndex  = 0;
    TcgEvent.EventType = EV_SEPARATOR;
    TcgEvent.EventSize = (UINT32) sizeof (EventData);

    Status = TpmHashAndExtendPcrEventLog (
             0,
             (UINT8 *) &EventData,
             sizeof (EventData),
             EV_SEPARATOR,
             TcgEvent.EventSize,
             (UINT8 *) &TcgEvent
             );

    if (!EFI_ERROR (Status)) {
      ASSERT_EFI_ERROR (Status);
      DEBUG ((DEBUG_ERROR, "Separator Event with Error not Measured. Error!\n"));
    }
    return;
  }

  Status = CreatePolicyDataMeasurementEvent (ActivePcrBanks, AcmPolicySts.Data, Km, Bpm, BpmIbb);
  if (EFI_ERROR (Status)) {
    DEBUG((DEBUG_ERROR, "Creation of policy data measurement event failed!\n"));
  }
}

/**
  Get SVN from an ACM.

  @param[in] Hdr          Acm Header.
  @param[out] Svn         Svn of Acm.

  @retval EFI_SUCCESS     The Svn was found for the given Acm
  @retval Others          An error occured in finding the given Acm Svn
**/
EFI_STATUS
EFIAPI
GetAcmSvnFromAcmHdr (
  IN  UINTN  Hdr,
  OUT UINT16 *Svn
  )
{
  ACM_HEADER_4  *AcmHdr;

  *Svn = 0;
  AcmHdr = (ACM_HEADER_4*) Hdr;

  if (AcmHdr == NULL) {
    return EFI_NOT_FOUND;
  }

  *Svn = AcmHdr->AcmSvn;
  return EFI_SUCCESS;
}

/**
  Get SVN from existing ACM.

  @retval EFI_SUCCESS     The Svn was found for the existing Acm
  @retval Others          An error occured in finding the existing Acm Svn
**/
EFI_STATUS
EFIAPI
GetExistingAcmSvn (
  OUT UINT16  *Svn
  )
{
  ACM_HEADER_4  *AcmHdr;

  AcmHdr = FindAcm ();
  return GetAcmSvnFromAcmHdr ((UINTN) AcmHdr, Svn);
}
