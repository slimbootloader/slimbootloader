/** @file
  CPU Exception Handler Library.

  Copyright (c) 2012 - 2017, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _CPU_EXCEPTION_LIB_H_
#define _CPU_EXCEPTION_LIB_H_

#define  CPU_EXCEPTION_NUM          32
#define  CPU_INTERRUPT_NUM         256

//
// Record exception handler information
//
typedef struct {
  UINTN ExceptionStart;
  UINTN ExceptionStubHeaderSize;
} EXCEPTION_HANDLER_TEMPLATE_MAP;


/**
  Return address map of exception handler template so that C code can generate
  exception tables.

  @param AddressMap[in, out]  Pointer to a buffer where the address map is returned.
**/
VOID
EFIAPI
AsmGetTemplateAddressMap (
  OUT EXCEPTION_HANDLER_TEMPLATE_MAP *AddressMap
  );


/**
  Update exception handler in IDT table .

  This function is used to update the IDT exception handler with current stage.

  @param[in]  IdtDescriptor   If not NULL, it is new IDT descriptor to be updated.
                              if it is NULL, the IDT descriptor will be retrieved from IDT base register.

**/
VOID
UpdateExceptionHandler (
  IN IA32_DESCRIPTOR         *IdtDescriptor
  );

#endif

