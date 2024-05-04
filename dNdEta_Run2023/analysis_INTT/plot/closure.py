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


# def Draw_1Dhist_datasimcomp(hdata, hsims, gpadmargin, norm, logy, ymaxscale, XaxisName, Ytitle_unit, prelim, simlegtex, outname):
def Draw_1Dhist_datasimcomp(hdata, hsims, gpadmargin, logy, ymaxscale, XaxisName, YaxisName, Ytitle_unit, prelim, simlegtex, outname):
    hsimcolor = colorset(len(hsims))

    maxbincontent = -1E6
    hdata.Sumw2()
    for hsim in hsims:
        hsim.Sumw2()
        # Get the maximum bin content 
        maxbincontent = max(hdata.GetMaximum(), hsim.GetMaximum())

    binwidth = hdata.GetXaxis().GetBinWidth(1)

    # if norm == 'unity':
    #     hdata.Scale(1. / hdata.Integral(-1, -1))
    #     for hsim in hsims:
    #         hsim.Scale(1. / hsim.Integral(-1, -1))
    # elif norm == 'data':
    #     for hsim in hsims:
    #         hsim.Scale(hdata.Integral(-1, -1) / hsim.Integral(-1, -1))
    # else:
    #     if norm != 'none':
    #         print('Invalid normalization option: {}'.format(norm))
    #         sys.exit(1)
    

    c = TCanvas('c', 'c', 800, 700)
    if logy:
        c.SetLogy()
    c.cd()
    gPad.SetRightMargin(gpadmargin[0])
    gPad.SetTopMargin(gpadmargin[1])
    gPad.SetLeftMargin(gpadmargin[2])
    gPad.SetBottomMargin(gpadmargin[3])
    
    for i, hsim in enumerate(hsims):
        if i == 0:
            # if norm == 'unity':
            #     if Ytitle_unit == '':
            #         hsim.GetYaxis().SetTitle(
            #             'Normalized entries / ({:.2f})'.format(binwidth))
            #     else:
            #         hsim.GetYaxis().SetTitle(
            #             'Normalized entries / ({:.2f} {unit})'.format(binwidth, unit=Ytitle_unit))
            # else:
            #     if Ytitle_unit == '':
            #         hsim.GetYaxis().SetTitle('Entries / ({:.2f})'.format(binwidth))
            #     else:
            #         hsim.GetYaxis().SetTitle(
            #             'Entries / ({:.2f} {unit})'.format(binwidth, unit=Ytitle_unit))
            
            hsim.GetYaxis().SetTitle(YaxisName)

            if logy:
                hsim.GetYaxis().SetRangeUser(hsim.GetMinimum(0)*0.5, maxbincontent * ymaxscale)
            else:
                hsim.GetYaxis().SetRangeUser(0., maxbincontent * ymaxscale)

            hsim.GetXaxis().SetTitle(XaxisName)
            hsim.GetXaxis().SetTitleOffset(1.1)
            hsim.GetYaxis().SetTitleOffset(1.5)
            hsim.SetLineColor(TColor.GetColor(hsimcolor[i]))
            hsim.SetLineWidth(2)
            hsim.SetMarkerSize(0)
            hsim.Draw('histe')
        else:
            hsim.SetLineColor(TColor.GetColor(hsimcolor[i]))
            hsim.SetLineWidth(2)
            hsim.SetMarkerSize(0)
            hsim.Draw('histe same')

    hdata.SetMarkerStyle(20)
    hdata.SetMarkerSize(1)
    hdata.SetMarkerColor(1)
    hdata.SetLineColor(1)
    hdata.SetLineWidth(2)
    hdata.Draw('same PE1')
    shift = 0.45 if prelim else 0.54
    legylow = 0.2 + 0.04 * (len(hsims) - 1)
    leg = TLegend((1-RightMargin)-shift, (1-TopMargin)-legylow,
                  (1-RightMargin)-0.1, (1-TopMargin)-0.03)
    leg.SetTextSize(0.04)
    leg.SetFillStyle(0)
    prelimtext = 'Preliminary' if prelim else 'Work-in-progress'
    leg.AddEntry('', '#it{#bf{sPHENIX}} '+prelimtext, '')
    leg.AddEntry('', 'Au+Au #sqrt{s_{NN}}=200 GeV', '')
    leg.AddEntry(hdata, 'Data', "PE1");
    for i, lt in enumerate(simlegtex):
        leg.AddEntry(hsims[i], lt, "le");
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
    parser = OptionParser(usage='usage: %prog ver [options -h]')
    parser.add_option('-d', '--datahistdir', dest='datahistdir', type='string', default='/sphenix/user/hjheng/TrackletAna/analysis_INTT/plot/corrections/Data_set1', help='Histogram file name (data)')
    parser.add_option('-s', '--simhistdir', action='append', dest='simhistdir', type='string', help='Histogram file name (simulation). Example: /sphenix/user/hjheng/TrackletAna/analysis_INTT/plot/corrections/closure_HIJING_set2')
    parser.add_option('-c', '--centralitydirstr', dest='centralitydirstr', type='string', default='Centrality0to10', help='Centrality string')
    parser.add_option('-l', '--simlegtext', action='append', dest='simlegtext', type='string', help='Legend text for simulation. Example: HIJING/EPOS/AMPT)')
    parser.add_option('-p', '--plotdir', dest='plotdir', type='string', default='cross-checks', help='Plot directory')

    (opt, args) = parser.parse_args()
    print('opt: {}'.format(opt))

    datahistdir = opt.datahistdir
    simhistdir = opt.simhistdir
    simlegtext = opt.simlegtext
    plotdir = opt.plotdir
    centralitydirstr = opt.centralitydirstr

    os.makedirs('./corrections/{}'.format(plotdir), exist_ok=True)

    h1WEfinal_data = GetHistogram("{}/{}/correction_hists.root".format(datahistdir,centralitydirstr), 'h1WEfinal')
    l_h1WEfinal_sim = []
    l_h1WGhadron_sim = []
    for i, simhistd in enumerate(simhistdir):
        l_h1WEfinal_sim.append(GetHistogram("{}/{}/correction_hists.root".format(simhistd,centralitydirstr), 'h1WEfinal'))
        l_h1WGhadron_sim.append(GetHistogram("{}/{}/correction_hists.root".format(simhistd,centralitydirstr), 'h1WGhadron'))

    # Draw_1Dhist_datasimcomp(hdata, hsims, gpadmargin, logy, ymaxscale, XaxisName, YaxisName, Ytitle_unit, prelim, simlegtex, outname)
    Draw_1Dhist_datasimcomp(h1WEfinal_data, l_h1WEfinal_sim, [0.06,0.06,0.15,0.13], False, 1.5, '#eta', 'dN_{ch}/d#eta', '', False, simlegtext, './corrections/{}/dNdeta_{}_crosscheck'.format(plotdir,centralitydirstr))