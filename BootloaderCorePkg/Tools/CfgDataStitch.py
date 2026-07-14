#!/usr/bin/env python

## @ CfgDataStitch.py
# Project template script to patch CFGDATA blob into IFWI image
#
# Copyright (c) 2018 - 2021, Intel Corporation. All rights reserved.<BR>
# SPDX-License-Identifier: BSD-2-Clause-Patent
#
##

import os
import sys
import glob
import argparse
import subprocess
import tempfile

dlt_files = [] # TO BE PATCHED

def get_script_dir():
    return os.path.abspath(os.path.dirname(__file__))


def run_cmd(cmd_list):
    sys.stdout.flush()
    print (' '.join(cmd_list))
    ret = subprocess.call(cmd_list)
    if ret:
        sys.exit(1)


def check_file_exist (chk_files):
    for dir, files in chk_files:
        for file in files:
            path = os.path.join(dir, file)
            if not os.path.exists(path):
                return path
    return ''


def _find_source_dlt_path(dlt_path, cfg_dir):
    base = os.path.basename(dlt_path)
    # Accept both the Autogen_<name>.dlt naming used by some platforms
    # and same-named DLTs used by others (e.g. QEMU CfgDataExt_Brd1.dlt).
    src_name = base[len('Autogen_'):] if base.startswith('Autogen_') else base

    candidates = [os.path.join(cfg_dir, src_name)]

    for env_name in ['PLT_SOURCE', 'SBL_SOURCE']:
        env_root = os.environ.get(env_name, '')
        if not env_root:
            continue
        pattern = os.path.join(env_root, 'Platform', '*', 'CfgData', src_name)
        candidates.extend(glob.glob(pattern))

    # Fallback for standalone Outputs bundles where env vars are not set:
    # walk up a few levels and probe common repo-local source locations.
    # Supports both the upstream Platform/*/CfgData/ layout and vendor
    # SblInternal/SblOpen sub-tree layouts.
    probe_root = os.path.abspath(cfg_dir)
    for _ in range(8):
        candidates.extend(glob.glob(
            os.path.join(probe_root, 'Platform', '*', 'CfgData', src_name)))
        for root_name in ['SblInternal', 'SblOpen']:
            pattern = os.path.join(probe_root, root_name, 'Platform', '*', 'CfgData', src_name)
            candidates.extend(glob.glob(pattern))
        parent = os.path.dirname(probe_root)
        if parent == probe_root:
            break
        probe_root = parent

    # Never match the target file itself (would be a self-regen no-op
    # and defeats the purpose of the lookup).
    dlt_abs = os.path.abspath(dlt_path)
    for each in candidates:
        if each and os.path.exists(each) and os.path.abspath(each) != dlt_abs:
            return each
    return ''


def _regen_autogen_dlt(gen_cfg_data_tool, yaml_path, source_dlt, autogen_dlt):
    """Regenerate a canonical autogen DLT from the source DLT.

    Uses a GENBIN -> GENDLT roundtrip through a flat cfg binary so the
    result has no phantom lines caused by the CFGDATA-container-vs-flat
    format mismatch that afflicts running GENDLT directly on the per-
    board bin extracted from an IFWI.
    """
    with tempfile.TemporaryDirectory() as tmp:
        flat_bin = os.path.join(tmp, 'flat.bin')
        run_cmd([sys.executable, gen_cfg_data_tool, 'GENBIN',
                 '%s;%s' % (yaml_path, source_dlt), flat_bin])
        run_cmd([sys.executable, gen_cfg_data_tool, 'GENDLT',
                 '%s;%s' % (yaml_path, flat_bin), autogen_dlt])


def cfgdata_stitch(ifwi_file, ifwi_out_file, cfg_dir, key_file, script_dir, tool_dir, platform_id):
    if len (dlt_files) == 0:
        raise Exception("Please run the generated CfgDataStitch.py script instead of the original one in source tree !")

    # --------------------------------------------------------------
    # Resolve tool / working directories.
    # --------------------------------------------------------------
    fv_dir = get_script_dir()
    if not os.path.exists(fv_dir + '/ImgStitch.txt'):
        fv_dir = ''

    if ifwi_out_file == '':
        ifwi_out_file = os.path.join('Output', os.path.basename(ifwi_file))

    out_dir = os.path.dirname(ifwi_out_file) or '.'

    if tool_dir == '':
        if fv_dir:
            tool_dir = os.path.realpath(os.path.join(fv_dir, '../../../../BaseTools/Bin/Win32'))
        else:
            tool_dir = get_script_dir()

    if script_dir == '':
        if fv_dir:
            script_dir = os.path.realpath(os.path.join(fv_dir, '../../../../BootloaderCorePkg/Tools'))
        else:
            script_dir = get_script_dir()

    if key_file == '':
        raise Exception("Key file is not specified!!")

    if not os.path.exists(out_dir):
        os.makedirs(out_dir)

    if cfg_dir == "":
        dlt_dir = out_dir
        cfg_dir = '.'
    else:
        dlt_dir = cfg_dir

    # --------------------------------------------------------------
    # Path shorthands used by the rest of the flow.
    # --------------------------------------------------------------
    gencfg_tool  = os.path.join(script_dir, 'GenCfgData.py')
    cfgtool_tool = os.path.join(script_dir, 'CfgDataTool.py')
    yaml_path    = os.path.join(cfg_dir, 'CfgDataDef.yaml')
    cfg_int_bin  = os.path.join(out_dir, 'CfgDataInt.bin')
    cfg_ext_bin  = os.path.join(out_dir, 'CfgDataExt.bin')
    signed_bin   = os.path.join(out_dir, 'CfgData.bin')

    if not os.path.exists(yaml_path):
        raise Exception("No CfgDataDef.yaml file found under directory '%s' !" % cfg_dir)

    chk_files = [
        (script_dir, ['GenCfgData.py', 'CfgDataTool.py']),
        (cfg_dir,    ['CfgDataDef.yaml']),
    ]
    if not key_file.startswith('KEY_ID'):
        chk_files.append((os.path.dirname(key_file), [os.path.basename(key_file)]))

    missing = check_file_exist(chk_files)
    if missing:
        raise Exception("Cannot find file '%s' !" % missing)

    # --------------------------------------------------------------
    # Regenerate autogen DLTs + export CfgDataInt.bin from IFWI
    # when any required bundle output is absent.
    # --------------------------------------------------------------
    dlt_names       = [name for _pid, name in dlt_files]
    missing_dlts    = [n for n in dlt_names if not os.path.exists(os.path.join(dlt_dir, n))]
    cfg_int_missing = not os.path.exists(cfg_int_bin)

    if missing_dlts or cfg_int_missing:
        name_str = ','.join('%d:%s' % (pid, os.path.splitext(dlt)[0])
                            for pid, dlt in dlt_files)
        run_cmd([sys.executable, cfgtool_tool, 'export',
                 '-i', ifwi_file, '-t', tool_dir, '-o', out_dir, '-n', name_str])

        # Regenerate each autogen DLT from its source DLT via a
        # GENBIN -> GENDLT roundtrip.  If no source DLT can be located,
        # fall back to the historical GENDLT-on-exported-bin path so
        # the file at least exists for the downstream GENBIN step.
        for _pid, dlt_name in dlt_files:
            dlt_path = os.path.join(dlt_dir, dlt_name)
            bin_path = os.path.join(out_dir, os.path.splitext(dlt_name)[0] + '.bin')
            src_dlt  = _find_source_dlt_path(dlt_path, cfg_dir)
            if src_dlt:
                print("Regenerating %s from source DLT %s" % (dlt_name, src_dlt))
                _regen_autogen_dlt(gencfg_tool, yaml_path, src_dlt, dlt_path)
            else:
                print("Regenerating %s from exported bin (no source DLT found)" % dlt_name)
                run_cmd([sys.executable, gencfg_tool, 'GENDLT',
                         '%s;%s' % (yaml_path, bin_path), dlt_path])

    # --------------------------------------------------------------
    # Build per-board CFGDATA bins from the (regenerated) DLTs.
    # --------------------------------------------------------------
    bin_files = []
    for _pid, dlt in dlt_files:
        bin_file = os.path.join(out_dir, os.path.splitext(dlt)[0] + '.bin')
        run_cmd([sys.executable, gencfg_tool, 'GENBIN',
                 '%s;%s' % (yaml_path, os.path.join(dlt_dir, dlt)), bin_file])
        bin_files.append(bin_file)

    # Merge internal defaults + per-board bins into external CFGDATA.
    # The trailing '*' on CfgDataInt.bin marks it as built-in.
    merge_cmd = [sys.executable, cfgtool_tool, 'merge', '-o', cfg_ext_bin]
    if platform_id:
        merge_cmd.extend(['-p', platform_id])
    merge_cmd.append(cfg_int_bin + '*')
    merge_cmd.extend(bin_files)
    run_cmd(merge_cmd)

    # --------------------------------------------------------------
    # Regenerate FSP UPD per-board delta tags from YAML defaults and
    # the current per-board DLTs.  Safe because GenFspUpdYaml.py
    # extracts YAML defaults byte-exactly from the FSP binary
    # CfgRegion, so a YAML-derived delta is bit-identical to an
    # FSP-derived one.
    # --------------------------------------------------------------
    if script_dir not in sys.path:
        sys.path.insert(0, script_dir)
    from GenFspUpdDelta import gen_and_inject_fsp_upd_deltas
    gen_and_inject_fsp_upd_deltas(
        fv_dir=out_dir,
        brd_cfg_dir=dlt_dir,
        cfg_bin_path=cfg_ext_bin,
        dlt_files=dlt_names,
        cfg_dir=cfg_dir,
        gen_cfg_data_tool=gencfg_tool)

    # Sign external CFGDATA and swap it into the IFWI.
    run_cmd([sys.executable, cfgtool_tool, 'sign',
             '-o', signed_bin, '-k', key_file, cfg_ext_bin])
    run_cmd([sys.executable, cfgtool_tool, 'replace',
             '-i', ifwi_file, signed_bin,
             '-o', os.path.join(out_dir, os.path.basename(ifwi_out_file))])

    # --------------------------------------------------------------
    # Remove intermediate artifacts (leave autogen DLTs +
    # CfgDataInt.bin in place so re-runs stay fast).
    # --------------------------------------------------------------
    intermediates = bin_files + [
        cfg_ext_bin, signed_bin,
        os.path.join(out_dir, 'Stage1b.fd'),
        os.path.join(out_dir, 'Stage1b.lz'),
        os.path.join(out_dir, 'CfgData.bin.hash'),
        os.path.join(out_dir, 'CfgData.bin.hash.tmp'),
    ]
    for f in intermediates:
        if os.path.exists(f):
            os.remove(f)

def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('-i',
                    '--ifwi-image',
                    dest='ifwi_image',
                    type=str,
                    required=True,
                    help='Specify input IFWI image')
    ap.add_argument('-o',
                    '--output-file',
                    dest='output_file',
                    type=str,
                    default='',
                    help='Specify output IFWI image')
    ap.add_argument('-c',
                    '--cfgdata-dir',
                    dest='cfgdata_dir',
                    type=str,
                    default='',
                    help='CFGDATA directory path')
    ap.add_argument('-k',
                    '--key-file',
                    dest='key_file',
                    type=str,
                    default='',
                    help='Key Id or Signing key path')
    ap.add_argument('-s',
                    '--script-dir',
                    dest='script_dir',
                    type=str,
                    default='',
                    help='Scripts directory path')
    ap.add_argument('-t',
                    '--tool-dir',
                    dest='tool_dir',
                    type=str,
                    default='',
                    help='Compress binary tools directory path')
    ap.add_argument('-p',
                    '--platform-id',
                    dest='platform_id',
                    type=str,
                    default='',
                    help='Force a given platform ID')

    args = ap.parse_args()

    cfgdata_stitch(args.ifwi_image, args.output_file, args.cfgdata_dir,
                   args.key_file, args.script_dir, args.tool_dir, args.platform_id)


if __name__ == '__main__':
    sys.exit(main())
