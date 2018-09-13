## @ ConfigEditor.py
#
# Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
# This program and the accompanying materials are licensed and made available under
# the terms and conditions of the BSD License that accompanies this distribution.
# The full text of the license may be found at
# http://opensource.org/licenses/bsd-license.php.
#
# THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
# WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
#
##

import os
import re
import sys
import marshal
from Tkinter import *
import ttk
import tkMessageBox
import tkFileDialog

from GenCfgData import CGenCfgData, Bytes2Str, Bytes2Val, Array2Val


class CreateToolTip(object):
    '''
    create a tooltip for a given widget
    '''
    InProgress = False

    def __init__(self, Widget, Text=''):
        self.TopWin = None
        self.Widget = Widget
        self.Text = Text
        self.Widget.bind("<Enter>", self.Enter)
        self.Widget.bind("<Leave>", self.Leave)

    def Enter(self, event=None):
        if self.InProgress:
            return
        x, y, cx, cy = self.Widget.bbox("insert")
        Cursor = self.Widget.winfo_pointerxy()
        x += self.Widget.winfo_rootx() + 35
        y = self.Widget.winfo_rooty() + 20
        if Cursor[1] > y and Cursor[1] < y + 20:
            y += 20

        # creates a toplevel window
        self.TopWin = Toplevel(self.Widget)
        # Leaves only the label and removes the app window
        self.TopWin.wm_overrideredirect(True)
        self.TopWin.wm_geometry("+%d+%d" % (x, y))
        label = Label(self.TopWin,
                      text=self.Text,
                      justify='left',
                      background='bisque',
                      relief='solid',
                      borderwidth=1,
                      font=("times", "9", "normal"))
        label.pack(ipadx=1)
        self.InProgress = True

    def Leave(self, event=None):
        if self.TopWin:
            self.TopWin.destroy()
            self.InProgress = False


class ValidatingEntry(Entry):
    def __init__(self, master, value="", **kw):
        apply(Entry.__init__, (self, master), kw)
        self.Value = value
        self.Variable = StringVar()
        self.Variable.set(value)
        self.Variable.trace("w", self.Callback)
        self.config(textvariable=self.Variable)
        self.bind("<FocusOut>", self.FocusOut)

    def FocusOut(self, Event):
        Value = self.Variable.get()
        if len(Value) == 0:
            self.Value = '00'
        else:
            self.Value = '%02X' % int(Value, 16)
        self.Variable.set(self.Value)

    def Callback(self, *Args):
        CurVal = self.Variable.get()
        NewVal = self.Validate(CurVal)
        if NewVal is None:
            self.Variable.set(self.Value)
        else:
            self.Value = NewVal
            self.Variable.set(self.Value)

    def Validate(self, Value):
        if len(Value) > 0:
            try:
                int(Value, 16)
            except:
                return None

        if len(Value) > 2:
            return None

        return Value.upper()


class CustomTable(Frame):
    def __init__(self, Parent, ColHdr, Bins):
        Frame.__init__(self, Parent)
        Idx = 0
        Cols = len(ColHdr)
        if Cols > 0:
            Rows = (len(Bins) + Cols - 1) // Cols
        self.Row = Rows
        self.Col = Cols
        ColAdj = 2
        RowAdj = 1
        for Col in range(Cols):  #Columns
            Text = ColHdr[Col].split(':')[0]
            Header = Label(self, text=Text)
            Header.grid(row=0, column=Col + ColAdj)
        for Row in range(Rows):  #Rows
            Text = '%04X' % (Row * len(ColHdr))
            Header = Label(self, text=Text)
            Header.grid(row=Row + RowAdj,
                        column=0,
                        columnspan=1,
                        sticky='ewsn')
            for Col in range(Cols):  #Columns
                if Idx >= len(Bins):
                    break
                Hex = "%02X" % Bins[Idx]
                ValidatingEntry(self, width=4,
                                justify=CENTER,
                                value=Hex).grid(row=Row + RowAdj,
                                                column=Col + ColAdj)
                Idx += 1
            if Idx >= len(Bins):
                break

    def get(self):
        Bins = bytearray()
        for Widget in self.winfo_children():
            if not isinstance(Widget, ValidatingEntry):
                continue
            Hex = Widget.get()
            if not Hex:
                break
            Bins.append(int(Hex, 16) & 0xff)
        return Bins

    def destroy(self):
        for Widget in self.winfo_children():
            Widget.destroy()
        Frame.destroy(self)


class Application(Frame):
    def __init__(self, master=None):
        Root = master

        self.Debug = True
        self.LastDir = '.'
        self.PageId = ''
        self.PageList = {}
        self.ConfList = {}
        self.CfgDataObj = None
        self.OrgCfgDataBin = None

        Frame.__init__(self, master, borderwidth=2)

        self.MenuString = [
            'Save Config Data to Binary', 'Load Config Data from Binary',
            'Load Config Changes from Delta File',
            'Save Config Changes to Delta File',
            'Save Full Config Data to Delta File'
        ]

        Root.geometry("1200x800")

        Paned = ttk.Panedwindow(Root, orient=HORIZONTAL)
        Paned.pack(fill=BOTH, expand=True, padx=(4, 4))

        Status = Label(master, text="", bd=1, relief=SUNKEN, anchor=W)
        Status.pack(side=BOTTOM, fill=X)

        FrameLeft = ttk.Frame(Paned, height=800, relief="groove")

        self.Left = ttk.Treeview(FrameLeft, show="tree")

        # Set up tree HScroller
        Pady = (10, 10)
        self.TreeScroll = ttk.Scrollbar(FrameLeft,
                                        orient="vertical",
                                        command=self.Left.yview)
        self.Left.configure(yscrollcommand=self.TreeScroll.set)
        self.Left.bind('<<TreeviewSelect>>', self.OnConfigPageSelectChange)
        self.Left.pack(side='left',
                       fill=BOTH,
                       expand=True,
                       padx=(5, 0),
                       pady=Pady)
        self.TreeScroll.pack(side='right', fill=Y, pady=Pady, padx=(0, 5))

        FrameRight = ttk.Frame(Paned, relief="groove")
        self.FrameRight = FrameRight

        self.ConfCanvas = Canvas(FrameRight, highlightthickness=0)
        self.PageScroll = ttk.Scrollbar(FrameRight,
                                        orient="vertical",
                                        command=self.ConfCanvas.yview)
        self.RightGrid = ttk.Frame(self.ConfCanvas)
        self.ConfCanvas.configure(yscrollcommand=self.PageScroll.set)
        self.ConfCanvas.pack(side='left',
                             fill=BOTH,
                             expand=True,
                             pady=Pady,
                             padx=(5, 0))
        self.PageScroll.pack(side='right', fill=Y, pady=Pady, padx=(0, 5))
        self.ConfCanvas.create_window(0, 0, window=self.RightGrid, anchor='nw')
        #self.ConfCanvas.bind_all('<MouseWheel>', self.OnPageScroll)
        self.ConfCanvas.bind("<Configure>", self.OnCanvasConfigure)

        Paned.add(FrameLeft, weight=2)
        Paned.add(FrameRight, weight=10)

        Style = ttk.Style()
        Style.layout("Treeview", [('Treeview.treearea', {'sticky': 'nswe'})])

        Menubar = Menu(Root)
        FileMenu = Menu(Menubar, tearoff=0)
        FileMenu.add_command(label="Open Config DSC file...",
                             command=self.LoadFromDsc)
        FileMenu.add_command(label=self.MenuString[0],
                             command=self.SaveToBin,
                             state='disabled')
        FileMenu.add_command(label=self.MenuString[1],
                             command=self.LoadFromBin,
                             state='disabled')
        FileMenu.add_command(label=self.MenuString[2],
                             command=self.LoadFromDelta,
                             state='disabled')
        FileMenu.add_command(label=self.MenuString[3],
                             command=self.SaveToDelta,
                             state='disabled')
        FileMenu.add_command(label=self.MenuString[4],
                             command=self.SaveFullToDelta,
                             state='disabled')
        FileMenu.add_command(label="About", command=self.About)
        Menubar.add_cascade(label="File", menu=FileMenu)
        self.FileMenu = FileMenu

        Root.config(menu=Menubar)


    def SetObjectName(self, Widget, Name):
        self.ConfList[id(Widget)] = Name

    def GetObjectName(self, Widget):
        if id(Widget) in self.ConfList:
            return self.ConfList[id(Widget)]
        else:
            return None

    def LimitEntrySize(self, Variable, Limit):
        Value = Variable.get()
        if len(Value) > Limit: Variable.set(Value[:Limit])

    def Donothing():
        print "12345"
        pass

    def OnCanvasConfigure(self, Event):
        self.RightGrid.grid_columnconfigure(0, minsize=Event.width)

    def OnPageScroll(self, Event):
        self.ConfCanvas.yview_scroll(-1 * Event.delta, 'units')

    def UpdateVisibilityForWidget(self, Widget, Args):

        Visible = True

        if isinstance(Widget, Label):
            Item = self.GetConfigDataItemFromWidget(Widget, True)
        else:
            Item = self.GetConfigDataItemFromWidget(Widget, True)

        if Item is None:
            return Visible
        elif not Item:
            return Visible

        if Item['condition']:
            Visible = self.EvaluateCondition(Item['condition'])

        if Visible:
            Widget.grid()
        else:
            Widget.grid_remove()

        return Visible

    def UpdateWidgetsVisibilityOnPage(self):
        self.WalkWidgetsInLayout(self.RightGrid,
                                 self.UpdateVisibilityForWidget)

    def ComboSelectChanged(self, Event):
        self.UpdateConfigDataFromWidget(Event.widget, None)
        self.UpdateWidgetsVisibilityOnPage()

    def EditNumFinished(self, Event):
        Widget = Event.widget
        Item = self.GetConfigDataItemFromWidget(Widget)
        if not Item:
            return
        Parts = Item['type'].split(',')
        if len(Parts) > 3:
            Min = Parts[2].lstrip()[1:]
            Max = Parts[3].rstrip()[:-1]
            MinVal = Array2Val(Min)
            MaxVal = Array2Val(Max)
            Text = Widget.get()
            if ',' in Text:
                Text = '{ %s }' % Text
            try:
                Value = Array2Val(Text)
                if Value < MinVal or Value > MaxVal:
                    raise Exception('Invalid input!')
                self.SetConfigItemValue(Item, Text)
            except Exception as e:
                pass

            Text = Item['value'].strip('{').strip('}').strip()
            Widget.delete(0, END)
            Widget.insert(0, Text)

        self.UpdateWidgetsVisibilityOnPage()

    def UpdatePageScrollBar(self):
        # Update scrollbar
        self.FrameRight.update()
        self.ConfCanvas.config(scrollregion=self.ConfCanvas.bbox("all"))

    def OnConfigPageSelectChange(self, Event):
        self.UpdateConfigDataOnPage()
        PageId = self.Left.selection()[0]
        self.BuildConfigDataPage(PageId)
        self.UpdateWidgetsVisibilityOnPage()
        self.UpdatePageScrollBar()

    def WalkWidgetsInLayout(self, Parent, CallbackFunction, Args=None):
        for Widget in Parent.winfo_children():
            CallbackFunction(Widget, Args)

    def ClearWidgetsInLayout(self, Parent=None):
        if Parent is None:
            Parent = self.RightGrid

        for Widget in Parent.winfo_children():
            Widget.destroy()

        Parent.grid_forget()
        self.ConfList.clear()

    def BuildConfigPageTree(self, CfgPage, Parent):
        for Page in CfgPage:
            PageId = next(iter(Page))
            # Put CFG items into related page list
            self.PageList[PageId] = [
                Item
                for Item in self.CfgDataObj._CfgItemList
                if Item['name'] and (Item['page'] == PageId)
            ]
            self.PageList[PageId].sort (key=lambda x: x['order'])
            PageName = self.CfgDataObj._CfgPageDict[PageId]
            Child = self.Left.insert(
                Parent, 'end',
                iid=PageId, text=PageName,
                value=0)
            if len(Page[PageId]) > 0:
                self.BuildConfigPageTree(Page[PageId], Child)

    def IsConfigDataLoaded(self):
        return True if len(self.PageList) else False

    def SetCurrentConfigPage(self, PageId):
        self.PageId = PageId

    def GetCurrentConfigPage(self):
        return self.PageId

    def GetCurrentConfigData(self):
        PageId = self.GetCurrentConfigPage()
        if PageId in self.PageList:
            return self.PageList[PageId]
        else:
            return []

    def BuildConfigDataPage(self, PageId):
        self.ClearWidgetsInLayout()
        self.SetCurrentConfigPage(PageId)
        DispList = []
        for Item in self.GetCurrentConfigData():
            if Item['subreg']:
                for SubItem in Item['subreg']:
                    DispList.append(SubItem)
            else:
                DispList.append(Item)
        Row = 0
        DispList.sort(key=lambda x:x['order'])
        for Item in DispList:
            self.AddConfigItem (Item, Row)
            Row += 2

    def LoadConfigData(self, FileName):
        GenCfgData = CGenCfgData()
        if FileName.endswith('.pkl'):
            with open(FileName, "rb") as PklFile:
                GenCfgData.__dict__ = marshal.load(PklFile)
        elif FileName.endswith('.dsc'):
            if GenCfgData.ParseDscFile(FileName) != 0:
                raise Exception(GenCfgData.Error)
            if GenCfgData.CreateVarDict() != 0:
                raise Exception(GenCfgData.Error)
        else:
            raise Exception('Unsupported file "%s" !' % FileName)
        GenCfgData.UpdateDefaultValue()
        return GenCfgData

    def About(self):
        Msg = 'Configuration Editor\n--------------------------------\nVersion 0.5\n2018'
        Lines = Msg.split('\n')
        Width = 30
        Text = []
        for Line in Lines:
            Text.append(Line.center(Width, ' '))
        tkMessageBox.showinfo('Config Editor', '\n'.join(Text))

    def GetOpenFileName(self, Type):
        if self.IsConfigDataLoaded():
            if Type == 'dlt':
                Question = 'Configuration will be changed using Delta file, continue ?'
            elif Type == 'bin':
                Question = 'All configuration will be reloaded from BIN file, continue ?'
            elif Type == 'dsc':
                Question = 'All configuration will be reloaded from DSC file, continue ?'
            else:
                raise Exception('Unsupported file type !')
            Reply = tkMessageBox.askquestion('', Question, icon='warning')
            if Reply == 'no':
                return None

        if Type == 'dsc':
            FileType = 'DSC or PKL'
            FileExt  = 'dsc *.pkl'
        else:
            FileType = Type.upper()
            FileExt  = Type

        Path = tkFileDialog.askopenfilename(
            initialdir=self.LastDir,
            title="Load file",
            filetypes=(("%s files" % FileType, "*.%s" % FileExt), (
                "all files", "*.*")))
        if Path:
            self.LastDir = os.path.dirname(Path)
            return Path
        else:
            return None


    def LoadFromDelta(self):
        Path = self.GetOpenFileName('dlt')
        if not Path:
            return

        self.ReloadConfigDataFromBin(self.OrgCfgDataBin)

        try:
            self.CfgDataObj.OverrideDefaultValue(Path)
            self.CfgDataObj.UpdateDefaultValue()
        except Exception as e:
            tkMessageBox.showerror('LOADING ERROR', str(e))
            return

        self.RefreshConfigDataPage()

    def LoadFromBin(self):
        Path = self.GetOpenFileName('bin')
        if not Path:
            return

        with open(Path, 'rb') as Fd:
            BinData = bytearray(Fd.read())

        self.ReloadConfigDataFromBin(BinData)
        try:
            self.ReloadConfigDataFromBin(BinData)
        except Exception as e:
            tkMessageBox.showerror('LOADING ERROR', str(e))
            return

    def LoadDscFile(self, Path):
        # Save current values in widget and clear  database
        self.ClearWidgetsInLayout()
        self.Left.delete(*self.Left.get_children())

        try:
            self.CfgDataObj = self.LoadConfigData(Path)
        except Exception as e:
            tkMessageBox.showerror('LOADING ERROR', str(e))
            return -1

        self.OrgCfgDataBin = self.CfgDataObj.GenerateBinaryArray()

        self.BuildConfigPageTree(self.CfgDataObj._CfgPageTree['root'], '')

        for Menu in self.MenuString:
            self.FileMenu.entryconfig(Menu, state="normal")

        return 0

    def LoadFromDsc(self):
        Path = self.GetOpenFileName('dsc')
        if not Path:
            return

        self.LoadDscFile(Path)

    def GetSaveFileName (self, Extension):
        Path = tkFileDialog.asksaveasfilename(
                  initialdir=self.LastDir,
                  title="Save file",
                  defaultextension=Extension)
        if Path:
            self.LastDir = os.path.dirname(Path)
            return Path
        else:
            return None



    def SaveDeltaFile(self, Full=False):
        Path = self.GetSaveFileName (".dlt")
        if not Path:
            return

        self.UpdateConfigDataOnPage()
        self.GenerateDeltaFile(Path, Full)

    def SaveToDelta(self):
        self.SaveDeltaFile()

    def SaveFullToDelta(self):
        self.SaveDeltaFile(True)

    def SaveToBin(self):
        Path = self.GetSaveFileName (".bin")
        if not Path:
            return

        self.UpdateConfigDataOnPage()
        with open(Path, 'wb') as Fd:
            Bins = self.CfgDataObj.GenerateBinaryArray()
            Fd.write(Bins)

    def GenerateDeltaFile(self, DeltaFile, Full=False):
        NewData = self.CfgDataObj.GenerateBinaryArray()
        Lines = []
        TagName = ''
        Level = 0
        PlatformId = None
        DefPlatformId = 0

        for Item in self.CfgDataObj._CfgItemList:
            if Level == 0 and Item['embed'].endswith(':START'):
                TagName = Item['embed'].split(':')[0]
                Level += 1

            Start = Item['offset']
            End = Start + Item['length']
            FullName = '%s.%s' % (TagName, Item['cname'])
            if 'PLATFORMID_CFG_DATA.PlatformId' == FullName:
                DefPlatformId = Bytes2Val(self.OrgCfgDataBin[Start:End])

            if NewData[Start:End] != self.OrgCfgDataBin[Start:End] or (
                    Full and Item['name'] and (Item['cname'] != 'Dummy')):
                if not Item['subreg']:
                    Text = '%-40s | %s' % (FullName, Item['value'])
                    if 'PLATFORMID_CFG_DATA.PlatformId' == FullName:
                        PlatformId = Array2Val(Item['value'])
                    else:
                        Lines.append(Text)
                else:
                    OldArray = self.OrgCfgDataBin[Start:End]
                    NewArray = NewData[Start:End]
                    for SubItem in Item['subreg']:
                        NewBitValue = self.CfgDataObj.GetBsfBitFields(SubItem,
                                                                      NewArray)
                        OldBitValue = self.CfgDataObj.GetBsfBitFields(SubItem,
                                                                      OldArray)
                        if OldBitValue != NewBitValue or (
                                Full and Item['name'] and
                            (Item['cname'] != 'Dummy')):
                            if SubItem['cname'].startswith(Item['cname']):
                                Offset = len(Item['cname']) + 1
                                FieldName = '%s.%s' % (
                                    FullName, SubItem['cname'][Offset:])
                            Text = '%-40s | %s' % (FieldName, SubItem['value'])
                            Lines.append(Text)

            if Item['embed'].endswith(':END'):
                EndTagName = Item['embed'].split(':')[0]
                if EndTagName == TagName:
                    Level -= 1

        if PlatformId is None or DefPlatformId == PlatformId:
            PlatformId = DefPlatformId
            print("WARNING: 'PlatformId' configuration is same as default %d!"
                  % PlatformId)

        Lines.insert(0, '%-40s | %s\n\n' %
                     ('PLATFORMID_CFG_DATA.PlatformId', '0x%04X' % PlatformId))
        self.CfgDataObj.WriteDeltaFile(DeltaFile, PlatformId, Lines)

    def RefreshConfigDataPage(self):
        self.ClearWidgetsInLayout()
        self.OnConfigPageSelectChange(None)

    def ReloadConfigDataFromBin(self, BinDat):
        self.CfgDataObj.LoadDefaultFromBinaryArray(BinDat)
        self.RefreshConfigDataPage()

    def NormalizeValueStr(self, Item, ValueStr):
        IsArray = True if Item['value'].startswith('{') else False
        if IsArray and not ValueStr.startswith('{'):
            ValueStr = "{ %s }" % ValueStr
        try:
            OldBytes = self.CfgDataObj.ValueToByteArray(Item['value'],
                                                        Item['length'])
            NewBytes = self.CfgDataObj.ValueToByteArray(ValueStr,
                                                        Item['length'])
            if OldBytes == NewBytes:
                NewValue = Item['value']
            else:
                if IsArray:
                    NewValue = Bytes2Str(NewBytes)
                else:
                    if Item['value'].startswith('0x'):
                        HexLen = Item['length'] * 2
                        if len(Item['value']) == HexLen + 2:
                            Fmt = '0x%%0%dX' % HexLen
                        else:
                            Fmt = '0x%X'
                    else:
                        Fmt = '%d'
                    NewValue = Fmt % Bytes2Val(NewBytes)
        except:
            NewValue = Item['value']
        return NewValue

    def SetConfigItemValue(self, Item, ValueStr):
        Type = Item['type'].split(',')[0]
        if Type == "Table":
            NewValue = ValueStr
        elif Type == "EditText":
            NewValue = ValueStr[:Item['length']]
            if Item['value'].startswith("'"):
                NewValue = "'%s'" % NewValue
        else:
            NewValue = self.NormalizeValueStr(Item, ValueStr)

        if Item['value'] != NewValue:
            if self.Debug:
                print('Update %s from %s to %s !' % (Item['cname'],
                                                     Item['value'], NewValue))
            Item['value'] = NewValue

    def SyncConfigDataFromSubRegion(self):
        if not len(self.PageList) or not self.PageId:
            return

        for Item in self.GetCurrentConfigData():
            if not Item['subreg']:
                continue
            ValArray = self.CfgDataObj.ValueToByteArray(Item['value'],
                                                        Item['length'])
            for SubItem in Item['subreg']:
                Value = Array2Val(SubItem['value'])
                self.CfgDataObj.UpdateBsfBitFields(SubItem, Value, ValArray)
            if Item['value'].startswith('{'):
                NewValue = Bytes2Str(ValArray)
            else:
                BitsValue = ''.join('{0:08b}'.format(i)
                                    for i in ValArray[::-1])
                NewValue = '0x%X' % (int(BitsValue, 2))
                NewValue = self.NormalizeValueStr(Item, NewValue)
            if NewValue != Item['value']:
                if self.Debug:
                    print('Update Subregion %s from %s to %s' %
                          (Item['cname'], Item['value'], NewValue))
                Item['value'] = NewValue

    def GetConfigDataItemFromWidget(self, Widget, Label=False):
        Name = self.GetObjectName(Widget)
        if not Name or not len(self.PageList):
            return None

        if Label and Name.startswith('LABEL_'):
            Name = Name[6:]

        Item = 0
        for Conf in self.GetCurrentConfigData():
            if Conf['subreg']:
                for SubConf in Conf['subreg']:
                    if SubConf['cname'] == Name:
                        Item = SubConf
                        break
            else:
                if Conf['cname'] == Name:
                    Item = Conf
                    break
            if Item:
                break

        return Item

    def UpdateConfigDataFromWidget(self, Widget, Args):
        Item = self.GetConfigDataItemFromWidget(Widget)
        if Item is None:
            return
        elif not Item:
            if isinstance(Widget, Label):
                return
            raise Exception('Failed to find "%s" !' %
                            self.GetObjectName(Widget))

        Type = Item['type'].split(',')[0]
        if Type == "Combo":
            if Item['option'] in self.CfgDataObj._BuidinOption:
                OptList = [('0', 'Disable'), ('1', 'Enable')]
            else:
                OptList = self.CfgDataObj.GetItemOptionList(Item)
            TmpList = [Opt[0] for Opt in OptList]
            Idx = Widget.current()
            self.SetConfigItemValue(Item, TmpList[Idx])
        elif Type in ["EditNum", "EditText"]:
            self.SetConfigItemValue(Item, Widget.get())
        elif Type in ["Table"]:
            NewValue = Bytes2Str(Widget.get())
            self.SetConfigItemValue(Item, NewValue)

    def GetConfigDataItemFromName(self, Name, InPage=True):
        CfgItemList = self.GetCurrentConfigData(
        ) if InPage else self.CfgDataObj._CfgItemList
        for Item in CfgItemList:
            if not Item['length'] or not Item['name']:
                continue
            if Item['subreg']:
                for SubItem in Item['subreg']:
                    if not SubItem['name']:
                        continue
                    if SubItem['cname'] == Name:
                        return SubItem
            else:
                if Item['cname'] == Name:
                    return Item
        return None

    def EvaluateCondition(self, Cond):
        Item = None
        ReplaceList = []
        Parts = re.findall("\$(\w+)(\.\w+)?", Cond)
        for Part in Parts:
            if len(Part) > 1 and len(Part[1]) > 0:
                Name = Part[0] + '_' + Part[1][1:]
            else:
                Name = Part[0]
            Value = None
            Item = self.GetConfigDataItemFromName(Name, False)
            if Item:
                try:
                    Value = '0x%x' % Bytes2Val(
                        self.CfgDataObj.ValueToByteArray(Item['value'], Item[
                            'length']))
                except:
                    Value = None
            ReplaceList.append(('$' + ''.join(Part), Value))

        OrgCond = Cond
        Result = True
        ReplaceList.sort(key=lambda x: len(x[0]), reverse=True)
        for Each in ReplaceList:
            if Each[1] is None:
                break
            Cond = Cond.replace(Each[0], Each[1])

        try:
            Result = True if eval(Cond) else False
        except:
            print("WARNING: Condition '%s' is invalid!" % OrgCond)

        return Result

    def AddConfigItem(self, Item, Row):
        Parent = self.RightGrid

        Name = Label(Parent, text=Item['name'], anchor="w")

        Parts = Item['type'].split(',')
        Type = Parts[0]
        Widget = None

        if Type == "Combo":
            # Build
            if Item['option'] in self.CfgDataObj._BuidinOption:
                OptList = [('0', 'Disable'), ('1', 'Enable')]
            else:
                OptList = self.CfgDataObj.GetItemOptionList(Item)

            CurrentValue = self.CfgDataObj.ValueToByteArray(Item['value'],
                                                            Item['length'])
            OptionList = []
            Current = None
            for Idx, Option in enumerate(OptList):
                OptionValue = self.CfgDataObj.ValueToByteArray(Option[0],
                                                               Item['length'])
                if OptionValue == CurrentValue:
                    Current = Idx
                OptionList.append(Option[1])

            Widget = ttk.Combobox(Parent, value=OptionList, state="readonly")
            Widget.bind("<<ComboboxSelected>>", self.ComboSelectChanged)

            if Current is None:
                print('WARNING: Value "%s" is an invalid option for "%s" !' %
                      (Bytes2Val(CurrentValue), Item['cname']))
            else:
                Widget.current(Current)

        elif Type in ["EditNum", "EditText"]:
            TxtVal = StringVar()
            Widget = Entry(Parent, textvariable=TxtVal)
            Value = Item['value'].strip("'")
            if Type in ["EditText"]:
                TxtVal.trace(
                    'w',
                    lambda *args: self.LimitEntrySize(TxtVal, Item['length']))
            elif Type in ["EditNum"]:
                Value = Item['value'].strip("{").strip("}").strip()
                Widget.bind("<FocusOut>", self.EditNumFinished)
            TxtVal.set(Value)

        elif Type in ["Table"]:
            Bins = self.CfgDataObj.ValueToByteArray(Item['value'],
                                                    Item['length'])
            ColHdr = Item['option'].split(',')
            Widget = CustomTable(Parent, ColHdr, Bins)
            #Widget.setEditTriggers(QAbstractItemView.DoubleClicked)

        if Widget:
            Ttp = CreateToolTip(Widget, Item['help'])
            self.SetObjectName(Name, 'LABEL_' + Item['cname'])
            self.SetObjectName(Widget, Item['cname'])
            Name.grid(row=Row, column=0, padx=10, pady=5, sticky="nsew")
            Widget.grid(row=Row + 1, column=0, padx=10, pady=5, sticky="nsew")

    def UpdateConfigDataOnPage(self):
        self.WalkWidgetsInLayout(self.RightGrid,
                                 self.UpdateConfigDataFromWidget)
        self.SyncConfigDataFromSubRegion()

    def OnConfigPageSelectChange(self, Event):
        self.UpdateConfigDataOnPage()
        Sel = self.Left.selection()
        if len(Sel) > 0:
            PageId = Sel[0]
            self.BuildConfigDataPage(PageId)
            self.UpdateWidgetsVisibilityOnPage()
            self.UpdatePageScrollBar()

if __name__ == '__main__':
    Root = Tk()
    App = Application(master=Root)
    Root.title("Config Editor")
    Root.mainloop()
