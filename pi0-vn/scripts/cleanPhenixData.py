#!/usr/bin/env python3

import pandas as pd

phenix_data_path = '/Users/patsfan753/Desktop/PHENIX.csv'
phenix_data = pd.read_csv(phenix_data_path)

# Identifying the start of each centrality block
start_indices = phenix_data.index[phenix_data['$p_T$ (GeV/$c$) LOW'] == 1.0].tolist()

# Extracting relevant centralities (20-30%, 30-40%, 40-50%, 50-60%)
# Assuming each centrality block has 17 rows of data
relevant_centralities = [2, 3, 4, 5]  # Indices for 20-30%, 30-40%, 40-50%, 50-60%
relevant_data = pd.DataFrame()

for centrality_index in relevant_centralities:
    start_row = start_indices[centrality_index]
    centrality_data = phenix_data.iloc[start_row:start_row + 17]
    relevant_data = pd.concat([relevant_data, centrality_data])

# Filtering out rows for the pT range from 2 to 5 GeV
relevant_data = relevant_data[(relevant_data['$p_T$ (GeV/$c$) LOW'] >= 2) &
                             (relevant_data['$p_T$ (GeV/$c$) HIGH'] <= 5)]

# Resetting the index of the dataframe
relevant_data.reset_index(drop=True, inplace=True)

# Saving the cleaned data to a new CSV file
output_data_path = '/Users/patsfan753/Desktop/Cleaned_PHENIX.csv'
relevant_data.to_csv(output_data_path, index=False)

print("Final cleaned data head (first 5 rows):")
print(relevant_data.head())
