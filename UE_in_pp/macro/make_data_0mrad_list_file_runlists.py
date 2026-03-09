import os

# Base directory path containing the .list files
base_directory = '/sphenix/user/egm2153/calib_study/analysis/UE_in_pp/macro/data_0mrad_list_files'

# Get all files in the directory with the .list extension
file_list = [f for f in os.listdir(base_directory) if f.endswith('.list')]

# Separate files into two categories based on their names
jet_files = sorted([f for f in file_list if 'dst_jet_run2pp' in f])
jetcalo_files = sorted([f for f in file_list if 'dst_jetcalo_run2pp' in f])

# Number of files per output file
files_per_runlist = 15000

# Function to create runlist files
def create_runlist_files(file_list, category):
    for i in range(0, len(file_list), files_per_runlist):
        chunk = file_list[i:i + files_per_runlist]
        
        # Create the filename for the runlist file
        runlist_filename = f'{category}_runlist_{i // files_per_runlist}.txt'
        
        # Write the chunk of file paths to the runlist file
        with open(runlist_filename, 'w') as runlist_file:
            for file in chunk:
                runlist_file.write(os.path.join(base_directory, file) + '\n')

# Create runlist files for each category
create_runlist_files(jet_files, 'dst_jet_run2pp')
create_runlist_files(jetcalo_files, 'dst_jetcalo_run2pp')

print("Runlist files created successfully!")
