## @ ConfigEditor.py
#
# Copyright (c) 2018 - 2019, Intel Corporation. All rights reserved.<BR>
# SPDX-License-Identifier: BSD-2-Clause-Patent
#
##

import os
import re
import sys
import marshal

sys.dont_write_bytecode = True
if sys.hexversion >= 0x3000000:
    # for Python3
    from tkinter import *   ## notice lowercase 't' in tkinter here
    import tkinter.ttk as ttk
    import tkinter.messagebox as messagebox
    import tkinter.filedialog as filedialog
else:
    # for Python2
    from Tkinter import *   ## notice capitalized T in Tkinter
    import ttk
    import tkMessageBox as messagebox
    import tkFileDialog as filedialog

from GenCfgData import CGenCfgData, Bytes2Str, Bytes2Val, Val2Bytes, Array2Val

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
        if self.Widget.winfo_class() == 'Treeview':
            # Only show help when cursor is on row header.
            rowid  = self.Widget.identify_row(event.y)
            if rowid != '':
                return
        else:
            x, y, cx, cy = self.Widget.bbox("insert")

        Cursor = self.Widget.winfo_pointerxy()
        x = self.Widget.winfo_rootx() + 35
        y = self.Widget.winfo_rooty() + 20
        if Cursor[1] > y and Cursor[1] < y + 20:
            y += 20

        # creates a toplevel window
        self.TopWin = Toplevel(self.Widget)
        # Leaves only the label and removes the app window
        self.TopWin.wm_overrideredirect(True)
        self.TopWin.wm_geometry("+%d+%d" % (x, y))
        label = Message(self.TopWin,
                      text=self.Text,
                      justify='left',
                      background='bisque',
                      relief='solid',
                      borderwidth=1,
                      font=("times", "10", "normal"))
        label.pack(ipadx=1)
        self.InProgress = True

    def Leave(self, event=None):
        if self.TopWin:
            self.TopWin.destroy()
            self.InProgress = False


class ValidatingEntry(Entry):
    def __init__(self, master, **kw):
        Entry.__init__(*(self, master), **kw)
        self.Parent    = master
        self.OldValue  = ''
        self.LastValue = ''
        self.Variable  = StringVar()
        self.Variable.trace("w", self.Callback)
        self.config(textvariable=self.Variable)
        self.config({"background": "#c0c0c0"})
        self.bind("<Return>", self.MoveNext)
        self.bind("<Tab>", self.MoveNext)
        self.bind("<Escape>", self.Cancel)
        for each in ['BackSpace', 'Delete']:
            self.bind("<%s>" % each, self.Ignore)
        self.Display (None)

    def Ignore (self, even):
        return "break"

    def MoveNext (self, event):
        if self.Row < 0:
            return
        Row, Col = self.Row, self.Col
        Txt, RowId, ColId = self.Parent.GetNextCell (Row, Col)
        self.Display (Txt, RowId, ColId)
        return "break"

    def Cancel (self, event):
        self.Variable.set(self.OldValue)
        self.Display (None)

    def Display (self, Txt, RowId = '', ColId = ''):
        if Txt is None:
            self.Row = -1
            self.Col = -1
            self.place_forget()
        else:
            Row      = int('0x' + RowId[1:], 0) - 1
            Col      = int(ColId[1:]) - 1
            self.Row = Row
            self.Col = Col
            self.OldValue  = Txt
            self.LastValue = Txt
            x, y, width, height = self.Parent.bbox(RowId, Col)
            self.place(x=x, y=y, w=width)
            self.Variable.set(Txt)
            self.focus_set()
            self.icursor(0)

    def Callback(self, *Args):
        CurVal = self.Variable.get()
        NewVal = self.Validate(CurVal)
        if NewVal is not None and self.Row >= 0:
            self.LastValue = NewVal
            self.Parent.SetCell (self.Row , self.Col, NewVal)
        self.Variable.set(self.LastValue)


    def Validate(self, Value):
        if len(Value) > 0:
            try:
                int(Value, 16)
            except:
                return None

        # Normalize the cell format
        self.update()
        CellWidth = self.winfo_width ()
        MaxLen = CustomTable.ToByteLength(CellWidth) * 2
        CurPos = self.index("insert")
        if CurPos == MaxLen + 1:
            Value = Value[-MaxLen:]
        else:
            Value = Value[:MaxLen]
        if Value == '':
            Value = '0'
        Fmt =  '%%0%dX' % MaxLen
        return Fmt % int(Value, 16)


class CustomTable(ttk.Treeview):
    _Padding   = 20
    _CharWidth = 6

    def __init__(self, Parent, ColHdr, Bins):
        Cols = len(ColHdr)

        ColByteLen = []
        for Col in range(Cols):  #Columns
            ColByteLen.append(int(ColHdr[Col].split(':')[1]))

        ByteLen = sum(ColByteLen)
        Rows = (len(Bins) + ByteLen - 1) // ByteLen

        self.Rows = Rows
        self.Cols = Cols
        self.ColByteLen = ColByteLen
        self.ColHdr = ColHdr

        self.Size = len(Bins)
        self.LastDir = ''

        style = ttk.Style()
        style.configure("Custom.Treeview.Heading", font=('Calibri', 10, 'bold'),  foreground="blue")
        ttk.Treeview.__init__(self, Parent, height=Rows, columns=[''] + ColHdr, show='headings', style="Custom.Treeview", selectmode='none')
        self.bind("<Button-1>", self.Click)
        self.bind("<FocusOut>", self.FocusOut)
        self.entry = ValidatingEntry(self, width=4,  justify=CENTER)

        self.heading(0, text='LOAD')
        self.column (0, width=60, stretch=0, anchor=CENTER)

        for Col in range(Cols):  #Columns
            Text = ColHdr[Col].split(':')[0]
            ByteLen  = int(ColHdr[Col].split(':')[1])
            self.heading(Col+1, text=Text)
            self.column(Col+1, width=self.ToCellWidth(ByteLen), stretch=0, anchor=CENTER)

        Idx = 0
        for Row in range(Rows):  #Rows
            Text = '%04X' % (Row * len(ColHdr))
            Vals = ['%04X:' % (Cols * Row)]
            for Col in range(Cols):  #Columns
                if Idx >= len(Bins):
                    break
                ByteLen  = int(ColHdr[Col].split(':')[1])
                Value = Bytes2Val (Bins[Idx:Idx+ByteLen])
                Hex = ("%%0%dX" % (ByteLen * 2) ) % Value
                Vals.append (Hex)
                Idx += ByteLen
            self.insert('', 'end', values=tuple(Vals))
            if Idx >= len(Bins):
                break

    @staticmethod
    def ToCellWidth(ByteLen):
        return ByteLen * 2 * CustomTable._CharWidth + CustomTable._Padding

    @staticmethod
    def ToByteLength(CellWidth):
        return (CellWidth - CustomTable._Padding) // (2 * CustomTable._CharWidth)

    def FocusOut (self, event):
        self.entry.Display (None)

    def RefreshBin (self, Bins):
        if not Bins:
            return

        # Reload binary into widget
        BinLen = len(Bins)
        for Row in range(self.Rows):
            Iid  = self.get_children()[Row]
            for Col in range(self.Cols):
                Idx = Row * sum(self.ColByteLen) + sum(self.ColByteLen[:Col])
                ByteLen = self.ColByteLen[Col]
                if Idx + ByteLen <= self.Size:
                    ByteLen  = int(self.ColHdr[Col].split(':')[1])
                    if Idx + ByteLen > BinLen:
                      Val = 0
                    else:
                      Val = Bytes2Val (Bins[Idx:Idx+ByteLen])
                    HexVal = ("%%0%dX" % (ByteLen * 2) ) % Val
                    self.set (Iid, Col + 1, HexVal)

    def GetCell (self, Row, Col):
        Iid  = self.get_children()[Row]
        Txt  = self.item(Iid, 'values')[Col]
        return Txt

    def GetNextCell (self, Row, Col):
        Rows  = self.get_children()
        Col  += 1
        if Col > self.Cols:
          Col = 1
          Row +=1
        Cnt = Row * sum(self.ColByteLen) + sum(self.ColByteLen[:Col])
        if Cnt > self.Size:
          # Reached the last cell, so roll back to beginning
          Row  = 0
          Col  = 1

        Txt   = self.GetCell(Row, Col)
        RowId = Rows[Row]
        ColId = '#%d' % (Col + 1)
        return (Txt, RowId, ColId)

    def SetCell (self, Row, Col, Val):
        Iid  = self.get_children()[Row]
        self.set (Iid, Col, Val)

    def LoadBin (self):
        # Load binary from file
        Path = filedialog.askopenfilename(
            initialdir=self.LastDir,
            title="Load binary file",
            filetypes=(("Binary files", "*.bin"), (
                "binary files", "*.bin")))
        if Path:
            self.LastDir = os.path.dirname(Path)
            Fd = open(Path, 'rb')
            Bins = bytearray(Fd.read())[:self.Size]
            Fd.close()
            Bins.extend (b'\x00' * (self.Size - len(Bins)))
            return Bins

        return None

    def Click (self, event):
        RowId  = self.identify_row(event.y)
        ColId  = self.identify_column(event.x)
        if RowId == '' and ColId == '#1':
            # Clicked on "LOAD" cell
            Bins = self.LoadBin ()
            self.RefreshBin (Bins)
            return

        if ColId == '#1':
            # Clicked on column 1 (Offset column)
            return

        Item = self.identify('item', event.x, event.y)
        if not Item or not ColId:
            # Not clicked on valid cell
            return

        # Clicked cell
        Row    = int('0x' + RowId[1:], 0) - 1
        Col    = int(ColId[1:]) - 1
        if Row * self.Cols + Col > self.Size:
            return

        Vals = self.item(Item, 'values')
        if Col < len(Vals):
            Txt = self.item(Item, 'values')[Col]
            self.entry.Display (Txt, RowId, ColId)

    def get(self):
        Bins = bytearray()
        RowIds = self.get_children()
        for RowId in RowIds:
            Row = int('0x' + RowId[1:], 0) - 1
            for Col in range(self.Cols):
              Idx = Row * sum(self.ColByteLen) + sum(self.ColByteLen[:Col])
              ByteLen = self.ColByteLen[Col]
              if Idx + ByteLen > self.Size:
                  break
              Hex = self.item(RowId, 'values')[Col + 1]
              Values = Val2Bytes (int(Hex, 16) & ((1 << ByteLen * 8) - 1), ByteLen)
              Bins.extend(Values)
        return Bins

class State:
    def __init__(self):
        self.state = False

    def set(self, value):
        self.state = value

    def get(self):
        return self.state

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
        self.InLeft  = State()
        self.InRight = State()

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
        self.Left.bind('<Enter>',  lambda e: self.InLeft.set(True))
        self.Left.bind('<Leave>',  lambda e: self.InLeft.set(False))
        self.Left.bind('<MouseWheel>',  self.OnTreeScroll)

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
        self.ConfCanvas.bind('<Enter>',  lambda e: self.InRight.set(True))
        self.ConfCanvas.bind('<Leave>',  lambda e: self.InRight.set(False))
        self.ConfCanvas.bind("<Configure>", self.OnCanvasConfigure)
        self.ConfCanvas.bind_all("<MouseWheel>", self.OnPageScroll)

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

        if len(sys.argv) > 1:
            Path = sys.argv[1]
            if not Path.endswith('.dsc'):
                DscPath = os.path.join (os.path.dirname(Path), 'CfgDataDef.dsc')
            else:
                DscPath = Path
            if self.LoadDscFile (DscPath) == 0:
                if Path.endswith('.dsc') and len(sys.argv) > 2:
                    Path = sys.argv[2]
                if not (Path.endswith('.dlt') or Path.endswith('.bin') or Path.endswith('.dsc')):
                    messagebox.showerror('LOADING ERROR', "Unsupported file '%s' !" % Path)
                    return
                if Path.endswith('.dlt'):
                    self.LoadDeltaFile (Path)
                elif Path.endswith('.bin'):
                    self.LoadBinFile (Path)


    def SetObjectName(self, Widget, Name):
        self.ConfList[id(Widget)] = Name

    def GetObjectName(self, Widget):
        if id(Widget) in self.ConfList:
            return self.ConfList[id(Widget)]
        else:
            return None

    def LimitEntrySize(self, Variable, Limit):
        Value = Variable.get()
        if len(Value) > Limit:
            Variable.set(Value[:Limit])

    def OnCanvasConfigure(self, Event):
        self.RightGrid.grid_columnconfigure(0, minsize=Event.width)

    def OnTreeScroll(self, Event):
        if not self.InLeft.get() and self.InRight.get():
            # This prevents scroll event from being handled by both left and
            # right frame at the same time.
            self.OnPageScroll (Event)
            return 'break'

    def OnPageScroll(self, Event):
        if self.InRight.get():
            # Only scroll when it is in active area
            min, max = self.PageScroll.get()
            if not ((min == 0.0) and (max == 1.0)):
                self.ConfCanvas.yview_scroll(-1 * int(Event.delta / 120), 'units')

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
        Sel = self.Left.selection()
        if len(Sel) > 0:
            PageId = Sel[0]
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
        messagebox.showinfo('Config Editor', '\n'.join(Text))

    def UpdateLastDir (self, Path):
        self.LastDir = os.path.dirname(Path)

    def GetOpenFileName(self, Type):
        if self.IsConfigDataLoaded():
            if Type == 'dlt':
                Question = ''
            elif Type == 'bin':
                Question = 'All configuration will be reloaded from BIN file, continue ?'
            elif Type == 'dsc':
                Question = ''
            else:
                raise Exception('Unsupported file type !')
            if Question:
                Reply = messagebox.askquestion('', Question, icon='warning')
                if Reply == 'no':
                    return None

        if Type == 'dsc':
            FileType = 'DSC or PKL'
            FileExt  = 'pkl *Def.dsc'
        else:
            FileType = Type.upper()
            FileExt  = Type

        Path = filedialog.askopenfilename(
            initialdir=self.LastDir,
            title="Load file",
            filetypes=(("%s files" % FileType, "*.%s" % FileExt), (
                "all files", "*.*")))
        if Path:
            self.UpdateLastDir (Path)
            return Path
        else:
            return None


    def LoadFromDelta(self):
        Path = self.GetOpenFileName('dlt')
        if not Path:
            return
        self.LoadDeltaFile (Path)

    def LoadDeltaFile (self, Path):
        self.ReloadConfigDataFromBin(self.OrgCfgDataBin)

        try:
            self.CfgDataObj.OverrideDefaultValue(Path)
            self.CfgDataObj.UpdateDefaultValue()
        except Exception as e:
            messagebox.showerror('LOADING ERROR', str(e))
            return

        self.UpdateLastDir (Path)
        self.RefreshConfigDataPage()

    def LoadFromBin(self):
        Path = self.GetOpenFileName('bin')
        if not Path:
            return
        self.LoadBinFile (Path)

    def LoadBinFile (self, Path):
        with open(Path, 'rb') as Fd:
            BinData = bytearray(Fd.read())
        if len(BinData) < len(self.OrgCfgDataBin):
            messagebox.showerror('Binary file size is smaller than what DSC requires !')
            return

        try:
            self.ReloadConfigDataFromBin(BinData)
        except Exception as e:
            messagebox.showerror('LOADING ERROR', str(e))
            return

    def LoadDscFile(self, Path):
        # Save current values in widget and clear  database
        self.ClearWidgetsInLayout()
        self.Left.delete(*self.Left.get_children())

        try:
            self.CfgDataObj = self.LoadConfigData(Path)
        except Exception as e:
            messagebox.showerror('LOADING ERROR', str(e))
            return -1

        self.UpdateLastDir (Path)
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
        Path = filedialog.asksaveasfilename(
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
        NewData = self.CfgDataObj.GenerateBinaryArray()
        self.CfgDataObj.GenerateDeltaFileFromBin (Path, self.OrgCfgDataBin, NewData, Full)

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

        if Widget:
            Ttp = CreateToolTip(Widget, Item['help'])
            self.SetObjectName(Name, 'LABEL_' + Item['cname'])
            self.SetObjectName(Widget, Item['cname'])
            Name.grid(row=Row, column=0, padx=10, pady=5, sticky="nsew")
            Widget.grid(row=Row + 1, rowspan=1, column=0, padx=10, pady=5, sticky="nsew")

    def UpdateConfigDataOnPage(self):
        self.WalkWidgetsInLayout(self.RightGrid,
                                 self.UpdateConfigDataFromWidget)
        self.SyncConfigDataFromSubRegion()


if __name__ == '__main__':
    Root = Tk()
    App = Application(master=Root)
    Root.title("Config Editor")
    Root.mainloop()
