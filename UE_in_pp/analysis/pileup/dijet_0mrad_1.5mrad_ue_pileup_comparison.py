import ROOT
from ROOT import TCanvas, TFile, TProfile, TNtuple, TH1I, TH1F, TH2F, TH3F, TColor, TEfficiency
from ROOT import gROOT, gBenchmark, gRandom, gSystem
import numpy as np
import pdb
from array import array

gROOT.LoadMacro("/sphenix/u/egm2153/spring_2023/sPhenixStyle.C")
gROOT.ProcessLine("SetsPhenixStyle()")

topo_thres = ['-9999','0','100','200','300','500']
thres_string = ['All E_{topo}','E_{topo} > 0 MeV','E_{topo} > 100 MeV','E_{topo} > 200 MeV','E_{topo} > 300 MeV','E_{topo} > 500 MeV']

leg_tags = ['0-2','2-3','3-4','4-5','5-6','6-7','7-10','10-20']
rgb = [[230, 25, 75], [60, 180, 75], [255, 225, 25], [0, 130, 200], [245, 130, 48], [145, 30, 180], [70, 240, 240], [240, 50, 230], [210, 245, 60], [250, 190, 212], [0, 128, 128], [220, 190, 255], [170, 110, 40], [128, 128, 128], [128, 0, 0], [0, 0, 0], [128, 128, 0], [255, 215, 180], [0, 0, 128], [34, 139, 34]]
colors = [TColor.GetColor(rgb[i][0],rgb[i][1],rgb[i][2]) for i in range(len(rgb))]

files_0mrad = ['output_0mrad_lumi/pileup_0mrad_UE_analysis_w_lumi_correction_leadjet_15_20_GeV_maxrate_0.020000.root',
               'output_0mrad_lumi/pileup_0mrad_UE_analysis_w_lumi_correction_leadjet_15_20_GeV_maxrate_0.030000.root',
               'output_0mrad_lumi/pileup_0mrad_UE_analysis_w_lumi_correction_leadjet_15_20_GeV_maxrate_0.040000.root',
               'output_0mrad_lumi/pileup_0mrad_UE_analysis_w_lumi_correction_leadjet_15_20_GeV_maxrate_0.050000.root',
               'output_0mrad_lumi/pileup_0mrad_UE_analysis_w_lumi_correction_leadjet_15_20_GeV_maxrate_0.060000.root',
               'output_0mrad_lumi/pileup_0mrad_UE_analysis_w_lumi_correction_leadjet_15_20_GeV_maxrate_0.070000.root',
               'output_0mrad_lumi/pileup_0mrad_UE_analysis_w_lumi_correction_leadjet_15_20_GeV_maxrate_0.080000.root',
               'output_0mrad_lumi/pileup_0mrad_UE_analysis_w_lumi_correction_leadjet_15_20_GeV_maxrate_0.090000.root',
               'output_0mrad_lumi/pileup_0mrad_UE_analysis_w_lumi_correction_leadjet_15_20_GeV_maxrate_0.100000.root']
               #'output_0mrad_lumi/pileup_0mrad_UE_analysis_w_lumi_correction_leadjet_15_20_GeV_maxrate_0.200000.root']

files_15mrad = ['pileup_UE_analysis_leadjet_15_20_GeV_rate_range_0.000000_0.020000.root',
                'pileup_UE_analysis_leadjet_15_20_GeV_rate_range_0.020000_0.030000.root',
                'pileup_UE_analysis_leadjet_15_20_GeV_rate_range_0.030000_0.040000.root',
                'pileup_UE_analysis_leadjet_15_20_GeV_rate_range_0.040000_0.050000.root']

files_0mrad = ['output_0mrad_lumi/pileup_0mrad_UE_analysis_w_lumi_correction_leadjet_15_20_GeV_zvtx_lt_10cm_maxrate_0.020000.root',
               'output_0mrad_lumi/pileup_0mrad_UE_analysis_w_lumi_correction_leadjet_15_20_GeV_zvtx_lt_10cm_maxrate_0.030000.root',
               'output_0mrad_lumi/pileup_0mrad_UE_analysis_w_lumi_correction_leadjet_15_20_GeV_zvtx_lt_10cm_maxrate_0.040000.root',
               'output_0mrad_lumi/pileup_0mrad_UE_analysis_w_lumi_correction_leadjet_15_20_GeV_zvtx_lt_10cm_maxrate_0.050000.root',
               'output_0mrad_lumi/pileup_0mrad_UE_analysis_w_lumi_correction_leadjet_15_20_GeV_zvtx_lt_10cm_maxrate_0.060000.root',
               'output_0mrad_lumi/pileup_0mrad_UE_analysis_w_lumi_correction_leadjet_15_20_GeV_zvtx_lt_10cm_maxrate_0.070000.root',
               'output_0mrad_lumi/pileup_0mrad_UE_analysis_w_lumi_correction_leadjet_15_20_GeV_zvtx_lt_10cm_maxrate_0.080000.root',
               'output_0mrad_lumi/pileup_0mrad_UE_analysis_w_lumi_correction_leadjet_15_20_GeV_zvtx_lt_10cm_maxrate_0.090000.root',
               'output_0mrad_lumi/pileup_0mrad_UE_analysis_w_lumi_correction_leadjet_15_20_GeV_zvtx_lt_10cm_maxrate_0.100000.root']
               #'output_0mrad_lumi/pileup_0mrad_UE_analysis_w_lumi_correction_leadjet_15_20_GeV_zvtx_lt_10cm_maxrate_0.200000.root']

files_15mrad = ['pileup_UE_analysis_leadjet_15_20_GeV_zvtx_lt_10cm_rate_range_0.000000_0.020000.root',
                'pileup_UE_analysis_leadjet_15_20_GeV_zvtx_lt_10cm_rate_range_0.020000_0.030000.root',
                'pileup_UE_analysis_leadjet_15_20_GeV_zvtx_lt_10cm_rate_range_0.030000_0.040000.root',
                'pileup_UE_analysis_leadjet_15_20_GeV_zvtx_lt_10cm_rate_range_0.040000_0.050000.root']

h_ue_towards = []
h_ue_transverse = []
h_ue_away = []
h_ntopo_towards = []
h_ntopo_transverse = []
h_ntopo_away = []
h_topo_towards = []
h_topo_transverse = []
h_topo_away = []

for i, file in enumerate(files_15mrad):
    f2 = ROOT.TFile.Open(file)
    events = f2.Get("h_pass_deltaphi").GetEntries()
    h_ue_towards.append(TH1F(f2.Get("h_ue_towards")))
    h_ue_transverse.append(TH1F(f2.Get("h_ue_transverse")))
    h_ue_away.append(TH1F(f2.Get("h_ue_away")))
    h_ue_towards[i].Rebin(2)
    h_ue_transverse[i].Rebin(2)
    h_ue_away[i].Rebin(2)
    h_ue_towards[i].Scale(1.0/h_ue_towards[i].Integral())
    h_ue_transverse[i].Scale(1.0/h_ue_transverse[i].Integral())
    h_ue_away[i].Scale(1.0/h_ue_away[i].Integral())
    h_ue_towards[i].SetDirectory(0)
    h_ue_transverse[i].SetDirectory(0)
    h_ue_away[i].SetDirectory(0)
    ntopo_towards = []
    ntopo_transverse = []
    ntopo_away = []
    topo_towards = []
    topo_transverse = []
    topo_away = []
    for j, t in enumerate(topo_thres):
        ntopo_towards.append(f2.Get('h_ntopo'+t+'_towards'))
        ntopo_transverse.append(f2.Get('h_ntopo'+t+'_transverse'))
        ntopo_away.append(f2.Get('h_ntopo'+t+'_away'))
        ntopo_towards[j].Scale(1.0/ntopo_towards[j].Integral())
        ntopo_transverse[j].Scale(1.0/ntopo_transverse[j].Integral())
        ntopo_away[j].Scale(1.0/ntopo_away[j].Integral())
        ntopo_towards[j].SetDirectory(0)
        ntopo_transverse[j].SetDirectory(0)
        ntopo_away[j].SetDirectory(0)
        topo_towards.append(f2.Get('h_topo'+t+'_towards'))
        topo_transverse.append(f2.Get('h_topo'+t+'_transverse'))
        topo_away.append(f2.Get('h_topo'+t+'_away'))
        topo_towards[j].Rebin(10)
        topo_transverse[j].Rebin(2)
        topo_away[j].Rebin(10)
        topo_towards[j].Scale(1.0/events)
        topo_transverse[j].Scale(1.0/events)
        topo_away[j].Scale(1.0/events)
        topo_towards[j].SetDirectory(0)
        topo_transverse[j].SetDirectory(0)
        topo_away[j].SetDirectory(0)
    h_ntopo_towards.append(ntopo_towards)
    h_ntopo_transverse.append(ntopo_transverse)
    h_ntopo_away.append(ntopo_away)
    h_topo_towards.append(topo_towards)
    h_topo_transverse.append(topo_transverse)
    h_topo_away.append(topo_away)
    f2.Close()

h0_ue_towards = []
h0_ue_transverse = []
h0_ue_away = []
h0_ntopo_towards = []
h0_ntopo_transverse = []
h0_ntopo_away = []
h0_topo_towards = []
h0_topo_transverse = []
h0_topo_away = []

for i, file in enumerate(files_0mrad):
    f2 = ROOT.TFile.Open(file)
    events = f2.Get("h_pass_deltaphi").GetEntries()
    h0_ue_towards.append(TH1F(f2.Get("h_ue_towards")))
    h0_ue_transverse.append(TH1F(f2.Get("h_ue_transverse")))
    h0_ue_away.append(TH1F(f2.Get("h_ue_away")))
    h0_ue_towards[i].Rebin(2)
    h0_ue_transverse[i].Rebin(2)
    h0_ue_away[i].Rebin(2)
    h0_ue_towards[i].Scale(1.0/h0_ue_towards[i].Integral())
    h0_ue_transverse[i].Scale(1.0/h0_ue_transverse[i].Integral())
    h0_ue_away[i].Scale(1.0/h0_ue_away[i].Integral())
    h0_ue_towards[i].SetDirectory(0)
    h0_ue_transverse[i].SetDirectory(0)
    h0_ue_away[i].SetDirectory(0)
    ntopo_towards = []
    ntopo_transverse = []
    ntopo_away = []
    topo_towards = []
    topo_transverse = []
    topo_away = []
    for j, t in enumerate(topo_thres):
        ntopo_towards.append(f2.Get('h_ntopo'+t+'_towards'))
        ntopo_transverse.append(f2.Get('h_ntopo'+t+'_transverse'))
        ntopo_away.append(f2.Get('h_ntopo'+t+'_away'))
        ntopo_towards[j].Scale(1.0/ntopo_towards[j].Integral())
        ntopo_transverse[j].Scale(1.0/ntopo_transverse[j].Integral())
        ntopo_away[j].Scale(1.0/ntopo_away[j].Integral())
        ntopo_towards[j].SetDirectory(0)
        ntopo_transverse[j].SetDirectory(0)
        ntopo_away[j].SetDirectory(0)
        topo_towards.append(f2.Get('h_topo'+t+'_towards'))
        topo_transverse.append(f2.Get('h_topo'+t+'_transverse'))
        topo_away.append(f2.Get('h_topo'+t+'_away'))
        topo_towards[j].Rebin(10)
        topo_transverse[j].Rebin(2)
        topo_away[j].Rebin(10)
        topo_towards[j].Scale(1.0/events)
        topo_transverse[j].Scale(1.0/events)
        topo_away[j].Scale(1.0/events)
        topo_towards[j].SetDirectory(0)
        topo_transverse[j].SetDirectory(0)
        topo_away[j].SetDirectory(0)
    h0_ntopo_towards.append(ntopo_towards)
    h0_ntopo_transverse.append(ntopo_transverse)
    h0_ntopo_away.append(ntopo_away)
    h0_topo_towards.append(topo_towards)
    h0_topo_transverse.append(topo_transverse)
    h0_topo_away.append(topo_away)
    f2.Close()

mean_ntopo_towards = []
mean_ntopo_transverse = []
mean_ntopo_away = []
mean_topo_towards = []
mean_topo_transverse = []
mean_topo_away = []
std_ntopo_towards = []
std_ntopo_transverse = []
std_ntopo_away = []
std_topo_towards = []
std_topo_transverse = []
std_topo_away = []

for i in range(len(h_ntopo_towards)):
    for j in range(len(h_ntopo_towards[i])):
        mean_ntopo_towards.append([h_ntopo_towards[i][j].GetMean() for j in range(len(h_ntopo_towards[i]))])
        mean_ntopo_transverse.append([h_ntopo_transverse[i][j].GetMean() for j in range(len(h_ntopo_transverse[i]))])
        mean_ntopo_away.append([h_ntopo_away[i][j].GetMean() for j in range(len(h_ntopo_away[i]))])
        mean_topo_towards.append([h_topo_towards[i][j].GetMean() for j in range(len(h_topo_towards[i]))])
        mean_topo_transverse.append([h_topo_transverse[i][j].GetMean() for j in range(len(h_topo_transverse[i]))])
        mean_topo_away.append([h_topo_away[i][j].GetMean() for j in range(len(h_topo_away[i]))])
        std_ntopo_towards.append([h_ntopo_towards[i][j].GetMeanError() for j in range(len(h_ntopo_towards[i]))])
        std_ntopo_transverse.append([h_ntopo_transverse[i][j].GetMeanError() for j in range(len(h_ntopo_transverse[i]))])
        std_ntopo_away.append([h_ntopo_away[i][j].GetMeanError() for j in range(len(h_ntopo_away[i]))])
        std_topo_towards.append([h_topo_towards[i][j].GetMeanError() for j in range(len(h_topo_towards[i]))])
        std_topo_transverse.append([h_topo_transverse[i][j].GetMeanError() for j in range(len(h_topo_transverse[i]))])
        std_topo_away.append([h_topo_away[i][j].GetMeanError() for j in range(len(h_topo_away[i]))])
    
tm, te, trm, tre, am, ae = [], [], [], [], [], []
for i in range(len(h_ue_towards)):
    tm.append(h_ue_towards[i].GetMean())
    te.append(h_ue_towards[i].GetMeanError())
    trm.append(h_ue_transverse[i].GetMean())
    tre.append(h_ue_transverse[i].GetMeanError())
    am.append(h_ue_away[i].GetMean())
    ae.append(h_ue_away[i].GetMeanError())

tm0, te0, trm0, tre0, am0, ae0 = [], [], [], [], [], []
for i in range(len(h0_ue_towards)):
    tm0.append(h0_ue_towards[i].GetMean())
    te0.append(h0_ue_towards[i].GetMeanError())
    trm0.append(h0_ue_transverse[i].GetMean())
    tre0.append(h0_ue_transverse[i].GetMeanError())
    am0.append(h0_ue_away[i].GetMean())
    ae0.append(h0_ue_away[i].GetMeanError())

tpd = []
tpe = []
trpd = []
trpe = []
apd = []
ape = []

for i in range(1, len(tm)):
    tpd.append((tm[i] - tm[0]) /tm[0] * 100)
    tpe.append(100 * np.sqrt((te[i] / tm[0]) ** 2 + ((tm[i] * te[0]) / (tm[0] ** 2)) ** 2))
    trpd.append((trm[i] - trm[0]) /trm[0] * 100)
    trpe.append(100 * np.sqrt((tre[i] / trm[0]) ** 2 + ((trm[i] * tre[0]) / (trm[0] ** 2)) ** 2))
    apd.append((am[i] - am[0]) /am[0] * 100)
    ape.append(100 * np.sqrt((ae[i] / am[0]) ** 2 + ((am[i] * ae[0]) / (am[0] ** 2)) ** 2))

for i in range(len(tm)):
    print(f'{tm[i]:.3f} +/- {te[i]:.3f}')
print()
for i in range(len(trm)):
    print(f'{trm[i]:.3f} +/- {tre[i]:.3f}')
print()
for i in range(len(am)):
    print(f'{am[i]:.3f} +/- {ae[i]:.3f}')
print()
for i in range(len(tpd)):
    print(f'{tpd[i]:.3f} +/- {tpe[i]:.3f}')
print()
for i in range(len(trpd)):
    print(f'{trpd[i]:.3f} +/- {trpe[i]:.3f}')
print()
for i in range(len(apd)):
    print(f'{apd[i]:.3f} +/- {ape[i]:.3f}')

x_vals = array('f', [1.1, 2.6, 3.6, 4.6])
n_points = len(x_vals)
x_vals0 = array('f', [1.0, 2.5, 3.5, 4.5, 5.5, 6.5, 7.5, 8.5, 9.5])
n_points0 = len(x_vals0)

tm_arr = array('f', tm)
trm_arr = array('f', trm)
am_arr = array('f', am)
te_arr = array('f', te)
tre_arr = array('f', tre)
ae_arr = array('f', ae)
tm0_arr = array('f', tm0)
trm0_arr = array('f', trm0)
am0_arr = array('f', am0)
te0_arr = array('f', te0)
tre0_arr = array('f', tre0)
ae0_arr = array('f', ae0)

x_err = array('f', [0]*n_points)  # No x errors
x_err0 = array('f', [0]*n_points0) 

# Draw the graphs
c = ROOT.TCanvas("c8", "TGraphErrors Example", 800, 600)

g_tm = ROOT.TGraphErrors(n_points, x_vals, tm_arr, x_err, te_arr)
g_trm = ROOT.TGraphErrors(n_points, x_vals, trm_arr, x_err, tre_arr)
g_am = ROOT.TGraphErrors(n_points, x_vals, am_arr, x_err, ae_arr)
g_tm0 = ROOT.TGraphErrors(n_points0, x_vals0, tm0_arr, x_err0, te0_arr)
g_trm0 = ROOT.TGraphErrors(n_points0, x_vals0, trm0_arr, x_err0, tre0_arr)
g_am0 = ROOT.TGraphErrors(n_points0, x_vals0, am0_arr, x_err0, ae0_arr)

# Style the graphs
g_tm.SetMarkerStyle(20)
g_tm.SetMarkerColor(ROOT.kBlue)
g_tm.SetLineColor(ROOT.kBlue)

g_trm.SetMarkerStyle(20)
g_trm.SetMarkerColor(ROOT.kRed)
g_trm.SetLineColor(ROOT.kRed)

g_am.SetMarkerStyle(20)
g_am.SetMarkerColor(ROOT.kGreen+2)
g_am.SetLineColor(ROOT.kGreen+2)

g_tm0.SetMarkerStyle(24)
g_tm0.SetMarkerColor(ROOT.kBlue)
g_tm0.SetLineColor(ROOT.kBlue)

g_trm0.SetMarkerStyle(24)
g_trm0.SetMarkerColor(ROOT.kRed)
g_trm0.SetLineColor(ROOT.kRed)

g_am0.SetMarkerStyle(24)
g_am0.SetMarkerColor(ROOT.kGreen+2)
g_am0.SetLineColor(ROOT.kGreen+2)

#g_tm.GetYaxis().SetRangeUser(3.74,3.8)
g_tm0.GetXaxis().SetTitle("<PU> [%]")
g_tm0.GetYaxis().SetTitle("#SigmaE_{T}/#delta#eta#delta#phi [GeV]") 
g_tm0.GetYaxis().SetTitleOffset(1.6)

g_tm0.Draw("AP")
g_tm.Draw("P SAME")

# Add legend
legend = ROOT.TLegend(0.17, 0.8, 0.47, 0.92)
legend.AddEntry(g_tm, "Towards Region 1.5mrad", "lp")
legend.AddEntry(g_tm0, "Towards Region 0mrad", "lp")
legend.Draw()

c.Update()
c.Draw()
c.SaveAs("0mrad_plots/h_ue_pileup_towards_0mrad_1.5mrad_15_20GeV_zvtx_lt_10cm_Topoclusters.png")
# Draw the graphs
c = ROOT.TCanvas("c6", "TGraphErrors Example", 800, 600)

g_trm0.GetXaxis().SetTitle("<PU> [%]")
g_trm0.GetYaxis().SetTitle("#SigmaE_{T}/#delta#eta#delta#phi [GeV]") 
g_trm0.GetYaxis().SetTitleOffset(1.6)

g_trm0.Draw("AP")
g_trm.Draw("P same")

# Add legend
legend = ROOT.TLegend(0.17, 0.8, 0.47, 0.92)
legend.AddEntry(g_trm, "Transverse Region 1.5mrad", "lp")
legend.AddEntry(g_trm0, "Transverse Region 0mrad", "lp")
legend.Draw()

c.Update()
c.Draw()
c.SaveAs("0mrad_plots/h_ue_pileup_transverse_0mrad_1.5mrad_15_20GeV_zvtx_lt_10cm_Topoclusters.png")

# Draw the graphs
c = ROOT.TCanvas("c11", "TGraphErrors Example", 800, 600)

g_am0.GetXaxis().SetTitle("<PU> [%]")
g_am0.GetYaxis().SetTitle("#SigmaE_{T}/#delta#eta#delta#phi [GeV]") 
g_am0.GetYaxis().SetTitleOffset(1.6)
g_am0.GetYaxis().SetRangeUser(2.7,2.9)
g_am0.Draw("AP")
g_am.Draw("P SAME")

# Add legend
legend = ROOT.TLegend(0.17, 0.8, 0.47, 0.92)
legend.AddEntry(g_am, "Away Region 1.5mrad", "lp")
legend.AddEntry(g_am0, "Away Region 0mrad", "lp")
legend.Draw()

c.Update()
c.Draw()
c.SaveAs("0mrad_plots/h_ue_pileup_away_0mrad_1.5mrad_15_20GeV_zvtx_lt_10cm_Topoclusters.png")

'''

xlist0 = [-125,-25,75,175,275,475]
xlist1 = [-115,-15,85,185,285,485]
xlist2 = [-105,-5,95,195,295,495]
xlist3 = [-95,5,105,205,305,505]
xlist4 = [-85,15,115,215,315,515]
xlist5 = [-75,25,125,225,325,525]
xerr = [0,0,0,0,0,0]
x0 = np.array(xlist0, dtype='float64')
x1 = np.array(xlist1, dtype='float64')
x2 = np.array(xlist2, dtype='float64')
x3 = np.array(xlist3, dtype='float64')
x4 = np.array(xlist4, dtype='float64')
x5 = np.array(xlist5, dtype='float64')
# Create a canvas
canvas = ROOT.TCanvas("canvas2", "Graph", 500, 400)

# Define marker styles and colors
marker_styles = {
    "towards": ROOT.kFullCircle,
    "transverse": ROOT.kFullCircle,
    "away": ROOT.kFullCircle
}

colors = {
    "ntopo": ROOT.kBlack,
    "mc_ntopo": ROOT.kRed,
    "clus_ntopo": ROOT.kBlue,
    "nz_ntopo": ROOT.kMagenta,
    #"dt_ntopo": ROOT.kCyan
}

# Prepare a list of all data groups
data_groups = [
    ("mc_ntopo", x0, mean_mc_ntopo_towards, mean_mc_ntopo_transverse, mean_mc_ntopo_away, std_mc_ntopo_towards, std_mc_ntopo_transverse, std_mc_ntopo_away),
    ("clus_ntopo", x1, mean_clus_ntopo_towards, mean_clus_ntopo_transverse, mean_clus_ntopo_away, std_clus_ntopo_towards, std_clus_ntopo_transverse, std_clus_ntopo_away),
    ("nz_ntopo", x2, mean_nz_ntopo_towards, mean_nz_ntopo_transverse, mean_nz_ntopo_away, std_nz_ntopo_towards, std_nz_ntopo_transverse, std_nz_ntopo_away),
    #("dt_ntopo", x3, mean_dt_ntopo_towards, mean_dt_ntopo_transverse, mean_dt_ntopo_away, std_dt_ntopo_towards, std_dt_ntopo_transverse, std_dt_ntopo_away),
    ("ntopo", x4, mean_ntopo_towards, mean_ntopo_transverse, mean_ntopo_away, std_ntopo_towards, std_ntopo_transverse, std_ntopo_away)
]

graphs = []
    
# Create TGraphs for each data group
for group, x, towards, transverse, away, towards_width, transverse_width, away_width in data_groups:
    n_points = len(towards)

    # Create graphs for towards, transverse, and away
    graph_towards = ROOT.TGraphErrors(n_points, x, np.array(towards, dtype='float64'), np.array(xerr, dtype='float64'), np.array(towards_width, dtype='float64'))
    graph_transverse = ROOT.TGraphErrors(n_points, x, np.array(transverse, dtype='float64'), np.array(xerr, dtype='float64'), np.array(transverse_width, dtype='float64'))
    graph_away = ROOT.TGraphErrors(n_points, x, np.array(away, dtype='float64'), np.array(xerr, dtype='float64'), np.array(away_width, dtype='float64'))

    # Set marker styles and colors
    graph_towards.SetMarkerStyle(marker_styles["towards"])
    graph_transverse.SetMarkerStyle(marker_styles["transverse"])
    graph_away.SetMarkerStyle(marker_styles["away"])

    graph_towards.SetMarkerColor(colors[group])
    graph_transverse.SetMarkerColor(colors[group])
    graph_away.SetMarkerColor(colors[group])

    graph_towards.SetLineColor(colors[group])
    graph_transverse.SetLineColor(colors[group])
    graph_away.SetLineColor(colors[group])

    graphs.extend([graph_towards, graph_transverse, graph_away])

# Draw all graphs on the same canvas
for i, graph in enumerate(graphs):
    if i % 3 == 0:
        if i == 0:
            graph.Draw("AP")
            graph.GetYaxis().SetRangeUser(5,12)
            graph.GetXaxis().SetTitle("E_{topo} thres [MeV]")
            graph.GetYaxis().SetTitle("<N_{topo}>")
        else:
             graph.Draw("P SAME")

# Adding a legend
legend = ROOT.TLegend(0.53, 0.65, 0.92, 0.92)
legend.SetNColumns(1)
legend.SetTextSize(0.035)
legend.AddEntry("","#bf{Towards Region}","")
legend.AddEntry("","15 < p_{T,lead} < 20 GeV","")
legend.AddEntry(graphs[10], "<PU> < 1.5%", "pe")
legend.AddEntry(graphs[1], "1.5 < <PU> < 2.0%", "pe")
legend.AddEntry(graphs[4], "2.0 < <PU> < 2.8%", "pe")
legend.AddEntry(graphs[7], "<PU> > 2.8%", "pe")
#legend.AddEntry(graphs[10], "<PU> > 2.8%", "ep")
legend.Draw()

# Update and display the canvas
canvas.Update()
canvas.Draw()
canvas.SaveAs("/sphenix/u/egm2153/spring_2025/"+direct+"/h_mean_ntopo_15_20GeV_towards.png")
# Create a canvas
canvas = ROOT.TCanvas("canvas6", "Graph", 500, 400)

# Define marker styles and colors
marker_styles = {
    "towards": ROOT.kFullCircle,
    "transverse": ROOT.kFullCircle,
    "away": ROOT.kFullCircle
}

colors = {
    "ntopo": ROOT.kBlack,
    "mc_ntopo": ROOT.kRed,
    "clus_ntopo": ROOT.kBlue,
    "nz_ntopo": ROOT.kMagenta,
    #"dt_ntopo": ROOT.kCyan
}

# Prepare a list of all data groups
data_groups = [
    ("mc_ntopo", x0, mean_mc_ntopo_towards, mean_mc_ntopo_transverse, mean_mc_ntopo_away, std_mc_ntopo_towards, std_mc_ntopo_transverse, std_mc_ntopo_away),
    ("clus_ntopo", x1, mean_clus_ntopo_towards, mean_clus_ntopo_transverse, mean_clus_ntopo_away, std_clus_ntopo_towards, std_clus_ntopo_transverse, std_clus_ntopo_away),
    ("nz_ntopo", x2, mean_nz_ntopo_towards, mean_nz_ntopo_transverse, mean_nz_ntopo_away, std_nz_ntopo_towards, std_nz_ntopo_transverse, std_nz_ntopo_away),
    #("dt_ntopo", x3, mean_dt_ntopo_towards, mean_dt_ntopo_transverse, mean_dt_ntopo_away, std_dt_ntopo_towards, std_dt_ntopo_transverse, std_dt_ntopo_away),
    ("ntopo", x4, mean_ntopo_towards, mean_ntopo_transverse, mean_ntopo_away, std_ntopo_towards, std_ntopo_transverse, std_ntopo_away)
]

graphs = []
    
# Create TGraphs for each data group
for group, x, towards, transverse, away, towards_width, transverse_width, away_width in data_groups:
    n_points = len(towards)

    # Create graphs for towards, transverse, and away
    graph_towards = ROOT.TGraphErrors(n_points, x, np.array(towards, dtype='float64'), np.array(xerr, dtype='float64'), np.array(towards_width, dtype='float64'))
    graph_transverse = ROOT.TGraphErrors(n_points, x, np.array(transverse, dtype='float64'), np.array(xerr, dtype='float64'), np.array(transverse_width, dtype='float64'))
    graph_away = ROOT.TGraphErrors(n_points, x, np.array(away, dtype='float64'), np.array(xerr, dtype='float64'), np.array(away_width, dtype='float64'))

    # Set marker styles and colors
    graph_towards.SetMarkerStyle(marker_styles["towards"])
    graph_transverse.SetMarkerStyle(marker_styles["transverse"])
    graph_away.SetMarkerStyle(marker_styles["away"])

    graph_towards.SetMarkerColor(colors[group])
    graph_transverse.SetMarkerColor(colors[group])
    graph_away.SetMarkerColor(colors[group])

    graph_towards.SetLineColor(colors[group])
    graph_transverse.SetLineColor(colors[group])
    graph_away.SetLineColor(colors[group])

    graphs.extend([graph_towards, graph_transverse, graph_away])

# Draw all graphs on the same canvas
for i, graph in enumerate(graphs):
    if (i-1) % 3 == 0:
        if i == 1:
            graph.Draw("AP")
            graph.GetYaxis().SetRangeUser(0.5,5)
            graph.GetXaxis().SetTitle("E_{topo} thres [MeV]")
            graph.GetYaxis().SetTitle("<N_{topo}>")
        else:
             graph.Draw("P SAME")

# Adding a legend
legend = ROOT.TLegend(0.53, 0.65, 0.92, 0.92)
legend.SetNColumns(1)
legend.SetTextSize(0.035)
legend.AddEntry("","#bf{Transverse Region}","")
legend.AddEntry("","15 < p_{T,lead} < 20 GeV","")
legend.AddEntry(graphs[10], "<PU> < 1.5%", "pe")
legend.AddEntry(graphs[1], "1.5 < <PU> < 2.0%", "pe")
legend.AddEntry(graphs[4], "2.0 < <PU> < 2.8%", "pe")
legend.AddEntry(graphs[7], "<PU> > 2.8%", "pe")
#legend.AddEntry(graphs[10], "Run 150 (Detroit)", "ep")
legend.Draw()

# Update and display the canvas
canvas.Update()
canvas.Draw()
canvas.SaveAs("/sphenix/u/egm2153/spring_2025/"+direct+"/h_mean_ntopo_15_20GeV_transverse.png")
# Create a canvas
canvas = ROOT.TCanvas("canvas7", "Graph", 500, 400)

# Define marker styles and colors
marker_styles = {
    "towards": ROOT.kFullCircle,
    "transverse": ROOT.kFullCircle,
    "away": ROOT.kFullCircle
}

colors = {
    "ntopo": ROOT.kBlack,
    "mc_ntopo": ROOT.kRed,
    "clus_ntopo": ROOT.kBlue,
    "nz_ntopo": ROOT.kMagenta,
    #"dt_ntopo": ROOT.kCyan
}

# Prepare a list of all data groups
data_groups = [
    ("mc_ntopo", x0, mean_mc_ntopo_towards, mean_mc_ntopo_transverse, mean_mc_ntopo_away, std_mc_ntopo_towards, std_mc_ntopo_transverse, std_mc_ntopo_away),
    ("clus_ntopo", x1, mean_clus_ntopo_towards, mean_clus_ntopo_transverse, mean_clus_ntopo_away, std_clus_ntopo_towards, std_clus_ntopo_transverse, std_clus_ntopo_away),
    ("nz_ntopo", x2, mean_nz_ntopo_towards, mean_nz_ntopo_transverse, mean_nz_ntopo_away, std_nz_ntopo_towards, std_nz_ntopo_transverse, std_nz_ntopo_away),
    #("dt_ntopo", x3, mean_dt_ntopo_towards, mean_dt_ntopo_transverse, mean_dt_ntopo_away, std_dt_ntopo_towards, std_dt_ntopo_transverse, std_dt_ntopo_away),
    ("ntopo", x4, mean_ntopo_towards, mean_ntopo_transverse, mean_ntopo_away, std_ntopo_towards, std_ntopo_transverse, std_ntopo_away)
]

graphs = []
    
# Create TGraphs for each data group
for group, x, towards, transverse, away, towards_width, transverse_width, away_width in data_groups:
    n_points = len(towards)

    # Create graphs for towards, transverse, and away
    graph_towards = ROOT.TGraphErrors(n_points, x, np.array(towards, dtype='float64'), np.array(xerr, dtype='float64'), np.array(towards_width, dtype='float64'))
    graph_transverse = ROOT.TGraphErrors(n_points, x, np.array(transverse, dtype='float64'), np.array(xerr, dtype='float64'), np.array(transverse_width, dtype='float64'))
    graph_away = ROOT.TGraphErrors(n_points, x, np.array(away, dtype='float64'), np.array(xerr, dtype='float64'), np.array(away_width, dtype='float64'))

    # Set marker styles and colors
    graph_towards.SetMarkerStyle(marker_styles["towards"])
    graph_transverse.SetMarkerStyle(marker_styles["transverse"])
    graph_away.SetMarkerStyle(marker_styles["away"])

    graph_towards.SetMarkerColor(colors[group])
    graph_transverse.SetMarkerColor(colors[group])
    graph_away.SetMarkerColor(colors[group])

    graph_towards.SetLineColor(colors[group])
    graph_transverse.SetLineColor(colors[group])
    graph_away.SetLineColor(colors[group])

    graphs.extend([graph_towards, graph_transverse, graph_away])

# Draw all graphs on the same canvas
for i, graph in enumerate(graphs):
    if (i-2) % 3 == 0:
        if i == 2:
            graph.Draw("AP")
            graph.GetYaxis().SetRangeUser(4,14)
            graph.GetXaxis().SetTitle("E_{topo} thres [MeV]")
            graph.GetYaxis().SetTitle("<N_{topo}>")
        else:
             graph.Draw("P SAME")

# Adding a legend
legend = ROOT.TLegend(0.53, 0.65, 0.92, 0.92)
legend.SetNColumns(1)
legend.SetTextSize(0.035)
legend.AddEntry("","#bf{Away Region}","")
legend.AddEntry("","15 < p_{T,lead} < 20 GeV","")
legend.AddEntry(graphs[1], "1.5 < <PU> < 2.0%", "pe")
legend.AddEntry(graphs[4], "2.0 < <PU> < 2.8%", "pe")
legend.AddEntry(graphs[7], "<PU> > 2.8%", "pe")
#legend.AddEntry(graphs[10], "Run 150 (Detroit)", "ep")
legend.AddEntry(graphs[10], "<PU> < 1.5%", "pe")
legend.Draw()

# Update and display the canvas
canvas.Update()
canvas.Draw()
canvas.SaveAs("/sphenix/u/egm2153/spring_2025/"+direct+"/h_mean_ntopo_15_20GeV_away.png")


# Create a canvas
canvas = ROOT.TCanvas("canvas", "Graph", 500, 400)

# Define marker styles and colors
marker_styles = {
    "towards": ROOT.kFullCircle,
    "transverse": ROOT.kFullCircle,
    "away": ROOT.kFullCircle
}

colors = {
    "ntopo": ROOT.kBlack,
    "mc_ntopo": ROOT.kRed,
    "clus_ntopo": ROOT.kBlue,
    "nz_ntopo": ROOT.kMagenta,
    "dt_ntopo": ROOT.kCyan
}

# Prepare a list of all data groups
data_groups = [
    ("mc_ntopo", x0, mean_mc_topo_towards, mean_mc_topo_transverse, mean_mc_topo_away, std_mc_topo_towards, std_mc_topo_transverse, std_mc_topo_away),
    ("clus_ntopo", x1, mean_clus_topo_towards, mean_clus_topo_transverse, mean_clus_topo_away, std_clus_topo_towards, std_clus_topo_transverse, std_clus_topo_away),
    ("nz_ntopo", x2, mean_nz_topo_towards, mean_nz_topo_transverse, mean_nz_topo_away, std_nz_topo_towards, std_nz_topo_transverse, std_nz_topo_away),
    #("dt_ntopo", x3, mean_dt_topo_towards, mean_dt_topo_transverse, mean_dt_topo_away, std_dt_topo_towards, std_dt_topo_transverse, std_dt_topo_away),
    ("ntopo", x4, mean_topo_towards, mean_topo_transverse, mean_topo_away, std_topo_towards, std_topo_transverse, std_topo_away)
]

graphs = []
    
# Create TGraphs for each data group
for group, x, towards, transverse, away, towards_width, transverse_width, away_width in data_groups:
    n_points = len(towards)

    # Create graphs for towards, transverse, and away
    graph_towards = ROOT.TGraphErrors(n_points, x, np.array(towards, dtype='float64'), np.array(xerr, dtype='float64'), np.array(towards_width, dtype='float64'))
    graph_transverse = ROOT.TGraphErrors(n_points, x, np.array(transverse, dtype='float64'), np.array(xerr, dtype='float64'), np.array(transverse_width, dtype='float64'))
    graph_away = ROOT.TGraphErrors(n_points, x, np.array(away, dtype='float64'), np.array(xerr, dtype='float64'), np.array(away_width, dtype='float64'))

    # Set marker styles and colors
    graph_towards.SetMarkerStyle(marker_styles["towards"])
    graph_transverse.SetMarkerStyle(marker_styles["transverse"])
    graph_away.SetMarkerStyle(marker_styles["away"])

    graph_towards.SetMarkerColor(colors[group])
    graph_transverse.SetMarkerColor(colors[group])
    graph_away.SetMarkerColor(colors[group])

    graph_towards.SetMarkerSize(1)
    graph_transverse.SetMarkerSize(1)
    graph_away.SetMarkerSize(1)

    graph_towards.SetLineColor(colors[group])
    graph_transverse.SetLineColor(colors[group])
    graph_away.SetLineColor(colors[group])

    graphs.extend([graph_towards, graph_transverse, graph_away])

# Draw all graphs on the same canvas
for i, graph in enumerate(graphs):
    if i % 3 == 0:
        if i == 0:
            graph.Draw("AP")
            graph.GetYaxis().SetRangeUser(1,4)
            graph.GetXaxis().SetTitle("E_{topo} thres [MeV]")
            graph.GetYaxis().SetTitle("<E_{T,topo}> [GeV]")
        else:
             graph.Draw("P SAME")

# Adding a legend
legend = ROOT.TLegend(0.17, 0.65, 0.5, 0.92)
legend.SetNColumns(1)
legend.SetTextSize(0.035)
legend.AddEntry("","#bf{Towards Region}","")
legend.AddEntry("","15 < p_{T,lead} < 20 GeV","")
legend.AddEntry(graphs[10], "<PU> < 1.5%", "pe")
legend.AddEntry(graphs[1], "1.5 < <PU> < 2.0%", "pe")
legend.AddEntry(graphs[4], "2.0 < <PU> < 2.8%", "pe")
legend.AddEntry(graphs[7], "<PU> > 2.8%", "pe")
#legend.AddEntry(graphs[10], "Run 150 (Detroit)", "ep")
legend.Draw()

# Update and display the canvas
canvas.Update()
canvas.Draw()
canvas.SaveAs("/sphenix/u/egm2153/spring_2025/"+direct+"/h_mean_etopo_15_20GeV_towards.png")
# Create a canvas
canvas = ROOT.TCanvas("canvas", "Graph", 500, 400)

# Define marker styles and colors
marker_styles = {
    "towards": ROOT.kFullCircle,
    "transverse": ROOT.kFullCircle,
    "away": ROOT.kFullCircle
}

colors = {
    "ntopo": ROOT.kBlack,
    "mc_ntopo": ROOT.kRed,
    "clus_ntopo": ROOT.kBlue,
    "nz_ntopo": ROOT.kMagenta,
    "dt_ntopo": ROOT.kCyan
}

# Prepare a list of all data groups
data_groups = [
    ("mc_ntopo", x0, mean_mc_topo_towards, mean_mc_topo_transverse, mean_mc_topo_away, std_mc_topo_towards, std_mc_topo_transverse, std_mc_topo_away),
    ("clus_ntopo", x1, mean_clus_topo_towards, mean_clus_topo_transverse, mean_clus_topo_away, std_clus_topo_towards, std_clus_topo_transverse, std_clus_topo_away),
    ("nz_ntopo", x2, mean_nz_topo_towards, mean_nz_topo_transverse, mean_nz_topo_away, std_nz_topo_towards, std_nz_topo_transverse, std_nz_topo_away),
    #("dt_ntopo", x3, mean_dt_topo_towards, mean_dt_topo_transverse, mean_dt_topo_away, std_dt_topo_towards, std_dt_topo_transverse, std_dt_topo_away),
    ("ntopo", x4, mean_topo_towards, mean_topo_transverse, mean_topo_away, std_topo_towards, std_topo_transverse, std_topo_away)
]

graphs = []
    
# Create TGraphs for each data group
for group, x, towards, transverse, away, towards_width, transverse_width, away_width in data_groups:
    n_points = len(towards)

    # Create graphs for towards, transverse, and away
    graph_towards = ROOT.TGraphErrors(n_points, x, np.array(towards, dtype='float64'), np.array(xerr, dtype='float64'), np.array(towards_width, dtype='float64'))
    graph_transverse = ROOT.TGraphErrors(n_points, x, np.array(transverse, dtype='float64'), np.array(xerr, dtype='float64'), np.array(transverse_width, dtype='float64'))
    graph_away = ROOT.TGraphErrors(n_points, x, np.array(away, dtype='float64'), np.array(xerr, dtype='float64'), np.array(away_width, dtype='float64'))

    # Set marker styles and colors
    graph_towards.SetMarkerStyle(marker_styles["towards"])
    graph_transverse.SetMarkerStyle(marker_styles["transverse"])
    graph_away.SetMarkerStyle(marker_styles["away"])

    graph_towards.SetMarkerColor(colors[group])
    graph_transverse.SetMarkerColor(colors[group])
    graph_away.SetMarkerColor(colors[group])
    
    graph_towards.SetMarkerSize(1)
    graph_transverse.SetMarkerSize(1)
    graph_away.SetMarkerSize(1)

    graph_towards.SetLineColor(colors[group])
    graph_transverse.SetLineColor(colors[group])
    graph_away.SetLineColor(colors[group])

    graphs.extend([graph_towards, graph_transverse, graph_away])

# Draw all graphs on the same canvas
for i, graph in enumerate(graphs):
    if (i-1) % 3 == 0:
        if i == 1:
            graph.Draw("AP")
            graph.GetYaxis().SetRangeUser(0,1)
            graph.GetXaxis().SetTitle("E_{topo} thres [MeV]")
            graph.GetYaxis().SetTitle("<E_{T,topo}> [GeV]")
        else:
             graph.Draw("P SAME")

# Adding a legend
legend = ROOT.TLegend(0.17, 0.65, 0.5, 0.92)
legend.SetNColumns(1)
legend.SetTextSize(0.035)
legend.AddEntry("","#bf{Transverse Region}","")
legend.AddEntry("","15 < p_{T,lead} < 20 GeV","")
legend.AddEntry(graphs[10], "<PU> < 1.5%", "pe")
legend.AddEntry(graphs[1], "1.5 < <PU> < 2.0%", "pe")
legend.AddEntry(graphs[4], "2.0 < <PU> < 2.8%", "pe")
legend.AddEntry(graphs[7], "<PU> > 2.8%", "pe")
#legend.AddEntry(graphs[10], "Run 150 (Detroit)", "ep")
legend.Draw()

# Update and display the canvas
canvas.Update()
canvas.Draw()
canvas.SaveAs("/sphenix/u/egm2153/spring_2025/"+direct+"/h_mean_etopo_15_20GeV_transverse.png")
# Create a canvas
canvas = ROOT.TCanvas("canvas", "Graph", 500, 400)

# Define marker styles and colors
marker_styles = {
    "towards": ROOT.kFullCircle,
    "transverse": ROOT.kFullCircle,
    "away": ROOT.kFullCircle
}

colors = {
    "ntopo": ROOT.kBlack,
    "mc_ntopo": ROOT.kRed,
    "clus_ntopo": ROOT.kBlue,
    "nz_ntopo": ROOT.kMagenta,
    "dt_ntopo": ROOT.kCyan
}

# Prepare a list of all data groups
data_groups = [
    ("mc_ntopo", x0, mean_mc_topo_towards, mean_mc_topo_transverse, mean_mc_topo_away, std_mc_topo_towards, std_mc_topo_transverse, std_mc_topo_away),
    ("clus_ntopo", x1, mean_clus_topo_towards, mean_clus_topo_transverse, mean_clus_topo_away, std_clus_topo_towards, std_clus_topo_transverse, std_clus_topo_away),
    ("nz_ntopo", x2, mean_nz_topo_towards, mean_nz_topo_transverse, mean_nz_topo_away, std_nz_topo_towards, std_nz_topo_transverse, std_nz_topo_away),
    #("dt_ntopo", x3, mean_dt_topo_towards, mean_dt_topo_transverse, mean_dt_topo_away, std_dt_topo_towards, std_dt_topo_transverse, std_dt_topo_away),
    ("ntopo", x4, mean_topo_towards, mean_topo_transverse, mean_topo_away, std_topo_towards, std_topo_transverse, std_topo_away)
]

graphs = []
    
# Create TGraphs for each data group
for group, x, towards, transverse, away, towards_width, transverse_width, away_width in data_groups:
    n_points = len(towards)

    # Create graphs for towards, transverse, and away
    graph_towards = ROOT.TGraphErrors(n_points, x, np.array(towards, dtype='float64'), np.array(xerr, dtype='float64'), np.array(towards_width, dtype='float64'))
    graph_transverse = ROOT.TGraphErrors(n_points, x, np.array(transverse, dtype='float64'), np.array(xerr, dtype='float64'), np.array(transverse_width, dtype='float64'))
    graph_away = ROOT.TGraphErrors(n_points, x, np.array(away, dtype='float64'), np.array(xerr, dtype='float64'), np.array(away_width, dtype='float64'))

    # Set marker styles and colors
    graph_towards.SetMarkerStyle(marker_styles["towards"])
    graph_transverse.SetMarkerStyle(marker_styles["transverse"])
    graph_away.SetMarkerStyle(marker_styles["away"])

    graph_towards.SetMarkerColor(colors[group])
    graph_transverse.SetMarkerColor(colors[group])
    graph_away.SetMarkerColor(colors[group])

    graph_towards.SetLineColor(colors[group])
    graph_transverse.SetLineColor(colors[group])
    graph_away.SetLineColor(colors[group])

    graphs.extend([graph_towards, graph_transverse, graph_away])

# Draw all graphs on the same canvas
for i, graph in enumerate(graphs):
    if (i-2) % 3 == 0:
        if i == 2:
            graph.Draw("AP")
            graph.GetYaxis().SetRangeUser(0.5,2.5)
            graph.GetXaxis().SetTitle("E_{topo} thres [MeV]")
            graph.GetYaxis().SetTitle("<E_{T,topo}> [GeV]")
        else:
             graph.Draw("P SAME")

# Adding a legend
legend = ROOT.TLegend(0.17, 0.65, 0.5, 0.92)
legend.SetNColumns(1)d
legend.SetTextSize(0.035)
legend.AddEntry("","#bf{Away Region}","")
legend.AddEntry("","15 < p_{T,lead} < 20 GeV","")
legend.AddEntry(graphs[10], "<PU> < 1.5%", "pe")
legend.AddEntry(graphs[1], "1.5 < <PU> < 2.0%", "pe")
legend.AddEntry(graphs[4], "2.0 < <PU> < 2.8%", "pe")
legend.AddEntry(graphs[7], "<PU> > 2.8%", "pe")
#legend.AddEntry(graphs[10], "Run 150 (Detroit)", "ep")
legend.Draw()

# Update and display the canvas
canvas.Update()
canvas.Draw()
canvas.SaveAs("/sphenix/u/egm2153/spring_2025/"+direct+"/h_mean_etopo_15_20GeV_away.png")
'''