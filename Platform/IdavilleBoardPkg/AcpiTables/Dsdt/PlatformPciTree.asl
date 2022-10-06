/** @file

  Copyright (c) 2020 - 2022, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef CONCATENATE2
#define CONCATENATE2_(a, b) a ## b
#define CONCATENATE2(a, b)  CONCATENATE2_(a, b)
#endif
#ifndef CONCATENATE3
#define CONCATENATE3_(a, b, c) a ## b ## c
#define CONCATENATE3(a, b, c) CONCATENATE3_(a, b, c)
#endif

Scope (\_SB) {

    Name (AR00, Package() {
        // [IIM0]: IIOMISC on PC00
        Package() { 0x0000FFFF, 0, 0, 16 },
        Package() { 0x0000FFFF, 1, 0, 17 },
        Package() { 0x0000FFFF, 2, 0, 18 },
        Package() { 0x0000FFFF, 3, 0, 19 },
        // [CB0A]: CB3DMA on PC00
        // [CB0E]: CB3DMA on PC00
        Package() { 0x0001FFFF, 0, 0, 16 },
        // [CB0B]: CB3DMA on PC00
        // [CB0F]: CB3DMA on PC00
        Package() { 0x0001FFFF, 1, 0, 17 },
        // [CB0C]: CB3DMA on PC00
        // [CB0G]: CB3DMA on PC00
        Package() { 0x0001FFFF, 2, 0, 18 },
        // [CB0D]: CB3DMA on PC00
        // [CB0H]: CB3DMA on PC00
        Package() { 0x0001FFFF, 3, 0, 19 },
        // CPU Trace Hub
        Package() { 0x0002FFFF, 0, 0, 16 },
        Package() { 0x0002FFFF, 1, 0, 17 },
        Package() { 0x0002FFFF, 2, 0, 18 },
        Package() { 0x0002FFFF, 3, 0, 19 },
        // [RLK0]: Legacy PCI Express Port 0 on PC00
        Package() { 0x0003FFFF, 0, 0, 16 },
        // VRP0 for nCPM in CDF
        Package() { 0x0006FFFF, 0, 0, 16 },
        // [SAT1]: SATA controller 1 on PCH
        Package() { 0x0007FFFF, 0, 0, 16 },
        // [SAT2]: sSATA Host controller 2 on PCH
        Package() { 0x0008FFFF, 0, 0, 16 },
        // [RP01]: PCIE PCH Root Port #1
        Package() { 0x0009FFFF, 0, 0, 16 },
        // [RP02]: PCIE PCH Root Port #2
        Package() { 0x000AFFFF, 0, 0, 16 },
        // [RP03]: PCIE PCH Root Port #3
        Package() { 0x000BFFFF, 0, 0, 16 },
        // [RP04]: PCIE PCH Root Port #4
        Package() { 0x000CFFFF, 0, 0, 16 },
        // [SAT3]: sSATA Host controller 3 on PCH
        Package() { 0x000EFFFF, 0, 0, 16 },
        // [SMBD]: SMBus DMA
        Package() { 0x000FFFFF, 0, 0, 16 },
        // [RP05]: PCIE PCH Root Port #5
        Package() { 0x0010FFFF, 0, 0, 16 },
        // [RP06]: PCIE PCH Root Port #6
        Package() { 0x0011FFFF, 0, 0, 16 },
        // [RP07]: PCIE PCH Root Port #7
        Package() { 0x0012FFFF, 0, 0, 16 },
        // [RP08]: PCIE PCH Root Port #8
        Package() { 0x0013FFFF, 0, 0, 16 },
        // [RP09]: PCIE PCH Root Port #9
        Package() { 0x0014FFFF, 0, 0, 16 },
        // [RP10]: PCIE PCH Root Port #10
        Package() { 0x0015FFFF, 0, 0, 16 },
        // [RP11]: PCIE PCH Root Port #11
        Package() { 0x0016FFFF, 0, 0, 16 },
        // [RP12]: PCIE PCH Root Port #12
        Package() { 0x0017FFFF, 0, 0, 16 },
        // [HEC1]: HECI #1 on PCH
        // [HEC3]: HECI #3 on PCH
        Package() { 0x0018FFFF, 0, 0, 16 },
        // [HEC2]: HECI #2 on PCH
        // [HEC4]: HECI #4 on PCH
        Package() { 0x0018FFFF, 1, 0, 17 },
        // [IDER]: ME IDE redirect on PCH
        Package() { 0x0018FFFF, 2, 0, 18 },
        // [MEKT]: ME KT on PCH
        Package() { 0x0018FFFF, 3, 0, 19 },
        // [UAR0]: UART #0 on PCH
        Package() { 0x001AFFFF, 0, 0, 16 },
        // [UAR1]: UART #1 on PCH
        Package() { 0x001AFFFF, 1, 0, 17 },
        // [UAR2]: UART #2 on PCH
        Package() { 0x001AFFFF, 2, 0, 18 },
        // [eMMC]: eMMC controller on PCH
        Package() { 0x001CFFFF, 0, 0, 16 },
        // [XHCI]: xHCI controller 1 on PCH
        Package() { 0x001EFFFF, 0, 0, 16 },
        // [LPC0]: ISA Bridge on PCH
        Package() { 0x001FFFFF, 2, 0, 18 },
        // [SMBS]: SMBus controller on PCH
        Package() { 0x001FFFFF, 1, 0, 17 },
        // [NTPK]: Northpeak Controller on PCH
        Package() { 0x001FFFFF, 0, 0, 16 },
        // [SPIC]: SPI controller on PCH
        Package() { 0x001FFFFF, 3, 0, 19 },
    })

    Name (AR10, Package() {
        // [UBX0]: Uncore 0 UBOX Device
        Package() { 0x0000FFFF, 0, 0, 16 },
        Package() { 0x0000FFFF, 1, 0, 17 },
        Package() { 0x0000FFFF, 2, 0, 18 },
        Package() { 0x0000FFFF, 3, 0, 19 },
        // [CSM0]: Uncore 0 CHASIS_SMBUS Devices
        Package() { 0x000BFFFF, 0, 0, 16 },
        Package() { 0x000BFFFF, 1, 0, 17 },
        Package() { 0x000BFFFF, 2, 0, 18 },
        Package() { 0x000BFFFF, 3, 0, 19 },
        // [M2M0]: Uncore 0 MS2MEM_SCF_MS2MEM0 Device
        Package() { 0x000CFFFF, 0, 0, 16 },
        Package() { 0x000CFFFF, 1, 0, 17 },
        Package() { 0x000CFFFF, 2, 0, 18 },
        Package() { 0x000CFFFF, 3, 0, 19 },
        // [M2M1]: Uncore 0 MS2MEM_SCF_MS2MEM1 Device
        Package() { 0x000DFFFF, 0, 0, 16 },
        Package() { 0x000DFFFF, 1, 0, 17 },
        Package() { 0x000DFFFF, 2, 0, 18 },
        Package() { 0x000DFFFF, 3, 0, 19 },
        // [MCD0]: Uncore 0 MCDDR0 Device
        Package() { 0x001AFFFF, 0, 0, 16 },
        Package() { 0x001AFFFF, 1, 0, 17 },
        Package() { 0x001AFFFF, 2, 0, 18 },
        Package() { 0x001AFFFF, 3, 0, 19 },
        // [MCD1]: Uncore 0 MCDDR1 Device
        Package() { 0x001BFFFF, 0, 0, 16 },
        Package() { 0x001BFFFF, 1, 0, 17 },
        Package() { 0x001BFFFF, 2, 0, 18 },
        Package() { 0x001BFFFF, 3, 0, 19 },
    })

    Name (AR11, Package() {
        // [CHA1]: Uncore 1 GRP1_CHA8-10 Device
        Package() { 0x0001FFFF, 0, 0, 16 },
        Package() { 0x0001FFFF, 1, 0, 17 },
        Package() { 0x0001FFFF, 2, 0, 18 },
        Package() { 0x0001FFFF, 3, 0, 19 },
        // [CHA2]: Uncore 1 GRP0_CHA0-7 Device
        Package() { 0x000AFFFF, 0, 0, 16 },
        Package() { 0x000AFFFF, 1, 0, 17 },
        Package() { 0x000AFFFF, 2, 0, 18 },
        Package() { 0x000AFFFF, 3, 0, 19 },
        // [CHA3]: Uncore 1 GRP0_CHA8-10 Device
        Package() { 0x00B0FFFF, 0, 0, 16 },
        Package() { 0x00B0FFFF, 1, 0, 17 },
        Package() { 0x00B0FFFF, 2, 0, 18 },
        Package() { 0x00B0FFFF, 3, 0, 19 },
        // [CA00]: Uncore 1 CHAALL0-1 Device
        Package() { 0x001DFFFF, 0, 0, 16 },
        Package() { 0x001DFFFF, 1, 0, 17 },
        Package() { 0x001DFFFF, 2, 0, 18 },
        Package() { 0x001DFFFF, 3, 0, 19 },
        // [PUC0]: Uncore 1 CHASIS_PUINT0-7 Device
        Package() { 0x001EFFFF, 0, 0, 16 },
        Package() { 0x001EFFFF, 1, 0, 17 },
        Package() { 0x001EFFFF, 2, 0, 18 },
        Package() { 0x001EFFFF, 3, 0, 19 },
        // [GN30]: Uncore 1 Gen3Phy Device
        Package() { 0x001FFFFF, 0, 0, 16 },
        Package() { 0x001FFFFF, 1, 0, 17 },
        Package() { 0x001FFFFF, 2, 0, 18 },
        Package() { 0x001FFFFF, 3, 0, 19 },
    })

    Name(AR12, Package(){
      Package(){0x0000FFFF, 0, 0, 16 },
      Package(){0x0000FFFF, 1, 0, 17 },
      Package(){0x0000FFFF, 2, 0, 18 },
      Package(){0x0000FFFF, 3, 0, 19 },
    })

#if PIC_MODE_SUP
    Name(PD12, Package(){
      Package(){0x0000FFFF, 0, 0, 11 },
      Package(){0x0000FFFF, 1, 0, 10 },
      Package(){0x0000FFFF, 2, 0, 11 },
      Package(){0x0000FFFF, 3, 0, 11 },
    })
#else
    Name(PD12, Package(){
      Package(){0x0000FFFF, 0, 0, 16 },
      Package(){0x0000FFFF, 1, 0, 17 },
      Package(){0x0000FFFF, 2, 0, 18 },
      Package(){0x0000FFFF, 3, 0, 19 },
    })
#endif
    Name(AR13, Package(){
      Package(){0x0000FFFF, 0, 0, 16 },
      Package(){0x0000FFFF, 1, 0, 17 },
      Package(){0x0000FFFF, 2, 0, 18 },
      Package(){0x0000FFFF, 3, 0, 19 },
    })

#if PIC_MODE_SUP
    Name(PD13, Package(){
      Package(){0x0000FFFF, 0, 0, 11 },
      Package(){0x0000FFFF, 1, 0, 10 },
      Package(){0x0000FFFF, 2, 0, 11 },
      Package(){0x0000FFFF, 3, 0, 11 },
    })
#else
    Name(PD13, Package(){
      Package(){0x0000FFFF, 0, 0, 16 },
      Package(){0x0000FFFF, 1, 0, 17 },
      Package(){0x0000FFFF, 2, 0, 18 },
      Package(){0x0000FFFF, 3, 0, 19 },
    })
#endif

    Name(AR14, Package(){
      Package(){0x0000FFFF, 0, 0, 16 },
      Package(){0x0000FFFF, 1, 0, 17 },
      Package(){0x0000FFFF, 2, 0, 18 },
      Package(){0x0000FFFF, 3, 0, 19 },
    })

#if PIC_MODE_SUP
    Name(PD14, Package(){
      Package(){0x0000FFFF, 0, 0, 11 },
      Package(){0x0000FFFF, 1, 0, 10 },
      Package(){0x0000FFFF, 2, 0, 11 },
      Package(){0x0000FFFF, 3, 0, 11 },
    })
#else
    Name(PD14, Package(){
      Package(){0x0000FFFF, 0, 0, 16 },
      Package(){0x0000FFFF, 1, 0, 17 },
      Package(){0x0000FFFF, 2, 0, 18 },
      Package(){0x0000FFFF, 3, 0, 19 },
    })
#endif

    Name(AR15, Package(){
      Package(){0x0000FFFF, 0, 0, 16 },
      Package(){0x0000FFFF, 1, 0, 17 },
      Package(){0x0000FFFF, 2, 0, 18 },
      Package(){0x0000FFFF, 3, 0, 19 },
    })

#if PIC_MODE_SUP
    Name(PD15, Package(){
      Package(){0x0000FFFF, 0, 0, 11 },
      Package(){0x0000FFFF, 1, 0, 10 },
      Package(){0x0000FFFF, 2, 0, 11 },
      Package(){0x0000FFFF, 3, 0, 11 },
    })
#else
    Name(PD15, Package(){
      Package(){0x0000FFFF, 0, 0, 16 },
      Package(){0x0000FFFF, 1, 0, 17 },
      Package(){0x0000FFFF, 2, 0, 18 },
      Package(){0x0000FFFF, 3, 0, 19 },
    })
#endif

    Name(AR16, Package(){
      Package(){0x0000FFFF, 0, 0, 16 },
      Package(){0x0000FFFF, 1, 0, 17 },
      Package(){0x0000FFFF, 2, 0, 18 },
      Package(){0x0000FFFF, 3, 0, 19 },
    })

#if PIC_MODE_SUP
    Name(PD16, Package(){
      Package(){0x0000FFFF, 0, 0, 11 },
      Package(){0x0000FFFF, 1, 0, 10 },
      Package(){0x0000FFFF, 2, 0, 11 },
      Package(){0x0000FFFF, 3, 0, 11 },
    })
#else
    Name(PD16, Package(){
      Package(){0x0000FFFF, 0, 0, 16 },
      Package(){0x0000FFFF, 1, 0, 17 },
      Package(){0x0000FFFF, 2, 0, 18 },
      Package(){0x0000FFFF, 3, 0, 19 },
    })
#endif

    Name(AR17, Package(){
      Package(){0x0000FFFF, 0, 0, 16 },
      Package(){0x0000FFFF, 1, 0, 17 },
      Package(){0x0000FFFF, 2, 0, 18 },
      Package(){0x0000FFFF, 3, 0, 19 },
    })

#if PIC_MODE_SUP
    Name(PD17, Package(){
      Package(){0x0000FFFF, 0, 0, 11 },
      Package(){0x0000FFFF, 1, 0, 10 },
      Package(){0x0000FFFF, 2, 0, 11 },
      Package(){0x0000FFFF, 3, 0, 11 },
    })
#else
    Name(PD17, Package(){
      Package(){0x0000FFFF, 0, 0, 16 },
      Package(){0x0000FFFF, 1, 0, 17 },
      Package(){0x0000FFFF, 2, 0, 18 },
      Package(){0x0000FFFF, 3, 0, 19 },
    })
#endif

    Name(AR18, Package(){
      Package(){0x0000FFFF, 0, 0, 16 },
      Package(){0x0000FFFF, 1, 0, 17 },
      Package(){0x0000FFFF, 2, 0, 18 },
      Package(){0x0000FFFF, 3, 0, 19 },
    })

#if PIC_MODE_SUP
    Name(PD18, Package(){
      Package(){0x0000FFFF, 0, 0, 11 },
      Package(){0x0000FFFF, 1, 0, 10 },
      Package(){0x0000FFFF, 2, 0, 11 },
      Package(){0x0000FFFF, 3, 0, 11 },
    })
#else
    Name(PD18, Package(){
      Package(){0x0000FFFF, 0, 0, 16 },
      Package(){0x0000FFFF, 1, 0, 17 },
      Package(){0x0000FFFF, 2, 0, 18 },
      Package(){0x0000FFFF, 3, 0, 19 },
    })
#endif

    Name(AR19, Package(){
      Package(){0x0000FFFF, 0, 0, 16 },
      Package(){0x0000FFFF, 1, 0, 17 },
      Package(){0x0000FFFF, 2, 0, 18 },
      Package(){0x0000FFFF, 3, 0, 19 },
    })

#if PIC_MODE_SUP
    Name(PD19, Package(){
      Package(){0x0000FFFF, 0, 0, 11 },
      Package(){0x0000FFFF, 1, 0, 10 },
      Package(){0x0000FFFF, 2, 0, 11 },
      Package(){0x0000FFFF, 3, 0, 11 },
    })
#else
    Name(PD19, Package(){
      Package(){0x0000FFFF, 0, 0, 16 },
      Package(){0x0000FFFF, 1, 0, 17 },
      Package(){0x0000FFFF, 2, 0, 18 },
      Package(){0x0000FFFF, 3, 0, 19 },
    })
#endif

    Name(AR1A, Package(){
      Package(){0x0000FFFF, 0, 0, 16 },
      Package(){0x0000FFFF, 1, 0, 17 },
      Package(){0x0000FFFF, 2, 0, 18 },
      Package(){0x0000FFFF, 3, 0, 19 },
    })

#if PIC_MODE_SUP
    Name(PD1A, Package(){
      Package(){0x0000FFFF, 0, 0, 11 },
      Package(){0x0000FFFF, 1, 0, 10 },
      Package(){0x0000FFFF, 2, 0, 11 },
      Package(){0x0000FFFF, 3, 0, 11 },
    })
#else
    Name(PD1A, Package(){
      Package(){0x0000FFFF, 0, 0, 16 },
      Package(){0x0000FFFF, 1, 0, 17 },
      Package(){0x0000FFFF, 2, 0, 18 },
      Package(){0x0000FFFF, 3, 0, 19 },
    })
#endif

    Name(AR1B, Package(){
      Package(){0x0000FFFF, 0, 0, 16 },
      Package(){0x0000FFFF, 1, 0, 17 },
      Package(){0x0000FFFF, 2, 0, 18 },
      Package(){0x0000FFFF, 3, 0, 19 },
    })

#if PIC_MODE_SUP
    Name(PD1B, Package(){
      Package(){0x0000FFFF, 0, 0, 11 },
      Package(){0x0000FFFF, 1, 0, 10 },
      Package(){0x0000FFFF, 2, 0, 11 },
      Package(){0x0000FFFF, 3, 0, 11 },
    })
#else
    Name(PD1B, Package(){
      Package(){0x0000FFFF, 0, 0, 16 },
      Package(){0x0000FFFF, 1, 0, 17 },
      Package(){0x0000FFFF, 2, 0, 18 },
      Package(){0x0000FFFF, 3, 0, 19 },
    })
#endif

    Name(AR1C, Package(){
      Package(){0x0000FFFF, 0, 0, 16 },
      Package(){0x0000FFFF, 1, 0, 17 },
      Package(){0x0000FFFF, 2, 0, 18 },
      Package(){0x0000FFFF, 3, 0, 19 },
    })

#if PIC_MODE_SUP
    Name(PD1C, Package(){
      Package(){0x0000FFFF, 0, 0, 11 },
      Package(){0x0000FFFF, 1, 0, 10 },
      Package(){0x0000FFFF, 2, 0, 11 },
      Package(){0x0000FFFF, 3, 0, 11 },
    })
#else
    Name(PD1C, Package(){
      Package(){0x0000FFFF, 0, 0, 16 },
      Package(){0x0000FFFF, 1, 0, 17 },
      Package(){0x0000FFFF, 2, 0, 18 },
      Package(){0x0000FFFF, 3, 0, 19 },
    })
#endif

    Name(AR1D, Package(){
      Package(){0x0000FFFF, 0, 0, 16 },
      Package(){0x0000FFFF, 1, 0, 17 },
      Package(){0x0000FFFF, 2, 0, 18 },
      Package(){0x0000FFFF, 3, 0, 19 },
    })

#if PIC_MODE_SUP
    Name(PD1D, Package(){
      Package(){0x0000FFFF, 0, 0, 11 },
      Package(){0x0000FFFF, 1, 0, 10 },
      Package(){0x0000FFFF, 2, 0, 11 },
      Package(){0x0000FFFF, 3, 0, 11 },
    })
#else
    Name(PD1D, Package(){
      Package(){0x0000FFFF, 0, 0, 16 },
      Package(){0x0000FFFF, 1, 0, 17 },
      Package(){0x0000FFFF, 2, 0, 18 },
      Package(){0x0000FFFF, 3, 0, 19 },
    })
#endif
    //
    // Socket 0 Root bridge (Stack 0)
    //
#undef SOCKET
#undef STACK
#define SOCKET 0
#define STACK  0
    Device (PC00) {
      Name(_HID, EISAID("PNP0A08"))
      Name(_CID, EISAID("PNP0A03"))
      Name(_UID, 0x00)

      //
      // _PRT Pci Routing Table – returns a list of PCI interrupt mappings.
      //
      Method (_PRT, 0)
      {
        Return (AR00)
      }

        #include "PC00.asi"

      // IIOMISC on PC00
      Device (IIM0) {
        Name(_ADR, 0x00000000)
      }

      // CB3DMA on PC00
      Device (CB0A) {
        Name(_ADR, 0x00010000)
      }

      // CB3DMA on PC00
      Device (CB0B) {
        Name(_ADR, 0x00010001)
      }

      // CB3DMA on PC00
      Device (CB0C) {
        Name(_ADR, 0x00010002)
      }

      // CB3DMA on PC00
      Device (CB0D) {
        Name(_ADR, 0x00010003)
      }

      // CB3DMA on PC00
      Device (CB0E) {
        Name(_ADR, 0x00010004)
      }

      // CB3DMA on PC00
      Device (CB0F) {
        Name(_ADR, 0x00010005)
      }

      // CB3DMA on PC00
      Device (CB0G) {
        Name(_ADR, 0x00010006)
      }

      // CB3DMA on PC00
      Device (CB0H) {
        Name(_ADR, 0x00010007)
      }

      // Legacy PCI Express Port 0 on PC00
      Device (RLK0) {
        Name(_ADR, 0x00030000)
      }

      // VRP0 for nCPM from CDF
      Device (VRP0) {

        Name(_ADR, 0x00060000)
        //
        // Power Resources for Wake – returns a list of dependent power resources for waking.
        //
        Method (_PRW, 0)
        {
          Return (Package (0x02) {0x09, 0x04})
        }
        //
        // _PRT Pci Routing Table – returns a list of PCI interrupt mappings.
        //
        Method (_PRT, 0)
        {
          Return (Package() {Package() { 0x0000FFFF, 0, 0, 16 }})
        }
      }

      // SMBus DMA
      Device (SMBD) {
        Name(_ADR, 0x000F0000)
      }

      // UART #0 on PCH
      Device (UAR0) {
        Name(_ADR, 0x001A0000)
      }

      // UART #1 on PCH
      Device (UAR1) {
        Name(_ADR, 0x001A0001)
      }

      // UART #2 on PCH
      Device (UAR2) {
        Name(_ADR, 0x001A0002)
      }

      // HECI #1 on PCH
      Device (HEC1) {
        Name(_ADR, 0x00180000)
      }

      // HECI #2 on PCH
      Device (HEC2) {
        Name(_ADR, 0x00180001)
      }

      // ME IDE redirect on PCH
      Device (IDER) {
        Name(_ADR, 0x00180002)
      }

      // ME KT on PCH
      Device (MEKT) {
        Name(_ADR, 0x00180003)
      }

      // HECI #3 on PCH
      Device (HEC3) {
        Name(_ADR, 0x00180004)
      }

      // HECI #4 on PCH
      Device (HEC4) {
        Name(_ADR, 0x00180006)
      }

      // ISA Bridge on PCH
      Device (LPC0) {
        Name(_ADR, 0x001F0000)

        #include "Mother.asi"
      }

      // Power Management Controller on PCH
      Device (PMC1) {
        Name(_ADR, 0x001F0002)
      }

      // SMBus controller on PCH
      Device (SMBS) {
        Name(_ADR, 0x001F0004)
      }

      // SPI controller on PCH
      Device (SPIC) {
        Name(_ADR, 0x001F0005)
      }

      // Northpeak Phantom (ACPI) Function on PCH
      Device (NTHP) {
        Name(_ADR, 0x001F0006)
      }

      // Northpeak Controller on PCH
      Device (NTPK) {
        Name(_ADR, 0x001F0007)
      }
    }
    //
    // Socket 0 Root bridge (Stack 1)
    //
#undef SOCKET
#undef STACK
#define SOCKET 0
#define STACK  1
    Device (PC01) {
      Name(_HID, EISAID("PNP0A08"))
      Name(_CID, EISAID("PNP0A03"))
      Name(_UID, 0x01)

      //
      // _PRT Pci Routing Table – returns a list of PCI interrupt mappings.
      //
      Method (_PRT, 0)
      {
        Return (Package() {// [RP1A]: PCI Express Port A on PC01
                           Package() { 0x0002FFFF, 0, 0, 18 },
                           Package() { 0x0002FFFF, 1, 0, 19 },
                           Package() { 0x0002FFFF, 2, 0, 16 },
                           Package() { 0x0002FFFF, 3, 0, 17 },
                           // [RP1B]: PCI Express Port B on PC01
                           Package() { 0x0003FFFF, 0, 0, 19 },
                           Package() { 0x0003FFFF, 1, 0, 16 },
                           Package() { 0x0003FFFF, 2, 0, 17 },
                           Package() { 0x0003FFFF, 3, 0, 18 },
                           // [RP1C]: PCI Express Port C on PC01
                           Package() { 0x0004FFFF, 0, 0, 16 },
                           Package() { 0x0004FFFF, 1, 0, 17 },
                           Package() { 0x0004FFFF, 2, 0, 18 },
                           Package() { 0x0004FFFF, 3, 0, 19 },
                           // [RP1D]: PCI Express Port D on PC01
                           Package() { 0x0005FFFF, 0, 0, 17 },
                           Package() { 0x0005FFFF, 1, 0, 18 },
                           Package() { 0x0005FFFF, 2, 0, 19 },
                           Package() { 0x0005FFFF, 3, 0, 16 }})
      }
      #include "IioRootBridge.asi"

      // PCI Express Port A on PC01
      Device (RP1A) {

        Name(_ADR, 0x00020000)
        //
        // Power Resources for Wake – returns a list of dependent power resources for waking.
        //
        Method (_PRW, 0)
        {
          Return (Package (0x02) {0x09, 0x04})
        }
        //
        // _PRT Pci Routing Table – returns a list of PCI interrupt mappings.
        //
        Method (_PRT, 0)
        {
          Return (Package() {Package() { 0x0000FFFF, 0, 0, 18 },
                             Package() { 0x0000FFFF, 1, 0, 19 },
                             Package() { 0x0000FFFF, 2, 0, 16 },
                             Package() { 0x0000FFFF, 3, 0, 17 }})
        }
        If(LEqual(ATSC,1)) {
          Name(_DSD, Package () {
              ToUUID("E995B7C2-BD1F-45F5-A911-5BCD9269CCEB"),
                  Package () {
              }
          }) // END Name(_DSD)
        }
        #include "PcieHp.asi"
        #include "PcieNonHpDev.asi"
      }

      // PCI Express Port B on PC01
      Device (RP1B) {

        Name(_ADR, 0x00030000)
        //
        // Power Resources for Wake – returns a list of dependent power resources for waking.
        //
        Method (_PRW, 0)
        {
          Return (Package (0x02) {0x09, 0x04})
        }
        //
        // _PRT Pci Routing Table – returns a list of PCI interrupt mappings.
        //
        Method (_PRT, 0)
        {
          Return (Package() {Package() { 0x0000FFFF, 0, 0, 19 },
                             Package() { 0x0000FFFF, 1, 0, 16 },
                             Package() { 0x0000FFFF, 2, 0, 17 },
                             Package() { 0x0000FFFF, 3, 0, 18 }})
        }
        #include "PcieHp.asi"
        #include "PcieNonHpDev.asi"
      }

      // PCI Express Port C on PC01
      Device (RP1C) {

        Name(_ADR, 0x00040000)
        //
        // Power Resources for Wake – returns a list of dependent power resources for waking.
        //
        Method (_PRW, 0)
        {
          Return (Package (0x02) {0x09, 0x04})
        }
        //
        // _PRT Pci Routing Table – returns a list of PCI interrupt mappings.
        //
        Method (_PRT, 0)
        {
          Return (Package() {Package() { 0x0000FFFF, 0, 0, 16 },
                             Package() { 0x0000FFFF, 1, 0, 17 },
                             Package() { 0x0000FFFF, 2, 0, 18 },
                             Package() { 0x0000FFFF, 3, 0, 19 }})
        }
        If(LEqual(ATSC,1)) {
          Name(_DSD, Package () {
              ToUUID("E995B7C2-BD1F-45F5-A911-5BCD9269CCEB"),
                  Package () {
              }
          }) // END Name(_DSD)
        }
        #include "PcieHp.asi"
        #include "PcieNonHpDev.asi"
      }

      // PCI Express Port D on PC01
      Device (RP1D) {

        Name(_ADR, 0x00050000)
        //
        // Power Resources for Wake – returns a list of dependent power resources for waking.
        //
        Method (_PRW, 0)
        {
          Return (Package (0x02) {0x09, 0x04})
        }
        //
        // _PRT Pci Routing Table – returns a list of PCI interrupt mappings.
        //
        Method (_PRT, 0)
        {
          Return (Package() {Package() { 0x0000FFFF, 0, 0, 17 },
                             Package() { 0x0000FFFF, 1, 0, 18 },
                             Package() { 0x0000FFFF, 2, 0, 19 },
                             Package() { 0x0000FFFF, 3, 0, 16 }})
        }
        If(LEqual(ATSC,1)) {
          Name(_DSD, Package () {
              ToUUID("E995B7C2-BD1F-45F5-A911-5BCD9269CCEB"),
                  Package () {
              }
          }) // END Name(_DSD)
        }
        #include "PcieHp.asi"
        #include "PcieNonHpDev.asi"
      }
    }
    //
    // Socket 0 Root bridge (Stack 2)
    //
#undef SOCKET
#undef STACK
#define SOCKET 0
#define STACK  2
    Device (PC02) {
      Name(_HID, EISAID("PNP0A08"))
      Name(_CID, EISAID("PNP0A03"))
      Name(_UID, 0x02)

      //
      // _PRT Pci Routing Table – returns a list of PCI interrupt mappings.
      //
      Method (_PRT, 0)
      {
        Return (Package() {// [VP2A]: Virtual Port A on PC02
                           Package() { 0x0004FFFF, 0, 0, 16 },
                           Package() { 0x0004FFFF, 1, 0, 17 },
                           Package() { 0x0004FFFF, 2, 0, 18 },
                           Package() { 0x0004FFFF, 3, 0, 19 },
                           // [VP2B]: Virtual Port B on PC02
                           Package() { 0x0005FFFF, 0, 0, 17 },
                           Package() { 0x0005FFFF, 1, 0, 18 },
                           Package() { 0x0005FFFF, 2, 0, 19 },
                           Package() { 0x0005FFFF, 3, 0, 16 }})
      }
      #include "IioRootBridge.asi"

      Device (VP2A) {

        Name(_ADR, 0x00040000)
        //
        // Power Resources for Wake – returns a list of dependent power resources for waking.
        //
        Method (_PRW, 0)
        {
          Return (Package (0x02) {0x09, 0x04})
        }
        //
        // _PRT Pci Routing Table – returns a list of PCI interrupt mappings.
        //
        Method (_PRT, 0)
        {
          Return (Package() {Package() { 0x0000FFFF, 0, 0, 16 },
                             Package() { 0x0000FFFF, 1, 0, 17 },
                             Package() { 0x0000FFFF, 2, 0, 18 },
                             Package() { 0x0000FFFF, 3, 0, 19 }})
        }
        Device (CPM0) {
          Name(_ADR, 0x00000000)
        }
      }

      Device (VP2B) {

        Name(_ADR, 0x00050000)
        //
        // Power Resources for Wake – returns a list of dependent power resources for waking.
        //
        Method (_PRW, 0)
        {
          Return (Package (0x02) {0x09, 0x04})
        }
        //
        // _PRT Pci Routing Table – returns a list of PCI interrupt mappings.
        //
        Method (_PRT, 0)
        {
          Return (Package() {Package() { 0x0000FFFF, 0, 0, 17 },
                             Package() { 0x0000FFFF, 1, 0, 18 },
                             Package() { 0x0000FFFF, 2, 0, 19 },
                             Package() { 0x0000FFFF, 3, 0, 16 }})
        }
        Device (CPK0) {
          Name(_ADR, 0x00000000)
        }
      }
    }
    //
    // Socket 0 Root bridge (Stack 3)
    //
#undef SOCKET
#undef STACK
#define SOCKET 0
#define STACK  3
    Device (PC03) {
      Name(_HID, EISAID("PNP0A08"))
      Name(_CID, EISAID("PNP0A03"))
      Name(_UID, 0x03)

      //
      // _PRT Pci Routing Table – returns a list of PCI interrupt mappings.
      //
      Method (_PRT, 0)
      {
        Return (Package() {Package() { 0x0004FFFF, 0, 0, 16 },
                           Package() { 0x0004FFFF, 1, 0, 17 },
                           Package() { 0x0004FFFF, 2, 0, 18 },
                           Package() { 0x0004FFFF, 3, 0, 19 }})
      }
      #include "IioRootBridge.asi"

      // Virtual RootPort connected to the CPK
      Device (VP3A) {

        Name(_ADR, 0x00040000)
        //
        // Power Resources for Wake – returns a list of dependent power resources for waking.
        //
        Method (_PRW, 0)
        {
          Return (Package (0x02) {0x09, 0x04})
        }
        //
        // _PRT Pci Routing Table – returns a list of PCI interrupt mappings.
        //
        Method (_PRT, 0)
        {
          Return (Package() {Package() { 0x0000FFFF, 0, 0, 16 },
                             Package() { 0x0000FFFF, 1, 0, 17 },
                             Package() { 0x0000FFFF, 2, 0, 18 },
                             Package() { 0x0000FFFF, 3, 0, 19 }})
        }
        Device (CPK0) {
          Name(_ADR, 0x00000000)
        }
      }
    }
    //
    // Socket 0 Root bridge (Stack 4)
    //
#undef SOCKET
#undef STACK
#define SOCKET 0
#define STACK  4
    Device (PC04) {
      Name(_HID, EISAID("PNP0A08"))
      Name(_CID, EISAID("PNP0A03"))
      Name(_UID, 0x04)

      #include "IioRootBridge.asi"
    }
    //
    // Socket 0 Virtual Root bridge (Stack 5)
    //
#undef SOCKET
#undef STACK
#define SOCKET 0
#define STACK  5
    Device (PC05) {
      Name(_HID, EISAID("PNP0A08"))
      Name(_CID, EISAID("PNP0A03"))
      Name(_UID, 0x05)

      //
      // _PRT Pci Routing Table – returns a list of PCI interrupt mappings.
      //
      Method (_PRT, 0)
      {
        Return (Package() {// [RP5A]: PCI Express Port A on PC05
                           Package() { 0x0002FFFF, 0, 0, 18 },
                           Package() { 0x0002FFFF, 1, 0, 19 },
                           Package() { 0x0002FFFF, 2, 0, 16 },
                           Package() { 0x0002FFFF, 3, 0, 17 },
                           // [RP5B]: PCI Express Port B on PC05
                           Package() { 0x0003FFFF, 0, 0, 19 },
                           Package() { 0x0003FFFF, 1, 0, 16 },
                           Package() { 0x0003FFFF, 2, 0, 17 },
                           Package() { 0x0003FFFF, 3, 0, 18 },
                           // [RP5C]: PCI Express Port C on PC05
                           Package() { 0x0004FFFF, 0, 0, 16 },
                           Package() { 0x0004FFFF, 1, 0, 17 },
                           Package() { 0x0004FFFF, 2, 0, 18 },
                           Package() { 0x0004FFFF, 3, 0, 19 },
                           // [RP5D]: PCI Express Port D on PC05
                           Package() { 0x0005FFFF, 0, 0, 17 },
                           Package() { 0x0005FFFF, 1, 0, 18 },
                           Package() { 0x0005FFFF, 2, 0, 19 },
                           Package() { 0x0005FFFF, 3, 0, 16 }})
      }
      #include "IioRootBridge.asi"
      //
      // PCI Express Port A on PC05
      //
      Device (RP5A) {

        Name(_ADR, 0x00020000)
        //
        // Power Resources for Wake – returns a list of dependent power resources for waking.
        //
        Method (_PRW, 0)
        {
          Return (Package (0x02) {0x09, 0x04})
        }
        //
        // _PRT Pci Routing Table – returns a list of PCI interrupt mappings.
        //
        Method (_PRT, 0)
        {
          Return (Package() {Package() { 0x0000FFFF, 0, 0, 18 },
                             Package() { 0x0000FFFF, 1, 0, 19 },
                             Package() { 0x0000FFFF, 2, 0, 16 },
                             Package() { 0x0000FFFF, 3, 0, 17 }})
        }
        If(LEqual(ATSC,1)) {
          Name(_DSD, Package () {
              ToUUID("E995B7C2-BD1F-45F5-A911-5BCD9269CCEB"),
                  Package () {
              }
          }) // END Name(_DSD)
        }
        #include "PcieHp.asi"
        #include "PcieNonHpDev.asi"
      }
      //
      // PCI Express Port B on PC05
      //
      Device (RP5B) {

        Name(_ADR, 0x00030000)
        //
        // Power Resources for Wake – returns a list of dependent power resources for waking.
        //
        Method (_PRW, 0)
        {
          Return (Package (0x02) {0x09, 0x04})
        }
        //
        // _PRT Pci Routing Table – returns a list of PCI interrupt mappings.
        //
        Method (_PRT, 0)
        {
          Return (Package() {Package() { 0x0000FFFF, 0, 0, 19 },
                             Package() { 0x0000FFFF, 1, 0, 16 },
                             Package() { 0x0000FFFF, 2, 0, 17 },
                             Package() { 0x0000FFFF, 3, 0, 18 }})
        }
        If(LEqual(ATSC,1)) {
          Name(_DSD, Package () {
              ToUUID("E995B7C2-BD1F-45F5-A911-5BCD9269CCEB"),
                  Package () {
              }
          }) // END Name(_DSD)
        }
        #include "PcieHp.asi"
        #include "PcieNonHpDev.asi"
      }
      //
      // PCI Express Port C on PC05
      //
      Device (RP5C) {

        Name(_ADR, 0x00040000)
        //
        // Power Resources for Wake – returns a list of dependent power resources for waking.
        //
        Method (_PRW, 0)
        {
          Return (Package (0x02) {0x09, 0x04})
        }
        //
        // _PRT Pci Routing Table – returns a list of PCI interrupt mappings.
        //
        Method (_PRT, 0)
        {
          Return (Package() {Package() { 0x0000FFFF, 0, 0, 16 },
                             Package() { 0x0000FFFF, 1, 0, 17 },
                             Package() { 0x0000FFFF, 2, 0, 18 },
                             Package() { 0x0000FFFF, 3, 0, 19 }})
        }
        If(LEqual(ATSC,1)) {
          Name(_DSD, Package () {
              ToUUID("E995B7C2-BD1F-45F5-A911-5BCD9269CCEB"),
                  Package () {
              }
          }) // END Name(_DSD)
        }
        #include "PcieHp.asi"
        #include "PcieNonHpDev.asi"
      }
      //
      // PCI Express Port D on PC05
      //
      Device (RP5D) {

        Name(_ADR, 0x00050000)
        //
        // Power Resources for Wake – returns a list of dependent power resources for waking.
        //
        Method (_PRW, 0)
        {
          Return (Package (0x02) {0x09, 0x04})
        }
        //
        // _PRT Pci Routing Table – returns a list of PCI interrupt mappings.
        //
        Method (_PRT, 0)
        {
          Return (Package() {Package() { 0x0000FFFF, 0, 0, 17 },
                             Package() { 0x0000FFFF, 1, 0, 18 },
                             Package() { 0x0000FFFF, 2, 0, 19 },
                             Package() { 0x0000FFFF, 3, 0, 16 }})
        }
        If(LEqual(ATSC,1)) {
          Name(_DSD, Package () {
              ToUUID("E995B7C2-BD1F-45F5-A911-5BCD9269CCEB"),
                  Package () {
              }
          }) // END Name(_DSD)
        }
        #include "PcieHp.asi"
        #include "PcieNonHpDev.asi"
      }
    }
    //
    // Socket 0 Uncore 0
    //
#undef SOCKET
#undef STACK
#define SOCKET 0
#define STACK  6
    Device (CONCATENATE3(UC, SOCKET, STACK)) {
      Name(_HID, EISAID("PNP0A03"))
      Name(_UID, 0x32)

      //
      // _PRT Pci Routing Table – returns a list of PCI interrupt mappings.
      //
      Method (_PRT, 0)
      {
        Return (AR10)
      }

      #include "Uncore.asi"

      // Uncore 0 UBOX Device
      Device (UBX0) {
          Name(_ADR, 0x00000000)
      }

      // Uncore 0 CHASIS_SMBUS Devices
      Device (CSM0) {
          Name(_ADR, 0x000B0000)
      }

      // Uncore 0 MS2MEM_SCF_MS2MEM0 Device
      Device (M2M0) {
          Name(_ADR, 0x000C0000)
      }

      // Uncore 0 MS2MEM_SCF_MS2MEM1 Device
      Device (M2M1) {
          Name(_ADR, 0x000D0000)
      }

      // Uncore 0 MCDDR0 Device
      Device (MCD0) {
          Name(_ADR, 0x001A0000)
      }

      // Uncore 0 MCDDR1 Device
      Device (MCD1) {
          Name(_ADR, 0x001B0000)
      }
    }

    // Socket 0 Uncore 1
    //
#undef SOCKET
#undef STACK
#define SOCKET 0
#define STACK  7
    Device (CONCATENATE3(UC, SOCKET, STACK)) {
      Name(_HID, EISAID("PNP0A03"))
      Name(_UID, 0x33)

      //
      // _PRT Pci Routing Table – returns a list of PCI interrupt mappings.
      //
      Method (_PRT, 0)
      {
        Return (AR11)
      }

      #include "Uncore.asi"

      // Uncore 1 GRP1_CHA8-10 Device
      Device (CHA1) {
          Name(_ADR, 0x00010000)
      }

      // Uncore 1 GRP0_CHA0-7 Device
      Device (CHA2) {
          Name(_ADR, 0x000A0000)
      }

      // Uncore 1 GRP0_CHA8-10 Device
      Device (CHA3) {
          Name(_ADR, 0x00B00000)
      }

      // Uncore 1 CHAALL0-1 Device
      Device (CA00) {
          Name(_ADR, 0x001D0000)
      }

      // Uncore 1 CHASIS_PUINT0-7 Device
      Device (PUC0) {
          Name(_ADR, 0x001E0000)
      }

      // Uncore 1 Gen3Phy Device
      Device (GN30) {
          Name(_ADR, 0x001F0000)
      }
    }
}

Scope (\_GPE) {
  Method (_L09, 0x0, NotSerialized) {
    #include "Gpe.asi"
    Notify (\_SB.PC01.RP1A, 0x02)  // PCI Root Port A on PC01
    Notify (\_SB.PC01.RP1B, 0x02)  // PCI Root Port B on PC01
    Notify (\_SB.PC01.RP1C, 0x02)  // PCI Root Port C on PC01
    Notify (\_SB.PC01.RP1D, 0x02)  // PCI Root Port D on PC01
    Notify (\_SB.PC02.VP2A, 0x02)  // Virtual Root Port A on PC02
    Notify (\_SB.PC02.VP2B, 0x02)  // Virtual Root Port B on PC02
    Notify (\_SB.PC03.VP3A, 0x02)  // Virtual Root Port A on PC03
    Notify (\_SB.PC05.RP5A, 0x02)  // PCI Root Port A on PC05
    Notify (\_SB.PC05.RP5B, 0x02)  // PCI Root Port B on PC05
    Notify (\_SB.PC05.RP5C, 0x02)  // PCI Root Port C on PC05
    Notify (\_SB.PC05.RP5D, 0x02)  // PCI Root Port D on PC05
  }
}

