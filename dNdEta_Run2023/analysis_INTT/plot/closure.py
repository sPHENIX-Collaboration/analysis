#! /usr/bin/env python
from optparse import OptionParser
import sys
import os
import datetime
from array import *
from ROOT import TH1F, TH2F, TFile, TCanvas, TLegend, TColor, gROOT, gSystem, gPad, kGreen, kBlack, kOrange, TGraphAsymmErrors
import numpy
import math
import glob
from plotUtil import GetHistogram, colorset

TickSize = 0.03
AxisTitleSize = 0.05
AxisLabelSize = 0.04
LeftMargin = 0.15
RightMargin = 0.08
TopMargin = 0.08
BottomMargin = 0.13

f_phobosAuAu200GeV = '/sphenix/user/hjheng/sPHENIXRepo/analysis/dNdEta_Run2023/analysis_INTT/plot/measurements/PHOBOS-PhysRevC.83.024913/auau_200GeV.root'

gROOT.SetBatch(True)

def Draw_1Dhist_datasimcomp(hdatas, hsims, gpadmargin, logy, ymaxscale, XaxisName, YaxisName, Ytitle_unit, prelim, addstr, datalegtex, simlegtex, outname):
    nhists = len(hdatas) + len(hsims)
    hsimcolor = colorset(len(hsims))
    
    # get the centrality low and high from addstr: Centrality [0-5]%
    centstr = addstr.split(',')[0].split('[')[1].split(']')[0].split('-')
    centlow = int(centstr[0])
    centhigh = int(centstr[1])
    # get phobos measurement: the results are TGraphAsymmErrors
    f_phobos = TFile(f_phobosAuAu200GeV, 'READ')
    # get the TGraphAsymmErrors, if not found, return None and skip
    g_phobos = f_phobos.Get('AuAu_200GeV_Centrality_{}to{}'.format(centlow, centhigh))
    if g_phobos:
        nhists+=1
    else:
        print('PHOBOS measurement not found for centrality {}-{}%'.format(centlow, centhigh))
        
    maxbincontent = -1E6
    xrange_low = 999 
    xrange_high = -999
    
    gdatas = []
    for hdata in hdatas:
        hdata.Sumw2()
        # maxbincontent = max(hdata.GetMaximum()+hdata.GetBinError(hdata.GetMaximumBin()), maxbincontent)
        tgae = TGraphAsymmErrors(hdata)
        # remove the points with zero bin content
        for i in range(tgae.GetN()-1, -1, -1):
            if tgae.GetY()[i] == 0:
                tgae.RemovePoint(i)
        gdatas.append(tgae)
    
    xrange_low = hdatas[0].GetXaxis().GetXmin()
    xrange_high = hdatas[0].GetXaxis().GetXmax()
        
    for hsim in hsims:
        hsim.Sumw2()
        # Get the maximum bin content 
        # maxbincontent = max(hsim.GetMaximum()+hsim.GetBinError(hsim.GetMaximumBin()), maxbincontent)
        # Get the x-axis range
        if hsim.GetXaxis().GetXmin() < xrange_low:
            xrange_low = hsim.GetXaxis().GetXmin()
        if hsim.GetXaxis().GetXmax() > xrange_high:
            xrange_high = hsim.GetXaxis().GetXmax()

    binwidth = hdatas[0].GetXaxis().GetBinWidth(1)    

    # get the maximum bin content from HIJING truth, the first histogram in hsims
    maxbincontent = hsims[0].GetMaximum()
    print ("maxbincontent: ", maxbincontent)
    c = TCanvas('c', 'c', 800, 700)
    if logy:
        c.SetLogy()
    c.cd()
    gPad.SetRightMargin(gpadmargin[0])
    gPad.SetTopMargin(gpadmargin[1])
    gPad.SetLeftMargin(gpadmargin[2])
    gPad.SetBottomMargin(gpadmargin[3])
    
    for i, hsim in enumerate(hsims):
        hsim.SetLineColor(TColor.GetColor(hsimcolor[i]))
        hsim.SetLineWidth(2)
        if 'generator' in simlegtex[i]: # HIING truth always the first
            hsim.GetYaxis().SetTitle(YaxisName)
            hsim.GetYaxis().SetRangeUser(0, maxbincontent * ymaxscale)
            # if logy:
            #     hsim.GetYaxis().SetRangeUser(hdata.GetMinimum(0)*0.5, maxbincontent * ymaxscale)
            # else:
            #     hsim.GetYaxis().SetRangeUser(0., maxbincontent * ymaxscale)
                
            hsim.GetXaxis().SetTitle(XaxisName)
            # hsim.GetXaxis().SetRangeUser(xrange_low, xrange_high)
            hsim.GetXaxis().SetRange(hsim.FindFirstBinAbove(0),hsim.FindLastBinAbove(0))
            hsim.GetXaxis().SetTitleOffset(1.1)
            hsim.GetYaxis().SetTitleOffset(1.5)
            hsim.SetMarkerSize(0)
            hsim.Draw('histe')   
        else:
            hsim.SetFillColorAlpha(TColor.GetColor(hsimcolor[i]), 0.3)
            # hsim.SetMarkerSize(0.6)
            hsim.SetMarkerColor(TColor.GetColor(hsimcolor[i]))
            hsim.Draw('E5 same')
    
    for i, gdata in enumerate(gdatas): # the first is the CMS approach, the second is the PHOBOS approach
        # if hdata.Integral(-1,-1) == 0:
        #     continiue
        
        gdata.SetMarkerStyle(34 if i==0 else 47)
        gdata.SetMarkerSize(1)
        gdata.SetMarkerColor(TColor.GetColor('#FF6700') if i == 0 else TColor.GetColor('#228B22'))
        gdata.SetLineColor(TColor.GetColor('#FF6700') if i == 0 else TColor.GetColor('#228B22'))
        gdata.SetLineWidth(1)
        gdata.SetFillColorAlpha(TColor.GetColor('#FF6700') if i == 0 else TColor.GetColor('#228B22'), 0.3)
        gdata.GetXaxis().SetRange(hdata.FindFirstBinAbove(0),hdata.FindLastBinAbove(0))
        gdata.Draw('5p same')
            
    # Draw the PHOBOS measurement
    if g_phobos:
        g_phobos.SetMarkerStyle(25)
        g_phobos.SetMarkerSize(0.8)
        g_phobos.SetMarkerColor(TColor.GetColor('#035397'))
        g_phobos.SetLineColor(TColor.GetColor('#035397'))
        g_phobos.SetLineWidth(0)
        g_phobos.SetFillColorAlpha(TColor.GetColor('#035397'), 0.3)
        g_phobos.Draw('p same')
        g_phobos.Draw('3 same')
        
    
    shift = 0.45 if prelim else 0.75
    legyspace = 0.08
    if not g_phobos:
        legyspace = 0.085
    leg = TLegend((1-RightMargin)-shift, BottomMargin + 0.03,
                  (1-RightMargin)-0.1, BottomMargin + 0.03 + legyspace * nhists)
    leg.SetTextSize(0.035)
    leg.SetFillStyle(0)
    prelimtext = 'Preliminary' if prelim else 'Internal'
    leg.AddEntry('', '#it{#bf{sPHENIX}} '+prelimtext, '')
    leg.AddEntry('', 'Au+Au #sqrt{s_{NN}}=200 GeV', '')
    leg.AddEntry('', addstr, '')
    for i, lt in enumerate(datalegtex):
        leg.AddEntry(gdatas[i], lt, "pf");
    for i, lt in enumerate(simlegtex):
        if 'generator' in lt:
            leg.AddEntry(hsims[i], lt, "le");
        else:
            leg.AddEntry(hsims[i], lt, "pl");
    if g_phobos:
        leg.AddEntry(g_phobos, 'PHOBOS [Phys. Rev. C 83, 024913]', 'pf')
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
    if 'Centrality0to3' in fstr:
        mbinnum = 0
    elif 'Centrality3to6' in fstr:
        mbinnum = 1
    elif 'Centrality6to10' in fstr:
        mbinnum = 2
    elif 'Centrality10to15' in fstr:
        mbinnum = 3
    elif 'Centrality15to20' in fstr:
        mbinnum = 4
    elif 'Centrality20to25' in fstr:
        mbinnum = 5
    elif 'Centrality25to30' in fstr:
        mbinnum = 6
    elif 'Centrality30to35' in fstr:
        mbinnum = 7
    elif 'Centrality35to40' in fstr:
        mbinnum = 8
    elif 'Centrality40to45' in fstr:
        mbinnum = 9
    elif 'Centrality45to50' in fstr:
        mbinnum = 10
    elif 'Centrality50to55' in fstr:
        mbinnum = 11
    elif 'Centrality55to60' in fstr:
        mbinnum = 12
    elif 'Centrality60to65' in fstr:
        mbinnum = 13
    elif 'Centrality65to70' in fstr:
        mbinnum = 14
    elif 'Centrality0to70' in fstr:
        mbinnum = 70
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
    # loop over the bins, for |eta| > 1.1, set the bin content to 0
    for i in range(1, h1WEfinal_data.GetNbinsX()+1):
        if abs(h1WEfinal_data.GetBinCenter(i)) > 1.1:
            h1WEfinal_data.SetBinContent(i, 0)
            h1WEfinal_data.SetBinError(i, 0)
    
    l_h1WEfinal_sim = []
    # l_h1WGhadron_sim = []
    for i, simhistd in enumerate(simhistdir):
        # tmplegtxt = simlegtext[i]
        
        l_h1WEfinal_sim.append(GetHistogram("{}/{}/correction_hists.root".format(simhistd,filedesc), 'h1WGhadron'))
        simlegtext.append('HIJING (generator)')
        # From the CMS-style analysis
        # l_h1WEfinal_sim.append(GetHistogram("{}/{}/correction_hists.root".format(simhistd,filedesc), 'h1WEfinal'))
        # simlegtext.append('Simulation closure [The closest-match method]')
        # From the PHOBOS-style analysis
        # if docompare:
        #     l_h1WEfinal_sim.append(GetHistogram("/sphenix/user/ChengWei/INTT/INTT_commissioning/ZeroField/F4A_20869/2024_05_07/folder_Data_CombinedNtuple_Run20869_HotDead_BCO_ADC_Survey/evt_tracklet/complete_file/merged_file_folder/eta_closer_out_test_MultiZBin/hist_for_AN.root", 'dNdeta_1D_MCreco_loose_PostCorrection_corr_Mbin{}'.format(MbinNum)))
        #     simlegtext.append('Simulation closure [The combinatoric method]')
        # Raw generated hadron distribution
        
    if docompare:
        hM_dNdeta_1D_Datareco_tight_PostCorrection = GetHistogram("/sphenix/tg/tg01/commissioning/INTT/work/cwshih/seflgendata/run_54280_HR_Feb102025/Run6_EvtZFitWidthChange/EvtVtxZ/FinalResult_10cm_Pol2BkgFit/completed/vtxZ_-10_10cm_MBin{}/Final_Mbin{}_00054280/Final_Mbin{}_00054280.root".format(MbinNum, MbinNum, MbinNum), 'h1D_dNdEta_reco')
        for i in range(1, hM_dNdeta_1D_Datareco_tight_PostCorrection.GetNbinsX()+1):
            if abs(hM_dNdeta_1D_Datareco_tight_PostCorrection.GetBinCenter(i)) > 1.1:
                hM_dNdeta_1D_Datareco_tight_PostCorrection.SetBinContent(i, 0)
                hM_dNdeta_1D_Datareco_tight_PostCorrection.SetBinError(i, 0)
    
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
    zvtxstr = zvtxstr.replace('Zvtx', 'vtx_{Z} [')
    zvtxstr = zvtxstr + '] cm'
    
    aselstr = '' if aselstr == 'noasel' else ', (with additional selection)'
    
    # legstr = centstr + ', ' + zvtxstr + aselstr
    legstr = centstr
    
    # Draw_1Dhist_datasimcomp(hdatas, hsims, gpadmargin, logy, ymaxscale, XaxisName, YaxisName, Ytitle_unit, prelim, addstr, datalegtex, simlegtex, outname):
    if docompare:
        Draw_1Dhist_datasimcomp([h1WEfinal_data, hM_dNdeta_1D_Datareco_tight_PostCorrection], l_h1WEfinal_sim, [0.06,0.06,0.15,0.13], False, 1.5, 'Pseudorapidity #eta', 'dN_{ch}/d#eta', '', False, legstr, ['Data - The closest-match method', 'Data - The combinatoric method'], simlegtext, './corrections/{}/dNdeta_{}_crosscheck'.format(plotdir,filedesc))
    else:
        Draw_1Dhist_datasimcomp([h1WEfinal_data], l_h1WEfinal_sim, [0.06,0.06,0.15,0.13], False, 1.5, 'Pseudorapidity #eta', 'dN_{ch}/d#eta', '', False, legstr, ['Data - The closest-match method'], simlegtext, './corrections/{}/dNdeta_{}_crosscheck'.format(plotdir,filedesc))