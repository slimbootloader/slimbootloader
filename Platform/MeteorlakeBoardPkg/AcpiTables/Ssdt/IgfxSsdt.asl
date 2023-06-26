/** @file
  This file contains the Intel Graphics SSDT Table ASL code.

  Copyright (c) 2023, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

DefinitionBlock (
  "IgfxSsdt.aml",
  "SSDT",
  2,
  "INTEL ",
  "IgfxSsdt",
  0x3000
  )
{
  External(\_SB.PC00, DeviceObj)
  External(\_SB.PC00.MC, DeviceObj)
  External(\_SB.PC00.GFX0, DeviceObj)
  External(\NDID)
  External(\DID1)
  External(\DID2)
  External(\DID3)
  External(\DID4)
  External(\DID5)
  External(\DID6)
  External(\DID7)
  External(\DID8)
  External(\DID9)
  External(\DIDA)
  External(\DIDB)
  External(\DIDC)
  External(\DIDD)
  External(\DIDE)
  External(\DIDF)
  External(\DIDX)
  External(\DIDY)

  External(\NXD1)
  External(\NXD2)
  External(\NXD3)
  External(\NXD4)
  External(\NXD5)
  External(\NXD6)
  External(\NXD7)
  External(\NXD8)
  External(\NXDY)

  External (\IPTP)
  External (\EDPV)
  External (\NXDX)
  External (\HGDT)
  External (\LIDS)
  External (\BRTL)
  External (\ASLB)

  Scope (\_SB.PC00.GFX0)
  {
    include("Igfx.asl")

    Method(_S3D, 0, NotSerialized)
    {
      Return(3)
    }
    Method(_S4D, 0, NotSerialized)
    {
      Return(3)
    }
    Method(_PS0,0,Serialized)
    {
    }
    Method(_PS3,0,Serialized)
    {
    }
  }
}
