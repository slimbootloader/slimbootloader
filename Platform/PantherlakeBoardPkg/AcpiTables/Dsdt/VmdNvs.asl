/** @file

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

  //
  // Define VMD NVS Area operation region.
  //


  OperationRegion(VMNV,SystemMemory,VMNB,VMNL)
  Field(VMNV,AnyAcc,Lock,Preserve)
  {
  VMDE, 8,  // Offset(0),     VMD Device Enable
  VMSP, 32, // Offset(1),     VMD SOC and IOE PCIe RP mapped under VMD
  VMPP, 32, // Offset(5),     VMD PCH PCIe RP mapped under VMD
  VMS0, 8,  // Offset(9),     VMD SATA PORT 0 to 7 mapped under VMD
  PBUS, 8,  // Offset(10),    PCH ROOT PORT BUS Number
  }