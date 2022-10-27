/** @file
  Intel ACPI Reference Code for Intel Dynamic Power Performance Management

  Copyright (c) 2022, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

Scope(\_SB.IETM)
{

  // PTTL (Participant Temperature Tolerance Level)
  // This object evaluates to an integer representing the temperature range within which any change
  // in participant temperature is considered acceptable and no policy actions will be taken by the
  // policy. The temperature tolerance level is expressed in the units of 10s of Kelvin.
  //
  Name (PTTL, 20) // ToleranceLevel

  // Used by passive policy 2.0
  //
  //  Name (PSVT, Package()
  //  {
  //    0x80000000,     // Revision: 1 = true proportional limit, 2 = depth limit
  //    Package() {
  //      "Source",     // Source device: String
  //      "Target",     // Target device: String
  //      0x80000000,   // Priority: for use by passive policy 2.0
  //      0x80000000,   // Sampling Period: in 1/10 of seconds unit
  //      0x80000000,   // Passive Temp: Threshold(1/10 of K) at which Passive Policy 2.0 will take action
  //      0x80000000,   // Domain: type under the source that is controlled
  //      0x80000000,   // Control Knob: type of control knob that need to be exercised
  //      0x80000000,   // Control Value: Depth Limit or Target State which policy will jump to on a thermal condition(based on revision).
  //      0x80000000,   // Step size for this specific control knob
  //      0x80000000,   // Limit Coefficient: 1/10 of units, used with StepSize to control limiting action
  //      0x80000000,   // UnLimit Coefficient: 1/10 of units, used with StepSize to control unlimiting action
  //      0x80000000    // Reserved1
  //    }
  //  })
  Name (PSVT, Package()
  {
    2,
    Package(){\_SB.PC00.LPCB.H_EC.CHRG, \_SB.PC00.LPCB.H_EC.SEN3, 1, 200, 3182, 14, 0x000A0000, "MAX",   1, 10, 10, 0},
    Package(){\_SB.PC00.LPCB.H_EC.CHRG, \_SB.PC00.LPCB.H_EC.SEN3, 1, 200, 3232, 14, 0x000A0000,     1,   1, 10, 10, 0},
    Package(){\_SB.PC00.LPCB.H_EC.CHRG, \_SB.PC00.LPCB.H_EC.SEN3, 1, 200, 3282, 14, 0x000A0000,     2,   1, 10, 10, 0},
    Package(){\_SB.PC00.LPCB.H_EC.CHRG, \_SB.PC00.LPCB.H_EC.SEN3, 1, 200, 3382, 14, 0x000A0000, "MIN",   1, 10, 10, 0}
  })

}// end Scope(\_SB.IETM)
