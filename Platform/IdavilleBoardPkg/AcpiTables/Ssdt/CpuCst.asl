/** @file

  Copyright (c) 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Acpi/AcpiTablePlatform.h>

//
// Sync with Library\ProcMemInit\Platform\Include\PlatformHost.h
//
#define  PCH_ACPI_PORT_C3_ADDR                0x514
#define  PCH_ACPI_PORT_C6_ADDR                0x515
#define  PCH_ACPI_PORT_C7_ADDR                0x516

// Note:
// These tables are static as defined here. But they are patched during
// POST from UpdateTables () function in AcpiPlatformHooks.c  to make
// them dynamic and reflect the user configuration and capabilities for
// particular system boot and OS session

DefinitionBlock (
  "CPUCST.aml",     // Output Filename
  "OEM4",           // Signature
  2,                // SSDT Revision
  "INTEL",          // OEMID
  "CPU  CST",       // Table ID
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

}

