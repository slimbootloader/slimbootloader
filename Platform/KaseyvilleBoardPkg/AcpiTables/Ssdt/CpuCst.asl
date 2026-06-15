/** @file

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Acpi/AcpiTablePlatform.h>

#define  PCH_ACPI_PORT_C3_ADDR                0x514
#define  PCH_ACPI_PORT_C6_ADDR                0x515
#define  PCH_ACPI_PORT_C7_ADDR                0x516

DefinitionBlock (
  "CPUCST.aml",     // Output Filename
  "OEM4",           // Signature
  2,                // SSDT Revision
  "INTEL",          // OEMID
  "CPU  CST",       // Table ID
  0x3000            // OEM Revision
  ) {

  External (HWEN, FieldUnitObj) //HWPM
  External (PSEN, FieldUnitObj)
  External (SCKN, FieldUnitObj)

  #undef   ToName
  #define  ToName(x)  x##000
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##001
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##002
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##003
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##004
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##005
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##006
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##007
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##008
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##009
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##00A
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##00B
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##00C
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##00D
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##00E
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##00F
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##010
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##011
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##012
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##013
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##014
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##015
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##016
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##017
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##018
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##019
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##01A
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##01B
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##01C
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##01D
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##01E
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##01F
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##020
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##021
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##022
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##023
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##024
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##025
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##026
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##027
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##028
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##029
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##02A
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##02B
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##02C
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##02D
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##02E
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##02F
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##030
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##031
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##032
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##033
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##034
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##035
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##036
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##037
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##038
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##039
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##03A
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##03B
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##03C
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##03D
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##03E
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##03F
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##040
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##041
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##042
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##043
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##044
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##045
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##046
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##047
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##048
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##049
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##04A
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##04B
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##04C
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##04D
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##04E
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##04F
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##050
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##051
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##052
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##053
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##054
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##055
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##056
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##057
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##058
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##059
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##05A
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##05B
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##05C
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##05D
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##05E
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##05F
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##060
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##061
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##062
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##063
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##064
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##065
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##066
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##067
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##068
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##069
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##06A
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##06B
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##06C
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##06D
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##06E
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##06F
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##070
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##071
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##072
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##073
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##074
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##075
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##076
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##077
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##078
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##079
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##07A
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##07B
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##07C
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##07D
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##07E
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##07F
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##080
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##081
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##082
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##083
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##084
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##085
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##086
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##087
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##088
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##089
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##08A
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##08B
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##08C
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##08D
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##08E
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##08F
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##090
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##091
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##092
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##093
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##094
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##095
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##096
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##097
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##098
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##099
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##09A
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##09B
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##09C
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##09D
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##09E
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##09F
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##0A0
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##0A1
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##0A2
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##0A3
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##0A4
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##0A5
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##0A6
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##0A7
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##0A8
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##0A9
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##0AA
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##0AB
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##0AC
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##0AD
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##0AE
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##0AF
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##0B0
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##0B1
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##0B2
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##0B3
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##0B4
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##0B5
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##0B6
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##0B7
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##0B8
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##0B9
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##0BA
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##0BB
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##0BC
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##0BD
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##0BE
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##0BF
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##0C0
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##0C1
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##0C2
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##0C3
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##0C4
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##0C5
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##0C6
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##0C7
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##0C8
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##0C9
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##0CA
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##0CB
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##0CC
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##0CD
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##0CE
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##0CF
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##0D0
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##0D1
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##0D2
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##0D3
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##0D4
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##0D5
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##0D6
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##0D7
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##0D8
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##0D9
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##0DA
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##0DB
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##0DC
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##0DD
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##0DE
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##0DF
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##0E0
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##0E1
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##0E2
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##0E3
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##0E4
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##0E5
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##0E6
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##0E7
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##0E8
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##0E9
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##0EA
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##0EB
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##0EC
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##0ED
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##0EE
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##0EF
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##0F0
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##0F1
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##0F2
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##0F3
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##0F4
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##0F5
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##0F6
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##0F7
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##0F8
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##0F9
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##0FA
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##0FB
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##0FC
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##0FD
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##0FE
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##0FF
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##100
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##101
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##102
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##103
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##104
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##105
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##106
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##107
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##108
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##109
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##10A
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##10B
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##10C
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##10D
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##10E
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##10F
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##110
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##111
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##112
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##113
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##114
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##115
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##116
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##117
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##118
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##119
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##11A
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##11B
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##11C
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##11D
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##11E
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##11F
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##120
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##121
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##122
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##123
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##124
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##125
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##126
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##127
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##128
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##129
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##12A
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##12B
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##12C
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##12D
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##12E
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##12F
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##130
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##131
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##132
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##133
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##134
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##135
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##136
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##137
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##138
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##139
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##13A
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##13B
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##13C
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##13D
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##13E
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##13F
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##140
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##141
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##142
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##143
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##144
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##145
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##146
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##147
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##148
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##149
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##14A
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##14B
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##14C
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##14D
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##14E
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##14F
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##150
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##151
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##152
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##153
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##154
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##155
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##156
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##157
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##158
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##159
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##15A
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##15B
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##15C
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##15D
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##15E
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##15F
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##160
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##161
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##162
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##163
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##164
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##165
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##166
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##167
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##168
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##169
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##16A
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##16B
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##16C
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##16D
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##16E
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##16F
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##170
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##171
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##172
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##173
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##174
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##175
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##176
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##177
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##178
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##179
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##17A
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##17B
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##17C
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##17D
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##17E
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##17F
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##180
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##181
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##182
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##183
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##184
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##185
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##186
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##187
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##188
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##189
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##18A
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##18B
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##18C
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##18D
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##18E
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##18F
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##190
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##191
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##192
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##193
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##194
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##195
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##196
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##197
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##198
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##199
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##19A
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##19B
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##19C
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##19D
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##19E
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##19F
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##1A0
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##1A1
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##1A2
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##1A3
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##1A4
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##1A5
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##1A6
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##1A7
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##1A8
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##1A9
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##1AA
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##1AB
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##1AC
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##1AD
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##1AE
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##1AF
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##1B0
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##1B1
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##1B2
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##1B3
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##1B4
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##1B5
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##1B6
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##1B7
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##1B8
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##1B9
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##1BA
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##1BB
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##1BC
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##1BD
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##1BE
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##1BF
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##1C0
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##1C1
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##1C2
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##1C3
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##1C4
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##1C5
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##1C6
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##1C7
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##1C8
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##1C9
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##1CA
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##1CB
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##1CC
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##1CD
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##1CE
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##1CF
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##1D0
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##1D1
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##1D2
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##1D3
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##1D4
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##1D5
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##1D6
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##1D7
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##1D8
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##1D9
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##1DA
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##1DB
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##1DC
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##1DD
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##1DE
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##1DF
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##1E0
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##1E1
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##1E2
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##1E3
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##1E4
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##1E5
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##1E6
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##1E7
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##1E8
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##1E9
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##1EA
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##1EB
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##1EC
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##1ED
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##1EE
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##1EF
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##1F0
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##1F1
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##1F2
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##1F3
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##1F4
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##1F5
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##1F6
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##1F7
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##1F8
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##1F9
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##1FA
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##1FB
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##1FC
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##1FD
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##1FE
  #include "ProcCst.asi"

  #undef   ToName
  #define  ToName(x)  x##1FF
  #include "ProcCst.asi"
}
