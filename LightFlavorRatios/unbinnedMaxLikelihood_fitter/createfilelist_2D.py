import os 
import sys
import math 

# granular
# bin_pt = [0.0, 0.5, 1.0, 1.5, 2.0, 8.0]
# bin_eta = [-1.0, -0.75, -0.5, -0.25, 0.0, 0.25, 0.5, 0.75, 1.0]
# bin_phi = [-3.15, -2.5, -2.0, -1.5, -1.0, -0.5, 0.0, 0.5, 1.0, 1.5, 2.0, 2.5, 3.15]

# coarse
# bin_pt = [1.5, 8.0]
# bin_eta = [-1.0, -0.5, 0.0, 0.5, 1.0]
# bin_phi = [-3.15, -2.0, -1.0, 0.0, 1.0, 2.0, 3.15]

# for lambda
bin_pt = [0.5, 0.6, 0.7, 0.8, 0.9, 1.0, 1.1, 1.2, 1.3, 1.4, 1.5, 1.8, 2.1, 2.4, 2.7, 3.0, 4.0]
# bin_pt = [0.8, 0.9, 1.0, 1.1, 1.2, 1.3, 1.4, 1.5, 1.8, 2.1]
# bin_pt = [1.8, 2.1]
bin_phi = [-3.15, -2.0, -1.0, 0.0, 1.0, 2.0, 3.15]

# for kshort
# bin_pt = [0.5, 0.6, 0.7, 0.8, 0.9, 1.0, 1.1, 1.2, 1.3, 1.4, 1.5, 1.8, 2.1, 2.4, 2.7, 3.0, 4.0]
# bin_eta = [-1.0, -0.5, 0.0, 0.5, 1.0]
# bin_phi = [-3.15, -2.0, -1.0, 0.0, 1.0, 2.0, 3.15]

queuetext = ""
idx = 0
# for ipt in range(len(bin_pt)-1):
#     print ("pt bin %d: %f - %f" % (ipt, bin_pt[ipt], bin_pt[ipt+1]))
#     cut_pt = "K_S0_pT>={:.2f}&&K_S0_pT<{:.2f}".format(bin_pt[ipt], bin_pt[ipt+1])
#     for ieta in range(len(bin_eta)-1):
#         print ("eta bin %d: %f - %f" % (ieta, bin_eta[ieta], bin_eta[ieta+1]))
#         cut_eta = "K_S0_pseudorapidity>={:.2f}&&K_S0_pseudorapidity<{:.2f}".format(bin_eta[ieta], bin_eta[ieta+1])
#         for iphi in range(len(bin_phi)-1):
#             print ("phi bin %d: %f - %f" % (iphi, bin_phi[iphi], bin_phi[iphi+1]))
#             cut_phi = "K_S0_phi>={:.2f}&&K_S0_phi<{:.2f}".format(bin_phi[iphi], bin_phi[iphi+1])
#             cut = "{}&&{}&&{}".format(cut_pt, cut_eta, cut_phi)
#             queuetext += "{}\n".format(cut)
#             idx += 1

for ipt in range(len(bin_pt)-1):
    print ("pt bin %d: %f - %f" % (ipt, bin_pt[ipt], bin_pt[ipt+1]))
    cut_pt = "Lambda0_pT>={:.2f}&&Lambda0_pT<{:.2f}".format(bin_pt[ipt], bin_pt[ipt+1])
    # only do phi bin for bins pt > 0.8 and pt < 2.7 bins
    if bin_pt[ipt] > 0.6 and bin_pt[ipt+1] < 2.4:
       for iphi in range(len(bin_phi)-1):
            print ("phi bin %d: %f - %f" % (iphi, bin_phi[iphi], bin_phi[iphi+1]))
            cut_phi = "Lambda0_phi>={:.2f}&&Lambda0_phi<{:.2f}".format(bin_phi[iphi], bin_phi[iphi+1])
            cut = "{}&&{}".format(cut_pt, cut_phi)
            queuetext += "{}\n".format(cut)
            idx += 1
    else:
        cut = "{}".format(cut_pt)
        queuetext += "{}\n".format(cut)
        idx += 1

# for ipt in range(len(bin_pt)-1):
#     print ("pt bin %d: %f - %f" % (ipt, bin_pt[ipt], bin_pt[ipt+1]))
#     cut_pt = "K_S0_pT>={:.2f}&&K_S0_pT<{:.2f}".format(bin_pt[ipt], bin_pt[ipt+1])
    
#     if bin_pt[ipt] > 0. and bin_pt[ipt+1] < 10.0:
#        for iphi in range(len(bin_phi)-1):
#             print ("phi bin %d: %f - %f" % (iphi, bin_phi[iphi], bin_phi[iphi+1]))
#             cut_phi = "K_S0_phi>={:.2f}&&K_S0_phi<{:.2f}".format(bin_phi[iphi], bin_phi[iphi+1])
#             cut = "{}&&{}".format(cut_pt, cut_phi)
#             queuetext += "{}\n".format(cut)
#             idx += 1
#     else:
#         cut = "{}".format(cut_pt)
#         queuetext += "{}\n".format(cut)
#         idx += 1

with open("./queue_lambda.list", "w") as f:
# with open("./queue_kshort.list", "w") as f:
    f.write(queuetext)
f.close()
