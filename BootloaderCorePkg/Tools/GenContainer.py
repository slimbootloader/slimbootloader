#!/usr/bin/env python
## @ GenContainer.py
# Tools to operate on a container image
#
# Copyright (c) 2019 - 2022, Intel Corporation. All rights reserved.<BR>
# SPDX-License-Identifier: BSD-2-Clause-Patent
#
##
import sys
import argparse
import re
sys.dont_write_bytecode = True
from   ctypes import *
from   CommonUtility import *



class COMPONENT_ENTRY (Structure):
    _pack_ = 1
    _fields_ = [
        ('name',        ARRAY(c_char, 4)),   # SBL pod entry name
        ('offset',      c_uint32),   # Component offset in byte from the payload (data)        ('size',        c_uint32),   # Region/Component size in byte
        ('size',        c_uint32),   # Region/Component size in byte
        ('attribute',   c_uint8),    # Attribute:  BIT7 Reserved component entry
        ('alignment',   c_uint8),    # This image need to be loaded to memory  in (1 << Alignment) address
        ('auth_type',   c_uint8),    # Refer AUTH_TYPE_VALUE: 0 - "NONE"; 1- "SHA2_256";  2- "SHA2_384";  3- "RSA2048_PKCS1_SHA2_256"; 4 - RSA3072_PKCS1_SHA2_384;
                                     # 5 - RSA2048_PSS_SHA2_256; 6 - RSA3072_PSS_SHA2_384
        ('hash_size',   c_uint8)     # Hash data size, it could be image hash or public key hash
        ]

    _attr = {
        'RESERVED' : 0x80
    }

    def __new__(cls, buf = None):
        if buf is None:
            return Structure.__new__(cls)
        else:
            return cls.from_buffer_copy(buf)

    def __init__(self, buf = None):
        if buf is None:
            self.hash_data = bytearray()
        else:
            off = sizeof(COMPONENT_ENTRY)
            self.hash_data = bytearray(buf[off : off + self.hash_size])
        self.data      = bytearray()
        self.auth_data = bytearray()


class CONTAINER_HDR (Structure):
    _pack_ = 1
    _fields_ = [
        ('signature',    ARRAY(c_char, 4)), # Identifies structure
        ('version',      c_uint8),          # Header version
        ('svn',          c_uint8),          # Security version number
        ('data_offset',  c_uint16),         # Offset of payload (data) from header in byte
        ('data_size',    c_uint32),         # Size of payload (data) in byte
        ('auth_type',    c_uint8),          # Refer AUTH_TYPE_VALUE: 0 - "NONE"; 1- "SHA2_256";  2- "SHA2_384";  3- "RSA2048_PKCS1_SHA2_256"; 4 - RSA3072_PKCS1_SHA2_384;
                                            # 5 - RSA2048_PSS_SHA2_256; 6 - RSA3072_PSS_SHA2_384
        ('image_type',   c_uint8),          # 0: Normal
        ('flags',        c_uint8),          # BIT0: monolithic signing
        ('entry_count',  c_uint8),          # Number of entry in the header
        ]

    _flags = {
      'MONO_SIGNING' : 0x01
    }

    _image_type = {
      'NORMAL'     :  0x00,
      'CLASSIC'    :  0xF3,
      'MULTIBOOT'  :  0xF4,
    }

    def __new__(cls, buf = None):
        if buf is None:
            return Structure.__new__(cls)
        else:
            return cls.from_buffer_copy(buf)

    def __init__(self, buf = None):
        self.priv_key   = ''
        self.alignment  = 0x1000
        self.auth_data  = bytearray()
        self.comp_entry = []

        if buf is not None:
            # construct CONTAINER_HDR from existing buffer
            offset = sizeof(self)
            alignment = None
            for i in range(self.entry_count):
                component = COMPONENT_ENTRY(buf[offset:])
                if alignment is None:
                    alignment = 1 << component.alignment
                offset += (sizeof(component) + component.hash_size)
                comp_offset = component.offset + self.data_offset
                lz_hdr = LZ_HEADER.from_buffer(bytearray(buf[comp_offset:comp_offset + sizeof(LZ_HEADER)]))
                auth_offset = comp_offset + lz_hdr.compressed_len + sizeof(lz_hdr)
                component.data = bytearray (buf[comp_offset:auth_offset])
                auth_offset = get_aligned_value (auth_offset, 4)
                auth_size = CONTAINER.get_auth_size (component.auth_type, True)
                component.auth_data = bytearray (buf[auth_offset:auth_offset + auth_size])
                self.comp_entry.append (component)
            auth_size   = CONTAINER.get_auth_size (self.auth_type, True)
            auth_offset = get_aligned_value (offset, 4)
            self.auth_data = bytearray (buf[auth_offset:auth_offset + auth_size])
            if alignment is not None:
                self.alignment = alignment

class CONTAINER ():
    _struct_display_indent = 18
    _auth_type_value = {
            "NONE"                       : 0,
            "SHA2_256"                   : 1,
            "SHA2_384"                   : 2,
            "RSA2048_PKCS1_SHA2_256"     : 3,
            "RSA3072_PKCS1_SHA2_384"     : 4,
            "RSA2048_PSS_SHA2_256"       : 5,
            "RSA3072_PSS_SHA2_384"       : 6,
        }

    _auth_to_hashalg_str = {
        "NONE"                       : "NONE",
        "SHA2_256"                   : "SHA2_256",
        "SHA2_384"                   : "SHA2_384",
        "RSA2048_PKCS1_SHA2_256"     : "SHA2_256",
        "RSA3072_PKCS1_SHA2_384"     : "SHA2_384",
        "RSA2048_PSS_SHA2_256"       : "SHA2_256",
        "RSA3072_PSS_SHA2_384"       : "SHA2_384",
        }


    _auth_to_signscheme_str = {
        "NONE"                       : "",
        "SHA2_256"                   : "",
        "SHA2_384"                   : "",
        "RSA2048_PKCS1_SHA2_256"     : "RSA_PKCS1",
        "RSA3072_PKCS1_SHA2_384"     : "RSA_PKCS1",
        "RSA2048_PSS_SHA2_256"       : "RSA_PSS",
        "RSA3072_PSS_SHA2_384"       : "RSA_PSS",
        }

    def __init__(self, buf = None):
        self.out_dir   = '.'
        self.input_dir = '.'
        self.key_dir   = '.'
        self.tool_dir  = '.'
        if buf is None:
            self.header = CONTAINER_HDR ()
        else:
            self.header = CONTAINER_HDR (buf)
            # Check if image type is valid
            image_type_str = CONTAINER.get_image_type_str(self.header.image_type)

    def init_header (self, signature, alignment, image_type = 'NORMAL'):
        self.header.signature  = signature
        self.header.version    = 1
        self.header.alignment  = alignment
        self.header.flags      = 0
        if image_type not in CONTAINER_HDR._image_type.keys():
            raise Exception ("Invalid image type '%s' specified !" % image_type)
        self.header.image_type = CONTAINER_HDR._image_type[image_type]

    @staticmethod
    def get_image_type_str (image_type_val):
        try:
            image_type_str = next((key for key, value in CONTAINER_HDR._image_type.items() if value == image_type_val))
        except StopIteration:
            raise Exception ("Unknown image type value 0x%x in container header !" % image_type_val)
        return image_type_str

    @staticmethod
    def get_auth_type_val (auth_type_str):
        return CONTAINER._auth_type_value[auth_type_str]

    @staticmethod
    def get_auth_type_str (auth_type_val):
        try:
            auth_type_str = next(k for k, v in CONTAINER._auth_type_value.items() if v == auth_type_val)
        except StopIteration:
            raise Exception ("Unknown auth type value 0x%x !" % auth_type_val)
        return auth_type_str

    @staticmethod
    def get_auth_size (auth_type, signed = False):
        # calculate the length for the required authentication info
        if type(auth_type) is type(1):
            auth_type_str = CONTAINER.get_auth_type_str (auth_type)
        else:
            auth_type_str = auth_type
        if  auth_type_str == 'NONE':
            auth_len = 0
        elif auth_type_str.startswith ('RSA'):
            auth_len = int(auth_type_str[3:7]) >> 3
            if signed:
                auth_len = auth_len * 2 + sizeof(PUB_KEY_HDR) + sizeof(SIGNATURE_HDR) +  4
        elif auth_type_str.startswith ('SHA2_'):
            auth_len = int(auth_type_str[5:]) >> 3
            if signed:
                auth_len = 0
        else:
            raise Exception ("Unsupported authentication type '%s' !" % auth_type)
        return auth_len

    @staticmethod
    def decode_field (name, val):
        # decode auth type into readable string
        extra = ''
        if name in ['CONTAINER_HDR.auth_type', 'COMPONENT_ENTRY.auth_type']:
            auth_type = next(k for k, v in CONTAINER._auth_type_value.items() if v == val)
            extra = '%d : %s' % (val, auth_type)
        return extra

    @staticmethod
    def hex_str (data, name = ''):
        # convert bytearray to hex string
        dlen = len(data)
        if dlen == 0:
            hex_str = ''
        else:
            if dlen <= 16:
                hex_str = ' '.join(['%02x' % x for x in data])
            else:
                hex_str = ' '.join(['%02x' % x for x in data[:8]]) + \
                ' .... ' + ' '.join(['%02x' % x for x in data[-8:]])
        hex_str = '  %s %s [%s]' % (name, ' ' * (CONTAINER._struct_display_indent - len(name) + 1), hex_str)
        if len(data) > 0:
            hex_str = hex_str + ' (len=0x%x)' % len(data)
        return hex_str

    @staticmethod
    def output_struct (obj, indent = 0, plen = 0):
        # print out a struct info
        body = '' if indent else (' ' * indent + '<%s>:\n') % obj.__class__.__name__
        if plen == 0:
            plen = sizeof(obj)
        pstr = ('  ' * (indent + 1) + '{0:<%d} = {1}\n') % CONTAINER._struct_display_indent
        for field in obj._fields_:
            key = field[0]
            val = getattr(obj, key)
            rep = ''
            if type(val) is str:
                rep = "0x%X ('%s')" % (bytes_to_value(bytearray(val)), val)
            elif type(val) in [int]:
                rep = CONTAINER.decode_field ('%s.%s' % (obj.__class__.__name__, key), val)
                if not rep:
                    rep = '0x%X' % (val)
            else:
                rep = str(val)
            plen -= sizeof(field[1])
            body += pstr.format(key, rep)
            if plen <= 0:
                break
        return body.strip()

    @staticmethod
    def get_pub_key_hash (key, hash_type):
        # calculate publish key hash
        dh = bytearray (key)[sizeof(PUB_KEY_HDR):]
        if hash_type == 'SHA2_256':
            return bytearray(hashlib.sha256(dh).digest())
        elif hash_type == 'SHA2_384':
            return bytearray(hashlib.sha384(dh).digest())
        else:
            raise Exception ("Unsupported hash type in get_pub_key_hash!")

    @staticmethod
    def calculate_auth_data (file, auth_type, priv_key, out_dir):
        # calculate auth info for a given file
        hash_data = bytearray()
        auth_data = bytearray()
        basename = os.path.basename (file)
        if auth_type in ['NONE']:
            pass
        elif auth_type in ["SHA2_256"]:
            data = get_file_data (file)
            hash_data.extend (hashlib.sha256(data).digest())
        elif auth_type in ["SHA2_384"]:
            data = get_file_data (file)
            hash_data.extend (hashlib.sha384(data).digest())
        elif auth_type in ['RSA2048_PKCS1_SHA2_256', 'RSA3072_PKCS1_SHA2_384', 'RSA2048_PSS_SHA2_256', 'RSA3072_PSS_SHA2_384' ]:
            auth_type = adjust_auth_type (auth_type, priv_key)
            pub_key = os.path.join(out_dir, basename + '.pub')
            di = gen_pub_key (priv_key, pub_key)
            key_hash = CONTAINER.get_pub_key_hash (di, CONTAINER._auth_to_hashalg_str[auth_type])
            hash_data.extend (key_hash)
            out_file = os.path.join(out_dir, basename + '.sig')
            rsa_sign_file (priv_key, pub_key, CONTAINER._auth_to_hashalg_str[auth_type], CONTAINER._auth_to_signscheme_str[auth_type], file, out_file, False, True)
            auth_data.extend (get_file_data(out_file))
        else:
            raise Exception ("Unsupport AuthType '%s' !" % auth_type)
        return hash_data, auth_data


    def set_dir_path(self, out_dir, inp_dir, key_dir, tool_dir):
        self.out_dir   = out_dir
        self.inp_dir   = inp_dir
        self.key_dir   = key_dir
        self.tool_dir  = tool_dir

    def set_header_flags (self, flags, overwrite = False):
        if overwrite:
            self.header.flags  = flags
        else:
            self.header.flags |= flags

    def set_header_svn_info (self, svn):
        self.header.svn = svn

    def set_header_auth_info (self, auth_type_str = None, priv_key = None):
        if priv_key is not None:
            self.header.priv_key   = priv_key

        if auth_type_str is not None:
            self.header.auth_type = CONTAINER.get_auth_type_val (auth_type_str)
            auth_size = CONTAINER.get_auth_size (self.header.auth_type, True)
            self.header.auth_data = b'\xff' * auth_size

    def get_header_size (self):
        length = sizeof (self.header)
        for comp in self.header.comp_entry:
            length += comp.hash_size
        length += sizeof(COMPONENT_ENTRY) * len(self.header.comp_entry)
        length += len(self.header.auth_data)
        return length

    def get_auth_data (self, comp_file, auth_type_str):
        # calculate auth info for a give component file with specified auth type
        auth_size = CONTAINER.get_auth_size (auth_type_str, True)
        file_data = bytearray(get_file_data (comp_file))
        auth_data = None
        hash_data = bytearray()

        if len(file_data) < sizeof (LZ_HEADER):
            return file_data, hash_data, auth_data

        lz_header = LZ_HEADER.from_buffer(file_data)
        data      = bytearray()
        if lz_header.signature in LZ_HEADER._compress_alg:
            offset = sizeof(lz_header) + get_aligned_value (lz_header.compressed_len)
            if len(file_data) == auth_size + offset:
                auth_data = file_data[offset:offset+auth_size]
                data = file_data[:sizeof(lz_header) + lz_header.compressed_len]
                if auth_type_str in ["SHA2_256"]:
                    hash_data.extend (hashlib.sha256(data).digest())
                if auth_type_str in ["SHA2_384"]:
                    hash_data.extend (hashlib.sha384(data).digest())
                elif auth_type_str in ['RSA2048', 'RSA3072']:
                    offset += ((CONTAINER.get_auth_size (auth_type_str)))
                    key_hash = self.get_pub_key_hash (file_data[offset:])
                    hash_data.extend (key_hash)
                else:
                    raise Exception ("Unsupport AuthType '%s' !" % auth_type)
        return data, hash_data, auth_data

    def adjust_header (self):
        # finalize the container
        header = self.header
        header.entry_count = len(header.comp_entry)
        alignment = header.alignment - 1
        header.data_offset = (self.get_header_size() + alignment) & ~alignment
        if header.entry_count > 0:
            length = header.comp_entry[-1].offset + header.comp_entry[-1].size
            header.data_size   = (length + alignment) & ~alignment
        else:
            header.data_size   = 0
        auth_type = self.get_auth_type_str (header.auth_type)
        basename = header.signature.decode()
        hdr_file = os.path.join(self.out_dir, basename + '.hdr')
        hdr_data = bytearray (header)
        for component in header.comp_entry:
            hdr_data.extend (component)
            hdr_data.extend (component.hash_data)
        gen_file_from_object (hdr_file, hdr_data)
        hash_data, auth_data = CONTAINER.calculate_auth_data (hdr_file, auth_type, header.priv_key, self.out_dir)
        if len(auth_data) != len(header.auth_data):
            print (len(auth_data) , len(header.auth_data))
            raise Exception ("Unexpected authentication data length for container header !")
        header.auth_data = auth_data

    def get_data (self):
        # Prepare data buffer
        header = self.header
        data = bytearray(header)
        for component in header.comp_entry:
            data.extend (component)
            data.extend (component.hash_data)
        padding = b'\xff' *  get_padding_length (len(data))
        data.extend(padding + header.auth_data)
        for component in header.comp_entry:
            offset = component.offset + header.data_offset
            data.extend (b'\xff' * (offset - len(data)))
            comp_data = bytearray(component.data)
            padding = b'\xff' * get_padding_length (len(comp_data))
            comp_data.extend (padding + component.auth_data)
            if len(comp_data) > component.size:
                raise Exception ("Component '%s' needs space 0x%X, but region size is 0x%X !" % (component.name.decode(), len(comp_data), component.size))
            data.extend (comp_data)
        offset = header.data_offset + header.data_size
        data.extend (b'\xff' * (offset - len(data)))
        return data

    def locate_component (self, comp_name):
        component = None
        for each in self.header.comp_entry:
            if each.name.decode() == comp_name.upper():
                component = each
                break;
        return component

    def dump (self):
        print ('%s' % self.output_struct (self.header))
        print (self.hex_str (self.header.auth_data, 'auth_data'))
        for component in self.header.comp_entry:
            print ('%s' % self.output_struct (component))
            print (self.hex_str (component.hash_data, 'hash_data'))
            print (self.hex_str (component.auth_data, 'auth_data'))
            print (self.hex_str (component.data, 'data') + ' %s' % str(component.data[:4].decode()))

    def create (self, layout):

        # for monolithic signing, need to add a reserved _SG_ entry to hold the auth info
        mono_sig = '_SG_'
        is_mono_signing = True if layout[-1][0] == mono_sig else False

        # get the first entry in layout as CONTAINER_HDR
        container_sig, container_file, image_type, auth_type, key_file, alignment, region_size, svn = layout[0]

        if alignment == 0:
            alignment = 0x1000

        if auth_type == '':
            auth_type = 'NONE'

        if image_type == '':
            image_type = 'NORMAL'

        if container_file == '':
            container_file = container_sig + '.bin'
        key_path  = os.path.join(self.key_dir, key_file)
        if os.path.isfile (key_path):
            auth_type = adjust_auth_type (auth_type, key_path)

        # build header
        self.init_header (container_sig.encode(), alignment, image_type)
        self.set_header_auth_info (auth_type, key_path)
        self.set_header_svn_info (svn)

        name_set = set()
        is_last_entry = False
        for name, file, compress_alg, auth_type, key_file, alignment, region_size, svn in layout[1:]:
            if is_last_entry:
                raise Exception ("'%s' must be the last entry in layout for monolithic signing!" % mono_sig)
            if compress_alg == '':
                compress_alg = 'Dummy'
            if auth_type == '':
                auth_type = 'NONE'

            # build a component entry
            component = COMPONENT_ENTRY ()
            component.name      = name.encode()
            if alignment == 0:
                component.alignment = self.header.alignment.bit_length() - 1
            else:
                component.alignment = alignment.bit_length() - 1
            component.attribute = 0
            component.auth_type = self.get_auth_type_val (auth_type)
            key_file = os.path.join (self.key_dir, key_file)
            if file:
                if os.path.isabs(file):
                    in_file = file
                else:
                    for tst in [self.inp_dir, self.out_dir]:
                        in_file = os.path.join(tst, file)
                        if os.path.isfile(in_file):
                            break
                if not os.path.isfile(in_file):
                    raise Exception ("Component file path '%s' is invalid !" % file)
            else:
                in_file = os.path.join(self.out_dir, component.name.decode() + '.bin')
                gen_file_with_size (in_file, 0)
                if component.name == mono_sig.encode():
                    component.attribute = COMPONENT_ENTRY._attr['RESERVED']
                    compress_alg        = 'Dummy'
                    is_last_entry       = True

            # compress the component
            lz_file = compress (in_file, compress_alg, svn, self.out_dir, self.tool_dir)
            component.data = bytearray(get_file_data (lz_file))

            # calculate the component auth info
            component.hash_data, component.auth_data = CONTAINER.calculate_auth_data (lz_file, auth_type, key_file, self.out_dir)
            component.hash_size = len(component.hash_data)
            if region_size == 0:
                # arrange the region size automatically
                region_size = len(component.data)
                region_size = get_aligned_value (region_size, 4) + len(component.auth_data)
                if  is_mono_signing:
                    region_size = get_aligned_value (region_size, self.header.alignment)
                else:
                    region_size = get_aligned_value (region_size, (1 << component.alignment))
            component.size = region_size
            name_set.add (component.name)
            self.header.comp_entry.append (component)

        if len(name_set) != len(self.header.comp_entry):
            raise Exception ("Found duplicated component names in a container !")

        # calculate the component offset based on alignment requirement
        base_offset = None
        offset = self.get_header_size ()
        for component in self.header.comp_entry:
            alignment = (1 << component.alignment) - 1
            next_offset  = (offset + alignment) & ~alignment
            if is_mono_signing and  (next_offset - offset >=  sizeof(LZ_HEADER)):
                offset = next_offset - sizeof(LZ_HEADER)
            else:
                offset = next_offset
            if base_offset is None:
                base_offset = offset
            component.offset = offset - base_offset
            offset += component.size

        if is_mono_signing:
            # for monolithic signing, set proper flags and update header
            self.set_header_flags (CONTAINER_HDR._flags['MONO_SIGNING'])
            self.adjust_header ()

            # update auth info for last _SG_ entry
            data = self.get_data ()[self.header.data_offset:]
            pods_comp = self.header.comp_entry[-1]
            pods_data = data[:pods_comp.offset]
            gen_file_from_object (in_file, pods_data)
            pods_comp.hash_data, pods_comp.auth_data = CONTAINER.calculate_auth_data (in_file, auth_type, key_file, self.out_dir)

        self.adjust_header ()
        data = self.get_data ()

        out_file = os.path.join(self.out_dir, container_file)
        gen_file_from_object (out_file, data)

        return out_file

    def replace (self, comp_name, comp_file, comp_alg, key_file, svn, new_name):
        if self.header.flags & CONTAINER_HDR._flags['MONO_SIGNING']:
            raise Exception ("Counld not replace component for monolithically signed container!")

        component = self.locate_component (comp_name)
        if not component:
            raise Exception ("Counld not locate component '%s' in container !" % comp_name)
        if comp_alg == '':
            # reuse the original compression alg
            lz_header = LZ_HEADER.from_buffer(component.data)
            comp_alg  = LZ_HEADER._compress_alg[lz_header.signature]
        else:
            comp_alg = comp_alg[0].upper() + comp_alg[1:]

        # verify the new component hash does match the hash stored in the container header
        auth_type_str = self.get_auth_type_str (component.auth_type)
        data, hash_data, auth_data = self.get_auth_data (comp_file, auth_type_str)
        if auth_data is None:
            lz_file = compress (comp_file, comp_alg, svn, self.out_dir, self.tool_dir)
            if auth_type_str.startswith ('RSA') and key_file == '':
                raise Exception ("Signing key needs to be specified !")
            hash_data, auth_data = CONTAINER.calculate_auth_data (lz_file, auth_type_str, key_file, self.out_dir)
            data = get_file_data (lz_file)
        component.data = bytearray(data)
        component.auth_data = bytearray(auth_data)
        if component.hash_data != bytearray(hash_data):
            raise Exception ('Compoent hash does not match the one stored in container header !')

        # create the final output file
        data = self.get_data ()
        if new_name == '':
            new_name = self.header.signature + '.bin'
        out_file = os.path.join(self.out_dir, new_name)
        gen_file_from_object (out_file, data)

        return out_file

    def extract (self, name = '', file_path = ''):
        if name == '':
            # extract all components inside a container
            # so creat a layout file first
            if file_path == '':
                file_name = self.header.signature + '.bin'
            else:
                file_name = os.path.splitext(os.path.basename (file_path))[0] + '.bin'

            # create header entry
            auth_type_str = self.get_auth_type_str (self.header.auth_type)
            match = re.match('RSA(\d+)_', auth_type_str)
            if match:
                key_file = 'KEY_ID_CONTAINER_RSA%s' % match.group(1)
            else:
                key_file = ''
            alignment = self.header.alignment
            image_type_str = CONTAINER.get_image_type_str(self.header.image_type)
            header = ['%s' % self.header.signature.decode(), file_name, image_type_str,  auth_type_str,  key_file]
            layout = [(' Name', ' ImageFile', ' CompAlg', ' AuthType',  ' KeyFile', ' Alignment', ' Size', 'Svn')]
            layout.append(tuple(["'%s'" % x for x in header] + ['0x%x' % alignment, '0', '0x%x' % self.header.svn]))
            # create component entry
            for component in self.header.comp_entry:
                auth_type_str = self.get_auth_type_str (component.auth_type)
                match = re.match('RSA(\d+)_', auth_type_str)
                if match:
                    key_file = 'KEY_ID_CONTAINER_COMP_RSA%s' % match.group(1)
                else:
                    key_file = ''
                lz_header = LZ_HEADER.from_buffer(component.data)
                alg = LZ_HEADER._compress_alg[lz_header.signature]
                svn = lz_header.svn
                if component.attribute & COMPONENT_ENTRY._attr['RESERVED']:
                    comp_file = ''
                else:
                    comp_file = component.name.decode() + '.bin'
                comp = [component.name.decode(), comp_file, alg,  auth_type_str,  key_file]
                layout.append(tuple(["'%s'" % x for x in comp] + ['0x%x' % (1 << component.alignment), '0x%x' % component.size, '0x%x' % svn]))

            # write layout file
            layout_file = os.path.join(self.out_dir, self.header.signature.decode() + '.txt')
            fo = open (layout_file, 'w')
            fo.write ('# Container Layout File\n#\n')
            for idx, each in enumerate(layout):
                line = ' %-6s, %-16s, %-10s, %-24s, %-32s, %-10s, %-10s, %-10s' % each
                if idx == 0:
                    line = '#  %s\n' % line
                else:
                    line = '  (%s),\n' % line
                fo.write (line)
                if idx == 0:
                    line = '# %s\n' % ('=' * 136)
                    fo.write (line)
            fo.close()

        for component in self.header.comp_entry:
            if component.attribute & COMPONENT_ENTRY._attr['RESERVED']:
                continue
            # creat individual component region and image binary
            if (component.name.decode() == name) or (name == ''):
                basename = os.path.join(self.out_dir, '%s' % component.name.decode())
                sig_file = basename + '.rgn'
                sig_data = component.data + b'\xff' * get_padding_length (len(component.data)) + component.auth_data
                gen_file_from_object (sig_file, sig_data)

                bin_file = basename + '.bin'
                lz_header = LZ_HEADER.from_buffer(component.data)
                signature = lz_header.signature
                if signature in [b'LZDM']:
                    offset = sizeof(lz_header)
                    data = component.data[offset : offset + lz_header.compressed_len]
                    gen_file_from_object (bin_file, data)
                elif signature in [b'LZMA', b'LZ4 ']:
                    decompress (sig_file, bin_file, self.tool_dir)
                else:
                    raise Exception ("Unknown LZ format!")

def gen_container_bin (container_list, out_dir, inp_dir, key_dir = '.', tool_dir = ''):
    for each in container_list:
        container = CONTAINER ()
        container.set_dir_path (out_dir, inp_dir, key_dir, tool_dir)
        out_file = container.create (each)
        print ("Container '%s' was created successfully at:  \n  %s" % (container.header.signature.decode(), out_file))

def adjust_auth_type (auth_type_str, key_path):
    if os.path.exists(key_path):
        sign_key_type = get_key_type(key_path)
        if auth_type_str != '':
            sign_scheme = CONTAINER._auth_to_signscheme_str[auth_type_str]
        else:
            # Set to default signing scheme if auth type is generated.
            sign_scheme = 'RSA_PSS'
        auth_type, hash_type = get_auth_hash_type (sign_key_type, sign_scheme)
        if auth_type_str and (auth_type != auth_type_str):
            print ("Override auth type to '%s' in order to match the private key type !" % auth_type)
        auth_type_str = auth_type

    return auth_type_str

def gen_layout (comp_list, img_type, auth_type_str, svn, out_file, key_dir, key_file):
    hash_type = CONTAINER._auth_to_hashalg_str[auth_type_str] if auth_type_str else ''
    auth_type = auth_type_str
    key_path  = os.path.join(key_dir, key_file)
    auth_type = adjust_auth_type (auth_type, key_path)
    if auth_type == '':
        raise Exception ("'auth' parameter is expected  !")

    # prepare the layout from individual components from '-cl'
    if img_type not in CONTAINER_HDR._image_type.keys():
        raise Exception ("Invalid Container Type '%s' !" % img_type)
    layout = "('BOOT', '%s', '%s', '%s' , '%s', 0x10, 0, %s),\n" % (out_file, img_type, auth_type, key_file, svn)
    end_layout = "('_SG_', '', 'Dummy', '%s', '', 0, 0, %s)," % (hash_type, svn)
    for idx, each in enumerate(comp_list):
        parts = each.split(':')
        comp_name = parts[0]
        if len(comp_name) != 4:
            raise Exception ("Invalid component string format '%s' !" % each)

        if (len(parts)) > 2:
            comp_file = ':'.join(parts[1:2])
            com_svn   = ':'.join(parts[2:])
        else:
            comp_file = ':'.join(parts[1:])
            com_svn   = 0    # set to default  svn

        if comp_name == 'INRD':
            align = 0x1000
        else:
            align = 0
        layout += "('%s', '%s', 'Dummy', 'NONE', '', %s, 0, %s),\n" % (comp_name, comp_file, align, com_svn)
    layout += end_layout
    return layout

def create_container (args):
    layout = ""
    # if '-l', get the layout content directly
    # if '-cl' prepare the layout

    #extract key dir and file
    key_path = os.path.abspath(args.key_path)
    if os.path.isdir(key_path):
        key_dir  = key_path
        key_file = ''
    else:
        key_dir  = os.path.dirname(key_path)
        key_file = os.path.basename(key_path)

    #extract out dir and file
    out_path = os.path.abspath(args.out_path)
    if os.path.isdir(out_path):
        out_dir  = out_path
        out_file = ''
    else:
        out_dir  = os.path.dirname(out_path)
        out_file = os.path.basename(out_path)

    if args.layout:
        # Using layout file
        layout = get_file_data(args.layout, 'r')
    else:
        # Using component list
        if not key_file:
            raise Exception ("key_path expects a key file path !")
        layout = gen_layout (args.comp_list, args.img_type, args.auth, args.svn, out_file, key_dir, key_file)
    container_list = eval ('[[%s]]' % layout.replace('\\', '/'))

    comp_dir = os.path.abspath(args.comp_dir)
    if not os.path.isdir(comp_dir):
        raise Exception ("'comp_dir' expects a directory path !")
    tool_dir = os.path.abspath(args.tool_dir)
    if not os.path.isdir(tool_dir):
        raise Exception ("'tool_dir' expects a directory path !")

    if out_file:
        # override the output file name
        hdr_entry = list (container_list[0][0])
        hdr_entry[1] = out_file
        container_list[0][0] = tuple(hdr_entry)

    if args.layout and args.auth:
        # override auth
        hdr_entry = list (container_list[0][0])
        hdr_entry[3] = args.auth
        container_list[0][0] = tuple(hdr_entry)

    gen_container_bin (container_list, out_dir, comp_dir, key_dir, tool_dir)

def extract_container (args):
    tool_dir = args.tool_dir if args.tool_dir else '.'
    data = get_file_data (args.image)
    container = CONTAINER (data)
    container.set_dir_path (args.out_dir, '.', '.', tool_dir)
    container.extract (args.comp_name, args.image)
    print ("Components were extraced successfully at:\n  %s" % args.out_dir)

def replace_component (args):
    tool_dir = args.tool_dir if args.tool_dir else '.'
    data = get_file_data (args.image)
    container = CONTAINER (data)
    out_path = os.path.abspath(args.out_image)
    out_dir  = os.path.dirname(out_path)
    out_file = os.path.basename(out_path)
    container.set_dir_path (out_dir, '.', '.', tool_dir)
    file = container.replace (args.comp_name, args.comp_file, args.compress, args.key_file, args.svn, out_file)
    print ("Component '%s' was replaced successfully at:\n  %s" % (args.comp_name, file))

def sign_component (args):
    compress_alg = args.compress
    compress_alg = compress_alg[0].upper() + compress_alg[1:]

    #extract out dir and file
    sign_file = os.path.abspath(args.out_file)
    out_dir   = os.path.dirname(sign_file)

    lz_file = compress (args.comp_file, compress_alg, args.svn, out_dir, args.tool_dir)
    data = bytearray(get_file_data (lz_file))
    hash_data, auth_data = CONTAINER.calculate_auth_data (lz_file, args.auth, args.key_file, out_dir)

    data.extend (b'\xff' * get_padding_length(len(data)))
    data.extend (auth_data)
    gen_file_from_object (sign_file, data)
    print ("Component file was signed successfully at:\n  %s" % sign_file)

def display_container (args):
    data = get_file_data (args.image)
    container = CONTAINER (data)
    container.dump ()

def main():
    parser = argparse.ArgumentParser()
    sub_parser = parser.add_subparsers(help='command')

    # Command for display
    cmd_display = sub_parser.add_parser('view', help='display a container image')
    cmd_display.add_argument('-i', dest='image',  type=str, required=True, help='Container input image')
    cmd_display.set_defaults(func=display_container)

    # Command for create
    cmd_display = sub_parser.add_parser('create', help='create a container image')
    group = cmd_display.add_mutually_exclusive_group (required=True)
    # '-l' or '-cl', one of them is mandatory
    group.add_argument('-l',  dest='layout',   type=str, help='Container layout input file if no -cl')
    group.add_argument('-cl', dest='comp_list',nargs='+', help='List of each component files, following XXXX:FileName format')
    cmd_display.add_argument('-t', dest='img_type',  type=str, default='CLASSIC', help='Container Image Type : [NORMAL, CLASSIC, MULTIBOOT]')
    cmd_display.add_argument('-o', dest='out_path',  type=str, default='.', help='Container output directory/file')
    cmd_display.add_argument('-k', dest='key_path',  type=str, default='', help='Input key directory/file. Use key directoy path when container layout -l option is used \
                                                                                 Use Key Id or key file path when component files with -cl option is specified')
    cmd_display.add_argument('-a',  dest='auth', choices=['SHA2_256', 'SHA2_384', 'RSA2048_PKCS1_SHA2_256',
                    'RSA3072_PKCS1_SHA2_384', 'RSA2048_PSS_SHA2_256', 'RSA3072_PSS_SHA2_384', 'NONE'], default='',  help='authentication algorithm')
    cmd_display.add_argument('-cd', dest='comp_dir', type=str, default='', help='Componet image input directory')
    cmd_display.add_argument('-td', dest='tool_dir', type=str, default='', help='Compression tool directory')
    cmd_display.add_argument('-s', dest='svn', type=int, default=0, help='Security version number for Container header')
    cmd_display.set_defaults(func=create_container)

    # Command for extract
    cmd_display = sub_parser.add_parser('extract', help='extract a component image')
    cmd_display.add_argument('-i',  dest='image',  type=str, required=True, help='Container input image path')
    cmd_display.add_argument('-n',  dest='comp_name',  type=str, default='', help='Component name to extract')
    cmd_display.add_argument('-od', dest='out_dir',  type=str, default='.', help='Output directory')
    cmd_display.add_argument('-td', dest='tool_dir', type=str, default='', help='Compression tool directory')
    cmd_display.set_defaults(func=extract_container)

    # Command for replace
    cmd_display = sub_parser.add_parser('replace', help='replace a component image')
    cmd_display.add_argument('-i',  dest='image',  type=str, required=True, help='Container input image path')
    cmd_display.add_argument('-o',  dest='out_image',  type=str, default='', help='Container new output image path')
    cmd_display.add_argument('-n',  dest='comp_name',  type=str, required=True, help='Component name to replace')
    cmd_display.add_argument('-f',  dest='comp_file',  type=str, required=True, help='Component input file path')
    cmd_display.add_argument('-c',  dest='compress', choices=['lz4', 'lzma', 'dummy'], default='dummy', help='compression algorithm')
    cmd_display.add_argument('-k',  dest='key_file',  type=str, default='', help='Key Id or Private key file path to sign component')
    cmd_display.add_argument('-td', dest='tool_dir', type=str, default='', help='Compression tool directory')
    cmd_display.add_argument('-s', dest='svn', type=int,  default=0, help='Security version number for Component')
    cmd_display.set_defaults(func=replace_component)

    # Command for sign
    cmd_display = sub_parser.add_parser('sign', help='compress and sign a component image')
    cmd_display.add_argument('-f',  dest='comp_file',  type=str, required=True, help='Component input file path')
    cmd_display.add_argument('-o',  dest='out_file',  type=str, default='', help='Signed output image path')
    cmd_display.add_argument('-c',  dest='compress', choices=['lz4', 'lzma', 'dummy'],  default='dummy', help='compression algorithm')
    cmd_display.add_argument('-a',  dest='auth', choices=['SHA2_256', 'SHA2_384', 'RSA2048_PKCS1_SHA2_256',
                'RSA3072_PKCS1_SHA2_384', 'RSA2048_PSS_SHA2_256', 'RSA3072_PSS_SHA2_384', 'NONE'], default='NONE',  help='authentication algorithm')
    cmd_display.add_argument('-k',  dest='key_file',  type=str, default='', help='Key Id or Private key file path to sign component')
    cmd_display.add_argument('-td', dest='tool_dir', type=str, default='',  help='Compression tool directory')
    cmd_display.add_argument('-s', dest='svn', type=int,  default=0, help='Security version number for Component')
    cmd_display.set_defaults(func=sign_component)

    # Parse arguments and run sub-command
    args = parser.parse_args()
    try:
        func = args.func
    except AttributeError:
        parser.error("too few arguments")

    # Additional check
    if args.func == sign_component:
        if args.auth.startswith('RSA') and args.key_file == '':
            parser.error("the following arguments are required: -k")

    func(args)


if __name__ == '__main__':
    sys.exit(main())
