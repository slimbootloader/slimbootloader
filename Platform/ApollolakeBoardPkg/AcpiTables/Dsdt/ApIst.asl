/** @file
  Intel Processor Power Management IST ACPI Code.

  Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

  Scope(\_PR.CPU1)
  {
    Method(_PPC,0)
    {
      Return(\_PR.CPU0._PPC)  // Return P0 _PPC value.
    }

    Method(_PCT,0)
    {
      Return(\_PR.CPU0._PCT)  // Return P0 _PCT.
    }

    Method(_PSS,0)
    {
      //Return the same table as CPU0 for CMP cases.
      Return(\_PR.CPU0._PSS)
    }
    Method(_PSD,0)
    {
      Return(\_PR.CPU0._PSD)	// Return P0 _PSD.			
    }

  }

  Scope(\_PR.CPU2)
  {
    Method(_PPC,0)
    {
      Return(\_PR.CPU0._PPC)  // Return CPU0 _PPC value.
    }

    Method(_PCT,0)
    {
      Return(\_PR.CPU0._PCT)  // Return CPU0 _PCT value.
    }

    Method(_PSS,0)
    {
      Return(\_PR.CPU0._PSS)  // Return CPU0 _PSS.
    }

    Method(_PSD,0)
    {
      Return(\_PR.CPU0._PSD)	// Return P0 _PSD.			
    }

  }


  Scope(\_PR.CPU3)
  {
    Method(_PPC,0)
    {
      Return(\_PR.CPU0._PPC)  // Return CPU0 _PPC value.
    }

    Method(_PCT,0)
    {
      Return(\_PR.CPU0._PCT)  // Return CPU0 _PCT value.
    }

    Method(_PSS,0)
    {
      Return(\_PR.CPU0._PSS)  // Return CPU0 _PSS.
    }

    Method(_PSD,0)
    {
      Return(\_PR.CPU0._PSD)	// Return P0 _PSD.			
    }

  }

