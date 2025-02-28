from ROOT import TH1F, TH2F, TFile, TCanvas, TLegend, TColor, gROOT, gSystem, TPad, gPad, gStyle, kHAlignRight, kVAlignBottom

TickSize = 0.03
AxisTitleSize = 0.05
AxisLabelSize = 0.04
LeftMargin = 0.15
RightMargin = 0.08
TopMargin = 0.08
BottomMargin = 0.13

black_hex = '#1B1A17'
red_hex = '#9A031E'
blue_hex = '#0B60B0'
green_hex = '#186F65'
orange_hex = '#e99960'
purple_hex = '#7F167F'
pink_hex = '#FFC0CB'
yellow_hex = '#ffcc66'
cyan_hex = '#7FE9DE'

def markerset(i):
    if i == 1:
        return [20]
    elif i == 2:
        return [20, 21]
    elif i == 3:
        return [20, 21, 33]
    elif i == 4:
        return [20, 21, 33, 34]
    elif i == 5:
        return [20, 21, 33, 34, 47]
    elif i == 6:
        return [20, 21, 33, 34, 47, 43]
    elif i == 7:
        return [20, 21, 33, 34, 47, 43, 45]
    elif i == 8:
        return [20, 21, 33, 34, 47, 43, 45, 49]
    else:
        print ("Attempt to use more than 8 markers, probably too many histograms. Set all markers to 20")
        return [20 for _ in range(i)]

def colorset(i):
    if i == 1:
        return ['#810000']
    elif i == 2:
        return ['#810000', '#0F4C75']
    elif i == 3:
        return ['#810000', '#0F4C75', '#7F167F']
    elif i == 4:
        return ['#810000', '#0F4C75', '#7F167F', '#5E8B7E']
    elif i == 5:
        return ['#810000', '#0F4C75', '#7F167F', '#5E8B7E', '#e99960']
    elif i == 6:
        return ['#810000', '#0F4C75', '#7F167F', '#5E8B7E', '#e99960', '#FFC0CB']
    elif i == 7:
        return ['#810000', '#0F4C75', '#7F167F', '#5E8B7E', '#e99960', '#FFC0CB', '#ffcc66']
    elif i == 8:
        return ['#810000', '#0F4C75', '#7F167F', '#5E8B7E', '#e99960', '#FFC0CB', '#ffcc66', '#7FE9DE']
    else:
        print ("Attempt to use more than 5 colors")
        return ['#810000', '#0F4C75', '#7F167F', '#5E8B7E', '#e99960', '#FFC0CB', '#ffcc66', '#7FE9DE']
    

def colorset2(i):
    if i == 1:
        return ['#f2777a']
    elif i == 2:
        return ['#f2777a', '#6699cc']
    elif i == 3:
        return ['#f2777a', '#6699cc', '#9999cc']
    elif i == 4:
        return ['#f2777a', '#6699cc', '#9999cc', '#99cc99']
    elif i == 5:
        return ['#f2777a', '#6699cc', '#9999cc', '#99cc99', '#e99960']
    elif i == 6:
        return ['#f2777a', '#6699cc', '#9999cc', '#99cc99', '#e99960', '#FFC0CB']
    elif i == 7:
        return ['#f2777a', '#6699cc', '#9999cc', '#99cc99', '#e99960', '#FFC0CB', '#ffcc66']
    elif i == 8:
        return ['#f2777a', '#6699cc', '#9999cc', '#99cc99', '#e99960', '#FFC0CB', '#ffcc66', '#7FE9DE']
    else:
        print ("Attempt to use more than 5 colors")
        return ['#f2777a', '#6699cc', '#9999cc', '#99cc99', '#e99960', '#FFC0CB', '#ffcc66', '#7FE9DE']
    

def Draw_1Dhist(hist, IsData, norm1, logy, ymaxscale, XaxisName, Ytitle_unit, outname):
    hist.Sumw2()
    binwidth = hist.GetXaxis().GetBinWidth(1)
    c = TCanvas('c', 'c', 800, 700)
    if norm1:
        hist.Scale(1. / hist.Integral(-1, -1))
    if logy:
        c.SetLogy()
    c.cd()
    gPad.SetRightMargin(RightMargin)
    gPad.SetTopMargin(TopMargin)
    gPad.SetLeftMargin(LeftMargin)
    gPad.SetBottomMargin(BottomMargin)
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
        hist.GetYaxis().SetRangeUser(hist.GetMinimum(0)*0.5, (hist.GetMaximum()) * ymaxscale)
    else:
        hist.GetYaxis().SetRangeUser(0., (hist.GetMaximum()) * ymaxscale)
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
    leg = TLegend((1-RightMargin)-0.5, (1-TopMargin)-0.13,
                  (1-RightMargin)-0.1, (1-TopMargin)-0.03)
    leg.SetTextSize(0.04)
    leg.SetFillStyle(0)
    if IsData:
        leg.AddEntry("", "#it{#bf{sPHENIX}} Internal", "")
        leg.AddEntry("", "Au+Au #sqrt{s_{NN}}=200 GeV", "")
    else:
        leg.AddEntry("", "#it{#bf{sPHENIX}} Simulation", "")
        leg.AddEntry("", "Au+Au #sqrt{s_{NN}}=200 GeV", "")
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


def Draw_1DhistsComp(lhist, norm1, logx, logy, ymaxscale, XaxisName, Ytitle_unit, outname):
    color = ['#1B1A17', '#035397', '#9B0000']
    legtext = ['1st+2nd layers', '2nd+3rd layers', '1st+3rd layers']
    ymax = -1
    ymin = 10e10
    for h in lhist:
        h.Sumw2()
        if h.GetMaximum() > ymax:
            ymax = h.GetMaximum()
        if h.GetMinimum(0) < ymin:
            ymin = h.GetMinimum(0)
        if norm1:
            h.Scale(1. / h.Integral(-1, -1))
            ymax = h.GetMaximum()
            ymin = h.GetMinimum(0)

    binwidth_bin1 = lhist[0].GetXaxis().GetBinWidth(1)
    binwidth_bin2 = lhist[0].GetXaxis().GetBinWidth(2)
    printbinwidth = True
    if binwidth_bin1 != binwidth_bin2:
        printbinwidth = False

    c = TCanvas('c', 'c', 800, 700)
    if logx:
        c.SetLogx()
    if logy:
        c.SetLogy()
    c.cd()
    gPad.SetRightMargin(RightMargin)
    gPad.SetTopMargin(TopMargin)
    gPad.SetLeftMargin(LeftMargin)
    gPad.SetBottomMargin(BottomMargin)
    if printbinwidth:
        if norm1:
            if Ytitle_unit == '':
                lhist[0].GetYaxis().SetTitle('Normalized entries / ({:g})'.format(binwidth_bin1))
            else:
                lhist[0].GetYaxis().SetTitle('Normalized entries / ({:g} {unit})'.format(binwidth_bin1, unit=Ytitle_unit))
        else:
            if Ytitle_unit == '':
                lhist[0].GetYaxis().SetTitle('Entries / ({:g})'.format(binwidth_bin1))
            else:
                lhist[0].GetYaxis().SetTitle('Entries / ({:g} {unit})'.format(binwidth_bin1, unit=Ytitle_unit))
    else:
        if norm1:
            if Ytitle_unit == '':
                lhist[0].GetYaxis().SetTitle('Normalized entries')
            else:
                lhist[0].GetYaxis().SetTitle('Normalized entries {unit})'.format(unit=Ytitle_unit))
        else:
            if Ytitle_unit == '':
                lhist[0].GetYaxis().SetTitle('Entries')
            else:
                lhist[0].GetYaxis().SetTitle('Entries {unit}'.format(unit=Ytitle_unit))
        # lhist[0].GetXaxis().SetRangeUser(lhist[0].GetBinLowEdge(1)-binwidth, lhist[0].GetBinLowEdge(lhist[0].GetNbinsX())+2*binwidth)
    if logy:
        lhist[0].GetYaxis().SetRangeUser(ymin * 0.05, ymax * 100)
    else:
        lhist[0].GetYaxis().SetRangeUser(0., ymax * ymaxscale)
    lhist[0].GetXaxis().SetTitle(XaxisName)
    lhist[0].GetXaxis().SetTickSize(TickSize)
    lhist[0].GetXaxis().SetTitleSize(AxisTitleSize)
    lhist[0].GetXaxis().SetLabelSize(AxisLabelSize)
    lhist[0].GetYaxis().SetTickSize(TickSize)
    lhist[0].GetYaxis().SetTitleSize(AxisTitleSize)
    lhist[0].GetYaxis().SetLabelSize(AxisLabelSize)
    lhist[0].GetXaxis().SetTitleOffset(1.1)
    lhist[0].GetYaxis().SetTitleOffset(1.4)
    for i, h in enumerate(lhist):
        if i == 0:
            h.SetLineColor(TColor.GetColor(color[i]))
            h.SetLineWidth(2)
            h.Draw('hist')
        else:
            h.SetLineColor(TColor.GetColor(color[i]))
            h.SetLineWidth(2)
            h.Draw('histsame')

    leg = TLegend((1-RightMargin)-0.45, (1-TopMargin)-0.15,
                  (1-RightMargin)-0.1, (1-TopMargin)-0.03)
    leg.SetTextSize(0.045)
    leg.SetFillStyle(0)
    leg.AddEntry("", "#it{#bf{sPHENIX}} Simulation", "")
    leg.AddEntry("", "Au+Au #sqrt{s_{NN}}=200 GeV", "")
    leg.Draw()

    leg1 = TLegend(LeftMargin+0.04, (1-TopMargin)-0.21,
                   LeftMargin+0.34, (1-TopMargin)-0.03)
    leg1.SetTextSize(0.035)
    leg1.SetFillStyle(0)
    for i, h in enumerate(lhist):
        leg1.AddEntry(h, legtext[i], "l")
    leg1.Draw()
    c.RedrawAxis()
    c.Draw()
    c.SaveAs(outname+'.pdf')
    c.SaveAs(outname+'.png')
    if(c):
        c.Close()
        gSystem.ProcessEvents()
        del c
        c = 0


def Draw_2Dhist(hist, IsData, logz, norm1, rmargin, XaxisName, YaxisName, ZaxisName, drawopt, outname):
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
        hist.GetZaxis().SetTitleOffset(1.4)
        
    hist.GetXaxis().SetTitleOffset(1.1)
    hist.GetYaxis().SetTitleOffset(1.3)
    hist.GetZaxis().SetLabelSize(AxisLabelSize)
    hist.SetContour(1000)
    hist.Draw(drawopt)

    rightshift = 0.1
    leg = TLegend((1-RightMargin)-0.5, (1-TopMargin)+0.01, 1-gPad.GetRightMargin(), (1-TopMargin)+0.04)
    leg.SetTextAlign(kHAlignRight+kVAlignBottom)
    leg.SetTextSize(0.045)
    leg.SetFillStyle(0)
    if IsData:
        leg.AddEntry("", "#it{#bf{sPHENIX}} Internal", "")
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

def plot_Stack(totalhist, list_hist, color, list_legtext, logy, ymaxscale, XaxisName, Ytitle_unit, plotname):
    # color = ['#073b4c','#118ab2','#06d6a0','#ffd166','#ef476f']
    binwidth = totalhist.GetXaxis().GetBinWidth(1)
    hs = THStack('hs','hs');
    for i, hist in enumerate(list_hist):
        hist.SetLineColor(1)
        hist.SetLineWidth(1)
        hist.SetFillColor(TColor.GetColor(color[i]))
        hs.Add(hist)

    # print(totalhist.GetMaximum(), hs.GetMaximum())

    c = TCanvas('c', 'c', 800, 700)
    if logy:
        c.SetLogy()
    c.cd()
    hs.Draw()
    hs.GetXaxis().SetTitle(XaxisName)
    if Ytitle_unit == '':
        hist.GetYaxis().SetTitle('Entries / ({:g})'.format(binwidth))
    else:
        hist.GetYaxis().SetTitle('Entries / ({:g} {unit})'.format(binwidth, unit=Ytitle_unit))
    hs.GetXaxis().SetTitleSize(AxisTitleSize)
    hs.GetYaxis().SetTitleSize(AxisTitleSize)
    hs.GetXaxis().SetTickSize(TickSize)
    hs.GetYaxis().SetTickSize(TickSize)
    hs.GetXaxis().SetLabelSize(AxisLabelSize)
    hs.GetYaxis().SetLabelSize(AxisLabelSize)
    hs.GetYaxis().SetTitleOffset(1.3)
    hs.SetMaximum(totalhist.GetMaximum() * ymaxscale)
    hs.SetMinimum(0.1)
    totalhist.SetLineWidth(3)
    totalhist.SetLineColor(1)
    totalhist.Draw('histsame')
    c.Update()
    leg = TLegend((1-RightMargin)-0.45, (1-TopMargin)-0.15,
                  (1-RightMargin)-0.1, (1-TopMargin)-0.03)
    leg.SetTextSize(0.045)
    leg.SetFillStyle(0)
    leg.AddEntry('', '#it{#bf{sPHENIX}} Simulation', '')
    leg.AddEntry('', 'Au+Au #sqrt{s_{NN}}=200 GeV', '')
    leg.Draw()
    leg1 = TLegend(LeftMargin+0.05, (1-TopMargin)-0.22,
                   LeftMargin+0.3, (1-TopMargin)-0.01)
    # leg1.SetNColumns(3)
    leg1.SetTextSize(0.03)
    leg1.SetFillStyle(0)
    for i, text in enumerate(list_legtext):
        leg1.AddEntry(list_hist[i], text, 'f')
    leg1.Draw()
    c.Update()
    c.SaveAs(plotname+'.png')
    c.SaveAs(plotname+'.pdf')
    if(c):
        c.Close()
        gSystem.ProcessEvents()
        del c
        c = 0


def GetHistogram(filename, histname):
    f = TFile(filename, 'r')
    hm = f.Get(histname)
    hm.SetDirectory(0)
    f.Close()
    return hm


def str_pttop(s):
    return str(s).replace('.', 'p')

