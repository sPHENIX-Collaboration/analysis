#!/usr/bin/env python3
#
# Script to extract data from postgres db

import psycopg2
import pandas as pd
import pandas.io.sql as sqlio

dbname ='daq'
user   ='phnxro'
host   ='db1.sphenix.bnl.gov'

with psycopg2.connect(f"host='{host}' dbname='{dbname}' user='{user}'") as conn:
    sql = 'select readtime, sector, ib, vmeas, vset, imeas from emcal_mpodlog where abs(vmeas-vset) < 1.5 and vmeas != 0 and vset != 0 order by readtime desc limit 10000000;'

    df = pd.read_sql_query(sql, conn)

    df.to_csv('test.csv',index=False)

    # preview of contents
    print(df.head(20))
