/** @file

  Copyright (c) 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Acpi/AcpiTablePlatform.h>

DefinitionBlock (
  "CPUPM.aml",      // Output Filename
  "SSDT",           // Signature
  2,                // SSDT Revision
  "INTEL",          // OEMID
  "SSDT  PM",       // Table ID
  0x4000            // OEM Revision
  ) {

  External (ADBG, MethodObj)

  External (PSEN, FieldUnitObj)
  External (HWAL, FieldUnitObj)
  External (TSEN, FieldUnitObj)
  External (FGTS, FieldUnitObj)
  External (CSEN, FieldUnitObj)
  External (HWEN, FieldUnitObj) //HWPM

#if ME_SPS_SUPPORT
  //
  // SPS Node Manager defines current P-State & T-State limit
  // Objects PSLM and TSLM are referenced in _PPC and _TPC
  // PETE is a bitmask of P-states T-states support enabled in OS.
  //
  Name(PETE, 0)
  Name(PSLM, Package(){0,0,0,0,0,0,0,0}) // P-State limit per socket
  Name(TSLM, Package(){0,0,0,0,0,0,0,0}) // T-State limit per socket
#endif

    Name(HI1,0)    // Handle to EIST
    Name(HW1,0)    // Handle to HWP

    Name(SDTL,0x00)        // Loaded SSDT Flags.
    // SDTL[0]   = EIST SSDT Loaded
    // SDTL[1]   = HWP SSDT Loaded
    // SDTL[2]   = TST SSDT Loaded
    // SDTL[3]   = CST SSDT Loaded

  #undef   ToName
  #define  ToName(x)  x##000
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##001
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##002
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##003
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##004
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##005
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##006
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##007
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##008
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##009
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##00A
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##00B
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##00C
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##00D
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##00E
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##00F
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##010
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##011
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##012
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##013
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##014
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##015
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##016
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##017
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##018
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##019
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##01A
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##01B
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##01C
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##01D
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##01E
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##01F
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##020
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##021
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##022
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##023
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##024
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##025
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##026
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##027
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##028
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##029
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##02A
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##02B
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##02C
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##02D
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##02E
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##02F
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##030
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##031
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##032
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##033
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##034
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##035
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##036
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##037
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##038
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##039
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##03A
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##03B
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##03C
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##03D
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##03E
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##03F
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##040
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##041
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##042
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##043
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##044
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##045
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##046
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##047
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##048
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##049
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##04A
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##04B
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##04C
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##04D
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##04E
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##04F
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##050
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##051
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##052
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##053
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##054
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##055
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##056
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##057
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##058
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##059
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##05A
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##05B
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##05C
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##05D
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##05E
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##05F
  #include "ProcPm.asi"

    #undef   ToName
  #define  ToName(x)  x##060
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##061
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##062
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##063
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##064
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##065
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##066
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##067
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##068
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##069
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##06A
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##06B
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##06C
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##06D
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##06E
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##06F
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##070
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##071
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##072
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##073
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##074
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##075
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##076
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##077
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##078
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##079
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##07A
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##07B
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##07C
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##07D
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##07E
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##07F
  #include "ProcPm.asi"

}

