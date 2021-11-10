## @ security_stitch_help.py
#  This is a python stitching script for security helper funtions amd xml updates
#
# Copyright (c) 2020-21, Intel Corporation. All rights reserved. <BR>
# SPDX-License-Identifier: BSD-2-Clause-Patent
#
##

import sys
import os
import re
import imp
import struct
import shutil
import glob
import shlex
import subprocess
import xml.etree.ElementTree as ET
from   xml.dom import minidom
from   subprocess   import call
from   BtgSign   import *

btg_profile_values = [\
                    "Boot Guard Profile 0 - No_FVME",\
                    "Not Applicable",\
                    "Not Applicable",\
                    "Boot Guard Profile 3 - VM",\
                    "Boot Guard Profile 4 - FVE",\
                    "Boot Guard Profile 5 - FVME"]

def update_tpm_type(tpm_type, tree):

  ptt_supported_xml_entry     = "./PlatformProtection/IntelPttConfiguration/PttSupported"
  ptt_pwrup_state_xml_entry   = "./PlatformProtection/IntelPttConfiguration/PttPwrUpState"
  ptt_supported_fpf_xml_entry = "./PlatformProtection/IntelPttConfiguration/PttSupportedFpf"
  spi_over_tpmbus_xml_entry   = "./PlatformProtection/TpmOverSpiBusConfiguration/SpiOverTpmBusEnable"

  tpm_config = {
    'ptt': {
              ptt_supported_xml_entry: 'Yes',
              ptt_pwrup_state_xml_entry: 'Enabled',
              ptt_supported_fpf_xml_entry: 'Yes',
              spi_over_tpmbus_xml_entry: 'No'
            },
    'dtpm': {
              ptt_supported_xml_entry: 'No',
              ptt_pwrup_state_xml_entry: 'Disabled',
              ptt_supported_fpf_xml_entry: 'Yes',
              spi_over_tpmbus_xml_entry:'Yes'
            },
    'none': {
              ptt_supported_xml_entry: 'No',
              ptt_pwrup_state_xml_entry: 'Disabled',
              ptt_supported_fpf_xml_entry: 'Yes',
              spi_over_tpmbus_xml_entry: 'No'
            },
    }

  #Enable TPM/PTT device
  for key in tpm_config[tpm_type]:
    node = tree.find(key)
    if node is not None:
      node.attrib['value'] = tpm_config[tpm_type][key]

  print("[INFO] TPM device type enabled for Measured Boot: %s" % tpm_type)

def update_btGuard_xml(btg_profile, stitch_dir, tree):
    output_dir = os.path.join(stitch_dir, "Temp")
    kmsigpubkeyhashfile = os.path.join(output_dir, "kmsigpubkey.hash")

    if btg_profile == 'vm':
        btguardprofile = 3
    elif btg_profile == 'fve':
        btguardprofile = 4
    elif btg_profile == 'fvme':
        btguardprofile = 5
    else:
       print ("Boot Guard is NOT enabled.....")
       btguardprofile = 0

    # Convert OEM key hash into hexadecimal for Fit tool consumption
    with open(kmsigpubkeyhashfile,"rb") as kmsigpubkeyhash_fh:
        hash = bytearray (kmsigpubkeyhash_fh.read())
        oemkeyhash = ""
        for b in hash:
            oemkeyhash = oemkeyhash + "%02X " % b
        oemkeyhash = oemkeyhash[:-1]
    print("oemkeyhash:")
    print(oemkeyhash)

    node = tree.find('./PlatformProtection/PlatformIntegrity/OemPublicKeyHash')
    node.attrib['value'] = oemkeyhash

    node = tree.find('./PlatformProtection/BootGuardConfiguration/BtGuardKeyManifestId')
    node.attrib['value'] = '0x1'

    node = tree.find('./PlatformProtection/PlatformIntegrity/OemExtInputFile')
    node.attrib['value'] = os.path.join(output_dir, "OemExtInputFile.bin")

    node = tree.find('./PlatformProtection/BootGuardConfiguration/BtGuardProfileConfig')
    node.attrib['value'] = btg_profile_values[btguardprofile]


def gen_bpmgen2_params (stitch_cfg_file, InFile, OutFile):
    InFileptr = open(InFile, 'r', encoding='utf8')
    lines = InFileptr.readlines()
    InFileptr.close()

    params_change_list = stitch_cfg_file.get_bpmgen2_params_change_list()

    for item in params_change_list:
        for variable, value in item:
            for linenumber, line in enumerate(lines):
                if line.split(':')[0].strip() == variable:
                    lines[linenumber] = variable + ':    ' + value + '\n'
                    break

    if OutFile == '':
        OutFile = Infile

    Outfileptr = open(OutFile, 'w')
    Outfileptr.write("".join(lines))
    Outfileptr.close()

def update_btGuard_manifests(stitch_dir, stitch_cfg_file, btg_profile, tpm):
    output_dir  = os.path.join(stitch_dir, "Temp")
    sbl_file    = os.path.join (output_dir, 'SlimBootloader.bin')
    out_file    = os.path.join(output_dir, "BiosRegion.bin")
    bpm_gen2dir = os.path.join (stitch_dir, 'BpmGen2')
    bmpgen_params = os.path.join(output_dir, "bpmgen2.params")
    bpm_key_dir = os.path.join (bpm_gen2dir, 'keys')
    key_size    = '3072'
    hash_type   = 'sha384'
    key_manifest_hash_file = os.path.join(output_dir, "kmsigpubkey.hash")

    print ("Generating BPM GEN2 params file")
    gen_bpmgen2_params(stitch_cfg_file, os.path.join(bpm_gen2dir, "bpmgen2.params"), os.path.join(output_dir, "bpmgen2.params"))

    print("Sign partitions....")
    sign_slimboot_binary(sbl_file, bpm_gen2dir, bmpgen_params, bpm_key_dir, key_size, hash_type, out_file, output_dir, key_manifest_hash_file)

    # Generate OemKeyManifest bin if not available in inputs
    print("Generate Oem Key Manifest....")
    if os.name == 'nt':
        meu_path = os.path.join (stitch_dir, 'Meu', 'meu.exe')
    else:
        meu_path = os.path.join (stitch_dir, 'Meu', 'meu')
    oem_bin_input  = os.path.join (stitch_dir, 'Input', 'OemExtInputFile.bin')
    oem_bin_output = os.path.join (stitch_dir, 'Temp', 'OemExtInputFile.bin')
    gen_sign_oem_key_manifest(meu_path, oem_bin_input, bpm_key_dir, key_size, output_dir, oem_bin_output)

    # Update xml for Btg
    updated_xml_file = os.path.join (stitch_dir, 'Temp', 'updated.xml')

    tree = ET.parse(updated_xml_file)
    update_btGuard_xml(btg_profile, stitch_dir, tree)
    update_tpm_type(tpm, tree)

    tree.write(updated_xml_file)
