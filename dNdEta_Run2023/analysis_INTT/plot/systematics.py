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
from plotUtil import GetHistogram

TickSize = 0.03
AxisTitleSize = 0.05
AxisLabelSize = 0.04
LeftMargin = 0.15
RightMargin = 0.08
TopMargin = 0.08
BottomMargin = 0.13

gROOT.SetBatch(True)

def getRelativeDiff(h_2diff, h_nominal):
    # check if the histograms have the same binning
    if h_2diff.GetNbinsX() != h_nominal.GetNbinsX():
        print('Error: histograms have different number of bins')
        return None

    h_reldiff = h_2diff.Clone('h_reldiff')
    h_reldiff.Reset()
    for i in range(1, h_2diff.GetNbinsX() + 1):
        absdiff = abs(h_2diff.GetBinContent(i) - h_nominal.GetBinContent(i))
        nominal = h_nominal.GetBinContent(i)
        reldiff = absdiff / nominal if nominal != 0 else 0
        h_reldiff.SetBinContent(i, reldiff)
    
    return h_reldiff
   

def getMaxRelDiff(l_hm_reldiff):
    hm_maxRelDiff = l_hm_reldiff[0].Clone('hm_maxRelDiff')
    hm_maxRelDiff.Reset()
    for hm_reldiff in l_hm_reldiff:
        for i in range(1, hm_reldiff.GetNbinsX() + 1):
            if hm_reldiff.GetBinContent(i) > hm_maxRelDiff.GetBinContent(i):
                hm_maxRelDiff.SetBinContent(i, hm_reldiff.GetBinContent(i))
    
    return hm_maxRelDiff


def getFinalUncertainty(l_hm):
    # sum in quadrature of the uncertainties
    hM_TotalRelUnc = l_hm[0].Clone('hM_TotalRelUnc')
    hM_TotalRelUnc.Reset()
    for i in range(1, hM_TotalRelUnc.GetNbinsX() + 1):
        totalUnc = 0
        for hm in l_hm:
            totalUnc += hm.GetBinContent(i)**2
            
        totalUnc = math.sqrt(totalUnc)
        hM_TotalRelUnc.SetBinContent(i, totalUnc)
    
    return hM_TotalRelUnc
    

if __name__ == '__main__':
    parser = OptionParser(usage='usage: %prog ver [options -h]')
    parser.add_option('--desc', dest='desc', type='string', default='Centrality0to70_Zvtxm30p0tom10p0_noasel', help='String for input file and output plot description (cuts, binnings, etc...)')
    
    (opt, args) = parser.parse_args()
    print('opt: {}'.format(opt))
    
    desc = opt.desc
    
    # deltaR selection - baseline 0.5, variation 0.4 and 0.6
    # cluster ADC cut - baseline 35, variation 50 and 65 -> maybe use 50 as baseline (?)
    # random noise cluster
    # Statistical uncertainties in the geometric correction and alpha corrections as propagated to the final dNdEta
    
    # desc = 'Centrality0to70_Zvtxm30p0tom10p0_noasel'
    os.makedirs('./systematics/{}'.format(desc), exist_ok=True)
    
    h1WEfinal_nominal = GetHistogram('/sphenix/user/hjheng/sPHENIXRepo/analysis/dNdEta_Run2023/analysis_INTT/plot/corrections/Data_Run20869_dRcut0p5_NominalVtxZ_RandomClusSet0_clusAdcCutSet0/{}/correction_hists.root'.format(desc), 'h1WEfinal')
    h1WEfinal_dRcut0p4 = GetHistogram('/sphenix/user/hjheng/sPHENIXRepo/analysis/dNdEta_Run2023/analysis_INTT/plot/corrections/Data_Run20869_dRcut0p4_NominalVtxZ_RandomClusSet0_clusAdcCutSet0/{}/correction_hists.root'.format(desc), 'h1WEfinal')
    h1WEfinal_dRcut0p6 = GetHistogram('/sphenix/user/hjheng/sPHENIXRepo/analysis/dNdEta_Run2023/analysis_INTT/plot/corrections/Data_Run20869_dRcut0p6_NominalVtxZ_RandomClusSet0_clusAdcCutSet0/{}/correction_hists.root'.format(desc), 'h1WEfinal')
    h1WEfinal_clusAdcCut50 = GetHistogram('/sphenix/user/hjheng/sPHENIXRepo/analysis/dNdEta_Run2023/analysis_INTT/plot/corrections/Data_Run20869_dRcut0p5_NominalVtxZ_RandomClusSet0_clusAdcCutSet1/{}/correction_hists.root'.format(desc), 'h1WEfinal')
    h1WEfinal_clusAdcCut65 = GetHistogram('/sphenix/user/hjheng/sPHENIXRepo/analysis/dNdEta_Run2023/analysis_INTT/plot/corrections/Data_Run20869_dRcut0p5_NominalVtxZ_RandomClusSet0_clusAdcCutSet2/{}/correction_hists.root'.format(desc), 'h1WEfinal')
    h1WEfinal_noise1perc = GetHistogram('/sphenix/user/hjheng/sPHENIXRepo/analysis/dNdEta_Run2023/analysis_INTT/plot/corrections/Data_Run20869_dRcut0p5_NominalVtxZ_RandomClusSet1_clusAdcCutSet0/{}/correction_hists.root'.format(desc), 'h1WEfinal')
    h1WEfinal_noise3perc = GetHistogram('/sphenix/user/hjheng/sPHENIXRepo/analysis/dNdEta_Run2023/analysis_INTT/plot/corrections/Data_Run20869_dRcut0p5_NominalVtxZ_RandomClusSet2_clusAdcCutSet0/{}/correction_hists.root'.format(desc), 'h1WEfinal')
    h1WEfinal_noise5perc = GetHistogram('/sphenix/user/hjheng/sPHENIXRepo/analysis/dNdEta_Run2023/analysis_INTT/plot/corrections/Data_Run20869_dRcut0p5_NominalVtxZ_RandomClusSet3_clusAdcCutSet0/{}/correction_hists.root'.format(desc), 'h1WEfinal')
    h1WEfinal_noise10perc = GetHistogram('/sphenix/user/hjheng/sPHENIXRepo/analysis/dNdEta_Run2023/analysis_INTT/plot/corrections/Data_Run20869_dRcut0p5_NominalVtxZ_RandomClusSet4_clusAdcCutSet0/{}/correction_hists.root'.format(desc), 'h1WEfinal')
    
    # get the uncertainty from the nominal 
    hM_statunc_nominal = h1WEfinal_nominal.Clone('hM_statunc_nominal')
    hM_statunc_nominal.Reset()
    for i in range(1, h1WEfinal_nominal.GetNbinsX() + 1):
        statunc = h1WEfinal_nominal.GetBinError(i) / h1WEfinal_nominal.GetBinContent(i) if h1WEfinal_nominal.GetBinContent(i) != 0 else 0
        hM_statunc_nominal.SetBinContent(i, statunc)
    
    # calculate the relative difference (absolute value of difference divided by the nominal value)
    hM_reldiff_h1WEfinal_dRcut0p4 = getRelativeDiff(h1WEfinal_dRcut0p4, h1WEfinal_nominal)
    hM_reldiff_h1WEfinal_dRcut0p6 = getRelativeDiff(h1WEfinal_dRcut0p6, h1WEfinal_nominal)
    hM_maxreldiff_dRcut = getMaxRelDiff([hM_reldiff_h1WEfinal_dRcut0p4, hM_reldiff_h1WEfinal_dRcut0p6])
    hM_reldiff_h1WEfinal_clusAdcCut50 = getRelativeDiff(h1WEfinal_clusAdcCut50, h1WEfinal_nominal)
    hM_reldiff_h1WEfinal_clusAdcCut65 = getRelativeDiff(h1WEfinal_clusAdcCut65, h1WEfinal_nominal)
    hM_maxreldiff_clusAdcCut = getMaxRelDiff([hM_reldiff_h1WEfinal_clusAdcCut50, hM_reldiff_h1WEfinal_clusAdcCut65])
    hM_reldiff_h1WEfinal_noise1perc = getRelativeDiff(h1WEfinal_noise1perc, h1WEfinal_nominal)
    hM_reldiff_h1WEfinal_noise3perc = getRelativeDiff(h1WEfinal_noise3perc, h1WEfinal_nominal)
    hM_reldiff_h1WEfinal_noise5perc = getRelativeDiff(h1WEfinal_noise5perc, h1WEfinal_nominal)
    hM_reldiff_h1WEfinal_noise10perc = getRelativeDiff(h1WEfinal_noise10perc, h1WEfinal_nominal)
    hM_maxreldiff_noise = getMaxRelDiff([hM_reldiff_h1WEfinal_noise1perc, hM_reldiff_h1WEfinal_noise3perc, hM_reldiff_h1WEfinal_noise5perc, hM_reldiff_h1WEfinal_noise10perc])
    
    hM_TotalRelUnc = getFinalUncertainty([hM_statunc_nominal, hM_maxreldiff_dRcut, hM_maxreldiff_clusAdcCut, hM_maxreldiff_noise])
    
    # get the absolute uncertainty 
    hM_final = h1WEfinal_nominal.Clone('hM_final')
    hM_TotalUnc = hM_TotalRelUnc.Clone('hM_TotalUnc')
    for i in range(1, hM_TotalUnc.GetNbinsX() + 1):
        hM_TotalUnc.SetBinContent(i, hM_TotalRelUnc.GetBinContent(i) * h1WEfinal_nominal.GetBinContent(i))
        hM_final.SetBinError(i, hM_TotalRelUnc.GetBinContent(i) * h1WEfinal_nominal.GetBinContent(i))
        
        
    # split the string by '_' and replace some characters
    descstr = desc.split('_')
    centstr = descstr[0]
    zvtxstr = descstr[1]
    aselstr = descstr[2]
    centstr = centstr.replace('Centrality', 'Centrality [')
    centstr = centstr.replace('to', '-')
    centstr = centstr + ']%'
    zvtxstr = zvtxstr.replace('m', '-')
    zvtxstr = zvtxstr.replace('p', '.')
    zvtxstr = zvtxstr.replace('to', ', ')
    zvtxstr = zvtxstr.replace('Zvtx', 'Z_{vtx} [')
    zvtxstr = zvtxstr + '] cm'
    
    aselstr = '' if aselstr == 'noasel' else ', (with additional selection)'
    
    legstr = centstr + ', ' + zvtxstr + aselstr
    
    # plot the relative difference
    c = TCanvas('c', 'c', 800, 700)
    c.cd()
    hM_TotalRelUnc.GetXaxis().SetTitle('#eta')
    hM_TotalRelUnc.GetYaxis().SetTitle('Relative uncertainty')
    hM_TotalRelUnc.SetLineColor(kBlack)
    hM_TotalRelUnc.SetLineWidth(2)
    hM_TotalRelUnc.GetYaxis().SetRangeUser(0, hM_TotalRelUnc.GetMaximum() * 1.8)
    hM_TotalRelUnc.Draw('hist')
    hM_statunc_nominal.SetMarkerColor(TColor.GetColor('#810000'))
    hM_statunc_nominal.SetLineColor(TColor.GetColor('#810000'))
    hM_statunc_nominal.SetLineWidth(2)
    hM_statunc_nominal.Draw('P same')
    hM_maxreldiff_dRcut.SetMarkerColor(TColor.GetColor('#0F4C75'))
    hM_maxreldiff_dRcut.SetLineColor(TColor.GetColor('#0F4C75'))
    hM_maxreldiff_dRcut.SetLineWidth(2)
    hM_maxreldiff_dRcut.Draw('P same')
    hM_maxreldiff_clusAdcCut.SetMarkerColor(TColor.GetColor('#5E8B7E'))
    hM_maxreldiff_clusAdcCut.SetLineColor(TColor.GetColor('#5E8B7E'))
    hM_maxreldiff_clusAdcCut.SetLineWidth(2)
    hM_maxreldiff_clusAdcCut.Draw('P same')
    hM_maxreldiff_noise.SetMarkerColor(TColor.GetColor('#e99960'))
    hM_maxreldiff_noise.SetLineColor(TColor.GetColor('#e99960'))
    hM_maxreldiff_noise.SetLineWidth(2)
    hM_maxreldiff_noise.Draw('P same')
    leg = TLegend(0.2, 0.68, 0.45, 0.91)
    leg.SetHeader(legstr)
    leg.SetTextSize(0.035)
    leg.SetBorderSize(0)
    leg.SetFillStyle(0)
    leg.AddEntry(hM_TotalRelUnc, 'Total uncertainty', 'l')
    leg.AddEntry(hM_statunc_nominal, 'Statistical uncertainty in geom. & #alpha corrections', 'p')
    leg.AddEntry(hM_maxreldiff_dRcut, 'Tracklet #DeltaR cut variation', 'p')
    leg.AddEntry(hM_maxreldiff_clusAdcCut, 'Cluster ADC cut variation', 'p')
    leg.AddEntry(hM_maxreldiff_noise, 'Random noise cluster variation', 'p')
    leg.Draw()
    c.RedrawAxis()
    c.Draw()
    c.SaveAs('./systematics/{}/hM_TotalRelUnc_{}.pdf'.format(desc, desc))
    c.SaveAs('./systematics/{}/hM_TotalRelUnc_{}.png'.format(desc, desc))
    if(c):
        c.Close()
        gSystem.ProcessEvents()
        del c
        c = 0
    
    # Root file with the histograms (with the final uncertainty)
    fout = TFile('./systematics/{}/finalhists_systematics_{}.root'.format(desc, desc), 'recreate')
    fout.cd()
    hM_statunc_nominal.Write()
    hM_maxreldiff_dRcut.Write()
    hM_maxreldiff_clusAdcCut.Write()
    hM_TotalRelUnc.Write()
    hM_TotalUnc.Write()
    hM_final.Write()
    fout.Close()
    
    
    