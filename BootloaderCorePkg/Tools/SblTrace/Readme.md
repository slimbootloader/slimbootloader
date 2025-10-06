# SblTrace

## Overview
SblTrace is designed to simplify the Slim Bootloader debugging process by automatically injecting debug print statements. The tool works with a call graph to help identify the exact point where an error occurs, streamlining the debugging workflow.

## How It Works
1. **File Tracking**: The tool scans and generates a list (cscope.files) of all relevant C source files that need to be tracked.
2. **Code Tokenization**: The tracked C files are processed using [Cscope](https://cscope.sourceforge.io/) to extract tokenized data (cscope.out) information.
3. **Call Graph Generation**: The tokenized data is then passed to [tceetree](https://sourceforge.net/p/tceetree/wiki/Home/) to generate a function call graph (tceetree.out).
4. **Debug Print Injection**: Based on the call graph, the tool automatically inserts debug print statements at each function in the firmware code call graph.
5. **Build and Fix**: The updated firmware code is then built and checked for any size-related issues. Then it auto-fixes the issue and rebuilds till a successful build is achieved.
6. **Error Detection**: After execution of new firmware, it helps to pinpoint the exact location where error occurred with the additional prints.

## Prerequisites
- Python (to execute the tool).
- Slim Bootloader code build environment.
- Cscope and tceetree installed in "SblTrace/Tools" folder.

## Usage
   ```sh
   cd SblTrace
   ```
- Update Config.yaml:

The config file contains all the build commands, clean commands, and platform-specific information. Update the same if needed.
To add support for a new platform, add a new entry in this config file. Use the provided data as a reference.

- Update IgnoreList.yaml:

IgnoreList.yaml contains folder, file and function names that need to be ignored during generation of the call graph.
Update them accordingly. This list is optional, but you can add entries that are not related to the specific issue to make the call graph clean and speed up the process.

Run the following command to analyze your firmware code:
```sh
usage: SblTrace.py [-h] -f FUNCTION_NAME -d SBL_PATH -p PLATFORM_NAME

SblTrace - Slim Bootloader Automated Debug and Triage Tool

optional arguments:
  -h, --help            show this help message and exit

required arguments:
  -f FUNCTION_NAME, --FunctionName FUNCTION_NAME
                        Last reported error function name
  -d SBL_PATH, --DirectoryPath SBL_PATH
                        Path to Slim Bootloader source directory
  -p PLATFORM_NAME, --Platform PLATFORM_NAME
                        Platform name as defined in Config.yaml file
```

Example (on Windows host):
```sh

python.exe .\SblTrace.py -f PciEnumeration -d ..\..\..\..\slimbootloader\ -p AZB
```
