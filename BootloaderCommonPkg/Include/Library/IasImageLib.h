/** @file
  This file defines IAS File structures.

  Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
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

typedef struct {        /* a file (sub-image) inside a boot image */
  VOID      *Addr;
  UINT32    Size;
} IMAGE_DATA;

//
// IAS Pub Key format for use with IPP CryptoLib
//
typedef struct {
  UINT32  Signature;
  UINT8 PubKeyMod[RSA_MOD_SIZE];
  UINT8 PubKeyExp[RSA_E_SIZE];
} IAS_PUB_KEY;

/**
Check the Addr parameter for a valid IAS image; ensure the format is correct,
confirm the IAS header CRC is correct, and (optional) confirm the hash of the
data region of the IAS image matches the expected value. Also, return the
hash of the IAS image to the caller.

@param           Addr              Address of the IAS image to be verified for validity.
@param           Size              Size of the IAS image to be verified for validity.
@param           ImageHash         Hash of the IAS image is returned to the caller.

@retval NULL  The IAS image is compromised.
@retval Hdr   The IAS image is valid.

**/
IAS_HEADER *
IsIasImageValid (
  IN  VOID  *Addr,
  IN  UINT32 Size,
  OUT UINT8 *ImageHash
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
#define IAS_EXT_HDR(h)                ((UINT32*) (((UINT32) (h)) + sizeof(IAS_HEADER)))
#define IAS_PAYLOAD(h)                ((((UINT32) (h)) + (h)->DataOffset))
#define IAS_PAYLOAD_CRC(h)            (* (UINT32*) (((UINT32*) (h)) + (h)->DataOffset + (h)->DataLength))
#define IAS_PAYLOAD_END(h)            (IAS_PAYLOAD(h) + (h)->DataLength + sizeof(UINT32))
#define IAS_SIGNATURE(h)              (((UINT32) h) + ROUNDED_UP((h)->DataOffset + (h)->DataLength + sizeof(UINT32), 256))
#define IAS_PUBLIC_KEY(h)             ((IAS_SIGNATURE(h) + 256))
#define IAS_IMAGE_END(h)              ((IAS_PUBLIC_KEY(h) + (( (h)->ImageType  & 1<<9 ) == (1 <<9) ? 260 : 0)))
#define IAS_IMAGE_SIZE(h)             (((UINT32) IAS_IMAGE_END(h)) - ((UINT32) h))

// Note: Alignment argument must be a power of two.
#define ROUNDED_DOWN(val, align)    ((val) & ~((align) - 1))
#define ROUNDED_UP(  val, align)    ROUNDED_DOWN((val) + (align) - 1, (align))

#endif
