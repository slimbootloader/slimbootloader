/** @file


  Copyright (c) 2021, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Base.h>
#include <Library/BdatLib.h>
#include <Library/Bdat4.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/HobLib.h>

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
  @param[in] HobList        A pointer to the HOB list

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
  EFI_BDAT_ACPI_DESCRIPTION_TABLE  *pRmtAcpiTable;
  VOID                 *Buffer;
  BDAT_STRUCTURE       *Bdat;
  BDAT_SCHEMA_LIST_HOB *BdatSchemaListHob;
  EFI_GUID             *Guid;
  EFI_HOB_GUID_TYPE    *Schema[MAX_SCHEMA_LIST_LENGTH];
  UINT32               *SchemaOffsetList;
  UINT8                *NextBlock;
  UINT8                *SchemaData;
  EFI_STATUS           Status;
  UINT64               TempBuffer;
  UINT32               DataSize;
  UINT32               BufferSize;
  UINT32               BdatHeaderSize;
  UINT16               SchemaListIndex;
  UINT8                NextSchema;
  UINT8                SchemaCount;
  UINT8                Check;
  UINT8                Index;

  Status = EFI_SUCCESS;
  while (Status == EFI_SUCCESS) {
    DEBUG ((DEBUG_INFO, "Creating BDAT Table...\n"));

    ///
    /// Get the memory information HOB so we can locate the BDAT information.
    ///
    BdatSchemaListHob =  (BDAT_SCHEMA_LIST_HOB *)GetNextGuidHob (&gSchemaListGuid, HobList);
    if (BdatSchemaListHob == NULL) {
      DEBUG ((DEBUG_INFO, "No BDAT data exists, exiting without creating table\n"));
      Status = EFI_UNSUPPORTED;
      break;
    }
    DEBUG ((DEBUG_INFO, "Found Schema List HOB\n"));

    ///
    /// Gather up all the BDAT specific HOBs.
    /// We need to get pointers to them and determine the buffer size needed to store them into the ACPI table.
    ///
    BufferSize  = 0;
    SchemaCount = 0;
    for (SchemaListIndex = 0; SchemaListIndex < BdatSchemaListHob->SchemaHobCount; SchemaListIndex++) {
      Guid = (EFI_GUID *) &(BdatSchemaListHob->SchemaHobGuids[SchemaListIndex]);

      Schema[SchemaListIndex] = NULL;
      if (Guid != NULL) {
        DEBUG ((DEBUG_INFO, "Checking GUID: "));
        for (Index = 0, Check = 0; Index < sizeof (EFI_GUID); Index++) {
          Check |= ((UINT8 *) Guid) [Index];
        }
        DEBUG ((DEBUG_INFO, "%d\n", (UINT32) Check));
        DEBUG ((DEBUG_INFO, "GUID = %g\n", Guid));
        if (Check != 0) {
          Schema[SchemaListIndex] = (EFI_HOB_GUID_TYPE *)GetNextGuidHob (Guid, HobList);
          DEBUG ((DEBUG_INFO, "Schema HOB pointer: %x\n", (UINT32) (UINTN) Schema[SchemaListIndex]));
          if (Schema[SchemaListIndex] != NULL) {
            BufferSize += Schema[SchemaListIndex]->Header.HobLength - sizeof (EFI_HOB_GUID_TYPE);
            DEBUG ((DEBUG_INFO, "HOB Length = %d\n", Schema[SchemaListIndex]->Header.HobLength));
            SchemaCount++;
          }
        } else {
          DEBUG ((DEBUG_INFO, "BDAT Schema List HOB is corrupted, aborting\n"));
          Status = EFI_UNSUPPORTED;
          break;
        }
      }
    }
    DEBUG ((DEBUG_INFO, "BdatSchemaListHob->SchemaHobCount = %d, SchemaCount = %d\n", (UINT32) BdatSchemaListHob->SchemaHobCount, (UINT32) SchemaCount));

    ///
    /// Return if we did not find any schemas
    ///
    if (SchemaCount == 0) {
      DEBUG ((DEBUG_INFO, "No BDAT data exists, exiting without creating table\n"));
      Status = EFI_UNSUPPORTED;
      break;
    }

    ///
    /// Allocate and clear memory, in 4kb pages. This memory is used to store the BDAT into the ACPI table.
    ///
    Buffer         = NULL;
    BdatHeaderSize = sizeof (BDAT_HEADER_STRUCTURE) + sizeof (BDAT_SCHEMA_LIST_STRUCTURE) + (SchemaCount * sizeof (UINT32));
    BufferSize    += BdatHeaderSize;
    DEBUG ((DEBUG_INFO, "BufferSize = %d\n", BufferSize));
    Buffer = AllocatePages (EFI_SIZE_TO_PAGES (BufferSize));
//  Buffer = (VOID *)((UINTN) Table + sizeof (EFI_ACPI_DESCRIPTION_HEADER) + sizeof (EFI_ACPI_3_0_GENERIC_ADDRESS_STRUCTURE));
    if (Buffer == NULL) {
      Status = EFI_OUT_OF_RESOURCES;
      break;
    }
    ZeroMem (Buffer, BufferSize);
    DEBUG ((DEBUG_INFO, "Buffer = 0x%X\n", (UINT32) (UINTN) Buffer));
    ///
    /// Copy the BDAT structure into the memory specified for the ACPI table.
    ///
    Bdat       = (BDAT_STRUCTURE *) Buffer;
    NextBlock  = (UINT8 *) ((UINTN) Buffer + BdatHeaderSize);
    DEBUG ((DEBUG_INFO, "NextBlock = 0x%X\n", NextBlock));
    NextSchema = 0;
    SchemaOffsetList = (UINT32*) (((UINTN) Buffer) + sizeof (BDAT_STRUCTURE));
    Status = CreateBdatHeader (SchemaCount, Bdat);
    ASSERT_EFI_ERROR (Status);
    for (SchemaListIndex = 0; SchemaListIndex < SchemaCount; SchemaListIndex++) {
      if (Schema[SchemaListIndex] != NULL) {
        DataSize = Schema[SchemaListIndex]->Header.HobLength - sizeof (EFI_HOB_GUID_TYPE);
        DEBUG ((DEBUG_INFO, "DataSize = %d\n", DataSize));
        SchemaData = (UINT8*) Schema[SchemaListIndex];
        SchemaData += sizeof (EFI_HOB_GUID_TYPE);
        DEBUG ((DEBUG_INFO, "SchemaData = 0x%X\n", (UINT32) ((UINTN) SchemaData)));
        CopyMem (NextBlock, SchemaData, DataSize);
        SchemaOffsetList[NextSchema++] = (UINT32) ((UINTN) NextBlock  - (UINTN) Bdat);
        NextBlock = (UINT8 *) ((UINTN) NextBlock + DataSize);
        DEBUG ((DEBUG_INFO, "NextBlock = 0x%X\n", NextBlock));
      }
    }
    ///
    /// Initialize the Size and CRC of the BDAT structure.
    /// Ensure that the CRC calculation is the last field initialized.
    ///
    Bdat->Header.BiosDataStructSize = BufferSize;
    Bdat->Header.Crc16 = GetCrc16 ((CONST UINT8 * CONST) Bdat, BdatHeaderSize);

    ///
    /// RMT ACPI table
    ///
    pRmtAcpiTable = (EFI_BDAT_ACPI_DESCRIPTION_TABLE*)Table;
    DEBUG ((DEBUG_INFO, "In RMT ACPI table\n"));
    TempBuffer = SIGNATURE_64 ('I', 'N', 'T', 'E', 'L', 0, 0, 0);
    CopyMem (&pRmtAcpiTable->Header.OemId, &TempBuffer, sizeof (pRmtAcpiTable->Header.OemId));
    pRmtAcpiTable->Header.OemTableId       = SIGNATURE_64 ('B', 'D', 'W', '-', 'W', 'P', 'T', 0);
    pRmtAcpiTable->Header.CreatorId        = CREATOR_ID_INTEL;
    pRmtAcpiTable->Header.CreatorRevision  = CREATOR_REV_INTEL;
    pRmtAcpiTable->Header.OemRevision      = ACPI_BDAT_OEM_REV;

    ///
    /// Copy pointer to RMT ACPI BDAT structure and protocol.
    ///
    pRmtAcpiTable->BdatGas.Address = (EFI_PHYSICAL_ADDRESS) ((UINTN)Buffer & 0xFFFFFFFF);
    break;
  }
  return Status;
}
