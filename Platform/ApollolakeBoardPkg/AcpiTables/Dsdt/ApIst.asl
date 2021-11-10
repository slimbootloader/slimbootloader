/** @file
  Intel Processor Power Management IST ACPI Code.

  Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

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
      Return(\_PR.CPU0._PSD)  // Return P0 _PSD.
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
      Return(\_PR.CPU0._PSD)  // Return P0 _PSD.
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
      Return(\_PR.CPU0._PSD)  // Return P0 _PSD.
    }

  }

