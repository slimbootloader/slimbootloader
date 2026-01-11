/**@file
  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

  //
  // Define TSE NVS Area operation region.
  //


  OperationRegion(TSNV,SystemMemory,TSNB,TSNL)
  Field(TSNV,AnyAcc,Lock,Preserve)
  {
  TSEE, 8,  // Offset(0),     TSE Device Enable
  }