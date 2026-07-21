#!/usr/bin/env python
## @ ResignSlimBootloader.py
# Re-sign a built SlimBootloader.bin using a replacement SBL key directory.
#
# Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
# SPDX-License-Identifier: BSD-2-Clause-Patent
#
# This tool discovers the flash layout, KEYH content, per-container key IDs,
# and Stage1A HashStore geometry directly from the input SlimBootloader.bin.
#
# It performs post-build re-signing without rebuilding the full image, while
# preserving the source image's existing layout, build-time signing strategy,
# compression algorithm, and SVN values.
#
# The tool updates the following image content:
#   - KEYH: rebuilds the key-hash container from the replacement key set.
#   - Master key: updates the master-key hash in Stage1A's HashStoreTable.
#   - Container: re-generates hash and signature data according to the existing
#     signing strategy while preserving each component's compressed payload
#     bytes.
#   - CFGDATA: re-signs and re-stitches CFGDATA in the image.
#
# Reuses existing in-tree code:
#   - BuildUtility.gen_pub_key_hash_store   (rebuilds the KEYH container)
#   - SingleSign.single_sign_gen_pub_key    (Master pub-key hash for Stage1A)
#   - CONTAINER.calculate_auth_data         (per-component / header hash + sign)
#   - CONTAINER.adjust_header               (reseals the container header)
#   - CfgDataTool.py                        (re-signs and re-stitches CFGDATA)
#
##

import argparse
import ctypes
import hashlib
import logging
import os
import re
import shutil
import subprocess
import sys
import tempfile

sys.dont_write_bytecode = True

SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
if SCRIPT_DIR not in sys.path:
    sys.path.insert(0, SCRIPT_DIR)

from CommonUtility import (                                          # noqa: E402
    HASH_DIGEST_SIZE,
    HASH_TYPE_VALUE,
    HASH_VAL_STRING,
    LZ_HEADER,
    get_file_data,
    gen_file_from_object,
)
from SingleSign import (                                             # noqa: E402
    SIGNING_KEY,
    get_key_from_store,
    single_sign_gen_pub_key,
)
from BuildUtility import (                                           # noqa: E402
    HashStoreData,
    HashStoreTable,
    HASH_USAGE,
    gen_pub_key_hash_store,
)
from GenContainer import (                                           # noqa: E402
    CONTAINER,
    CONTAINER_HDR,
)
from IfwiUtility import IFWI_PARSER                                  # noqa: E402


# ---------------------------------------------------------------------------
# Errors and logging
# ---------------------------------------------------------------------------

class ResignError(Exception):
    """Raised on any recoverable failure in the re-sign pipeline."""


LOG = logging.getLogger('resign-sbl')


def _configure_logging(verbosity):
    level = logging.WARNING
    if verbosity >= 1:
        level = logging.INFO
    logging.basicConfig(
        level=level,
        format='%(levelname)-7s %(message)s',
        stream=sys.stdout,
    )


def hash_all_leaves(image, inspector):
    """Return {leaf_path: (offset, length, sha256_hex)} for the hash log."""
    rows = {}
    for side in ImageInspector.SIDES:
        for name, ref in sorted(inspector.container_leaves[side].items()):
            rows['%s/%s' % (side, ref.path)] = (
                ref.node.offset, ref.node.length,
                _sha256(_leaf_bytes(image, ref.node)))
        s1a = inspector.stage1a[side]
        if s1a is not None:
            rows['%s/%s' % (side, s1a.path)] = (
                s1a.node.offset, s1a.node.length,
                _sha256(_leaf_bytes(image, s1a.node)))
        for cfg in inspector.cfgd_leaves[side]:
            rows['%s/%s' % (side, cfg.path)] = (
                cfg.node.offset, cfg.node.length,
                _sha256(_leaf_bytes(image, cfg.node)))
    return rows


def emit_hash_log(before_rows, after_rows, args, inspector, path):
    side_order = {
        'primary': 0,
        'backup': 1,
        'non_redundant': 2,
    }

    lines = ['# ResignSlimBootloader hash log',
             '#',
             '#   Input image : %s' % os.path.abspath(args.input_image),
             '#   Output image: %s' % os.path.abspath(args.output_image),
             '#   Key dir     : %s' % os.path.abspath(args.key_dir),
             '#   Master key  : %s (hash_alg=%s, scheme=%s)' % (
                 inspector.keyh.master_key_id,
                 inspector.keyh.hash_alg,
                 inspector.keyh.sign_scheme),
             '']
    for title, rows in (('BEFORE', before_rows), ('AFTER', after_rows)):
        lines.append('# %s' % title)
        lines.append('#   %-32s  %-10s  %-8s  %-64s' % ('LEAF', 'OFFSET', 'LEN', 'SHA256'))
        for key in sorted(rows, key=lambda leaf_key: (
                side_order.get(leaf_key.partition('/')[0], 99),
                -rows[leaf_key][0],
                leaf_key)):
            off, length, digest = rows[key]
            lines.append('#   %-32s  0x%08X  0x%06X  %s' % (key, off, length, digest))
        lines.append('')
    with open(path, 'w') as fh:
        fh.write('\n'.join(lines) + '\n')
    LOG.info('Hash log written to %s', path)


# ---------------------------------------------------------------------------
# Small helpers
# ---------------------------------------------------------------------------

_HASH_USAGE_NAME = {v: k for k, v in HASH_USAGE.items()}


def _hash_alg_for_digest_len(digest_len):
    """Reverse-lookup the SBL hash algorithm name from its digest length."""
    for name, size in HASH_DIGEST_SIZE.items():
        if size == digest_len:
            return name
    raise ResignError('Unrecognized hash digest length: %d' % digest_len)


def _looks_like_container(data):
    """Return True iff `data` starts with a plausible CONTAINER_HDR."""
    if len(data) < ctypes.sizeof(CONTAINER_HDR):
        return False
    if data[:4] == b'\xff\xff\xff\xff' or data[:4] == b'\x00\x00\x00\x00':
        return False
    # Signature must be four printable ASCII characters.
    for b in data[:4]:
        if b < 0x20 or b > 0x7E:
            return False
    try:
        hdr = CONTAINER_HDR.from_buffer_copy(bytes(data[:ctypes.sizeof(CONTAINER_HDR)]))
    except ValueError:
        return False
    # Sanity checks: entry_count > 0, data_offset >= header size, data fits.
    if hdr.entry_count == 0 or hdr.data_offset < ctypes.sizeof(CONTAINER_HDR):
        return False
    if hdr.data_offset + hdr.data_size > len(data):
        return False
    return True


def _sha256(data):
    return hashlib.sha256(bytes(data)).hexdigest()


def _leaf_bytes(image, node):
    return bytes(image[node.offset:node.offset + node.length])


def replace_leaf(image, leaf_ref, new_bytes):
    if len(new_bytes) > leaf_ref.node.length:
        raise ResignError("New %s size 0x%X exceeds region 0x%X"
                          % (leaf_ref.path, len(new_bytes), leaf_ref.node.length))
    padded = bytes(new_bytes) + b'\xff' * (leaf_ref.node.length - len(new_bytes))
    image[leaf_ref.node.offset:leaf_ref.node.offset + leaf_ref.node.length] = padded


# ---------------------------------------------------------------------------
# Image inspection
# ---------------------------------------------------------------------------

class LeafRef:
    """A leaf inside SlimBootloader.bin identified by IFWI path."""

    def __init__(self, path, node):
        self.path = path
        self.node = node

    @property
    def name(self):
        return self.node.name

    def __repr__(self):
        return 'LeafRef(%s @ 0x%08X len=0x%X)' % (self.path, self.node.offset, self.node.length)


class KeyhInfo:
    """Everything we recovered from the current KEYH container."""

    def __init__(self, container_sig, header_auth_type, master_key_id,
                 hash_alg, sign_scheme, svn, key_hash_list):
        self.container_sig    = container_sig     # 'KEYH'
        self.header_auth_type = header_auth_type  # e.g. 'RSA3072_PSS_SHA2_384'
        self.master_key_id    = master_key_id     # e.g. 'KEY_ID_MASTER_RSA3072'
        self.hash_alg         = hash_alg          # e.g. 'SHA2_384'
        self.sign_scheme      = sign_scheme       # 'RSA_PSS' or 'RSA_PKCS1'
        self.svn              = svn
        # List of tuples (usage_int, key_id_or_filename)
        self.key_hash_list    = key_hash_list


class ImageInspector:
    """Discovers layout, containers, KEYH content, and Stage1A hash slot.

    SBL's redundant BIOS layout has three logical "sides":

        primary       : partitions whose FLASH_MAP_REGION name ends in '0'
                        (typically TS0 = Stage1A primary, RD0 = other components
                        primary).
        backup        : partitions whose name ends in '1' (TS1, RD1).
        non_redundant : partitions like NRD, NVS (only exist once).

    Any leaf name appearing in both primary and backup is a redundant pair and
    is always updated symmetrically.
    """

    SIDES = ('primary', 'backup', 'non_redundant')

    # Region names known to be redundant top-swap or code partitions.
    _PRIMARY_PART_NAMES = ('TS0', 'RD0')
    _BACKUP_PART_NAMES  = ('TS1', 'RD1')

    def __init__(self, image_path):
        self.image_path = os.path.abspath(image_path)
        self.image_data = bytearray(get_file_data(self.image_path))
        self.root = IFWI_PARSER.parse_ifwi_binary(self.image_data)
        if self.root is None:
            raise ResignError("Cannot parse '%s' as an SBL/IFWI image" % image_path)

        # BIOS region root (typically IFWI/BIOS).
        bios = IFWI_PARSER.locate_component(self.root, 'IFWI/BIOS')
        if bios is None:
            raise ResignError("BIOS region not found in the input image")
        self.bios = bios

        # partitions[side] -> [top-level partition COMPONENT node]
        self.partitions = {s: [] for s in self.SIDES}
        self._classify_partitions()

        # container_leaves[side] : {leaf_name: LeafRef}
        # stage1a[side]          : LeafRef or None
        # cfgd_leaves[side]      : [LeafRef]
        self.container_leaves = {s: {} for s in self.SIDES}
        self.stage1a          = {s: None for s in self.SIDES}
        self.cfgd_leaves      = {s: [] for s in self.SIDES}

        self._discover_leaves()
        self.keyh = self._parse_keyh()
        self.stage1a_geometry = self._probe_stage1a_hash_slot()

    # -- layout discovery ---------------------------------------------------

    def _classify_partitions(self):
        """Walk BIOS children and put each partition into a side bucket."""
        for part in self.bios.child:
            name = part.name.upper()
            if name in self._PRIMARY_PART_NAMES:
                self.partitions['primary'].append(part)
            elif name in self._BACKUP_PART_NAMES:
                self.partitions['backup'].append(part)
            else:
                # Anything else (NRD, NVS, ...) is non-redundant.
                self.partitions['non_redundant'].append(part)

        if not self.partitions['primary']:
            raise ResignError("No primary BIOS partition (TS0/RD0) found")
        if not self.partitions['backup']:
            raise ResignError("No backup BIOS partition (TS1/RD1) found; "
                              "this tool assumes a redundant SBL layout")

    def _discover_leaves(self):
        """Walk each partition; classify every leaf we can identify."""
        # First pass: obvious Stage1A leaves (named SG1A under a TS* partition).
        for side, parts in self.partitions.items():
            for part_node in parts:
                if not part_node.name.upper().startswith('TS'):
                    continue
                for leaf in self._iter_leaves(part_node):
                    if leaf.name.upper() == 'SG1A' and self.stage1a[side] is None:
                        self.stage1a[side] = LeafRef(
                            '%s/%s' % (part_node.name, leaf.name), leaf)

        # Second pass: everything else (containers + CFGDATA).
        for side, parts in self.partitions.items():
            for part_node in parts:
                for leaf in self._iter_leaves(part_node):
                    data = _leaf_bytes(self.image_data, leaf)
                    name = leaf.name
                    leaf_path = '%s/%s' % (part_node.name, name)
                    ref = LeafRef(leaf_path, leaf)

                    # Skip the Stage1A leaf we already picked in pass 1.
                    if self.stage1a[side] is not None and \
                       ref.path == self.stage1a[side].path:
                        continue

                    # CFGDATA
                    if data[:4] in (b'CFGD', b'CNFG'):
                        self.cfgd_leaves[side].append(ref)
                        continue

                    # Stage1A fallback: TS* partition leaf that carries a
                    # HashStoreTable and has a plausible header. Only used
                    # if pass 1 did not find an SG1A-named leaf.
                    if self.stage1a[side] is None and \
                       part_node.name.upper().startswith('TS') and \
                       self._has_valid_hashstore_table(data):
                        self.stage1a[side] = ref
                        continue

                    # Container: parses as CONTAINER_HDR.
                    if _looks_like_container(data):
                        self.container_leaves[side][name] = ref
                        continue

        # We must have Stage1A on both redundant sides.
        for side in ('primary', 'backup'):
            if self.stage1a[side] is None:
                raise ResignError("Stage1A HashStoreTable not found on '%s' side" % side)

        # Sanity: primary and backup should carry the same container-leaf names.
        pri_names = set(self.container_leaves['primary'].keys())
        bak_names = set(self.container_leaves['backup'].keys())
        diff = pri_names.symmetric_difference(bak_names)
        if diff:
            LOG.warning('Primary/backup have asymmetric container leaves: %s',
                        sorted(diff))

    @staticmethod
    def _has_valid_hashstore_table(data):
        """True iff `data` contains a plausibly-structured HashStoreTable."""
        off = data.find(HashStoreTable.HASH_STORE_SIGNATURE)
        if off < 0 or off + ctypes.sizeof(HashStoreTable) > len(data):
            return False
        try:
            table = HashStoreTable.from_buffer_copy(
                bytes(data[off:off + ctypes.sizeof(HashStoreTable)]))
        except ValueError:
            return False
        # UsedLength must be sane: at least the header, at most the total.
        if table.UsedLength < ctypes.sizeof(HashStoreTable):
            return False
        if table.UsedLength > table.TotalLength:
            return False
        if off + table.UsedLength > len(data):
            return False
        # HeaderLength should equal the struct size (SBL convention).
        if table.HeaderLength != ctypes.sizeof(HashStoreTable):
            return False
        return True

    @staticmethod
    def _iter_leaves(node):
        """Yield every terminal-ish leaf (non-empty child list is not a leaf)."""
        stack = [node]
        while stack:
            n = stack.pop()
            if not n.child:
                yield n
            else:
                stack.extend(reversed(n.child))

    # -- KEYH parsing -------------------------------------------------------

    def _parse_keyh(self):
        """Decode the KEYH container from the primary side (any partition)."""
        keyh_ref = self.container_leaves['primary'].get('KEYH')
        if keyh_ref is None:
            raise ResignError("KEYH container not found on primary side. Cannot rotate keys.")
        keyh_bytes = _leaf_bytes(self.image_data, keyh_ref.node)
        container = CONTAINER(bytearray(keyh_bytes))
        hdr = container.header

        header_auth_type = CONTAINER.get_auth_type_str(hdr.auth_type)
        m = re.match(r'RSA(\d+)_(PKCS1|PSS)_(SHA2_\d+)$', header_auth_type)
        if not m:
            raise ResignError("Unexpected KEYH header auth type '%s'" % header_auth_type)
        key_size = m.group(1)
        scheme   = 'RSA_' + m.group(2)
        hash_alg = m.group(3)
        master_key_id = 'KEY_ID_MASTER_RSA%s' % key_size

        # Extract the _HS_ component data and decode HashStoreData entries.
        hs_comp = container.locate_component('_HS_')
        if hs_comp is None:
            raise ResignError("KEYH container has no '_HS_' component")
        # component.data is <LZ_HEADER><payload>. Strip the LZ header.
        from CommonUtility import LZ_HEADER
        lz_hdr = LZ_HEADER.from_buffer_copy(bytes(hs_comp.data[:ctypes.sizeof(LZ_HEADER)]))
        payload = bytes(hs_comp.data[ctypes.sizeof(LZ_HEADER):
                                     ctypes.sizeof(LZ_HEADER) + lz_hdr.compressed_len])

        key_hash_list = []
        # Duplicate-usage handling: same usage may appear multiple times.
        # Two idioms exist:
        #   - NVLP-style: same PUBKEY_OS registered at multiple key sizes
        #     (all map to OS1, just different sizes).
        #   - OS1/OS2 style: two PUBKEY_OS entries with the same key size
        #     (map to OS1 and OS2 respectively).
        # We track (usage_name, key_size) tuples already assigned and only
        # promote to OS2 when the exact (usage, size) pair was already used
        # for OS1.
        assigned = set()
        off = 0
        while off + ctypes.sizeof(HashStoreData) <= len(payload):
            entry = HashStoreData.from_buffer_copy(payload[off:off + ctypes.sizeof(HashStoreData)])
            if entry.DigestLen == 0:
                break
            usage_name = _HASH_USAGE_NAME.get(entry.Usage)
            if usage_name is None:
                LOG.warning('Unknown PUBKEY_* usage 0x%X in existing KEYH; skipping', entry.Usage)
                off += ctypes.sizeof(HashStoreData) + entry.DigestLen
                continue
            # Per-entry key size derived from the digest length.
            if entry.DigestLen == 32:
                entry_key_size = '2048'
            elif entry.DigestLen == 48:
                entry_key_size = '3072'
            else:
                LOG.warning('Unexpected digest length %d for usage %s; '
                            'defaulting to master key size %s',
                            entry.DigestLen, usage_name, key_size)
                entry_key_size = key_size
            # Promote to alternate template only if the (usage, size) pair
            # is already taken (i.e. we've already emitted the primary
            # KEY_ID for this exact combination).
            occurrence = 1 if (usage_name, entry_key_size) in assigned else 0
            key_id = self._infer_key_id_from_usage(usage_name, entry_key_size, occurrence)
            assigned.add((usage_name, entry_key_size))
            key_hash_list.append((entry.Usage, key_id))
            off += ctypes.sizeof(HashStoreData) + entry.DigestLen

        if not key_hash_list:
            raise ResignError("KEYH container declares no PUBKEY entries")

        return KeyhInfo(
            container_sig    = hdr.signature.decode(),
            header_auth_type = header_auth_type,
            master_key_id    = master_key_id,
            hash_alg         = hash_alg,
            sign_scheme      = scheme,
            svn              = hdr.svn,
            key_hash_list    = key_hash_list,
        )

    @staticmethod
    def _infer_key_id_from_usage(usage_name, key_size, occurrence=0):
        """Map a PUBKEY_* usage to the standard SBL KEY_ID_* naming.

        `occurrence` disambiguates repeated usages (two PUBKEY_OS entries
        commonly map to OS1 and OS2; some platforms register OS1 at multiple
        key sizes instead).
        """
        # Names follow BootloaderCorePkg/Tools/GenerateKeys.py conventions.
        mapping = {
            'PUBKEY_CFG_DATA':  ['KEY_ID_CFGDATA_RSA%s'],
            'PUBKEY_FWU':       ['KEY_ID_FIRMWAREUPDATE_RSA%s'],
            'PUBKEY_OS':        ['KEY_ID_OS1_PUBLIC_RSA%s',
                                 'KEY_ID_OS2_PUBLIC_RSA%s'],
            'PUBKEY_CONT_DEF':  ['KEY_ID_CONTAINER_RSA%s'],
            'PUBKEY_MASTER':    ['KEY_ID_MASTER_RSA%s'],
        }
        if usage_name in mapping:
            templates = mapping[usage_name]
            template = templates[occurrence] if occurrence < len(templates) \
                       else templates[0]
            key_id = template % key_size
            # Fall back to OS1 if OS2 file is not present in SBL_KEY_DIR.
            if 'OS2_PUBLIC' in key_id:
                sbl_key_dir = os.environ.get('SBL_KEY_DIR', '')
                key_file = SIGNING_KEY.get(key_id, '')
                if sbl_key_dir and key_file and \
                   not os.path.exists(os.path.join(sbl_key_dir, key_file)):
                    fallback = 'KEY_ID_OS1_PUBLIC_RSA%s' % key_size
                    LOG.warning("KEY_ID '%s' file not found in SBL_KEY_DIR; "
                                "falling back to '%s'", key_id, fallback)
                    key_id = fallback
            return key_id
        # OEM slots: image only carries their hash, not a canonical key_id.
        # Operator must supply the concrete file separately (unsupported).
        return usage_name  # placeholder; will trigger a clear error later

    # -- Stage1A HashStoreTable -------------------------------------------

    def _probe_stage1a_hash_slot(self):
        """Locate the PUBKEY_MASTER slot inside Stage1A's HashStoreTable.

        Returns (relative_offset_into_stage1a, digest_len, hash_alg_name).
        """
        ref = self.stage1a['primary']
        blob = bytearray(_leaf_bytes(self.image_data, ref.node))
        hs_off = blob.find(HashStoreTable.HASH_STORE_SIGNATURE)
        if hs_off < 0:
            raise ResignError("HashStoreTable '_HS_' not found in Stage1A of TS0")

        table = HashStoreTable.from_buffer(blob, hs_off)
        cur = hs_off + ctypes.sizeof(HashStoreTable)
        end = hs_off + table.UsedLength
        while cur < end:
            entry = HashStoreData.from_buffer(blob, cur)
            digest_off = cur + ctypes.sizeof(HashStoreData)
            if entry.Usage == HASH_USAGE['PUBKEY_MASTER']:
                hash_alg = _hash_alg_for_digest_len(entry.DigestLen)
                return (digest_off, entry.DigestLen, hash_alg)
            cur = digest_off + entry.DigestLen

        raise ResignError("PUBKEY_MASTER slot not present in Stage1A HashStoreTable")


# ---------------------------------------------------------------------------
# Invariant checks
# ---------------------------------------------------------------------------

def _key_type_for(key_id_or_file, key_dir):
    """Return canonical 'RSA2048'/'RSA3072' derived from a key or KEY_ID."""
    from CommonUtility import get_key_type
    priv = get_key_from_store(key_id_or_file) if os.environ.get('SBL_KEY_DIR') \
           else os.path.join(key_dir, SIGNING_KEY.get(key_id_or_file, key_id_or_file))
    return get_key_type(priv)


def verify_key_contract(inspector, key_dir):
    """Fail fast if any replacement key violates a build-time invariant."""
    os.environ['SBL_KEY_DIR'] = os.path.abspath(key_dir)

    keyh = inspector.keyh
    master_kt = 'RSA' + re.match(r'.*_RSA(\d+)$', keyh.master_key_id).group(1)

    def _expected_from_key_id(key_id):
        m = re.search(r'RSA(\d+)$', key_id)
        return 'RSA' + m.group(1) if m else master_kt

    def _check(key_id, label, expected_kt):
        try:
            kt = _key_type_for(key_id, key_dir)
        except Exception as ex:
            raise ResignError("%s: cannot load replacement key '%s' from %s: %s"
                              % (label, key_id, key_dir, ex))
        if kt != expected_kt:
            raise ResignError("%s: replacement key '%s' is %s; image expects %s"
                              % (label, key_id, kt, expected_kt))

    # Master key: strict — must match image's master key size.
    _check(keyh.master_key_id, 'MASTER', master_kt)

    # KEYH entries: each is checked against its OWN key size (derived from the
    # KEY_ID suffix, which itself was derived from the entry's digest length).
    for usage_val, key_id in keyh.key_hash_list:
        if key_id.startswith('PUBKEY_'):
            LOG.warning('Skipping algorithm check for OEM-slot usage %s', key_id)
            continue
        _check(key_id, 'KEYH:%s' % _HASH_USAGE_NAME.get(usage_val, hex(usage_val)),
               _expected_from_key_id(key_id))

    # Container header + component keys, per container leaf on primary side.
    # Container-level keys must match the master's key size.
    for name, ref in sorted(inspector.container_leaves['primary'].items()):
        if name == 'KEYH':
            continue
        data = _leaf_bytes(inspector.image_data, ref.node)
        container = CONTAINER(bytearray(data))
        hdr_auth = CONTAINER.get_auth_type_str(container.header.auth_type)
        if hdr_auth.startswith('RSA'):
            m = re.match(r'RSA(\d+)_', hdr_auth)
            if m.group(1) != master_kt[3:]:
                raise ResignError("Container '%s' header uses %s; image master is %s"
                                  % (name, hdr_auth, master_kt))
        for comp in container.header.comp_entry:
            comp_auth = CONTAINER.get_auth_type_str(comp.auth_type)
            if comp_auth.startswith('RSA'):
                m = re.match(r'RSA(\d+)_', comp_auth)
                if m.group(1) != master_kt[3:]:
                    raise ResignError("Container '%s' component '%s' uses %s; image master is %s"
                                      % (name, comp.name.decode(), comp_auth, master_kt))

    LOG.info('Verified replacement keys against image signing contract')


# ---------------------------------------------------------------------------
# KEYH rebuild
# ---------------------------------------------------------------------------

def rebuild_keyh(inspector, key_dir, work_dir):
    """Regenerate the KEYH container with the new key directory.

    The KEYH SVN is always preserved from the source image (this tool never
    changes SVNs). Returns the path to the new KEYH binary.
    """
    keyh = inspector.keyh
    svn  = keyh.svn

    keyh_dir = os.path.join(work_dir, 'keyh')
    os.makedirs(keyh_dir, exist_ok=True)
    out_file = os.path.join(keyh_dir, 'KEYH.bin')

    # Translate KEYH entries to (usage_val, key_file_name) as expected by
    # gen_pub_key_hash_store: the second element must be the *filename* under
    # the key directory.
    resolved = []
    for usage_val, key_id in keyh.key_hash_list:
        key_file = SIGNING_KEY.get(key_id)
        if key_file is None:
            raise ResignError("Unknown KEY_ID '%s' in SIGNING_KEY table; "
                              "OEM-slot usages must be handled manually" % key_id)
        resolved.append((usage_val, key_file))

    gen_pub_key_hash_store(
        keyh.master_key_id,
        resolved,
        keyh.hash_alg,
        keyh.sign_scheme,
        svn,
        os.path.abspath(key_dir),
        out_file,
    )
    LOG.info('Updated KEYH container (svn=%d, hash=%s, scheme=%s)',
             svn, keyh.hash_alg, keyh.sign_scheme)
    return out_file


# ---------------------------------------------------------------------------
# Container re-sign (in-place, no compression tools needed)
# ---------------------------------------------------------------------------

def _pick_container_header_key_id(hdr):
    """Match GenContainer.CONTAINER.extract()'s naming convention."""
    auth_str = CONTAINER.get_auth_type_str(hdr.auth_type)
    m = re.match(r'RSA(\d+)_', auth_str)
    if not m:
        return None  # header is unsigned
    key_size = m.group(1)
    if hdr.signature == b'BOOT':
        return 'KEY_ID_OS1_PRIVATE_RSA%s' % key_size
    return 'KEY_ID_CONTAINER_RSA%s' % key_size


def _pick_component_key_id(comp):
    """Container components are always signed with the ContainerComp key."""
    auth_str = CONTAINER.get_auth_type_str(comp.auth_type)
    m = re.match(r'RSA(\d+)_', auth_str)
    if not m:
        return None  # NONE or plain SHA hash -> no key
    return 'KEY_ID_CONTAINER_COMP_RSA%s' % m.group(1)


def resign_container(container_bytes, key_dir, work_dir, container_name):
    """Re-sign an existing container in place, preserving every component's
    compressed payload byte-for-byte.

    Only the following bytes change in the output:
        - each component's hash_data     (inside container header)
        - each component's auth_data     (trailing per-component signature)
        - header.auth_data               (trailing container header signature)

    Returns the path to the newly-signed container binary, or None if the
    container is unsigned (AUTH_TYPE_NONE at the header).
    """
    container = CONTAINER(bytearray(container_bytes))
    hdr = container.header
    header_auth_type = CONTAINER.get_auth_type_str(hdr.auth_type)

    if header_auth_type == 'NONE':
        LOG.info("Container '%s' is unsigned (AUTH_TYPE_NONE); skipping",
                 container_name)
        return None

    header_key_id = _pick_container_header_key_id(hdr)
    if header_key_id is None:
        raise ResignError("Container '%s' header auth type '%s' has no "
                          "recognised signing key" % (container_name, header_auth_type))
    header_key_path = get_key_from_store(header_key_id)

    workdir = os.path.join(work_dir, 'containers', container_name)
    os.makedirs(workdir, exist_ok=True)
    # tool_dir='' -- compress/decompress are never invoked in this path.
    container.set_dir_path(workdir, workdir, os.path.abspath(key_dir), '')
    hdr.priv_key = header_key_path

    for comp in hdr.comp_entry:
        comp_auth_type = CONTAINER.get_auth_type_str(comp.auth_type)
        if comp_auth_type == 'NONE':
            comp.hash_data = bytearray()
            comp.auth_data = bytearray()
            continue

        # component.data was populated by CONTAINER.__init__ as
        # LZ_HEADER || compressed_payload. Keep the exact same bytes so the
        # on-flash compressed body is byte-identical. We only recompute the
        # hash over those bytes and (if RSA-signed) sign that hash.
        lz_hdr_bytes = bytes(comp.data[:ctypes.sizeof(LZ_HEADER)])
        lz_hdr = LZ_HEADER.from_buffer_copy(lz_hdr_bytes)
        comp_body_len = ctypes.sizeof(LZ_HEADER) + lz_hdr.compressed_len
        comp_body = bytes(comp.data[:comp_body_len])

        # Sanity: component.data must exactly wrap LZ_HEADER + compressed body.
        if len(comp.data) != comp_body_len:
            raise ResignError(
                "Component '%s' in container '%s': data length 0x%X does not "
                "match LZ_HEADER declared length 0x%X (container corrupted or "
                "unexpected layout)" %
                (comp.name.decode(), container_name, len(comp.data), comp_body_len))

        comp_file = os.path.join(workdir, comp.name.decode() + '.lz')
        gen_file_from_object(comp_file, comp_body)

        # Resolve the component signing key (if any).
        comp_key_id = None
        comp_key_path = None
        if comp_auth_type.startswith('RSA'):
            comp_key_id = _pick_component_key_id(comp)
            if comp_key_id is None:
                raise ResignError("Component '%s' auth type '%s' unrecognised"
                                  % (comp.name.decode(), comp_auth_type))
            comp_key_path = get_key_from_store(comp_key_id)

        hash_data, auth_data = CONTAINER.calculate_auth_data(
            comp_file, comp_auth_type, comp_key_path, workdir)

        if len(hash_data) != comp.hash_size:
            raise ResignError(
                "Component '%s' hash length mismatch: got %d, container header "
                "reserves %d" % (comp.name.decode(), len(hash_data), comp.hash_size))

        comp.hash_data = bytearray(hash_data)
        comp.auth_data = bytearray(auth_data)

    # Reseal the container header (recomputes hash over header + entries and
    # signs with the header private key set on hdr.priv_key).
    container.adjust_header()

    out_path = os.path.join(workdir, container_name + '.bin')
    gen_file_from_object(out_path, container.get_data())
    LOG.info("Re-signed container '%s' (%d components) in place",
             container_name, len(hdr.comp_entry))
    return out_path


# ---------------------------------------------------------------------------
# CFGDATA re-sign
# ---------------------------------------------------------------------------

def resign_cfgdata(image_path, out_path, inspector, key_dir, work_dir):
    """Re-sign every CFGDATA leaf present in the image and write the result back.

    The CFGDATA SVN is always preserved from the source blob (this tool
    never changes SVNs). Uses CfgDataTool.py sign + replace via subprocess
    (both partitions are handled by 'replace' in a single call).
    """
    if not any(inspector.cfgd_leaves[s] for s in ImageInspector.SIDES):
        LOG.info('No CFGDATA leaves in image; skipping CFGDATA re-sign')
        shutil.copyfile(image_path, out_path)
        return

    # Take the primary-side CFGD leaf as authoritative; backup must match.
    primary = inspector.cfgd_leaves['primary']
    if not primary:
        LOG.warning('CFGDATA present on backup side but not primary; skipping')
        shutil.copyfile(image_path, out_path)
        return

    src_leaf = primary[0]
    raw = _leaf_bytes(inspector.image_data, src_leaf.node)

    # CDATA_BLOB_HEADER.UsedLength tells us the length of the unsigned payload.
    # (Structure lives in CfgDataTool.py's CCfgData class; we re-declare the
    # minimal layout here to avoid pulling that whole class in.)
    class _CDATA_BLOB_HEADER(ctypes.Structure):
        _pack_ = 1
        _fields_ = [
            ('Signature',    ctypes.c_char * 4),
            ('HeaderLength', ctypes.c_uint8),
            ('Attribute',    ctypes.c_uint8),
            ('Svn',          ctypes.c_uint8),
            ('Reserved',     ctypes.c_uint8),
            ('UsedLength',   ctypes.c_uint32),
            ('TotalLength',  ctypes.c_uint32),
        ]
    hdr = _CDATA_BLOB_HEADER.from_buffer_copy(raw[:ctypes.sizeof(_CDATA_BLOB_HEADER)])
    if hdr.Signature != b'CFGD':
        raise ResignError("CFGDATA leaf has unexpected signature %r" % hdr.Signature)
    if hdr.UsedLength == 0 or hdr.UsedLength > len(raw):
        raise ResignError("CFGDATA UsedLength 0x%X out of range" % hdr.UsedLength)

    svn = hdr.Svn

    unsigned_path = os.path.join(work_dir, 'CfgDataExt.bin')
    gen_file_from_object(unsigned_path, raw[:hdr.UsedLength])
    signed_path = os.path.join(work_dir, 'CfgData.bin')

    # Look up the CFGDATA key id from KEYH.
    cfg_key_id = None
    for usage_val, key_id in inspector.keyh.key_hash_list:
        if usage_val == HASH_USAGE['PUBKEY_CFG_DATA']:
            cfg_key_id = key_id
            break
    if cfg_key_id is None:
        raise ResignError("KEYH does not declare PUBKEY_CFG_DATA; cannot re-sign CFGDATA")

    tool = os.path.join(SCRIPT_DIR, 'CfgDataTool.py')
    subprocess.check_call([
        sys.executable, tool, 'sign',
        unsigned_path,
        '-k', cfg_key_id,
        '-a', 'AUTO',
        '-s', inspector.keyh.sign_scheme,
        '-svn', str(svn),
        '-o', signed_path,
    ])
    LOG.info('Re-signed CFGDATA with %s (svn=%d)', cfg_key_id, svn)

    subprocess.check_call([
        sys.executable, tool, 'replace',
        signed_path,
        '-i', image_path,
        '-o', out_path,
    ])
    LOG.info('Updated CFGDATA in output image')


# ---------------------------------------------------------------------------
# Stage1A PUBKEY_MASTER hash patch
# ---------------------------------------------------------------------------

def compute_master_pub_key_hash(inspector, key_dir):
    """Return the hash bytes of the new Master public key, in the algorithm
    the image was built with."""
    key_id = inspector.keyh.master_key_id
    key_path = get_key_from_store(key_id)
    # single_sign_gen_pub_key returns raw modulus||exponent bytes.
    keydata = single_sign_gen_pub_key(key_path)
    alg_name = inspector.stage1a_geometry[2]
    if alg_name == 'SHA2_256':
        return hashlib.sha256(bytes(keydata)).digest()
    if alg_name == 'SHA2_384':
        return hashlib.sha384(bytes(keydata)).digest()
    if alg_name == 'SHA2_512':
        return hashlib.sha512(bytes(keydata)).digest()
    raise ResignError("Unsupported Stage1A hash algorithm '%s'" % alg_name)


def patch_stage1a_master_hash(image, inspector, new_hash):
    """Overwrite the PUBKEY_MASTER digest inside Stage1A on both sides."""
    _, digest_len, alg = inspector.stage1a_geometry
    if len(new_hash) != digest_len:
        raise ResignError("Master hash length mismatch: got %d, image expects %d"
                          % (len(new_hash), digest_len))

    for side in ('primary', 'backup'):
        ref = inspector.stage1a[side]
        blob = bytearray(_leaf_bytes(image, ref.node))
        hs_off = blob.find(HashStoreTable.HASH_STORE_SIGNATURE)
        if hs_off < 0:
            raise ResignError("HashStoreTable not found in Stage1A on %s side" % side)
        table = HashStoreTable.from_buffer(blob, hs_off)
        cur = hs_off + ctypes.sizeof(HashStoreTable)
        end = hs_off + table.UsedLength
        patched = False
        while cur < end:
            entry = HashStoreData.from_buffer(blob, cur)
            digest_off = cur + ctypes.sizeof(HashStoreData)
            if entry.Usage == HASH_USAGE['PUBKEY_MASTER']:
                blob[digest_off:digest_off + digest_len] = new_hash
                patched = True
                break
            cur = digest_off + entry.DigestLen
        if not patched:
            raise ResignError("PUBKEY_MASTER slot not found in %s Stage1A" % side)
        image[ref.node.offset:ref.node.offset + ref.node.length] = blob
        LOG.info('Updated PUBKEY_MASTER in %s Stage1A (alg=%s)', side, alg)


# ---------------------------------------------------------------------------
# Orchestration
# ---------------------------------------------------------------------------

def do_resign(args):
    inspector = ImageInspector(args.input_image)
    if args.verbose:
        _print_summary(inspector)

    work_dir = os.path.abspath(args.work_dir) if args.work_dir else \
               tempfile.mkdtemp(prefix='sbl-resign-')
    cleanup = not args.work_dir
    os.makedirs(work_dir, exist_ok=True)
    LOG.info('Working directory: %s', work_dir)

    try:
        verify_key_contract(inspector, args.key_dir)

        before_rows = hash_all_leaves(inspector.image_data, inspector)

        image = bytearray(inspector.image_data)  # working copy

        # 1) KEYH rebuild + replace in every partition that carries it.
        keyh_bin = rebuild_keyh(inspector, args.key_dir, work_dir)
        keyh_data = get_file_data(keyh_bin)
        for side in ('primary', 'backup'):
            if 'KEYH' in inspector.container_leaves[side]:
                replace_leaf(image, inspector.container_leaves[side]['KEYH'], keyh_data)

        # 2) Recompute the Stage1A PUBKEY_MASTER digest and patch the
        #    existing slot in both Stage1A copies.
        new_hash = compute_master_pub_key_hash(inspector, args.key_dir)
        patch_stage1a_master_hash(image, inspector, new_hash)

        # 3) Re-sign every other container leaf on the primary side; replace it
        #    in whichever sides carry a leaf of the same name.
        for name in sorted(inspector.container_leaves['primary'].keys()):
            if name == 'KEYH':
                continue
            ref_pri = inspector.container_leaves['primary'][name]
            src_bytes = _leaf_bytes(inspector.image_data, ref_pri.node)
            new_path = resign_container(src_bytes, args.key_dir, work_dir, name)
            if new_path is None:
                continue  # unsigned container, skipped
            new_bytes = get_file_data(new_path)
            for side in ImageInspector.SIDES:
                if name in inspector.container_leaves[side]:
                    replace_leaf(image, inspector.container_leaves[side][name], new_bytes)

        # 4) Non-redundant containers that don't have a primary-side twin.
        for name in sorted(inspector.container_leaves['non_redundant'].keys()):
            if name in inspector.container_leaves['primary']:
                continue  # already handled above
            ref = inspector.container_leaves['non_redundant'][name]
            src_bytes = _leaf_bytes(inspector.image_data, ref.node)
            new_path = resign_container(src_bytes, args.key_dir, work_dir, name)
            if new_path is None:
                continue
            new_bytes = get_file_data(new_path)
            replace_leaf(image, ref, new_bytes)

        # 5) Write the intermediate image so we can hand it to CfgDataTool.
        interim = os.path.join(work_dir, 'SlimBootloader.interim.bin')
        gen_file_from_object(interim, image)

        # 6) CFGDATA re-sign (subprocess to CfgDataTool.py sign + replace).
        resign_cfgdata(interim, os.path.abspath(args.output_image), inspector,
                       args.key_dir, work_dir)

        # 7) Hash log against the *output* image.
        final = bytearray(get_file_data(args.output_image))
        after_rows = hash_all_leaves(final, inspector)
        if args.hash_log:
            emit_hash_log(before_rows, after_rows, args, inspector, args.hash_log)

        _print_completion_reminder(args)
        return 0

    finally:
        if cleanup and os.path.isdir(work_dir):
            shutil.rmtree(work_dir, ignore_errors=True)


def _print_summary(inspector):
    lines = [
        'Discovered image information:',
        '  Image layout:',
        '    Partitions:',
    ]
    for side in ImageInspector.SIDES:
        lines.append('      %-14s : %s'
                     % (side, ', '.join(part.name for part in inspector.partitions[side])))

    lines.append('')
    lines.append('    Leaves:')
    for side in ImageInspector.SIDES:
        lines.append('      %s side:' % side)
        s1a = inspector.stage1a.get(side)
        if s1a:
            lines.append('        Stage1A      : %s  (off=0x%08X len=0x%X)'
                         % (s1a.path, s1a.node.offset, s1a.node.length))
        for name, ref in sorted(inspector.container_leaves[side].items()):
            lines.append('        Container    : %s  (off=0x%08X len=0x%X)'
                         % (ref.path, ref.node.offset, ref.node.length))
        for cfg in inspector.cfgd_leaves[side]:
            lines.append('        CFGDATA      : %s  (off=0x%08X len=0x%X)'
                         % (cfg.path, cfg.node.offset, cfg.node.length))
    lines.extend([
        '',
        '  Signing context:',
        '    Master key id  : %s' % inspector.keyh.master_key_id,
        '    KEYH hash alg  : %s' % inspector.keyh.hash_alg,
        '    KEYH scheme    : %s' % inspector.keyh.sign_scheme,
        '    KEYH SVN       : %d' % inspector.keyh.svn,
        '    KEYH entries   :',
    ])
    for usage_val, key_id in inspector.keyh.key_hash_list:
        name = _HASH_USAGE_NAME.get(usage_val, hex(usage_val))
        lines.append('      - %-18s -> %s' % (name, key_id))

    print()
    for line in lines:
        if line:
            print('INFO    %s' % line)
        else:
            print('INFO')
    print()


def _print_completion_reminder(args):
    print()
    print('SBL key rotation complete.')
    print('  Output image: %s' % os.path.abspath(args.output_image))
    if args.hash_log:
        print('  Hash log    : %s' % os.path.abspath(args.hash_log))


# ---------------------------------------------------------------------------
# CLI
# ---------------------------------------------------------------------------

def main(argv=None):
    ap = argparse.ArgumentParser(
        description='Post-build re-sign SlimBootloader.bin using a '
                    'replacement SBL key directory.',
    )
    ap.add_argument('-i', '--input-image', required=True,
                    help='Path to the input SlimBootloader.bin to be re-signed.')
    ap.add_argument('-o', '--output-image', required=True,
                    help='Path to write the re-signed SlimBootloader.bin.')
    ap.add_argument('-k', '--key-dir', required=True,
                    help='Directory containing the replacement SBL keys '
                         '(GenerateKeys.py naming convention).')
    ap.add_argument('-w', '--work-dir', default='',
                    help='Working directory for intermediates. If supplied, '
                         'the directory is kept after the run so you can '
                         'inspect intermediate artifacts. If omitted, a '
                         'temporary directory is created and auto-deleted.')
    ap.add_argument('--hash-log', dest='hash_log', default='',
                    help='Path to write a BEFORE/AFTER SHA-256 hash log.')
    ap.add_argument('-v', '--verbose', action='store_true',
                    help='Enable verbose logging.')
    args = ap.parse_args(argv)

    _configure_logging(args.verbose)

    try:
        return do_resign(args)
    except ResignError as ex:
        LOG.error(str(ex))
        return 2
    except subprocess.CalledProcessError as ex:
        LOG.error('Sub-tool failed: %s', ex)
        return 3
    except FileNotFoundError as ex:
        LOG.error('File not found: %s', ex)
        return 3
    except Exception:
        LOG.exception('Unexpected failure')
        return 1


if __name__ == '__main__':
    sys.exit(main())
