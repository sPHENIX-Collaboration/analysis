#!/usr/bin/env python3
"""
This module extracts and filters and slims the pulse width info csv based on sector, ib, and best pulse width.
"""
import argparse
import os
import pandas as pd

parser = argparse.ArgumentParser()

parser.add_argument('-i'
                    , '--input-csv', type=str
                    , required=True
                    , help='Input CSV')

parser.add_argument('-o'
                    , '--output', type=str
                    , default='pulse-width-info.csv'
                    , help='Output pulse width info csv.')

args = parser.parse_args()

if __name__ == '__main__':
    input_csv = os.path.realpath(args.input_csv)
    output    = os.path.realpath(args.output)

    os.makedirs(os.path.dirname(output), exist_ok=True)

    print(f'Input: {input_csv}')
    print(f'Output: {output}')

    df = pd.read_csv(input_csv)

    # Write a slim version of the original csv
    df[['sector','ib','bestW']].drop_duplicates(ignore_index=True).to_csv(output,index=False)
