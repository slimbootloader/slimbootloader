/** @file
  This header file provides common definitions just for System Agent
  using to avoid including extra module's file.

  Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#ifndef _SA_COMMON_DEFINITIONS_H_
#define _SA_COMMON_DEFINITIONS_H_

#include <Library/PcdLib.h>

#define MmPciAddress( Segment, Bus, Device, Function, Register ) \
  ( (UINTN) PcdGet64 (PcdPciExpressBaseAddress) + \
    (UINTN)(Bus << 20) + \
    (UINTN)(Device << 15) + \
    (UINTN)(Function << 12) + \
    (UINTN)(Register) \
  )

//
// UINT64
//
#define MmPci64Ptr( Segment, Bus, Device, Function, Register ) \
  ( (volatile UINT64 *)MmPciAddress( Segment, Bus, Device, Function, Register ) )

#define MmPci64( Segment, Bus, Device, Function, Register ) \
  *MmPci64Ptr( Segment, Bus, Device, Function, Register )

#define MmPci64Or( Segment, Bus, Device, Function, Register, OrData ) \
  MmPci64( Segment, Bus, Device, Function, Register ) = \
    (UINT64) ( \
      MmPci64( Segment, Bus, Device, Function, Register ) | \
      (UINT64)(OrData) \
    )

#define MmPci64And( Segment, Bus, Device, Function, Register, AndData ) \
  MmPci64( Segment, Bus, Device, Function, Register ) = \
    (UINT64) ( \
      MmPci64( Segment, Bus, Device, Function, Register ) & \
      (UINT64)(AndData) \
    )

#define MmPci64AndThenOr( Segment, Bus, Device, Function, Register, AndData, OrData ) \
  MmPci64( Segment, Bus, Device, Function, Register ) = \
    (UINT64) ( \
      ( MmPci64( Segment, Bus, Device, Function, Register ) & \
          (UINT64)(AndData) \
      ) | \
      (UINT64)(OrData) \
    )

//
// UINT32
//

#define MmPci32Ptr( Segment, Bus, Device, Function, Register ) \
  ( (volatile UINT32 *) MmPciAddress( Segment, Bus, Device, Function, Register ) )

#define MmPci32( Segment, Bus, Device, Function, Register ) \
  *MmPci32Ptr( Segment, Bus, Device, Function, Register )

#define MmPci32Or( Segment, Bus, Device, Function, Register, OrData ) \
  MmPci32( Segment, Bus, Device, Function, Register ) = \
    (UINT32) ( \
      MmPci32( Segment, Bus, Device, Function, Register ) | \
      (UINT32)(OrData) \
    )

#define MmPci32And( Segment, Bus, Device, Function, Register, AndData ) \
  MmPci32( Segment, Bus, Device, Function, Register ) = \
    (UINT32) ( \
      MmPci32( Segment, Bus, Device, Function, Register ) & \
      (UINT32)(AndData) \
    )

#define MmPci32AndThenOr( Segment, Bus, Device, Function, Register, AndData, OrData ) \
  MmPci32( Segment, Bus, Device, Function, Register ) = \
    (UINT32) ( \
      ( MmPci32( Segment, Bus, Device, Function, Register ) & \
          (UINT32)(AndData) \
      ) | \
      (UINT32)(OrData) \
    )

//
// UINT16
//

#define MmPci16Ptr( Segment, Bus, Device, Function, Register ) \
  ( (volatile UINT16 *)MmPciAddress( Segment, Bus, Device, Function, Register ) )

#define MmPci16( Segment, Bus, Device, Function, Register ) \
  *MmPci16Ptr( Segment, Bus, Device, Function, Register )

#define MmPci16Or( Segment, Bus, Device, Function, Register, OrData ) \
  MmPci16( Segment, Bus, Device, Function, Register ) = \
    (UINT16) ( \
      MmPci16( Segment, Bus, Device, Function, Register ) | \
      (UINT16)(OrData) \
    )

#define MmPci16And( Segment, Bus, Device, Function, Register, AndData ) \
  MmPci16( Segment, Bus, Device, Function, Register ) = \
    (UINT16) ( \
      MmPci16( Segment, Bus, Device, Function, Register ) & \
      (UINT16)(AndData) \
    )

#define MmPci16AndThenOr( Segment, Bus, Device, Function, Register, AndData, OrData ) \
  MmPci16( Segment, Bus, Device, Function, Register ) = \
    (UINT16) ( \
      ( MmPci16( Segment, Bus, Device, Function, Register ) & \
          (UINT16)(AndData) \
      ) | \
      (UINT16)(OrData) \
    )

//
// UINT8
//

#define MmPci8Ptr( Segment, Bus, Device, Function, Register ) \
  ( (volatile UINT8 *)MmPciAddress( Segment, Bus, Device, Function, Register ) )

#define MmPci8( Segment, Bus, Device, Function, Register ) \
  *MmPci8Ptr( Segment, Bus, Device, Function, Register )

#define MmPci8Or( Segment, Bus, Device, Function, Register, OrData ) \
  MmPci8( Segment, Bus, Device, Function, Register ) = \
    (UINT8) ( \
      MmPci8( Segment, Bus, Device, Function, Register ) | \
      (UINT8)(OrData) \
    )

#define MmPci8And( Segment, Bus, Device, Function, Register, AndData ) \
  MmPci8( Segment, Bus, Device, Function, Register ) = \
    (UINT8) ( \
      MmPci8( Segment, Bus, Device, Function, Register ) & \
      (UINT8)(AndData) \
    )

#define MmPci8AndThenOr( Segment, Bus, Device, Function, Register, AndData, OrData ) \
  MmPci8( Segment, Bus, Device, Function, Register ) = \
    (UINT8) ( \
      ( MmPci8( Segment, Bus, Device, Function, Register ) & \
          (UINT8)(AndData) \
        ) | \
      (UINT8)(OrData) \
    )

#endif

