import os

# Base directory path containing the .list files
base_directory = '/sphenix/user/egm2153/calib_study/JetValidation/macro/data_list_files'

# Get all files in the directory with the .list extension
file_list = [f for f in os.listdir(base_directory) if f.endswith('.list')]

# Number of files per output file
files_per_runlist = 5000

# Split the file list into chunks of 5000
for i in range(0, len(file_list), files_per_runlist):
    chunk = file_list[i:i + files_per_runlist]
    
    # Create the filename for the runlist file
    runlist_filename = f'runlist_{i // files_per_runlist}.txt'
    
    # Write the chunk of file paths to the runlist file
    with open(runlist_filename, 'w') as runlist_file:
        for file in chunk:
            runlist_file.write(os.path.join(base_directory, file) + '\n')

print("Runlist files created successfully!")
