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
                    dsttype LIKE 'HIST_CALO%') h
            ON
                d.dataset = h.dataset AND d.runnumber = h.runnumber AND d.segment = h.segment
            WHERE
                d.dsttype LIKE 'DST_CALO_run2pp'
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
                    dsttype LIKE 'HIST_CALO%') h
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
        a.filename LIKE 'HIST_CALO%';
    """

    with psycopg2.connect(f"dbname=FileCatalog") as conn:
        return pd.read_sql_query(query, conn)

def main():
    df = get_file_paths()

    print(df)

    print(df.drop_duplicates(['runnumber','dataset']))

    os.makedirs('files/hists',exist_ok=True)

    for index in df.drop_duplicates(['runnumber','dataset']).index:
        dataset = df["dataset"][index]
        run = df["runnumber"][index]
        print(f'Processing: {run},{dataset}')

        df[(df['runnumber']==run) & (df['dataset']==dataset)]['full_file_path'].to_csv(f'files/hists/{run}_{dataset}.list', index=False, header=False)

if __name__ == "__main__":
    main()
