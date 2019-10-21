## @ StitchIfwi.py
#  This is a python stitching script for Slim Bootloader WHL/CFL build
#
# Copyright (c) 2019, Intel Corporation. All rights reserved. <BR>
# SPDX-License-Identifier: BSD-2-Clause-Patent
#
##

import sys
import os
import re
import struct
import argparse
import zipfile
import shutil
import glob
import shlex
import subprocess
import xml.etree.ElementTree as ET
from   xml.dom import minidom
from   ctypes  import *
from   subprocess   import call

sys.dont_write_bytecode = True
from   StitchLoader import IFWI_PARSER, add_platform_data, get_file_data, gen_file_from_object

btg_profile_values = [\
                    "Boot Guard Profile 0 - No_FVME",\
                    "Not Applicable",\
                    "Not Applicable",\
                    "Boot Guard Profile 3 - VM",\
                    "Boot Guard Profile 4 - FVE",\
                    "Boot Guard Profile 5 - FVME"]

extra_usage_txt = \
"""This script creates a new Whiskey Lake/Coffee Lake Slim Bootloader IFWI image using FIT
tool.  For the FIT tool and stitching ingredients listed in step 2 below, please
contact your Intel representative.

Please follow the steps below for WHL/CFL IFWI stitching.

  1. Create a stitching workspace. The paths mentioned below are all relative
     to it.

  2. Extract required tools and ingredients to stitching workspace.
     - FIT tool
       Copy 'fit.exe' or 'fit' and 'vsccommn.bin' to 'CSE/FIT' folder
       Copy 'cse_image.bin' to 'CSE/Silicon/cse_image.bin'

     - PMC firmware
       Copy PMC firmware image to 'PMC/pmc.bin'.

     - EC firmware
       Copy EC firmware image to 'EC/ec.bin'.
       copy ECregionpointer.bin to 'EC/ecregionpointer.bin'

     - GBE Binary
       Copy GBE binary image to 'GBE/gbe.bin'.

     - ACM firmware
       Copy ACM firmware image to 'ACM/acm.bin'.

     - BPMGEN2 - Tool to create boot policy manifest
       Copy the contents of the tool to bpmgen2 folder

  3. Extract additional public released ingredients to stitching workspace.
     - Openssl
       For Windows environment, please download required OpenSSL tool.
       For example, it can be downloaded from:
       https://sourceforge.net/projects/openssl
       Once downloaded, please extract the executable openssl.exe and configuration file
       openssl.cnf into 'Openssl' folder in stitching workspace.
       For Linux environment, please install openssl package directly.

  4. Stitch the final image
     Run this script from Slim Bootloader root directory:
     EX:
       Assuming stitching workspace is at D:\Stitch and building ifwi for WHL platform
       To stitch IFWI with SPI QUAD mode and Boot Guard profile VM:
         python  Platform\CoffeelakeBoardPkg\Script\StitchIfwi.py -b vm -q -a whl -w D:\Stitch -s Stitch_Components.zip -t ptt
       To clean all generated files:
         python  Platform\CoffeelakeBoardPkg\Script\StitchIfwi.py -c -w D:\Stitch

"""
def get_config ():
    # This dictionary defines path for stitching components
    cfg_var = {
      'wkspace'   :   'output',
      'fitinput'  :   'Output/input',
      'ifwiname'  :   'SBL_IFWI',
      'fit'       :   'CSE/FIT/fit%s' %     ('.exe' if os.name == 'nt' else ''),
      'vsccommn'  :   'CSE/FIT/vsccommn.bin',
      'openssl'   :   'Openssl/openssl.exe' if os.name == 'nt' else '/usr/bin/openssl',
      'cseimg'    :   'CSE/Silicon/cse_image.bin',
      'acm'       :   'ACM/acm.bin',
      'pmc'       :   'PMC/pmc.bin',
      'ec'        :   'EC/ec.bin',
      'ecptr'     :   'EC/ecregionpointer.bin',
      'gbe'       :   'GBE/gbe.bin',
      'bpm'       :   'bpmgen2',
      'bpm_params':   'bpmgen2/Example.bpDef',
    }
    return cfg_var

def get_bpmgen2_params_change_list ():
    params_change_list = []
    params_change_list.append ([
      # variable                | value |
      # ===================================
      ('PlatformRules',         'CFL Client'),
      ('BpmStrutVersion',       '0x10'),
      ('BpmRevision',           '0x01'),
      ('BpmRevocation',         '1'),
      ('AcmRevocation',         '2'),
      ('NEMPages',              '3'),
      ('IbbFlags',              '0x2'),
      ('TxtInclude',            'FALSE'),
      ('BpmSigScheme',          '0x14:RSASSA'),
      ('BpmSigPubKey',          'keys\pubkey.pem'),
      ('BpmSigPrivKey',         'keys\privkey.pem'),
      ])
    return params_change_list

def get_xml_change_list (platform, spi_quad):
    # Disable Quad Io and Out read enable
    if spi_quad:
        value = 'Yes'
    else:
        value = 'No'
    xml_change_list = []
    xml_change_list.append ([
      # Path                                                                            | value |
      # =========================================================================================
      #Region Order
      ('./BuildSettings/BuildResults/RegionOrder',                                        '45321'),
      ('./FlashLayout/BiosRegion/InputFile',                                              '$SourceDir\Slimbootloader.bin'),
      ('./FlashLayout/Ifwi_IntelMePmcRegion/MeRegionFile',                                '$SourceDir\cse_image.bin'),
      ('./FlashLayout/Ifwi_IntelMePmcRegion/PmcBinary',                                   '$SourceDir\pmc.bin'),
      ('./FlashLayout/EcRegion/InputFile',                                                '$SourceDir\ec.bin'),
      ('./FlashLayout/EcRegion/Enabled',                                                  'Enabled'),
      ('./FlashLayout/EcRegion/EcRegionPointer',                                          '$SourceDir\ecregionpointer.bin'),
      ('./FlashLayout/GbeRegion/Enabled',                                                 'Enabled'),
      ('./FlashLayout/GbeRegion/InputFile',                                               '$SourceDir\gbe.bin'),
      ('./FlashSettings/FlashConfiguration/QuadIoReadEnable',                             value),
      ('./FlashSettings/FlashConfiguration/QuadOutReadEnable',                            value),
      #VsccTable
      ('./FlashSettings/VsccTable/VsccEntries/VsccEntry/VsccEntryName',                   'W25Q256FV'),
      ('./FlashSettings/VsccTable/VsccEntries/VsccEntry/VsccEntryVendorId',               '0xEF'),
      ('./FlashSettings/VsccTable/VsccEntries/VsccEntry/VsccEntryDeviceId0',              '0x40'),
      ('./FlashSettings/VsccTable/VsccEntries/VsccEntry/VsccEntryDeviceId1',              '0x19'),
      #PttConfiguration
      ('./PlatformProtection/IntelPttConfiguration/PttSupported',                         'No'),
      ('./PlatformProtection/IntelPttConfiguration/PttPwrUpState',                        'Disabled'),
      #BootGuardConfiguration
      ('./PlatformProtection/BiosGuardConfiguration/BiosGrdProtOvrdEn',                   'No'),
      #ICC
      ('./Icc/IccPolicies/Profiles/Profile/ClockOutputConfiguration/ClkoutCpunsscPnPath', 'Direct XTAL IN / Out Path'),
      #Networking
      ('./NetworkingConnectivity/WiredLanConfiguration/PhyConnected',                     'PHY on SMLink0'),
      #ISH
      ('./IntegratedSensorHub/IshSupported',                                              'No'),
      ])

    if platform == 'whl':
        xml_change_list.append ([
            ('./PlatformProtection/IntelPttConfiguration/PttSupportedFpf',                'No'),
            #IntelMekernel
            ('./IntelMeKernel/Processor/ProcEmulation',                                   'EMULATE Intel (R) vPro (TM) capable Processor'),
            #StrapsDifferences
            #CPU Straps
            ('./CpuStraps/IaPowerPlaneTopology',                                          '0x00000000'),
            ('./CpuStraps/RingPowerPlaneTopology',                                        '0x00000000'),
            ('./CpuStraps/GtUsPowerPlaneTopology',                                        '0x00000001'),
            ('./CpuStraps/GtSPowerPlaneTopology',                                         '0x00000001'),
            ('./StrapsDifferences/PCH_Strap_SPI_touch2_max_freq_Diff',                    '0x03'),
            ('./StrapsDifferences/PCH_Strap_PN0_RPCFG_0_Diff',                            '0x03'),
            ('./StrapsDifferences/PCH_Strap_PN1_RPCFG_0_Diff',                            '0x03'),
            ('./StrapsDifferences/PCH_Strap_PN2_RPCFG_0_Diff',                            '0x00'),
            ('./StrapsDifferences/PCH_Strap_PN3_RPCFG_0_Diff',                            '0x03'),
            ('./StrapsDifferences/PCH_Strap_EXI_PTSS_PORT3_Diff',                         '0x01'),
            ('./StrapsDifferences/PCH_Strap_EXI_PTSS_PORT1_Diff',                         '0x01'),
            ('./StrapsDifferences/PCH_Strap_GBE_SMLink1_Frequency_Diff',                  '0x00'),
            ('./StrapsDifferences/PCH_Strap_GBE_Reserved3_Diff',                          '0x0F'),
            ('./StrapsDifferences/CPU_Strap_SEPARATE_VCCAGSH_EXISTS_Diff',                '0x00'),
            #FlexIO
            ('./FlexIO/PciePortConfiguration/PCIeContoller2Config',                       '1x4'),
            ('./FlexIO/SataPcieComboPortConfiguration/SataPCIeComboPort0',                'PCIe'),
            ('./FlexIO/SataPcieComboPortConfiguration/SataPCIeComboPort2',                'PCIe'),
            ('./FlexIO/Usb3PortConfiguration/USB3Prt2ConTypeSel',                         'Type A'),
            ('./FlexIO/Usb3PortConfiguration/USB3Prt3ConTypeSel',                         'Type A'),
            ('./FlexIO/Usb2PortConfiguration/USB2Prt2ConTypeSel',                         'Type C'),
            ('./FlexIO/Usb2PortConfiguration/USB2Prt3ConTypeSel',                         'Type A'),
            ('./FlexIO/Usb2PortConfiguration/USB2Prt5ConTypeSel',                         'Type A'),
            #GPIO
            ('./Gpio/GpioVccioVoltageControl/GppA7voltSelect',                            '3.3Volts'),
            ('./Gpio/GpioVccioVoltageControl/GppA8voltSelect',                            '3.3Volts'),
            ('./Gpio/GpioVccioVoltageControl/GppA16voltSelect',                           '3.3Volts'),
            ('./Gpio/GpioVccioVoltageControl/GppA21voltSelect',                           '3.3Volts'),
            ('./Gpio/GpioVccioVoltageControl/GppC11voltSelect',                           '3.3Volts'),
            ('./Gpio/GpioVccioVoltageControl/GppD9voltSelect',                            '1.8Volts'),
            ('./Gpio/GpioVccioVoltageControl/GppH12voltSelect',                           '3.3Volts'),
            ('./Gpio/GpioVccioVoltageControl/GppH14voltSelect',                           '3.3Volts'),
            ('./Gpio/GpioVccioVoltageControl/GppH16voltSelect',                           '1.8Volts'),
            ('./Gpio/GpioVccioVoltageControl/GppH17voltSelect',                           '1.8Volts'),
            ('./Gpio/GpioVccioVoltageControl/GppH20voltSelect',                           '3.3Volts'),
            ('./Gpio/GpioVccioVoltageControl/GppH22voltSelect',                           '3.3Volts'),
            #SMBUS
            ('./InternalPchBuses/SmbusSmlinkConfiguration/SLink1freq',                    '1 MHz'),
            ])

    if platform == 'cflh':
        xml_change_list.append ([
            #Networking
            ('./NetworkingConnectivity/WiredLanConfiguration/GbePCIePortSelect',          'Port 5'),
            ('./InternalPchBuses/DmiConfiguration/DmiLaneReversal',                       'No'),
            ('./Debug/DirectConnectInterfaceConfiguration/Usb9DciBssbEnable',             'Yes'),
            ('./CpuStraps/SaVrType',                                                      'SVID'),
            ('./CpuStraps/VccinSvidAddrs',                                                '0x0'),
            ('./CpuStraps/VccinVrType',                                                   'SVID'),
            ('./StrapsDifferences/PCH_Strap_PN1_RPCFG_2_Diff',                            '0x2'),
            ('./StrapsDifferences/PCH_Strap_PN2_RPCFG_2_Diff',                            '0x2'),
            ('./FlexIO/IntelRstForPcieConfiguration/RstPCIeController3',                  '2x2'),
            ('./FlexIO/PcieLaneReversalConfiguration/PCIeCtrl3LnReversal',                'Yes'),
            ('./FlexIO/PciePortConfiguration/PCIeContoller2Config',                       '4x1'),
            ('./FlexIO/PciePortConfiguration/PCIeContoller5Config',                       '4x1'),
            ('./FlexIO/SataPcieComboPortConfiguration/SataPCIeComboPort1',                'GPIO Polarity PCIe'),
            ('./FlexIO/SataPcieComboPortConfiguration/SataPCIeComboPort2',                'Disabled'),
            ('./FlexIO/SataPcieComboPortConfiguration/SataPCIeComboPort3',                'Disabled'),
            ('./FlexIO/SataPcieComboPortConfiguration/SataPCIeComboPort6',                'SATA'),
            ('./FlexIO/SataPcieComboPortConfiguration/SataPCIeComboPort7',                'SATA'),
            ('./FlexIO/SataPcieComboPortConfiguration/SataPCIeComboPort8',                'SATA'),
            ('./FlexIO/SataPcieComboPortConfiguration/SataPCIeComboPort9',                'SATA'),
            ('./FlexIO/Usb3PortConfiguration/USB3PCIeComboPort2',                         'USB3'),
            ('./FlexIO/Usb3PortConfiguration/USB3Prt9ConTypeSel',                         'Type A'),
            ('./FlexIO/Usb2PortConfiguration/USB2Prt9ConTypeSel',                         'Type A'),
            ('./FlexIO/Usb2PortConfiguration/USB2Prt14ConTypeSel',                        'Express Card / M.2 S2'),
            ('./Gpio/MeFeaturePins/TouchResetGpio',                                       'GPP_B_14'),
            ('./Gpio/MeFeaturePins/TouchIntGpio',                                         'GPP_D_15'),
            ('./Gpio/GpioVccioVoltageControl/Clkout48ModeConfig',                         'GPP_A16'),
            ])

    if platform == 'cfls':
        xml_change_list.append ([
            #IntelMekernel
            ('./IntelMeKernel/Processor/ProcEmulation',                                   'EMULATE Intel (R) vPro (TM) capable Processor'),
            ])

    return xml_change_list

def run_cmd (cmd, cwd):
    sys.stdout.flush()
    try:
        cmd_args = shlex.split(cmd)
        if os.name == 'nt':
            shell = True
            if cmd_args[0].startswith('./'):
                cmd_args[0] = cmd_args[0][2:]
        else:
            shell = False
        proc = subprocess.Popen(cmd_args, shell = shell, cwd=cwd)
        out, err = proc.communicate()
        ret = proc.returncode
    except Exception as e:
        out, err = '', e
        ret = 1
    if ret:
        sys.stdout.flush()
        print('%s\n%s' % (out, err))
        print("Failed to run command:\n  PATH: %s\n   CMD: %s" % (cwd, ' '.join(cmd_args)))
        sys.exit(1)

def gen_bpmgen2_params (InFile, OutFile):
    InFileptr = open(InFile, 'rb')
    lines = InFileptr.readlines()
    InFileptr.close()

    params_change_list = get_bpmgen2_params_change_list()

    for item in params_change_list:
        for variable, value in item:
            for linenumber, line in enumerate(lines):
                if line.split(':')[0].strip() == variable:
                    lines[linenumber] = variable + ':    ' + value + '\n'
                    break

    if OutFile == '':
        OutFile = Infile

    Outfileptr = open(OutFile, 'wb')
    Outfileptr.write("".join(lines))
    Outfileptr.close()

def swap_ts_block(in_file, out_file, ts_size):
    print("Swapping Top Swap Blocks....")

    if not os.path.exists(in_file):
        raise Exception("%s not found !" % in_file)
        return 1

    in_file_size = os.path.getsize(in_file)

    if ts_size > in_file_size:
        raise Exception("TsSize %x is greater than the file size !" % ts_size)
        return 1

    in_fileptr = open(in_file, 'rb')
    in_file_data = bytearray(in_fileptr.read())
    in_fileptr.close()

    write_data = bytearray(0)
    out_file_ptr = open(out_file, 'wb')
    block_data = in_file_data[0:(in_file_size-(2*ts_size))]
    block_data += in_file_data[(in_file_size-ts_size):in_file_size]
    block_data += in_file_data[(in_file_size-(2*ts_size)):(in_file_size-ts_size)]
    out_file_ptr.write(block_data)
    out_file_ptr.close()

def sign_binary(infile, stitch_dir, cfg_var):
    cfg_var      = get_config ()
    openssl_path = (stitch_dir.replace('\\', '/') + '/' if os.name == 'nt' else '') + cfg_var['openssl']

    output_dir = os.path.join(stitch_dir,"Output", "input")
    shutil.copy(infile, os.path.join(output_dir,"sbl_sec_temp.bin"))

    print("Generating new keys....")

    bpm_gen2dir = stitch_dir + '/' + 'bpmgen2'
    if not os.path.exists(os.path.join(stitch_dir,"bpmgen2", "keys")):
        os.mkdir(os.path.join(bpm_gen2dir, "keys"))

        cmd = '%s genrsa -F4 -out keys/keyprivkey.pem 2048' % openssl_path
        run_cmd (cmd, bpm_gen2dir)

        cmd = '%s rsa -pubout -in keys/keyprivkey.pem -out keys/keypubkey.pem' % openssl_path
        run_cmd (cmd, bpm_gen2dir)

        cmd = '%s genrsa -F4 -out keys/privkey.pem 2048' % openssl_path
        run_cmd (cmd, bpm_gen2dir)

        cmd = '%s rsa -pubout -in keys/privkey.pem -out keys/pubkey.pem' % openssl_path
        run_cmd (cmd, bpm_gen2dir)

    print "Generating BPM GEN2 params file"
    gen_bpmgen2_params(os.path.join(bpm_gen2dir, "Example.bpDef"), os.path.join(bpm_gen2dir, "bpmgen2.params"))

    print("Generating KeyManifest.bin....")
    cmd = './bpmgen2 KM1GEN -KEY keys/pubkey.pem BPM -KM ../output/input/KeyManifest.bin -SIGNKEY keys/keyprivkey.pem -SIGNPUBKEY keys/keypubkey.pem -KMID 0x01 -SVN 0 -d:2 > ../output/input/bpmgen2_km.txt'
    run_cmd (cmd, bpm_gen2dir)

    print("Generating Manifest.bin....")
    cmd = './bpmgen2 GEN ../output/input/sbl_sec_temp.bin bpmgen2.params -BPM ../output/input/Manifest.bin -U ../output/input/sbl_sec.bin -KM ../output/input/KeyManifest.bin -d:2 > ../output/input/bpmgen2_bpm.txt'
    run_cmd (cmd, bpm_gen2dir)

    print("Calculate public key hash....")
    print("Extract Public key")
    cmd = '%s rsa -in %s -pubin -modulus > pubkey.txt' % (openssl_path, '../../bpmgen2/keys/keypubkey.pem')
    run_cmd (cmd, output_dir)

    line=open('output/input/pubkey'+'.txt','r').readline()[8:8+256*2]
    if sys.hexversion >= 0x3000000:
        keybin = bytearray.fromhex(line)
    else:
        keybin = line.decode('hex')
    open('output/input/pubkey'+'.bin','wb').write(keybin[::-1])
    cmd = '%s dgst -sha256 -binary -out pubkey.hash pubkey.bin' % openssl_path
    run_cmd (cmd, output_dir)
    print("printing hash")
    hash = bytearray(open('output/input/pubkey'+'.hash','rb').read())
    print(''.join('%02X ' % b for b in hash))

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

def update_ifwi_xml(btguardprofile, stitch_dir, cfg_var, tree):
    cfg_var      = get_config ()
    openssl_path = (stitch_dir.replace('\\', '/') + '/' if os.name == 'nt' else '') + cfg_var['openssl']

    output_dir = os.path.join(stitch_dir,"Output", "input")
    kmsigpubkeytxtfile  = os.path.join(output_dir, "kmsigpubkey.txt")
    kmsigpubkeybinfile  = os.path.join(output_dir, "kmsigpubkey.bin")
    kmsigpubkeyhashfile = os.path.join(output_dir, "kmsigpubkey.hash")

    with open(kmsigpubkeytxtfile, "w") as kmsigpubkeytxt_fh:
        cmd = '%s rsa -in ../../bpmgen2/keys/keypubkey.pem -pubin -modulus > kmsigpubkeytxt_fh'  % openssl_path
        run_cmd (cmd, output_dir)

    shutil.copy(os.path.join(output_dir, "kmsigpubkeytxt_fh"), kmsigpubkeytxtfile)

    line=open(kmsigpubkeytxtfile,"r").readline()[8:8+256*2]
    if sys.hexversion >= 0x3000000:
        keybin = bytearray.fromhex(line)
    else:
        keybin = line.decode('hex')
    open(kmsigpubkeybinfile,"wb").write(keybin[::-1])

    # In, CFL and  future platforms, public exponent (i.e. 65537) is concatenated for OEM key hash calculation.
    line=open(kmsigpubkeytxtfile,"r").readline()[8:8+256*2]
    open(kmsigpubkeybinfile,"ab").write(bytearray.fromhex('01000100'))
    cmd = '%s dgst -sha256 -binary -out Output/input/kmsigpubkey.hash Output/input/kmsigpubkey.bin' % openssl_path
    run_cmd (cmd, stitch_dir)

    # Convert OEM key hash into hexadecimal for Fit tool consumption
    with open(kmsigpubkeyhashfile,"rb") as kmsigpubkeyhash_fh:
        hash = bytearray (kmsigpubkeyhash_fh.read())
        oemkeyhash = ""
        for b in hash:
            oemkeyhash = oemkeyhash + "%02X " % b
    print(oemkeyhash)

    node = tree.find('./PlatformProtection/BootGuardConfiguration/BtGuardKeyManifestId')
    node.attrib['value'] = '0x1'

    node = tree.find('./PlatformProtection/PlatformIntegrity/OemPublicKeyHash')
    node.attrib['value'] = oemkeyhash

    node = tree.find('./PlatformProtection/BootGuardConfiguration/BtGuardProfileConfig')
    node.attrib['value'] = btg_profile_values[btguardprofile]

def update_btGuard_manifests(stitch_dir, cfg_var):
    print("Sigining Coffeelake....")

    output_dir = os.path.join(stitch_dir,"Output", "input")

    print("Patch ACM binary in both partitions....")
    sbl_file = os.path.join (output_dir, 'SlimBootloader.bin')
    sbl_bin  = bytearray (get_file_data (sbl_file))
    acm_bin  = bytearray (get_file_data (os.path.join (stitch_dir, 'Output/input/acm.bin')))
    ifwi = IFWI_PARSER.parse_ifwi_binary (sbl_bin)
    for x in range (2):
      ret  = IFWI_PARSER.replace_component (sbl_bin, acm_bin, 'IFWI/BIOS/TS%d/ACM0' % x)
      if ret:
          raise Exception("Failed to inject ACM binary !")
    gen_file_from_object (sbl_file, sbl_bin)

    print("Sign primary partition....")
    sign_binary(os.path.join(output_dir,"SlimBootloader.bin"), stitch_dir, cfg_var)

    print("Swap top swap block....")
    swap_ts_block(os.path.join(output_dir, "sbl_sec.bin"), os.path.join(output_dir, "SwappedA.bin"), 0x20000)

    print("Sign backup partition....")
    sign_binary(os.path.join(output_dir,"SwappedA.bin"), stitch_dir, cfg_var)
    os.remove(os.path.join(output_dir,"SwappedA.bin"))

    print("Swap to original top swap block....")
    swap_ts_block(os.path.join(output_dir, "sbl_sec.bin"), os.path.join(output_dir, "SwappedA.bin"), 0x20000)
    shutil.copy(os.path.join(output_dir, "SwappedA.bin"), os.path.join(output_dir, "SlimBootloader.bin"))
    os.remove(os.path.join(output_dir,"SwappedA.bin"))

def gen_xml_file(stitch_dir, cfg_var, btg_profile, spi_quad, platform, tpm):

    print ("Generating xml file .........")

    if platform == 'whl':
        cmd = './fit -sku "CNP-LP Base U" -save new.xml'
    elif platform == 'cfls':
        cmd = './fit -sku Q370 -save new.xml'
    elif platform == 'cflh':
        cmd = './fit -sku QM370 -save new.xml'
    run_cmd (cmd, os.path.join(stitch_dir, cfg_var['fitinput']))

    tree = ET.parse(os.path.join(stitch_dir, cfg_var['fitinput'], 'new.xml'))

    xml_change_list = get_xml_change_list (platform, spi_quad)
    for each in xml_change_list:
        for xml_path, value in each:
            node = tree.find('%s' % xml_path)
            node.attrib['value'] = value

    if (btg_profile == 'vm') or (btg_profile == 'fve') or (btg_profile == 'fvme'):
        print ("Boot Guard is enabled.....")
        update_btGuard_manifests(stitch_dir, cfg_var)
        if btg_profile == 'vm':
            update_ifwi_xml(3, stitch_dir, cfg_var, tree)
        if btg_profile == 'fve':
            update_ifwi_xml(4, stitch_dir, cfg_var, tree)
        if btg_profile == 'fvme':
            update_ifwi_xml(5, stitch_dir, cfg_var, tree)

    update_tpm_type(tpm, tree)

    tree.write(os.path.join(stitch_dir, cfg_var['fitinput'], 'Platform.xml'))

def stitch (stitch_dir, stitch_zip, btg_profile, spi_quad_mode, platform_data, platform, tpm, full_rdundant = True):

    cfg_var    = get_config ()

    print ("\nUnpack files from stitching zip file ...")
    if not os.path.isabs(stitch_zip):
        stitch_zip = os.path.join(os.environ['SBL_SOURCE'], stitch_zip)

    zf = zipfile.ZipFile(stitch_zip, 'r', zipfile.ZIP_DEFLATED)
    zf.extractall(os.path.join(stitch_dir, cfg_var['fitinput']))
    zf.close()

    os.chdir(stitch_dir)

    if platform_data:
        fd = open(os.path.join(stitch_dir, cfg_var['fitinput'], "SlimBootloader.bin"), "rb")
        input_data = bytearray(fd.read())
        fd.close()
        print ("\n Adding platform data to Slimbootloader ...")
        data = add_platform_data(input_data, platform_data)
        fd = open(os.path.join(stitch_dir, cfg_var['fitinput'], "SlimBootloader.bin"), "wb")
        fd.write(data)
        fd.close()

    print ("\nChecking and copying components ...")
    copy_list = ['cseimg', 'pmc', 'gbe', 'ec', 'ecptr', 'acm', 'fit', 'vsccommn']
    for each in ['fit', 'openssl'] + copy_list:
        if not os.path.exists(cfg_var[each]):
             raise Exception ("Could not find file '%s' !" % cfg_var[each])

    for each in copy_list:
        shutil.copy (cfg_var[each], cfg_var['fitinput'])

    gen_xml_file(stitch_dir, cfg_var, btg_profile, spi_quad_mode, platform, tpm)

    print ("Run fit tool to generate ifwi.........")
    cmd = './fit -b -o Ifwi.bin -f Platform.xml'
    run_cmd (cmd, os.path.join(stitch_dir, cfg_var['fitinput']))

    return 0

def clean (stitch_dir, dist_mode):
    print ("Clean up workspace ...")

    cfg_var    = get_config ()
    for each in [cfg_var['wkspace']]:
        each = stitch_dir + '/' + each
        if os.path.exists(each):
            shutil.rmtree(each)

    fit_dir = stitch_dir + '/' + os.path.dirname(cfg_var['fit'])
    patterns = [
      fit_dir    + '/*.log',
      fit_dir    + '/*_sbl.xml',
    ]

    for pattern in patterns:
        for file in glob.glob(pattern):
            os.remove (file)

    return 0

def main():

    if len(sys.argv) == 1:
        print ('%s' % extra_usage_txt)

    hexstr = lambda x: int(x, 16)

    ap = argparse.ArgumentParser()
    ap.add_argument('-w',
                    '--stitch-work-dir',
                    dest='stitch_dir',
                    type=str,
                    required=True,
                    help='specify workspace directory for stitching')

    ap.add_argument('-s',
                    '--stitch-zip-file',
                    dest='stitch_zip',
                    type=str,
                    default='Outputs/cfl/Stitch_Components.zip',
                    help='specify input stitching zip package file path')

    ap.add_argument('-b',
                    '--boot-guard-profile',
                    default = 'vm',
                    choices=['legacy', 'vm', 'fve', 'fvme'],
                    dest='btg_profile',
                    help='specify Boot Guard profile type')

    ap.add_argument('-q',
                    '--spi-quad-mode',
                    dest='quad_mode',
                    action = "store_true",
                    default = False,
                    help = "enable SPI QUAD mode")

    ap.add_argument('-c',
                    '--clean',
                    dest='clean',
                    action = "store_true",
                    default = False,
                    help = "clean stitching workspace")

    ap.add_argument('-a',
                    '--platform',
                    default = 'whl',
                    choices=['whl', 'cfls', 'cflh'],
                    dest='platform',
                    help='specify platform')

    ap.add_argument('-p',
                    '--platform-data',
                    dest='plat_data',
                    type=hexstr,
                    default=None,
                    help='Specify a platform specific data (HEX, DWORD) for customization')

    ap.add_argument('-t',
                    '--tpm-type',
                    dest='tpm',
                    default = 'ptt',
                    choices=['ptt', 'dtpm', 'none'],
                    help='specify TPM type')

    args = ap.parse_args()

    stitch_dir = args.stitch_dir
    print ("Clean all temporary files.....")
    if clean (stitch_dir, args.clean):
        raise Exception ('Stitching clean up failed !')

    if args.clean:
        print ("Cleaning completed successfully !\n")
        return 0

    if args.btg_profile in ["vm","fvme"] and args.tpm == "none":
        print "ERROR: Choose appropriate Tpm type for BootGuard profile 3 and 5"
        return 0

    print ("Executing stitch.......")
    if stitch (stitch_dir, args.stitch_zip, args.btg_profile, args.quad_mode, args.plat_data, args.platform, args.tpm):
        raise Exception ('Stitching process failed !')

    cfg_var       = get_config ()
    ifwi_file_name = '%s/%s/%s_%s.bin' % (stitch_dir, cfg_var['wkspace'], cfg_var['ifwiname'], args.platform)
    ifwi_file_name = os.path.normpath (ifwi_file_name)

    shutil.copy (os.path.join(stitch_dir, cfg_var['fitinput'], 'Ifwi.bin'), ifwi_file_name)

    print ("\nIFWI Stitching completed successfully !")
    print ("Boot Guard Profile: %s" % args.btg_profile.upper())
    print ("IFWI image: %s\n" % ifwi_file_name)

    return 0


if __name__ == '__main__':
    sys.exit(main())


