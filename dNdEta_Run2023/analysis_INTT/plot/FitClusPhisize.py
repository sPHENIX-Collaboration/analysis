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

titlesize = 0.05
labelsize = 0.05
pad1scale = 1.05
pad2scale = 2.6

def fitfunction(fstr):
    if fstr == 'powerlaw':
        return '[0]*x^(-[1])', 'N#timesx^{-#lambda}'
    elif fstr == 'expo':
        return '[0]*exp(-x/[1])', 'N#timesexp(-x/#lambda)'
    elif fstr == 'pwexp':
        return '[0]*x^(-[1])*exp(-x/[2])', 'N#timesx^{-#alpha}#timesexp(-x/#lambda)'
    else:
        print('Invalid function. Returning powerlaw as default')
        return '[0]*x^{-[1]}', 'N#timesx^{-#lambda}'
    
def FillRandom(h, f, n):
    for i in range(n):
        h.Fill(f.GetRandom())
    return h

gROOT.SetBatch(True)

if __name__ == '__main__': 
    gStyle.SetPalette(kSolar)
    os.makedirs('./ClusPhiSize/', exist_ok=True)
    
    hM_ClusPhiSize_all_sim = GetHistogram('./hists/Sim_Ntuple_HIJING_ana443_20241030/Cluster/hists_merged.root', 'hM_ClusPhiSize_all')
    hM_ClusPhiSize_all_data = GetHistogram('./hists/Data_CombinedNtuple_Run54280_HotChannel_BCO/Cluster/hists_merged.root', 'hM_ClusPhiSize_all')
    # normalize the histograms
    hM_ClusPhiSize_all_sim.Scale(1/hM_ClusPhiSize_all_sim.Integral(-1,-1))
    hM_ClusPhiSize_all_data.Scale(1/hM_ClusPhiSize_all_data.Integral(-1,-1))
    maxbincontent = max(hM_ClusPhiSize_all_sim.GetMaximum(), hM_ClusPhiSize_all_data.GetMaximum())
    minbincontent = min(hM_ClusPhiSize_all_sim.GetMinimum(0), hM_ClusPhiSize_all_data.GetMinimum(0))
    
    fitstr_data, fitleg_data = fitfunction('pwexp')
    f_data = TF1('f_data', fitstr_data, 1, 100)
    f_data.SetParameter(0, 2.3)
    f_data.SetParameter(1, 3.1)
    f_data.SetParameter(2, 15)
    # set parameter range for the fit
    # f_data.SetParLimits(0, 0, maxbincontent*10)
    # f_data.SetParLimits(1, 1E-6, 10)
    # f_data.SetParLimits(2, 10, 100)
    f_data.SetParNames('N', '#alpha', '#lambda')
    f_data.SetLineColorAlpha(kRed+1,0.5)
    f_data.Draw()
    # hM_ClusPhiSize_all_data.Fit('f_data', 'R')
    # print chi2/ndf
    # print('Chi2/ndf for data fit: {}'.format(f_data.GetChisquare()/f_data.GetNDF()))
    
    fitstr_sim, fitleg_sim = fitfunction('pwexp')
    f_sim = TF1('f_sim', fitstr_sim, 1, 100)
    f_sim.SetParameter(0, 2.9)
    f_sim.SetParameter(1, 2.5)
    f_sim.SetParameter(2, 4.5)
    # set parameter range for the fit
    # f_sim.SetParLimits(0, 0, maxbincontent*10)
    # f_sim.SetParLimits(1, 1E-6, 50)
    # f_sim.SetParLimits(2, 1E-6, 10)
    f_sim.SetParNames('N', '#alpha', '#lambda')
    f_sim.SetLineColorAlpha(kTMutCyan,0.5)
    f_sim.Draw()
    # hM_ClusPhiSize_all_sim.Fit('f_sim', 'R')
    # print chi2/ndf
    # print('Chi2/ndf for sim fit: {}'.format(f_sim.GetChisquare()/f_sim.GetNDF()))
    
    # produce histogram with random numbers from the fit function with various parameters
    ref_alpha = 3.0 # for fixed alpha
    ref_lambda = 4.5 # for fixed lambda
    nentires = 10000000
    nhists = 6
    l_alpha = [1.+0.5*i for i in range(nhists)]
    l_lambda = [4.+2*i for i in range(nhists)]
    l_hist_fixed_alpha = []
    l_hist_fixed_lambda = []
    for i in range(nhists):
        f_alpha = TF1('f_alpha', fitstr_sim, 1, 100)
        f_alpha.SetParameter(0, 1)
        f_alpha.SetParameter(1, l_alpha[i])
        f_alpha.SetParameter(2, ref_lambda)
        h_alpha = TH1F('h_alpha', 'h_alpha', 100, 1, 100)
        h_alpha = FillRandom(h_alpha, f_alpha, nentires)
        # normalize the histograms to 1 
        h_alpha.Scale(1/h_alpha.Integral(-1,-1))
        l_hist_fixed_alpha.append(h_alpha)
        
        f_lambda = TF1('f_lambda', fitstr_sim, 1, 100)
        f_lambda.SetParameter(0, 1)
        f_lambda.SetParameter(1, ref_alpha)
        f_lambda.SetParameter(2, l_lambda[i])
        h_lambda = TH1F('h_lambda', 'h_lambda', 100, 1, 100)
        h_lambda = FillRandom(h_lambda, f_lambda, nentires)
        # normalize the histograms to 1
        h_lambda.Scale(1/h_lambda.Integral(-1,-1))
        l_hist_fixed_lambda.append(h_lambda)
    
    c = TCanvas('c','c',800,600)
    c.SetLogy()
    hM_ClusPhiSize_all_data.SetLineColor(kBlack)
    hM_ClusPhiSize_all_data.SetLineWidth(2)
    hM_ClusPhiSize_all_data.SetMarkerStyle(20)
    hM_ClusPhiSize_all_data.SetMarkerSize(0.8)
    hM_ClusPhiSize_all_data.GetYaxis().SetTitle('Normalized Counts')
    hM_ClusPhiSize_all_data.GetYaxis().SetTitleOffset(1.5)
    hM_ClusPhiSize_all_data.GetYaxis().SetRangeUser(minbincontent*0.5, maxbincontent*10)
    hM_ClusPhiSize_all_data.GetYaxis().SetTitleSize(titlesize)
    hM_ClusPhiSize_all_data.GetXaxis().SetTitle('Cluster #phi size')
    hM_ClusPhiSize_all_data.GetXaxis().SetRangeUser(1, 100)
    hM_ClusPhiSize_all_data.GetXaxis().SetTitleSize(titlesize)
    hM_ClusPhiSize_all_data.Draw('PE1')
    hM_ClusPhiSize_all_sim.SetLineColor(kTVibBlue)
    hM_ClusPhiSize_all_sim.SetFillColorAlpha(kTVibBlue, 0.1)
    hM_ClusPhiSize_all_sim.SetLineWidth(2)
    hM_ClusPhiSize_all_sim.Draw('hist same')
    f_data.Draw('same')
    f_sim.Draw('same')
    c.RedrawAxis()
    leg = TLegend(0.5, 0.6, 0.9, 0.9)
    leg.AddEntry(hM_ClusPhiSize_all_data, 'Data', 'PE')
    leg.AddEntry(hM_ClusPhiSize_all_sim, 'Simulation', 'lf')
    # leg.AddEntry(f_data, 'Function: ' + fitleg_data, 'l')
    # for i in range(f_data.GetNpar()):
    #     leg.AddEntry(0, f_data.GetParName(i) + ': {:.3e}'.format(f_data.GetParameter(i)) + ' #pm {:.3e}'.format(f_data.GetParError(i)), '')
    # leg.AddEntry(f_sim, 'Simulation Fit: ' + fitleg_sim, 'l')
    # for i in range(f_sim.GetNpar()):
    #     leg.AddEntry(0, f_sim.GetParName(i) + ': {:.3e}'.format(f_sim.GetParameter(i)) + ' #pm {:.3e}'.format(f_sim.GetParError(i)), '')
    leg.AddEntry(f_data, 'Function:' + fitleg_data, 'l')
    leg.AddEntry(0, f_data.GetParName(1) + '={:.1f}'.format(f_data.GetParameter(1)) + ', ' + f_data.GetParName(2) + '={:.1f}'.format(f_data.GetParameter(2)), '')
    leg.AddEntry(f_sim, 'Function:' + fitleg_sim, 'l')
    leg.AddEntry(0, f_sim.GetParName(1) + '={:.1f}'.format(f_sim.GetParameter(1)) + ', ' + f_sim.GetParName(2) + '={:.1f}'.format(f_sim.GetParameter(2)), '')
    leg.SetBorderSize(0)
    leg.SetFillStyle(0)
    leg.SetTextSize(0.035)
    leg.Draw()
    c.SaveAs('./ClusPhiSize/ClusPhiSize_fit.pdf')
    c.SaveAs('./ClusPhiSize/ClusPhiSize_fit.png')
    
    # plot the histograms with fixed alpha 
    c.Clear()
    c.SetLogy()
    hM_ClusPhiSize_all_data.GetYaxis().SetRangeUser(minbincontent*0.5, maxbincontent*100)
    hM_ClusPhiSize_all_data.Draw('PE1')
    hM_ClusPhiSize_all_sim.Draw('hist same')
    leg_2 = TLegend(0.3, 0.83, 0.6, 0.9)
    leg_2.SetNColumns(2)
    leg_2.AddEntry(hM_ClusPhiSize_all_data, 'Data', 'PE')
    leg_2.AddEntry(hM_ClusPhiSize_all_sim, 'Simulation', 'lf')
    leg_2.SetBorderSize(0)
    leg_2.SetFillStyle(0)
    leg_2.SetTextSize(0.04)
    leg_2.Draw()
    leg_alpha = TLegend(0.3, 0.66, 0.6, 0.82)
    leg_alpha.SetHeader('Toy model N#timesx^{-#alpha}#timesexp(-x/#lambda), fixed'+ ' #lambda={:.1f}'.format(ref_lambda) + ', varying #alpha')
    leg_alpha.SetNColumns(2)
    for i in range(nhists):
        # l_hist_fixed_alpha[i].SetLineColorAlpha(kRed+1+2*i, 0.5)
        l_hist_fixed_alpha[i].Draw('same hist PLC')
        leg_alpha.AddEntry(l_hist_fixed_alpha[i], '#alpha={:.1f}'.format(l_alpha[i]), 'l')
    
    leg_alpha.SetBorderSize(0)
    leg_alpha.SetFillStyle(0)
    leg_alpha.SetTextSize(0.04)
    leg_alpha.Draw()
    c.SaveAs('./ClusPhiSize/ClusPhiSize_toys_fixed_alpha.pdf')
    c.SaveAs('./ClusPhiSize/ClusPhiSize_toys_fixed_alpha.png')
    
    c.Clear()
    c.SetLogy()
    hM_ClusPhiSize_all_data.GetYaxis().SetRangeUser(minbincontent*0.5, maxbincontent*100)
    hM_ClusPhiSize_all_data.Draw('PE1')
    hM_ClusPhiSize_all_sim.Draw('hist same')
    leg_2.Draw()
    leg_lambda = TLegend(0.3, 0.66, 0.6, 0.82)
    leg_lambda.SetHeader('Toy model N#timesx^{-#alpha}#timesexp(-x/#lambda), fixed' + ' #alpha={:.1f}'.format(ref_alpha) + ', varying #lambda')
    leg_lambda.SetNColumns(2)
    for i in range(nhists):
        # l_hist_fixed_lambda[i].SetLineColorAlpha(kRed+1+2*i, 0.5)
        l_hist_fixed_lambda[i].Draw('same hist PLC')
        leg_lambda.AddEntry(l_hist_fixed_lambda[i], '#lambda={:.1f}'.format(l_lambda[i]), 'l')
        
    leg_lambda.SetBorderSize(0)
    leg_lambda.SetFillStyle(0)
    leg_lambda.SetTextSize(0.04)
    leg_lambda.Draw()
    c.SaveAs('./ClusPhiSize/ClusPhiSize_toys_fixed_lambda.pdf')
    c.SaveAs('./ClusPhiSize/ClusPhiSize_toys_fixed_lambda.png')