import ROOT
from ROOT import TCanvas, TFile, TProfile, TNtuple, TH1I, TH1F, TH2F, TH3F, TColor, TEfficiency
from ROOT import gROOT, gBenchmark, gRandom, gSystem
import numpy as np
import pdb

gROOT.LoadMacro("/sphenix/u/egm2153/spring_2023/sPhenixStyle.C");
gROOT.ProcessLine("SetsPhenixStyle()")

leg_tags = ['0-2','2-3','3-4','4-5','5-6','6-7','7-10','10-20']
rgb = [[230, 25, 75], [60, 180, 75], [255, 225, 25], [0, 130, 200], [245, 130, 48], [145, 30, 180], [70, 240, 240], [240, 50, 230], [210, 245, 60], [250, 190, 212], [0, 128, 128], [220, 190, 255], [170, 110, 40], [128, 128, 128], [128, 0, 0], [0, 0, 0], [128, 128, 0], [255, 215, 180], [0, 0, 128], [34, 139, 34]]
colors = [TColor.GetColor(rgb[i][0],rgb[i][1],rgb[i][2]) for i in range(len(rgb))]

files = ['output_0mrad_lumi/pileup_0mrad_UE_analysis_w_lumi_correction_0.020000.root',
		 'output_0mrad_lumi/pileup_0mrad_UE_analysis_w_lumi_correction_0.030000.root',
		 'output_0mrad_lumi/pileup_0mrad_UE_analysis_w_lumi_correction_0.040000.root',
		 'output_0mrad_lumi/pileup_0mrad_UE_analysis_w_lumi_correction_0.050000.root',
		 'output_0mrad_lumi/pileup_0mrad_UE_analysis_w_lumi_correction_0.060000.root',
		 'output_0mrad_lumi/pileup_0mrad_UE_analysis_w_lumi_correction_0.070000.root',
		 'output_0mrad_lumi/pileup_0mrad_UE_analysis_w_lumi_correction_0.100000.root',
		 'output_0mrad_lumi/pileup_0mrad_UE_analysis_w_lumi_correction_0.200000.root']

h_jetspectra = []
h_deltaphi = []
h_xj = []

for i, file in enumerate(files):
	f1 = ROOT.TFile.Open(file)
	h_jetspectra.append(TH1F(f1.Get("h_leadjet")))
	h_deltaphi.append(TH1F(f1.Get("h_deltaphi")))
	h_xj.append(TH1F(f1.Get("h_pass_xj")))
	h_jetspectra[i].Scale(1.0/h_deltaphi[i].GetEntries())
	h_deltaphi[i].Scale(1.0/h_deltaphi[i].Integral())
	h_xj[i].Scale(1.0/h_xj[i].Integral())
	h_jetspectra[i].SetDirectory(0)
	h_deltaphi[i].SetDirectory(0)
	h_xj[i].SetDirectory(0)
	f1.Close()

canvas = ROOT.TCanvas("canvas", "", 600, 800)
pad1 = ROOT.TPad("pad1", "", 0, 0.4, 1, 1.0)
pad1.SetBottomMargin(0.02)  # Adjust the margin for better separation
pad1.Draw()
pad1.cd()
pad1.SetLogy(1)  # Set logarithmic scale for the spectra plot

# Customize the histograms (color, marker, etc.)
for i, h in enumerate(h_jetspectra):
	h.SetStats(0)
	h.SetLineColor(colors[i])
	h.SetMarkerColor(colors[i])
	h.GetXaxis().SetLabelSize(0)
	h.GetXaxis().SetRangeUser(10,50)

	if i == 0:
		h.SetYTitle("1/N*lumi_{18} dN/dp_{T,lead}")
		h.Draw()
	else:
		h.Draw("same")

# Add legend
leg = ROOT.TLegend(.57, .65, .9, .9)
leg.AddEntry("","Dijet Events |#Delta#phi| > 2.75","")
for i, h in enumerate(h_jetspectra):
	leg.AddEntry(h,leg_tags[i]+"% <PU>","pl")
leg.Draw()
leg.SetTextSize(0.035)

canvas.cd()
pad2 = ROOT.TPad("pad2", "", 0, 0.05, 1, 0.4)
pad2.SetTopMargin(0.02)
pad2.SetBottomMargin(0.4)
pad2.Draw()
pad2.cd()

ratio = []
for i in range(4, len(h_jetspectra)):
	ratio.append(h_jetspectra[i].Clone(f"ratio{i}"))
	ratio[-1].Divide(h_jetspectra[3])

ratio[0].GetYaxis().SetTitle("PU/4-5% PU Ratio")
ratio[0].GetYaxis().SetNdivisions(208)
ratio[0].GetYaxis().SetRangeUser(0.9,1.3)
ratio[0].GetYaxis().SetTitleSize(25)
ratio[0].GetYaxis().SetTitleFont(43)
ratio[0].GetYaxis().SetTitleOffset(1.5)
ratio[0].GetYaxis().SetLabelFont(43)
ratio[0].GetYaxis().SetLabelSize(25)
ratio[0].GetXaxis().SetTitle("p_{T,lead reco jet} [GeV]")
ratio[0].GetXaxis().SetTitleSize(25)
ratio[0].GetXaxis().SetTitleFont(43)
ratio[0].GetXaxis().SetTitleOffset(0)
ratio[0].GetXaxis().SetLabelFont(43)
ratio[0].GetXaxis().SetLabelSize(25)

# Draw ratio plots
for i in range(len(ratio)):
	if i == 0:
		ratio[i].Draw("ep")
	else:
		ratio[i].Draw("ep,same")

# Update canvas
canvas.Update()
canvas.Draw()
canvas.SaveAs("0mrad_plots/h_jetspectra_pileup_lumi_corrected.png")
'''
canvas = ROOT.TCanvas("canvas","",600,500)
leg = ROOT.TLegend(.5,.65,.85,.89)
leg.AddEntry(h_deltaphi,"Jet Trig. Data","l")
leg.AddEntry(h_mc_deltaphi,"Run 22 Jet 10","l")
leg.SetTextSize(0.03)
h_deltaphi.SetStats(0)
h_mc_deltaphi.SetStats(0)
h_clus_deltaphi.SetStats(0)
h_nz_deltaphi.SetStats(0)
h_mc_deltaphi.SetLineColor(2)
h_mc_deltaphi.SetMarkerColor(2)
h_clus_deltaphi.SetLineColor(1)
h_clus_deltaphi.SetMarkerColor(1)
h_nz_deltaphi.SetLineColor(2)
h_nz_deltaphi.SetMarkerColor(2)
h_mc_deltaphi.GetYaxis().SetRangeUser(0,0.16)
h_mc_deltaphi.GetXaxis().SetRangeUser(-3.2,3.2)
h_mc_deltaphi.Draw('hist')
#h_clus_deltaphi.Draw('hist,same')
#h_nz_deltaphi.Draw('hist,same')
h_deltaphi.Draw('hist,same')
h_mc_deltaphi.SetXTitle("#Delta#phi") 
canvas.SetLogy(0)
leg.Draw()
canvas.Draw()
canvas.SaveAs("0mrad_plots/h_deltaphi.png")

canvas = ROOT.TCanvas("canvas","",600,500)
leg = ROOT.TLegend(.17,.67,.45,.92)
leg.AddEntry("","Dijet Events |#Delta#phi| > 2.75","")
leg.AddEntry(h_xj,"Jet Trig. Data","lp")
leg.AddEntry(h_mc_xj,"Run 22 Jet 10","lp")
leg.SetTextSize(0.04)
h_xj.SetStats(0)
h_mc_xj.SetStats(0)
h_clus_xj.SetStats(0)
h_nz_xj.SetStats(0)
h_mc_xj.SetLineColor(2)
h_mc_xj.SetMarkerColor(2)
h_clus_xj.SetLineColor(1)
h_clus_xj.SetMarkerColor(1)
h_nz_xj.SetLineColor(2)
h_nz_xj.SetMarkerColor(2)
h_mc_xj.Draw()
#h_clus_xj.Draw('same')
#h_nz_xj.Draw('same')
h_xj.Draw('same')
h_mc_xj.SetXTitle("x_{j}") 
canvas.SetLogy(0)
leg.Draw()
canvas.Draw()
canvas.SaveAs("0mrad_plots/h_xj.png")
'''