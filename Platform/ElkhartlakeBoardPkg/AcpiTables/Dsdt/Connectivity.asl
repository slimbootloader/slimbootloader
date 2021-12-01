/** @file
  Intel ACPI Sample Code for connectivity modules

  Copyright (c) 2016 - 2021, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

Include ("AcpiResourcesOffsets.h")
Scope(\_SB)
{
  //
  // Set M.2 BT RF-Kill(W_DISABLE2#) pin
  //
  Method (BTRK, 0x1, Serialized)
  {
    //
    // Arg0 - Value to W_DISABLE2#
    //

    \_SB.SGOV (GBTK, Arg0)
  }

  //
  // Get value of M.2 BT RF-Kill(W_DISABLE2#) pin
  //
  Method (GBTR, 0)
  {
    Return (\_SB.GGOV (GBTK))
  }
}

//
// Report thermal & regulatory methods if CNVi WiFi is present
//
If (\_SB.PC00.CNIP ()) {
  Scope (\_SB.PC00.CNVW) {
    Include("WifiThermal.asl")
    Include("WifiRegulatory.asl")
    Include("AntennaDiversity.asl")
    Include("GeneralPurposeConfig.asl")
  }
}

Scope(\_SB.PC00.UA00)
{
  //
  // Bluetooth controller using serial interface
  //
  Device(BTH0){

    Method(_HID) {
      Return("INT33E1")  // TODO: HID is open EHL Discrete BT and can be udpated once it is avaialbe
    }

    Method(_INI) {
      SHPO(GBTI,1) // configure gpio pad in gpio driver mode; Bluetooth Interrupt
      SHPO(GBTK,1) // configure gpio pad in gpio driver mode; Bluetooth RF-Kill
    }

    Method(_CRS, 0x0, Serialized){
      Name(SBFG, ResourceTemplate (){
        UARTSerialBus(115200,,,0xc0,,,FlowControlHardware,32,32,"\\_SB.PC00.UA00" )
        GpioIo(Exclusive, PullDefault, 0, 0, IoRestrictionOutputOnly, "\\_SB.GPI0",,,KIL1 ) {0}
        GpioInt (Edge, ActiveLow, ExclusiveAndWake, PullDefault, 0x0000, "\\_SB.GPI0", 0x00, ResourceConsumer, INT1) { 0 }
      })
      If(LEqual(\NGPS,1)) {
        CreateWordField (SBFG, Add(INT1, ACPI_GPIO_CONN_DESC_OFFSET_NAME), IOF1)
        CreateField (SBFG, Multiply (Add (IOF1, INT1), 8), 72, ICS1)
        CreateWordField (SBFG, Add(KIL1, ACPI_GPIO_CONN_DESC_OFFSET_NAME), KOF1)
        CreateField (SBFG, Multiply (Add (KOF1, KIL1), 8), 72, KCS1)
      }
      CreateWordField(SBFG,INT1._PIN,INT3)
      CreateWordField(SBFG,KIL1._PIN,KIL3)
      Store(GNUM(GBTI),INT3)
      Store(GNUM(GBTK),KIL3)
      If(LEqual(\NGPS,1)) {
        Concatenate ("\\_SB.GPI", ToDecimalString(GCOM(GBTI)), ICS1)
        Concatenate ("\\_SB.GPI", ToDecimalString(GCOM(GBTK)), KCS1)
      }

      Name(SBFI, ResourceTemplate (){
        UARTSerialBus(115200,,,0xc0,,,FlowControlHardware,32,32,"\\_SB.PC00.UA00" )
        GpioIo(Exclusive, PullDefault, 0, 0, IoRestrictionOutputOnly, "\\_SB.GPI0",,,KIL2 ) {0}
        Interrupt(ResourceConsumer, Edge, ActiveLow, ExclusiveAndWake,,,INT2 ) {0}
      })
      If(LEqual(\NGPS,1)) {
        CreateWordField (SBFI, Add(INT2, ACPI_GPIO_CONN_DESC_OFFSET_NAME), IOF2)
        CreateField (SBFI, Multiply (Add (IOF2, INT2), 8), 72, ICS2)
        CreateWordField (SBFI, Add(KIL2, ACPI_GPIO_CONN_DESC_OFFSET_NAME), KOF2)
        CreateField (SBFI, Multiply (Add (KOF2, KIL2), 8), 72, KCS2)
      }
      CreateDWordField(SBFI,INT2._INT,INT4)
      CreateWordField(SBFI,KIL2._PIN,KIL4)
      Store(INUM(GBTI),INT4)
      Store(GNUM(GBTK),KIL4)
      If(LEqual(\NGPS,1)) {
        Concatenate ("\\_SB.GPI", ToDecimalString(GCOM(GBTI)), ICS2)
        Concatenate ("\\_SB.GPI", ToDecimalString(GCOM(GBTK)), KCS2)
      }

      If(LEqual(SDM9,0)) {
        Return (SBFG)
      } Else {
        Return (SBFI)
      }
    }

    Method (_STA, 0x0, NotSerialized){
      If(LNotEqual(SDS9,2)) { // Check Discrete BT Over UART is enabled
        Return (0x0)
      }
      Return (0x0F)
    }
    Name (_S0W, 2)                            // Required to put the device to D2 during S0 idle
  } // Device BTH0
  Include("BtRegulatory.asl")
  Include("GeneralPurposeConfig.asl")
}  // End of Scope(\_SB.PC00.UART0)
