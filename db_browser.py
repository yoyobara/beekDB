#!/bin/python3

"""
gui for the sql tables using the beeklib.py library.
"""

import tkinter as tk
from tkinter import ttk

class BeekGui(tk.Tk):
    """
    the gui app for viewing and quering the beekDB server
    """

    def __init__(self):
        super().__init__()
        self.query_entry: ttk.Entry = ttk.Entry(self)
        self.query_button: ttk.Button = ttk.Button(self, command=self.button_action)
        self.table: ttk.Treeview = ttk.Treeview(self, columns=['ok', 'yes', 'sure', 'obviously'], show=['headings'])

    def grid_stuff(self) -> None:
        """
        grids widgets to the screen
        """

        self.query_entry.grid(row=0, column=0)
        self.query_button.grid(row=0, column=1)
        self.table.grid(row=1, column=0, columnspan=2)

    def setup_table(self, table_dict: dict[str, list[int | float | bytes]]) -> ttk.Treeview:
        return ttk.Treeview(self, columns=table_dict.keys())

    def button_action(self) -> None:
        self.table = self.setup_table({'only_me': [1, 2, 3]})


if __name__ == '__main__':
    gui = BeekGui()
    gui.grid_stuff()
    gui.mainloop()
