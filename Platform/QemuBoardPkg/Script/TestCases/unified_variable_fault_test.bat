@echo off
setlocal
if /I not "%VSCMD_ARG_TGT_ARCH%"=="x86" call "%ProgramFiles(x86)%\Microsoft Visual Studio\2019\Professional\VC\Auxiliary\Build\vcvars32.bat"
if errorlevel 1 exit /b 1
pushd "%~dp0..\..\..\.."
if not exist Outputs\qemu\variable_fault_test mkdir Outputs\qemu\variable_fault_test
cl /nologo /W3 /Od /D MDEPKG_NDEBUG /D _PCD_GET_MODE_8_PcdVariableLibId=0 /I MdePkg\Include /I MdePkg\Include\Ia32 /I BootloaderCommonPkg\Include /I BootloaderCorePkg\Include /FoOutputs\qemu\variable_fault_test\ /FeOutputs\qemu\variable_fault_test\unified_variable_fault_test.exe Platform\QemuBoardPkg\Script\TestCases\unified_variable_fault_test.c
if errorlevel 1 goto failed
Outputs\qemu\variable_fault_test\unified_variable_fault_test.exe
if errorlevel 1 goto failed
popd
exit /b 0
:failed
popd
exit /b 1