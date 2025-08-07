#!/usr/bin/env python3
"""
This module generates a list of run / datasets given a run type and event threshold.
"""
import argparse
import os
import logging
import pandas as pd
from sqlalchemy import create_engine

parser = argparse.ArgumentParser()

parser.add_argument('-i'
                    , '--run-type', type=str
                    , default='run2auau'
                    , choices=['run2pp','run2auau','run3auau']
                    , help='Run Type. Default: run2auau')

parser.add_argument('-n'
                    , '--min-events', type=int
                    , default=500000
                    , help='Minimum Events (for Run). Default: 500k')

parser.add_argument('-o'
                    , '--output', type=str
                    , default='.'
                    , help='Output directory of datasets.')

parser.add_argument('-v'
                    , '--verbose', action='store_true'
                    , help='Verbose.')

args = parser.parse_args()

def get_file_paths(engine, runtype='run2auau', threshold=500000):
    """
    Generate file paths from given minimum events and run type.
    """

    query = f"""
    SELECT
        a.dataset, a.runnumber, f.full_file_path, f.time
    FROM
        datasets a
    JOIN (
            SELECT
                d.dataset, d.runnumber
            FROM
                datasets d
            JOIN (
                SELECT
                    dataset, runnumber, segment
                FROM
                    datasets
                WHERE
                    dsttype = 'HIST_CALOQA_{runtype}') h
            ON
                d.dataset = h.dataset AND d.runnumber = h.runnumber AND d.segment = h.segment
            WHERE
                d.dsttype LIKE 'DST_CALO_{runtype}'
            GROUP BY
                d.dataset, d.runnumber
            HAVING
                SUM(d.events) > {threshold}

            UNION

            SELECT
                d.dataset, d.runnumber
            FROM
                datasets d
            JOIN (
                SELECT
                    dataset, runnumber, segment
                FROM
                    datasets
                WHERE
                    dsttype = 'HIST_CALOQA_{runtype}') h
            ON
                d.dataset = h.dataset AND d.runnumber = h.runnumber AND d.segment = h.segment
            WHERE
                d.dsttype LIKE 'DST_CALOFITTING_{runtype}'
            GROUP BY
                d.dataset, d.runnumber
            HAVING
                SUM(d.events) > {threshold}) k
    ON
        k.dataset = a.dataset AND k.runnumber = a.runnumber
    JOIN
        files f
    ON
        f.lfn = a.filename
    WHERE
        a.filename LIKE %(filename_pattern)s;
    """
    parameters = {'filename_pattern': f'HIST_CALOQA_{runtype}%'}

    return pd.read_sql_query(query, engine, params=parameters)

def main():
    """
    Main Function
    """

    run_type   = args.run_type
    min_events = args.min_events
    output     = os.path.realpath(args.output)
    verbose    = args.verbose

    os.makedirs(output, exist_ok=True)

    print(f'Run Type: {run_type}')
    print(f'Min Events: {min_events}')
    print(f'Output: {output}')
    print(f'Verbose: {verbose}')

    # Database Connection
    DB_NAME = "FileCatalog"
    DATABASE_URL = f"postgresql+psycopg2:///{DB_NAME}"
    logging.basicConfig()

    if verbose:
        logging.getLogger('sqlalchemy.engine').setLevel(logging.INFO) # Set to logging.DEBUG for even more detail

    engine = create_engine(DATABASE_URL)

    # 1. Get the dataframe from the database
    df = get_file_paths(engine, run_type, min_events)

    if verbose:
        print("Original")
        print(df)
        print("\n" + "="*70 + "\n")

    # 2. Calculate the minimum time for each dataset
    min_times_per_dataset = df.groupby('dataset')['time'].min().sort_values(ascending=False)

    if verbose:
        print("Minimum time for each dataset:")
        print(min_times_per_dataset)
        print("\n" + "="*70 + "\n")

    # 3. Add this minimum time back to the original DataFrame as 'dataset_min_time'
    df_processed = df.merge(min_times_per_dataset.rename('dataset_min_time'),
                            left_on='dataset',
                            right_index=True)

    if verbose:
        print("DataFrame with 'dataset_min_time' column:")
        print(df_processed[['dataset', 'runnumber', 'time', 'full_file_path', 'dataset_min_time']])
        print("\n" + "="*70 + "\n")

    # 4. For each 'runnumber', find the 'dataset_min_time' of the HIGHEST PRIORITY dataset containing it.
    #    "Highest priority" means the dataset with the LATEST (maximum) 'dataset_min_time'.
    highest_priority_time_for_runnumber = df_processed.groupby('runnumber')['dataset_min_time'].max()
    highest_priority_time_for_runnumber.name = 'highest_priority_dataset_min_time_for_runnumber'

    if verbose:
        print("Highest Priority 'dataset_min_time' for each 'runnumber':")
        print(highest_priority_time_for_runnumber)
        print("\n" + "="*70 + "\n")

    # 5. Merge this information back to the DataFrame
    df_processed = df_processed.merge(highest_priority_time_for_runnumber,
                                    left_on='runnumber',
                                    right_index=True)

    if verbose:
        print("DataFrame with 'highest_priority_dataset_min_time_for_runnumber' column:")
        print(df_processed[['dataset', 'runnumber', 'time', 'full_file_path', 'dataset_min_time', 'highest_priority_dataset_min_time_for_runnumber']])
        print("\n" + "="*70 + "\n")

    # 6. Filter the DataFrame: Keep only rows where the row's 'dataset_min_time'
    #    matches the 'highest_priority_dataset_min_time_for_runnumber'.
    #    This ensures we keep ALL rows for a runnumber from its highest-priority dataset.
    reduced_df = df_processed[
        df_processed['dataset_min_time'] == df_processed['highest_priority_dataset_min_time_for_runnumber']
    ]

    if verbose:
        print("Final Reduced DataFrame (sorted by time for readability):")
        print(reduced_df.sort_values(by='time').reset_index(drop=True))

    # 7. Group by 'runnumber' and 'dataset'
    # Iterating over this grouped object is efficient.
    grouped = reduced_df.groupby(['runnumber', 'dataset'])

    # 8. Loop through each unique group
    for (run, dataset), group_df in grouped:
        print(f'Processing: {run},{dataset}')

        filepath = f'{output}/{run}_{dataset}.list'

        group_df['full_file_path'].to_csv(filepath, index=False, header=False)

if __name__ == "__main__":
    main()
