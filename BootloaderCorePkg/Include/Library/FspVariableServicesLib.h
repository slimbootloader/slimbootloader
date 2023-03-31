/** @file

  Copyright (c) 2023, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#ifndef __FSP_VARIABLE_SERVICES_LIB_H__
#define __FSP_VARIABLE_SERVICES_LIB_H__

/**
  FSP wrapper for SBL's Get Variable implementation.

  @param VariableName
  @param VariableGuid
  @param Attributes
  @param DataSize
  @param Data
  @return EFI_STATUS
 */
EFI_STATUS
EFIAPI
FspGetVariable(
  IN  CHAR16      *VariableName,
  IN  EFI_GUID    *VariableGuid,
  OUT UINT32      *Attributes,
  IN OUT UINT64   *DataSize,
  IN  VOID        *Data
);

/**
  FSP wrapper for SBL's Get Next Variable implementation.
  This must always be called successively without any other
  LiteVariable GetNextVariablName calls in between.

  @param VariableNameSize
  @param VariableName
  @param VariableGuid
  @return EFI_STATUS
 */
EFI_STATUS
EFIAPI
FspGetNextVariableName(
  IN OUT  UINT64   *VariableNameSize,
  OUT     CHAR16   *VariableName,
  OUT     EFI_GUID *VariableGuid
);

/**
  FSP wrapper for SBL's Set Variable implementation.

  @param VariableName
  @param VariableGuid
  @param Attributes
  @param DataSize
  @param Data
  @return EFI_STATUS
 */
EFI_STATUS
EFIAPI
FspSetVariable(
  IN  CHAR16      *VariableName,
  IN  EFI_GUID    *VariableGuid,
  IN  UINT32      *Attributes,
  IN  UINT64      *DataSize,
  IN  VOID        *Data
);

/**
  SBL implementation of FSP QueryVariableInfo API

  @param Attributes
  @param MaximumVariableStorageSize
  @param RemainingVariableStorageSize
  @param MaximumVariableSize
  @return EFI_STATUS
 */
EFI_STATUS
EFIAPI
FspQueryVariableInfo(
  IN  UINT32      *Attributes,
  OUT UINT64      *MaximumVariableStorageSize,
  OUT UINT64      *RemainingVariableStorageSize,
  OUT UINT64      *MaximumVariableSize
);
#endif //__FSP_VARIABLE_SERVICES_LIB_H__
