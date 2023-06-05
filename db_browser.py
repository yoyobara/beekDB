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

        self.__query_entry: ttk.Entry = ttk.Entry(self, width=70)
        self.__query_button: ttk.Button = ttk.Button(self, command=self.button_action, text='SEND')
        self.__table: ttk.Treeview = self.setup_table({}, 0)

        self.bind("<Return>", self.button_action)

    def grid_stuff(self) -> None:
        """
        grids widgets to the screen
        """

        self.__query_entry.grid(row=0, column=0, padx=10, pady=10)
        self.__query_button.grid(row=0, column=1, padx=10, pady=10)
        self.grid_table()

    def grid_table(self) -> None:
        """
        grid the table Treeview
        """

        self.__table.grid(row=1, column=0, columnspan=2, padx=10, pady=10, sticky='nsew')

    def setup_table(self, table_dict: dict[str, list[int | float | bytes]], records_count: int) -> ttk.Treeview:
        """
        create a new treeview from table data
        """

        tab = ttk.Treeview(self, columns=list(table_dict.keys()), show=['headings'])

        for col in tab['columns']:
            tab.heading(col, text=col.upper())

        for i in range(records_count):
            values = [(vals[i] if not isinstance(vals[i], bytes) else vals[i].decode()) for vals in table_dict.values()]
            tab.insert('', 'end', values=values)

        return tab

    def button_action(self, dummy=None) -> None:
        """
        action of the send button. (dummy cause might be called from enter key)
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
                table = self.setup_table(data, len(data[list(data.keys())[0]]))
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
