/** @file

  Copyright (c) 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

//
// Include in the remapped device scope (NVMx)
//

#define PCI_CARD_BASE_ADDR0     0x10
#define PCI_CARD_BASE_ADDR1     0x14
#define PCI_CARD_BASE_ADDR2     0x18
#define PCI_CARD_BASE_ADDR3     0x1C
#define PCI_CARD_BASE_ADDR4     0x20
#define PCI_CARD_BASE_ADDR5     0x24

#define VDID_OFFSET 0x0 // 32bit value
#define L23E_OFFSET 0xE2 // bit 2 - L23_Rdy Entry Request (L23ER)
#define L23R_OFFSET 0xE2 // bit 3 - L23_Rdy to Detect Transition (L23R2DT)
#define LEDM_OFFSET 0x324 // bit 3 - DMI L1 Entry Disable Mask (DMIL1EDM)
#define LDIS_OFFSET 0x50 //bit 4 - Link Disable (LD)
#define DPGE_OFFSET 0x420 // bit 30 - Disabled, Detect, L23_Rdy State and Un-Configured PHY Lane Power Gating Enable (DLSULPPGE)
#define LASX_OFFSET 0x52 // bit 13 - Link Active (LA)

#define ROOTPORT_READ           0
#define ROOTPORT_WRITE          1
#define ENDPOINT_READ           2
#define ENDPOINT_WRITE          3

//RST Pcie Storage Remapped Base Address Index Value
Name(PRBI, 0)

//RST Pcie Storage Remapped Base Address Data Value
Name(PRBD, 0)

//RST Pcie Storage Endpoint Command Data
Name(PCMD, 0)

 //RST Pcie Storage Cycle Router
Name(NCRN, 0)

//
// Variables list to store corresponding value for different NVM device
//
Name(NITV, 0)  // Interface Type
Name(NPMV, 0)  // Power Management Capability Pointer
Name(NPCV, 0)  // PCIe Capabilities Pointer
Name(NL1V, 0)  // L1SS Capability Pointer
Name(ND2V, 0)  // Endpoint L1SS Control Data2
Name(ND1V, 0)  // Endpoint L1SS Control Data1
Name(NLRV, 0)  // LTR Capability Pointer
Name(NLDV, 0)  // Endpoint LTR Data
Name(NEAV, 0)  // Endpoint LCTL Data
Name(NEBV, 0)  // Endpoint DCTL Data
Name(NECV, 0)  // Endpoint DCTL2 Data
Name(NRAV, 0)  // RootPort DCTL2 Data
Name(NMBV, 0)  // Endpoint unique MSI-X Table BAR
Name(NMVV, 0)  // Endpoint unique MSI-X Table BAR value
Name(NPBV, 0)  // Endpoint unique MSI-X PBA BAR
Name(NPVV, 0)  // Endpoint unique MSI-X PBA BAR value
Name(NRPN, 0) //  Assigned Root Port number

Name(MXIE, 0)

Name(ID3C, 0)  // Is device in D3 cold state
Name(ID3H, 0)  // Is device in D3 hot state
Name(CSAV, 0) // Remapping configuration saved

Method(_PS3, 0, Serialized) {

  //
  // If RAID support has been added check if RTD3 isn't blocked - IR3D method
  // If RTD3 is blocked return.
  //
  If (IR3D()) {
    Return()
  }
  NVD3()
}

Method(_PS0, 0, Serialized) {
  //
  // If RAID support has been added check if RTD3 isn't blocked - IR3D method
  // If RTD3 is blocked return.
  //
  If (IR3D()) {
    Return()
  }
  NVD0()
}

//
// Put remapped port into L23Ready state
//
Method(RL23) {
  // Set L23_Rdy Entry Request (L23ER)
  RDCA(NCRN,L23E_OFFSET,0xFFFFFFFF,0x4,ROOTPORT_WRITE) // Store(1, L23E)
  Sleep(16)
  Store(0, Local0)

  //
  // Wait for link to transition to L23R
  //
  While(And(RDCA(NCRN,L23E_OFFSET,0x0,0x0,ROOTPORT_READ),0x4))
  {
    If(Lgreater(Local0, 4))
    {
        Break
    }
    Sleep(16)
    Increment(Local0)
  }
  RDCA(NCRN,LEDM_OFFSET,0xFFFFFFFF,0x8,ROOTPORT_WRITE)  //Store(1, LEDM) // PCIEDBG.DMIL1EDM (324[3]) = 1

}

//
// Transition remapped port into link active state
//
Method(RLA) {
  // Clear DLSULPPGE, then set L23_Rdy to Detect Transition  (L23R2DT)
  RDCA(NCRN,DPGE_OFFSET,0xBFFFFFFF,0x0,ROOTPORT_WRITE)  //Store(0, DPGE)
  RDCA(NCRN,L23R_OFFSET,0xFFFFFFFF,0x8,ROOTPORT_WRITE)  //Store(1, L23R)
  Sleep(16)
  Store(0, Local0)

  //
  // Wait for PCIe link to exit L23
  //
  While(And(RDCA(NCRN,L23R_OFFSET,0x0,0x0,ROOTPORT_READ),0x8))  {
    If(Lgreater(Local0, 4))
    {
      Break
    }
    Sleep(16)
    Increment(Local0)
  }

  // Once in Detect, wait up to 124 ms for Link Active (typically happens in under 70ms)
  // Worst case per PCIe spec from Detect to Link Active is:
  // 24ms in Detect (12+12), 72ms in Polling (24+48), 28ms in Config (24+2+2+2+2)
  RDCA(NCRN,DPGE_OFFSET,0xFFFFFFFF,0x40000000,ROOTPORT_WRITE) //Store(1, DPGE)
  Store(0, Local0)
  While(LEqual(And(RDCA(NCRN,LASX_OFFSET,0x0,0x0,ROOTPORT_READ),0x2000),0)) {
    If(Lgreater(Local0, 8))
    {
      Break
    }
    Sleep(16)
    Increment(Local0)
  }

  RDCA(NCRN,LEDM_OFFSET,0xFFFFFFF7,0x0,ROOTPORT_WRITE) //Store(0, LEDM) // Set PCIEDBG.DMIL1EDM (324[3]) = 0

}

//
// Remapped Port ON
//
Method(RPON, 0, Serialized) {

  // If device was not set in D3 cold do not re-initiate.
  If(LEqual(ID3C ,0)) {
    Return()
  }

  // Turn on power to the remapped PCIe slot
  \_SB.PC00.SAT0.RSON(NRPN)

  // Transition into Link Active
  RLA()

  //
  // Sleep for 100ms after transition to link active
  //
  Sleep(100)
  Store(0, ID3C)
}

//
// Remapped Port OFF
//
Method(RPOF, 0, Serialized) {

  // Return if D3Hot transition wasn't performed.
  If(LEqual(ID3H, 0)) {
    BreakPoint
    Return()
  }

  // Check if root port supports D3Cold
  // Return if there is no support
  // to avoid L2/L3 ready transition
  If(LNot(D3CS(NRPN))) {
    Return()
  }

  // Transition into L23 Ready
  RL23()

  // Turn off power to the remapped PCIe slot
  \_SB.PC00.SAT0.RSOF(NRPN)

  Store(1, ID3C)
}

//
// Method used to save remapped storage configuration
//
Method(SAVC, 0, Serialized) {

  Store(RDCA(NCRN,0x4,0x0,0x0,ENDPOINT_READ),PCMD)
  If(LEqual(NITV,1)) // Store BAR5 if Endpoint is AHCI Interface
  {
    Store(0x24,PRBI)
    Store(RDCA(NCRN,0x24,0x0,0x0,ENDPOINT_READ),PRBD)
  }
  ElseIf(LEqual(NITV,2)) // Store BAR0 if Endpoint is NVMe Interface
  {
    Store(0x10,PRBI)
    Store(RDCA(NCRN,0x10,0x0,0x0,ENDPOINT_READ),PRBD)

    //Get MSI-X capability pointer (0x33C)
    OperationRegion(MCRC, SystemMemory, Add(\_SB.PC00.GPCB(),Add(0xB8000, R_SATA_CFG_RST_PCIE_STORAGE_MXCRC)), 0x4)
    Field(MCRC,AnyAcc, NoLock, Preserve)
    {
      SCSO, 8,
      , 8,
      TCSO, 8,
      , 7,
      RE, 1,
    }
    Store(RDCA(NCRN,TCSO,0x0,0x0,ENDPOINT_READ), MXIE)
  }
  Store(1, CSAV)
}

//
// Method to put remapped device NVMx to D3hot state.
//
Method(NVD3,0,Serialized) {
  //
  // Return if RST Pcie Storage Remapping is disabled
  //
  If(LEqual(NITV,0))
  {
    Return()
  }

  //
  // Skip if already in D3
  //
  If (LEqual(ID3H, 1)) {
    Return()
  }

  //
  // Store Endpoint CMD and remapped BAR for CNRS() restoration
  //
  SAVC()

  // Set endpoint device PM register to D3hot state.
  RDCA(NCRN,Add(NPMV,0x04),0xFFFFFFFC,0x3,ENDPOINT_WRITE)
  // Set root port PM register to D3hot state.
  RDCA(NCRN,0xA4,0xFFFFFFFC,0x3,ROOTPORT_WRITE)
  Store(1, ID3H)
  Return()
}

//
// Method to put remapped device NVMx to D0 state.
//
Method(NVD0,0,Serialized) {
  //
  // Return if RST Pcie Storage Remapping is disabled
  //
  If(LEqual(NITV,0))
  {
    Return()
  }

  //
  // If already in D0 skip
  //
  If (LEqual(ID3H, 0)) {
    Return()
  }

  // Set root port PM register to D0 state..
  RDCA(NCRN,0xA4,0xFFFFFFFC,0x0,ROOTPORT_WRITE)

  // Set endpoint device PM register to D0 state.
  RDCA(NCRN,Add(NPMV,0x04),0xFFFFFFFC,0x0,ENDPOINT_WRITE)

  //
  // Restore remapping configuration
  // Only restore configuration if CSAV has been set to 1
  //
  If (LEqual(CSAV, 1)) {
    //
    // Restore BAR and PCI config space configuration
    //
    CNRS()

    //
    // Restore MSIX configuration
    //
    If(LEqual(NITV,2)) // If Endpoint is NVMe Interface
    {
      // offset 0x33C
      OperationRegion(MCRC, SystemMemory, Add(\_SB.PC00.GPCB(), Add(0xB8000, R_SATA_CFG_RST_PCIE_STORAGE_MXCRC)), 0x4)
      Field(MCRC,AnyAcc, NoLock, Preserve)
      {
        SCSO, 8,
        , 8,
        TCSO, 8,
        , 7,
        RE, 1,
      }

      RDCA(NCRN,TCSO,0xFFFFFFFF,And(MXIE, 0x80000000),ENDPOINT_WRITE)
    }
  } Else {
  }
  Store(0, ID3H)
  Return()
}

//
// Restore of Remapped Device and Hidden Root Port
// This method is called after the endpoint is to be power ungated (D3-cold to D0 unitialized)
// This method must be called after SAVC saved the remapping configuration.
//
Method(CNRS, 0, Serialized) {
  //
  // Return if RST Pcie Storage Remapping is disabled
  //
  If(LEqual(NITV,0))
  {
     Return()
  }

  //
  // Clear all BARs in Remapped Device
  //
  RDCA(NCRN,PCI_CARD_BASE_ADDR0,0x0,0x0,ENDPOINT_WRITE)
  RDCA(NCRN,PCI_CARD_BASE_ADDR1,0x0,0x0,ENDPOINT_WRITE)
  RDCA(NCRN,PCI_CARD_BASE_ADDR2,0x0,0x0,ENDPOINT_WRITE)
  RDCA(NCRN,PCI_CARD_BASE_ADDR3,0x0,0x0,ENDPOINT_WRITE)
  RDCA(NCRN,PCI_CARD_BASE_ADDR4,0x0,0x0,ENDPOINT_WRITE)
  RDCA(NCRN,PCI_CARD_BASE_ADDR5,0x0,0x0,ENDPOINT_WRITE)

  //
  // Restore remapped BAR and Endpoint CMD
  //
  RDCA(NCRN,PRBI,0x0,PRBD,ENDPOINT_WRITE)
  RDCA(NCRN,0x4,0xFFFFFFF8,PCMD,ENDPOINT_WRITE)

  //
  // Restore of Remapped Device L1 Substate if this Capability is supported
  //
  If(LNotEqual(NL1V,0))
  {
    RDCA(NCRN,Add(NL1V,0x0C),0xFFFFFF00,ND2V,ENDPOINT_WRITE)
    RDCA(NCRN,Add(NL1V,0x08),0x0000000F,And(ND1V,0xFFFFFFF0),ENDPOINT_WRITE)
    RDCA(NCRN,Add(NL1V,0x08),0xFFFFFFFF,ND1V,ENDPOINT_WRITE)
  }

  //
  // Restore of Remapped Device LTR if this Capability is supported
  //
  If(LNotEqual(NLRV,0))
  {
    RDCA(NCRN,Add(NLRV,0x04),0xFFFFFFFF,NLDV,ENDPOINT_WRITE)
  }

  //
  // Restore of Remapped Device Link Control's "Enable Clock Power Management" field and "Common Clock Configuration" field
  //
  RDCA(NCRN,Add(NPCV,0x10),0xFFFFFEBF,And(NEAV,0xFFFC),ENDPOINT_WRITE)

  //
  // Restore of Remapped Device Device Control 2 field
  //
  RDCA(NCRN,Add(NPCV,0x28),0xFFFFFBFF,NECV,ENDPOINT_WRITE)

  //
  // Restore of Remapped Device Device Control field
  //
  RDCA(NCRN,Add(NPCV,0x8),0xFFFFFF1F,NEBV,ENDPOINT_WRITE)

  //
  // Restore of Hidden Root Port field
  //
  RDCA(NCRN,0x68,0xFFFFFBFF,NRAV,ROOTPORT_WRITE)

  //
  // Check CCC bit
  // If this bit is 1, perform link retrain by setting the "Retrain Link" bit
  //
  If(LEqual(And(NEAV,0x40),0x40))
  {
    RDCA(NCRN,0x50,0xFFFFFFDF,0x20,ROOTPORT_WRITE)
    //
    // Poll PCIe Link Active status till it is active
    //
    while(LEqual(And(RDCA(NCRN,0x52,0x0,0x0,ROOTPORT_READ),0x2000),0))
    {
      Stall(10)
    }
  }

  //
  // Restore of Remapped Device Link Control's "Active State Link PM Control" field
  //
  RDCA(NCRN,Add(NPCV,0x10),0xFFFFFFFC,And(NEAV,0x0003),ENDPOINT_WRITE)

  //
  // Restore of Remapped Device related device BAR for the MSI-X Table BAR if the device supports unique MSI-X Table BAR
  //
  If(LNotEqual(NMVV,0))
  {
    RDCA(NCRN,NMBV,0x0,NMVV,ENDPOINT_WRITE)
  }

  //
  // Restore of Remapped Device related device BAR for the MSI-X PBA BAR if the device supports unique MSI-X PBA BAR
  //
  If(LNotEqual(NPVV,0))
  {
    RDCA(NCRN,NPBV,0x0,NPVV,ENDPOINT_WRITE)
  }

}

