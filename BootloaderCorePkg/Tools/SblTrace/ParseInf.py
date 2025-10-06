## @ ParseInf.py
#
# Copyright (c) 2025, Intel Corporation. All rights reserved.<BR>
# SPDX-License-Identifier: BSD-2-Clause-Patent
#
##

import json
import os

def ParseInfFile(FilePath):
    if not os.path.exists(FilePath):
        print(f"{FilePath} INF not found")
        exit()

    f = open(FilePath, "r")
    Lines = f.readlines()
    f.close()

    ParsedDict = dict()
    Section    = None

    try:
        for Line in Lines:
            Line = Line.split("#", 1)[0].strip()
            if Line == "":
                continue
            if Line.startswith('[') and Line.endswith(']'):
                Section = Line[1:-1].lower()
            elif Section:
                if '=' in Line:
                    if Section not in ParsedDict:
                        ParsedDict[Section] = dict()
                    Key, Value = Line.split('=', 1)
                    ParsedDict[Section][Key.strip()] = Value.strip()
                else:
                    if Section not in ParsedDict:
                        ParsedDict[Section] = list()
                    ParsedDict[Section].append(Line)
    except Exception as e:
        print("Error:", e)
        return None

    return ParsedDict


def GetAllInfFileList(FolderDest, ExcludeFolderList = None):
    if not os.path.exists(FolderDest):
        print(f"{FolderDest} Folder not found")
        exit()

    InfFileList = []
    for Root, _, Files in os.walk(FolderDest):
        for file in Files:
            if file.endswith('.inf') and not file.endswith('Null.inf'):
                InfFile = os.path.join(Root, file)
                InfIncluded = True
                if ExcludeFolderList:
                    for Folder in ExcludeFolderList:
                        if Folder in InfFile:
                            InfIncluded = False
                            break
                if InfIncluded:
                    InfFileList.append(InfFile)

    # print(json.dumps(InfFileList, indent=2))
    return InfFileList


def GetLibraryName(LibraryData):
    if "defines" in LibraryData:
        LibraryData = LibraryData["defines"]

    if "LIBRARY_CLASS" in LibraryData:
        return LibraryData["LIBRARY_CLASS"]
    elif "BASE_NAME" in LibraryData:
        return LibraryData["BASE_NAME"]
    else:
        return None


def FindDependency(DependencyDict, LibraryKey, Dependencies = None):
    if Dependencies == None:
        Dependencies = dict()
        if LibraryKey in DependencyDict:
            Dependencies[LibraryKey] = DependencyDict[LibraryKey]["file"]

    if LibraryKey in DependencyDict:
        UpdateNeeded = False
        for Dependency in DependencyDict[LibraryKey]["library"]:
            if Dependency not in Dependencies:
                if Dependency in DependencyDict:
                    File = DependencyDict[Dependency]["file"]
                else:
                    File = None
                Dependencies[Dependency] = File
                UpdateNeeded = True
                FindDependency(DependencyDict, Dependency, Dependencies)
        if not UpdateNeeded:
            return Dependencies

    return Dependencies


def FindLibraryDependency(InfLibraryList, BaseLibraryName = "SerialPortLib"):
    FullLibraryDependencyDict = dict()
    LibraryDependencyList     = list()
    for LibraryInf in InfLibraryList:
        LibraryDict = ParseInfFile(LibraryInf)
        LibraryName = GetLibraryName(LibraryDict)
        if LibraryName == None:
            continue

        FullLibraryDependencyDict[LibraryName] = dict()
        FullLibraryDependencyDict[LibraryName]["file"]    = LibraryInf
        if "libraryclasses" in LibraryDict:
            FullLibraryDependencyDict[LibraryName]["library"] = LibraryDict["libraryclasses"]
        else:
            FullLibraryDependencyDict[LibraryName]["library"] = []

    # PrintDict(FullLibraryDependencyDict, "FullLibraryDependencyDict", True)

    LibraryDependencyList = FindDependency(FullLibraryDependencyDict, BaseLibraryName)

    return LibraryDependencyList


def PrintDict(Dict, FileName, Console = False):
    if Dict != None:
        f = open(FileName, "w")
        f.write(json.dumps(Dict, indent=2))
        f.close()

        if Console:
            print((json.dumps(Dict, indent=2)))
