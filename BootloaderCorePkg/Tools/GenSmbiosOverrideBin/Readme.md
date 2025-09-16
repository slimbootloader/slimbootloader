# GenSmbiosOverrideBin Tool

This tool converts DeviceInfo.json files to binary format to be consumed by SBL (Slim Bootloader) SMBIOS value customization.

## Usage

```bash
python GenSmbiosOverrideBin.py <input_json_file>
```

```bash
python GenSmbiosOverrideBin.py <input_json_file> --output_bin <output_binary_file>
```

### Parameters

- `input_json`: Path to the input JSON file containing device information (required)
- `--output_bin`: Path for the output binary file (optional, default: `DeviceInfo.bin`)

## JSON Input Format

The input JSON file should follow this structure:

```json
{
  "deviceInfo": {
    "system": {
      "manufacturer": "Your Company Name",
      "productName": "Your Product Name",
      "version": "1.0",
      "serialNumber": "SN123456789",
      "uuid": "12345678-1234-1234-1234-123456789ABC",
      "sku": "SKU001",
      "family": "Product Family"
    },
    "baseboard": {
      "manufacturer": "Board Manufacturer",
      "productName": "Board Product Name",
      "version": "1.0",
      "serialNumber": "BSN123456789",
      "assetTag": "ASSET001"
    },
    "chassis": {
      "manufacturer": "Chassis Manufacturer",
      "version": "1.0",
      "serialNumber": "CSN123456789",
      "assetTag": "CHASSIS001"
    },
    "oem": {
      "version": "OEM1.0",
      "customField1": "Custom OEM Data 1",
      "customField2": "Custom OEM Data 2"
    }
  }
}
```

## Integration into SBL

To use the generated binary with Slim Bootloader, copy it to `Platform/<Platform>BoardPkg/Binaries`. Then, enable `ENABLE_SMBIOS_OVERRIDE` in your `BoardConfig.py`. During initialization, SBL will use this binary to update SMBIOS values.


## Adding a New Variable to the Binary Structure

To add a new variable (e.g., BIOS version), follow these steps:

### 1. Update `GenSmbiosOverrideBin.py`
- In the `__init__` method, add your new field to `self.struct_format`:
  ```python
  self.struct_format += f'{MAX_SHORT_STRING_LEN}s'  # BiosVersion
  ```
- In `convert_json_to_binary` method, add the new field to the `struct.pack` call:
  ```python
  bios = data.get('bios', {})  # SMBIOS TYPE 0
  self.string_to_bytes(bios.get('biosVersion', ''), MAX_SHORT_STRING_LEN),
  ```

### 2. Update `DeviceInfo.h`
- Add the new field to your C structure so SBL can parse the binary correctly:
  ```c
  CHAR8 BiosVersion[MAX_SHORT_STRING_LEN];
  ```

### 3. Update Your JSON File
- Add the new variable to your JSON input:
  ```json
  "biosVersion": "BIOS 1.2.3"
  ```
