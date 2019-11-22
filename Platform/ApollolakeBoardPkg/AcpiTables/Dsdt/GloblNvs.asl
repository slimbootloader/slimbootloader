/** @file
  ACPI GNVS


  Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/


  //
  // Define a Global region of ACPI NVS Region that may be used for any
  // type of implementation.  The starting offset and size will be fixed
  // up by the System BIOS during POST.  Note that the Size must be a word
  // in size to be fixed up correctly.
  //

  OperationRegion(GNVS, SystemMemory, 0xFFFF0000, 0xAA55)
  Field(GNVS, AnyAcc, Lock, Preserve)
  {
    Offset(0),      // Miscellaneous Dynamic Registers:

//  Name-----Size-------Offset----Description
    MPEN,    8,     //   Number of Logical Processors if MP Enabled != 0
    BDID,    8,     //   Platform board id
    RSV1,    8,     //   Reserved for alignment
    RSV2,    8,     //   Reserved for alignment
    ASLB,    32,    //   IGD OpRegion base address
    NHLA,    32,    //   HD-Audio NHLT ACPI address
    NHLL,    32,    //   HD-Audio NHLT ACPI length
    GP0A,    32,    //   GPIO0 BAR
    GP0L,    32,    //   GPIO0 BAR Length
    GP1A,    32,    //   GPIO1 BAR
    GP1L,    32,    //   GPIO1 BAR Length
    GP2A,    32,    //   GPIO2 BAR
    GP2L,    32,    //   GPIO2 BAR Length
    GP3A,    32,    //   GPIO3 BAR
    GP3L,    32,    //   GPIO3 BAR Length
    DD1A,    32,    //   IPC Bar0 Address
    DD1L,    32,    //   IPC Bar0 Length
    DD3A,    32,    //   IPC Bar1 Address
    DD3L,    32,    //   IPC Bar1 Length
    BMDA,    32,    //   IPC BIOS mail box data
    BMIA,    32,    //   IPC BIOS mail box interface
    P2BA,    32,    //   P2SB Base Address: only used by BXT-M
    IPCE,    8,     //   IPC1Enable
    STEP,    8,     //   BXT Stepping ID
    RSV4,    16,    //   Reserved for alignment
    RPA1,    32,    //   Root Port address 1
    RPA2,    32,    //   Root Port address 2
    RPA3,    32,    //   Root Port address 3
    RPA4,    32,    //   Root Port address 4
    RPA5,    32,    //   Root Port address 5
    RPA6,    32,    //   Root Port address 6
    M32B,    32,    //   PCIE MMIO resource base
    M32L,    32,    //   PCIE MMIO resource length
    ADFM,    32,    //   HD-Audio DSP Feature Mask
    ADPM,    32,    //   Hd-Audio DSP Post-Processing Module Mask
    RPW1,     8,    //   RP 1 wake GPE bit
    RPW2,     8,    //   RP 2 wake GPE bit
    RPW3,     8,    //   RP 3 wake GPE bit
    RPW4,     8,    //   RP 4 wake GPE bit
    RPW5,     8,    //   RP 5 wake GPE bit
    RPW6,     8,    //   RP 6 wake GPE bit
    RSV5,    16,    //   Reserved for alignment
}
