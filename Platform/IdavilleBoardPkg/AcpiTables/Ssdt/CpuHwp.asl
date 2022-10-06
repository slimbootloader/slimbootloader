/** @file

  Copyright (c) 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Acpi/AcpiTablePlatform.h>

DefinitionBlock (
  "CPUHWP.aml",     // Output Filename
  "OEM2",           // Signature
  2,                // SSDT Revision
  "INTEL",          // OEMID
  "CPU  HWP",       // Table ID
  0x3000            // OEM Revision
  ) {

  External(SCKN, FieldUnitObj)

  #undef   ToName
  #define  ToName(x)  x##000
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##001
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##002
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##003
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##004
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##005
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##006
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##007
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##008
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##009
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##00A
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##00B
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##00C
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##00D
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##00E
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##00F
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##010
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##011
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##012
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##013
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##014
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##015
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##016
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##017
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##018
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##019
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##01A
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##01B
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##01C
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##01D
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##01E
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##01F
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##020
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##021
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##022
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##023
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##024
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##025
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##026
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##027
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##028
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##029
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##02A
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##02B
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##02C
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##02D
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##02E
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##02F
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##030
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##031
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##032
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##033
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##034
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##035
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##036
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##037
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##038
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##039
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##03A
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##03B
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##03C
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##03D
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##03E
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##03F
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##040
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##041
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##042
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##043
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##044
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##045
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##046
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##047
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##048
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##049
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##04A
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##04B
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##04C
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##04D
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##04E
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##04F
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##050
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##051
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##052
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##053
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##054
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##055
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##056
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##057
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##058
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##059
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##05A
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##05B
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##05C
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##05D
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##05E
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##05F
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##060
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##061
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##062
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##063
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##064
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##065
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##066
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##067
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##068
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##069
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##06A
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##06B
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##06C
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##06D
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##06E
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##06F
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##070
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##071
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##072
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##073
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##074
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##075
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##076
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##077
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##078
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##079
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##07A
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##07B
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##07C
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##07D
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##07E
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##07F
  #include "ProcHwp.asi"

}


