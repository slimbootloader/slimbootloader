## @ StitchIfwi.py
#  This is a python stitching script for Slim Bootloader APL build
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
from   subprocess   import check_output
from   collections  import OrderedDict

sys.dont_write_bytecode = True
from   StitchLoader import COMPONENT, SUBPART_DIR_HEADER, SUBPART_DIR_ENTRY, BIOS_ENTRY
from   StitchLoader import FlashMap, FlashMapDesc, Bytes2Val, PatchFlashMap
from   StitchLoader import ManipulateIfwi, LocateComponent, ParseIfwiLayout, PrintIfwiLayout

ExtraUsageTxt = """
This script creates a new Apollo Lake Slim Bootloader IFWI image using FIT
tool.  For the FIT tool and stitching ingredients listed in step 2 below, please
contact your Intel representative to get them.

Please follow the steps below for APL-I IFWI stitching.

  1. Create a stitching workspace. The paths mentioned below are all relative
     to it.

  2. Extract required tools and ingredients to stitching workspace.
     - FIT tool (APL-I Trusted Execution Engine firmware package)
       Copy 'fit.exe' or 'fit' and 'vsccommn.bin' to 'CSE/FIT' folder
       Copy 'meu.exe' or 'meu' to 'CSE/MEU' folder
       Copy 'cse_image.bin' to 'CSE/Silicon/cse_image.bin'
       For Linux, please ensure 'fit' and 'meu' have execution permission.

     - ISH firmware (APL Windows 10 Best Known Configuration package)
       Copy ISHC firmware image (EX: 'Production_ishC_4.1.0.3364.bin')
            to 'ISH/ISHC.bin'.
       Copy ISHC data image (EX: 'INTC_pdt_BXT_P_NS_BOM3_SENSORS')
            to 'ISH/INTC_pdt_APL_NS_BOM3_SENSORS'.

     - PMC firmware (APL Windows 10 Best Known Configuration package)
       Copy PMC firmware image (EX: 'v03_1a_00000000_prod_signed.bin')
            to 'PMC/pmcp.bin'.

     - IUnit and FIT template XML (Latest APL SOC FSP release package)
       Copy 'IUnit.bin' in BlStitch.7z to 'IUnit/IUnit.bin'
       Copy 'bxt_spi_8mb.xml' in BlStitch.7z to 'CSE/FIT/bxt_spi_8mb.xml'
       Please note, this XML is for Intel's APL LeafHill CRB platform.
       For other boards, plese use FIT to modify this XML accordingly.

  3. Extract additional public released ingredients to stitching workspace.
     - Audio firmware
       Please download the audio firmware from:
       https://downloadcenter.intel.com/download/26895
       After downloading, please extract and copy the DSP FW image into
       'Audio/dsp_fw.bin'

     - Microcode
       Please download the latest microcode package from:
       https://downloadcenter.intel.com/download/28087
       Current APL SOC needs microcode intel-ucode/06-5c-09 (APL D0) and 06-5c-0a (APL E0)
       After downloading, please extract and copy '06-5c-09' and '06-5c-0a 'into 'Patch'
       folder in stitching workspace.

     - Keys
       This is the private key for signing. Here, a test key is used for demonstration.
       Please download the test key TestSigningPrivateKey.pem from:
       https://github.com/slimbootloader/slimbootloader/tree/master/BootloaderCorePkg/Tools/Keys
       and copy it to 'Keys/bxt_dbg_priv_key.pem'

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
       Assume stitching workspace is at D:\Stitch
       To stitch IFWI with SPI QUAD mode and Boot Guard profile VM:
         python  Platform\ApollolakeBoardPkg\Script\StitchIfwi.py -b vm -q -w D:\Stitch
       To clean all generated files:
         python  Platform\ApollolakeBoardPkg\Script\StitchIfwi.py -c -w D:\Stitch

"""


def GetConfig ():
    # This dictionary defines path for stitching components
    CfgVar = {
      'wkspace'   :   'Output',
      'fitinput'  :   'Output/input',
      'ifwiname'  :   'APL_SBL_IFWI.bin',
      'fitxml'    :   'CSE/FIT/bxt_spi_8mb.xml',
      'fit'       :   'CSE/FIT/fit%s' %     ('.exe' if os.name == 'nt' else ''),
      'meu'       :   'CSE/MEU/meu%s' %     ('.exe' if os.name == 'nt' else ''),
      'openssl'   :   'Openssl/openssl.exe' if os.name == 'nt' else '/usr/bin/openssl',
      'audio'     :   'Audio/dsp_fw.bin',
      'cseimg'    :   'CSE/Silicon/cse_image.bin',
      'ishdata'   :   'ISH/INTC_pdt_APL_NS_BOM3_SENSORS',
      'ishimg'    :   'ISH/ISHC.bin',
      'iunit'     :   'IUnit/IUnit.bin',
      'pmc'       :   'PMC/pmcp.bin',
      'patch1'    :   'Patch/06-5c-09',
      'patch2'    :   'Patch/06-5c-0a',
      'keys'      :   'Keys/bxt_dbg_priv_key.pem',
    }
    return CfgVar


def ParseBiosLayout (ImgData):
    Offset = 0
    BiosHdr = BIOS_ENTRY.from_buffer(ImgData, Offset)
    if BiosHdr.Name != "BIOS":
        return None

    BiosComp = COMPONENT(BiosHdr.Name, COMPONENT.TYPE_IMG, 0, len(ImgData))
    Offset += sizeof(BiosHdr)
    EntryNum = BiosHdr.Offset
    for Idx in xrange(EntryNum):
        PartEntry = BIOS_ENTRY.from_buffer(ImgData, Offset)
        PartComp = COMPONENT(PartEntry.Name, COMPONENT.TYPE_DIR,
                             PartEntry.Offset, PartEntry.Length)
        BiosComp.AddChild(PartComp)
        SubPartDirHdr = SUBPART_DIR_HEADER.from_buffer(ImgData,
                                                       PartEntry.Offset)
        if '$CPD' == SubPartDirHdr.HeaderMarker:
            for Dir in xrange(SubPartDirHdr.NumOfEntries):
                PartDir = SUBPART_DIR_ENTRY.from_buffer(
                    ImgData, PartEntry.Offset + sizeof(SUBPART_DIR_HEADER) +
                    sizeof(SUBPART_DIR_ENTRY) * Dir)
                DirComp = COMPONENT(PartDir.EntryName, COMPONENT.TYPE_FILE,
                                    PartEntry.Offset + PartDir.EntryOffset,
                                    PartDir.EntrySize)
                PartComp.AddChild(DirComp)
        Offset += sizeof(PartEntry)

    return BiosComp


def CopyPrimaryBpdt (IfwiFile, OutputFile = ''):
    Fd = open(IfwiFile, "rb")
    IfwiImgData = bytearray(Fd.read())
    Fd.close()

    Ret = ManipulateIfwi  ('COPY', '', IfwiImgData)
    if Ret != 0:
        return -1

    if OutputFile == '':
        OutputFile = IfwiFile

    Fd = open(OutputFile, "wb")
    Fd.write(IfwiImgData)
    Fd.close()

    return 0


def PatchIbbFlashMap (IbblFile, IfwiFile, OutputFile = ''):
    Fd = open(IfwiFile, "rb")
    IfwiImgData = bytearray(Fd.read())
    Fd.close()
    Ifwi = PatchFlashMap (IfwiImgData)
    if Ifwi:
        return -1

    if OutputFile == '':
        OutputFile = IbblFile

    Ifwi = ParseIfwiLayout (IfwiImgData)
    if not Ifwi:
        return -2

    # Locate IBBL in both BP0 and BP1
    for Part in range(2):
        Path = 'ROOT/IFWI/BP%d/BPDT/BpdtIbb/IBBL' % Part
        Ibbl = LocateComponent (Ifwi, Path)
        if not Ibbl:
            if Part == 0:
                return -3
            continue

        IbblImgData = IfwiImgData[Ibbl.Offset:Ibbl.Offset + Ibbl.Length]

        if Part == 1:
            OutputFile = OutputFile[0:-4] + "_backup.bin"

        Fd = open(OutputFile, "wb")
        Fd.write(IbblImgData)
        Fd.close()

    return 0


def ReplaceIbbPartition (BiosImage, IfwiImage, OutputImage, BiosPath, IfwiPath):
    Fd = open(BiosImage, "rb")
    BiosImgData = bytearray(Fd.read())
    Fd.close()
    Bios = ParseBiosLayout (BiosImgData)
    if not Bios:
        return -1

    Fd = open(IfwiImage, "rb")
    IfwiImgData = bytearray(Fd.read())
    Fd.close()
    Ifwi = ParseIfwiLayout (IfwiImgData)
    if not Ifwi:
        return -2

    BiosComp = LocateComponent (Bios, BiosPath)
    if not BiosComp:
        return -3

    IfwiComp = LocateComponent (Ifwi, IfwiPath)
    if not IfwiComp:
        return -4

    if BiosComp.Length != IfwiComp.Length:
        return -5

    IfwiImgData[IfwiComp.Offset:IfwiComp.Offset + IfwiComp.Length] = \
        BiosImgData[BiosComp.Offset:BiosComp.Offset + BiosComp.Length]

    if OutputImage == '':
        OutputImage = IfwiImage

    Fd = open(IfwiImage, "wb")
    Fd.write(IfwiImgData)
    Fd.close()

    return 0


def PaddingIasImage (IbblImage):
    Fd = open(IbblImage, "rb")
    IbblImgData = bytearray(Fd.read())
    Fd.close()

    # Locate FlashMap offset
    FlaMapOff = (Bytes2Val(IbblImgData[-8:-4]) + len(IbblImgData)) & 0xFFFFFFFF
    FlaMapStr = FlashMap.from_buffer (IbblImgData, FlaMapOff)
    EntryNum  = (FlaMapStr.Length - sizeof(FlashMap)) // sizeof(FlashMapDesc)
    for Idx in xrange (EntryNum):
        Desc  = FlashMapDesc.from_buffer (IbblImgData, FlaMapOff + sizeof(FlashMap) + Idx * sizeof(FlashMapDesc))
        if Desc.Sig != 'IAS1' :
            continue
        Ias1Name  = os.path.join(os.path.dirname (IbblImage), 'Stitch_PROV.bin')
        if not os.path.exists(Ias1Name):
            Ias1Size = 0
            Mode     = 'wb'
        else:
            Ias1Size = os.path.getsize(Ias1Name)
            Mode     = 'r+b'
        if Desc.Size > Ias1Size:
            IasFh = open (Ias1Name, Mode)
            IasFh.seek (Ias1Size)
            IasFh.write ('\xff' * (Desc.Size - Ias1Size))
            IasFh.close()
        if Desc.Size < Ias1Size:
            raise Exception("ERROR: IAS1 image 0x%x bigger than size 0x%x in FlashMap!" % (Ias1Size, Desc.Size))

    return 0


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
        Proc = subprocess.Popen(CmdArgs, shell = Shell, cwd=Cwd, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        Out, Err = Proc.communicate()
        Ret = Proc.returncode
    except Exception as e:
        Out, Err = '', e
        Ret = 1
    if Ret:
        sys.stdout.flush()
        print ('%s\n%s' % (Out, Err))
        print "Failed to run command:\n  PATH: %s\n   CMD: %s" % (Cwd, Cmd)
        sys.exit(1)


def RebuildModules (Node, ModList):
    for Child in list(Node):
        Node.remove (Child)
    for Mod, FileName in ModList:
        Child  = ET.SubElement(Node, "DataModule")
        Child.attrib['name'] = Mod
        Child2 = ET.SubElement(Child, "InputFile")
        Child2.attrib['value'] = "$SourceDir/Stitch_%s.bin" % FileName


def XmlWrite (Tree, File):
    Text = ET.tostring(Tree.getroot(), 'utf-8')
    Reparsed  = minidom.parseString(Text)
    Formatted = Reparsed.toprettyxml(indent='    ', newl='\n', encoding="utf-8")
    Fp = open (File, "w")
    for Line in Formatted.splitlines():
        if Line.strip():
            Fp.write ('%s\n' % Line)
    Fp.close()


def GenMeuOemKeyXml (MeuDir, Src, Dst):
    CfgVar = GetConfig ()
    Tree = ET.parse(MeuDir + '/%s' % Src)
    Node = Tree.find('./KeyManifestEntries')
    for Child in list(Node):
        Node.remove (Child)

    ManifestList = [
      ('IfwiManifest | OemSmipManifest | OemDnxIfwiManifest | BootPolicyManifest' ,
        'bxt_dbg_pub_key_hash.bin'),
      ('cAvsImage0Manifest | cAvsImage1Manifest' ,
        'bxt_prod_pub_key_hash_Audio.bin'),
      ('iUnitBootLoaderManifest | iUnitMainFwManifest' ,
        'bxt_prod_pub_key_hash_iUnit.bin'),
      ('IshManifest' ,
        'bxt_prod_pub_key_hash_ISH.bin')
    ]

    Prefix = '../..'
    for Manifest, FileName in ManifestList:
        Child  = ET.SubElement(Node, "KeyManifestEntry")
        Child2 = ET.SubElement(Child, "Usage")
        Child2.attrib['value'] = Manifest
        Child3 = ET.SubElement(Child, "HashBinary")
        Child3.attrib['value'] = '%s/%s/%s' % (Prefix, CfgVar['fitinput'], FileName)

    XmlWrite (Tree, MeuDir + '/%s' % Dst)


def GenMeuCfgXml (MeuDir, Src, Dst):
    CfgVar = GetConfig ()
    Tree = ET.parse(MeuDir + '/%s' % Src)
    Node = Tree.find('./SigningConfig/SigningToolPath')
    if CfgVar['openssl'].startswith('/'):
        Node.attrib['value'] = CfgVar['openssl']
    else:
        Node.attrib['value'] = '../../' + CfgVar['openssl']
    Node = Tree.find('./SigningConfig/PrivateKeyPath')
    Node.attrib['value'] = '../../' + CfgVar['keys']
    XmlWrite (Tree, MeuDir + '/%s' % Dst)


def GenFitXml (FitDir, Src, Dst, BtgProfile, SpiQuad):
    CfgVar = GetConfig ()
    Tree = ET.parse(FitDir + '/%s' % Src)

    # Change tool path
    Node = Tree.find('./BuildSettings/BuildResults/MeuToolPath')
    if os.name == 'nt':
        Node.attrib['value'] = '../MEU/meu.exe'
    else:
        Node.attrib['value'] = '../MEU/meu'
    Node = Tree.find('./BuildSettings/BuildResults/SigningToolPath')
    if CfgVar['openssl'].startswith('/'):
        Node.attrib['value'] = CfgVar['openssl']
    else:
        Node.attrib['value'] = '../../' + CfgVar['openssl']

    # Change region order
    Node = Tree.find('./BuildSettings/BuildResults/RegionOrder')
    Node.attrib['value'] = '415'

    # Enable PDR region
    Node = Tree.find('./FlashLayout/SubPartitions/PdrRegion/Enabled')
    Node.attrib['value'] = 'Enabled'
    Node = Tree.find('./FlashLayout/SubPartitions/PdrRegion/InputFile')
    Node.attrib['value'] = '$SourceDir\input\pdr.bin'

    # Change SPI mode settings
    if SpiQuad:
        Value = 'Yes'
    else:
        Value = 'No'
    print "Set SPI QUAD mode: %s" % Value
    Node = Tree.find('./FlashSettings/FlashConfiguration/QuadIoReadEnable')
    Node.attrib['value'] = Value
    Node = Tree.find('./FlashSettings/FlashConfiguration/QuadOutReadEnable')
    Node.attrib['value'] = Value

    # Change boot guard settings
    Node = Tree.find('./PlatformProtection/BootGuardConfiguration/BtGuardProfileConfig')
    Profiles = Node.attrib['value_list'].split(',,')
    if len(Profiles) != 3:
        print "Expected Boot Guard profile list in XML !"
        return -1
    print "Set Boot Guard profile: %s" % BtgProfile
    if BtgProfile == 'legacy':
        Node.attrib['value'] = Profiles[0]
    elif BtgProfile == 'v':
        Node.attrib['value'] = Profiles[1]
    else:
        Node.attrib['value'] = Profiles[2]

    XmlWrite (Tree, FitDir + '/%s' % Dst)


def GenMeuSblXml (MeuDir, Src, Dst):
    Tree = ET.parse(MeuDir + '/%s' % Src)
    Node = Tree.find('./IbbSubPartition/Modules')
    ModList = [
      ("IPAD",  "IPAD"),
      ("IBBL",  "IBBL"),
      ("IBB",   "IBBM"),
      ("OBB",   "OBB"),
      ("FWUP",  "FWU"),
      ("CFGD",  "CFGDATA"),
      ("PLD",   "PLD"),
      ("VAR",   "VAR"),
      ("MRCD",  "MRCDATA")
      ]
    RebuildModules (Node, ModList)

    Node = Tree.find('./ObbSubPartition/Modules')
    ModList = [
      ("OPAD",  "OPAD"),
      ("PROV",  "PROV"),
      ("EPLD",  "EPLD"),
      ("UVAR",  "UVAR")
      ]
    RebuildModules (Node, ModList)
    XmlWrite (Tree, MeuDir + '/%s' % Dst)


def Clean (StitchDir, DistMode):
    print ("Clean up workspace ...")

    CfgVar    = GetConfig ()
    for Each in [CfgVar['wkspace']]:
        Each = StitchDir + '/' + Each
        if os.path.exists(Each):
            shutil.rmtree(Each)

    FitDir = StitchDir + '/' + os.path.dirname(CfgVar['fit'])
    MeuDir = StitchDir + '/' + os.path.dirname(CfgVar['meu'])
    Patterns = [
      FitDir    + '/*.log',
      MeuDir    + '/*.log',
      FitDir    + '/*_sbl.xml',
      MeuDir    + '/*.xml',
    ]

    for Pattern in Patterns:
        for File in glob.glob(Pattern):
            os.remove (File)

    return 0


def Stitch (StitchDir, StitchZip, BtgProfile, SpiQuadMode, FullRdundant = True):

    CfgVar    = GetConfig ()

    print ("\nUnpack files from stitching zip file ...")
    Zf = zipfile.ZipFile(StitchZip, 'r', zipfile.ZIP_DEFLATED)
    Zf.extractall(os.path.join(StitchDir, CfgVar['fitinput']))
    Zf.close()

    os.chdir(StitchDir)

    print ("\nPadding IAS image size ...")
    Ret = PaddingIasImage ('%s/Stitch_IBBL.bin' % CfgVar['fitinput'])
    if Ret:
        raise Exception ("Failed with error %d !" % Ret)

    print ("\nChecking and copying components ...")
    CopyList = ['cseimg', 'ishdata', 'ishimg', 'iunit', 'pmc', 'audio']
    for Each in ['fit', 'meu', 'openssl', 'fitxml'] + CopyList:
        if not os.path.exists(CfgVar[Each]):
             raise Exception ("Could not find file '%s' !" % CfgVar[Each])

    for Each in CopyList:
        shutil.copy (CfgVar[Each], CfgVar['fitinput'])
    shutil.copy (CfgVar['patch1'], CfgVar['fitinput'] + '/upatch1.bin')
    shutil.copy (CfgVar['patch2'], CfgVar['fitinput'] + '/upatch2.bin')
    KeyDir = CfgVar['wkspace'] + '/keys'
    if not os.path.exists(KeyDir):
        os.mkdir(KeyDir)
    shutil.copy (CfgVar['keys'],   KeyDir)

    # SMIP_IAFW is not used by SBL, create a dummy one for stitching.
    print ("\nGenerating smip_iafw.bin ...")
    OutputDir = StitchDir + '/' + CfgVar['fitinput']
    Fp = open (OutputDir + '/smip_iafw.bin', 'wb')
    Fp.write (b'\xAF\xBE\xED\xDE' + b'\x00' * 0x380 + b'\xAA\xCC\xFF\xAA')
    Fp.close()

    # PDR region is not used by SBL, here create one for stitching.
    InputPdr = StitchDir + '/%s/Stitch_PDR.bin' % CfgVar['fitinput']
    OutputPdr = OutputDir  + '/pdr.bin'
    if not os.path.exists(InputPdr):
        print ("\nGenerating pdr.bin ...")
        Fp = open (OutputPdr, 'wb')
        Fp.write (b'\xff' * 0xFF000)
        Fp.close()
    else:
        shutil.copy (InputPdr, OutputPdr)

    Prefix = '../..'
    MeuDir = StitchDir + '/' + os.path.dirname(CfgVar['meu'])
    if not os.path.exists (MeuDir + '/meu_config.xml'):
        print ("\nGenerating meu_config.xml ...")
        Cmd = './meu -gen meu_config'
        RunCmd (Cmd, MeuDir)
        GenMeuCfgXml (MeuDir, 'meu_config.xml', 'meu_config.xml')

    print ("\nGenerating public key hash ...")
    Cmd = './meu -keyhash %s -f %s' % (Prefix + '/%s/bxt_prod_pub_key_hash_ISH' % CfgVar['fitinput'], Prefix + '/%s' % CfgVar['ishimg'])
    RunCmd (Cmd, MeuDir)
    Cmd = './meu -keyhash %s -f %s' % (Prefix + '/%s/bxt_prod_pub_key_hash_iUnit' % CfgVar['fitinput'], Prefix + '/%s' % CfgVar['iunit'])
    RunCmd (Cmd, MeuDir)
    Cmd = './meu -keyhash %s -f %s' % (Prefix + '/%s/bxt_prod_pub_key_hash_Audio' % CfgVar['fitinput'], Prefix + '/%s' % CfgVar['audio'])
    RunCmd (Cmd, MeuDir)
    Cmd = './meu -keyhash %s -key %s' % (Prefix + '/%s/bxt_dbg_pub_key_hash' % CfgVar['fitinput'], Prefix + '/%s' % CfgVar['keys'])
    RunCmd (Cmd, MeuDir)

    print ("\nGenerating sbl.xml ...")
    Cmd = './meu -gen Bios -o sbl.xml'
    RunCmd (Cmd, MeuDir)
    GenMeuSblXml (MeuDir, 'sbl.xml', 'sbl.xml')

    print ("\nGenerating OEMKeyManifest.xml ...")
    Cmd = './meu -gen OEMKeyManifest -o OEMKeyManifest.xml'
    RunCmd (Cmd, MeuDir)
    GenMeuOemKeyXml (MeuDir, 'OEMKeyManifest.xml', 'OEMKeyManifest.xml')

    print ("\nGenerating BIOS.bin ...")
    Cmd  = './meu -f sbl.xml -o %s -u1 . -key %s -s %s' % (Prefix + '/%s/bios.bin' % CfgVar['fitinput'],
           Prefix + '/%s' % CfgVar['keys'], Prefix + '/%s' % CfgVar['fitinput'])
    RunCmd (Cmd, MeuDir)

    print ("\nGenerating OEM key manifest ...")
    Cmd  = './meu -f OEMKeyManifest.xml -o %s' % (Prefix + '/%s/oemkeymn2.bin' % CfgVar['fitinput'])
    RunCmd (Cmd, MeuDir)

    FitDir = StitchDir + '/' + os.path.dirname(CfgVar['fit'])
    print ("\nGenerating bxt_spi_8mb_sbl.xml ...")
    GenFitXml (FitDir, 'bxt_spi_8mb.xml', 'bxt_spi_8mb_sbl.xml', BtgProfile, SpiQuadMode)

    print ("\nGernerating SPI Image ...")
    Cmd = './fit -b -f bxt_spi_8mb_sbl.xml -o %s -w %s -s %s' % (Prefix + '/%s/%s' % (CfgVar['wkspace'],
          CfgVar['ifwiname']),   Prefix + '/%s' % CfgVar['wkspace'], Prefix + '/%s' % CfgVar['wkspace'])

    if CfgVar['openssl'].startswith('/'):
        Cmd = Cmd + ' -st_path %s'   % (CfgVar['openssl'])
    else:
        Cmd = Cmd + ' -st_path %s'   % (Prefix + '/' + CfgVar['openssl'])
    RunCmd (Cmd, FitDir)

    IfwiFileName = '%s/%s' % (CfgVar['wkspace'], CfgVar['ifwiname'])
    if FullRdundant:
        print ("\nCopy BP0 BPDT into BP1 ...")
        Ret = CopyPrimaryBpdt ('%s' % IfwiFileName)

    print ("\nPatching IBBL Flash map ...")
    Ret = PatchIbbFlashMap (CfgVar['fitinput'] + '/Stitch_IBBL.bin', IfwiFileName)
    if Ret:
        raise Exception ("Failed with error %d !" % ret)

    print ("\nRe-gernerating BIOS.bin for BP0 ...")
    Cmd  = './meu -f sbl.xml -o %s -u1 . -key %s -s %s' % (Prefix + '/%s/bios.bin' % CfgVar['fitinput'], Prefix + '/%s' % CfgVar['keys'], Prefix + '/%s' % CfgVar['fitinput'])
    RunCmd (Cmd, MeuDir)

    print ("\nUpdating IBB partition for BP0 in IFWI ...")
    Ret = ReplaceIbbPartition (CfgVar['fitinput'] + '/bios.bin', IfwiFileName,  IfwiFileName, 'BIOS/IBBP', 'ROOT/IFWI/BP0/BPDT/BpdtIbb')
    if Ret:
        raise Exception ("Failed with error %d !" % ret)

    if FullRdundant:
        print ("\nRe-gernerating BIOS.bin for BP1 ...")
        shutil.copy (CfgVar['fitinput'] + '/Stitch_IBBL_backup.bin', CfgVar['fitinput'] + '/Stitch_IBBL.bin')
        Cmd  = './meu -f sbl.xml -o %s -u1 . -key %s -s %s' % (Prefix + '/%s/bios_backup.bin' % CfgVar['fitinput'], Prefix + '/%s' % CfgVar['keys'], Prefix + '/%s' % CfgVar['fitinput'])
        RunCmd (Cmd, MeuDir)

        print ("\nUpdating BIOS.bin for BP1 in IFWI ...")
        Ret = ReplaceIbbPartition (CfgVar['fitinput'] + '/bios_backup.bin', IfwiFileName, IfwiFileName, 'BIOS/IBBP', 'ROOT/IFWI/BP1/BPDT/BpdtIbb')
        if Ret:
            raise Exception ("Failed with error %d !" % ret)

    return 0


def main():

    if len(sys.argv) == 1:
        print ('%s' % ExtraUsageTxt)

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
                    choices=['legacy', 'v', 'vm'],
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

    Args = Ap.parse_args()

    StitchDir = Args.stitch_dir
    if Clean (StitchDir, Args.clean):
        raise Exception ('Stitching clean up failed !')

    if Args.clean:
        print ("Cleaning completed successfully !\n")
        return 0

    if Stitch (StitchDir, Args.stitch_zip, Args.btg_profile, Args.quad_mode):
        raise Exception ('Stitching process failed !')

    CfgVar       = GetConfig ()
    IfwiFileName = '%s/%s/%s' % (StitchDir, CfgVar['wkspace'], CfgVar['ifwiname'])
    IfwiFileName = os.path.normpath (IfwiFileName)

    print ("\nIFWI Stitching completed successfully !")
    print ("Boot Guard Profile: %s" % Args.btg_profile.upper())
    print ("IFWI image: %s\n" % IfwiFileName)

    return 0


if __name__ == '__main__':
    sys.exit(main())
