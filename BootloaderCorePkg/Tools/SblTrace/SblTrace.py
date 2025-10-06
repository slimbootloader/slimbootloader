#!/usr/bin/env python3
## @ SblTrace.py
#
# Copyright (c) 2025, Intel Corporation. All rights reserved.<BR>
# SPDX-License-Identifier: BSD-2-Clause-Patent
#
##

import argparse
import subprocess
import os
import re
import threading
import yaml
import json
import time
from collections import defaultdict

import ParseInf
import CheckBuildFailure

SRC_FOLDER_EXCLUDE_LIST = []
SRC_FILE_EXCLUDE_LIST   = []
SRC_FUNCTION_EXCLUDE_LIST = []

if os.name == "nt":
    CSCOPE_BIN    = os.path.join("Tools", "cscope.exe")
    TCEETREE_BIN = os.path.join("Tools", "tceetree.exe")
elif os.name == "posix":
    CSCOPE_BIN    = "cscope"
    TCEETREE_BIN = "tceetree"

TCEETREE_DIR = "tceetree_out"
LOCAL_DIRS   = [TCEETREE_DIR]
LOCAL_FILES  = [
    "cscope.files",
    "cscope.out",
    "tceetree.out",
    "tceetree_dot.out",
    "build_OSLOADER.log",
    "build_STAGE1A.log",
    "build_STAGE1B.log",
    "build_STAGE2.log",
    "build_Success.log"
]

LastNodeFunction = []
LastNodeFunctionIdx = 0
FuncFileMap = defaultdict(list)
ParentFunctionCallDict = dict()
FileNameDict = None
WorkSpacePath = None

UseThreading = True

def GetFullFilePathFromFileName(FileName):
    global FileNameDict
    if FileNameDict == None:
        with open("cscope.files", "r") as readfiles:
            FileLines = readfiles.readlines()
            FileNameDict = dict()
            for Fline in FileLines:
                Name = Fline.split(os.path.sep)[-1].strip()
                FileNameDict[Name] = Fline.strip()

    if FileName in FileNameDict:
        return FileNameDict[FileName]
    return None


def GetParentChildFuncAndFile(Line):
    ParentFuncName = None
    ChildFuncName  = None
    ParentFileName = None

    pattern = r'(.*?)->(.*?) \[label="(.*?)"'
    match = re.search(pattern, Line)
    if match:
        ParentFuncName = match.group(1).strip()
        ChildFuncName  = match.group(2).strip()
        ParentFileName = match.group(3).strip()

    if ParentFileName:
        ParentFileName = GetFullFilePathFromFileName(ParentFileName)

    return ParentFuncName, ChildFuncName, ParentFileName


def GetFileNameOfLastNodeFromFuncName(LastNode):
    pattern = r'subgraph cluster_.*?{ label="(.*?)";.*?; (.*?);'
    with open("tceetree.out", "r") as readdata:
        for Line in readdata.readlines():
            match = re.search(pattern, Line)
            if match:
                FileName = match.group(1).strip()
                FuncName = match.group(2).strip()
                if LastNode == FuncName:
                    if FileName != "LIBRARY":
                        return GetFullFilePathFromFileName(FileName)
                    return FileName
    return None



def GetFileAndFunctionNameFromSubGraph(Line):
    pattern = r'subgraph cluster_.*?{ label="(.*?)";.*?; (.*?);'
    match = re.search(pattern, Line)
    if match:
        FileName = match.group(1).strip()
        FuncName = match.group(2).strip()
        if FileName != "LIBRARY":
            FileName = GetFullFilePathFromFileName(FileName)
        return FileName, FuncName
    return None, None


def UpdateFunctionFileMap(RootFunction, FileName, Index):
    global FuncFileMap
    # print("Appending data for item index:", Index)
    if RootFunction in FuncFileMap:
        if FileName not in FuncFileMap[RootFunction]:
            FuncFileMap[RootFunction].append(FileName)
    else:
        FuncFileMap[RootFunction] = [FileName]


def ReIterateSubGraph(RootFunction, Index):
    CscopeOut = "cscope.out"
    tceetree_out_file_name = f"{TCEETREE_DIR}/{RootFunction}_tceetree.out"

    resp = subprocess.run(f"{TCEETREE_BIN} -i {CscopeOut} -f -F -c 0 -C 0 -r {RootFunction} -o {tceetree_out_file_name}", shell=True)
    if not resp.returncode:
        with open(tceetree_out_file_name, "r") as readdata:
            lines = readdata.readlines()
            for line in lines:
                FileName, _ = GetFileAndFunctionNameFromSubGraph(line)
                if FileName:
                    if UseThreading:
                        thread = threading.Thread(target = UpdateFunctionFileMap, args = (RootFunction, FileName, Index,))
                        thread.start()
                        thread.join()
                    else:
                        UpdateFunctionFileMap(RootFunction, FileName, Index)



def AddPrint (FunctionName, StrPath):
    global WorkSpacePath

    WriteList       = []
    DebugPrintFound = False
    DebugLibFound   = False
    IncludeLibIndex = 0
    FileRelativePath             = os.path.relpath(StrPath, WorkSpacePath)
    if os.name == "nt":
        FileRelativePath         = FileRelativePath.replace(os.path.sep, "/")
    IncludeDebugLibStringPattern = r'#include\s*(?:"|<)?Library/DebugLib\.h(?:"|>)'
    IncludeAnyLibStringPattern   = r'#include\s*(?:"|<)?(.*?)\.h(?:"|>)'
    DebugLibString               = "#include <Library/DebugLib.h>\n"
    DebugString                  = f"\tDEBUG ((DEBUG_ERROR, \"CallGraph Debug %a() from {FileRelativePath}:%d \\n\", __FUNCTION__, __LINE__));\n"

    f = open(StrPath, "r")
    data = f.readlines()
    f.close()

    for Index in range(len(data)):
        line = data[Index]
        WriteList.append(line)
        if not DebugLibFound:
            if re.findall(IncludeDebugLibStringPattern, line):
                DebugLibFound = True
            if IncludeLibIndex == 0 and re.findall(IncludeAnyLibStringPattern, line):
                IncludeLibIndex = Index
        if re.findall(fr'\b{FunctionName}\b', line):
            for i in range(1, 25):
                NextLine = data[Index + i]
                #Preventing the wrong position
                if NextLine.strip() == "}":
                    break
                if NextLine.strip() == "{":
                    NextLine = data[Index + i + 1]
                    if NextLine.strip() == DebugString.strip():
                        break
                    DebugPrintFound = True
                    break
        if DebugPrintFound and line.strip() == "{":
            DebugPrintFound = False
            WriteList.append(DebugString)

    if not DebugLibFound:
        WriteList = [DebugLibString+Line if Index == IncludeLibIndex else Line for Index, Line in enumerate(WriteList)]

    f = open(StrPath, "w")
    f.write("".join(WriteList))
    f.close()


def AddDebugPrint():
    global FuncFileMap
    for Function in FuncFileMap:
        FilePathList = FuncFileMap[Function]
        for FilePath in FilePathList:
            if FilePath and FilePath != "LIBRARY":
                AddPrint(Function, FilePath)


def ParseFuncAndFile():
    global LastNodeFunction
    global ParentFunctionCallDict
    TempFuncFileMap        = dict()
    ChildFunctionCallList  = list()
    SubGraphFuctionDict    = dict()

    with open("tceetree.out", "r") as readdata:
        lines = readdata.readlines()
        #checking if provided function is valid or not
        #If invalid function is given as an argument, then it will generate an empty file of length 3
        if len(lines) < 4:
            print("ERROR: Tceetree.out is blank")
            exit()
        for line in lines:
            ParentFuncName, ChildFuncName, ParentFileName = GetParentChildFuncAndFile(line)
            if ParentFuncName in SRC_FUNCTION_EXCLUDE_LIST or ChildFuncName in SRC_FUNCTION_EXCLUDE_LIST:
                continue

            if ChildFuncName and ParentFileName:
                ParentFunctionCallDict[ParentFuncName] = [ParentFileName]
                ChildFunctionCallList.append(ChildFuncName)

            # if the GetParentChildFuncAndFile() failed for not
            else:
                SubGraphFileName, SubGraphFuctionName = GetFileAndFunctionNameFromSubGraph(line)
                SubGraphFuctionDict[SubGraphFuctionName] = SubGraphFileName

    # To remove the duplicates from the list it converted to set
    for Child in set(ChildFunctionCallList):
        IsLastNode = True
        for Parent in ParentFunctionCallDict:
            if Child == Parent:
                IsLastNode = False
                break
        if IsLastNode:
            LastNodeFunction.append(Child)

    # print("LastNodeFunction::", LastNodeFunction)
    TempFuncFileMap = ParentFunctionCallDict
    for Function in LastNodeFunction:
        FullFilePath = GetFileNameOfLastNodeFromFuncName(Function)
        if FullFilePath != None:
            TempFuncFileMap[Function] = [FullFilePath]

    FuncCount   = 0
    LibCount    = 0
    AllFileList = []
    for item in TempFuncFileMap:
        if TempFuncFileMap[item][0] == "LIBRARY":
            LibCount += 1
        else:
            FuncCount += 1
            for file in TempFuncFileMap[item]:
                if file not in AllFileList:
                    AllFileList.append(file)

    print("Total Functions found:", len(TempFuncFileMap))
    print(f"Library Functions: {LibCount}, User Functions: {FuncCount}")
    print("Total number of files parsed:", len(AllFileList))

    if UseThreading:
        print("\nUsing threads to speed up.")
    else:
        print("\nNo threading used.")

    VerifyFunctionAllIteration(TempFuncFileMap)


def VerifyFunctionAllIteration(FunctionDict):
    threads = []
    for Index, Function in enumerate(FunctionDict):
        if Function == None or FunctionDict[Function][0] == "LIBRARY":
            # print("Skip:", Function)
            continue
        # print("Creating tceetree for :", Function)
        if UseThreading:
            thread = threading.Thread(target = ReIterateSubGraph, args = (Function, Index,))
            threads.append(thread)
            thread.start()
        else:
            ReIterateSubGraph(Function, Index)
    if UseThreading:
        for thread in threads:
            thread.join()


#find . -name '*.c' > cscope.files
#cscope -b -c -R
#tceetree -i cscope.out -r <funtion> -f -F
#dot -Tpdf -O tceetree.out
def GenerateDotFile(RootFunction):
    global SRC_FOLDER_EXCLUDE_LIST
    global SRC_FILE_EXCLUDE_LIST
    global SRC_FUNCTION_EXCLUDE_LIST
    global CSCOPE_BIN
    global TCEETREE_BIN

    readdata  = open("cscope.files", 'r')
    writedata = open("cscope_write.files", "w")
    # Read the file lines using readlines()
    lines = readdata.readlines()
    for line in lines:
        FolderInclude = True
        FileInclude   = True
        for folder in SRC_FOLDER_EXCLUDE_LIST:
            if folder in line:
                FolderInclude = False
                break
        for file in SRC_FILE_EXCLUDE_LIST:
            if file in line:
                FileInclude = False
                break
        if FileInclude and FolderInclude:
            writedata.write(line)


    readdata.close()
    writedata.close()

    if not os.path.exists("cscope_write.files"):
        print("Error: cscope_write.files not Found")
        return -1

    os.replace('cscope_write.files', 'cscope.files')
    resp = subprocess.run(f"{CSCOPE_BIN} -b -c -R -i cscope.files", shell = True)
    if resp.returncode != 0:
        print("Error in creating cscope.files file")
        return resp.returncode

    resp = subprocess.run(f"{TCEETREE_BIN} -i cscope.out -f -F -r {RootFunction}", shell = True)
    if resp.returncode != 0:
        print("Error in creating cscope.out file")
        return resp.returncode

    if not os.path.exists("tceetree.out"):
        print("tceetree.out not found")
        return -1

    readdata = open("tceetree.out", "r")
    dotdata = open("tceetree_dot.out", "w")
    lines = readdata.readlines()
    for line in lines:
        if line.strip().startswith("subgraph "):
            continue
        ParentFuncName, ChildFuncName, ParentFileName = GetParentChildFuncAndFile(line)
        if ParentFuncName in SRC_FUNCTION_EXCLUDE_LIST or ChildFuncName in SRC_FUNCTION_EXCLUDE_LIST:
            continue
        for file in SRC_FILE_EXCLUDE_LIST:
            if ParentFileName and (file in ParentFileName):
                continue
        dotdata.write(line)
    readdata.close()
    dotdata.close()
    # resp = subprocess.run(f"{DOT_BIN} -Tpng -O tceetree_dot.out", shell=False)
    # print("return code:", resp.returncode)

    if resp.returncode != 0:
        print("Error in creating tceetree.out file")
        return resp.returncode

    return resp.returncode


def ParseIgnoreConfigList():
    global SRC_FOLDER_EXCLUDE_LIST
    global SRC_FILE_EXCLUDE_LIST
    global SRC_FUNCTION_EXCLUDE_LIST

    IgnoreListConfig = "IgnoreList.yaml"
    if not os.path.exists(IgnoreListConfig):
        print(f"ERROR: {IgnoreListConfig} Not Found!!!")
        exit()

    with open(IgnoreListConfig) as stream:
        try:
            data = yaml.safe_load(stream)
            SRC_FOLDER_EXCLUDE_LIST = data["IgnoreFolders"]
            SRC_FILE_EXCLUDE_LIST   = data["IgnoreFiles"]
            SRC_FUNCTION_EXCLUDE_LIST = data["IgnoreFunctions"]
        except yaml.YAMLError as exc:
            print(f"ERROR: {exc}")
            exit()


def UpdateSerialPortDependency(DirectoryPath, LibraryName, ExcludeFolderList = None):
    global SRC_FOLDER_EXCLUDE_LIST
    InfFileList    = ParseInf.GetAllInfFileList(DirectoryPath, ExcludeFolderList)
    DependencyDict = ParseInf.FindLibraryDependency(InfFileList, LibraryName)

    # print("Dependency for: ", LibraryName)
    # print(json.dumps(DependencyDict, indent=2))

    for DepencencyFile in DependencyDict.values():
        if DepencencyFile:
            DependencyFolder = os.path.dirname(DepencencyFile)
            DependencyFolder = DependencyFolder.replace(DirectoryPath, "")
            # print(DependencyFolder)
            SRC_FOLDER_EXCLUDE_LIST.append(DependencyFolder)


def ParseConfig():
    Config = "Config.yaml"
    if not os.path.exists(Config):
        print(f"ERROR: {Config} Not Found!!!")
        exit()

    with open(Config) as stream:
        try:
            data = yaml.safe_load(stream)

            return data
        except yaml.YAMLError as exc:
            print(f"ERROR: {exc}")
            exit()


def GetTimeString(TimeInSec, RoundOff = 2):
    TimeStr = ""
    if TimeInSec < 60:
        TimeStr = str(round(TimeInSec, RoundOff)) + " Seconds"
    elif TimeInSec > 3600:
        TimeStr = str(round(TimeInSec // 3600)) + " Hours " + \
                  str(round((TimeInSec % 3600) // 60)) + " Minutes " + \
                  str(round((TimeInSec % 60), RoundOff)) + " Seconds"
    else:
        TimeStr = str(round(TimeInSec // 60)) + " Minutes " + \
                  str(round((TimeInSec % 60), RoundOff)) + " Seconds"

    return TimeStr


def CleanupWorkspace():
    global LOCAL_FILES
    global LOCAL_DIRS

    print("Cleaning up the workspace")
    for file in LOCAL_FILES:
        if os.path.exists(file):
            os.remove(file)

    for dir in LOCAL_DIRS:
        if os.path.exists(dir):
            for root, dirs, files in os.walk(dir):
                for file in files:
                    os.remove(os.path.join(root, file))
            os.rmdir(dir)


def CheckToolAvailability(Tool):
    if os.name == "nt":
        if not os.path.exists(Tool):
            print(f"ERROR: {Tool} Not Found!!!")
            exit()
        else:
            print(f"{Tool} found at: {os.path.abspath(Tool)}")
    elif os.name == "posix":
        result = subprocess.run(f"which {Tool}", shell=True, capture_output=True, text=True)
        if result.returncode != 0:
            print(f"ERROR: {Tool} Not Found!!!")
            exit()
        else:
            tool_path = result.stdout.strip()
            print(f"{Tool} found at: {tool_path}")


def main():
    global LastNodeFunction
    global FuncFileMap
    global WorkSpacePath

    ScritSrartTime       = time.time()
    BuildCompletedTime   = ScritSrartTime
    PrintAddCompleteTime = ScritSrartTime

    Config = ParseConfig()

    parser = argparse.ArgumentParser(description='SblTrace - Slim Bootloader Automated Debug and Triage Tool')

    # Required arguments
    required_group = parser.add_argument_group('required arguments')
    required_group.add_argument("-f", "--FunctionName", required=True, metavar="FUNCTION_NAME", help="Last reported error function name")
    required_group.add_argument("-d", "--DirectoryPath", required=True, metavar="SBL_PATH", help="Path to Slim Bootloader source directory")
    required_group.add_argument("-p", "--Platform", required=True, choices = Config.keys(), metavar="PLATFORM_NAME", help="Platform name as defined in Config.yaml file")
    # Parse the command line arguments
    args = parser.parse_args()
    print("Function Name is: " + args.FunctionName)
    print("SBL Path is: " + args.DirectoryPath)
    WorkSpacePath = args.DirectoryPath

    CleanupWorkspace()
    CheckToolAvailability(CSCOPE_BIN)
    CheckToolAvailability(TCEETREE_BIN)

    if os.name == "nt":
        print("Execute for Windows")
        resp = subprocess.run(f"dir /B /S {args.DirectoryPath}\*.c > cscope.files", shell=True)
    elif os.name == "posix":
        print("Execute for Linux")
        resp = subprocess.run(f"find {args.DirectoryPath} -name '*.c' > cscope.files", shell=True)

    if resp.returncode != 0:
        print("Error in cscope.out file creation")
        return resp.returncode


    ParseIgnoreConfigList()
    IgnoreLibraryName = [
        # "Stage1A",
        "SerialPortLib",
        "DebugAgentLib",
        "WatchDogTimerLib",
        "PlatformHookLib"
    ]
    for Library in IgnoreLibraryName:
        UpdateSerialPortDependency(args.DirectoryPath, Library, SRC_FOLDER_EXCLUDE_LIST)

    if GenerateDotFile(args.FunctionName) == 0:
        print('CallGraph generated Successfully..!')
        os.makedirs(TCEETREE_DIR, exist_ok = True)
        ParseFuncAndFile()
        AddDebugPrint()
        # for item in FuncFileMap:
        #     print(len(FuncFileMap[item]), item, "\t", FuncFileMap[item])
        print(f"\nNumber of Debug Prints Added: {len(FuncFileMap)}\n")
        PrintAddCompleteTime = time.time()
        print("Checking the Build failure due to FV size")
        BuildExitCode, BuildTimeList, InvestigationTimeList = CheckBuildFailure.ResolveBuildFailure(args.DirectoryPath, Config[args.Platform])
        # print(BuildTimeList)
        # print(InvestigationTimeList)
        if BuildExitCode:
            print("Build Successful")
        else:
            print("Build Failure; Some out of Scope issue arised!!")
        BuildCompletedTime = time.time()
    else:
        print('CallGraph generation Failed!!')

    print("\n")
    print("Time taken to add Debug prints        :", GetTimeString(PrintAddCompleteTime - ScritSrartTime))
    print("Time taken for the first build        :", GetTimeString(BuildTimeList[0]))
    print("Time taken for Build Failure Check    :", GetTimeString(BuildCompletedTime - PrintAddCompleteTime - BuildTimeList[0]))
    print("Time taken for Total Execution        :", GetTimeString(BuildCompletedTime - ScritSrartTime))

    CleanupWorkspace()

if __name__ == "__main__":
    main()
