#! /usr/bin/env python
from optparse import OptionParser
import sys
import os
import datetime
from array import *
import ROOT
import numpy as np
import pandas as pd
import math
import glob
import ctypes
from plotUtil import Draw_2Dhist
from sigmaEff import minimum_size_range

ROOT.gROOT.LoadMacro('./sPHENIXStyle/sPhenixStyle.C')
ROOT.gROOT.ProcessLine('SetsPhenixStyle()')
ROOT.gROOT.SetBatch(True)

TickSize = 0.03
AxisTitleSize = 0.05
AxisLabelSize = 0.04
LeftMargin = 0.15
RightMargin = 0.08
TopMargin = 0.08
BottomMargin = 0.13

global nbinsdphi, nbinsdca 
global dphiinterval, dcainterval

def Draw_1Dhist_wTF1(hist, histdata, norm1, logy, ymaxscale, XaxisName, Ytitle_unit, outname):
    xmin, xmax = minimum_size_range(histdata, 68.5)
    effsigma = xmax - xmin

    hist.Sumw2()
    binwidth = hist.GetXaxis().GetBinWidth(1)
    histmax = hist.GetMaximum()
    c = ROOT.TCanvas('c', 'c', 800, 700)
    if norm1:
        hist.Scale(1. / hist.Integral(-1, -1))
    if logy:
        c.SetLogy()
    c.cd()
    ROOT.gPad.SetRightMargin(RightMargin)
    ROOT.gPad.SetTopMargin(TopMargin)
    ROOT.gPad.SetLeftMargin(LeftMargin)
    ROOT.gPad.SetBottomMargin(BottomMargin)
    if norm1:
        if Ytitle_unit == '':
            hist.GetYaxis().SetTitle(
                'Normalized entries / ({:g})'.format(binwidth))
        else:
            hist.GetYaxis().SetTitle(
                'Normalized entries / ({:g} {unit})'.format(binwidth, unit=Ytitle_unit))
    else:
        if Ytitle_unit == '':
            hist.GetYaxis().SetTitle('Entries / ({:g})'.format(binwidth))
        else:
            hist.GetYaxis().SetTitle(
                'Entries / ({:g} {unit})'.format(binwidth, unit=Ytitle_unit))

    # hist.GetXaxis().SetRangeUser(hist.GetBinLowEdge(1)-binwidth, hist.GetBinLowEdge(hist.GetNbinsX())+2*binwidth)
    if logy:
        hist.GetYaxis().SetRangeUser(hist.GetMinimum(0)*0.5, histmax * ymaxscale)
    else:
        hist.GetYaxis().SetRangeUser(0., histmax * ymaxscale)
    hist.GetXaxis().SetTitle(XaxisName)
    hist.GetXaxis().SetTickSize(TickSize)
    hist.GetXaxis().SetTitleSize(AxisTitleSize)
    hist.GetXaxis().SetLabelSize(AxisLabelSize)
    hist.GetYaxis().SetTickSize(TickSize)
    hist.GetYaxis().SetTitleSize(AxisTitleSize)
    hist.GetYaxis().SetLabelSize(AxisLabelSize)
    hist.GetXaxis().SetTitleOffset(1.1)
    hist.GetYaxis().SetTitleOffset(1.35)
    hist.SetLineColor(1)
    hist.SetLineWidth(2)
    hist.Draw('hist')
    # Gaussian fit
    # maxbincenter = hist.GetBinCenter(hist.GetMaximumBin())
    # f1 = ROOT.TF1('f1', 'gaus', maxbincenter - hist.GetStdDev(), maxbincenter + hist.GetStdDev())
    # f1.SetParameter(1,hist.GetMean())
    # f1.SetParLimits(1,-1,1)
    # f1.SetParameter(2,hist.GetStdDev())
    # f1.SetParLimits(2,0,5)
    # f1.SetLineColorAlpha(ROOT.TColor.GetColor('#F54748'), 0.9)
    # hist.Fit(f1,'NQ')
    # f1.Draw('same')
    # Draw lines
    l1 = ROOT.TLine(xmin, 0, xmin, histmax)
    l1.SetLineColor(ROOT.kRed)
    l1.SetLineStyle(ROOT.kDashed)
    l1.SetLineWidth(2)
    l1.Draw()
    l2 = ROOT.TLine(xmax, 0, xmax, histmax)
    l2.SetLineColor(ROOT.kRed)
    l2.SetLineStyle(ROOT.kDashed)
    l2.SetLineWidth(2)
    l2.Draw()
    leg = ROOT.TLegend((1-RightMargin)-0.45, (1-TopMargin)-0.1,
                  (1-RightMargin)-0.1, (1-TopMargin)-0.03)
    leg.SetTextSize(0.045)
    leg.SetFillStyle(0)
    leg.AddEntry('', '#it{#bf{sPHENIX}} Simulation', '')
    # leg.AddEntry('','Au+Au #sqrt{s_{NN}}=200 GeV','')
    leg.Draw()
    leg2 = ROOT.TLegend(0.45, 0.67, 0.88, 0.8)
    leg2.SetTextSize(0.033)
    leg2.SetFillStyle(0)
    # leg2.AddEntry(f1,'Gaussian fit','l')
    # leg2.AddEntry('', '#mu={0:.2e}#pm{1:.2e}'.format(f1.GetParameter(1), f1.GetParError(1)), '')
    # leg2.AddEntry('', '#sigma={0:.2e}#pm{1:.2e}'.format(f1.GetParameter(2), f1.GetParError(2)), '')
    leg2.AddEntry('', '#LT\Deltaz#GT={0:.2e}#pm{1:.2e} {2}'.format(hist.GetMean(), hist.GetMeanError(), Ytitle_unit), '')
    # leg2.AddEntry('', '#sigma={0:.2e}#pm{1:.2e}'.format(hist.GetStdDev(), hist.GetStdDevError()), '')
    leg2.AddEntry('', '#sigma_{{eff}} (68.5%) ={0:.4g} {1}'.format(effsigma, Ytitle_unit), '')
    leg2.Draw()
    c.RedrawAxis()
    c.Draw()
    c.SaveAs(outname+'.pdf')
    c.SaveAs(outname+'.png')
    if(c):
        c.Close()
        ROOT.gSystem.ProcessEvents()
        del c
        c = 0
    # return f1.GetParameter(1), f1.GetParError(1), f1.GetParameter(2), f1.GetParError(2)
    # return hist.GetMean(), hist.GetMeanError(), hist.GetStdDev(), hist.GetStdDevError()
        return hist.GetMean(), hist.GetMeanError(), effsigma, 0.0


def Draw2Dhisttable(hist, XaxisName, YaxisName, ZaxisName, DrawOpt, outname):
    c = ROOT.TCanvas('c', 'c', 4000, 3700)
    c.cd()
    ROOT.gPad.SetRightMargin(0.185)
    ROOT.gPad.SetTopMargin(TopMargin)
    ROOT.gPad.SetLeftMargin(LeftMargin)
    ROOT.gPad.SetBottomMargin(BottomMargin)
    ROOT.gStyle.SetPaintTextFormat('1.5f')
    hist.SetMarkerSize(0.4)
    hist.GetXaxis().SetTitle(XaxisName)
    hist.GetYaxis().SetTitle(YaxisName)
    hist.GetZaxis().SetTitle(ZaxisName)
    hist.GetXaxis().SetTickSize(TickSize)
    hist.GetYaxis().SetTickSize(TickSize)
    hist.GetXaxis().SetTitleSize(AxisTitleSize)
    hist.GetYaxis().SetTitleSize(AxisTitleSize)
    hist.GetZaxis().SetTitleSize(AxisTitleSize)
    hist.GetXaxis().SetLabelSize(AxisLabelSize)
    hist.GetYaxis().SetLabelSize(AxisLabelSize)
    hist.GetXaxis().SetTitleOffset(1.1)
    hist.GetYaxis().SetTitleOffset(1.3)
    hist.GetZaxis().SetTitleOffset(1.3)
    hist.GetZaxis().SetLabelSize(AxisLabelSize)
    hist.GetZaxis().SetRangeUser(hist.GetMinimum(), hist.GetMaximum())
    hist.LabelsOption("v")
    hist.SetContour(10000)
    hist.Draw(DrawOpt)
    # bx,by,bz = (ctypes.c_int(),ctypes.c_int(),ctypes.c_int()) # Ref: https://github.com/svenkreiss/decouple/blob/master/Decouple/plot_utils.py#L44-L52
    # hist.GetBinXYZ(hist.GetMinimumBin(),bx,by,bz)
    # binxCenter = hist.GetXaxis().GetBinCenter(bx.value)
    # binyCenter = hist.GetYaxis().GetBinCenter(by.value)
    # el = ROOT.TEllipse(binxCenter,binyCenter,.1,.1)
    # el.SetFillColor(2)
    # el.Draw()
    c.RedrawAxis()
    c.Draw()
    c.SaveAs(outname+'.pdf')
    c.SaveAs(outname+'.png')
    if(c):
        c.Close()
        ROOT.gSystem.ProcessEvents()
        del c
        c = 0

def doFitSaveFitresult(reshist, resdata, fitresname, histwTF1name):
    print ('In doFitSaveFitresult()')
    out = ROOT.TFile(fitresname, 'recreate') 
    tree = ROOT.TTree('tree', 'tree')
    arrmean = array('f', [0])
    arrmeanerr = array('f', [0])
    arrsigma = array('f', [0])
    arrsigmaerr = array('f', [0])
    tree.Branch('mean', arrmean, 'mean/F') 
    tree.Branch('mean_err', arrmeanerr, 'mean_err/F')
    tree.Branch('sigma', arrsigma, 'sigma/F') 
    tree.Branch('sigma_err', arrsigmaerr, 'sigma_err/F')
    arrmean[0], arrmeanerr[0], arrsigma[0], arrsigmaerr[0] = Draw_1Dhist_wTF1(reshist, resdata, False, True, 50, '#Deltaz(PV_{Truth}, PV_{Reco}) [cm]', 'cm', histwTF1name)
    tree.Fill()
    tree.Write('', ROOT.TObject.kOverwrite) 
    out.Close()
    del reshist, out, tree, arrmean, arrmeanerr, arrsigma, arrsigmaerr

def main(dphitxt, dcatxt, infiledir, outfiledir):
    print ('In main()')

    hM_DiffVtxZ = ROOT.TH1F('hM_DiffVtxZ', 'hM_DiffVtxZ_altrange', 200, -5, 5)
    hM_DiffVtxZ_NClusLayer1 = ROOT.TH2F('hM_DiffVtxZ_NClusLayer1', 'hM_DiffVtxZ_NClusLayer1', 200, -5, 5, 200, 0, 4000)

    df = ROOT.RDataFrame('minitree', '{}/dphi{}deg_dca{}cm/INTTVtxZ.root'.format(infiledir, dphitxt, dcatxt))
    data = df.AsNumpy(columns=['NTruthVtx','PV_z','TruthPV_z','NClusLayer1'])

    for i, ntruthvtx in enumerate(data['NTruthVtx']):
        if ntruthvtx == 1:
            hM_DiffVtxZ.Fill((data['PV_z'][i] - data['TruthPV_z'][i]))
            hM_DiffVtxZ_NClusLayer1.Fill((data['PV_z'][i] - data['TruthPV_z'][i]), data['NClusLayer1'][i])

    diffarray = data['PV_z'] - data['TruthPV_z']
    doFitSaveFitresult(hM_DiffVtxZ, diffarray, outfiledir+'fitresult_dPhi{}deg_dca{}cm.root'.format(dphitxt, dcatxt), outfiledir+'DiffVtxZ_wTF1')
    Draw_2Dhist(hM_DiffVtxZ_NClusLayer1, False, False, False, 0.13, '#Deltaz(PV_{Truth}, PV_{Reco}) [cm]', 'Number of clusters (inner)', 'colz', outfiledir+'DiffVtxZ_NClusLayer1_dphi{}deg_dca{}cm'.format(dphitxt, dcatxt))

    del df, data, hM_DiffVtxZ, hM_DiffVtxZ_NClusLayer1
    
def get2DTable(fitresdir):
    print('In get2DTable')
    hM_mean_dPhidcaCut = ROOT.TH2F('hM_mean_dPhidcaCut', 'hM_mean_dPhidcaCut', nbinsdphi, 0, nbinsdphi, nbinsdca, 0, nbinsdca)
    hM_sigma_dPhidcaCut = ROOT.TH2F('hM_sigma_dPhidcaCut', 'hM_sigma_dPhidcaCut', nbinsdphi, 0, nbinsdphi, nbinsdca, 0, nbinsdca)

    l_sigma = []
    l_mean = []
    l_dPhi = []
    l_dca = []
    for i in range(0, nbinsdphi):
        for j in range(0, nbinsdca):
            dphicut_deg = dphiinterval * (i+1)
            dcacut = dcainterval * (j+1)
            dphitext = '{:.3f}'.format(dphicut_deg).replace('.', 'p')
            dcadtext = '{:.3f}'.format(dcacut).replace('.', 'p')
            try:
                fitrs = ROOT.RDataFrame('tree', '{}/dPhi{}deg_dca{}cm/fitresult_dPhi{}deg_dca{}cm.root'.format(fitresdir, dphitext, dcadtext, dphitext, dcadtext))
                res = fitrs.AsNumpy(columns=['mean','sigma'])
                l_dPhi.append(dphicut_deg)
                l_dca.append(dcacut)
                l_mean.append(res['mean'][0])
                l_sigma.append(res['sigma'][0])
                hM_mean_dPhidcaCut.SetBinContent(i + 1, j + 1, res['mean'][0])
                hM_sigma_dPhidcaCut.SetBinContent(i + 1, j + 1, res['sigma'][0])
                hM_mean_dPhidcaCut.GetXaxis().SetBinLabel(i+1, '{:.2f}'.format((i+1)*dphiinterval))
                hM_mean_dPhidcaCut.GetYaxis().SetBinLabel(j+1, '{:.3f}'.format((j+1)*dcainterval))
                hM_sigma_dPhidcaCut.GetXaxis().SetBinLabel(i+1, '{:.2f}'.format((i+1)*dphiinterval))
                hM_sigma_dPhidcaCut.GetYaxis().SetBinLabel(j+1, '{:.3f}'.format((j+1)*dcainterval))
                del fitrs, res

            except Exception as e:
                print(e)

    df_opt = pd.DataFrame(list(zip(l_dPhi, l_dca, l_mean, l_sigma)), columns =['dPhiBin', 'dZBin', 'mean', 'sigma'])
    df_opt = df_opt.sort_values(by='sigma', ascending=True, ignore_index=True)        

    del l_dPhi, l_dca, l_mean, l_sigma
    return hM_mean_dPhidcaCut, hM_sigma_dPhidcaCut, df_opt

if __name__ == '__main__':
    parser = OptionParser(usage="usage: %prog ver [options -h]")
    parser.add_option("-i", "--infiledir", dest="infiledir", default='/sphenix/user/hjheng/TrackletAna/minitree/INTT/VtxEvtMap_ana382_zvtx-20cm_dummyAlignParams', help="Input file directory")
    parser.add_option("-o", "--outdirprefix", dest="outdirprefix", default='ana382_zvtx-20cm_dummyAlignParams', help="Output file directory")
    parser.add_option("-f", "--dofit", action="store_true", dest="dofit", default=False, help="Do fit")
    parser.add_option("-p", "--nbinsdphi", dest="nbinsdphi", default=20, help="Number of bins in dPhi")
    parser.add_option("-d", "--nbinsdca", dest="nbinsdca", default=20, help="Number of bins in dca")

    (opt, args) = parser.parse_args()
    print('opt: {}'.format(opt)) 

    infiledir = opt.infiledir
    outdirprefix = opt.outdirprefix
    dofit = opt.dofit
    nbinsdphi = int(opt.nbinsdphi)
    nbinsdca = int(opt.nbinsdca)

    plotpath = './RecoPV_optimization/'
    os.makedirs('{}/{}'.format(plotpath,outdirprefix), exist_ok=True)

    dphiinterval = 0.1
    dcainterval = 0.025

    for i in range(0, nbinsdphi):
        for j in range(0, nbinsdca):
            dphicut_deg = dphiinterval * (i+1)
            dphicut_rad = dphicut_deg * (np.pi / 180.)
            dcacut = dcainterval * (j+1)
            dphitext = '{:.3f}'.format(dphicut_deg).replace('.', 'p')
            dcadtext = '{:.3f}'.format(dcacut).replace('.', 'p')

            outpath = './RecoPV_optimization/{}/dPhi{}deg_dca{}cm/'.format(outdirprefix,dphitext,dcadtext)
            os.makedirs(outpath, exist_ok=True)
            try:
                if dofit == True:
                    main(dphitext,dcadtext,infiledir,outpath)
            except Exception as e:
                print(e)

    hM_mean_dPhidcaCut, hM_sigma_dPhidcaCut, df_opt = get2DTable('./RecoPV_optimization/{}'.format(outdirprefix))

    ROOT.gStyle.SetPalette(ROOT.kThermometer)
    ROOT.TGaxis.SetMaxDigits(3)
    # set the number of divisions to show the bin labels 
    # hM_mean_dPhidcaCut.GetYaxis().SetMaxDigits(4)
    # hM_sigma_dPhidcaCut.GetYaxis().SetMaxDigits(4)
    # hM_mean_dPhidcaCut.GetYaxis().SetDecimals()
    # hM_sigma_dPhidcaCut.GetYaxis().SetDecimals()
    
    Draw2Dhisttable(hM_mean_dPhidcaCut, '#Delta#phi [degree]', 'DCA [cm]', '#LT\Deltaz#GT [cm]', 'colztext45', '{}/{}/Optimization_GaussianMean_Table'.format(plotpath,outdirprefix))
    Draw2Dhisttable(hM_sigma_dPhidcaCut, '#Delta#phi [degree]', 'DCA [cm]', '#sigma_{eff} (68.5%) [cm]', 'colztext45', '{}/{}/Optimization_GaussianSigma_Table'.format(plotpath,outdirprefix))

    for i in range(5):
        sigmainfo = df_opt.iloc[i]
        print(sigmainfo)

    