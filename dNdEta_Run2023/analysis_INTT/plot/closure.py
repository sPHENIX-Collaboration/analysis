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

def colorset(i):
    if i == 1:
        return ['#810000']
    elif i == 2:
        return ['#810000', '#0F4C75']
    elif i == 3:
        return ['#810000', '#0F4C75', '#5E8B7E']
    elif i == 4:
        return ['#810000', '#0F4C75', '#5E8B7E', '#7F167F']
    elif i == 5:
        return ['#810000', '#0F4C75', '#5E8B7E', '#7F167F', '#e99960']
    else:
        print ("Attempt to use more than 5 colors")
        return ['#810000', '#0F4C75', '#5E8B7E', '#7F167F', '#e99960', '#FFC0CB', '#ffcc66', '#7FE9DE']


def Draw_1Dhist_datasimcomp(hdatas, hsims, gpadmargin, logy, ymaxscale, XaxisName, YaxisName, Ytitle_unit, prelim, addstr, datalegtex, simlegtex, outname):
    nhists = len(hdatas) + len(hsims)
    hsimcolor = colorset(len(hsims))

    maxbincontent = -1E6
    xrange_low = 999 
    xrange_high = -999
    
    for hdata in hdatas:
        hdata.Sumw2()
        # Get the maximum bin content
        maxbincontent = max(hdata.GetMaximum()+hdata.GetBinError(hdata.GetMaximumBin()), maxbincontent)
        # Get the x-axis range
        if hdata.GetXaxis().GetXmin() < xrange_low:
            xrange_low = hdata.GetXaxis().GetXmin()
        if hdata.GetXaxis().GetXmax() > xrange_high:
            xrange_high = hdata.GetXaxis().GetXmax()
        
    for hsim in hsims:
        hsim.Sumw2()
        # Get the maximum bin content 
        maxbincontent = max(hsim.GetMaximum()+hsim.GetBinError(hsim.GetMaximumBin()), maxbincontent)
        # Get the x-axis range
        if hsim.GetXaxis().GetXmin() < xrange_low:
            xrange_low = hsim.GetXaxis().GetXmin()
        if hsim.GetXaxis().GetXmax() > xrange_high:
            xrange_high = hsim.GetXaxis().GetXmax()

    binwidth = hdatas[0].GetXaxis().GetBinWidth(1)    

    c = TCanvas('c', 'c', 800, 700)
    if logy:
        c.SetLogy()
    c.cd()
    gPad.SetRightMargin(gpadmargin[0])
    gPad.SetTopMargin(gpadmargin[1])
    gPad.SetLeftMargin(gpadmargin[2])
    gPad.SetBottomMargin(gpadmargin[3])
    
    for i, hdata in enumerate(hdatas):
        if hdata.Integral(-1,-1) == 0:
            continiue
        
        
        hdata.SetMarkerStyle(21)
        # hdata.SetMarkerSize(0.6)
        hdata.SetMarkerColor(12 if i == 0 else 28)
        hdata.SetLineColor(12 if i == 0 else 28)
        hdata.SetLineWidth(2)
        hdata.SetFillColorAlpha(12 if i == 0 else 28, 0.3)
        if i == 0:
            hdata.GetYaxis().SetTitle(YaxisName)
            if logy:
                hdata.GetYaxis().SetRangeUser(hdata.GetMinimum(0)*0.5, maxbincontent * ymaxscale)
            else:
                hdata.GetYaxis().SetRangeUser(0., maxbincontent * ymaxscale)
                
            hdata.GetXaxis().SetTitle(XaxisName)
            hdata.GetXaxis().SetRangeUser(xrange_low, xrange_high)
            hdata.GetXaxis().SetTitleOffset(1.1)
            hdata.GetYaxis().SetTitleOffset(1.5)
            hdata.Draw('E5')
        else:
            hdata.Draw('E5 same')
    
    for i, hsim in enumerate(hsims):
        hsim.SetLineColor(TColor.GetColor(hsimcolor[i]))
        hsim.SetLineWidth(2)
        if 'generator' in simlegtex[i]:
            hsim.SetMarkerSize(0)
            hsim.Draw('histe same')   
        else:
            hsim.SetFillColorAlpha(TColor.GetColor(hsimcolor[i]), 0.3)
            # hsim.SetMarkerSize(0.6)
            hsim.SetMarkerColor(TColor.GetColor(hsimcolor[i]))
            hsim.Draw('E5 same')
            
    # Draw again to be on top
    for i, hsim in enumerate(hsims):
        if 'generator' in simlegtex[i]:
            continue
        else:
            hsim.SetLineColor(TColor.GetColor(hsimcolor[i]))
            hsim.SetLineWidth(2)
            hsim.SetFillColorAlpha(TColor.GetColor(hsimcolor[i]), 0.3)
            # hsim.SetMarkerSize(0.6)
            hsim.SetMarkerColor(TColor.GetColor(hsimcolor[i]))
            hsim.Draw('E5 same')
    
    shift = 0.45 if prelim else 0.75
    legylowbase = 0.44
    legylow = legylowbase + 0.065 * nhists
    leg = TLegend((1-RightMargin)-shift, (1-TopMargin)-legylow,
                  (1-RightMargin)-0.1, (1-TopMargin)-legylowbase)
    leg.SetTextSize(0.035)
    leg.SetFillStyle(0)
    prelimtext = 'Preliminary' if prelim else 'Work-in-progress'
    leg.AddEntry('', '#it{#bf{sPHENIX}} '+prelimtext, '')
    leg.AddEntry('', 'Au+Au #sqrt{s_{NN}}=200 GeV', '')
    leg.AddEntry('', addstr, '')
    for i, lt in enumerate(datalegtex):
        leg.AddEntry(hdatas[i], lt, "plf");
    for i, lt in enumerate(simlegtex):
        if 'generator' in lt:
            leg.AddEntry(hsims[i], lt, "le");
        else:
            leg.AddEntry(hsims[i], lt, "plf");
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
        
        
def GetMbinNum(fstr):
    mbinnum = -1
    if 'Centrality0to5' in fstr:
        mbinnum = 0
    elif 'Centrality5to10' in fstr:
        mbinnum = 1
    elif 'Centrality10to20' in fstr:
        mbinnum = 2
    elif 'Centrality20to30' in fstr:
        mbinnum = 3
    elif 'Centrality30to40' in fstr:
        mbinnum = 4
    elif 'Centrality40to50' in fstr:
        mbinnum = 5
    elif 'Centrality50to60' in fstr:
        mbinnum = 6
    elif 'Centrality60to70' in fstr:
        mbinnum = 7
    elif 'Centrality0to70' in fstr:
        mbinnum = 11
    return mbinnum
    

if __name__ == '__main__':
    parser = OptionParser(usage='usage: %prog ver [options -h]')
    parser.add_option('--datahistdir', dest='datahistdir', type='string', default='/sphenix/user/hjheng/sPHENIXRepo/analysis/dNdEta_Run2023/analysis_INTT/plot/systematics/', help='Histogram file name (data)')
    parser.add_option('--simhistdir', action='append', dest='simhistdir', type='string', help='Histogram file name (simulation). Example: /sphenix/user/hjheng/TrackletAna/analysis_INTT/plot/corrections/closure_HIJING_set2')
    parser.add_option('--filedesc', dest='filedesc', type='string', default='Centrality0to5_Zvtxm30p0tom10p0_noasel', help='String for input file and output plot description (cuts, binnings, etc...)')
    # parser.add_option('--simlegtext', action='append', dest='simlegtext', type='string', help='Legend text for simulation. Example: HIJING/EPOS/AMPT)')
    parser.add_option('--docompare', action='store_true', dest='docompare', default=False, help='Compare two analyses')
    parser.add_option('--plotdir', dest='plotdir', type='string', default='cross-checks', help='Plot directory')

    (opt, args) = parser.parse_args()
    print('opt: {}'.format(opt))

    datahistdir = opt.datahistdir
    simhistdir = opt.simhistdir
    # simlegtext = opt.simlegtext
    plotdir = opt.plotdir
    filedesc = opt.filedesc
    docompare = opt.docompare

    os.makedirs('./corrections/{}'.format(plotdir), exist_ok=True)
    
    MbinNum = GetMbinNum(filedesc)
    simlegtext = []

    h1WEfinal_data = GetHistogram("{}/{}/finalhists_systematics_{}.root".format(datahistdir,filedesc,filedesc), 'hM_final')
    l_h1WEfinal_sim = []
    # l_h1WGhadron_sim = []
    for i, simhistd in enumerate(simhistdir):
        # tmplegtxt = simlegtext[i]
        
        # From the CMS-style analysis
        l_h1WEfinal_sim.append(GetHistogram("{}/{}/correction_hists.root".format(simhistd,filedesc), 'h1WEfinal'))
        simlegtext.append('Simulation closure (CMS approach)')
        # From the PHOBOS-style analysis
        l_h1WEfinal_sim.append(GetHistogram("/sphenix/user/ChengWei/INTT/INTT_commissioning/ZeroField/F4A_20869/2024_05_07/folder_Data_CombinedNtuple_Run20869_HotDead_BCO_ADC_Survey/evt_tracklet/complete_file/merged_file_folder/eta_closer_out_test_MultiZBin/hist_for_AN.root", 'dNdeta_1D_MCreco_loose_PostCorrection_corr_Mbin{}'.format(MbinNum)))
        simlegtext.append('Simulation closure (PHOBOS approach)')
        # Raw generated hadron distribution
        l_h1WEfinal_sim.append(GetHistogram("{}/{}/correction_hists.root".format(simhistd,filedesc), 'h1WGhadron'))
        simlegtext.append('HIJING (generator)')
        
    
    hM_dNdeta_1D_Datareco_tight_PostCorrection = GetHistogram("/sphenix/user/ChengWei/INTT/INTT_commissioning/ZeroField/F4A_20869/2024_05_07/folder_Data_CombinedNtuple_Run20869_HotDead_BCO_ADC_Survey/evt_tracklet/complete_file/merged_file_folder/eta_closer_out_test_MultiZBin/hist_for_AN.root", 'dNdeta_1D_Datareco_tight_PostCorrection_Mbin{}'.format(MbinNum))
    
    # split the string by '_' and replace some characters
    descstr = filedesc.split('_')
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
    
    # Draw_1Dhist_datasimcomp(hdatas, hsims, gpadmargin, logy, ymaxscale, XaxisName, YaxisName, Ytitle_unit, prelim, addstr, datalegtex, simlegtex, outname):
    if 'Centrality0to5' in filedesc:
        Draw_1Dhist_datasimcomp([h1WEfinal_data], l_h1WEfinal_sim, [0.06,0.06,0.15,0.13], False, 1.1, '#eta', 'dN_{ch}/d#eta', '', False, legstr, ['Data (CMS approach)'], simlegtext, './corrections/{}/dNdeta_{}_crosscheck'.format(plotdir,filedesc))
    else:
        Draw_1Dhist_datasimcomp([h1WEfinal_data, hM_dNdeta_1D_Datareco_tight_PostCorrection], l_h1WEfinal_sim, [0.06,0.06,0.15,0.13], False, 1.1, '#eta', 'dN_{ch}/d#eta', '', False, legstr, ['Data (CMS approach)', 'Data (PHOBOS approach)'], simlegtext, './corrections/{}/dNdeta_{}_crosscheck'.format(plotdir,filedesc))