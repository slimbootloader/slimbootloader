/** @file

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
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

  External (PSEN, FieldUnitObj)
  External (HWAL, FieldUnitObj)
  External (CSEN, FieldUnitObj)
  External (HWEN, FieldUnitObj) //HWPM

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

  #undef   ToName
  #define  ToName(x)  x##080
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##081
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##082
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##083
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##084
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##085
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##086
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##087
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##088
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##089
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##08A
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##08B
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##08C
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##08D
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##08E
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##08F
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##090
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##091
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##092
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##093
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##094
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##095
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##096
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##097
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##098
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##099
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##09A
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##09B
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##09C
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##09D
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##09E
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##09F
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##0A0
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##0A1
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##0A2
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##0A3
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##0A4
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##0A5
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##0A6
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##0A7
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##0A8
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##0A9
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##0AA
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##0AB
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##0AC
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##0AD
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##0AE
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##0AF
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##0B0
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##0B1
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##0B2
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##0B3
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##0B4
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##0B5
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##0B6
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##0B7
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##0B8
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##0B9
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##0BA
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##0BB
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##0BC
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##0BD
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##0BE
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##0BF
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##0C0
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##0C1
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##0C2
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##0C3
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##0C4
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##0C5
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##0C6
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##0C7
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##0C8
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##0C9
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##0CA
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##0CB
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##0CC
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##0CD
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##0CE
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##0CF
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##0D0
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##0D1
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##0D2
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##0D3
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##0D4
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##0D5
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##0D6
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##0D7
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##0D8
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##0D9
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##0DA
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##0DB
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##0DC
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##0DD
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##0DE
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##0DF
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##0E0
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##0E1
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##0E2
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##0E3
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##0E4
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##0E5
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##0E6
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##0E7
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##0E8
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##0E9
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##0EA
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##0EB
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##0EC
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##0ED
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##0EE
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##0EF
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##0F0
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##0F1
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##0F2
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##0F3
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##0F4
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##0F5
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##0F6
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##0F7
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##0F8
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##0F9
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##0FA
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##0FB
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##0FC
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##0FD
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##0FE
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##0FF
  #include "ProcPm.asi"


  #undef   ToName
  #define  ToName(x)  x##100
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##101
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##102
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##103
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##104
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##105
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##106
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##107
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##108
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##109
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##10A
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##10B
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##10C
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##10D
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##10E
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##10F
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##110
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##111
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##112
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##113
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##114
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##115
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##116
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##117
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##118
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##119
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##11A
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##11B
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##11C
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##11D
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##11E
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##11F
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##120
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##121
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##122
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##123
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##124
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##125
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##126
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##127
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##128
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##129
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##12A
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##12B
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##12C
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##12D
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##12E
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##12F
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##130
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##131
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##132
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##133
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##134
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##135
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##136
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##137
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##138
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##139
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##13A
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##13B
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##13C
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##13D
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##13E
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##13F
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##140
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##141
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##142
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##143
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##144
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##145
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##146
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##147
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##148
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##149
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##14A
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##14B
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##14C
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##14D
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##14E
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##14F
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##150
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##151
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##152
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##153
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##154
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##155
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##156
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##157
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##158
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##159
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##15A
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##15B
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##15C
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##15D
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##15E
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##15F
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##160
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##161
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##162
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##163
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##164
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##165
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##166
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##167
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##168
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##169
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##16A
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##16B
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##16C
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##16D
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##16E
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##16F
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##170
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##171
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##172
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##173
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##174
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##175
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##176
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##177
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##178
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##179
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##17A
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##17B
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##17C
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##17D
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##17E
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##17F
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##180
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##181
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##182
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##183
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##184
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##185
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##186
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##187
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##188
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##189
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##18A
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##18B
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##18C
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##18D
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##18E
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##18F
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##190
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##191
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##192
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##193
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##194
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##195
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##196
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##197
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##198
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##199
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##19A
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##19B
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##19C
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##19D
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##19E
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##19F
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##1A0
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##1A1
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##1A2
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##1A3
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##1A4
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##1A5
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##1A6
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##1A7
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##1A8
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##1A9
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##1AA
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##1AB
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##1AC
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##1AD
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##1AE
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##1AF
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##1B0
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##1B1
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##1B2
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##1B3
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##1B4
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##1B5
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##1B6
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##1B7
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##1B8
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##1B9
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##1BA
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##1BB
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##1BC
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##1BD
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##1BE
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##1BF
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##1C0
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##1C1
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##1C2
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##1C3
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##1C4
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##1C5
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##1C6
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##1C7
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##1C8
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##1C9
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##1CA
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##1CB
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##1CC
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##1CD
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##1CE
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##1CF
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##1D0
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##1D1
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##1D2
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##1D3
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##1D4
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##1D5
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##1D6
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##1D7
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##1D8
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##1D9
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##1DA
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##1DB
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##1DC
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##1DD
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##1DE
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##1DF
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##1E0
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##1E1
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##1E2
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##1E3
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##1E4
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##1E5
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##1E6
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##1E7
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##1E8
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##1E9
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##1EA
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##1EB
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##1EC
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##1ED
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##1EE
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##1EF
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##1F0
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##1F1
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##1F2
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##1F3
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##1F4
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##1F5
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##1F6
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##1F7
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##1F8
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##1F9
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##1FA
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##1FB
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##1FC
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##1FD
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##1FE
  #include "ProcPm.asi"

  #undef   ToName
  #define  ToName(x)  x##1FF
  #include "ProcPm.asi"
}
