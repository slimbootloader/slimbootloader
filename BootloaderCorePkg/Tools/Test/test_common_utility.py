## @ test_common_utility.py
#
# Tests for the pure helper functions in CommonUtility.py.
#
# These functions underpin container packing, configuration-data packing and
# IFWI layout arithmetic. They have no I/O and no tool dependencies, which
# makes them the cleanest unit-test targets in the Python tool set.
#
# Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
# SPDX-License-Identifier: BSD-2-Clause-Patent
#
##

import pytest

import CommonUtility as CU


# ---------------------------------------------------------------------------
# value <-> bytes
# ---------------------------------------------------------------------------

@pytest.mark.parametrize ('value, length', [
    (0, 1),
    (0xFF, 1),
    (0x1234, 2),
    (0xDEADBEEF, 4),
    (0x0102030405060708, 8),
])
def test_value_to_bytes_round_trip (value, length):
    data = CU.value_to_bytes (value, length)
    assert len (data) == length
    assert CU.bytes_to_value (data) == value


def test_value_to_bytes_is_little_endian ():
    assert CU.value_to_bytes (0x1234, 2) == b'\x34\x12'
    assert CU.value_to_bytes (0xDEADBEEF, 4) == b'\xef\xbe\xad\xde'


def test_value_to_bytearray_matches_value_to_bytes ():
    assert bytes (CU.value_to_bytearray (0xCAFEBABE, 4)) == \
        CU.value_to_bytes (0xCAFEBABE, 4)


# ---------------------------------------------------------------------------
# bit field access
# ---------------------------------------------------------------------------

def test_get_bits_from_bytes_single_byte ():
    data = bytearray ([0b1011_0010])
    assert CU.get_bits_from_bytes (data, 0, 1) == 0
    assert CU.get_bits_from_bytes (data, 1, 1) == 1
    assert CU.get_bits_from_bytes (data, 4, 4) == 0b1011


def test_get_bits_from_bytes_crosses_byte_boundary ():
    # 0x3412 little endian -> bits 8..15 are 0x34.
    data = bytearray ([0x12, 0x34])
    assert CU.get_bits_from_bytes (data, 8, 8) == 0x34
    assert CU.get_bits_from_bytes (data, 4, 8) == 0x41


@pytest.mark.parametrize ('start, length, value', [
    (0, 1, 1),
    (3, 5, 0b10101),
    (6, 4, 0b1101),      # spans a byte boundary
    (8, 8, 0xA5),
    (0, 16, 0xBEEF),
])
def test_set_then_get_bits_round_trip (start, length, value):
    data = bytearray (4)
    CU.set_bits_to_bytes (data, start, length, value)
    assert CU.get_bits_from_bytes (data, start, length) == value


def test_set_bits_leaves_neighbouring_bits_untouched ():
    data = bytearray ([0xFF, 0xFF, 0xFF, 0xFF])
    CU.set_bits_to_bytes (data, 4, 4, 0)
    # Only bits 4..7 cleared.
    assert data == bytearray ([0x0F, 0xFF, 0xFF, 0xFF])


# ---------------------------------------------------------------------------
# alignment arithmetic
# ---------------------------------------------------------------------------

@pytest.mark.parametrize ('value, alignment, expected', [
    (0, 4, 0),
    (1, 4, 4),
    (4, 4, 4),
    (5, 4, 8),
    (0x1001, 0x1000, 0x2000),
    (0x1000, 0x1000, 0x1000),
])
def test_get_aligned_value (value, alignment, expected):
    assert CU.get_aligned_value (value, alignment) == expected


@pytest.mark.parametrize ('length, alignment', [
    (0, 4), (1, 4), (7, 4), (0x1001, 0x1000), (0x2000, 0x1000),
])
def test_padding_completes_alignment (length, alignment):
    pad = CU.get_padding_length (length, alignment)
    assert 0 <= pad < alignment
    assert (length + pad) % alignment == 0
    assert length + pad == CU.get_aligned_value (length, alignment)
