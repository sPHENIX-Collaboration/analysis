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

if __name__ == '__main__':
    hM_dR = TH1F('hM_dR', 'hM_dR', 60, 0, 3);

    zpos = [-22.57245, -20.57245, -18.57245, -16.57245, -14.57245, -12.57245, -10.97245, -9.372450, -7.772450, -6.172450, -4.572450, -2.972450, -1.372450, 0.4275496, 2.0275495, 3.6275494, 5.2275495, 6.8275494, 8.4275493, 10.027549, 11.627549, 13.627549, 15.627549, 17.627550, 19.627550, 21.627550];

    # Very fine binning for ladder overlap study
    # INTT pitch size in phi = 78 um (Reference: https://indico.bnl.gov/event/15547/contributions/62868/attachments/41171/68968/2022_sPHENIX_School_RN.pdf)
    # INTT outer radius 102.62 mm
    # Azimuthal angle per pitch = (78*1E-6) / (102.62*1E-3) ~= 7.601E-4 rad ~= 0.0435 degree
    dPhi_pitch = 7.601E-4

    # average vertex position
    vx = 0.1 * ((-0.2367 + (-0.2665)) / 2.)
    vy = 0.1 * ((2.6001 + 2.5812) / 2.)
    vz = -20.0

    layer_radius = [7.453, 8.046, 9.934, 10.569]

    for idphi in range(128):
        for layer in range(4):
            for i in range(26):
                for j in range(26):
                    rz1 = zpos[i] - vz
                    rz2 = zpos[j] - vz
                    rsq1 = layer_radius[layer]**2 + rz1**2
                    rsq2 = layer_radius[layer]**2 + rz2**2
                    costheta1 = rz1 / sqrt(rsq1)
                    costheta2 = rz2 / sqrt(rsq2)
                    eta1 = -0.5 * log((1.0 - costheta1) / (1.0 + costheta1))
                    eta2 = -0.5 * log((1.0 - costheta2) / (1.0 + costheta2))
                    deta = eta1 - eta2
                    dphi = idphi * dPhi_pitch
                    dR = sqrt(deta**2 + dphi**2)
                    hM_dR.Fill(dR)
    
    # Draw the histogram
    c = TCanvas('c', 'c', 800, 700)
    gPad.SetLeftMargin(0.18)
    c.cd()
    hM_dR.GetXaxis().SetTitle('#DeltaR')
    hM_dR.GetYaxis().SetTitle('Entries')
    hM_dR.GetYaxis().SetTitleOffset(1.7)
    hM_dR.SetLineColor(kBlack)
    hM_dR.SetLineWidth(2)
    hM_dR.GetYaxis().SetRangeUser(0, hM_dR.GetMaximum() * 1.2)
    hM_dR.Draw('hist')
    c.SaveAs('./RecoTracklet/INTTdR.pdf')
    c.SaveAs('./RecoTracklet/INTTdR.png')
    
    