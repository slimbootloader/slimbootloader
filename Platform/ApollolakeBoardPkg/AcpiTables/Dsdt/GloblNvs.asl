/** @file
  ACPI GNVS


  Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

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
    RSV3,    8,     //   Reserved for alignment
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
}
