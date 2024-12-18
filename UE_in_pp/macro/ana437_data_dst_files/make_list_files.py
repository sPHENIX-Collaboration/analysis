import os

# Directory containing the .list files
directory = '/sphenix/user/egm2153/calib_study/JetValidation/macro/ana437_data_dst_files'
outdirectory = '/sphenix/user/egm2153/calib_study/JetValidation/macro/data_list_files'

# Iterate over all files in the directory
for filename in os.listdir(directory):
    if filename.endswith('.list'):
        filepath = os.path.join(directory, filename)
        
        # Read the lines from the current .list file
        with open(filepath, 'r') as file:
            lines = file.readlines()
        
        # Split the lines into chunks of 2 lines
        for i in range(0, len(lines), 2):
            chunk = lines[i:i+2]
            
            # Generate the new filename with a chunk index
            new_filename = f"{filename[:-5]}-{i//2:05d}.list"
            new_filepath = os.path.join(outdirectory, new_filename)
            
            # Write the chunk to the new file
            with open(new_filepath, 'w') as new_file:
                new_file.writelines(chunk)
