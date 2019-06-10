/** @file
  This file contains the SystemAgent SSDT Table ASL code.
  It defines a Global NVS table which exchanges datas between OS
  and BIOS.

  Copyright (c) 2018 - 2019, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

DefinitionBlock (
  "SaSsdt.aml",
  "SSDT",
  0x02,
  "SaSsdt",
  "SaSsdt ",
  0x3000
  )
{
  include ("SaNvs.asl")
  include ("Sa.asl")
}
