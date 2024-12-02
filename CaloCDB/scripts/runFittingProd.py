#!/usr/bin/env python3
import pandas as pd
import psycopg2
import os

def get_file_paths(threshold=500000):
    query = f"""

    SELECT
        a.dataset, a.runnumber, f.full_file_path
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
                    dsttype LIKE 'HIST_CALOFITTINGQA%') h
            ON
                d.dataset = h.dataset AND d.runnumber = h.runnumber AND d.segment = h.segment
            WHERE
                d.dsttype LIKE 'DST_CALOFITTING_run2pp'
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
        a.filename LIKE 'HIST_CALOFITTINGQA%';
    """

    with psycopg2.connect(f"dbname=FileCatalog") as conn:
        return pd.read_sql_query(query, conn)

def main():
    df = get_file_paths()

    print(df)
    df.drop_duplicates(subset=['runnumber','dataset'], inplace=True)
    print(df)

    os.makedirs('files/hists',exist_ok=True)

    if os.path.exists('completedruns.txt'):
        with open('completedruns.txt') as f:
            completed_runs_datasets = set(line.strip() for line in f)

    fdf = df[~df.apply(lambda row: f"{row['runnumber']},{row['dataset']}" in completed_runs_datasets, axis=1)]

    for index in fdf.index:
        dataset = fdf["dataset"][index]
        run = fdf["runnumber"][index]
        print(f'Processing: {run},{dataset}')

        with open('completedruns.txt', 'a') as f:
            f.write(f"{run},{dataset}\n")

        fdf[(fdf['runnumber']==run) & (fdf['dataset']==dataset)]['full_file_path'].to_csv(f'files/hists/{run}_{dataset}.list', index=False, header=False)

if __name__ == "__main__":
    main()
