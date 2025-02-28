from sigmaEff import *
import numpy as np
from ROOT import *

# Path: analysis_INTT/plot/testSigmaEff.py
# test sigma eff 
# generate 20000 random numbers according to Gaussian distribution with mean 0 and sigma 1 
# and calculate the sigma eff with different threshold

# generate 20000 random numbers according to Gaussian distribution with mean 0 and sigma 1
np.random.seed(0)
v = np.random.normal(0, 1, 20000)
print ('v: {}'.format(v))

# calculate the sigma eff with different threshold
xmin, xmax = minimum_size_range(v, 68.5)
print (xmin, xmax) 

# Draw the histogram
c = TCanvas('c', 'c', 800, 600)
h = TH1F('h', 'h', 100, -5, 5)
for i in range(0, 20000):
    h.Fill(v[i])
h.SetLineColor(kBlack)
h.SetLineWidth(2)
h.SetStats(0)
h.Draw('hist')
# Draw the sigma eff range
l1 = TLine(xmin, 0, xmin, 1000)
l1.SetLineColor(kRed)
l1.SetLineWidth(2)
l1.Draw()
l2 = TLine(xmax, 0, xmax, 1000)
l2.SetLineColor(kRed)
l2.SetLineWidth(2)
l2.Draw()

c.SaveAs('testSigmaEff.png')
c.SaveAs('testSigmaEff.pdf')


