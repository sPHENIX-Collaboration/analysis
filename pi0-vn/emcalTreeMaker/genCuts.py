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

    e          = [1]
    e2         = [1, 1.25]
    asym       = [0.3, 0.4, 0.5, 0.8]
    deltaR     = [0]
    deltaR_max = [0.5, 1, 2, 3]
    chi        = [4]

    print(f'Cluster E:               {e}')
    print(f'Cluster E2:              {e2}')
    print(f'Cluster pair asym:       {asym}')
    print(f'Cluster pair min deltaR: {deltaR}')
    print(f'Cluster pair max deltaR: {deltaR_max}')
    print(f'Max Cluster chi:         {chi}')

    with open(f'{output}', mode='w') as file:
        file.write(f'E1_min,E2_min,asym_max,deltaR_min,deltaR_max,chi_max\n')
        for i1 in e:
            for i2 in e2:
                for i3 in asym:
                    for i4 in deltaR:
                        for i5 in deltaR_max:
                            for i6 in chi:
                                file.write(f'{i1},{i2},{i3},{i4},{i5},{i6}\n')
