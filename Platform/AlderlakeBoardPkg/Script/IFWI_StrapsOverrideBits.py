## @ IFWI_StrapsOverrideBits.py
#  This python file will apply straps on IFWI and generate an IFWI as provided in csv file.
#
#  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
#
#  SPDX-License-Identifier: BSD-2-Clause-Patent
#
import csv, struct
import argparse

# Validates all the command line options and returns parsed arguments.
# @param   None
# @retval  args  Parsed command line arguments.
def setup_arg_parse():
  parser = argparse.ArgumentParser ()
  parser.add_argument ('-i', help='FullIFWIBinNamePath',required=True,metavar="Inputbinfile")
  parser.add_argument ('-c', help='FullCSVFileNamePath',required=True,metavar="csvfile")
  args = parser.parse_args ()
  return args

# Applies strap overrides from a CSV file onto the specified IFWI binary.
# @param[in]  FullIFWIBinNamePath  Full path to the IFWI binary file.
# @param[in]  FullCSVFileNamePath  Full path to the CSV file with strap override values.
def OverrideStrapBits(FullIFWIBinNamePath, FullCSVFileNamePath):
  # Read CSV file
  updatedfile = []
  max_offset = -1
  with open (FullCSVFileNamePath, newline='') as csv_file:
    filereader = csv.DictReader (csv_file, delimiter=",")

    # Go over the CSV file and update the offset column
    for row in filereader:
      offset = row['Offset']
      base_offset = 0x000
      normalized_offset = int (offset, 0) + base_offset

      row['Offset'] = hex (normalized_offset)
      updatedfile.append (row)
      if normalized_offset > max_offset:
        max_offset = normalized_offset

  if max_offset < 0:
    raise ValueError ("CSV file is empty or contains no valid offset entries")

  # Sort the CSV file according to the offset column
  sortedfile = sorted (updatedfile, key=lambda d: int (d['Offset'], 0))

  # Read enough bytes to cover the largest offset (DWORD aligned)
  read_size = (max_offset + 4) & ~0x3
  buf_0 = []
  with open (FullIFWIBinNamePath, "rb") as ifwi_0:
    data_0 = ifwi_0.read (read_size)
  byte_count = 0
  shift = 0
  value = 0

  for byte in data_0:
    value = value + (byte << (8 * shift))
    byte_count = byte_count + 1
    shift = (shift + 1) % 4
    if (shift == 0):
      buf_0.append (value)
      value = 0

  # Go over the CSV file and compare it to the IFWI or IFWIs
  for row in sortedfile:
    offset = row['Offset']
    start_bit = row['Start Bit']
    strap_size = row['Strap Size']
    harness = row['Value']
    strap_mask = (1 << int (strap_size, 0)) - 1
    strap_shift = int (start_bit, 0) + ((int (offset, 0) % 4) * 8)
    buf_index = int (offset, 0) // 4
    actual_dword_0 = buf_0[buf_index]
    actual_0 = (actual_dword_0 >> strap_shift) & strap_mask
    harness_int = int (harness, 0)
    if (actual_0 != harness_int):
      print ("Offset - " + offset + ", start bit - " + start_bit + ", strap size - " + strap_size + ", Straps Override value - " + harness + ", IFWI value - " + str (hex (actual_0).rstrip ("L")))
      print ("")
      updated_dword = (actual_dword_0 & ~(strap_mask << strap_shift)) | (harness_int << strap_shift)
      buf_0[buf_index] = updated_dword

  # Write back the updated values
  FullIFWIOutputBinNamePath = FullIFWIBinNamePath
  with open (FullIFWIOutputBinNamePath, "rb+") as ifwi_1:
    for dword in buf_0:
      ifwi_1.write (struct.pack ('<I', dword & 0xFFFFFFFF))

def main():
  args = setup_arg_parse ()
  FullIFWIBinNamePath = args.i
  FullCSVFileNamePath = args.c
  OverrideStrapBits (FullIFWIBinNamePath, FullCSVFileNamePath)

if __name__ == "__main__":
  main()
