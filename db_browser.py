#!/bin/python3

"""
gui for the sql tables using the beeklib.py library.
"""

import sys
import tkinter as tk
from tkinter import ttk
from tkinter import messagebox

import beeklib

class BeekGui(tk.Tk):
    """
    the gui app for viewing and quering the beekDB server
    """

    def __init__(self, db_connection: beeklib.BeekDbConnection):
        super().__init__()

        self.conn = db_connection

        self.__query_entry: ttk.Entry = ttk.Entry(self)
        self.__query_button: ttk.Button = ttk.Button(self, command=self.button_action)
        self.__table: ttk.Treeview = self.setup_table({'wow': [4], 'sss': [2]}, 1)

    def grid_stuff(self) -> None:
        """
        grids widgets to the screen
        """

        self.__query_entry.grid(row=0, column=0)
        self.__query_button.grid(row=0, column=1)
        self.grid_table()

    def grid_table(self) -> None:
        """
        grid the table Treeview
        """

        self.__table.grid(row=1, column=0, columnspan=2)

    def setup_table(self, table_dict: dict[str, list[int | float | bytes]], records_count: int) -> ttk.Treeview:
        """
        create a new treeview from table data
        """

        tab = ttk.Treeview(self, columns=table_dict.keys(), show=['headings'])

        for i in range(records_count):
            values = [vals[i] for vals in table_dict.values()]
            tab.insert('', 'end', values=values)

        return tab

    def button_action(self) -> None:
        """
        action of the send button
        """

        # query from entry
        query = self.__query_entry.get()

        # query the server
        success, data = self.conn.query(query)

        table = None
        if success:
            if data is None:
                # empty table
                table = self.setup_table({}, 0)
            else:
                # non empty table
                table = self.setup_table(data, len(data.values()[0]))
        else:
            # error
            messagebox.showerror("SQL Query Error", data)
            return

        self.__table.destroy()
        self.__table = table
        self.grid_table()

def main():
    """
    Main function
    """

    args = None

    if len(sys.argv) == 2: # address only
        args = (sys.argv[1],)

    elif len(sys.argv) == 3: # address and port
        args = (sys.argv[1], int(sys.argv[2]))

    else:
        print(f"usage: {sys.argv[0]} <address> [port]")
        sys.exit()

    with beeklib.connect(*args) as connection:
        gui = BeekGui(connection)
        gui.grid_stuff()
        gui.mainloop()

if __name__ == '__main__':
    main()
