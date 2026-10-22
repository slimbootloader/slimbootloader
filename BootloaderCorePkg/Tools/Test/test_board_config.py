## @ test_board_config.py
#
# Invariant tests over every Platform/*/BoardConfig*.py in the tree.
#
# BoardConfig.py is the single source of truth for a board: flash layout,
# feature knobs and component lists all come from it. A malformed board
# config normally surfaces as a confusing failure deep inside a build, and
# only for the boards CI happens to compile. These tests load every board in
# about a second and assert the properties that must hold for all of them.
#
# Only invariants verified to hold across the whole tree are asserted here.
# Notably, the region sizes do NOT have to sum exactly to
# SLIMBOOTLOADER_SIZE: boards such as 'ksv' are non-redundant and set
# NON_REDUNDANT_SIZE to the full image size, so no such assertion is made.
#
# Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
# SPDX-License-Identifier: BSD-2-Clause-Patent
#
##

import collections

import pytest

import CheckDscLibraryClasses as Chk

REGION_ATTRS = ('TOP_SWAP_SIZE', 'REDUNDANT_SIZE', 'NON_VOLATILE_SIZE',
                'NON_REDUNDANT_SIZE')

FLASH_BLOCK_SIZE = 0x1000


@pytest.fixture (scope='module')
def loaded_boards (workspace):
    boards, errors = Chk.load_boards (workspace)
    assert not errors, 'BoardConfig files failed to load: %s' % errors
    assert boards, 'no boards discovered'
    return boards


def test_every_board_config_loads (loaded_boards):
    # BuildLoader.py imports every BoardConfig*.py at startup, so a board
    # that cannot be imported breaks the build for all boards, not just
    # its own.
    assert len (loaded_boards) > 5


def test_board_names_are_unique (loaded_boards):
    counts = collections.Counter ()
    for module in loaded_boards.values ():
        counts[module.Board ().BOARD_NAME] += 1
    duplicates = [name for name, count in counts.items () if count > 1]
    assert duplicates == [], 'duplicate BOARD_NAME: %s' % duplicates


def test_board_names_are_non_empty_and_clean (loaded_boards):
    for name in loaded_boards:
        assert name.strip () == name, 'BOARD_NAME %r has stray whitespace' % name
        assert name, 'empty BOARD_NAME'


def test_region_sizes_are_flash_block_aligned (loaded_boards):
    bad = []
    for name in sorted (loaded_boards):
        board = loaded_boards[name].Board ()
        for attr in REGION_ATTRS:
            value = getattr (board, attr, 0)
            if value % FLASH_BLOCK_SIZE:
                bad.append ('%s.%s = 0x%X' % (name, attr, value))
    assert bad == [], 'region sizes must be 4KB aligned: %s' % bad


def test_region_sizes_are_not_negative (loaded_boards):
    bad = []
    for name in sorted (loaded_boards):
        board = loaded_boards[name].Board ()
        for attr in REGION_ATTRS:
            value = getattr (board, attr, 0)
            if value < 0:
                bad.append ('%s.%s = %d' % (name, attr, value))
    assert bad == [], bad


def test_image_fits_in_flash (loaded_boards):
    # FLASH_SIZE of 0 means 'use SLIMBOOTLOADER_SIZE', which BuildLoader.py
    # substitutes later; only a non-zero value is a real constraint.
    bad = []
    for name in sorted (loaded_boards):
        board = loaded_boards[name].Board ()
        sbl_size = getattr (board, 'SLIMBOOTLOADER_SIZE', None)
        flash_size = getattr (board, 'FLASH_SIZE', 0)
        if sbl_size is None or not flash_size:
            continue
        if sbl_size > flash_size:
            bad.append ('%s: SLIMBOOTLOADER_SIZE 0x%X > FLASH_SIZE 0x%X'
                        % (name, sbl_size, flash_size))
    assert bad == [], bad


def test_every_board_can_generate_platform_dsc (loaded_boards, tmp_path):
    """Platform.dsc generation must work for every board.

    BootloaderCorePkg.dsc '!include Platform.dsc', so a board that cannot
    produce this file cannot be built at all.
    """
    failures = []
    for name in sorted (loaded_boards):
        out_path = str (tmp_path / ('%s-Platform.dsc' % name))
        try:
            Chk.gen_platform_dsc (loaded_boards[name], out_path)
        except Exception as exc:   # noqa: BLE001 - collect all failures
            failures.append ('%s: %s: %s' % (name, type (exc).__name__, exc))
            continue
        with open (out_path, 'r', encoding='utf-8', errors='replace') as fp:
            text = fp.read ()
        if '[Defines]' not in text:
            failures.append ('%s: generated Platform.dsc has no [Defines]'
                             % name)
    assert failures == [], '\n'.join (failures)
