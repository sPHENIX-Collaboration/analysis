#!/bin/sh

./make_table.sh
./make_table_g4.sh
./make_table_rc.sh
./make_table_de.sh
./make_files_de.sh
./make_table_pa.sh
./make_file_pa.sh

#the order in which the XEMC DE-related utils are called
#is actually kinda important
#because i am bad at writing good code.

./make_files_de_XEMC.sh
./make_table_de_XEMC.sh
./make_file_pa_XEMC.sh
./make_table_pa_XEMC.sh
