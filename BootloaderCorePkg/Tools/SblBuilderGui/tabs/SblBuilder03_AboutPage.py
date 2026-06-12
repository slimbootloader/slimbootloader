## @ SblBuilder03_AboutPage.py
#
# Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
# SPDX-License-Identifier: BSD-2-Clause-Patent
#
##

import tkinter as tk
import webbrowser
from SblBuilderGuiComp import *


class AboutPage(TabPage):
    def __init__(self, parent, global_variables, global_flags):
        super().__init__(parent, tabName="About", tabTitle="Slim Bootloader Builder GUI")
        self.global_variables = global_variables
        self.global_flags = global_flags

        about_text = (
            "Slim Bootloader Builder GUI helps you configure, build, and maintain "
            "Slim Bootloader workspaces.\n\n"
            "Main functions:\n"
            "- Setup workspace and source repository\n"
            "- Build selected board targets\n"
            "- Manage payload-related options\n\n"
        )

        self.about_message = tk.Message(
            self,
            text=about_text,
            width=900,
            font=FONT_CONTENT,
            fg="black",
            bg="white",
            justify="left"
        )
        self.about_message.pack(padx=50, pady=(5, 10), anchor="w")

        self.user_manual_url = "https://slimbootloader.github.io/tools/SblBuilderGui.html"
        self.manual_frame = tk.Frame(self, bg="white")
        self.manual_frame.pack(padx=50, pady=(0, 10), anchor="w")

        self.manual_label = tk.Label(
            self.manual_frame,
            text="User Manual: ",
            font=FONT_CONTENT,
            fg="black",
            bg="white",
            justify="left"
        )
        self.manual_label.pack(side="left")

        self.manual_link = tk.Label(
            self.manual_frame,
            text="User Manual",
            font=(FONT_CONTENT[0], FONT_CONTENT[1], "underline"),
            fg="blue",
            bg="white",
            cursor="hand2",
            justify="left"
        )
        self.manual_link.pack(side="left")
        self.manual_link.bind("<Button-1>", self.open_user_manual)

        self.note_frame = tk.Frame(self, bg="white")
        self.note_frame.pack(padx=50, pady=(0, 10), anchor="w")

        self.note_regular = tk.Label(
            self.note_frame,
            text="Note: SblBuilderGui may modify source files during operation, ",
            font=FONT_CONTENT,
            fg="black",
            bg="white",
            justify="left"
        )
        self.note_regular.pack(side="left")

        self.note_bold = tk.Label(
            self.note_frame,
            text="it is recommended to back up your source code before running this tool.",
            font=(FONT_CONTENT[0], FONT_CONTENT[1], "bold"),
            fg="black",
            bg="white",
            justify="left"
        )
        self.note_bold.pack(side="left")

        self.update()

    def update(self):
        pass

    def open_user_manual(self, event=None):
        if self.user_manual_url:
            webbrowser.open(self.user_manual_url)

    def on_closing(self):
        print("AboutPage: on_closing called.")
