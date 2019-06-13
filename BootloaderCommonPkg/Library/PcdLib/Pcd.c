/** @file
  All Pcd Ppi services are implemented here.

Copyright (c) 2006 - 2017, Intel Corporation. All rights reserved.<BR>
(C) Copyright 2016 Hewlett Packard Enterprise Development LP<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "Service.h"

/**
  Retrieves an 8-bit value for a given PCD token.

  Retrieves the current byte-sized value for a PCD token number.
  If the TokenNumber is invalid, the results are unpredictable.

  @param[in]  TokenNumber The PCD token number.

  @return The UINT8 value.

**/
UINT8
EFIAPI
PeiPcdGet8 (
  IN UINTN                    TokenNumber
  )
{
  return * ((UINT8 *) GetWorker (TokenNumber, sizeof (UINT8)));
}

/**
  Retrieves an 16-bit value for a given PCD token.

  Retrieves the current 16-bits value for a PCD token number.
  If the TokenNumber is invalid, the results are unpredictable.

  @param[in]  TokenNumber The PCD token number.

  @return The UINT16 value.

**/
UINT16
EFIAPI
PeiPcdGet16 (
  IN UINTN                    TokenNumber
  )
{
  return ReadUnaligned16 (GetWorker (TokenNumber, sizeof (UINT16)));
}

/**
  Retrieves an 32-bit value for a given PCD token.

  Retrieves the current 32-bits value for a PCD token number.
  If the TokenNumber is invalid, the results are unpredictable.

  @param[in]  TokenNumber The PCD token number.

  @return The UINT32 value.

**/
UINT32
EFIAPI
PeiPcdGet32 (
  IN UINTN                    TokenNumber
  )
{
  return ReadUnaligned32 (GetWorker (TokenNumber, sizeof (UINT32)));
}

/**
  Retrieves an 64-bit value for a given PCD token.

  Retrieves the current 64-bits value for a PCD token number.
  If the TokenNumber is invalid, the results are unpredictable.

  @param[in]  TokenNumber The PCD token number.

  @return The UINT64 value.

**/
UINT64
EFIAPI
PeiPcdGet64 (
  IN UINTN                    TokenNumber
  )
{
  return ReadUnaligned64 (GetWorker (TokenNumber, sizeof (UINT64)));
}

/**
  Retrieves a pointer to a value for a given PCD token.

  Retrieves the current pointer to the buffer for a PCD token number.
  Do not make any assumptions about the alignment of the pointer that
  is returned by this function call.  If the TokenNumber is invalid,
  the results are unpredictable.

  @param[in]  TokenNumber The PCD token number.

  @return The pointer to the buffer to be retrieved.

**/
VOID *
EFIAPI
PeiPcdGetPtr (
  IN UINTN                    TokenNumber
  )
{
  return GetWorker (TokenNumber, 0);
}

/**
  Retrieves a Boolean value for a given PCD token.

  Retrieves the current boolean value for a PCD token number.
  Do not make any assumptions about the alignment of the pointer that
  is returned by this function call.  If the TokenNumber is invalid,
  the results are unpredictable.

  @param[in]  TokenNumber The PCD token number.

  @return The Boolean value.

**/
BOOLEAN
EFIAPI
PeiPcdGetBool (
  IN UINTN                    TokenNumber
  )
{
  return * ((BOOLEAN *) GetWorker (TokenNumber, sizeof (BOOLEAN)));
}

/**
  Retrieves the size of the value for a given PCD token.

  Retrieves the current size of a particular PCD token.
  If the TokenNumber is invalid, the results are unpredictable.

  @param[in]  TokenNumber The PCD token number.

  @return The size of the value for the PCD token.

**/
UINTN
EFIAPI
PeiPcdGetSize (
  IN UINTN                    TokenNumber
  )
{
  PEI_PCD_DATABASE    *PeiPcdDb;
  UINTN               Size;
  UINTN               MaxSize;
  UINT32              LocalTokenCount;

  PeiPcdDb        = GetPcdDatabase ();
  LocalTokenCount = PeiPcdDb->LocalTokenCount;
  //
  // TokenNumber Zero is reserved as PCD_INVALID_TOKEN_NUMBER.
  // We have to decrement TokenNumber by 1 to make it usable
  // as the array index.
  //
  TokenNumber--;

  // EBC compiler is very choosy. It may report warning about comparison
  // between UINTN and 0 . So we add 1 in each size of the
  // comparison.
  ASSERT (TokenNumber + 1 < (LocalTokenCount + 1));

  Size = (*((UINT32 *)((UINT8 *)PeiPcdDb + PeiPcdDb->LocalTokenNumberTableOffset) + TokenNumber) & PCD_DATUM_TYPE_ALL_SET) >> PCD_DATUM_TYPE_SHIFT;

  if (Size == 0) {
    //
    // For pointer type, we need to scan the SIZE_TABLE to get the current size.
    //
    return GetPtrTypeSize (TokenNumber, &MaxSize, PeiPcdDb);
  } else {
    return Size;
  }

}

/**
  Sets an 8-bit value for a given PCD token.

  When the PCD service sets a value, it will check to ensure that the
  size of the value being set is compatible with the Token's existing definition.
  If it is not, an error will be returned.

  @param[in]  TokenNumber The PCD token number.
  @param[in]  Value The value to set for the PCD token.

  @retval EFI_SUCCESS  Procedure returned successfully.
  @retval EFI_INVALID_PARAMETER The PCD service determined that the size of the data
                                  being set was incompatible with a call to this function.
                                  Use GetSize() to retrieve the size of the target data.
  @retval EFI_NOT_FOUND The PCD service could not find the requested token number.

**/
EFI_STATUS
EFIAPI
PeiPcdSet8 (
  IN UINTN                        TokenNumber,
  IN UINT8                        Value
  )
{
  return SetValueWorker (TokenNumber, &Value, sizeof (Value));
}

/**
  Sets an 16-bit value for a given PCD token.

  When the PCD service sets a value, it will check to ensure that the
  size of the value being set is compatible with the Token's existing definition.
  If it is not, an error will be returned.

  @param[in]  TokenNumber The PCD token number.
  @param[in]  Value The value to set for the PCD token.

  @retval EFI_SUCCESS  Procedure returned successfully.
  @retval EFI_INVALID_PARAMETER The PCD service determined that the size of the data
                                  being set was incompatible with a call to this function.
                                  Use GetSize() to retrieve the size of the target data.
  @retval EFI_NOT_FOUND The PCD service could not find the requested token number.

**/
EFI_STATUS
EFIAPI
PeiPcdSet16 (
  IN UINTN                         TokenNumber,
  IN UINT16                        Value
  )
{
  return SetValueWorker (TokenNumber, &Value, sizeof (Value));
}

/**
  Sets an 32-bit value for a given PCD token.

  When the PCD service sets a value, it will check to ensure that the
  size of the value being set is compatible with the Token's existing definition.
  If it is not, an error will be returned.

  @param[in]  TokenNumber The PCD token number.
  @param[in]  Value The value to set for the PCD token.

  @retval EFI_SUCCESS  Procedure returned successfully.
  @retval EFI_INVALID_PARAMETER The PCD service determined that the size of the data
                                  being set was incompatible with a call to this function.
                                  Use GetSize() to retrieve the size of the target data.
  @retval EFI_NOT_FOUND The PCD service could not find the requested token number.

**/
EFI_STATUS
EFIAPI
PeiPcdSet32 (
  IN UINTN                         TokenNumber,
  IN UINT32                        Value
  )
{
  return SetValueWorker (TokenNumber, &Value, sizeof (Value));
}

/**
  Sets an 64-bit value for a given PCD token.

  When the PCD service sets a value, it will check to ensure that the
  size of the value being set is compatible with the Token's existing definition.
  If it is not, an error will be returned.

  @param[in]  TokenNumber The PCD token number.
  @param[in]  Value The value to set for the PCD token.

  @retval EFI_SUCCESS  Procedure returned successfully.
  @retval EFI_INVALID_PARAMETER The PCD service determined that the size of the data
                                  being set was incompatible with a call to this function.
                                  Use GetSize() to retrieve the size of the target data.
  @retval EFI_NOT_FOUND The PCD service could not find the requested token number.

**/
EFI_STATUS
EFIAPI
PeiPcdSet64 (
  IN UINTN                         TokenNumber,
  IN UINT64                        Value
  )
{
  return SetValueWorker (TokenNumber, &Value, sizeof (Value));
}

/**
  Sets a value of a specified size for a given PCD token.

  When the PCD service sets a value, it will check to ensure that the
  size of the value being set is compatible with the Token's existing definition.
  If it is not, an error will be returned.

  @param[in]  TokenNumber The PCD token number.
  @param[in, out] SizeOfBuffer A pointer to the length of the value being set for the PCD token.
                              On input, if the SizeOfValue is greater than the maximum size supported
                              for this TokenNumber then the output value of SizeOfValue will reflect
                              the maximum size supported for this TokenNumber.
  @param[in]  Buffer The buffer to set for the PCD token.

  @retval EFI_SUCCESS  Procedure returned successfully.
  @retval EFI_INVALID_PARAMETER The PCD service determined that the size of the data
                                  being set was incompatible with a call to this function.
                                  Use GetSize() to retrieve the size of the target data.
  @retval EFI_NOT_FOUND The PCD service could not find the requested token number.

**/
EFI_STATUS
EFIAPI
PeiPcdSetPtr (
  IN      UINTN                    TokenNumber,
  IN OUT  UINTN                    *SizeOfBuffer,
  IN      VOID                     *Buffer
  )
{
  return SetWorker (TokenNumber, Buffer, SizeOfBuffer, TRUE);
}

/**
  Sets an Boolean value for a given PCD token.

  When the PCD service sets a value, it will check to ensure that the
  size of the value being set is compatible with the Token's existing definition.
  If it is not, an error will be returned.

  @param[in]  TokenNumber The PCD token number.
  @param[in]  Value The value to set for the PCD token.

  @retval EFI_SUCCESS  Procedure returned successfully.
  @retval EFI_INVALID_PARAMETER The PCD service determined that the size of the data
                                  being set was incompatible with a call to this function.
                                  Use GetSize() to retrieve the size of the target data.
  @retval EFI_NOT_FOUND The PCD service could not find the requested token number.

**/
EFI_STATUS
EFIAPI
PeiPcdSetBool (
  IN UINTN                         TokenNumber,
  IN BOOLEAN                       Value
  )
{
  return SetValueWorker (TokenNumber, &Value, sizeof (Value));
}

/**
  Get PCD value's size for POINTER type PCD.

  The POINTER type PCD's value will be stored into a buffer in specified size.
  The max size of this PCD's value is described in PCD's definition in DEC file.

  @param LocalTokenNumberTableIdx Index of PCD token number in PCD token table
  @param MaxSize                  Maximum size of PCD's value
  @param Database                 Pcd database in PEI phase.

  @return PCD value's size for POINTER type PCD.

**/
UINTN
GetPtrTypeSize (
  IN    UINTN             LocalTokenNumberTableIdx,
  OUT   UINTN             *MaxSize,
  IN    PEI_PCD_DATABASE  *Database
  )
{
  INTN        SizeTableIdx;
  UINTN       LocalTokenNumber;
  SIZE_INFO   *SizeTable;

  SizeTableIdx = GetSizeTableIndex (LocalTokenNumberTableIdx, Database);

  LocalTokenNumber = *((UINT32 *)((UINT8 *)Database + Database->LocalTokenNumberTableOffset) + LocalTokenNumberTableIdx);

  ASSERT ((LocalTokenNumber & PCD_DATUM_TYPE_ALL_SET) == PCD_DATUM_TYPE_POINTER);

  SizeTable = (SIZE_INFO *) ((UINT8 *)Database + Database->SizeTableOffset);

  *MaxSize = SizeTable[SizeTableIdx];
  //
  // SizeTable only contain record for PCD_DATUM_TYPE_POINTER type
  // PCD entry.
  //
  return SizeTable[SizeTableIdx + 1];
}

/**
  Set PCD value's size for POINTER type PCD.

  The POINTER type PCD's value will be stored into a buffer in specified size.
  The max size of this PCD's value is described in PCD's definition in DEC file.

  @param LocalTokenNumberTableIdx Index of PCD token number in PCD token table
  @param CurrentSize              Maximum size of PCD's value
  @param Database                 Pcd database in PEI phase.

  @retval TRUE  Success to set PCD's value size, which is not exceed maximum size
  @retval FALSE Fail to set PCD's value size, which maybe exceed maximum size

**/
BOOLEAN
SetPtrTypeSize (
  IN          UINTN             LocalTokenNumberTableIdx,
  IN    OUT   UINTN             *CurrentSize,
  IN          PEI_PCD_DATABASE  *Database
  )
{
  INTN        SizeTableIdx;
  UINTN       LocalTokenNumber;
  SIZE_INFO   *SizeTable;
  UINTN       MaxSize;

  SizeTableIdx = GetSizeTableIndex (LocalTokenNumberTableIdx, Database);

  LocalTokenNumber = *((UINT32 *)((UINT8 *)Database + Database->LocalTokenNumberTableOffset) + LocalTokenNumberTableIdx);

  ASSERT ((LocalTokenNumber & PCD_DATUM_TYPE_ALL_SET) == PCD_DATUM_TYPE_POINTER);

  SizeTable = (SIZE_INFO *) ((UINT8 *)Database + Database->SizeTableOffset);

  MaxSize = SizeTable[SizeTableIdx];
  //
  // SizeTable only contain record for PCD_DATUM_TYPE_POINTER type
  // PCD entry.
  //
  if ((*CurrentSize > MaxSize) ||
      (*CurrentSize == MAX_ADDRESS)) {
    *CurrentSize = MaxSize;
    return FALSE;
  }

  //
  // We have only two entry for Non-Sku enabled PCD entry:
  // 1) MAX SIZE
  // 2) Current Size
  //
  SizeTable[SizeTableIdx + 1] = (SIZE_INFO) * CurrentSize;
  return TRUE;
}
