/** @file
  This file is sample code for Boot Guard TPM event log.

  Copyright (c) 2012 - 2023, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.


@par Specification Reference:

**/

#include <Uefi/UefiBaseType.h>
#include <Library/DebugLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/CryptoLib.h>
#include <Library/IoLib.h>
#include <BootGuardRegister.h>
#include <Library/TpmLib.h>
#include <Library/BootloaderCommonLib.h>
#include <Library/BootGuardLib.h>
#include <Library/SecureBootLib.h>
#include <Library/TpmLib/Tpm2CommandLib.h>

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

#define ACM_KEY_HASH_MMIO_ADDR_0  0xFED30400
#define ACM_KEY_HASH_MMIO_ADDR_1  (ACM_KEY_HASH_MMIO_ADDR_0 + 8)
#define ACM_KEY_HASH_MMIO_ADDR_2  (ACM_KEY_HASH_MMIO_ADDR_0 + 16)
#define ACM_KEY_HASH_MMIO_ADDR_3  (ACM_KEY_HASH_MMIO_ADDR_0 + 24)
#define ACM_PKCS_1_5_RSA_SIGNATURE_SIZE  256
#define ACM_MODULE_TYPE_CHIPSET_ACM      2
#define ACM_MODULE_SUBTYPE_CAPABLE_OF_EXECUTE_AT_RESET  0x1
#define ACM_MODULE_SUBTYPE_ANC_MODULE                   0x2
#define ACM_HEADER_FLAG_DEBUG_SIGNED     BIT15
#define ACM_NPW_SVN                      0x2
typedef struct {
  UINT16     ModuleType;
  UINT16     ModuleSubType;
  UINT32     HeaderLen;
  UINT32     HeaderVersion;
  UINT16     ChipsetId;
  UINT16     Flags;
  UINT32     ModuleVendor;
  UINT32     Date;
  UINT32     Size;
  UINT16     AcmSvn;
  UINT16     SeAcmSvn;
  UINT32     CodeControl;
  UINT32     ErrorEntryPoint;
  UINT32     GdtLimit;
  UINT32     GdtBasePtr;
  UINT32     SegSel;
  UINT32     EntryPoint;
  UINT8      Rsvd2[64];
  UINT32     KeySize;            // 64 DWORDS in the Key
  UINT32     ScratchSize;
  UINT8      Rsa2048PubKey[256];
  UINT32     RsaPubExp;
  UINT8      Rsa2048Sig[256];
  UINT8      Scratch[572];       // 143 DWORDS = 572 BYTES Scratch Size
} ACM_HEADER;

typedef struct {
  UINT16     ModuleType;
  UINT16     ModuleSubType;
  UINT32     HeaderLen;
  UINT32     HeaderVersion;
  UINT16     ChipsetId;
  UINT16     Flags;
  UINT32     ModuleVendor;
  UINT32     Date;
  UINT32     Size;
  UINT16     AcmSvn;
  UINT16     SeAcmSvn;
  UINT32     CodeControl;
  UINT32     ErrorEntryPoint;
  UINT32     GdtLimit;
  UINT32     GdtBasePtr;
  UINT32     SegSel;
  UINT32     EntryPoint;
  UINT8      Rsvd2[64];
  UINT32     KeySize; // 64
  UINT32     ScratchSize; // 2 * KeySize + 15
  UINT8      Rsa3072PubKey[384];
  UINT8      Rsa3072Sig[384];
  UINT8      Scratch[832];       // 208 DWORDS = 832 BYTES Scratch Size
} ACM_HEADER_3;

//
// BPM Policy:
//   FIT record type 12 points to Boot Policy Manifest.
//   FIT record type 11 points to Key Manifest.
//
#define RSA_KEY_SIZE_1K              1024
#define RSA_KEY_SIZE_2K              2048
#define RSA_KEY_SIZE_3K              3072
#define PKCS_1_5_RSA_SHA1_SIGNATURE_SIZE    (RSA_KEY_SIZE_1K / 8)
#define PKCS_1_5_RSA_SHA256_SIGNATURE_SIZE  (RSA_KEY_SIZE_2K / 8)
#define PKCS_1_5_RSA_SHA384_SIGNATURE_SIZE  (RSA_KEY_SIZE_3K / 8)
#define SHA1_DIGEST_SIZE             20
#define SHA256_DIGEST_SIZE  32
#define SHA384_DIGEST_SIZE           48
#define SM3_256_DIGEST_SIZE          32


typedef union {
    UINT8   Digest8[SHA256_DIGEST_SIZE];
    UINT32  Digest32[SHA256_DIGEST_SIZE / 4];
    UINT64  Digest64[SHA256_DIGEST_SIZE / 8];
} BTG_SHA256_DIGEST;

typedef union {
    UINT8   Digest8[SHA384_DIGEST_SIZE];
    UINT32  Digest32[SHA384_DIGEST_SIZE / 4];
    UINT64  Digest64[SHA384_DIGEST_SIZE / 8];
} BTG_SHA384_DIGEST;

typedef struct {
  UINT16               HashAlg;
  UINT16               Size;
  BTG_SHA256_DIGEST    HashBuffer;
} SHA256_HASH_STRUCTURE;
typedef struct {
  UINT16               HashAlg;
  UINT16               Size;
  BTG_SHA384_DIGEST    HashBuffer;
} SHA384_HASH_STRUCTURE;
typedef struct {
  UINT16               HashAlg;
  UINT16               Size;
  UINT8                HashBuffer[];
} SHAX_HASH_STRUCTURE;

typedef struct {
  UINT16               Size;           //Total number of bytes of HASH_LIST structure
  UINT16               Count;          //Number of Digest elements
} HASH_LIST;

typedef struct {
  UINT16                 Size;         //Total number of bytes of HASH_LIST structure
  UINT16                 Count;        //Number of Digest elements
  SHA256_HASH_STRUCTURE  Sha256Digest; //Array of digests  {AlgID, Size, HashValue}
  SHA256_HASH_STRUCTURE  ShaSm3Digest; //Array of digests  {AlgID, Size, HashValue}
  SHA384_HASH_STRUCTURE  Sha384Digest; //Array of digests  {AlgID, Size, HashValue}
} MAX_HASH_LIST;
typedef struct {
  UINT64               Usage;          // Bit mask of usages
  SHAX_HASH_STRUCTURE  Digest;         // Standard BtG hash structure primitive
} SHAX_KMHASH_STRUCT;
#define ALG_RSA             0x1
#define ALG_RSASSA          0x14

#define RSA_PUBLIC_KEY_STRUCT_VERSION_1_0  0x10
#define RSA_PUBLIC_KEY_STRUCT_KEY_EXPONENT_DEFAULT  0x11 // NOT 0x10001
typedef struct {
  UINT8  Version;
  UINT16   KeySizeBits;                // 1024 or 2048 or 3072 bits
} KEY_STRUCT_HEADER;
typedef struct {
  UINT8    Version;                    // 0x10
  UINT16   KeySizeBits;                // Number of bits in the modulus.
  UINT32   Exponent;
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
  UINT8    Modulus[1];                  // The modulus in LSB format
} RSA_PUBLIC_KEY_STRUCT;

#define ECC_PUBLIC_KEY_STRUCT_VERSION_1_0  0x10
#define ECC_PUBLIC_KEY_STRUCT_KEY_SIZE_DEFAULT  256
#define ECC_PUBLIC_KEY_STRUCT_KEY_LEN_DEFAULT   (ECC_PUBLIC_KEY_STRUCT_KEY_SIZE_DEFAULT/8)
typedef struct {
  UINT8  Version;
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
  UINT8                   Version;
  UINT16                  KeyAlg;
} KEY_AND_SIGNATURE_STRUCT_HEADER;

typedef union {
  RSA_PUBLIC_KEY_STRUCT    RsaKey;
  ECC_PUBLIC_KEY_STRUCT    EccKey;
} KEY_INFO;

typedef union {
  RSASSA_SIGNATURE_STRUCT  SignatureRsa;
  ECC_SIGNATURE_STRUCT     SignatureEcc;
} SIGNATURE_INFO;

typedef struct {
  UINT8                      Version;
  UINT16                     KeyAlg;       // TPM_ALG_RSA=0x1 or TPM_ALG_ECC=0x23
  KEY_INFO                   Key;
  UINT16                     SigScheme;
  SIGNATURE_INFO             Sig;
} KEY_AND_SIGNATURE_STRUCT;

#define BP_KEY_TYPE_BOOT_POLICY_MANIFEST  0
#define BP_KEY_TYPE_KEY_MANIFEST          1

#define BOOT_POLICY_MANIFEST_HEADER_STRUCTURE_ID  (*(UINT64 *)"__ACBP__")
#define BOOT_POLICY_MANIFEST_HEADER_VERSION_2_1          0x21
//#define BOOT_POLICY_MANIFEST_HEADER_STRUCTURE_VERSION_1_0   0x01
typedef struct {
  UINT8              StructureId[8];
  UINT8              StructVersion;
  UINT8              HdrStructVersion;
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
//#define IBB_ALIGNMENT            0x3F    // 64 bytes for BootGuard
typedef struct {
  UINT16             Reserved;         // Alignment
  UINT16 Flags;
  UINT32 Base;
  UINT32 Size;
} IBB_SEGMENT;

#define BOOT_POLICY_MANIFEST_IBB_ELEMENT_STRUCTURE_ID  (*(UINT64 *)"__IBBS__")
#define BOOT_POLICY_MANIFEST_IBB_ELEMENT_DIGEST_ID     (*(UINT64 *)"__DIGE__")
#define BOOT_POLICY_MANIFEST_IBB_ELEMENT_VERSION_2_0       0x20
//#define IBB_FLAG_ENABLE_VTD            0x1
#define IBB_FLAG_INITIAL_MEASURE_LOC3  0x2
#define IBB_FLAG_AUTHORITY_MEASURE     0x4
//#define IBB_ENTRYPOINT_MIN   0xFFFFFFC0
typedef struct {
  UINT8               StructureId[8];
  UINT8               StructVersion;
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
//  UINT32              IbbEntryPoint;
//  HASH_STRUCTURE      ObbHash;
//  UINT8               SegmentCount;
//  IBB_SEGMENT*        IbbSegment;  //IbbSegment[SegmentCount]
} IBB_ELEMENT;

#define BOOT_POLICY_MANIFEST_TXT_ELEMENT_STRUCTURE_ID  (*(UINT64 *)"__TXTS__")
#define BOOT_POLICY_MANIFEST_TXT_ELEMENT_VERSION_2_0       0x20
typedef struct {
  UINT8               StructureId[8];
  UINT8               StructVersion;
  UINT8               Reserved0;
  UINT16              ElementSize;     // Total number of bytes in the element
  UINT8               Reserved1;
  UINT8               SetType;
  UINT16              Reserved;           // 1 //
  UINT32              Flags;
  UINT16              PwrDownInterval;
  UINT8               PttCmosOffset0;
  UINT8               PttCmosOffset1;
  UINT16              AcpiBaseOffset;
  UINT16              Reserved2;
  UINT32              PrwmBaseOffset;
  HASH_LIST           DigestList;
  UINT8               Reserved3[3];
  UINT8               SegmentCount; // 1 ~ 8
  IBB_SEGMENT*        TxtSegment;      // TxtSegment[SegmentCount]
} TXT_ELEMENT;

#define BOOT_POLICY_MANIFEST_PLATFORM_CONFIG_DATA_ELEMENT_STRUCTURE_ID  (*(UINT64 *)"__PCDS__")
#define BOOT_POLICY_MANIFEST_PLATFORM_CONFIG_DATA_ELEMENT_VERSION_2_0       0x20
typedef struct {
  UINT8    StructureId[8];
  UINT8    StructVersion;
  UINT8    Reserved0;
  UINT16   ElementSize;     // Total number of bytes in the element
  UINT16   Reserved1;
  UINT16   SizeOfData;
  UINT8*   Data;  //Data[SizeofData]  // Any data but starts from PDRS
//
//
} PLATFORM_CONFIG_DATA_ELEMENT;

#define BOOT_POLICY_MANIFEST_PLATFORM_MANUFACTURER_ELEMENT_STRUCTURE_ID  (*(UINT64 *)"__PMDA__")
#define BOOT_POLICY_MANIFEST_PLATFORM_MANUFACTURER_ELEMENT_VERSION_2_0       0x20
//#define PLATFORM_DATA_ALIGNMENT  0x3  // 4 bytes
typedef struct {
  UINT8    StructureId[8];
  UINT8    StructVersion;
  UINT8    Reserved0;
  UINT16   ElementSize;     // Total number of bytes in the element
  UINT16   Reserved1;
  UINT16   PmDataSize;
  UINT8*   PmData; //PmData[PmDataSize]
} PLATFORM_MANUFACTURER_ELEMENT;

#define BOOT_POLICY_MANIFEST_SIGNATURE_ELEMENT_STRUCTURE_ID  (*(UINT64 *)"__PMSG__")
#define BOOT_POLICY_MANIFEST_SIGNATURE_ELEMENT_VERSION_1_0       0x10
typedef struct {
  UINT8                StructureId[8];
  UINT8                StructVersion;
  UINT8                Reserved[3];         // KeySignature must be DWORD aligned
  KEY_AND_SIGNATURE_STRUCT  KeySignature;  // this is now a variable Size
} BOOT_POLICY_MANIFEST_SIGNATURE_ELEMENT;

#define KEY_MANIFEST_STRUCTURE_ID  (*(UINT64 *)"__KEYM__")
#define KEY_MANIFEST_STRUCTURE_VERSION_2_1     0x21
typedef struct {
  UINT8                  StructureId[8];
  UINT8                  StructVersion;
  UINT8                  Reserved[3];         // 3 bytes to make KeySignatureOffset same offset as for BPM
  UINT16                 KeySignatureOffset;  // Offset from start of KM to KeyManifestSignature
  UINT8                  Reserved2[3];        // Alignment
  UINT8                  KeyManifestRevision;
  UINT8                  KmSvn;
  UINT8                  KeyManifestId;
  UINT16                 KmPubKeyHashAlg;
  UINT16                 KeyCount;
  //  KEY_AND_SIGNATURE_STRUCT  KeyManifestSignature;
} KEY_MANIFEST_STRUCTURE;

//
// DetailPCR data
//
typedef struct {
  UINT64 AcmPolicySts;
  UINT16 AcmSvn;                       // ACM_SVN from ACM Header
  UINT8  AcmRsaSignature[];            // copy from ACM.HEADER.RSASIG
  //  UINT8  KmRsaSignature[];         // copy from KM signature
  //  UINT8  BpmRsaSignature[];        // copy from BPM signature
  //  UINT8  IbbHash[];                // hash value of IBB
} DETAIL_PCR_DATA;

typedef struct {
  UINT64  AcmPolicySts;              // lower 8 bits of the BP.RSTR
  UINT16  AcmSvn;                    // ACM_SVN from ACM Header
  //buffer size: Max AcmRSASignature + KmRSASignature + BpmRSASignature + IbbHash
  UINT8   Buffer[RSA_KEY_SIZE_3K/8 + RSA_KEY_SIZE_3K/8 + RSA_KEY_SIZE_3K/8 + SHA384_DIGEST_SIZE ] ;
} MAX_DETAIL_PCR_DATA;

//
// AuthorityPCR data
//
typedef struct {
  UINT64 AcmPolicySts;
  UINT16 AcmSvn;
  UINT8  AcmKeyHash[];                   // The hash of the key used to verify the ACM (SHAxxx)
//  UINT8  BpKeyHash[];                  // The hash of the key used to verify the Key Manifest (SHAxxx)
//  UINT8  BpmKeyHashFromKm[];           // The hash of the key used to verify the Boot Policy Manifest (SHAxxx)
} AUTHORITY_PCR_DATA;

typedef struct {
  UINT64 AcmPolicySts;
  UINT16 AcmSvn;                               // ACM_SVN from ACM Header
  UINT8  AcmKeyHash[SHA384_DIGEST_SIZE];       // The hash of the key used to verify the ACM (SHAxxx)
  UINT8  BpKeyHash[SHA384_DIGEST_SIZE];        // The hash of the key used to verify the Key Manifest (SHAxxx)
  UINT8  BpmKeyHashFromKm[SHA384_DIGEST_SIZE]; // The hash of the key used to verify the Boot Policy Manifest (SHAxxx)
} MAX_AUTHORITY_PCR_DATA;
//
// Boot Policy Restrictions definition
//
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


#pragma pack ()


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
  Check if ACM is a NPW ACM.

  @retval TRUE   It is a NPW ACM
  @retval FALSE  It is NOT a NPW ACM
**/
BOOLEAN
IsNpwAcm (
  VOID
  )
{
  ACM_HEADER  *Acm;

  Acm = FindAcm ();
  ASSERT (Acm != NULL);
  if (Acm == NULL) {
    return FALSE;
  }

  if (((Acm->Flags & ACM_HEADER_FLAG_DEBUG_SIGNED) == 0) && (Acm->AcmSvn < ACM_NPW_SVN)) {
    return TRUE;
  } else {
    return FALSE;
  }
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
  Calculate DetailPCR extend value.

  @param[out] Digest  DetailPCR digest
**/
BOOLEAN
CaculateDetailPCRExtendValue (
  IN UINT32  ActivePcrBanks,
  OUT UINT8 *Sha1Digest,
  OUT UINT8 *Sha256Digest,
  OUT UINT8 *Sha384Digest,
  OUT UINT8 *Sm3Digest
  )
{
  ACM_HEADER                       *Acm;
  KEY_MANIFEST_STRUCTURE           *Km;
  BOOT_POLICY_MANIFEST_HEADER      *Bpm;
  IBB_ELEMENT                      *BpmIbb;
  MAX_DETAIL_PCR_DATA              MaxDetailPcrData;
  UINT8                            *DetailPcrDataPtr;
  UINT32                           AcmSigSize;
  UINT32                           KmSigSize;
  UINT32                           BpmSigSize;
  UINT32                           DetailPcrDataSize;
  UINT32                           SavedDetailPcrDataSize;
  UINT8                            *CurrPos;
  UINT16                           KeyStructSize;
  UINT16                           KeyModulusSize;
  UINT16                           IbbDigestCount;
  UINT16                           IbbDigestSize;
  UINT8                            Idx;
  HASH_LIST                        *IbbHashPtr;

  AcmSigSize = 0;
  KmSigSize  = 0;
  BpmSigSize = 0;

  Acm = FindAcm ();
  ASSERT (Acm != NULL);
  if (Acm == NULL) return FALSE;

  Km = FindKm ();
  ASSERT (Km != NULL);
  if (Km == NULL) return FALSE;


  Bpm = FindBpm ();
  ASSERT (Bpm != NULL);
  if (Bpm == NULL) return FALSE;

  BpmIbb = FindBpmIbb (Bpm);
  ASSERT (BpmIbb != NULL);
  if (BpmIbb == NULL) return FALSE;

  IbbHashPtr = (HASH_LIST*)FindBpmElement(Bpm, BOOT_POLICY_MANIFEST_IBB_ELEMENT_DIGEST_ID);
  ASSERT (IbbHashPtr != NULL);
  if (IbbHashPtr == NULL) return FALSE;

  DetailPcrDataPtr = (UINT8*)&MaxDetailPcrData;

  DEBUG ((DEBUG_INFO, "DetailPcrData:\n"));

  // 1. Get ACM Policy Status
  ((DETAIL_PCR_DATA*)DetailPcrDataPtr)->AcmPolicySts = GetAcmPolicySts ();
  DEBUG ((DEBUG_INFO, "AcmPolicySts  - 0x%04lx\n", ((DETAIL_PCR_DATA*)DetailPcrDataPtr)->AcmPolicySts));

  // 2. Get ACM SVN
  ((DETAIL_PCR_DATA*)DetailPcrDataPtr)->AcmSvn = Acm->AcmSvn;
  DEBUG ((DEBUG_INFO, "AcmSvn        - 0x%04x\n", ((DETAIL_PCR_DATA*)DetailPcrDataPtr)->AcmSvn));

  // 3. Get ACM Signature
  DetailPcrDataSize = sizeof(((DETAIL_PCR_DATA*)DetailPcrDataPtr)->AcmPolicySts) + sizeof(((DETAIL_PCR_DATA*)DetailPcrDataPtr)->AcmSvn);
  DetailPcrDataPtr += DetailPcrDataSize;
  if (((ACM_HEADER *) Acm)->HeaderVersion == 0) {
    AcmSigSize = RSA_KEY_SIZE_2K / 8;
    CopyMem ( DetailPcrDataPtr, (UINT8 *) ((ACM_HEADER *) Acm)->Rsa2048Sig, AcmSigSize);
  }
  else {
    AcmSigSize = RSA_KEY_SIZE_3K / 8;
    CopyMem ( DetailPcrDataPtr, (UINT8 *) ((ACM_HEADER_3 *) Acm)->Rsa3072Sig, AcmSigSize);
  }
  DEBUG ((DEBUG_INFO, "AcmRsaSignature:  AcmSigSize= 0x%04x\n", AcmSigSize));
  DumpHex (2, 0, AcmSigSize, DetailPcrDataPtr);

  // 4. Get BtGuard KM Signature
  DetailPcrDataSize += AcmSigSize;
  DetailPcrDataPtr += AcmSigSize;
  CurrPos = ((UINT8*)Km + Km->KeySignatureOffset);
  if (((KEY_AND_SIGNATURE_STRUCT*)CurrPos)->KeyAlg == TPM_ALG_RSA) {

    CurrPos += sizeof(UINT8) + sizeof(UINT16);
    KeyModulusSize = (((KEY_STRUCT_HEADER *)(UINT8*)CurrPos)->KeySizeBits)/8;
    KeyStructSize = sizeof(UINT8) + sizeof(UINT16) + sizeof(UINT32) + KeyModulusSize;
    CurrPos = CurrPos + KeyStructSize;

    // Here we have reached up to SigScheme
    if((*(UINT16*)CurrPos == TPM_ALG_RSAPSS) || (*(UINT16*)CurrPos == TPM_ALG_RSASSA))  {
      CurrPos += sizeof(UINT16);
      KmSigSize = (((SIGNATURE_STRUCT_HEADER*)CurrPos)->SigSizeBits)/8;
      CurrPos += sizeof(UINT8) + sizeof(UINT16) + sizeof(UINT16);
      CopyMem ( DetailPcrDataPtr, CurrPos, KmSigSize);
      DEBUG ((DEBUG_INFO, "KmSignature:  KmSigSize= 0x%04x\n", KmSigSize));
      DumpHex (2, 0, KmSigSize, CurrPos);
    }
  } else {
    DEBUG ((DEBUG_ERROR, "KmSignature: Unsupported KeyAlg\n"));
    return FALSE;
  }

  DetailPcrDataSize += KmSigSize;
  DetailPcrDataPtr += KmSigSize;

  // 5. Get BPM Signature
  CurrPos = NULL;
  CurrPos = ((UINT8*)Bpm + Bpm->KeySignatureOffset);
  DEBUG ((DEBUG_INFO, "BpmSignatureoffset:0x%04x\n", Bpm->KeySignatureOffset));
  if(((KEY_AND_SIGNATURE_STRUCT*)CurrPos)->KeyAlg == TPM_ALG_RSA) {
    CurrPos = (UINT8 *) CurrPos + sizeof(UINT8) + sizeof(UINT16);
    KeyModulusSize = (((KEY_STRUCT_HEADER *)(UINT8*) CurrPos)-> KeySizeBits)/8;
    KeyStructSize = sizeof(UINT8) + sizeof(UINT16) + sizeof(UINT32) + KeyModulusSize;
    CurrPos = CurrPos + KeyStructSize;
    if((*(UINT16*)CurrPos == TPM_ALG_RSAPSS) || (*(UINT16*)CurrPos == TPM_ALG_RSASSA))  {
      CurrPos += sizeof(UINT16);
      BpmSigSize = (((SIGNATURE_STRUCT_HEADER*)CurrPos)->SigSizeBits)/8;
      CurrPos += sizeof(UINT8) + sizeof(UINT16) + sizeof(UINT16) ;
      DEBUG ((DEBUG_INFO, "BpmSignature:  BpmSigSize= 0x%04x\n", BpmSigSize));
      DumpHex (2, 0, BpmSigSize, CurrPos);
      CopyMem ( DetailPcrDataPtr, CurrPos, BpmSigSize);
    }
  } else {
    DEBUG ((DEBUG_INFO, "BpmSignature : Unsupported KeyAlg\n"));
  }

  DetailPcrDataSize += BpmSigSize;
  DetailPcrDataPtr += BpmSigSize;

  // 5. Get IBB digest
  CurrPos = (UINT8 *)IbbHashPtr;
  SavedDetailPcrDataSize = DetailPcrDataSize;
  IbbDigestCount = ((HASH_LIST*)CurrPos)->Count;
  DEBUG ((DEBUG_INFO, "IbbDigestCount = 0x%04x\n", IbbDigestCount));
  CurrPos += sizeof(UINT16) + sizeof(UINT16);
  for(Idx = 0; Idx < IbbDigestCount ; Idx++) {
    DEBUG ((DEBUG_INFO, "Idx = 0x%04x\n", Idx));
    DEBUG ((DEBUG_INFO, "ActivePcrBanks = 0x%x\n", ActivePcrBanks));

    if(((SHAX_HASH_STRUCTURE *)CurrPos)->HashAlg ==TPM_ALG_SHA1) {
      IbbDigestSize =  SHA1_DIGEST_SIZE;
      CurrPos += sizeof(UINT16) + sizeof(UINT16);
      CurrPos += IbbDigestSize;
    }
    if(((SHAX_HASH_STRUCTURE *)CurrPos)->HashAlg ==TPM_ALG_SHA256) {
      IbbDigestSize =  SHA256_DIGEST_SIZE;
      CurrPos += sizeof(UINT16) + sizeof(UINT16);

      if ((ActivePcrBanks & HASH_ALG_SHA256) != 0) {
        CopyMem ( DetailPcrDataPtr, CurrPos,  IbbDigestSize);
        DEBUG ((DEBUG_INFO, "IbbDigestSize:  %d\n",IbbDigestSize));
        DumpHex (2, 0, IbbDigestSize, CurrPos);
        DetailPcrDataSize = SavedDetailPcrDataSize + IbbDigestSize;
        CalculateHash ((UINT8 *)&MaxDetailPcrData, DetailPcrDataSize, HASH_TYPE_SHA256, (UINT8 *)Sha256Digest);
      }

      CurrPos += IbbDigestSize;
    }
    if(((SHAX_HASH_STRUCTURE *)CurrPos)->HashAlg ==TPM_ALG_SHA384) {
      IbbDigestSize =  SHA384_DIGEST_SIZE;
      CurrPos += sizeof(UINT16) + sizeof(UINT16);

      if ((ActivePcrBanks & HASH_ALG_SHA384) != 0) {
        CopyMem ( DetailPcrDataPtr, CurrPos,  IbbDigestSize);
        DEBUG ((DEBUG_INFO, "IbbDigestSize:  %d\n",IbbDigestSize));
        DumpHex (2, 0, IbbDigestSize, CurrPos);
        DetailPcrDataSize = SavedDetailPcrDataSize + IbbDigestSize;
        CalculateHash ((UINT8 *)&MaxDetailPcrData, DetailPcrDataSize, HASH_TYPE_SHA384, (UINT8 *)Sha384Digest);
      }

      CurrPos += IbbDigestSize;
    }
    if(((SHAX_HASH_STRUCTURE *)CurrPos)->HashAlg ==TPM_ALG_SM3_256) {
      IbbDigestSize =  SM3_DIGEST_SIZE;
      CurrPos += sizeof(UINT16) + sizeof(UINT16);

      if ((ActivePcrBanks & HASH_ALG_SM3_256) != 0) {
        CopyMem ( DetailPcrDataPtr, CurrPos,  IbbDigestSize);
        DEBUG ((DEBUG_INFO, "IbbDigestSize:  %d\n",IbbDigestSize));
        DumpHex (2, 0, IbbDigestSize, CurrPos);
        DetailPcrDataSize = SavedDetailPcrDataSize + IbbDigestSize;
        CalculateHash ((UINT8 *)&MaxDetailPcrData, DetailPcrDataSize, HASH_TYPE_SM3, (UINT8 *)Sm3Digest);
      }

      CurrPos += IbbDigestSize;
    }
  }

  return TRUE;
}

/**
  Calculate AuthorityPCR extend value.

  @param[out] Digest  AuthorityPCR digest
**/
BOOLEAN
CaculateAuthorityPCRExtendValue (
  OUT TPMU_HA *Digest
  )
{
  ACM_HEADER                               *Acm;
  KEY_MANIFEST_STRUCTURE                   *Km;
  BOOT_POLICY_MANIFEST_HEADER              *Bpm;
  MAX_AUTHORITY_PCR_DATA                   MaxAuthorityPcrData;
  UINT8                                    *AuthorityPcrDataPtr;
  UINT32                                   AuthorityPcrDataSize;
  UINT8                                    *CurrPos;
  UINT16                                   KeyModulusSize;
  UINT8                                    MaxModulusExpo[RSA_KEY_SIZE_3K/8 + 4];
  UINT8                                    Index;

  Acm = FindAcm ();
  ASSERT (Acm != NULL);
  if (Acm == NULL) return FALSE;

  Km = FindKm ();
  ASSERT (Km != NULL);
  if ((Km == NULL) ||
      (Km->StructVersion != KEY_MANIFEST_STRUCTURE_VERSION_2_1)) {
      DEBUG ((DEBUG_ERROR, "Unsupported KM Struct Version: 0x%02x\n", Km->StructVersion));
      return FALSE;
  }

  Bpm = FindBpm ();
  ASSERT (Bpm != NULL);
  if (Bpm == NULL) return FALSE;

  AuthorityPcrDataPtr = (UINT8*)&MaxAuthorityPcrData;

  DEBUG ((DEBUG_INFO, "AuthorityPcrData:\n"));

  // 1. Get ACM Policy Status
  ((AUTHORITY_PCR_DATA*)AuthorityPcrDataPtr)->AcmPolicySts = GetAcmPolicySts ();
  DEBUG ((DEBUG_INFO, "AcmPolicySts  - 0x%04lx\n", ((AUTHORITY_PCR_DATA*)AuthorityPcrDataPtr)->AcmPolicySts));

  // 2. Get ACM SVN
  ((AUTHORITY_PCR_DATA*)AuthorityPcrDataPtr)->AcmSvn = Acm->AcmSvn;
  DEBUG ((DEBUG_INFO, "AcmSvn        - 0x%04x\n", ((AUTHORITY_PCR_DATA*)AuthorityPcrDataPtr)->AcmSvn));

  // 3. Get SHA256 hash of the public key used for signing ACM
  *(UINT64*)&(((AUTHORITY_PCR_DATA*)AuthorityPcrDataPtr)->AcmKeyHash[0])  = MmioRead64 (ACM_KEY_HASH_MMIO_ADDR_0);
  *(UINT64*)&(((AUTHORITY_PCR_DATA*)AuthorityPcrDataPtr)->AcmKeyHash[8])  = MmioRead64 (ACM_KEY_HASH_MMIO_ADDR_1);
  *(UINT64*)&(((AUTHORITY_PCR_DATA*)AuthorityPcrDataPtr)->AcmKeyHash[16]) = MmioRead64 (ACM_KEY_HASH_MMIO_ADDR_2);
  *(UINT64*)&(((AUTHORITY_PCR_DATA*)AuthorityPcrDataPtr)->AcmKeyHash[24]) = MmioRead64 (ACM_KEY_HASH_MMIO_ADDR_3);

  DEBUG ((DEBUG_INFO, "AcmKeyHash:  \n"));
  DumpHex (2, 0, SHA256_DIGEST_SIZE, ((AUTHORITY_PCR_DATA*)AuthorityPcrDataPtr)->AcmKeyHash);

  AuthorityPcrDataSize = sizeof(((AUTHORITY_PCR_DATA*)AuthorityPcrDataPtr)->AcmPolicySts) + sizeof(((AUTHORITY_PCR_DATA*)AuthorityPcrDataPtr)->AcmSvn)
      + SHA256_DIGEST_SIZE;
  AuthorityPcrDataPtr += AuthorityPcrDataSize;

  // 4. Get SHA256 hash of the public key used for signing BtGuard KM
  CurrPos = ((UINT8*)Km + Km->KeySignatureOffset);
  if (((KEY_AND_SIGNATURE_STRUCT*)CurrPos)->KeyAlg == TPM_ALG_RSA) {

    CurrPos += sizeof(UINT8) + sizeof(UINT16);
    KeyModulusSize = (((KEY_STRUCT_HEADER *)(UINT8*)CurrPos)->KeySizeBits)/8;

    CopyMem ( MaxModulusExpo, (UINT8 *)((RSA_PUBLIC_KEY_STRUCT *)(UINT8*)CurrPos)->Modulus, KeyModulusSize);
    CopyMem ( MaxModulusExpo + KeyModulusSize, (UINT8 *)&(((RSA_PUBLIC_KEY_STRUCT *)(UINT8*)CurrPos)->Exponent), 4);
    Sha256 (MaxModulusExpo, KeyModulusSize + 4, (UINT8 *)AuthorityPcrDataPtr);

    DEBUG ((DEBUG_INFO, "BtG Key Hash:  \n"));
    DumpHex (2, 0, SHA256_DIGEST_SIZE, AuthorityPcrDataPtr);
    AuthorityPcrDataPtr += SHA256_DIGEST_SIZE;
    AuthorityPcrDataSize += SHA256_DIGEST_SIZE;
  } else {
    DEBUG ((DEBUG_ERROR, "KmSignature: Unsupported KeyAlg\n"));
    return FALSE;
  }

  // 4. Get SHA256 hash of the public key used for signing Boot Policy Manifest
  CurrPos = (UINT8*)Km + sizeof(KEY_MANIFEST_STRUCTURE);
  for(Index = 0; Index < Km->KeyCount; Index++) {
    SHAX_KMHASH_STRUCT      *KmHash;
    SHAX_HASH_STRUCTURE     *ShaxHash;

    KmHash = (SHAX_KMHASH_STRUCT*) (CurrPos);
    ShaxHash = (SHAX_HASH_STRUCTURE*) &(KmHash->Digest);

    if ((KmHash->Usage & 0x1) != 0) {
      CopyMem ( AuthorityPcrDataPtr, ShaxHash->HashBuffer,  ShaxHash->Size);
      DEBUG ((DEBUG_INFO, "BPM Key Hash:  \n"));
      DumpHex (2, 0, SHA256_DIGEST_SIZE, AuthorityPcrDataPtr);
      AuthorityPcrDataPtr += ShaxHash->Size;
      AuthorityPcrDataSize += ShaxHash->Size;
    }
    CurrPos += sizeof(SHAX_KMHASH_STRUCT) + ShaxHash->Size;
  }

  Sha256 ((UINT8 *)&MaxAuthorityPcrData, AuthorityPcrDataSize, (UINT8 *)Digest);

  DEBUG ((DEBUG_INFO, "AuthorityPCR (PCR7): Hash extended by ACM:  \n"));
  DumpHex (2, 0, SHA256_DIGEST_SIZE, (UINT8*)Digest);

  return TRUE;
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
  Create DetailPCR event log.

  @param[in] ActivePcrBanks   Active PCR banks
**/
VOID
CreateDetailPcrEvent (
  IN UINT32   ActivePcrBanks
  )
{
  TCG_PCR_EVENT2_HDR        NewEventHdr;
  UINT8                     Sha1[SHA1_DIGEST_SIZE];
  UINT8                     Sha256[SHA256_DIGEST_SIZE];
  UINT8                     Sha384[SHA384_DIGEST_SIZE];
  UINT8                     Sm3[SM3_256_DIGEST_SIZE];
  TPML_DIGEST_VALUES        *Digests;

  DEBUG ((DEBUG_INFO, "Adding DetailPCR Event in TCG Event Log.\n"));

  NewEventHdr.PCRIndex  = 0;
  NewEventHdr.EventType = EV_S_CRTM_CONTENTS;
  Digests = &NewEventHdr.Digests;
  CaculateDetailPCRExtendValue (ActivePcrBanks, Sha1, Sha256, Sha384, Sm3);
  DEBUG ((DEBUG_INFO, "CreateDetailPCREvent()\n"));
  ZeroMem (Digests, sizeof(TPML_DIGEST_VALUES));
  if ((ActivePcrBanks & HASH_ALG_SHA1) != 0) {
    Digests->digests[Digests->count].hashAlg = TPM_ALG_SHA1;
    CopyMem (Digests->digests[Digests->count].digest.sha1, Sha1, SHA1_DIGEST_SIZE);
    Digests->count ++;
  }
  if ((ActivePcrBanks & HASH_ALG_SHA256) != 0) {
    Digests->digests[Digests->count].hashAlg = TPM_ALG_SHA256;
    CopyMem (Digests->digests[Digests->count].digest.sha256, Sha256, SHA256_DIGEST_SIZE);
    Digests->count ++;
  }
  if ((ActivePcrBanks & HASH_ALG_SHA384) != 0) {
    Digests->digests[Digests->count].hashAlg = TPM_ALG_SHA384;
    CopyMem (Digests->digests[Digests->count].digest.sha384, Sha384, SHA384_DIGEST_SIZE);
    Digests->count ++;
  }
  if ((ActivePcrBanks & HASH_ALG_SM3_256) != 0) {
    Digests->digests[Digests->count].hashAlg = TPM_ALG_SM3_256;
    CopyMem (Digests->digests[Digests->count].digest.sm3_256, Sm3, SM3_256_DIGEST_SIZE);
    Digests->count ++;
  }
  if ((ActivePcrBanks & HASH_ALG_SHA512) != 0) {
    DEBUG ((DEBUG_INFO, "Unsupported PCR bank\n"));
  }


  if (IsNpwAcm()) {
    NewEventHdr.EventSize = sizeof ("Boot Guard Debug Measured S-CRTM");
    TpmLogEvent (&NewEventHdr, (UINT8 *) "Boot Guard Debug Measured S-CRTM");
  } else {
    NewEventHdr.EventSize = sizeof ("Boot Guard Measured S-CRTM");
    TpmLogEvent (&NewEventHdr, (UINT8 *) "Boot Guard Measured S-CRTM");
  }
}

/**
  Create AuthorityPCR event log.

  @param[in] ActivePcrBanks Active PCR banks
**/
VOID
CreateAuthorityPcrEvent (
  IN UINT32   ActivePcrBanks
  )
{
  TCG_PCR_EVENT2_HDR NewEventHdr;
  TPML_DIGEST_VALUES        *Digests;

  if (NeedAuthorityMeasure() && IsVerifiedBoot()) {
    DEBUG ((DEBUG_INFO, "Adding AuthorityPCR Event in TCG Event Log.\n"));
    NewEventHdr.PCRIndex  = 7;
    NewEventHdr.EventType = EV_EFI_VARIABLE_DRIVER_CONFIG;
    Digests = &NewEventHdr.Digests;
    Digests->count = 1;
    Digests->digests[0].hashAlg = TPM_ALG_SHA256;
    CaculateAuthorityPCRExtendValue (&(Digests->digests[0].digest));

      if (IsNpwAcm()) {
        NewEventHdr.EventSize = sizeof (L"Boot Guard Debug Measured S-CRTM");
        TpmLogEvent (&NewEventHdr, (UINT8 *)L"Boot Guard Debug Measured S-CRTM");
      } else {
        NewEventHdr.EventSize = sizeof (L"Boot Guard Measured S-CRTM");
        TpmLogEvent (&NewEventHdr, (UINT8 *)L"Boot Guard Measured S-CRTM");
      }
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
  Create Locality Startup event entry

  @param[in] StartupLocality  Startup locality
  @param[in] TpmType          TPM type
**/
VOID
CreateLocalityStartupEvent (
  IN UINT8              StartupLocality,
  IN UINT32             ActivePcrBanks
  )
{
  TCG_PCR_EVENT2_HDR               PcrEventHdr;
  UINT8                            Sha1[SHA1_DIGEST_SIZE];
  UINT8                            Sha256[SHA256_DIGEST_SIZE];
  UINT8                            Sha384[SHA384_DIGEST_SIZE];
  UINT8                            Sm3[SM3_256_DIGEST_SIZE];
  TPML_DIGEST_VALUES               DigestList;
  TCG_EfiStartupLocalityEvent      LocalityEventData;
  STATIC CONST CHAR8               LocalityString[] = "StartupLocality\0";

  ZeroMem (&Sha1, SHA1_DIGEST_SIZE);
  ZeroMem (&Sha256, SHA256_DIGEST_SIZE);
  ZeroMem (&Sha384, SHA384_DIGEST_SIZE);
  ZeroMem (&Sm3, SM3_256_DIGEST_SIZE);

  PcrEventHdr.PCRIndex = 0;
  PcrEventHdr.EventType = EV_NO_ACTION;
  PcrEventHdr.EventSize = sizeof (LocalityEventData);

  ZeroMem (&DigestList, sizeof(TPML_DIGEST_VALUES));

  if ((ActivePcrBanks & HASH_ALG_SHA1) != 0) {
    DigestList.digests[DigestList.count].hashAlg = TPM_ALG_SHA1;
    CopyMem (DigestList.digests[DigestList.count].digest.sha1, Sha1, SHA1_DIGEST_SIZE);
    DigestList.count ++;
  }

  if ((ActivePcrBanks & HASH_ALG_SHA256) != 0) {
    DigestList.digests[DigestList.count].hashAlg = TPM_ALG_SHA256;
    CopyMem (DigestList.digests[DigestList.count].digest.sha256, Sha256, SHA256_DIGEST_SIZE);
    DigestList.count ++;
  }

  if ((ActivePcrBanks & HASH_ALG_SHA384) != 0) {
    DigestList.digests[DigestList.count].hashAlg = TPM_ALG_SHA384;
    CopyMem (DigestList.digests[DigestList.count].digest.sha384, Sha384, SHA384_DIGEST_SIZE);
    DigestList.count ++;
  }

  if ((ActivePcrBanks & HASH_ALG_SM3_256) != 0) {
    DigestList.digests[DigestList.count].hashAlg = TPM_ALG_SM3_256;
    CopyMem (DigestList.digests[DigestList.count].digest.sm3_256, Sm3, SM3_256_DIGEST_SIZE);
    DigestList.count ++;
  }

  CopyMem (LocalityEventData.Signature, LocalityString, AsciiStrSize (LocalityString));
  LocalityEventData.StartupLocality = StartupLocality;

  LogAcmPcrExtendedEvent (&DigestList, &PcrEventHdr, (UINT8 *) &LocalityEventData);
}


/**
  Create Boot Guard TPM event log.

  @param[in] TpmType  Which type of TPM is available on system.
**/
VOID
CreateTpmEventLog (
  IN TPM_TYPE TpmType
  )
{
  UINT32               ActivePcrBanks;
  UINT32               TpmHashAlgorithmBitmap;
  ACM_BIOS_POLICY      AcmPolicySts;
  UINT8                TpmStartupLocality;

  AcmPolicySts.Data = GetAcmPolicySts ();

  Tpm2GetCapabilitySupportedAndActivePcrs (&TpmHashAlgorithmBitmap, &ActivePcrBanks);

  //
  // Initialize TPM Startup locality to 0
  //
  TpmStartupLocality = LOCALITY_0_INDICATOR;

  //
  // If BootGuard ACM is the S-CRTM, check for the TPM Startup locality used.
  //   b'001--> BTG / b'010 (2) --> TXT / b'100 (4) --> PFR
  //
  if (AcmPolicySts.Bits.SCrtmStatus != 0) {
    //
    // Update TPM startup locality based on the ACM Policy Status TpmStartupLocality field:
    //   0x0 : Startup Locality = 3
    //   0x1 : Startup Locality = 0
    //
    if (AcmPolicySts.Bits.TpmStartupLocality == 0) {
      TpmStartupLocality = LOCALITY_3_INDICATOR;
    }
  }

  CreateLocalityStartupEvent (TpmStartupLocality, ActivePcrBanks);

  //
  // If BootGuard ACM is the S-CRTM,
  // create event logs from previous PCR extensions on behalf of ACM.
  //
  if (AcmPolicySts.Bits.SCrtmStatus != 0) {
    CreateDetailPcrEvent (ActivePcrBanks);
    CreateAuthorityPcrEvent (ActivePcrBanks);
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
  ACM_HEADER  *AcmHdr;

  *Svn = 0;
  AcmHdr = (ACM_HEADER*) Hdr;

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
  ACM_HEADER  *AcmHdr;

  AcmHdr = FindAcm ();
  return GetAcmSvnFromAcmHdr ((UINTN) AcmHdr, Svn);
}