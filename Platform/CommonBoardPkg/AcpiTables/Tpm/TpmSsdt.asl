/**@file
  The TPM2 definition block in ACPI table for TCG2 physical presence
  and MemoryClear.

  Copyright (c) 2015 - 2017, Intel Corporation. All rights reserved.<BR>
  (c) Copyright 2016 HP Development Company, L.P.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

DefinitionBlock (
  "Tpm.aml",
  "SSDT",
  2,
  "INTEL ",
  "Tpm2Tabl",
  0x1000
  )
{
  Scope (\_SB)
  {
    Device (TPM)
    {
      //
      // TCG2
      //

      //
      //  TAG for patching TPM2.0 _HID
      //
      //Name (_HID, "NNNN0000")

      Name (_HID, "MSFT0101")

      //
      // Readable name of this device, don't know if this way is correct yet
      //
      Name (_STR, Unicode ("TPM 2.0 Device"))

      //
      // Return the resource consumed by TPM device
      //
      Name (_CRS, ResourceTemplate () {
        Memory32Fixed (ReadWrite, 0xfed40000, 0x5000)
      })


      //
      // Operational region for TPM access
      //
      OperationRegion (TPMR, SystemMemory, 0xfed40000, 0x5000)
      Field (TPMR, AnyAcc, NoLock, Preserve)
      {
        ACC0, 8,
      }


      Method (_STA, 0)
      {
        if (LEqual (ACC0, 0xff))
        {
            Return (0)
        }
        Return (0x0f)
      }

      //
      // TCG Hardware Information
      //
      Method (HINF, 3, Serialized, 0, {BuffObj, PkgObj}, {UnknownObj, UnknownObj, UnknownObj}) // IntObj, IntObj, PkgObj
      {
        //
        // Switch by function index
        //
        Switch (ToInteger(Arg1))
        {
          Case (0)
          {
            //
            // Standard query
            //
            Return (Buffer () {0x03})
          }
          Case (1)
          {
            //
            // Return failure if no TPM present
            //
            Name(TPMV, Package () {0x01, Package () {0x2, 0x0}})
            if (LEqual (_STA (), 0x00))
            {
              Return (Package () {0x00})
            }

            //
            // Return TPM version
            //
            Return (TPMV)
          }
          Default {BreakPoint}
        }
        Return (Buffer () {0})
      }      

      Method (_DSM, 4, Serialized, 0, UnknownObj, {BuffObj, IntObj, IntObj, PkgObj})
      {
        //
        // TCG Hardware Information
        //
        If(LEqual(Arg0, ToUUID ("cf8e16a5-c1e8-4e25-b712-4f54a96702c8")))
        {
          Return (HINF (Arg1, Arg2, Arg3))
        }

        Return (Buffer () {0})
      }
    }
  }
}
