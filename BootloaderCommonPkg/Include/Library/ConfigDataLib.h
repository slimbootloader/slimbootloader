/** @file
  Config data library instance for data access.

  Copyright (c) 2017 - 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/


#ifndef __CONFIGURATION_DATA_LIB_H__
#define __CONFIGURATION_DATA_LIB_H__

#define CFG_DATA_SIGNATURE  SIGNATURE_32 ('C', 'F', 'G', 'D')

#define CDATA_BLOB_ATTR_SIGNED  (1 << 0)
#define CDATA_BLOB_ATTR_MERGED  (1 << 7)

#define CDATA_FLAG_TYPE_MASK    (3 << 0)
#define CDATA_FLAG_TYPE_NORMAL  (0 << 0)
#define CDATA_FLAG_TYPE_ARRAY   (1 << 0)
#define CDATA_FLAG_TYPE_REFER   (2 << 0)

#define CFG_LOAD_SRC_PDR        (1 << 0)
#define CFG_LOAD_SRC_BIOS       (1 << 1)

#define PID_TO_MASK(x)          (1 << ((x) & 0x1F))

#define CDATA_NO_TAG            0x000
#define CDATA_PLATFORMID_TAG    0x0F0

typedef struct {
  UINT16   PlatformId;
  UINT16   Reserved;
} PLATFORMID_CFG_DATA;

typedef struct {
  UINT16   PlatformId;
  UINT16   Tag        : 12;
  UINT16   IsInternal : 1;
  UINT16   Reserved   : 3;
} REFERENCE_CFG_DATA;

typedef struct {
  UINT32  Value;    // Bit masks on supported platforms
} CDATA_COND;

typedef struct {
  UINT32  ConditionNum   :  2;      // [1:0]   #of condition words present
  UINT32  Length         : 10;      // [11:2]  total size of item (in dwords)
  UINT32  Flags          :  4;      // [15:12] unused/reserved so far
  UINT32  Version        :  4;      // [19:16] item (payload) format version
  UINT32  Tag            : 12;      // [31:20] identifies item (in payload)
  CDATA_COND     Condition[0];
} CDATA_HEADER;

typedef struct {
  UINT32  Signature;
  //
  // This header Length
  //
  UINT8   HeaderLength;
  UINT8   Attribute;
  union {
    //
    // Internal configuration data offset in DWORD from the start of data blob.
    // This value is only valid in runtime.
    //
    UINT16  InternalDataOffset;
    //
    // Security version number
    // This is available only in flash. It would be overwritten by CDATA_BLOB.InternalDataOffset in runtime
    //
    UINT8   Svn;
  } ExtraInfo;
  //
  // The total valid configuration data length including this header.
  //
  UINT32  UsedLength;
  //
  // The total space for configration data
  //
  UINT32  TotalLength;
} CDATA_BLOB;

typedef struct {

  /* header size */
  UINT8                       HeaderSize;

  /* base table ID */
  UINT8                       BaseTableId;

  /* size in byte for every array entry */
  UINT16                      ItemSize;

  /* array entry count */
  UINT16                      ItemCount;

  /* array entry ID bit offset */
  UINT8                       ItemIdBitOff;

  /* array entry ID bit length */
  UINT8                       ItemIdBitLen;

  /* array entry valid bit offset */
  UINT8                       ItemValidBitOff;

  /* unused */
  UINT8                       ItemUnused;

  /* array entry bit mask, 1 bit per entry to indicate the entry exists or not */
  UINT8                       BaseTableBitMask[0];

} ARRAY_CFG_HDR;


/**
  Load the configuration data blob from media into destination buffer.

  @param[in]    Dst        Destination address to load configuration data blob.
  @param[in]    Src        Source address to load configuration data blob.
  @param[in]    Len        The destination address buffer size.

  @retval  EFI_SUCCESS             Configuration data blob was loaded successfully.
  @retval  EFI_NOT_FOUND           Configuration data blob cannot be found.
  @retval  EFI_OUT_OF_RESOURCES    Destination buffer is too small to hold the
                                   configuration data blob.
  @retval  Others                  Failed to load configuration data blob.

**/
EFI_STATUS
EFIAPI
LoadExternalConfigData (
  IN UINT32  Dst,
  IN UINT32  Src,
  IN UINT32  Len
  );


/**
  Find configuration data header by its tag and platform ID.

  @param[in] PlatformId   Platform ID.
  @param[in] Tag          Configuration TAG ID to find.

  @retval            Configuration data header pointer.
                     NULL if the tag cannot be found.

**/
CDATA_HEADER *
EFIAPI
FindConfigHdrByPidTag (
  UINT16  PlatformId,
  UINT32  Tag
  );

/**
  Find configuration data header by its tag.

  @param[in] Tag     Configuration TAG ID to find.

  @retval            Configuration data header pointer.
                     NULL if the tag cannot be found.

**/
CDATA_HEADER *
EFIAPI
FindConfigHdrByTag (
  UINT32  Tag
  );

/**
  Find configuration data by its tag.

  @param[in] PlatformId   Platform ID.
  @param[in] Tag          Configuration TAG ID to find.

  @retval            Configuration data pointer.
                     NULL if the tag cannot be found.

**/
VOID *
EFIAPI
FindConfigDataByPidTag (
  UINT16  PlatformId,
  UINT32  Tag
  );

/**
  Find configuration data by its tag.

  @param[in] Tag     Configuration TAG ID to find.

  @retval            Configuration data pointer.
                     NULL if the tag cannot be found.

**/
VOID *
EFIAPI
FindConfigDataByTag (
  UINT32  Tag
  );

/**
  Add new Config Data

  Ext/Built-In Config Data for a specific board
  is added to the Config Data Blob present at the
  CfgDataPtr in the LDR_GLOBAL_DATA. Exclude the
  CFG_BLOB header here as it is already there with CfgDataPtr.

  @param[in] CfgAddPtr              Address of the CfgBlob that is to be added

  @retval EFI_SUCCESS               if the CfgBlob is added successfully
  @retval EFI_UNSUPPORTED           if not a valid CfgBlob
  @retval EFI_OUT_OF_RESOURCES      if not enough memory available to add Config Data

**/
EFI_STATUS
EFIAPI
AddConfigData (
  IN  UINT8                *CfgAddPtr
  );

/**
  Build a full set of CFGDATA for current platform.

  @param[in] Buffer             Buffer pointer to store the full CFGDATA set.
  @param[in] Length             Buffer length.

  @retval EFI_BUFFER_TOO_SMALL  The buffer size is too small to store the CFGDATA.
  @retval EFI_INVALID_PARAMETER The buffer pointer is NULL.
  @retval EFI_NOT_FOUND         Could not find some CFGDATA tag.
  @retval EFI_SUCCESS           The full set of CFGDATA was built successfully.

**/
EFI_STATUS
EFIAPI
BuildConfigData (
  IN  UINT8      *Buffer,
  IN  UINT32      Length
  );

/**
  Get a full CFGDATA set length.

  @retval   Length of a full CFGDATA set.
            0 indicates no CFGDATA exists.

**/
UINT32
EFIAPI
GetConfigDataSize (
  VOID
  );

#endif
