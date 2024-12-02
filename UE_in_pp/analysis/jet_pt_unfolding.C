#include <iostream>
#include <TH2D.h>
#include <TH1D.h>
#include <TChain.h>
#include <TMath.h>
#include <TTree.h>
#include <TFile.h>
#include <sstream> //std::ostringstsream
#include <fstream> //std::ifstream
#include <iostream> //std::cout, std::endl
#include <cmath>
#include <TGraphErrors.h>
#include <TGraph.h>
#include <TSpectrum.h>
#include <TF1.h>
#include <TTreeReader.h>
#include <TTreeReaderValue.h>
#include <TTreeReaderArray.h>
#include <string>
#include <set>
#include <TVector3.h>
#include <map>
#include <vector>
#include <TDatabasePDG.h>
#include <tuple>
#include <TProfile.h>
#include <TProfile2D.h>
#include "TRandom.h"
#include "TH1D.h"

using namespace std;

R__LOAD_LIBRARY(/sphenix/user/egm2153/calib_study/JetValidation/analysis/roounfold/libRooUnfold.so)

bool isInRange(float truthJetPt, float mcWeight);
void getLeadSubleadJet(std::vector<float> *pt, std::vector<float> *eta, int &ind_lead, int &ind_sub);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                              //
//  Note: 11.26.24                                                                                                              //
//  This macro creates dijet QA plots and does 1D unfolding of leading jet pT for back-to-back dijet events                     //
//                                                                                                                              //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void jet_pt_unfolding(string filename = "jet_pt_unfolding.root") {
    
    // event and jet histograms 
    TH1F* h_pass_deltaphi = new TH1F("h_pass_deltaphi","",125,-2*M_PI,2*M_PI);
    TH1F* h_pass_xj = new TH1F("h_pass_xj","",20,0,1);
    TH1F* h_pass_spectra = new TH1F("h_pass_spectra","",50,0,50);
    TH2F* h_pass_aj_ptavg = new TH2F("h_pass_aj_ptavg","",100,0,100,100,0,1);
    TH1F* h_pass_truth_deltaphi = new TH1F("h_pass_truth_deltaphi","",125,-2*M_PI,2*M_PI);
    TH1F* h_pass_truth_xj = new TH1F("h_pass_truth_xj","",20,0,1);
    TH1F* h_pass_truth_spectra = new TH1F("h_pass_truth_spectra","",50,0,50);
    TH2F* h_pass_truth_aj_ptavg = new TH2F("h_pass_truth_aj_ptavg","",100,0,100,100,0,1);

    // should create unfolding histograms and response matrices here
    //Needed for gaus function
    TRandom3 obj;

    //random number in each event
    TRandom3 Random;

    double ptbins[] = {5,5.5,6,6.5,7,7.5,8,8.5,9,9.5,10,10.5,11,11.5,12,12.5,13,13.5,14,14.5,15,16,17,18,20,22,24,26,28,31,35,40,50};
    float dRMax = 0.3;
    float leadptmin = 10;
    float subptmin = 5;

    int nptbins = sizeof(ptbins) / sizeof(ptbins[0]) - 1;
    bool doUnfolding = true;
    bool sim = true;
    bool topoclusters = true;
    bool applyCorr = true;

    // need to adjust ptbins to have equally significant bins 
    // need to have a much lower pT to start 
    TProfile* jes_ratio = new TProfile("jes_ratio","",nptbins,ptbins);

    //defining Meas and Truth Histograms
    TH1D* hMeasPT = new TH1D("hMeasPT","",nptbins,ptbins);
    TH1D* hTruthPT = new TH1D("hTruthPT","",nptbins,ptbins);

    // closure test histograms 
    TH1D* hMeasPTHalf = new TH1D("hMeasPTHalf","",nptbins,ptbins);
    TH1D* hTruthPTHalf = new TH1D("hTruthPTHalf","",nptbins,ptbins);

    //making response matrices
    RooUnfoldResponse *resp_full = new RooUnfoldResponse(hMeasPT,hTruthPT,"resp_full","");
    RooUnfoldResponse *resp_half = new RooUnfoldResponse(hMeasPTHalf,hTruthPTHalf,"resp_half","");

    //histograms for errors
    TH2D* hResponseTruthMeasFull = new TH2D("hResponseTruthMeasFull","",nptbins,ptbins,nptbins,ptbins);
    TH2D* hResponseTruthMeasHalf = new TH2D("hResponseTruthMeasHalf","",nptbins,ptbins,nptbins,ptbins);

    //histograms and responses for inverse testing
    TProfile* inverse_jes_ratio = new TProfile("inverse_jes_ratio","",nptbins,ptbins);
    TH1D* hInverseMeasPT = new TH1D("hInverseMeasPT","",nptbins,ptbins);
    TH1D* hInverseTruthPT = new TH1D("hInverseTruthPT","",nptbins,ptbins);
    TH1D* hInverseMeasPTHalf = new TH1D("hInverseMeasPTHalf","",nptbins,ptbins);
    TH1D* hInverseTruthPTHalf = new TH1D("hInverseTruthPTHalf","",nptbins,ptbins);
    RooUnfoldResponse *inverse_resp_full = new RooUnfoldResponse(hInverseMeasPT,hInverseTruthPT,"inverse_resp_full","");
    RooUnfoldResponse *inverse_resp_half = new RooUnfoldResponse(hInverseMeasPTHalf,hInverseTruthPTHalf,"inverse_resp_half","");

    TChain chain("T");
    const char* inputDirectory = "/sphenix/tg/tg01/jets/egm2153/";
    //TString wildcardPath = TString::Format("%sJetValOutput/sim_truth_jet_output.root", inputDirectory); // run 15 dataset
    //chain.Add(wildcardPath);
    
    for (int i = 0; i < 1000; i++) {
        TString wildcardPath = TString::Format("%sUEinppOutput/sim_run22_jet10_output_%d.root", inputDirectory, i); // run 22 dataset
        chain.Add(wildcardPath);
    }

    int m_event;
    int nJet;
    int nTruthJet;
    float zvtx;
    float deltaeta = 0.0916667;
    float deltaphi = 0.0981748;
    float secteta = 2.2;
    float sectphi = (2.0*M_PI)/3.0;

    vector<int> *triggerVector = nullptr;
    vector<float> *eta = nullptr;
    vector<float> *phi = nullptr;
    vector<float> *e = nullptr;
    vector<float> *pt = nullptr;
    vector<float> *truthEta = nullptr;
    vector<float> *truthPhi = nullptr;
    vector<float> *truthE = nullptr;
    vector<float> *truthPt = nullptr;

    chain.SetBranchAddress("m_event",&m_event);
    chain.SetBranchAddress("nJet",&nJet);
    chain.SetBranchAddress("nTruthJet",&nTruthJet);
    chain.SetBranchAddress("zvtx",&zvtx);
    chain.SetBranchAddress("triggerVector",&triggerVector);
    chain.SetBranchAddress("eta",&eta);
    chain.SetBranchAddress("phi",&phi);
    chain.SetBranchAddress("e",&e);
    chain.SetBranchAddress("pt",&pt);
    chain.SetBranchAddress("truthEta",&truthEta);
    chain.SetBranchAddress("truthPhi",&truthPhi);
    chain.SetBranchAddress("truthE",&truthE);
    chain.SetBranchAddress("truthPt",&truthPt);

    TFile *corrFile;
    TF1 *correction = new TF1("jet energy correction","1",0,80);
    if(applyCorr) {
        corrFile = new TFile("JES_IsoCorr_NumInv.root","READ");
        corrFile -> cd();
        correction = (TF1*)corrFile -> Get("corrFit_Iso0");
        corrFile->Close();
    }

    int eventnumber = 0;
    int events = 0;
    Long64_t nEntries = chain.GetEntries();
    std::cout << nEntries << std::endl;
    for (Long64_t entry = 0; entry < nEntries; ++entry) {
    //for (Long64_t entry = 0; entry < 2000; ++entry) {
        chain.GetEntry(entry);
        if (eventnumber % 10000 == 0) cout << "event " << eventnumber << endl;
        eventnumber++;

        bool negJet = false;
        for (int i = 0; i < nJet; i++) {
            if (e->at(i) < 0) {
                negJet = true;
            }
        }

        // require event with |zvtx| < 30 cm 
        if (isnan(zvtx)) { continue; }
        if (zvtx < -30 || zvtx > 30) { continue; }
        if (negJet) { continue; }

        /*
        // implemented to match truth jet eta cut to reco jet eta cut 
        for (int i = 0; i < truthEta->size();) {
            if (fabs(truthEta->at(i)) > 0.7) {
                truthEta->erase(truthEta->begin() + i);
                truthPt->erase(truthPt->begin() + i);
                truthE->erase(truthE->begin() + i);
                truthPhi->erase(truthPhi->begin() + i);
            } else {
                ++i;
            }
        }
        int nTruthJet = truthPt->size();
        */

        // indices to find leading and subleading jets 
        int ind_truth_lead = -1;
        int ind_truth_sub = -1;
        int ind_lead = -1;
        int ind_sub = -1;

        // if both nreco jets < 2 and nTruthJet jets < 2, discard event
        if (nJet < 2 && nTruthJet < 2) {
            continue;
        }

        // find reco leading and subleading jets if nreco jets >= 2
        TVector3 lead, sub;
        if (nJet >= 2) {
            getLeadSubleadJet(pt, eta, ind_lead, ind_sub);  
            lead.SetPtEtaPhi(pt->at(ind_lead)*correction->Eval(pt->at(ind_lead)), eta->at(ind_lead), phi->at(ind_lead));
            sub.SetPtEtaPhi(pt->at(ind_sub)*correction->Eval(pt->at(ind_sub)), eta->at(ind_sub), phi->at(ind_sub));
        } else {
            lead.SetPtEtaPhi(0,0,0);
            sub.SetPtEtaPhi(0,0,0);
        }

        //find truth leading and subleading jets if nTruthJet jets >= 2
        TVector3 truthlead, truthsub;
        if (nTruthJet >= 2) {
            getLeadSubleadJet(truthPt, truthEta, ind_truth_lead, ind_truth_sub);
            truthlead.SetPtEtaPhi(truthPt->at(ind_truth_lead), truthEta->at(ind_truth_lead), truthPhi->at(ind_truth_lead));
            truthsub.SetPtEtaPhi(truthPt->at(ind_truth_sub), truthEta->at(ind_truth_sub), truthPhi->at(ind_truth_sub));
        } else {
            truthlead.SetPtEtaPhi(0,0,0);
            truthsub.SetPtEtaPhi(0,0,0);
        }

        // if reco or truth info passes dijet pT and back to back criteria, record event 
        if ((nJet >= 2 && lead.Pt() > leadptmin && sub.Pt() > subptmin && fabs(lead.DeltaPhi(sub)) > 2.75) || (nTruthJet >= 2 && truthlead.Pt() > leadptmin && truthsub.Pt() > subptmin && fabs(truthlead.DeltaPhi(truthsub)) > 2.75)) {
            double  choice = Random.Rndm();
            h_pass_deltaphi->Fill(lead.DeltaPhi(sub));
            h_pass_xj->Fill(sub.Pt()/lead.Pt());
            h_pass_spectra->Fill(lead.Pt());
            h_pass_spectra->Fill(sub.Pt());
            h_pass_aj_ptavg->Fill((lead.Pt()+sub.Pt())/2.0, (lead.Pt()-sub.Pt())/(lead.Pt()+sub.Pt()));
            
            h_pass_truth_deltaphi->Fill(truthlead.DeltaPhi(truthsub));
            h_pass_truth_xj->Fill(truthsub.Pt()/truthlead.Pt());
            h_pass_truth_spectra->Fill(truthlead.Pt());
            h_pass_truth_spectra->Fill(truthsub.Pt());
            h_pass_truth_aj_ptavg->Fill((truthlead.Pt()+truthsub.Pt())/2.0, (truthlead.Pt()-truthsub.Pt())/(truthlead.Pt()+truthsub.Pt()));

            if ((nJet >= 2 && lead.Pt() > leadptmin && sub.Pt() > subptmin && fabs(lead.DeltaPhi(sub)) > 2.75) && (nTruthJet >= 2 && truthlead.Pt() > leadptmin && truthsub.Pt() > subptmin && fabs(truthlead.DeltaPhi(truthsub)) > 2.75)) {
                if (truthlead.DeltaR(lead) < dRMax && truthsub.DeltaR(sub) < dRMax) { // should this match be both the leading and subleading? 
                    // MATCH 
                    hMeasPT->Fill(lead.Pt());
                    hTruthPT->Fill(truthlead.Pt());
                    resp_full->Fill(lead.Pt(),truthlead.Pt());
                    jes_ratio->Fill(truthlead.Pt(),lead.Pt()/truthlead.Pt());
                    if (choice > 0.5) {
                        hTruthPTHalf->Fill(truthlead.Pt());
                        resp_half->Fill(lead.Pt(),truthlead.Pt());
                    } else {
                        hMeasPTHalf->Fill(lead.Pt());
                    }
                } else {
                    // FAKE AND MISS
                    hTruthPT->Fill(truthlead.Pt());
                    resp_full->Miss(truthlead.Pt());
                    if (choice > 0.5) {
                        hTruthPTHalf->Fill(truthlead.Pt());
                        resp_half->Miss(truthlead.Pt());
                    }
                    hMeasPT->Fill(lead.Pt());
                    resp_full->Fake(lead.Pt());
                    if (choice > 0.5) {
                        resp_half->Fake(lead.Pt());
                    } else {
                        hMeasPTHalf->Fill(lead.Pt());
                    }
                }
            } else if (nJet >= 2 && lead.Pt() > leadptmin && sub.Pt() > subptmin && fabs(lead.DeltaPhi(sub)) > 2.75) {
                // FAKE
                hMeasPT->Fill(lead.Pt());
                resp_full->Fake(lead.Pt());
                if (choice > 0.5) {
                    resp_half->Fake(lead.Pt());
                } else {
                    hMeasPTHalf->Fill(lead.Pt());
                }
            } else if (nTruthJet >= 2 && truthlead.Pt() > leadptmin && truthsub.Pt() > subptmin && fabs(truthlead.DeltaPhi(truthsub)) > 2.75) {
                // MISS
                hTruthPT->Fill(truthlead.Pt());
                resp_full->Miss(truthlead.Pt());
                if (choice > 0.5) {
                    hTruthPTHalf->Fill(truthlead.Pt());
                    resp_half->Miss(truthlead.Pt());
                }
            }

            // inverse unfolding testing 
            if ((nJet >= 2 && lead.Pt() > leadptmin && sub.Pt() > subptmin && fabs(lead.DeltaPhi(sub)) > 2.75) && (nTruthJet >= 2 && truthlead.Pt() > leadptmin && truthsub.Pt() > subptmin && fabs(truthlead.DeltaPhi(truthsub)) > 2.75)) {
                if (lead.DeltaR(truthlead) < dRMax && sub.DeltaR(truthsub) < dRMax) { // should this match be both the leading and subleading? 
                    // MATCH 
                    hInverseMeasPT->Fill(truthlead.Pt());
                    hInverseTruthPT->Fill(lead.Pt());
                    inverse_resp_full->Fill(truthlead.Pt(),lead.Pt());
                    inverse_jes_ratio->Fill(lead.Pt(),truthlead.Pt()/lead.Pt());
                    if (choice > 0.5) {
                        hInverseTruthPTHalf->Fill(lead.Pt());
                        inverse_resp_half->Fill(truthlead.Pt(),lead.Pt());
                    } else {
                        hInverseMeasPTHalf->Fill(truthlead.Pt());
                    }
                } else {
                    // FAKE AND MISS
                    hInverseTruthPT->Fill(lead.Pt());
                    inverse_resp_full->Miss(lead.Pt());
                    if (choice > 0.5) {
                        hInverseTruthPTHalf->Fill(lead.Pt());
                        inverse_resp_half->Miss(lead.Pt());
                    }
                    hInverseMeasPT->Fill(truthlead.Pt());
                    inverse_resp_full->Fake(truthlead.Pt());
                    if (choice > 0.5) {
                        inverse_resp_half->Fake(truthlead.Pt());
                    } else {
                        hInverseMeasPTHalf->Fill(truthlead.Pt());
                    }
                }
            } else if (nTruthJet >= 2 && truthlead.Pt() > leadptmin && truthsub.Pt() > subptmin && fabs(truthlead.DeltaPhi(truthsub)) > 2.75) {
                // FAKE
                hInverseMeasPT->Fill(truthlead.Pt());
                inverse_resp_full->Fake(truthlead.Pt());
                if (choice > 0.5) {
                    inverse_resp_half->Fake(truthlead.Pt());
                } else {
                    hInverseMeasPTHalf->Fill(truthlead.Pt());
                }
            } else if (nJet >= 2 && lead.Pt() > leadptmin && sub.Pt() > subptmin && fabs(lead.DeltaPhi(sub)) > 2.75) {
                // MISS
                hInverseTruthPT->Fill(lead.Pt());
                inverse_resp_full->Miss(lead.Pt());
                if (choice > 0.5) {
                    hInverseTruthPTHalf->Fill(lead.Pt());
                    inverse_resp_half->Miss(lead.Pt());
                }
            }
            // inverse unfolding testing 

            events++;
        }

    }

    // unfolding 
    RooUnfoldBayes *full_unfold = new RooUnfoldBayes(resp_full,hMeasPT,1);
    //full_unfold->SetNToys(100);
    TH1D*  hRecoPT = (TH1D*)full_unfold->Hreco();
    hRecoPT->SetName("hRecoPT");

    RooUnfoldBayes *half_unfold = new RooUnfoldBayes(resp_half,hMeasPTHalf,4);
    TH1D* hRecoPTHalf = (TH1D*)half_unfold->Hreco();
    hRecoPTHalf->SetName("hRecoPTHalf");

    // inverse unfolding
    RooUnfoldBayes *inverse_full_unfold = new RooUnfoldBayes(inverse_resp_full,hInverseMeasPT,1);
    TH1D*  hInverseRecoPT = (TH1D*)inverse_full_unfold->Hreco();
    hInverseRecoPT->SetName("hInverseRecoPT");

    RooUnfoldBayes *inverse_half_unfold = new RooUnfoldBayes(inverse_resp_half,hInverseMeasPTHalf,4);
    TH1D* hInverseRecoPTHalf = (TH1D*)inverse_half_unfold->Hreco();
    hInverseRecoPTHalf->SetName("hInverseRecoPTHalf");
    
    // output histograms 
    std::cout << filename << std::endl;
    TFile *out = new TFile(filename.c_str(),"RECREATE");

    out->cd();
    h_pass_deltaphi->Write();
    h_pass_xj->Write();
    h_pass_spectra->Write();
    h_pass_aj_ptavg->Write();
    h_pass_truth_deltaphi->Write();
    h_pass_truth_xj->Write();
    h_pass_truth_spectra->Write();
    h_pass_aj_ptavg->Write();
    jes_ratio->Write();
    hMeasPT->Write();
    hTruthPT->Write();
    hRecoPT->Write();
    hMeasPTHalf->Write();
    hTruthPTHalf->Write();
    hRecoPTHalf->Write();
    resp_full->Write();
    resp_half->Write();
    inverse_jes_ratio->Write();
    hInverseMeasPT->Write();
    hInverseTruthPT->Write();
    hInverseRecoPT->Write();
    hInverseMeasPTHalf->Write();
    hInverseTruthPTHalf->Write();
    hInverseRecoPTHalf->Write();
    inverse_resp_full->Write();
    inverse_resp_half->Write();
    out->Close();

}

bool isInRange(float truthJetPt, float mcWeight)
{
    float ptCutOff1 = -1; 
    float ptCutOff2 = -1;


    if(abs(mcWeight/39.06e-3 - 1) < 1e-7)//Minimum bias
    {
        //std::cout << "MB event found" << std::endl;
        ptCutOff1 = 0;
        ptCutOff2 = 14;
    }
    else if(abs(mcWeight/3.210e-6 - 1) < 1e-7)
    {
        //std::cout << "10GeV event found" << std::endl;
        ptCutOff1 = 14;
        ptCutOff2 = 37;
    }
    else if(abs(mcWeight/2.178e-9 - 1) < 1e-7)
    {
        //std::cout << "30GeV event found" << std::endl;
        ptCutOff1 = 37;
        ptCutOff2 = 3000;
    }

    if(truthJetPt < ptCutOff2 && truthJetPt >= ptCutOff1) return true;
    return false;
    }
 
void getLeadSubleadJet(std::vector<float> *pt, std::vector<float> *eta, int &ind_lead, int &ind_sub)
{
    float temp_lead = -1;
    float temp_sub = -1;
    if (pt->size() < 2 || eta->size() < 2 || pt->size() != eta->size()) { std::cout << "PT and ETA vectors smaller than 2 or not equal, something is wrong!" << std::endl; return; }

    for (int i = 0; i < pt->size(); i++) {
        if (pt->at(i) > temp_lead) {
            if (temp_lead != -1) {
                temp_sub = temp_lead;
                ind_sub = ind_lead;
            }
            temp_lead = pt->at(i);
            ind_lead = i;
        } else if (pt->at(i) > temp_sub) {
            temp_sub = pt->at(i);
            ind_sub = i;
        }
    }
}