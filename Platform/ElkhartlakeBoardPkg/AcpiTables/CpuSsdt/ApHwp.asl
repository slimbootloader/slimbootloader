/** @file
  This file contains the APs HWP SSDT Table ASL code.

  Copyright (c) 2012 - 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

DefinitionBlock (
  "ApHwp.aml",
  "SSDT",
  2,
  "PmRef",
  "ApHwp",
  0x3000
  )
{

External(\_SB.PR00, ProcessorObj)
External(\_SB.PR01, ProcessorObj)
External(\_SB.PR02, ProcessorObj)
External(\_SB.PR03, ProcessorObj)
External(\_SB.PR04, ProcessorObj)
External(\_SB.PR05, ProcessorObj)
External(\_SB.PR06, ProcessorObj)
External(\_SB.PR07, ProcessorObj)
External(\_SB.PR08, ProcessorObj)
External(\_SB.PR09, ProcessorObj)
External(\_SB.PR10, ProcessorObj)
External(\_SB.PR11, ProcessorObj)
External(\_SB.PR12, ProcessorObj)
External(\_SB.PR13, ProcessorObj)
External(\_SB.PR14, ProcessorObj)
External(\_SB.PR15, ProcessorObj)
External(\_SB.PR00._CPC, MethodObj)

Scope(\_SB.PR01)
{
  Method(_CPC,0)
  {
    //Return the same table as PR00 for CMP cases.
    Return(\_SB.PR00._CPC)
  }
}//End of Scope(\_SB.PR01)

Scope(\_SB.PR02)
{
  Method(_CPC,0)
  {
    //Return the same table as PR00 for CMP cases.
    Return(\_SB.PR00._CPC)
  }
}//End of Scope(\_SB.PR02)

Scope(\_SB.PR03)
{
  Method(_CPC,0)
  {
    //Return the same table as PR00 for CMP cases.
    Return(\_SB.PR00._CPC)
  }
}//End of Scope(\_SB.PR03)

Scope(\_SB.PR04)
{
  Method(_CPC,0)
  {
    //Return the same table as PR00 for CMP cases.
    Return(\_SB.PR00._CPC)
  }
}//End of Scope(\_SB.PR04)

Scope(\_SB.PR05)
{
  Method(_CPC,0)
  {
    //Return the same table as PR00 for CMP cases.
    Return(\_SB.PR00._CPC)
  }
}//End of Scope(\_SB.PR05)

Scope(\_SB.PR06)
{
  Method(_CPC,0)
  {
    //Return the same table as PR00 for CMP cases.
    Return(\_SB.PR00._CPC)
  }
}//End of Scope(\_SB.PR06)

Scope(\_SB.PR07)
{
  Method(_CPC,0)
  {
    //Return the same table as PR00 for CMP cases.
    Return(\_SB.PR00._CPC)
  }
}//End of Scope(\_SB.PR07)

Scope(\_SB.PR08)
{
  Method(_CPC,0)
  {
    //Return the same table as PR00 for CMP cases.
    Return(\_SB.PR00._CPC)
  }
}//End of Scope(\_SB.PR08)

Scope(\_SB.PR09)
{
  Method(_CPC,0)
  {
    //Return the same table as PR00 for CMP cases.
    Return(\_SB.PR00._CPC)
  }
}//End of Scope(\_SB.PR09)

Scope(\_SB.PR10)
{
  Method(_CPC,0)
  {
    //Return the same table as PR00 for CMP cases.
    Return(\_SB.PR00._CPC)
  }
}//End of Scope(\_SB.PR10)

Scope(\_SB.PR11)
{
  Method(_CPC,0)
  {
    //Return the same table as PR00 for CMP cases.
    Return(\_SB.PR00._CPC)
  }
}//End of Scope(\_SB.PR11)

Scope(\_SB.PR12)
{
  Method(_CPC,0)
  {
    //Return the same table as PR00 for CMP cases.
    Return(\_SB.PR00._CPC)
  }
}//End of Scope(\_SB.PR12)

Scope(\_SB.PR13)
{
  Method(_CPC,0)
  {
    //Return the same table as PR00 for CMP cases.
    Return(\_SB.PR00._CPC)
  }
}//End of Scope(\_SB.PR13)

Scope(\_SB.PR14)
{
  Method(_CPC,0)
  {
    //Return the same table as PR00 for CMP cases.
    Return(\_SB.PR00._CPC)
  }
}//End of Scope(\_SB.PR14)

Scope(\_SB.PR15)
{
  Method(_CPC,0)
  {
    //Return the same table as PR00 for CMP cases.
    Return(\_SB.PR00._CPC)
  }
}//End of Scope(\_SB.PR15)
}// end of definition block
