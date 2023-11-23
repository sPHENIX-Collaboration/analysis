#!/usr/bin/env python3
import numpy as np
import argparse
import os

parser = argparse.ArgumentParser()
parser.add_argument('-o', '--output', type=str, default='cuts.txt', help='Name of output file')

args = parser.parse_args()

if __name__ == '__main__':
    output = os.path.realpath(args.output)
    print(f'Output: {output}')

    e      = np.round(np.linspace(0.5,2,int((2-0.5)/0.25)+1),4)
    chi    = [4]
    deltaR = np.round(np.linspace(0.05,0.1,int((0.1-0.05)/0.01)+1),4)
    asym   = np.round(np.linspace(0.3, 0.8, int((0.8-0.3)/0.1)+1),4)

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
