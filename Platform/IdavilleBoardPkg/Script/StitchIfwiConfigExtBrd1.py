## @ StitchIfwiConfigExtBrd1.py
#  This is an IFWI stitch config script for Slim Bootloader
#
# Copyright (c) 2021, Intel Corporation. All rights reserved. <BR>
# SPDX-License-Identifier: BSD-2-Clause-Patent
#
##

from StitchIfwiConfig import StitchIfwiConfig

class StitchIfwiConfigExtBrd1(StitchIfwiConfig):

    def get_xml_change_list (self, platform, plt_params_list):
        xml_change_list = super().get_xml_change_list(platform, plt_params_list)
        xml_change_list.extend([
            # Path                                         | value |
            # =========================================================================================
            # Override CPK FW for Ext_Brd1
            ('./FlashLayout/Regions/GbeARegion/InputFile', '$SourceDir\img_nis_region_ext_brd1.bin'),

            # PchStraps for Ext_Brd1
            ('./PchStraps/PchStrap1/PchStrap1SS1bit0PCIeRPCluster2-RP8Enable', '0x1'),
            ('./PchStraps/PchStrap1/PchStrap1SS1bit1PCIeRPCluster2-RP9Enable', '0x0'),
            ('./PchStraps/PchStrap1/PchStrap1SS1bit2PCIeRPCluster2-RP10Enable', '0x1'),
            ('./PchStraps/PchStrap1/PchStrap1SS1bit3PCIeRPCluster2-RP11Enable', '0x1'),
            ('./PchStraps/PchStrap65/PchStrap65SS65bits30HSIOLane6is', '0x2'),
            ('./PchStraps/PchStrap65/PchStrap65SS65bits74HSIOLane7is', '0x2'),
            ('./PchStraps/PchStrap65/PchStrap65SS65bits118HSIOLane8is', '0x1'),
            ('./PchStraps/PchStrap65/PchStrap65SS65bits1512HSIOLane9is', '0x1'),
            ('./PchStraps/PchStrap65/PchStrap65SS65bits1916HSIOLane10is', '0x2'),
            ('./PchStraps/PchStrap65/PchStrap65SS65bits2320HSIOLane11is', '0x1'),
            ('./PchStraps/PchStrap65/PchStrap65SS65bits2724HSIOLane12is', '0x2'),
            ('./PchStraps/PchStrap65/PchStrap65SS65bits3128HSIOLane13is', '0x2'),
            ('./PchStraps/PchStrap66/PchStrap66SS66bits30HSIOLane14is', '0x2'),
            ('./PchStraps/PchStrap66/PchStrap66SS66bits74HSIOLane15is', '0x2'),
            ('./PchStraps/PchStrap66/PchStrap66SS66bits118HSIOLane16is', '0x2'),
            ('./PchStraps/PchStrap66/PchStrap66SS66bits1512HSIOLane17is', '0x2'),
            ('./PchStraps/PchStrap66/PchStrap66SS66bits1916HSIOLane18is', '0x2'),
            ('./PchStraps/PchStrap66/PchStrap66SS66bits2320HSIOLane19is', '0x2'),
            ('./PchStraps/PchStrap66/PchStrap66SS66bits2724HSIOLane20is', '0x3'),
            ('./PchStraps/PchStrap66/PchStrap66SS66bits3128HSIOLane21is', '0x2'),
            ('./PchStraps/PchStrap71/PchStrap71SS71bit0PCIeRPCluster1-RP4Enable', '0x1'),
            ('./PchStraps/PchStrap71/PchStrap71SS71bit1PCIeRPCluster1-RP5Enable', '0x1'),
            ('./PchStraps/PchStrap71/PchStrap71SS71bit2PCIeRPCluster1-RP6Enable', '0x1'),
            ('./PchStraps/PchStrap71/PchStrap71SS71bit3PCIeRPCluster1-RP7Enable', '0x0'),
            ('./PchStraps/PchStrap73/PchStrap73SS73bit0PCIeRPCluster0-RP0Enable', '0x1'),
            ('./PchStraps/PchStrap73/PchStrap73SS73bit1PCIeRPCluster0-RP1Enable', '0x0'),
            ('./PchStraps/PchStrap73/PchStrap73SS73bit2PCIeRPCluster0-RP2Enable', '0x0'),
            ('./PchStraps/PchStrap73/PchStrap73SS73bit3PCIeRPCluster0-RP3Enable', '0x0'),
            ('./PchStraps/PchStrap76/PchStrap76_register', '0x5510'),
            ('./PchStraps/PchStrap103/PchStrap103SS103bit26CGUCLK_SYNCE0DriverEnable', '0x1'),
        ])
        return xml_change_list

def get_stitch_ifwi_config_instance():
    return StitchIfwiConfigExtBrd1()
