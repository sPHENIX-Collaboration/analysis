Steps to generate DIS events with Pythia6 (via ROOT) and making plots:

cd eventgen
root
.x generate_ep_dis.C(<p_electron>, <p_proton>, <nevents>)
.q

where:
<p_electron> = electron momentum
<p_proton> = proton momentum
<nevents> = number of events

cd -
root -l
.x makePlot_EtaDistribution.C(<input_file>)
.x makePlot_LoI_Momentum.C(<input_file>)
.x makePlot_xQ2_Studies.C(<input_file>)
.q

where:
<input_file> = input file, i.e. output from generate_ep_dis.C

LoI (and figure numbers in makePlot_LoI_Momentum.C) refer to 2014 ePHENIX Letter of Intent, arXiv 1402.1209v1
