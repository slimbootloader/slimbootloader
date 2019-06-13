/** @file
Implementation of PcdLib class library for PEI phase.

Copyright (c) 2006 - 2015, Intel Corporation. All rights reserved.<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent


**/




#include <PiPei.h>
#include <Library/PcdLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>

#include "Service.h"

/**
  This function provides a means by which to retrieve a value for a given PCD token.

  Returns the 8-bit value for the token specified by TokenNumber.

  @param[in]  TokenNumber The PCD token number to retrieve a current value for.

  @return Returns the 8-bit value for the token specified by TokenNumber.

**/
UINT8
EFIAPI
LibPcdGet8 (
  IN UINTN             TokenNumber
  )
{
  return PeiPcdGet8 (TokenNumber);
}



/**
  This function provides a means by which to retrieve a value for a given PCD token.

  Returns the 16-bit value for the token specified by TokenNumber.

  @param[in]  TokenNumber The PCD token number to retrieve a current value for.

  @return Returns the 16-bit value for the token specified by TokenNumber.

**/
UINT16
EFIAPI
LibPcdGet16 (
  IN UINTN             TokenNumber
  )
{
  return PeiPcdGet16 (TokenNumber);
}



/**
  This function provides a means by which to retrieve a value for a given PCD token.

  Returns the 32-bit value for the token specified by TokenNumber.

  @param[in]  TokenNumber The PCD token number to retrieve a current value for.

  @return Returns the 32-bit value for the token specified by TokenNumber.

**/
UINT32
EFIAPI
LibPcdGet32 (
  IN UINTN             TokenNumber
  )
{
  return PeiPcdGet32 (TokenNumber);
}



/**
  This function provides a means by which to retrieve a value for a given PCD token.

  Returns the 64-bit value for the token specified by TokenNumber.

  @param[in]  TokenNumber The PCD token number to retrieve a current value for.

  @return Returns the 64-bit value for the token specified by TokenNumber.

**/
UINT64
EFIAPI
LibPcdGet64 (
  IN UINTN             TokenNumber
  )
{
  return PeiPcdGet64 (TokenNumber);
}



/**
  This function provides a means by which to retrieve a value for a given PCD token.

  Returns the pointer to the buffer of the token specified by TokenNumber.

  @param[in]  TokenNumber The PCD token number to retrieve a current value for.

  @return Returns the pointer to the token specified by TokenNumber.

**/
VOID *
EFIAPI
LibPcdGetPtr (
  IN UINTN             TokenNumber
  )
{
  return PeiPcdGetPtr (TokenNumber);
}



/**
  This function provides a means by which to retrieve a value for a given PCD token.

  Returns the Boolean value of the token specified by TokenNumber.

  @param[in]  TokenNumber The PCD token number to retrieve a current value for.

  @return Returns the Boolean value of the token specified by TokenNumber.

**/
BOOLEAN
EFIAPI
LibPcdGetBool (
  IN UINTN             TokenNumber
  )
{
  return PeiPcdGetBool (TokenNumber);
}



/**
  This function provides a means by which to retrieve the size of a given PCD token.

  @param[in]  TokenNumber The PCD token number to retrieve a current value for.

  @return Returns the size of the token specified by TokenNumber.

**/
UINTN
EFIAPI
LibPcdGetSize (
  IN UINTN             TokenNumber
  )
{
  return PeiPcdGetSize (TokenNumber);
}


#ifndef DISABLE_NEW_DEPRECATED_INTERFACES
/**
  This function provides a means by which to set a value for a given PCD token.

  Sets the 8-bit value for the token specified by TokenNumber
  to the value specified by Value.  Value is returned.

  @param[in]  TokenNumber   The PCD token number to set a current value for.
  @param[in]  Value         The 8-bit value to set.

  @return Return the value that was set.

**/
UINT8
EFIAPI
LibPcdSet8 (
  IN UINTN             TokenNumber,
  IN UINT8             Value
  )
{
  PeiPcdSet8 (TokenNumber, Value);

  return Value;
}



/**
  This function provides a means by which to set a value for a given PCD token.

  Sets the 16-bit value for the token specified by TokenNumber
  to the value specified by Value.  Value is returned.

  @param[in]  TokenNumber   The PCD token number to set a current value for.
  @param[in]  Value         The 16-bit value to set.

  @return Return the value that was set.

**/
UINT16
EFIAPI
LibPcdSet16 (
  IN UINTN             TokenNumber,
  IN UINT16            Value
  )
{
  PeiPcdSet16 (TokenNumber, Value);

  return Value;
}



/**
  This function provides a means by which to set a value for a given PCD token.

  Sets the 32-bit value for the token specified by TokenNumber
  to the value specified by Value.  Value is returned.

  @param[in]  TokenNumber   The PCD token number to set a current value for.
  @param[in]  Value         The 32-bit value to set.

  @return Return the value that was set.

**/
UINT32
EFIAPI
LibPcdSet32 (
  IN UINTN             TokenNumber,
  IN UINT32            Value
  )
{
  PeiPcdSet32 (TokenNumber, Value);

  return Value;
}



/**
  This function provides a means by which to set a value for a given PCD token.

  Sets the 64-bit value for the token specified by TokenNumber
  to the value specified by Value.  Value is returned.

  @param[in]  TokenNumber   The PCD token number to set a current value for.
  @param[in]  Value         The 64-bit value to set.

  @return Return the value that was set.

**/
UINT64
EFIAPI
LibPcdSet64 (
  IN UINTN             TokenNumber,
  IN UINT64            Value
  )
{
  PeiPcdSet64 (TokenNumber, Value);

  return Value;
}



/**
  This function provides a means by which to set a value for a given PCD token.

  Sets a buffer for the token specified by TokenNumber to the value
  specified by Buffer and SizeOfBuffer.  Buffer is returned.
  If SizeOfBuffer is greater than the maximum size support by TokenNumber,
  then set SizeOfBuffer to the maximum size supported by TokenNumber and
  return NULL to indicate that the set operation was not actually performed.

  If SizeOfBuffer is set to MAX_ADDRESS, then SizeOfBuffer must be set to the
  maximum size supported by TokenName and NULL must be returned.

  If SizeOfBuffer is NULL, then ASSERT().
  If SizeOfBuffer > 0 and Buffer is NULL, then ASSERT().

  @param[in]      TokenNumber   The PCD token number to set a current value for.
  @param[in, out] SizeOfBuffer  The size, in bytes, of Buffer.
  @param[in]      Buffer        A pointer to the buffer to set.

  @return Return the pointer for the buffer been set.

**/
VOID *
EFIAPI
LibPcdSetPtr (
  IN        UINTN             TokenNumber,
  IN OUT    UINTN             *SizeOfBuffer,
  IN CONST  VOID              *Buffer
  )
{
  EFI_STATUS Status;
  UINTN      InputSizeOfBuffer;

  ASSERT (SizeOfBuffer != NULL);

  if (*SizeOfBuffer > 0) {
    ASSERT (Buffer != NULL);
  }

  InputSizeOfBuffer = *SizeOfBuffer;
  Status = PeiPcdSetPtr (TokenNumber, SizeOfBuffer, (VOID *) Buffer);
  if (EFI_ERROR (Status) && (*SizeOfBuffer < InputSizeOfBuffer)) {
    return NULL;
  }

  return (VOID *) Buffer;
}



/**
  This function provides a means by which to set a value for a given PCD token.

  Sets the Boolean value for the token specified by TokenNumber
  to the value specified by Value.  Value is returned.

  @param[in]  TokenNumber   The PCD token number to set a current value for.
  @param[in]  Value         The boolean value to set.

  @return Return the value that was set.

**/
BOOLEAN
EFIAPI
LibPcdSetBool (
  IN UINTN             TokenNumber,
  IN BOOLEAN           Value
  )
{
  PeiPcdSetBool (TokenNumber, Value);

  return Value;
}

#endif

/**
  This function provides a means by which to set a value for a given PCD token.

  Sets the 8-bit value for the token specified by TokenNumber
  to the value specified by Value.

  @param[in] TokenNumber    The PCD token number to set a current value for.
  @param[in] Value          The 8-bit value to set.

  @return The status of the set operation.

**/
RETURN_STATUS
EFIAPI
LibPcdSet8S (
  IN UINTN          TokenNumber,
  IN UINT8          Value
  )
{
  return PeiPcdSet8 (TokenNumber, Value);
}

/**
  This function provides a means by which to set a value for a given PCD token.

  Sets the 16-bit value for the token specified by TokenNumber
  to the value specified by Value.

  @param[in] TokenNumber    The PCD token number to set a current value for.
  @param[in] Value          The 16-bit value to set.

  @return The status of the set operation.

**/
RETURN_STATUS
EFIAPI
LibPcdSet16S (
  IN UINTN          TokenNumber,
  IN UINT16         Value
  )
{
  return PeiPcdSet16 (TokenNumber, Value);
}

/**
  This function provides a means by which to set a value for a given PCD token.

  Sets the 32-bit value for the token specified by TokenNumber
  to the value specified by Value.

  @param[in] TokenNumber    The PCD token number to set a current value for.
  @param[in] Value          The 32-bit value to set.

  @return The status of the set operation.

**/
RETURN_STATUS
EFIAPI
LibPcdSet32S (
  IN UINTN          TokenNumber,
  IN UINT32         Value
  )
{
  return PeiPcdSet32 (TokenNumber, Value);
}

/**
  This function provides a means by which to set a value for a given PCD token.

  Sets the 64-bit value for the token specified by TokenNumber
  to the value specified by Value.

  @param[in] TokenNumber    The PCD token number to set a current value for.
  @param[in] Value          The 64-bit value to set.

  @return The status of the set operation.

**/
RETURN_STATUS
EFIAPI
LibPcdSet64S (
  IN UINTN          TokenNumber,
  IN UINT64         Value
  )
{
  return PeiPcdSet64 (TokenNumber, Value);
}

/**
  This function provides a means by which to set a value for a given PCD token.

  Sets a buffer for the token specified by TokenNumber to the value specified
  by Buffer and SizeOfBuffer. If SizeOfBuffer is greater than the maximum size
  support by TokenNumber, then set SizeOfBuffer to the maximum size supported by
  TokenNumber and return EFI_INVALID_PARAMETER to indicate that the set operation
  was not actually performed.

  If SizeOfBuffer is set to MAX_ADDRESS, then SizeOfBuffer must be set to the
  maximum size supported by TokenName and EFI_INVALID_PARAMETER must be returned.

  If SizeOfBuffer is NULL, then ASSERT().
  If SizeOfBuffer > 0 and Buffer is NULL, then ASSERT().

  @param[in]      TokenNumber   The PCD token number to set a current value for.
  @param[in, out] SizeOfBuffer  The size, in bytes, of Buffer.
  @param[in]      Buffer        A pointer to the buffer to set.

  @return The status of the set operation.

**/
RETURN_STATUS
EFIAPI
LibPcdSetPtrS (
  IN       UINTN    TokenNumber,
  IN OUT   UINTN    *SizeOfBuffer,
  IN CONST VOID     *Buffer
  )
{
  ASSERT (SizeOfBuffer != NULL);

  if (*SizeOfBuffer > 0) {
    ASSERT (Buffer != NULL);
  }

  return PeiPcdSetPtr (TokenNumber, SizeOfBuffer, (VOID *) Buffer);
}

/**
  This function provides a means by which to set a value for a given PCD token.

  Sets the boolean value for the token specified by TokenNumber
  to the value specified by Value.

  @param[in] TokenNumber    The PCD token number to set a current value for.
  @param[in] Value          The boolean value to set.

  @return The status of the set operation.

**/
RETURN_STATUS
EFIAPI
LibPcdSetBoolS (
  IN UINTN          TokenNumber,
  IN BOOLEAN        Value
  )
{
  return PeiPcdSetBool (TokenNumber, Value);
}
