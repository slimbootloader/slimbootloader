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
from   StitchLoader import AddPlatformData

BTG_Profile_Values = [\
                    "Boot Guard Profile 0 - No_FVME",\
                    "Not Applicable",\
                    "Not Applicable",\
                    "Boot Guard Profile 3 - VM",\
                    "Boot Guard Profile 4 - FVE",\
                    "Boot Guard Profile 5 - FVME"]

ExtraUsageTxt = """
This script creates a new Whiskey Lake/Coffee Lake Slim Bootloader IFWI image using FIT
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

def GetConfig ():
    # This dictionary defines path for stitching components
    CfgVar = {
      'wkspace'   :   'Output',
      'fitinput'  :   'Output/input',
      'ifwiname'  :   'SBL_IFWI.bin',
      'fit'       :   'CSE/FIT/fit%s' %     ('.exe' if os.name == 'nt' else ''),
      'fithelp'   :   'CSE/FIT/FitHelp.py',
      'openssl'   :   'Openssl/openssl.exe' if os.name == 'nt' else '/usr/bin/openssl',
      'cseimg'    :   'CSE/Silicon/cse_image.bin',
      'acm'       :   'ACM/acm.bin',
      'pmc'       :   'PMC/pmc.bin',
      'ec'        :   'EC/ec.bin',
      'ecptr'     :   'EC/ecregionpointer.bin',
      'gbe'       :   'GBE/gbe.bin',
      'bpm'       :   'bpmgen2',
    }
    return CfgVar

def RunCmd (Cmd, Cwd):
    sys.stdout.flush()
    try:
        CmdArgs = shlex.split(Cmd)
        if os.name == 'nt':
            Shell = True
            if CmdArgs[0].startswith('./'):
                CmdArgs[0] = CmdArgs[0][2:]
        else:
            Shell = False
        Proc = subprocess.Popen(CmdArgs, shell = Shell, cwd=Cwd)
        Out, Err = Proc.communicate()
        Ret = Proc.returncode
    except Exception as e:
        Out, Err = '', e
        Ret = 1
    if Ret:
        sys.stdout.flush()
        print('%s\n%s' % (Out, Err))
        print("Failed to run command:\n  PATH: %s\n   CMD: %s" % (Cwd, Cmd))
        sys.exit(1)

def SwapTSBlock(InFile, OutFile, TsSize):
    print("Swapping Top Swap Blocks....")

    if not os.path.exists(InFile):
        raise Exception("%s not found !" % InFile)
        return 1

    InFileSize = os.path.getsize(InFile)

    if TsSize > InFileSize:
        raise Exception("TsSize %x is greater than the file size !" % TsSize)
        return 1

    InFileptr = open(InFile, 'rb')
    InFileData = bytearray(InFileptr.read())
    InFileptr.close()

    WriteData = bytearray(0)
    OutFilePtr = open(OutFile, 'wb')
    BlockData = InFileData[0:(InFileSize-(2*TsSize))]
    BlockData += InFileData[(InFileSize-TsSize):InFileSize]
    BlockData += InFileData[(InFileSize-(2*TsSize)):(InFileSize-TsSize)]
    OutFilePtr.write(BlockData)
    OutFilePtr.close()

def SignBinary(Infile, StitchDir, CfgVar):
    OutputDir = os.path.join(StitchDir,"Output", "input")
    shutil.copy(Infile, os.path.join(OutputDir,"sbl_sec_temp.bin"))

    print("Generating new keys....")

    BpmGen2Dir = StitchDir + '/' + 'bpmgen2'
    if not os.path.exists(os.path.join(StitchDir,"bpmgen2", "keys")):
        os.mkdir(os.path.join(BpmGen2Dir, "keys"))

        Cmd = './openssl genrsa -F4 -out keys/keyprivkey.pem 2048'
        RunCmd (Cmd, BpmGen2Dir)

        Cmd = './openssl rsa -pubout -in keys/keyprivkey.pem -out keys/keypubkey.pem'
        RunCmd (Cmd, BpmGen2Dir)

        Cmd = './openssl genrsa -F4 -out keys/privkey.pem 2048'
        RunCmd (Cmd, BpmGen2Dir)

        Cmd = './openssl rsa -pubout -in keys/privkey.pem -out keys/pubkey.pem'
        RunCmd (Cmd, BpmGen2Dir)

    Cmd = 'python CSE/FIT/FitHelp.py "FITACM" Output/input/sbl_sec_temp.bin Output/input/acm.bin'
    RunCmd (Cmd, StitchDir)

    print("Generating KeyManifest.bin....")
    Cmd = './bpmgen2 KM1GEN -KEY keys/pubkey.pem BPM -KM ../output/input/KeyManifest.bin -SIGNKEY keys/keyprivkey.pem -SIGNPUBKEY keys/keypubkey.pem -KMID 0x01 -SVN 0 -d:2 > ../output/input/bpmgen2_km.txt'
    RunCmd (Cmd, BpmGen2Dir)

    print("Generating Manifest.bin....")
    Cmd = './bpmgen2 GEN ../output/input/sbl_sec_temp.bin bpmgen2.params -BPM ../output/input/Manifest.bin -U ../output/input/sbl_sec.bin -KM ../output/input/KeyManifest.bin -d:2 > ../output/input/bpmgen2_bpm.txt'
    RunCmd (Cmd, BpmGen2Dir)

    print("Calculate public key hash....")
    print("Extract Public key")
    Cmd = './openssl rsa -in %s -pubin -modulus > pubkey.txt' % ('../../bpmgen2/keys/keypubkey.pem')
    RunCmd (Cmd, OutputDir)

    line=open('output/input/pubkey'+'.txt','r').readline()[8:8+256*2]
    if sys.hexversion >= 0x3000000:
        keybin = bytearray.fromhex(line)
    else:
        keybin = line.decode('hex')
    open('output/input/pubkey'+'.bin','wb').write(keybin[::-1])
    Cmd = './openssl dgst -sha256 -binary -out pubkey.hash pubkey.bin'
    RunCmd (Cmd, OutputDir)
    print("printing hash")
    hash = bytearray(open('output/input/pubkey'+'.hash','rb').read())
    print(''.join('%02X ' % b for b in hash))

def UpdateIfwiXml(Btguardprofile, StitchDir, CfgVar, Tree):
    OutputDir = os.path.join(StitchDir,"Output", "input")

    Openssl             = os.path.join(OutputDir, "openssl.exe")
    kmsigpubkeytxtfile  = os.path.join(OutputDir, "kmsigpubkey.txt")
    kmsigpubkeybinfile  = os.path.join(OutputDir, "kmsigpubkey.bin")
    kmsigpubkeyhashfile = os.path.join(OutputDir, "kmsigpubkey.hash")

    with open(kmsigpubkeytxtfile, "w") as kmsigpubkeytxt_fh:
        Cmd = './openssl rsa -in ../../bpmgen2/keys/keypubkey.pem -pubin -modulus > kmsigpubkeytxt_fh'
        RunCmd (Cmd, OutputDir)

    shutil.copy(os.path.join(OutputDir, "kmsigpubkeytxt_fh"), kmsigpubkeytxtfile)

    line=open(kmsigpubkeytxtfile,"r").readline()[8:8+256*2]
    if sys.hexversion >= 0x3000000:
        keybin = bytearray.fromhex(line)
    else:
        keybin = line.decode('hex')
    open(kmsigpubkeybinfile,"wb").write(keybin[::-1])

    # In, CFL and  future platforms, public exponent (i.e. 65537) is concatenated for OEM key hash calculation.
    line=open(kmsigpubkeytxtfile,"r").readline()[8:8+256*2]
    open(kmsigpubkeybinfile,"ab").write(bytearray.fromhex('01000100'))
    Cmd = './openssl dgst -sha256 -binary -out Output/input/kmsigpubkey.hash Output/input/kmsigpubkey.bin'
    RunCmd (Cmd, StitchDir)

    # Convert OEM key hash into hexadecimal for Fit tool consumption
    with open(kmsigpubkeyhashfile,"rb") as kmsigpubkeyhash_fh:
        hash = bytearray (kmsigpubkeyhash_fh.read())
        oemkeyhash = ""
        for b in hash:
            oemkeyhash = oemkeyhash + "%02X " % b
    print(oemkeyhash)

    # if measured boot enabled, enable ptt
    if Btguardprofile in [3, 5]:
        #PttConfiguration
        Node = Tree.find('./PlatformProtection/IntelPttConfiguration/PttSupported')
        Node.attrib['value'] = 'Yes'
        Node = Tree.find('./PlatformProtection/IntelPttConfiguration/PttPwrUpState')
        Node.attrib['value'] = 'Enabled'
        Node = Tree.find('./PlatformProtection/IntelPttConfiguration/PttSupportedFpf')
        Node.attrib['value'] = 'Yes'

    Node = Tree.find('./PlatformProtection/BootGuardConfiguration/BtGuardKeyManifestId')
    Node.attrib['value'] = '0x1'

    Node = Tree.find('./PlatformProtection/PlatformIntegrity/OemPublicKeyHash')
    Node.attrib['value'] = oemkeyhash

    Node = Tree.find('./PlatformProtection/BootGuardConfiguration/BtGuardProfileConfig')
    Node.attrib['value'] = BTG_Profile_Values[Btguardprofile]

def UpdateBtGuardManifests(StitchDir, CfgVar):
    print("Sigining Coffeelake....")

    OutputDir = os.path.join(StitchDir,"Output", "input")

    print("Sign primary partition....")
    SignBinary(os.path.join(OutputDir,"SlimBootloader.bin"), StitchDir, CfgVar)

    print("Swap top swap block....")
    SwapTSBlock(os.path.join(OutputDir, "sbl_sec.bin"), os.path.join(OutputDir, "SwappedA.bin"), 0x20000)

    print("Sign backup partition....")
    SignBinary(os.path.join(OutputDir,"SwappedA.bin"), StitchDir, CfgVar)
    os.remove(os.path.join(OutputDir,"SwappedA.bin"))

    print("Swap to original top swap block....")
    SwapTSBlock(os.path.join(OutputDir, "sbl_sec.bin"), os.path.join(OutputDir, "SwappedA.bin"), 0x20000)
    shutil.copy(os.path.join(OutputDir, "SwappedA.bin"), os.path.join(OutputDir, "SlimBootloader.bin"))
    os.remove(os.path.join(OutputDir,"SwappedA.bin"))

def GenXmlFile(StitchDir, CfgVar, BtgProfile, SpiQuad):

    print ("Generating xml file .........")

    Cmd = './fit -save new.xml'
    RunCmd (Cmd, os.path.join(StitchDir, CfgVar['fitinput']))

    Tree = ET.parse(os.path.join(StitchDir, CfgVar['fitinput'], 'new.xml'))

    #Region Order
    Node = Tree.find('./BuildSettings/BuildResults/RegionOrder')
    Node.attrib['value'] = '45321'

    # Enable BIOS region
    #Node = Tree.find('./FlashLayout/BiosRegion/Enabled')
    #Node.attrib['value'] = 'Enabled'
    Node = Tree.find('./FlashLayout/BiosRegion/InputFile')
    Node.attrib['value'] = '$SourceDir\Slimbootloader.bin'

    # Enable BIOS region
    Node = Tree.find('./FlashLayout/Ifwi_IntelMePmcRegion/MeRegionFile')
    Node.attrib['value'] = '$SourceDir\cse_image.bin'
    Node = Tree.find('./FlashLayout/Ifwi_IntelMePmcRegion/PmcBinary')
    Node.attrib['value'] = '$SourceDir\pmc.bin'

    # Enable EC region
    Node = Tree.find('./FlashLayout/EcRegion/InputFile')
    Node.attrib['value'] = '$SourceDir\ec.bin'
    Node = Tree.find('./FlashLayout/EcRegion/Enabled')
    Node.attrib['value'] = 'Enabled'
    Node = Tree.find('./FlashLayout/EcRegion/EcRegionPointer')
    Node.attrib['value'] = '$SourceDir\ecregionpointer.bin'

    # Enable GBE region
    Node = Tree.find('./FlashLayout/GbeRegion/Enabled')
    Node.attrib['value'] = 'Enabled'
    Node = Tree.find('./FlashLayout/GbeRegion/InputFile')
    Node.attrib['value'] = '$SourceDir\gbe.bin'

    # Disable Quad Io and Out read enable
    if SpiQuad:
        Value = 'Yes'
    else:
        Value = 'No'
    Node = Tree.find('./FlashSettings/FlashConfiguration/QuadIoReadEnable')
    Node.attrib['value'] = Value
    Node = Tree.find('./FlashSettings/FlashConfiguration/QuadOutReadEnable')
    Node.attrib['value'] = Value

    #VsccTable
    Node = Tree.find('./FlashSettings/VsccTable/VsccEntries/VsccEntry/VsccEntryName')
    Node.attrib['value'] = 'VsccEntry0'
    Node = Tree.find('./FlashSettings/VsccTable/VsccEntries/VsccEntry/VsccEntryVendorId')
    Node.attrib['value'] = '0xEF'
    Node = Tree.find('./FlashSettings/VsccTable/VsccEntries/VsccEntry/VsccEntryDeviceId0')
    Node.attrib['value'] = '0x40'
    Node = Tree.find('./FlashSettings/VsccTable/VsccEntries/VsccEntry/VsccEntryDeviceId1')
    Node.attrib['value'] = '0x19'

    #IntelMekernel
    Node = Tree.find('./IntelMeKernel/Processor/ProcEmulation')
    Node.attrib['value'] = 'EMULATE Intel (R) vPro (TM) capable Processor'

    #PttConfiguration
    Node = Tree.find('./PlatformProtection/IntelPttConfiguration/PttSupported')
    Node.attrib['value'] = 'No'
    Node = Tree.find('./PlatformProtection/IntelPttConfiguration/PttPwrUpState')
    Node.attrib['value'] = 'Disabled'
    Node = Tree.find('./PlatformProtection/IntelPttConfiguration/PttSupportedFpf')
    Node.attrib['value'] = 'No'

    #BootGuardConfiguration
    Node = Tree.find('./PlatformProtection/BiosGuardConfiguration/BiosGrdProtOvrdEn')
    Node.attrib['value'] = 'No'

    #ICC
    Node = Tree.find('./Icc/IccPolicies/Profiles/Profile/ClockOutputConfiguration/ClkoutCpunsscPnPath')
    Node.attrib['value'] = 'Direct XTAL IN / Out Path'

    #Networking
    Node = Tree.find('./NetworkingConnectivity/WiredLanConfiguration/PhyConnected')
    Node.attrib['value'] = 'PHY on SMLink0'

    #SMBUS
    Node = Tree.find('./InternalPchBuses/SmbusSmlinkConfiguration/SLink1freq')
    Node.attrib['value'] = '1 MHz'

    #ISH
    Node = Tree.find('./IntegratedSensorHub/IshSupported')
    Node.attrib['value'] = 'No'

    #CPU Straps
    Node = Tree.find('./CpuStraps/IaPowerPlaneTopology')
    Node.attrib['value'] = '0x00000000'
    Node = Tree.find('./CpuStraps/RingPowerPlaneTopology')
    Node.attrib['value'] = '0x00000000'
    Node = Tree.find('./CpuStraps/GtUsPowerPlaneTopology')
    Node.attrib['value'] = '0x00000001'
    Node = Tree.find('./CpuStraps/GtSPowerPlaneTopology')
    Node.attrib['value'] = '0x00000001'

    #StrapsDifferences
    Node = Tree.find('./StrapsDifferences/PCH_Strap_SPI_touch2_max_freq_Diff')
    Node.attrib['value'] = '0x03'
    Node = Tree.find('./StrapsDifferences/PCH_Strap_PN0_RPCFG_0_Diff')
    Node.attrib['value'] = '0x03'
    Node = Tree.find('./StrapsDifferences/PCH_Strap_PN1_RPCFG_0_Diff')
    Node.attrib['value'] = '0x03'
    Node = Tree.find('./StrapsDifferences/PCH_Strap_PN2_RPCFG_0_Diff')
    Node.attrib['value'] = '0x00'
    Node = Tree.find('./StrapsDifferences/PCH_Strap_PN3_RPCFG_0_Diff')
    Node.attrib['value'] = '0x03'
    Node = Tree.find('./StrapsDifferences/PCH_Strap_EXI_PTSS_PORT3_Diff')
    Node.attrib['value'] = '0x01'
    Node = Tree.find('./StrapsDifferences/PCH_Strap_EXI_PTSS_PORT1_Diff')
    Node.attrib['value'] = '0x01'
    Node = Tree.find('./StrapsDifferences/PCH_Strap_GBE_SMLink1_Frequency_Diff')
    Node.attrib['value'] = '0x00'
    Node = Tree.find('./StrapsDifferences/PCH_Strap_GBE_Reserved3_Diff')
    Node.attrib['value'] = '0x0F'
    Node = Tree.find('./StrapsDifferences/CPU_Strap_SEPARATE_VCCAGSH_EXISTS_Diff')
    Node.attrib['value'] = '0x00'

    #FlexIO
    Node = Tree.find('./FlexIO/PciePortConfiguration/PCIeContoller2Config')
    Node.attrib['value'] = '1x4'
    Node = Tree.find('./FlexIO/SataPcieComboPortConfiguration/SataPCIeComboPort0')
    Node.attrib['value'] = 'PCIe'
    Node = Tree.find('./FlexIO/SataPcieComboPortConfiguration/SataPCIeComboPort2')
    Node.attrib['value'] = 'PCIe'
    Node = Tree.find('./FlexIO/Usb3PortConfiguration/USB3Prt2ConTypeSel')
    Node.attrib['value'] = 'Type A'
    Node = Tree.find('./FlexIO/Usb3PortConfiguration/USB3Prt3ConTypeSel')
    Node.attrib['value'] = 'Type A'
    Node = Tree.find('./FlexIO/Usb2PortConfiguration/USB2Prt2ConTypeSel')
    Node.attrib['value'] = 'Type C'
    Node = Tree.find('./FlexIO/Usb2PortConfiguration/USB2Prt3ConTypeSel')
    Node.attrib['value'] = 'Type A'
    Node = Tree.find('./FlexIO/Usb2PortConfiguration/USB2Prt5ConTypeSel')
    Node.attrib['value'] = 'Type A'

    #GPIO
    Node = Tree.find('./Gpio/GpioVccioVoltageControl/GppA7voltSelect')
    Node.attrib['value'] = '3.3Volts'
    Node = Tree.find('./Gpio/GpioVccioVoltageControl/GppA8voltSelect')
    Node.attrib['value'] = '3.3Volts'
    Node = Tree.find('./Gpio/GpioVccioVoltageControl/GppA16voltSelect')
    Node.attrib['value'] = '3.3Volts'
    Node = Tree.find('./Gpio/GpioVccioVoltageControl/GppA21voltSelect')
    Node.attrib['value'] = '3.3Volts'
    Node = Tree.find('./Gpio/GpioVccioVoltageControl/GppC11voltSelect')
    Node.attrib['value'] = '3.3Volts'
    Node = Tree.find('./Gpio/GpioVccioVoltageControl/GppD9voltSelect')
    Node.attrib['value'] = '1.8Volts'
    Node = Tree.find('./Gpio/GpioVccioVoltageControl/GppH12voltSelect')
    Node.attrib['value'] = '3.3Volts'
    Node = Tree.find('./Gpio/GpioVccioVoltageControl/GppH14voltSelect')
    Node.attrib['value'] = '3.3Volts'
    Node = Tree.find('./Gpio/GpioVccioVoltageControl/GppH16voltSelect')
    Node.attrib['value'] = '1.8Volts'
    Node = Tree.find('./Gpio/GpioVccioVoltageControl/GppH17voltSelect')
    Node.attrib['value'] = '1.8Volts'
    Node = Tree.find('./Gpio/GpioVccioVoltageControl/GppH20voltSelect')
    Node.attrib['value'] = '3.3Volts'
    Node = Tree.find('./Gpio/GpioVccioVoltageControl/GppH22voltSelect')
    Node.attrib['value'] = '3.3Volts'

    if (BtgProfile == 'vm') or (BtgProfile == 'fve') or (BtgProfile == 'fvme'):
        print ("Boot Guard is enabled.....")
        UpdateBtGuardManifests(StitchDir, CfgVar)
        if BtgProfile == 'vm':
            UpdateIfwiXml(3, StitchDir, CfgVar, Tree)
        if BtgProfile == 'fve':
            UpdateIfwiXml(4, StitchDir, CfgVar, Tree)
        if BtgProfile == 'fvme':
            UpdateIfwiXml(5, StitchDir, CfgVar, Tree)

    Tree.write(os.path.join(StitchDir, CfgVar['fitinput'], 'Platform.xml'))

def Stitch (StitchDir, StitchZip, BtgProfile, SpiQuadMode, PlatformData, FullRdundant = True):

    CfgVar    = GetConfig ()

    print ("\nUnpack files from stitching zip file ...")
    Zf = zipfile.ZipFile(StitchZip, 'r', zipfile.ZIP_DEFLATED)
    Zf.extractall(os.path.join(StitchDir, CfgVar['fitinput']))
    Zf.close()

    os.chdir(StitchDir)

    if PlatformData:
        Fd = open(os.path.join(StitchDir, CfgVar['fitinput'], "SlimBootloader.bin"), "rb")
        InputData = bytearray(Fd.read())
        Fd.close()
        print ("\n Adding platform data to Slimbootloader ...")
        Data = AddPlatformData(InputData, PlatformData)
        Fd = open(os.path.join(StitchDir, CfgVar['fitinput'], "SlimBootloader.bin"), "wb")
        Fd.write(Data)
        Fd.close()

    print ("\nChecking and copying components ...")
    CopyList = ['cseimg', 'pmc', 'gbe', 'ec', 'ecptr', 'acm', 'fit']
    for Each in ['fit', 'openssl', 'fithelp'] + CopyList:
        if not os.path.exists(CfgVar[Each]):
             raise Exception ("Could not find file '%s' !" % CfgVar[Each])

    for Each in CopyList:
        shutil.copy (CfgVar[Each], CfgVar['fitinput'])

    FitDir = StitchDir + '/' + os.path.dirname(CfgVar['fit'])
    shutil.copy (os.path.join(FitDir, 'vsccommn.bin'), CfgVar['fitinput'])

    GenXmlFile(StitchDir, CfgVar, BtgProfile, SpiQuadMode)

    print ("Run fit tool to generate ifwi.........")
    Cmd = './fit -b -o Ifwi.bin -f Platform.xml'
    RunCmd (Cmd, os.path.join(StitchDir, CfgVar['fitinput']))

    return 0

def Clean (StitchDir, DistMode):
    print ("Clean up workspace ...")

    CfgVar    = GetConfig ()
    for Each in [CfgVar['wkspace']]:
        Each = StitchDir + '/' + Each
        if os.path.exists(Each):
            shutil.rmtree(Each)

    FitDir = StitchDir + '/' + os.path.dirname(CfgVar['fit'])
    Patterns = [
      FitDir    + '/*.log',
      FitDir    + '/*_sbl.xml',
    ]

    for Pattern in Patterns:
        for File in glob.glob(Pattern):
            os.remove (File)

    return 0

def main():

    if len(sys.argv) == 1:
        print ('%s' % ExtraUsageTxt)

    hexstr = lambda x: int(x, 16)

    Ap = argparse.ArgumentParser()
    Ap.add_argument('-w',
                    '--stitch-work-dir',
                    dest='stitch_dir',
                    type=str,
                    required=True,
                    help='specify workspace directory for stitching')

    Ap.add_argument('-s',
                    '--stitch-zip-file',
                    dest='stitch_zip',
                    type=str,
                    default='Outputs/apl/Stitch_Components.zip',
                    help='specify input stitching zip package file path')

    Ap.add_argument('-b',
                    '--boot-guard-profile',
                    default = 'vm',
                    choices=['legacy', 'vm', 'fve', 'fvme'],
                    dest='btg_profile',
                    help='specify Boot Guard profile type')

    Ap.add_argument('-q',
                    '--spi-quad-mode',
                    dest='quad_mode',
                    action = "store_true",
                    default = False,
                    help = "enable SPI QUAD mode")

    Ap.add_argument('-c',
                    '--clean',
                    dest='clean',
                    action = "store_true",
                    default = False,
                    help = "clean stitching workspace")

    Ap.add_argument('-p',
                    '--platform-data',
                    dest='plat_data',
                    type=hexstr,
                    default=None,
                    help='Specify a platform specific data (HEX, DWORD) for customization')

    Args = Ap.parse_args()

    StitchDir = Args.stitch_dir
    print ("Clean all temporary files.....")
    if Clean (StitchDir, Args.clean):
        raise Exception ('Stitching clean up failed !')

    if Args.clean:
        print ("Cleaning completed successfully !\n")
        return 0

    print ("Executing stitch.......")
    if Stitch (StitchDir, Args.stitch_zip, Args.btg_profile, Args.quad_mode, Args.plat_data):
        raise Exception ('Stitching process failed !')

    CfgVar       = GetConfig ()
    IfwiFileName = '%s/%s/%s' % (StitchDir, CfgVar['wkspace'], CfgVar['ifwiname'])
    IfwiFileName = os.path.normpath (IfwiFileName)

    shutil.copy (os.path.join(StitchDir, CfgVar['fitinput'], 'Ifwi.bin'), os.path.join(StitchDir, CfgVar['wkspace'], '%s' % CfgVar['ifwiname']))

    print ("\nIFWI Stitching completed successfully !")
    print ("Boot Guard Profile: %s" % Args.btg_profile.upper())
    print ("IFWI image: %s\n" % IfwiFileName)

    return 0


if __name__ == '__main__':
    sys.exit(main())

