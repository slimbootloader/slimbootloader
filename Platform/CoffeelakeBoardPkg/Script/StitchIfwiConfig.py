## @ StitchIfwiConfig.py
#  This is an IFWI stitching configuration script for Slim Bootloader
#
# Copyright (c) 2019 - 2020, Intel Corporation. All rights reserved.<BR>
# SPDX-License-Identifier: BSD-2-Clause-Patent
#
##

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
      ('BpmSigPubKey',          'Bpmgen2\keys\pubkey.pem'),
      ('BpmSigPrivKey',         'Bpmgen2\keys\privkey.pem'),
      ])
    return params_change_list

def get_platform_sku():
    platform_sku ={
        'whl'    :   'CNP-LP Base U',
        'cfls'   :   'Q370',
        'cflh'   :   'QM370'
    }
    return platform_sku

def get_component_replace_list():
    replace_list = [
       #    Path                   file name                  compress     Key
       ('IFWI/BIOS/TS0/ACM0',      'Input/acm.bin',          'dummy',   ''),
       ('IFWI/BIOS/TS1/ACM0',      'Input/acm.bin',          'dummy',   ''),
       ]

    return replace_list

def check_parameter(para_list):
    return True

def get_xml_change_list (platform):
    xml_change_list = []
    xml_change_list.append ([
      # Path                                                                            | value |
      # =========================================================================================
      #Region Order
      ('./BuildSettings/BuildResults/RegionOrder',                                        '45321'),
      ('./FlashLayout/BiosRegion/InputFile',                                              '$SourceDir\BiosRegion.bin'),
      ('./FlashLayout/Ifwi_IntelMePmcRegion/MeRegionFile',                                '$SourceDir\cse_image.bin'),
      ('./FlashLayout/Ifwi_IntelMePmcRegion/PmcBinary',                                   '$SourceDir\pmc.bin'),
      ('./FlashLayout/EcRegion/InputFile',                                                '$SourceDir\ec.bin'),
      ('./FlashLayout/EcRegion/Enabled',                                                  'Enabled'),
      ('./FlashLayout/EcRegion/EcRegionPointer',                                          '$SourceDir\ecregionpointer.bin'),
      ('./FlashLayout/GbeRegion/Enabled',                                                 'Enabled'),
      ('./FlashLayout/GbeRegion/InputFile',                                               '$SourceDir\gbe.bin'),
      ('./FlashSettings/FlashConfiguration/QuadIoReadEnable',                             'Yes'),
      ('./FlashSettings/FlashConfiguration/QuadOutReadEnable',                            'Yes'),
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

