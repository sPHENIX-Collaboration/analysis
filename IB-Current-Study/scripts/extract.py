#!/usr/bin/env python3
#
# Script to extract data from postgres db

import psycopg2
import pandas as pd
import pandas.io.sql as sqlio
import argparse

parser = argparse.ArgumentParser(description='Extract the IB Currents from the database.')

parser.add_argument('-n', '--samples', type=int, default=5000000, help='Number of samples to read from the database. Default: 5e6')
parser.add_argument('-d', '--dbname', type=str, default='daq', help='Database name. Default: daq')
parser.add_argument('-u', '--user', type=str, default='phnxro', help='User. Default: phnxro')
parser.add_argument('-s', '--host', type=str, default='db1.sphenix.bnl.gov', help='Database host. Default: db1.sphenix.bnl.gov')
parser.add_argument('-t', '--threshold', type=float, default=1.5, help='Maximum difference in voltage between vmeas (measured voltage) and vset (set voltage). Default: 1.5 V')
parser.add_argument('-o', '--output', type=str, default='test.csv', help='Name of output CSV. Default: test.csv')

args = parser.parse_args()

if __name__ == '__main__':

    samples   = args.samples
    dbname    = args.dbname
    user      = args.user
    host      = args.host
    threshold = args.threshold
    output    = args.output

    print(f'samples: {samples}')
    print(f'dbname: {dbname}')
    print(f'user: {user}')
    print(f'host: {host}')
    print(f'Ensure |vmeas - vset| < {threshold}')
    print(f'Output CSV: {output}')

    with psycopg2.connect(f"host='{host}' dbname='{dbname}' user='{user}'") as conn:
        sql = f'select readtime, sector, ib, vmeas, vset, imeas from emcal_mpodlog where abs(vmeas-vset) < {threshold} and vmeas != 0 and vset != 0 order by readtime desc limit {samples};'

        df = pd.read_sql_query(sql, conn)

        df.to_csv(output,index=False)

        # preview of contents
        print(df.head(20))
