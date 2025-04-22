## @ CorruptComponentUtility.py
#  This is a python IFWI/SBL component corruption utility.
#
# Copyright (c) 2022, Intel Corporation. All rights reserved. <BR>
# SPDX-License-Identifier: BSD-2-Clause-Patent
#
##

from CommonUtility import *
from IfwiUtility import *

import random
import argparse
import sys

def corrupt_component(input_image, output_image, component_path):

    image_bin = bytearray(get_file_data(input_image))
    comp_bin = bytearray()

    # Extract the component from the IFWI/BIOS
    status = IFWI_PARSER.extract_component(image_bin, comp_bin, component_path)
    if status:
        sys.exit(1)

    # Perturb some random byte in the component, ensuring it's not
    # padding/null and different from what's currently there
    comp_offset = random.randrange(0, len(comp_bin))
    while comp_bin[comp_offset] == 0xff or comp_bin[comp_offset] == 0x00:
        comp_offset = random.randrange(0, len(comp_bin))
    comp_bin[comp_offset] = random.choice([i for i in range(0, 0xff + 1) if i != comp_bin[comp_offset]])

    # Replace the component in the IFWI/BIOS
    status = IFWI_PARSER.replace_component(image_bin, comp_bin, component_path)
    if status:
        sys.exit(1)

    # Create a new IFWI/BIOS binary
    gen_file_from_object(output_image, image_bin)

if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('-i', '--input-image', dest='input_image', type=str,
                    required=True, help='Specify input IFWI/SBL image file path')
    parser.add_argument('-o', '--output-image', dest='output_image', type=str,
                    required=True,  help='Specify output IFWI/SBL image file path')
    parser.add_argument('-p', '--path', dest='component_path', type=str,
                    required=True,  help='Specify path of component to corrupt '
                    'in IFWI/SBL binary (e.g. IFWI/BIOS/TS0/SG1A for BP0 Stage 1A of '
                    'IFWI binary, use IfwiUtility.py to see all available paths)')
    args = parser.parse_args()

    corrupt_component(args.input_image, args.output_image, args.component_path)
