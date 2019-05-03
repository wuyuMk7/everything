#!/usr/bin/env python

import sqlite3, os

# Record format:
# tuple (time, fsr_data, fsr_result, loadcell_data, loadcell_result, final_result) 
# "time" field should be a timestamp. 
# "fsr_result", "loadcell_result", and "final_result" fields are boolean values represented by 0 or 1.

def insert(record):
    conn = sqlite3.connect(os.path.dirname(os.path.realpath(__file__)) + '/data.db')
    c = conn.cursor()

    sql = 'INSERT INTO '
    sql += 'sittinginfo(time, fsr_data, fsr_result, loadcell_data, loadcell_result, final_result) '
    sql += 'VALUES (?, ?, ?, ?, ?, ?)'

    c.execute(sql, record)

    conn.commit()
    conn.close()


# NOTE: use SQL - SELECT strftime('%s', 'now') to get current timestamp in sqlite3.

if __name__ == "__main__":
    conn = sqlite3.connect('data.db')
    c = conn.cursor()

    c.execute(''' CREATE TABLE IF NOT EXISTS SITTINGINFO 
            (ID                 INTEGER  PRIMARY KEY,
             TIME               INTEGER  NOT NULL,
             FSR_DATA           TEXT     NOT NULL,
             FSR_RESULT         INTEGER  NOT NULL,
             LOADCELL_DATA      TEXT     NOT NULL,
             LOADCELL_RESULT    INTEGER  NOT NULL,
             FINAL_RESULT       INTEGER  NOT NULL);''')

    conn.commit()
    conn.close()
    print("Table created successfully.")

    # Insertion test
    # insert((1556340, "123", 1, "rrr", 1, 1))

