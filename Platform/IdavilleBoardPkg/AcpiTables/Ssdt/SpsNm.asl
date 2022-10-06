/** @file

  Copyright (c) 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Acpi/AcpiTablePlatform.h>

#define POWER_METER          1 // Can temporary disable Power Meter if needed
#define PROCESSOR_AGGREGATOR 1 // Can temporary disable Processor Aggregator if needed

#if 0
#define DbgTrace(a) Store(a, Debug)
#else
#define DbgTrace(a)
#endif

#define OSPM_READY     0x0F
#define OSPM_READY_ACK 0x8F

#define Notify80Socket0()  Notify(\_SB.SCK0.C000, 0x80)  \
                           Notify(\_SB.SCK0.C001, 0x80)  \
                           Notify(\_SB.SCK0.C002, 0x80)  \
                           Notify(\_SB.SCK0.C003, 0x80)  \
                           Notify(\_SB.SCK0.C004, 0x80)  \
                           Notify(\_SB.SCK0.C005, 0x80)  \
                           Notify(\_SB.SCK0.C006, 0x80)  \
                           Notify(\_SB.SCK0.C007, 0x80)  \
                           Notify(\_SB.SCK0.C008, 0x80)  \
                           Notify(\_SB.SCK0.C009, 0x80)  \
                           Notify(\_SB.SCK0.C00A, 0x80)  \
                           Notify(\_SB.SCK0.C00B, 0x80)  \
                           Notify(\_SB.SCK0.C00C, 0x80)  \
                           Notify(\_SB.SCK0.C00D, 0x80)  \
                           Notify(\_SB.SCK0.C00E, 0x80)  \
                           Notify(\_SB.SCK0.C00F, 0x80)  \
                           Notify(\_SB.SCK0.C010, 0x80)  \
                           Notify(\_SB.SCK0.C011, 0x80)  \
                           Notify(\_SB.SCK0.C012, 0x80)  \
                           Notify(\_SB.SCK0.C013, 0x80)  \
                           Notify(\_SB.SCK0.C014, 0x80)  \
                           Notify(\_SB.SCK0.C015, 0x80)  \
                           Notify(\_SB.SCK0.C016, 0x80)  \
                           Notify(\_SB.SCK0.C017, 0x80)  \
                           Notify(\_SB.SCK0.C018, 0x80)  \
                           Notify(\_SB.SCK0.C019, 0x80)  \
                           Notify(\_SB.SCK0.C01A, 0x80)  \
                           Notify(\_SB.SCK0.C01B, 0x80)  \
                           Notify(\_SB.SCK0.C01C, 0x80)  \
                           Notify(\_SB.SCK0.C01D, 0x80)  \
                           Notify(\_SB.SCK0.C01E, 0x80)  \
                           Notify(\_SB.SCK0.C01F, 0x80)  \
                           Notify(\_SB.SCK0.C020, 0x80)  \
                           Notify(\_SB.SCK0.C021, 0x80)  \
                           Notify(\_SB.SCK0.C022, 0x80)  \
                           Notify(\_SB.SCK0.C023, 0x80)  \
                           Notify(\_SB.SCK0.C024, 0x80)  \
                           Notify(\_SB.SCK0.C025, 0x80)  \
                           Notify(\_SB.SCK0.C026, 0x80)  \
                           Notify(\_SB.SCK0.C027, 0x80)  \
                           Notify(\_SB.SCK0.C028, 0x80)  \
                           Notify(\_SB.SCK0.C029, 0x80)  \
                           Notify(\_SB.SCK0.C02A, 0x80)  \
                           Notify(\_SB.SCK0.C02B, 0x80)  \
                           Notify(\_SB.SCK0.C02C, 0x80)  \
                           Notify(\_SB.SCK0.C02D, 0x80)  \
                           Notify(\_SB.SCK0.C02E, 0x80)  \
                           Notify(\_SB.SCK0.C02F, 0x80)  \
                           Notify(\_SB.SCK0.C030, 0x80)  \
                           Notify(\_SB.SCK0.C031, 0x80)  \
                           Notify(\_SB.SCK0.C032, 0x80)  \
                           Notify(\_SB.SCK0.C033, 0x80)  \
                           Notify(\_SB.SCK0.C034, 0x80)  \
                           Notify(\_SB.SCK0.C035, 0x80)  \
                           Notify(\_SB.SCK0.C036, 0x80)  \
                           Notify(\_SB.SCK0.C037, 0x80)  \
                           Notify(\_SB.SCK0.C038, 0x80)  \
                           Notify(\_SB.SCK0.C039, 0x80)  \
                           Notify(\_SB.SCK0.C03A, 0x80)  \
                           Notify(\_SB.SCK0.C03B, 0x80)  \
                           Notify(\_SB.SCK0.C03C, 0x80)  \
                           Notify(\_SB.SCK0.C03D, 0x80)  \
                           Notify(\_SB.SCK0.C03E, 0x80)  \
                           Notify(\_SB.SCK0.C03F, 0x80)  \
                           Notify(\_SB.SCK0.C040, 0x80)  \
                           Notify(\_SB.SCK0.C041, 0x80)  \
                           Notify(\_SB.SCK0.C042, 0x80)  \
                           Notify(\_SB.SCK0.C043, 0x80)  \
                           Notify(\_SB.SCK0.C044, 0x80)  \
                           Notify(\_SB.SCK0.C045, 0x80)  \
                           Notify(\_SB.SCK0.C046, 0x80)  \
                           Notify(\_SB.SCK0.C047, 0x80)  \
                           Notify(\_SB.SCK0.C048, 0x80)  \
                           Notify(\_SB.SCK0.C049, 0x80)  \
                           Notify(\_SB.SCK0.C04A, 0x80)  \
                           Notify(\_SB.SCK0.C04B, 0x80)  \
                           Notify(\_SB.SCK0.C04C, 0x80)  \
                           Notify(\_SB.SCK0.C04D, 0x80)  \
                           Notify(\_SB.SCK0.C04E, 0x80)  \
                           Notify(\_SB.SCK0.C04F, 0x80)  \
                           Notify(\_SB.SCK0.C050, 0x80)  \
                           Notify(\_SB.SCK0.C051, 0x80)  \
                           Notify(\_SB.SCK0.C052, 0x80)  \
                           Notify(\_SB.SCK0.C053, 0x80)  \
                           Notify(\_SB.SCK0.C054, 0x80)  \
                           Notify(\_SB.SCK0.C055, 0x80)  \
                           Notify(\_SB.SCK0.C056, 0x80)  \
                           Notify(\_SB.SCK0.C057, 0x80)  \
                           Notify(\_SB.SCK0.C058, 0x80)  \
                           Notify(\_SB.SCK0.C059, 0x80)  \
                           Notify(\_SB.SCK0.C05A, 0x80)  \
                           Notify(\_SB.SCK0.C05B, 0x80)  \
                           Notify(\_SB.SCK0.C05C, 0x80)  \
                           Notify(\_SB.SCK0.C05D, 0x80)  \
                           Notify(\_SB.SCK0.C05E, 0x80)  \
                           Notify(\_SB.SCK0.C05F, 0x80)  \
                           Notify(\_SB.SCK0.C060, 0x80)  \
                           Notify(\_SB.SCK0.C061, 0x80)  \
                           Notify(\_SB.SCK0.C062, 0x80)  \
                           Notify(\_SB.SCK0.C063, 0x80)  \
                           Notify(\_SB.SCK0.C064, 0x80)  \
                           Notify(\_SB.SCK0.C065, 0x80)  \
                           Notify(\_SB.SCK0.C066, 0x80)  \
                           Notify(\_SB.SCK0.C067, 0x80)  \
                           Notify(\_SB.SCK0.C068, 0x80)  \
                           Notify(\_SB.SCK0.C069, 0x80)  \
                           Notify(\_SB.SCK0.C06A, 0x80)  \
                           Notify(\_SB.SCK0.C06B, 0x80)  \
                           Notify(\_SB.SCK0.C06C, 0x80)  \
                           Notify(\_SB.SCK0.C06D, 0x80)  \
                           Notify(\_SB.SCK0.C06E, 0x80)  \
                           Notify(\_SB.SCK0.C06F, 0x80)  \
                           Notify(\_SB.SCK0.C070, 0x80)  \
                           Notify(\_SB.SCK0.C071, 0x80)  \
                           Notify(\_SB.SCK0.C072, 0x80)  \
                           Notify(\_SB.SCK0.C073, 0x80)  \
                           Notify(\_SB.SCK0.C074, 0x80)  \
                           Notify(\_SB.SCK0.C075, 0x80)  \
                           Notify(\_SB.SCK0.C076, 0x80)  \
                           Notify(\_SB.SCK0.C077, 0x80)  \
                           Notify(\_SB.SCK0.C078, 0x80)  \
                           Notify(\_SB.SCK0.C079, 0x80)  \
                           Notify(\_SB.SCK0.C07A, 0x80)  \
                           Notify(\_SB.SCK0.C07B, 0x80)  \
                           Notify(\_SB.SCK0.C07C, 0x80)  \
                           Notify(\_SB.SCK0.C07D, 0x80)  \
                           Notify(\_SB.SCK0.C07E, 0x80)  \
                           Notify(\_SB.SCK0.C07F, 0x80)

#define Notify82Socket0()  Notify(\_SB.SCK0.C000, 0x82)  \
                           Notify(\_SB.SCK0.C001, 0x82)  \
                           Notify(\_SB.SCK0.C002, 0x82)  \
                           Notify(\_SB.SCK0.C003, 0x82)  \
                           Notify(\_SB.SCK0.C004, 0x82)  \
                           Notify(\_SB.SCK0.C005, 0x82)  \
                           Notify(\_SB.SCK0.C006, 0x82)  \
                           Notify(\_SB.SCK0.C007, 0x82)  \
                           Notify(\_SB.SCK0.C008, 0x82)  \
                           Notify(\_SB.SCK0.C009, 0x82)  \
                           Notify(\_SB.SCK0.C00A, 0x82)  \
                           Notify(\_SB.SCK0.C00B, 0x82)  \
                           Notify(\_SB.SCK0.C00C, 0x82)  \
                           Notify(\_SB.SCK0.C00D, 0x82)  \
                           Notify(\_SB.SCK0.C00E, 0x82)  \
                           Notify(\_SB.SCK0.C00F, 0x82)  \
                           Notify(\_SB.SCK0.C010, 0x82)  \
                           Notify(\_SB.SCK0.C011, 0x82)  \
                           Notify(\_SB.SCK0.C012, 0x82)  \
                           Notify(\_SB.SCK0.C013, 0x82)  \
                           Notify(\_SB.SCK0.C014, 0x82)  \
                           Notify(\_SB.SCK0.C015, 0x82)  \
                           Notify(\_SB.SCK0.C016, 0x82)  \
                           Notify(\_SB.SCK0.C017, 0x82)  \
                           Notify(\_SB.SCK0.C018, 0x82)  \
                           Notify(\_SB.SCK0.C019, 0x82)  \
                           Notify(\_SB.SCK0.C01A, 0x82)  \
                           Notify(\_SB.SCK0.C01B, 0x82)  \
                           Notify(\_SB.SCK0.C01C, 0x82)  \
                           Notify(\_SB.SCK0.C01D, 0x82)  \
                           Notify(\_SB.SCK0.C01E, 0x82)  \
                           Notify(\_SB.SCK0.C01F, 0x82)  \
                           Notify(\_SB.SCK0.C020, 0x82)  \
                           Notify(\_SB.SCK0.C021, 0x82)  \
                           Notify(\_SB.SCK0.C022, 0x82)  \
                           Notify(\_SB.SCK0.C023, 0x82)  \
                           Notify(\_SB.SCK0.C024, 0x82)  \
                           Notify(\_SB.SCK0.C025, 0x82)  \
                           Notify(\_SB.SCK0.C026, 0x82)  \
                           Notify(\_SB.SCK0.C027, 0x82)  \
                           Notify(\_SB.SCK0.C028, 0x82)  \
                           Notify(\_SB.SCK0.C029, 0x82)  \
                           Notify(\_SB.SCK0.C02A, 0x82)  \
                           Notify(\_SB.SCK0.C02B, 0x82)  \
                           Notify(\_SB.SCK0.C02C, 0x82)  \
                           Notify(\_SB.SCK0.C02D, 0x82)  \
                           Notify(\_SB.SCK0.C02E, 0x82)  \
                           Notify(\_SB.SCK0.C02F, 0x82)  \
                           Notify(\_SB.SCK0.C030, 0x82)  \
                           Notify(\_SB.SCK0.C031, 0x82)  \
                           Notify(\_SB.SCK0.C032, 0x82)  \
                           Notify(\_SB.SCK0.C033, 0x82)  \
                           Notify(\_SB.SCK0.C034, 0x82)  \
                           Notify(\_SB.SCK0.C035, 0x82)  \
                           Notify(\_SB.SCK0.C036, 0x82)  \
                           Notify(\_SB.SCK0.C037, 0x82)  \
                           Notify(\_SB.SCK0.C038, 0x82)  \
                           Notify(\_SB.SCK0.C039, 0x82)  \
                           Notify(\_SB.SCK0.C03A, 0x82)  \
                           Notify(\_SB.SCK0.C03B, 0x82)  \
                           Notify(\_SB.SCK0.C03C, 0x82)  \
                           Notify(\_SB.SCK0.C03D, 0x82)  \
                           Notify(\_SB.SCK0.C03E, 0x82)  \
                           Notify(\_SB.SCK0.C03F, 0x82)  \
                           Notify(\_SB.SCK0.C040, 0x82)  \
                           Notify(\_SB.SCK0.C041, 0x82)  \
                           Notify(\_SB.SCK0.C042, 0x82)  \
                           Notify(\_SB.SCK0.C043, 0x82)  \
                           Notify(\_SB.SCK0.C044, 0x82)  \
                           Notify(\_SB.SCK0.C045, 0x82)  \
                           Notify(\_SB.SCK0.C046, 0x82)  \
                           Notify(\_SB.SCK0.C047, 0x82)  \
                           Notify(\_SB.SCK0.C048, 0x82)  \
                           Notify(\_SB.SCK0.C049, 0x82)  \
                           Notify(\_SB.SCK0.C04A, 0x82)  \
                           Notify(\_SB.SCK0.C04B, 0x82)  \
                           Notify(\_SB.SCK0.C04C, 0x82)  \
                           Notify(\_SB.SCK0.C04D, 0x82)  \
                           Notify(\_SB.SCK0.C04E, 0x82)  \
                           Notify(\_SB.SCK0.C04F, 0x82)  \
                           Notify(\_SB.SCK0.C050, 0x82)  \
                           Notify(\_SB.SCK0.C051, 0x82)  \
                           Notify(\_SB.SCK0.C052, 0x82)  \
                           Notify(\_SB.SCK0.C053, 0x82)  \
                           Notify(\_SB.SCK0.C054, 0x82)  \
                           Notify(\_SB.SCK0.C055, 0x82)  \
                           Notify(\_SB.SCK0.C056, 0x82)  \
                           Notify(\_SB.SCK0.C057, 0x82)  \
                           Notify(\_SB.SCK0.C058, 0x82)  \
                           Notify(\_SB.SCK0.C059, 0x82)  \
                           Notify(\_SB.SCK0.C05A, 0x82)  \
                           Notify(\_SB.SCK0.C05B, 0x82)  \
                           Notify(\_SB.SCK0.C05C, 0x82)  \
                           Notify(\_SB.SCK0.C05D, 0x82)  \
                           Notify(\_SB.SCK0.C05E, 0x82)  \
                           Notify(\_SB.SCK0.C05F, 0x82)  \
                           Notify(\_SB.SCK0.C060, 0x82)  \
                           Notify(\_SB.SCK0.C061, 0x82)  \
                           Notify(\_SB.SCK0.C062, 0x82)  \
                           Notify(\_SB.SCK0.C063, 0x82)  \
                           Notify(\_SB.SCK0.C064, 0x82)  \
                           Notify(\_SB.SCK0.C065, 0x82)  \
                           Notify(\_SB.SCK0.C066, 0x82)  \
                           Notify(\_SB.SCK0.C067, 0x82)  \
                           Notify(\_SB.SCK0.C068, 0x82)  \
                           Notify(\_SB.SCK0.C069, 0x82)  \
                           Notify(\_SB.SCK0.C06A, 0x82)  \
                           Notify(\_SB.SCK0.C06B, 0x82)  \
                           Notify(\_SB.SCK0.C06C, 0x82)  \
                           Notify(\_SB.SCK0.C06D, 0x82)  \
                           Notify(\_SB.SCK0.C06E, 0x82)  \
                           Notify(\_SB.SCK0.C06F, 0x82)  \
                           Notify(\_SB.SCK0.C070, 0x82)  \
                           Notify(\_SB.SCK0.C071, 0x82)  \
                           Notify(\_SB.SCK0.C072, 0x82)  \
                           Notify(\_SB.SCK0.C073, 0x82)  \
                           Notify(\_SB.SCK0.C074, 0x82)  \
                           Notify(\_SB.SCK0.C075, 0x82)  \
                           Notify(\_SB.SCK0.C076, 0x82)  \
                           Notify(\_SB.SCK0.C077, 0x82)  \
                           Notify(\_SB.SCK0.C078, 0x82)  \
                           Notify(\_SB.SCK0.C079, 0x82)  \
                           Notify(\_SB.SCK0.C07A, 0x82)  \
                           Notify(\_SB.SCK0.C07B, 0x82)  \
                           Notify(\_SB.SCK0.C07C, 0x82)  \
                           Notify(\_SB.SCK0.C07D, 0x82)  \
                           Notify(\_SB.SCK0.C07E, 0x82)  \
                           Notify(\_SB.SCK0.C07F, 0x82)

#if MAX_SOCKET > 1
#define Notify80Socket1()  Notify(\_SB.SCK1.C000, 0x80)  \
                           Notify(\_SB.SCK1.C001, 0x80)  \
                           Notify(\_SB.SCK1.C002, 0x80)  \
                           Notify(\_SB.SCK1.C003, 0x80)  \
                           Notify(\_SB.SCK1.C004, 0x80)  \
                           Notify(\_SB.SCK1.C005, 0x80)  \
                           Notify(\_SB.SCK1.C006, 0x80)  \
                           Notify(\_SB.SCK1.C007, 0x80)  \
                           Notify(\_SB.SCK1.C008, 0x80)  \
                           Notify(\_SB.SCK1.C009, 0x80)  \
                           Notify(\_SB.SCK1.C00A, 0x80)  \
                           Notify(\_SB.SCK1.C00B, 0x80)  \
                           Notify(\_SB.SCK1.C00C, 0x80)  \
                           Notify(\_SB.SCK1.C00D, 0x80)  \
                           Notify(\_SB.SCK1.C00E, 0x80)  \
                           Notify(\_SB.SCK1.C00F, 0x80)  \
                           Notify(\_SB.SCK1.C010, 0x80)  \
                           Notify(\_SB.SCK1.C011, 0x80)  \
                           Notify(\_SB.SCK1.C012, 0x80)  \
                           Notify(\_SB.SCK1.C013, 0x80)  \
                           Notify(\_SB.SCK1.C014, 0x80)  \
                           Notify(\_SB.SCK1.C015, 0x80)  \
                           Notify(\_SB.SCK1.C016, 0x80)  \
                           Notify(\_SB.SCK1.C017, 0x80)  \
                           Notify(\_SB.SCK1.C018, 0x80)  \
                           Notify(\_SB.SCK1.C019, 0x80)  \
                           Notify(\_SB.SCK1.C01A, 0x80)  \
                           Notify(\_SB.SCK1.C01B, 0x80)  \
                           Notify(\_SB.SCK1.C01C, 0x80)  \
                           Notify(\_SB.SCK1.C01D, 0x80)  \
                           Notify(\_SB.SCK1.C01E, 0x80)  \
                           Notify(\_SB.SCK1.C01F, 0x80)  \
                           Notify(\_SB.SCK1.C020, 0x80)  \
                           Notify(\_SB.SCK1.C021, 0x80)  \
                           Notify(\_SB.SCK1.C022, 0x80)  \
                           Notify(\_SB.SCK1.C023, 0x80)  \
                           Notify(\_SB.SCK1.C024, 0x80)  \
                           Notify(\_SB.SCK1.C025, 0x80)  \
                           Notify(\_SB.SCK1.C026, 0x80)  \
                           Notify(\_SB.SCK1.C027, 0x80)  \
                           Notify(\_SB.SCK1.C028, 0x80)  \
                           Notify(\_SB.SCK1.C029, 0x80)  \
                           Notify(\_SB.SCK1.C02A, 0x80)  \
                           Notify(\_SB.SCK1.C02B, 0x80)  \
                           Notify(\_SB.SCK1.C02C, 0x80)  \
                           Notify(\_SB.SCK1.C02D, 0x80)  \
                           Notify(\_SB.SCK1.C02E, 0x80)  \
                           Notify(\_SB.SCK1.C02F, 0x80)  \
                           Notify(\_SB.SCK1.C030, 0x80)  \
                           Notify(\_SB.SCK1.C031, 0x80)  \
                           Notify(\_SB.SCK1.C032, 0x80)  \
                           Notify(\_SB.SCK1.C033, 0x80)  \
                           Notify(\_SB.SCK1.C034, 0x80)  \
                           Notify(\_SB.SCK1.C035, 0x80)  \
                           Notify(\_SB.SCK1.C036, 0x80)  \
                           Notify(\_SB.SCK1.C037, 0x80)  \
                           Notify(\_SB.SCK1.C038, 0x80)  \
                           Notify(\_SB.SCK1.C039, 0x80)  \
                           Notify(\_SB.SCK1.C03A, 0x80)  \
                           Notify(\_SB.SCK1.C03B, 0x80)  \
                           Notify(\_SB.SCK1.C03C, 0x80)  \
                           Notify(\_SB.SCK1.C03D, 0x80)  \
                           Notify(\_SB.SCK1.C03E, 0x80)  \
                           Notify(\_SB.SCK1.C03F, 0x80)  \
                           Notify(\_SB.SCK1.C040, 0x80)  \
                           Notify(\_SB.SCK1.C041, 0x80)  \
                           Notify(\_SB.SCK1.C042, 0x80)  \
                           Notify(\_SB.SCK1.C043, 0x80)  \
                           Notify(\_SB.SCK1.C044, 0x80)  \
                           Notify(\_SB.SCK1.C045, 0x80)  \
                           Notify(\_SB.SCK1.C046, 0x80)  \
                           Notify(\_SB.SCK1.C047, 0x80)  \
                           Notify(\_SB.SCK1.C048, 0x80)  \
                           Notify(\_SB.SCK1.C049, 0x80)  \
                           Notify(\_SB.SCK1.C04A, 0x80)  \
                           Notify(\_SB.SCK1.C04B, 0x80)  \
                           Notify(\_SB.SCK1.C04C, 0x80)  \
                           Notify(\_SB.SCK1.C04D, 0x80)  \
                           Notify(\_SB.SCK1.C04E, 0x80)  \
                           Notify(\_SB.SCK1.C04F, 0x80)  \
                           Notify(\_SB.SCK1.C050, 0x80)  \
                           Notify(\_SB.SCK1.C051, 0x80)  \
                           Notify(\_SB.SCK1.C052, 0x80)  \
                           Notify(\_SB.SCK1.C053, 0x80)  \
                           Notify(\_SB.SCK1.C054, 0x80)  \
                           Notify(\_SB.SCK1.C055, 0x80)  \
                           Notify(\_SB.SCK1.C056, 0x80)  \
                           Notify(\_SB.SCK1.C057, 0x80)  \
                           Notify(\_SB.SCK1.C058, 0x80)  \
                           Notify(\_SB.SCK1.C059, 0x80)  \
                           Notify(\_SB.SCK1.C05A, 0x80)  \
                           Notify(\_SB.SCK1.C05B, 0x80)  \
                           Notify(\_SB.SCK1.C05C, 0x80)  \
                           Notify(\_SB.SCK1.C05D, 0x80)  \
                           Notify(\_SB.SCK1.C05E, 0x80)  \
                           Notify(\_SB.SCK1.C05F, 0x80)  \
                           Notify(\_SB.SCK1.C060, 0x80)  \
                           Notify(\_SB.SCK1.C061, 0x80)  \
                           Notify(\_SB.SCK1.C062, 0x80)  \
                           Notify(\_SB.SCK1.C063, 0x80)  \
                           Notify(\_SB.SCK1.C064, 0x80)  \
                           Notify(\_SB.SCK1.C065, 0x80)  \
                           Notify(\_SB.SCK1.C066, 0x80)  \
                           Notify(\_SB.SCK1.C067, 0x80)  \
                           Notify(\_SB.SCK1.C068, 0x80)  \
                           Notify(\_SB.SCK1.C069, 0x80)  \
                           Notify(\_SB.SCK1.C06A, 0x80)  \
                           Notify(\_SB.SCK1.C06B, 0x80)  \
                           Notify(\_SB.SCK1.C06C, 0x80)  \
                           Notify(\_SB.SCK1.C06D, 0x80)  \
                           Notify(\_SB.SCK1.C06E, 0x80)  \
                           Notify(\_SB.SCK1.C06F, 0x80)  \
                           Notify(\_SB.SCK1.C070, 0x80)  \
                           Notify(\_SB.SCK1.C071, 0x80)  \
                           Notify(\_SB.SCK1.C072, 0x80)  \
                           Notify(\_SB.SCK1.C073, 0x80)  \
                           Notify(\_SB.SCK1.C074, 0x80)  \
                           Notify(\_SB.SCK1.C075, 0x80)  \
                           Notify(\_SB.SCK1.C076, 0x80)  \
                           Notify(\_SB.SCK1.C077, 0x80)  \
                           Notify(\_SB.SCK1.C078, 0x80)  \
                           Notify(\_SB.SCK1.C079, 0x80)  \
                           Notify(\_SB.SCK1.C07A, 0x80)  \
                           Notify(\_SB.SCK1.C07B, 0x80)  \
                           Notify(\_SB.SCK1.C07C, 0x80)  \
                           Notify(\_SB.SCK1.C07D, 0x80)  \
                           Notify(\_SB.SCK1.C07E, 0x80)  \
                           Notify(\_SB.SCK1.C07F, 0x80)

#define Notify82Socket1()  Notify(\_SB.SCK1.C000, 0x82)  \
                           Notify(\_SB.SCK1.C001, 0x82)  \
                           Notify(\_SB.SCK1.C002, 0x82)  \
                           Notify(\_SB.SCK1.C003, 0x82)  \
                           Notify(\_SB.SCK1.C004, 0x82)  \
                           Notify(\_SB.SCK1.C005, 0x82)  \
                           Notify(\_SB.SCK1.C006, 0x82)  \
                           Notify(\_SB.SCK1.C007, 0x82)  \
                           Notify(\_SB.SCK1.C008, 0x82)  \
                           Notify(\_SB.SCK1.C009, 0x82)  \
                           Notify(\_SB.SCK1.C00A, 0x82)  \
                           Notify(\_SB.SCK1.C00B, 0x82)  \
                           Notify(\_SB.SCK1.C00C, 0x82)  \
                           Notify(\_SB.SCK1.C00D, 0x82)  \
                           Notify(\_SB.SCK1.C00E, 0x82)  \
                           Notify(\_SB.SCK1.C00F, 0x82)  \
                           Notify(\_SB.SCK1.C010, 0x82)  \
                           Notify(\_SB.SCK1.C011, 0x82)  \
                           Notify(\_SB.SCK1.C012, 0x82)  \
                           Notify(\_SB.SCK1.C013, 0x82)  \
                           Notify(\_SB.SCK1.C014, 0x82)  \
                           Notify(\_SB.SCK1.C015, 0x82)  \
                           Notify(\_SB.SCK1.C016, 0x82)  \
                           Notify(\_SB.SCK1.C017, 0x82)  \
                           Notify(\_SB.SCK1.C018, 0x82)  \
                           Notify(\_SB.SCK1.C019, 0x82)  \
                           Notify(\_SB.SCK1.C01A, 0x82)  \
                           Notify(\_SB.SCK1.C01B, 0x82)  \
                           Notify(\_SB.SCK1.C01C, 0x82)  \
                           Notify(\_SB.SCK1.C01D, 0x82)  \
                           Notify(\_SB.SCK1.C01E, 0x82)  \
                           Notify(\_SB.SCK1.C01F, 0x82)  \
                           Notify(\_SB.SCK1.C020, 0x82)  \
                           Notify(\_SB.SCK1.C021, 0x82)  \
                           Notify(\_SB.SCK1.C022, 0x82)  \
                           Notify(\_SB.SCK1.C023, 0x82)  \
                           Notify(\_SB.SCK1.C024, 0x82)  \
                           Notify(\_SB.SCK1.C025, 0x82)  \
                           Notify(\_SB.SCK1.C026, 0x82)  \
                           Notify(\_SB.SCK1.C027, 0x82)  \
                           Notify(\_SB.SCK1.C028, 0x82)  \
                           Notify(\_SB.SCK1.C029, 0x82)  \
                           Notify(\_SB.SCK1.C02A, 0x82)  \
                           Notify(\_SB.SCK1.C02B, 0x82)  \
                           Notify(\_SB.SCK1.C02C, 0x82)  \
                           Notify(\_SB.SCK1.C02D, 0x82)  \
                           Notify(\_SB.SCK1.C02E, 0x82)  \
                           Notify(\_SB.SCK1.C02F, 0x82)  \
                           Notify(\_SB.SCK1.C030, 0x82)  \
                           Notify(\_SB.SCK1.C031, 0x82)  \
                           Notify(\_SB.SCK1.C032, 0x82)  \
                           Notify(\_SB.SCK1.C033, 0x82)  \
                           Notify(\_SB.SCK1.C034, 0x82)  \
                           Notify(\_SB.SCK1.C035, 0x82)  \
                           Notify(\_SB.SCK1.C036, 0x82)  \
                           Notify(\_SB.SCK1.C037, 0x82)  \
                           Notify(\_SB.SCK1.C038, 0x82)  \
                           Notify(\_SB.SCK1.C039, 0x82)  \
                           Notify(\_SB.SCK1.C03A, 0x82)  \
                           Notify(\_SB.SCK1.C03B, 0x82)  \
                           Notify(\_SB.SCK1.C03C, 0x82)  \
                           Notify(\_SB.SCK1.C03D, 0x82)  \
                           Notify(\_SB.SCK1.C03E, 0x82)  \
                           Notify(\_SB.SCK1.C03F, 0x82)  \
                           Notify(\_SB.SCK1.C040, 0x82)  \
                           Notify(\_SB.SCK1.C041, 0x82)  \
                           Notify(\_SB.SCK1.C042, 0x82)  \
                           Notify(\_SB.SCK1.C043, 0x82)  \
                           Notify(\_SB.SCK1.C044, 0x82)  \
                           Notify(\_SB.SCK1.C045, 0x82)  \
                           Notify(\_SB.SCK1.C046, 0x82)  \
                           Notify(\_SB.SCK1.C047, 0x82)  \
                           Notify(\_SB.SCK1.C048, 0x82)  \
                           Notify(\_SB.SCK1.C049, 0x82)  \
                           Notify(\_SB.SCK1.C04A, 0x82)  \
                           Notify(\_SB.SCK1.C04B, 0x82)  \
                           Notify(\_SB.SCK1.C04C, 0x82)  \
                           Notify(\_SB.SCK1.C04D, 0x82)  \
                           Notify(\_SB.SCK1.C04E, 0x82)  \
                           Notify(\_SB.SCK1.C04F, 0x82)  \
                           Notify(\_SB.SCK1.C050, 0x82)  \
                           Notify(\_SB.SCK1.C051, 0x82)  \
                           Notify(\_SB.SCK1.C052, 0x82)  \
                           Notify(\_SB.SCK1.C053, 0x82)  \
                           Notify(\_SB.SCK1.C054, 0x82)  \
                           Notify(\_SB.SCK1.C055, 0x82)  \
                           Notify(\_SB.SCK1.C056, 0x82)  \
                           Notify(\_SB.SCK1.C057, 0x82)  \
                           Notify(\_SB.SCK1.C058, 0x82)  \
                           Notify(\_SB.SCK1.C059, 0x82)  \
                           Notify(\_SB.SCK1.C05A, 0x82)  \
                           Notify(\_SB.SCK1.C05B, 0x82)  \
                           Notify(\_SB.SCK1.C05C, 0x82)  \
                           Notify(\_SB.SCK1.C05D, 0x82)  \
                           Notify(\_SB.SCK1.C05E, 0x82)  \
                           Notify(\_SB.SCK1.C05F, 0x82)  \
                           Notify(\_SB.SCK1.C060, 0x82)  \
                           Notify(\_SB.SCK1.C061, 0x82)  \
                           Notify(\_SB.SCK1.C062, 0x82)  \
                           Notify(\_SB.SCK1.C063, 0x82)  \
                           Notify(\_SB.SCK1.C064, 0x82)  \
                           Notify(\_SB.SCK1.C065, 0x82)  \
                           Notify(\_SB.SCK1.C066, 0x82)  \
                           Notify(\_SB.SCK1.C067, 0x82)  \
                           Notify(\_SB.SCK1.C068, 0x82)  \
                           Notify(\_SB.SCK1.C069, 0x82)  \
                           Notify(\_SB.SCK1.C06A, 0x82)  \
                           Notify(\_SB.SCK1.C06B, 0x82)  \
                           Notify(\_SB.SCK1.C06C, 0x82)  \
                           Notify(\_SB.SCK1.C06D, 0x82)  \
                           Notify(\_SB.SCK1.C06E, 0x82)  \
                           Notify(\_SB.SCK1.C06F, 0x82)  \
                           Notify(\_SB.SCK1.C070, 0x82)  \
                           Notify(\_SB.SCK1.C071, 0x82)  \
                           Notify(\_SB.SCK1.C072, 0x82)  \
                           Notify(\_SB.SCK1.C073, 0x82)  \
                           Notify(\_SB.SCK1.C074, 0x82)  \
                           Notify(\_SB.SCK1.C075, 0x82)  \
                           Notify(\_SB.SCK1.C076, 0x82)  \
                           Notify(\_SB.SCK1.C077, 0x82)  \
                           Notify(\_SB.SCK1.C078, 0x82)  \
                           Notify(\_SB.SCK1.C079, 0x82)  \
                           Notify(\_SB.SCK1.C07A, 0x82)  \
                           Notify(\_SB.SCK1.C07B, 0x82)  \
                           Notify(\_SB.SCK1.C07C, 0x82)  \
                           Notify(\_SB.SCK1.C07D, 0x82)  \
                           Notify(\_SB.SCK1.C07E, 0x82)  \
                           Notify(\_SB.SCK1.C07F, 0x82)
#else
#define Notify80Socket1()
#define Notify82Socket1()
#endif

#if MAX_SOCKET > 2
#define Notify80Socket2()  Notify(\_SB.SCK2.C000, 0x80)  \
                           Notify(\_SB.SCK2.C001, 0x80)  \
                           Notify(\_SB.SCK2.C002, 0x80)  \
                           Notify(\_SB.SCK2.C003, 0x80)  \
                           Notify(\_SB.SCK2.C004, 0x80)  \
                           Notify(\_SB.SCK2.C005, 0x80)  \
                           Notify(\_SB.SCK2.C006, 0x80)  \
                           Notify(\_SB.SCK2.C007, 0x80)  \
                           Notify(\_SB.SCK2.C008, 0x80)  \
                           Notify(\_SB.SCK2.C009, 0x80)  \
                           Notify(\_SB.SCK2.C00A, 0x80)  \
                           Notify(\_SB.SCK2.C00B, 0x80)  \
                           Notify(\_SB.SCK2.C00C, 0x80)  \
                           Notify(\_SB.SCK2.C00D, 0x80)  \
                           Notify(\_SB.SCK2.C00E, 0x80)  \
                           Notify(\_SB.SCK2.C00F, 0x80)  \
                           Notify(\_SB.SCK2.C010, 0x80)  \
                           Notify(\_SB.SCK2.C011, 0x80)  \
                           Notify(\_SB.SCK2.C012, 0x80)  \
                           Notify(\_SB.SCK2.C013, 0x80)  \
                           Notify(\_SB.SCK2.C014, 0x80)  \
                           Notify(\_SB.SCK2.C015, 0x80)  \
                           Notify(\_SB.SCK2.C016, 0x80)  \
                           Notify(\_SB.SCK2.C017, 0x80)  \
                           Notify(\_SB.SCK2.C018, 0x80)  \
                           Notify(\_SB.SCK2.C019, 0x80)  \
                           Notify(\_SB.SCK2.C01A, 0x80)  \
                           Notify(\_SB.SCK2.C01B, 0x80)  \
                           Notify(\_SB.SCK2.C01C, 0x80)  \
                           Notify(\_SB.SCK2.C01D, 0x80)  \
                           Notify(\_SB.SCK2.C01E, 0x80)  \
                           Notify(\_SB.SCK2.C01F, 0x80)  \
                           Notify(\_SB.SCK2.C020, 0x80)  \
                           Notify(\_SB.SCK2.C021, 0x80)  \
                           Notify(\_SB.SCK2.C022, 0x80)  \
                           Notify(\_SB.SCK2.C023, 0x80)  \
                           Notify(\_SB.SCK2.C024, 0x80)  \
                           Notify(\_SB.SCK2.C025, 0x80)  \
                           Notify(\_SB.SCK2.C026, 0x80)  \
                           Notify(\_SB.SCK2.C027, 0x80)  \
                           Notify(\_SB.SCK2.C028, 0x80)  \
                           Notify(\_SB.SCK2.C029, 0x80)  \
                           Notify(\_SB.SCK2.C02A, 0x80)  \
                           Notify(\_SB.SCK2.C02B, 0x80)  \
                           Notify(\_SB.SCK2.C02C, 0x80)  \
                           Notify(\_SB.SCK2.C02D, 0x80)  \
                           Notify(\_SB.SCK2.C02E, 0x80)  \
                           Notify(\_SB.SCK2.C02F, 0x80)  \
                           Notify(\_SB.SCK2.C030, 0x80)  \
                           Notify(\_SB.SCK2.C031, 0x80)  \
                           Notify(\_SB.SCK2.C032, 0x80)  \
                           Notify(\_SB.SCK2.C033, 0x80)  \
                           Notify(\_SB.SCK2.C034, 0x80)  \
                           Notify(\_SB.SCK2.C035, 0x80)  \
                           Notify(\_SB.SCK2.C036, 0x80)  \
                           Notify(\_SB.SCK2.C037, 0x80)  \
                           Notify(\_SB.SCK2.C038, 0x80)  \
                           Notify(\_SB.SCK2.C039, 0x80)  \
                           Notify(\_SB.SCK2.C03A, 0x80)  \
                           Notify(\_SB.SCK2.C03B, 0x80)  \
                           Notify(\_SB.SCK2.C03C, 0x80)  \
                           Notify(\_SB.SCK2.C03D, 0x80)  \
                           Notify(\_SB.SCK2.C03E, 0x80)  \
                           Notify(\_SB.SCK2.C03F, 0x80)  \
                           Notify(\_SB.SCK2.C040, 0x80)  \
                           Notify(\_SB.SCK2.C041, 0x80)  \
                           Notify(\_SB.SCK2.C042, 0x80)  \
                           Notify(\_SB.SCK2.C043, 0x80)  \
                           Notify(\_SB.SCK2.C044, 0x80)  \
                           Notify(\_SB.SCK2.C045, 0x80)  \
                           Notify(\_SB.SCK2.C046, 0x80)  \
                           Notify(\_SB.SCK2.C047, 0x80)  \
                           Notify(\_SB.SCK2.C048, 0x80)  \
                           Notify(\_SB.SCK2.C049, 0x80)  \
                           Notify(\_SB.SCK2.C04A, 0x80)  \
                           Notify(\_SB.SCK2.C04B, 0x80)  \
                           Notify(\_SB.SCK2.C04C, 0x80)  \
                           Notify(\_SB.SCK2.C04D, 0x80)  \
                           Notify(\_SB.SCK2.C04E, 0x80)  \
                           Notify(\_SB.SCK2.C04F, 0x80)  \
                           Notify(\_SB.SCK2.C050, 0x80)  \
                           Notify(\_SB.SCK2.C051, 0x80)  \
                           Notify(\_SB.SCK2.C052, 0x80)  \
                           Notify(\_SB.SCK2.C053, 0x80)  \
                           Notify(\_SB.SCK2.C054, 0x80)  \
                           Notify(\_SB.SCK2.C055, 0x80)  \
                           Notify(\_SB.SCK2.C056, 0x80)  \
                           Notify(\_SB.SCK2.C057, 0x80)  \
                           Notify(\_SB.SCK2.C058, 0x80)  \
                           Notify(\_SB.SCK2.C059, 0x80)  \
                           Notify(\_SB.SCK2.C05A, 0x80)  \
                           Notify(\_SB.SCK2.C05B, 0x80)  \
                           Notify(\_SB.SCK2.C05C, 0x80)  \
                           Notify(\_SB.SCK2.C05D, 0x80)  \
                           Notify(\_SB.SCK2.C05E, 0x80)  \
                           Notify(\_SB.SCK2.C05F, 0x80)  \
                           Notify(\_SB.SCK2.C060, 0x80)  \
                           Notify(\_SB.SCK2.C061, 0x80)  \
                           Notify(\_SB.SCK2.C062, 0x80)  \
                           Notify(\_SB.SCK2.C063, 0x80)  \
                           Notify(\_SB.SCK2.C064, 0x80)  \
                           Notify(\_SB.SCK2.C065, 0x80)  \
                           Notify(\_SB.SCK2.C066, 0x80)  \
                           Notify(\_SB.SCK2.C067, 0x80)  \
                           Notify(\_SB.SCK2.C068, 0x80)  \
                           Notify(\_SB.SCK2.C069, 0x80)  \
                           Notify(\_SB.SCK2.C06A, 0x80)  \
                           Notify(\_SB.SCK2.C06B, 0x80)  \
                           Notify(\_SB.SCK2.C06C, 0x80)  \
                           Notify(\_SB.SCK2.C06D, 0x80)  \
                           Notify(\_SB.SCK2.C06E, 0x80)  \
                           Notify(\_SB.SCK2.C06F, 0x80)  \
                           Notify(\_SB.SCK2.C070, 0x80)  \
                           Notify(\_SB.SCK2.C071, 0x80)  \
                           Notify(\_SB.SCK2.C072, 0x80)  \
                           Notify(\_SB.SCK2.C073, 0x80)  \
                           Notify(\_SB.SCK2.C074, 0x80)  \
                           Notify(\_SB.SCK2.C075, 0x80)  \
                           Notify(\_SB.SCK2.C076, 0x80)  \
                           Notify(\_SB.SCK2.C077, 0x80)  \
                           Notify(\_SB.SCK2.C078, 0x80)  \
                           Notify(\_SB.SCK2.C079, 0x80)  \
                           Notify(\_SB.SCK2.C07A, 0x80)  \
                           Notify(\_SB.SCK2.C07B, 0x80)  \
                           Notify(\_SB.SCK2.C07C, 0x80)  \
                           Notify(\_SB.SCK2.C07D, 0x80)  \
                           Notify(\_SB.SCK2.C07E, 0x80)  \
                           Notify(\_SB.SCK2.C07F, 0x80)

#define Notify82Socket2()  Notify(\_SB.SCK2.C000, 0x82)  \
                           Notify(\_SB.SCK2.C001, 0x82)  \
                           Notify(\_SB.SCK2.C002, 0x82)  \
                           Notify(\_SB.SCK2.C003, 0x82)  \
                           Notify(\_SB.SCK2.C004, 0x82)  \
                           Notify(\_SB.SCK2.C005, 0x82)  \
                           Notify(\_SB.SCK2.C006, 0x82)  \
                           Notify(\_SB.SCK2.C007, 0x82)  \
                           Notify(\_SB.SCK2.C008, 0x82)  \
                           Notify(\_SB.SCK2.C009, 0x82)  \
                           Notify(\_SB.SCK2.C00A, 0x82)  \
                           Notify(\_SB.SCK2.C00B, 0x82)  \
                           Notify(\_SB.SCK2.C00C, 0x82)  \
                           Notify(\_SB.SCK2.C00D, 0x82)  \
                           Notify(\_SB.SCK2.C00E, 0x82)  \
                           Notify(\_SB.SCK2.C00F, 0x82)  \
                           Notify(\_SB.SCK2.C010, 0x82)  \
                           Notify(\_SB.SCK2.C011, 0x82)  \
                           Notify(\_SB.SCK2.C012, 0x82)  \
                           Notify(\_SB.SCK2.C013, 0x82)  \
                           Notify(\_SB.SCK2.C014, 0x82)  \
                           Notify(\_SB.SCK2.C015, 0x82)  \
                           Notify(\_SB.SCK2.C016, 0x82)  \
                           Notify(\_SB.SCK2.C017, 0x82)  \
                           Notify(\_SB.SCK2.C018, 0x82)  \
                           Notify(\_SB.SCK2.C019, 0x82)  \
                           Notify(\_SB.SCK2.C01A, 0x82)  \
                           Notify(\_SB.SCK2.C01B, 0x82)  \
                           Notify(\_SB.SCK2.C01C, 0x82)  \
                           Notify(\_SB.SCK2.C01D, 0x82)  \
                           Notify(\_SB.SCK2.C01E, 0x82)  \
                           Notify(\_SB.SCK2.C01F, 0x82)  \
                           Notify(\_SB.SCK2.C020, 0x82)  \
                           Notify(\_SB.SCK2.C021, 0x82)  \
                           Notify(\_SB.SCK2.C022, 0x82)  \
                           Notify(\_SB.SCK2.C023, 0x82)  \
                           Notify(\_SB.SCK2.C024, 0x82)  \
                           Notify(\_SB.SCK2.C025, 0x82)  \
                           Notify(\_SB.SCK2.C026, 0x82)  \
                           Notify(\_SB.SCK2.C027, 0x82)  \
                           Notify(\_SB.SCK2.C028, 0x82)  \
                           Notify(\_SB.SCK2.C029, 0x82)  \
                           Notify(\_SB.SCK2.C02A, 0x82)  \
                           Notify(\_SB.SCK2.C02B, 0x82)  \
                           Notify(\_SB.SCK2.C02C, 0x82)  \
                           Notify(\_SB.SCK2.C02D, 0x82)  \
                           Notify(\_SB.SCK2.C02E, 0x82)  \
                           Notify(\_SB.SCK2.C02F, 0x82)  \
                           Notify(\_SB.SCK2.C030, 0x82)  \
                           Notify(\_SB.SCK2.C031, 0x82)  \
                           Notify(\_SB.SCK2.C032, 0x82)  \
                           Notify(\_SB.SCK2.C033, 0x82)  \
                           Notify(\_SB.SCK2.C034, 0x82)  \
                           Notify(\_SB.SCK2.C035, 0x82)  \
                           Notify(\_SB.SCK2.C036, 0x82)  \
                           Notify(\_SB.SCK2.C037, 0x82)  \
                           Notify(\_SB.SCK2.C038, 0x82)  \
                           Notify(\_SB.SCK2.C039, 0x82)  \
                           Notify(\_SB.SCK2.C03A, 0x82)  \
                           Notify(\_SB.SCK2.C03B, 0x82)  \
                           Notify(\_SB.SCK2.C03C, 0x82)  \
                           Notify(\_SB.SCK2.C03D, 0x82)  \
                           Notify(\_SB.SCK2.C03E, 0x82)  \
                           Notify(\_SB.SCK2.C03F, 0x82)  \
                           Notify(\_SB.SCK2.C040, 0x82)  \
                           Notify(\_SB.SCK2.C041, 0x82)  \
                           Notify(\_SB.SCK2.C042, 0x82)  \
                           Notify(\_SB.SCK2.C043, 0x82)  \
                           Notify(\_SB.SCK2.C044, 0x82)  \
                           Notify(\_SB.SCK2.C045, 0x82)  \
                           Notify(\_SB.SCK2.C046, 0x82)  \
                           Notify(\_SB.SCK2.C047, 0x82)  \
                           Notify(\_SB.SCK2.C048, 0x82)  \
                           Notify(\_SB.SCK2.C049, 0x82)  \
                           Notify(\_SB.SCK2.C04A, 0x82)  \
                           Notify(\_SB.SCK2.C04B, 0x82)  \
                           Notify(\_SB.SCK2.C04C, 0x82)  \
                           Notify(\_SB.SCK2.C04D, 0x82)  \
                           Notify(\_SB.SCK2.C04E, 0x82)  \
                           Notify(\_SB.SCK2.C04F, 0x82)  \
                           Notify(\_SB.SCK2.C050, 0x82)  \
                           Notify(\_SB.SCK2.C051, 0x82)  \
                           Notify(\_SB.SCK2.C052, 0x82)  \
                           Notify(\_SB.SCK2.C053, 0x82)  \
                           Notify(\_SB.SCK2.C054, 0x82)  \
                           Notify(\_SB.SCK2.C055, 0x82)  \
                           Notify(\_SB.SCK2.C056, 0x82)  \
                           Notify(\_SB.SCK2.C057, 0x82)  \
                           Notify(\_SB.SCK2.C058, 0x82)  \
                           Notify(\_SB.SCK2.C059, 0x82)  \
                           Notify(\_SB.SCK2.C05A, 0x82)  \
                           Notify(\_SB.SCK2.C05B, 0x82)  \
                           Notify(\_SB.SCK2.C05C, 0x82)  \
                           Notify(\_SB.SCK2.C05D, 0x82)  \
                           Notify(\_SB.SCK2.C05E, 0x82)  \
                           Notify(\_SB.SCK2.C05F, 0x82)  \
                           Notify(\_SB.SCK2.C060, 0x82)  \
                           Notify(\_SB.SCK2.C061, 0x82)  \
                           Notify(\_SB.SCK2.C062, 0x82)  \
                           Notify(\_SB.SCK2.C063, 0x82)  \
                           Notify(\_SB.SCK2.C064, 0x82)  \
                           Notify(\_SB.SCK2.C065, 0x82)  \
                           Notify(\_SB.SCK2.C066, 0x82)  \
                           Notify(\_SB.SCK2.C067, 0x82)  \
                           Notify(\_SB.SCK2.C068, 0x82)  \
                           Notify(\_SB.SCK2.C069, 0x82)  \
                           Notify(\_SB.SCK2.C06A, 0x82)  \
                           Notify(\_SB.SCK2.C06B, 0x82)  \
                           Notify(\_SB.SCK2.C06C, 0x82)  \
                           Notify(\_SB.SCK2.C06D, 0x82)  \
                           Notify(\_SB.SCK2.C06E, 0x82)  \
                           Notify(\_SB.SCK2.C06F, 0x82)  \
                           Notify(\_SB.SCK2.C070, 0x82)  \
                           Notify(\_SB.SCK2.C071, 0x82)  \
                           Notify(\_SB.SCK2.C072, 0x82)  \
                           Notify(\_SB.SCK2.C073, 0x82)  \
                           Notify(\_SB.SCK2.C074, 0x82)  \
                           Notify(\_SB.SCK2.C075, 0x82)  \
                           Notify(\_SB.SCK2.C076, 0x82)  \
                           Notify(\_SB.SCK2.C077, 0x82)  \
                           Notify(\_SB.SCK2.C078, 0x82)  \
                           Notify(\_SB.SCK2.C079, 0x82)  \
                           Notify(\_SB.SCK2.C07A, 0x82)  \
                           Notify(\_SB.SCK2.C07B, 0x82)  \
                           Notify(\_SB.SCK2.C07C, 0x82)  \
                           Notify(\_SB.SCK2.C07D, 0x82)  \
                           Notify(\_SB.SCK2.C07E, 0x82)  \
                           Notify(\_SB.SCK2.C07F, 0x82)
#else
#define Notify80Socket2()
#define Notify82Socket2()
#endif

#if MAX_SOCKET > 3
#define Notify80Socket3()  Notify(\_SB.SCK3.C000, 0x80)  \
                           Notify(\_SB.SCK3.C001, 0x80)  \
                           Notify(\_SB.SCK3.C002, 0x80)  \
                           Notify(\_SB.SCK3.C003, 0x80)  \
                           Notify(\_SB.SCK3.C004, 0x80)  \
                           Notify(\_SB.SCK3.C005, 0x80)  \
                           Notify(\_SB.SCK3.C006, 0x80)  \
                           Notify(\_SB.SCK3.C007, 0x80)  \
                           Notify(\_SB.SCK3.C008, 0x80)  \
                           Notify(\_SB.SCK3.C009, 0x80)  \
                           Notify(\_SB.SCK3.C00A, 0x80)  \
                           Notify(\_SB.SCK3.C00B, 0x80)  \
                           Notify(\_SB.SCK3.C00C, 0x80)  \
                           Notify(\_SB.SCK3.C00D, 0x80)  \
                           Notify(\_SB.SCK3.C00E, 0x80)  \
                           Notify(\_SB.SCK3.C00F, 0x80)  \
                           Notify(\_SB.SCK3.C010, 0x80)  \
                           Notify(\_SB.SCK3.C011, 0x80)  \
                           Notify(\_SB.SCK3.C012, 0x80)  \
                           Notify(\_SB.SCK3.C013, 0x80)  \
                           Notify(\_SB.SCK3.C014, 0x80)  \
                           Notify(\_SB.SCK3.C015, 0x80)  \
                           Notify(\_SB.SCK3.C016, 0x80)  \
                           Notify(\_SB.SCK3.C017, 0x80)  \
                           Notify(\_SB.SCK3.C018, 0x80)  \
                           Notify(\_SB.SCK3.C019, 0x80)  \
                           Notify(\_SB.SCK3.C01A, 0x80)  \
                           Notify(\_SB.SCK3.C01B, 0x80)  \
                           Notify(\_SB.SCK3.C01C, 0x80)  \
                           Notify(\_SB.SCK3.C01D, 0x80)  \
                           Notify(\_SB.SCK3.C01E, 0x80)  \
                           Notify(\_SB.SCK3.C01F, 0x80)  \
                           Notify(\_SB.SCK3.C020, 0x80)  \
                           Notify(\_SB.SCK3.C021, 0x80)  \
                           Notify(\_SB.SCK3.C022, 0x80)  \
                           Notify(\_SB.SCK3.C023, 0x80)  \
                           Notify(\_SB.SCK3.C024, 0x80)  \
                           Notify(\_SB.SCK3.C025, 0x80)  \
                           Notify(\_SB.SCK3.C026, 0x80)  \
                           Notify(\_SB.SCK3.C027, 0x80)  \
                           Notify(\_SB.SCK3.C028, 0x80)  \
                           Notify(\_SB.SCK3.C029, 0x80)  \
                           Notify(\_SB.SCK3.C02A, 0x80)  \
                           Notify(\_SB.SCK3.C02B, 0x80)  \
                           Notify(\_SB.SCK3.C02C, 0x80)  \
                           Notify(\_SB.SCK3.C02D, 0x80)  \
                           Notify(\_SB.SCK3.C02E, 0x80)  \
                           Notify(\_SB.SCK3.C02F, 0x80)  \
                           Notify(\_SB.SCK3.C030, 0x80)  \
                           Notify(\_SB.SCK3.C031, 0x80)  \
                           Notify(\_SB.SCK3.C032, 0x80)  \
                           Notify(\_SB.SCK3.C033, 0x80)  \
                           Notify(\_SB.SCK3.C034, 0x80)  \
                           Notify(\_SB.SCK3.C035, 0x80)  \
                           Notify(\_SB.SCK3.C036, 0x80)  \
                           Notify(\_SB.SCK3.C037, 0x80)  \
                           Notify(\_SB.SCK3.C038, 0x80)  \
                           Notify(\_SB.SCK3.C039, 0x80)  \
                           Notify(\_SB.SCK3.C03A, 0x80)  \
                           Notify(\_SB.SCK3.C03B, 0x80)  \
                           Notify(\_SB.SCK3.C03C, 0x80)  \
                           Notify(\_SB.SCK3.C03D, 0x80)  \
                           Notify(\_SB.SCK3.C03E, 0x80)  \
                           Notify(\_SB.SCK3.C03F, 0x80)  \
                           Notify(\_SB.SCK3.C040, 0x80)  \
                           Notify(\_SB.SCK3.C041, 0x80)  \
                           Notify(\_SB.SCK3.C042, 0x80)  \
                           Notify(\_SB.SCK3.C043, 0x80)  \
                           Notify(\_SB.SCK3.C044, 0x80)  \
                           Notify(\_SB.SCK3.C045, 0x80)  \
                           Notify(\_SB.SCK3.C046, 0x80)  \
                           Notify(\_SB.SCK3.C047, 0x80)  \
                           Notify(\_SB.SCK3.C048, 0x80)  \
                           Notify(\_SB.SCK3.C049, 0x80)  \
                           Notify(\_SB.SCK3.C04A, 0x80)  \
                           Notify(\_SB.SCK3.C04B, 0x80)  \
                           Notify(\_SB.SCK3.C04C, 0x80)  \
                           Notify(\_SB.SCK3.C04D, 0x80)  \
                           Notify(\_SB.SCK3.C04E, 0x80)  \
                           Notify(\_SB.SCK3.C04F, 0x80)  \
                           Notify(\_SB.SCK3.C050, 0x80)  \
                           Notify(\_SB.SCK3.C051, 0x80)  \
                           Notify(\_SB.SCK3.C052, 0x80)  \
                           Notify(\_SB.SCK3.C053, 0x80)  \
                           Notify(\_SB.SCK3.C054, 0x80)  \
                           Notify(\_SB.SCK3.C055, 0x80)  \
                           Notify(\_SB.SCK3.C056, 0x80)  \
                           Notify(\_SB.SCK3.C057, 0x80)  \
                           Notify(\_SB.SCK3.C058, 0x80)  \
                           Notify(\_SB.SCK3.C059, 0x80)  \
                           Notify(\_SB.SCK3.C05A, 0x80)  \
                           Notify(\_SB.SCK3.C05B, 0x80)  \
                           Notify(\_SB.SCK3.C05C, 0x80)  \
                           Notify(\_SB.SCK3.C05D, 0x80)  \
                           Notify(\_SB.SCK3.C05E, 0x80)  \
                           Notify(\_SB.SCK3.C05F, 0x80)  \
                           Notify(\_SB.SCK3.C060, 0x80)  \
                           Notify(\_SB.SCK3.C061, 0x80)  \
                           Notify(\_SB.SCK3.C062, 0x80)  \
                           Notify(\_SB.SCK3.C063, 0x80)  \
                           Notify(\_SB.SCK3.C064, 0x80)  \
                           Notify(\_SB.SCK3.C065, 0x80)  \
                           Notify(\_SB.SCK3.C066, 0x80)  \
                           Notify(\_SB.SCK3.C067, 0x80)  \
                           Notify(\_SB.SCK3.C068, 0x80)  \
                           Notify(\_SB.SCK3.C069, 0x80)  \
                           Notify(\_SB.SCK3.C06A, 0x80)  \
                           Notify(\_SB.SCK3.C06B, 0x80)  \
                           Notify(\_SB.SCK3.C06C, 0x80)  \
                           Notify(\_SB.SCK3.C06D, 0x80)  \
                           Notify(\_SB.SCK3.C06E, 0x80)  \
                           Notify(\_SB.SCK3.C06F, 0x80)  \
                           Notify(\_SB.SCK3.C070, 0x80)  \
                           Notify(\_SB.SCK3.C071, 0x80)  \
                           Notify(\_SB.SCK3.C072, 0x80)  \
                           Notify(\_SB.SCK3.C073, 0x80)  \
                           Notify(\_SB.SCK3.C074, 0x80)  \
                           Notify(\_SB.SCK3.C075, 0x80)  \
                           Notify(\_SB.SCK3.C076, 0x80)  \
                           Notify(\_SB.SCK3.C077, 0x80)  \
                           Notify(\_SB.SCK3.C078, 0x80)  \
                           Notify(\_SB.SCK3.C079, 0x80)  \
                           Notify(\_SB.SCK3.C07A, 0x80)  \
                           Notify(\_SB.SCK3.C07B, 0x80)  \
                           Notify(\_SB.SCK3.C07C, 0x80)  \
                           Notify(\_SB.SCK3.C07D, 0x80)  \
                           Notify(\_SB.SCK3.C07E, 0x80)  \
                           Notify(\_SB.SCK3.C07F, 0x80)

#define Notify82Socket3()  Notify(\_SB.SCK3.C000, 0x82)  \
                           Notify(\_SB.SCK3.C001, 0x82)  \
                           Notify(\_SB.SCK3.C002, 0x82)  \
                           Notify(\_SB.SCK3.C003, 0x82)  \
                           Notify(\_SB.SCK3.C004, 0x82)  \
                           Notify(\_SB.SCK3.C005, 0x82)  \
                           Notify(\_SB.SCK3.C006, 0x82)  \
                           Notify(\_SB.SCK3.C007, 0x82)  \
                           Notify(\_SB.SCK3.C008, 0x82)  \
                           Notify(\_SB.SCK3.C009, 0x82)  \
                           Notify(\_SB.SCK3.C00A, 0x82)  \
                           Notify(\_SB.SCK3.C00B, 0x82)  \
                           Notify(\_SB.SCK3.C00C, 0x82)  \
                           Notify(\_SB.SCK3.C00D, 0x82)  \
                           Notify(\_SB.SCK3.C00E, 0x82)  \
                           Notify(\_SB.SCK3.C00F, 0x82)  \
                           Notify(\_SB.SCK3.C010, 0x82)  \
                           Notify(\_SB.SCK3.C011, 0x82)  \
                           Notify(\_SB.SCK3.C012, 0x82)  \
                           Notify(\_SB.SCK3.C013, 0x82)  \
                           Notify(\_SB.SCK3.C014, 0x82)  \
                           Notify(\_SB.SCK3.C015, 0x82)  \
                           Notify(\_SB.SCK3.C016, 0x82)  \
                           Notify(\_SB.SCK3.C017, 0x82)  \
                           Notify(\_SB.SCK3.C018, 0x82)  \
                           Notify(\_SB.SCK3.C019, 0x82)  \
                           Notify(\_SB.SCK3.C01A, 0x82)  \
                           Notify(\_SB.SCK3.C01B, 0x82)  \
                           Notify(\_SB.SCK3.C01C, 0x82)  \
                           Notify(\_SB.SCK3.C01D, 0x82)  \
                           Notify(\_SB.SCK3.C01E, 0x82)  \
                           Notify(\_SB.SCK3.C01F, 0x82)  \
                           Notify(\_SB.SCK3.C020, 0x82)  \
                           Notify(\_SB.SCK3.C021, 0x82)  \
                           Notify(\_SB.SCK3.C022, 0x82)  \
                           Notify(\_SB.SCK3.C023, 0x82)  \
                           Notify(\_SB.SCK3.C024, 0x82)  \
                           Notify(\_SB.SCK3.C025, 0x82)  \
                           Notify(\_SB.SCK3.C026, 0x82)  \
                           Notify(\_SB.SCK3.C027, 0x82)  \
                           Notify(\_SB.SCK3.C028, 0x82)  \
                           Notify(\_SB.SCK3.C029, 0x82)  \
                           Notify(\_SB.SCK3.C02A, 0x82)  \
                           Notify(\_SB.SCK3.C02B, 0x82)  \
                           Notify(\_SB.SCK3.C02C, 0x82)  \
                           Notify(\_SB.SCK3.C02D, 0x82)  \
                           Notify(\_SB.SCK3.C02E, 0x82)  \
                           Notify(\_SB.SCK3.C02F, 0x82)  \
                           Notify(\_SB.SCK3.C030, 0x82)  \
                           Notify(\_SB.SCK3.C031, 0x82)  \
                           Notify(\_SB.SCK3.C032, 0x82)  \
                           Notify(\_SB.SCK3.C033, 0x82)  \
                           Notify(\_SB.SCK3.C034, 0x82)  \
                           Notify(\_SB.SCK3.C035, 0x82)  \
                           Notify(\_SB.SCK3.C036, 0x82)  \
                           Notify(\_SB.SCK3.C037, 0x82)  \
                           Notify(\_SB.SCK3.C038, 0x82)  \
                           Notify(\_SB.SCK3.C039, 0x82)  \
                           Notify(\_SB.SCK3.C03A, 0x82)  \
                           Notify(\_SB.SCK3.C03B, 0x82)  \
                           Notify(\_SB.SCK3.C03C, 0x82)  \
                           Notify(\_SB.SCK3.C03D, 0x82)  \
                           Notify(\_SB.SCK3.C03E, 0x82)  \
                           Notify(\_SB.SCK3.C03F, 0x82)  \
                           Notify(\_SB.SCK3.C040, 0x82)  \
                           Notify(\_SB.SCK3.C041, 0x82)  \
                           Notify(\_SB.SCK3.C042, 0x82)  \
                           Notify(\_SB.SCK3.C043, 0x82)  \
                           Notify(\_SB.SCK3.C044, 0x82)  \
                           Notify(\_SB.SCK3.C045, 0x82)  \
                           Notify(\_SB.SCK3.C046, 0x82)  \
                           Notify(\_SB.SCK3.C047, 0x82)  \
                           Notify(\_SB.SCK3.C048, 0x82)  \
                           Notify(\_SB.SCK3.C049, 0x82)  \
                           Notify(\_SB.SCK3.C04A, 0x82)  \
                           Notify(\_SB.SCK3.C04B, 0x82)  \
                           Notify(\_SB.SCK3.C04C, 0x82)  \
                           Notify(\_SB.SCK3.C04D, 0x82)  \
                           Notify(\_SB.SCK3.C04E, 0x82)  \
                           Notify(\_SB.SCK3.C04F, 0x82)  \
                           Notify(\_SB.SCK3.C050, 0x82)  \
                           Notify(\_SB.SCK3.C051, 0x82)  \
                           Notify(\_SB.SCK3.C052, 0x82)  \
                           Notify(\_SB.SCK3.C053, 0x82)  \
                           Notify(\_SB.SCK3.C054, 0x82)  \
                           Notify(\_SB.SCK3.C055, 0x82)  \
                           Notify(\_SB.SCK3.C056, 0x82)  \
                           Notify(\_SB.SCK3.C057, 0x82)  \
                           Notify(\_SB.SCK3.C058, 0x82)  \
                           Notify(\_SB.SCK3.C059, 0x82)  \
                           Notify(\_SB.SCK3.C05A, 0x82)  \
                           Notify(\_SB.SCK3.C05B, 0x82)  \
                           Notify(\_SB.SCK3.C05C, 0x82)  \
                           Notify(\_SB.SCK3.C05D, 0x82)  \
                           Notify(\_SB.SCK3.C05E, 0x82)  \
                           Notify(\_SB.SCK3.C05F, 0x82)  \
                           Notify(\_SB.SCK3.C060, 0x82)  \
                           Notify(\_SB.SCK3.C061, 0x82)  \
                           Notify(\_SB.SCK3.C062, 0x82)  \
                           Notify(\_SB.SCK3.C063, 0x82)  \
                           Notify(\_SB.SCK3.C064, 0x82)  \
                           Notify(\_SB.SCK3.C065, 0x82)  \
                           Notify(\_SB.SCK3.C066, 0x82)  \
                           Notify(\_SB.SCK3.C067, 0x82)  \
                           Notify(\_SB.SCK3.C068, 0x82)  \
                           Notify(\_SB.SCK3.C069, 0x82)  \
                           Notify(\_SB.SCK3.C06A, 0x82)  \
                           Notify(\_SB.SCK3.C06B, 0x82)  \
                           Notify(\_SB.SCK3.C06C, 0x82)  \
                           Notify(\_SB.SCK3.C06D, 0x82)  \
                           Notify(\_SB.SCK3.C06E, 0x82)  \
                           Notify(\_SB.SCK3.C06F, 0x82)  \
                           Notify(\_SB.SCK3.C070, 0x82)  \
                           Notify(\_SB.SCK3.C071, 0x82)  \
                           Notify(\_SB.SCK3.C072, 0x82)  \
                           Notify(\_SB.SCK3.C073, 0x82)  \
                           Notify(\_SB.SCK3.C074, 0x82)  \
                           Notify(\_SB.SCK3.C075, 0x82)  \
                           Notify(\_SB.SCK3.C076, 0x82)  \
                           Notify(\_SB.SCK3.C077, 0x82)  \
                           Notify(\_SB.SCK3.C078, 0x82)  \
                           Notify(\_SB.SCK3.C079, 0x82)  \
                           Notify(\_SB.SCK3.C07A, 0x82)  \
                           Notify(\_SB.SCK3.C07B, 0x82)  \
                           Notify(\_SB.SCK3.C07C, 0x82)  \
                           Notify(\_SB.SCK3.C07D, 0x82)  \
                           Notify(\_SB.SCK3.C07E, 0x82)  \
                           Notify(\_SB.SCK3.C07F, 0x82)
#else
#define Notify80Socket3()
#define Notify82Socket3()
#endif

#if MAX_SOCKET > 4
#define Notify80Socket4()  Notify(\_SB.SCK4.C000, 0x80)  \
                           Notify(\_SB.SCK4.C001, 0x80)  \
                           Notify(\_SB.SCK4.C002, 0x80)  \
                           Notify(\_SB.SCK4.C003, 0x80)  \
                           Notify(\_SB.SCK4.C004, 0x80)  \
                           Notify(\_SB.SCK4.C005, 0x80)  \
                           Notify(\_SB.SCK4.C006, 0x80)  \
                           Notify(\_SB.SCK4.C007, 0x80)  \
                           Notify(\_SB.SCK4.C008, 0x80)  \
                           Notify(\_SB.SCK4.C009, 0x80)  \
                           Notify(\_SB.SCK4.C00A, 0x80)  \
                           Notify(\_SB.SCK4.C00B, 0x80)  \
                           Notify(\_SB.SCK4.C00C, 0x80)  \
                           Notify(\_SB.SCK4.C00D, 0x80)  \
                           Notify(\_SB.SCK4.C00E, 0x80)  \
                           Notify(\_SB.SCK4.C00F, 0x80)  \
                           Notify(\_SB.SCK4.C010, 0x80)  \
                           Notify(\_SB.SCK4.C011, 0x80)  \
                           Notify(\_SB.SCK4.C012, 0x80)  \
                           Notify(\_SB.SCK4.C013, 0x80)  \
                           Notify(\_SB.SCK4.C014, 0x80)  \
                           Notify(\_SB.SCK4.C015, 0x80)  \
                           Notify(\_SB.SCK4.C016, 0x80)  \
                           Notify(\_SB.SCK4.C017, 0x80)  \
                           Notify(\_SB.SCK4.C018, 0x80)  \
                           Notify(\_SB.SCK4.C019, 0x80)  \
                           Notify(\_SB.SCK4.C01A, 0x80)  \
                           Notify(\_SB.SCK4.C01B, 0x80)  \
                           Notify(\_SB.SCK4.C01C, 0x80)  \
                           Notify(\_SB.SCK4.C01D, 0x80)  \
                           Notify(\_SB.SCK4.C01E, 0x80)  \
                           Notify(\_SB.SCK4.C01F, 0x80)  \
                           Notify(\_SB.SCK4.C020, 0x80)  \
                           Notify(\_SB.SCK4.C021, 0x80)  \
                           Notify(\_SB.SCK4.C022, 0x80)  \
                           Notify(\_SB.SCK4.C023, 0x80)  \
                           Notify(\_SB.SCK4.C024, 0x80)  \
                           Notify(\_SB.SCK4.C025, 0x80)  \
                           Notify(\_SB.SCK4.C026, 0x80)  \
                           Notify(\_SB.SCK4.C027, 0x80)  \
                           Notify(\_SB.SCK4.C028, 0x80)  \
                           Notify(\_SB.SCK4.C029, 0x80)  \
                           Notify(\_SB.SCK4.C02A, 0x80)  \
                           Notify(\_SB.SCK4.C02B, 0x80)  \
                           Notify(\_SB.SCK4.C02C, 0x80)  \
                           Notify(\_SB.SCK4.C02D, 0x80)  \
                           Notify(\_SB.SCK4.C02E, 0x80)  \
                           Notify(\_SB.SCK4.C02F, 0x80)  \
                           Notify(\_SB.SCK4.C030, 0x80)  \
                           Notify(\_SB.SCK4.C031, 0x80)  \
                           Notify(\_SB.SCK4.C032, 0x80)  \
                           Notify(\_SB.SCK4.C033, 0x80)  \
                           Notify(\_SB.SCK4.C034, 0x80)  \
                           Notify(\_SB.SCK4.C035, 0x80)  \
                           Notify(\_SB.SCK4.C036, 0x80)  \
                           Notify(\_SB.SCK4.C037, 0x80)  \
                           Notify(\_SB.SCK4.C038, 0x80)  \
                           Notify(\_SB.SCK4.C039, 0x80)  \
                           Notify(\_SB.SCK4.C03A, 0x80)  \
                           Notify(\_SB.SCK4.C03B, 0x80)  \
                           Notify(\_SB.SCK4.C03C, 0x80)  \
                           Notify(\_SB.SCK4.C03D, 0x80)  \
                           Notify(\_SB.SCK4.C03E, 0x80)  \
                           Notify(\_SB.SCK4.C03F, 0x80)  \
                           Notify(\_SB.SCK4.C040, 0x80)  \
                           Notify(\_SB.SCK4.C041, 0x80)  \
                           Notify(\_SB.SCK4.C042, 0x80)  \
                           Notify(\_SB.SCK4.C043, 0x80)  \
                           Notify(\_SB.SCK4.C044, 0x80)  \
                           Notify(\_SB.SCK4.C045, 0x80)  \
                           Notify(\_SB.SCK4.C046, 0x80)  \
                           Notify(\_SB.SCK4.C047, 0x80)  \
                           Notify(\_SB.SCK4.C048, 0x80)  \
                           Notify(\_SB.SCK4.C049, 0x80)  \
                           Notify(\_SB.SCK4.C04A, 0x80)  \
                           Notify(\_SB.SCK4.C04B, 0x80)  \
                           Notify(\_SB.SCK4.C04C, 0x80)  \
                           Notify(\_SB.SCK4.C04D, 0x80)  \
                           Notify(\_SB.SCK4.C04E, 0x80)  \
                           Notify(\_SB.SCK4.C04F, 0x80)  \
                           Notify(\_SB.SCK4.C050, 0x80)  \
                           Notify(\_SB.SCK4.C051, 0x80)  \
                           Notify(\_SB.SCK4.C052, 0x80)  \
                           Notify(\_SB.SCK4.C053, 0x80)  \
                           Notify(\_SB.SCK4.C054, 0x80)  \
                           Notify(\_SB.SCK4.C055, 0x80)  \
                           Notify(\_SB.SCK4.C056, 0x80)  \
                           Notify(\_SB.SCK4.C057, 0x80)  \
                           Notify(\_SB.SCK4.C058, 0x80)  \
                           Notify(\_SB.SCK4.C059, 0x80)  \
                           Notify(\_SB.SCK4.C05A, 0x80)  \
                           Notify(\_SB.SCK4.C05B, 0x80)  \
                           Notify(\_SB.SCK4.C05C, 0x80)  \
                           Notify(\_SB.SCK4.C05D, 0x80)  \
                           Notify(\_SB.SCK4.C05E, 0x80)  \
                           Notify(\_SB.SCK4.C05F, 0x80)  \
                           Notify(\_SB.SCK4.C060, 0x80)  \
                           Notify(\_SB.SCK4.C061, 0x80)  \
                           Notify(\_SB.SCK4.C062, 0x80)  \
                           Notify(\_SB.SCK4.C063, 0x80)  \
                           Notify(\_SB.SCK4.C064, 0x80)  \
                           Notify(\_SB.SCK4.C065, 0x80)  \
                           Notify(\_SB.SCK4.C066, 0x80)  \
                           Notify(\_SB.SCK4.C067, 0x80)  \
                           Notify(\_SB.SCK4.C068, 0x80)  \
                           Notify(\_SB.SCK4.C069, 0x80)  \
                           Notify(\_SB.SCK4.C06A, 0x80)  \
                           Notify(\_SB.SCK4.C06B, 0x80)  \
                           Notify(\_SB.SCK4.C06C, 0x80)  \
                           Notify(\_SB.SCK4.C06D, 0x80)  \
                           Notify(\_SB.SCK4.C06E, 0x80)  \
                           Notify(\_SB.SCK4.C06F, 0x80)  \
                           Notify(\_SB.SCK4.C070, 0x80)  \
                           Notify(\_SB.SCK4.C071, 0x80)  \
                           Notify(\_SB.SCK4.C072, 0x80)  \
                           Notify(\_SB.SCK4.C073, 0x80)  \
                           Notify(\_SB.SCK4.C074, 0x80)  \
                           Notify(\_SB.SCK4.C075, 0x80)  \
                           Notify(\_SB.SCK4.C076, 0x80)  \
                           Notify(\_SB.SCK4.C077, 0x80)  \
                           Notify(\_SB.SCK4.C078, 0x80)  \
                           Notify(\_SB.SCK4.C079, 0x80)  \
                           Notify(\_SB.SCK4.C07A, 0x80)  \
                           Notify(\_SB.SCK4.C07B, 0x80)  \
                           Notify(\_SB.SCK4.C07C, 0x80)  \
                           Notify(\_SB.SCK4.C07D, 0x80)  \
                           Notify(\_SB.SCK4.C07E, 0x80)  \
                           Notify(\_SB.SCK4.C07F, 0x80)

#define Notify82Socket4()  Notify(\_SB.SCK4.C000, 0x82)  \
                           Notify(\_SB.SCK4.C001, 0x82)  \
                           Notify(\_SB.SCK4.C002, 0x82)  \
                           Notify(\_SB.SCK4.C003, 0x82)  \
                           Notify(\_SB.SCK4.C004, 0x82)  \
                           Notify(\_SB.SCK4.C005, 0x82)  \
                           Notify(\_SB.SCK4.C006, 0x82)  \
                           Notify(\_SB.SCK4.C007, 0x82)  \
                           Notify(\_SB.SCK4.C008, 0x82)  \
                           Notify(\_SB.SCK4.C009, 0x82)  \
                           Notify(\_SB.SCK4.C00A, 0x82)  \
                           Notify(\_SB.SCK4.C00B, 0x82)  \
                           Notify(\_SB.SCK4.C00C, 0x82)  \
                           Notify(\_SB.SCK4.C00D, 0x82)  \
                           Notify(\_SB.SCK4.C00E, 0x82)  \
                           Notify(\_SB.SCK4.C00F, 0x82)  \
                           Notify(\_SB.SCK4.C010, 0x82)  \
                           Notify(\_SB.SCK4.C011, 0x82)  \
                           Notify(\_SB.SCK4.C012, 0x82)  \
                           Notify(\_SB.SCK4.C013, 0x82)  \
                           Notify(\_SB.SCK4.C014, 0x82)  \
                           Notify(\_SB.SCK4.C015, 0x82)  \
                           Notify(\_SB.SCK4.C016, 0x82)  \
                           Notify(\_SB.SCK4.C017, 0x82)  \
                           Notify(\_SB.SCK4.C018, 0x82)  \
                           Notify(\_SB.SCK4.C019, 0x82)  \
                           Notify(\_SB.SCK4.C01A, 0x82)  \
                           Notify(\_SB.SCK4.C01B, 0x82)  \
                           Notify(\_SB.SCK4.C01C, 0x82)  \
                           Notify(\_SB.SCK4.C01D, 0x82)  \
                           Notify(\_SB.SCK4.C01E, 0x82)  \
                           Notify(\_SB.SCK4.C01F, 0x82)  \
                           Notify(\_SB.SCK4.C020, 0x82)  \
                           Notify(\_SB.SCK4.C021, 0x82)  \
                           Notify(\_SB.SCK4.C022, 0x82)  \
                           Notify(\_SB.SCK4.C023, 0x82)  \
                           Notify(\_SB.SCK4.C024, 0x82)  \
                           Notify(\_SB.SCK4.C025, 0x82)  \
                           Notify(\_SB.SCK4.C026, 0x82)  \
                           Notify(\_SB.SCK4.C027, 0x82)  \
                           Notify(\_SB.SCK4.C028, 0x82)  \
                           Notify(\_SB.SCK4.C029, 0x82)  \
                           Notify(\_SB.SCK4.C02A, 0x82)  \
                           Notify(\_SB.SCK4.C02B, 0x82)  \
                           Notify(\_SB.SCK4.C02C, 0x82)  \
                           Notify(\_SB.SCK4.C02D, 0x82)  \
                           Notify(\_SB.SCK4.C02E, 0x82)  \
                           Notify(\_SB.SCK4.C02F, 0x82)  \
                           Notify(\_SB.SCK4.C030, 0x82)  \
                           Notify(\_SB.SCK4.C031, 0x82)  \
                           Notify(\_SB.SCK4.C032, 0x82)  \
                           Notify(\_SB.SCK4.C033, 0x82)  \
                           Notify(\_SB.SCK4.C034, 0x82)  \
                           Notify(\_SB.SCK4.C035, 0x82)  \
                           Notify(\_SB.SCK4.C036, 0x82)  \
                           Notify(\_SB.SCK4.C037, 0x82)  \
                           Notify(\_SB.SCK4.C038, 0x82)  \
                           Notify(\_SB.SCK4.C039, 0x82)  \
                           Notify(\_SB.SCK4.C03A, 0x82)  \
                           Notify(\_SB.SCK4.C03B, 0x82)  \
                           Notify(\_SB.SCK4.C03C, 0x82)  \
                           Notify(\_SB.SCK4.C03D, 0x82)  \
                           Notify(\_SB.SCK4.C03E, 0x82)  \
                           Notify(\_SB.SCK4.C03F, 0x82)  \
                           Notify(\_SB.SCK4.C040, 0x82)  \
                           Notify(\_SB.SCK4.C041, 0x82)  \
                           Notify(\_SB.SCK4.C042, 0x82)  \
                           Notify(\_SB.SCK4.C043, 0x82)  \
                           Notify(\_SB.SCK4.C044, 0x82)  \
                           Notify(\_SB.SCK4.C045, 0x82)  \
                           Notify(\_SB.SCK4.C046, 0x82)  \
                           Notify(\_SB.SCK4.C047, 0x82)  \
                           Notify(\_SB.SCK4.C048, 0x82)  \
                           Notify(\_SB.SCK4.C049, 0x82)  \
                           Notify(\_SB.SCK4.C04A, 0x82)  \
                           Notify(\_SB.SCK4.C04B, 0x82)  \
                           Notify(\_SB.SCK4.C04C, 0x82)  \
                           Notify(\_SB.SCK4.C04D, 0x82)  \
                           Notify(\_SB.SCK4.C04E, 0x82)  \
                           Notify(\_SB.SCK4.C04F, 0x82)  \
                           Notify(\_SB.SCK4.C050, 0x82)  \
                           Notify(\_SB.SCK4.C051, 0x82)  \
                           Notify(\_SB.SCK4.C052, 0x82)  \
                           Notify(\_SB.SCK4.C053, 0x82)  \
                           Notify(\_SB.SCK4.C054, 0x82)  \
                           Notify(\_SB.SCK4.C055, 0x82)  \
                           Notify(\_SB.SCK4.C056, 0x82)  \
                           Notify(\_SB.SCK4.C057, 0x82)  \
                           Notify(\_SB.SCK4.C058, 0x82)  \
                           Notify(\_SB.SCK4.C059, 0x82)  \
                           Notify(\_SB.SCK4.C05A, 0x82)  \
                           Notify(\_SB.SCK4.C05B, 0x82)  \
                           Notify(\_SB.SCK4.C05C, 0x82)  \
                           Notify(\_SB.SCK4.C05D, 0x82)  \
                           Notify(\_SB.SCK4.C05E, 0x82)  \
                           Notify(\_SB.SCK4.C05F, 0x82)  \
                           Notify(\_SB.SCK4.C060, 0x82)  \
                           Notify(\_SB.SCK4.C061, 0x82)  \
                           Notify(\_SB.SCK4.C062, 0x82)  \
                           Notify(\_SB.SCK4.C063, 0x82)  \
                           Notify(\_SB.SCK4.C064, 0x82)  \
                           Notify(\_SB.SCK4.C065, 0x82)  \
                           Notify(\_SB.SCK4.C066, 0x82)  \
                           Notify(\_SB.SCK4.C067, 0x82)  \
                           Notify(\_SB.SCK4.C068, 0x82)  \
                           Notify(\_SB.SCK4.C069, 0x82)  \
                           Notify(\_SB.SCK4.C06A, 0x82)  \
                           Notify(\_SB.SCK4.C06B, 0x82)  \
                           Notify(\_SB.SCK4.C06C, 0x82)  \
                           Notify(\_SB.SCK4.C06D, 0x82)  \
                           Notify(\_SB.SCK4.C06E, 0x82)  \
                           Notify(\_SB.SCK4.C06F, 0x82)  \
                           Notify(\_SB.SCK4.C070, 0x82)  \
                           Notify(\_SB.SCK4.C071, 0x82)  \
                           Notify(\_SB.SCK4.C072, 0x82)  \
                           Notify(\_SB.SCK4.C073, 0x82)  \
                           Notify(\_SB.SCK4.C074, 0x82)  \
                           Notify(\_SB.SCK4.C075, 0x82)  \
                           Notify(\_SB.SCK4.C076, 0x82)  \
                           Notify(\_SB.SCK4.C077, 0x82)  \
                           Notify(\_SB.SCK4.C078, 0x82)  \
                           Notify(\_SB.SCK4.C079, 0x82)  \
                           Notify(\_SB.SCK4.C07A, 0x82)  \
                           Notify(\_SB.SCK4.C07B, 0x82)  \
                           Notify(\_SB.SCK4.C07C, 0x82)  \
                           Notify(\_SB.SCK4.C07D, 0x82)  \
                           Notify(\_SB.SCK4.C07E, 0x82)  \
                           Notify(\_SB.SCK4.C07F, 0x82)
#else
#define Notify80Socket4()
#define Notify82Socket4()
#endif

#if MAX_SOCKET > 5
#define Notify80Socket5()  Notify(\_SB.SCK5.C000, 0x80)  \
                           Notify(\_SB.SCK5.C001, 0x80)  \
                           Notify(\_SB.SCK5.C002, 0x80)  \
                           Notify(\_SB.SCK5.C003, 0x80)  \
                           Notify(\_SB.SCK5.C004, 0x80)  \
                           Notify(\_SB.SCK5.C005, 0x80)  \
                           Notify(\_SB.SCK5.C006, 0x80)  \
                           Notify(\_SB.SCK5.C007, 0x80)  \
                           Notify(\_SB.SCK5.C008, 0x80)  \
                           Notify(\_SB.SCK5.C009, 0x80)  \
                           Notify(\_SB.SCK5.C00A, 0x80)  \
                           Notify(\_SB.SCK5.C00B, 0x80)  \
                           Notify(\_SB.SCK5.C00C, 0x80)  \
                           Notify(\_SB.SCK5.C00D, 0x80)  \
                           Notify(\_SB.SCK5.C00E, 0x80)  \
                           Notify(\_SB.SCK5.C00F, 0x80)  \
                           Notify(\_SB.SCK5.C010, 0x80)  \
                           Notify(\_SB.SCK5.C011, 0x80)  \
                           Notify(\_SB.SCK5.C012, 0x80)  \
                           Notify(\_SB.SCK5.C013, 0x80)  \
                           Notify(\_SB.SCK5.C014, 0x80)  \
                           Notify(\_SB.SCK5.C015, 0x80)  \
                           Notify(\_SB.SCK5.C016, 0x80)  \
                           Notify(\_SB.SCK5.C017, 0x80)  \
                           Notify(\_SB.SCK5.C018, 0x80)  \
                           Notify(\_SB.SCK5.C019, 0x80)  \
                           Notify(\_SB.SCK5.C01A, 0x80)  \
                           Notify(\_SB.SCK5.C01B, 0x80)  \
                           Notify(\_SB.SCK5.C01C, 0x80)  \
                           Notify(\_SB.SCK5.C01D, 0x80)  \
                           Notify(\_SB.SCK5.C01E, 0x80)  \
                           Notify(\_SB.SCK5.C01F, 0x80)  \
                           Notify(\_SB.SCK5.C020, 0x80)  \
                           Notify(\_SB.SCK5.C021, 0x80)  \
                           Notify(\_SB.SCK5.C022, 0x80)  \
                           Notify(\_SB.SCK5.C023, 0x80)  \
                           Notify(\_SB.SCK5.C024, 0x80)  \
                           Notify(\_SB.SCK5.C025, 0x80)  \
                           Notify(\_SB.SCK5.C026, 0x80)  \
                           Notify(\_SB.SCK5.C027, 0x80)  \
                           Notify(\_SB.SCK5.C028, 0x80)  \
                           Notify(\_SB.SCK5.C029, 0x80)  \
                           Notify(\_SB.SCK5.C02A, 0x80)  \
                           Notify(\_SB.SCK5.C02B, 0x80)  \
                           Notify(\_SB.SCK5.C02C, 0x80)  \
                           Notify(\_SB.SCK5.C02D, 0x80)  \
                           Notify(\_SB.SCK5.C02E, 0x80)  \
                           Notify(\_SB.SCK5.C02F, 0x80)  \
                           Notify(\_SB.SCK5.C030, 0x80)  \
                           Notify(\_SB.SCK5.C031, 0x80)  \
                           Notify(\_SB.SCK5.C032, 0x80)  \
                           Notify(\_SB.SCK5.C033, 0x80)  \
                           Notify(\_SB.SCK5.C034, 0x80)  \
                           Notify(\_SB.SCK5.C035, 0x80)  \
                           Notify(\_SB.SCK5.C036, 0x80)  \
                           Notify(\_SB.SCK5.C037, 0x80)  \
                           Notify(\_SB.SCK5.C038, 0x80)  \
                           Notify(\_SB.SCK5.C039, 0x80)  \
                           Notify(\_SB.SCK5.C03A, 0x80)  \
                           Notify(\_SB.SCK5.C03B, 0x80)  \
                           Notify(\_SB.SCK5.C03C, 0x80)  \
                           Notify(\_SB.SCK5.C03D, 0x80)  \
                           Notify(\_SB.SCK5.C03E, 0x80)  \
                           Notify(\_SB.SCK5.C03F, 0x80)  \
                           Notify(\_SB.SCK5.C040, 0x80)  \
                           Notify(\_SB.SCK5.C041, 0x80)  \
                           Notify(\_SB.SCK5.C042, 0x80)  \
                           Notify(\_SB.SCK5.C043, 0x80)  \
                           Notify(\_SB.SCK5.C044, 0x80)  \
                           Notify(\_SB.SCK5.C045, 0x80)  \
                           Notify(\_SB.SCK5.C046, 0x80)  \
                           Notify(\_SB.SCK5.C047, 0x80)  \
                           Notify(\_SB.SCK5.C048, 0x80)  \
                           Notify(\_SB.SCK5.C049, 0x80)  \
                           Notify(\_SB.SCK5.C04A, 0x80)  \
                           Notify(\_SB.SCK5.C04B, 0x80)  \
                           Notify(\_SB.SCK5.C04C, 0x80)  \
                           Notify(\_SB.SCK5.C04D, 0x80)  \
                           Notify(\_SB.SCK5.C04E, 0x80)  \
                           Notify(\_SB.SCK5.C04F, 0x80)  \
                           Notify(\_SB.SCK5.C050, 0x80)  \
                           Notify(\_SB.SCK5.C051, 0x80)  \
                           Notify(\_SB.SCK5.C052, 0x80)  \
                           Notify(\_SB.SCK5.C053, 0x80)  \
                           Notify(\_SB.SCK5.C054, 0x80)  \
                           Notify(\_SB.SCK5.C055, 0x80)  \
                           Notify(\_SB.SCK5.C056, 0x80)  \
                           Notify(\_SB.SCK5.C057, 0x80)  \
                           Notify(\_SB.SCK5.C058, 0x80)  \
                           Notify(\_SB.SCK5.C059, 0x80)  \
                           Notify(\_SB.SCK5.C05A, 0x80)  \
                           Notify(\_SB.SCK5.C05B, 0x80)  \
                           Notify(\_SB.SCK5.C05C, 0x80)  \
                           Notify(\_SB.SCK5.C05D, 0x80)  \
                           Notify(\_SB.SCK5.C05E, 0x80)  \
                           Notify(\_SB.SCK5.C05F, 0x80)  \
                           Notify(\_SB.SCK5.C060, 0x80)  \
                           Notify(\_SB.SCK5.C061, 0x80)  \
                           Notify(\_SB.SCK5.C062, 0x80)  \
                           Notify(\_SB.SCK5.C063, 0x80)  \
                           Notify(\_SB.SCK5.C064, 0x80)  \
                           Notify(\_SB.SCK5.C065, 0x80)  \
                           Notify(\_SB.SCK5.C066, 0x80)  \
                           Notify(\_SB.SCK5.C067, 0x80)  \
                           Notify(\_SB.SCK5.C068, 0x80)  \
                           Notify(\_SB.SCK5.C069, 0x80)  \
                           Notify(\_SB.SCK5.C06A, 0x80)  \
                           Notify(\_SB.SCK5.C06B, 0x80)  \
                           Notify(\_SB.SCK5.C06C, 0x80)  \
                           Notify(\_SB.SCK5.C06D, 0x80)  \
                           Notify(\_SB.SCK5.C06E, 0x80)  \
                           Notify(\_SB.SCK5.C06F, 0x80)  \
                           Notify(\_SB.SCK5.C070, 0x80)  \
                           Notify(\_SB.SCK5.C071, 0x80)  \
                           Notify(\_SB.SCK5.C072, 0x80)  \
                           Notify(\_SB.SCK5.C073, 0x80)  \
                           Notify(\_SB.SCK5.C074, 0x80)  \
                           Notify(\_SB.SCK5.C075, 0x80)  \
                           Notify(\_SB.SCK5.C076, 0x80)  \
                           Notify(\_SB.SCK5.C077, 0x80)  \
                           Notify(\_SB.SCK5.C078, 0x80)  \
                           Notify(\_SB.SCK5.C079, 0x80)  \
                           Notify(\_SB.SCK5.C07A, 0x80)  \
                           Notify(\_SB.SCK5.C07B, 0x80)  \
                           Notify(\_SB.SCK5.C07C, 0x80)  \
                           Notify(\_SB.SCK5.C07D, 0x80)  \
                           Notify(\_SB.SCK5.C07E, 0x80)  \
                           Notify(\_SB.SCK5.C07F, 0x80)

#define Notify82Socket5()  Notify(\_SB.SCK5.C000, 0x82)  \
                           Notify(\_SB.SCK5.C001, 0x82)  \
                           Notify(\_SB.SCK5.C002, 0x82)  \
                           Notify(\_SB.SCK5.C003, 0x82)  \
                           Notify(\_SB.SCK5.C004, 0x82)  \
                           Notify(\_SB.SCK5.C005, 0x82)  \
                           Notify(\_SB.SCK5.C006, 0x82)  \
                           Notify(\_SB.SCK5.C007, 0x82)  \
                           Notify(\_SB.SCK5.C008, 0x82)  \
                           Notify(\_SB.SCK5.C009, 0x82)  \
                           Notify(\_SB.SCK5.C00A, 0x82)  \
                           Notify(\_SB.SCK5.C00B, 0x82)  \
                           Notify(\_SB.SCK5.C00C, 0x82)  \
                           Notify(\_SB.SCK5.C00D, 0x82)  \
                           Notify(\_SB.SCK5.C00E, 0x82)  \
                           Notify(\_SB.SCK5.C00F, 0x82)  \
                           Notify(\_SB.SCK5.C010, 0x82)  \
                           Notify(\_SB.SCK5.C011, 0x82)  \
                           Notify(\_SB.SCK5.C012, 0x82)  \
                           Notify(\_SB.SCK5.C013, 0x82)  \
                           Notify(\_SB.SCK5.C014, 0x82)  \
                           Notify(\_SB.SCK5.C015, 0x82)  \
                           Notify(\_SB.SCK5.C016, 0x82)  \
                           Notify(\_SB.SCK5.C017, 0x82)  \
                           Notify(\_SB.SCK5.C018, 0x82)  \
                           Notify(\_SB.SCK5.C019, 0x82)  \
                           Notify(\_SB.SCK5.C01A, 0x82)  \
                           Notify(\_SB.SCK5.C01B, 0x82)  \
                           Notify(\_SB.SCK5.C01C, 0x82)  \
                           Notify(\_SB.SCK5.C01D, 0x82)  \
                           Notify(\_SB.SCK5.C01E, 0x82)  \
                           Notify(\_SB.SCK5.C01F, 0x82)  \
                           Notify(\_SB.SCK5.C020, 0x82)  \
                           Notify(\_SB.SCK5.C021, 0x82)  \
                           Notify(\_SB.SCK5.C022, 0x82)  \
                           Notify(\_SB.SCK5.C023, 0x82)  \
                           Notify(\_SB.SCK5.C024, 0x82)  \
                           Notify(\_SB.SCK5.C025, 0x82)  \
                           Notify(\_SB.SCK5.C026, 0x82)  \
                           Notify(\_SB.SCK5.C027, 0x82)  \
                           Notify(\_SB.SCK5.C028, 0x82)  \
                           Notify(\_SB.SCK5.C029, 0x82)  \
                           Notify(\_SB.SCK5.C02A, 0x82)  \
                           Notify(\_SB.SCK5.C02B, 0x82)  \
                           Notify(\_SB.SCK5.C02C, 0x82)  \
                           Notify(\_SB.SCK5.C02D, 0x82)  \
                           Notify(\_SB.SCK5.C02E, 0x82)  \
                           Notify(\_SB.SCK5.C02F, 0x82)  \
                           Notify(\_SB.SCK5.C030, 0x82)  \
                           Notify(\_SB.SCK5.C031, 0x82)  \
                           Notify(\_SB.SCK5.C032, 0x82)  \
                           Notify(\_SB.SCK5.C033, 0x82)  \
                           Notify(\_SB.SCK5.C034, 0x82)  \
                           Notify(\_SB.SCK5.C035, 0x82)  \
                           Notify(\_SB.SCK5.C036, 0x82)  \
                           Notify(\_SB.SCK5.C037, 0x82)  \
                           Notify(\_SB.SCK5.C038, 0x82)  \
                           Notify(\_SB.SCK5.C039, 0x82)  \
                           Notify(\_SB.SCK5.C03A, 0x82)  \
                           Notify(\_SB.SCK5.C03B, 0x82)  \
                           Notify(\_SB.SCK5.C03C, 0x82)  \
                           Notify(\_SB.SCK5.C03D, 0x82)  \
                           Notify(\_SB.SCK5.C03E, 0x82)  \
                           Notify(\_SB.SCK5.C03F, 0x82)  \
                           Notify(\_SB.SCK5.C040, 0x82)  \
                           Notify(\_SB.SCK5.C041, 0x82)  \
                           Notify(\_SB.SCK5.C042, 0x82)  \
                           Notify(\_SB.SCK5.C043, 0x82)  \
                           Notify(\_SB.SCK5.C044, 0x82)  \
                           Notify(\_SB.SCK5.C045, 0x82)  \
                           Notify(\_SB.SCK5.C046, 0x82)  \
                           Notify(\_SB.SCK5.C047, 0x82)  \
                           Notify(\_SB.SCK5.C048, 0x82)  \
                           Notify(\_SB.SCK5.C049, 0x82)  \
                           Notify(\_SB.SCK5.C04A, 0x82)  \
                           Notify(\_SB.SCK5.C04B, 0x82)  \
                           Notify(\_SB.SCK5.C04C, 0x82)  \
                           Notify(\_SB.SCK5.C04D, 0x82)  \
                           Notify(\_SB.SCK5.C04E, 0x82)  \
                           Notify(\_SB.SCK5.C04F, 0x82)  \
                           Notify(\_SB.SCK5.C050, 0x82)  \
                           Notify(\_SB.SCK5.C051, 0x82)  \
                           Notify(\_SB.SCK5.C052, 0x82)  \
                           Notify(\_SB.SCK5.C053, 0x82)  \
                           Notify(\_SB.SCK5.C054, 0x82)  \
                           Notify(\_SB.SCK5.C055, 0x82)  \
                           Notify(\_SB.SCK5.C056, 0x82)  \
                           Notify(\_SB.SCK5.C057, 0x82)  \
                           Notify(\_SB.SCK5.C058, 0x82)  \
                           Notify(\_SB.SCK5.C059, 0x82)  \
                           Notify(\_SB.SCK5.C05A, 0x82)  \
                           Notify(\_SB.SCK5.C05B, 0x82)  \
                           Notify(\_SB.SCK5.C05C, 0x82)  \
                           Notify(\_SB.SCK5.C05D, 0x82)  \
                           Notify(\_SB.SCK5.C05E, 0x82)  \
                           Notify(\_SB.SCK5.C05F, 0x82)  \
                           Notify(\_SB.SCK5.C060, 0x82)  \
                           Notify(\_SB.SCK5.C061, 0x82)  \
                           Notify(\_SB.SCK5.C062, 0x82)  \
                           Notify(\_SB.SCK5.C063, 0x82)  \
                           Notify(\_SB.SCK5.C064, 0x82)  \
                           Notify(\_SB.SCK5.C065, 0x82)  \
                           Notify(\_SB.SCK5.C066, 0x82)  \
                           Notify(\_SB.SCK5.C067, 0x82)  \
                           Notify(\_SB.SCK5.C068, 0x82)  \
                           Notify(\_SB.SCK5.C069, 0x82)  \
                           Notify(\_SB.SCK5.C06A, 0x82)  \
                           Notify(\_SB.SCK5.C06B, 0x82)  \
                           Notify(\_SB.SCK5.C06C, 0x82)  \
                           Notify(\_SB.SCK5.C06D, 0x82)  \
                           Notify(\_SB.SCK5.C06E, 0x82)  \
                           Notify(\_SB.SCK5.C06F, 0x82)  \
                           Notify(\_SB.SCK5.C070, 0x82)  \
                           Notify(\_SB.SCK5.C071, 0x82)  \
                           Notify(\_SB.SCK5.C072, 0x82)  \
                           Notify(\_SB.SCK5.C073, 0x82)  \
                           Notify(\_SB.SCK5.C074, 0x82)  \
                           Notify(\_SB.SCK5.C075, 0x82)  \
                           Notify(\_SB.SCK5.C076, 0x82)  \
                           Notify(\_SB.SCK5.C077, 0x82)  \
                           Notify(\_SB.SCK5.C078, 0x82)  \
                           Notify(\_SB.SCK5.C079, 0x82)  \
                           Notify(\_SB.SCK5.C07A, 0x82)  \
                           Notify(\_SB.SCK5.C07B, 0x82)  \
                           Notify(\_SB.SCK5.C07C, 0x82)  \
                           Notify(\_SB.SCK5.C07D, 0x82)  \
                           Notify(\_SB.SCK5.C07E, 0x82)  \
                           Notify(\_SB.SCK5.C07F, 0x82)
#else
#define Notify80Socket5()
#define Notify82Socket5()
#endif

#if MAX_SOCKET > 6
#define Notify80Socket6()  Notify(\_SB.SCK6.C000, 0x80)  \
                           Notify(\_SB.SCK6.C001, 0x80)  \
                           Notify(\_SB.SCK6.C002, 0x80)  \
                           Notify(\_SB.SCK6.C003, 0x80)  \
                           Notify(\_SB.SCK6.C004, 0x80)  \
                           Notify(\_SB.SCK6.C005, 0x80)  \
                           Notify(\_SB.SCK6.C006, 0x80)  \
                           Notify(\_SB.SCK6.C007, 0x80)  \
                           Notify(\_SB.SCK6.C008, 0x80)  \
                           Notify(\_SB.SCK6.C009, 0x80)  \
                           Notify(\_SB.SCK6.C00A, 0x80)  \
                           Notify(\_SB.SCK6.C00B, 0x80)  \
                           Notify(\_SB.SCK6.C00C, 0x80)  \
                           Notify(\_SB.SCK6.C00D, 0x80)  \
                           Notify(\_SB.SCK6.C00E, 0x80)  \
                           Notify(\_SB.SCK6.C00F, 0x80)  \
                           Notify(\_SB.SCK6.C010, 0x80)  \
                           Notify(\_SB.SCK6.C011, 0x80)  \
                           Notify(\_SB.SCK6.C012, 0x80)  \
                           Notify(\_SB.SCK6.C013, 0x80)  \
                           Notify(\_SB.SCK6.C014, 0x80)  \
                           Notify(\_SB.SCK6.C015, 0x80)  \
                           Notify(\_SB.SCK6.C016, 0x80)  \
                           Notify(\_SB.SCK6.C017, 0x80)  \
                           Notify(\_SB.SCK6.C018, 0x80)  \
                           Notify(\_SB.SCK6.C019, 0x80)  \
                           Notify(\_SB.SCK6.C01A, 0x80)  \
                           Notify(\_SB.SCK6.C01B, 0x80)  \
                           Notify(\_SB.SCK6.C01C, 0x80)  \
                           Notify(\_SB.SCK6.C01D, 0x80)  \
                           Notify(\_SB.SCK6.C01E, 0x80)  \
                           Notify(\_SB.SCK6.C01F, 0x80)  \
                           Notify(\_SB.SCK6.C020, 0x80)  \
                           Notify(\_SB.SCK6.C021, 0x80)  \
                           Notify(\_SB.SCK6.C022, 0x80)  \
                           Notify(\_SB.SCK6.C023, 0x80)  \
                           Notify(\_SB.SCK6.C024, 0x80)  \
                           Notify(\_SB.SCK6.C025, 0x80)  \
                           Notify(\_SB.SCK6.C026, 0x80)  \
                           Notify(\_SB.SCK6.C027, 0x80)  \
                           Notify(\_SB.SCK6.C028, 0x80)  \
                           Notify(\_SB.SCK6.C029, 0x80)  \
                           Notify(\_SB.SCK6.C02A, 0x80)  \
                           Notify(\_SB.SCK6.C02B, 0x80)  \
                           Notify(\_SB.SCK6.C02C, 0x80)  \
                           Notify(\_SB.SCK6.C02D, 0x80)  \
                           Notify(\_SB.SCK6.C02E, 0x80)  \
                           Notify(\_SB.SCK6.C02F, 0x80)  \
                           Notify(\_SB.SCK6.C030, 0x80)  \
                           Notify(\_SB.SCK6.C031, 0x80)  \
                           Notify(\_SB.SCK6.C032, 0x80)  \
                           Notify(\_SB.SCK6.C033, 0x80)  \
                           Notify(\_SB.SCK6.C034, 0x80)  \
                           Notify(\_SB.SCK6.C035, 0x80)  \
                           Notify(\_SB.SCK6.C036, 0x80)  \
                           Notify(\_SB.SCK6.C037, 0x80)  \
                           Notify(\_SB.SCK6.C038, 0x80)  \
                           Notify(\_SB.SCK6.C039, 0x80)  \
                           Notify(\_SB.SCK6.C03A, 0x80)  \
                           Notify(\_SB.SCK6.C03B, 0x80)  \
                           Notify(\_SB.SCK6.C03C, 0x80)  \
                           Notify(\_SB.SCK6.C03D, 0x80)  \
                           Notify(\_SB.SCK6.C03E, 0x80)  \
                           Notify(\_SB.SCK6.C03F, 0x80)  \
                           Notify(\_SB.SCK6.C040, 0x80)  \
                           Notify(\_SB.SCK6.C041, 0x80)  \
                           Notify(\_SB.SCK6.C042, 0x80)  \
                           Notify(\_SB.SCK6.C043, 0x80)  \
                           Notify(\_SB.SCK6.C044, 0x80)  \
                           Notify(\_SB.SCK6.C045, 0x80)  \
                           Notify(\_SB.SCK6.C046, 0x80)  \
                           Notify(\_SB.SCK6.C047, 0x80)  \
                           Notify(\_SB.SCK6.C048, 0x80)  \
                           Notify(\_SB.SCK6.C049, 0x80)  \
                           Notify(\_SB.SCK6.C04A, 0x80)  \
                           Notify(\_SB.SCK6.C04B, 0x80)  \
                           Notify(\_SB.SCK6.C04C, 0x80)  \
                           Notify(\_SB.SCK6.C04D, 0x80)  \
                           Notify(\_SB.SCK6.C04E, 0x80)  \
                           Notify(\_SB.SCK6.C04F, 0x80)  \
                           Notify(\_SB.SCK6.C050, 0x80)  \
                           Notify(\_SB.SCK6.C051, 0x80)  \
                           Notify(\_SB.SCK6.C052, 0x80)  \
                           Notify(\_SB.SCK6.C053, 0x80)  \
                           Notify(\_SB.SCK6.C054, 0x80)  \
                           Notify(\_SB.SCK6.C055, 0x80)  \
                           Notify(\_SB.SCK6.C056, 0x80)  \
                           Notify(\_SB.SCK6.C057, 0x80)  \
                           Notify(\_SB.SCK6.C058, 0x80)  \
                           Notify(\_SB.SCK6.C059, 0x80)  \
                           Notify(\_SB.SCK6.C05A, 0x80)  \
                           Notify(\_SB.SCK6.C05B, 0x80)  \
                           Notify(\_SB.SCK6.C05C, 0x80)  \
                           Notify(\_SB.SCK6.C05D, 0x80)  \
                           Notify(\_SB.SCK6.C05E, 0x80)  \
                           Notify(\_SB.SCK6.C05F, 0x80)  \
                           Notify(\_SB.SCK6.C060, 0x80)  \
                           Notify(\_SB.SCK6.C061, 0x80)  \
                           Notify(\_SB.SCK6.C062, 0x80)  \
                           Notify(\_SB.SCK6.C063, 0x80)  \
                           Notify(\_SB.SCK6.C064, 0x80)  \
                           Notify(\_SB.SCK6.C065, 0x80)  \
                           Notify(\_SB.SCK6.C066, 0x80)  \
                           Notify(\_SB.SCK6.C067, 0x80)  \
                           Notify(\_SB.SCK6.C068, 0x80)  \
                           Notify(\_SB.SCK6.C069, 0x80)  \
                           Notify(\_SB.SCK6.C06A, 0x80)  \
                           Notify(\_SB.SCK6.C06B, 0x80)  \
                           Notify(\_SB.SCK6.C06C, 0x80)  \
                           Notify(\_SB.SCK6.C06D, 0x80)  \
                           Notify(\_SB.SCK6.C06E, 0x80)  \
                           Notify(\_SB.SCK6.C06F, 0x80)  \
                           Notify(\_SB.SCK6.C070, 0x80)  \
                           Notify(\_SB.SCK6.C071, 0x80)  \
                           Notify(\_SB.SCK6.C072, 0x80)  \
                           Notify(\_SB.SCK6.C073, 0x80)  \
                           Notify(\_SB.SCK6.C074, 0x80)  \
                           Notify(\_SB.SCK6.C075, 0x80)  \
                           Notify(\_SB.SCK6.C076, 0x80)  \
                           Notify(\_SB.SCK6.C077, 0x80)  \
                           Notify(\_SB.SCK6.C078, 0x80)  \
                           Notify(\_SB.SCK6.C079, 0x80)  \
                           Notify(\_SB.SCK6.C07A, 0x80)  \
                           Notify(\_SB.SCK6.C07B, 0x80)  \
                           Notify(\_SB.SCK6.C07C, 0x80)  \
                           Notify(\_SB.SCK6.C07D, 0x80)  \
                           Notify(\_SB.SCK6.C07E, 0x80)  \
                           Notify(\_SB.SCK6.C07F, 0x80)

#define Notify82Socket6()  Notify(\_SB.SCK6.C000, 0x82)  \
                           Notify(\_SB.SCK6.C001, 0x82)  \
                           Notify(\_SB.SCK6.C002, 0x82)  \
                           Notify(\_SB.SCK6.C003, 0x82)  \
                           Notify(\_SB.SCK6.C004, 0x82)  \
                           Notify(\_SB.SCK6.C005, 0x82)  \
                           Notify(\_SB.SCK6.C006, 0x82)  \
                           Notify(\_SB.SCK6.C007, 0x82)  \
                           Notify(\_SB.SCK6.C008, 0x82)  \
                           Notify(\_SB.SCK6.C009, 0x82)  \
                           Notify(\_SB.SCK6.C00A, 0x82)  \
                           Notify(\_SB.SCK6.C00B, 0x82)  \
                           Notify(\_SB.SCK6.C00C, 0x82)  \
                           Notify(\_SB.SCK6.C00D, 0x82)  \
                           Notify(\_SB.SCK6.C00E, 0x82)  \
                           Notify(\_SB.SCK6.C00F, 0x82)  \
                           Notify(\_SB.SCK6.C010, 0x82)  \
                           Notify(\_SB.SCK6.C011, 0x82)  \
                           Notify(\_SB.SCK6.C012, 0x82)  \
                           Notify(\_SB.SCK6.C013, 0x82)  \
                           Notify(\_SB.SCK6.C014, 0x82)  \
                           Notify(\_SB.SCK6.C015, 0x82)  \
                           Notify(\_SB.SCK6.C016, 0x82)  \
                           Notify(\_SB.SCK6.C017, 0x82)  \
                           Notify(\_SB.SCK6.C018, 0x82)  \
                           Notify(\_SB.SCK6.C019, 0x82)  \
                           Notify(\_SB.SCK6.C01A, 0x82)  \
                           Notify(\_SB.SCK6.C01B, 0x82)  \
                           Notify(\_SB.SCK6.C01C, 0x82)  \
                           Notify(\_SB.SCK6.C01D, 0x82)  \
                           Notify(\_SB.SCK6.C01E, 0x82)  \
                           Notify(\_SB.SCK6.C01F, 0x82)  \
                           Notify(\_SB.SCK6.C020, 0x82)  \
                           Notify(\_SB.SCK6.C021, 0x82)  \
                           Notify(\_SB.SCK6.C022, 0x82)  \
                           Notify(\_SB.SCK6.C023, 0x82)  \
                           Notify(\_SB.SCK6.C024, 0x82)  \
                           Notify(\_SB.SCK6.C025, 0x82)  \
                           Notify(\_SB.SCK6.C026, 0x82)  \
                           Notify(\_SB.SCK6.C027, 0x82)  \
                           Notify(\_SB.SCK6.C028, 0x82)  \
                           Notify(\_SB.SCK6.C029, 0x82)  \
                           Notify(\_SB.SCK6.C02A, 0x82)  \
                           Notify(\_SB.SCK6.C02B, 0x82)  \
                           Notify(\_SB.SCK6.C02C, 0x82)  \
                           Notify(\_SB.SCK6.C02D, 0x82)  \
                           Notify(\_SB.SCK6.C02E, 0x82)  \
                           Notify(\_SB.SCK6.C02F, 0x82)  \
                           Notify(\_SB.SCK6.C030, 0x82)  \
                           Notify(\_SB.SCK6.C031, 0x82)  \
                           Notify(\_SB.SCK6.C032, 0x82)  \
                           Notify(\_SB.SCK6.C033, 0x82)  \
                           Notify(\_SB.SCK6.C034, 0x82)  \
                           Notify(\_SB.SCK6.C035, 0x82)  \
                           Notify(\_SB.SCK6.C036, 0x82)  \
                           Notify(\_SB.SCK6.C037, 0x82)  \
                           Notify(\_SB.SCK6.C038, 0x82)  \
                           Notify(\_SB.SCK6.C039, 0x82)  \
                           Notify(\_SB.SCK6.C03A, 0x82)  \
                           Notify(\_SB.SCK6.C03B, 0x82)  \
                           Notify(\_SB.SCK6.C03C, 0x82)  \
                           Notify(\_SB.SCK6.C03D, 0x82)  \
                           Notify(\_SB.SCK6.C03E, 0x82)  \
                           Notify(\_SB.SCK6.C03F, 0x82)  \
                           Notify(\_SB.SCK6.C040, 0x82)  \
                           Notify(\_SB.SCK6.C041, 0x82)  \
                           Notify(\_SB.SCK6.C042, 0x82)  \
                           Notify(\_SB.SCK6.C043, 0x82)  \
                           Notify(\_SB.SCK6.C044, 0x82)  \
                           Notify(\_SB.SCK6.C045, 0x82)  \
                           Notify(\_SB.SCK6.C046, 0x82)  \
                           Notify(\_SB.SCK6.C047, 0x82)  \
                           Notify(\_SB.SCK6.C048, 0x82)  \
                           Notify(\_SB.SCK6.C049, 0x82)  \
                           Notify(\_SB.SCK6.C04A, 0x82)  \
                           Notify(\_SB.SCK6.C04B, 0x82)  \
                           Notify(\_SB.SCK6.C04C, 0x82)  \
                           Notify(\_SB.SCK6.C04D, 0x82)  \
                           Notify(\_SB.SCK6.C04E, 0x82)  \
                           Notify(\_SB.SCK6.C04F, 0x82)  \
                           Notify(\_SB.SCK6.C050, 0x82)  \
                           Notify(\_SB.SCK6.C051, 0x82)  \
                           Notify(\_SB.SCK6.C052, 0x82)  \
                           Notify(\_SB.SCK6.C053, 0x82)  \
                           Notify(\_SB.SCK6.C054, 0x82)  \
                           Notify(\_SB.SCK6.C055, 0x82)  \
                           Notify(\_SB.SCK6.C056, 0x82)  \
                           Notify(\_SB.SCK6.C057, 0x82)  \
                           Notify(\_SB.SCK6.C058, 0x82)  \
                           Notify(\_SB.SCK6.C059, 0x82)  \
                           Notify(\_SB.SCK6.C05A, 0x82)  \
                           Notify(\_SB.SCK6.C05B, 0x82)  \
                           Notify(\_SB.SCK6.C05C, 0x82)  \
                           Notify(\_SB.SCK6.C05D, 0x82)  \
                           Notify(\_SB.SCK6.C05E, 0x82)  \
                           Notify(\_SB.SCK6.C05F, 0x82)  \
                           Notify(\_SB.SCK6.C060, 0x82)  \
                           Notify(\_SB.SCK6.C061, 0x82)  \
                           Notify(\_SB.SCK6.C062, 0x82)  \
                           Notify(\_SB.SCK6.C063, 0x82)  \
                           Notify(\_SB.SCK6.C064, 0x82)  \
                           Notify(\_SB.SCK6.C065, 0x82)  \
                           Notify(\_SB.SCK6.C066, 0x82)  \
                           Notify(\_SB.SCK6.C067, 0x82)  \
                           Notify(\_SB.SCK6.C068, 0x82)  \
                           Notify(\_SB.SCK6.C069, 0x82)  \
                           Notify(\_SB.SCK6.C06A, 0x82)  \
                           Notify(\_SB.SCK6.C06B, 0x82)  \
                           Notify(\_SB.SCK6.C06C, 0x82)  \
                           Notify(\_SB.SCK6.C06D, 0x82)  \
                           Notify(\_SB.SCK6.C06E, 0x82)  \
                           Notify(\_SB.SCK6.C06F, 0x82)  \
                           Notify(\_SB.SCK6.C070, 0x82)  \
                           Notify(\_SB.SCK6.C071, 0x82)  \
                           Notify(\_SB.SCK6.C072, 0x82)  \
                           Notify(\_SB.SCK6.C073, 0x82)  \
                           Notify(\_SB.SCK6.C074, 0x82)  \
                           Notify(\_SB.SCK6.C075, 0x82)  \
                           Notify(\_SB.SCK6.C076, 0x82)  \
                           Notify(\_SB.SCK6.C077, 0x82)  \
                           Notify(\_SB.SCK6.C078, 0x82)  \
                           Notify(\_SB.SCK6.C079, 0x82)  \
                           Notify(\_SB.SCK6.C07A, 0x82)  \
                           Notify(\_SB.SCK6.C07B, 0x82)  \
                           Notify(\_SB.SCK6.C07C, 0x82)  \
                           Notify(\_SB.SCK6.C07D, 0x82)  \
                           Notify(\_SB.SCK6.C07E, 0x82)  \
                           Notify(\_SB.SCK6.C07F, 0x82)
#else
#define Notify80Socket6()
#define Notify82Socket6()
#endif

#if MAX_SOCKET > 7
#define Notify80Socket7()  Notify(\_SB.SCK7.C000, 0x80)  \
                           Notify(\_SB.SCK7.C001, 0x80)  \
                           Notify(\_SB.SCK7.C002, 0x80)  \
                           Notify(\_SB.SCK7.C003, 0x80)  \
                           Notify(\_SB.SCK7.C004, 0x80)  \
                           Notify(\_SB.SCK7.C005, 0x80)  \
                           Notify(\_SB.SCK7.C006, 0x80)  \
                           Notify(\_SB.SCK7.C007, 0x80)  \
                           Notify(\_SB.SCK7.C008, 0x80)  \
                           Notify(\_SB.SCK7.C009, 0x80)  \
                           Notify(\_SB.SCK7.C00A, 0x80)  \
                           Notify(\_SB.SCK7.C00B, 0x80)  \
                           Notify(\_SB.SCK7.C00C, 0x80)  \
                           Notify(\_SB.SCK7.C00D, 0x80)  \
                           Notify(\_SB.SCK7.C00E, 0x80)  \
                           Notify(\_SB.SCK7.C00F, 0x80)  \
                           Notify(\_SB.SCK7.C010, 0x80)  \
                           Notify(\_SB.SCK7.C011, 0x80)  \
                           Notify(\_SB.SCK7.C012, 0x80)  \
                           Notify(\_SB.SCK7.C013, 0x80)  \
                           Notify(\_SB.SCK7.C014, 0x80)  \
                           Notify(\_SB.SCK7.C015, 0x80)  \
                           Notify(\_SB.SCK7.C016, 0x80)  \
                           Notify(\_SB.SCK7.C017, 0x80)  \
                           Notify(\_SB.SCK7.C018, 0x80)  \
                           Notify(\_SB.SCK7.C019, 0x80)  \
                           Notify(\_SB.SCK7.C01A, 0x80)  \
                           Notify(\_SB.SCK7.C01B, 0x80)  \
                           Notify(\_SB.SCK7.C01C, 0x80)  \
                           Notify(\_SB.SCK7.C01D, 0x80)  \
                           Notify(\_SB.SCK7.C01E, 0x80)  \
                           Notify(\_SB.SCK7.C01F, 0x80)  \
                           Notify(\_SB.SCK7.C020, 0x80)  \
                           Notify(\_SB.SCK7.C021, 0x80)  \
                           Notify(\_SB.SCK7.C022, 0x80)  \
                           Notify(\_SB.SCK7.C023, 0x80)  \
                           Notify(\_SB.SCK7.C024, 0x80)  \
                           Notify(\_SB.SCK7.C025, 0x80)  \
                           Notify(\_SB.SCK7.C026, 0x80)  \
                           Notify(\_SB.SCK7.C027, 0x80)  \
                           Notify(\_SB.SCK7.C028, 0x80)  \
                           Notify(\_SB.SCK7.C029, 0x80)  \
                           Notify(\_SB.SCK7.C02A, 0x80)  \
                           Notify(\_SB.SCK7.C02B, 0x80)  \
                           Notify(\_SB.SCK7.C02C, 0x80)  \
                           Notify(\_SB.SCK7.C02D, 0x80)  \
                           Notify(\_SB.SCK7.C02E, 0x80)  \
                           Notify(\_SB.SCK7.C02F, 0x80)  \
                           Notify(\_SB.SCK7.C030, 0x80)  \
                           Notify(\_SB.SCK7.C031, 0x80)  \
                           Notify(\_SB.SCK7.C032, 0x80)  \
                           Notify(\_SB.SCK7.C033, 0x80)  \
                           Notify(\_SB.SCK7.C034, 0x80)  \
                           Notify(\_SB.SCK7.C035, 0x80)  \
                           Notify(\_SB.SCK7.C036, 0x80)  \
                           Notify(\_SB.SCK7.C037, 0x80)  \
                           Notify(\_SB.SCK7.C038, 0x80)  \
                           Notify(\_SB.SCK7.C039, 0x80)  \
                           Notify(\_SB.SCK7.C03A, 0x80)  \
                           Notify(\_SB.SCK7.C03B, 0x80)  \
                           Notify(\_SB.SCK7.C03C, 0x80)  \
                           Notify(\_SB.SCK7.C03D, 0x80)  \
                           Notify(\_SB.SCK7.C03E, 0x80)  \
                           Notify(\_SB.SCK7.C03F, 0x80)  \
                           Notify(\_SB.SCK7.C040, 0x80)  \
                           Notify(\_SB.SCK7.C041, 0x80)  \
                           Notify(\_SB.SCK7.C042, 0x80)  \
                           Notify(\_SB.SCK7.C043, 0x80)  \
                           Notify(\_SB.SCK7.C044, 0x80)  \
                           Notify(\_SB.SCK7.C045, 0x80)  \
                           Notify(\_SB.SCK7.C046, 0x80)  \
                           Notify(\_SB.SCK7.C047, 0x80)  \
                           Notify(\_SB.SCK7.C048, 0x80)  \
                           Notify(\_SB.SCK7.C049, 0x80)  \
                           Notify(\_SB.SCK7.C04A, 0x80)  \
                           Notify(\_SB.SCK7.C04B, 0x80)  \
                           Notify(\_SB.SCK7.C04C, 0x80)  \
                           Notify(\_SB.SCK7.C04D, 0x80)  \
                           Notify(\_SB.SCK7.C04E, 0x80)  \
                           Notify(\_SB.SCK7.C04F, 0x80)  \
                           Notify(\_SB.SCK7.C050, 0x80)  \
                           Notify(\_SB.SCK7.C051, 0x80)  \
                           Notify(\_SB.SCK7.C052, 0x80)  \
                           Notify(\_SB.SCK7.C053, 0x80)  \
                           Notify(\_SB.SCK7.C054, 0x80)  \
                           Notify(\_SB.SCK7.C055, 0x80)  \
                           Notify(\_SB.SCK7.C056, 0x80)  \
                           Notify(\_SB.SCK7.C057, 0x80)  \
                           Notify(\_SB.SCK7.C058, 0x80)  \
                           Notify(\_SB.SCK7.C059, 0x80)  \
                           Notify(\_SB.SCK7.C05A, 0x80)  \
                           Notify(\_SB.SCK7.C05B, 0x80)  \
                           Notify(\_SB.SCK7.C05C, 0x80)  \
                           Notify(\_SB.SCK7.C05D, 0x80)  \
                           Notify(\_SB.SCK7.C05E, 0x80)  \
                           Notify(\_SB.SCK7.C05F, 0x80)  \
                           Notify(\_SB.SCK7.C060, 0x80)  \
                           Notify(\_SB.SCK7.C061, 0x80)  \
                           Notify(\_SB.SCK7.C062, 0x80)  \
                           Notify(\_SB.SCK7.C063, 0x80)  \
                           Notify(\_SB.SCK7.C064, 0x80)  \
                           Notify(\_SB.SCK7.C065, 0x80)  \
                           Notify(\_SB.SCK7.C066, 0x80)  \
                           Notify(\_SB.SCK7.C067, 0x80)  \
                           Notify(\_SB.SCK7.C068, 0x80)  \
                           Notify(\_SB.SCK7.C069, 0x80)  \
                           Notify(\_SB.SCK7.C06A, 0x80)  \
                           Notify(\_SB.SCK7.C06B, 0x80)  \
                           Notify(\_SB.SCK7.C06C, 0x80)  \
                           Notify(\_SB.SCK7.C06D, 0x80)  \
                           Notify(\_SB.SCK7.C06E, 0x80)  \
                           Notify(\_SB.SCK7.C06F, 0x80)  \
                           Notify(\_SB.SCK7.C070, 0x80)  \
                           Notify(\_SB.SCK7.C071, 0x80)  \
                           Notify(\_SB.SCK7.C072, 0x80)  \
                           Notify(\_SB.SCK7.C073, 0x80)  \
                           Notify(\_SB.SCK7.C074, 0x80)  \
                           Notify(\_SB.SCK7.C075, 0x80)  \
                           Notify(\_SB.SCK7.C076, 0x80)  \
                           Notify(\_SB.SCK7.C077, 0x80)  \
                           Notify(\_SB.SCK7.C078, 0x80)  \
                           Notify(\_SB.SCK7.C079, 0x80)  \
                           Notify(\_SB.SCK7.C07A, 0x80)  \
                           Notify(\_SB.SCK7.C07B, 0x80)  \
                           Notify(\_SB.SCK7.C07C, 0x80)  \
                           Notify(\_SB.SCK7.C07D, 0x80)  \
                           Notify(\_SB.SCK7.C07E, 0x80)  \
                           Notify(\_SB.SCK7.C07F, 0x80)

#define Notify82Socket7()  Notify(\_SB.SCK7.C000, 0x82)  \
                           Notify(\_SB.SCK7.C001, 0x82)  \
                           Notify(\_SB.SCK7.C002, 0x82)  \
                           Notify(\_SB.SCK7.C003, 0x82)  \
                           Notify(\_SB.SCK7.C004, 0x82)  \
                           Notify(\_SB.SCK7.C005, 0x82)  \
                           Notify(\_SB.SCK7.C006, 0x82)  \
                           Notify(\_SB.SCK7.C007, 0x82)  \
                           Notify(\_SB.SCK7.C008, 0x82)  \
                           Notify(\_SB.SCK7.C009, 0x82)  \
                           Notify(\_SB.SCK7.C00A, 0x82)  \
                           Notify(\_SB.SCK7.C00B, 0x82)  \
                           Notify(\_SB.SCK7.C00C, 0x82)  \
                           Notify(\_SB.SCK7.C00D, 0x82)  \
                           Notify(\_SB.SCK7.C00E, 0x82)  \
                           Notify(\_SB.SCK7.C00F, 0x82)  \
                           Notify(\_SB.SCK7.C010, 0x82)  \
                           Notify(\_SB.SCK7.C011, 0x82)  \
                           Notify(\_SB.SCK7.C012, 0x82)  \
                           Notify(\_SB.SCK7.C013, 0x82)  \
                           Notify(\_SB.SCK7.C014, 0x82)  \
                           Notify(\_SB.SCK7.C015, 0x82)  \
                           Notify(\_SB.SCK7.C016, 0x82)  \
                           Notify(\_SB.SCK7.C017, 0x82)  \
                           Notify(\_SB.SCK7.C018, 0x82)  \
                           Notify(\_SB.SCK7.C019, 0x82)  \
                           Notify(\_SB.SCK7.C01A, 0x82)  \
                           Notify(\_SB.SCK7.C01B, 0x82)  \
                           Notify(\_SB.SCK7.C01C, 0x82)  \
                           Notify(\_SB.SCK7.C01D, 0x82)  \
                           Notify(\_SB.SCK7.C01E, 0x82)  \
                           Notify(\_SB.SCK7.C01F, 0x82)  \
                           Notify(\_SB.SCK7.C020, 0x82)  \
                           Notify(\_SB.SCK7.C021, 0x82)  \
                           Notify(\_SB.SCK7.C022, 0x82)  \
                           Notify(\_SB.SCK7.C023, 0x82)  \
                           Notify(\_SB.SCK7.C024, 0x82)  \
                           Notify(\_SB.SCK7.C025, 0x82)  \
                           Notify(\_SB.SCK7.C026, 0x82)  \
                           Notify(\_SB.SCK7.C027, 0x82)  \
                           Notify(\_SB.SCK7.C028, 0x82)  \
                           Notify(\_SB.SCK7.C029, 0x82)  \
                           Notify(\_SB.SCK7.C02A, 0x82)  \
                           Notify(\_SB.SCK7.C02B, 0x82)  \
                           Notify(\_SB.SCK7.C02C, 0x82)  \
                           Notify(\_SB.SCK7.C02D, 0x82)  \
                           Notify(\_SB.SCK7.C02E, 0x82)  \
                           Notify(\_SB.SCK7.C02F, 0x82)  \
                           Notify(\_SB.SCK7.C030, 0x82)  \
                           Notify(\_SB.SCK7.C031, 0x82)  \
                           Notify(\_SB.SCK7.C032, 0x82)  \
                           Notify(\_SB.SCK7.C033, 0x82)  \
                           Notify(\_SB.SCK7.C034, 0x82)  \
                           Notify(\_SB.SCK7.C035, 0x82)  \
                           Notify(\_SB.SCK7.C036, 0x82)  \
                           Notify(\_SB.SCK7.C037, 0x82)  \
                           Notify(\_SB.SCK7.C038, 0x82)  \
                           Notify(\_SB.SCK7.C039, 0x82)  \
                           Notify(\_SB.SCK7.C03A, 0x82)  \
                           Notify(\_SB.SCK7.C03B, 0x82)  \
                           Notify(\_SB.SCK7.C03C, 0x82)  \
                           Notify(\_SB.SCK7.C03D, 0x82)  \
                           Notify(\_SB.SCK7.C03E, 0x82)  \
                           Notify(\_SB.SCK7.C03F, 0x82)  \
                           Notify(\_SB.SCK7.C040, 0x82)  \
                           Notify(\_SB.SCK7.C041, 0x82)  \
                           Notify(\_SB.SCK7.C042, 0x82)  \
                           Notify(\_SB.SCK7.C043, 0x82)  \
                           Notify(\_SB.SCK7.C044, 0x82)  \
                           Notify(\_SB.SCK7.C045, 0x82)  \
                           Notify(\_SB.SCK7.C046, 0x82)  \
                           Notify(\_SB.SCK7.C047, 0x82)  \
                           Notify(\_SB.SCK7.C048, 0x82)  \
                           Notify(\_SB.SCK7.C049, 0x82)  \
                           Notify(\_SB.SCK7.C04A, 0x82)  \
                           Notify(\_SB.SCK7.C04B, 0x82)  \
                           Notify(\_SB.SCK7.C04C, 0x82)  \
                           Notify(\_SB.SCK7.C04D, 0x82)  \
                           Notify(\_SB.SCK7.C04E, 0x82)  \
                           Notify(\_SB.SCK7.C04F, 0x82)  \
                           Notify(\_SB.SCK7.C050, 0x82)  \
                           Notify(\_SB.SCK7.C051, 0x82)  \
                           Notify(\_SB.SCK7.C052, 0x82)  \
                           Notify(\_SB.SCK7.C053, 0x82)  \
                           Notify(\_SB.SCK7.C054, 0x82)  \
                           Notify(\_SB.SCK7.C055, 0x82)  \
                           Notify(\_SB.SCK7.C056, 0x82)  \
                           Notify(\_SB.SCK7.C057, 0x82)  \
                           Notify(\_SB.SCK7.C058, 0x82)  \
                           Notify(\_SB.SCK7.C059, 0x82)  \
                           Notify(\_SB.SCK7.C05A, 0x82)  \
                           Notify(\_SB.SCK7.C05B, 0x82)  \
                           Notify(\_SB.SCK7.C05C, 0x82)  \
                           Notify(\_SB.SCK7.C05D, 0x82)  \
                           Notify(\_SB.SCK7.C05E, 0x82)  \
                           Notify(\_SB.SCK7.C05F, 0x82)  \
                           Notify(\_SB.SCK7.C060, 0x82)  \
                           Notify(\_SB.SCK7.C061, 0x82)  \
                           Notify(\_SB.SCK7.C062, 0x82)  \
                           Notify(\_SB.SCK7.C063, 0x82)  \
                           Notify(\_SB.SCK7.C064, 0x82)  \
                           Notify(\_SB.SCK7.C065, 0x82)  \
                           Notify(\_SB.SCK7.C066, 0x82)  \
                           Notify(\_SB.SCK7.C067, 0x82)  \
                           Notify(\_SB.SCK7.C068, 0x82)  \
                           Notify(\_SB.SCK7.C069, 0x82)  \
                           Notify(\_SB.SCK7.C06A, 0x82)  \
                           Notify(\_SB.SCK7.C06B, 0x82)  \
                           Notify(\_SB.SCK7.C06C, 0x82)  \
                           Notify(\_SB.SCK7.C06D, 0x82)  \
                           Notify(\_SB.SCK7.C06E, 0x82)  \
                           Notify(\_SB.SCK7.C06F, 0x82)  \
                           Notify(\_SB.SCK7.C070, 0x82)  \
                           Notify(\_SB.SCK7.C071, 0x82)  \
                           Notify(\_SB.SCK7.C072, 0x82)  \
                           Notify(\_SB.SCK7.C073, 0x82)  \
                           Notify(\_SB.SCK7.C074, 0x82)  \
                           Notify(\_SB.SCK7.C075, 0x82)  \
                           Notify(\_SB.SCK7.C076, 0x82)  \
                           Notify(\_SB.SCK7.C077, 0x82)  \
                           Notify(\_SB.SCK7.C078, 0x82)  \
                           Notify(\_SB.SCK7.C079, 0x82)  \
                           Notify(\_SB.SCK7.C07A, 0x82)  \
                           Notify(\_SB.SCK7.C07B, 0x82)  \
                           Notify(\_SB.SCK7.C07C, 0x82)  \
                           Notify(\_SB.SCK7.C07D, 0x82)  \
                           Notify(\_SB.SCK7.C07E, 0x82)  \
                           Notify(\_SB.SCK7.C07F, 0x82)
#else
#define Notify80Socket7()
#define Notify82Socket7()
#endif

DefinitionBlock("SpsNm.aml", "SSDT", 2, "INTEL", "SpsNm   ", 2)
{
  External(\HWEN, FieldUnitObj)       // HWPM enabled as provided in BIOS_ACPI_PARAM::HWPMEnable
  External(\PETE, IntObj)
  External(\PSLM, PkgObj)
  External(\TSLM, PkgObj)
  External(\OSCP, FieldUnitObj)

  External(\_SB.SCK0, DeviceObj)
  External(\_SB.SCK0.C000, DeviceObj)
  External(\_SB.SCK0.C001, DeviceObj)
  External(\_SB.SCK0.C002, DeviceObj)
  External(\_SB.SCK0.C003, DeviceObj)
  External(\_SB.SCK0.C004, DeviceObj)
  External(\_SB.SCK0.C005, DeviceObj)
  External(\_SB.SCK0.C006, DeviceObj)
  External(\_SB.SCK0.C007, DeviceObj)
  External(\_SB.SCK0.C008, DeviceObj)
  External(\_SB.SCK0.C009, DeviceObj)
  External(\_SB.SCK0.C00A, DeviceObj)
  External(\_SB.SCK0.C00B, DeviceObj)
  External(\_SB.SCK0.C00C, DeviceObj)
  External(\_SB.SCK0.C00D, DeviceObj)
  External(\_SB.SCK0.C00E, DeviceObj)
  External(\_SB.SCK0.C00F, DeviceObj)
  External(\_SB.SCK0.C010, DeviceObj)
  External(\_SB.SCK0.C011, DeviceObj)
  External(\_SB.SCK0.C012, DeviceObj)
  External(\_SB.SCK0.C013, DeviceObj)
  External(\_SB.SCK0.C014, DeviceObj)
  External(\_SB.SCK0.C015, DeviceObj)
  External(\_SB.SCK0.C016, DeviceObj)
  External(\_SB.SCK0.C017, DeviceObj)
  External(\_SB.SCK0.C018, DeviceObj)
  External(\_SB.SCK0.C019, DeviceObj)
  External(\_SB.SCK0.C01A, DeviceObj)
  External(\_SB.SCK0.C01B, DeviceObj)
  External(\_SB.SCK0.C01C, DeviceObj)
  External(\_SB.SCK0.C01D, DeviceObj)
  External(\_SB.SCK0.C01E, DeviceObj)
  External(\_SB.SCK0.C01F, DeviceObj)
  External(\_SB.SCK0.C020, DeviceObj)
  External(\_SB.SCK0.C021, DeviceObj)
  External(\_SB.SCK0.C022, DeviceObj)
  External(\_SB.SCK0.C023, DeviceObj)
  External(\_SB.SCK0.C024, DeviceObj)
  External(\_SB.SCK0.C025, DeviceObj)
  External(\_SB.SCK0.C026, DeviceObj)
  External(\_SB.SCK0.C027, DeviceObj)
  External(\_SB.SCK0.C028, DeviceObj)
  External(\_SB.SCK0.C029, DeviceObj)
  External(\_SB.SCK0.C02A, DeviceObj)
  External(\_SB.SCK0.C02B, DeviceObj)
  External(\_SB.SCK0.C02C, DeviceObj)
  External(\_SB.SCK0.C02D, DeviceObj)
  External(\_SB.SCK0.C02E, DeviceObj)
  External(\_SB.SCK0.C02F, DeviceObj)
  External(\_SB.SCK0.C030, DeviceObj)
  External(\_SB.SCK0.C031, DeviceObj)
  External(\_SB.SCK0.C032, DeviceObj)
  External(\_SB.SCK0.C033, DeviceObj)
  External(\_SB.SCK0.C034, DeviceObj)
  External(\_SB.SCK0.C035, DeviceObj)
  External(\_SB.SCK0.C036, DeviceObj)
  External(\_SB.SCK0.C037, DeviceObj)
  External(\_SB.SCK0.C038, DeviceObj)
  External(\_SB.SCK0.C039, DeviceObj)
  External(\_SB.SCK0.C03A, DeviceObj)
  External(\_SB.SCK0.C03B, DeviceObj)
  External(\_SB.SCK0.C03C, DeviceObj)
  External(\_SB.SCK0.C03D, DeviceObj)
  External(\_SB.SCK0.C03E, DeviceObj)
  External(\_SB.SCK0.C03F, DeviceObj)
  External(\_SB.SCK0.C040, DeviceObj)
  External(\_SB.SCK0.C041, DeviceObj)
  External(\_SB.SCK0.C042, DeviceObj)
  External(\_SB.SCK0.C043, DeviceObj)
  External(\_SB.SCK0.C044, DeviceObj)
  External(\_SB.SCK0.C045, DeviceObj)
  External(\_SB.SCK0.C046, DeviceObj)
  External(\_SB.SCK0.C047, DeviceObj)
  External(\_SB.SCK0.C048, DeviceObj)
  External(\_SB.SCK0.C049, DeviceObj)
  External(\_SB.SCK0.C04A, DeviceObj)
  External(\_SB.SCK0.C04B, DeviceObj)
  External(\_SB.SCK0.C04C, DeviceObj)
  External(\_SB.SCK0.C04D, DeviceObj)
  External(\_SB.SCK0.C04E, DeviceObj)
  External(\_SB.SCK0.C04F, DeviceObj)
  External(\_SB.SCK0.C050, DeviceObj)
  External(\_SB.SCK0.C051, DeviceObj)
  External(\_SB.SCK0.C052, DeviceObj)
  External(\_SB.SCK0.C053, DeviceObj)
  External(\_SB.SCK0.C054, DeviceObj)
  External(\_SB.SCK0.C055, DeviceObj)
  External(\_SB.SCK0.C056, DeviceObj)
  External(\_SB.SCK0.C057, DeviceObj)
  External(\_SB.SCK0.C058, DeviceObj)
  External(\_SB.SCK0.C059, DeviceObj)
  External(\_SB.SCK0.C05A, DeviceObj)
  External(\_SB.SCK0.C05B, DeviceObj)
  External(\_SB.SCK0.C05C, DeviceObj)
  External(\_SB.SCK0.C05D, DeviceObj)
  External(\_SB.SCK0.C05E, DeviceObj)
  External(\_SB.SCK0.C05F, DeviceObj)
  External(\_SB.SCK0.C060, DeviceObj)
  External(\_SB.SCK0.C061, DeviceObj)
  External(\_SB.SCK0.C062, DeviceObj)
  External(\_SB.SCK0.C063, DeviceObj)
  External(\_SB.SCK0.C064, DeviceObj)
  External(\_SB.SCK0.C065, DeviceObj)
  External(\_SB.SCK0.C066, DeviceObj)
  External(\_SB.SCK0.C067, DeviceObj)
  External(\_SB.SCK0.C068, DeviceObj)
  External(\_SB.SCK0.C069, DeviceObj)
  External(\_SB.SCK0.C06A, DeviceObj)
  External(\_SB.SCK0.C06B, DeviceObj)
  External(\_SB.SCK0.C06C, DeviceObj)
  External(\_SB.SCK0.C06D, DeviceObj)
  External(\_SB.SCK0.C06E, DeviceObj)
  External(\_SB.SCK0.C06F, DeviceObj)
  External(\_SB.SCK0.C070, DeviceObj)
  External(\_SB.SCK0.C071, DeviceObj)
  External(\_SB.SCK0.C072, DeviceObj)
  External(\_SB.SCK0.C073, DeviceObj)
  External(\_SB.SCK0.C074, DeviceObj)
  External(\_SB.SCK0.C075, DeviceObj)
  External(\_SB.SCK0.C076, DeviceObj)
  External(\_SB.SCK0.C077, DeviceObj)
  External(\_SB.SCK0.C078, DeviceObj)
  External(\_SB.SCK0.C079, DeviceObj)
  External(\_SB.SCK0.C07A, DeviceObj)
  External(\_SB.SCK0.C07B, DeviceObj)
  External(\_SB.SCK0.C07C, DeviceObj)
  External(\_SB.SCK0.C07D, DeviceObj)
  External(\_SB.SCK0.C07E, DeviceObj)
  External(\_SB.SCK0.C07F, DeviceObj)

#if MAX_SOCKET > 1
  External(\_SB.SCK1, DeviceObj)
  External(\_SB.SCK1.C000, DeviceObj)
  External(\_SB.SCK1.C001, DeviceObj)
  External(\_SB.SCK1.C002, DeviceObj)
  External(\_SB.SCK1.C003, DeviceObj)
  External(\_SB.SCK1.C004, DeviceObj)
  External(\_SB.SCK1.C005, DeviceObj)
  External(\_SB.SCK1.C006, DeviceObj)
  External(\_SB.SCK1.C007, DeviceObj)
  External(\_SB.SCK1.C008, DeviceObj)
  External(\_SB.SCK1.C009, DeviceObj)
  External(\_SB.SCK1.C00A, DeviceObj)
  External(\_SB.SCK1.C00B, DeviceObj)
  External(\_SB.SCK1.C00C, DeviceObj)
  External(\_SB.SCK1.C00D, DeviceObj)
  External(\_SB.SCK1.C00E, DeviceObj)
  External(\_SB.SCK1.C00F, DeviceObj)
  External(\_SB.SCK1.C010, DeviceObj)
  External(\_SB.SCK1.C011, DeviceObj)
  External(\_SB.SCK1.C012, DeviceObj)
  External(\_SB.SCK1.C013, DeviceObj)
  External(\_SB.SCK1.C014, DeviceObj)
  External(\_SB.SCK1.C015, DeviceObj)
  External(\_SB.SCK1.C016, DeviceObj)
  External(\_SB.SCK1.C017, DeviceObj)
  External(\_SB.SCK1.C018, DeviceObj)
  External(\_SB.SCK1.C019, DeviceObj)
  External(\_SB.SCK1.C01A, DeviceObj)
  External(\_SB.SCK1.C01B, DeviceObj)
  External(\_SB.SCK1.C01C, DeviceObj)
  External(\_SB.SCK1.C01D, DeviceObj)
  External(\_SB.SCK1.C01E, DeviceObj)
  External(\_SB.SCK1.C01F, DeviceObj)
  External(\_SB.SCK1.C020, DeviceObj)
  External(\_SB.SCK1.C021, DeviceObj)
  External(\_SB.SCK1.C022, DeviceObj)
  External(\_SB.SCK1.C023, DeviceObj)
  External(\_SB.SCK1.C024, DeviceObj)
  External(\_SB.SCK1.C025, DeviceObj)
  External(\_SB.SCK1.C026, DeviceObj)
  External(\_SB.SCK1.C027, DeviceObj)
  External(\_SB.SCK1.C028, DeviceObj)
  External(\_SB.SCK1.C029, DeviceObj)
  External(\_SB.SCK1.C02A, DeviceObj)
  External(\_SB.SCK1.C02B, DeviceObj)
  External(\_SB.SCK1.C02C, DeviceObj)
  External(\_SB.SCK1.C02D, DeviceObj)
  External(\_SB.SCK1.C02E, DeviceObj)
  External(\_SB.SCK1.C02F, DeviceObj)
  External(\_SB.SCK1.C030, DeviceObj)
  External(\_SB.SCK1.C031, DeviceObj)
  External(\_SB.SCK1.C032, DeviceObj)
  External(\_SB.SCK1.C033, DeviceObj)
  External(\_SB.SCK1.C034, DeviceObj)
  External(\_SB.SCK1.C035, DeviceObj)
  External(\_SB.SCK1.C036, DeviceObj)
  External(\_SB.SCK1.C037, DeviceObj)
  External(\_SB.SCK1.C038, DeviceObj)
  External(\_SB.SCK1.C039, DeviceObj)
  External(\_SB.SCK1.C03A, DeviceObj)
  External(\_SB.SCK1.C03B, DeviceObj)
  External(\_SB.SCK1.C03C, DeviceObj)
  External(\_SB.SCK1.C03D, DeviceObj)
  External(\_SB.SCK1.C03E, DeviceObj)
  External(\_SB.SCK1.C03F, DeviceObj)
  External(\_SB.SCK1.C040, DeviceObj)
  External(\_SB.SCK1.C041, DeviceObj)
  External(\_SB.SCK1.C042, DeviceObj)
  External(\_SB.SCK1.C043, DeviceObj)
  External(\_SB.SCK1.C044, DeviceObj)
  External(\_SB.SCK1.C045, DeviceObj)
  External(\_SB.SCK1.C046, DeviceObj)
  External(\_SB.SCK1.C047, DeviceObj)
  External(\_SB.SCK1.C048, DeviceObj)
  External(\_SB.SCK1.C049, DeviceObj)
  External(\_SB.SCK1.C04A, DeviceObj)
  External(\_SB.SCK1.C04B, DeviceObj)
  External(\_SB.SCK1.C04C, DeviceObj)
  External(\_SB.SCK1.C04D, DeviceObj)
  External(\_SB.SCK1.C04E, DeviceObj)
  External(\_SB.SCK1.C04F, DeviceObj)
  External(\_SB.SCK1.C050, DeviceObj)
  External(\_SB.SCK1.C051, DeviceObj)
  External(\_SB.SCK1.C052, DeviceObj)
  External(\_SB.SCK1.C053, DeviceObj)
  External(\_SB.SCK1.C054, DeviceObj)
  External(\_SB.SCK1.C055, DeviceObj)
  External(\_SB.SCK1.C056, DeviceObj)
  External(\_SB.SCK1.C057, DeviceObj)
  External(\_SB.SCK1.C058, DeviceObj)
  External(\_SB.SCK1.C059, DeviceObj)
  External(\_SB.SCK1.C05A, DeviceObj)
  External(\_SB.SCK1.C05B, DeviceObj)
  External(\_SB.SCK1.C05C, DeviceObj)
  External(\_SB.SCK1.C05D, DeviceObj)
  External(\_SB.SCK1.C05E, DeviceObj)
  External(\_SB.SCK1.C05F, DeviceObj)
  External(\_SB.SCK1.C060, DeviceObj)
  External(\_SB.SCK1.C061, DeviceObj)
  External(\_SB.SCK1.C062, DeviceObj)
  External(\_SB.SCK1.C063, DeviceObj)
  External(\_SB.SCK1.C064, DeviceObj)
  External(\_SB.SCK1.C065, DeviceObj)
  External(\_SB.SCK1.C066, DeviceObj)
  External(\_SB.SCK1.C067, DeviceObj)
  External(\_SB.SCK1.C068, DeviceObj)
  External(\_SB.SCK1.C069, DeviceObj)
  External(\_SB.SCK1.C06A, DeviceObj)
  External(\_SB.SCK1.C06B, DeviceObj)
  External(\_SB.SCK1.C06C, DeviceObj)
  External(\_SB.SCK1.C06D, DeviceObj)
  External(\_SB.SCK1.C06E, DeviceObj)
  External(\_SB.SCK1.C06F, DeviceObj)
  External(\_SB.SCK1.C070, DeviceObj)
  External(\_SB.SCK1.C071, DeviceObj)
  External(\_SB.SCK1.C072, DeviceObj)
  External(\_SB.SCK1.C073, DeviceObj)
  External(\_SB.SCK1.C074, DeviceObj)
  External(\_SB.SCK1.C075, DeviceObj)
  External(\_SB.SCK1.C076, DeviceObj)
  External(\_SB.SCK1.C077, DeviceObj)
  External(\_SB.SCK1.C078, DeviceObj)
  External(\_SB.SCK1.C079, DeviceObj)
  External(\_SB.SCK1.C07A, DeviceObj)
  External(\_SB.SCK1.C07B, DeviceObj)
  External(\_SB.SCK1.C07C, DeviceObj)
  External(\_SB.SCK1.C07D, DeviceObj)
  External(\_SB.SCK1.C07E, DeviceObj)
  External(\_SB.SCK1.C07F, DeviceObj)
#endif

#if MAX_SOCKET > 2
  External(\_SB.SCK2, DeviceObj)
  External(\_SB.SCK2.C000, DeviceObj)
  External(\_SB.SCK2.C001, DeviceObj)
  External(\_SB.SCK2.C002, DeviceObj)
  External(\_SB.SCK2.C003, DeviceObj)
  External(\_SB.SCK2.C004, DeviceObj)
  External(\_SB.SCK2.C005, DeviceObj)
  External(\_SB.SCK2.C006, DeviceObj)
  External(\_SB.SCK2.C007, DeviceObj)
  External(\_SB.SCK2.C008, DeviceObj)
  External(\_SB.SCK2.C009, DeviceObj)
  External(\_SB.SCK2.C00A, DeviceObj)
  External(\_SB.SCK2.C00B, DeviceObj)
  External(\_SB.SCK2.C00C, DeviceObj)
  External(\_SB.SCK2.C00D, DeviceObj)
  External(\_SB.SCK2.C00E, DeviceObj)
  External(\_SB.SCK2.C00F, DeviceObj)
  External(\_SB.SCK2.C010, DeviceObj)
  External(\_SB.SCK2.C011, DeviceObj)
  External(\_SB.SCK2.C012, DeviceObj)
  External(\_SB.SCK2.C013, DeviceObj)
  External(\_SB.SCK2.C014, DeviceObj)
  External(\_SB.SCK2.C015, DeviceObj)
  External(\_SB.SCK2.C016, DeviceObj)
  External(\_SB.SCK2.C017, DeviceObj)
  External(\_SB.SCK2.C018, DeviceObj)
  External(\_SB.SCK2.C019, DeviceObj)
  External(\_SB.SCK2.C01A, DeviceObj)
  External(\_SB.SCK2.C01B, DeviceObj)
  External(\_SB.SCK2.C01C, DeviceObj)
  External(\_SB.SCK2.C01D, DeviceObj)
  External(\_SB.SCK2.C01E, DeviceObj)
  External(\_SB.SCK2.C01F, DeviceObj)
  External(\_SB.SCK2.C020, DeviceObj)
  External(\_SB.SCK2.C021, DeviceObj)
  External(\_SB.SCK2.C022, DeviceObj)
  External(\_SB.SCK2.C023, DeviceObj)
  External(\_SB.SCK2.C024, DeviceObj)
  External(\_SB.SCK2.C025, DeviceObj)
  External(\_SB.SCK2.C026, DeviceObj)
  External(\_SB.SCK2.C027, DeviceObj)
  External(\_SB.SCK2.C028, DeviceObj)
  External(\_SB.SCK2.C029, DeviceObj)
  External(\_SB.SCK2.C02A, DeviceObj)
  External(\_SB.SCK2.C02B, DeviceObj)
  External(\_SB.SCK2.C02C, DeviceObj)
  External(\_SB.SCK2.C02D, DeviceObj)
  External(\_SB.SCK2.C02E, DeviceObj)
  External(\_SB.SCK2.C02F, DeviceObj)
  External(\_SB.SCK2.C030, DeviceObj)
  External(\_SB.SCK2.C031, DeviceObj)
  External(\_SB.SCK2.C032, DeviceObj)
  External(\_SB.SCK2.C033, DeviceObj)
  External(\_SB.SCK2.C034, DeviceObj)
  External(\_SB.SCK2.C035, DeviceObj)
  External(\_SB.SCK2.C036, DeviceObj)
  External(\_SB.SCK2.C037, DeviceObj)
  External(\_SB.SCK2.C038, DeviceObj)
  External(\_SB.SCK2.C039, DeviceObj)
  External(\_SB.SCK2.C03A, DeviceObj)
  External(\_SB.SCK2.C03B, DeviceObj)
  External(\_SB.SCK2.C03C, DeviceObj)
  External(\_SB.SCK2.C03D, DeviceObj)
  External(\_SB.SCK2.C03E, DeviceObj)
  External(\_SB.SCK2.C03F, DeviceObj)
  External(\_SB.SCK2.C040, DeviceObj)
  External(\_SB.SCK2.C041, DeviceObj)
  External(\_SB.SCK2.C042, DeviceObj)
  External(\_SB.SCK2.C043, DeviceObj)
  External(\_SB.SCK2.C044, DeviceObj)
  External(\_SB.SCK2.C045, DeviceObj)
  External(\_SB.SCK2.C046, DeviceObj)
  External(\_SB.SCK2.C047, DeviceObj)
  External(\_SB.SCK2.C048, DeviceObj)
  External(\_SB.SCK2.C049, DeviceObj)
  External(\_SB.SCK2.C04A, DeviceObj)
  External(\_SB.SCK2.C04B, DeviceObj)
  External(\_SB.SCK2.C04C, DeviceObj)
  External(\_SB.SCK2.C04D, DeviceObj)
  External(\_SB.SCK2.C04E, DeviceObj)
  External(\_SB.SCK2.C04F, DeviceObj)
  External(\_SB.SCK2.C050, DeviceObj)
  External(\_SB.SCK2.C051, DeviceObj)
  External(\_SB.SCK2.C052, DeviceObj)
  External(\_SB.SCK2.C053, DeviceObj)
  External(\_SB.SCK2.C054, DeviceObj)
  External(\_SB.SCK2.C055, DeviceObj)
  External(\_SB.SCK2.C056, DeviceObj)
  External(\_SB.SCK2.C057, DeviceObj)
  External(\_SB.SCK2.C058, DeviceObj)
  External(\_SB.SCK2.C059, DeviceObj)
  External(\_SB.SCK2.C05A, DeviceObj)
  External(\_SB.SCK2.C05B, DeviceObj)
  External(\_SB.SCK2.C05C, DeviceObj)
  External(\_SB.SCK2.C05D, DeviceObj)
  External(\_SB.SCK2.C05E, DeviceObj)
  External(\_SB.SCK2.C05F, DeviceObj)
  External(\_SB.SCK2.C060, DeviceObj)
  External(\_SB.SCK2.C061, DeviceObj)
  External(\_SB.SCK2.C062, DeviceObj)
  External(\_SB.SCK2.C063, DeviceObj)
  External(\_SB.SCK2.C064, DeviceObj)
  External(\_SB.SCK2.C065, DeviceObj)
  External(\_SB.SCK2.C066, DeviceObj)
  External(\_SB.SCK2.C067, DeviceObj)
  External(\_SB.SCK2.C068, DeviceObj)
  External(\_SB.SCK2.C069, DeviceObj)
  External(\_SB.SCK2.C06A, DeviceObj)
  External(\_SB.SCK2.C06B, DeviceObj)
  External(\_SB.SCK2.C06C, DeviceObj)
  External(\_SB.SCK2.C06D, DeviceObj)
  External(\_SB.SCK2.C06E, DeviceObj)
  External(\_SB.SCK2.C06F, DeviceObj)
  External(\_SB.SCK2.C070, DeviceObj)
  External(\_SB.SCK2.C071, DeviceObj)
  External(\_SB.SCK2.C072, DeviceObj)
  External(\_SB.SCK2.C073, DeviceObj)
  External(\_SB.SCK2.C074, DeviceObj)
  External(\_SB.SCK2.C075, DeviceObj)
  External(\_SB.SCK2.C076, DeviceObj)
  External(\_SB.SCK2.C077, DeviceObj)
  External(\_SB.SCK2.C078, DeviceObj)
  External(\_SB.SCK2.C079, DeviceObj)
  External(\_SB.SCK2.C07A, DeviceObj)
  External(\_SB.SCK2.C07B, DeviceObj)
  External(\_SB.SCK2.C07C, DeviceObj)
  External(\_SB.SCK2.C07D, DeviceObj)
  External(\_SB.SCK2.C07E, DeviceObj)
  External(\_SB.SCK2.C07F, DeviceObj)
#endif

#if MAX_SOCKET > 3
  External(\_SB.SCK3, DeviceObj)
  External(\_SB.SCK3.C000, DeviceObj)
  External(\_SB.SCK3.C001, DeviceObj)
  External(\_SB.SCK3.C002, DeviceObj)
  External(\_SB.SCK3.C003, DeviceObj)
  External(\_SB.SCK3.C004, DeviceObj)
  External(\_SB.SCK3.C005, DeviceObj)
  External(\_SB.SCK3.C006, DeviceObj)
  External(\_SB.SCK3.C007, DeviceObj)
  External(\_SB.SCK3.C008, DeviceObj)
  External(\_SB.SCK3.C009, DeviceObj)
  External(\_SB.SCK3.C00A, DeviceObj)
  External(\_SB.SCK3.C00B, DeviceObj)
  External(\_SB.SCK3.C00C, DeviceObj)
  External(\_SB.SCK3.C00D, DeviceObj)
  External(\_SB.SCK3.C00E, DeviceObj)
  External(\_SB.SCK3.C00F, DeviceObj)
  External(\_SB.SCK3.C010, DeviceObj)
  External(\_SB.SCK3.C011, DeviceObj)
  External(\_SB.SCK3.C012, DeviceObj)
  External(\_SB.SCK3.C013, DeviceObj)
  External(\_SB.SCK3.C014, DeviceObj)
  External(\_SB.SCK3.C015, DeviceObj)
  External(\_SB.SCK3.C016, DeviceObj)
  External(\_SB.SCK3.C017, DeviceObj)
  External(\_SB.SCK3.C018, DeviceObj)
  External(\_SB.SCK3.C019, DeviceObj)
  External(\_SB.SCK3.C01A, DeviceObj)
  External(\_SB.SCK3.C01B, DeviceObj)
  External(\_SB.SCK3.C01C, DeviceObj)
  External(\_SB.SCK3.C01D, DeviceObj)
  External(\_SB.SCK3.C01E, DeviceObj)
  External(\_SB.SCK3.C01F, DeviceObj)
  External(\_SB.SCK3.C020, DeviceObj)
  External(\_SB.SCK3.C021, DeviceObj)
  External(\_SB.SCK3.C022, DeviceObj)
  External(\_SB.SCK3.C023, DeviceObj)
  External(\_SB.SCK3.C024, DeviceObj)
  External(\_SB.SCK3.C025, DeviceObj)
  External(\_SB.SCK3.C026, DeviceObj)
  External(\_SB.SCK3.C027, DeviceObj)
  External(\_SB.SCK3.C028, DeviceObj)
  External(\_SB.SCK3.C029, DeviceObj)
  External(\_SB.SCK3.C02A, DeviceObj)
  External(\_SB.SCK3.C02B, DeviceObj)
  External(\_SB.SCK3.C02C, DeviceObj)
  External(\_SB.SCK3.C02D, DeviceObj)
  External(\_SB.SCK3.C02E, DeviceObj)
  External(\_SB.SCK3.C02F, DeviceObj)
  External(\_SB.SCK3.C030, DeviceObj)
  External(\_SB.SCK3.C031, DeviceObj)
  External(\_SB.SCK3.C032, DeviceObj)
  External(\_SB.SCK3.C033, DeviceObj)
  External(\_SB.SCK3.C034, DeviceObj)
  External(\_SB.SCK3.C035, DeviceObj)
  External(\_SB.SCK3.C036, DeviceObj)
  External(\_SB.SCK3.C037, DeviceObj)
  External(\_SB.SCK3.C038, DeviceObj)
  External(\_SB.SCK3.C039, DeviceObj)
  External(\_SB.SCK3.C03A, DeviceObj)
  External(\_SB.SCK3.C03B, DeviceObj)
  External(\_SB.SCK3.C03C, DeviceObj)
  External(\_SB.SCK3.C03D, DeviceObj)
  External(\_SB.SCK3.C03E, DeviceObj)
  External(\_SB.SCK3.C03F, DeviceObj)
  External(\_SB.SCK3.C040, DeviceObj)
  External(\_SB.SCK3.C041, DeviceObj)
  External(\_SB.SCK3.C042, DeviceObj)
  External(\_SB.SCK3.C043, DeviceObj)
  External(\_SB.SCK3.C044, DeviceObj)
  External(\_SB.SCK3.C045, DeviceObj)
  External(\_SB.SCK3.C046, DeviceObj)
  External(\_SB.SCK3.C047, DeviceObj)
  External(\_SB.SCK3.C048, DeviceObj)
  External(\_SB.SCK3.C049, DeviceObj)
  External(\_SB.SCK3.C04A, DeviceObj)
  External(\_SB.SCK3.C04B, DeviceObj)
  External(\_SB.SCK3.C04C, DeviceObj)
  External(\_SB.SCK3.C04D, DeviceObj)
  External(\_SB.SCK3.C04E, DeviceObj)
  External(\_SB.SCK3.C04F, DeviceObj)
  External(\_SB.SCK3.C050, DeviceObj)
  External(\_SB.SCK3.C051, DeviceObj)
  External(\_SB.SCK3.C052, DeviceObj)
  External(\_SB.SCK3.C053, DeviceObj)
  External(\_SB.SCK3.C054, DeviceObj)
  External(\_SB.SCK3.C055, DeviceObj)
  External(\_SB.SCK3.C056, DeviceObj)
  External(\_SB.SCK3.C057, DeviceObj)
  External(\_SB.SCK3.C058, DeviceObj)
  External(\_SB.SCK3.C059, DeviceObj)
  External(\_SB.SCK3.C05A, DeviceObj)
  External(\_SB.SCK3.C05B, DeviceObj)
  External(\_SB.SCK3.C05C, DeviceObj)
  External(\_SB.SCK3.C05D, DeviceObj)
  External(\_SB.SCK3.C05E, DeviceObj)
  External(\_SB.SCK3.C05F, DeviceObj)
  External(\_SB.SCK3.C060, DeviceObj)
  External(\_SB.SCK3.C061, DeviceObj)
  External(\_SB.SCK3.C062, DeviceObj)
  External(\_SB.SCK3.C063, DeviceObj)
  External(\_SB.SCK3.C064, DeviceObj)
  External(\_SB.SCK3.C065, DeviceObj)
  External(\_SB.SCK3.C066, DeviceObj)
  External(\_SB.SCK3.C067, DeviceObj)
  External(\_SB.SCK3.C068, DeviceObj)
  External(\_SB.SCK3.C069, DeviceObj)
  External(\_SB.SCK3.C06A, DeviceObj)
  External(\_SB.SCK3.C06B, DeviceObj)
  External(\_SB.SCK3.C06C, DeviceObj)
  External(\_SB.SCK3.C06D, DeviceObj)
  External(\_SB.SCK3.C06E, DeviceObj)
  External(\_SB.SCK3.C06F, DeviceObj)
  External(\_SB.SCK3.C070, DeviceObj)
  External(\_SB.SCK3.C071, DeviceObj)
  External(\_SB.SCK3.C072, DeviceObj)
  External(\_SB.SCK3.C073, DeviceObj)
  External(\_SB.SCK3.C074, DeviceObj)
  External(\_SB.SCK3.C075, DeviceObj)
  External(\_SB.SCK3.C076, DeviceObj)
  External(\_SB.SCK3.C077, DeviceObj)
  External(\_SB.SCK3.C078, DeviceObj)
  External(\_SB.SCK3.C079, DeviceObj)
  External(\_SB.SCK3.C07A, DeviceObj)
  External(\_SB.SCK3.C07B, DeviceObj)
  External(\_SB.SCK3.C07C, DeviceObj)
  External(\_SB.SCK3.C07D, DeviceObj)
  External(\_SB.SCK3.C07E, DeviceObj)
  External(\_SB.SCK3.C07F, DeviceObj)
#endif

#if MAX_SOCKET > 4
  External(\_SB.SCK4, DeviceObj)
  External(\_SB.SCK4.C000, DeviceObj)
  External(\_SB.SCK4.C001, DeviceObj)
  External(\_SB.SCK4.C002, DeviceObj)
  External(\_SB.SCK4.C003, DeviceObj)
  External(\_SB.SCK4.C004, DeviceObj)
  External(\_SB.SCK4.C005, DeviceObj)
  External(\_SB.SCK4.C006, DeviceObj)
  External(\_SB.SCK4.C007, DeviceObj)
  External(\_SB.SCK4.C008, DeviceObj)
  External(\_SB.SCK4.C009, DeviceObj)
  External(\_SB.SCK4.C00A, DeviceObj)
  External(\_SB.SCK4.C00B, DeviceObj)
  External(\_SB.SCK4.C00C, DeviceObj)
  External(\_SB.SCK4.C00D, DeviceObj)
  External(\_SB.SCK4.C00E, DeviceObj)
  External(\_SB.SCK4.C00F, DeviceObj)
  External(\_SB.SCK4.C010, DeviceObj)
  External(\_SB.SCK4.C011, DeviceObj)
  External(\_SB.SCK4.C012, DeviceObj)
  External(\_SB.SCK4.C013, DeviceObj)
  External(\_SB.SCK4.C014, DeviceObj)
  External(\_SB.SCK4.C015, DeviceObj)
  External(\_SB.SCK4.C016, DeviceObj)
  External(\_SB.SCK4.C017, DeviceObj)
  External(\_SB.SCK4.C018, DeviceObj)
  External(\_SB.SCK4.C019, DeviceObj)
  External(\_SB.SCK4.C01A, DeviceObj)
  External(\_SB.SCK4.C01B, DeviceObj)
  External(\_SB.SCK4.C01C, DeviceObj)
  External(\_SB.SCK4.C01D, DeviceObj)
  External(\_SB.SCK4.C01E, DeviceObj)
  External(\_SB.SCK4.C01F, DeviceObj)
  External(\_SB.SCK4.C020, DeviceObj)
  External(\_SB.SCK4.C021, DeviceObj)
  External(\_SB.SCK4.C022, DeviceObj)
  External(\_SB.SCK4.C023, DeviceObj)
  External(\_SB.SCK4.C024, DeviceObj)
  External(\_SB.SCK4.C025, DeviceObj)
  External(\_SB.SCK4.C026, DeviceObj)
  External(\_SB.SCK4.C027, DeviceObj)
  External(\_SB.SCK4.C028, DeviceObj)
  External(\_SB.SCK4.C029, DeviceObj)
  External(\_SB.SCK4.C02A, DeviceObj)
  External(\_SB.SCK4.C02B, DeviceObj)
  External(\_SB.SCK4.C02C, DeviceObj)
  External(\_SB.SCK4.C02D, DeviceObj)
  External(\_SB.SCK4.C02E, DeviceObj)
  External(\_SB.SCK4.C02F, DeviceObj)
  External(\_SB.SCK4.C030, DeviceObj)
  External(\_SB.SCK4.C031, DeviceObj)
  External(\_SB.SCK4.C032, DeviceObj)
  External(\_SB.SCK4.C033, DeviceObj)
  External(\_SB.SCK4.C034, DeviceObj)
  External(\_SB.SCK4.C035, DeviceObj)
  External(\_SB.SCK4.C036, DeviceObj)
  External(\_SB.SCK4.C037, DeviceObj)
  External(\_SB.SCK4.C038, DeviceObj)
  External(\_SB.SCK4.C039, DeviceObj)
  External(\_SB.SCK4.C03A, DeviceObj)
  External(\_SB.SCK4.C03B, DeviceObj)
  External(\_SB.SCK4.C03C, DeviceObj)
  External(\_SB.SCK4.C03D, DeviceObj)
  External(\_SB.SCK4.C03E, DeviceObj)
  External(\_SB.SCK4.C03F, DeviceObj)
  External(\_SB.SCK4.C040, DeviceObj)
  External(\_SB.SCK4.C041, DeviceObj)
  External(\_SB.SCK4.C042, DeviceObj)
  External(\_SB.SCK4.C043, DeviceObj)
  External(\_SB.SCK4.C044, DeviceObj)
  External(\_SB.SCK4.C045, DeviceObj)
  External(\_SB.SCK4.C046, DeviceObj)
  External(\_SB.SCK4.C047, DeviceObj)
  External(\_SB.SCK4.C048, DeviceObj)
  External(\_SB.SCK4.C049, DeviceObj)
  External(\_SB.SCK4.C04A, DeviceObj)
  External(\_SB.SCK4.C04B, DeviceObj)
  External(\_SB.SCK4.C04C, DeviceObj)
  External(\_SB.SCK4.C04D, DeviceObj)
  External(\_SB.SCK4.C04E, DeviceObj)
  External(\_SB.SCK4.C04F, DeviceObj)
  External(\_SB.SCK4.C050, DeviceObj)
  External(\_SB.SCK4.C051, DeviceObj)
  External(\_SB.SCK4.C052, DeviceObj)
  External(\_SB.SCK4.C053, DeviceObj)
  External(\_SB.SCK4.C054, DeviceObj)
  External(\_SB.SCK4.C055, DeviceObj)
  External(\_SB.SCK4.C056, DeviceObj)
  External(\_SB.SCK4.C057, DeviceObj)
  External(\_SB.SCK4.C058, DeviceObj)
  External(\_SB.SCK4.C059, DeviceObj)
  External(\_SB.SCK4.C05A, DeviceObj)
  External(\_SB.SCK4.C05B, DeviceObj)
  External(\_SB.SCK4.C05C, DeviceObj)
  External(\_SB.SCK4.C05D, DeviceObj)
  External(\_SB.SCK4.C05E, DeviceObj)
  External(\_SB.SCK4.C05F, DeviceObj)
  External(\_SB.SCK4.C060, DeviceObj)
  External(\_SB.SCK4.C061, DeviceObj)
  External(\_SB.SCK4.C062, DeviceObj)
  External(\_SB.SCK4.C063, DeviceObj)
  External(\_SB.SCK4.C064, DeviceObj)
  External(\_SB.SCK4.C065, DeviceObj)
  External(\_SB.SCK4.C066, DeviceObj)
  External(\_SB.SCK4.C067, DeviceObj)
  External(\_SB.SCK4.C068, DeviceObj)
  External(\_SB.SCK4.C069, DeviceObj)
  External(\_SB.SCK4.C06A, DeviceObj)
  External(\_SB.SCK4.C06B, DeviceObj)
  External(\_SB.SCK4.C06C, DeviceObj)
  External(\_SB.SCK4.C06D, DeviceObj)
  External(\_SB.SCK4.C06E, DeviceObj)
  External(\_SB.SCK4.C06F, DeviceObj)
  External(\_SB.SCK4.C070, DeviceObj)
  External(\_SB.SCK4.C071, DeviceObj)
  External(\_SB.SCK4.C072, DeviceObj)
  External(\_SB.SCK4.C073, DeviceObj)
  External(\_SB.SCK4.C074, DeviceObj)
  External(\_SB.SCK4.C075, DeviceObj)
  External(\_SB.SCK4.C076, DeviceObj)
  External(\_SB.SCK4.C077, DeviceObj)
  External(\_SB.SCK4.C078, DeviceObj)
  External(\_SB.SCK4.C079, DeviceObj)
  External(\_SB.SCK4.C07A, DeviceObj)
  External(\_SB.SCK4.C07B, DeviceObj)
  External(\_SB.SCK4.C07C, DeviceObj)
  External(\_SB.SCK4.C07D, DeviceObj)
  External(\_SB.SCK4.C07E, DeviceObj)
  External(\_SB.SCK4.C07F, DeviceObj)
#endif

#if MAX_SOCKET > 5
  External(\_SB.SCK5, DeviceObj)
  External(\_SB.SCK5.C000, DeviceObj)
  External(\_SB.SCK5.C001, DeviceObj)
  External(\_SB.SCK5.C002, DeviceObj)
  External(\_SB.SCK5.C003, DeviceObj)
  External(\_SB.SCK5.C004, DeviceObj)
  External(\_SB.SCK5.C005, DeviceObj)
  External(\_SB.SCK5.C006, DeviceObj)
  External(\_SB.SCK5.C007, DeviceObj)
  External(\_SB.SCK5.C008, DeviceObj)
  External(\_SB.SCK5.C009, DeviceObj)
  External(\_SB.SCK5.C00A, DeviceObj)
  External(\_SB.SCK5.C00B, DeviceObj)
  External(\_SB.SCK5.C00C, DeviceObj)
  External(\_SB.SCK5.C00D, DeviceObj)
  External(\_SB.SCK5.C00E, DeviceObj)
  External(\_SB.SCK5.C00F, DeviceObj)
  External(\_SB.SCK5.C010, DeviceObj)
  External(\_SB.SCK5.C011, DeviceObj)
  External(\_SB.SCK5.C012, DeviceObj)
  External(\_SB.SCK5.C013, DeviceObj)
  External(\_SB.SCK5.C014, DeviceObj)
  External(\_SB.SCK5.C015, DeviceObj)
  External(\_SB.SCK5.C016, DeviceObj)
  External(\_SB.SCK5.C017, DeviceObj)
  External(\_SB.SCK5.C018, DeviceObj)
  External(\_SB.SCK5.C019, DeviceObj)
  External(\_SB.SCK5.C01A, DeviceObj)
  External(\_SB.SCK5.C01B, DeviceObj)
  External(\_SB.SCK5.C01C, DeviceObj)
  External(\_SB.SCK5.C01D, DeviceObj)
  External(\_SB.SCK5.C01E, DeviceObj)
  External(\_SB.SCK5.C01F, DeviceObj)
  External(\_SB.SCK5.C020, DeviceObj)
  External(\_SB.SCK5.C021, DeviceObj)
  External(\_SB.SCK5.C022, DeviceObj)
  External(\_SB.SCK5.C023, DeviceObj)
  External(\_SB.SCK5.C024, DeviceObj)
  External(\_SB.SCK5.C025, DeviceObj)
  External(\_SB.SCK5.C026, DeviceObj)
  External(\_SB.SCK5.C027, DeviceObj)
  External(\_SB.SCK5.C028, DeviceObj)
  External(\_SB.SCK5.C029, DeviceObj)
  External(\_SB.SCK5.C02A, DeviceObj)
  External(\_SB.SCK5.C02B, DeviceObj)
  External(\_SB.SCK5.C02C, DeviceObj)
  External(\_SB.SCK5.C02D, DeviceObj)
  External(\_SB.SCK5.C02E, DeviceObj)
  External(\_SB.SCK5.C02F, DeviceObj)
  External(\_SB.SCK5.C030, DeviceObj)
  External(\_SB.SCK5.C031, DeviceObj)
  External(\_SB.SCK5.C032, DeviceObj)
  External(\_SB.SCK5.C033, DeviceObj)
  External(\_SB.SCK5.C034, DeviceObj)
  External(\_SB.SCK5.C035, DeviceObj)
  External(\_SB.SCK5.C036, DeviceObj)
  External(\_SB.SCK5.C037, DeviceObj)
  External(\_SB.SCK5.C038, DeviceObj)
  External(\_SB.SCK5.C039, DeviceObj)
  External(\_SB.SCK5.C03A, DeviceObj)
  External(\_SB.SCK5.C03B, DeviceObj)
  External(\_SB.SCK5.C03C, DeviceObj)
  External(\_SB.SCK5.C03D, DeviceObj)
  External(\_SB.SCK5.C03E, DeviceObj)
  External(\_SB.SCK5.C03F, DeviceObj)
  External(\_SB.SCK5.C040, DeviceObj)
  External(\_SB.SCK5.C041, DeviceObj)
  External(\_SB.SCK5.C042, DeviceObj)
  External(\_SB.SCK5.C043, DeviceObj)
  External(\_SB.SCK5.C044, DeviceObj)
  External(\_SB.SCK5.C045, DeviceObj)
  External(\_SB.SCK5.C046, DeviceObj)
  External(\_SB.SCK5.C047, DeviceObj)
  External(\_SB.SCK5.C048, DeviceObj)
  External(\_SB.SCK5.C049, DeviceObj)
  External(\_SB.SCK5.C04A, DeviceObj)
  External(\_SB.SCK5.C04B, DeviceObj)
  External(\_SB.SCK5.C04C, DeviceObj)
  External(\_SB.SCK5.C04D, DeviceObj)
  External(\_SB.SCK5.C04E, DeviceObj)
  External(\_SB.SCK5.C04F, DeviceObj)
  External(\_SB.SCK5.C050, DeviceObj)
  External(\_SB.SCK5.C051, DeviceObj)
  External(\_SB.SCK5.C052, DeviceObj)
  External(\_SB.SCK5.C053, DeviceObj)
  External(\_SB.SCK5.C054, DeviceObj)
  External(\_SB.SCK5.C055, DeviceObj)
  External(\_SB.SCK5.C056, DeviceObj)
  External(\_SB.SCK5.C057, DeviceObj)
  External(\_SB.SCK5.C058, DeviceObj)
  External(\_SB.SCK5.C059, DeviceObj)
  External(\_SB.SCK5.C05A, DeviceObj)
  External(\_SB.SCK5.C05B, DeviceObj)
  External(\_SB.SCK5.C05C, DeviceObj)
  External(\_SB.SCK5.C05D, DeviceObj)
  External(\_SB.SCK5.C05E, DeviceObj)
  External(\_SB.SCK5.C05F, DeviceObj)
  External(\_SB.SCK5.C060, DeviceObj)
  External(\_SB.SCK5.C061, DeviceObj)
  External(\_SB.SCK5.C062, DeviceObj)
  External(\_SB.SCK5.C063, DeviceObj)
  External(\_SB.SCK5.C064, DeviceObj)
  External(\_SB.SCK5.C065, DeviceObj)
  External(\_SB.SCK5.C066, DeviceObj)
  External(\_SB.SCK5.C067, DeviceObj)
  External(\_SB.SCK5.C068, DeviceObj)
  External(\_SB.SCK5.C069, DeviceObj)
  External(\_SB.SCK5.C06A, DeviceObj)
  External(\_SB.SCK5.C06B, DeviceObj)
  External(\_SB.SCK5.C06C, DeviceObj)
  External(\_SB.SCK5.C06D, DeviceObj)
  External(\_SB.SCK5.C06E, DeviceObj)
  External(\_SB.SCK5.C06F, DeviceObj)
  External(\_SB.SCK5.C070, DeviceObj)
  External(\_SB.SCK5.C071, DeviceObj)
  External(\_SB.SCK5.C072, DeviceObj)
  External(\_SB.SCK5.C073, DeviceObj)
  External(\_SB.SCK5.C074, DeviceObj)
  External(\_SB.SCK5.C075, DeviceObj)
  External(\_SB.SCK5.C076, DeviceObj)
  External(\_SB.SCK5.C077, DeviceObj)
  External(\_SB.SCK5.C078, DeviceObj)
  External(\_SB.SCK5.C079, DeviceObj)
  External(\_SB.SCK5.C07A, DeviceObj)
  External(\_SB.SCK5.C07B, DeviceObj)
  External(\_SB.SCK5.C07C, DeviceObj)
  External(\_SB.SCK5.C07D, DeviceObj)
  External(\_SB.SCK5.C07E, DeviceObj)
  External(\_SB.SCK5.C07F, DeviceObj)
#endif

#if MAX_SOCKET > 6
  External(\_SB.SCK6, DeviceObj)
  External(\_SB.SCK6.C000, DeviceObj)
  External(\_SB.SCK6.C001, DeviceObj)
  External(\_SB.SCK6.C002, DeviceObj)
  External(\_SB.SCK6.C003, DeviceObj)
  External(\_SB.SCK6.C004, DeviceObj)
  External(\_SB.SCK6.C005, DeviceObj)
  External(\_SB.SCK6.C006, DeviceObj)
  External(\_SB.SCK6.C007, DeviceObj)
  External(\_SB.SCK6.C008, DeviceObj)
  External(\_SB.SCK6.C009, DeviceObj)
  External(\_SB.SCK6.C00A, DeviceObj)
  External(\_SB.SCK6.C00B, DeviceObj)
  External(\_SB.SCK6.C00C, DeviceObj)
  External(\_SB.SCK6.C00D, DeviceObj)
  External(\_SB.SCK6.C00E, DeviceObj)
  External(\_SB.SCK6.C00F, DeviceObj)
  External(\_SB.SCK6.C010, DeviceObj)
  External(\_SB.SCK6.C011, DeviceObj)
  External(\_SB.SCK6.C012, DeviceObj)
  External(\_SB.SCK6.C013, DeviceObj)
  External(\_SB.SCK6.C014, DeviceObj)
  External(\_SB.SCK6.C015, DeviceObj)
  External(\_SB.SCK6.C016, DeviceObj)
  External(\_SB.SCK6.C017, DeviceObj)
  External(\_SB.SCK6.C018, DeviceObj)
  External(\_SB.SCK6.C019, DeviceObj)
  External(\_SB.SCK6.C01A, DeviceObj)
  External(\_SB.SCK6.C01B, DeviceObj)
  External(\_SB.SCK6.C01C, DeviceObj)
  External(\_SB.SCK6.C01D, DeviceObj)
  External(\_SB.SCK6.C01E, DeviceObj)
  External(\_SB.SCK6.C01F, DeviceObj)
  External(\_SB.SCK6.C020, DeviceObj)
  External(\_SB.SCK6.C021, DeviceObj)
  External(\_SB.SCK6.C022, DeviceObj)
  External(\_SB.SCK6.C023, DeviceObj)
  External(\_SB.SCK6.C024, DeviceObj)
  External(\_SB.SCK6.C025, DeviceObj)
  External(\_SB.SCK6.C026, DeviceObj)
  External(\_SB.SCK6.C027, DeviceObj)
  External(\_SB.SCK6.C028, DeviceObj)
  External(\_SB.SCK6.C029, DeviceObj)
  External(\_SB.SCK6.C02A, DeviceObj)
  External(\_SB.SCK6.C02B, DeviceObj)
  External(\_SB.SCK6.C02C, DeviceObj)
  External(\_SB.SCK6.C02D, DeviceObj)
  External(\_SB.SCK6.C02E, DeviceObj)
  External(\_SB.SCK6.C02F, DeviceObj)
  External(\_SB.SCK6.C030, DeviceObj)
  External(\_SB.SCK6.C031, DeviceObj)
  External(\_SB.SCK6.C032, DeviceObj)
  External(\_SB.SCK6.C033, DeviceObj)
  External(\_SB.SCK6.C034, DeviceObj)
  External(\_SB.SCK6.C035, DeviceObj)
  External(\_SB.SCK6.C036, DeviceObj)
  External(\_SB.SCK6.C037, DeviceObj)
  External(\_SB.SCK6.C038, DeviceObj)
  External(\_SB.SCK6.C039, DeviceObj)
  External(\_SB.SCK6.C03A, DeviceObj)
  External(\_SB.SCK6.C03B, DeviceObj)
  External(\_SB.SCK6.C03C, DeviceObj)
  External(\_SB.SCK6.C03D, DeviceObj)
  External(\_SB.SCK6.C03E, DeviceObj)
  External(\_SB.SCK6.C03F, DeviceObj)
  External(\_SB.SCK6.C040, DeviceObj)
  External(\_SB.SCK6.C041, DeviceObj)
  External(\_SB.SCK6.C042, DeviceObj)
  External(\_SB.SCK6.C043, DeviceObj)
  External(\_SB.SCK6.C044, DeviceObj)
  External(\_SB.SCK6.C045, DeviceObj)
  External(\_SB.SCK6.C046, DeviceObj)
  External(\_SB.SCK6.C047, DeviceObj)
  External(\_SB.SCK6.C048, DeviceObj)
  External(\_SB.SCK6.C049, DeviceObj)
  External(\_SB.SCK6.C04A, DeviceObj)
  External(\_SB.SCK6.C04B, DeviceObj)
  External(\_SB.SCK6.C04C, DeviceObj)
  External(\_SB.SCK6.C04D, DeviceObj)
  External(\_SB.SCK6.C04E, DeviceObj)
  External(\_SB.SCK6.C04F, DeviceObj)
  External(\_SB.SCK6.C050, DeviceObj)
  External(\_SB.SCK6.C051, DeviceObj)
  External(\_SB.SCK6.C052, DeviceObj)
  External(\_SB.SCK6.C053, DeviceObj)
  External(\_SB.SCK6.C054, DeviceObj)
  External(\_SB.SCK6.C055, DeviceObj)
  External(\_SB.SCK6.C056, DeviceObj)
  External(\_SB.SCK6.C057, DeviceObj)
  External(\_SB.SCK6.C058, DeviceObj)
  External(\_SB.SCK6.C059, DeviceObj)
  External(\_SB.SCK6.C05A, DeviceObj)
  External(\_SB.SCK6.C05B, DeviceObj)
  External(\_SB.SCK6.C05C, DeviceObj)
  External(\_SB.SCK6.C05D, DeviceObj)
  External(\_SB.SCK6.C05E, DeviceObj)
  External(\_SB.SCK6.C05F, DeviceObj)
  External(\_SB.SCK6.C060, DeviceObj)
  External(\_SB.SCK6.C061, DeviceObj)
  External(\_SB.SCK6.C062, DeviceObj)
  External(\_SB.SCK6.C063, DeviceObj)
  External(\_SB.SCK6.C064, DeviceObj)
  External(\_SB.SCK6.C065, DeviceObj)
  External(\_SB.SCK6.C066, DeviceObj)
  External(\_SB.SCK6.C067, DeviceObj)
  External(\_SB.SCK6.C068, DeviceObj)
  External(\_SB.SCK6.C069, DeviceObj)
  External(\_SB.SCK6.C06A, DeviceObj)
  External(\_SB.SCK6.C06B, DeviceObj)
  External(\_SB.SCK6.C06C, DeviceObj)
  External(\_SB.SCK6.C06D, DeviceObj)
  External(\_SB.SCK6.C06E, DeviceObj)
  External(\_SB.SCK6.C06F, DeviceObj)
  External(\_SB.SCK6.C070, DeviceObj)
  External(\_SB.SCK6.C071, DeviceObj)
  External(\_SB.SCK6.C072, DeviceObj)
  External(\_SB.SCK6.C073, DeviceObj)
  External(\_SB.SCK6.C074, DeviceObj)
  External(\_SB.SCK6.C075, DeviceObj)
  External(\_SB.SCK6.C076, DeviceObj)
  External(\_SB.SCK6.C077, DeviceObj)
  External(\_SB.SCK6.C078, DeviceObj)
  External(\_SB.SCK6.C079, DeviceObj)
  External(\_SB.SCK6.C07A, DeviceObj)
  External(\_SB.SCK6.C07B, DeviceObj)
  External(\_SB.SCK6.C07C, DeviceObj)
  External(\_SB.SCK6.C07D, DeviceObj)
  External(\_SB.SCK6.C07E, DeviceObj)
  External(\_SB.SCK6.C07F, DeviceObj)
#endif

#if MAX_SOCKET > 7
  External(\_SB.SCK7, DeviceObj)
  External(\_SB.SCK7.C000, DeviceObj)
  External(\_SB.SCK7.C001, DeviceObj)
  External(\_SB.SCK7.C002, DeviceObj)
  External(\_SB.SCK7.C003, DeviceObj)
  External(\_SB.SCK7.C004, DeviceObj)
  External(\_SB.SCK7.C005, DeviceObj)
  External(\_SB.SCK7.C006, DeviceObj)
  External(\_SB.SCK7.C007, DeviceObj)
  External(\_SB.SCK7.C008, DeviceObj)
  External(\_SB.SCK7.C009, DeviceObj)
  External(\_SB.SCK7.C00A, DeviceObj)
  External(\_SB.SCK7.C00B, DeviceObj)
  External(\_SB.SCK7.C00C, DeviceObj)
  External(\_SB.SCK7.C00D, DeviceObj)
  External(\_SB.SCK7.C00E, DeviceObj)
  External(\_SB.SCK7.C00F, DeviceObj)
  External(\_SB.SCK7.C010, DeviceObj)
  External(\_SB.SCK7.C011, DeviceObj)
  External(\_SB.SCK7.C012, DeviceObj)
  External(\_SB.SCK7.C013, DeviceObj)
  External(\_SB.SCK7.C014, DeviceObj)
  External(\_SB.SCK7.C015, DeviceObj)
  External(\_SB.SCK7.C016, DeviceObj)
  External(\_SB.SCK7.C017, DeviceObj)
  External(\_SB.SCK7.C018, DeviceObj)
  External(\_SB.SCK7.C019, DeviceObj)
  External(\_SB.SCK7.C01A, DeviceObj)
  External(\_SB.SCK7.C01B, DeviceObj)
  External(\_SB.SCK7.C01C, DeviceObj)
  External(\_SB.SCK7.C01D, DeviceObj)
  External(\_SB.SCK7.C01E, DeviceObj)
  External(\_SB.SCK7.C01F, DeviceObj)
  External(\_SB.SCK7.C020, DeviceObj)
  External(\_SB.SCK7.C021, DeviceObj)
  External(\_SB.SCK7.C022, DeviceObj)
  External(\_SB.SCK7.C023, DeviceObj)
  External(\_SB.SCK7.C024, DeviceObj)
  External(\_SB.SCK7.C025, DeviceObj)
  External(\_SB.SCK7.C026, DeviceObj)
  External(\_SB.SCK7.C027, DeviceObj)
  External(\_SB.SCK7.C028, DeviceObj)
  External(\_SB.SCK7.C029, DeviceObj)
  External(\_SB.SCK7.C02A, DeviceObj)
  External(\_SB.SCK7.C02B, DeviceObj)
  External(\_SB.SCK7.C02C, DeviceObj)
  External(\_SB.SCK7.C02D, DeviceObj)
  External(\_SB.SCK7.C02E, DeviceObj)
  External(\_SB.SCK7.C02F, DeviceObj)
  External(\_SB.SCK7.C030, DeviceObj)
  External(\_SB.SCK7.C031, DeviceObj)
  External(\_SB.SCK7.C032, DeviceObj)
  External(\_SB.SCK7.C033, DeviceObj)
  External(\_SB.SCK7.C034, DeviceObj)
  External(\_SB.SCK7.C035, DeviceObj)
  External(\_SB.SCK7.C036, DeviceObj)
  External(\_SB.SCK7.C037, DeviceObj)
  External(\_SB.SCK7.C038, DeviceObj)
  External(\_SB.SCK7.C039, DeviceObj)
  External(\_SB.SCK7.C03A, DeviceObj)
  External(\_SB.SCK7.C03B, DeviceObj)
  External(\_SB.SCK7.C03C, DeviceObj)
  External(\_SB.SCK7.C03D, DeviceObj)
  External(\_SB.SCK7.C03E, DeviceObj)
  External(\_SB.SCK7.C03F, DeviceObj)
  External(\_SB.SCK7.C040, DeviceObj)
  External(\_SB.SCK7.C041, DeviceObj)
  External(\_SB.SCK7.C042, DeviceObj)
  External(\_SB.SCK7.C043, DeviceObj)
  External(\_SB.SCK7.C044, DeviceObj)
  External(\_SB.SCK7.C045, DeviceObj)
  External(\_SB.SCK7.C046, DeviceObj)
  External(\_SB.SCK7.C047, DeviceObj)
  External(\_SB.SCK7.C048, DeviceObj)
  External(\_SB.SCK7.C049, DeviceObj)
  External(\_SB.SCK7.C04A, DeviceObj)
  External(\_SB.SCK7.C04B, DeviceObj)
  External(\_SB.SCK7.C04C, DeviceObj)
  External(\_SB.SCK7.C04D, DeviceObj)
  External(\_SB.SCK7.C04E, DeviceObj)
  External(\_SB.SCK7.C04F, DeviceObj)
  External(\_SB.SCK7.C050, DeviceObj)
  External(\_SB.SCK7.C051, DeviceObj)
  External(\_SB.SCK7.C052, DeviceObj)
  External(\_SB.SCK7.C053, DeviceObj)
  External(\_SB.SCK7.C054, DeviceObj)
  External(\_SB.SCK7.C055, DeviceObj)
  External(\_SB.SCK7.C056, DeviceObj)
  External(\_SB.SCK7.C057, DeviceObj)
  External(\_SB.SCK7.C058, DeviceObj)
  External(\_SB.SCK7.C059, DeviceObj)
  External(\_SB.SCK7.C05A, DeviceObj)
  External(\_SB.SCK7.C05B, DeviceObj)
  External(\_SB.SCK7.C05C, DeviceObj)
  External(\_SB.SCK7.C05D, DeviceObj)
  External(\_SB.SCK7.C05E, DeviceObj)
  External(\_SB.SCK7.C05F, DeviceObj)
  External(\_SB.SCK7.C060, DeviceObj)
  External(\_SB.SCK7.C061, DeviceObj)
  External(\_SB.SCK7.C062, DeviceObj)
  External(\_SB.SCK7.C063, DeviceObj)
  External(\_SB.SCK7.C064, DeviceObj)
  External(\_SB.SCK7.C065, DeviceObj)
  External(\_SB.SCK7.C066, DeviceObj)
  External(\_SB.SCK7.C067, DeviceObj)
  External(\_SB.SCK7.C068, DeviceObj)
  External(\_SB.SCK7.C069, DeviceObj)
  External(\_SB.SCK7.C06A, DeviceObj)
  External(\_SB.SCK7.C06B, DeviceObj)
  External(\_SB.SCK7.C06C, DeviceObj)
  External(\_SB.SCK7.C06D, DeviceObj)
  External(\_SB.SCK7.C06E, DeviceObj)
  External(\_SB.SCK7.C06F, DeviceObj)
  External(\_SB.SCK7.C070, DeviceObj)
  External(\_SB.SCK7.C071, DeviceObj)
  External(\_SB.SCK7.C072, DeviceObj)
  External(\_SB.SCK7.C073, DeviceObj)
  External(\_SB.SCK7.C074, DeviceObj)
  External(\_SB.SCK7.C075, DeviceObj)
  External(\_SB.SCK7.C076, DeviceObj)
  External(\_SB.SCK7.C077, DeviceObj)
  External(\_SB.SCK7.C078, DeviceObj)
  External(\_SB.SCK7.C079, DeviceObj)
  External(\_SB.SCK7.C07A, DeviceObj)
  External(\_SB.SCK7.C07B, DeviceObj)
  External(\_SB.SCK7.C07C, DeviceObj)
  External(\_SB.SCK7.C07D, DeviceObj)
  External(\_SB.SCK7.C07E, DeviceObj)
  External(\_SB.SCK7.C07F, DeviceObj)
#endif

  External(\_SB.PC00, DeviceObj)
  External(\_SB.PC00.HEC1, DeviceObj)
  External(\_SB.PC00.HEC2, DeviceObj)

  Scope(\)
  {
#if PROCESSOR_AGGREGATOR
    Name(PURA, 0)
    Name(PURB, 0)
    Name(PURE, 0)
#endif
    Name(THNU, 0x800)
  }

  Scope(\_SB.PC00.HEC1)
  {
    //
    // HECI-1 HFS register
    // It holds ME Firmware Status #1
    //
    OperationRegion(MFS1, PCI_Config, 0x40, 4)
    Field(MFS1, DWordAcc, NoLock, Preserve)
    {
      MEFS, 32
    }
    //
    // Return HECI-1 device status according to HECI visibility on PCI bus.
    //
    Method(_STA, 0)
    {
      DbgTrace("H1: _STA")
      DbgTrace(MEFS)
      If (LEqual(MEFS, 0xFFFFFFFF)) // if HECI-1 hidden on disabled
      {
        Return(0x00)
      }
      Return(0x0F)
    }
  }

  Scope(\_SB.PC00.HEC2)
  {
    //
    // H2ST is HECI-2 device status and is checked by OSPM before _INI
    // is called. Initially it is defined as not present and it is
    // updated by SPS ACPI hooks according to true ME state.
    // If ME firmware is not operational, or NM is not enabled
    // HECI-2 is reported present but disabled. If ME is operational
    // HECI-2 is present and functioning, but hidden from UI.
    //
    Name(H2ST, 0x04)
    Method(_STA, 0)
    {
      DbgTrace("H2: _STA")
      DbgTrace(H2ST)
      Return(H2ST)
    }

    //
    // Define the region for HECI-2 registers mapped to HECI-2 MBAR.
    //
    //OperationRegion(HCSR, PCIBARTarget, 0x10, 0x10)
    // The PCIBARTarget is not implemented in several OSes.
    // The address below is just stub to be updated in UpdateAslCode().
    //
    OperationRegion(H2CS, SystemMemory, 0x8000000080000000, 0x10)
    Field(H2CS, DWordAcc, NoLock, Preserve)
    {
      CBWW,32, // write window - R_HECI_HOST_CB_WW
      // Bits of R_HECI_HOST_H_CSR (Host Control Status Register)
      HIE, 1,
      HIS, 1,
      HIG, 1,
      HRD, 1,
      HRS, 1,
         , 3,
      HRP, 8, // host read pointer
      HWP, 8, // host write pointer
      HBD, 8, // host buffer depth
      CBRW,32, // read window - R_HECI_ME_CB_RW
      // Bits of R_HECI_ME_CSR_HA (ME Control Status Register - Host Access)
      MIE, 1,
      MIS, 1,
      MIG, 1,
      MRD, 1,
      MRS, 1,
         , 3,
      MRP, 8, // ME read pointer
      MWP, 8, // ME write pointer
      MBD, 8  // ME buffer depth
    }

    //
    // Init method enables HECI-2 on OS boot
    //
    Method(_INI, 0, Serialized)
    {
      DbgTrace("H2: Init")
      If (LGreater(HWEN, 1))
      {
        DbgTrace("H2: Hardware P-states enabled without legacy support")
        Or(PETE, 0x01, PETE) // Set bit 0 to indicate Hardware P-states for ME
      }
      HWAK()
    }

    //
    // Wake-up handler method - called from platform _WAK() method
    //
    Method(HWAK, 0, Serialized)
    {
      DbgTrace("H2: Wake-up")
      If (And(H2ST, 0x08)) // If HECI-2 functional
      {
        Store(1, HRD) // set host ready
        Store(1, HIE) // set interrupt enable
        Store(1, HIG) // kick ME - I'm ready now
        OSRD()
      }
    }

    //
    // Prepare-to-sleep handler method - called from platform _PTS() method
    //
    Method(HPTS, 0, Serialized)
    {
      DbgTrace("H2: Prepare-to-sleep")
      If (And(H2ST, 0x08)) // If HECI-2 functional
      {
        Store(0, HIE) // clear interrupts enable
        Store(0, HRD) //clear host ready
        Store(1, HIG) // kick ME - I'm not ready any more
      }
    }

    //
    // Send one dword message via HECI-2
    //
    // Arg0 - The body of the message to send.
    //
    Method(SEND, 1, Serialized)
    {
      DbgTrace("H2: Send:")
      DbgTrace(Arg0)
      If (LAnd(HRD, MRD))  // If host and ME side ready
      {
        Store(0x80040011, CBWW)
        Store(Arg0, CBWW)
        Store(1, HIG)
      }
    }

    //
    // Trigger HECI-2 reset
    //
    Method(RSET, 0, Serialized)
    {
      DbgTrace("H2: Resetting HECI")
      Store(1, HIS)
      Store(1, HRS)
      Store(1, HIG)
    }

    //
    // HECI-2 queue read method. It retrieves and dispatches ME requests.
    //
    Method(H2RD, 0, Serialized)
    {
      If (LNot(And(H2ST, 0x08))) // If HECI-2 not functional just return
      {
        Return(0xFF);
      }
      If (MRS) // If reset request from ME - respond with host reset
      {
        DbgTrace("H2: HECI reset requested")
        RSET()
      }
      Else
      {
        If (MRD) // If ME is ready
        {
          If (LNot(HRD)) // ME signals ready, make this side ready too
          {
            DbgTrace("H2: HECI ready")
            Store(0, HRS)
            Store(1, HRD)
            Store(1, HIG)                    // kick ME - I'm ready now
          }
          If (LNotEqual(MWP, MRP))           // If buffer not empty
          {
            Store(CBRW, Local0)              // Read message header
            Store(CBRW, Local1)              // Read message DW1
            DbgTrace("H2: Got message:")
            DbgTrace(Local0)
            DbgTrace(Local1)
            Store(1, HIG)                    // message consumed kick ME - queue is not full

            And(Local0, 0x01FF0000, Local6)  // extract msg length and store in Local6
            And(Local1, 0x000000FF, Local7)  // extract msg command and store in Local7

            If (LEqual(Local7, 0x00))        // if performance change request
            {
              If (LEqual(Local6, 0x00040000)) // If it is 4 bytes long legacy message
              {
                DbgTrace("H2: Handle Performance Change legacy msg")
                //
                // Copy P-state and T-state to Local2 and Local3 to allow handling it
                // as if it was the new message format.
                //
                And(Local1, 0x00FF0000, Local2)
                Or(Local2,  ShiftLeft(And(Local1, 0x00FF0000), 8), Local2)
                Or(Local2, ShiftRight(And(Local1, 0x00FF0000), 8), Local2)
                Or(Local2, ShiftRight(And(Local1, 0x00FF0000),16), Local2)
                Store(Local2, Local3)
                And(Local1, 0xFF000000, Local4)
                Or(Local4, ShiftRight(And(Local1, 0xFF000000), 8), Local4)
                Or(Local4, ShiftRight(And(Local1, 0xFF000000),16), Local4)
                Or(Local4, ShiftRight(And(Local1, 0xFF000000),24), Local4)
                Store(Local4, Local5)
              }
              ElseIf (LEqual(Local6, 0x00140000)) // if new 20 bytes long message
              {
                DbgTrace("H2: Handle Performance Change")
                Store(CBRW, Local2)
                Store(CBRW, Local3)
                Store(CBRW, Local4)
                Store(CBRW, Local5)
              }
              Else
              {
                DbgTrace("H2: Unknown message, resetting HECI")
                RSET()
                Return (0xFF)
              }
              If (And(\PETE, 0x80))
              {
                //
                // If P-states supported update current P-state limit
                //
                DbgTrace("H2: Notify 80 to processors")
                And(           Local2    , 0xFF, Index(\PSLM, 0))
                And(ShiftRight(Local2, 8), 0xFF, Index(\PSLM, 1))
                And(ShiftRight(Local2,16), 0xFF, Index(\PSLM, 2))
                And(ShiftRight(Local2,24), 0xFF, Index(\PSLM, 3))
                And(           Local3    , 0xFF, Index(\PSLM, 4))
                And(ShiftRight(Local3, 8), 0xFF, Index(\PSLM, 5))
                And(ShiftRight(Local3,16), 0xFF, Index(\PSLM, 6))
                And(ShiftRight(Local3,24), 0xFF, Index(\PSLM, 7))
                //
                // Notify processor objects to recalculate limits
                //
                Notify80Socket0()
                Notify80Socket1()
                Notify80Socket2()
                Notify80Socket3()
                Notify80Socket4()
                Notify80Socket5()
                Notify80Socket6()
                Notify80Socket7()
              }
              If (And(\PETE, 0x40))
              {
                //
                // If T-states supported update current T-state limit
                //
                DbgTrace("H2: Notify 82 to processors")
                And(           Local4    , 0xFF, Index(\TSLM, 0))
                And(ShiftRight(Local4, 8), 0xFF, Index(\TSLM, 1))
                And(ShiftRight(Local4,16), 0xFF, Index(\TSLM, 2))
                And(ShiftRight(Local4,24), 0xFF, Index(\TSLM, 3))
                And(           Local5    , 0xFF, Index(\TSLM, 4))
                And(ShiftRight(Local5, 8), 0xFF, Index(\TSLM, 5))
                And(ShiftRight(Local5,16), 0xFF, Index(\TSLM, 6))
                And(ShiftRight(Local5,24), 0xFF, Index(\TSLM, 7))
                //
                // Notify processor objects to recalculate limit
                //
                Notify82Socket0()
                Notify82Socket1()
                Notify82Socket2()
                Notify82Socket3()
                Notify82Socket4()
                Notify82Socket5()
                Notify82Socket6()
                Notify82Socket7()
              }
              //
              // Send ACK for P/T-state message
              //
              SEND(Or(Local1, \PETE))
            } // If cmd is HECI_CMD_MAXPTSTATE
#if PROCESSOR_AGGREGATOR
            ElseIf (LAnd(LEqual(Local7, 0x03), LEqual(Local6, 0x00040000)))
            {
              DbgTrace("H2: Handle Processor Utilisation req")
              If (\PURE)
              {
                Store(Local1, \PURA)
                Notify(\_SB.PRAD, 0x80)
              }
              else
              {
                SEND(Local1)
              }
            }
#endif
#if POWER_METER
            ElseIf (LAnd(LEqual(Local7, 0x04), LEqual(Local6, 0x00040000)))
            {
              DbgTrace("H2: Handle Measurement req")
              Store(Multiply(And(ShiftRight(Local1, 0x10), 0xFFFF), 1000), PWR)
              SEND(Or(Local1, 0x80))
            }
            ElseIf (LAnd(LEqual(Local7, 0x05), LEqual(Local6, 0x00040000)))
            {
              DbgTrace("H2: Handle Get Averaging Interval req")
              Store(Multiply(And(ShiftRight(Local1, 0x10), 0xFFFF), 1000), PMAI)
              SEND(Or(Local1, 0x80))
              Notify(\_SB.PMT1, 0x84)
            }
            ElseIf (LAnd(LEqual(Local7, 0x06), LEqual(Local6, 0x00040000)))
            {
              DbgTrace("H2: Handle Get HW Limit req")
              Store(Multiply(And(ShiftRight(Local1, 0x10), 0xFFFF), 1000), HWL)
              SEND(Or(Local1, 0x80))
              Notify(\_SB.PMT1, 0x82)
            }
            ElseIf (LAnd(LEqual(Local7, 0x0E), LEqual(Local6, 0x00040000)))
            {
              SEND(Or(Local1, 0x80))
              // Check (1) that the OSPM policy has been enforced.
              If (LEqual(And(ShiftRight(Local1, 0x10), 0x01), 0x01))
              {
                Notify(\_SB.PMT1, 0x83)
              }
            }
            ElseIf (LAnd(LEqual(Local7, 0x0C), LEqual(Local6, 0x00040000)))
            {
              DbgTrace("H2: Handle Power State Change req")
              //
              // ACK: Rsvd:Active:SeqNo:Command
              //
              SEND(Or(ShiftLeft(PMAV, 0x10), And(Or(Local1, 0x80), 0xFFFF)))
              If (LEqual(1, PMAV)) // If Power Meter is active
              {
                // capabilities possible change
                If (LEqual(And(ShiftRight(Local1, 0x10), 0xFF), 1))
                {
                  \_SB.PC00.HEC2.SSFW(0, 0x08, 0, 0)
                }
                Store(Divide(HWL, 1000), Local2)
                \_SB.PC00.HEC2.SSFW(0, 0x07, Local2, 0) // Set HWL
                Store(Divide(PMAI, 1000), Local2)
                \_SB.PC00.HEC2.SSFW(0, 0x0A, Local2, 0) // Set PAI
                Store(Divide(PTPU, 1000), Local2)
                Store(Divide(PTPL, 1000), Local3)
                \_SB.PC00.HEC2.SSFW(1, 0x0B, Local2, Local3) // Set PAI
              }
            }
            // if Get Current Capabilities ACK, or Report Capabilities REQ
            ElseIf (LAnd(Or(LEqual(Local7, 0x88),
                             LEqual(Local7, 0x0D)), LEqual(Local6, 0x00200000)))
            {
              DbgTrace("H2: Handle Get Current Capabilities ack or Report Capabilities req")
              Store(And(ShiftRight(Local1, 0x18), 0xFF), Index(PMCP, 0)) // supported capabilities
              Store(And(ShiftRight(Local1, 0x88), 0x01), Index(PMCP, 2)) // measurement type
              If (LEqual(And(ShiftRight(Local1, 0x10), 0x01), 0x01))    // Is hw limit configurable
              {
                Store(0xFFFFFFFF, Index(PMCP, 8))
              }
              Else
              {
                Store(0x00000000, Index(PMCP, 8))
              }
              Store(CBRW, Index(PMCP, 3))
              Store(CBRW, Index(PMCP, 4))
              Store(CBRW, Index(PMCP, 5))
              Store(CBRW, Index(PMCP, 6))
              Store(CBRW, Index(PMCP, 7))
              Store(CBRW, Index(PMCP, 9))
              Store(CBRW, Index(PMCP, 10))
              If (LEqual(Local7, 0x0D))
              {
                SEND(And(Or(Local1, 0x80), 0xFFFF))
              }
              Else
              {
                Store(1, HIG)
              }
              Store(1, PMAV)
              Notify(\_SB.PMT1, 0x80)
            }
            ElseIf (LAnd(LEqual(Local7, 0x09), LEqual(Local6, 0x00040000)))
            {
              DbgTrace("H2: Handle Power Trip Points exceeded req")
              SEND(Or(Local1, 0x80))
              Notify(\_SB.PMT1, 0x81)
            }
            ElseIf (LAnd(LEqual(Local7, 0x87), LEqual(Local6, 0x00040000)))
            {
              DbgTrace("H2: Handle Set Hardware Limit ack")
              Store(And(ShiftRight(Local1, 0x08), 0xFF), HLST)
            }
            ElseIf (LAnd(LEqual(Local7, 0x8A), LEqual(Local6, 0x00040000)))
            {
              DbgTrace("H2: Handle Power Averaging Interval ack")
              Store(And(ShiftRight(Local1, 0x08), 0xFF), AIST)
            }
            ElseIf (LAnd(LEqual(Local7, 0x8B), LEqual(Local6, 0x00040000)))
            {
              DbgTrace("H2: Handle Power Trip Points ack")
              Store(And(ShiftRight(Local1, 0x08), 0xFF), TPST)
            }
            ElseIf (LAnd(LEqual(Local7, OSPM_READY_ACK), LEqual(Local6, 0x00040000)))
            {
              DbgTrace("H2: Handle OSPM READY ack")
              // Do nothing for this ACK
            }
#endif // POWER_METER
            Else
            {
              DbgTrace("H2: Unknown message, resetting HECI")
              RSET()
              Return (0xFF)
            }
            Return(Local7) // The command recently handled is returned by H2RD
          } // If (buffer not empty)
        } // If (MRD)
      } // if (MRS )
      Return(0xFF) // There is no command to return
    } // Method(H2RD,...)

    //
    // SCI handler method - called from _L24() SCI handler
    //
    Method(HSCI, 0, Serialized)
    {
      DbgTrace("H2: SCI Handler for HECI-2")
      If (HIS) // If Host Interrupt Status is Set (when ME_IG is set to 1)
      {
        Store(1, HIS)    // clear interrupt status before reading queue
        Store(8, Local0) // set timeout counter
        While (LNotEqual(H2RD(), 0xFF))
        {
          Decrement(Local0)
          If (LNot(Local0))
          {
            //
            // Too many requests from ME, reset queue to slow it down.
            //
            DbgTrace("H2: Got 8 messages in a row, resetting HECI-2")
            RSET()
            Break
          }
        }
      } // If (HIS)
      //
      // If ME reset occurred during H2RD() HECI may be isolated.
      // We need to make sure HECI Interrupt Enable (HIE) is set
      // before leaving HECI SCI.
      //
      Store(300, Local0) // 3 seconds timeout
      While (LAnd(LNot(HIE), Local0))
      {
        Decrement(Local0)
        Sleep(10)
        Store(1, HIE)
      }
    } // Method(HSCI,...)

    Method(OSRD, 0, Serialized)
    {
      Store(OSPM_READY, Local2)  // Set command code
      Or(ShiftRight(And(OSCP, 0x400), 2), Local2, Local2)  // Set OS capabilities - bit 8
      If (\HWEN)
      {
        // if HWP is enabled, set bit 9
        Or(Local2, 0x200, Local2)
      }

      SEND(Local2) // send message
    } // Method(OSRD, ...)

#if POWER_METER
    //
    // Send request to ME and wait for answer.
    // It sends to ME message frame of 1 or 2 DWORDs,
    // Message header plus depending on input parameter.
    // Arg0  Message size: 0 - 1 DWORD (_SHL, _PAI), 1 - 2 DWORDS (_PTP).
    // Arg1  Command.
    // Arg2  Setting param. Depending on command, it represents
    //         Hardware limit [W], or averaging interval [s], or upper trip point [W]
    // Arg3  Setting param in 2 DWORDs message. Lower trip point [sec].
    //
    Method(SSFW, 4, Serialized)
    {
      If (LAnd(HRD, MRD))  // If host and ME side ready
      {
        Store(HIE, Local1) // Save HECI-2 SCI interrupt enable state
        Store(0, HIE)      // Disable HECI-2 SCI interrupt
        If (LEqual(Arg0, 0))
        {
          Store(0x80040011, CBWW)
          Store(Or(ShiftLeft(Arg2, 0x10), And(Arg1, 0x00FF)), CBWW)
        }
        ElseIf (LEqual(Arg0, 1))
        {
          Store(0x80080011, CBWW)
          Store(And (Arg1, 0xFF), CBWW)
          Store(Or(ShiftLeft(Arg3, 0x10), And(Arg2, 0xFFFF)), CBWW)
        }
        Store(1, HIG)
        Store(0, Local0)  // Timeout counter
        While (LNotEqual(Local0, 10))
        {
          If (LEqual(1, HIS))
          {
            Store(1, HIS) // Clear interrupt status
            If (LEqual(H2RD(), Or(Arg1, 0x80)))
            {
              Break
            }
          }
          Add(Local0, 1, Local0)
          Sleep(100)
        }
        Store(Local1, HIE)  // Restore HECI-2 SCI interrupt enable state
      }
    }
#endif // POWER_METER
  } // Scope(\_SB.PC00.HEC2)

#if POWER_METER
  Scope (\_SB)
  {
    Name(PWR, 0xFFFFFFFF)  // Power reading
    Name(PMAI, 0xFFFFFFFF) // Power meter averaging interval
    Name(HWL, 0xFFFFFFFF)  // HW limit
    Name(PTPU, 0)          // Upper Trip Point
    Name(PTPL, 0)          // Lower Trip Point
    Name(PMAV, 0)          // Is Power Meter interface active
    Name(HLST, 0)          // Set HW Limit Status, return code
    Name(AIST, 0)          // Set Averaging Interval Status, return code
    Name(TPST, 0)          // Set Trip Points Status, return code

    // Power Meter capabilities
    Name(PMCP, Package()
    {
      0x00000000, // Supported capabilities, (from LSB) -> measurement, trip points, HW enforced limit, notifications when HW limit is enforced, 3 x reserved, report only when discharging
      0x00000000, // Measurement unit, mW only
      0x00000000, // Measurement input power
      0x00000000, // Accuracy of PMT measurements, 95000 = 95%
      0xFFFFFFFF, // Sampling time of PMT in ms
      0x00000000, // Min averaging interval in ms
      0x00000000, // Max averaging interval in ms
      0xFFFFFFFF, // Hysteresis margin (mW)
      0x00000000, // Is HW limit configurable, 0x00000000 = read only, 0xFFFFFFFF = writeable
      0x00000000, // Min configurable HW limit (mW)
      0x00000000, // Max configurable HW limit (mW)
      "Intel(R) Node Manager",// PMT model number, empty string if not supported
      "",         // PMT serial number,  empty string if not supported
      "Meter measures total domain" // OEM Info
    })

    Device(PMT1) //Power Meter # 1
    {
      Name(_HID, "ACPI000D")
      Name(_UID,  0x00)      // Unique ID required if many power meters

      //
      // Device status handler method - called by OSPM before _INI.
      // Power Meter status follows HECI-2 status but is not hidden from UI.
      //
      Method(_STA, 0, NotSerialized)
      {
        Return(Or(\_SB.PC00.HEC2._STA, 0x04))
      }

      //
      // Power Meter Capabilities
      //
      Method(_PMC, 0, NotSerialized)
      {
        If (LEqual(PMAV, 0))
        {
          \_SB.PC00.HEC2.SSFW(0, 0x08, 0, 0)
        }
        Return(PMCP)
      }

      //
      // Power Meter Devices
      //
      Method(_PMD, 0, NotSerialized)
      {
        Return(Package(){\_SB})
      }

      //
      // Power Meter Measurement
      //
      Method(_PMM, 0, NotSerialized)
      {
        Return(PWR)
      }

      //
      // Power Meter Averaging Interval
      //
      Method(_PAI, 1, NotSerialized)
      {
        Store(Divide(Arg0, 1000), Local0)
        Store(0x00000002, AIST)
        \_SB.PC00.HEC2.SSFW(0, 0x0A, Local0, 0)
        If (LEqual(And(AIST, 0xFF), 0x00))
        {
          Store(Arg0, PMAI)
          Notify(\_SB.PMT1, 0x84)
        }
        Return(AIST)
      }

      //
      // Get Averaging Interval
      //
      Method(_GAI, 0, NotSerialized)
      {
        Return(PMAI)
      }

      //
      // Power Trip Points
      //
      Method(_PTP, 2, NotSerialized)
      {
        Store(Divide(Arg0, 1000), Local0)
        //
        // Round lower trip point to higher value
        //
        Store(Add(Arg1, 500), Local1)
        Store(Divide(Arg1, 1000), Local1)
        Store(0x00000002, TPST)
        \_SB.PC00.HEC2.SSFW(1, 0x0B, Local0, Local1)
        If (LEqual(And(TPST, 0xFF), 0))
        {
          Store(Arg0, PTPU)
          Store(Arg1, PTPL)
        }
        Return (TPST)
      }

      //
      //Get Hardware Limit
      //
      Method(_GHL, 0, NotSerialized)
      {
        Return(HWL)
      }

      //
      // Set Hardware Limit
      //
      Method(_SHL, 1, NotSerialized)
      {
        Store(Divide(Arg0, 1000), Local0)
        Store(0x00000002, HLST)
        \_SB.PC00.HEC2.SSFW (0, 0x07, Local0, 0)
        If (LEqual(And(HLST, 0xFF), 0x00))
        {
          Store(Arg0, HWL)
        }
        Return (HLST)
      }
    } // Device(PMT1)
  } // Scope(SB)
#endif // POWER_METER

#if PROCESSOR_AGGREGATOR
  Scope(\_SB)
  {
    //
    // Processor Aggregator Device
    //
    Device(PRAD)
    {
      Name(_HID, "ACPI000C")

      //
      // Device status handler method - called by OSPM before _INI.
      // If OSPM support Processor Aggregator Device its status follows HECI-2
      // status, but is not hidden from UI. If not then report "not present".
      //
      Method(_STA)
      {
        If (\_OSI("Processor Aggregator Device"))
        {
          DbgTrace("PRAD: OS does support Processor Aggregator Device")
          Store(1, \PURE)
          Return(Or(\_SB.PC00.HEC2._STA, 0x04))
        }
        Else
        {
          DbgTrace("PRAD: OS does NOT support Processor Aggregator Device")
          Return(0)
        }
      }

      //
      // Processor Utilization Request method returns the
      // number of processors that platform wants to idle.
      //
      Method(_PUR, 0, Serialized)
      {
        DbgTrace("PRAD: Entering _PUR")
        Name(IDL, Package() {1, 0})
        And(Or(\PURA, 0x80), 0xFFFF, \PURB)
        And(ShiftRight(\PURA, 16), 0xffff, Index(IDL, 1))
        Return(IDL)
      }

      //
      // OSPM Status Indication method
      //
      Method(_OST, 3, Serialized)
      {
        DbgTrace("PRAD: Entering _OST")
        //
        // If _OST is called after _PUR send ACK to ME.
        // Arg2 conveys number of threads currently idled.
        // Calculate number of active threads and send it to ME.
        //
        If (LEqual(And(\PURB, 0xff), 0x83))
        {
          Subtract(\THNU, ToInteger(Arg2), Local0)
          Or(\PURB, ShiftLeft(Local0, 16), \PURB)
          \_SB.PC00.HEC2.SEND(\PURB) // Send ACK for core idling
        }
      } // _OST
    } // Device PRAD
  } // Scope SB
#endif // PROCESSOR_AGGREGATOR

  Scope(\_GPE)
  {
    Method(_L6C, 0x0, Serialized)
    {
      \_SB.PC00.HEC2.HSCI()
    }
  }
}// Definition Block


