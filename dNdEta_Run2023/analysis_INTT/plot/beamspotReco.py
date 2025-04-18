#! /usr/bin/env python
from optparse import OptionParser
import sys
import os
import datetime
from array import *
from ROOT import TH1F, TH2F, TF1, TFile, TTree, TCanvas, TGraphErrors, TLatex, gROOT, gPad, kRed, kBlue, kGreen, kBlack, gSystem
import numpy as np
import math
import glob

LeftMargin = 0.15
RightMargin = 0.08
TopMargin = 0.08
BottomMargin = 0.13

# gROOT.LoadMacro('./sPHENIXStyle/sPhenixStyle.C')
# gROOT.ProcessLine('SetsPhenixStyle()')
gROOT.SetBatch(True)

# Function to draw TGraphErrors
def Draw_TGraphErrors(gr, xtitle, ytitle, plotname):
    
    data = True if 'BCO' in xtitle else False
    
    # Fit a straight line
    f = TF1('f', 'pol1', gr.GetPointX(0)*0.9997, gr.GetPointX(gr.GetN()-1)*1.0003)
    gr.Fit('f', 'R')
    
    # Get the maximal of the Y axis
    ymax = gr.GetY()[0]
    ymin = gr.GetY()[0]
    for i in range(gr.GetN()):
        if gr.GetY()[i]+gr.GetErrorY(i) > ymax:
            ymax = gr.GetY()[i]+gr.GetErrorY(i)
        if gr.GetY()[i]-gr.GetErrorY(i) < ymin:
            ymin = gr.GetY()[i]-gr.GetErrorY(i)
        
    c = TCanvas('c', 'c', 800, 600)
    gPad.SetRightMargin(0.1)
    c.cd()
    gr.GetXaxis().SetTitle(xtitle)
    gr.GetYaxis().SetTitle(ytitle)
    # Set the range of the Y axis
    w = 0.12 if data else 0.05
    gr.GetHistogram().SetMaximum(gr.GetMean(2)+w)
    gr.GetHistogram().SetMinimum(gr.GetMean(2)-w)
    # gr.GetHistogram().SetMaximum(gr.GetMean(2)+6*gr.GetRMS(2))
    # gr.GetHistogram().SetMinimum(gr.GetMean(2)-6*gr.GetRMS(2))
    gr.SetMarkerStyle(20)
    gr.SetMarkerSize(0.5)
    gr.Draw('AP')
    f.SetLineColor(kRed)
    f.Draw('same')
    # print the mean and standard deviation of Y on the plot
    text = TLatex()
    text.SetNDC()
    text.SetTextSize(0.035)
    text.DrawLatex(0.2, 0.88, 'Avg. = {:.4g}, stddev. = {:.4g} '.format(gr.GetMean(2), gr.GetRMS(2)))
    # print the fit function on the plot
    text.DrawLatex(0.2, 0.83, 'Fit function: y = {:.4g} + {:.4g}x'.format(f.GetParameter(0), f.GetParameter(1)))
    # print the chi2/ndf on the plot
    chi2ndf = f.GetChisquare()/f.GetNDF()
    text.DrawLatex(0.2, 0.78, '#chi^{2}/ndf'+' = {:.4g}'.format(chi2ndf))
    c.SaveAs('{}.png'.format(plotname))
    c.SaveAs('{}.pdf'.format(plotname))
    if(c):
        c.Close()
        gSystem.ProcessEvents()
        del c
        c = 0


if __name__ == '__main__':
    parser = OptionParser(usage="usage: %prog ver [options -h]")
    parser.add_option("-d", "--isdata", dest="isdata", action="store_true", default=False, help="Is data")
    parser.add_option("-i", "--infiledir", dest="infiledir", default='/sphenix/user/hjheng/sPHENIXRepo/analysis/dNdEta_Run2023/analysis_INTT/minitree/BeamspotMinitree_data_Run20869_HotDead_BCO_ADC_Survey', help="Input file directory")
    parser.add_option('-p', '--plotdir', dest='plotdir', type='string', default='Data_Run20869_HotDead_BCO_ADC_Survey', help='Plot directory')

    (opt, args) = parser.parse_args()
    print('opt: {}'.format(opt)) 
    
    isdata = opt.isdata
    infiledir = opt.infiledir
    plotdir = opt.plotdir
    
    if os.path.isfile("{}/minitree_merged.root".format(infiledir)):
        os.system("rm {}/minitree_merged.root".format(infiledir))
        os.system("hadd -f -j 20 {}/minitree_merged.root {}/minitree_0*.root".format(infiledir, infiledir))
    else:
        os.system("hadd -f -j 20 {}/minitree_merged.root {}/minitree_0*.root".format(infiledir, infiledir))
        
    os.makedirs('./BeamspotReco/{}'.format(plotdir), exist_ok=True)
    
    
    bs_x = np.array([])
    bs_y = np.array([])
    bs_x_err = np.array([])
    bs_y_err = np.array([])
    bs_bco_med = np.array([])
    
    f = TFile('{}/minitree_merged.root'.format(infiledir), 'r')
    tree = f.Get('reco_beamspot')
    for idx in range(tree.GetEntries()):
        tree.GetEntry(idx)
        print ('entry={}, x={}, y={}, intt_bco_median={}'.format(idx, tree.x, tree.y, tree.intt_bco_median))
        bs_x = np.append(bs_x, tree.x)
        bs_x_err = np.append(bs_x_err, tree.sigma_x)
        bs_y = np.append(bs_y, tree.y)
        bs_y_err = np.append(bs_y_err, tree.sigma_y)
        if isdata:
            bs_bco_med = np.append(bs_bco_med, tree.intt_bco_median)
        else:
            bs_bco_med = np.append(bs_bco_med, idx)
        
    # TGraphErrors for beamspot x
    gr_bs_x = TGraphErrors(len(bs_x), bs_bco_med, bs_x, np.zeros(len(bs_x)), bs_x_err)
    gr_bs_y = TGraphErrors(len(bs_y), bs_bco_med, bs_y, np.zeros(len(bs_y)), bs_y_err)
    
    Draw_TGraphErrors(gr_bs_x, ('INTT BCO' if isdata else 'Sub-sample index'), 'Beamspot x [cm]', './BeamspotReco/{}/beamspot_x'.format(plotdir))
    Draw_TGraphErrors(gr_bs_y, ('INTT BCO' if isdata else 'Sub-sample index'), 'Beamspot y [cm]', './BeamspotReco/{}/beamspot_y'.format(plotdir))
    
    f.Close()
    
    # A TTree to store the beamspot reco results: average x, y
    f_out = TFile('{}/beamspot_res.root'.format(infiledir), 'recreate')
    t_out = TTree('minitree', 'minitree')
    bs_x_mean = np.array([gr_bs_x.GetMean(2)], dtype='float64')
    bs_y_mean = np.array([gr_bs_y.GetMean(2)], dtype='float64')
    t_out.Branch('bs_x', bs_x_mean, 'bs_x/D')
    t_out.Branch('bs_y', bs_y_mean, 'bs_y/D')
    t_out.Fill()
    t_out.Write()
    f_out.Close()
    
    # intermediate plots
    f_check = TFile('{}/minitree_00000.root'.format(infiledir), 'r')
    d0phi0dist = f_check.Get('d0phi0dist')
    d0phi0dist_bkgrm = f_check.Get('d0phi0dist_bkgrm')
    d0phi0dist_bkgrm_prof = f_check.Get('d0phi0dist_bkgrm_prof')
    maxgraph_rmoutl = f_check.Get('maxgraph_rmoutl')
    
    c = TCanvas('c', 'c', 800, 700)
    gPad.SetRightMargin(0.12)
    c.cd()
    d0phi0dist.GetYaxis().SetTitle('DCA w.r.t origin [cm]')
    d0phi0dist.GetXaxis().SetTitle('#phi_{PCA} [rad]')
    d0phi0dist.Draw('colz')
    c.SaveAs('./BeamspotReco/{}/d0phi0dist.png'.format(plotdir))
    c.SaveAs('./BeamspotReco/{}/d0phi0dist.pdf'.format(plotdir))
    c.Clear()
    d0phi0dist_bkgrm.GetYaxis().SetTitle('DCA w.r.t origin [cm]')
    d0phi0dist_bkgrm.GetXaxis().SetTitle('#phi_{PCA} [rad]')
    d0phi0dist_bkgrm.Draw('colz')
    maxgraph_rmoutl.SetMarkerColorAlpha(kRed, 0.5)
    maxgraph_rmoutl.SetMarkerStyle(20)
    maxgraph_rmoutl.SetMarkerSize(0.6)
    maxgraph_rmoutl.SetLineColorAlpha(kRed, 0.5)
    maxgraph_rmoutl.Draw('p same')
    c.SaveAs('./BeamspotReco/{}/d0phi0dist_bkgrm_wGraphFit.png'.format(plotdir))
    c.SaveAs('./BeamspotReco/{}/d0phi0dist_bkgrm_wGraphFit.pdf'.format(plotdir))
    c.Clear()
    