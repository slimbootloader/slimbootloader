/** @file
  Header file for ACPI GPIO Resources

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#ifndef _ACPI_GPIO_H_
#define _ACPI_GPIO_H_

//
// GpioIo (GPIO Connection IO Resource Descriptor)
// GpioIo (Shared, PinConfig, DebounceTimeout, DriveStrength, IORestriction, ResourceSource,
//         ResourceSourceIndex, ResourceUsage, DescriptorName, VendorData) {PinList}
// Example: GpioIo (Exclusive, PullDefault, 0, 0, IoRestrictionOutputOnly, "\\_SB.GPI0",,, ABCD) {0}
// Macro Arguments:
// - Shared
#define GPIO_IO_SHR_SHARED       0x1 // This IO connection is shared by two or more devices
#define GPIO_IO_SHR_EXCLUSIVE    0x0 // This IO connection is used exclusively by one device
// - PinConfig can be one of PullDefault, PullUp, PullDown, PullNone or a vendor-supplied value in the range 128-255
#define GPIO_PPI_PULL_DEFAULT 0x0
#define GPIO_PPI_PULL_UP      0x1
#define GPIO_PPI_PULL_DOWN    0x2
#define GPIO_PPI_PULL_NONE    0x3
// - DebounceTimeout _DBT - no macro available
// - DriveStrength _DRS - no macro available
// - IORestriction limit the mode in which the pin can be accessed (Input or Output)
// This pin or pins can be used for either Input or Output.
#define GPIO_IO_IOR_NONE              0x0 // IoRestrictionNone
// This pin or pins can only be used for Input, and the
// pin configuration must be preserved while not in use.
#define GPIO_IO_IOR_INPUT_ONLY        0x1 // IoRestrictionInputOnly
// This pin or pins can only be used for Output, and the
// pin configuration must be preserved while not in use.
#define GPIO_IO_IOR_OUTPUT_ONLY       0x2 // IoRestrictionOutputOnly
// This pin or pins can be used for either input or output,
// but the configuration must be preserved until explicitly changed.
#define GPIO_IO_IOR_NONE_AND_PRESERVE 0x3 // IORestrictionNoneAndPreserve
// - ResourceSource  is a string which uniquely identifies the GPIO controller referred to by this descriptor
// Automatically filled by the G_IO
// Below Fields are SOC/PCH specific
#define GPIO_RESOURCE_SOURCE_GPI0 "\\_SB.GPI0"
#define GPIO_RESOURCE_SOURCE_GPI1 "\\_SB.GPI1"
#define GPIO_RESOURCE_SOURCE_GPI2 "\\_SB.GPI2"
#define GPIO_RESOURCE_SOURCE_GPI3 "\\_SB.GPI3"
#define GPIO_RESOURCE_SOURCE_GPI4 "\\_SB.GPI4"
#define GPIO_RESOURCE_SOURCE_GPI5 "\\_SB.GPI5"
// - ResourceSourceIndex - optional argument and is always 0 - no macro available
// - ResourceUsage - always ResourceConsumer - no macro available
// - DescriptorName - specifies a name for an integer constant, used internally by G_IO - no macro available
// - VendorData -  RawDataBuffer containing vendor-defined byte data - not used, no macro available
// - PinList - is a list of pin numbers on the ResourceSource that are described by this descriptor. Automatically filled by the G_IO


//
// GpioInt (GPIO Interrupt Connection Resource Descriptor Macro)
// GpioInt (EdgeLevel, ActiveLevel, Shared, PinConfig, DebounceTimeout, ResourceSource,
//          ResourceSourceIndex, ResourceUsage, DescriptorName, VendorData) {PinList}
// Example: GpioInt (Edge, ActiveLow, ExclusiveAndWake, PullDefault, 0, "\\_SB.GPI0", 0, ResourceConsumer, ABCD) {0}
// In the new scheme the reporting of GPIOs to the OS needs to be changed.
// Old definition:
// GpioInt(Edge, ActiveLow, ExclusiveAndWake, PullDefault, 0x0000, "\\_SB.GPI0", 0x00, ResourceConsumer, INT1) {32 * GroupNumber + PadNumberWithinGroup}
// New definition:
// GpioInt(Edge, ActiveLow, ExclusiveAndWake, PullDefault, 0x0000, "\\SB.GPI3",  ResourceConsumer, INT1) {PadNumberWithinCommunity}
// Note that the string must point to the correct ACPI device and the pad number calculation is replaced with a pad number within community.
// Pad number calculation also applies to the GpioIo macro.
// To simplify creating of those macros we provide 2 helper functions:
// G_IO and G_IN which create GpioIo and GpioInt macro respectively. Applying them to the previous example:
// G_IN(GPIOV2_XXX_PAD_NAME, Edge, ActiveLow, ExclusiveAndWake, PullDefault, 0x0000)
// Those functions accept pad define from OneSiliconPkg/Include/Register/XXX/GpioV2XXXPins/GpioV2XXXPins.h
// XXX stands for SOC name.
// Macro Arguments:
// - EdgeLevel - Interrupt Mode
#define GPIO_INT_MOD_LEVEL 0x0 // Interrupt is triggered in response to the signal being in either a high or low state.
#define GPIO_INT_MOD_EDGE  0x1 // This interrupt is triggered in response to a change in signal state, either high to low or low to high
// - ActiveLevel Interrupt polarity
#define GPIO_INT_POL_ACTIVE_HIGH     0x0 // This interrupt is sampled when the signal is high, or true.
#define GPIO_INT_POL_ACTIVE_LOW      0x1 // This interrupt is sampled when the signal is low, or false.
#define GPIO_INT_POL_ACTIVE_BOTH     0x2 // This interrupt is sampled on both rising and falling edges. Interrupt mode must be set to Edge-triggered.
// - Shared - Interrupt Sharing (_SHR) and Wake Capability (_WKC)
// Shared, Exclusive, SharedAndWake or ExclusiveAndWake
#define GPIO_INT_EXCLUSIVE           0x0 // This IO connection is used exclusively by one device, This interrupt is not capable of waking the system.
#define GPIO_INT_SHARED              0x1 // This IO connection is shared by two or more devices, This interrupt is not capable of waking the system.
// This IO connection is used exclusively by one device
// This interrupt is capable of waking the system from a low-power idle state or a system sleep state.
#define GPIO_INT_EXCLUSIVE_AND_WAKE  0x2
// This IO connection is shared by two or more devices,
// This interrupt is capable of waking the system from a low-power idle state or a system sleep state.
#define GPIO_INT_SHARED_AND_WAKE     0x3
// - PinConfig
// Refer to #define GPIO_PPI_*
// - DebounceTimeout _DBT - no macro available
// - ResourceSource  is a string which uniquely identifies the GPIO controller referred to by this descriptor
// Automatically filled by the G_IN
// Below Fields are SOC/PCH specific
// Use #define GPIO_RESOURCE_SOURCE_GPI<Number>
// - ResourceSourceIndex - optional argument and is always 0 - no macro available
// - ResourceUsage - always ResourceConsumer - no macro available
// - DescriptorName - specifies a name for an integer constant, used internally by G_IN - no macro available
// - VendorData -  RawDataBuffer containing vendor-defined byte data - not used, no macro available
// - PinList - is a list of pin numbers on the ResourceSource that are described by this descriptor. Automatically filled by the G_IN

#endif
