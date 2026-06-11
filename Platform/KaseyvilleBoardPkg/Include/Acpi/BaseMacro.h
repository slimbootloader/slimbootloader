/** @file
  Definitions shared with HFR/VFR files.

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _BASEMACRO_H_
#define _BASEMACRO_H_

#ifndef MAX
#define MAX(a,b)               (((a) > (b)) ? (a) : (b))
#endif
#ifndef MIN
#define MIN(a, b)              (((a) < (b)) ? (a) : (b))
#endif

#ifndef NELEMENTS
#define NELEMENTS(Array) (sizeof(Array)/sizeof((Array)[0]))
#endif

/**
 * The macros below shall be used when defining symbos shared with HFR and ASL files.
 */
//
// CONCATENATE2 allows to concatenate two tokens
//
#ifndef CONCATENATE2
#define CONCATENATE2_(a, b)    a ## b
#define CONCATENATE2(a, b)     CONCATENATE2_(a, b)
#endif

//
// CONCATENATE3 allows to concatenate three tokens
//
#ifndef CONCATENATE3
#define CONCATENATE3_(a, b, c) a ## b ## c
#define CONCATENATE3(a, b, c)  CONCATENATE3_(a, b, c)
#endif

//
// CONCATENATE4 allows to concatenate four tokens
//
#ifndef CONCATENATE4
#define CONCATENATE4_(a, b, c, d) a ## b  ## c ## d
#define CONCATENATE4(a, b, c, d) CONCATENATE4_(a, b, c, d)
#endif

//
// CONCATENATE5 allows to concatenate five tokens
//
#ifndef CONCATENATE5
#define CONCATENATE5_(a, b, c, d ,e) a ## b  ## c ## d ## e
#define CONCATENATE5(a, b, c, d ,e) CONCATENATE5_(a, b, c, d ,e)
#endif

//
// CONCATENATE6 allows to concatenate six tokens
//
#ifndef CONCATENATE6
#define CONCATENATE6_(a, b, c, d ,e, f) a ## b  ## c ## d ## e ## f
#define CONCATENATE6(a, b, c, d ,e, f) CONCATENATE6_(a, b, c, d ,e, f)
#endif

#endif // _BASEMACRO_H_
