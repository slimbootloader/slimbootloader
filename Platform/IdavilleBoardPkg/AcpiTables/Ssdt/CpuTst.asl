/** @file

  Copyright (c) 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Acpi/AcpiTablePlatform.h>

// Note:
// These tables are static as defined here. But they are patched during
// POST from UpdateTables () function in AcpiPlatformHooks.c  to make
// them dynamic and reflect the user configuration and capabilities for
// particular system boot and OS session

DefinitionBlock (
  "CPUTST.aml",     // Output Filename
  "OEM3",           // Signature
  2,                // SSDT Revision
  "INTEL",          // OEMID
  "CPU  TST",       // Table ID
  0x3000            // OEM Revision
  ) {

  External(PETE, IntObj)
  External(TSLM, PkgObj)
  External (HWEN, FieldUnitObj) //HWPM
  External (PSEN, FieldUnitObj)
  External (TSEN, FieldUnitObj)
  External (FGTS, FieldUnitObj)
  External (SCKN, FieldUnitObj)

  #undef   ToName
  #define  ToName(x)  x##000
  #include "ProcTst.asi"

  #undef   ToName
  #define  ToName(x)  x##001
  #include "ProcTst.asi"

  #undef   ToName
  #define  ToName(x)  x##002
  #include "ProcTst.asi"

  #undef   ToName
  #define  ToName(x)  x##003
  #include "ProcTst.asi"

  #undef   ToName
  #define  ToName(x)  x##004
  #include "ProcTst.asi"

  #undef   ToName
  #define  ToName(x)  x##005
  #include "ProcTst.asi"

  #undef   ToName
  #define  ToName(x)  x##006
  #include "ProcTst.asi"

  #undef   ToName
  #define  ToName(x)  x##007
  #include "ProcTst.asi"

  #undef   ToName
  #define  ToName(x)  x##008
  #include "ProcTst.asi"

  #undef   ToName
  #define  ToName(x)  x##009
  #include "ProcTst.asi"

  #undef   ToName
  #define  ToName(x)  x##00A
  #include "ProcTst.asi"

  #undef   ToName
  #define  ToName(x)  x##00B
  #include "ProcTst.asi"

  #undef   ToName
  #define  ToName(x)  x##00C
  #include "ProcTst.asi"

  #undef   ToName
  #define  ToName(x)  x##00D
  #include "ProcTst.asi"

  #undef   ToName
  #define  ToName(x)  x##00E
  #include "ProcTst.asi"

  #undef   ToName
  #define  ToName(x)  x##00F
  #include "ProcTst.asi"

  #undef   ToName
  #define  ToName(x)  x##010
  #include "ProcTst.asi"

  #undef   ToName
  #define  ToName(x)  x##011
  #include "ProcTst.asi"

  #undef   ToName
  #define  ToName(x)  x##012
  #include "ProcTst.asi"

  #undef   ToName
  #define  ToName(x)  x##013
  #include "ProcTst.asi"

  #undef   ToName
  #define  ToName(x)  x##014
  #include "ProcTst.asi"

  #undef   ToName
  #define  ToName(x)  x##015
  #include "ProcTst.asi"

  #undef   ToName
  #define  ToName(x)  x##016
  #include "ProcTst.asi"

  #undef   ToName
  #define  ToName(x)  x##017
  #include "ProcTst.asi"

  #undef   ToName
  #define  ToName(x)  x##018
  #include "ProcTst.asi"

  #undef   ToName
  #define  ToName(x)  x##019
  #include "ProcTst.asi"

  #undef   ToName
  #define  ToName(x)  x##01A
  #include "ProcTst.asi"

  #undef   ToName
  #define  ToName(x)  x##01B
  #include "ProcTst.asi"

  #undef   ToName
  #define  ToName(x)  x##01C
  #include "ProcTst.asi"

  #undef   ToName
  #define  ToName(x)  x##01D
  #include "ProcTst.asi"

  #undef   ToName
  #define  ToName(x)  x##01E
  #include "ProcTst.asi"

  #undef   ToName
  #define  ToName(x)  x##01F
  #include "ProcTst.asi"

  #undef   ToName
  #define  ToName(x)  x##020
  #include "ProcTst.asi"

  #undef   ToName
  #define  ToName(x)  x##021
  #include "ProcTst.asi"

  #undef   ToName
  #define  ToName(x)  x##022
  #include "ProcTst.asi"

  #undef   ToName
  #define  ToName(x)  x##023
  #include "ProcTst.asi"

  #undef   ToName
  #define  ToName(x)  x##024
  #include "ProcTst.asi"

  #undef   ToName
  #define  ToName(x)  x##025
  #include "ProcTst.asi"

  #undef   ToName
  #define  ToName(x)  x##026
  #include "ProcTst.asi"

  #undef   ToName
  #define  ToName(x)  x##027
  #include "ProcTst.asi"

  #undef   ToName
  #define  ToName(x)  x##028
  #include "ProcTst.asi"

  #undef   ToName
  #define  ToName(x)  x##029
  #include "ProcTst.asi"

  #undef   ToName
  #define  ToName(x)  x##02A
  #include "ProcTst.asi"

  #undef   ToName
  #define  ToName(x)  x##02B
  #include "ProcTst.asi"

  #undef   ToName
  #define  ToName(x)  x##02C
  #include "ProcTst.asi"

  #undef   ToName
  #define  ToName(x)  x##02D
  #include "ProcTst.asi"

  #undef   ToName
  #define  ToName(x)  x##02E
  #include "ProcTst.asi"

  #undef   ToName
  #define  ToName(x)  x##02F
  #include "ProcTst.asi"

  #undef   ToName
  #define  ToName(x)  x##030
  #include "ProcTst.asi"

  #undef   ToName
  #define  ToName(x)  x##031
  #include "ProcTst.asi"

  #undef   ToName
  #define  ToName(x)  x##032
  #include "ProcTst.asi"

  #undef   ToName
  #define  ToName(x)  x##033
  #include "ProcTst.asi"

  #undef   ToName
  #define  ToName(x)  x##034
  #include "ProcTst.asi"

  #undef   ToName
  #define  ToName(x)  x##035
  #include "ProcTst.asi"

  #undef   ToName
  #define  ToName(x)  x##036
  #include "ProcTst.asi"

  #undef   ToName
  #define  ToName(x)  x##037
  #include "ProcTst.asi"

  #undef   ToName
  #define  ToName(x)  x##038
  #include "ProcTst.asi"

  #undef   ToName
  #define  ToName(x)  x##039
  #include "ProcTst.asi"

  #undef   ToName
  #define  ToName(x)  x##03A
  #include "ProcTst.asi"

  #undef   ToName
  #define  ToName(x)  x##03B
  #include "ProcTst.asi"

  #undef   ToName
  #define  ToName(x)  x##03C
  #include "ProcTst.asi"

  #undef   ToName
  #define  ToName(x)  x##03D
  #include "ProcTst.asi"

  #undef   ToName
  #define  ToName(x)  x##03E
  #include "ProcTst.asi"

  #undef   ToName
  #define  ToName(x)  x##03F
  #include "ProcTst.asi"

  #undef   ToName
  #define  ToName(x)  x##040
  #include "ProcTst.asi"

  #undef   ToName
  #define  ToName(x)  x##041
  #include "ProcTst.asi"

  #undef   ToName
  #define  ToName(x)  x##042
  #include "ProcTst.asi"

  #undef   ToName
  #define  ToName(x)  x##043
  #include "ProcTst.asi"

    #undef   ToName
  #define  ToName(x)  x##044
  #include "ProcTst.asi"

  #undef   ToName
  #define  ToName(x)  x##045
  #include "ProcTst.asi"

  #undef   ToName
  #define  ToName(x)  x##046
  #include "ProcTst.asi"

  #undef   ToName
  #define  ToName(x)  x##047
  #include "ProcTst.asi"

  #undef   ToName
  #define  ToName(x)  x##048
  #include "ProcTst.asi"

  #undef   ToName
  #define  ToName(x)  x##049
  #include "ProcTst.asi"

  #undef   ToName
  #define  ToName(x)  x##04A
  #include "ProcTst.asi"

  #undef   ToName
  #define  ToName(x)  x##04B
  #include "ProcTst.asi"

  #undef   ToName
  #define  ToName(x)  x##04C
  #include "ProcTst.asi"

  #undef   ToName
  #define  ToName(x)  x##04D
  #include "ProcTst.asi"

  #undef   ToName
  #define  ToName(x)  x##04E
  #include "ProcTst.asi"

  #undef   ToName
  #define  ToName(x)  x##04F
  #include "ProcTst.asi"

  #undef   ToName
  #define  ToName(x)  x##050
  #include "ProcTst.asi"

  #undef   ToName
  #define  ToName(x)  x##051
  #include "ProcTst.asi"

  #undef   ToName
  #define  ToName(x)  x##052
  #include "ProcTst.asi"

  #undef   ToName
  #define  ToName(x)  x##053
  #include "ProcTst.asi"

  #undef   ToName
  #define  ToName(x)  x##054
  #include "ProcTst.asi"

  #undef   ToName
  #define  ToName(x)  x##055
  #include "ProcTst.asi"

  #undef   ToName
  #define  ToName(x)  x##056
  #include "ProcTst.asi"

  #undef   ToName
  #define  ToName(x)  x##057
  #include "ProcTst.asi"

  #undef   ToName
  #define  ToName(x)  x##058
  #include "ProcTst.asi"

  #undef   ToName
  #define  ToName(x)  x##059
  #include "ProcTst.asi"

  #undef   ToName
  #define  ToName(x)  x##05A
  #include "ProcTst.asi"

  #undef   ToName
  #define  ToName(x)  x##05B
  #include "ProcTst.asi"

  #undef   ToName
  #define  ToName(x)  x##05C
  #include "ProcTst.asi"

  #undef   ToName
  #define  ToName(x)  x##05D
  #include "ProcTst.asi"

  #undef   ToName
  #define  ToName(x)  x##05E
  #include "ProcTst.asi"

  #undef   ToName
  #define  ToName(x)  x##05F
  #include "ProcTst.asi"

  #undef   ToName
  #define  ToName(x)  x##060
  #include "ProcTst.asi"

  #undef   ToName
  #define  ToName(x)  x##061
  #include "ProcTst.asi"

  #undef   ToName
  #define  ToName(x)  x##062
  #include "ProcTst.asi"

  #undef   ToName
  #define  ToName(x)  x##063
  #include "ProcTst.asi"

  #undef   ToName
  #define  ToName(x)  x##064
  #include "ProcTst.asi"

  #undef   ToName
  #define  ToName(x)  x##065
  #include "ProcTst.asi"

  #undef   ToName
  #define  ToName(x)  x##066
  #include "ProcTst.asi"

  #undef   ToName
  #define  ToName(x)  x##067
  #include "ProcTst.asi"

  #undef   ToName
  #define  ToName(x)  x##068
  #include "ProcTst.asi"

  #undef   ToName
  #define  ToName(x)  x##069
  #include "ProcTst.asi"

  #undef   ToName
  #define  ToName(x)  x##06A
  #include "ProcTst.asi"

  #undef   ToName
  #define  ToName(x)  x##06B
  #include "ProcTst.asi"

  #undef   ToName
  #define  ToName(x)  x##06C
  #include "ProcTst.asi"

  #undef   ToName
  #define  ToName(x)  x##06D
  #include "ProcTst.asi"

  #undef   ToName
  #define  ToName(x)  x##06E
  #include "ProcTst.asi"

  #undef   ToName
  #define  ToName(x)  x##06F
  #include "ProcTst.asi"

  #undef   ToName
  #define  ToName(x)  x##070
  #include "ProcTst.asi"

  #undef   ToName
  #define  ToName(x)  x##071
  #include "ProcTst.asi"

  #undef   ToName
  #define  ToName(x)  x##072
  #include "ProcTst.asi"

  #undef   ToName
  #define  ToName(x)  x##073
  #include "ProcTst.asi"

  #undef   ToName
  #define  ToName(x)  x##074
  #include "ProcTst.asi"

  #undef   ToName
  #define  ToName(x)  x##075
  #include "ProcTst.asi"

  #undef   ToName
  #define  ToName(x)  x##076
  #include "ProcTst.asi"

  #undef   ToName
  #define  ToName(x)  x##077
  #include "ProcTst.asi"

  #undef   ToName
  #define  ToName(x)  x##078
  #include "ProcTst.asi"

  #undef   ToName
  #define  ToName(x)  x##079
  #include "ProcTst.asi"

  #undef   ToName
  #define  ToName(x)  x##07A
  #include "ProcTst.asi"

  #undef   ToName
  #define  ToName(x)  x##07B
  #include "ProcTst.asi"

  #undef   ToName
  #define  ToName(x)  x##07C
  #include "ProcTst.asi"

  #undef   ToName
  #define  ToName(x)  x##07D
  #include "ProcTst.asi"

  #undef   ToName
  #define  ToName(x)  x##07E
  #include "ProcTst.asi"

  #undef   ToName
  #define  ToName(x)  x##07F
  #include "ProcTst.asi"

 }

