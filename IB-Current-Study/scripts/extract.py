#!/usr/bin/env python3
#
# Script to extract data from postgres db

import psycopg2
import pandas as pd
import pandas.io.sql as sqlio
import argparse

parser = argparse.ArgumentParser(description='Extract data from the DAQ Database.')

subparser = parser.add_subparsers(dest='command')

currents = subparser.add_parser('currents', help='Extract the IB Currents from the database.')

currents.add_argument('-n', '--samples', type=int, default=5000000, help='Number of samples to read from the database. Default: 5e6')
currents.add_argument('-d', '--dbname', type=str, default='daq', help='Database name. Default: daq')
currents.add_argument('-u', '--user', type=str, default='phnxro', help='User. Default: phnxro')
currents.add_argument('-s', '--host', type=str, default='db1.sphenix.bnl.gov', help='Database host. Default: db1.sphenix.bnl.gov')
currents.add_argument('-t', '--threshold', type=float, default=1.5, help='Maximum difference in voltage between vmeas (measured voltage) and vset (set voltage). Default: 1.5 V')
currents.add_argument('-o', '--output', type=str, default='currents.csv', help='Name of output CSV. Default: currents.csv')

raw = subparser.add_parser('raw', help='Extract the raw counts from the database.')

raw.add_argument('-n', '--samples', type=int, default=5000, help='Number of samples to read from the database. Default: 1e5')
raw.add_argument('-t', '--trigger', type=int, default=24, help='Trigger type. Default: 24 (Photon 2 GeV + MBD NS >= 1)')
raw.add_argument('-d', '--dbname', type=str, default='daq', help='Database name. Default: daq')
raw.add_argument('-u', '--user', type=str, default='phnxro', help='User. Default: phnxro')
raw.add_argument('-s', '--host', type=str, default='db1.sphenix.bnl.gov', help='Database host. Default: db1.sphenix.bnl.gov')
raw.add_argument('-o', '--output', type=str, default='raw.csv', help='Name of output CSV. Default: raw.csv')

args = parser.parse_args()

if __name__ == '__main__':
    if(args.command == 'currents'):
        samples   = args.samples
        threshold = args.threshold
        dbname    = args.dbname
        user      = args.user
        host      = args.host
        output    = args.output

        print(f'samples: {samples}')
        print(f'ensure |vmeas - vset| < {threshold}')
        print(f'dbname: {dbname}')
        print(f'user: {user}')
        print(f'host: {host}')
        print(f'output CSV: {output}')

        with psycopg2.connect(f"host='{host}' dbname='{dbname}' user='{user}'") as conn:
            sql = f'select readtime, sector, ib, vmeas, vset, imeas from emcal_mpodlog where abs(vmeas-vset) < {threshold} and vmeas != 0 and vset != 0 order by readtime desc limit {samples};'

            df = pd.read_sql_query(sql, conn)

            df.to_csv(output,index=False)

            # preview of contents
            print(df.head(20))

    if(args.command == 'raw'):
        samples   = args.samples
        trigger   = args.trigger
        dbname    = args.dbname
        user      = args.user
        host      = args.host
        output    = args.output

        print(f'samples: {samples}')
        print(f'trigger: {trigger}')
        print(f'dbname: {dbname}')
        print(f'user: {user}')
        print(f'host: {host}')
        print(f'output CSV: {output}')

        with psycopg2.connect(f"host='{host}' dbname='{dbname}' user='{user}'") as conn:
            sql = f'select updated as readtime, index as trigger, raw from gl1_scalers where index={trigger} order by updated desc limit {samples};'

            df = pd.read_sql_query(sql, conn)

            df.to_csv(output,index=False)

            # preview of contents
            print(df.head(20))
