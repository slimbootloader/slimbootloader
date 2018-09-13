@REM @file
@REM   Windows batch file
@REM
@REM Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
@REM (C) Copyright 2016 Hewlett Packard Enterprise Development LP<BR>
@REM This program and the accompanying materials
@REM are licensed and made available under the terms and conditions of the BSD License
@REM which accompanies this distribution.  The full text of the license may be found at
@REM http://opensource.org/licenses/bsd-license.php
@REM
@REM THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
@REM WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
@REM
@echo off

call    EdkSetup.bat

echo.
python  BuildLoader.py  %*
