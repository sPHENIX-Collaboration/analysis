#!/usr/bin/env python3
import numpy as np
import argparse
import os

parser = argparse.ArgumentParser()
parser.add_argument('-o', '--output', type=str, default='cuts.txt', help='Name of output file')

args = parser.parse_args()

def getRange(low, high, step, precision=4):
    n = int((high-low)/step)+1 # number of samples from low to high
    return np.round(np.linspace(low,high,n),precision)

if __name__ == '__main__':
    output = os.path.realpath(args.output)
    print(f'Output: {output}')

    e      = getRange(0.5, 1, 0.05)
    chi    = [4]
    deltaR = [0]
    asym   = getRange(0.5, 0.8, 0.05)

    print(f'Cluster E:           {e}')
    print(f'Cluster chi:         {chi}')
    print(f'Cluster pair deltaR: {deltaR}')
    print(f'Cluster pair asym:   {asym}')

    with open(f'{output}', mode='w') as file:
        for i in e:
            for j in asym:
                for k in deltaR:
                    for l in chi:
                       file.write(f'{i}, {j}, {k}, {l}\n')
