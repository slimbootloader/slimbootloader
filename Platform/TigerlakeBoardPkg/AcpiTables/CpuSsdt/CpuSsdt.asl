/** @file
  Intel Processor SSDT ACPI Code.

  Copyright (c) 1999 - 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include "CpuPowerMgmt.h"

DefinitionBlock (
  "CPUSSDT.aml",
  "SSDT",
  0x02,
  "CpuRef",
  "CpuSsdt",
  0x3000
  )
{
  External(\_SB.OSCP, IntObj)
  External(\_SB.PR00, DeviceObj)
  External(\_SB.PR01, DeviceObj)
  External(\_SB.PR02, DeviceObj)
  External(\_SB.PR03, DeviceObj)
  External(\_SB.PR04, DeviceObj)
  External(\_SB.PR05, DeviceObj)
  External(\_SB.PR06, DeviceObj)
  External(\_SB.PR07, DeviceObj)
  External(\_SB.PR08, DeviceObj)
  External(\_SB.PR09, DeviceObj)
  External(\_SB.PR10, DeviceObj)
  External(\_SB.PR11, DeviceObj)
  External(\_SB.PR12, DeviceObj)
  External(\_SB.PR13, DeviceObj)
  External(\_SB.PR14, DeviceObj)
  External(\_SB.PR15, DeviceObj)
  External(\_SB.PR16, DeviceObj)
  External(\_SB.PR17, DeviceObj)
  External(\_SB.PR18, DeviceObj)
  External(\_SB.PR19, DeviceObj)
  External(\_SB.PR20, DeviceObj)
  External(\_SB.PR21, DeviceObj)
  External(\_SB.PR22, DeviceObj)
  External(\_SB.PR23, DeviceObj)
  External(OSYS)

Scope(\)
{

  // Package of pointers to SSDT's
  //
  // First column is SSDT name, used for debug only.
  // (First column must be EXACTLY eight characters.)
  // Second column is physical address.
  // Third column is table length.
  //
  // IF modifying this file, see warnings listed in ppminit.asm.
  //
  Name(SSDT,Package()
  {
    "CPU0IST ", 0x80000000, 0x80000000,
    "APIST   ", 0x80000000, 0x80000000,
    "CPU0CST ", 0x80000000, 0x80000000,
    "APCST   ", 0x80000000, 0x80000000,
    "CPU0HWP ", 0x80000000, 0x80000000,
    "APHWP   ", 0x80000000, 0x80000000,
    "HWPLVT  ", 0x80000000, 0x80000000,
    "CPU0PSD ", 0x80000000, 0x80000000,
    "APPSD   ", 0x80000000, 0x80000000
  })

  Name(\PF00,0x80000000)  // PR00 _PDC Flags.
  Name(\PF01,0x80000000)  // PR01 _PDC Flags.
  Name(\PF02,0x80000000)  // PR02 _PDC Flags.
  Name(\PF03,0x80000000)  // PR03 _PDC Flags.
  Name(\PF04,0x80000000)  // PR04 _PDC Flags.
  Name(\PF05,0x80000000)  // PR05 _PDC Flags.
  Name(\PF06,0x80000000)  // PR06 _PDC Flags.
  Name(\PF07,0x80000000)  // PR07 _PDC Flags.
  Name(\PF08,0x80000000)  // PR08 _PDC Flags.
  Name(\PF09,0x80000000)  // PR09 _PDC Flags.
  Name(\PF10,0x80000000)  // PR10 _PDC Flags.
  Name(\PF11,0x80000000)  // PR11 _PDC Flags.
  Name(\PF12,0x80000000)  // PR12 _PDC Flags.
  Name(\PF13,0x80000000)  // PR13 _PDC Flags.
  Name(\PF14,0x80000000)  // PR14 _PDC Flags.
  Name(\PF15,0x80000000)  // PR15 _PDC Flags.
  Name(\PF16,0x80000000)  // PR16 _PDC Flags.
  Name(\PF17,0x80000000)  // PR17 _PDC Flags.
  Name(\PF18,0x80000000)  // PR18 _PDC Flags.
  Name(\PF19,0x80000000)  // PR19 _PDC Flags.
  Name(\PF20,0x80000000)  // PR20 _PDC Flags.
  Name(\PF21,0x80000000)  // PR21 _PDC Flags.
  Name(\PF22,0x80000000)  // PR22 _PDC Flags.
  Name(\PF23,0x80000000)  // PR23 _PDC Flags.

  Name(\SDTL,0x00)        // Loaded SSDT Flags.
  // SDTL[1]   = Cpu0Cst SSDT Loaded
  // SDTL[3]   = Cpu0Ist SSDT Loaded
  // SDTL[4]   = AP IST SSDT Loaded
  // SDTL[5]   = AP CST SSDT Loaded
  // SDTL[6]   = Cpu0Hwp SSDT Loaded
  // SDTL[7]   = Ap0Hwp SSDT Loaded
  // SDTL[8]   = Cpu0Psd SSDT Loaded
  // SDTL[9]   = ApPsd SSDT Loaded
}

Scope(\_SB)
{
  Name(CTPC, 0 )           // Value for _TPC
  Include ("CpuNvs.asl")
}

Scope (\_SB.PR00)
{
  //
  // Define handles for opregions (used by load.)
  //
  Name(HI0,0)    // Handle to CPU0IST
  Name(HC0,0)    // Handle to CPU0CST
  Name(HW0,0)    // Handle to CPU0HWP
  Name(HW2,0)    // Handle to HWPLVT
  Name(HP0,0)    // Handle to CPU0PSD

  Method(_PDC,1)
  {
    //
    // Check and extract the _PDC information.
    //
    Store(CPDC(Arg0), Local0)
    //
    // Save the capability information and load tables as needed.
    //
    GCAP(Local0)
  }

  Method(_OSC, 4)
  {
    //
    // Check and extract the _OSC information.
    //
    Store(COSC(Arg0, Arg1, Arg2, Arg3), Local0)
    //
    // Save the capability information and load tables as needed.
    //
    GCAP(Local0)
    //
    // Return status.
    //

    Return (Local0)
  }

  //
  // Implement a generic Method to check _PDC information which may be called
  // by any of the processor scopes.  (The use of _PDC is deprecated in ACPI 3.
  // in favor of _OSC. However, for backwards compatibility, _PDC may be
  // implemented using _OSC as follows:)
  //
  Method(CPDC,1)
  {
    CreateDwordField (Arg0, 0, REVS)
    CreateDwordField (Arg0, 4, SIZE)

    //
    // Local0 = Number of bytes for Arg0
    //
    Store (SizeOf (Arg0), Local0)

    //
    // Local1 = Number of Capabilities bytes in Arg0
    //
    Store (Subtract (Local0, 8), Local1)

    //
    // TEMP = Temporary field holding Capability DWORDs
    //
    CreateField (Arg0, 64, Multiply (Local1, 8), TEMP)

    //
    // Create the Status (Local3) buffer with the first DWORD = 0
    // This is required as per ACPI 3.0 Spec which says the
    // first DWORD is used to return errors defined by _OSC.
    //
    Store(Buffer () {0x00, 0x00, 0x00, 0x00}, Local3)

    //
    // Concatenate the _PDC capabilities bytes to the STS0 Buffer
    // and store them in a local variable for calling OSC
    //
    Concatenate (Local3, TEMP, Local2)

    Return(COSC (ToUUID("4077A616-290C-47BE-9EBD-D87058713953"), REVS, SIZE, Local2))
  }

  //
  // Implement a generic Method to check _OSC information which may be called
  // by any of the processor scopes.
  //
  Method(COSC, 4)
  {
    //
    // Point to Status DWORD in the Arg3 buffer (STATUS)
    //
    CreateDWordField(Arg3, 0, STS0)

    //
    // _OSC needs to validate the UUID and Revision.
    //
    // IF Unrecognized UUID
    //  Return Unrecognized UUID _OSC Failure
    // IF Unsupported Revision
    //  Return Unsupported Revision _OSC Failure
    //
    //    STS0[0] = Reserved
    //    STS0[1] = _OSC Failure
    //    STS0[2] = Unrecognized UUID
    //    STS0[3] = Unsupported Revision
    //    STS0[4] = Capabilities masked
    //
    // Note:  The comparison method used is necessary due to
    // limitations of certain OSes which cannot perform direct
    // buffer comparisons.
    //
    // Create a set of "Input" UUID fields.
    //
    CreateDwordField(Arg0, 0x0, IID0)
    CreateDwordField(Arg0, 0x4, IID1)
    CreateDwordField(Arg0, 0x8, IID2)
    CreateDwordField(Arg0, 0xC, IID3)
    //
    // Create a set of "Expected" UUID fields.
    //
    Store (ToUUID("4077A616-290C-47BE-9EBD-D87058713953"), Local0)
    CreateDwordField(Local0, 0x0, EID0)
    CreateDwordField(Local0, 0x4, EID1)
    CreateDwordField(Local0, 0x8, EID2)
    CreateDwordField(Local0, 0xC, EID3)
    //
    // Verify the input UUID matches the expected UUID.
    //
    If(LNot(LAnd(LAnd(LEqual(IID0, EID0),LEqual(IID1, EID1)),LAnd(LEqual(IID2, EID2),LEqual(IID3, EID3)))))
    {
      //
      // Return Unrecognized UUID _OSC Failure
      //
      Store (0x6, STS0)
      Return (Arg3)
    }

    If(LNot(LEqual(Arg1,1)))
    {
      //
      // Return Unsupported Revision _OSC Failure
      //
      Store (0xA, STS0)
      Return (Arg3)
    }

    Return (Arg3)
  }

  //
  // Get the capability information and load appropriate tables as needed.
  //
  Method (GCAP, 1, Serialized)
  {
    //
    // Point to Status DWORD in the Arg0 buffer (STATUS)
    //
    CreateDWordField(Arg0, 0, STS0)
    //
    // Point to Caps DWORDs of the Arg0 buffer (CAPABILITIES)
    //
    CreateDwordField(Arg0, 4, CAP0)
    //
    // If the UUID was unrecognized or the _OSC revision was unsupported,
    // return without updating capabilities.
    //
    If(LOr(LEqual(STS0,0x6),LEqual(STS0,0xA)))
    {
      Return()
    }
    //
    // Check if this is a query (BIT0 of Status = 1).
    // If so, mask off the bits we support and return.
    //
    If (And(STS0, 1))
    {
      And(CAP0, 0xBFF, CAP0)
      Return()
    }
    //
    // Store result of PDC. (We clear out the MSB, which was just
    // used as a placeholder for the compiler; and then "OR" the
    // value in case we get multiple calls, each of which only
    // reports partial support.)
    //
    Or(And(PF00, 0x7FFFFFFF), CAP0, PF00)

    //
    // Check IF the CST SSDTs should be loaded.
    //   CFGD[5:1] = C7, C6, C3, C1E, C1 Capable/Enabled
    If (And(CFGD, PPM_C_STATES))
    {
      //
      // Load the CST SSDTs if:
      //   (1) CMP capable/enabled
      //   (2) Driver supports multi-processor configurations
      //   (3) CPU0 CST ISDT is not already loaded
      //
      //   CFGD[9] = Two or more cores enabled
      //   PCxx[3]  = OS supports C1 and P-states in MP systems
      //   PCxx[4]  = OS supports ind. C2/C3 in MP systems
      //   SDTL[1]  = CPU0 CST SSDT Loaded
      //
      If (LAnd(LAnd(And(CFGD, PPM_CMP),And(PF00,0x0018)),LNot(And(SDTL,0x02))))
      {
        //
        // Flag the CST SSDT as loaded for PR00
        //
        Or(SDTL, 0x02, SDTL)
        OperationRegion(CST0,SystemMemory,DeRefOf(Index(SSDT,7)),DeRefOf(Index(SSDT,8)))
        // Load(CST0, HC0)  // Dynamically load the CPU0CST SSDT
      }
    }

    //
    // Attempt to dynamically load the IST SSDTs if:
    //   (1) Driver supports P-States in MP configurations
    //   (2) Driver supports direct HW P-State control
    //
    //   PCxx[3]  = OS supports C1 and P-states in MP systems
    //   PCxx[0]  = OS supports direct access of the perf MSR
    //
    If(LEqual(And(PF00, 0x0009), 0x0009))
    {
      //
      // Dynamically load the IST SSDTs if:
      //   (1) If EIST capable and enabled
      //   (2) CPU0 IST SSDT is not already loaded
      //
      //   CFGD[0] = EIST Capable/Enabled
      //   SDTL[3] = CPU0 IST SSDT Loaded
      //
      If (LAnd(And(CFGD,PPM_EIST),LNot(And(SDTL,0x08))))
      {
        //
        // Flag the IST SSDT as loaded for PR00
        //
        Or(SDTL, 0x08, SDTL)
        OperationRegion(IST0,SystemMemory,DeRefOf(Index(SSDT,1)),DeRefOf(Index(SSDT,2)))
        // Load(IST0, HI0)  // Dynamically load the CPU1IST SSDT
      }
    }

    //
    // Dynamically load the PSD SSDTs if:
    //   (1) If EIST or HWP Enabled
    //   (2) CPU0 PSD SSDT is not already loaded
    //
    //   CFGD[0] = EIST Capable/Enabled
    //   CFGD[22] = HWP capable & enabled
    //   SDTL[8] = CPU0 PSD SSDT Loaded
    //
    If (LAnd(LOr(And(CFGD, PPM_EIST), And(CFGD, PPM_HWP)), LNot(And(SDTL, 0x100))))
    {
      //
      // Flag the PSD SSDT as loaded for PR00
      //
      Or(SDTL, 0x100, SDTL)
      OperationRegion(PSD0,SystemMemory,DeRefOf(Index(SSDT,22)),DeRefOf(Index(SSDT,23)))
      // Load(PSD0, HP0)  // Dynamically load the CPU0PSD SSDT
    }

    // Dynamically load the Cpu0Hwp & HwpLvt  SSDT if:
    //   (1) If HWP capable and enabled
    //   (2) SSDT is not already loaded
    //
    // CFGD[22]  = HWP capable & enabled
    // CFGD[23]  = HWP LVT interrupt (SCI) enabled
    // SDTL[6]   = Cpu0Hwp SSDT Loaded
    // SB.OSC[5] = OS supports HWP v1
    // SB.OSC[6] = OS supports HWP v2
    //
    If (LGreaterEqual(OSYS, 2015)) {
      If (LAnd(And(CFGD, PPM_HWP),LNot(And(SDTL,0x40)))) {
        If (And(\_SB.OSCP,0x0040)) {
          //
          // Flag the Cpu0Hwp SSDT as loaded for PR00. We only support HWP v2.
          //
          Or(SDTL, 0x40, SDTL)
          OperationRegion(HWP0,SystemMemory,DeRefOf(Index(SSDT,13)),DeRefOf(Index(SSDT,14)))
          // Load(HWP0, HW0)  // Dynamically load the Cpu0Hwp SSDT
        }
        //
        // Update HWP version
        //
        // If CAP0[5] = 1, CAP0[6] = 0 (HWP V1)
        // If CAP0[5] = 1, CAP0[6] = 1 (HWP V2)
        //
        If (And(\_SB.OSCP,0x0020)) {
          If (LNot(And(\_SB.OSCP,0x0040))) {
            //
            // HWP is not supported on Win 8.1, set HWPV = 0
            //
            Store(0x0, HWPV)
          }
        }
        If (And(\_SB.OSCP,0x0040)) {
          Store(0x2, HWPV)
        }
      }
    }

    Return ()

  } // end of Method (GCAP, 1)

} // end of Scope (\_SB.PR00)

Scope(\_SB.PR01)
{
  //
  // Define handles for opregions (used by load.)
  //
  Name(HI1,0)    // Handle to APIST
  Name(HC1,0)    // Handle to APCST
  Name(HW1,0)    // Handle to APHWP
  Name(HP1,0)    // Handle to APPSD

  Method(_PDC,1)
  {
    //
    // Refer to \_SB.PR00._PDC for description.
    //
    Store(\_SB.PR00.CPDC(Arg0), Local0)
    GCAP(Local0)
  }

  Method(_OSC, 4)
  {
    //
    // Refer to \_SB.PR00._OSC for description.
    //
    Store(\_SB.PR00.COSC(Arg0, Arg1, Arg2, Arg3), Local0)
    GCAP(Local0)
    Return (Local0)
  }

  //
  // Get the capability information and load appropriate tables as needed.
  //
  Method(GCAP, 1)
  {
    //
    // Point to Status DWORD in the Arg0 buffer (STATUS)
    //
    CreateDWordField(Arg0, 0, ST01)
    //
    // Point to Caps DWORDs of the Arg0 buffer (CAPABILITIES)
    //
    CreateDwordField(Arg0, 4, CP01)
    //
    // If the UUID was unrecognized or the _OSC revision was unsupported,
    // return without updating capabilities.
    //
    If(LOr(LEqual(ST01,0x6),LEqual(ST01,0xA)))
    {
      Return()
    }

    //
    // Check if this is a query (BIT0 of Status = 1).
    // If so, mask off the bits we support and return.
    //
    if (And(ST01, 1))
    {
      And(CP01, 0xBFF, CP01)
      Return()
    }

    //
    // Store result of PDC. (We clear out the MSB, which was just
    // used as a placeholder for the compiler; and then "OR" the
    // value in case we get multiple calls, each of which only
    // reports partial support.)
    //
    Or(And(PF01, 0x7FFFFFFF), CP01, PF01)
    //
    // Attempt to dynamically load the IST SSDTs if:
    //   (1) Driver supports P-States in MP configurations
    //   (2) Driver supports direct HW P-State control
    //
    //   PFxx[3]  = OS supports C1 and P-states in MP systems
    //   PFxx[0]  = OS supports direct access of the perf MSR
    //
    If(LEqual(And(PF01, 0x0009), 0x0009))
    {
      APPT()
    }

    //
    // Load Ap0Hwp SSDT
    // SB.OSC[5]   = OS supports CPPC from OSPM
    //
    if(And(\_SB.OSCP,0x0020)) {
      HWPT()
    }

    //
    // Load Ap0Psd SSDT
    //   (1) Driver supports P-States in MP configurations
    //   (2) Driver supports direct HW P-State control
    //   (3) SB.OSC[5]   = OS supports CPPC from OSPM
    //
    //   PFxx[3]  = OS supports C1 and P-states in MP systems
    //   PFxx[0]  = OS supports direct access of the perf MSR
    //
    If(Lor(LEqual(And(PF01, 0x0009), 0x0009), And(\_SB.OSCP,0x0020))) {
      APPS()
    }

    //
    // Load the CST SSDTs if:
    //   (1) Driver supports multi-processor configurations
    //
    //   PFxx[3]  = OS supports C1 and P-states in MP systems
    //   PFxx[4]  = OS supports ind. C2/C3 in MP systems
    //
    If(And(PF01,0x0018))
    {
      APCT()
    }

    Store (PF01, PF00)

    Return()
  }

  //
  // Dynamically load the CST SSDTs if:
  //   (1) C-States are enabled
  //   (2) SSDT is not already loaded
  //
  //   CFGD[5:1] = Basic C-States supported (C1, C1E, C3, C6, C7)
  //   SDTL[5]   = AP CST SSDT Loaded
  //
  Method(APCT, 0, Serialized)
  {
    If(LAnd(And(CFGD,PPM_C_STATES),LNot(And(SDTL,0x20))))
    {
      //
      // Flag the CST SSDT as loaded for the AP's
      //
      Or(SDTL, 0x20, SDTL)
      //
      // Dynamically load the APCST SSDT
      //
      OperationRegion(CST1,SystemMemory,DeRefOf(Index(SSDT,10)),DeRefOf(Index(SSDT,11)))
      //Load(CST1, HC1)
    }
  }

  //
  // Dynamically load the IST SSDTs if:
  //   (1) If EIST capable and enabled
  //   (2) SSDT is not already loaded
  //
  //   CFGD[0] = EIST Capable/Enabled
  //   SDTL[4] = AP IST SSDT Loaded
  //
  Method(APPT, 0, Serialized)
  {
    If(LAnd(And(CFGD,PPM_EIST),LNot(And(SDTL,0x10))))
    {
      //
      // Flag the IST SSDT as loaded for PR00
      //
      Or(SDTL, 0x10, SDTL)

      OperationRegion(IST1,SystemMemory,DeRefOf(Index(SSDT,4)),DeRefOf(Index(SSDT,5)))
      //Load(IST1, HI1)  // Dynamically load the CPU1IST SSDT
    }
  }

  //
  // Dynamically load the PSD SSDTs if:
  //   (1) If EIST or HWP Enabled
  //   (2) PSD SSDT is not already loaded
  //
  //   CFGD[0] = EIST Capable/Enabled
  //   CFGD[22] = HWP capable & enabled
  //   SDTL[9] = AP PSD SSDT Loaded
  //
  Method(APPS, 0, Serialized)
  {
    If (LAnd(LOr(And(CFGD, PPM_EIST), And(CFGD, PPM_HWP)), LNot(And(SDTL, 0x200))))
    {
      //
      // Flag the PSD SSDT as loaded for PR00
      //
      Or(SDTL, 0x200, SDTL)
      OperationRegion(PSD1,SystemMemory,DeRefOf(Index(SSDT,25)),DeRefOf(Index(SSDT,26)))
      //Load(PSD1, HP1)  // Dynamically load the APPSD SSDT
    }
  }

  //
  // Dynamically load the Ap0Hwp SSDT if:
  //   (1) If HWP capable and enabled
  //   (2) SSDT is not already loaded
  //
  // CFGD[22]  = HWP capable & enabled
  // SDTL[7]   = Ap0Hwp SSDT Loaded
  //
  Method(HWPT, 0, Serialized)
  {
    If ( LGreaterEqual(OSYS, 2015) ) {
      If(LAnd(And(CFGD, PPM_HWP),LNot(And(SDTL,0x80)))) {
        //
        // Flag the Ap0Hwp SSDT as loaded for PR00
        //
        Or(SDTL, 0x80, SDTL)
        OperationRegion(HWP1,SystemMemory,DeRefOf(Index(SSDT,16)),DeRefOf(Index(SSDT,17)))
        //Load(HWP1, HW1)  // Dynamically load the Ap0Hwp SSDT
      }
    }
  }
}  // End PR01


Scope(\_SB.PR02)
{
  Method(_PDC,1)
  {
    //
    // Call the _PDC for CPU1.
    //
    Store(\_SB.PR00.CPDC(Arg0), Local0)
    GCAP(Local0)
  }

  Method(_OSC, 4)
  {
    //
    // Call the _OSC for CPU1.
    //
    Store(\_SB.PR00.COSC(Arg0, Arg1, Arg2, Arg3), Local0)
    GCAP(Local0)
    Return (Local0)
  }

  Method(GCAP,1)
  {
    // Point to Status DWORD in the Arg0 buffer (STATUS)
    CreateDWordField(Arg0, 0, ST02)

    // Point to Caps DWORDs of the Arg0 buffer (CAPABILITIES)
    CreateDwordField(Arg0, 4, CP02)

    //
    // If the UUID was unrecognized or the _OSC revision was unsupported,
    // return without updating capabilities.
    //
    If(LOr(LEqual(ST02,0x6),LEqual(ST02,0xA)))
    {
      Return()
    }

    //
    // Check if this is a query (BIT0 of Status = 1).
    // If so, mask off the bits we support and return.
    //
    if (And(ST02, 1))
    {
      And(CP02, 0xBFF, CP02)
      Return()
    }

    //
    // Store result of PDC. (We clear out the MSB, which was just
    // used as a placeholder for the compiler; and then "OR" the
    // value in case we get multiple calls, each of which only
    // reports partial support.)
    //
    Or(And(PF02, 0x7FFFFFFF), CP02, PF02)
    //
    // Attempt to dynamically load the IST SSDTs if:
    //   (1) Driver supports P-States in MP configurations
    //   (2) Driver supports direct HW P-State control
    //
    //   PFxx[3]  = OS supports C1 and P-states in MP systems
    //   PFxx[0]  = OS supports direct access of the perf MSR
    //
    If(LEqual(And(PF02, 0x0009), 0x0009))
    {
      \_SB.PR01.APPT()
    }
    // Load Ap0Hwp SSDT
    // SB.OSC[5]   = OS supports CPPC from OSPM
    //
    if(And(\_SB.OSCP,0x0020)) {
      \_SB.PR01.HWPT()
    }

    //
    // Dynamically load the PSD SSDTs if:
    //   (1) If EIST or HWP Enabled
    //   (2) PSD SSDT is not already loaded
    //
    //   CFGD[0] = EIST Capable/Enabled
    //   CFGD[22] = HWP capable & enabled
    //   SDTL[9] = AP PSD SSDT Loaded
    //
    If(Lor(LEqual(And(PF02, 0x0009), 0x0009), And(\_SB.OSCP,0x0020)))
    {
      \_SB.PR01.APPS()
    }

    //
    // Load the CST SSDTs if:
    //   (1) Driver supports multi-processor configurations
    //
    //   PFxx[3]  = OS supports C1 and P-states in MP systems
    //   PFxx[4]  = OS supports ind. C2/C3 in MP systems
    //
    If(And(PF02,0x0018))
    {
      \_SB.PR01.APCT()
    }

    Store (PF02, PF00)
    Return()
  }
}  // End PR02

Scope(\_SB.PR03)
{
  Method(_PDC,1)
  {
    //
    // Call the _PDC for CPU1.
    //
    Store(\_SB.PR00.CPDC(Arg0), Local0)
    GCAP(Local0)
  }

  Method(_OSC, 4)
  {
    //
    // Call the _OSC for CPU1.
    //
    Store(\_SB.PR00.COSC(Arg0, Arg1, Arg2, Arg3), Local0)
    GCAP(Local0)
    Return (Local0)
  }

  Method(GCAP,1)
  {
    // Point to Status DWORD in the Arg0 buffer (STATUS)
    CreateDWordField(Arg0, 0, ST03)

    // Point to Caps DWORDs of the Arg0 buffer (CAPABILITIES)
    CreateDwordField(Arg0, 4, CP03)

    //
    // If the UUID was unrecognized or the _OSC revision was unsupported,
    // return without updating capabilities.
    //
    If(LOr(LEqual(ST03,0x6),LEqual(ST03,0xA)))
    {
      Return()
    }

    //
    // Check if this is a query (BIT0 of Status = 1).
    // If so, mask off the bits we support and return.
    //
    if (And(ST03, 1))
    {
      And(CP03, 0xBFF, CP03)
      Return()
    }

    //
    // Store result of PDC. (We clear out the MSB, which was just
    // used as a placeholder for the compiler; and then "OR" the
    // value in case we get multiple calls, each of which only
    // reports partial support.)
    //
    Or(And(PF03, 0x7FFFFFFF), CP03, PF03)
    //
    // Attempt to dynamically load the IST SSDTs if:
    //   (1) Driver supports P-States in MP configurations
    //   (2) Driver supports direct HW P-State control
    //
    //   PFxx[3]  = OS supports C1 and P-states in MP systems
    //   PFxx[0]  = OS supports direct access of the perf MSR
    //
    If(LEqual(And(PF03, 0x0009), 0x0009))
    {
      \_SB.PR01.APPT()
    }
    // Load Ap0Hwp SSDT
    // SB.OSC[5]   = OS supports CPPC from OSPM
    //
    if(And(\_SB.OSCP,0x0020)) {
      \_SB.PR01.HWPT()
    }

    //
    // Dynamically load the PSD SSDTs if:
    //   (1) If EIST or HWP Enabled
    //   (2) PSD SSDT is not already loaded
    //
    //   CFGD[0] = EIST Capable/Enabled
    //   CFGD[22] = HWP capable & enabled
    //   SDTL[9] = AP PSD SSDT Loaded
    //
    If(Lor(LEqual(And(PF03, 0x0009), 0x0009), And(\_SB.OSCP,0x0020)))
    {
      \_SB.PR01.APPS()
    }

    //
    // Load the CST SSDTs if:
    //   (1) Driver supports multi-processor configurations
    //
    //   PFxx[3]  = OS supports C1 and P-states in MP systems
    //   PFxx[4]  = OS supports ind. C2/C3 in MP systems
    //
    If(And(PF03,0x0018))
    {
      \_SB.PR01.APCT()
    }

    Store (PF03, PF00)
    Return()
  }
} // End PR03

Scope(\_SB.PR04)
{
  Method(_PDC,1)
  {
    //
    // Call the _PDC for CPU1.
    //
    Store(\_SB.PR00.CPDC(Arg0), Local0)
    GCAP(Local0)
  }

  Method(_OSC, 4)
  {
    //
    // Call the _OSC for CPU1.
    //
    Store(\_SB.PR00.COSC(Arg0, Arg1, Arg2, Arg3), Local0)
    GCAP(Local0)
    Return (Local0)
  }

  Method(GCAP,1)
  {
    // Point to Status DWORD in the Arg0 buffer (STATUS)
    CreateDWordField(Arg0, 0, ST04)

    // Point to Caps DWORDs of the Arg0 buffer (CAPABILITIES)
    CreateDwordField(Arg0, 4, CP04)

    //
    // If the UUID was unrecognized or the _OSC revision was unsupported,
    // return without updating capabilities.
    //
    If(LOr(LEqual(ST04,0x6),LEqual(ST04,0xA)))
    {
      Return()
    }

    //
    // Check if this is a query (BIT0 of Status = 1).
    // If so, mask off the bits we support and return.
    //
    if (And(ST04, 1))
    {
      And(CP04, 0xBFF, CP04)
      Return()
    }

    //
    // Store result of PDC. (We clear out the MSB, which was just
    // used as a placeholder for the compiler; and then "OR" the
    // value in case we get multiple calls, each of which only
    // reports partial support.)
    //
    Or(And(PF04, 0x7FFFFFFF), CP04, PF04)
    //
    // Attempt to dynamically load the IST SSDTs if:
    //   (1) Driver supports P-States in MP configurations
    //   (2) Driver supports direct HW P-State control
    //
    //   PFxx[3]  = OS supports C1 and P-states in MP systems
    //   PFxx[0]  = OS supports direct access of the perf MSR
    //
    If(LEqual(And(PF04, 0x0009), 0x0009))
    {
      \_SB.PR01.APPT()
    }
    // Load Ap0Hwp SSDT
    // SB.OSC[5]   = OS supports CPPC from OSPM
    //
    if(And(\_SB.OSCP,0x0020)) {
      \_SB.PR01.HWPT()
    }

    //
    // Dynamically load the PSD SSDTs if:
    //   (1) If EIST or HWP Enabled
    //   (2) PSD SSDT is not already loaded
    //
    //   CFGD[0] = EIST Capable/Enabled
    //   CFGD[22] = HWP capable & enabled
    //   SDTL[9] = AP PSD SSDT Loaded
    //
    If(Lor(LEqual(And(PF04, 0x0009), 0x0009), And(\_SB.OSCP,0x0020)))
    {
      \_SB.PR01.APPS()
    }

    //
    // Load the CST SSDTs if:
    //   (1) Driver supports multi-processor configurations
    //
    //   PFxx[3]  = OS supports C1 and P-states in MP systems
    //   PFxx[4]  = OS supports ind. C2/C3 in MP systems
    //
    If(And(PF04,0x0018))
    {
      \_SB.PR01.APCT()
    }

    Store (PF04, PF00)
    Return()
  }
} // End PR04

Scope(\_SB.PR05)
{
  Method(_PDC,1)
  {
    //
    // Call the _PDC for CPU1.
    //
    Store(\_SB.PR00.CPDC(Arg0), Local0)
    GCAP(Local0)
  }

  Method(_OSC, 4)
  {
    //
    // Call the _OSC for CPU1.
    //
    Store(\_SB.PR00.COSC(Arg0, Arg1, Arg2, Arg3), Local0)
    GCAP(Local0)
    Return (Local0)
  }

  Method(GCAP,1)
  {
    // Point to Status DWORD in the Arg0 buffer (STATUS)
    CreateDWordField(Arg0, 0, ST05)

    // Point to Caps DWORDs of the Arg0 buffer (CAPABILITIES)
    CreateDwordField(Arg0, 4, CP05)

    //
    // If the UUID was unrecognized or the _OSC revision was unsupported,
    // return without updating capabilities.
    //
    If(LOr(LEqual(ST05,0x6),LEqual(ST05,0xA)))
    {
      Return()
    }

    //
    // Check if this is a query (BIT0 of Status = 1).
    // If so, mask off the bits we support and return.
    //
    if (And(ST05, 1))
    {
      And(CP05, 0xBFF, CP05)
      Return()
    }

    //
    // Store result of PDC. (We clear out the MSB, which was just
    // used as a placeholder for the compiler; and then "OR" the
    // value in case we get multiple calls, each of which only
    // reports partial support.)
    //
    Or(And(PF05, 0x7FFFFFFF), CP05, PF05)
    //
    // Attempt to dynamically load the IST SSDTs if:
    //   (1) Driver supports P-States in MP configurations
    //   (2) Driver supports direct HW P-State control
    //
    //   PFxx[3]  = OS supports C1 and P-states in MP systems
    //   PFxx[0]  = OS supports direct access of the perf MSR
    //
    If(LEqual(And(PF05, 0x0009), 0x0009))
    {
      \_SB.PR01.APPT()
    }
    // Load Ap0Hwp SSDT
    // SB.OSC[5]   = OS supports CPPC from OSPM
    //
    if(And(\_SB.OSCP,0x0020)) {
      \_SB.PR01.HWPT()
    }

    //
    // Dynamically load the PSD SSDTs if:
    //   (1) If EIST or HWP Enabled
    //   (2) PSD SSDT is not already loaded
    //
    //   CFGD[0] = EIST Capable/Enabled
    //   CFGD[22] = HWP capable & enabled
    //   SDTL[9] = AP PSD SSDT Loaded
    //
    If(Lor(LEqual(And(PF05, 0x0009), 0x0009), And(\_SB.OSCP,0x0020)))
    {
      \_SB.PR01.APPS()
    }
    //
    // Load the CST SSDTs if:
    //   (1) Driver supports multi-processor configurations
    //
    //   PFxx[3]  = OS supports C1 and P-states in MP systems
    //   PFxx[4]  = OS supports ind. C2/C3 in MP systems
    //
    If(And(PF05,0x0018))
    {
      \_SB.PR01.APCT()
    }

    Store (PF05, PF00)
    Return()
  }
} // End PR05

Scope(\_SB.PR06)
{
  Method(_PDC,1)
  {
    //
    // Call the _PDC for CPU1.
    //
    Store(\_SB.PR00.CPDC(Arg0), Local0)
    GCAP(Local0)
  }

  Method(_OSC, 4)
  {
    //
    // Call the _OSC for CPU1.
    //
    Store(\_SB.PR00.COSC(Arg0, Arg1, Arg2, Arg3), Local0)
    GCAP(Local0)
    Return (Local0)
  }

  Method(GCAP,1)
  {
    // Point to Status DWORD in the Arg0 buffer (STATUS)
    CreateDWordField(Arg0, 0, ST06)

    // Point to Caps DWORDs of the Arg0 buffer (CAPABILITIES)
    CreateDwordField(Arg0, 4, CP06)

    //
    // If the UUID was unrecognized or the _OSC revision was unsupported,
    // return without updating capabilities.
    //
    If(LOr(LEqual(ST06,0x6),LEqual(ST06,0xA)))
    {
      Return()
    }

    //
    // Check if this is a query (BIT0 of Status = 1).
    // If so, mask off the bits we support and return.
    //
    if (And(ST06, 1))
    {
      And(CP06, 0xBFF, CP06)
      Return()
    }

    //
    // Store result of PDC. (We clear out the MSB, which was just
    // used as a placeholder for the compiler; and then "OR" the
    // value in case we get multiple calls, each of which only
    // reports partial support.)
    //
    Or(And(PF06, 0x7FFFFFFF), CP06, PF06)
    //
    // Attempt to dynamically load the IST SSDTs if:
    //   (1) Driver supports P-States in MP configurations
    //   (2) Driver supports direct HW P-State control
    //
    //   PFxx[3]  = OS supports C1 and P-states in MP systems
    //   PFxx[0]  = OS supports direct access of the perf MSR
    //
    If(LEqual(And(PF06, 0x0009), 0x0009))
    {
      \_SB.PR01.APPT()
    }
    // Load Ap0Hwp SSDT
    // SB.OSC[5]   = OS supports CPPC from OSPM
    //
    if(And(\_SB.OSCP,0x0020)) {
      \_SB.PR01.HWPT()
    }
    //
    // Dynamically load the PSD SSDTs if:
    //   (1) If EIST or HWP Enabled
    //   (2) PSD SSDT is not already loaded
    //
    //   CFGD[0] = EIST Capable/Enabled
    //   CFGD[22] = HWP capable & enabled
    //   SDTL[9] = AP PSD SSDT Loaded
    //
    If(Lor(LEqual(And(PF06, 0x0009), 0x0009), And(\_SB.OSCP,0x0020)))
    {
      \_SB.PR01.APPS()
    }
    //
    // Load the CST SSDTs if:
    //   (1) Driver supports multi-processor configurations
    //
    //   PFxx[3]  = OS supports C1 and P-states in MP systems
    //   PFxx[4]  = OS supports ind. C2/C3 in MP systems
    //
    If(And(PF06,0x0018))
    {
      \_SB.PR01.APCT()
    }

    Store (PF06, PF00)
    Return()
  }
} // End PR06

Scope(\_SB.PR07)
{
  Method(_PDC,1)
  {
    //
    // Call the _PDC for CPU1.
    //
    Store(\_SB.PR00.CPDC(Arg0), Local0)
    GCAP(Local0)
  }

  Method(_OSC, 4)
  {
    //
    // Call the _OSC for CPU1.
    //
    Store(\_SB.PR00.COSC(Arg0, Arg1, Arg2, Arg3), Local0)
    GCAP(Local0)
    Return (Local0)
  }

  Method(GCAP,1)
  {
    // Point to Status DWORD in the Arg0 buffer (STATUS)
    CreateDWordField(Arg0, 0, ST07)

    // Point to Caps DWORDs of the Arg0 buffer (CAPABILITIES)
    CreateDwordField(Arg0, 4, CP07)

    //
    // If the UUID was unrecognized or the _OSC revision was unsupported,
    // return without updating capabilities.
    //
    If(LOr(LEqual(ST07,0x6),LEqual(ST07,0xA)))
    {
      Return()
    }

    //
    // Check if this is a query (BIT0 of Status = 1).
    // If so, mask off the bits we support and return.
    //
    if (And(ST07, 1))
    {
      And(CP07, 0xBFF, CP07)
      Return()
    }

    //
    // Store result of PDC. (We clear out the MSB, which was just
    // used as a placeholder for the compiler; and then "OR" the
    // value in case we get multiple calls, each of which only
    // reports partial support.)
    //
    Or(And(PF07, 0x7FFFFFFF), CP07, PF07)
    //
    // Attempt to dynamically load the IST SSDTs if:
    //   (1) Driver supports P-States in MP configurations
    //   (2) Driver supports direct HW P-State control
    //
    //   PFxx[3]  = OS supports C1 and P-states in MP systems
    //   PFxx[0]  = OS supports direct access of the perf MSR
    //
    If(LEqual(And(PF07, 0x0009), 0x0009))
    {
      \_SB.PR01.APPT()
    }
    // Load Ap0Hwp SSDT
    // SB.OSC[5]   = OS supports CPPC from OSPM
    //
    if(And(\_SB.OSCP,0x0020)) {
      \_SB.PR01.HWPT()
    }
    //
    // Dynamically load the PSD SSDTs if:
    //   (1) If EIST or HWP Enabled
    //   (2) PSD SSDT is not already loaded
    //
    //   CFGD[0] = EIST Capable/Enabled
    //   CFGD[22] = HWP capable & enabled
    //   SDTL[9] = AP PSD SSDT Loaded
    //
    If(Lor(LEqual(And(PF07, 0x0009), 0x0009), And(\_SB.OSCP,0x0020)))
    {
      \_SB.PR01.APPS()
    }
    //
    // Load the CST SSDTs if:
    //   (1) Driver supports multi-processor configurations
    //
    //   PFxx[3]  = OS supports C1 and P-states in MP systems
    //   PFxx[4]  = OS supports ind. C2/C3 in MP systems
    //
    If(And(PF07,0x0018))
    {
      \_SB.PR01.APCT()
    }

    Store (PF07, PF00)
    Return()
  }
} // End PR07

Scope(\_SB.PR08)
{
  Method(_PDC,1)
  {
    //
    // Call the _PDC for CPU1.
    //
    Store(\_SB.PR00.CPDC(Arg0), Local0)
    GCAP(Local0)
  }

  Method(_OSC, 4)
  {
    //
    // Call the _OSC for CPU1.
    //
    Store(\_SB.PR00.COSC(Arg0, Arg1, Arg2, Arg3), Local0)
    GCAP(Local0)
    Return (Local0)
  }

  Method(GCAP,1)
  {
    // Point to Status DWORD in the Arg0 buffer (STATUS)
    CreateDWordField(Arg0, 0, ST08)

    // Point to Caps DWORDs of the Arg0 buffer (CAPABILITIES)
    CreateDwordField(Arg0, 4, CP08)

    //
    // If the UUID was unrecognized or the _OSC revision was unsupported,
    // return without updating capabilities.
    //
    If(LOr(LEqual(ST08,0x6),LEqual(ST08,0xA)))
    {
      Return()
    }

    //
    // Check if this is a query (BIT0 of Status = 1).
    // If so, mask off the bits we support and return.
    //
    if (And(ST08, 1))
    {
      And(CP08, 0xBFF, CP08)
      Return()
    }

    //
    // Store result of PDC. (We clear out the MSB, which was just
    // used as a placeholder for the compiler; and then "OR" the
    // value in case we get multiple calls, each of which only
    // reports partial support.)
    //
    Or(And(PF08, 0x7FFFFFFF), CP08, PF08)
    //
    // Attempt to dynamically load the IST SSDTs if:
    //   (1) Driver supports P-States in MP configurations
    //   (2) Driver supports direct HW P-State control
    //
    //   PFxx[3]  = OS supports C1 and P-states in MP systems
    //   PFxx[0]  = OS supports direct access of the perf MSR
    //
    If(LEqual(And(PF08, 0x0009), 0x0009))
    {
      \_SB.PR01.APPT()
    }
    // Load Ap0Hwp SSDT
    // SB.OSC[5]   = OS supports CPPC from OSPM
    //
    if(And(\_SB.OSCP,0x0020)) {
      \_SB.PR01.HWPT()
    }
    //
    // Dynamically load the PSD SSDTs if:
    //   (1) If EIST or HWP Enabled
    //   (2) PSD SSDT is not already loaded
    //
    //   CFGD[0] = EIST Capable/Enabled
    //   CFGD[22] = HWP capable & enabled
    //   SDTL[9] = AP PSD SSDT Loaded
    //
    If(Lor(LEqual(And(PF08, 0x0009), 0x0009), And(\_SB.OSCP,0x0020)))
    {
      \_SB.PR01.APPS()
    }
    //
    // Load the CST SSDTs if:
    //   (1) Driver supports multi-processor configurations
    //
    //   PFxx[3]  = OS supports C1 and P-states in MP systems
    //   PFxx[4]  = OS supports ind. C2/C3 in MP systems
    //
    If(And(PF08,0x0018))
    {
      \_SB.PR01.APCT()
    }

    Store (PF08, PF00)
    Return()
  }
} // End PR08

Scope(\_SB.PR09)
{
  Method(_PDC,1)
  {
    //
    // Call the _PDC for CPU1.
    //
    Store(\_SB.PR00.CPDC(Arg0), Local0)
    GCAP(Local0)
  }

  Method(_OSC, 4)
  {
    //
    // Call the _OSC for CPU1.
    //
    Store(\_SB.PR00.COSC(Arg0, Arg1, Arg2, Arg3), Local0)
    GCAP(Local0)
    Return (Local0)
  }

  Method(GCAP,1)
  {
    // Point to Status DWORD in the Arg0 buffer (STATUS)
    CreateDWordField(Arg0, 0, ST09)

    // Point to Caps DWORDs of the Arg0 buffer (CAPABILITIES)
    CreateDwordField(Arg0, 4, CP09)

    //
    // If the UUID was unrecognized or the _OSC revision was unsupported,
    // return without updating capabilities.
    //
    If(LOr(LEqual(ST09,0x6),LEqual(ST09,0xA)))
    {
      Return()
    }

    //
    // Check if this is a query (BIT0 of Status = 1).
    // If so, mask off the bits we support and return.
    //
    if (And(ST09, 1))
    {
      And(CP09, 0xBFF, CP09)
      Return()
    }

    //
    // Store result of PDC. (We clear out the MSB, which was just
    // used as a placeholder for the compiler; and then "OR" the
    // value in case we get multiple calls, each of which only
    // reports partial support.)
    //
    Or(And(PF09, 0x7FFFFFFF), CP09, PF09)
    //
    // Attempt to dynamically load the IST SSDTs if:
    //   (1) Driver supports P-States in MP configurations
    //   (2) Driver supports direct HW P-State control
    //
    //   PFxx[3]  = OS supports C1 and P-states in MP systems
    //   PFxx[0]  = OS supports direct access of the perf MSR
    //
    If(LEqual(And(PF09, 0x0009), 0x0009))
    {
      \_SB.PR01.APPT()
    }
    // Load Ap0Hwp SSDT
    // SB.OSC[5]   = OS supports CPPC from OSPM
    //
    if(And(\_SB.OSCP,0x0020)) {
      \_SB.PR01.HWPT()
    }
    //
    // Dynamically load the PSD SSDTs if:
    //   (1) If EIST or HWP Enabled
    //   (2) PSD SSDT is not already loaded
    //
    //   CFGD[0] = EIST Capable/Enabled
    //   CFGD[22] = HWP capable & enabled
    //   SDTL[9] = AP PSD SSDT Loaded
    //
    If(Lor(LEqual(And(PF09, 0x0009), 0x0009), And(\_SB.OSCP,0x0020)))
    {
      \_SB.PR01.APPS()
    }
    //
    // Load the CST SSDTs if:
    //   (1) Driver supports multi-processor configurations
    //
    //   PFxx[3]  = OS supports C1 and P-states in MP systems
    //   PFxx[4]  = OS supports ind. C2/C3 in MP systems
    //
    If(And(PF09,0x0018))
    {
      \_SB.PR01.APCT()
    }

    Store (PF09, PF00)
    Return()
  }
} // End PR09

Scope(\_SB.PR10)
{
  Method(_PDC,1)
  {
    //
    // Call the _PDC for CPU1.
    //
    Store(\_SB.PR00.CPDC(Arg0), Local0)
    GCAP(Local0)
  }

  Method(_OSC, 4)
  {
    //
    // Call the _OSC for CPU1.
    //
    Store(\_SB.PR00.COSC(Arg0, Arg1, Arg2, Arg3), Local0)
    GCAP(Local0)
    Return (Local0)
  }

  Method(GCAP,1)
  {
    // Point to Status DWORD in the Arg0 buffer (STATUS)
    CreateDWordField(Arg0, 0, ST10)

    // Point to Caps DWORDs of the Arg0 buffer (CAPABILITIES)
    CreateDwordField(Arg0, 4, CP10)

    //
    // If the UUID was unrecognized or the _OSC revision was unsupported,
    // return without updating capabilities.
    //
    If(LOr(LEqual(ST10,0x6),LEqual(ST10,0xA)))
    {
      Return()
    }

    //
    // Check if this is a query (BIT0 of Status = 1).
    // If so, mask off the bits we support and return.
    //
    if (And(ST10, 1))
    {
      And(ST10, 0xBFF, CP10)
      Return()
    }

    //
    // Store result of PDC. (We clear out the MSB, which was just
    // used as a placeholder for the compiler; and then "OR" the
    // value in case we get multiple calls, each of which only
    // reports partial support.)
    //
    Or(And(PF10, 0x7FFFFFFF), CP10, PF10)
    //
    // Attempt to dynamically load the IST SSDTs if:
    //   (1) Driver supports P-States in MP configurations
    //   (2) Driver supports direct HW P-State control
    //
    //   PFxx[3]  = OS supports C1 and P-states in MP systems
    //   PFxx[0]  = OS supports direct access of the perf MSR
    //
    If(LEqual(And(PF10, 0x0009), 0x0009))
    {
      \_SB.PR01.APPT()
    }
    // Load Ap0Hwp SSDT
    // SB.OSC[5]   = OS supports CPPC from OSPM
    //
    if(And(\_SB.OSCP,0x0020)) {
      \_SB.PR01.HWPT()
    }
    //
    // Dynamically load the PSD SSDTs if:
    //   (1) If EIST or HWP Enabled
    //   (2) PSD SSDT is not already loaded
    //
    //   CFGD[0] = EIST Capable/Enabled
    //   CFGD[22] = HWP capable & enabled
    //   SDTL[9] = AP PSD SSDT Loaded
    //
    If(Lor(LEqual(And(PF10, 0x0009), 0x0009), And(\_SB.OSCP,0x0020)))
    {
      \_SB.PR01.APPS()
    }
    //
    // Load the CST SSDTs if:
    //   (1) Driver supports multi-processor configurations
    //
    //   PFxx[3]  = OS supports C1 and P-states in MP systems
    //   PFxx[4]  = OS supports ind. C2/C3 in MP systems
    //
    If(And(PF10,0x0018))
    {
      \_SB.PR01.APCT()
    }

    Store (PF10, PF00)
    Return()
  }
} // End PR10

Scope(\_SB.PR11)
{
  Method(_PDC,1)
  {
    //
    // Call the _PDC for CPU1.
    //
    Store(\_SB.PR00.CPDC(Arg0), Local0)
    GCAP(Local0)
  }

  Method(_OSC, 4)
  {
    //
    // Call the _OSC for CPU1.
    //
    Store(\_SB.PR00.COSC(Arg0, Arg1, Arg2, Arg3), Local0)
    GCAP(Local0)
    Return (Local0)
  }

  Method(GCAP,1)
  {
    // Point to Status DWORD in the Arg0 buffer (STATUS)
    CreateDWordField(Arg0, 0, ST11)

    // Point to Caps DWORDs of the Arg0 buffer (CAPABILITIES)
    CreateDwordField(Arg0, 4, CP11)

    //
    // If the UUID was unrecognized or the _OSC revision was unsupported,
    // return without updating capabilities.
    //
    If(LOr(LEqual(ST11,0x6),LEqual(ST11,0xA)))
    {
      Return()
    }

    //
    // Check if this is a query (BIT0 of Status = 1).
    // If so, mask off the bits we support and return.
    //
    if (And(ST11, 1))
    {
      And(ST11, 0xBFF, CP11)
      Return()
    }

    //
    // Store result of PDC. (We clear out the MSB, which was just
    // used as a placeholder for the compiler; and then "OR" the
    // value in case we get multiple calls, each of which only
    // reports partial support.)
    //
    Or(And(PF11, 0x7FFFFFFF), CP11, PF11)
    //
    // Attempt to dynamically load the IST SSDTs if:
    //   (1) Driver supports P-States in MP configurations
    //   (2) Driver supports direct HW P-State control
    //
    //   PFxx[3]  = OS supports C1 and P-states in MP systems
    //   PFxx[0]  = OS supports direct access of the perf MSR
    //
    If(LEqual(And(PF11, 0x0009), 0x0009))
    {
      \_SB.PR01.APPT()
    }
    // Load Ap0Hwp SSDT
    // SB.OSC[5]   = OS supports CPPC from OSPM
    //
    if(And(\_SB.OSCP,0x0020)) {
      \_SB.PR01.HWPT()
    }
    //
    // Dynamically load the PSD SSDTs if:
    //   (1) If EIST or HWP Enabled
    //   (2) PSD SSDT is not already loaded
    //
    //   CFGD[0] = EIST Capable/Enabled
    //   CFGD[22] = HWP capable & enabled
    //   SDTL[9] = AP PSD SSDT Loaded
    //
    If(Lor(LEqual(And(PF11, 0x0009), 0x0009), And(\_SB.OSCP,0x0020)))
    {
      \_SB.PR01.APPS()
    }
    //
    // Load the CST SSDTs if:
    //   (1) Driver supports multi-processor configurations
    //
    //   PFxx[3]  = OS supports C1 and P-states in MP systems
    //   PFxx[4]  = OS supports ind. C2/C3 in MP systems
    //
    If(And(PF11,0x0018))
    {
      \_SB.PR01.APCT()
    }

    Store (PF11, PF00)
    Return()
  }
} // End PR11

Scope(\_SB.PR12)
{
  Method(_PDC,1)
  {
    //
    // Call the _PDC for CPU1.
    //
    Store(\_SB.PR00.CPDC(Arg0), Local0)
    GCAP(Local0)
  }

  Method(_OSC, 4)
  {
    //
    // Call the _OSC for CPU1.
    //
    Store(\_SB.PR00.COSC(Arg0, Arg1, Arg2, Arg3), Local0)
    GCAP(Local0)
    Return (Local0)
  }

  Method(GCAP,1)
  {
    // Point to Status DWORD in the Arg0 buffer (STATUS)
    CreateDWordField(Arg0, 0, ST12)

    // Point to Caps DWORDs of the Arg0 buffer (CAPABILITIES)
    CreateDwordField(Arg0, 4, CP12)

    //
    // If the UUID was unrecognized or the _OSC revision was unsupported,
    // return without updating capabilities.
    //
    If(LOr(LEqual(ST12,0x6),LEqual(ST12,0xA)))
    {
      Return()
    }

    //
    // Check if this is a query (BIT0 of Status = 1).
    // If so, mask off the bits we support and return.
    //
    if (And(ST12, 1))
    {
      And(ST12, 0xBFF, CP12)
      Return()
    }

    //
    // Store result of PDC. (We clear out the MSB, which was just
    // used as a placeholder for the compiler; and then "OR" the
    // value in case we get multiple calls, each of which only
    // reports partial support.)
    //
    Or(And(PF12, 0x7FFFFFFF), CP12, PF12)
    //
    // Attempt to dynamically load the IST SSDTs if:
    //   (1) Driver supports P-States in MP configurations
    //   (2) Driver supports direct HW P-State control
    //
    //   PFxx[3]  = OS supports C1 and P-states in MP systems
    //   PFxx[0]  = OS supports direct access of the perf MSR
    //
    If(LEqual(And(PF12, 0x0009), 0x0009))
    {
      \_SB.PR01.APPT()
    }
    // Load Ap0Hwp SSDT
    // SB.OSC[5]   = OS supports CPPC from OSPM
    //
    if(And(\_SB.OSCP,0x0020)) {
      \_SB.PR01.HWPT()
    }
    //
    // Dynamically load the PSD SSDTs if:
    //   (1) If EIST or HWP Enabled
    //   (2) PSD SSDT is not already loaded
    //
    //   CFGD[0] = EIST Capable/Enabled
    //   CFGD[22] = HWP capable & enabled
    //   SDTL[9] = AP PSD SSDT Loaded
    //
    If(Lor(LEqual(And(PF12, 0x0009), 0x0009), And(\_SB.OSCP,0x0020)))
    {
      \_SB.PR01.APPS()
    }
    //
    // Load the CST SSDTs if:
    //   (1) Driver supports multi-processor configurations
    //
    //   PFxx[3]  = OS supports C1 and P-states in MP systems
    //   PFxx[4]  = OS supports ind. C2/C3 in MP systems
    //
    If(And(PF12,0x0018))
    {
      \_SB.PR01.APCT()
    }

    Store (PF12, PF00)
    Return()
  }
} // End PR12

Scope(\_SB.PR13)
{
  Method(_PDC,1)
  {
    //
    // Call the _PDC for CPU1.
    //
    Store(\_SB.PR00.CPDC(Arg0), Local0)
    GCAP(Local0)
  }

  Method(_OSC, 4)
  {
    //
    // Call the _OSC for CPU1.
    //
    Store(\_SB.PR00.COSC(Arg0, Arg1, Arg2, Arg3), Local0)
    GCAP(Local0)
    Return (Local0)
  }

  Method(GCAP,1)
  {
    // Point to Status DWORD in the Arg0 buffer (STATUS)
    CreateDWordField(Arg0, 0, ST13)

    // Point to Caps DWORDs of the Arg0 buffer (CAPABILITIES)
    CreateDwordField(Arg0, 4, CP13)

    //
    // If the UUID was unrecognized or the _OSC revision was unsupported,
    // return without updating capabilities.
    //
    If(LOr(LEqual(ST13,0x6),LEqual(ST13,0xA)))
    {
      Return()
    }

    //
    // Check if this is a query (BIT0 of Status = 1).
    // If so, mask off the bits we support and return.
    //
    if (And(ST13, 1))
    {
      And(ST13, 0xBFF, CP13)
      Return()
    }

    //
    // Store result of PDC. (We clear out the MSB, which was just
    // used as a placeholder for the compiler; and then "OR" the
    // value in case we get multiple calls, each of which only
    // reports partial support.)
    //
    Or(And(PF13, 0x7FFFFFFF), CP13, PF13)
    //
    // Attempt to dynamically load the IST SSDTs if:
    //   (1) Driver supports P-States in MP configurations
    //   (2) Driver supports direct HW P-State control
    //
    //   PFxx[3]  = OS supports C1 and P-states in MP systems
    //   PFxx[0]  = OS supports direct access of the perf MSR
    //
    If(LEqual(And(PF13, 0x0009), 0x0009))
    {
      \_SB.PR01.APPT()
    }
    // Load Ap0Hwp SSDT
    // SB.OSC[5]   = OS supports CPPC from OSPM
    //
    if(And(\_SB.OSCP,0x0020)) {
      \_SB.PR01.HWPT()
    }
    //
    // Dynamically load the PSD SSDTs if:
    //   (1) If EIST or HWP Enabled
    //   (2) PSD SSDT is not already loaded
    //
    //   CFGD[0] = EIST Capable/Enabled
    //   CFGD[22] = HWP capable & enabled
    //   SDTL[9] = AP PSD SSDT Loaded
    //
    If(Lor(LEqual(And(PF13, 0x0009), 0x0009), And(\_SB.OSCP,0x0020)))
    {
      \_SB.PR01.APPS()
    }
    //
    // Load the CST SSDTs if:
    //   (1) Driver supports multi-processor configurations
    //
    //   PFxx[3]  = OS supports C1 and P-states in MP systems
    //   PFxx[4]  = OS supports ind. C2/C3 in MP systems
    //
    If(And(PF13,0x0018))
    {
      \_SB.PR01.APCT()
    }

    Store (PF13, PF00)
    Return()
  }
} // End PR13

Scope(\_SB.PR14)
{
  Method(_PDC,1)
  {
    //
    // Call the _PDC for CPU1.
    //
    Store(\_SB.PR00.CPDC(Arg0), Local0)
    GCAP(Local0)
  }

  Method(_OSC, 4)
  {
    //
    // Call the _OSC for CPU1.
    //
    Store(\_SB.PR00.COSC(Arg0, Arg1, Arg2, Arg3), Local0)
    GCAP(Local0)
    Return (Local0)
  }

  Method(GCAP,1)
  {
    // Point to Status DWORD in the Arg0 buffer (STATUS)
    CreateDWordField(Arg0, 0, ST14)

    // Point to Caps DWORDs of the Arg0 buffer (CAPABILITIES)
    CreateDwordField(Arg0, 4, CP14)

    //
    // If the UUID was unrecognized or the _OSC revision was unsupported,
    // return without updating capabilities.
    //
    If(LOr(LEqual(ST14,0x6),LEqual(ST14,0xA)))
    {
      Return()
    }

    //
    // Check if this is a query (BIT0 of Status = 1).
    // If so, mask off the bits we support and return.
    //
    if (And(ST14, 1))
    {
      And(ST14, 0xBFF, CP14)
      Return()
    }

    //
    // Store result of PDC. (We clear out the MSB, which was just
    // used as a placeholder for the compiler; and then "OR" the
    // value in case we get multiple calls, each of which only
    // reports partial support.)
    //
    Or(And(PF14, 0x7FFFFFFF), CP14, PF14)
    //
    // Attempt to dynamically load the IST SSDTs if:
    //   (1) Driver supports P-States in MP configurations
    //   (2) Driver supports direct HW P-State control
    //
    //   PFxx[3]  = OS supports C1 and P-states in MP systems
    //   PFxx[0]  = OS supports direct access of the perf MSR
    //
    If(LEqual(And(PF14, 0x0009), 0x0009))
    {
      \_SB.PR01.APPT()
    }
    // Load Ap0Hwp SSDT
    // SB.OSC[5]   = OS supports CPPC from OSPM
    //
    if(And(\_SB.OSCP,0x0020)) {
      \_SB.PR01.HWPT()
    }
    //
    // Dynamically load the PSD SSDTs if:
    //   (1) If EIST or HWP Enabled
    //   (2) PSD SSDT is not already loaded
    //
    //   CFGD[0] = EIST Capable/Enabled
    //   CFGD[22] = HWP capable & enabled
    //   SDTL[9] = AP PSD SSDT Loaded
    //
    If(Lor(LEqual(And(PF14, 0x0009), 0x0009), And(\_SB.OSCP,0x0020)))
    {
      \_SB.PR01.APPS()
    }
    //
    // Load the CST SSDTs if:
    //   (1) Driver supports multi-processor configurations
    //
    //   PFxx[3]  = OS supports C1 and P-states in MP systems
    //   PFxx[4]  = OS supports ind. C2/C3 in MP systems
    //
    If(And(PF14,0x0018))
    {
      \_SB.PR01.APCT()
    }

    Store (PF14, PF00)
    Return()
  }
} // End PR14

Scope(\_SB.PR15)
{
  Method(_PDC,1)
  {
    //
    // Call the _PDC for CPU1.
    //
    Store(\_SB.PR00.CPDC(Arg0), Local0)
    GCAP(Local0)
  }

  Method(_OSC, 4)
  {
    //
    // Call the _OSC for CPU1.
    //
    Store(\_SB.PR00.COSC(Arg0, Arg1, Arg2, Arg3), Local0)
    GCAP(Local0)
    Return (Local0)
  }

  Method(GCAP,1)
  {
    // Point to Status DWORD in the Arg0 buffer (STATUS)
    CreateDWordField(Arg0, 0, ST15)

    // Point to Caps DWORDs of the Arg0 buffer (CAPABILITIES)
    CreateDwordField(Arg0, 4, CP15)

    //
    // If the UUID was unrecognized or the _OSC revision was unsupported,
    // return without updating capabilities.
    //
    If(LOr(LEqual(ST15,0x6),LEqual(ST15,0xA)))
    {
      Return()
    }

    //
    // Check if this is a query (BIT0 of Status = 1).
    // If so, mask off the bits we support and return.
    //
    if (And(ST15, 1))
    {
      And(ST15, 0xBFF, CP15)
      Return()
    }

    //
    // Store result of PDC. (We clear out the MSB, which was just
    // used as a placeholder for the compiler; and then "OR" the
    // value in case we get multiple calls, each of which only
    // reports partial support.)
    //
    Or(And(PF15, 0x7FFFFFFF), CP15, PF15)
    //
    // Attempt to dynamically load the IST SSDTs if:
    //   (1) Driver supports P-States in MP configurations
    //   (2) Driver supports direct HW P-State control
    //
    //   PFxx[3]  = OS supports C1 and P-states in MP systems
    //   PFxx[0]  = OS supports direct access of the perf MSR
    //
    If(LEqual(And(PF15, 0x0009), 0x0009))
    {
      \_SB.PR01.APPT()
    }
    // Load Ap0Hwp SSDT
    // SB.OSC[5]   = OS supports CPPC from OSPM
    //
    if(And(\_SB.OSCP,0x0020)) {
      \_SB.PR01.HWPT()
    }
    //
    // Dynamically load the PSD SSDTs if:
    //   (1) If EIST or HWP Enabled
    //   (2) PSD SSDT is not already loaded
    //
    //   CFGD[0] = EIST Capable/Enabled
    //   CFGD[22] = HWP capable & enabled
    //   SDTL[9] = AP PSD SSDT Loaded
    //
    If(Lor(LEqual(And(PF15, 0x0009), 0x0009), And(\_SB.OSCP,0x0020)))
    {
      \_SB.PR01.APPS()
    }
    //
    // Load the CST SSDTs if:
    //   (1) Driver supports multi-processor configurations
    //
    //   PFxx[3]  = OS supports C1 and P-states in MP systems
    //   PFxx[4]  = OS supports ind. C2/C3 in MP systems
    //
    If(And(PF15,0x0018))
    {
      \_SB.PR01.APCT()
    }

    Store (PF15, PF00)
    Return()
  }
} // End PR15
Scope(\_SB.PR16)
{
  Method(_PDC,1)
  {
    //
    // Call the _PDC for CPU1.
    //
    Store(\_SB.PR00.CPDC(Arg0), Local0)
    GCAP(Local0)
  }

  Method(_OSC, 4)
  {
    //
    // Call the _OSC for CPU1.
    //
    Store(\_SB.PR00.COSC(Arg0, Arg1, Arg2, Arg3), Local0)
    GCAP(Local0)
    Return (Local0)
  }

  Method(GCAP,1)
  {
    // Point to Status DWORD in the Arg0 buffer (STATUS)
    CreateDWordField(Arg0, 0, ST16)

    // Point to Caps DWORDs of the Arg0 buffer (CAPABILITIES)
    CreateDwordField(Arg0, 4, CP16)

    //
    // If the UUID was unrecognized or the _OSC revision was unsupported,
    // return without updating capabilities.
    //
    If(LOr(LEqual(ST16,0x6),LEqual(ST16,0xA)))
    {
      Return()
    }

    //
    // Check if this is a query (BIT0 of Status = 1).
    // If so, mask off the bits we support and return.
    //
    if (And(ST16, 1))
    {
      And(ST16, 0xBFF, CP16)
      Return()
    }

    //
    // Store result of PDC. (We clear out the MSB, which was just
    // used as a placeholder for the compiler; and then "OR" the
    // value in case we get multiple calls, each of which only
    // reports partial support.)
    //
    Or(And(PF15, 0x7FFFFFFF), CP16, PF16)
    //
    // Attempt to dynamically load the IST SSDTs if:
    //   (1) Driver supports P-States in MP configurations
    //   (2) Driver supports direct HW P-State control
    //
    //   PFxx[3]  = OS supports C1 and P-states in MP systems
    //   PFxx[0]  = OS supports direct access of the perf MSR
    //
    If(LEqual(And(PF16, 0x0009), 0x0009))
    {
      \_SB.PR01.APPT()
    }
    // Load Ap0Hwp SSDT
    // SB.OSC[5]   = OS supports CPPC from OSPM
    //
    if(And(\_SB.OSCP,0x0020)) {
      \_SB.PR01.HWPT()
    }
    //
    // Dynamically load the PSD SSDTs if:
    //   (1) If EIST or HWP Enabled
    //   (2) PSD SSDT is not already loaded
    //
    //   CFGD[0] = EIST Capable/Enabled
    //   CFGD[22] = HWP capable & enabled
    //   SDTL[9] = AP PSD SSDT Loaded
    //
    If(Lor(LEqual(And(PF16, 0x0009), 0x0009), And(\_SB.OSCP,0x0020)))
    {
      \_SB.PR01.APPS()
    }
    //
    // Load the CST SSDTs if:
    //   (1) Driver supports multi-processor configurations
    //
    //   PFxx[3]  = OS supports C1 and P-states in MP systems
    //   PFxx[4]  = OS supports ind. C2/C3 in MP systems
    //
    If(And(PF16,0x0018))
    {
      \_SB.PR01.APCT()
    }

    Store (PF16, PF00)
    Return()
  }
} // End PR16
Scope(\_SB.PR17)
{
  Method(_PDC,1)
  {
    //
    // Call the _PDC for CPU1.
    //
    Store(\_SB.PR00.CPDC(Arg0), Local0)
    GCAP(Local0)
  }

  Method(_OSC, 4)
  {
    //
    // Call the _OSC for CPU1.
    //
    Store(\_SB.PR00.COSC(Arg0, Arg1, Arg2, Arg3), Local0)
    GCAP(Local0)
    Return (Local0)
  }

  Method(GCAP,1)
  {
    // Point to Status DWORD in the Arg0 buffer (STATUS)
    CreateDWordField(Arg0, 0, ST17)

    // Point to Caps DWORDs of the Arg0 buffer (CAPABILITIES)
    CreateDwordField(Arg0, 4, CP17)

    //
    // If the UUID was unrecognized or the _OSC revision was unsupported,
    // return without updating capabilities.
    //
    If(LOr(LEqual(ST17,0x6),LEqual(ST17,0xA)))
    {
      Return()
    }

    //
    // Check if this is a query (BIT0 of Status = 1).
    // If so, mask off the bits we support and return.
    //
    if (And(ST17, 1))
    {
      And(ST17, 0xBFF, CP17)
      Return()
    }

    //
    // Store result of PDC. (We clear out the MSB, which was just
    // used as a placeholder for the compiler; and then "OR" the
    // value in case we get multiple calls, each of which only
    // reports partial support.)
    //
    Or(And(PF17, 0x7FFFFFFF), CP17, PF17)
    //
    // Attempt to dynamically load the IST SSDTs if:
    //   (1) Driver supports P-States in MP configurations
    //   (2) Driver supports direct HW P-State control
    //
    //   PFxx[3]  = OS supports C1 and P-states in MP systems
    //   PFxx[0]  = OS supports direct access of the perf MSR
    //
    If(LEqual(And(PF17, 0x0009), 0x0009))
    {
      \_SB.PR01.APPT()
    }
    // Load Ap0Hwp SSDT
    // SB.OSC[5]   = OS supports CPPC from OSPM
    //
    if(And(\_SB.OSCP,0x0020)) {
      \_SB.PR01.HWPT()
    }
    //
    // Dynamically load the PSD SSDTs if:
    //   (1) If EIST or HWP Enabled
    //   (2) PSD SSDT is not already loaded
    //
    //   CFGD[0] = EIST Capable/Enabled
    //   CFGD[22] = HWP capable & enabled
    //   SDTL[9] = AP PSD SSDT Loaded
    //
    If(Lor(LEqual(And(PF17, 0x0009), 0x0009), And(\_SB.OSCP,0x0020)))
    {
      \_SB.PR01.APPS()
    }
    //
    // Load the CST SSDTs if:
    //   (1) Driver supports multi-processor configurations
    //
    //   PFxx[3]  = OS supports C1 and P-states in MP systems
    //   PFxx[4]  = OS supports ind. C2/C3 in MP systems
    //
    If(And(PF17,0x0018))
    {
      \_SB.PR01.APCT()
    }

    Store (PF17, PF00)
    Return()
  }
} // End PR17
Scope(\_SB.PR18)
{
  Method(_PDC,1)
  {
    //
    // Call the _PDC for CPU1.
    //
    Store(\_SB.PR00.CPDC(Arg0), Local0)
    GCAP(Local0)
  }

  Method(_OSC, 4)
  {
    //
    // Call the _OSC for CPU1.
    //
    Store(\_SB.PR00.COSC(Arg0, Arg1, Arg2, Arg3), Local0)
    GCAP(Local0)
    Return (Local0)
  }

  Method(GCAP,1)
  {
    // Point to Status DWORD in the Arg0 buffer (STATUS)
    CreateDWordField(Arg0, 0, ST18)

    // Point to Caps DWORDs of the Arg0 buffer (CAPABILITIES)
    CreateDwordField(Arg0, 4, CP18)

    //
    // If the UUID was unrecognized or the _OSC revision was unsupported,
    // return without updating capabilities.
    //
    If(LOr(LEqual(ST18,0x6),LEqual(ST18,0xA)))
    {
      Return()
    }

    //
    // Check if this is a query (BIT0 of Status = 1).
    // If so, mask off the bits we support and return.
    //
    if (And(ST18, 1))
    {
      And(ST18, 0xBFF, CP18)
      Return()
    }

    //
    // Store result of PDC. (We clear out the MSB, which was just
    // used as a placeholder for the compiler; and then "OR" the
    // value in case we get multiple calls, each of which only
    // reports partial support.)
    //
    Or(And(PF18, 0x7FFFFFFF), CP18, PF18)
    //
    // Attempt to dynamically load the IST SSDTs if:
    //   (1) Driver supports P-States in MP configurations
    //   (2) Driver supports direct HW P-State control
    //
    //   PFxx[3]  = OS supports C1 and P-states in MP systems
    //   PFxx[0]  = OS supports direct access of the perf MSR
    //
    If(LEqual(And(PF18, 0x0009), 0x0009))
    {
      \_SB.PR01.APPT()
    }
    // Load Ap0Hwp SSDT
    // SB.OSC[5]   = OS supports CPPC from OSPM
    //
    if(And(\_SB.OSCP,0x0020)) {
      \_SB.PR01.HWPT()
    }
    //
    // Dynamically load the PSD SSDTs if:
    //   (1) If EIST or HWP Enabled
    //   (2) PSD SSDT is not already loaded
    //
    //   CFGD[0] = EIST Capable/Enabled
    //   CFGD[22] = HWP capable & enabled
    //   SDTL[9] = AP PSD SSDT Loaded
    //
    If(Lor(LEqual(And(PF18, 0x0009), 0x0009), And(\_SB.OSCP,0x0020)))
    {
      \_SB.PR01.APPS()
    }
    //
    // Load the CST SSDTs if:
    //   (1) Driver supports multi-processor configurations
    //
    //   PFxx[3]  = OS supports C1 and P-states in MP systems
    //   PFxx[4]  = OS supports ind. C2/C3 in MP systems
    //
    If(And(PF18,0x0018))
    {
      \_SB.PR01.APCT()
    }

    Store (PF18, PF00)
    Return()
  }
} // End PR18
Scope(\_SB.PR19)
{
  Method(_PDC,1)
  {
    //
    // Call the _PDC for CPU1.
    //
    Store(\_SB.PR00.CPDC(Arg0), Local0)
    GCAP(Local0)
  }

  Method(_OSC, 4)
  {
    //
    // Call the _OSC for CPU1.
    //
    Store(\_SB.PR00.COSC(Arg0, Arg1, Arg2, Arg3), Local0)
    GCAP(Local0)
    Return (Local0)
  }

  Method(GCAP,1)
  {
    // Point to Status DWORD in the Arg0 buffer (STATUS)
    CreateDWordField(Arg0, 0, ST19)

    // Point to Caps DWORDs of the Arg0 buffer (CAPABILITIES)
    CreateDwordField(Arg0, 4, CP19)

    //
    // If the UUID was unrecognized or the _OSC revision was unsupported,
    // return without updating capabilities.
    //
    If(LOr(LEqual(ST19,0x6),LEqual(ST19,0xA)))
    {
      Return()
    }

    //
    // Check if this is a query (BIT0 of Status = 1).
    // If so, mask off the bits we support and return.
    //
    if (And(ST19, 1))
    {
      And(ST19, 0xBFF, CP19)
      Return()
    }

    //
    // Store result of PDC. (We clear out the MSB, which was just
    // used as a placeholder for the compiler; and then "OR" the
    // value in case we get multiple calls, each of which only
    // reports partial support.)
    //
    Or(And(PF19, 0x7FFFFFFF), CP19, PF19)
    //
    // Attempt to dynamically load the IST SSDTs if:
    //   (1) Driver supports P-States in MP configurations
    //   (2) Driver supports direct HW P-State control
    //
    //   PFxx[3]  = OS supports C1 and P-states in MP systems
    //   PFxx[0]  = OS supports direct access of the perf MSR
    //
    If(LEqual(And(PF19, 0x0009), 0x0009))
    {
      \_SB.PR01.APPT()
    }
    // Load Ap0Hwp SSDT
    // SB.OSC[5]   = OS supports CPPC from OSPM
    //
    if(And(\_SB.OSCP,0x0020)) {
      \_SB.PR01.HWPT()
    }
    //
    // Dynamically load the PSD SSDTs if:
    //   (1) If EIST or HWP Enabled
    //   (2) PSD SSDT is not already loaded
    //
    //   CFGD[0] = EIST Capable/Enabled
    //   CFGD[22] = HWP capable & enabled
    //   SDTL[9] = AP PSD SSDT Loaded
    //
    If(Lor(LEqual(And(PF19, 0x0009), 0x0009), And(\_SB.OSCP,0x0020)))
    {
      \_SB.PR01.APPS()
    }
    //
    // Load the CST SSDTs if:
    //   (1) Driver supports multi-processor configurations
    //
    //   PFxx[3]  = OS supports C1 and P-states in MP systems
    //   PFxx[4]  = OS supports ind. C2/C3 in MP systems
    //
    If(And(PF19,0x0018))
    {
      \_SB.PR01.APCT()
    }

    Store (PF19, PF00)
    Return()
  }
} // End PR19
Scope(\_SB.PR20)
{
  Method(_PDC,1)
  {
    //
    // Call the _PDC for CPU1.
    //
    Store(\_SB.PR00.CPDC(Arg0), Local0)
    GCAP(Local0)
  }

  Method(_OSC, 4)
  {
    //
    // Call the _OSC for CPU1.
    //
    Store(\_SB.PR00.COSC(Arg0, Arg1, Arg2, Arg3), Local0)
    GCAP(Local0)
    Return (Local0)
  }

  Method(GCAP,1)
  {
    // Point to Status DWORD in the Arg0 buffer (STATUS)
    CreateDWordField(Arg0, 0, ST20)

    // Point to Caps DWORDs of the Arg0 buffer (CAPABILITIES)
    CreateDwordField(Arg0, 4, CP20)

    //
    // If the UUID was unrecognized or the _OSC revision was unsupported,
    // return without updating capabilities.
    //
    If(LOr(LEqual(ST20,0x6),LEqual(ST20,0xA)))
    {
      Return()
    }

    //
    // Check if this is a query (BIT0 of Status = 1).
    // If so, mask off the bits we support and return.
    //
    if (And(ST20, 1))
    {
      And(ST20, 0xBFF, CP20)
      Return()
    }

    //
    // Store result of PDC. (We clear out the MSB, which was just
    // used as a placeholder for the compiler; and then "OR" the
    // value in case we get multiple calls, each of which only
    // reports partial support.)
    //
    Or(And(PF20, 0x7FFFFFFF), CP20, PF20)
    //
    // Attempt to dynamically load the IST SSDTs if:
    //   (1) Driver supports P-States in MP configurations
    //   (2) Driver supports direct HW P-State control
    //
    //   PFxx[3]  = OS supports C1 and P-states in MP systems
    //   PFxx[0]  = OS supports direct access of the perf MSR
    //
    If(LEqual(And(PF20, 0x0009), 0x0009))
    {
      \_SB.PR01.APPT()
    }
    // Load Ap0Hwp SSDT
    // SB.OSC[5]   = OS supports CPPC from OSPM
    //
    if(And(\_SB.OSCP,0x0020)) {
      \_SB.PR01.HWPT()
    }
    //
    // Dynamically load the PSD SSDTs if:
    //   (1) If EIST or HWP Enabled
    //   (2) PSD SSDT is not already loaded
    //
    //   CFGD[0] = EIST Capable/Enabled
    //   CFGD[22] = HWP capable & enabled
    //   SDTL[9] = AP PSD SSDT Loaded
    //
    If(Lor(LEqual(And(PF20, 0x0009), 0x0009), And(\_SB.OSCP,0x0020)))
    {
      \_SB.PR01.APPS()
    }
    //
    // Load the CST SSDTs if:
    //   (1) Driver supports multi-processor configurations
    //
    //   PFxx[3]  = OS supports C1 and P-states in MP systems
    //   PFxx[4]  = OS supports ind. C2/C3 in MP systems
    //
    If(And(PF20,0x0018))
    {
      \_SB.PR01.APCT()
    }

    Store (PF20, PF00)
    Return()
  }
} // End PR20
Scope(\_SB.PR21)
{
  Method(_PDC,1)
  {
    //
    // Call the _PDC for CPU1.
    //
    Store(\_SB.PR00.CPDC(Arg0), Local0)
    GCAP(Local0)
  }

  Method(_OSC, 4)
  {
    //
    // Call the _OSC for CPU1.
    //
    Store(\_SB.PR00.COSC(Arg0, Arg1, Arg2, Arg3), Local0)
    GCAP(Local0)
    Return (Local0)
  }

  Method(GCAP,1)
  {
    // Point to Status DWORD in the Arg0 buffer (STATUS)
    CreateDWordField(Arg0, 0, ST21)

    // Point to Caps DWORDs of the Arg0 buffer (CAPABILITIES)
    CreateDwordField(Arg0, 4, CP21)

    //
    // If the UUID was unrecognized or the _OSC revision was unsupported,
    // return without updating capabilities.
    //
    If(LOr(LEqual(ST21,0x6),LEqual(ST21,0xA)))
    {
      Return()
    }

    //
    // Check if this is a query (BIT0 of Status = 1).
    // If so, mask off the bits we support and return.
    //
    if (And(ST21, 1))
    {
      And(ST21, 0xBFF, CP21)
      Return()
    }

    //
    // Store result of PDC. (We clear out the MSB, which was just
    // used as a placeholder for the compiler; and then "OR" the
    // value in case we get multiple calls, each of which only
    // reports partial support.)
    //
    Or(And(PF21, 0x7FFFFFFF), CP21, PF21)
    //
    // Attempt to dynamically load the IST SSDTs if:
    //   (1) Driver supports P-States in MP configurations
    //   (2) Driver supports direct HW P-State control
    //
    //   PFxx[3]  = OS supports C1 and P-states in MP systems
    //   PFxx[0]  = OS supports direct access of the perf MSR
    //
    If(LEqual(And(PF21, 0x0009), 0x0009))
    {
      \_SB.PR01.APPT()
    }
    // Load Ap0Hwp SSDT
    // SB.OSC[5]   = OS supports CPPC from OSPM
    //
    if(And(\_SB.OSCP,0x0021)) {
      \_SB.PR01.HWPT()
    }
    //
    // Dynamically load the PSD SSDTs if:
    //   (1) If EIST or HWP Enabled
    //   (2) PSD SSDT is not already loaded
    //
    //   CFGD[0] = EIST Capable/Enabled
    //   CFGD[22] = HWP capable & enabled
    //   SDTL[9] = AP PSD SSDT Loaded
    //
    If(Lor(LEqual(And(PF21, 0x0009), 0x0009), And(\_SB.OSCP,0x0020)))
    {
      \_SB.PR01.APPS()
    }
    //
    // Load the CST SSDTs if:
    //   (1) Driver supports multi-processor configurations
    //
    //   PFxx[3]  = OS supports C1 and P-states in MP systems
    //   PFxx[4]  = OS supports ind. C2/C3 in MP systems
    //
    If(And(PF21,0x0018))
    {
      \_SB.PR01.APCT()
    }

    Store (PF21, PF00)
    Return()
  }
} // End PR21
Scope(\_SB.PR22)
{
  Method(_PDC,1)
  {
    //
    // Call the _PDC for CPU1.
    //
    Store(\_SB.PR00.CPDC(Arg0), Local0)
    GCAP(Local0)
  }

  Method(_OSC, 4)
  {
    //
    // Call the _OSC for CPU1.
    //
    Store(\_SB.PR00.COSC(Arg0, Arg1, Arg2, Arg3), Local0)
    GCAP(Local0)
    Return (Local0)
  }

  Method(GCAP,1)
  {
    // Point to Status DWORD in the Arg0 buffer (STATUS)
    CreateDWordField(Arg0, 0, ST22)

    // Point to Caps DWORDs of the Arg0 buffer (CAPABILITIES)
    CreateDwordField(Arg0, 4, CP22)

    //
    // If the UUID was unrecognized or the _OSC revision was unsupported,
    // return without updating capabilities.
    //
    If(LOr(LEqual(ST22,0x6),LEqual(ST22,0xA)))
    {
      Return()
    }

    //
    // Check if this is a query (BIT0 of Status = 1).
    // If so, mask off the bits we support and return.
    //
    if (And(ST22, 1))
    {
      And(ST22, 0xBFF, CP22)
      Return()
    }

    //
    // Store result of PDC. (We clear out the MSB, which was just
    // used as a placeholder for the compiler; and then "OR" the
    // value in case we get multiple calls, each of which only
    // reports partial support.)
    //
    Or(And(PF22, 0x7FFFFFFF), CP22, PF22)
    //
    // Attempt to dynamically load the IST SSDTs if:
    //   (1) Driver supports P-States in MP configurations
    //   (2) Driver supports direct HW P-State control
    //
    //   PFxx[3]  = OS supports C1 and P-states in MP systems
    //   PFxx[0]  = OS supports direct access of the perf MSR
    //
    If(LEqual(And(PF22, 0x0009), 0x0009))
    {
      \_SB.PR01.APPT()
    }
    // Load Ap0Hwp SSDT
    // SB.OSC[5]   = OS supports CPPC from OSPM
    //
    if(And(\_SB.OSCP,0x0020)) {
      \_SB.PR01.HWPT()
    }
    //
    // Dynamically load the PSD SSDTs if:
    //   (1) If EIST or HWP Enabled
    //   (2) PSD SSDT is not already loaded
    //
    //   CFGD[0] = EIST Capable/Enabled
    //   CFGD[22] = HWP capable & enabled
    //   SDTL[9] = AP PSD SSDT Loaded
    //
    If(Lor(LEqual(And(PF22, 0x0009), 0x0009), And(\_SB.OSCP,0x0020)))
    {
      \_SB.PR01.APPS()
    }
    //
    // Load the CST SSDTs if:
    //   (1) Driver supports multi-processor configurations
    //
    //   PFxx[3]  = OS supports C1 and P-states in MP systems
    //   PFxx[4]  = OS supports ind. C2/C3 in MP systems
    //
    If(And(PF22,0x0018))
    {
      \_SB.PR01.APCT()
    }

    Store (PF22, PF00)
    Return()
  }
} // End PR22
Scope(\_SB.PR23)
{
  Method(_PDC,1)
  {
    //
    // Call the _PDC for CPU1.
    //
    Store(\_SB.PR00.CPDC(Arg0), Local0)
    GCAP(Local0)
  }

  Method(_OSC, 4)
  {
    //
    // Call the _OSC for CPU1.
    //
    Store(\_SB.PR00.COSC(Arg0, Arg1, Arg2, Arg3), Local0)
    GCAP(Local0)
    Return (Local0)
  }

  Method(GCAP,1)
  {
    // Point to Status DWORD in the Arg0 buffer (STATUS)
    CreateDWordField(Arg0, 0, ST23)

    // Point to Caps DWORDs of the Arg0 buffer (CAPABILITIES)
    CreateDwordField(Arg0, 4, CP23)

    //
    // If the UUID was unrecognized or the _OSC revision was unsupported,
    // return without updating capabilities.
    //
    If(LOr(LEqual(ST23,0x6),LEqual(ST23,0xA)))
    {
      Return()
    }

    //
    // Check if this is a query (BIT0 of Status = 1).
    // If so, mask off the bits we support and return.
    //
    if (And(ST23, 1))
    {
      And(ST23, 0xBFF, CP23)
      Return()
    }

    //
    // Store result of PDC. (We clear out the MSB, which was just
    // used as a placeholder for the compiler; and then "OR" the
    // value in case we get multiple calls, each of which only
    // reports partial support.)
    //
    Or(And(PF23, 0x7FFFFFFF), CP23, PF23)
    //
    // Attempt to dynamically load the IST SSDTs if:
    //   (1) Driver supports P-States in MP configurations
    //   (2) Driver supports direct HW P-State control
    //
    //   PFxx[3]  = OS supports C1 and P-states in MP systems
    //   PFxx[0]  = OS supports direct access of the perf MSR
    //
    If(LEqual(And(PF23, 0x0009), 0x0009))
    {
      \_SB.PR01.APPT()
    }
    // Load Ap0Hwp SSDT
    // SB.OSC[5]   = OS supports CPPC from OSPM
    //
    if(And(\_SB.OSCP,0x0020)) {
      \_SB.PR01.HWPT()
    }
    //
    // Dynamically load the PSD SSDTs if:
    //   (1) If EIST or HWP Enabled
    //   (2) PSD SSDT is not already loaded
    //
    //   CFGD[0] = EIST Capable/Enabled
    //   CFGD[22] = HWP capable & enabled
    //   SDTL[9] = AP PSD SSDT Loaded
    //
    If(Lor(LEqual(And(PF23, 0x0009), 0x0009), And(\_SB.OSCP,0x0020)))
    {
      \_SB.PR01.APPS()
    }
    //
    // Load the CST SSDTs if:
    //   (1) Driver supports multi-processor configurations
    //
    //   PFxx[3]  = OS supports C1 and P-states in MP systems
    //   PFxx[4]  = OS supports ind. C2/C3 in MP systems
    //
    If(And(PF23,0x0018))
    {
      \_SB.PR01.APCT()
    }

    Store (PF23, PF00)
    Return()
  }
} // End PR23

//
//TxtAcm
//
  Scope(\_SB) {
    If (LEqual(TXTE, 1)) {
      Device (ACM) {
        Name (_HID, "INTC1025" )  // _HID: Hardware ID
        //
        // Update _CID (Compatibility ID)
        //   _CID will be updated based on CPU ID
        //
        Method (_CID) {
          //
          // Mask CPU Family
          //
          And (CPID, 0xFFFFFFF0, Local0)
          If (LOr (LEqual (Local0, 0x000806C0), LEqual (Local0, 0x000806D0))) {
            Return ("INT_TGL_SINIT")
          }
          If (LOr (LEqual (Local0, 0x000A0680), LEqual (Local0, 0x000A0670))) {
            Return ("INT_RKL_SINIT")
          }
          Return ("INT_UNK_SINIT")
        } // End of Method (_CID)
      } // End of Device (ACM)
    }
  } // End of Scope(\_SB) for TxtAcm
} // End of Definition Block
