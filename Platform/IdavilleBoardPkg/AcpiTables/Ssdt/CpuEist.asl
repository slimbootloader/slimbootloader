/** @file

  Copyright (c) 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Acpi/AcpiTablePlatform.h>

DefinitionBlock (
  "CPUEIST.aml",    // Output Filename
  "OEM1",           // Signature
  2,                // SSDT Revision
  "INTEL",          // OEMID
  "CPU EIST",       // Table ID
  0x3000            // OEM Revision
  ) {

  External (PSEN, FieldUnitObj)
  External (HWAL, FieldUnitObj)
  External (TSEN, FieldUnitObj)
  External (FGTS, FieldUnitObj)
  External (HWEN, FieldUnitObj) //HWPM

  External (TYPE, FieldUnitObj)
  External (PSDC, PkgObj)
  External (PSDD, PkgObj)
  External (PSDE, PkgObj)
  External (NPSS, PkgObj)

  External(PETE, IntObj)
  External (PSLM, PkgObj)

  #undef   ToName
  #define  ToName(x)  x##000
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##001
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##002
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##003
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##004
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##005
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##006
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##007
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##008
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##009
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##00A
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##00B
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##00C
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##00D
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##00E
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##00F
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##010
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##011
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##012
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##013
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##014
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##015
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##016
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##017
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##018
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##019
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##01A
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##01B
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##01C
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##01D
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##01E
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##01F
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##020
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##021
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##022
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##023
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##024
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##025
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##026
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##027
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##028
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##029
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##02A
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##02B
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##02C
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##02D
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##02E
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##02F
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##030
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##031
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##032
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##033
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##034
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##035
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##036
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##037
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##038
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##039
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##03A
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##03B
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##03C
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##03D
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##03E
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##03F
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##040
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##041
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##042
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##043
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##044
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##045
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##046
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##047
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##048
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##049
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##04A
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##04B
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##04C
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##04D
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##04E
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##04F
  #include "ProcEist.asi"


  #undef   ToName
  #define  ToName(x)  x##050
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##051
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##052
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##053
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##054
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##055
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##056
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##057
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##058
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##059
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##05A
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##05B
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##05C
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##05D
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##05E
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##05F
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##060
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##061
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##062
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##063
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##064
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##065
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##066
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##067
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##068
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##069
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##06A
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##06B
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##06C
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##06D
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##06E
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##06F
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##070
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##071
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##072
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##073
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##074
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##075
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##076
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##077
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##078
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##079
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##07A
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##07B
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##07C
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##07D
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##07E
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##07F
  #include "ProcEist.asi"

}

