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
       Assuming stitching workspace is at D:\Stitch
       To stitch IFWI with SPI QUAD mode and Boot Guard profile VM:
         python  Platform\CoffeelakeBoardPkg\Script\StitchIfwi.py -b vm -q -w D:\Stitch -s Stitch_Components.zip
       To clean all generated files:
         python  Platform\CoffeelakeBoardPkg\Script\StitchIfwi.py -c -w D:\Stitch

"""
def get_config ():
    # This dictionary defines path for stitching components
    cfg_var = {
      'wkspace'   :   'output',
      'fitinput'  :   'Output/input',
      'ifwiname'  :   'SBL_IFWI.bin',
      'fit'       :   'CSE/FIT/fit%s' %     ('.exe' if os.name == 'nt' else ''),
      'openssl'   :   'Openssl/openssl.exe' if os.name == 'nt' else '/usr/bin/openssl',
      'cseimg'    :   'CSE/Silicon/cse_image.bin',
      'acm'       :   'ACM/acm.bin',
      'pmc'       :   'PMC/pmc.bin',
      'ec'        :   'EC/ec.bin',
      'ecptr'     :   'EC/ecregionpointer.bin',
      'gbe'       :   'GBE/gbe.bin',
      'bpm'       :   'bpmgen2',
    }
    return cfg_var

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

    # if measured boot enabled, enable ptt
    if btguardprofile in [3, 5]:
        #PttConfiguration
        node = tree.find('./PlatformProtection/IntelPttConfiguration/PttSupported')
        node.attrib['value'] = 'Yes'
        node = tree.find('./PlatformProtection/IntelPttConfiguration/PttPwrUpState')
        node.attrib['value'] = 'Enabled'
        node = tree.find('./PlatformProtection/IntelPttConfiguration/PttSupportedFpf')
        node.attrib['value'] = 'Yes'

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

def gen_xml_file(stitch_dir, cfg_var, btg_profile, spi_quad):

    print ("Generating xml file .........")

    cmd = './fit -save new.xml'
    run_cmd (cmd, os.path.join(stitch_dir, cfg_var['fitinput']))

    tree = ET.parse(os.path.join(stitch_dir, cfg_var['fitinput'], 'new.xml'))

    #Region Order
    node = tree.find('./BuildSettings/BuildResults/RegionOrder')
    node.attrib['value'] = '45321'

    # Enable BIOS region
    node = tree.find('./FlashLayout/BiosRegion/InputFile')
    node.attrib['value'] = '$SourceDir\Slimbootloader.bin'

    # Enable BIOS region
    node = tree.find('./FlashLayout/Ifwi_IntelMePmcRegion/MeRegionFile')
    node.attrib['value'] = '$SourceDir\cse_image.bin'
    node = tree.find('./FlashLayout/Ifwi_IntelMePmcRegion/PmcBinary')
    node.attrib['value'] = '$SourceDir\pmc.bin'

    # Enable EC region
    node = tree.find('./FlashLayout/EcRegion/InputFile')
    node.attrib['value'] = '$SourceDir\ec.bin'
    node = tree.find('./FlashLayout/EcRegion/Enabled')
    node.attrib['value'] = 'Enabled'
    node = tree.find('./FlashLayout/EcRegion/EcRegionPointer')
    node.attrib['value'] = '$SourceDir\ecregionpointer.bin'

    # Enable GBE region
    node = tree.find('./FlashLayout/GbeRegion/Enabled')
    node.attrib['value'] = 'Enabled'
    node = tree.find('./FlashLayout/GbeRegion/InputFile')
    node.attrib['value'] = '$SourceDir\gbe.bin'

    # Disable Quad Io and Out read enable
    if spi_quad:
        value = 'Yes'
    else:
        value = 'No'
    node = tree.find('./FlashSettings/FlashConfiguration/QuadIoReadEnable')
    node.attrib['value'] = value
    node = tree.find('./FlashSettings/FlashConfiguration/QuadOutReadEnable')
    node.attrib['value'] = value

    #VsccTable
    node = tree.find('./FlashSettings/VsccTable/VsccEntries/VsccEntry/VsccEntryName')
    node.attrib['value'] = 'vscc_entry0'
    node = tree.find('./FlashSettings/VsccTable/VsccEntries/VsccEntry/VsccEntryVendorId')
    node.attrib['value'] = '0xEF'
    node = tree.find('./FlashSettings/VsccTable/VsccEntries/VsccEntry/VsccEntryDeviceId0')
    node.attrib['value'] = '0x40'
    node = tree.find('./FlashSettings/VsccTable/VsccEntries/VsccEntry/VsccEntryDeviceId1')
    node.attrib['value'] = '0x19'

    #IntelMekernel
    node = tree.find('./IntelMeKernel/Processor/ProcEmulation')
    node.attrib['value'] = 'EMULATE Intel (R) vPro (TM) capable Processor'

    #PttConfiguration
    node = tree.find('./PlatformProtection/IntelPttConfiguration/PttSupported')
    node.attrib['value'] = 'No'
    node = tree.find('./PlatformProtection/IntelPttConfiguration/PttPwrUpState')
    node.attrib['value'] = 'Disabled'
    node = tree.find('./PlatformProtection/IntelPttConfiguration/PttSupportedFpf')
    node.attrib['value'] = 'No'

    #BootGuardConfiguration
    node = tree.find('./PlatformProtection/BiosGuardConfiguration/BiosGrdProtOvrdEn')
    node.attrib['value'] = 'No'

    #ICC
    node = tree.find('./Icc/IccPolicies/Profiles/Profile/ClockOutputConfiguration/ClkoutCpunsscPnPath')
    node.attrib['value'] = 'Direct XTAL IN / Out Path'

    #Networking
    node = tree.find('./NetworkingConnectivity/WiredLanConfiguration/PhyConnected')
    node.attrib['value'] = 'PHY on SMLink0'

    #SMBUS
    node = tree.find('./InternalPchBuses/SmbusSmlinkConfiguration/SLink1freq')
    node.attrib['value'] = '1 MHz'

    #ISH
    node = tree.find('./IntegratedSensorHub/IshSupported')
    node.attrib['value'] = 'No'

    #CPU Straps
    node = tree.find('./CpuStraps/IaPowerPlaneTopology')
    node.attrib['value'] = '0x00000000'
    node = tree.find('./CpuStraps/RingPowerPlaneTopology')
    node.attrib['value'] = '0x00000000'
    node = tree.find('./CpuStraps/GtUsPowerPlaneTopology')
    node.attrib['value'] = '0x00000001'
    node = tree.find('./CpuStraps/GtSPowerPlaneTopology')
    node.attrib['value'] = '0x00000001'

    #StrapsDifferences
    node = tree.find('./StrapsDifferences/PCH_Strap_SPI_touch2_max_freq_Diff')
    node.attrib['value'] = '0x03'
    node = tree.find('./StrapsDifferences/PCH_Strap_PN0_RPCFG_0_Diff')
    node.attrib['value'] = '0x03'
    node = tree.find('./StrapsDifferences/PCH_Strap_PN1_RPCFG_0_Diff')
    node.attrib['value'] = '0x03'
    node = tree.find('./StrapsDifferences/PCH_Strap_PN2_RPCFG_0_Diff')
    node.attrib['value'] = '0x00'
    node = tree.find('./StrapsDifferences/PCH_Strap_PN3_RPCFG_0_Diff')
    node.attrib['value'] = '0x03'
    node = tree.find('./StrapsDifferences/PCH_Strap_EXI_PTSS_PORT3_Diff')
    node.attrib['value'] = '0x01'
    node = tree.find('./StrapsDifferences/PCH_Strap_EXI_PTSS_PORT1_Diff')
    node.attrib['value'] = '0x01'
    node = tree.find('./StrapsDifferences/PCH_Strap_GBE_SMLink1_Frequency_Diff')
    node.attrib['value'] = '0x00'
    node = tree.find('./StrapsDifferences/PCH_Strap_GBE_Reserved3_Diff')
    node.attrib['value'] = '0x0F'
    node = tree.find('./StrapsDifferences/CPU_Strap_SEPARATE_VCCAGSH_EXISTS_Diff')
    node.attrib['value'] = '0x00'

    #FlexIO
    node = tree.find('./FlexIO/PciePortConfiguration/PCIeContoller2Config')
    node.attrib['value'] = '1x4'
    node = tree.find('./FlexIO/SataPcieComboPortConfiguration/SataPCIeComboPort0')
    node.attrib['value'] = 'PCIe'
    node = tree.find('./FlexIO/SataPcieComboPortConfiguration/SataPCIeComboPort2')
    node.attrib['value'] = 'PCIe'
    node = tree.find('./FlexIO/Usb3PortConfiguration/USB3Prt2ConTypeSel')
    node.attrib['value'] = 'Type A'
    node = tree.find('./FlexIO/Usb3PortConfiguration/USB3Prt3ConTypeSel')
    node.attrib['value'] = 'Type A'
    node = tree.find('./FlexIO/Usb2PortConfiguration/USB2Prt2ConTypeSel')
    node.attrib['value'] = 'Type C'
    node = tree.find('./FlexIO/Usb2PortConfiguration/USB2Prt3ConTypeSel')
    node.attrib['value'] = 'Type A'
    node = tree.find('./FlexIO/Usb2PortConfiguration/USB2Prt5ConTypeSel')
    node.attrib['value'] = 'Type A'

    #GPIO
    node = tree.find('./Gpio/GpioVccioVoltageControl/GppA7voltSelect')
    node.attrib['value'] = '3.3Volts'
    node = tree.find('./Gpio/GpioVccioVoltageControl/GppA8voltSelect')
    node.attrib['value'] = '3.3Volts'
    node = tree.find('./Gpio/GpioVccioVoltageControl/GppA16voltSelect')
    node.attrib['value'] = '3.3Volts'
    node = tree.find('./Gpio/GpioVccioVoltageControl/GppA21voltSelect')
    node.attrib['value'] = '3.3Volts'
    node = tree.find('./Gpio/GpioVccioVoltageControl/GppC11voltSelect')
    node.attrib['value'] = '3.3Volts'
    node = tree.find('./Gpio/GpioVccioVoltageControl/GppD9voltSelect')
    node.attrib['value'] = '1.8Volts'
    node = tree.find('./Gpio/GpioVccioVoltageControl/GppH12voltSelect')
    node.attrib['value'] = '3.3Volts'
    node = tree.find('./Gpio/GpioVccioVoltageControl/GppH14voltSelect')
    node.attrib['value'] = '3.3Volts'
    node = tree.find('./Gpio/GpioVccioVoltageControl/GppH16voltSelect')
    node.attrib['value'] = '1.8Volts'
    node = tree.find('./Gpio/GpioVccioVoltageControl/GppH17voltSelect')
    node.attrib['value'] = '1.8Volts'
    node = tree.find('./Gpio/GpioVccioVoltageControl/GppH20voltSelect')
    node.attrib['value'] = '3.3Volts'
    node = tree.find('./Gpio/GpioVccioVoltageControl/GppH22voltSelect')
    node.attrib['value'] = '3.3Volts'

    if (btg_profile == 'vm') or (btg_profile == 'fve') or (btg_profile == 'fvme'):
        print ("Boot Guard is enabled.....")
        update_btGuard_manifests(stitch_dir, cfg_var)
        if btg_profile == 'vm':
            update_ifwi_xml(3, stitch_dir, cfg_var, tree)
        if btg_profile == 'fve':
            update_ifwi_xml(4, stitch_dir, cfg_var, tree)
        if btg_profile == 'fvme':
            update_ifwi_xml(5, stitch_dir, cfg_var, tree)

    tree.write(os.path.join(stitch_dir, cfg_var['fitinput'], 'Platform.xml'))

def stitch (stitch_dir, stitch_zip, btg_profile, spi_quad_mode, platform_data, full_rdundant = True):

    cfg_var    = get_config ()

    print ("\nUnpack files from stitching zip file ...")
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
    copy_list = ['cseimg', 'pmc', 'gbe', 'ec', 'ecptr', 'acm', 'fit']
    for each in ['fit', 'openssl'] + copy_list:
        if not os.path.exists(cfg_var[each]):
             raise Exception ("Could not find file '%s' !" % cfg_var[each])

    for each in copy_list:
        shutil.copy (cfg_var[each], cfg_var['fitinput'])

    fit_dir = stitch_dir + '/' + os.path.dirname(cfg_var['fit'])
    shutil.copy (os.path.join(fit_dir, 'vsccommn.bin'), cfg_var['fitinput'])

    gen_xml_file(stitch_dir, cfg_var, btg_profile, spi_quad_mode)

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

    ap.add_argument('-p',
                    '--platform-data',
                    dest='plat_data',
                    type=hexstr,
                    default=None,
                    help='Specify a platform specific data (HEX, DWORD) for customization')

    args = ap.parse_args()

    stitch_dir = args.stitch_dir
    print ("Clean all temporary files.....")
    if clean (stitch_dir, args.clean):
        raise Exception ('Stitching clean up failed !')

    if args.clean:
        print ("Cleaning completed successfully !\n")
        return 0

    print ("Executing stitch.......")
    if stitch (stitch_dir, args.stitch_zip, args.btg_profile, args.quad_mode, args.plat_data):
        raise Exception ('Stitching process failed !')

    cfg_var       = get_config ()
    ifwi_file_name = '%s/%s/%s' % (stitch_dir, cfg_var['wkspace'], cfg_var['ifwiname'])
    ifwi_file_name = os.path.normpath (ifwi_file_name)

    shutil.copy (os.path.join(stitch_dir, cfg_var['fitinput'], 'Ifwi.bin'), os.path.join(stitch_dir, cfg_var['wkspace'], '%s' % cfg_var['ifwiname']))

    print ("\nIFWI Stitching completed successfully !")
    print ("Boot Guard Profile: %s" % args.btg_profile.upper())
    print ("IFWI image: %s\n" % ifwi_file_name)

    return 0


if __name__ == '__main__':
    sys.exit(main())


