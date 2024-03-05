# The offline event combiner for INTT and MBD for the sPHENIX run 23 data
Updated: 2024-03-05

The python code requires at least 5 arguments
1. <outfile_full_directory>
2. <intt_file_full_directory>
3. <intt_tree_name>
4. <mbd_file_in_full_directory>
5. <mbd_tree_name>
6. <N_event>: this is optional, if it's not given, then all events of the file will be included

To run the code: python3 intt_mbd_evt_combiner_v1.py <outfile_full_directory> <intt_file_full_directory> <intt_tree_name> <mbd_file_in_full_directory> <mbd_tree_name> <N event>
