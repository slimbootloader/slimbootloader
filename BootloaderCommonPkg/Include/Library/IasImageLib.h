/** @file
  This file defines IAS File structures.

  Copyright (c) 2023, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#ifndef __IAS_IMAGE_LIB_H__
#define __IAS_IMAGE_LIB_H__
#include <Library/CryptoLib.h>

#define IAS_MAGIC_PATTERN   0x2E6B7069 // ".kpi"
#define MAX_IAS_SUB_IMAGE   32
#define MAX_IAS_IMAGE_SIZE  0x19000000

/*

  IAS Image Layout

--> +---------------------------+
|   |       IAS_HEADER          |
|   +---------------------------+  <- IAS_EXT_HDR_START
|   |       DATA_1_OFFSET       |
|S  +---------------------------+
|I  |       DATA_2_OFFSET       |  <- (OPTIONAL)
|G  +---------------------------+
|N  |           ...             |  <- (OPTIONAL)
|E  +---------------------------+
|D  |       DATA_N_OFFSET       |  <- (OPTIONAL)
|   +---------------------------+  <- IAS_EXT_HDR_END/DATA_OFFSET_START
|R  | +----------------------+  |
|E  | |     IMAGE_DATA_1     |  |
|G  | +----------------------+  |
|I  | |     IMAGE_DATA_2     |  |
|O  | +----------------------+  |
|N  | |         ...          |  |
|   | +----------------------+  |
|   | |     IMAGE_DATA_N     |  |
|   | +----------------------+  |
--> +---------------------------+  <- IAS_SIGNATURE (OPTIONAL)
    |       IAS_SIGNATURE       |
    +---------------------------+  <- IAS_PUBLIC_KEY (OPTIONAL)
    |       IAS_SIGNATURE       |
    +---------------------------+

*/

typedef struct {               // an IAS image generic header:
  UINT32    MagicPattern;    // identifies structure (acts as valid flag)
  UINT32    ImageType;       // image and compression type; values TBD
  UINT32    Version;         // header version
  UINT32    DataLength;      // size of payload (data) in image
  UINT32    DataOffset;      // offset to payload data from header
  UINT32    UncompressedLen; // uncompresse data length
  UINT32    HeaderCrc;       // CRC-32C over entire header
} IAS_HEADER;

typedef enum {
  ImageAllocateTypePool = 0,      // Allocate memory in Pool
  ImageAllocateTypePage,          // Allocate memory in Page
  ImageAllocateTypePointer,       // No allocate memory, but pointer to the existing memory
  ImageAllocateTypeMax
} IMAGE_ALLOCATE_TYPE;

typedef struct {        /* a file (sub-image) inside a boot image */
  VOID                 *Addr;
  UINT32                Size;
  IMAGE_ALLOCATE_TYPE   AllocType;
  UINT32                Name;   // Name specified for the component when building the container
} IMAGE_DATA;

//
// IAS Pub Key format for use
// Supported for only RSA2048
//
typedef struct {
  UINT8 Modulus[RSA2048_MOD_SIZE];
  UINT8 PubExp[RSA_E_SIZE];
} IAS_PUB_KEY;

//
// IAS Iamge buf address and hash info for use
//

typedef struct {
  UINT8           *CompBuf;  // IAS Image buffer address
  UINT32           CompLen;  // IAS image length
  HASH_ALG_TYPE    HashAlg;  // Hash Alg used for HashData
  UINT8            HashData[HASH_DIGEST_MAX]; // Hash of the IAS image
} IAS_IMAGE_INFO;

/**
Check the Addr parameter for a valid IAS image; ensure the format is correct,
confirm the IAS header CRC is correct, and (optional) confirm the hash of the
data region of the IAS image matches the expected value. Also, return the
buffer and hash info of the IAS image to the caller.

@param  Addr              Address of the IAS image to be verified for validity.
@param  Size              Size of the IAS image to be verified for validity.
@param  IasImageInfo      IasImage buffer and hash info data structure

@retval NULL  The IAS image is compromised.
@retval Hdr   The IAS image is valid.

**/
IAS_HEADER *
IsIasImageValid (
  IN  VOID               *Addr,
  IN  UINT32              Size,
  OUT IAS_IMAGE_INFO     *IasImageInfo
  );

/**
Check the Addr parameter for a valid IAS image; ensure the format is correct,
confirm the IAS header CRC is correct, and (optional) confirm the hash of the
data region of the IAS image matches the expected value.

@param  IasImage  Pointer to the IAS image to retrieve files from.
@param  NumImg    The number of files expected to be extracted from the IAS image.
@param  Img       Buffer to be filled with each retrieved file's address and size from the IAS image.

@retval MAX (NumFile, 1)  The number of files retrieved from the IAS image.

**/
UINT32
IasGetFiles (
  IN   IAS_HEADER  *IasImage,
  IN   UINT32      NumImg,
  OUT  IMAGE_DATA  *Img
  );

// Image type subfields (cf. boot subsustem HLD, appendix A for details).
#define IAS_IMAGE_TYPE(it)      (((it) & 0xffff0000) >> 16)
#define IAS_IMAGE_IS_SIGNED(it)  ((it) &      0x100)
#define IAS_IMAGE_COMP_ALGO(it)  ((it) &       0xff)
#define IAS_IMAGE_HAS_PUBKEY(it) ((it) &      0x200)

#define IAS_TYPE_CLASSIC    0x3
#define IAS_TYPE_MULTIBOOT  0x4

// Find the extended header, payload data and CRC, and the RSA signature from the generic header:
#define IAS_EXT_HDR_SIZE(h)           ((h)->DataOffset - sizeof(IAS_HEADER))
#define IAS_EXT_HDR(h)                ((UINT32*) (((UINT32)(UINTN)(h)) + sizeof(IAS_HEADER)))
#define IAS_PAYLOAD(h)                ((((UINT32)(UINTN)(h)) + (h)->DataOffset))
#define IAS_PAYLOAD_CRC(h)            (* (UINT32*) (((UINT32*) (h)) + (h)->DataOffset + (h)->DataLength))
#define IAS_PAYLOAD_END(h)            (IAS_PAYLOAD(h) + (h)->DataLength + sizeof(UINT32))
#define IAS_SIGNATURE(h)              (((UINT32)(UINTN)h) + ROUNDED_UP((h)->DataOffset + (h)->DataLength + sizeof(UINT32), 256))
#define IAS_PUBLIC_KEY(h)             (((( (h)->ImageType  & BIT8 ) == BIT8 ? IAS_SIGNATURE(h) + 256 : IAS_PAYLOAD_END(h))))
#define IAS_IMAGE_END(h)              ((IAS_PUBLIC_KEY(h) + (( (h)->ImageType  & BIT9 ) == BIT9 ? 260 : 0)))
#define IAS_IMAGE_SIZE(h)             (((UINT32) IAS_IMAGE_END(h)) - ((UINT32)(UINTN)h))

// Note: Alignment argument must be a power of two.
#define ROUNDED_DOWN(val, align)    ((val) & ~((align) - 1))
#define ROUNDED_UP(  val, align)    ROUNDED_DOWN((val) + (align) - 1, (align))

#endif
