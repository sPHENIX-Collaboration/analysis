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

def read_data(file_path):
    eta = []
    dn_deta = []
    err_plus = []
    err_minus = []

    with open(file_path, 'r') as file:
        lines = file.readlines()

    start_reading = False

    for line in lines:
        if line.strip() == "&":  # Check for the end marker
            break

        if start_reading:
            # Split the line into components and convert them to float
            columns = line.split()
            if len(columns) == 4:
                eta.append(float(columns[0]))
                dn_deta.append(float(columns[1]))
                err_plus.append(float(columns[2]))
                err_minus.append(-1.*float(columns[3]))

        if "dN/dEta vs. Eta" in line:
            start_reading = True

    return eta, dn_deta, err_plus, err_minus


if __name__ == '__main__':
    fout = TFile("./PHOBOS-PhysRevC.83.024913/auau_200GeV.root", "RECREATE") # Reference: https://journals.aps.org/prc/supplemental/10.1103/PhysRevC.83.024913
    
    phobos_list = glob.glob("./PHOBOS-PhysRevC.83.024913/*.txt")
    for f in phobos_list:
        eta, dn_deta, err_plus, err_minus = read_data(f)
        print(f)
        print(eta)
        print(dn_deta)
        print(err_plus)
        print(err_minus)
        print("")
        
        gr = TGraphAsymmErrors(len(eta), array('d', eta), array('d', dn_deta), array('d', [0]*len(eta)), array('d', [0]*len(eta)), array('d', err_minus), array('d', err_plus))
        gr.SetName(f.split("/")[-1].replace(".txt", "").replace("-", "to"))
        gr.SetTitle(f.split("/")[-1].replace(".txt", "").replace("-", "to"))
        gr.Write(f.split("/")[-1].replace(".txt", "").replace("-", "to"))
    
    fout.Close()
    
    