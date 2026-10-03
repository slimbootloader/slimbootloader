## @ test_container_format.py
#
# Tests for the container binary format contracts in GenContainer.py.
#
# The container header and component entry are on-disk structures consumed by
# Stage2 and the firmware update path, so their size and field layout are a
# compatibility contract, not an implementation detail. The authentication
# size arithmetic decides how much space is reserved for signatures, and
# getting it wrong produces images that fail verified boot at runtime rather
# than at build time.
#
# Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
# SPDX-License-Identifier: BSD-2-Clause-Patent
#
##

from ctypes import sizeof

import pytest

import GenContainer as GC


# ---------------------------------------------------------------------------
# On-disk structure contracts
# ---------------------------------------------------------------------------

def test_container_header_is_sixteen_bytes ():
    assert sizeof (GC.CONTAINER_HDR) == 16


def test_component_entry_is_sixteen_bytes ():
    assert sizeof (GC.COMPONENT_ENTRY) == 16


def test_container_header_fields ():
    names = [field[0] for field in GC.CONTAINER_HDR._fields_]
    assert names == ['signature', 'version', 'svn', 'data_offset',
                     'data_size', 'auth_type', 'image_type', 'flags',
                     'entry_count']


def test_component_entry_fields ():
    names = [field[0] for field in GC.COMPONENT_ENTRY._fields_]
    assert names == ['name', 'offset', 'size', 'attribute', 'alignment',
                     'auth_type', 'hash_size']


# ---------------------------------------------------------------------------
# Authentication size arithmetic
# ---------------------------------------------------------------------------

@pytest.mark.parametrize ('auth_type, expected', [
    ('NONE', 0),
    ('SHA2_256', 32),
    ('SHA2_384', 48),
    ('RSA2048_PKCS1_SHA2_256', 256),
    ('RSA3072_PKCS1_SHA2_384', 384),
    ('RSA2048_PSS_SHA2_256', 256),
    ('RSA3072_PSS_SHA2_384', 384),
])
def test_unsigned_auth_size (auth_type, expected):
    assert GC.CONTAINER.get_auth_size (auth_type) == expected


@pytest.mark.parametrize ('auth_type', [
    'RSA2048_PKCS1_SHA2_256', 'RSA3072_PKCS1_SHA2_384',
    'RSA2048_PSS_SHA2_256', 'RSA3072_PSS_SHA2_384',
])
def test_signed_rsa_reserves_key_and_signature_headers (auth_type):
    unsigned = GC.CONTAINER.get_auth_size (auth_type)
    signed = GC.CONTAINER.get_auth_size (auth_type, signed=True)
    expected = unsigned * 2 + sizeof (GC.PUB_KEY_HDR) \
        + sizeof (GC.SIGNATURE_HDR) + 4
    assert signed == expected
    assert signed > unsigned


@pytest.mark.parametrize ('auth_type', ['SHA2_256', 'SHA2_384'])
def test_signed_hash_only_auth_reserves_nothing (auth_type):
    # A hash-only container carries its digest in the parent, so the signed
    # reservation collapses to zero.
    assert GC.CONTAINER.get_auth_size (auth_type, signed=True) == 0


def test_unknown_auth_type_is_rejected ():
    with pytest.raises (Exception):
        GC.CONTAINER.get_auth_size ('BOGUS_SCHEME')


# ---------------------------------------------------------------------------
# Enumeration round trips
# ---------------------------------------------------------------------------

def test_auth_type_value_and_string_round_trip ():
    for name in GC.CONTAINER._auth_type_value:
        value = GC.CONTAINER.get_auth_type_val (name)
        assert GC.CONTAINER.get_auth_type_str (value) == name


def test_auth_size_accepts_value_or_string ():
    for name in GC.CONTAINER._auth_type_value:
        value = GC.CONTAINER.get_auth_type_val (name)
        assert GC.CONTAINER.get_auth_size (value) == \
            GC.CONTAINER.get_auth_size (name)
