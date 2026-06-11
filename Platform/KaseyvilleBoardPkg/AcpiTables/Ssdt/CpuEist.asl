/** @file

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
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
  External (HWEN, FieldUnitObj) //HWPM

  External (TYPE, FieldUnitObj)
  External (PSDC, PkgObj)
  External (PSDD, PkgObj)
  External (PSDE, PkgObj)
  External (NPSS, PkgObj)


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

  #undef   ToName
  #define  ToName(x)  x##080
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##081
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##082
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##083
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##084
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##085
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##086
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##087
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##088
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##089
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##08A
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##08B
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##08C
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##08D
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##08E
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##08F
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##090
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##091
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##092
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##093
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##094
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##095
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##096
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##097
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##098
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##099
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##09A
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##09B
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##09C
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##09D
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##09E
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##09F
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##0A0
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##0A1
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##0A2
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##0A3
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##0A4
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##0A5
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##0A6
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##0A7
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##0A8
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##0A9
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##0AA
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##0AB
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##0AC
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##0AD
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##0AE
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##0AF
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##0B0
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##0B1
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##0B2
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##0B3
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##0B4
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##0B5
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##0B6
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##0B7
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##0B8
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##0B9
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##0BA
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##0BB
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##0BC
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##0BD
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##0BE
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##0BF
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##0C0
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##0C1
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##0C2
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##0C3
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##0C4
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##0C5
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##0C6
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##0C7
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##0C8
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##0C9
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##0CA
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##0CB
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##0CC
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##0CD
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##0CE
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##0CF
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##0D0
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##0D1
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##0D2
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##0D3
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##0D4
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##0D5
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##0D6
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##0D7
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##0D8
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##0D9
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##0DA
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##0DB
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##0DC
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##0DD
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##0DE
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##0DF
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##0E0
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##0E1
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##0E2
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##0E3
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##0E4
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##0E5
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##0E6
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##0E7
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##0E8
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##0E9
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##0EA
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##0EB
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##0EC
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##0ED
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##0EE
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##0EF
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##0F0
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##0F1
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##0F2
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##0F3
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##0F4
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##0F5
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##0F6
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##0F7
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##0F8
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##0F9
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##0FA
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##0FB
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##0FC
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##0FD
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##0FE
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##0FF
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##100
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##101
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##102
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##103
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##104
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##105
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##106
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##107
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##108
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##109
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##10A
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##10B
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##10C
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##10D
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##10E
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##10F
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##110
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##111
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##112
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##113
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##114
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##115
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##116
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##117
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##118
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##119
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##11A
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##11B
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##11C
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##11D
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##11E
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##11F
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##120
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##121
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##122
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##123
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##124
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##125
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##126
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##127
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##128
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##129
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##12A
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##12B
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##12C
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##12D
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##12E
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##12F
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##130
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##131
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##132
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##133
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##134
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##135
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##136
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##137
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##138
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##139
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##13A
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##13B
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##13C
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##13D
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##13E
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##13F
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##140
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##141
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##142
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##143
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##144
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##145
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##146
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##147
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##148
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##149
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##14A
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##14B
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##14C
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##14D
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##14E
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##14F
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##150
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##151
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##152
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##153
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##154
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##155
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##156
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##157
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##158
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##159
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##15A
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##15B
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##15C
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##15D
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##15E
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##15F
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##160
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##161
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##162
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##163
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##164
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##165
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##166
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##167
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##168
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##169
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##16A
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##16B
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##16C
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##16D
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##16E
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##16F
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##170
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##171
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##172
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##173
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##174
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##175
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##176
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##177
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##178
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##179
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##17A
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##17B
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##17C
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##17D
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##17E
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##17F
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##180
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##181
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##182
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##183
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##184
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##185
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##186
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##187
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##188
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##189
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##18A
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##18B
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##18C
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##18D
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##18E
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##18F
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##190
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##191
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##192
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##193
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##194
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##195
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##196
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##197
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##198
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##199
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##19A
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##19B
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##19C
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##19D
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##19E
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##19F
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##1A0
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##1A1
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##1A2
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##1A3
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##1A4
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##1A5
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##1A6
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##1A7
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##1A8
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##1A9
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##1AA
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##1AB
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##1AC
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##1AD
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##1AE
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##1AF
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##1B0
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##1B1
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##1B2
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##1B3
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##1B4
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##1B5
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##1B6
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##1B7
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##1B8
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##1B9
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##1BA
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##1BB
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##1BC
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##1BD
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##1BE
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##1BF
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##1C0
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##1C1
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##1C2
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##1C3
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##1C4
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##1C5
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##1C6
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##1C7
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##1C8
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##1C9
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##1CA
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##1CB
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##1CC
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##1CD
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##1CE
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##1CF
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##1D0
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##1D1
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##1D2
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##1D3
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##1D4
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##1D5
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##1D6
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##1D7
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##1D8
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##1D9
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##1DA
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##1DB
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##1DC
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##1DD
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##1DE
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##1DF
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##1E0
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##1E1
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##1E2
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##1E3
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##1E4
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##1E5
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##1E6
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##1E7
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##1E8
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##1E9
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##1EA
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##1EB
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##1EC
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##1ED
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##1EE
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##1EF
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##1F0
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##1F1
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##1F2
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##1F3
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##1F4
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##1F5
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##1F6
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##1F7
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##1F8
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##1F9
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##1FA
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##1FB
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##1FC
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##1FD
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##1FE
  #include "ProcEist.asi"

  #undef   ToName
  #define  ToName(x)  x##1FF
  #include "ProcEist.asi"
}
