/** @file

  Copyright (c) 2021, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "CpuPowerMgmt.h"

#define AP_SSDT(INDEX) \
If (CondRefOf (\_SB.PR ## INDEX)) { \
  Scope(\_SB.PR ## INDEX) \
  { \
    Method(_PDC,1) \
    { \
      Store(\_SB.PR00.CPDC(Arg0), Local0) \
      GCAP(Local0) \
    } \
  \
    Method(_OSC, 4) \
    { \
      Store(\_SB.PR00.COSC(Arg0, Arg1, Arg2, Arg3), Local0) \
      GCAP(Local0) \
      Return (Local0) \
    } \
  \
    Method(GCAP,1) \
    { \
      /* Point to Status DWORD in the Arg0 buffer (STATUS) */ \
      CreateDWordField(Arg0, 0, ST ## INDEX) \
  \
      /* Point to Caps DWORDs of the Arg0 buffer (CAPABILITIES) */ \
      CreateDwordField(Arg0, 4, CP ## INDEX) \
  \
      /* If the UUID was unrecognized or the _OSC revision was unsupported, */  \
      /* return without updating capabilities. */ \
      If(LOr(LEqual(ST ## INDEX,0x6),LEqual(ST ## INDEX,0xA))) \
      { \
        Return() \
      } \
  \
      /* Check if this is a query (BIT0 of Status = 1). */ \
      /* If so, mask off the bits we support and return. */ \
      if (And(ST ## INDEX, 1)) \
      { \
        And(CP ## INDEX, 0xBFF, CP ## INDEX) \
        Return() \
      } \
  \
      /* Store result of PDC. (We clear out the MSB, which was just */ \
      /* used as a placeholder for the compiler; and then "OR" the */ \
      /* value in case we get multiple calls, each of which only */ \
      /* reports partial support.) */ \
      Or(And(PF ## INDEX, 0x7FFFFFFF), CP ## INDEX, PF ## INDEX) \
      /* Attempt to dynamically load the IST SSDTs if: */ \
      /*   (1) Driver supports P-States in MP configurations */ \
      /*   (2) Driver supports direct HW P-State control */ \
      /*  */ \
      /*   PFxx[3]  = OS supports C1 and P-states in MP systems */ \
      /*   PFxx[0]  = OS supports direct access of the perf MSR */ \
      If(LEqual(And(PF ## INDEX, 0x0009), 0x0009)) \
      { \
        \_SB.PR01.APPT() \
      } \
      /* Load Ap0Hwp SSDT */ \
      /* SB.OSC[5]   = OS supports CPPC from OSPM */ \
      if(And(\_SB.OSCP,0x0020)) { \
        \_SB.PR01.HWPT() \
      } \
  \
      /* Dynamically load the PSD SSDTs if: */ \
      /*   (1) If EIST or HWP Enabled */ \
      /*   (2) PSD SSDT is not already loaded */ \
      /*  */ \
      /*   CFGD[0] = EIST Capable/Enabled */ \
      /*   CFGD[22] = HWP capable & enabled */ \
      /*   SDTL[9] = AP PSD SSDT Loaded */ \
      If(Lor(LEqual(And(PF ## INDEX, 0x0009), 0x0009), And(\_SB.OSCP,0x0020))) \
      { \
        \_SB.PR01.APPS() \
      } \
  \
      /* Load the CST SSDTs if: */ \
      /*   (1) Driver supports multi-processor configurations */ \
      /*  */ \
      /*   PFxx[3]  = OS supports C1 and P-states in MP systems */ \
      /*   PFxx[4]  = OS supports ind. C2/C3 in MP systems */ \
      If(And(PF ## INDEX,0x0018)) \
      { \
        \_SB.PR01.APCT() \
      } \
  \
      Store (PF ## INDEX, PF00) \
      Return() \
    } \
  \
  } \
}

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
  External(\_SB.PR24, DeviceObj)
  External(\_SB.PR25, DeviceObj)
  External(\_SB.PR26, DeviceObj)
  External(\_SB.PR27, DeviceObj)
  External(\_SB.PR28, DeviceObj)
  External(\_SB.PR29, DeviceObj)
  External(\_SB.PR30, DeviceObj)
  External(\_SB.PR31, DeviceObj)
  External(\_SB.PR32, DeviceObj)
  External(\_SB.PR33, DeviceObj)
  External(\_SB.PR34, DeviceObj)
  External(\_SB.PR35, DeviceObj)
  External(\_SB.PR36, DeviceObj)
  External(\_SB.PR37, DeviceObj)
  External(\_SB.PR38, DeviceObj)
  External(\_SB.PR39, DeviceObj)
  External(\_SB.PR40, DeviceObj)
  External(\_SB.PR41, DeviceObj)
  External(\_SB.PR42, DeviceObj)
  External(\_SB.PR43, DeviceObj)
  External(\_SB.PR44, DeviceObj)
  External(\_SB.PR45, DeviceObj)
  External(\_SB.PR46, DeviceObj)
  External(\_SB.PR47, DeviceObj)
  External(\_SB.PR48, DeviceObj)
  External(\_SB.PR49, DeviceObj)
  External(\_SB.PR50, DeviceObj)
  External(\_SB.PR51, DeviceObj)
  External(\_SB.PR52, DeviceObj)
  External(\_SB.PR53, DeviceObj)
  External(\_SB.PR54, DeviceObj)
  External(\_SB.PR55, DeviceObj)
  External(\_SB.PR56, DeviceObj)
  External(\_SB.PR57, DeviceObj)
  External(\_SB.PR58, DeviceObj)
  External(\_SB.PR59, DeviceObj)
  External(\_SB.PR60, DeviceObj)
  External(\_SB.PR61, DeviceObj)
  External(\_SB.PR62, DeviceObj)
  External(\_SB.PR63, DeviceObj)
  External(OSYS)

  Scope(\)
  {

#ifdef LOAD_SSDT_DYNAMIC
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
#endif

    Name(\PF00, 0x80000000)  // PR00 _PDC Flags.
    Name(\PF01, 0x80000000)  // PR01 _PDC Flags.
    Name(\PF02, 0x80000000)  // PR02 _PDC Flags.
    Name(\PF03, 0x80000000)  // PR03 _PDC Flags.
    Name(\PF04, 0x80000000)  // PR04 _PDC Flags.
    Name(\PF05, 0x80000000)  // PR05 _PDC Flags.
    Name(\PF06, 0x80000000)  // PR06 _PDC Flags.
    Name(\PF07, 0x80000000)  // PR07 _PDC Flags.
    Name(\PF08, 0x80000000)  // PR08 _PDC Flags.
    Name(\PF09, 0x80000000)  // PR09 _PDC Flags.
    Name(\PF10, 0x80000000)  // PR10 _PDC Flags.
    Name(\PF11, 0x80000000)  // PR11 _PDC Flags.
    Name(\PF12, 0x80000000)  // PR12 _PDC Flags.
    Name(\PF13, 0x80000000)  // PR13 _PDC Flags.
    Name(\PF14, 0x80000000)  // PR14 _PDC Flags.
    Name(\PF15, 0x80000000)  // PR15 _PDC Flags.
    Name(\PF16, 0x80000000)  // PR16 _PDC Flags.
    Name(\PF17, 0x80000000)  // PR17 _PDC Flags.
    Name(\PF18, 0x80000000)  // PR18 _PDC Flags.
    Name(\PF19, 0x80000000)  // PR19 _PDC Flags.
    Name(\PF20, 0x80000000)  // PR20 _PDC Flags.
    Name(\PF21, 0x80000000)  // PR21 _PDC Flags.
    Name(\PF22, 0x80000000)  // PR22 _PDC Flags.
    Name(\PF23, 0x80000000)  // PR23 _PDC Flags.
    Name(\PF24, 0x80000000)  // PR24 _PDC Flags.
    Name(\PF25, 0x80000000)  // PR25 _PDC Flags.
    Name(\PF26, 0x80000000)  // PR26 _PDC Flags.
    Name(\PF27, 0x80000000)  // PR27 _PDC Flags.
    Name(\PF28, 0x80000000)  // PR28 _PDC Flags.
    Name(\PF29, 0x80000000)  // PR29 _PDC Flags.
    Name(\PF30, 0x80000000)  // PR30 _PDC Flags.
    Name(\PF31, 0x80000000)  // PR31 _PDC Flags.
    Name(\PF32, 0x80000000)  // PR32 _PDC Flags.
    Name(\PF33, 0x80000000)  // PR33 _PDC Flags.
    Name(\PF34, 0x80000000)  // PR34 _PDC Flags.
    Name(\PF35, 0x80000000)  // PR35 _PDC Flags.
    Name(\PF36, 0x80000000)  // PR36 _PDC Flags.
    Name(\PF37, 0x80000000)  // PR37 _PDC Flags.
    Name(\PF38, 0x80000000)  // PR38 _PDC Flags.
    Name(\PF39, 0x80000000)  // PR39 _PDC Flags.
    Name(\PF40, 0x80000000)  // PR40 _PDC Flags.
    Name(\PF41, 0x80000000)  // PR41 _PDC Flags.
    Name(\PF42, 0x80000000)  // PR42 _PDC Flags.
    Name(\PF43, 0x80000000)  // PR43 _PDC Flags.
    Name(\PF44, 0x80000000)  // PR44 _PDC Flags.
    Name(\PF45, 0x80000000)  // PR45 _PDC Flags.
    Name(\PF46, 0x80000000)  // PR46 _PDC Flags.
    Name(\PF47, 0x80000000)  // PR47 _PDC Flags.
    Name(\PF48, 0x80000000)  // PR48 _PDC Flags.
    Name(\PF49, 0x80000000)  // PR49 _PDC Flags.
    Name(\PF50, 0x80000000)  // PR50 _PDC Flags.
    Name(\PF51, 0x80000000)  // PR51 _PDC Flags.
    Name(\PF52, 0x80000000)  // PR52 _PDC Flags.
    Name(\PF53, 0x80000000)  // PR53 _PDC Flags.
    Name(\PF54, 0x80000000)  // PR54 _PDC Flags.
    Name(\PF55, 0x80000000)  // PR55 _PDC Flags.
    Name(\PF56, 0x80000000)  // PR56 _PDC Flags.
    Name(\PF57, 0x80000000)  // PR57 _PDC Flags.
    Name(\PF58, 0x80000000)  // PR58 _PDC Flags.
    Name(\PF59, 0x80000000)  // PR59 _PDC Flags.
    Name(\PF60, 0x80000000)  // PR60 _PDC Flags.
    Name(\PF61, 0x80000000)  // PR61 _PDC Flags.
    Name(\PF62, 0x80000000)  // PR62 _PDC Flags.
    Name(\PF63, 0x80000000)  // PR63 _PDC Flags.

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
#ifdef LOAD_SSDT_DYNAMIC
          OperationRegion(CST0,SystemMemory,DeRefOf(Index(SSDT,7)),DeRefOf(Index(SSDT,8)))
          Load(CST0, HC0)  // Dynamically load the CPU0CST SSDT
#endif
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
#ifdef LOAD_SSDT_DYNAMIC
          OperationRegion(IST0,SystemMemory,DeRefOf(Index(SSDT,1)),DeRefOf(Index(SSDT,2)))
          Load(IST0, HI0)  // Dynamically load the CPU1IST SSDT
#endif
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
#ifdef LOAD_SSDT_DYNAMIC
        OperationRegion(PSD0,SystemMemory,DeRefOf(Index(SSDT,22)),DeRefOf(Index(SSDT,23)))
        Load(PSD0, HP0)  // Dynamically load the CPU0PSD SSDT
#endif
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
#ifdef LOAD_SSDT_DYNAMIC
            OperationRegion(HWP0,SystemMemory,DeRefOf(Index(SSDT,13)),DeRefOf(Index(SSDT,14)))
            Load(HWP0, HW0)  // Dynamically load the Cpu0Hwp SSDT
#endif
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
#ifdef LOAD_SSDT_DYNAMIC
        //
        // Dynamically load the APCST SSDT
        //
        OperationRegion(CST1,SystemMemory,DeRefOf(Index(SSDT,10)),DeRefOf(Index(SSDT,11)))
        Load(CST1, HC1)
#endif
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
#ifdef LOAD_SSDT_DYNAMIC
        OperationRegion(IST1,SystemMemory,DeRefOf(Index(SSDT,4)),DeRefOf(Index(SSDT,5)))
        Load(IST1, HI1)  // Dynamically load the CPU1IST SSDT
#endif
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
#ifdef LOAD_SSDT_DYNAMIC
        OperationRegion(PSD1,SystemMemory,DeRefOf(Index(SSDT,25)),DeRefOf(Index(SSDT,26)))
        Load(PSD1, HP1)  // Dynamically load the APPSD SSDT
#endif
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
#ifdef LOAD_SSDT_DYNAMIC
          OperationRegion(HWP1,SystemMemory,DeRefOf(Index(SSDT,16)),DeRefOf(Index(SSDT,17)))
          Load(HWP1, HW1)  // Dynamically load the Ap0Hwp SSDT
#endif
        }
      }
    }
  }  // End PR01

  AP_SSDT (02)
  AP_SSDT (03)
  AP_SSDT (04)
  AP_SSDT (05)
  AP_SSDT (06)
  AP_SSDT (07)
  AP_SSDT (08)
  AP_SSDT (09)
  AP_SSDT (10)
  AP_SSDT (11)
  AP_SSDT (12)
  AP_SSDT (13)
  AP_SSDT (14)
  AP_SSDT (15)
  AP_SSDT (16)
  AP_SSDT (17)
  AP_SSDT (18)
  AP_SSDT (19)
  AP_SSDT (20)
  AP_SSDT (21)
  AP_SSDT (22)
  AP_SSDT (23)
  AP_SSDT (24)
  AP_SSDT (25)
  AP_SSDT (26)
  AP_SSDT (27)
  AP_SSDT (28)
  AP_SSDT (29)
  AP_SSDT (30)
  AP_SSDT (31)
  AP_SSDT (32)
  AP_SSDT (33)
  AP_SSDT (34)
  AP_SSDT (35)
  AP_SSDT (36)
  AP_SSDT (37)
  AP_SSDT (38)
  AP_SSDT (39)
  AP_SSDT (40)
  AP_SSDT (41)
  AP_SSDT (42)
  AP_SSDT (43)
  AP_SSDT (44)
  AP_SSDT (45)
  AP_SSDT (46)
  AP_SSDT (47)
  AP_SSDT (48)
  AP_SSDT (49)
  AP_SSDT (50)
  AP_SSDT (51)
  AP_SSDT (52)
  AP_SSDT (53)
  AP_SSDT (54)
  AP_SSDT (55)
  AP_SSDT (56)
  AP_SSDT (57)
  AP_SSDT (58)
  AP_SSDT (59)
  AP_SSDT (60)
  AP_SSDT (61)
  AP_SSDT (62)
  AP_SSDT (63)

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
          Return ("INT_UNK_SINIT")
        } // End of Method (_CID)
      } // End of Device (ACM)
    }
  } // End of Scope(\_SB) for TxtAcm
} // End of Definition Block

#undef AP_SSDT
