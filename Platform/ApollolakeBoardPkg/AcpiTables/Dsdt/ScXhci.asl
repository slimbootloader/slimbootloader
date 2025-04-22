/**@file

  Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

//scope is \_SB.PCI0.XHC
Device(XHC) {
  Name(_ADR, 0x00150000)                     //Device 21, Function 0

  Name (_DDN, "Broxton XHCI controller (Host only)" )
  Name (_STR, Unicode ("Broxton XHCI controller (Host only)"))

  Method(_S0W, 0x0, NotSerialized)   // _S0W: S0 Device Wake State
  {
    Return(0x3)
  }
  Name (_PRW, Package() {0x0D, 4})  // Declare XHCI GPE status and enable bits are bit 13.

  OperationRegion (USBR, PCI_Config, 0x74, 0x04)
  Field (USBR, WordAcc, NoLock, Preserve)
  {
     Offset (0x01),
      PMEE,   1,
          ,   6,
      PMES,   1
  }

  Method (_DSW, 3, NotSerialized)  // _DSW: Device Sleep Wake
  {
     Store (Arg0, PMEE)
  }

  Method (_RMV, 0, NotSerialized)  // _RMV: Removal Status
  {
      Return (Zero)
  }
  //_PS3 is removed as requested (SV D3 verified)

  Method(_STA, 0)
  {
    Return (0xF)
  }

  Device(RHUB)
  {
    Name(_ADR, Zero)

    Method(TPLD, 2, Serialized) { // For the port related with USB Tyep C. copied and modifined from GPLD
    // Arg0:  Visiable
    // Arg1:  Group position
      Name(PCKG, Package() { Buffer(0x14) {} } )
      CreateField(DerefOf(Index(PCKG,0)), 0, 7, REV)
      Store(1,REV)
      CreateField(DerefOf(Index(PCKG,0)), 64, 1, VISI)
      Store(Arg0, VISI)
      CreateField(DerefOf(Index(PCKG,0)), 87, 8, GPOS)
      Store(Arg1, GPOS)
      CreateField(DerefOf(Index(PCKG,0)), 128, 32,VHOS)
      Store(0xFFFFFFFF, VHOS)

    // Following add for USB type C
      CreateField(DerefOf(Index(PCKG,0)), 74, 4, SHAP)  // Shape set to Oval
      Store(1, SHAP)
      CreateField(DerefOf(Index(PCKG,0)), 32, 16, WID)  // Width of the connector, 8.34mm
      Store(8, WID)
      CreateField(DerefOf(Index(PCKG,0)), 48, 16, HGT)  // Height of the connector, 2.56mm
      Store(3, HGT)
      return (PCKG)
    }
    Method(TUPC, 1, Serialized) { // For the port related with USB Tyep C. copied and modifined from GUPC
    // Arg0: Type
    // Type:
    //  0x08:     Type-C connector - USB2-only
    //  0x09:     Type-C connector - USB2 and SS with Switch
    //  0x0A:     Type-C connector - USB2 and SS without Switch
      Name(PCKG, Package(4) { 1, 0x00, 0, 0 } )
    Store(Arg0,Index(PCKG,1))
      return (PCKG)
    }

    //
    // High Speed Ports
    // It would be USB2.0 port if SS termination is disabled

    //    The UPC declarations for LS/FS/HS and SS ports that are paired to form a USB3.0 compatible connector.
    //    A "pair" is defined by two ports that declare _PLDs with identical Panel, Vertical Position, Horizontal Postion, Shape, Group Orientation
    //    and Group Token

    Device(HS01)                                                          // Pair with SS01: OTG(Dual Role) Port
    {
      Name(_ADR, 0x01)
      Method(_UPC,0,Serialized) {
          Return (TUPC(9))
      }

      Method(_PLD,0,Serialized) {
          Return (TPLD(1,1))
      }
    }//end of HS01

    Device(HS02)
    {
      Name(_ADR, 0x02)
      Method(_UPC,0,Serialized) {
          Name(UPCP, Package() { 0xFF,0x00,0x00,0x00 })
          Return(UPCP)
      }

      Method(_PLD,0,Serialized) {
          Name(PLDP, Package() {
              Buffer(0x14)
              {
              //31:0   - Bit[6:0]=2 revision is 0x2, Bit[7]=1 Ignore Color Bit[31:8]=0 RGB color is ignored
              0x82, 0x00, 0x00, 0x00,
              //63:32 - Bit[47:32]=0 width: 0x0000  Bit[63:48]=0 Height:0x0000
              0x00, 0x00, 0x00, 0x00,
              //95:64 - bit[66:64]=b'011 visiable/docking/no lid bit[69:67]=b'001 bottom panel bit[71:70]=b'01 Center  bit[73:72]=b'01 Center
              //           bit[77:74]=6 Horizontal Trapezoid bit[78]=0 bit[86:79]=0 bit[94:87]='0 no group info' bit[95]=0 not a bay
              //[71:64][79:72][87:80][95:88]
              0x41, 0x08, 0x02, 0x00,
              //127:96 -bit[96]=1 Ejectable bit[97]=1 OSPM Ejection required Bit[105:98]=0 no Cabinet Number
              //            bit[113:106]=0 no Card cage Number bit[114]=0 no reference shape Bit[118:115]=0 no rotation Bit[123:119]=0 no order
              0x00, 0x00, 0x00, 0x00,
              //159:128  Vert. and Horiz. Offsets not supplied
              0xFF, 0xFF, 0xFF, 0xFF
              }
              })

          Return (PLDP)
      }
    }//end of HS02

    Device(HS03)
    {
      Name(_ADR, 0x03)
      Method(_UPC,0,Serialized) {
          Name(UPCP, Package() { 0xFF,0xFF,0x00,0x00 })
          Return(UPCP)
      }

      Method(_PLD,0,Serialized) {
          Name(PLDP, Package() {
              Buffer(0x14)
              {
              //31:0   - Bit[6:0]=2 revision is 0x2, Bit[7]=1 Ignore Color Bit[31:8]=0 RGB color is ignored
              0x82, 0x00, 0x00, 0x00,
              //63:32 - Bit[47:32]=0 width: 0x0000  Bit[63:48]=0 Height:0x0000
              0x00, 0x00, 0x00, 0x00,
              //95:64 - bit[66:64]=b'011 visiable/docking/no lid bit[69:67]=b'001 bottom panel bit[71:70]=b'01 Center  bit[73:72]=b'01 Center
              //           bit[77:74]=6 Horizontal Trapezoid bit[78]=0 bit[86:79]=0 bit[94:87]='0 no group info' bit[95]=0 not a bay
              //[71:64][79:72][87:80][95:88]
              0x40, 0x08, 0x03, 0x00,
              //127:96 -bit[96]=1 Ejectable bit[97]=1 OSPM Ejection required Bit[105:98]=0 no Cabinet Number
              //            bit[113:106]=0 no Card cage Number bit[114]=0 no reference shape Bit[118:115]=0 no rotation Bit[123:119]=0 no order
              0x00, 0x00, 0x00, 0x00,
              //159:128  Vert. and Horiz. Offsets not supplied
              0xFF, 0xFF, 0xFF, 0xFF
              }
              })

          Return (PLDP)
      }
    }//end of HS03

    Device(HS04)
    {
      Name(_ADR, 0x04)
      Method(_UPC,0,Serialized) {
          Name(UPCP, Package() { 0xFF,0x00,0x00,0x00 })
          Return(UPCP)
      }

      Method(_PLD,0,Serialized) {
          Name(PLDP, Package() {
              Buffer(0x14)
              {
              //31:0   - Bit[6:0]=2 revision is 0x2, Bit[7]=1 Ignore Color Bit[31:8]=0 RGB color is ignored
              0x82, 0x00, 0x00, 0x00,
              //63:32 - Bit[47:32]=0 width: 0x0000  Bit[63:48]=0 Height:0x0000
              0x00, 0x00, 0x00, 0x00,
              //95:64 - bit[66:64]=b'011 visiable/docking/no lid bit[69:67]=b'001 bottom panel bit[71:70]=b'01 Center  bit[73:72]=b'01 Center
              //           bit[77:74]=6 Horizontal Trapezoid bit[78]=0 bit[86:79]=0 bit[94:87]='0 no group info' bit[95]=0 not a bay
              //[71:64][79:72][87:80][95:88]
              0x41, 0x08, 0x04, 0x00,
              //127:96 -bit[96]=1 Ejectable bit[97]=1 OSPM Ejection required Bit[105:98]=0 no Cabinet Number
              //            bit[113:106]=0 no Card cage Number bit[114]=0 no reference shape Bit[118:115]=0 no rotation Bit[123:119]=0 no order
              0x00, 0x00, 0x00, 0x00,
              //159:128  Vert. and Horiz. Offsets not supplied
              0xFF, 0xFF, 0xFF, 0xFF
              }
              })

          Return (PLDP)
      }
    }//end of HS04

    Device(HS05)
    {
      Name(_ADR, 0x05)
      Method(_UPC,0,Serialized) {
          Name(UPCP, Package() { 0xFF,0x00,0x00,0x00 })
          Return(UPCP)
      }

      Method(_PLD,0,Serialized) {
          Name(PLDP, Package() {
              Buffer(0x14)
              {
              //31:0   - Bit[6:0]=2 revision is 0x2, Bit[7]=1 Ignore Color Bit[31:8]=0 RGB color is ignored
              0x82, 0x00, 0x00, 0x00,
              //63:32 - Bit[47:32]=0 width: 0x0000  Bit[63:48]=0 Height:0x0000
              0x00, 0x00, 0x00, 0x00,
              //95:64 - bit[66:64]=b'011 visiable/docking/no lid bit[69:67]=b'001 bottom panel bit[71:70]=b'01 Center  bit[73:72]=b'01 Center
              //           bit[77:74]=6 Horizontal Trapezoid bit[78]=0 bit[86:79]=0 bit[94:87]='0 no group info' bit[95]=0 not a bay
              //[71:64][79:72][87:80][95:88]
              0x41, 0x08, 0x05, 0x00,
              //127:96 -bit[96]=1 Ejectable bit[97]=1 OSPM Ejection required Bit[105:98]=0 no Cabinet Number
              //            bit[113:106]=0 no Card cage Number bit[114]=0 no reference shape Bit[118:115]=0 no rotation Bit[123:119]=0 no order
              0x00, 0x00, 0x00, 0x00,
              //159:128  Vert. and Horiz. Offsets not supplied
              0xFF, 0xFF, 0xFF, 0xFF
              }
              })

          Return (PLDP)
      }
    }//end of HS05

    Device(HS06)
    {
      Name(_ADR, 0x06)
      Method(_UPC,0,Serialized) {
          // No connect
          Name(UPCP, Package() { 0x00,0xFF,0x00,0x00 })
          Return(UPCP)
      }

      Method(_PLD,0,Serialized) {
          Name(PLDP, Package() {
              Buffer(0x14)
              {
              //31:0   - Bit[6:0]=2 revision is 0x2, Bit[7]=1 Ignore Color Bit[31:8]=0 RGB color is ignored
              0x82, 0x00, 0x00, 0x00,
              //63:32 - Bit[47:32]=0 width: 0x0000  Bit[63:48]=0 Height:0x0000
              0x00, 0x00, 0x00, 0x00,
              //95:64 - bit[66:64]=b'011 visiable/docking/no lid bit[69:67]=b'001 bottom panel bit[71:70]=b'01 Center  bit[73:72]=b'01 Center
              //           bit[77:74]=6 Horizontal Trapezoid bit[78]=0 bit[86:79]=0 bit[94:87]='0 no group info' bit[95]=0 not a bay
              //[71:64][79:72][87:80][95:88]
              0x40, 0x08, 0x06, 0x00,
              //127:96 -bit[96]=1 Ejectable bit[97]=1 OSPM Ejection required Bit[105:98]=0 no Cabinet Number
              //            bit[113:106]=0 no Card cage Number bit[114]=0 no reference shape Bit[118:115]=0 no rotation Bit[123:119]=0 no order
              0x00, 0x00, 0x00, 0x00,
              //159:128  Vert. and Horiz. Offsets not supplied
              0xFF, 0xFF, 0xFF, 0xFF
              }
              })

          Return (PLDP)
      }
    }//end of HS06

    Device(HS07)
    {
      Name(_ADR, 0x07)
      Method(_UPC,0,Serialized) {
          // No connect
          Name(UPCP, Package() { 0xFF,0xFF,0x00,0x00 })
          Return(UPCP)
      }

      Method(_PLD,0,Serialized) {
          Name(PLDP, Package() {
              Buffer(0x14)
              {
              //31:0   - Bit[6:0]=2 revision is 0x2, Bit[7]=1 Ignore Color Bit[31:8]=0 RGB color is ignored
              0x82, 0x00, 0x00, 0x00,
              //63:32 - Bit[47:32]=0 width: 0x0000  Bit[63:48]=0 Height:0x0000
              0x00, 0x00, 0x00, 0x00,
              //95:64 - bit[66:64]=b'011 visiable/docking/no lid bit[69:67]=b'001 bottom panel bit[71:70]=b'01 Center  bit[73:72]=b'01 Center
              //           bit[77:74]=6 Horizontal Trapezoid bit[78]=0 bit[86:79]=0 bit[94:87]='0 no group info' bit[95]=0 not a bay
              //[71:64][79:72][87:80][95:88]
              0x40, 0x08, 0x07, 0x00,
              //127:96 -bit[96]=1 Ejectable bit[97]=1 OSPM Ejection required Bit[105:98]=0 no Cabinet Number
              //            bit[113:106]=0 no Card cage Number bit[114]=0 no reference shape Bit[118:115]=0 no rotation Bit[123:119]=0 no order
              0x00, 0x00, 0x00, 0x00,
              //159:128  Vert. and Horiz. Offsets not supplied
              0xFF, 0xFF, 0xFF, 0xFF
              }
              })

          Return (PLDP)
      }
    }//end of HS07

    Device(HS08)
    {
      Name(_ADR, 0x08)
      Method(_UPC,0,Serialized) {
          // No connect
          Name(UPCP, Package() { 0xFF,0xFF,0x00,0x00 })
          Return(UPCP)
      }

      Method(_PLD,0,Serialized) {
          Name(PLDP, Package() {
              Buffer(0x14)
              {
              //31:0   - Bit[6:0]=2 revision is 0x2, Bit[7]=1 Ignore Color Bit[31:8]=0 RGB color is ignored
              0x82, 0x00, 0x00, 0x00,
              //63:32 - Bit[47:32]=0 width: 0x0000  Bit[63:48]=0 Height:0x0000
              0x00, 0x00, 0x00, 0x00,
              //95:64 - bit[66:64]=b'011 visiable/docking/no lid bit[69:67]=b'001 bottom panel bit[71:70]=b'01 Center  bit[73:72]=b'01 Center
              //           bit[77:74]=6 Horizontal Trapezoid bit[78]=0 bit[86:79]=0 bit[94:87]='0 no group info' bit[95]=0 not a bay
              //[71:64][79:72][87:80][95:88]
              0x40, 0x08, 0x08, 0x00,
              //127:96 -bit[96]=1 Ejectable bit[97]=1 OSPM Ejection required Bit[105:98]=0 no Cabinet Number
              //            bit[113:106]=0 no Card cage Number bit[114]=0 no reference shape Bit[118:115]=0 no rotation Bit[123:119]=0 no order
              0x00, 0x00, 0x00, 0x00,
              //159:128  Vert. and Horiz. Offsets not supplied
              0xFF, 0xFF, 0xFF, 0xFF
              }
              })

          Return (PLDP)
      }

      // Add Camera built in Device
      Device (FCAM)
      {
        Name(_ADR, 0x08)

         Method(_UPC,0,Serialized) {
           Name(UPCP, Package() { 0xFF,0xFF,0x00,0x00 }) // Connectable, Proprietary connector
           Return(UPCP)
         }

         Method(_PLD,0,Serialized) {
           Name(PLDP, Package() {
           Buffer(0x14)
           {
             //31:0   - Bit[6:0]=2 revision is 0x2, Bit[7]=1 Ignore Color Bit[31:8]=0 RGB color is ignored
             0x82, 0x00, 0x00, 0x00,
             //63:32 - Bit[47:32]=0 width: 0x0000  Bit[63:48]=0 Height:0x0000
             0x00, 0x00, 0x00, 0x00,
             //95:64 - bit[66:64]=b'000 in-visiable/ no docking/no lid bit[69:67]=b'100 front panel bit[71:70]=b'01 Center  bit[73:72]=b'01 Center
             //           bit[77:74]=6 Horizontal Trapezoid bit[78]=0 bit[86:79]=0 bit[94:87]='0 no group info' bit[95]=0 not a bay
             //[71:64][79:72][87:80][95:88]
             0x60, 0x08, 0x08, 0x00,
             //127:96 -bit[96]=1 Ejectable bit[97]=1 OSPM Ejection required Bit[105:98]=0 no Cabinet Number
             //            bit[113:106]=0 no Card cage Number bit[114]=0 no reference shape Bit[118:115]=0 no rotation Bit[123:119]=0 no order
             0x00, 0x00, 0x00, 0x00,
             //159:128  Vert. and Horiz. Offsets not supplied
             0xFF, 0xFF, 0xFF, 0xFF
           }
           })

          Return (PLDP)
        }
      }
    }//end of HS08

    //
    // Super Speed Ports - must match _UPC declarations of the corresponding Full Speed Ports.
    //
    Device(SSP1)                                                         //Pair with HS01: OTG(Dual Role) Port
    {
        Name(_ADR, 0x09)
        Method(_UPC,0,Serialized) {
            Return (TUPC(9))
        }

        Method(_PLD,0,Serialized) {
            Return (TPLD(1,1))
        }
    }//end of SSP1

    Device(SSP2)
    {
        Name(_ADR, 0x0A)
        Method(_UPC,0,Serialized) {
            Name(UPCP, Package() {
               0xFF,             //port is connectable if non-zero
               0x03,             //USB3 Type A connector
               0x00,
               0x00 })

            Return(UPCP)
        }

        Method(_PLD,0,Serialized) {
            Name(PLDP, Package() {     //pls check ACPI 5.0 section 6.1.8
                Buffer(0x14)
                {
                //31:0   - Bit[6:0]=2 revision is 0x2, Bit[7]=1 Ignore Color Bit[31:8]=0 RGB color is ignored
                0x82, 0x00, 0x00, 0x00,
                //63:32 - Bit[47:32]=0 width: 0x0000  Bit[63:48]=0 Height:0x0000
                0x00, 0x00, 0x00, 0x00,
                //95:64 - bit[66:64]=b'011 visiable/docking/no lid bit[69:67]=b'001 bottom panel bit[71:70]=b'01 Center  bit[73:72]=b'01 Center
                //           bit[77:74]=6 Horizontal Trapezoid bit[78]=0 bit[86:79]=0 bit[94:87]='0 no group info' bit[95]=0 not a bay
                //[71:64][79:72][87:80][95:88]
                0x41, 0x08, 0x02, 0x00,
                //127:96 -bit[96]=1 Ejectable bit[97]=1 OSPM Ejection required Bit[105:98]=0 no Cabinet Number
                //            bit[113:106]=0 no Card cage Number bit[114]=0 no reference shape Bit[118:115]=0 no rotation Bit[123:119]=0 no order
                0x00, 0x00, 0x00, 0x00,
                //159:128  Vert. and Horiz. Offsets not supplied
                0xFF, 0xFF, 0xFF, 0xFF
                }
                })

            Return (PLDP)
        }
    }//end of SSP2

    Device(SSP3)
    {
        Name(_ADR, 0x0B)
        Method(_UPC,0,Serialized) {
            Name(UPCP, Package() {
               0xFF,             //port is connectable if non-zero
               0xFF,
               0x00,
               0x00 })

            Return(UPCP)
        }

        Method(_PLD,0,Serialized) {
            Name(PLDP, Package() {     //pls check ACPI 5.0 section 6.1.8
                Buffer(0x14)
                {
                //31:0   - Bit[6:0]=2 revision is 0x2, Bit[7]=1 Ignore Color Bit[31:8]=0 RGB color is ignored
                0x82, 0x00, 0x00, 0x00,
                //63:32 - Bit[47:32]=0 width: 0x0000  Bit[63:48]=0 Height:0x0000
                0x00, 0x00, 0x00, 0x00,
                //95:64 - bit[66:64]=b'011 visiable/docking/no lid bit[69:67]=b'001 bottom panel bit[71:70]=b'01 Center  bit[73:72]=b'01 Center
                //           bit[77:74]=6 Horizontal Trapezoid bit[78]=0 bit[86:79]=0 bit[94:87]='0 no group info' bit[95]=0 not a bay
                //[71:64][79:72][87:80][95:88]
                0x40, 0x08, 0x03, 0x00,
                //127:96 -bit[96]=1 Ejectable bit[97]=1 OSPM Ejection required Bit[105:98]=0 no Cabinet Number
                //            bit[113:106]=0 no Card cage Number bit[114]=0 no reference shape Bit[118:115]=0 no rotation Bit[123:119]=0 no order
                0x00, 0x00, 0x00, 0x00,
                //159:128  Vert. and Horiz. Offsets not supplied
                0xFF, 0xFF, 0xFF, 0xFF
                }
                })

            Return (PLDP)
        }
    }//end of SSP3

    Device(SSP4)
    {
        Name(_ADR, 0x0C)
        Method(_UPC,0,Serialized) {
            Name(UPCP, Package() {
               0x00,             //port is not-connectable
               0xFF,             //proprietary connector going to PCIe port
               0x00,
               0x00 })

            Return(UPCP)
        }

        Method(_PLD,0,Serialized) {
            Name(PLDP, Package() {     //pls check ACPI 5.0 section 6.1.8
                Buffer(0x14)
                {
                //31:0   - Bit[6:0]=2 revision is 0x2, Bit[7]=1 Ignore Color Bit[31:8]=0 RGB color is ignored
                0x82, 0x00, 0x00, 0x00,
                //63:32 - Bit[47:32]=0 width: 0x0000  Bit[63:48]=0 Height:0x0000
                0x00, 0x00, 0x00, 0x00,
                //95:64 - bit[66:64]=b'011 visiable/docking/no lid bit[69:67]=b'001 bottom panel bit[71:70]=b'01 Center  bit[73:72]=b'01 Center
                //           bit[77:74]=6 Horizontal Trapezoid bit[78]=0 bit[86:79]=0 bit[94:87]='0 no group info' bit[95]=0 not a bay
                //[71:64][79:72][87:80][95:88]
                0x40, 0x08, 0x04, 0x00,
                //127:96 -bit[96]=1 Ejectable bit[97]=1 OSPM Ejection required Bit[105:98]=0 no Cabinet Number
                //            bit[113:106]=0 no Card cage Number bit[114]=0 no reference shape Bit[118:115]=0 no rotation Bit[123:119]=0 no order
                0x00, 0x00, 0x00, 0x00,
                //159:128  Vert. and Horiz. Offsets not supplied
                0xFF, 0xFF, 0xFF, 0xFF
                }
                })

            Return (PLDP)
        }
    }//end of SSP4

    Device(SSP5)
    {
        Name(_ADR, 0x0D)
        Method(_UPC,0,Serialized) {
            Name(UPCP, Package() {
               0xFF,             //port is connectable if non-zero
               0x03,             //USB3 Type A connector
               0x00,
               0x00 })

            Return(UPCP)
        }

        Method(_PLD,0,Serialized) {
            Name(PLDP, Package() {     //pls check ACPI 5.0 section 6.1.8
                Buffer(0x14)
                {
                //31:0   - Bit[6:0]=2 revision is 0x2, Bit[7]=1 Ignore Color Bit[31:8]=0 RGB color is ignored
                0x82, 0x00, 0x00, 0x00,
                //63:32 - Bit[47:32]=0 width: 0x0000  Bit[63:48]=0 Height:0x0000
                0x00, 0x00, 0x00, 0x00,
                //95:64 - bit[66:64]=b'011 visiable/docking/no lid bit[69:67]=b'001 bottom panel bit[71:70]=b'01 Center  bit[73:72]=b'01 Center
                //           bit[77:74]=6 Horizontal Trapezoid bit[78]=0 bit[86:79]=0 bit[94:87]='0 no group info' bit[95]=0 not a bay
                //[71:64][79:72][87:80][95:88]
                0x41, 0x08, 0x05, 0x00,
                //127:96 -bit[96]=1 Ejectable bit[97]=1 OSPM Ejection required Bit[105:98]=0 no Cabinet Number
                //            bit[113:106]=0 no Card cage Number bit[114]=0 no reference shape Bit[118:115]=0 no rotation Bit[123:119]=0 no order
                0x00, 0x00, 0x00, 0x00,
                //159:128  Vert. and Horiz. Offsets not supplied
                0xFF, 0xFF, 0xFF, 0xFF
                }
                })

            Return (PLDP)
        }
    }//end of SSP5

    Device(SSP6)
    {
        Name(_ADR, 0x0E)
        Method(_UPC,0,Serialized) {
            Name(UPCP, Package() {
               0x00,             //port is connectable if non-zero
               0xFF,
               0x00,
               0x00 })

            Return(UPCP)
        }

        Method(_PLD,0,Serialized) {
            Name(PLDP, Package() {     //pls check ACPI 5.0 section 6.1.8
                Buffer(0x14)
                {
                //31:0   - Bit[6:0]=2 revision is 0x2, Bit[7]=1 Ignore Color Bit[31:8]=0 RGB color is ignored
                0x82, 0x00, 0x00, 0x00,
                //63:32 - Bit[47:32]=0 width: 0x0000  Bit[63:48]=0 Height:0x0000
                0x00, 0x00, 0x00, 0x00,
                //95:64 - bit[66:64]=b'011 visiable/docking/no lid bit[69:67]=b'001 bottom panel bit[71:70]=b'01 Center  bit[73:72]=b'01 Center
                //           bit[77:74]=6 Horizontal Trapezoid bit[78]=0 bit[86:79]=0 bit[94:87]='0 no group info' bit[95]=0 not a bay
                //[71:64][79:72][87:80][95:88]
                0x40, 0x08, 0x06, 0x00,
                //127:96 -bit[96]=1 Ejectable bit[97]=1 OSPM Ejection required Bit[105:98]=0 no Cabinet Number
                //            bit[113:106]=0 no Card cage Number bit[114]=0 no reference shape Bit[118:115]=0 no rotation Bit[123:119]=0 no order
                0x00, 0x00, 0x00, 0x00,
                //159:128  Vert. and Horiz. Offsets not supplied
                0xFF, 0xFF, 0xFF, 0xFF
                }
                })

            Return (PLDP)
        }
    }//end of SSP6

    //
    // SSIC PORT
    //
    Device(SSP7)
    {
        Name(_ADR, 0x0F)
        Method(_UPC,0,Serialized) {
            Name(UPCP, Package() {
               0x00,             //port is connectable if non-zero
               0xFF,             //proprietary connector
               0x00,
               0x00 })

            Return(UPCP)
        }

        Method(_PLD,0,Serialized) {
            Name(PLDP, Package() {     //pls check ACPI 5.0 section 6.1.8
                Buffer(0x14)
                {
                //31:0   - Bit[6:0]=2 revision is 0x2, Bit[7]=1 Ignore Color Bit[31:8]=0 RGB color is ignored
                0x82, 0x00, 0x00, 0x00,
                //63:32 - Bit[47:32]=0 width: 0x0000  Bit[63:48]=0 Height:0x0000
                0x00, 0x00, 0x00, 0x00,
                //95:64 - bit[66:64]=b'011 visiable/docking/no lid bit[69:67]=b'001 bottom panel bit[71:70]=b'01 Center  bit[73:72]=b'01 Center
                //           bit[77:74]=6 Horizontal Trapezoid bit[78]=0 bit[86:79]=0 bit[94:87]='0 no group info' bit[95]=0 not a bay
                //[71:64][79:72][87:80][95:88]
                0x40, 0x08, 0x07, 0x00,
                //127:96 -bit[96]=1 Ejectable bit[97]=1 OSPM Ejection required Bit[105:98]=0 no Cabinet Number
                //            bit[113:106]=0 no Card cage Number bit[114]=0 no reference shape Bit[118:115]=0 no rotation Bit[123:119]=0 no order
                0x00, 0x00, 0x00, 0x00,
                //159:128  Vert. and Horiz. Offsets not supplied
                0xFF, 0xFF, 0xFF, 0xFF
                }
                })

            Return (PLDP)
        }
    }//end of SSP7
  }  //end of root hub
} // end of XHC1
