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
from plotUtil import *

gROOT.LoadMacro('./sPHENIXStyle/sPhenixStyle.C')
gROOT.ProcessLine('SetsPhenixStyle()')
gROOT.SetBatch(True)

def Draw_1Dhist_datasimcomp(hdata, hsim, gpadmargin, norm, logy, ymaxscale, XaxisName, Ytitle_unit, prelim, outname):
    hdata.Sumw2()
    hsim.Sumw2()
    binwidth = hdata.GetXaxis().GetBinWidth(1)
    if norm == 'unity':
        hdata.Scale(1. / hdata.Integral(-1, -1))
        hsim.Scale(1. / hsim.Integral(-1, -1))
    elif norm == 'data':
        hsim.Scale(hdata.Integral(-1, -1) / hsim.Integral(-1, -1))
    else:
        if norm != 'none':
            print('Invalid normalization option: {}'.format(norm))
            sys.exit(1)
    
    # Get the maximum bin content 
    maxbincontent = max(hdata.GetMaximum(), hsim.GetMaximum())

    c = TCanvas('c', 'c', 800, 700)
    if logy:
        c.SetLogy()
    c.cd()
    gPad.SetRightMargin(gpadmargin[0])
    gPad.SetTopMargin(gpadmargin[1])
    gPad.SetLeftMargin(gpadmargin[2])
    gPad.SetBottomMargin(gpadmargin[3])
    if norm == 'unity':
        if Ytitle_unit == '':
            hsim.GetYaxis().SetTitle(
                'Normalized entries / ({:g})'.format(binwidth))
        else:
            hsim.GetYaxis().SetTitle(
                'Normalized entries / ({:g} {unit})'.format(binwidth, unit=Ytitle_unit))
    else:
        if Ytitle_unit == '':
            hsim.GetYaxis().SetTitle('Entries / ({:g})'.format(binwidth))
        else:
            hsim.GetYaxis().SetTitle(
                'Entries / ({:g} {unit})'.format(binwidth, unit=Ytitle_unit))

    if logy:
        hsim.GetYaxis().SetRangeUser(hsim.GetMinimum(0)*0.5, (hsim.GetMaximum()) * ymaxscale)
    else:
        hsim.GetYaxis().SetRangeUser(0., (hsim.GetMaximum()) * ymaxscale)
    hsim.GetXaxis().SetTitle(XaxisName)
    hsim.GetXaxis().SetTitleOffset(1.1)
    hsim.GetYaxis().SetTitleOffset(1.35)
    hsim.SetLineColor(1)
    hsim.SetLineWidth(2)
    hsim.Draw('hist')
    hdata.SetMarkerStyle(20)
    hdata.SetMarkerSize(1)
    hdata.SetMarkerColor(1)
    hdata.SetLineColor(1)
    hdata.SetLineWidth(2)
    hdata.Draw('same PE1')
    shift = 0.45 if prelim else 0.52
    leg = TLegend((1-RightMargin)-shift, (1-TopMargin)-0.2,
                  (1-RightMargin)-0.1, (1-TopMargin)-0.03)
    leg.SetTextSize(0.04)
    leg.SetFillStyle(0)
    prelimtext = 'Preliminary' if prelim else 'Work-in-progress'
    leg.AddEntry('', '#it{#bf{sPHENIX}} '+prelimtext, '')
    leg.AddEntry('', 'Au+Au #sqrt{s_{NN}}=200 GeV', '')
    leg.AddEntry(hdata, 'Data', "PE1");
    leg.AddEntry(hsim, 'Simulation', "l");
    leg.Draw()
    c.RedrawAxis()
    c.Draw()
    c.SaveAs(outname+'.pdf')
    c.SaveAs(outname+'.png')
    if(c):
        c.Close()
        gSystem.ProcessEvents()
        del c
        c = 0

if __name__ == '__main__':
    parser = OptionParser(usage='usage: %prog ver [options -n]')
    parser.add_option('-d', '--datahistfile', dest='datahistfile', type='string', default='/sphenix/user/mjpeters/analysis/dNdEta_Run2023/macros/merged_dataHistograms.root', help='Histogram file name (data)')
    parser.add_option('-s', '--simhistfile', dest='simhistfile', type='string', default='/sphenix/user/hjheng/TrackletAna/analysis_INTT/plot/hists/ana382_zvtx-20cm_dummyAlignParams/Hists_RecoTracklets_merged.root', help='Histogram file name (simulation)')
    parser.add_option('-p', '--plotdir', dest='plotdir', type='string', default='ana382_zvtx-20cm_dummyAlignParams', help='Plot directory')

    (opt, args) = parser.parse_args()

    print('opt: {}'.format(opt))

    datahistfile = opt.datahistfile
    simhistfile = opt.simhistfile
    plotdir = opt.plotdir

    os.makedirs('./DataSimComp/{}'.format(plotdir), exist_ok=True)

    hM_NClusLayer1_data = GetHistogram(datahistfile, 'hM_NClusLayer1')
    hM_NPrototkl_data = GetHistogram(datahistfile, 'hM_NPrototkl')
    hM_NRecotkl_Raw_data = GetHistogram(datahistfile, 'hM_NRecotkl_Raw')
    hM_dEta_reco_data = GetHistogram(datahistfile, 'hM_dEta_reco') 
    hM_dEta_reco_altrange_data = GetHistogram(datahistfile, 'hM_dEta_reco_altrange')
    hM_dPhi_reco_data = GetHistogram(datahistfile, 'hM_dPhi_reco')
    hM_dPhi_reco_altrange_data = GetHistogram(datahistfile, 'hM_dPhi_reco_altrange')
    hM_dR_reco_data = GetHistogram(datahistfile, 'hM_dR_reco')
    hM_Eta_reco_data = GetHistogram(datahistfile, 'hM_Eta_reco')
    hM_Phi_reco_data = GetHistogram(datahistfile, 'hM_Phi_reco')

    hM_NClusLayer1_sim = GetHistogram(simhistfile, 'hM_NClusLayer1')
    hM_NPrototkl_sim = GetHistogram(simhistfile, 'hM_NPrototkl')
    hM_NRecotkl_Raw_sim = GetHistogram(simhistfile, 'hM_NRecotkl_Raw')
    hM_dEta_reco_sim = GetHistogram(simhistfile, 'hM_dEta_reco')
    hM_dEta_reco_altrange_sim = GetHistogram(simhistfile, 'hM_dEta_reco_altrange')
    hM_dPhi_reco_sim = GetHistogram(simhistfile, 'hM_dPhi_reco')
    hM_dPhi_reco_altrange_sim = GetHistogram(simhistfile, 'hM_dPhi_reco_altrange')
    hM_dR_reco_sim = GetHistogram(simhistfile, 'hM_dR_reco')
    hM_Eta_reco_sim = GetHistogram(simhistfile, 'hM_Eta_reco')
    hM_Phi_reco_sim = GetHistogram(simhistfile, 'hM_Phi_reco')



    # Draw_1Dhist_datasimcomp(hdata, hsim, norm, logy, ymaxscale, XaxisName, Ytitle_unit, prelim, outname)
    hM_NClusLayer1_data.GetXaxis().SetMaxDigits(2)
    hM_NClusLayer1_sim.GetXaxis().SetMaxDigits(2)
    Draw_1Dhist_datasimcomp(hM_NClusLayer1_data, hM_NClusLayer1_sim, [0.1,0.08,0.15,0.13], 'data', True, 10, 'Number of clusters (Layer 3+4)', '', False, './DataSimComp/{}/NClusLayer1'.format(plotdir))
    hM_NPrototkl_data.GetXaxis().SetMaxDigits(2)
    hM_NPrototkl_sim.GetXaxis().SetMaxDigits(2)
    Draw_1Dhist_datasimcomp(hM_NPrototkl_data, hM_NPrototkl_sim, [0.1,0.08,0.15,0.13], 'data', True, 10, 'Number of proto-tracklets', '', False, './DataSimComp/{}/NProtoTracklets'.format(plotdir))
    hM_NRecotkl_Raw_data.GetXaxis().SetMaxDigits(2)
    hM_NRecotkl_Raw_sim.GetXaxis().SetMaxDigits(2)
    Draw_1Dhist_datasimcomp(hM_NRecotkl_Raw_data, hM_NRecotkl_Raw_sim, [0.1,0.08,0.15,0.13], 'data', True, 10, 'Number of reco-tracklets', '', False, './DataSimComp/{}/NRecoTracklets_Raw'.format(plotdir))
    Draw_1Dhist_datasimcomp(hM_dEta_reco_data, hM_dEta_reco_sim, [0.08,0.08,0.15,0.13], 'data', True, 50, 'Reco-tracklet #Delta#eta', '', False, './DataSimComp/{}/RecoTracklet_dEta'.format(plotdir))
    Draw_1Dhist_datasimcomp(hM_dEta_reco_altrange_data, hM_dEta_reco_altrange_sim, [0.08,0.08,0.15,0.13], 'data', True, 50, 'Reco-tracklet #Delta#eta', '', False, './DataSimComp/{}/RecoTracklet_dEta_altrange'.format(plotdir))
    Draw_1Dhist_datasimcomp(hM_dPhi_reco_data, hM_dPhi_reco_sim, [0.08,0.08,0.15,0.13], 'data', True, 50, 'Reco-tracklet #Delta#phi', '', False, './DataSimComp/{}/RecoTracklet_dPhi'.format(plotdir))
    hM_dPhi_reco_altrange_data.GetXaxis().SetMaxDigits(2)
    hM_dPhi_reco_altrange_sim.GetXaxis().SetMaxDigits(2)
    Draw_1Dhist_datasimcomp(hM_dPhi_reco_altrange_data, hM_dPhi_reco_altrange_sim, [0.1,0.08,0.15,0.13], 'data', True, 10, 'Reco-tracklet #Delta#phi', '', False, './DataSimComp/{}/RecoTracklet_dPhi_altrange'.format(plotdir))
    Draw_1Dhist_datasimcomp(hM_dR_reco_data, hM_dR_reco_sim, [0.08,0.08,0.15,0.13], 'data', True, 3, 'Reco-tracklet #DeltaR', '', False, './DataSimComp/{}/RecoTracklet_dR'.format(plotdir))



