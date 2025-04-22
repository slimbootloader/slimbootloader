## @ StitchIfwi.py
#  This is an IFWI stitch config script for Slim Bootloader
#
# Copyright (c) 2019 - 2022, Intel Corporation. All rights reserved. <BR>
# SPDX-License-Identifier: BSD-2-Clause-Patent
#
##
from StitchLoader import *

extra_usage_txt = \
"""This is an IFWI stitch config script for Slim Bootloader For the FIT tool and
stitching ingredients listed in step 2 below, please contact your Intel representative.

  1. Create a stitching workspace directory. The paths mentioned below are all
     relative to it.

  2. Extract required tools and ingredients to stitching workspace.
     - FIT tool
       Copy 'fit.exe' or 'fit' and 'vsccommn.bin' to 'Fit' folder
     - BPMGEN2 Tool
       Copy the contents of the tool to Bpmgen2 folder
       Rename the bpmgen2 parameter to bpmgen2.params if its name is not this name.
      - Components
       Copy 'cse_image.bin'     to 'Input/cse_image.bin'
       Copy PMC firmware image  to 'Input/pmc.bin'.
       Copy EC firmware image   to 'Input/ec.bin'.
       copy ECregionpointer.bin to 'Input/ecregionpointer.bin'
       Copy GBE binary image    to 'Input/gbe.bin'.
       Copy ACM firmware image  to 'Input/acm.bin'.

  3. Openssl
       Openssl is required for stitch. the stitch tool will search evn OPENSSL_PATH,
       to find Openssl. If evn OPENSSL_PATH is not found, will find openssl from
       "C:\\Openssl\\Openssl"

  4. Stitch the final image
       EX:
       Assuming stitching workspace is at D:\Stitch and building ifwi for EHL platform
       To stitch IFWI with SPI QUAD mode and Boot Guard profile VM:
         StitchIfwi.py -b vm -p ehl -w D:\Stitch -s Stitch_Components.zip -c StitchIfwiConfig.py

"""

def get_bpmgen2_params_change_list ():
    params_change_list = []
    params_change_list.append ([
      # variable                | value |
      # ===================================
      ('PlatformRules',         'EHL Embedded'),
      ('BpmStrutVersion',       '0x20'),
      ('BpmRevision',           '0x01'),
      ('BpmRevocation',         '1'),
      ('AcmRevocation',         '2'),
      ('NEMPages',              '3'),
      ('IbbFlags',              '0x2'),
      ('IbbHashAlgID',          '0x0C:SHA384'),
      ('MCHBAR',                '0x00000000FEC80000'),
      ('VTD_BAR',               '0x00000000FED91000'),
      ('TxtInclude',            'FALSE'),
      ('PcdInclude',            'FALSE'),
      ('BpmSigScheme',          '0x16:RSAPSS'),
      ('BpmSigPubKey',          r'BpmGen2/keys/bpm_pubkey_3072.pem'),
      ('BpmSigPrivKey',         r'BpmGen2/keys/bpm_privkey_3072.pem'),
      ('BpmKeySizeBits',        '3072'),
      ('BpmSigHashAlgID',       '0x0C:SHA384'),
      ])
    return params_change_list

def get_platform_sku():
    platform_sku ={
        'ehl'    :   'EHL No Emulation'
    }
    return platform_sku

def get_component_replace_list():
    replace_list = [
      # Path                      file name                  compress  Key                              SVN
      ('IFWI/BIOS/TS0/ACM0',      'Input/acm.bin',           'dummy',  '',                              ''),
      ('IFWI/BIOS/TS1/ACM0',      'Input/acm.bin',           'dummy',  '',                              ''),
    ]

    # ChipsetInit binary
    if os.path.exists('IPFW/ChipInitBinary.bin'):
        replace_list.append (
            ('IFWI/BIOS/NRD/IPFW/CHIP', 'IPFW/ChipInitBinary.bin',   'dummy',  'KEY_ID_CONTAINER_COMP_RSA3072', 0 ),
        )
    # need to set ENABLE_PRE_OS_CHECKER = 1 in BoardConfig.py
    if os.path.exists('IPFW/PreOsChecker.bin'):
        replace_list.append (
            ('IFWI/BIOS/NRD/IPFW/POSC', 'IPFW/PreOsChecker.bin',   'dummy',  'KEY_ID_CONTAINER_COMP_RSA3072', 0 ), # Pre-OS Checker
        )

    # need to set ENABLE_PSEFW_LOADING = 1 in BoardConfig.py
    if os.path.exists('IPFW/PseFw.bin'):
        replace_list.append (
            ('IFWI/BIOS/NRD/IPFW/PSEF', 'IPFW/PseFw.bin',          'lz4',    'KEY_ID_CONTAINER_COMP_RSA3072', 0 ), # OSE FW
        )

    # need to set ENABLE_TSN = 1 in BoardConfig.py
    if os.path.exists('IPFW/PseTsnIpConfig.bin'):
        replace_list.append (
            ('IFWI/BIOS/NRD/IPFW/TSIP', 'IPFW/PseTsnIpConfig.bin', 'lz4',    'KEY_ID_CONTAINER_COMP_RSA3072', 0 ), # PSE TSN IP
        )

    if os.path.exists('IPFW/TsnConfig.bin'):
        replace_list.append (
            ('IFWI/BIOS/NRD/IPFW/TSNC', 'IPFW/TsnConfig.bin',      'lz4',    'KEY_ID_CONTAINER_COMP_RSA3072', 0 ), # TSN Config
        )

    if os.path.exists('IPFW/TsnMacAddr.bin'):
        replace_list.append (
            ('IFWI/BIOS/NRD/IPFW/TMAC', 'IPFW/TsnMacAddr.bin',     'lz4',    'KEY_ID_CONTAINER_COMP_RSA3072', 0 ), # TSN MAC Address
        )

    return replace_list

def check_parameter(para_list):
    print (para_list)
    para_supported = {
        'tsn'      : {},
        'dnx'      : {},
        'pt'       : {},
        'isi_conf' : {},
        'debug'    : {},
        }

    para_help = \
        """
        'tsn'       -- Enable TSN, by disable tsn port is disabled
        'dnx'       -- Enable DNX capsule binary build for IFWI updating via DNX
        'pt'        -- Enable Proof Test configuration
        'isi_conf'  -- Enable ISI FW configuration file
        'debug'     -- Enable DAM and DCI configuration (Only use for debug purpose but not for final production!)
        """
    for para in para_list:
        if para == '':
            print ("No platform specific parameter provided")
            return True
        if para not in para_supported:
            print ("NOT support platform parameter: %s" % para)
            print ("platform parameter list:\n")
            print (para_help)
            return False

        if para_list[para] != None:
            if para_list[para] not in para_supported[para]:
                print ("NOT support platform parameter: %s" %  para_list[para])
                print ("platform parameter list:", para_supported[para])
                return False
    return True

def get_xml_change_list (platform, plt_params_list, stitch_dir):
    xml_change_list = []

    # Append FIT Settings based on BIOS IFWI
    xml_change_list.append ([
      # Path                                                                            | value |
      # =========================================================================================
      #Region Order
      ('./BuildSettings/BuildResults/RegionOrder',                                 '4521'),
      ('./FlashLayout/DescriptorRegion/OemBinary',                                 '$SourceDir\OemBinary.bin'),
      ('./FlashLayout/BiosRegion/InputFile',                                       '$SourceDir\BiosRegion.bin'),
      ('./FlashLayout/Ifwi_IntelMePmcRegion/MeRegionFile',                         '$SourceDir\MeRegionFile.bin'),
      ('./FlashLayout/Ifwi_IntelMePmcRegion/ChipInitBinary',                       '$SourceDir\ChipInitBinary.bin'),
      ('./FlashLayout/Ifwi_IntelMePmcRegion/PmcBinary',                            '$SourceDir\PmcBinary.bin'),
      #('./FlashLayout/EcRegion/EcRegionPointer',                                   '$SourceDir\EcRegionPointer.bin'),
      ('./FlashLayout/SubPartitions/PchcSubPartitionData/InputFile',               '$SourceDir\PchcSubPartitionData.bin'),
      ('./FlashLayout/SubPartitions/IsifSubPartitionData/InputFile',               '$SourceDir\IsifSubPartitionData.bin'),
      ('./FlashSettings/FlashComponents/FlashComponent1Size',                      '32MB'),
      ('./FlashSettings/VsccTable/VsccEntries/VsccEntry/VsccEntryName',            'VsccEntry0'),
      ('./FlashSettings/VsccTable/VsccEntries/VsccEntry/VsccEntryVendorId',        '0xEF'),
      ('./FlashSettings/VsccTable/VsccEntries/VsccEntry/VsccEntryDeviceId0',       '0x40'),
      ('./FlashSettings/VsccTable/VsccEntries/VsccEntry/VsccEntryDeviceId1',       '0x19'),
      #('./IntelAmt/IntelAmtConfiguration/NetServicesSupported',                    'No'),
      #('./PlatformProtection/PlatformIntegrity/OemPublicKeyHash',                  'F8 F0 E3 69 15 81 76 99 0A 54 9E D4 C3 6D 1A 86 39 D8 87 3D EF F7 ED 2D E3 4C B4 1B CC B3 04 76 CE 0A A0 63 BC 5B 7A AC FF D9 50 9E 96 40 C6 99'),
      #('./PlatformProtection/PlatformIntegrity/OemExtInputFile',                   '$SourceDir\OemExtInputFile.bin'),
      ('./PlatformProtection/BootGuardConfiguration/BtGuardKeyManifestId',         '0x1'),
      ('./PlatformProtection/TpmOverSpiBusConfiguration/SpiOverTpmBusEnable',      '$Yes'),
      #('./Debug/IntelTraceHubTechnology/RomTraceFiltering',                        '$SourceDir\RomTraceFiltering.bin'),
      ('./PlatformProtection/BootGuardConfiguration/BtGuardCpuDebugEnable',        'Disabled'),
      #('./Debug/IntelTraceHubTechnology/UnlockToken',                              '$SourceDir\UnlockToken.bin'),
      ('./Debug/EarlyUsb2DbcOverType-AConfiguration/Usb2DbcPortEn',                'No USB2 Ports'),
      ('./Debug/EarlyUsb2DbcOverType-AConfiguration/EnEarlyUsb2DbcCon',            'No'),
      ('./Debug/DirectConnectInterfaceConfiguration/DciEnable',                    'No'),
      ('./Debug/DirectConnectInterfaceConfiguration/Usb1DciOobEnable',             'No'),
      ('./Debug/DirectConnectInterfaceConfiguration/Usb2DciOobEnable',             'No'),
      ('./Debug/DirectConnectInterfaceConfiguration/Usb3DciOobEnable',             'No'),
      ('./Debug/DirectConnectInterfaceConfiguration/Usb4DciOobEnable',             'No'),
      ('./StrapsDifferences/PCH_Strap_DMI_OPDMI_PAD_Diff',                         '0x00000005'),
      ('./Debug/IntelTraceHubTechnology/RomTraceDbgMsgsEnable',                    'No'),
      ('./Debug/EspiFeatureOverrides/EspiEcLowFreqOvrd',                           'No'),
      ('./FlexIO/PcieMultiVcPortConfiguration/PCIeMltVcCont3Conf',                 'x2 PCIe on Lanes 6 and 7'),
      ('./CpuStraps/VccSfrOcPgPresent',                                            'Yes'),
      ('./CpuStraps/VssStPgPresent',                                               'Yes'),
      ('./CpuStraps/VssStgPgPresent',                                              'No'),
      ('./FlexIO/SataPcieComboPortConfiguration/SataPCIeComboPort0',               'SATA'),
      ('./FlexIO/SataPcieComboPortConfiguration/SataPCIeComboPort1',               'GPIO Polarity PCIe'),
      ('./FlexIO/UfsConfiguration/UfsController1',                                 'X1'),
      ('./FlexIO/UfsConfiguration/Ufsx2En',                                        'No'),
      ('./FlexIO/UfsConfiguration/MmpUfsx2En',                                     'No'),
      ('./PlatformProtection/IntelPttConfiguration/Rsa1KSupport',                           'Disabled'),
      ('./NetworkingConnectivity/TimeSensitiveNetworkingConfiguration/TsnEnabled',          'TSN Disabled'),
      ('./NetworkingConnectivity/TimeSensitiveNetworkingConfiguration/SgmiHost0',           'Disabled'),

    ])

    # Append SBL specific FIT Settings
    # Top Swap Size given in BoardConfig.py is allocated to be 512KB
    xml_change_list.append ([('./FlashSettings/BiosConfiguration/TopSwapOverride', '512KB')])

    if 'tsn' in plt_params_list:
        print ("Applying changes to enable TSN")
        xml_change_list.append ([
            ('./NetworkingConnectivity/TimeSensitiveNetworkingConfiguration/TsnEnabled', 'TSN Enabled'),
            ('./NetworkingConnectivity/TimeSensitiveNetworkingConfiguration/SgmiHost0',  'Lane 10'),
            ('./FlexIO/SataPcieComboPortConfiguration/SataPCIeComboPort0',               'TSN1'),
            ('./FlexIO/SataPcieComboPortConfiguration/SataPCIeComboPort1',               'Disabled'),
            ])

    if 'pt' in plt_params_list:
        print ("Applying changes to enable PT")
        xml_change_list.append ([
            ('./FlashLayout/SubPartitions/GbstSubPartitionData/InputFile',               '$SourceDir\Pt.bin'),
            ('./PlatformProtection/FuSaConfiguration/FuSaPrfTestComponents',             '0x800036B0'),
            ])
        plt_params_list.update({'isi_conf':None})

    if 'isi_conf' in plt_params_list:
        print ("Applying changes to enable ISI configuration")
        xml_change_list.append ([
            ('./FlashLayout/SubPartitions/IsifSubPartitionData/ISIConfFile',             '$SourceDir\IsiConfFile.bin'),
            ])

    if 'dnx' in plt_params_list:
        print ("Applying changes to enable DNX binary generation")
        if os.name == 'nt':
            openssl_path = get_openssl_path() + '.exe'
            meu_path = os.path.join (stitch_dir, 'Meu', 'meu.exe')
        else:
            openssl_path = get_openssl_path()
            meu_path = os.path.join (stitch_dir, 'Meu', 'meu')

        xml_change_list.append ([
            ('./Dnx/DnxConfiguration/BuildEnabled', 'Yes'),
            ('./Dnx/DnxConfiguration/SigningKey', '$SourceDir/../BpmGen2/keys/oem_privkey_3072.pem'),
            ('./BuildSettings/BuildResults/MeuToolPath', meu_path),
            ('./BuildSettings/BuildResults/SigningToolPath', openssl_path)
            ])

    if 'debug' in plt_params_list:
        print ("Applying changes to enable DAM and DCI")
        xml_change_list.append ([
            ('./Debug/DelayedAuthenticationModeConfiguration/DelayedAuthMode',                    'Yes'),
            ('./PlatformProtection/BootGuardConfiguration/BtGuardCpuDebugEnable',                 'Enabled'),
            ('./Debug/DirectConnectInterfaceConfiguration/DciEnable',                             'Yes'),
            ('./Debug/DirectConnectInterfaceConfiguration/Usb1DciOobEnable',                      'Yes'),
            ('./Debug/DirectConnectInterfaceConfiguration/Usb2DciOobEnable',                      'Yes'),
            ('./Debug/DirectConnectInterfaceConfiguration/Usb3DciOobEnable',                      'Yes'),
            ('./Debug/DirectConnectInterfaceConfiguration/Usb4DciOobEnable',                      'Yes'),
            ])
    return xml_change_list

