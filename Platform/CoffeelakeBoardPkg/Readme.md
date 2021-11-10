# Whiskeylake(WHL)/CoffeeLake(CFL)

## Clone source
- git clone https://github.com/slimbootloader/slimbootloader.git

## Building
- python BuildLoader.py build cfl
- **NOTE**: Build target(cfl) is common for both WHL/CFL
- **NOTE**: For UEFI payload, please refer to UefiPayloadPkg in edk2 repo.
            In UefiPayloadPkg/BuildAndIntegrationInstructions.txt,
            refer to <HOW TO INTEGRATE INTO SLIM BOOTLOADER> section.

## Stitching
- Prepare/Download UEFI based IFWI for WHL or CFL
- python Platform/CoffeelakeBoardPkg/Script/StitchLoader.py
  -i `EXISTING IFWI IMAGE`
  -o `SBL IFWI IMAGE`
  -s Outputs/cfl/SlimBootloader.bin
- **NOTE**: Please make sure that this stitching method will work only if
            Boot Guard in the base image is not enabled, and the silicon
            is not fused with Boot Guard enabled.

## Flashing
- Flash generated `SBL IFWI IMAGE` to the target board using DediProg
  SF100 or SF600 programmer.
