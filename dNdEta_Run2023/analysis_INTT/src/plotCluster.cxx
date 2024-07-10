#include <TCanvas.h>
#include <TCut.h>
#include <TF1.h>
#include <TFile.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TMath.h>
#include <TObjString.h>
#include <TRandom3.h>
#include <TTree.h>
#include <TTreeIndex.h>

#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "GenHadron.h"
#include "Tracklet.h"
#include "Vertex.h"

// float theta2pseudorapidity(float theta) { return -1. * TMath::Log(TMath::Tan(theta / 2)); }

// TF1 *ClusADCCut(int constscale, float etascale)
// {
//     TF1 *f = new TF1("f", Form("%d*TMath::CosH(%f*x)", constscale, etascale), -10, 10);
//     return f;
// }

// TH1F *ClusADCCut_StepFunc_INTTPrivate()
// {
//     std::vector<float> thetastep = {0.001, 15, 20, 25, 30, 35, 45, 55, 125, 135, 145, 150, 155, 160, 165, 179.999};
//     // reverse the thetastep
//     std::reverse(thetastep.begin(), thetastep.end());
//     std::vector<float> adccut_theta = {225, 165, 135, 120, 105, 90, 75, 60, 75, 90, 105, 120, 135, 165, 225};
//     float etastep_array[thetastep.size()];
//     cout << "etastep (INTT private): ";
//     for (int i = 0; i < thetastep.size(); i++)
//     {
//         etastep_array[i] = theta2pseudorapidity(thetastep[i] * TMath::Pi() / 180);
//         cout << etastep_array[i] << " ";
//     }
//     cout << endl;

//     TH1F *hm_cut_inttprivate = new TH1F("hm_cut_inttprivate", "hm_cut_inttprivate", thetastep.size() - 1, etastep_array);
//     cout << "ADC cut value: ";
//     for (int j = 0; j < hm_cut_inttprivate->GetNbinsX(); j++)
//     {
//         hm_cut_inttprivate->SetBinContent(j + 1, adccut_theta[j]);
//         cout << hm_cut_inttprivate->GetBinContent(j + 1) << " ";
//     }
//     cout << endl;

//     return hm_cut_inttprivate;
// }

// TH1F *ClusADCCut_StepFunc(int constscale, float etascale)
// {
//     TF1 *f_cut = ClusADCCut(constscale, etascale);

//     std::vector<float> adcstep;
//     for (int i = 0; i < 20; i++)
//     {
//         adcstep.push_back(20 + i * 30);
//     }
//     std::vector<float> etastep;
//     for (int i = 0; i < 20; i++)
//     {
//         etastep.insert(etastep.begin(), f_cut->GetX(adcstep[i], -10, 0));
//         etastep.push_back(f_cut->GetX(adcstep[i], 0, 10));
//     }
//     // Remove nan values
//     etastep.erase(std::remove_if(etastep.begin(), etastep.end(), [](float x) { return std::isnan(x); }), etastep.end());

//     // print out cut information
//     std::cout << "constscale = " << constscale << std::endl;
//     // array of float from etastep
//     float etastep_array[etastep.size()];
//     cout << "etastep: ";
//     for (int i = 0; i < etastep.size(); i++)
//     {
//         cout << etastep[i] << " ";
//         etastep_array[i] = etastep[i];
//     }
//     cout << endl;
//     TH1F *hm_cut = new TH1F("hm_cut", "hm_cut", etastep.size() - 1, etastep_array);
//     cout << "ADC cut value: ";
//     for (int j = 0; j < hm_cut->GetNbinsX(); j++)
//     {
//         if (hm_cut->GetBinLowEdge(j + 1) < 0)
//         {
//             hm_cut->SetBinContent(j + 1, f_cut->Eval(hm_cut->GetBinLowEdge(j + 1)));
//         }
//         else
//         {
//             hm_cut->SetBinContent(j + 1, f_cut->Eval(hm_cut->GetBinCenter(j + 1) + hm_cut->GetBinWidth(j + 1) / 2));
//         }
//         cout << hm_cut->GetBinContent(j + 1) << " ";
//     }
//     cout << endl;

//     return hm_cut;

//     // // construct the histogram
//     // int nbins = 40, hmin = -10, hmax = 10;
//     // TH1F *h = new TH1F("h", "h", nbins, hmin, hmax);
//     // for (int i = 0; i < nbins; i++)
//     // {
//     //     h->SetBinContent(i + 1, f_cut->Eval(h->GetBinCenter(i + 1)));
//     // }

//     // // print the histogram
//     // for (int i = 0; i < nbins; i++)
//     // {
//     //     std::cout << "h->GetBinContent(" << i + 1 << ") = " << h->GetBinContent(i + 1) << std::endl;
//     // }

//     // return h;

//     // adcstep = [20+i*30 for i in range(0, 20)] # quantized step
//     // etastep = []
//     // f = TF1('f', '{}*TMath::CosH(x)'.format(scale), -10, 10)
//     // for i in range(0, 20):
//     //     etastep.insert(0, f.GetX(adcstep[i], -10, 0))
//     //     etastep.append(f.GetX(adcstep[i], 0, 10))

//     // # remove nan values
//     // etastep = [x for x in etastep if not math.isnan(x)]
//     // print(etastep)

//     // hm_cut_v2 = TH1F('hm_cut_v2', 'hm_cut_v2', len(etastep)-1, array('d', etastep))
//     // for j in range(hm_cut_v2.GetNbinsX()):
//     //     # Set bin content at the quantized step
//     //     if hm_cut_v2.GetBinLowEdge(j+1) < 0:
//     //         hm_cut_v2.SetBinContent(j+1, f.Eval(hm_cut_v2.GetBinLowEdge(j+1)))
//     //     else:
//     //         hm_cut_v2.SetBinContent(j+1, f.Eval(hm_cut_v2.GetBinCenter(j+1) + hm_cut_v2.GetBinWidth(j+1)/2))
//     //     # hm_cut_v2.SetBinContent(j+1, f.Eval(hm_cut_v2.GetBinCenter(j+1)))
// }

int main(int argc, char *argv[])
{
    if (argc != 5)
    {
        std::cout << "Usage: ./plotRecoCluster [isdata] [EvtVtx_map_filename] [infilename (ntuple)] [outfilename (histogram)]" << std::endl;
        return 0;
    }

    for (int i = 0; i < argc; i++)
    {
        std::cout << "argv[" << i << "] = " << argv[i] << std::endl;
    }

    bool IsData = (TString(argv[1]).Atoi() == 1) ? true : false;
    TString EvtVtx_map_filename = TString(argv[2]);
    TString infilename = TString(argv[3]);
    TString outfilename = TString(argv[4]);

    TString idxstr = (IsData) ? "INTT_BCO" : "event";

    // std::map<int, vector<float>> EvtVtx_map = EvtVtx_map_tklcluster(EvtVtx_map_filename.Data());
    std::map<int, vector<float>> EvtVtxMap_event = EvtVtx_map_event(EvtVtx_map_filename.Data());          // use with simulation
    std::map<uint64_t, vector<float>> EvtVtxMap_inttbco = EvtVtx_map_inttbco(EvtVtx_map_filename.Data()); // use with data

    // TH1F *hM_ClusX_all = new TH1F("hM_ClusX_all", "hM_ClusX_all", 200, -10, 10);
    // TH1F *hM_ClusX_layer1 = new TH1F("hM_ClusX_layer1", "hM_ClusX_layer1", 200, -10, 10);
    // TH1F *hM_ClusX_layer2 = new TH1F("hM_ClusX_layer2", "hM_ClusX_layer2", 200, -10, 10);
    // TH1F *hM_ClusY_all = new TH1F("hM_ClusY_all", "hM_ClusY_all", 200, -10, 10);
    // TH1F *hM_ClusY_layer1 = new TH1F("hM_ClusY_layer1", "hM_ClusY_layer1", 200, -10, 10);
    // TH1F *hM_ClusY_layer2 = new TH1F("hM_ClusY_layer2", "hM_ClusY_layer2", 200, -10, 10);
    TH1F *hM_ClusZ_all = new TH1F("hM_ClusZ_all", "hM_ClusZ_all", 200, -25, 25);
    TH1F *hM_ClusZ_layer1 = new TH1F("hM_ClusZ_layer1", "hM_ClusZ_layer1", 200, -25, 25);
    TH1F *hM_ClusZ_layer2 = new TH1F("hM_ClusZ_layer2", "hM_ClusZ_layer2", 200, -25, 25);
    // TH1F *hM_ClusR_all = new TH1F("hM_ClusR_all", "hM_ClusR_all", 120, 4, 7);
    // TH1F *hM_ClusR_layer1 = new TH1F("hM_ClusR_layer1", "hM_ClusR_layer1", 120, 4, 7);
    // TH1F *hM_ClusR_layer2 = new TH1F("hM_ClusR_layer2", "hM_ClusR_layer2", 120, 4, 7);
    // TH1F *hM_ClusEtaOri_all = new TH1F("hM_ClusEtaOri_all", "hM_ClusEtaOri_all", 160, -4, 4);
    // TH1F *hM_ClusEtaOri_layer1 = new TH1F("hM_ClusEtaOri_layer1", "hM_ClusEtaOri_layer1", 160, -4, 4);
    // TH1F *hM_ClusEtaOri_layer2 = new TH1F("hM_ClusEtaOri_layer2", "hM_ClusEtaOri_layer2", 160, -4, 4);
    TH1F *hM_ClusEtaPV_all = new TH1F("hM_ClusEtaPV_all", "hM_ClusEtaPV_all", 160, -4, 4);
    TH1F *hM_ClusEtaPV_layer1 = new TH1F("hM_ClusEtaPV_layer1", "hM_ClusEtaPV_layer1", 160, -4, 4);
    TH1F *hM_ClusEtaPV_layer2 = new TH1F("hM_ClusEtaPV_layer2", "hM_ClusEtaPV_layer2", 160, -4, 4);
    // TH1F *hM_ClusPhiOri_all = new TH1F("hM_ClusPhiOri_all", "hM_ClusPhiOri_all", 140, -3.5, 3.5);
    // TH1F *hM_ClusPhiOri_layer1 = new TH1F("hM_ClusPhiOri_layer1", "hM_ClusPhiOri_layer1", 140, -3.5, 3.5);
    // TH1F *hM_ClusPhiOri_layer2 = new TH1F("hM_ClusPhiOri_layer2", "hM_ClusPhiOri_layer2", 140, -3.5, 3.5);
    TH1F *hM_ClusPhiPV_all = new TH1F("hM_ClusPhiPV_all", "hM_ClusPhiPV_all", 140, -3.5, 3.5);
    TH1F *hM_ClusPhiPV_layer1 = new TH1F("hM_ClusPhiPV_layer1", "hM_ClusPhiPV_layer1", 140, -3.5, 3.5);
    TH1F *hM_ClusPhiPV_layer2 = new TH1F("hM_ClusPhiPV_layer2", "hM_ClusPhiPV_layer2", 140, -3.5, 3.5);
    TH1F *hM_ClusPhiPV_ClusPhiSize43or46 = new TH1F("hM_ClusPhiPV_ClusPhiSize43or46", "hM_ClusPhiPV_ClusPhiSize43or46", 330, -3.3, 3.3);
    // Very fine binning for ladder overlap study
    // INTT pitch size in phi = 78 um (Reference: https://indico.bnl.gov/event/15547/contributions/62868/attachments/41171/68968/2022_sPHENIX_School_RN.pdf)
    // INTT outer radius 102.62 mm
    // Azimuthal angle per pitch = (78*1E-6) / (102.62*1E-3) ~= 7.601E-4 rad ~= 0.0435 degree
    // Number of bins = (3.2*2) / (7.601E-4) = 8421
    TH1F *hM_ClusPhiPV_all_fine = new TH1F("hM_ClusPhiPV_all_fine", "hM_ClusPhiPV_all_fine", 8421, -3.2, 3.2);
    TH1F *hM_ClusPhiPV_layer1_fine = new TH1F("hM_ClusPhiPV_layer1_fine", "hM_ClusPhiPV_layer1_fine", 8421, -3.2, 3.2);
    TH1F *hM_ClusPhiPV_layer2_fine = new TH1F("hM_ClusPhiPV_layer2_fine", "hM_ClusPhiPV_layer2_fine", 8421, -3.2, 3.2);
    //
    TH1F *hM_ClusADC_all = new TH1F("hM_ClusADC_all", "hM_ClusADC_all", 1800, 0, 18000);
    TH1F *hM_ClusADC_layer1 = new TH1F("hM_ClusADC_layer1", "hM_ClusADC_layer1", 1800, 0, 18000);
    TH1F *hM_ClusADC_layer2 = new TH1F("hM_ClusADC_layer2", "hM_ClusADC_layer2", 1800, 0, 18000);
    TH1F *hM_ClusADC_all_0to500 = new TH1F("hM_ClusADC_all_0to500", "hM_ClusADC_all_0to500", 500, 0, 500);
    TH1F *hM_ClusADC_layer1_0to500 = new TH1F("hM_ClusADC_layer1_0to500", "hM_ClusADC_layer1_0to500", 500, 0, 500);
    TH1F *hM_ClusADC_layer2_0to500 = new TH1F("hM_ClusADC_layer2_0to500", "hM_ClusADC_layer2_0to500", 500, 0, 500);
    // TH1F *hM_ClusZSize_all = new TH1F("hM_ClusZSize_all", "hM_ClusZSize_all", 20, 0, 20);
    // TH1F *hM_ClusZSize_layer1 = new TH1F("hM_ClusZSize_layer1", "hM_ClusZSize_layer1", 20, 0, 20);
    // TH1F *hM_ClusZSize_layer2 = new TH1F("hM_ClusZSize_layer2", "hM_ClusZSize_layer2", 20, 0, 20);
    TH1F *hM_ClusPhiSize_all = new TH1F("hM_ClusPhiSize_all", "hM_ClusPhiSize_all", 100, 0, 100);
    TH1F *hM_ClusPhiSize_layer1 = new TH1F("hM_ClusPhiSize_layer1", "hM_ClusPhiSize_layer1", 100, 0, 100);
    TH1F *hM_ClusPhiSize_layer2 = new TH1F("hM_ClusPhiSize_layer2", "hM_ClusPhiSize_layer2", 100, 0, 100);
    TH2F *hM_ClusX_ClusY_all = new TH2F("hM_ClusX_ClusY_all", "hM_ClusX_ClusY_all", 260, -13, 13, 260, -13, 13);
    TH2F *hM_ClusX_ClusY_all_weiphisize = new TH2F("hM_ClusX_ClusY_all_weiphisize", "hM_ClusX_ClusY_all_weiphisize", 260, -13, 13, 260, -13, 13);
    TH2F *hM_ClusX_ClusY_all_weiclusadc = new TH2F("hM_ClusX_ClusY_all_weiclusadc", "hM_ClusX_ClusY_all_weiclusadc", 260, -13, 13, 260, -13, 13);
    TH2F *hM_ClusX_ClusY_ClusPhiSize43or46 = new TH2F("hM_ClusX_ClusY_ClusPhiSize43or46", "hM_ClusX_ClusY_ClusPhiSize43or46", 260, -13, 13, 260, -13, 13);
    TH2F *hM_ClusX_ClusY_ClusPhiSize43 = new TH2F("hM_ClusX_ClusY_ClusPhiSize43", "hM_ClusX_ClusY_ClusPhiSize43", 260, -13, 13, 260, -13, 13);
    TH2F *hM_ClusX_ClusY_ClusPhiSize46 = new TH2F("hM_ClusX_ClusY_ClusPhiSize46", "hM_ClusX_ClusY_ClusPhiSize46", 260, -13, 13, 260, -13, 13);

    TH2F *hM_ClusZ_ClusPhiPV_all = new TH2F("hM_ClusZ_ClusPhiPV_all", "hM_ClusZ_ClusPhiPV_all", 1000, -25, 25, 350, -3.5, 3.5);
    TH2F *hM_ClusZ_ClusPhiPV_layer1 = new TH2F("hM_ClusZ_ClusPhiPV_layer1", "hM_ClusZ_ClusPhiPV_layer1", 1000, -25, 25, 350, -3.5, 3.5);
    TH2F *hM_ClusZ_ClusPhiPV_layer2 = new TH2F("hM_ClusZ_ClusPhiPV_layer2", "hM_ClusZ_ClusPhiPV_layer2", 1000, -25, 25, 350, -3.5, 3.5);
    TH2F *hM_ClusZ_ClusPhiPV_all_coarse = new TH2F("hM_ClusZ_ClusPhiPV_all_coarse", "hM_ClusZ_ClusPhiPV_all_coarse", 100, -25, 25, 350, -3.5, 3.5);
    TH2F *hM_ClusZ_ClusPhiPV_all_coarse_weiphisize = new TH2F("hM_ClusZ_ClusPhiPV_all_coarse_weiphisize", "hM_ClusZ_ClusPhiPV_all_coarse_weiphisize", 100, -25, 25, 350, -3.5, 3.5);
    TH2F *hM_ClusZ_ClusPhiPV_all_coarse_weiclusadc = new TH2F("hM_ClusZ_ClusPhiPV_all_coarse_weiclusadc", "hM_ClusZ_ClusPhiPV_all_coarse_weiclusadc", 100, -25, 25, 350, -3.5, 3.5);
    // TH2F *hM_ClusEta_ClusZSize_all = new TH2F("hM_ClusEta_ClusZSize_all", "hM_ClusEta_ClusZSize_all", 160, -4, 4, 20, 0, 20);
    // TH2F *hM_ClusEta_ClusZSize_layer1 = new TH2F("hM_ClusEta_ClusZSize_layer1", "hM_ClusEta_ClusZSize_layer1", 160, -4, 4, 20, 0, 20);
    // TH2F *hM_ClusEta_ClusZSize_layer2 = new TH2F("hM_ClusEta_ClusZSize_layer2", "hM_ClusEta_ClusZSize_layer2", 160, -4, 4, 20, 0, 20);
    TH2F *hM_ClusPhiPV_ClusPhiSize_all = new TH2F("hM_ClusPhiPV_ClusPhiSize_all", "hM_ClusPhiPV_ClusPhiSize_all", 140, -3.5, 3.5, 100, 0, 100);
    TH2F *hM_ClusPhiPV_ClusPhiSize_layer1 = new TH2F("hM_ClusPhiPV_ClusPhiSize_layer1", "hM_ClusPhiPV_ClusPhiSize_layer1", 140, -3.5, 3.5, 100, 0, 100);
    TH2F *hM_ClusPhiPV_ClusPhiSize_layer2 = new TH2F("hM_ClusPhiPV_ClusPhiSize_layer2", "hM_ClusPhiPV_ClusPhiSize_layer2", 140, -3.5, 3.5, 100, 0, 100);
    TH2F *hM_ClusPhiPV_ClusADC_all = new TH2F("hM_ClusPhiPV_ClusADC_all", "hM_ClusPhiPV_ClusADC_all", 140, -3.5, 3.5, 180, 0, 18000);
    TH2F *hM_ClusPhiPV_ClusADC_layer1 = new TH2F("hM_ClusPhiPV_ClusADC_layer1", "hM_ClusPhiPV_ClusADC_layer1", 140, -3.5, 3.5, 180, 0, 18000);
    TH2F *hM_ClusPhiPV_ClusADC_layer2 = new TH2F("hM_ClusPhiPV_ClusADC_layer2", "hM_ClusPhiPV_ClusADC_layer2", 140, -3.5, 3.5, 180, 0, 18000);
    // TH2F *hM_ClusZSize_ClusPhiSize_all = new TH2F("hM_ClusZSize_ClusPhiSize_all", "hM_ClusZSize_ClusPhiSize_all", 20, 0, 20, 100, 0, 100);
    // TH2F *hM_ClusZSize_ClusPhiSize_layer1 = new TH2F("hM_ClusZSize_ClusPhiSize_layer1", "hM_ClusZSize_ClusPhiSize_layer1", 20, 0, 20, 100, 0, 100);
    // TH2F *hM_ClusZSize_ClusPhiSize_layer2 = new TH2F("hM_ClusZSize_ClusPhiSize_layer2", "hM_ClusZSize_ClusPhiSize_layer2", 20, 0, 20, 100, 0, 100);
    TH2F *hM_ClusEtaPV_ClusADC_all = new TH2F("hM_ClusEtaPV_ClusADC_all", "hM_ClusEtaPV_ClusADC_all", 160, -4, 4, 180, 0, 18000);
    TH2F *hM_ClusEtaPV_ClusADC_layer1 = new TH2F("hM_ClusEtaPV_ClusADC_layer1", "hM_ClusEtaPV_ClusADC_layer1", 160, -4, 4, 180, 0, 18000);
    TH2F *hM_ClusEtaPV_ClusADC_layer2 = new TH2F("hM_ClusEtaPV_ClusADC_layer2", "hM_ClusEtaPV_ClusADC_layer2", 160, -4, 4, 180, 0, 18000);
    TH2F *hM_ClusEtaPV_ClusADC_all_zoomin = new TH2F("hM_ClusEtaPV_ClusADC_all_zoomin", "hM_ClusEtaPV_ClusADC_all_zoomin", 120, -3, 3, 50, 0, 500);
    TH2F *hM_ClusEtaPV_ClusADC_layer1_zoomin = new TH2F("hM_ClusEtaPV_ClusADC_layer1_zoomin", "hM_ClusEtaPV_ClusADC_layer1_zoomin", 120, -3, 3, 50, 0, 500);
    TH2F *hM_ClusEtaPV_ClusADC_layer2_zoomin = new TH2F("hM_ClusEtaPV_ClusADC_layer2_zoomin", "hM_ClusEtaPV_ClusADC_layer2_zoomin", 120, -3, 3, 50, 0, 500);
    TH2F *hM_ClusEtaPV_ClusPhiSize_all = new TH2F("hM_ClusEtaPV_ClusPhiSize_all", "hM_ClusEtaPV_ClusPhiSize_all", 160, -4, 4, 100, 0, 100);
    TH2F *hM_ClusEtaPV_ClusPhiSize_layer1 = new TH2F("hM_ClusEtaPV_ClusPhiSize_layer1", "hM_ClusEtaPV_ClusPhiSize_layer1", 160, -4, 4, 100, 0, 100);
    TH2F *hM_ClusEtaPV_ClusPhiSize_layer2 = new TH2F("hM_ClusEtaPV_ClusPhiSize_layer2", "hM_ClusEtaPV_ClusPhiSize_layer2", 160, -4, 4, 100, 0, 100);
    TH2F *hM_ClusEtaPV_ClusPhiSize_all_zoomin = new TH2F("hM_ClusEtaPV_ClusPhiSize_all_zoomin", "hM_ClusEtaPV_ClusPhiSize_all_zoomin", 120, -3, 3, 20, 0, 20);
    TH2F *hM_ClusEtaPV_ClusPhiSize_layer1_zoomin = new TH2F("hM_ClusEtaPV_ClusPhiSize_layer1_zoomin", "hM_ClusEtaPV_ClusPhiSize_layer1_zoomin", 120, -3, 3, 20, 0, 20);
    TH2F *hM_ClusEtaPV_ClusPhiSize_layer2_zoomin = new TH2F("hM_ClusEtaPV_ClusPhiSize_layer2_zoomin", "hM_ClusEtaPV_ClusPhiSize_layer2_zoomin", 120, -3, 3, 20, 0, 20);

    // For cluster ADC cut optimization
    int constscale_low = 5, constscale_high = 55, constscale_step = 1;
    vector<TH1F *> v_hM_ClusEtaPV_EtaDepADCCut;
    vector<TH2F *> v_hM_ClusEtaPV_ClusADC_all_zoomin;
    // For cluster ADC cut optimization, histogram for cut values for each constscale
    vector<TH1F *> v_hM_ClusADCCut;
    vector<TF1 *> v_f_ClusADCCut;
    for (int constscale = constscale_low; constscale <= constscale_high; constscale += constscale_step)
    {
        v_hM_ClusEtaPV_EtaDepADCCut.push_back(new TH1F(Form("hM_ClusEtaPV_EtaDepADCCut_constscale%d_etascale1p0", constscale), Form("hM_ClusEtaPV_EtaDepADCCut_constscale%d_etascale1p0", constscale), 140, -3.5, 3.5));
        v_hM_ClusEtaPV_ClusADC_all_zoomin.push_back(
            new TH2F(Form("hM_ClusEtaPV_ClusADC_all_zoomin_constscale%d_etascale1p0", constscale), Form("hM_ClusEtaPV_ClusADC_all_zoomin_constscale%d_etascale1p0", constscale), 120, -3, 3, 50, 0, 500));
        v_hM_ClusADCCut.push_back(ClusADCCut_StepFunc(constscale, 1));
        v_f_ClusADCCut.push_back(ClusADCCut(constscale, 1));
    }

    vector<TH1F *> v_hM_ClusEtaPV_EtaDepADCCut_etascale;
    vector<TH2F *> v_hM_ClusEtaPV_ClusADC_all_zoomin_etascale;
    vector<TH1F *> v_hM_ClusADCCut_etascale;
    vector<TF1 *> v_f_ClusADCCut_etascale;
    float etascale_low = 0.6, etascale_high = 1.2, etascale_step = 0.02;
    for (float etascale = etascale_low; etascale <= etascale_high; etascale += etascale_step)
    {
        TString etascale_str = Form("%.2f", etascale);
        etascale_str.ReplaceAll(".", "p");
        v_hM_ClusEtaPV_EtaDepADCCut_etascale.push_back(
            new TH1F(Form("hM_ClusEtaPV_EtaDepADCCut_constscale30_etascale%s", etascale_str.Data()), Form("hM_ClusEtaPV_EtaDepADCCut_constscale30_etascale%s", etascale_str.Data()), 140, -3.5, 3.5));
        v_hM_ClusEtaPV_ClusADC_all_zoomin_etascale.push_back(
            new TH2F(Form("hM_ClusEtaPV_ClusADC_all_zoomin_constscale30_etascale%s", etascale_str.Data()), Form("hM_ClusEtaPV_ClusADC_all_zoomin_constscale30_etascale%s", etascale_str.Data()), 120, -3, 3, 50, 0, 500));
        v_hM_ClusADCCut_etascale.push_back(ClusADCCut_StepFunc(30, etascale));
        v_f_ClusADCCut_etascale.push_back(ClusADCCut(30, etascale));
    }
    // for the cluster adc cut from INTT private study
    TH1F *hM_ClusADCCut_inttprivate = ClusADCCut_StepFunc_INTTPrivate();
    TH1F *hM_ClusEtaPV_EtaDepADCCut_inttprivate = new TH1F("hM_ClusEtaPV_EtaDepADCCut_inttprivate", "hM_ClusEtaPV_EtaDepADCCut_inttprivate", 140, -3.5, 3.5);
    TH2F *hM_ClusEtaPV_ClusADC_all_zoomin_inttprivate = new TH2F("hM_ClusEtaPV_ClusADC_all_zoomin_inttprivate", "hM_ClusEtaPV_ClusADC_all_zoomin_inttprivate", 120, -3, 3, 50, 0, 500);

    TH2F *hM_ClusPhiSize_ClusADC_all = new TH2F("hM_ClusPhiSize_ClusADC_all", "hM_ClusPhiSize_ClusADC_all", 100, 0, 100, 180, 0, 18000);
    TH2F *hM_ClusPhiSize_ClusADC_layer1 = new TH2F("hM_ClusPhiSize_ClusADC_layer1", "hM_ClusPhiSize_ClusADC_layer1", 100, 0, 100, 180, 0, 18000);
    TH2F *hM_ClusPhiSize_ClusADC_layer2 = new TH2F("hM_ClusPhiSize_ClusADC_layer2", "hM_ClusPhiSize_ClusADC_layer2", 100, 0, 100, 180, 0, 18000);
    TH2F *hM_ClusPhiSize_ClusADC_all_zoomin = new TH2F("hM_ClusPhiSize_ClusADC_all_zoomin", "hM_ClusPhiSize_ClusADC_all_zoomin", 20, 0, 20, 50, 0, 500);
    TH2F *hM_ClusPhiSize_ClusADC_layer1_zoomin = new TH2F("hM_ClusPhiSize_ClusADC_layer1_zoomin", "hM_ClusPhiSize_ClusADC_layer1_zoomin", 20, 0, 20, 50, 0, 500);
    TH2F *hM_ClusPhiSize_ClusADC_layer2_zoomin = new TH2F("hM_ClusPhiSize_ClusADC_layer2_zoomin", "hM_ClusPhiSize_ClusADC_layer2_zoomin", 20, 0, 20, 50, 0, 500);

    // Vertex Z reweighting
    TH1F *hM_vtxzweight = VtxZ_ReweiHist();

    TFile *f = new TFile(infilename, "READ");
    TTree *t = (TTree *)f->Get("EventTree");
    t->BuildIndex(idxstr); // Reference: https://root-forum.cern.ch/t/sort-ttree-entries/13138
    TTreeIndex *index = (TTreeIndex *)t->GetTreeIndex();
    int event;
    uint64_t INTT_BCO;
    vector<int> *ClusLayer = 0;
    vector<float> *ClusX = 0, *ClusY = 0, *ClusZ = 0, *ClusR = 0, *ClusPhi = 0, *ClusEta = 0, *ClusPhiSize = 0, *ClusZSize = 0;
    vector<unsigned int> *ClusAdc = 0;
    t->SetBranchAddress("event", &event);
    t->SetBranchAddress("INTT_BCO", &INTT_BCO);
    t->SetBranchAddress("ClusLayer", &ClusLayer);
    t->SetBranchAddress("ClusX", &ClusX);
    t->SetBranchAddress("ClusY", &ClusY);
    t->SetBranchAddress("ClusZ", &ClusZ);
    t->SetBranchAddress("ClusR", &ClusR);
    t->SetBranchAddress("ClusPhi", &ClusPhi);
    t->SetBranchAddress("ClusEta", &ClusEta);
    t->SetBranchAddress("ClusAdc", &ClusAdc);
    t->SetBranchAddress("ClusPhiSize", &ClusPhiSize);
    t->SetBranchAddress("ClusZSize", &ClusZSize);
    for (int ev = 0; ev < t->GetEntriesFast(); ev++)
    {
        Long64_t local = t->LoadTree(index->GetIndex()[ev]);
        t->GetEntry(local);

        vector<float> PV = (IsData) ? EvtVtxMap_inttbco[INTT_BCO] : EvtVtxMap_event[event];

        if (PV.size() != 3)
        {
            cout << "[ERROR] No PV found for event " << event << endl;
            exit(1);
        }

        if (PV[2] < -40 || PV[2] > 0)
            continue;

        float vtxzwei = (IsData) ? 1. : hM_vtxzweight->GetBinContent(hM_vtxzweight->FindBin(PV[2]));

        for (size_t i = 0; i < ClusLayer->size(); i++)
        {
            if (ClusLayer->at(i) < 3 || ClusLayer->at(i) > 6)
            {
                cout << "[ERROR] Unknown layer: " << ClusLayer->at(i) << endl; // Should not happen
                exit(1);
            }

            int layer = (ClusLayer->at(i) == 3 || ClusLayer->at(i) == 4) ? 0 : 1;

            Hit *hit = new Hit(ClusX->at(i), ClusY->at(i), ClusZ->at(i), PV[0], PV[1], PV[2], layer);

            // hM_ClusX_all->Fill(ClusX->at(i));
            // hM_ClusY_all->Fill(ClusY->at(i));
            hM_ClusZ_all->Fill(ClusZ->at(i), vtxzwei);
            // hM_ClusR_all->Fill(ClusR->at(i));
            // hM_ClusEtaOri_all->Fill(ClusEta->at(i));
            hM_ClusEtaPV_all->Fill(hit->Eta(), vtxzwei);
            // hM_ClusPhiOri_all->Fill(ClusPhi->at(i));
            hM_ClusPhiPV_all->Fill(hit->Phi(), vtxzwei);
            hM_ClusPhiPV_all_fine->Fill(hit->Phi(), vtxzwei);
            hM_ClusADC_all->Fill(ClusAdc->at(i), vtxzwei);
            hM_ClusPhiSize_all->Fill(ClusPhiSize->at(i), vtxzwei);
            hM_ClusX_ClusY_all->Fill(ClusX->at(i), ClusY->at(i), vtxzwei);
            if (ClusPhiSize->at(i) == 43 || ClusPhiSize->at(i) == 46)
            {
                hM_ClusPhiPV_ClusPhiSize43or46->Fill(hit->Phi(), vtxzwei);
                hM_ClusX_ClusY_ClusPhiSize43or46->Fill(ClusX->at(i), ClusY->at(i), vtxzwei);

                if (ClusPhiSize->at(i) == 43)
                {
                    hM_ClusX_ClusY_ClusPhiSize43->Fill(ClusX->at(i), ClusY->at(i), vtxzwei);
                }

                if (ClusPhiSize->at(i) == 46)
                {
                    hM_ClusX_ClusY_ClusPhiSize46->Fill(ClusX->at(i), ClusY->at(i), vtxzwei);
                }
            }

            hM_ClusX_ClusY_all_weiphisize->Fill(ClusX->at(i), ClusY->at(i), ClusPhiSize->at(i) * vtxzwei);
            hM_ClusX_ClusY_all_weiclusadc->Fill(ClusX->at(i), ClusY->at(i), ClusAdc->at(i) * vtxzwei);
            hM_ClusZ_ClusPhiPV_all_coarse->Fill(ClusZ->at(i), hit->Phi(), vtxzwei);
            hM_ClusZ_ClusPhiPV_all_coarse_weiphisize->Fill(ClusZ->at(i), hit->Phi(), ClusPhiSize->at(i) * vtxzwei);
            hM_ClusZ_ClusPhiPV_all_coarse_weiclusadc->Fill(ClusZ->at(i), hit->Phi(), ClusAdc->at(i) * vtxzwei);
            hM_ClusZ_ClusPhiPV_all->Fill(ClusZ->at(i), hit->Phi(), vtxzwei);
            hM_ClusPhiPV_ClusPhiSize_all->Fill(hit->Phi(), ClusPhiSize->at(i), vtxzwei);
            hM_ClusEtaPV_ClusADC_all->Fill(hit->Eta(), ClusAdc->at(i), vtxzwei);
            hM_ClusEtaPV_ClusADC_all_zoomin->Fill(hit->Eta(), ClusAdc->at(i), vtxzwei);
            hM_ClusEtaPV_ClusPhiSize_all->Fill(hit->Eta(), ClusPhiSize->at(i), vtxzwei);
            hM_ClusEtaPV_ClusPhiSize_all_zoomin->Fill(hit->Eta(), ClusPhiSize->at(i), vtxzwei);
            hM_ClusPhiPV_ClusADC_all->Fill(hit->Phi(), ClusAdc->at(i), vtxzwei);
            hM_ClusPhiSize_ClusADC_all->Fill(ClusPhiSize->at(i), ClusAdc->at(i), vtxzwei);
            hM_ClusPhiSize_ClusADC_all_zoomin->Fill(ClusPhiSize->at(i), ClusAdc->at(i), vtxzwei);
            // hM_ClusZSize_all->Fill(ClusZSize->at(i));
            // hM_ClusZSize_ClusPhiSize_all->Fill(ClusZSize->at(i), ClusPhiSize->at(i));
            // hM_ClusZSize_ClusADC_all->Fill(ClusZSize->at(i), ClusAdc->at(i));

            // apply the eta-dependent cluster ADC
            for (int constscale = constscale_low; constscale <= constscale_high; constscale += constscale_step)
            {
                // Using TH1F (i.e step function)
                if (ClusAdc->at(i) >
                    v_hM_ClusADCCut[(constscale - constscale_low) / constscale_step]->GetBinContent(v_hM_ClusADCCut[(constscale - constscale_low) / constscale_step]->FindBin(hit->Eta())))
                // Using TF1
                // if (ClusAdc->at(i) > v_f_ClusADCCut[(constscale - constscale_low) / constscale_step]->Eval(hit->Eta()))
                {
                    v_hM_ClusEtaPV_ClusADC_all_zoomin[(constscale - constscale_low) / constscale_step]->Fill(hit->Eta(), ClusAdc->at(i), vtxzwei);
                    v_hM_ClusEtaPV_EtaDepADCCut[(constscale - constscale_low) / constscale_step]->Fill(hit->Eta(), vtxzwei);
                }
            }

            for (float etascale = etascale_low; etascale <= etascale_high; etascale += etascale_step)
            {
                // Using TH1F (i.e step function)
                if (ClusAdc->at(i) >
                    v_hM_ClusADCCut_etascale[(int)((etascale - etascale_low) / etascale_step)]->GetBinContent(v_hM_ClusADCCut_etascale[(int)((etascale - etascale_low) / etascale_step)]->FindBin(hit->Eta())))
                // if (ClusAdc->at(i) > v_f_ClusADCCut_etascale[(int)((etascale - etascale_low) / etascale_step)]->Eval(hit->Eta()))
                {
                    v_hM_ClusEtaPV_ClusADC_all_zoomin_etascale[(int)((etascale - etascale_low) / etascale_step)]->Fill(hit->Eta(), ClusAdc->at(i), vtxzwei);
                    v_hM_ClusEtaPV_EtaDepADCCut_etascale[(int)((etascale - etascale_low) / etascale_step)]->Fill(hit->Eta(), vtxzwei);
                }
            }

            // for intt pricate cluster adc cut
            if (ClusAdc->at(i) > hM_ClusADCCut_inttprivate->GetBinContent(hM_ClusADCCut_inttprivate->FindBin(hit->Eta())))
            {
                hM_ClusEtaPV_ClusADC_all_zoomin_inttprivate->Fill(hit->Eta(), ClusAdc->at(i), vtxzwei);
                hM_ClusEtaPV_EtaDepADCCut_inttprivate->Fill(hit->Eta(), vtxzwei);
            }

            if (layer == 0)
            {

                // hM_ClusX_layer1->Fill(ClusX->at(i));
                // hM_ClusY_layer1->Fill(ClusY->at(i));
                hM_ClusZ_layer1->Fill(ClusZ->at(i), vtxzwei);
                // hM_ClusR_layer1->Fill(ClusR->at(i));
                // hM_ClusEtaOri_layer1->Fill(ClusEta->at(i));
                hM_ClusEtaPV_layer1->Fill(hit->Eta(), vtxzwei);
                // hM_ClusPhiOri_layer1->Fill(ClusPhi->at(i));
                hM_ClusPhiPV_layer1->Fill(hit->Phi(), vtxzwei);
                hM_ClusPhiPV_layer1_fine->Fill(hit->Phi(), vtxzwei);
                hM_ClusADC_layer1->Fill(ClusAdc->at(i), vtxzwei);
                hM_ClusPhiSize_layer1->Fill(ClusPhiSize->at(i), vtxzwei);
                hM_ClusZ_ClusPhiPV_layer1->Fill(ClusZ->at(i), hit->Phi(), vtxzwei);
                hM_ClusPhiPV_ClusPhiSize_layer1->Fill(hit->Phi(), ClusPhiSize->at(i), vtxzwei);
                hM_ClusEtaPV_ClusADC_layer1->Fill(hit->Eta(), ClusAdc->at(i), vtxzwei);
                hM_ClusEtaPV_ClusADC_layer1_zoomin->Fill(hit->Eta(), ClusAdc->at(i), vtxzwei);
                hM_ClusEtaPV_ClusPhiSize_layer1->Fill(hit->Eta(), ClusPhiSize->at(i)), vtxzwei;
                hM_ClusEtaPV_ClusPhiSize_layer1_zoomin->Fill(hit->Eta(), ClusPhiSize->at(i), vtxzwei);
                hM_ClusPhiPV_ClusADC_layer1->Fill(hit->Phi(), ClusAdc->at(i), vtxzwei);
                hM_ClusPhiSize_ClusADC_layer1->Fill(ClusPhiSize->at(i), ClusAdc->at(i), vtxzwei);
                hM_ClusPhiSize_ClusADC_layer1_zoomin->Fill(ClusPhiSize->at(i), ClusAdc->at(i)), vtxzwei;

                // hM_ClusZSize_layer1->Fill(ClusZSize->at(i));
                // hM_ClusEta_ClusZSize_layer1->Fill(hit->Eta(), ClusZSize->at(i));
                // hM_ClusZSize_ClusPhiSize_layer1->Fill(ClusZSize->at(i), ClusPhiSize->at(i));
                // hM_ClusZSize_ClusADC_layer1->Fill(ClusZSize->at(i), ClusAdc->at(i));
            }
            else
            {
                // hM_ClusX_layer2->Fill(ClusX->at(i));
                // hM_ClusY_layer2->Fill(ClusY->at(i));
                hM_ClusZ_layer2->Fill(ClusZ->at(i), vtxzwei);
                // hM_ClusR_layer2->Fill(ClusR->at(i));
                // hM_ClusEtaOri_layer2->Fill(ClusEta->at(i));
                hM_ClusEtaPV_layer2->Fill(hit->Eta(), vtxzwei);
                // hM_ClusPhiOri_layer2->Fill(ClusPhi->at(i));
                hM_ClusPhiPV_layer2->Fill(hit->Phi(), vtxzwei);
                hM_ClusPhiPV_layer2_fine->Fill(hit->Phi(), vtxzwei);
                hM_ClusADC_layer2->Fill(ClusAdc->at(i), vtxzwei);
                hM_ClusPhiSize_layer2->Fill(ClusPhiSize->at(i), vtxzwei);
                hM_ClusZ_ClusPhiPV_layer2->Fill(ClusZ->at(i), hit->Phi(), vtxzwei);
                hM_ClusPhiPV_ClusPhiSize_layer2->Fill(hit->Phi(), ClusPhiSize->at(i), vtxzwei);
                hM_ClusEtaPV_ClusADC_layer2->Fill(hit->Eta(), ClusAdc->at(i), vtxzwei);
                hM_ClusEtaPV_ClusADC_layer2_zoomin->Fill(hit->Eta(), ClusAdc->at(i), vtxzwei);
                hM_ClusEtaPV_ClusPhiSize_layer2->Fill(hit->Eta(), ClusPhiSize->at(i), vtxzwei);
                hM_ClusEtaPV_ClusPhiSize_layer2_zoomin->Fill(hit->Eta(), ClusPhiSize->at(i), vtxzwei);
                hM_ClusPhiPV_ClusADC_layer2->Fill(hit->Phi(), ClusAdc->at(i), vtxzwei);
                hM_ClusPhiSize_ClusADC_layer2->Fill(ClusPhiSize->at(i), ClusAdc->at(i), vtxzwei);
                hM_ClusPhiSize_ClusADC_layer2_zoomin->Fill(ClusPhiSize->at(i), ClusAdc->at(i), vtxzwei);

                // hM_ClusZSize_layer2->Fill(ClusZSize->at(i));
                // hM_ClusEta_ClusZSize_layer2->Fill(ClusEta->at(i), ClusZSize->at(i));
                // hM_ClusZSize_ClusPhiSize_layer2->Fill(ClusZSize->at(i), ClusPhiSize->at(i));
                // hM_ClusZSize_ClusADC_layer2->Fill(ClusZSize->at(i), ClusAdc->at(i));
            }
        }
    }

    f->Close();

    TFile *fout = new TFile(outfilename, "RECREATE");
    fout->cd();
    // hM_ClusX_all->Write();
    // hM_ClusX_layer1->Write();
    // hM_ClusX_layer2->Write();
    // hM_ClusY_all->Write();
    // hM_ClusY_layer1->Write();
    // hM_ClusY_layer2->Write();
    hM_ClusZ_all->Write();
    hM_ClusZ_layer1->Write();
    hM_ClusZ_layer2->Write();
    // hM_ClusR_all->Write();
    // hM_ClusR_layer1->Write();
    // hM_ClusR_layer2->Write();
    hM_ClusEtaPV_all->Write();
    hM_ClusEtaPV_layer1->Write();
    hM_ClusEtaPV_layer2->Write();
    // hM_ClusEtaOri_all->Write();
    // hM_ClusEtaOri_layer1->Write();
    // hM_ClusEtaOri_layer2->Write();
    // hM_ClusPhiOri_all->Write();
    // hM_ClusPhiOri_layer1->Write();
    // hM_ClusPhiOri_layer2->Write();
    hM_ClusPhiPV_all->Write();
    hM_ClusPhiPV_layer1->Write();
    hM_ClusPhiPV_layer2->Write();
    hM_ClusPhiPV_ClusPhiSize43or46->Write();
    hM_ClusPhiPV_all_fine->Write();
    hM_ClusPhiPV_layer1_fine->Write();
    hM_ClusPhiPV_layer2_fine->Write();
    hM_ClusADC_all->Write();
    hM_ClusADC_layer1->Write();
    hM_ClusADC_layer2->Write();
    // hM_ClusZSize_all->Write();
    // hM_ClusZSize_layer1->Write();
    // hM_ClusZSize_layer2->Write();
    hM_ClusPhiSize_all->Write();
    hM_ClusPhiSize_layer1->Write();
    hM_ClusPhiSize_layer2->Write();
    hM_ClusX_ClusY_all->Write();
    hM_ClusX_ClusY_ClusPhiSize43or46->Write();
    hM_ClusX_ClusY_ClusPhiSize43->Write();
    hM_ClusX_ClusY_ClusPhiSize46->Write();
    hM_ClusX_ClusY_all_weiphisize->Write();
    hM_ClusX_ClusY_all_weiclusadc->Write();
    hM_ClusZ_ClusPhiPV_all_coarse->Write();
    hM_ClusZ_ClusPhiPV_all_coarse_weiphisize->Write();
    hM_ClusZ_ClusPhiPV_all_coarse_weiclusadc->Write();
    hM_ClusZ_ClusPhiPV_all->Write();
    hM_ClusZ_ClusPhiPV_layer1->Write();
    hM_ClusZ_ClusPhiPV_layer2->Write();
    // hM_ClusEta_ClusZSize_all->Write();
    // hM_ClusEta_ClusZSize_layer1->Write();
    // hM_ClusEta_ClusZSize_layer2->Write();
    hM_ClusPhiPV_ClusPhiSize_all->Write();
    hM_ClusPhiPV_ClusPhiSize_layer1->Write();
    hM_ClusPhiPV_ClusPhiSize_layer2->Write();
    hM_ClusEtaPV_ClusADC_all->Write();
    hM_ClusEtaPV_ClusADC_layer1->Write();
    hM_ClusEtaPV_ClusADC_layer2->Write();
    hM_ClusEtaPV_ClusADC_all_zoomin->Write();
    hM_ClusEtaPV_ClusADC_layer1_zoomin->Write();
    hM_ClusEtaPV_ClusADC_layer2_zoomin->Write();
    hM_ClusEtaPV_ClusPhiSize_all->Write();
    hM_ClusEtaPV_ClusPhiSize_layer1->Write();
    hM_ClusEtaPV_ClusPhiSize_layer2->Write();
    hM_ClusEtaPV_ClusPhiSize_all_zoomin->Write();
    hM_ClusEtaPV_ClusPhiSize_layer1_zoomin->Write();
    hM_ClusEtaPV_ClusPhiSize_layer2_zoomin->Write();
    hM_ClusPhiPV_ClusADC_all->Write();
    hM_ClusPhiPV_ClusADC_layer1->Write();
    hM_ClusPhiPV_ClusADC_layer2->Write();
    // hM_ClusZSize_ClusPhiSize_all->Write();
    // hM_ClusZSize_ClusPhiSize_layer1->Write();
    // hM_ClusZSize_ClusPhiSize_layer2->Write();
    // hM_ClusZSize_ClusADC_all->Write();
    // hM_ClusZSize_ClusADC_layer1->Write();
    // hM_ClusZSize_ClusADC_layer2->Write();
    hM_ClusPhiSize_ClusADC_all->Write();
    hM_ClusPhiSize_ClusADC_layer1->Write();
    hM_ClusPhiSize_ClusADC_layer2->Write();
    hM_ClusPhiSize_ClusADC_all_zoomin->Write();
    hM_ClusPhiSize_ClusADC_layer1_zoomin->Write();
    hM_ClusPhiSize_ClusADC_layer2_zoomin->Write();
    // For cluster ADC cut optimization
    for (int constscale = constscale_low; constscale <= constscale_high; constscale += constscale_step)
    {
        v_hM_ClusEtaPV_EtaDepADCCut[(constscale - constscale_low) / constscale_step]->Write();
        v_hM_ClusEtaPV_ClusADC_all_zoomin[(constscale - constscale_low) / constscale_step]->Write();
    }
    for (int i = 0; i < v_hM_ClusEtaPV_EtaDepADCCut_etascale.size(); i++)
    {
        v_hM_ClusEtaPV_EtaDepADCCut_etascale[i]->Write();
        v_hM_ClusEtaPV_ClusADC_all_zoomin_etascale[i]->Write();
    }
    // for intt private cluster adc cut
    hM_ClusEtaPV_EtaDepADCCut_inttprivate->Write();
    hM_ClusEtaPV_ClusADC_all_zoomin_inttprivate->Write();
    fout->Close();
}
