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

def Draw_2Dhist(hist, IsData, logz, norm1, rmargin, XaxisName, YaxisName, ZaxisName, drawopt, outname):
    c = TCanvas('c', 'c', 1000, 700)
    if logz:
        c.SetLogz()
    c.cd()
    if ZaxisName == '':
        gPad.SetRightMargin(rmargin)
    else:
        gPad.SetRightMargin(rmargin+0.03)

    gPad.SetTopMargin(TopMargin)
    gPad.SetLeftMargin(LeftMargin)
    gPad.SetBottomMargin(BottomMargin)
    if norm1:
        hist.Scale(1. / hist.Integral(-1, -1, -1, -1))
    hist.GetXaxis().SetTitle(XaxisName)
    hist.GetYaxis().SetTitle(YaxisName)
    hist.GetXaxis().SetTickSize(TickSize)
    hist.GetYaxis().SetTickSize(TickSize)
    hist.GetXaxis().SetTitleSize(AxisTitleSize)
    hist.GetYaxis().SetTitleSize(AxisTitleSize)
    hist.GetXaxis().SetLabelSize(AxisLabelSize)
    hist.GetYaxis().SetLabelSize(AxisLabelSize)
    if ZaxisName != '':
        hist.GetZaxis().SetTitle(ZaxisName)
        hist.GetZaxis().SetTitleSize(AxisTitleSize)
        hist.GetZaxis().SetTitleOffset(1.1)
        
    hist.GetXaxis().SetTitleOffset(1.1)
    hist.GetYaxis().SetTitleOffset(1.3)
    hist.GetZaxis().SetLabelSize(AxisLabelSize)
    hist.GetZaxis().SetRangeUser(hist.GetMinimum(0), hist.GetMaximum())
    hist.SetContour(1000)
    hist.Draw(drawopt)

    c.RedrawAxis()
    c.Draw()
    c.SaveAs(outname+'.pdf')
    c.SaveAs(outname+'.png')
    if(c):
        c.Close()
        gSystem.ProcessEvents()
        del c
        c = 0

# gROOT.LoadMacro('./sPHENIXStyle/sPhenixStyle.C')
# gROOT.ProcessLine('SetsPhenixStyle()')
gROOT.SetBatch(True)

if __name__ == '__main__':
    parser = OptionParser()
    parser.add_option("-f", "--histdir", dest="histdir", type="string", default='/sphenix/user/hjheng/TrackletAna/analysis_INTT/plot/hists/data_Run20869/Cluster', help="Input file name")
    parser.add_option("-d", "--plotdir", dest="plotdir", type="string", default='/sphenix/user/hjheng/TrackletAna/analysis_INTT/plot/RecoCluster/data_Run20869', help="Plot directory")
    parser.add_option("-s", "--isdata", dest="isdata", action="store_true", default=False, help="Is data")
    parser.add_option("-p", "--preliminary", dest="preliminary", action="store_true", default=False, help="Preliminary")
    (opt, args) = parser.parse_args()
    print('opt: {}'.format(opt))

    histdir = opt.histdir
    plotdir = opt.plotdir
    isdata = opt.isdata
    preliminary = opt.preliminary
    os.makedirs(plotdir, exist_ok=True)

    if os.path.isfile("{}/hists_merged.root".format(histdir)):
        os.system("rm {}/hists_merged.root".format(histdir))
        os.system("hadd {}/hists_merged.root {}/hists_*.root".format(histdir, histdir))
    else:
        os.system("hadd {}/hists_merged.root {}/hists_*.root".format(histdir, histdir))

    hM_hitmap_RowColumnFlatten_unwei = GetHistogram('{}/hists_merged.root'.format(histdir), 'hM_hitmap_RowColumnFlatten_unwei')
    hM_hitmap_RowColumnFlatten_weiADC = GetHistogram('{}/hists_merged.root'.format(histdir), 'hM_hitmap_RowColumnFlatten_weiADC')
    hM_hitmap_RowColumnFlatten_avgtrkrhitadc = GetHistogram('{}/hists_merged.root'.format(histdir), 'hM_hitmap_RowColumnFlatten_weiADC')
    

    # Draw_2Dhist(hist, IsData, logz, norm1, rmargin, XaxisName, YaxisName, drawopt, outname)
    gStyle.SetPalette(kRainBow)
    Draw_2Dhist(hM_hitmap_RowColumnFlatten_unwei, isdata, False, False, 0.15, '(z and layer) index', 'phi index', 'Entries', 'colz', '{}/TrkrHitRowColumnFlatten_unwei'.format(plotdir))
    Draw_2Dhist(hM_hitmap_RowColumnFlatten_weiADC, isdata, False, False, 0.15, '(z and layer) index', 'phi index', 'Entries (weighted by ADC)', 'colz', '{}/TrkrHitRowColumnFlatten_weiADC'.format(plotdir))
    hM_hitmap_RowColumnFlatten_avgtrkrhitadc.Divide(hM_hitmap_RowColumnFlatten_unwei)
    Draw_2Dhist(hM_hitmap_RowColumnFlatten_avgtrkrhitadc, isdata, False, False, 0.15, '(z and layer) index', 'phi index','Average TrkrHit ADC', 'colz', '{}/TrkrHitRowColumnFlatten_avgTrkrHitADC'.format(plotdir))


    