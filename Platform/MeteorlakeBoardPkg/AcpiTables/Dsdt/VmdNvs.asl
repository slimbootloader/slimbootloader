/** @file

  Copyright (c) 2023, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

  //
  // Define VMD NVS Area operation region.
  //


  OperationRegion(VMNV,SystemMemory,VMNB,VMNL)
  Field(VMNV,AnyAcc,Lock,Preserve)
  {  Offset(0),      VMDE, 8,  // Offset(0),     VMD Device Enable
  Offset(1),      VMSP, 32, // Offset(1),     VMD SOC and IOE PCIe RP mapped under VMD
  Offset(5),      VMPP, 32, // Offset(5),     VMD PCH PCIe RP mapped under VMD
  Offset(9),      VMS0, 8,  // Offset(9),     VMD SATA PORT 0 to 7 mapped under VMD
  Offset(10),     PBUS, 8,  // Offset(10),    PCH ROOT PORT BUS Number
  }