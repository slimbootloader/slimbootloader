## @ CheckBuildFailure.py
#
# Copyright (c) 2025, Intel Corporation. All rights reserved.<BR>
# SPDX-License-Identifier: BSD-2-Clause-Patent
#
##

import yaml
import os
import subprocess
import re
import time
import shutil

VariableNameDict = {
    "STAGE1A"  : "STAGE1A_SIZE",
    "STAGE1B"  : "STAGE1B_SIZE",
    "STAGE2"   : "STAGE2_FD_SIZE",
    "OSLOADER" : "OS_LOADER_FD_SIZE",
}

BuildLog = "build_{ErrorType}.log"

BuildTimeList         = list()
InvestigationTimeList = list()


def RenameBuildLog(Suffix):
    global BuildLog

    ExistBuildLog = BuildLog.format(ErrorType = "Success")
    NewBuildLog   = BuildLog.format(ErrorType = Suffix)
    if os.path.exists(ExistBuildLog):
        print(f"Renaming {ExistBuildLog} to {NewBuildLog}")
        if os.path.exists(NewBuildLog):
            os.remove(NewBuildLog)
        shutil.copyfile(ExistBuildLog, NewBuildLog)


def GetBoardConfigFilePath(Path, PlatformConfig):
    BoardPkgDir     = None
    BoardPkgName    = PlatformConfig["BoardPkg"]+"BoardPkg"
    IntBoardPkgName = PlatformConfig["BoardPkg"]+"BoardIntPkg"

    for Root, Dirs, _ in os.walk(Path):
        if BoardPkgName in Dirs:
            BoardPkgDir = os.path.join(Root, BoardPkgName)
            break
        elif IntBoardPkgName in Dirs:
            BoardPkgDir = os.path.join(Root, IntBoardPkgName)
            break

    if BoardPkgDir:
        print("BoardPkg Directory:", BoardPkgDir)
        if PlatformConfig["BoardConfig"] in os.listdir(BoardPkgDir):
            return os.path.join(BoardPkgDir, PlatformConfig["BoardConfig"])

    print("ERROR: BoardConfig File Not Found.")
    return None


def UpdateBoardConfig(BoardConfigPython, FvType, SizeDiff):
    if not os.path.exists(BoardConfigPython):
        print("BoardConfig not found")
        return False

    f = open(BoardConfigPython, "r")
    Lines = f.readlines()
    f.close()

    Pattern = fr'self.{VariableNameDict[FvType]}\s*=\s*(0x[0-9a-fA-F]+)'
    NewBoardConfig = []
    for Line in Lines:
        match = re.search(Pattern, Line.strip())
        if match:
            OldValue = match.group(1)
            NewValue = int(OldValue, 16) + SizeDiff
            NewValue = ((NewValue // 0x1000) + 1) * 0x1000 # Alignment to 0x1000
            NewValue = hex(NewValue)
            NewValue = "0x" + (NewValue.replace("0x", "").zfill((len(OldValue) - 2))).upper()
            Line     = Line.replace(OldValue, NewValue)
            print(f"Updated BoardConfig For {FvType} size to {NewValue}")
        NewBoardConfig.append(Line)

    f = open(BoardConfigPython, "w")
    for Line in NewBoardConfig:
        f.write(Line)
    f.close()


def InvestigateBuildLog(BoardConfigPython):
    global BuildLog

    _BuildLog = BuildLog.format(ErrorType = "Success")

    if not os.path.exists(_BuildLog):
        print("Build Log not found")
        return False

    f = open(_BuildLog, "r")
    Lines = f.readlines()
    f.close()

    Pattern = r'Generating (.*?) FV'
    FvType = None
    for Line in Lines:
        match = re.search(Pattern, Line)
        if match:
            FvType = match.group(1).strip()

    if FvType:
        print("Issue in FV type:", FvType)
        RenameBuildLog(FvType)
        # the required fv image size 0x20708 exceeds the set fv image size 0x1e000
        Pattern = r'the required fv image size (0x[0-9a-fA-F]+) exceeds the set fv image size (0x[0-9a-fA-F]+)'
        ProvidedSize   = None
        RequiredSize = None
        for Line in Lines:
            match = re.search(Pattern, Line.strip())
            if match:
                RequiredSize = match.group(1)
                ProvidedSize = match.group(2)
        if ProvidedSize and RequiredSize:
            print(f"{FvType} Required Size: {RequiredSize}\tActual Size: {ProvidedSize}")
            ProvidedSize  = int(ProvidedSize, 16)
            RequiredSize  = int(RequiredSize, 16)
            SizeDiff      = RequiredSize - ProvidedSize
            UpdateBoardConfig(BoardConfigPython, FvType, SizeDiff)
            return True
    return False


def WorkspaceClean(Path, CleanCmd):
    if os.name == 'posix':
        NullDevice = '/dev/null'
    elif os.name == 'nt':   # Windows
        NullDevice = 'NUL'

    with open(NullDevice, "w") as NullFile:
        subprocess.run(CleanCmd,
                       stdout = NullFile,
                       stderr = NullFile,
                       shell = True,
                       cwd = Path)


def BuildAndCheck(Path, CleanCmd, BuildCmd, BoardConfigPython):
    global BuildLog

    _BuildLog = BuildLog.format(ErrorType = "Success")
    print("Cleaning ...")
    WorkspaceClean(Path, CleanCmd)

    print("Building ...")
    BuildTimeStart = time.time()
    with open(_BuildLog, "w") as LogFile:
        res = subprocess.run(BuildCmd,
                             stdout = LogFile,
                             stderr = LogFile,
                             shell = True,
                             cwd = Path)
        BuildTimeList.append(time.time() - BuildTimeStart)
        if res.returncode != 0:
            print("Build failed; Invertigate Build log ...")
            InvestigationTimeStart = time.time()
            ReturnCode = InvestigateBuildLog(BoardConfigPython)
            InvestigationTimeList.append(time.time() - InvestigationTimeStart)
            if ReturnCode:
                BuildAndCheck(Path, CleanCmd, BuildCmd, BoardConfigPython)
            else:
                return False
    return True


def ResolveBuildFailure(Path, PlatformConfig):
    CleanCmd = PlatformConfig["CleanCmd"]
    BuildCmd = PlatformConfig["BuildCmd"]

    WorkspaceClean(Path, CleanCmd)

    BoardConfigPython = GetBoardConfigFilePath(Path, PlatformConfig)

    if BoardConfigPython == None:
        exit()

    return BuildAndCheck(Path, CleanCmd, BuildCmd, BoardConfigPython), BuildTimeList, InvestigationTimeList
