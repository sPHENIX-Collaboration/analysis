#! /usr/bin/env python
from optparse import OptionParser
import sys
import os
import datetime
from array import *
from ROOT import *
import numpy
import math
import glob

gROOT.SetBatch(True)

def centrality_bin(exp):
    if exp == 'ALICE':
        cbin = [0, 5, 10, 20, 30, 40, 50, 60, 70]
    elif exp == 'PHOBOS':
        cbin = [0, 3, 6, 10, 15, 20, 25, 30, 35, 40, 45, 50, 55, 60, 65, 70]
    elif exp == 'CMS':
        cbin = [0, 5, 10, 15, 20, 25, 30, 35, 40, 45, 50, 55, 60, 65, 70]
    else:
        print('Experiment not recognized. Exiting...')
        sys.exit()
        
    return cbin

if __name__ == '__main__':
    parser = OptionParser(usage='usage: %prog ver [options -h]')
    parser.add_option('--filename', dest='filename', type='string', default='./mbdana_npart_20869.root', help='file name')
    parser.add_option('--experiment', dest='experiment', type='string', default='ALICE', help='use the centrality binning from which experiment: ALICE, PHOBOS, or CMS')
    
    (opt, args) = parser.parse_args()
    print('opt: {}'.format(opt))

    filename = opt.filename
    experiment = opt.experiment
    
    centralitybin = centrality_bin(experiment)
    ncentbin = len(centralitybin) - 1
    avgnpart_centbin = [0] * ncentbin
    
    fullcentbin = [i for i in range(0, centralitybin[-1]+1)]
    nfullcentbin = len(fullcentbin) - 1
    npart_fullcentbin = []
    
    f = TFile(filename, "READ")
    ntp = f.Get("tn_npart")
    j = 0
    for i in range(0, ntp.GetEntries()):
        ntp.GetEntry(i)
        print('Centrality bin: {}-{}, <npart>: {}'.format(ntp.bin-1, ntp.bin, ntp.npart))
        npart_fullcentbin.append(ntp.npart)
        
        if ntp.bin <= centralitybin[j+1]:
            avgnpart_centbin[j] += ntp.npart
        
        if ntp.bin > centralitybin[j+1]:
            avgnpart_centbin[j] /= (centralitybin[j+1] - centralitybin[j])
            j += 1
            if j == ncentbin:
                break
            avgnpart_centbin[j] += ntp.npart
    
    f.Close()
       
    print('---------------------------------------------------------------------------\nUsing the centrality bin of experiment {}: '.format(experiment), centralitybin)
    print('The corresponding <npart> for each centrality interval is:', [float('{:.4g}'.format(x)) for x in avgnpart_centbin])
        
    print('Latex code for the table:')
    print('---------------------------------------------------------------------------')
    print(r'\begin{table}[h]')
    print(r'\centering')
    print(r'\begin{tabular}{c|c}')
    print(r'\hline')
    print(r'Centrality interval [\%] & $\langle$\npart$\rangle$ \\')
    print(r'\hline')
    for i in range(0, ncentbin):
        print(r'{}-{} & {:.4g} \\'.format(centralitybin[i], centralitybin[i+1], avgnpart_centbin[i]))
    print(r'\hline')
    print(r'\end{tabular}')
    print(r'\caption{Centrality intervals and corresponding $\langle$\npart$\rangle$ values.}')
    print(r'\label{tab:centinterval_npart}')
    print(r'\end{table}')
    print('---------------------------------------------------------------------------')
    print(r'\begin{table}[h]')
    print(r'\centering')
    print(r'\begin{tabular}{c|c}')
    print(r'\hline')
    print(r'Centrality interval [\%] & $\langle$\npart$\rangle$ \\')
    print(r'\hline')
    for i in range(0, nfullcentbin):
        print(r'{}-{} & {:.4g} \\'.format(fullcentbin[i], fullcentbin[i+1], npart_fullcentbin[i]))
    print(r'\hline')
    print(r'\end{tabular}')
    print(r'\caption{Centrality intervals and corresponding $\langle$\npart$\rangle$ values.}')
    print(r'\label{tab:centinterval_npart}')
    print(r'\end{table}')