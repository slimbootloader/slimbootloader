## @ StitchIfwiConfig.py
#  This is an IFWI stitch config script for ADL Slim Bootloader
#
# Copyright (c) 2023, Intel Corporation. All rights reserved. <BR>
# SPDX-License-Identifier: BSD-2-Clause-Patent
#
##

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
    'azb'    :   'Intel(R) AlderLake P Chipset - Consumer - SPI',
  }
  return platform_sku

def get_component_replace_list(plt_params_list):
  replace_list = [
      #    Path                   file name              compress    Key                           SVN
    ('IFWI/BIOS/TS0/ACM0',      'Input/acm0.bin',        'dummy',    '',                            ''),
    ('IFWI/BIOS/TS1/ACM0',      'Input/acm0.bin',        'dummy',    '',                            ''),
  ]

  return replace_list

def check_parameter(para_list):
  print (para_list)
  para_supported = {
    'lp5'    : {},
    'debug'  : {},
    'crb'    : {},
  }

  para_help = \
    """
    'lp5'    -- Stitch for LP5 board, default configuration is set for LP5 board
    'debug'  -- Enable DAM and DCI configuration (Only use for debug purpose but not for final production!)
    'crb'    -- Select board type as CRB
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

def get_oemkeymanifest_change_list():
  xml_change_list = []
  xml_change_list.append ([
    # Path                                                                       | value |
    # =========================================================================================
    ('./KeyManifestEntries/KeyManifestEntry/Usage',                              'OemDebugManifest'),
    ('./KeyManifestEntries/KeyManifestEntry/HashBinary',                         'Temp/kmsigpubkey.hash'),
  ])

  return xml_change_list

def get_xml_change_list (platform, plt_params_list):
  xml_change_list = []
  xml_change_list.append ([
    # Path                                                                         | value |
    # =========================================================================================
    #Region Order
    ('./BuildSettings/BuildResults/RegionOrder',                                 '653241'),
    ('./BuildSettings/HarnessGlobalData/SelectedRvp',                            'ADL-M LP5 RVP2A (ADL-M + ADP-P)'),
    ('./FlashLayout/BiosRegion/InputFile',                                       '$SourceDir\BiosRegion.bin'),
    ('./FlashLayout/Ifwi_IntelMePmcRegion/MeRegionFile',                         '$SourceDir\MeRegionFile.bin'),
    ('./FlashLayout/Ifwi_IntelMePmcRegion/PmcBinary',                            '$SourceDir\PmcBinary.bin'),
    ('./FlashLayout/GbeRegion/InputFile',                                        '$SourceDir\GbeRegion.bin'),
    ('./FlashLayout/GbeRegion/Enabled',                                          'Enabled'),
    ('./FlashLayout/SubPartitions/PchcSubPartitionData/InputFile',               '$SourceDir\PchcSubPartitionData.bin'),
    ('./FlashSettings/FlashConfiguration/SpiDualIoReadEnable',                   'No'),
    ('./FlashSettings/FlashConfiguration/SpiDualOutReadEnable',                  'No'),
    ('./FlashSettings/BiosConfiguration/BiosRedAssistance',                      'Disabled'),
    ('./PlatformProtection/ContentProtection/PavpSupported',                     'Yes'),
    ('./PlatformProtection/BootGuardConfiguration/BtGuardKeyManifestId',         '0x1'),
    ('./PlatformProtection/TpmOverSpiBusConfiguration/SpiOverTpmBusEnable',      'Yes'),
    ('./Icc/IccPolicies/Profiles/Profile/ClockOutputConfiguration/ClkoutSRC0',   'Enabled'),
    ('./Icc/IccPolicies/Profiles/Profile/ClockOutputConfiguration/ClkoutSRC1',   'Enabled'),
    ('./Icc/IccPolicies/Profiles/Profile/ClockOutputConfiguration/ClkoutSRC2',   'Enabled'),
    ('./Icc/IccPolicies/Profiles/Profile/ClockOutputConfiguration/ClkoutSRC3',   'Enabled'),
    ('./Icc/IccPolicies/Profiles/Profile/ClockOutputConfiguration/ClkoutSRC4',   'Enabled'),
    ('./Icc/IccPolicies/Profiles/Profile/ClockOutputConfiguration/ClkoutSRC5',   'Enabled'),
    ('./Icc/IccPolicies/Profiles/Profile/ClockOutputConfiguration/ClkoutSRC6',   'Disabled'),
    ('./Icc/IccPolicies/Profiles/Profile/ClockOutputConfiguration/ClkoutSRC7',   'Disabled'),
    ('./Icc/IccPolicies/Profiles/Profile/ClockOutputConfiguration/ClkoutSRC8',   'Disabled'),
    ('./Icc/IccPolicies/Profiles/Profile/ClockOutputConfiguration/ClkoutSRC9',   'Disabled'),
    ('./NetworkingConnectivity/WiredLanConfiguration/PhyConnected',              'PHY on SMLink0'),
    ('./NetworkingConnectivity/WiredLanConfiguration/GbePCIePortSelect',         'None'),
    ('./IntegratedSensorHub/IntegratedSensorHub/IshPowerUpState',                'Enabled'),
    ('./IntegratedSensorHub/IshImage/InputFile',                                 '$SourceDir\IshImage.bin'),
    ('./IntegratedSensorHub/IshData/PdtBinary',                                  '$SourceDir\PdtBinary.bin'),
    ('./Debug/DelayedAuthenticationModeConfiguration/DelayedAuthMode',           'No'),
    ('./FlexIO/Type-CSubsystemConfiguration/IomBinaryFile',                      '$SourceDir\Iom_L0R0.bin'),
    ('./FlexIO/Type-CSubsystemConfiguration/PhyBinaryFile',                      '$SourceDir\TypeC_NorthPHYRegion_L1.bin'),
    ('./FlexIO/Type-CSubsystemConfiguration/TbtBinaryFile',                      '$SourceDir\TBT.bin'),
    ('./FlexIO/Type-CSubsystemConfiguration/IomBinaryFile',                      '$SourceDir\Iom.bin'),
    ('./FlexIO/Type-CSubsystemConfiguration/PhyBinaryFile',                      '$SourceDir\TypeC_NorthPHYRegion.bin'),
    ('./PlatformProtection/IntelPttConfiguration/PttSupported',                  'No'),
    ('./PlatformProtection/IntelPttConfiguration/PttPwrUpState',                 'Disabled'),
    ('./PlatformProtection/BiosGuardConfiguration/BiosGrdProtOvrdEn',            'Yes'),
    ('./NetworkingConnectivity/WiredLanConfiguration/LanEnable',                 'No'),
    ('./NetworkingConnectivity/WirelessLanConfiguration/MeClinkEnable',          'No'),
    ('./FlexIO/PcieLaneReversalConfiguration/PCIeCtrl3LnReversal',               'No'),
    ('./FlexIO/SataPcieComboPortConfiguration/SataPCIeComboPort0',               'Disabled'),
    ('./FlexIO/SataPcieComboPortConfiguration/SataPCIeComboPort1',               'Disabled'),
    ('./FlexIO/Usb3PortConfiguration/USB3PCIeComboPort3',                        'PCIe'),
    ('./FlexIO/Usb2PortConfiguration/USB2Prt4ConTypeSel',                        'Express Card / M.2 S2'),
    ('./InternalPchBuses/EspiConfiguration/EspiEcMaxIoMode',                     'Single, Dual and Quad'),
    ('./CpuStraps/CpuStraps/Mipi124RailSrcPlat',                                 'Yes'),
    ('./CpuStraps/CpuStraps/VccSaSvidVrAddr',                                    '0x2'),
    ('./FlexIO/Type-CSubsystemConfiguration/TypeCPort3Config',                   'DP Fixed Connection'),
    ('./FlexIO/ThunderboltConfiguration/TboltEnable',                            'No'),
    ('./FlexIO/PowerDelivery_PdControllerConfiguration/TypeCPort3Mode',          'No'),
    ('./FlexIO/PowerDelivery_PdControllerConfiguration/TypeCPort3RetimerEnabled','No'),
    ('./FlexIO/PowerDelivery_PdControllerConfiguration/TypeCPort4Mode',          'No'),
    ('./FlexIO/PowerDelivery_PdControllerConfiguration/TypeCPort4RetimerEnabled','No'),
    ('./FlexIO/PowerDelivery_PdControllerConfiguration/USB2PortForTypeCPort4',   'USB2 Port 4'),
    ('./Gpio/GpioVccioVoltageControl/GppA23voltSelect',                          '3.3Volts'),
    ('./Gpio/ThunderboltLsxBssb-LsConfiguration/TbltLsxBssbLs2Cfg',              'Legacy VCCIO'),
    ('./Gpio/ThunderboltLsxBssb-LsConfiguration/TbltLsxBssbLs3Cfg',              'Legacy VCCIO'),
    ('./InternalPchBuses/EspiConfiguration/EspiEcBusfreq',                       '20MHz'),
    ('./FlashLayout/EcRegion/InputFile',                                         '$SourceDir\EcRegion.bin'),
    ('./FlashLayout/EcRegion/Enabled',                                           'Enabled'),
    ('./FlashLayout/EcRegion/EcRegionPointer',                                   '$SourceDir\EcRegionPointer.bin'),
    ('./FlashSettings/BiosConfiguration/TopSwapOverride',                        '256KB'),
    ('./PlatformProtection/PlatformIntegrity/OemPublicKeyHash',                  '17 D1 1C 85 98 77 13 B3 15 3D E5 80 F5 2F C0 F6 03 7C CE 22 91 E8 A6 B2 BD D4 95 20 BC 5E 00 6F 38 F8 18 60 6A 3A 7B 62 89 DA 05 7F D9 4E 50 B9'),
    ('./FlexIO/PciePortConfiguration/PCIeController2Config',                     '4x1'),
    ('./FlexIO/PciePortConfiguration/PCIeController3Config',                     '4x1'),
    ('./CpuStraps/CpuStraps/GtsVrType',                                          'Fixed VR'),
    ('./FlashSettings/FlashConfiguration/QuadOutReadEnable',                     'No'),
    ('./FlashSettings/FlashConfiguration/QuadIoReadEnable',                      'No'),
    ('./FlashSettings/FlashConfiguration/FastReadSupport',                       'No'),
    ('./FlexIO/UfsConfiguration/UfsCont1Config',                                 'X1'),
    ('./Gpio/GpioVccioVoltageControl/GppH3voltSelect',                           '3.3Volts'),
    ('./Gpio/GpioVccioVoltageControl/GppH4voltSelect',                           '3.3Volts'),
    ('./Gpio/GpioVccioVoltageControl/GppH5voltSelect',                           '3.3Volts'),
    ('./Gpio/GpioVccioVoltageControl/GppH6voltSelect',                           '3.3Volts'),
    ('./Gpio/GpioVccioVoltageControl/GppH7voltSelect',                           '3.3Volts'),
    ('./Gpio/GpioVccioVoltageControl/GppH12voltSelect',                          '3.3Volts'),
    ('./Dnx/OEMandPlatformIDs/DnxEnabled',                                       'No'),
    ('./FlexIO/Usb3PortConfiguration/USB3PCIeComboPort1',                        'PCIe'),
    ('./FlexIO/Usb2PortConfiguration/USB2Prt3ConTypeSel',                        'Express Card / M.2 S2'),
    ('./FlexIO/Usb2PortConfiguration/USB2Prt5ConTypeSel',                        'Express Card / M.2 S2'),
    ('./FlexIO/Usb2PortConfiguration/USB2Prt6ConTypeSel',                        'Express Card / M.2 S2'),
    ('./FlashSettings/FlashConfiguration/FastReadClockFreq',                     '14MHz'),
    ('./FlashSettings/FlashConfiguration/WriteEraseClockFreq',                   '14MHz'),
    ('./FlashSettings/FlashConfiguration/ReadIdAndReadStatClkFreq',              '14MHz'),
    ('./PlatformProtection/IntelPttConfiguration/PttSupportedFpf',               'No'),
    ('./Power/DeepSx/DeepSxSupportEnable',                                       'No'),
    ('./Camera/IPUSecurity/SecureTouchCameraMask',                               '0x0'),
    ('./Debug/IntelMeFirmwareDebuggingOverrides/DbgOverridePreProdSi',           '0x0'),
    ('./Debug/DirectConnectInterfaceConfiguration/DciDbcEnable',                 'No'),
    ('./Debug/DirectConnectInterfaceConfiguration/Usb1DciOobEnable',             'No'),
    ('./Debug/DirectConnectInterfaceConfiguration/Usb2DciOobEnable',             'No'),
    ('./Debug/DirectConnectInterfaceConfiguration/Usb3DciOobEnable',             'No'),
    ('./Debug/DirectConnectInterfaceConfiguration/Usb4DciOobEnable',             'No'),
    ('./FlexIO/Usb3PortConfiguration/USB3Prt1ConTypeSel',                        'Type A / Type C'),
    ('./FlexIO/Usb3PortConfiguration/USB3Prt2ConTypeSel',                        'Express Card / M.2 S2'),
    ('./FlexIO/Usb3PortConfiguration/USB3Prt3ConTypeSel',                        'Type A / Type C'),
    ('./FlexIO/Usb2PortConfiguration/USB2Prt1ConTypeSel',                        'Type A / Type C'),
    ('./FlexIO/Usb2PortConfiguration/USB2Prt2ConTypeSel',                        'Type A / Type C'),
    ('./FlexIO/PowerDelivery_PdControllerConfiguration/TypeCPort1Mode',          'No'),
    ('./FlexIO/PowerDelivery_PdControllerConfiguration/TypeCPort1RetimerEnabled','No'),
    ('./FlexIO/PowerDelivery_PdControllerConfiguration/TypeCPort1SmbusAddr',     '0x0'),
    ('./FlexIO/PowerDelivery_PdControllerConfiguration/TypeCPort2Mode',          'No'),
    ('./FlexIO/PowerDelivery_PdControllerConfiguration/TypeCPort2RetimerEnabled','No'),
    ('./FlexIO/PowerDelivery_PdControllerConfiguration/TypeCPort2SMBusAddr',     '0x0'),
    ('./FlexIO/PowerDelivery_PdControllerConfiguration/TypeCPort3SmbusAddr',     '0x0'),
    ('./IntelUniquePlatformId/EntitlementsConfiguration/IcpsSwSkuing',           'Yes'),
    ('./FlexIO/Type-CSubsystemConfiguration/TypecPort1ConTypeSel',               'Type C'),
    ('./FlexIO/Type-CSubsystemConfiguration/TcssPortEnMask',                     '0x1'),
    ('./FlexIO/Type-CSubsystemConfiguration/TypeCPort1Config',                   'No Thunderbolt'),
  ])

  if 'debug' in plt_params_list:
    print ("Applying changes to enable DAM and DCI")
    xml_change_list.append ([
      ('./Debug/DelayedAuthenticationModeConfiguration/DelayedAuthMode',          'Yes'),
      ('./PlatformProtection/BootGuardConfiguration/BtGuardCpuDebugEnable',       'Enabled'),
      ('./Debug/DirectConnectInterfaceConfiguration/DciDbcEnable',                'Yes'),
      ('./Debug/DirectConnectInterfaceConfiguration/Usb1DciOobEnable',            'Yes'),
      ('./Debug/DirectConnectInterfaceConfiguration/Usb2DciOobEnable',            'Yes'),
      ('./Debug/DirectConnectInterfaceConfiguration/Usb3DciOobEnable',            'Yes'),
      ('./Debug/DirectConnectInterfaceConfiguration/Usb4DciOobEnable',            'Yes'),
    ])
  return xml_change_list
