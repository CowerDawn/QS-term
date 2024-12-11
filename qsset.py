import tkinter as tk
from tkinter import filedialog, messagebox, font
import configparser
import os
import subprocess

CONFIG_FILE = "qsset.conf"

def create_config():
    config = configparser.ConfigParser()
    config['Settings'] = {
        'font': 'Courier 10',
        'color': 'dark',
        'icon': 'icon.png',
        'title': 'QS-term',
        'default_terminal': 'no',
        'shortcut_ctrl_s': 'yes'
    }
    with open(CONFIG_FILE, 'w') as configfile:
        config.write(configfile)

def load_config():
    config = configparser.ConfigParser()
    if not os.path.exists(CONFIG_FILE):
        create_config()
    config.read(CONFIG_FILE)
    return config

def save_config(config):
    with open(CONFIG_FILE, 'w') as configfile:
        config.write(configfile)

def set_default_terminal():
    config = load_config()
    config['Settings']['default_terminal'] = 'yes'
    save_config(config)
    messagebox.showinfo("Info", "QS-term is now set as the default terminal.")

def open_qsset():
    subprocess.Popen(["python3", "qsset.py"])

class QSSetApp(tk.Tk):
    def __init__(self):
        super().__init__()
        self.title("QS-term Settings")
        self.geometry("400x300")

        self.config = load_config()

        self.create_widgets()

    def create_widgets(self):
        tk.Label(self, text="Font:").grid(row=0, column=0, sticky="w")
        self.font_var = tk.StringVar(value=self.config['Settings']['font'])
        tk.Entry(self, textvariable=self.font_var).grid(row=0, column=1)
        tk.Button(self, text="Select Font", command=self.select_font).grid(row=0, column=2)

        tk.Label(self, text="Color Theme:").grid(row=1, column=0, sticky="w")
        self.color_var = tk.StringVar(value=self.config['Settings']['color'])
        tk.OptionMenu(self, self.color_var, "dark", "gray", "dark2", "pink", "home", "light").grid(row=1, column=1)

        tk.Label(self, text="Icon:").grid(row=2, column=0, sticky="w")
        self.icon_var = tk.StringVar(value=self.config['Settings']['icon'])
        tk.Entry(self, textvariable=self.icon_var).grid(row=2, column=1)
        tk.Button(self, text="Select Icon", command=self.select_icon).grid(row=2, column=2)

        tk.Label(self, text="Window Title:").grid(row=3, column=0, sticky="w")
        self.title_var = tk.StringVar(value=self.config['Settings']['title'])
        tk.Entry(self, textvariable=self.title_var).grid(row=3, column=1)

        tk.Button(self, text="Set as Default Terminal", command=set_default_terminal).grid(row=4, column=0, columnspan=3)

        tk.Button(self, text="Save Settings", command=self.save_settings).grid(row=5, column=0, columnspan=3)

    def select_font(self):
        font_tuple = tk.font.Font(font=self.font_var.get()).actual()
        font_name = f"{font_tuple['family']} {font_tuple['size']}"
        self.font_var.set(font_name)

    def select_icon(self):
        file_path = filedialog.askopenfilename(filetypes=[("Image Files", "*.png;*.jpg;*.jpeg;*.ico")])
        if file_path:
            self.icon_var.set(file_path)

    def save_settings(self):
        self.config['Settings']['font'] = self.font_var.get()
        self.config['Settings']['color'] = self.color_var.get()
        self.config['Settings']['icon'] = self.icon_var.get()
        self.config['Settings']['title'] = self.title_var.get()
        save_config(self.config)
        messagebox.showinfo("Info", "Settings saved successfully.")

if __name__ == "__main__":
    app = QSSetApp()
    app.mainloop()
