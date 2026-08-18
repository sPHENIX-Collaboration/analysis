#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on February 13 13:51 2025
Created in PyCharm
Created as sphenix_pp_qa/print_spindb_info_for_latex

@author: Dylan Neff, dn277127
"""

import subprocess

def main():
    # Database connection details
    dbname = "spinDB"
    table_name = "spin"

    # SQL query to get column names and data types
    query = f"""
    SELECT column_name, data_type
    FROM information_schema.columns
    WHERE table_name = '{table_name}'
    ORDER BY ordinal_position;
    """

    # Run the psql command using subprocess and capture the output
    command = f"psql -d {dbname} -c \"{query}\" -t -A"
    result = subprocess.run(command, shell=True, capture_output=True, text=True)

    # Split the output into lines
    lines = result.stdout.strip().split("\n")

    # Start LaTeX table code using longtable for page splitting
    latex_code = """\\begin{longtable}{|l|l|l|}
\\caption{Table structure of '""" + table_name + """'} \\\\
\\label{tab:spindb_cols} \\\\
\\hline
Column Name & Data Type & Notes \\\\ \\hline
\\endfirsthead
\\hline
Column Name & Data Type & Notes \\\\ \\hline
\\endhead
"""

    # Process each line of the result
    for line in lines:
        line = line.strip().split("|")
        if len(line) != 2:
            print(f"Skipping line: {line}")
            continue
        column_name, data_type = line  # Output columns are separated by '|'
        column_name = column_name.strip().replace('_', '\\_')
        data_type = data_type.strip()
        latex_code += f"{column_name} & {data_type} & \\\\ \\hline\n"

    latex_code += "\\end{longtable}"

    # Print the LaTeX code
    print(latex_code)
    print('donzo')

if __name__ == '__main__':
    main()