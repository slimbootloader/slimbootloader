## @ loadthis.py
#
# Copyright (c) 2019, Intel Corporation. All rights reserved.<BR>
# SPDX-License-Identifier: BSD-2-Clause-Patent
#
##

import os
import gdb
from functools import reduce


def bytes_to_value(bytes):
    return reduce(lambda x, y: (x << 8) | y, bytes[::-1])


def value_to_bytes(value, length):
    return [(value >> (i * 8) & 0xff) for i in range(length)]


class LoadThis(gdb.Command):
    def __init__(self):
        super(LoadThis, self).__init__("loadthis", gdb.COMMAND_DATA)
        self.gdb_inf = gdb.inferiors()[0]

    def ord(self, addr, width):
        val = self.gdb_inf.read_memory(addr, width)
        return bytes_to_value(val.tobytes())

    def ord2(self, addr):
        return self.ord(addr, 2)

    def ord4(self, addr):
        return self.ord(addr, 4)

    def ord8(self, addr):
        return self.ord(addr, 8)

    def gets(self, addr):
        result = []
        loop = True
        while loop:
            v = self.ord8(addr)
            t = 0
            while v > 0:
                l = v & 0x7F
                if l == 0:
                    break
                result.append("%c" % l)
                v >>= 8
                t = t + 1
            if t != 8:
                loop = False
            addr = addr + 8
        return ''.join(result)

    def invoke(self, arg, from_tty):
        arch = gdb.selected_frame().architecture().name()
        eip_str = "$eip"
        if "x86-64" in arch:
            eip_str = "$rip"
        eip = int(gdb.parse_and_eval(eip_str))
        if eip < 0x10000:
            eip += 0xffff0000

        curr = eip & 0xfffffffc
        back = curr - 0x40000
        step = 4
        found = False
        while curr > back:
            val = self.ord4(curr)
            if val == 0x00005a4d:  # PE image
                pe_image_base = curr
                pe00_base = pe_image_base + self.ord4(curr + 0x3c)
                pe_sig = self.ord4(pe00_base)
                if pe_sig == 0x4550:
                    found = True
            elif val == 0x014C5A56:  # TE image
                found = True

            if found == True: break
            curr -= step
        if not found:
            print("ERROR: could not locate PE/TE header !")
            return

        if val == 0x00005a4d:
            pe_coff_hdr = pe00_base + 4
            pe_num_sections = self.ord2(pe_coff_hdr + 2)
            pe_opt_hdr_base = pe_coff_hdr + 0x14
            pe_opt_hdr_size = self.ord2(pe_coff_hdr + 0x10)

            if pe_opt_hdr_size == 0:
                print("ERROR: optional header size is zero.")
                return

            pe_sig = self.ord2(pe_opt_hdr_base)
            if (pe_sig != 0x010B) and (pe_sig != 0x020B):
                print("ERROR: bad magic number")
                return

            code_off = self.ord4(pe_opt_hdr_base + 0x14)
            pe_num_rva_offs = 0
            pe_data_dir_offs = 0
            if pe_sig == 0x010b:
                # PE32 executable
                pe_num_rva_offs = 0x5c
                pe_data_dir_offs = 0x60
            else:
                # PE32+ executable
                pe_num_rva_offs = 0x6c
                pe_data_dir_offs = 0x70

            num_rva = self.ord4(pe_opt_hdr_base + pe_num_rva_offs)
            if self.ord4(pe_opt_hdr_base + pe_num_rva_offs) < 7:
                print("ERROR: number of RVA < 7")
                return

            pe_debug_dir_entry_offs = self.ord4(pe_opt_hdr_base +
                                                pe_data_dir_offs + 6 * 8)

        else:

            te_header = curr
            print('TE image base at 0x%08X' % te_header)
            te_header_size = 0xa * 0x4
            stripped_size = self.ord2(te_header + 6)
            code_off = self.ord4(te_header + 0xc)
            pe_start_of_file = te_header + te_header_size - stripped_size
            pe_image_base = pe_start_of_file
            pe_debug_dir_entry_offs = self.ord4(te_header + (0x8 * 0x4))

        if self.ord2(pe_image_base + pe_debug_dir_entry_offs + 0xc) != 0x2:
            print('ERROR: no debug info !')
            return

        val = self.ord4(pe_image_base + pe_debug_dir_entry_offs + 0x14)
        if val:
            baseptr = pe_image_base + val
        else:
            baseptr = pe_image_base + self.ord4(pe_image_base +
                                                pe_debug_dir_entry_offs + 0x18)
        sig = self.ord4(baseptr)
        if sig == 0x3031424e:
            baseptr += 0x10
            print("NB10 signature at 0x%X" % baseptr)
        elif sig == 0x53445352:
            baseptr += 0x18
            print("RSDS signature at 0x%X" % baseptr)
        else:
            print("Unknown debug signature")
            return

        text_base = pe_image_base + code_off
        dbgpath = self.gets(baseptr)[0:-3] + 'debug'
        print("Loading symbol %s at 0x%x" % (dbgpath, pe_image_base))
        gdb.execute("symbol-file")
        gdb.execute("add-symbol-file %s 0x%08x" % (dbgpath, text_base))


LoadThis()
