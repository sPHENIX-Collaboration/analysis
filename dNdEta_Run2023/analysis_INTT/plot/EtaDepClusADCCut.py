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
from plot_DataSimComp import Draw_1Dhist_datasimcomp

gROOT.SetBatch(True)

smin = 5
smax = 55
sstep = 1

esmin = 0.6
esmax = 1.2 
esstep = 0.02

def theta2pseudorapidity(theta):
    theta = theta * TMath.Pi() / 180.0
    return -1.0 * TMath.Log(TMath.Tan(theta/2.0))


def EtaDepADCCut_INTTPrivate():
    thetastep = [0.001, 15, 20, 25, 30, 35, 45, 55, 125, 135, 145, 150, 155, 160, 165, 179.999]
    # reverse thetastep 
    thetastep = thetastep[::-1]
    adccut_theta = [225, 165, 135, 120, 105, 90, 75, 60, 75, 90, 105, 120, 135, 165, 225]
    etastep = []
    for i in range(0, len(thetastep)):
        etastep.append(theta2pseudorapidity(thetastep[i]))
        
    hm = TH1F('hm', 'hm', len(etastep)-1, array('d', etastep))
    for j in range(hm.GetNbinsX()):
        hm.SetBinContent(j+1, adccut_theta[j])
    
    print (etastep)
    
    return hm


def funclist(scalemin, scalemax, scalestep):
    l_func = []
    for i in range(scalemin, scalemax, scalestep):
        f = TF1('f', '{}*TMath::CosH(x)'.format(i), -5, 5)
        l_func.append(f)
    return l_func


def Draw_2Dhist_wFunc(hist_wcut, hist_wocut, IsData, logz, norm1, rmargin, XaxisName, YaxisName, ZaxisName, drawopt_wcut, drawopt_wocut, outname):
    # fs = funclist(smin, smax, sstep)
    # nColors = gStyle.GetNumberOfColors()
    # nFuncs = len(fs)
    
    # strip the outname to get the const scale: it should be at the end of the string, remove 'constscale'
    scale = -999
    etascale = -999
    if 'inttprivate' not in outname:
        scale = outname.split('_')[-2].replace('constscale','')
        etascale = outname.split('_')[-1].replace('etascale','').replace('p','.')
        print ('scale = {}, etascale = {}'.format(scale, etascale))
    
    c = TCanvas('c', 'c', 800, 700)
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
        hist_wcut.Scale(1. / hist_wcut.Integral(-1, -1, -1, -1))
    hist_wcut.GetXaxis().SetTitle(XaxisName)
    hist_wcut.GetYaxis().SetTitle(YaxisName)
    hist_wcut.GetXaxis().SetTickSize(TickSize)
    hist_wcut.GetYaxis().SetTickSize(TickSize)
    hist_wcut.GetXaxis().SetTitleSize(AxisTitleSize)
    hist_wcut.GetYaxis().SetTitleSize(AxisTitleSize)
    hist_wcut.GetXaxis().SetLabelSize(AxisLabelSize)
    hist_wcut.GetYaxis().SetLabelSize(AxisLabelSize)
    if ZaxisName != '':
        hist_wcut.GetZaxis().SetTitle(ZaxisName)
        hist_wcut.GetZaxis().SetTitleSize(AxisTitleSize)
        hist_wcut.GetZaxis().SetTitleOffset(1.1)
        
    hist_wcut.GetXaxis().SetTitleOffset(1.1)
    hist_wcut.GetYaxis().SetTitleOffset(1.3)
    hist_wcut.GetZaxis().SetLabelSize(AxisLabelSize)
    hist_wcut.SetContour(1000)
    hist_wcut.Draw(drawopt_wcut)
    
    hist_wocut.SetLineWidth(1)
    hist_wocut.Draw(drawopt_wocut)
    
    if 'inttprivate' not in outname:
        adcstep = [20+i*30 for i in range(0, 20)] # quantized step
        etastep = []
        f = TF1('f', '{}*TMath::CosH({}*x)'.format(scale, etascale), -10, 10)
        for i in range(0, 20):
            etastep.insert(0, f.GetX(adcstep[i], -10, 0))
            etastep.append(f.GetX(adcstep[i], 0, 10))
        
        # remove nan values
        etastep = [x for x in etastep if not math.isnan(x)]
        print(etastep)
        
        hm_cut_v2 = TH1F('hm_cut_v2', 'hm_cut_v2', len(etastep)-1, array('d', etastep))
        for j in range(hm_cut_v2.GetNbinsX()):
            # Set bin content at the quantized step
            if hm_cut_v2.GetBinLowEdge(j+1) < 0:
                hm_cut_v2.SetBinContent(j+1, f.Eval(hm_cut_v2.GetBinLowEdge(j+1)))
            else:
                hm_cut_v2.SetBinContent(j+1, f.Eval(hm_cut_v2.GetBinCenter(j+1) + hm_cut_v2.GetBinWidth(j+1)/2))
            # hm_cut_v2.SetBinContent(j+1, f.Eval(hm_cut_v2.GetBinCenter(j+1)))
            
        hm_cut_v2.SetLineColorAlpha(kRed, 1)
        hm_cut_v2.SetLineWidth(2)
        hm_cut_v2.Draw('hist same')
        
        f.SetLineColorAlpha(kRed, 1)
        f.SetLineStyle(2)
        f.SetLineWidth(2)
        f.Draw('same')
    else:
        hm_cut = EtaDepADCCut_INTTPrivate()
        hm_cut.SetLineColorAlpha(kRed, 1)
        hm_cut.SetLineWidth(2)
        hm_cut.Draw('hist same')
    
    # hm_cut = TH1F('hm_cut', 'hm_cut', 40, -10, 10)
    # for j in range(hm_cut.GetNbinsX()):
    #     hm_cut.SetBinContent(j+1, f.Eval(hm_cut.GetBinCenter(j+1)))
    # hm_cut.SetLineColorAlpha(kRed, 1)
    # hm_cut.SetLineWidth(2)
    # hm_cut.Draw('hist same')
    
    # lhm_cut = []
    # for i, func in enumerate(fs):
    #     hm_cut = TH1F('hm_cut_{}'.format(i), 'hm_cut_{}'.format(i), 40, -10, 10)
    #     for j in range(hm_cut.GetNbinsX()):
    #         hm_cut.SetBinContent(j+1, func.Eval(hm_cut.GetBinCenter(j+1)))
        
    #     lhm_cut.append(hm_cut)
    #     fcolor = int(float(nColors) / nFuncs * i)
    #     # func.SetLineColorAlpha(gStyle.GetColorPalette(fcolor), 0.5)
    #     # func.SetLineWidth(2)
    #     # func.Draw('same')
    #     lhm_cut[i].SetLineColorAlpha(gStyle.GetColorPalette(fcolor), 0.5)
    #     lhm_cut[i].SetLineWidth(2)
    #     lhm_cut[i].Draw('hist same')
    
    # Add text for the function 
    tex = TLatex()
    tex.SetTextSize(0.04)
    tex.SetTextAlign(11)
    tex.SetTextFont(42)
    tex.SetTextColor(kRed)
    tex.SetNDC()
    if 'inttprivate' not in outname:
        tex.DrawLatex(LeftMargin+0.05, 0.85, 'ADC cut = {:d}#timescosh({:.2f}#times#eta)'.format(int(scale), float(etascale)))
    else:
        tex.DrawLatex(LeftMargin+0.05, 0.85, 'INTT private #eta-dependent ADC cut')
    
        
    rightshift = 0.0 if IsData else 0.0
    leg = TLegend((1-RightMargin)-0.45, (1-TopMargin)+0.01, (1-RightMargin)-rightshift, (1-TopMargin)+0.04)
    leg.SetTextAlign(kHAlignRight+kVAlignBottom)
    leg.SetTextSize(0.045)
    leg.SetFillStyle(0)
    if IsData:
        leg.AddEntry("", "#it{#bf{sPHENIX}} Work-in-progress", "")
        # leg.AddEntry("", "Au+Au #sqrt{s_{NN}}=200 GeV", "")
    else:
        leg.AddEntry("", "#it{#bf{sPHENIX}} Simulation", "")
        # leg.AddEntry("", "Au+Au #sqrt{s_{NN}}=200 GeV", "")
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
    parser = OptionParser()
    parser.add_option("-d", "--plotdir", dest="plotdir", type="string", default='/sphenix/user/hjheng/sPHENIXRepo/analysis/dNdEta_Run2023/analysis_INTT/plot/RecoCluster', help="Plot directory")
    
    (opt, args) = parser.parse_args()
    print('opt: {}'.format(opt))
    
    plotdir = opt.plotdir
    os.makedirs(plotdir, exist_ok=True)
    os.makedirs(plotdir+'/ClusADCCutOptim', exist_ok=True)
    os.makedirs(plotdir+'/ClusADCCutOptim/Hist1D_ClusEtaPV', exist_ok=True)
    os.makedirs(plotdir+'/ClusADCCutOptim/Hist2D_ClusEtaPVADC_wStepFunc', exist_ok=True)
    
    datahistfile = '/sphenix/user/hjheng/sPHENIXRepo/analysis/dNdEta_Run2023/analysis_INTT/plot/hists/Data_CombinedNtuple_Run20869_HotDead_BCO_ADC_Survey/Cluster/hists_merged.root'
    simhistfile = '/sphenix/user/hjheng/sPHENIXRepo/analysis/dNdEta_Run2023/analysis_INTT/plot/hists/Sim_Ntuple_HIJING_new_20240424/Cluster/hists_merged.root'

    if not os.path.isfile(datahistfile) or not os.path.isfile(simhistfile):
        print("Error: file for the merged histograms not found")
        sys.exit(1)
        
    EtaDepADCCut_INTTPrivate()
        
    hM_ClusEtaPV_ClusADC_all_zoomin_data = GetHistogram(datahistfile, 'hM_ClusEtaPV_ClusADC_all_zoomin')
    hM_ClusEtaPV_ClusADC_all_zoomin_sim = GetHistogram(simhistfile, 'hM_ClusEtaPV_ClusADC_all_zoomin')
    NClus_total_data = hM_ClusEtaPV_ClusADC_all_zoomin_data.Integral(-1,-1,-1,-1)
    NClus_total_sim = hM_ClusEtaPV_ClusADC_all_zoomin_sim.Integral(-1,-1,-1,-1)
    print ('Number of clusters in histogram (data) = ', hM_ClusEtaPV_ClusADC_all_zoomin_data.Integral(-1,-1,-1,-1))
    print ('Number of clusters in histogram (sim) = ', hM_ClusEtaPV_ClusADC_all_zoomin_sim.Integral(-1,-1,-1,-1))
        
        
    # Draw_2Dhist_wFunc(hist, IsData, logz, norm1, rmargin, XaxisName, YaxisName, ZaxisName, drawopt, outname)
    # Draw_2Dhist_wFunc(hM_ClusEtaPV_ClusADC_all_zoomin_data, True, False, False, 0.08, 'Cluster #eta', 'Cluster ADC', '', 'box', plotdir+'/ClusADCCutOptim/hM_ClusEtaPV_ClusADC_all_zoomin_data_wTF1Cosh')
    # Draw_2Dhist_wFunc(hM_ClusEtaPV_ClusADC_all_zoomin_sim, False, False, False, 0.08, 'Cluster #eta', 'Cluster ADC', '', 'box', plotdir+'/ClusADCCutOptim/hM_ClusEtaPV_ClusADC_all_zoomin_sim_wTF1Cosh')
    
    fraclost_constscale_data = []
    fracpreserve_constscale_sim = []
    mindistto1 = 999.
    bestscale = 0
    bestfraclos_data = 0
    bestfracpreserve_sim = 0
    
    l_hM_ClusEtaPV_ClusADC_all_zoomin_data_adccut = []
    l_hM_ClusEtaPV_ClusADC_all_zoomin_sim_adccut = []
    for i in range(smin, smax, sstep):
        l_hM_ClusEtaPV_ClusADC_all_zoomin_data_adccut.append(GetHistogram(datahistfile, 'hM_ClusEtaPV_ClusADC_all_zoomin_constscale{:d}_etascale1p0'.format(i)))
        l_hM_ClusEtaPV_ClusADC_all_zoomin_sim_adccut.append(GetHistogram(simhistfile, 'hM_ClusEtaPV_ClusADC_all_zoomin_constscale{:d}_etascale1p0'.format(i)))
        
        Nclus_data = l_hM_ClusEtaPV_ClusADC_all_zoomin_data_adccut[int((i-smin)/sstep)].Integral(-1,-1,-1,-1)
        Nclus_sim = l_hM_ClusEtaPV_ClusADC_all_zoomin_sim_adccut[int((i-smin)/sstep)].Integral(-1,-1,-1,-1)
        fl = (NClus_total_data - Nclus_data) / NClus_total_data
        fp = Nclus_sim / NClus_total_sim
        fraclost_constscale_data.append(fl)
        fracpreserve_constscale_sim.append(fp)
        # print ('Number of clusters in histogram (data) for ADC cut with const. scale {} = '.format(i), l_hM_ClusEtaPV_ClusADC_all_zoomin_data_adccut[int((i-smin)/sstep)].Integral(-1,-1,-1,-1))
        # print ('Number of clusters in histogram (sim) = ', l_hM_ClusEtaPV_ClusADC_all_zoomin_sim_adccut[int((i-smin)/sstep)].Integral(-1,-1,-1,-1))
        if fp > 0.945 and fp < 0.955:
            print ('Scale = {}, fraction lost in data = {:.4f}, fraction preserved in sim = {:.4f}'.format(i, fl, fp))
        
        if fp > 0.895 and fp < 0.905:
            print ('Scale = {}, fraction lost in data = {:.4f}, fraction preserved in sim = {:.4f}'.format(i, fl, fp))
            
        if fp > 0.845 and fp < 0.855:
            print ('Scale = {}, fraction lost in data = {:.4f}, fraction preserved in sim = {:.4f}'.format(i, fl, fp))
            
        if fp > 0.795 and fp < 0.805:
            print ('Scale = {}, fraction lost in data = {:.4f}, fraction preserved in sim = {:.4f}'.format(i, fl, fp))
            
        # Calculate the distance to the point (1,1), the closer the better
        distto1 = math.sqrt((1-fl)**2 + (1-fp)**2)
        if distto1 < mindistto1:
            mindistto1 = distto1
            bestscale = i
            bestfraclos_data = fl
            bestfracpreserve_sim = fp
            
        
        # Draw_1Dhist_datasimcomp(hdata, hsims, gpadmargin, norm, logy, ymaxscale, XaxisName, Ytitle_unit, prelim, simlegtex, evtseltexts, outname):
        # plot the cluster eta distribution for each const scale 
        hM_ClusEtaPV_EtaDepADCCut_constscale_data = GetHistogram(datahistfile, 'hM_ClusEtaPV_EtaDepADCCut_constscale{:d}_etascale1p0'.format(i))
        hM_ClusEtaPV_EtaDepADCCut_constscale_sim = GetHistogram(simhistfile, 'hM_ClusEtaPV_EtaDepADCCut_constscale{:d}_etascale1p0'.format(i))
        # Draw_1Dhist_datasimcomp(hM_cluseta_zvtxwei_data, l_hM_cluseta_zvtxwei_sim, [0.1,0.08,0.15,0.13], 'data', False, 1.8, 'Cluster #eta', '', False, simlegtext, ['Cluster ADC > 35'], './DataSimComp/{}/Cluster_Eta_zvtxwei'.format(plotdir))
        Draw_1Dhist_datasimcomp(hM_ClusEtaPV_EtaDepADCCut_constscale_data, [hM_ClusEtaPV_EtaDepADCCut_constscale_sim], [0.1,0.08,0.15,0.13], 'data', False, 1.8, 'Cluster #eta', '', False, ['Simulation'], ['Cluster ADC cut > {:d}#timescosh(1#times#eta)'.format(i)], plotdir+'/ClusADCCutOptim/Hist1D_ClusEtaPV/hM_ClusEtaPV_EtaDepADCCut_constscale{:d}_etascale1p0'.format(i))
        # Draw_2Dhist_wFunc(hist_wcut, hist_wocut, IsData, logz, norm1, rmargin, XaxisName, YaxisName, ZaxisName, drawopt_wcut, drawopt_wocut, outname):
        Draw_2Dhist_wFunc(l_hM_ClusEtaPV_ClusADC_all_zoomin_data_adccut[int((i-smin)/sstep)], hM_ClusEtaPV_ClusADC_all_zoomin_data, True, False, False, 0.08, 'Cluster #eta', 'Cluster ADC', '', 'col', 'box same', plotdir+'/ClusADCCutOptim/Hist2D_ClusEtaPVADC_wStepFunc/hM_ClusEtaPV_ClusADC_all_zoomin_data_wTF1Cosh_constscale{:d}_etascale1p0'.format(i))
        Draw_2Dhist_wFunc(l_hM_ClusEtaPV_ClusADC_all_zoomin_sim_adccut[int((i-smin)/sstep)], hM_ClusEtaPV_ClusADC_all_zoomin_sim, False, False, False, 0.08, 'Cluster #eta', 'Cluster ADC', '', 'col', 'box same', plotdir+'/ClusADCCutOptim/Hist2D_ClusEtaPVADC_wStepFunc/hM_ClusEtaPV_ClusADC_all_zoomin_sim_wTF1Cosh_constscale{:d}_etascale1p0'.format(i))
        
    
    fraclost_etascale_data = []
    fracpreserve_etascale_sim = []
    l_hM_ClusEtaPV_ClusADC_all_zoomin_data_adccut_escale = []
    l_hM_ClusEtaPV_ClusADC_all_zoomin_sim_adccut_escale = []
    for i in range(0, int(float((esmax-esmin)/esstep))):
        es = esmin + i*esstep
        esstr = '{:.2f}'.format(es).replace('.','p')
        print (es, esstr)
        l_hM_ClusEtaPV_ClusADC_all_zoomin_data_adccut_escale.append(GetHistogram(datahistfile, 'hM_ClusEtaPV_ClusADC_all_zoomin_constscale30_etascale{}'.format(esstr)))
        l_hM_ClusEtaPV_ClusADC_all_zoomin_sim_adccut_escale.append(GetHistogram(simhistfile, 'hM_ClusEtaPV_ClusADC_all_zoomin_constscale30_etascale{}'.format(esstr)))
        hM_ClusEtaPV_EtaDepADCCut_etascale_data = GetHistogram(datahistfile, 'hM_ClusEtaPV_EtaDepADCCut_constscale30_etascale{}'.format(esstr))
        hM_ClusEtaPV_EtaDepADCCut_etascale_sim = GetHistogram(simhistfile, 'hM_ClusEtaPV_EtaDepADCCut_constscale30_etascale{}'.format(esstr))
        
        Nclus_data = l_hM_ClusEtaPV_ClusADC_all_zoomin_data_adccut_escale[i].Integral(-1,-1,-1,-1)
        Nclus_sim = l_hM_ClusEtaPV_ClusADC_all_zoomin_sim_adccut_escale[i].Integral(-1,-1,-1,-1)
        fl = (NClus_total_data - Nclus_data) / NClus_total_data
        fp = Nclus_sim / NClus_total_sim
        fraclost_etascale_data.append(fl)
        fracpreserve_etascale_sim.append(fp)
        
        Draw_1Dhist_datasimcomp(hM_ClusEtaPV_EtaDepADCCut_etascale_data, [hM_ClusEtaPV_EtaDepADCCut_etascale_sim], [0.1,0.08,0.15,0.13], 'data', False, 1.8, 'Cluster #eta', '', False, ['Simulation'], ['Cluster ADC cut > 30#timescosh({:.2f}#times#eta)'.format(es)], plotdir+'/ClusADCCutOptim/Hist1D_ClusEtaPV/hM_ClusEtaPV_EtaDepADCCut_constscale30_etascale{}'.format(esstr))
        Draw_2Dhist_wFunc(l_hM_ClusEtaPV_ClusADC_all_zoomin_data_adccut_escale[i], hM_ClusEtaPV_ClusADC_all_zoomin_data, True, False, False, 0.08, 'Cluster #eta', 'Cluster ADC', '', 'col', 'box same', plotdir+'/ClusADCCutOptim/Hist2D_ClusEtaPVADC_wStepFunc/hM_ClusEtaPV_ClusADC_all_zoomin_data_wTF1Cosh_constscale30_etascale{}'.format(esstr))
        Draw_2Dhist_wFunc(l_hM_ClusEtaPV_ClusADC_all_zoomin_sim_adccut_escale[i], hM_ClusEtaPV_ClusADC_all_zoomin_sim, False, False, False, 0.08, 'Cluster #eta', 'Cluster ADC', '', 'col', 'box same', plotdir+'/ClusADCCutOptim/Hist2D_ClusEtaPVADC_wStepFunc/hM_ClusEtaPV_ClusADC_all_zoomin_sim_wTF1Cosh_constscale30_etascale{}'.format(esstr))
        
    
    fraclost_inttprivate_data = []
    fracpreserve_inttprivate_sim = []
    hM_ClusEtaPV_EtaDepADCCut_inttprivate_data = GetHistogram(datahistfile, 'hM_ClusEtaPV_EtaDepADCCut_inttprivate')
    hM_ClusEtaPV_EtaDepADCCut_inttprivate_sim = GetHistogram(simhistfile, 'hM_ClusEtaPV_EtaDepADCCut_inttprivate')
    hM_ClusEtaPV_ClusADC_all_zoomin_inttprivate_data = GetHistogram(datahistfile, 'hM_ClusEtaPV_ClusADC_all_zoomin_inttprivate')
    hM_ClusEtaPV_ClusADC_all_zoomin_inttprivate_sim = GetHistogram(simhistfile, 'hM_ClusEtaPV_ClusADC_all_zoomin_inttprivate')
    Nclus_data = hM_ClusEtaPV_ClusADC_all_zoomin_inttprivate_data.Integral(-1,-1,-1,-1)
    Nclus_sim = hM_ClusEtaPV_ClusADC_all_zoomin_inttprivate_sim.Integral(-1,-1,-1,-1)
    fl = (NClus_total_data - Nclus_data) / NClus_total_data
    fp = Nclus_sim / NClus_total_sim
    fraclost_inttprivate_data.append(fl)
    fracpreserve_inttprivate_sim.append(fp)
    Draw_1Dhist_datasimcomp(hM_ClusEtaPV_EtaDepADCCut_inttprivate_data, [hM_ClusEtaPV_EtaDepADCCut_inttprivate_sim], [0.1,0.08,0.15,0.13], 'data', False, 1.8, 'Cluster #eta', '', False, ['Simulation'], ['Cluster ADC cut (INTT private study)'], plotdir+'/ClusADCCutOptim/Hist1D_ClusEtaPV/hM_ClusEtaPV_EtaDepADCCut_inttprivate')
    hM_ClusEtaPV_ClusADC_all_zoomin_inttprivate_data = GetHistogram(datahistfile, 'hM_ClusEtaPV_ClusADC_all_zoomin_inttprivate')
    hM_ClusEtaPV_ClusADC_all_zoomin_inttprivate_sim = GetHistogram(simhistfile, 'hM_ClusEtaPV_ClusADC_all_zoomin_inttprivate')
    Draw_2Dhist_wFunc(hM_ClusEtaPV_ClusADC_all_zoomin_inttprivate_data, hM_ClusEtaPV_ClusADC_all_zoomin_data, True, False, False, 0.08, 'Cluster #eta', 'Cluster ADC', '', 'col', 'box same', plotdir+'/ClusADCCutOptim/Hist2D_ClusEtaPVADC_wStepFunc/hM_ClusEtaPV_ClusADC_all_zoomin_inttprivate_data_wTF1Cosh')
    Draw_2Dhist_wFunc(hM_ClusEtaPV_ClusADC_all_zoomin_inttprivate_sim, hM_ClusEtaPV_ClusADC_all_zoomin_sim, False, False, False, 0.08, 'Cluster #eta', 'Cluster ADC', '', 'col', 'box same', plotdir+'/ClusADCCutOptim/Hist2D_ClusEtaPVADC_wStepFunc/hM_ClusEtaPV_ClusADC_all_zoomin_inttprivate_sim_wTF1Cosh')  
                   
                   
                   
                   
    # print('Best scale = {}, with minimum distance to (1,1) = {:.4f}, fraction lost in data = {:.4f}, fraction preserved in sim = {:.4f}'.format(bestscale, mindistto1, bestfraclos_data, bestfracpreserve_sim))
    
    # Draw TGraph for the fraction of lost clusters, X-axis is data and Y-axis is sim
    # Get the maximum and minimum from the lists
    minfraclost = min(min(fraclost_constscale_data), min(fraclost_etascale_data), min(fraclost_inttprivate_data))
    maxfraclost = max(max(fraclost_constscale_data), max(fraclost_etascale_data), max(fraclost_inttprivate_data))
    minfracpreserve = min(min(fracpreserve_constscale_sim), min(fracpreserve_etascale_sim), min(fracpreserve_inttprivate_sim))
    maxfracpreserve = max(max(fracpreserve_constscale_sim), max(fracpreserve_etascale_sim), max(fracpreserve_inttprivate_sim))
    
    g_fraclost_constscale = TGraph(len(fraclost_constscale_data), array('d', fraclost_constscale_data), array('d', fracpreserve_constscale_sim))
    g_fraclost_etascale = TGraph(len(fraclost_etascale_data), array('d', fraclost_etascale_data), array('d', fracpreserve_etascale_sim))
    g_fraclost_inttprivate = TGraph(len(fraclost_inttprivate_data), array('d', fraclost_inttprivate_data), array('d', fracpreserve_inttprivate_sim))
    c_fraclost = TCanvas('c_fraclost', 'c_fraclost', 800, 700)
    c_fraclost.cd()
    c_fraclost.SetTopMargin(TopMargin)
    c_fraclost.SetLeftMargin(LeftMargin)
    c_fraclost.SetBottomMargin(BottomMargin)
    c_fraclost.SetRightMargin(RightMargin)
    g_fraclost_inttprivate.SetMarkerStyle(20)
    g_fraclost_inttprivate.SetMarkerSize(1)
    g_fraclost_inttprivate.SetMarkerColor(kBlack)
    g_fraclost_inttprivate.SetLineColor(kBlack)
    g_fraclost_inttprivate.SetLineWidth(2)
    g_fraclost_inttprivate.GetXaxis().SetTitle('Fraction of clusters excluded in data')
    g_fraclost_inttprivate.GetYaxis().SetTitle('Fraction of clusters perserved in sim')
    g_fraclost_inttprivate.GetXaxis().SetTitleSize(AxisTitleSize)
    g_fraclost_inttprivate.GetYaxis().SetTitleSize(AxisTitleSize)
    g_fraclost_inttprivate.GetXaxis().SetLabelSize(AxisLabelSize)
    g_fraclost_inttprivate.GetYaxis().SetLabelSize(AxisLabelSize)
    g_fraclost_inttprivate.GetXaxis().SetTitleOffset(1.1)
    g_fraclost_inttprivate.GetYaxis().SetTitleOffset(1.3)
    g_fraclost_inttprivate.GetXaxis().SetTickSize(TickSize)
    g_fraclost_inttprivate.GetYaxis().SetTickSize(TickSize)
    g_fraclost_inttprivate.GetXaxis().SetLimits(0, maxfraclost+0.03)
    g_fraclost_inttprivate.GetHistogram().SetMinimum(minfracpreserve-0.03)
    g_fraclost_inttprivate.GetHistogram().SetMaximum(1.02)    
    g_fraclost_inttprivate.Draw('AP')
    g_fraclost_constscale.SetMarkerStyle(20)
    g_fraclost_constscale.SetMarkerSize(1)
    g_fraclost_constscale.SetMarkerColor(TColor.GetColor('#0F4C75'))
    g_fraclost_constscale.SetLineColor(TColor.GetColor('#0F4C75'))
    g_fraclost_constscale.SetLineWidth(2)
    g_fraclost_constscale.Draw('P same')
    g_fraclost_etascale.SetMarkerStyle(20)
    g_fraclost_etascale.SetMarkerSize(1)
    g_fraclost_etascale.SetMarkerColor(TColor.GetColor('#810000'))
    g_fraclost_etascale.SetLineColor(TColor.GetColor('#810000'))
    g_fraclost_etascale.SetLineWidth(2)
    g_fraclost_etascale.Draw('P same')
    # legend
    leg = TLegend(0.18, 0.17, 0.4, 0.35)
    leg.SetHeader('Cluster ADC cut = c#timescosh(b#eta)')
    leg.SetTextSize(0.04)
    leg.SetFillStyle(0)
    leg.AddEntry(g_fraclost_constscale, 'Vary constant scale c (fix #eta scale)', 'p')
    leg.AddEntry(g_fraclost_etascale, 'Vary #eta scale b (fix constant scale)', 'p')
    leg.AddEntry(g_fraclost_inttprivate, 'INTT private study', 'p')
    leg.Draw()
    c_fraclost.Draw()
    c_fraclost.SaveAs(plotdir+'/ClusADCCutOptim/g_fraclost.pdf')
    c_fraclost.SaveAs(plotdir+'/ClusADCCutOptim/g_fraclost.png')
    
    
        