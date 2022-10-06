/** @file


  Copyright (c) 2021, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Base.h>
#include <Library/BdatLib.h>
#include <Library/Bdat4.h>
#include <Library/BdatSchema.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/HobLib.h>
#include <Library/Crc32Lib.h>

#define CRC_SEED                  (0)
#define CRC_XOR_MASK              (0x1021)

/**
  Calculate the CRC16 of the provided data. CRC16 formula is the same
  one that is used for calculating the CRC16 stored in the memory SPD.

  @param[in]  Buffer - Pointer to the start of the data.
  @param[in]  Size   - Amount of data in the buffer, in bytes.

  @retval The calculated CRC16 value.
**/
static
UINT16
GetCrc16 (
  IN  CONST UINT8  *CONST Buffer,
  IN  CONST UINT32        Size
  )
{
  CONST UINT8  *Data;
  UINT32       Value;
  UINT32       Byte;
  UINT8        Bit;

  Data  = Buffer;
  Value = CRC_SEED;
  for (Byte = 0; Byte < Size; Byte++) {
    Value ^= (UINT32) *Data++ << 8;
    for (Bit = 0; Bit < 8; Bit++) {
      Value = (Value & BIT15) ? (Value << 1) ^ CRC_XOR_MASK : Value << 1;
    }
  }

  return ((UINT16) Value);
}

/**
@brief
  Generate the header data for the BDAT structure

  @param[in]  SchemaCount : The number of schemas in the BDAT structure
  @param[out] Bdat        : A pointer to a pre-allocated memory area for the BDAT
                            structure. BDAT header data will be written to this address

  @retval EFI_SUCCESS:              BDAT header generated successfully
  @retval EFI_OUT_OF_RESOURCES:     Could not allocate needed resources
**/
EFI_STATUS
CreateBdatHeader (
  IN  UINT16         SchemaCount,
  OUT BDAT_STRUCTURE *Bdat
  )
{
  CONST UINT8                BdatHeaderSign[] = {'B', 'D', 'A', 'T', 'H', 'E', 'A', 'D'};
  EFI_STATUS                 Status;
  BDAT_HEADER_STRUCTURE      *BdatHeader;
  BDAT_SCHEMA_LIST_STRUCTURE *BdatSchemas;

  Status = EFI_SUCCESS;

  //
  // Initialize the header area of the BDAT structure.
  //
  BdatHeader = &Bdat->Header;
  CopyMem (&BdatHeader->BiosDataSignature[0], (UINT8 *) BdatHeaderSign, sizeof (BdatHeader->BiosDataSignature));
  BdatHeader->BiosDataStructSize    = sizeof (BDAT_STRUCTURE) + (SchemaCount * sizeof (UINT32));
  BdatHeader->Version.Rev.Primary   = BDAT_PRIMARY_VERSION;
  BdatHeader->Version.Rev.Secondary = BDAT_SECONDARY_VERSION;
  BdatHeader->OemOffset             = OEM_OFFSET;

  //
  // Initialize the schema list of the BDAT structure.
  //
  BdatSchemas                   = &Bdat->Schemas;
  BdatSchemas->SchemaListLength = SchemaCount;

  return Status;
}

/**
  Update BDAT ACPI table.

  @param[in] Table              BDAT ACPI table which needs to be updated
  @param[in] HobList            A pointer to the FSP HOB list

  @retval EFI_SUCCESS:              Driver initialized successfully
  @exception EFI_UNSUPPORTED:       A needed driver was not located
  @retval EFI_OUT_OF_RESOURCES:     Could not allocate needed resources
**/
EFI_STATUS
UpdateBdatAcpiTable (
  IN EFI_ACPI_DESCRIPTION_HEADER    *Table,
  IN VOID                           *HobList
  )
{
  EFI_BDAT_ACPI_DESCRIPTION_TABLE    *pRmtAcpiTable;
  BDAT_STRUCTURE                     *Bdat;
  BDAT_SCHEMA_HEADER_STRUCTURE       *BdatSchemaHeader;
  MEM_TRAINING_DATA_HOB_HEADER       *MemTrainingDataHob;
  MEM_TRAINING_DATA_STRUCTURE        *MemTrainingData;
  UINT32                              MemTrainDataLen;
  UINT32                              MemTrainRestDataLen;
  UINT32                              MemTrainSchemaSize;
  EFI_HOB_GUID_TYPE                  *MemTrainGuidHob;
  EWL_PRIVATE_DATA                   *EwlPrivateData;
  EFI_HOB_GUID_TYPE                  *EwlGuidHob;
  UINT32                              EwlSchemaSize;
  UINT32                              EwlDataSize;
  UINT32                             *SchemaOffsetArray;
  UINT32                              CrcOut;
  EFI_STATUS                          Status;
  UINT64                              TempBuffer;
  VOID                               *Buffer;
  UINT32                              BufferSize;
  UINT32                              BdatHeaderSize;
  UINT8                               SchemaCount;
  UINT8                               SchemaIdx;
  UINT32                              SchemaOffset;
  UINTN                               Address;

  MemTrainingDataHob  = NULL;
  MemTrainingData     = NULL;
  EwlGuidHob          = NULL;
  MemTrainGuidHob     = NULL;
  Buffer              = NULL;
  SchemaOffsetArray   = NULL;
  EwlPrivateData      = NULL;
  BufferSize          = 0;
  EwlSchemaSize       = 0;
  EwlDataSize         = 0;
  MemTrainSchemaSize  = 0;
  SchemaCount         = 0;
  SchemaIdx           = 0;
  SchemaOffset        = 0;
  MemTrainDataLen     = 0;
  MemTrainRestDataLen = 0;
  Status              = EFI_SUCCESS;

  //
  // 1.a Get Ewl Schema Size
  //
  EwlGuidHob = GetNextGuidHob (&gEwlIdGuid, HobList);
  if (EwlGuidHob != NULL) {
    EwlPrivateData  = GET_GUID_HOB_DATA (EwlGuidHob);
    if (EwlPrivateData != NULL) {
      EwlDataSize     = EwlPrivateData->Status.Header.Size;
      EwlSchemaSize   = sizeof(BDAT_SCHEMA_HEADER_STRUCTURE) + EwlDataSize;
      SchemaCount    += 1;
    }
  }
  //
  // 1.b Get Mem Training Schema Size,
  //     First Hob instance with this GUID contains
  //     MEM_TRAINING_DATA_STRUCTURE with TotalSize info
  //
  MemTrainGuidHob = GetNextGuidHob (&gMemTrainingDataHobGuid, HobList);
  if (MemTrainGuidHob != NULL) {
    MemTrainingDataHob  = GET_GUID_HOB_DATA (MemTrainGuidHob);
    if (MemTrainingDataHob != NULL) {
      MemTrainDataLen      = MemTrainingDataHob->Size - sizeof (MEM_TRAINING_DATA_HOB_HEADER);
      MemTrainingData     = (MEM_TRAINING_DATA_STRUCTURE *) ((UINTN)MemTrainingDataHob + sizeof(MEM_TRAINING_DATA_HOB_HEADER));
      MemTrainRestDataLen  = MemTrainingData->Header.Size - MemTrainDataLen;
      MemTrainSchemaSize  = sizeof(BDAT_SCHEMA_HEADER_STRUCTURE) + MemTrainingData->Header.Size;
      SchemaCount        += 1;
    }
  }

  //
  // 2. Allocate Memory for BDAT, and populate BDAT header
  //
  BdatHeaderSize  = sizeof (BDAT_STRUCTURE) + (SchemaCount * sizeof (UINT32));
  BufferSize      = BdatHeaderSize + EwlSchemaSize + MemTrainSchemaSize;
  Buffer          = AllocatePages (EFI_SIZE_TO_PAGES (BufferSize));
  if (Buffer == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
  }
  ZeroMem (Buffer, BufferSize);
  Bdat   = (BDAT_STRUCTURE *) Buffer;
  Status = CreateBdatHeader (SchemaCount, Bdat);
  SchemaOffsetArray = (UINT32 *) ((UINTN)Bdat + sizeof(BDAT_STRUCTURE));
  SchemaOffset = BdatHeaderSize;

  //
  // 3.a Save EWL Schema to BDAT
  //
  if (EwlSchemaSize > 0) {
    SchemaOffsetArray[SchemaIdx] = SchemaOffset;
    Address = (UINTN)Bdat + SchemaOffsetArray[SchemaIdx];
    BdatSchemaHeader = (BDAT_SCHEMA_HEADER_STRUCTURE *) Address;
    BdatSchemaHeader->SchemaId  = gEwlBdatSchemaGuid;
    BdatSchemaHeader->Crc16     = 0;
    BdatSchemaHeader->Crc16     = GetCrc16 ((CONST UINT8* CONST) BdatSchemaHeader, sizeof(BDAT_SCHEMA_HEADER_STRUCTURE));
    BdatSchemaHeader->DataSize  = EwlSchemaSize;
    Address += sizeof (BDAT_SCHEMA_HEADER_STRUCTURE);
    CopyMem ((VOID *) Address, (VOID *)&(EwlPrivateData->Status), EwlDataSize);
    SchemaOffset += EwlSchemaSize;
    SchemaIdx++;
  }

  //
  // 3.b Save Mem Train Schema to BDAT
  //
  if ((MemTrainSchemaSize > 0) && (MemTrainGuidHob != NULL)) {
    SchemaOffsetArray[SchemaIdx] = SchemaOffset;
    Address = (UINTN)Bdat + SchemaOffsetArray[SchemaIdx];
    BdatSchemaHeader = (BDAT_SCHEMA_HEADER_STRUCTURE *) Address;
    BdatSchemaHeader->SchemaId  = gMemTrainingDataBdatSchemaGuid;
    BdatSchemaHeader->Crc16     = 0;
    BdatSchemaHeader->Crc16     = GetCrc16 ((CONST UINT8* CONST) BdatSchemaHeader, sizeof(BDAT_SCHEMA_HEADER_STRUCTURE));
    BdatSchemaHeader->DataSize  = MemTrainSchemaSize;

    // Copy First Guid Hob, and deal with remaining Hobs later
    Address += sizeof (BDAT_SCHEMA_HEADER_STRUCTURE);
    CopyMem ((VOID *) Address, (VOID *) MemTrainingData, MemTrainDataLen);

    // Copy data from the remaining Hobs, these dont contain
    // any special headers like the First Guid Hob
    Address += MemTrainDataLen;
    while (MemTrainRestDataLen > 0) {
      MemTrainGuidHob = GET_NEXT_HOB (MemTrainGuidHob); // Increment to next HOB
      MemTrainGuidHob = GetNextGuidHob (&gMemTrainingDataHobGuid, MemTrainGuidHob);  // Now search for next instance of the BDAT HOB
      if (MemTrainGuidHob == NULL) {
        return EFI_NOT_FOUND;
      }
      MemTrainingDataHob = GET_GUID_HOB_DATA (MemTrainGuidHob);
      MemTrainDataLen     = MemTrainingDataHob->Size - sizeof (MEM_TRAINING_DATA_HOB_HEADER);
      CopyMem ((VOID *) Address, (VOID *) ((UINTN) MemTrainingDataHob + sizeof(MEM_TRAINING_DATA_HOB_HEADER)), MemTrainDataLen);
      Address += MemTrainDataLen;
      MemTrainRestDataLen -= MemTrainDataLen;
    }

    SchemaOffset += MemTrainSchemaSize;
    SchemaIdx++;

    // Calculate CRC for Mem Train Data Header
    MemTrainingData->Header.Crc = 0;
    Status = CalculateCrc32WithType ((UINT8 *) &(MemTrainingData->Header), MemTrainingData->Header.Size, Crc32TypeDefault, &CrcOut);
    MemTrainingData->Header.Crc = CrcOut;
  }

  //
  // 4. Initialize the Size and CRC of the BDAT structure.
  //    Ensure that the CRC calculation is the last field initialized.
  //
  Bdat->Header.BiosDataStructSize = BufferSize;
  Bdat->Header.Crc16 = GetCrc16 ((CONST UINT8 * CONST) Bdat, BdatHeaderSize);

  ///
  /// RMT ACPI table
  ///
  pRmtAcpiTable = (EFI_BDAT_ACPI_DESCRIPTION_TABLE*)Table;
  TempBuffer    = SIGNATURE_64 ('I', 'N', 'T', 'E', 'L', 0, 0, 0);
  CopyMem (&pRmtAcpiTable->Header.OemId, &TempBuffer, sizeof (pRmtAcpiTable->Header.OemId));
  pRmtAcpiTable->Header.OemTableId       = SIGNATURE_64 ('B', 'D', 'W', '-', 'W', 'P', 'T', 0);
  pRmtAcpiTable->Header.CreatorId        = CREATOR_ID_INTEL;
  pRmtAcpiTable->Header.CreatorRevision  = CREATOR_REV_INTEL;
  pRmtAcpiTable->Header.OemRevision      = ACPI_BDAT_OEM_REV;

  pRmtAcpiTable->BdatGas.Address         = (EFI_PHYSICAL_ADDRESS) ((UINTN)Buffer & 0xFFFFFFFF);

  return Status;
}
