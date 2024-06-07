/** @file
  Header file for register access.

  Copyright (c) 2024, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _REGISTER_ACCESS_H_
#define _REGISTER_ACCESS_H_

typedef struct _REGISTER_ACCESS REGISTER_ACCESS;

/**
  Reads an 8-bit register

  @param[in] This    Pointer to REGISTER_ACCESS
  @param[in] Offset  Offset of the register in the register bank

  @return The 8-bit register value specified by Offset
**/
typedef
UINT8
(*REG_READ8) (
  REGISTER_ACCESS  *This,
  UINT32           Offset
  );

/**
  Writes an 8-bit register

  @param[in] This    Pointer to REGISTER_ACCESS
  @param[in] Offset  Offset of the register in the register bank
  @param[in] Value   Value to write to register

  @return The 8-bit register value written to register
**/
typedef
UINT8
(*REG_WRITE8) (
  REGISTER_ACCESS  *This,
  UINT32           Offset,
  UINT8            Value
  );

/**
  Performs an 8-bit or on the register

  @param[in] This    Pointer to REGISTER_ACCESS
  @param[in] Offset  Offset of the register in the register bank
  @param[in] OrData  Data with which register should be OR-ed

  @return The 8-bit register value written to register
**/
typedef
UINT8
(*REG_OR8) (
  REGISTER_ACCESS  *This,
  UINT32           Offset,
  UINT8            OrData
  );

/**
  Performs an 8-bit and on the register

  @param[in] This     Pointer to REGISTER_ACCESS
  @param[in] Offset   Offset of the register in the register bank
  @param[in] AndData  Data with which register should be AND-ed

  @return The 8-bit register value written to register
**/
typedef
UINT8
(*REG_AND8) (
  REGISTER_ACCESS  *This,
  UINT32           Offset,
  UINT8            AndData
  );

/**
  Performs an 8-bit and then or on the register

  @param[in] This     Pointer to REGISTER_ACCESS
  @param[in] Offset   Offset of the register in the register bank
  @param[in] AndData  Data with which register should be AND-ed
  @param[in] OrData   Data with which register should be OR-ed

  @return The 8-bit register value written to register
**/
typedef
UINT8
(*REG_AND_THEN_OR8) (
  REGISTER_ACCESS  *This,
  UINT32           Offset,
  UINT8            AndData,
  UINT8            OrData
  );

/**
  Reads a 16-bit register

  @param[in] This    Pointer to REGISTER_ACCESS
  @param[in] Offset  Offset of the register in the register bank

  @return The 16-bit register value specified by Offset
**/
typedef
UINT16
(*REG_READ16) (
  REGISTER_ACCESS  *This,
  UINT32           Offset
  );

/**
  Writes a 16-bit register

  @param[in] This    Pointer to REGISTER_ACCESS
  @param[in] Offset  Offset of the register in the register bank
  @param[in] Value   Value to write to register

  @return The 16-bit register value written to register
**/
typedef
UINT16
(*REG_WRITE16) (
  REGISTER_ACCESS  *This,
  UINT32           Offset,
  UINT16           Value
  );

/**
  Performs a 16-bit or on the register

  @param[in] This    Pointer to REGISTER_ACCESS
  @param[in] Offset  Offset of the register in the register bank
  @param[in] OrData  Data with which register should be OR-ed

  @return The 16-bit register value written to register
**/
typedef
UINT16
(*REG_OR16) (
  REGISTER_ACCESS  *This,
  UINT32           Offset,
  UINT16           OrData
  );

/**
  Performs a 16-bit and on the register

  @param[in] This     Pointer to REGISTER_ACCESS
  @param[in] Offset   Offset of the register in the register bank
  @param[in] AndData  Data with which register should be AND-ed

  @return The 16-bit register value written to register
**/
typedef
UINT16
(*REG_AND16) (
  REGISTER_ACCESS  *This,
  UINT32           Offset,
  UINT16           AndData
  );

/**
  Performs a 16-bit and then or on the register

  @param[in] This     Pointer to REGISTER_ACCESS
  @param[in] Offset   Offset of the register in the register bank
  @param[in] AndData  Data with which register should be AND-ed
  @param[in] OrData   Data with which register should be OR-ed

  @return The 16-bit register value written to register
**/
typedef
UINT16
(*REG_AND_THEN_OR16) (
  REGISTER_ACCESS  *This,
  UINT32           Offset,
  UINT16           AndData,
  UINT16           OrData
  );

/**
  Reads a 32-bit register

  @param[in] This    Pointer to REGISTER_ACCESS
  @param[in] Offset  Offset of the register in the register bank

  @return The 32-bit register value specified by Offset
**/
typedef
UINT32
(*REG_READ32) (
  REGISTER_ACCESS  *This,
  UINT32           Offset
  );

/**
  Writes a 32-bit register

  @param[in] This    Pointer to REGISTER_ACCESS
  @param[in] Offset  Offset of the register in the register bank
  @param[in] Value   Value to write to register

  @return The 32-bit register value written to register
**/
typedef
UINT32
(*REG_WRITE32) (
  REGISTER_ACCESS  *This,
  UINT32           Offset,
  UINT32           Value
  );

/**
  Performs a 32-bit or on the register

  @param[in] This    Pointer to REGISTER_ACCESS
  @param[in] Offset  Offset of the register in the register bank
  @param[in] OrData  Data with which register should be OR-ed

  @return The 32-bit register value written to register
**/
typedef
UINT32
(*REG_OR32) (
  REGISTER_ACCESS  *This,
  UINT32           Offset,
  UINT32           OrData
  );

/**
  Performs a 32-bit and on the register

  @param[in] This     Pointer to REGISTER_ACCESS
  @param[in] Offset   Offset of the register in the register bank
  @param[in] AndData  Data with which register should be AND-ed

  @return The 32-bit register value written to register
**/
typedef
UINT32
(*REG_AND32) (
  REGISTER_ACCESS  *This,
  UINT32           Offset,
  UINT32           AndData
  );

/**
  Performs a 32-bit and then or on the register

  @param[in] This     Pointer to REGISTER_ACCESS
  @param[in] Offset   Offset of the register in the register bank
  @param[in] AndData  Data with which register should be AND-ed
  @param[in] OrData   Data with which register should be OR-ed

  @return The 32-bit register value written to register
**/
typedef
UINT32
(*REG_AND_THEN_OR32) (
  REGISTER_ACCESS  *This,
  UINT32           Offset,
  UINT32           AndData,
  UINT32           OrData
  );

struct _REGISTER_ACCESS {
  REG_READ8         Read8;
  REG_WRITE8        Write8;
  REG_OR8           Or8;
  REG_AND8          And8;
  REG_AND_THEN_OR8  AndThenOr8;

  REG_READ16         Read16;
  REG_WRITE16        Write16;
  REG_OR16           Or16;
  REG_AND16          And16;
  REG_AND_THEN_OR16  AndThenOr16;

  REG_READ32         Read32;
  REG_WRITE32        Write32;
  REG_OR32           Or32;
  REG_AND32          And32;
  REG_AND_THEN_OR32  AndThenOr32;
};

#endif
