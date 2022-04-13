## @ StitchIfwiConfig.py
#  This is an IFWI stitch config script for ADL Slim Bootloader
#
# Copyright (c) 2020 - 2022, Intel Corporation. All rights reserved. <BR>
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
       Copy DiagnosticACM firmware image  to 'Input/DiagnosticAcm.bin'.


  3. Openssl
       Openssl is required for stitch. the stitch tool will search evn OPENSSL_PATH,
       to find Openssl. If evn OPENSSL_PATH is not found, will find openssl from
       "C:\\Openssl\\Openssl"

  4. Stitch the final image
       EX:
       Assuming stitching workspace is at D:\Stitch and building ifwi for ADL platform
       To stitch IFWI with SPI QUAD mode and Boot Guard profile VM:
         StitchIfwi.py -b vm -p adl -w D:\Stitch -s Stitch_Components.zip -c StitchIfwiConfig.py

"""

def get_bpmgen2_params_change_list ():
    params_change_list = []
    params_change_list.append ([
      # variable                | value |
      # ===================================
      ('PlatformRules',         'ADL Client'),
      ('BpmStrutVersion',       '0x20'),
      ('BpmRevision',           '0x01'),
      ('BpmRevocation',         '1'),
      ('AcmRevocation',         '2'),
      ('NEMPages',              '3'),
      ('AcpiBase',              '0x1800'),
      ('IbbFlags',              '0x2'),
      ('IbbHashAlgID',          '0x0C:SHA384'),
      ('TxtInclude',            'FALSE'),
      ('PcdInclude',            'TRUE'),
      ('BpmSigScheme',          '0x16:RSAPSS'),
      ('BpmSigPubKey',          r'BpmGen2/keys/bpm_pubkey_3072.pem'),
      ('BpmSigPrivKey',         r'BpmGen2/keys/bpm_privkey_3072.pem'),
      ('BpmKeySizeBits',        '3072'),
      ('BpmSigHashAlgID',       '0x0C:SHA384'),
      ])
    return params_change_list

def get_platform_sku():
    platform_sku ={
        'adls'    :   'Intel(R) AlderLake S Chipset - Consumer - SPI - B0',
    }
    return platform_sku

def get_component_replace_list(plt_params_list):
    replace_list = [
       #    Path                   file name              compress    Key                           SVN
      ('IFWI/BIOS/TS0/ACM0',      'Input/acm0.bin',        'dummy',    '',                            ''),
      ('IFWI/BIOS/TS1/ACM0',      'Input/acm0.bin',        'dummy',    '',                            ''),
       ]

    if 'tsn' in plt_params_list:
        if os.path.exists('IPFW/TsnSubRegion.bin'):
            print ("TsnSubRegion.bin found")
            replace_list.append (
                ('IFWI/BIOS/NRD/IPFW/TMAC', 'IPFW/TsnSubRegion.bin',     'lz4',     'KEY_ID_CONTAINER_COMP_RSA3072', 0),   # TSN MAC Address
            )
    if 'tcc' in plt_params_list:
      if os.path.exists('IPFW/TccCrlBinary.bin'):
          print ("TccCrlBinary.bin found")
          replace_list.append (
              ('IFWI/BIOS/NRD/IPFW/TCCM', 'IPFW/TccCrlBinary.bin',         'lz4',     'KEY_ID_CONTAINER_COMP_RSA3072', 0),   # TCC CRL binary
          )
      if os.path.exists('IPFW/TccCacheCfg.bin'):
          print ("TccCacheCfg.bin found")
          replace_list.append (
              ('IFWI/BIOS/NRD/IPFW/TCCC', 'IPFW/TccCacheCfg.bin',         'lz4',     'KEY_ID_CONTAINER_COMP_RSA3072', 0),   # TCC Cache Cfg binary
          )
      if os.path.exists('IPFW/TccStreamCfg.bin'):
          print ("TccStreamCfg.bin found")
          replace_list.append (
              ('IFWI/BIOS/NRD/IPFW/TCCT', 'IPFW/TccStreamCfg.bin',         'lz4',     'KEY_ID_CONTAINER_COMP_RSA3072', 0),   # TCC StreamCfg binary
          )

    return replace_list

def check_parameter(para_list):
    print (para_list)
    para_supported = {
        'ddr5'   : {},
        'quad'   : {},
        'debug'  : {},
        'tsn'    : {'0G1G', '0G25G', '1G0G', '25G0G', '1G1G', '1G25G', '25G1G', '25G25G'},
        'tcc'     : {}
        }

    para_help = \
        """
        'ddr5' -- Stitch for DDR5 board, by default for DDR4 board
        'tsn'  -- Apply TSN softstraps for ADL Silicon with link speeds given {0G, 1G, 2.5G} for TSN1 & TSN2
                  Note: TSN is disabled by default, use this only if needing to enable TSN
                  Examples: 'tsn:0G1G' - TSN 1 Disabled, TSN 2 1G
                            'tsn:1G25G' - TSN 1 1G, TSN 2 2.5G
        'debug'-- Enable DAM and DCI configuration (Only use for debug purpose but not for final production!)
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

        if para == 'tsn':
            if para_list[para] == None:
                print ("TSN option expects a linkspeed value or 'disable' option")
                print (para_help)
                return False
    return True

def get_oemkeymanifest_change_list():
    xml_change_list = []
    xml_change_list.append ([
        # Path                                                                            | value |
        # =========================================================================================
        ('./KeyManifestEntries/KeyManifestEntry/Usage',                                    'OemDebugManifest'),
        ('./KeyManifestEntries/KeyManifestEntry/HashBinary',                               'Temp/kmsigpubkey.hash'),
    ])

    return xml_change_list

def get_xml_change_list (platform, plt_params_list):
    xml_change_list = []
    xml_change_list.append ([
      # Path                                                      | value |
      # =========================================================================================
      #Region Order
        ('./BuildSettings/BuildResults/RegionOrder',                                '53241'),
        ('./FlashLayout/BiosRegion/InputFile',                                      '$SourceDir\BiosRegion.bin'),
        ('./FlashLayout/Ifwi_IntelMePmcRegion/MeRegionFile',                        '$SourceDir\MeRegionFile.bin'),
        ('./FlashLayout/Ifwi_IntelMePmcRegion/PmcBinary',                           '$SourceDir\PmcBinary.bin'),
        ('./FlashLayout/EcRegion/InputFile',                                        '$SourceDir\EcRegion.bin'),
        ('./FlashLayout/EcRegion/Enabled',                                          'Enabled'),
        ('./FlashLayout/EcRegion/EcRegionPointer',                                  '$SourceDir\EcRegionPointer.bin'),
        ('./FlashLayout/GbeRegion/InputFile',                                       '$SourceDir\GbeRegion.bin'),
        ('./FlashLayout/GbeRegion/Enabled',                                         'Enabled'),
        ('./FlashLayout/SubPartitions/PchcSubPartitionData/InputFile',              '$SourceDir\PchcSubPartitionData.bin'),
        ('./FlashSettings/FlashConfiguration/SpiDualIoReadEnable',                   'Yes'),
        ('./FlashSettings/FlashConfiguration/SpiDualOutReadEnable',                  'Yes'),
        ('./FlashSettings/FlashConfiguration/FastReadClockFreq',                     '25MHz'),
        ('./FlashSettings/FlashConfiguration/ReadIdAndReadStatClkFreq',              '25MHz'),
        ('./FlashSettings/FlashConfiguration/WriteEraseClockFreq',                   '25MHz'),
        #('./FlashSettings/VsccTable/VsccEntries/VsccEntry/VsccEntryName',            'VsccEntry0'),
        ('./FlashSettings/VsccTable/VsccEntries/VsccEntry/VsccEntryVendorId',        '0x1F'),
        ('./FlashSettings/VsccTable/VsccEntries/VsccEntry/VsccEntryDeviceId0',       '0x47'),
        ('./FlashSettings/VsccTable/VsccEntries/VsccEntry/VsccEntryDeviceId1',       '0x0'),
        ('./FlashSettings/BiosConfiguration/TopSwapOverride',                        '512KB'),
        ('./PlatformProtection/PlatformIntegrity/OemPublicKeyHash',                  'F8 F0 E3 69 15 81 76 99 0A 54 9E D4 C3 6D 1A 86 39 D8 87 3D EF F7 ED 2D E3 4C B4 1B CC B3 04 76 CE 0A A0 63 BC 5B 7A AC FF D9 50 9E 96 40 C6 99'),
        ('./PlatformProtection/PlatformIntegrity/OemExtInputFile',                   '$SourceDir\OemExtInputFile.bin'),
        ('./PlatformProtection/ContentProtection/PavpSupported',                     'Yes'),
        ('./PlatformProtection/BootGuardConfiguration/BtGuardKeyManifestId',         '0x1'),
        ('./PlatformProtection/TpmOverSpiBusConfiguration/SpiOverTpmBusEnable',      'Yes'),
        ('./Icc/IccPolicies/Profiles/Profile/ClockOutputConfiguration/ClkoutSRC0',   'Enabled'),
        ('./Icc/IccPolicies/Profiles/Profile/ClockOutputConfiguration/ClkoutSRC1',   'Enabled'),
        ('./Icc/IccPolicies/Profiles/Profile/ClockOutputConfiguration/ClkoutSRC2',   'Enabled'),
        ('./Icc/IccPolicies/Profiles/Profile/ClockOutputConfiguration/ClkoutSRC3',   'Enabled'),
        ('./Icc/IccPolicies/Profiles/Profile/ClockOutputConfiguration/ClkoutSRC4',   'Enabled'),
        ('./Icc/IccPolicies/Profiles/Profile/ClockOutputConfiguration/ClkoutSRC5',   'Enabled'),
        ('./Icc/IccPolicies/Profiles/Profile/ClockOutputConfiguration/ClkoutSRC6',   'Enabled'),
        ('./Icc/IccPolicies/Profiles/Profile/ClockOutputConfiguration/ClkoutSRC7',   'Enabled'),
        ('./Icc/IccPolicies/Profiles/Profile/ClockOutputConfiguration/ClkoutSRC8',   'Enabled'),
        ('./Icc/IccPolicies/Profiles/Profile/ClockOutputConfiguration/ClkoutSRC9',   'Enabled'),
        ('./Icc/IccPolicies/Profiles/Profile/ClockOutputConfiguration/ClkoutSRC10',  'Enabled'),
        ('./Icc/IccPolicies/Profiles/Profile/ClockOutputConfiguration/ClkoutSRC11',  'Enabled'),
        ('./Icc/IccPolicies/Profiles/Profile/ClockOutputConfiguration/ClkoutSRC12',  'Enabled'),
        ('./Icc/IccPolicies/Profiles/Profile/ClockOutputConfiguration/ClkoutSRC13',  'Enabled'),
        ('./Icc/IccPolicies/Profiles/Profile/ClockOutputConfiguration/ClkoutSRC14',  'Enabled'),
        ('./Icc/IccPolicies/Profiles/Profile/ClockOutputConfiguration/ClkoutSRC15',  'Enabled'),
        ('./Icc/IccPolicies/Profiles/Profile/ClockOutputConfiguration/ClkoutSRC16',  'Enabled'),
        ('./Icc/IccPolicies/Profiles/Profile/ClockOutputConfiguration/ClkoutSRC17',  'Enabled'),
        ('./Icc/IccPolicies/Profiles/Profile/PwrManagementConfiguration/ClkreqMapSRC2', 'GPP_H_5'),
        ('./Icc/IccPolicies/Profiles/Profile/PwrManagementConfiguration/ClkreqMapSRC4', 'GPP_H_4'),
        ('./Icc/IccPolicies/Profiles/Profile/PwrManagementConfiguration/ClkreqMapSRC6', 'GPP_H_7'),
        ('./Icc/IccPolicies/Profiles/Profile/PwrManagementConfiguration/ClkreqMapSRC7', 'GPP_H_6'),
        ('./Icc/IccPolicies/Profiles/Profile/PwrManagementConfiguration/ClkreqMapSRC9', 'GPP_H_5'),
        ('./Icc/IccPolicies/Profiles/Profile/PwrManagementConfiguration/ClkreqMapSRC10', 'GPP_D_11'),
        ('./Icc/IccPolicies/Profiles/Profile/PwrManagementConfiguration/ClkreqMapSRC11', 'GPP_H_3'),
        ('./Icc/IccPolicies/Profiles/Profile/PwrManagementConfiguration/ClkreqMapSRC12', 'GPP_D_14'),
        ('./Icc/IccPolicies/Profiles/Profile/PwrManagementConfiguration/ClkreqMapSRC13', 'GPP_D_13'),
        ('./Icc/IccPolicies/Profiles/Profile/PwrManagementConfiguration/ClkreqMapSRC14', 'GPP_J_8'),
        ('./Icc/IccPolicies/Profiles/Profile/PwrManagementConfiguration/ClkreqMapSRC16', 'GPP_H_8'),
        ('./Icc/IccPolicies/Profiles/Profile/PwrManagementConfiguration/ClkreqMapSRC17', 'GPP_D_2'),
        ('./NetworkingConnectivity/WiredLanConfiguration/GbePCIePortSelect',             'Port7'),
        ('./NetworkingConnectivity/WiredLanConfiguration/PhyConnected',                  'PHY on SMLink0'),
        ('./NetworkingConnectivity/WirelessLanConfiguration/MeClinkEnable',              'No'),
        ('./Power/PchThermalReporting/PchThrmlRprtngEn',                                 'Yes'),
        ('./IntegratedSensorHub/IntegratedSensorHub/IshPowerUpState',                    'Enabled'),
        ('./IntegratedSensorHub/IshImage/Length',                                        '0x4E000'),
        ('./IntegratedSensorHub/IshImage/InputFile',                                     '$SourceDir\IshImage.bin'),
        ('./IntegratedSensorHub/IshData/PdtBinary',                                  '$SourceDir\PdtBinary.bin'),
        ('./Debug/DelayedAuthenticationModeConfiguration/DelayedAuthMode',           'No'),
        ('./Debug/IntelMeFirmwareDebuggingOverrides/DbgOverridePreProdSi',           '0'),
        ('./Debug/DirectConnectInterfaceConfiguration/DciDbcEnable',                 'Yes'),
        ('./Debug/EarlyUsb2DbcOverType-AConfiguration/EnEarlyUsb2DbcCon',            'Yes'),
        ('./Debug/EarlyUsb2DbcOverType-AConfiguration/Usb2DbcAutoDetect',            'Enabled'),
        ('./CpuStraps/CpuStraps/CpuMaxFreqBoot',                                     'Yes'),
        ('./FlexIO/PciePortConfiguration/PCIeController1Config',                     '1x4'),
        ('./FlexIO/PciePortConfiguration/PCIeController2Config',                     '1x2, 2x1'),
        ('./FlexIO/PciePortConfiguration/PCIeController4Config',                     '1x4'),
        ('./FlexIO/Usb3PortConfiguration/USB3Prt5ConTypeSel',                        'Type A / Type C'),
        ('./FlexIO/Usb3PortConfiguration/USB3Prt6ConTypeSel',                        'Type A / Type C'),
        ('./FlexIO/Usb2PortConfiguration/USB2Prt10ConTypeSel',                       'Type A / Type C'),
        ('./FlexIO/Usb2PortConfiguration/USB2Prt14ConTypeSel',                       'Express Card / M.2 S2'),
        ('./FlexIO/PhyConfiguration/PhyBinaryFile',                                  '$SourceDir\SPHYRegion.bin'),
        ('./FlexIO/Type-CSubsystemConfiguration/PhyBinaryFile',                      '$SourceDir\TypeC_NorthPHYRegion.bin'),
        ('./FlexIO/PowerDelivery_PdControllerConfiguration/PdMode',                  'PMC / SMBus'),
        ('./Gpio/GpioVccioVoltageControl/GppJmstrVoltSelect',                        '1.8Volts'),
        ('./IntelMeKernel/IntelMeMeasuredBootConfiguration/MeMeasuredBootState',     'Disabled'),

    ])

    if platform == 'adls':
        xml_change_list.append ([
          ('./InternalPchBuses/DmiPcieConfiguration/DmiLaneWidth',                        'DMI x8'),
          ('./FlexIO/Usb3PortConfiguration/Usb32Port3and4SpdselPair',                     'USB 3.2 Port 3 and 4 Gen 2x1'),
          ('./FlexIO/Usb3PortConfiguration/Usb32Port5and6SpdselPair',                     'USB 3.2 Port 5 and 6 Gen 2x1'),
          ('./FlexIO/Usb3PortConfiguration/Usb32Port9and10SpdselPair',                    'USB 3.2 Port 9 and 10 Gen 2x1'),
          ('./FlexIO/Usb2PortConfiguration/USB2Prt14ConTypeSel',                          'Type A / Type C'),
        ])

    if 'tsn' in plt_params_list:
        print ("Applying changes to enable TSN")
        tsn1_en_tsn2_dis = 'TSN  1 Enabled TSN 2 Disabled'
        tsn1_dis_tsn2_en = 'TSN  1 Disabled TSN 2 Enabled'
        tsn_both_en      = 'TSN  1 and TSN 2 Enabled'

        tsn1_1G_tsn2_25G = 'TSN 1 1 Gig and TSN 2 2.5 Gig'
        tsn1_25G_tsn2_1G = 'TSN 1 2.5 Gig and TSN 2 1 Gig'
        tsn_both_25G = 'TSN 1 and TSN 2 2.5 Gig'
        tsn_both_1G  = 'TSN 1 and TSN 2 1 Gig'

        tsn_config = {
          '0G1G'   : [tsn1_dis_tsn2_en, tsn_both_1G],
          '0G25G'  : [tsn1_dis_tsn2_en, tsn_both_25G],
          '1G0G'   : [tsn1_en_tsn2_dis, tsn_both_1G],
          '25G0G'  : [tsn1_en_tsn2_dis, tsn_both_25G],
          '1G1G'   : [tsn_both_en, tsn_both_1G],
          '1G25G'  : [tsn_both_en, tsn1_1G_tsn2_25G],
          '25G1G'  : [tsn_both_en, tsn1_25G_tsn2_1G],
          '25G25G' : [tsn_both_en, tsn_both_25G],
        }

        xml_change_list.append ([
          ('./NetworkingConnectivity/TimeSensitiveNetworkingConfiguration/TsnEnabled', tsn_config[plt_params_list['tsn']][0]),
          ('./NetworkingConnectivity/TimeSensitiveNetworkingConfiguration/TsnSpeedSelect', tsn_config[plt_params_list['tsn']][1]),
          ('./FlexIO/PciePortConfiguration/PCIeController3Config', '1x2, 2x1'),
          ('./BuildSettings/BuildResults/Sku', 'R680E'),
        ])

    if 'ddr5' in plt_params_list:
        print ("Applying changes to enable ddr5 board")
        xml_change_list.append ([
            ('./BuildSettings/HarnessGlobalData/SelectedRvp',                              'ADL-S DDR5 UDIMM (ADP-S + ADL-S)'),
            ('./Gpio/GpioVccioVoltageControl/GppDmstrVoltSelect',                          '1.8Volts'),
            ])

    if 'quad' in plt_params_list:
        print ("Applying changes to enable spi quad")
        xml_change_list.append ([
            ('./FlashSettings/FlashConfiguration/QuadOutReadEnable',                       'Yes'),
            ('./FlashSettings/FlashConfiguration/QuadIoReadEnable',                        'Yes'),
            ('./InternalPchBuses/EspiConfiguration/EspiEcBusfreq',                         '60MHZ'),
            ('./InternalPchBuses/EspiConfiguration/EspiEcMaxIoMode',                       'Single, Dual and Quad'),
            ])

    if 'debug' in plt_params_list:
        print ("Applying changes to enable DAM and DCI")
        xml_change_list.append ([
            ('./Debug/DelayedAuthenticationModeConfiguration/DelayedAuthMode',                    'Yes'),
            ('./PlatformProtection/BootGuardConfiguration/BtGuardCpuDebugEnable',                 'Enabled'),
            ('./Debug/DirectConnectInterfaceConfiguration/DciDbcEnable',                          'Yes'),
            ('./Debug/DirectConnectInterfaceConfiguration/Usb1DciOobEnable',                      'Yes'),
            ('./Debug/DirectConnectInterfaceConfiguration/Usb2DciOobEnable',                      'Yes'),
            ('./Debug/DirectConnectInterfaceConfiguration/Usb3DciOobEnable',                      'Yes'),
            ('./Debug/DirectConnectInterfaceConfiguration/Usb4DciOobEnable',                      'Yes'),
            ])

    return xml_change_list
