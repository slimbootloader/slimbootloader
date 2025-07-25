#!/bin/bash

set -Eeuo pipefail

trap 'echo "Build failed."' ERR

usage() {
  echo "${0} CMD"
  echo "Available CMDs:"
  echo -e "\todroid_h4              - build Dasharo compatible with Hardkernel ODROID H4"
}

INPUT_IMAGE="${INPUT_IMAGE:-image.rom}"

build_odroid_h4() {
  local flags="-D CRYPTO_PROTOCOL_SUPPORT=TRUE -D SIO_BUS_ENABLE=TRUE \
    -D PERFORMANCE_MEASUREMENT_ENABLE=TRUE \
    -D MULTIPLE_DEBUG_PORT_SUPPORT=TRUE -D BOOTSPLASH_IMAGE=TRUE \
    -D BOOT_MANAGER_ESCAPE=TRUE"
  if [ ! -f "$INPUT_IMAGE" ]; then
    echo "$INPUT_IMAGE doesn't exist or isn't file."
    echo "You can set different path to vendor image in INPUT_IMAGE variable"
    exit 1
  fi
  docker build -t sbl --network=host .
  build_edk2 "edk2-stable202505" "$flags"
  build_slimloader odroidh4 "0xAAFFFF0C"
  echo "Result binary placed in $PWD/Outputs/odroidh4/ifwi-release.bin"
  sha256sum Outputs/odroidh4/ifwi-release.bin >Outputs/odroidh4/ifwi-release.bin.sha256
}

build_edk2() {
  EDK2_VERSION="$1"
  FLAGS="$2"
  rm -rf edk2
  mkdir edk2
  cd edk2
  # clone one commit only
  git init
  git remote remove origin 2>/dev/null || true
  git remote add origin https://github.com/tianocore/edk2.git
  git fetch --depth 1 origin "$EDK2_VERSION"
  git checkout FETCH_HEAD --force
  git submodule update --init --checkout --recursive --depth 1

  docker run --rm -i -u "$UID" -v "$PWD":/edk2 -w /edk2 sbl /bin/bash <<EOF
source edksetup.sh
make -C BaseTools
python ./UefiPayloadPkg/UniversalPayloadBuild.py -t GCC5 -o Dasharo -b RELEASE \
  $FLAGS
EOF
  cd ..
}

build_slimloader() {
  platform="$1"
  platform_data="$2"
  input="$(realpath "$INPUT_IMAGE")"
  git submodule update --init --checkout --recursive --depth 1
  mkdir -p PayloadPkg/PayloadBins/
  cp edk2/Build/UefiPayloadPkgX64/UniversalPayload.elf PayloadPkg/PayloadBins/
  docker run --rm -i -u $UID -v "$input":/tmp/image.rom -v "$PWD":/home/docker/slimbootloader \
    -w /home/docker/slimbootloader sbl /bin/bash <<EOF
export SBL_KEY_DIR="\${PWD}/SblKeys"
if [ ! -d "\$SBL_KEY_DIR" ]; then
  python BootloaderCorePkg/Tools/GenerateKeys.py -k "\$SBL_KEY_DIR"
fi
python BuildLoader.py build "$platform" -r \
  -p "OsLoader.efi:LLDR:Lz4;UniversalPayload.elf:UEFI:Lzma"
python Platform/AlderlakeBoardPkg/Script/StitchLoader.py \
  -i "/tmp/image.rom" \
  -s "Outputs/$platform/SlimBootloader.bin" \
  -o "Outputs/$platform/ifwi-release.bin" \
  -p "$platform_data"
EOF
}

if [ $# -ne 1 ]; then
  usage
  echo ""
  echo "Error: missing CMD"
  exit 1
fi

CMD="$1"

case "$CMD" in
    "odroid_h4" | "odroid_H4" | "ODROID_H4" )
        build_odroid_h4 ""
        ;;
    *)
        echo "Invalid command: \"$CMD\""
        usage
        ;;
esac
