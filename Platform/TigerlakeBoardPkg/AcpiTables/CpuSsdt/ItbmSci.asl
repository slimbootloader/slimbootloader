/** @file
  This file contains the Intel Turbo Boost Max Technology 3.0 ASL SCI handler.

  Copyright (c) 2017 - 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include "CpuPowerMgmt.h"

DefinitionBlock (
  "ItbmSci.aml",
  "SSDT",
  0x02,
  "PmRef",
  "ItbmSci",
  0x3000
  )
{
External(\_SB.PR00, DeviceObj)
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
External(\_SB.PR16, ProcessorObj)
External(\_SB.PR17, ProcessorObj)
External(\_SB.PR18, ProcessorObj)
External(\_SB.PR19, ProcessorObj)
External(\_SB.PR20, ProcessorObj)
External(\_SB.PR21, ProcessorObj)
External(\_SB.PR22, ProcessorObj)
External(\_SB.PR23, ProcessorObj)

External(\TCNT, FieldUnitObj)

Scope (\_GPE)
{
  //
  // ITBH: Intel Turbo Boost Max Technology 3.0 Handler for SCI _L62
  //
  Method(ITBH, 0, Serialized)
  {
    Store(ToInteger(TCNT), Local0)
    Notify(\_SB.PR00, 0x85)
    If(LGreater(Local0,  1)) {Notify(\_SB.PR01, 0x85)}
    If(LGreater(Local0,  2)) {Notify(\_SB.PR02, 0x85)}
    If(LGreater(Local0,  3)) {Notify(\_SB.PR03, 0x85)}
    If(LGreater(Local0,  4)) {Notify(\_SB.PR04, 0x85)}
    If(LGreater(Local0,  5)) {Notify(\_SB.PR05, 0x85)}
    If(LGreater(Local0,  6)) {Notify(\_SB.PR06, 0x85)}
    If(LGreater(Local0,  7)) {Notify(\_SB.PR07, 0x85)}
    If(LGreater(Local0,  8)) {Notify(\_SB.PR08, 0x85)}
    If(LGreater(Local0,  9)) {Notify(\_SB.PR09, 0x85)}
    If(LGreater(Local0, 10)) {Notify(\_SB.PR10, 0x85)}
    If(LGreater(Local0, 11)) {Notify(\_SB.PR11, 0x85)}
    If(LGreater(Local0, 12)) {Notify(\_SB.PR12, 0x85)}
    If(LGreater(Local0, 13)) {Notify(\_SB.PR13, 0x85)}
    If(LGreater(Local0, 14)) {Notify(\_SB.PR14, 0x85)}
    If(LGreater(Local0, 15)) {Notify(\_SB.PR15, 0x85)}
    If(LGreater(Local0, 16)) {Notify(\_SB.PR16, 0x85)}
    If(LGreater(Local0, 17)) {Notify(\_SB.PR17, 0x85)}
    If(LGreater(Local0, 18)) {Notify(\_SB.PR18, 0x85)}
    If(LGreater(Local0, 19)) {Notify(\_SB.PR19, 0x85)}
    If(LGreater(Local0, 20)) {Notify(\_SB.PR20, 0x85)}
    If(LGreater(Local0, 21)) {Notify(\_SB.PR21, 0x85)}
    If(LGreater(Local0, 22)) {Notify(\_SB.PR22, 0x85)}
    If(LGreater(Local0, 23)) {Notify(\_SB.PR23, 0x85)}
  }

} // end of Scope (\_GPE)
} // end of definition block
