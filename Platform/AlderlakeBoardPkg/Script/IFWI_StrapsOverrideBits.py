'''
Copyright (c) 2025, Intel Corporation. All rights reserved.
This is the entry point for all IFWI Pre Build Activities that has to happen
before the BUILD is triggered
Title       : IFWI_StrapsOverrideBits.py
Author(s)   : Chandni, Vyas; Santosh, Deshpande
Description : Things to perform:
      `1. This python file will apply straps on IFWI and generate an IFWI as provided in csv file.
Inputs    : Each function has relevant header details for input and output
Output    : Each function has relevant header details for input and output
'''
import csv, struct
import argparse

'''
Validates all the command line options if users have entered appropriately and all the mandatory values are passed
Title       : setup_arg_parse
Description : This function handles all the command line arguments
Inputs    : user input from the command line.
Outputs   : args has all the arguments stored with commandline option names
'''
def setup_arg_parse():
  parser = argparse.ArgumentParser()
  parser.add_argument('-i', help='FullIFWIBinNamePath',required=True,metavar="Inputbinfile")
  parser.add_argument('-c', help='FullCSVFileNamePath',required=True,metavar="csvfile")
  args = parser.parse_args()
  return args
'''Performs Creating the Build infrastructure
Title       : StrapsOverrideBits
Description :
      This python file will apply straps on IFWI and generate an IFWI as provided in csv file.
Inputs    :
      OutputfileName: This signifies the name of the files to be given.
      ListOfInputFiles: This will take the list of delta files present in the config directory.
Outputs   :
      True on success.
      False otherwise.
'''
def OverrideStrapBits(FullIFWIBinNamePath, FullCSVFileNamePath):
  # Read CSV file
  updatedfile = []
  max_offset = -1
  with open(FullCSVFileNamePath, newline='') as csv_file:
    filereader = csv.DictReader(csv_file, delimiter=",")

    # Go over the CSV file and update the offset column
    for row in filereader:
      offset = row['Offset']
      base_offset = 0x000
      normalized_offset = int(offset, 0) + base_offset

      row['Offset'] = hex(normalized_offset)
      updatedfile.append(row)
      if normalized_offset > max_offset:
        max_offset = normalized_offset

  if max_offset < 0:
    raise ValueError("CSV file is empty or contains no valid offset entries")

  # Sort the CSV file according to the offset column
  sortedfile = sorted(updatedfile, key=lambda d: int(d['Offset'], 0))

  # Read enough bytes to cover the largest offset (DWORD aligned)
  read_size = (max_offset + 4) & ~0x3
  buf_0 = []
  with open(FullIFWIBinNamePath, "rb") as ifwi_0:
    data_0 = ifwi_0.read(read_size)
  byte_count = 0
  shift = 0
  value = 0

  for byte in data_0:
    value = value + (byte << (8 * shift))
    byte_count = byte_count + 1
    shift = (shift + 1) % 4
    if (shift == 0):
      buf_0.append(value)
      value = 0

  # Go over the CSV file and compare it to the IFWI or IFWIs
  for row in sortedfile:
    offset = row['Offset']
    start_bit = row['Start Bit']
    strap_size = row['Strap Size']
    harness = row['Value']
    strap_mask = (1 << int(strap_size, 0)) - 1
    strap_shift = int(start_bit, 0) + ((int(offset, 0) % 4) * 8)
    buf_index = int(offset, 0) // 4
    actual_dword_0 = buf_0[buf_index]
    actual_0 = (actual_dword_0 >> strap_shift) & strap_mask
    harness_int = int(harness, 0)
    if (actual_0 != harness_int):
      print ("Offset - " + offset + ", start bit - " + start_bit + ", strap size - " + strap_size + ", Straps Override value - " + harness + ", IFWI value - " + str(hex(actual_0).rstrip("L")))
      print ("")
      updated_dword = (actual_dword_0 & ~(strap_mask << strap_shift)) | (harness_int << strap_shift)
      buf_0[buf_index] = updated_dword

  # Write back the updated values
  FullIFWIOutputBinNamePath = FullIFWIBinNamePath
  with open(FullIFWIOutputBinNamePath, "rb+") as ifwi_1:
    for dword in buf_0:
      ifwi_1.write(struct.pack('<I', dword & 0xFFFFFFFF))

def main():
  args = setup_arg_parse()
  FullIFWIBinNamePath = args.i
  FullCSVFileNamePath = args.c
  OverrideStrapBits(FullIFWIBinNamePath, FullCSVFileNamePath)

if __name__ == "__main__":
  main()
