/** @file

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Base.h>
#include <Uefi.h>
#include "IioSocketTopology.h"
#include "Stage2BoardInitLib.h"

STACK_TO_PE StackToPeGnrD[] = {
  { IioStack2, PE0},
  { IioStack3, PE1},
  { IioStack5, PE2},
};


UINT8 Ioat2StackGnrD[] = {
  IioStack0,  //HCTA0
  IioStack7,  //HCTA3
  IioStack6,  //TIP1
  IioStack8,  //TIP2
  IioStack4,  //NAC0
  IioStack1,  //NAC1
  IioStack9,  //NAC2
  IioStack10, //NAC3
};

/**
  Get Stack ID from the Stack FW Instance.

  @param          - Socket ID
  @param          - Stack FW Instance

  @retval         The Stack ID of the Stack FW instance.
**/
UINT8
GetStackId (
  UINT8  Socket,
  UINT8  StackFwInstance
  )
{
  UINT8 StackId[MAX_IIO_STACK] = {IIO_STACK_0, IIO_STACK_7, IIO_STACK_6, IIO_STACK_8, \
                                  IIO_STACK_4, IIO_STACK_1, IIO_STACK_9, IIO_STACK_10, \
                                  IIO_STACK_2, IIO_STACK_3, IIO_STACK_5, IIO_STACK_INVALID, \
                                  IIO_STACK_INVALID,IIO_STACK_INVALID};

  if (StackFwInstance < MAX_IIO_STACK) {
    return StackId[StackFwInstance];
  }
  return IIO_STACK_INVALID;
}

/**
  Get the Stack FW Instance Index via the Stack ID.
    The Stack FW instance index can be used to get the stack's data structure (resource, attribute ...)

  @param          - Socket ID
  @param          - Stack ID

  @retval         The FW Instance Index of the Stack.
**/
UINT8
GetStackFwInstance (
  UINT8  Socket,
  UINT8  StackId
  )
{
  UINT8  Index;
  UINT8  StackIdArr[MAX_IIO_STACK] = {IIO_STACK_0, IIO_STACK_7, IIO_STACK_6, IIO_STACK_8, \
                                      IIO_STACK_4, IIO_STACK_1, IIO_STACK_9, IIO_STACK_10, \
                                      IIO_STACK_2, IIO_STACK_3, IIO_STACK_5, IIO_STACK_INVALID, \
                                      IIO_STACK_INVALID,IIO_STACK_INVALID};

  if (StackId < MAX_IIO_STACK) {
    for (Index = 0; Index < MAX_IIO_STACK; Index++) {
      if (StackIdArr[Index] == StackId) {
        return Index;
      }
    }
  }
  return IIO_STACK_INVALID;
}

EFI_STATUS
FindStackPeRecord (
  IN     UINT8    Socket,
  IN OUT UINT8    *StackPtr,
  IN OUT UINT8    *PePtr,
  IN     BOOLEAN  LookForStackBasedOnPE
  )
{
  STACK_TO_PE *StackToPe;
  UINT8       EntriesCount;
  UINT8       Index;
  UINT8       StackId;

  if (StackPtr == NULL || PePtr == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  StackToPe = StackToPeGnrD;
  EntriesCount = ARRAY_SIZE (StackToPeGnrD);


  if (LookForStackBasedOnPE) {
    for (Index = 0; Index < EntriesCount; Index++) {
      if (StackToPe[Index].PackagePE == *PePtr) {
        //
        // found record with PE - now translate it to stack index based on KTI stacks remapping
        //
        *StackPtr = GetStackFwInstance (Socket, StackToPe[Index].StackId);
        if (*StackPtr == IioStackUnknown) {
          return EFI_NOT_FOUND;
        }
        return EFI_SUCCESS;
      }
    }  // for each row in STACK_TO_PE table
  } else {
    StackId = GetStackId (Socket, *StackPtr);
    for (Index = 0; Index < EntriesCount; Index++) {
      //
      // First translate from stack index remapped by KTI and then check if this is requested record
      //
      if (StackToPe[Index].StackId == StackId) {
        // found record with PE - save matched stack
        *PePtr = StackToPe[Index].PackagePE;
        return EFI_SUCCESS;
      }
    }  // for each row in STACK_TO_PE table
  }

  return EFI_NOT_FOUND;
}


/**
  Return Package PE value assigned to given stack on board.
  This can be used mostly to load board config.

  @param[in] Socket  Socket index
  @param[in] Stack   The stack index inside socket

  @return UINT8  Right package PE assigned to given stack
 */
UINT8
IioStack2PeIndex (
  IN  UINT8 Socket,
  IN  UINT8  Stack
  )
{
  EFI_STATUS  Status;
  UINT8       PeIndex;

  Status = FindStackPeRecord (Socket, &Stack, &PeIndex, LOOK_FOR_PE_BASED_ON_STACK);
  if (EFI_ERROR (Status)) {
    return PE_;
  }
  return PeIndex;
}

/**
  Return stack index assigned to given PE on board.
  This can be used mostly to load board config.

  @param[in] Socket   Socket index
  @param[in] PeIndex  PCI Express index in socket pinout

  @return UINT8  Right stack assigned to given PCI Express
 */
UINT8
IioPe2StackIndex (
  IN  UINT8   Socket,
  IN  UINT8   PeIndex
  )
{
  EFI_STATUS Status;
  UINT8  Stack;

  Status = FindStackPeRecord (Socket, &Stack, &PeIndex, LOOK_FOR_STACK_BASED_ON_PE);
  if (EFI_ERROR (Status)) {
    return IioStackUnknown;
  }
  return Stack;
}


/**
  Find stack index assigned to given PE or vice versa.

  @param[in]     Socket   - Index of socket.
  @param[in,out] StackPtr - pointer to index of stack
  @param[in,out] IoatPtr  - pointer to index of IO accelerators stack
  @param[in]     Direction- direction of lookup

  @retval EFI_INVALID_PARAMETER - invalid poiner provided.
  @retval EFI_UNSUPPORTED       - unknown IIO topology.
  @retval EFI_NOT_FOUND         - stack not found.
  @retval EFI_SUCCESS           - stack mapping provided.
 */
EFI_STATUS
FindStackIoatRecord (
  IN     UINT8    Socket,
  IN OUT UINT8    *StackPtr,
  IN OUT UINT8    *IoatPtr,
  IN     BOOLEAN  LookForStackBasedOnIoat
  )
{
  UINT8              Ioat2StackTableLen;
  UINT8         *Ioat2StackTable = NULL;
  UINT8              StackId;
  UINT8              Index;

  if (StackPtr == NULL || IoatPtr == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Ioat2StackTable = Ioat2StackGnrD;
  Ioat2StackTableLen = ARRAY_SIZE (Ioat2StackGnrD);

  if (LookForStackBasedOnIoat) {
    if (*IoatPtr >= Ioat2StackTableLen) {
      return EFI_NOT_FOUND;
    }

    *StackPtr =GetStackFwInstance (Socket, Ioat2StackTable[*IoatPtr]);
    return EFI_SUCCESS;
  } else {
    StackId = GetStackId (Socket, *StackPtr);

    for (Index = 0; Index < Ioat2StackTableLen; Index++) {
      if (StackId == Ioat2StackTable[Index]) {
        *IoatPtr = Index;
        return EFI_SUCCESS;
      }
    }
  }
  return EFI_NOT_FOUND;
}
