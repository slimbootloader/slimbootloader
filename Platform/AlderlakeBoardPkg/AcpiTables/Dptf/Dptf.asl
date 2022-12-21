/** @file
  Intel ACPI Reference Code for Intel(R) Dynamic Tuning Technology

  Copyright (c) 2022, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include "CpuPowerMgmt.h"
#include <Include/AcpiDebug.h>
#include "PlatformBoardId.h"

#include "CpuRegs.h"
//#include "CpuGenInfo.h"
#include "Hid.h"

DefinitionBlock (
  "Dptf.aml",
  "SSDT",
  2,
  "DptfTb",
  "DptfTabl",
  0x1000
  )
{

External(\P8XH, MethodObj)
External(\TSOD, IntObj)
External(\PNHM, IntObj)
External(\TCNT, IntObj)
External(\PWRS, IntObj)
External(\CRTT, IntObj)
External(\ACTT, IntObj)
External(\PSVT, IntObj)
External(\ATPC, IntObj)
External(\PTPC, IntObj)

External(\PLID, IntObj) // PlatformId
External(\DPTF, IntObj) // EnableDptf
External(\DCFE, IntObj) // EnableDCFG

External(\SADE, IntObj) // EnableSaDevice

External(\FND1, IntObj) // EnableFan1Device
External(\FND2, IntObj) // EnableFan2Device
External(\FND3, IntObj) // EnableFan3Device

External(\S1DE, IntObj) // EnableSen1Participant
External(\SSP1, IntObj) // SensorSamplingPeriodSen1

External(\S2DE, IntObj) // EnableSen2Participant
External(\SSP2, IntObj) // SensorSamplingPeriodSen2

External(\S3DE, IntObj) // EnableSen3Participant
External(\SSP3, IntObj) // SensorSamplingPeriodSen3

External(\S4DE, IntObj) // EnableSen4Participant
External(\SSP4, IntObj) // SensorSamplingPeriodSen4

External(\S5DE, IntObj) // EnableSen5Participant
External(\SSP5, IntObj) // SensorSamplingPeriodSen5

External(\S6DE, IntObj) // EnableDgpuParticipant
External(\S6P2, IntObj) // Thermal Sampling Period

External(\CHGE, IntObj) // EnableChargerParticipant
External(\PWRE, IntObj) // EnablePowerParticipant
External(\PPPR, IntObj) // PowerParticipantPollingRate

External(\BATR, IntObj) // EnableBatteryParticipant
External(\IN34, IntObj) // EnableInt3400Device.

External(\PPSZ, IntObj) // PPCC Step Size
External(\PF00, IntObj) // PR00 _PDC Flags

External(\ODV0, IntObj) // OemDesignVariable0
External(\ODV1, IntObj) // OemDesignVariable1
External(\ODV2, IntObj) // OemDesignVariable2
External(\ODV3, IntObj) // OemDesignVariable3
External(\ODV4, IntObj) // OemDesignVariable4
External(\ODV5, IntObj) // OemDesignVariable5

External(\_TZ.ETMD, IntObj)
External(\_TZ.TZ00, ThermalZoneObj)

External(\_SB.PC00, DeviceObj)
External(\_SB.PC00.TCPU, DeviceObj)
External(\_SB.PC00.MC.MHBR, FieldUnitObj)

External(\ECON, IntObj)
External(\_SB.PC00.LPCB.H_EC, DeviceObj)

External(\_SB.PC00.LPCB.ITE8.TFN1, DeviceObj)
External(\_SB.PC00.LPCB.ITE8.SEN1, DeviceObj)
External(\_SB.PC00.LPCB.ITE8.SEN2, DeviceObj)

External(\_SB.PC00.LPCB.H_EC.ECAV, IntObj)
External(\_SB.PC00.LPCB.H_EC.ECMD, MethodObj)    // EC Command Method
External(\_SB.PC00.LPCB.H_EC.ECRD, MethodObj)    // EC Read Method
External(\_SB.PC00.LPCB.H_EC.ECWT, MethodObj)    // EC Write Method
External(\_SB.PC00.LPCB.H_EC.ECF2, OpRegionObj)

//
// Sensors
//
External(\_SB.PC00.LPCB.H_EC.TSR1, FieldUnitObj) // Sensor Temperature Values
External(\_SB.PC00.LPCB.H_EC.TSR2, FieldUnitObj)
External(\_SB.PC00.LPCB.H_EC.TSR3, FieldUnitObj)
External(\_SB.PC00.LPCB.H_EC.TSR4, FieldUnitObj)
External(\_SB.PC00.LPCB.H_EC.TSR5, FieldUnitObj)
External(\_SB.PC00.LPCB.H_EC.TSI,  FieldUnitObj)  // Sensor Select
External(\_SB.PC00.LPCB.H_EC.HYST, FieldUnitObj)
External(\_SB.PC00.LPCB.H_EC.TSHT, FieldUnitObj)
External(\_SB.PC00.LPCB.H_EC.TSLT, FieldUnitObj)
External(\_SB.PC00.LPCB.H_EC.TSSR, FieldUnitObj)

External(\_SB.PC00.LPCB.H_EC.PPSL, FieldUnitObj)
External(\_SB.PC00.LPCB.H_EC.PPSH, FieldUnitObj)
External(\_SB.PC00.LPCB.H_EC.PINV, FieldUnitObj)
External(\_SB.PC00.LPCB.H_EC.PENV, FieldUnitObj)
External(\_SB.PC00.LPCB.H_EC.PSTP, FieldUnitObj)
External(\_SB.PC00.LPCB.H_EC.CMDR, FieldUnitObj)
External(\_SB.PC00.LPCB.H_EC.CFSP, FieldUnitObj)
External(\_SB.PC00.LPCB.H_EC.DFSP, FieldUnitObj)
External(\_SB.PC00.LPCB.H_EC.GFSP, FieldUnitObj)
External(\_SB.PC00.LPCB.H_EC.CPUP, FieldUnitObj)
External(\_SB.PC00.LPCB.H_EC.PMAX, FieldUnitObj)
External(\_SB.PC00.LPCB.H_EC.PLMX, FieldUnitObj)
External(\_SB.PC00.LPCB.H_EC.PECH, FieldUnitObj)
External(\_SB.PC00.LPCB.H_EC.CFAN, FieldUnitObj)
External(\_SB.PC00.LPCB.H_EC.B1RC, FieldUnitObj)
External(\_SB.PC00.LPCB.H_EC.B1FC, FieldUnitObj)
External(\_SB.PC00.LPCB.H_EC.BAT1._BST, MethodObj)
External(\_SB.PC00.LPCB.H_EC.BAT1._BIX, MethodObj)
External(\_SB.PC00.LPCB.H_EC.BMAX, FieldUnitObj)
//
// Power Boss
//
External(\_SB.PC00.LPCB.H_EC.PWRT, FieldUnitObj)
External(\_SB.PC00.LPCB.H_EC.PBSS, FieldUnitObj)
External(\_SB.PC00.LPCB.H_EC.VMIN, FieldUnitObj)
External(\_SB.PC00.LPCB.H_EC.PSOC, FieldUnitObj)
External(\_SB.PC00.LPCB.H_EC.AVOL, FieldUnitObj)
External(\_SB.PC00.LPCB.H_EC.ACUR, FieldUnitObj)
External(\_SB.PC00.LPCB.H_EC.ARTG, FieldUnitObj)
External(\_SB.PC00.LPCB.H_EC.CTYP, FieldUnitObj)
External(\_SB.PC00.LPCB.H_EC.BICC, FieldUnitObj)
External(\_SB.PC00.LPCB.H_EC.PROP, FieldUnitObj)
External(\_SB.PC00.LPCB.H_EC.AP01, FieldUnitObj)
External(\_SB.PC00.LPCB.H_EC.AP02, FieldUnitObj)
External(\_SB.PC00.LPCB.H_EC.AP10, FieldUnitObj)

External(\_SB.PC00.LPCB.H_EC.PPWR, FieldUnitObj)
External(\_SB.PC00.LPCB.H_EC.CHGR, FieldUnitObj)
External(\_SB.PC00.LPCB.H_EC.FCHG, FieldUnitObj)

External(\_SB.CPPC, FieldUnitObj)
External(\_SB.PR00, ProcessorObj)
External(\_SB.PR00._PSS, MethodObj)
External(\_SB.PR00.TPSS, PkgObj)
External(\_SB.PR00.LPSS, PkgObj)
External(\_SB.PR00._PPC, MethodObj)
External(\_SB.PR00._TSS, MethodObj)
External(\_SB.CFGD, FieldUnitObj)
External(\_SB.PR00.TSMF, PkgObj)
External(\_SB.PR00.TSMC, PkgObj)
External(\_SB.PR00._PTC, MethodObj)
External(\_SB.PR00._TSD, MethodObj)
External(\_SB.PR00._TPC, IntObj)
External(\_SB.PR01, ProcessorObj)
External(\_SB.PR02, ProcessorObj)
External(\_SB.PR03, ProcessorObj)
External(\_SB.PR04, ProcessorObj)
External(\_SB.PR05, ProcessorObj)
External(\_SB.PR06, ProcessorObj)
External(\_SB.PR07, ProcessorObj)
External(\_SB.PR08, ProcessorObj)
External(\_SB.PR09, ProcessorObj)
External(\_SB.PR10, ProcessorObj)
External(\_SB.PR11, ProcessorObj)
External(\_SB.PR12, ProcessorObj)
External(\_SB.PR13, ProcessorObj)
External(\_SB.PR14, ProcessorObj)
External(\_SB.PR15, ProcessorObj)
External(\_SB.PR16, ProcessorObj)
External(\_SB.PR17, ProcessorObj)
External(\_SB.PR18, ProcessorObj)
External(\_SB.PR19, ProcessorObj)
External(\_SB.PR20, ProcessorObj)
External(\_SB.PR21, ProcessorObj)
External(\_SB.PR22, ProcessorObj)
External(\_SB.PR23, ProcessorObj)
External(\_SB.PR24, ProcessorObj)
External(\_SB.PR25, ProcessorObj)
External(\_SB.PR26, ProcessorObj)
External(\_SB.PR27, ProcessorObj)
External(\_SB.PR28, ProcessorObj)
External(\_SB.PR29, ProcessorObj)
External(\_SB.PR30, ProcessorObj)
External(\_SB.PR31, ProcessorObj)

External(\_SB.CLVL, FieldUnitObj)
External(\_SB.CBMI, FieldUnitObj)
External(\_SB.PL10, FieldUnitObj)
External(\_SB.PL20, FieldUnitObj)
External(\_SB.PLW0, FieldUnitObj)
External(\_SB.CTC0, FieldUnitObj)
External(\_SB.TAR0, FieldUnitObj)
External(\_SB.PL11, FieldUnitObj)
External(\_SB.PL21, FieldUnitObj)
External(\_SB.PLW1, FieldUnitObj)
External(\_SB.CTC1, FieldUnitObj)
External(\_SB.TAR1, FieldUnitObj)
External(\_SB.PL12, FieldUnitObj)
External(\_SB.PL22, FieldUnitObj)
External(\_SB.PLW2, FieldUnitObj)
External(\_SB.CTC2, FieldUnitObj)
External(\_SB.TAR2, FieldUnitObj)
External(\_SB.APSV, FieldUnitObj)
External(\_SB.AAC0, FieldUnitObj)
External(\_SB.ACRT, FieldUnitObj)
External(\_SB.PAGD, DeviceObj)
External(\_SB.PAGD._PUR, PkgObj)
External(\_SB.PAGD._STA, MethodObj)

// Platform-Wide OS Capable externals
External(\_SB.OSCP, IntObj)

// Intel Proprietary Wake up Event support externals.
External(\_SB.HIDD.HPEM, MethodObj)
External(\_SB.SLPB, DeviceObj)
External(HIDW, MethodObj)
External(HIWC, MethodObj)
External(\PCHE, FieldUnitObj) // EnablePchFivrParticipant
External(\_SB.CPID)

Scope(\_SB)
{
  //
  // DPTF Thermal Zone Device
  //
  //
  Device(IETM)
  {
      Name(_ADR,0)
      // GHID (Get HID)
      //
      // This method returns the HID value of the device
      //
      // Arguments: (1)
      //   Arg0: _UID Unique ID for the Device
      // Return Value:
      //   Returns appropriate HID for the respective participant
      //   "XXXX9999": return value if _UID doesn't match existing participants
      //

      Method(GHID, 1, Serialized,,StrObj)
      {
        If (LEqual(Arg0, "IETM")) {
          Return(IETM_PARTICIPANT_HID_ADL)
        }
        If (LEqual(Arg0, "SEN1")) {
          Return(SENX_PARTICIPANT_HID_ADL)
        }
        If (LEqual(Arg0, "SEN2")) {
          Return(SENX_PARTICIPANT_HID_ADL)
        }
        If (LEqual(Arg0, "SEN3")) {
          Return(SENX_PARTICIPANT_HID_ADL)
        }
        If (LEqual(Arg0, "SEN4")) {
          Return(SENX_PARTICIPANT_HID_ADL)
        }
        If (LEqual(Arg0, "SEN5")) {
          Return(SENX_PARTICIPANT_HID_ADL)
        }
        If (LEqual(Arg0, "TPCH")) {
          Return(PCHP_PARTICIPANT_HID_ADL)
        }
        If (LEqual(Arg0, "TFN1")) {
          Return(TFN1_PARTICIPANT_HID_ADL)
        }
        If (LEqual(Arg0, "TFN2")) {
          Return(TFN1_PARTICIPANT_HID_ADL)
        }
        If (LEqual(Arg0, "TFN3")) {
          Return(TFN1_PARTICIPANT_HID_ADL)
        }
        If (LEqual(Arg0, "TPWR")) {
          Return(TPWR_PARTICIPANT_HID_ADL)
        }
        If (LEqual(Arg0, "1")) {
          Return(BAT1_PARTICIPANT_HID_ADL)
        }
        If (LEqual(Arg0, "CHRG")) {
          Return(CHRG_PARTICIPANT_HID_ADL)
        }
        Return("XXXX9999")
      }

      //
      // Intel DPTF Thermal Framework Device
      //
      //Name (_UID, "IETM")
      /*Method(_HID)
      {
        Return(\_SB.IETM.GHID(_UID))  // Intel (R) Dynamic Tuning Technology IETM device
      }*/
      Name(_HID,IETM_PARTICIPANT_HID_ADL)

      //-------------------------------------------
      //  Intel Proprietary Wake up Event solution
      //-------------------------------------------
      Method(_DSM, 0x4, Serialized, 0, {IntObj, BuffObj}, {BuffObj, IntObj, IntObj, PkgObj})
      {
        If(CondRefOf(HIWC)) {
          If(HIWC(Arg0)) {
            If(CondRefOf(HIDW)) {
              Return (HIDW(Arg0, Arg1, Arg2, Arg3))
            }
          }
        }
        Return(Buffer(One) { 0x00 }) // Guid mismatch
      }

      // _STA (Status)
      //
      // This object returns the current status of a device.
      //
      // Arguments: (0)
      //   None
      // Return Value:
      //   An Integer containing a device status bitmap:
      //    Bit 0 - Set if the device is present.
      //    Bit 1 - Set if the device is enabled and decoding its resources.
      //    Bit 2 - Set if the device should be shown in the UI.
      //    Bit 3 - Set if the device is functioning properly (cleared if device failed its diagnostics).
      //    Bit 4 - Set if the battery is present.
      //    Bits 5-31 - Reserved (must be cleared).
      //
      Method(_STA)
      {
      If(LAnd(LEqual(\DPTF,1),LEqual(\IN34,1))){
          Return(0x0F)
        } Else {
          Return(0x00)
        }
      }

      //
      // Save original trip points so _OSC method can enable/disable Legacy thermal policies by manipulating trip points.
      //
      Name (PTRP,0)  // Passive trip point
      Name (PSEM,0)  // Passive semaphore
      Name (ATRP,0)  // Active trip point
      Name (ASEM,0)  // Active semaphore
      Name (YTRP,0)  // Critical trip point
      Name (YSEM,0)  // Critical semaphore

      // _OSC (Operating System Capabilities)
      //
      // This object is evaluated by each DPTF policy implementation to communicate to the platform of the existence and/or control transfer.
      //
      // Arguments: (4)
      //   Arg0 - A Buffer containing a UUID
      //   Arg1 - An Integer containing a Revision ID of the buffer format
      //   Arg2 - An Integer containing a count of entries in Arg3
      //   Arg3 - A Buffer containing a list of DWORD capabilities
      // Return Value:
      //   A Buffer containing a list of capabilities
      //
      Method(_OSC, 4,Serialized,,BuffObj,{BuffObj,IntObj,IntObj,BuffObj})
      {

        // Point to Status DWORD in the Arg3 buffer (STATUS)
        CreateDWordField(Arg3, 0, STS1)

        // Point to Caps DWORDs of the Arg3 buffer (CAPABILITIES)
        CreateDWordField(Arg3, 4, CAP1)

        //
        // _OSC needs to validate the Revision.
        //
        // IF Unsupported Revision
        //  Return Unsupported Revision _OSC Failure
        //
        //    STS0[0] = Reserved
        //    STS0[1] = _OSC Failure
        //    STS0[2] = Unrecognized UUID // IDSP is no longer required for Intel(R) Dynamic Tuning dynamic participants.
                                          // Hence bios do not need to report Unrecognized UUID
        //    STS0[3] = Unsupported Revision
        //    STS0[4] = Capabilities masked
        //

        If(LNot(LEqual(Arg1, 1)))
        {
          //
          // Return Unsupported Revision _OSC Failure
          //
          And(STS1,0xFFFFFF00,STS1)
          Or(STS1,0xA,STS1)
          Return(Arg3)
        }

        If(LNot(LEqual(Arg2, 2)))
        {
          //
          // Return Argument 3 Buffer Count not sufficient
          //
          And(STS1,0xFFFFFF00,STS1)
          Or(STS1,0x2,STS1)
          Return(Arg3)
        }

        //
        // Save Auto Passive Trip Point
        //
        If(CondRefOf(\_SB.APSV)){
          If(LEqual(PSEM,0)){
            Store(1,PSEM)
            Store(\_SB.APSV,PTRP)  // use semaphore so variable is only initialized once
          }
        }
        //
        // Save Auto Active Trip Point
        //
        If(CondRefOf(\_SB.AAC0)){
          If(LEqual(ASEM,0)){
            Store(1,ASEM)
            Store(\_SB.AAC0,ATRP)  // use semaphore so variable is only initialized once
          }
        }
        //
        // Save Auto Critical Trip Point
        //
        If(CondRefOf(\_SB.ACRT)){
          If(LEqual(YSEM,0)){
            Store(1,YSEM)
            Store(\_SB.ACRT,YTRP)  // use semaphore so variable is only initialized once
          }
        }

        // CAP1 contains 4 bits. 1st bit is to indicate that Intel(R) Dynamic Tuning is enabled and wants to enabled some policy.
        // Bios can ignore that 1st bit(indicated by "x" in following table) and
        // only look at bits which are dedicated to each policy as shown in following table.
        // When the bit0 is zero the all CAP bits will be zero.
        // 001x: Enable Active Policy
        // 010x: Enable Passive Policy
        // 100x: Enable Critical Policy
        // 011x: Enable Active/Passive Policy
        // 101x: Enable Active/Critical Policy
        // 110x: Enable Passive/Critical Policy
        // 111x: Enable Active/Passive/Critical Policy
        // xxx0: Disabled all of the Intel(R) Dynamic Tuning Policies.

        //
        // Verify the Intel(R) Dynamic Tuning UUID.
        //
        If(LEqual(Arg0, ToUUID ("B23BA85D-C8B7-3542-88DE-8DE2FFCFD698"))){  // Intel(R) Dynamic Tuning GUID
          If(Not(And(STS1, 0x01))) // Test Query Flag
          { // Not a query operation, so process the request
            If(And(CAP1, 0x1)){  // Validate Intel(R) Dynamic Tuning input, When the bit0 is zero then no Intel(R) Dynamic Tuning policy is enabled.
              If(And(CAP1, 0x2)){  // Enable Active Policy. Nullify the legacy thermal zone.
                Store(110,\_SB.AAC0)
                Store(0, \_TZ.ETMD)  // Legacy Active TM Management relies on this variable.
              } Else{  // re-enable legacy thermal zone with active trip point
                Store(ATRP,\_SB.AAC0)
                Store(1, \_TZ.ETMD)
              }
              If(And(CAP1,0x4)){  // Enable Passive Policy. Nullify the legacy thermal zone.
                Store(110,\_SB.APSV)
              }Else{  // re-enable legacy thermal zone with passive trip point
                Store(PTRP,\_SB.APSV)
              }
              If(And(CAP1, 0x8)){  // Enable Critical Policy. Nullify the legacy thermal zone.
                Store(210,\_SB.ACRT)
              }Else{  // re-enable legacy thermal zone with critical trip point
                Store(YTRP,\_SB.ACRT)
              }
              // Send notification to legacy thermal zone for legacy policy to be enabled/disabled
              If(CondRefOf(\_TZ.TZ00)){
                Notify(\_TZ.TZ00, 0x81)
              }
            } Else{
              Store(YTRP,\_SB.ACRT) // re-enable legacy thermal zone with critical trip point
              Store(PTRP,\_SB.APSV) // re-enable legacy thermal zone with passive trip point
              Store(ATRP,\_SB.AAC0) // re-enable legacy thermal zone with active trip point
              Store(1, \_TZ.ETMD)
            }
            // Send notification to legacy thermal zone for legacy policy to be enabled/disabled
            If(CondRefOf(\_TZ.TZ00)){
              Notify(\_TZ.TZ00, 0x81)
            }
          }
          Return(Arg3)
        }

        Return(Arg3)
      }

      // DCFG (DPTF Configuration)
      //
      // Returns a DWORD data representing the desired behavior of DPTF besides supported DSP and participants.
      //
      // Arguments: (0)
      //   None
      // Return Value:
      //   An Integer containing the DPTF Configuration bitmap:
      //    Bit 0 = Generic UI Access Control (0 - enable as default, 1 - disable access)
      //    Bit 1 = Restricted UI Access Control ( 0 - enable as default, 1 - disable access )
      //    Bit 2 = Shell Access Control ( 0 - enable as default, 1 - disable access)
      //    Bit 3 = Environment Monitoring Report Control ( 0 - report is allowed as default, 1 - No environmental monitoring report to Microsoft )
      //    Bit 4 = Thermal Mitigation Report Control ( 0 - No mitigation report to Microsoft as default, 1 - report is allowed)
      //    Bit 5 = Thermal Policy Report Control ( 0 - No policy report to Microsoft as default, 1 - report is allowed)
      //    Bits[31:6] - Reserved (must be cleared).
      //
      Method(DCFG)
      {
        Return(\DCFE)
      }

      // ODVP (Oem Design Variables Package)
      //
      // Variables for OEM's to customize DPTF behavior based on platform changes.
      //
      Name(ODVX,Package(){0,0,0,0,0,0})

      // ODVP (Oem Design Variables Package)
      //
      // Variables for OEM's to customize DPTF behavior based on platform changes.
      //
      // Arguments: (0)
      //   None
      // Return Value:
      //   Package of integers
      //
      Method(ODVP,0,Serialized,,PkgObj)
      {
        Store(\ODV0,Index(ODVX,0))
        Store(\ODV1,Index(ODVX,1))
        Store(\ODV2,Index(ODVX,2))
        Store(\ODV3,Index(ODVX,3))
        Store(\ODV4,Index(ODVX,4))
        Store(\ODV5,Index(ODVX,5))
        Return(ODVX)
      }

    } // End IETM Device
} // End \_SB Scope

If (LEqual(ECON,1)) {
//
// EC support code
//
Scope(\_SB.PC00.LPCB.H_EC) // Open scope to Embedded Controller
{
  //
  // Create a Mutex for PATx methods to prevent Sx resume race condition problems asscociated with EC commands.
  //
  Mutex(PATM, 0)

  // _QF1 (Query - Embedded Controller Query F1)
  //
  // Handler for EC generated SCI number F1.
  //
  // Arguments: (0)
  //   None
  // Return Value:
  //   None
  //
  Method(_QF1)
  { // Thermal sensor threshold crossing event handler
    Store(\_SB.PC00.LPCB.H_EC.ECRD(RefOf(\_SB.PC00.LPCB.H_EC.TSSR)), Local0)
    While(Local0) // Ensure that events occuring during execution
    {             // of this handler are not dropped
      \_SB.PC00.LPCB.H_EC.ECWT(0, RefOf(\_SB.PC00.LPCB.H_EC.TSSR)) // clear all status bits
      If(And(Local0, 0x10))
      { // BIT4: Sensor 5 Threshold Crossed
        Notify(\_SB.PC00.LPCB.H_EC.SEN5, 0x90)
      }
      If(And(Local0, 0x8))
      { // BIT3: Sensor 4 Threshold Crossed
        Notify(\_SB.PC00.LPCB.H_EC.SEN4, 0x90)
      }
      If(And(Local0, 0x4))
      { // BIT2: Sensor 3 Threshold Crossed
        Notify(\_SB.PC00.LPCB.H_EC.SEN3, 0x90)
      }
      If(And(Local0, 0x2))
      { // BIT1: Sensor 2 Threshold Crossed
        Notify(\_SB.PC00.LPCB.H_EC.SEN2, 0x90)
      }
      If(And(Local0, 0x1))
      { // BIT0: Sensor 1 (Dgpu) Threshold Crossed
        Notify(\_SB.PC00.LPCB.H_EC.DGPU, 0x90)
      }
      Store(\_SB.PC00.LPCB.H_EC.ECRD(RefOf(\_SB.PC00.LPCB.H_EC.TSSR)), Local0)
    }
  }

} // End \_SB.PC00.LPCB.H_EC Scope

//
// Fan participant.
//
  Include("TFN1Participant.asl")
  Include("TFN2Participant.asl")
  Include("TFN3Participant.asl")
//
// Participants using device sensors.
//
  Include("ChrgParticipant.asl")
  Include("TPwrParticipant.asl")

//
// Participants using battery.
//
  Include("BatteryParticipant.asl")

//
// Participants using motherboard sensors.
//
  Include("Sen1Participant.asl")
  Include("Sen2Participant.asl")
  Include("Sen3Participant.asl")
  Include("Sen4Participant.asl")
  Include("Sen5Participant.asl")
  Include("dGpuSensor.asl")

//
// Policy support files
//
  Include("Psvt.asl")


}
//#endif // FixedPcdGetBool(PcdEcEnable) == 1

// Policy support files for EC and SIO
  Include("Trt.asl")
  Include("Art.asl")

//
// DPTF Helper functions.
//
  Include("HelperFunctions.asl")


//
// Processor participant.
//
  Include("B0d4Participant.asl")
  Include("B0d4CtdpPolicy.asl")

  //
  // PCH FIVR Participant
  //
  Include("PchpParticipant.asl")

  Include("Dppm.asl")

//
// DPTF Trigger Event function.
//
  Include("DptfTriggerEvent.asl")

Scope(\_SB.IETM)
{
  // GDDV (Get Dptf Data Vault)
  //
  // The data vault can contain APCT, APAT, and PSVT tables.
  //
  //  Arguments: (0)
  //    None
  //  Return Value:
  //    A package containing the data vault
  //
  Method(GDDV,0,Serialized,0,PkgObj)
  {
    If (LEqual(ECON,1))
    {
      Return(Package()
      {
        Buffer()
        {
          Include("BiosDataVault.asl")
        }
      })
      } Else{
      Return(Package()
      {
        Buffer()
        {
          Include("BiosDataVaultCrb.asl")
        }
      })
    }
  }

  // IMOK
  //
  // IMOK to test if DPTF is OK and alive.
  //
  // Arguments: (1)
  //   Arg0 - integer
  //  Return Value: (0)
  //    An Integer containing the status of IMOK
  //
  Method(IMOK,1,,,IntObj)
  {
    //It is mainly a stub placeholder
    //OEMs can implement it in a way they choose
    Return (Arg0)
  }

} // End Scope(\_SB.IETM)


} // End SSDT
