## @ SblBuilderGuiComp.py
#
# Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
# SPDX-License-Identifier: BSD-2-Clause-Patent
#
##

import tkinter as tk
from tkinter import ttk, filedialog, font, StringVar, messagebox
import os
import re
import shutil
import subprocess
import sys
import threading

def get_python_cmd():
    """Return 'python' if available and functional, else 'python3'."""
    for cmd in ("python", "python3"):
        if shutil.which(cmd):
            try:
                result = subprocess.run([cmd, "--version"], stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
                if result.returncode == 0:
                    return cmd
            except Exception:
                pass
    return "python"

FONT_PAGE_TITLE=("Verdana", 18)
FONT_BUTTON=("Arial", 11)
FONT_CONTENT=("Arial", 11)
FONT_RADIO_BUTTON=("Arial", 10)
FONT_ICON=("Times New Roman", 15, font.BOLD)
CFG_FONT_CONTENT=("Arial", 10)

TEXT_WRAPPER_LENGTH=670

COMMON_BG="#ecf0f1"
BUTTON_FG="#262727"
BUTTON_BG="#9EA8AA"

# Unicode icon list:
INFO_SIGN="\U000024D8"
WARNING_SIGN="\U000026A0"

BUTTON_WIDTH=8
BUTTON_WIDTH_SMALL=5
BUTTON_STYLE="Rounded.TButton"

# TODO: Need to figure out on how to integrate command line execution, status bar and progress bar into one, so that only one command
# can be executed at a time in each tab. (aka singleton design pattern for command execution that utilizes the status bar and progress
# bar per tab)
class ExecutionManager:
    def __init__(self, parent, terminal=None):
        self.root = parent
        self.status_bar = StatusBar(parent)
        # Disabling progress bar for now
        # self.progress_bar = ProgressBar(parent, self.status_bar)
        self.status_indicator = StatusIndicator(parent)
        self.is_busy = False
        self.current_thread = None
        self.terminal = terminal

    def execute_command(self, command, cwd=None, shell=False, queue=False):
        if self.is_busy:
            if queue:
                self.current_thread.add_command(command, cwd, shell)
            else:
                messagebox.showwarning("Warning", "Another command is currently running. Please wait...")
            return
        # Start command thread execution and progress bar animation
        self.current_thread = CommandThread(command, status_manager=self, cwd=cwd, shell=shell)
        self.current_thread.start()

    def terminate_process(self, app_exit=False):
        if not app_exit:
            if not messagebox.askokcancel("Terminate Build", "Are you sure you want to terminate the ongoing build?"):
                return
        if self.is_busy and self.current_thread:
            if app_exit:
                self.current_thread.app_exit = True
            self.current_thread.kill()
            self.current_thread = None
            self.is_busy = False
            if not app_exit:
                self.status_indicator.stop()
                self.status_bar.set("Build process terminated by user.", color="red")

class CommandThread(threading.Thread):
    # NOTE: command is expected to be a string command. It will be split into list of string before passing into subprocess.Popen
    def __init__(self, command:str, status_manager, cwd=None, shell=False):
        super().__init__()
        self.command_queue = []
        self.command_queue.append((command, cwd, shell))
        self.status_manager = status_manager
        self.process = None
        self.app_exit = False

    def run(self):
        return_code = 0
        command = ""
        try:
            while len(self.command_queue) > 0:
                command, cwd, shell = self.command_queue.pop(0)
                self.status_manager.is_busy = True
                # self.status_manager.progress_bar.show_progress()
                self.status_manager.status_indicator.start()
                if self.status_manager.terminal:
                    self.status_manager.terminal.log(f"Executing \"{command}\" in {cwd}")
                    self.status_manager.terminal.log("----------------------------------")

                cmd = command if shell else command.split()
                self.process = subprocess.Popen(cmd, cwd=cwd, shell=shell,
                                                stdout=subprocess.PIPE, stderr=subprocess.STDOUT)

                # Read the output line by line and log it to the terminal widget in real-time
                if self.status_manager.terminal:
                    line = self.process.stdout.readline()
                    while line:
                        self.status_manager.terminal.log(line.decode().rstrip("\r\n"))
                        line = self.process.stdout.readline()
                return_code = self.process.wait()
                if self.status_manager.terminal:
                    self.status_manager.terminal.log("----------------------------------\n")
                if return_code != 0:
                    break
        except Exception as e:
            if self.status_manager.terminal and not self.app_exit:
                self.status_manager.terminal.log(f"Error executing command: {e}\n")
                self.status_manager.terminal.log("\nProcess terminated!!\n")
            else:
                print(f"\nProcess terminated!!\nError: {str(e)}")
            if not self.app_exit:
                self.status_manager.status_bar.set("Error executing command. See terminal log for details.", color="red")
        else:
            if self.process and return_code:
                if return_code < 0:
                    signal_num = -return_code
                    if self.status_manager.terminal:
                        self.status_manager.terminal.log(f"Error: \"{command}\" terminated by signal {signal_num} (possible segmentation fault).\n")
                    self.status_manager.status_bar.set(
                        f"Command terminated by signal {signal_num} (possible segmentation fault). See terminal log for details.",
                        color="red")
                else:
                    if self.status_manager.terminal:
                        self.status_manager.terminal.log(f"Error: \"{command}\" failed with return code {return_code}\n")
                    self.status_manager.status_bar.set(f"Command failed with return code {return_code}. See terminal log for details.", color="red")
            else:
                self.status_manager.status_bar.set("Ready", color="green")
        finally:
            # self.status_manager.progress_bar.hide_progress()
            if not self.app_exit:
                self.status_manager.status_indicator.stop()
            self.status_manager.is_busy = False
            self.kill()

    def kill(self):
        if self.process:
            # Force kill the process and avoid terminate prompts on certain build commands
            self.process.kill()
            self.process.wait()
        self.process = None

    def add_command(self, command, cwd=None, shell=False):
        self.command_queue.append((command, cwd, shell))

# Future implementation:
# - Add Save button to save the terminal output to a text file
class TerminalWindow(tk.Toplevel):
    def __init__(self, parent, title="Terminal Output"):
        super().__init__(parent)
        self.withdraw()  # Hidden by default
        self.title(title)
        self.geometry("800x600")

        frame = tk.Frame(self)
        frame.pack(fill="both", expand=True, padx=10, pady=10)

        # Create scrollbar
        scrollbar = tk.Scrollbar(frame)
        scrollbar.pack(side="right", fill="y")

        # Create text widget
        self.text_widget = tk.Text(frame, wrap="word", bg="black", fg="white", yscrollcommand=scrollbar.set, font=("Courier", 9))
        self.text_widget.pack(fill="both", expand=True)
        scrollbar.config(command=self.text_widget.yview)

        # Highlight tags for quick log scanning
        self.text_widget.tag_configure("kw_error", foreground="#ff6b6b")    # red-ish
        self.text_widget.tag_configure("kw_warning", foreground="#ffd166")  # yellow
        self.text_widget.tag_configure("kw_success", foreground="#80ed99")  # green
        self.text_widget.tag_configure("kw_info", foreground="#4cc9f0")     # cyan
        self.text_widget.tag_configure("kw_cmd", foreground="#f72585")      # magenta

        self._keyword_patterns = {
            "kw_error": re.compile(r"\b(error|failed|failure|exception|traceback|terminated)\b", re.IGNORECASE),
            "kw_warning": re.compile(r"\b(warning|warn|deprecated)\b", re.IGNORECASE),
            "kw_success": re.compile(r"\b(success|succeeded|complete|completed|done|pass|passed|ok)\b", re.IGNORECASE),
            "kw_info": re.compile(r"\b(info|starting|finished|building|running)\b", re.IGNORECASE),
            "kw_cmd": re.compile(r"\b(executing)\b", re.IGNORECASE),
        }

        button_frame = tk.Frame(self)
        button_frame.pack(anchor="center", pady=5)
        Button(button_frame, text="Clear", command=self.clear).pack(side="left", padx=5)

        # Don't destroy the terminal window on close, just hide it so that any incoming log can still be displayed if the user closed the terminal window
        self.protocol("WM_DELETE_WINDOW", self.hide)
        Button(button_frame, text="Close", command=self.hide).pack(side="left", padx=5)

    def _highlight_keywords(self, message, start_idx):
        for tag_name, pattern in self._keyword_patterns.items():
            for match in pattern.finditer(message):
                tag_start = f"{start_idx}+{match.start()}c"
                tag_end = f"{start_idx}+{match.end()}c"
                self.text_widget.tag_add(tag_name, tag_start, tag_end)

    def log(self, message, updateLabel=False, sep="\n"):
        start_idx = self.text_widget.index("end-1c")
        self.text_widget.insert("end", message + sep)
        self._highlight_keywords(message, start_idx)
        self.text_widget.see("end")

    def clear(self):
        self.text_widget.delete('1.0', tk.END)

    def show(self):
        self.deiconify()

    def hide(self):
        self.withdraw()

class StatusBar(tk.Label):
    def __init__(self, parent, initial_text="Ready"):
        self.textvariable = StringVar(value=initial_text)
        super().__init__(parent, textvariable=self.textvariable, font=("Arial", 9, font.BOLD), bd=1, relief="sunken", anchor="w")
        self.config(fg="green")
        self.pack(side="bottom", fill="x")

    def set(self, text, color="green"):
        self.textvariable.set(text)
        self.config(fg=color)

class ProgressBar(ttk.Progressbar):
    def __init__(self, parent, status_bar_widget):
        super().__init__(parent, mode="indeterminate", length=200)
        self.root = parent
        style = ttk.Style()
        style.configure("blue.Horizontal.TProgressbar",
                        troughcolor=status_bar_widget.cget('bg'),
                        background='blue',
                        thickness=20)
        self.config(style="blue.Horizontal.TProgressbar", mode='determinate')
        self.before_widget = status_bar_widget
        self.is_closing = False

    def show_progress(self):
        def animate_progress():
            if self.is_closing:
                return
            if self['value'] < 100:
                self['value'] += 1
                self.root.after(100, animate_progress)  # Update every 50ms
            else:
                self['value'] = 0  # Reset and loop
                self.root.after(100, animate_progress)

        self['value'] = 0
        animate_progress()
        self.pack(side="bottom", fill="x", before=self.before_widget)
        self.start(30)

    def hide_progress(self):
        self.is_closing = True
        self.stop()
        self.pack_forget()

class StatusIndicator:
    def __init__(self, parent):
        self.root = parent
        self._indicator = tk.Label(self.root, text="● Idle", font=("Arial", 10, "bold"),
                                   fg="gray", bg="white")
        self._indicator_visible = False
        self._indicator_blink_job = None
        self._place_indicator()
        self.root.bind("<Configure>", self._on_root_configure, add="+")

    def _place_indicator(self):
        self._indicator.place(relx=1.0, rely=0.0, anchor="ne", x=-10, y=30)
        self._indicator.lift()

    def _on_root_configure(self, event=None):
        self._place_indicator()

    def start(self):
        """Start the activity indicator with blinking animation."""
        self._indicator_visible = True
        self._indicator.config(text="● Running", fg="red")
        self._place_indicator()
        self._blink_indicator()

    def stop(self):
        """Stop the activity indicator."""
        self._indicator_visible = False
        if self._indicator_blink_job:
            self.root.after_cancel(self._indicator_blink_job)
            self._indicator_blink_job = None
        self._indicator.config(text="● Idle", fg="gray")
        self._place_indicator()

    def _blink_indicator(self):
        if not self._indicator_visible:
            return
        current = self._indicator.cget("fg")
        next_color = "white" if current == "red" else "red"
        self._indicator.config(fg=next_color)
        self._indicator_blink_job = self.root.after(500, self._blink_indicator)

class TabPage(tk.Frame):
    def __init__(self, parent, tabName = "", tabTitle = ""):
        super().__init__(parent, bg="white")
        parent.add(self, text=tabName)
        tk.Label(self, text=tabTitle, font=FONT_PAGE_TITLE, bg="white").pack(pady=20)

    def update(self):
        pass

    # Allows main app to send signal to all tabs when the app is closing, so that they can perform necessary cleanup to prevent errors on exit
    def on_closing(self):
        pass

class Tooltip(object):
    '''
    create a tooltip for a given widget
    '''

    def __init__(self, widget, text=''):
        self.top_win = None
        self.widget = widget
        self.text = text
        self.widget.bind("<Enter>", self.enter)
        self.widget.bind("<Leave>", self.leave)

    def enter(self, event=None):
        tool_tip_width = 500

        x = self.widget.winfo_rootx() + 25
        y = self.widget.winfo_rooty() + self.widget.winfo_height()

        # creates a toplevel window
        self.top_win = tk.Toplevel(self.widget)

        # Leaves only the label and removes the app window
        self.top_win.wm_overrideredirect(True)
        self.top_win.wm_geometry(f"+{x}+{y}")
        label = tk.Message(self.top_win,
                      text=self.text,
                      width=tool_tip_width,
                      justify='left',
                      background='bisque',
                      relief='solid',
                      borderwidth=1,
                      font=("times", "10", "normal"))
        label.pack(ipadx=1)

    def leave(self, event=None):
        if self.top_win:
            self.top_win.destroy()

    def update_text(self, new_text):
        self.text = new_text

class InfoLabel(tk.Label):
    def __init__(self, parent, info_text):
        super().__init__(parent, text=INFO_SIGN, font=FONT_ICON, fg="blue", bg=COMMON_BG)
        self.pack(side="left", padx=(5,0), pady=5)
        Tooltip(self, info_text)

class CommonFrame(tk.Frame):
    def __init__(self, parent):
        super().__init__(parent, bg=COMMON_BG)
        self.pack(pady=5, fill="x", padx=50)

class LabelFrame(CommonFrame):
    def __init__(self, parent, info_text = "", label_text="", bind_variable=None, has_warning=False):
        super().__init__(parent)

        if info_text:
            InfoLabel(self, info_text)
        if label_text:
            title_label = tk.Label(self, text=label_text, font=FONT_CONTENT, fg="black", bg=COMMON_BG, wraplength=150)
            title_label.pack(side="left", padx=10, pady=5)

        if has_warning:
            self.warning_icon = tk.StringVar()
            self.warning_label = tk.Label(self, textvariable=self.warning_icon, font=FONT_ICON, fg="orange", bg=COMMON_BG)
            self.warning_label.pack(side="left", padx=0, pady=5)
            self.warning_text = Tooltip(self.warning_label, "")

        if bind_variable:
            self.bind_variable = bind_variable
            self.value_label = tk.Label(self, textvariable=self.bind_variable, font=FONT_CONTENT, fg="blue", bg=COMMON_BG, wraplength=TEXT_WRAPPER_LENGTH)
            self.value_label.pack(side="left", padx=10, pady=5)

    # Dynamically add buttons from right side of the label frame
    def add_button(self, text, command):
        button = Button(self, text=text, command=command)
        button.pack(side="right", padx=10, pady=5)
        return button

    def add_listbox(self, options:list, command=None):
        dropdown = ttk.Combobox(self, values=options, font=FONT_CONTENT, state="readonly", background=COMMON_BG)
        dropdown.pack(side="right", padx=10, pady=5)
        if command:
            dropdown.bind("<<ComboboxSelected>>", command)
        dropdown.set(options[0])
        return dropdown

    def set_warning(self, warning_text):
        if hasattr(self, "warning_icon") and hasattr(self, "warning_text"):
            if warning_text:
                self.warning_icon.set(WARNING_SIGN)
                self.warning_label.config(padx=5)
                self.warning_text.update_text(warning_text)

    def clear_warning(self):
        if hasattr(self, "warning_icon") and hasattr(self, "warning_text"):
            self.warning_icon.set("")
            self.warning_label.config(padx=0)
            self.warning_text.update_text("")

class InfoFrame(CommonFrame):
    def __init__(self, parent, label_text="", bind_variable=None):
        super().__init__(parent)
        self.pack(fill="x", padx=50, pady=5)
        if label_text:
            title_label = tk.Label(self, text=label_text, font=FONT_CONTENT, fg="black", bg=COMMON_BG, justify="left")
            title_label.pack(anchor="w", padx=10, pady=(10, 5))

        if bind_variable:
            self.bind_variable = bind_variable
            self.value_label = tk.Label(self, textvariable=self.bind_variable, font=("Courier", 11), fg="blue", bg=COMMON_BG, justify="left")
            self.value_label.pack(anchor="w", padx=10, pady=(0, 10))

class FileDirectoryEntry(LabelFrame):
    def __init__(self, parent, label_text, bind_variable, browse_type, filetypes=None, callback=None, browse_title_text="", info_text=""):
        super().__init__(parent, label_text=label_text, bind_variable=bind_variable, info_text=info_text)

        self.browse_type = browse_type
        self.callback = callback

        self.browseButton = super().add_button("Browse", command=self.browse)

        self.browse_title_text = "Unknown"
        if self.browse_type == "file":
            self.browse_title_text = "Select file" if browse_title_text == "" else browse_title_text
            self.filetypes = filetypes if filetypes else [("All Files", "*.*")]
        elif self.browse_type == "folder":
            self.browse_title_text = "Select folder" if browse_title_text == "" else browse_title_text

    def browse(self):
        print(f"Browse button clicked for {self.browse_type} with title: {self.browse_title_text}")
        old_value = self.bind_variable.get()
        selected_item = None
        if self.browse_type == "file":
            selected_item = filedialog.askopenfilename(title=self.browse_title_text, filetypes=self.filetypes)
        elif self.browse_type == "folder":
            selected_item = filedialog.askdirectory(title=self.browse_title_text)
        if not selected_item:
            return
        self.bind_variable.set(selected_item)
        # Do not run callback function if user selects the same path
        if self.callback and selected_item != old_value:
            self.callback(selected_item)
        print(self.bind_variable.get())

class Button(ttk.Button):
    def __init__(self, parent, text, command, width=BUTTON_WIDTH, **kwargs):
        style = kwargs.pop("style", BUTTON_STYLE)
        super().__init__(parent, text=text, command=command, style=style, width=width, **kwargs)

class RadioButtonGroup:
    def __init__(self, parent, label_text, options:list, default_value=-1, tooltip_texts:list=None, callback=None):
        # NOTE: This frame is for one group of radio buttons. Please create top-level frame as parent if you want to place multiple
        #       groups of radio buttons in a single frame.
        self.frame = tk.Frame(parent)
        self.frame.pack(side="left", fill="none", padx=(10,10), pady=5)
        tk.Label(self.frame, text=label_text, font=FONT_CONTENT, fg="black", bg=COMMON_BG).pack(anchor="w")

        self.selectedValue = tk.IntVar()
        self.selectedValue.set(default_value)
        self.options = options

        style = ttk.Style()
        style.configure("Custom.TRadiobutton", background=COMMON_BG, font=FONT_RADIO_BUTTON)

        for i, text in enumerate(self.options):
            radioButton = ttk.Radiobutton(
                self.frame, text=text, variable=self.selectedValue, value=i, style="Custom.TRadiobutton", command=self.trigger_callback)
            radioButton.pack(anchor="w", padx=10)
            if tooltip_texts and i < len(tooltip_texts):
                Tooltip(radioButton, tooltip_texts[i])

        self.callback = callback

    def trigger_callback(self):
        print(f"Selected option: {self.get_option()}")
        if self.callback:
            self.callback(self.options[self.selectedValue.get()])

    def get_option(self):
        if self.selectedValue.get() >= 0 and self.selectedValue.get() < len(self.options):
            return self.options[self.selectedValue.get()]

    def set_option(self, option_text):
        if option_text in self.options:
            self.selectedValue.set(self.options.index(option_text))

    def set_idx(self, idx):
        if idx >= -1 and idx < len(self.options):
            self.selectedValue.set(idx)

class CheckBoxGroup:
    pass