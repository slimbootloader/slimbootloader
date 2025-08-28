#!/usr/bin/env python
##
# GenSmbiosOverrideBin.py
# Convert DeviceInfo.json to binary format for SBL SMBIOS customization
#
# Copyright (c) 2025, Intel Corporation. All rights reserved.<BR>
# SPDX-License-Identifier: BSD-2-Clause-Patent
#
##

import json
import struct
import sys
import os
from argparse import ArgumentParser

# Binary structure constants
MAX_STRING_LEN = 64
MAX_SHORT_STRING_LEN = 32
MAX_UUID_LEN = 37

class DeviceInfoBinary:
    def __init__(self):
        self.struct_format = '<'  # Little endian

        # System Information (SMBIOS Type 1)
        self.struct_format += f'{MAX_STRING_LEN}s'      # SystemManufacturer
        self.struct_format += f'{MAX_STRING_LEN}s'      # SystemProductName
        self.struct_format += f'{MAX_SHORT_STRING_LEN}s' # SystemVersion
        self.struct_format += f'{MAX_SHORT_STRING_LEN}s' # SystemSerialNumber
        self.struct_format += f'{MAX_UUID_LEN}s'        # SystemUUID
        self.struct_format += f'{MAX_SHORT_STRING_LEN}s' # SystemSKU
        self.struct_format += f'{MAX_STRING_LEN}s'      # SystemFamily

        # Baseboard Information (SMBIOS Type 2)
        self.struct_format += f'{MAX_STRING_LEN}s'      # BaseboardManufacturer
        self.struct_format += f'{MAX_STRING_LEN}s'      # BaseboardProductName
        self.struct_format += f'{MAX_SHORT_STRING_LEN}s' # BaseboardVersion
        self.struct_format += f'{MAX_SHORT_STRING_LEN}s' # BaseboardSerialNumber
        self.struct_format += f'{MAX_SHORT_STRING_LEN}s' # BaseboardAssetTag

        # Chassis Information (SMBIOS Type 3)
        self.struct_format += f'{MAX_STRING_LEN}s'      # ChassisManufacturer
        self.struct_format += f'{MAX_SHORT_STRING_LEN}s' # ChassisVersion
        self.struct_format += f'{MAX_SHORT_STRING_LEN}s' # ChassisSerialNumber
        self.struct_format += f'{MAX_SHORT_STRING_LEN}s' # ChassisAssetTag

        # OEM Data
        self.struct_format += f'{MAX_SHORT_STRING_LEN}s' # OemVersion
        self.struct_format += f'{MAX_STRING_LEN}s'       # OemCustomField1
        self.struct_format += f'{MAX_STRING_LEN}s'       # OemCustomField2

        self.binary_size = struct.calcsize(self.struct_format)

    def string_to_bytes(self, value, max_len):
        if value is None:
            value = ""
        value_bytes = value.encode('utf-8')
        if len(value_bytes) >= max_len:
            value_bytes = value_bytes[:max_len-1]
        return value_bytes + b'\x00' * (max_len - len(value_bytes))

    def convert_json_to_binary(self, json_file, output_file):
        try:
            with open(json_file, 'r', encoding='utf-8') as f:
                data = json.load(f)
        except Exception as e:
            print(f"ERROR: {e}")
            return False

        device_info = data.get('deviceInfo', {})
        system = device_info.get('system', {})
        baseboard = device_info.get('baseboard', {})
        chassis = device_info.get('chassis', {})
        oem = device_info.get('oem', {})

        binary_data = struct.pack(
            self.struct_format,

            # System Information
            self.string_to_bytes(system.get('manufacturer', ''), MAX_STRING_LEN),
            self.string_to_bytes(system.get('productName', ''), MAX_STRING_LEN),
            self.string_to_bytes(system.get('version', ''), MAX_SHORT_STRING_LEN),
            self.string_to_bytes(system.get('serialNumber', ''), MAX_SHORT_STRING_LEN),
            self.string_to_bytes(system.get('uuid', ''), MAX_UUID_LEN),
            self.string_to_bytes(system.get('sku', ''), MAX_SHORT_STRING_LEN),
            self.string_to_bytes(system.get('family', ''), MAX_STRING_LEN),

            # Baseboard Information
            self.string_to_bytes(baseboard.get('manufacturer', ''), MAX_STRING_LEN),
            self.string_to_bytes(baseboard.get('productName', ''), MAX_STRING_LEN),
            self.string_to_bytes(baseboard.get('version', ''), MAX_SHORT_STRING_LEN),
            self.string_to_bytes(baseboard.get('serialNumber', ''), MAX_SHORT_STRING_LEN),
            self.string_to_bytes(baseboard.get('assetTag', ''), MAX_SHORT_STRING_LEN),

            # Chassis Information
            self.string_to_bytes(chassis.get('manufacturer', ''), MAX_STRING_LEN),
            self.string_to_bytes(chassis.get('version', ''), MAX_SHORT_STRING_LEN),
            self.string_to_bytes(chassis.get('serialNumber', ''), MAX_SHORT_STRING_LEN),
            self.string_to_bytes(chassis.get('assetTag', ''), MAX_SHORT_STRING_LEN),

            # OEM Data
            self.string_to_bytes(oem.get('version', ''), MAX_SHORT_STRING_LEN),
            self.string_to_bytes(oem.get('customField1', ''), MAX_STRING_LEN),
            self.string_to_bytes(oem.get('customField2', ''), MAX_STRING_LEN),
        )

        try:
            with open(output_file, 'wb') as f:
                f.write(binary_data)
            print(f"Successfully converted '{json_file}' to '{output_file}'")
            print(f"Output binary size: {len(binary_data)} bytes")
            return True
        except IOError as e:
            print(f"ERROR: Failed to write binary file: {e}")
            return False

def main():
    parser = ArgumentParser(description='Convert DeviceInfo.json to binary format for SBL SMBIOS')
    parser.add_argument('input_json', help='Input JSON file path')
    parser.add_argument('--output_bin', default='DeviceInfo.bin', help='Output binary file path (default: DeviceInfo.bin)')

    args = parser.parse_args()

    if not os.path.exists(args.input_json):
        print(f"ERROR: Input file '{args.input_json}' does not exist")
        return 1

    converter = DeviceInfoBinary()
    if converter.convert_json_to_binary(args.input_json, args.output_bin):
        print(f"Conversion completed: {args.input_json} -> {args.output_bin}")
        return 0
    else:
        print("Conversion failed")
        return 1

if __name__ == "__main__":
    sys.exit(main())
