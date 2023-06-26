/** @file
  This file contains definitions of PCIE RP MACRO.

@copyright
  Copyright (c) 2021, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/
#ifndef _PCIE_RP_MACRO_DEFINITION_H
#define _PCIE_RP_MACRO_DEFINITION_H


//
// Note:- Macro Value 0 and 1 are treated as Boolean variable by ASL compiler.
// So avoiding using 0,1 as value in C Code. It will be helpful in avoiding
// the confusion during Runtime AML Patching.
//
#ifndef PCIE_RP_TYPE_UNKNOWN
#define PCIE_RP_TYPE_UNKNOWN              0x00
#endif

#ifndef PCIE_RP_TYPE_RESERVED
#define PCIE_RP_TYPE_RESERVED             0x01
#endif

// Defining MACRO Value for PCH PCIE RP Type.
#ifndef PCIE_RP_TYPE_PCH
#define PCIE_RP_TYPE_PCH                  0x02
#endif

// Defining MACRO Value for CPU PCIE RP Type.
#ifndef PCIE_RP_TYPE_CPU
#define PCIE_RP_TYPE_CPU                  0x04
#endif

// Defining MACRO Value for ITBT PCIE RP Type.
#ifndef PCIE_RP_TYPE_ITBT
#define PCIE_RP_TYPE_ITBT                 0x08
#endif


//
// Note:- Macro Value 0 and 1 are treated as Boolean variable by ASL compiler.
// So avoiding using 0,1 as value in C Code. It will be helpful in avoiding
// the confusion during Runtime AML Patching.
//
#ifndef PCIE_EP_TYPE_UNKNOWN
#define PCIE_EP_TYPE_UNKNOWN              0x00
#endif

#ifndef PCIE_EP_TYPE_RESERVED
#define PCIE_EP_TYPE_RESERVED             0x01
#endif

// Defining MACRO Value for Generic PCIE EP Type.
#ifndef PCIE_EP_TYPE_GENERIC_PCIE
#define PCIE_EP_TYPE_GENERIC_PCIE         0x02
#endif

// Defining MACRO Value for Discrete Thunderbolt EP Type.
#ifndef PCIE_EP_TYPE_DTBT
#define PCIE_EP_TYPE_DTBT                 0x04
#endif

// Defining MACRO Value for Invalid EP Type.
#ifndef PCIE_EP_TYPE_INVALID
#define PCIE_EP_TYPE_INVALID              0xFF
#endif

#endif // _PCIE_RP_MACRO_DEFINITION_H
