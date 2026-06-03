/** @file

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
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

  #undef   ToName
  #define  ToName(x)  x##080
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##081
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##082
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##083
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##084
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##085
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##086
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##087
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##088
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##089
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##08A
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##08B
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##08C
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##08D
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##08E
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##08F
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##090
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##091
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##092
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##093
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##094
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##095
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##096
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##097
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##098
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##099
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##09A
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##09B
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##09C
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##09D
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##09E
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##09F
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##0A0
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##0A1
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##0A2
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##0A3
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##0A4
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##0A5
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##0A6
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##0A7
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##0A8
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##0A9
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##0AA
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##0AB
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##0AC
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##0AD
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##0AE
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##0AF
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##0B0
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##0B1
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##0B2
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##0B3
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##0B4
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##0B5
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##0B6
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##0B7
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##0B8
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##0B9
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##0BA
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##0BB
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##0BC
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##0BD
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##0BE
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##0BF
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##0C0
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##0C1
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##0C2
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##0C3
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##0C4
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##0C5
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##0C6
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##0C7
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##0C8
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##0C9
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##0CA
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##0CB
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##0CC
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##0CD
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##0CE
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##0CF
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##0D0
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##0D1
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##0D2
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##0D3
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##0D4
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##0D5
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##0D6
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##0D7
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##0D8
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##0D9
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##0DA
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##0DB
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##0DC
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##0DD
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##0DE
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##0DF
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##0E0
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##0E1
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##0E2
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##0E3
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##0E4
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##0E5
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##0E6
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##0E7
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##0E8
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##0E9
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##0EA
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##0EB
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##0EC
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##0ED
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##0EE
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##0EF
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##0F0
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##0F1
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##0F2
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##0F3
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##0F4
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##0F5
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##0F6
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##0F7
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##0F8
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##0F9
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##0FA
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##0FB
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##0FC
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##0FD
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##0FE
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##0FF
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##100
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##101
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##102
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##103
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##104
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##105
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##106
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##107
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##108
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##109
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##10A
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##10B
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##10C
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##10D
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##10E
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##10F
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##110
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##111
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##112
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##113
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##114
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##115
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##116
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##117
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##118
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##119
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##11A
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##11B
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##11C
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##11D
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##11E
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##11F
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##120
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##121
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##122
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##123
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##124
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##125
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##126
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##127
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##128
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##129
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##12A
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##12B
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##12C
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##12D
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##12E
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##12F
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##130
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##131
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##132
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##133
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##134
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##135
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##136
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##137
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##138
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##139
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##13A
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##13B
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##13C
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##13D
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##13E
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##13F
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##140
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##141
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##142
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##143
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##144
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##145
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##146
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##147
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##148
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##149
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##14A
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##14B
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##14C
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##14D
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##14E
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##14F
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##150
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##151
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##152
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##153
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##154
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##155
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##156
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##157
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##158
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##159
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##15A
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##15B
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##15C
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##15D
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##15E
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##15F
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##160
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##161
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##162
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##163
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##164
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##165
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##166
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##167
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##168
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##169
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##16A
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##16B
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##16C
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##16D
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##16E
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##16F
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##170
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##171
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##172
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##173
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##174
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##175
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##176
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##177
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##178
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##179
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##17A
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##17B
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##17C
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##17D
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##17E
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##17F
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##180
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##181
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##182
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##183
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##184
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##185
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##186
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##187
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##188
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##189
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##18A
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##18B
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##18C
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##18D
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##18E
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##18F
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##190
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##191
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##192
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##193
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##194
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##195
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##196
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##197
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##198
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##199
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##19A
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##19B
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##19C
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##19D
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##19E
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##19F
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##1A0
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##1A1
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##1A2
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##1A3
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##1A4
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##1A5
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##1A6
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##1A7
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##1A8
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##1A9
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##1AA
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##1AB
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##1AC
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##1AD
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##1AE
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##1AF
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##1B0
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##1B1
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##1B2
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##1B3
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##1B4
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##1B5
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##1B6
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##1B7
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##1B8
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##1B9
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##1BA
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##1BB
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##1BC
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##1BD
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##1BE
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##1BF
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##1C0
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##1C1
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##1C2
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##1C3
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##1C4
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##1C5
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##1C6
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##1C7
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##1C8
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##1C9
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##1CA
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##1CB
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##1CC
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##1CD
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##1CE
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##1CF
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##1D0
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##1D1
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##1D2
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##1D3
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##1D4
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##1D5
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##1D6
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##1D7
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##1D8
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##1D9
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##1DA
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##1DB
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##1DC
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##1DD
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##1DE
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##1DF
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##1E0
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##1E1
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##1E2
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##1E3
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##1E4
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##1E5
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##1E6
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##1E7
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##1E8
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##1E9
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##1EA
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##1EB
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##1EC
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##1ED
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##1EE
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##1EF
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##1F0
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##1F1
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##1F2
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##1F3
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##1F4
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##1F5
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##1F6
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##1F7
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##1F8
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##1F9
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##1FA
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##1FB
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##1FC
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##1FD
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##1FE
  #include "ProcHwp.asi"

  #undef   ToName
  #define  ToName(x)  x##1FF
  #include "ProcHwp.asi"
}
