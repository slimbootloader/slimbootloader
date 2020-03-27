/** @file

  Copyright (c) 2017 - 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#include "Stage1B.h"

/**
  Load GDT table for current processor.

  It function initializes GDT table and loads it into current processor.

  @param  GdtTable  Pointer to STAGE_GDT_TABLE structure.

**/
VOID
LoadGdt (
  IN STAGE_GDT_TABLE   *GdtTable
  )
{
}

/**
  Load IDT table for current processor.

  It function initializes the exception handlers in IDT table and
  loads it into current processor.  It also try to store a specific DWORD
  data just before actual IDT base. It is used to set/get bootloader
  global data structure pointer.

  @param  IdtTable  Contain the IDT table pointer.
                    It must point to a STAGE_IDT_TABLE buffer preallocated.

  @param  Data      It contains a DWORD data that will be stored just before the actual IDT table.
                    It can be used to set/get global data using IDT pointer.

**/
VOID
LoadIdt (
  IN STAGE_IDT_TABLE   *IdtTable,
  IN UINT32             Data
  )
{
}

/**
  Remap the current stage image.

  It will create page tables to remap current Stage1B from CAR to Memory.
  It is necessary since CAR will be destroyed during CallFspTempRamExit().
**/
VOID
RemapStage (
  VOID
  )
{
}
