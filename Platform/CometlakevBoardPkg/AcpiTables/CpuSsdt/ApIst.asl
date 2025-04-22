/** @file
  Intel Processor Power Management IST ACPI Code.

  Copyright (c) 2018 - 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "CpuPowerMgmt.h"

DefinitionBlock (
  "APIST.aml",
  "SSDT",
  2,
  "PmRef",
  "ApIst",
  0x3000
  )
{
  External(\_SB.PR00, DeviceObj)
  External(\_SB.PR01, DeviceObj)
  External(\_SB.PR02, DeviceObj)
  External(\_SB.PR03, DeviceObj)
  External(\_SB.PR04, DeviceObj)
  External(\_SB.PR05, DeviceObj)
  External(\_SB.PR06, DeviceObj)
  External(\_SB.PR07, DeviceObj)
  External(\_SB.PR08, DeviceObj)
  External(\_SB.PR09, DeviceObj)
  External(\_SB.PR10, DeviceObj)
  External(\_SB.PR11, DeviceObj)
  External(\_SB.PR12, DeviceObj)
  External(\_SB.PR13, DeviceObj)
  External(\_SB.PR14, DeviceObj)
  External(\_SB.PR15, DeviceObj)
  External(\_SB.PR00._PPC, MethodObj)
  External(\_SB.PR00._PCT, MethodObj)
  External(\_SB.PR00._PSS, MethodObj)

  Scope(\_SB.PR01)
  {
    Method(_PPC,0)
    {
      Return(\_SB.PR00._PPC)  // Return P0 _PPC value.
    }

    Method(_PCT,0)
    {
      Return(\_SB.PR00._PCT)  // Return P0 _PCT.
    }

    Method(_PSS,0)
    {
      //Return the same table as PR00 for CMP cases.
      Return(\_SB.PR00._PSS)
    }


  } // End of Scope(\_SB.PR01)

  Scope(\_SB.PR02)
  {
    Method(_PPC,0)
    {
      Return(\_SB.PR00._PPC)  // Return PR00 _PPC value.
    }

    Method(_PCT,0)
    {
      Return(\_SB.PR00._PCT)  // Return PR00 _PCT value.
    }

    Method(_PSS,0)
    {
      Return(\_SB.PR00._PSS)  // Return PR00 _PSS.
    }


  } // End of Scope(\_SB.PR02)


  Scope(\_SB.PR03)
  {
    Method(_PPC,0)
    {
      Return(\_SB.PR00._PPC)  // Return PR00 _PPC value.
    }

    Method(_PCT,0)
    {
      Return(\_SB.PR00._PCT)  // Return PR00 _PCT value.
    }

    Method(_PSS,0)
    {
      Return(\_SB.PR00._PSS)  // Return PR00 _PSS.
    }


  } // End of Scope(\_SB.PR03)

  Scope(\_SB.PR04)
  {
    Method(_PPC,0)
    {
      Return(\_SB.PR00._PPC)  // Return PR00 _PPC value.
    }

    Method(_PCT,0)
    {
      Return(\_SB.PR00._PCT)  // Return PR00 _PCT value.
    }

    Method(_PSS,0)
    {
      Return(\_SB.PR00._PSS)  // Return PR00 _PSS.
    }


  } // End of Scope(\_SB.PR04)

  Scope(\_SB.PR05)
  {
    Method(_PPC,0)
    {
      Return(\_SB.PR00._PPC)  // Return PR00 _PPC value.
    }

    Method(_PCT,0)
    {
      Return(\_SB.PR00._PCT)  // Return PR00 _PCT value.
    }

    Method(_PSS,0)
    {
      Return(\_SB.PR00._PSS)  // Return PR00 _PSS.
    }
  } // End of Scope(\_SB.PR05)

  Scope(\_SB.PR06)
  {
    Method(_PPC,0)
    {
      Return(\_SB.PR00._PPC)  // Return PR00 _PPC value.
    }

    Method(_PCT,0)
    {
      Return(\_SB.PR00._PCT)  // Return PR00 _PCT value.
    }

    Method(_PSS,0)
    {
      Return(\_SB.PR00._PSS)  // Return PR00 _PSS.
    }
  } // End of Scope(\_SB.PR06)

  Scope(\_SB.PR07)
  {
    Method(_PPC,0)
    {
      Return(\_SB.PR00._PPC)  // Return PR00 _PPC value.
    }

    Method(_PCT,0)
    {
      Return(\_SB.PR00._PCT)  // Return PR00 _PCT value.
    }

    Method(_PSS,0)
    {
      Return(\_SB.PR00._PSS)  // Return PR00 _PSS.
    }

  } // End of Scope(\_SB.PR07)

  Scope(\_SB.PR08)
  {
    Method(_PPC,0)
    {
      Return(\_SB.PR00._PPC)  // Return PR00 _PPC value.
    }

    Method(_PCT,0)
    {
      Return(\_SB.PR00._PCT)  // Return PR00 _PCT value.
    }

    Method(_PSS,0)
    {
      Return(\_SB.PR00._PSS)  // Return PR00 _PSS.
    }
  } // End of Scope(\_SB.PR08)

  Scope(\_SB.PR09)
  {
    Method(_PPC,0)
    {
      Return(\_SB.PR00._PPC)  // Return PR00 _PPC value.
    }

    Method(_PCT,0)
    {
      Return(\_SB.PR00._PCT)  // Return PR00 _PCT value.
    }

    Method(_PSS,0)
    {
      Return(\_SB.PR00._PSS)  // Return PR00 _PSS.
    }
  } // End of Scope(\_SB.PR09)

  Scope(\_SB.PR10)
  {
    Method(_PPC,0)
    {
      Return(\_SB.PR00._PPC)  // Return PR00 _PPC value.
    }

    Method(_PCT,0)
    {
      Return(\_SB.PR00._PCT)  // Return PR00 _PCT value.
    }

    Method(_PSS,0)
    {
      Return(\_SB.PR00._PSS)  // Return PR00 _PSS.
    }
  } // End of Scope(\_SB.PR10)

  Scope(\_SB.PR11)
  {
    Method(_PPC,0)
    {
      Return(\_SB.PR00._PPC)  // Return PR00 _PPC value.
    }

    Method(_PCT,0)
    {
      Return(\_SB.PR00._PCT)  // Return PR00 _PCT value.
    }

    Method(_PSS,0)
    {
      Return(\_SB.PR00._PSS)  // Return PR00 _PSS.
    }
  } // End of Scope(\_SB.PR11)

  Scope(\_SB.PR12)
  {
    Method(_PPC,0)
    {
      Return(\_SB.PR00._PPC)  // Return PR00 _PPC value.
    }

    Method(_PCT,0)
    {
      Return(\_SB.PR00._PCT)  // Return PR00 _PCT value.
    }

    Method(_PSS,0)
    {
      Return(\_SB.PR00._PSS)  // Return PR00 _PSS.
    }
  } // End of Scope(\_SB.PR12)

  Scope(\_SB.PR13)
  {
    Method(_PPC,0)
    {
      Return(\_SB.PR00._PPC)  // Return PR00 _PPC value.
    }

    Method(_PCT,0)
    {
      Return(\_SB.PR00._PCT)  // Return PR00 _PCT value.
    }

    Method(_PSS,0)
    {
      Return(\_SB.PR00._PSS)  // Return PR00 _PSS.
    }
  } // End of Scope(\_SB.PR13)

  Scope(\_SB.PR14)
  {
    Method(_PPC,0)
    {
      Return(\_SB.PR00._PPC)  // Return PR00 _PPC value.
    }

    Method(_PCT,0)
    {
      Return(\_SB.PR00._PCT)  // Return PR00 _PCT value.
    }

    Method(_PSS,0)
    {
      Return(\_SB.PR00._PSS)  // Return PR00 _PSS.
    }
  } // End of Scope(\_SB.PR14)

  Scope(\_SB.PR15)
  {
    Method(_PPC,0)
    {
      Return(\_SB.PR00._PPC)  // Return PR00 _PPC value.
    }

    Method(_PCT,0)
    {
      Return(\_SB.PR00._PCT)  // Return PR00 _PCT value.
    }

    Method(_PSS,0)
    {
      Return(\_SB.PR00._PSS)  // Return PR00 _PSS.
    }
  } // End of Scope(\_SB.PR15)

} // End of Definition Block
