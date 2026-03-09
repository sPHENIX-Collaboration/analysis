import ROOT
from ROOT import TCanvas, TFile, TProfile, TNtuple, TH1I, TH1F, TH2F, TH3F, TColor, TEfficiency
from ROOT import gROOT, gBenchmark, gRandom, gSystem
import numpy as np
import pdb
from array import array

def mc_excess_func(x):
    return f_ue_away.Eval(x) - f_mc_ue_away.Eval(x)

def data_excess_func(x):
    return f_pu_ue_away.Eval(x) - f_mc_pu_ue_away.Eval(x)

ROOT.Math.MinimizerOptions.SetDefaultMinimizer("Minuit2")
ROOT.Math.MinimizerOptions.SetDefaultTolerance(0.01)
ROOT.Math.MinimizerOptions.SetDefaultMaxFunctionCalls(10000)
ROOT.Math.MinimizerOptions.SetDefaultMaxIterations(10000)

gROOT.LoadMacro("/sphenix/u/egm2153/spring_2023/sPhenixStyle.C");
gROOT.ProcessLine("SetsPhenixStyle()")

pileup_range = ["0.020000","0.030000","0.040000","0.050000","0.060000","0.070000","0.100000",]
pileup_range_leg = ['0-2% <PU>','2-3% <PU>','3-4% <PU>','4-5% <PU>','5-6% <PU>','6-7% <PU>','7-10% <PU>']

for ipu, pileup in enumerate(pileup_range):
    f2 = ROOT.TFile.Open("/sphenix/user/egm2153/calib_study/analysis/UE_in_pp/analysis/jet_calo_analysis_run22_jet10_3sigma_topo_efraccut_truthmatch.root")
    h_ue_away = TH1F(f2.Get("h_ue_away"))
    #h_ue_away.Scale(1.0/h_ue_away.GetEntries())
    h_ue_away.SetDirectory(0)
    f2.Close()
    
    f2 = ROOT.TFile.Open("/sphenix/user/egm2153/calib_study/analysis/UE_in_pp/analysis/jet_calo_analysis_run22_jet10_3sigma_topo_dijetcut_truthmatch.root")
    h_mc_ue_away = TH1F(f2.Get("h_ue_away"))
    #h_mc_ue_away.Scale(1.0/h_mc_ue_away.GetEntries())
    h_mc_ue_away.SetDirectory(0)
    f2.Close()
    
    f2 = ROOT.TFile.Open("/sphenix/user/egm2153/calib_study/analysis/UE_in_pp/analysis/pileup/pileup_inclusive_jet_0mrad_UE_analysis_leadjet_15_20_GeV_efrac_bkg_cut_maxrate_"+pileup+".root")
    h_pu_ue_away = TH1F(f2.Get("h_ue_away"))
    #h_pu_ue_away.Scale(1.0/h_pu_ue_away.GetEntries())
    h_pu_ue_away.SetDirectory(0)
    f2.Close()
    
    f2 = ROOT.TFile.Open("/sphenix/user/egm2153/calib_study/analysis/UE_in_pp/analysis/pileup/pileup_inclusive_jet_0mrad_UE_analysis_leadjet_15_20_GeV_dijet_bkg_cut_maxrate_"+pileup+".root")
    h_mc_pu_ue_away = TH1F(f2.Get("h_ue_away"))
    #h_pu_ue_away.Scale(1.0/h_pu_ue_away.GetEntries())
    h_mc_pu_ue_away.SetDirectory(0)
    f2.Close()
    
    print(h_ue_away.GetNbinsX(), h_mc_ue_away.GetNbinsX())
    
    total_scale = 0
    signal_scale = 0
    for i in range(228,291):
        total_scale += h_ue_away.GetBinContent(i)
        signal_scale += h_mc_ue_away.GetBinContent(i)
    
    print(total_scale, signal_scale)
    h_ue_away.Scale(1.0/total_scale)
    h_mc_ue_away.Scale(1.0/signal_scale) 
    
    data_total_scale = 0
    data_signal_scale = 0
    for i in range(228,291):
        data_total_scale += h_pu_ue_away.GetBinContent(i)
        data_signal_scale += h_mc_pu_ue_away.GetBinContent(i)
    
    #data_total_scale = data_signal_scale*total_scale/signal_scale
    print(data_total_scale, data_signal_scale)
    h_pu_ue_away.Scale(1.0/data_total_scale)
    h_mc_pu_ue_away.Scale(1.0/data_signal_scale) 
    
    mc_max = h_ue_away.GetBinContent(221)
    data_max = h_pu_ue_away.GetBinContent(221)
    
    print(mc_max, data_max)
    
    h_ue_away.Scale(1.0/(mc_max))
    h_mc_ue_away.Scale(1.0/(mc_max))
    h_pu_ue_away.Scale(1.0/(data_max))
    h_mc_pu_ue_away.Scale(1.0/(data_max))
    
    canvas = ROOT.TCanvas("canvas","",600,500)
    leg = ROOT.TLegend(.55,.65,.92,.92)
    leg.AddEntry("","#bf{Away Region}","")
    leg.AddEntry(h_ue_away,"Run22 Jet10 E Frac Cut","pl")
    leg.AddEntry(h_mc_ue_away,"Run22 Jet10 Dijet Cut","pl")
    h_ue_away.SetStats(0)
    h_mc_ue_away.SetStats(0)
    h_pu_ue_away.SetStats(0)
    h_mc_ue_away.SetLineColor(2)
    h_mc_ue_away.SetMarkerColor(2)
    h_ue_away.GetXaxis().SetRangeUser(-1,12)
    h_ue_away.Draw('hist')
    h_mc_ue_away.Draw('hist,same')
    h_ue_away.SetXTitle("#SigmaE_{T}/#delta#eta#delta#phi [GeV]") 
    canvas.SetLogy(0)
    leg.SetTextSize(0.035)
    leg.Draw()
    canvas.Draw()
    canvas.SaveAs("0mrad_plots/h_ue_away_for_norm_fits_mc_dijet_vs_efrac.png")
    
    canvas = ROOT.TCanvas("canvas","",600,500)
    leg = ROOT.TLegend(.55,.65,.92,.92)
    leg.AddEntry("","#bf{Away Region}","")
    leg.AddEntry("",pileup_range_leg[ipu],"")
    leg.AddEntry(h_pu_ue_away,"Data E Frac Cut","pl")
    leg.AddEntry(h_mc_pu_ue_away,"Data Dijet Cut","pl")
    h_mc_pu_ue_away.SetStats(0)
    h_pu_ue_away.SetStats(0)
    h_mc_pu_ue_away.SetLineColor(2)
    h_mc_pu_ue_away.SetMarkerColor(2)
    h_pu_ue_away.GetXaxis().SetRangeUser(-1,12)
    h_pu_ue_away.Draw('hist')
    h_mc_pu_ue_away.Draw('hist,same')
    h_pu_ue_away.SetXTitle("#SigmaE_{T}/#delta#eta#delta#phi [GeV]") 
    canvas.SetLogy(0)
    leg.SetTextSize(0.035)
    leg.Draw()
    canvas.Draw()
    canvas.SaveAs("0mrad_plots/h_ue_away_for_norm_fits_data_dijet_vs_efrac_maxrate_"+pileup+".png")
    
    g_ue_away = ROOT.TGraph(h_ue_away)
    g_mc_ue_away = ROOT.TGraph(h_mc_ue_away)
    g_pu_ue_away = ROOT.TGraph(h_pu_ue_away)
    g_mc_pu_ue_away = ROOT.TGraph(h_mc_pu_ue_away)
    
    f_ue_away = ROOT.TF1("f_ue_away", lambda x, _: g_ue_away.Eval(float(x[0]),0,"S"), 0, 9)
    f_mc_ue_away = ROOT.TF1("f_mc_ue_away", lambda x, _: g_mc_ue_away.Eval(float(x[0]),0,"S"), 0, 9)
    f_mc_excess = ROOT.TF1("", lambda x, _: mc_excess_func(x[0]), 0, 3.5)   
    
    f_pu_ue_away = ROOT.TF1("f_pu_ue_away", lambda x, _: g_pu_ue_away.Eval(float(x[0]),0,"S"), 0, 9)
    f_mc_pu_ue_away = ROOT.TF1("f_mc_pu_ue_away", lambda x, _: g_mc_pu_ue_away.Eval(float(x[0]),0,"S"), 0, 9)
    f_data_excess = ROOT.TF1("", lambda x, _: data_excess_func(x[0]), 0, 3.5)
    
    c = ROOT.TCanvas("c2", "TF1 from TH1F", 800, 600)
    f_ue_away.SetLineColor(4)
    f_mc_ue_away.SetLineColor(4)
    f_mc_excess.SetLineColor(6)
    h_ue_away.GetXaxis().SetRangeUser(-1,12)
    h_ue_away.Draw("HIST")  # Draw histogram first
    h_mc_ue_away.Draw("HIST,SAME")
    f_ue_away.Draw("SAME")  # Draw interpolated function over it
    f_mc_ue_away.Draw("SAME")  # Draw second function (may not match histogram drawn)
    f_mc_excess.Draw("SAME")
    
    # Add legend for clarity
    legend = ROOT.TLegend(0.5, 0.65, 0.88, 0.88)
    legend.AddEntry(h_ue_away,    "Run22 Jet10 E Frac Cut Hist","l")
    legend.AddEntry(h_mc_ue_away, "Run22 Jet10 Dijet Cut Hist", "l")
    legend.AddEntry(f_ue_away,    "Run22 Jet10 E Frac Cut TF1", "l")
    legend.AddEntry(f_mc_ue_away, "Run22 Jet10 Dijet Cut TF1", "l")
    legend.AddEntry(f_mc_excess,  "Run22 Jet10 E Frac - Dijet TF1 ", "l")
    legend.Draw()
    c.SetLogy(0)
    c.Update()
    c.Draw()
    c.SaveAs("0mrad_plots/normalized_mc_template_fits_dijet_vs_efrac.png")
    
    c = ROOT.TCanvas("c3", "TF1 from TH1F", 800, 600)
    f_pu_ue_away.SetLineColor(4)
    f_mc_pu_ue_away.SetLineColor(4)
    f_data_excess.SetLineColor(6)
    h_pu_ue_away.GetXaxis().SetRangeUser(-1,12)
    h_pu_ue_away.Draw("HIST")  # Draw histogram first
    h_mc_pu_ue_away.Draw("HIST,SAME")
    f_pu_ue_away.Draw("SAME")  # Draw interpolated function over it
    f_mc_pu_ue_away.Draw("SAME")  # Draw second function (may not match histogram drawn)
    f_data_excess.Draw("SAME")
    
    # Add legend for clarity
    legend = ROOT.TLegend(0.5, 0.65, 0.88, 0.88)
    legend.AddEntry("",pileup_range_leg[ipu],"")
    legend.AddEntry(h_pu_ue_away,    "Data E Frac Cut Hist","l")
    legend.AddEntry(h_mc_pu_ue_away, "Data Dijet Cut Hist", "l")
    legend.AddEntry(f_pu_ue_away,    "Data E Frac Cut TF1", "l")
    legend.AddEntry(f_mc_pu_ue_away, "Data Dijet Cut TF1", "l")
    legend.AddEntry(f_data_excess,   "Data E Frac - Dijet TF1 ", "l")
    legend.Draw()
    c.SetLogy(0)
    c.Update()
    c.Draw()
    c.SaveAs("0mrad_plots/normalized_data_template_fits_dijet_vs_efrac_maxrate_"+pileup+".png")
    
    c = ROOT.TCanvas("c4", "TF1 from TH1F", 800, 600)
    f_mc_excess.SetLineColor(2)
    f_data_excess.SetLineColor(1)
    h_pu_ue_away.GetXaxis().SetRangeUser(-1,12)
    h_pu_ue_away.Draw("HIST")  # Draw histogram first
    h_mc_pu_ue_away.Draw("HIST,SAME")
    f_pu_ue_away.Draw("SAME")  # Draw interpolated function over it
    f_mc_pu_ue_away.Draw("SAME")  # Draw second function (may not match histogram drawn)
    f_mc_excess.Draw("SAME")
    f_data_excess.Draw("SAME")
    
    # Add legend for clarity
    legend = ROOT.TLegend(0.5, 0.65, 0.88, 0.88)
    legend.AddEntry("",pileup_range_leg[ipu],"")
    legend.AddEntry(f_pu_ue_away,    "Data E Frac Cut TF1", "l")
    legend.AddEntry(f_mc_pu_ue_away, "Data Dijet Cut TF1", "l")
    legend.AddEntry(f_mc_excess,   "Run22 Jet10 E Frac - Dijet TF1 ", "l")
    legend.AddEntry(f_data_excess,   "Data E Frac - Dijet TF1 ", "l")
    legend.Draw()
    c.SetLogy(0)
    c.Update()
    c.Draw()
    c.SaveAs("0mrad_plots/normalized_data_vs_mc_template_fits_dijet_vs_efrac_maxrate_"+pileup+".png")