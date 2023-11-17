#include <iostream>
#include <vector>

using namespace std;


Int_t MatchJets(TString infile="../macro/output.root", TString outfile="matched.root", Float_t JetParameter=0.4, Float_t pTHard=30.0)
{

    // Set event selection cuts
    double dRMax = 0.75*JetParameter;
    float pTmin_truth = 10;
    float pTmin_reco = 5;
    float pThard_min, pThard_max;
    float weight;
    if(pTHard == 10){
        pThard_min = 10;
        pThard_max = 30;
        weight = 3.210e-6;
    }
    else{
        pThard_min = 30;
        pThard_max = 999;
        weight = 2.178e-9;
    }

    
    //=======================================================
    //================ Input File ===========================
    //=======================================================
    std::cout << "Input file: " << infile.Data() << std::endl;
   
    // open input file
    TFile fin(infile.Data(), "READ");
    if(!fin.IsOpen())
    {
        std::cout << "Error: could not open input file" << std::endl;
        exit(-1);
    }

    // get tree
    TTree *tree = (TTree*)fin.Get("T");
    if(!tree)
    {
        std::cout << "Error: could not find tree" << std::endl;
        exit(-1);
    }

    int event = 0;
    int centrality = 0;
    float event_leading_truth_pt = 0;
    double rho_area = 0;
    double rho_area_sigma = 0;
    double rho_mult = 0;

    // reco jet variables 
    std::vector<float> * iter_eta = 0;
    std::vector<float> * iter_phi = 0;
    std::vector<float> * iter_pt = 0;
    std::vector<float> * iter_pt_unsub = 0;

    // mult jet variables
    std::vector<int> * mult_ncomponent = 0;
    std::vector<float> * mult_eta = 0;
    std::vector<float> * mult_phi = 0;
    std::vector<float> * mult_pt = 0;
    std::vector<float> * mult_pt_unsub = 0;
    std::vector<int> * mult_nsignal = 0;

    // truth jet variables
    std::vector<int> * truth_ncomponent = 0;
    std::vector<float> * truth_eta = 0;
    std::vector<float> * truth_phi = 0;
    std::vector<float> * truth_pt = 0;

    //rhoA jet variables
    std::vector<float> * rhoA_eta = 0;
    std::vector<float> * rhoA_phi = 0;
    std::vector<float> * rhoA_pt = 0;
    std::vector<float> * rhoA_pt_unsub = 0;
    std::vector<float> * rhoA_area = 0;

    tree->SetBranchAddress("event", &event);
    tree->SetBranchAddress("event_leading_truth_pt", &event_leading_truth_pt);
    tree->SetBranchAddress("centrality", &centrality);
    tree->SetBranchAddress("truth_ncomponent", &truth_ncomponent);
    tree->SetBranchAddress("truth_eta", &truth_eta);
    tree->SetBranchAddress("truth_phi", &truth_phi);
    tree->SetBranchAddress("truth_pt", &truth_pt);
    tree->SetBranchAddress("rho_area", &rho_area);
    tree->SetBranchAddress("rho_area_sigma", &rho_area_sigma);
    tree->SetBranchAddress("rhoA_eta", &rhoA_eta);
    tree->SetBranchAddress("rhoA_phi", &rhoA_phi);
    tree->SetBranchAddress("rhoA_pt", &rhoA_pt);
    tree->SetBranchAddress("rhoA_area", &rhoA_area);
    tree->SetBranchAddress("rhoA_pt_unsub", &rhoA_pt_unsub);
    tree->SetBranchAddress("rho_mult", &rho_mult);
    tree->SetBranchAddress("mult_ncomponent", &mult_ncomponent);
    tree->SetBranchAddress("mult_nsignal", &mult_nsignal);
    tree->SetBranchAddress("mult_eta", &mult_eta);
    tree->SetBranchAddress("mult_phi", &mult_phi);
    tree->SetBranchAddress("mult_pt", &mult_pt);
    tree->SetBranchAddress("mult_pt_unsub", &mult_pt_unsub);
    tree->SetBranchAddress("iter_eta", &iter_eta);
    tree->SetBranchAddress("iter_phi", &iter_phi);
    tree->SetBranchAddress("iter_pt", &iter_pt);
    tree->SetBranchAddress("iter_pt_unsub", &iter_pt_unsub);


    //=======================================================

    // output file
    std::cout << "Output file: " << outfile.Data() << std::endl;
    TFile *fout = new TFile(outfile.Data(), "RECREATE");
    TTree *out_tree = new TTree("T", "MatchedJets");

    // output variables
    int out_centrality = 0;
    double out_rho_area = 0;
    double out_rho_mult = 0;

    std::vector<float> matched_pt_iter, matched_pt_area, matched_pt_mult;
    std::vector<float> matched_pt_iter_unsub, matched_pt_area_unsub, matched_pt_mult_unsub;
    std::vector<float> matched_truth_pt_iter, matched_truth_pt_area, matched_truth_pt_mult;
    std::vector<float> unmatched_pt_iter, unmatched_pt_area, unmatched_pt_mult;
    std::vector<float> unmatched_pt_iter_unsub, unmatched_pt_area_unsub, unmatched_pt_mult_unsub;
    std::vector<float> missed_truth_pt_iter, missed_truth_pt_area, missed_truth_pt_mult;

    // set branch addresses
    out_tree->Branch("weight", &weight);
    out_tree->Branch("centrality", &out_centrality);
    out_tree->Branch("rho_area", &out_rho_area);
    out_tree->Branch("rho_mult", &out_rho_mult);
    out_tree->Branch("matched_pt_iter", &matched_pt_iter);
    out_tree->Branch("matched_pt_area", &matched_pt_area);
    out_tree->Branch("matched_pt_mult", &matched_pt_mult);
    out_tree->Branch("matched_pt_iter_unsub", &matched_pt_iter_unsub);
    out_tree->Branch("matched_pt_area_unsub", &matched_pt_area_unsub);
    out_tree->Branch("matched_pt_mult_unsub", &matched_pt_mult_unsub);
    out_tree->Branch("matched_truth_pt_iter", &matched_truth_pt_iter);
    out_tree->Branch("matched_truth_pt_area", &matched_truth_pt_area);
    out_tree->Branch("matched_truth_pt_mult", &matched_truth_pt_mult);
    out_tree->Branch("unmatched_pt_iter", &unmatched_pt_iter);
    out_tree->Branch("unmatched_pt_area", &unmatched_pt_area);
    out_tree->Branch("unmatched_pt_mult", &unmatched_pt_mult);
    out_tree->Branch("unmatched_pt_iter_unsub", &unmatched_pt_iter_unsub);
    out_tree->Branch("unmatched_pt_area_unsub", &unmatched_pt_area_unsub);
    out_tree->Branch("unmatched_pt_mult_unsub", &unmatched_pt_mult_unsub);
    out_tree->Branch("missed_truth_pt_iter", &missed_truth_pt_iter);
    out_tree->Branch("missed_truth_pt_area", &missed_truth_pt_area);
    out_tree->Branch("missed_truth_pt_mult", &missed_truth_pt_mult);

    // get number of entries
    int nEntries = tree->GetEntries();
    for (int iEvent = 0; iEvent < nEntries; iEvent++){
        // get entry
        tree->GetEntry(iEvent);

        // event selection
        if(event_leading_truth_pt < pThard_min || event_leading_truth_pt > pThard_max) continue;

        // clear output variables
        out_centrality = centrality;
        out_rho_area = rho_area;
        out_rho_mult = rho_mult;
        // clear vectors
        matched_pt_iter.clear();
        matched_pt_area.clear();
        matched_pt_mult.clear();
        matched_pt_iter_unsub.clear();
        matched_pt_area_unsub.clear();
        matched_pt_mult_unsub.clear();  
        matched_truth_pt_iter.clear();
        matched_truth_pt_area.clear();
        matched_truth_pt_mult.clear();
        unmatched_pt_iter.clear();
        unmatched_pt_area.clear();
        unmatched_pt_mult.clear();
        unmatched_pt_iter_unsub.clear();
        unmatched_pt_area_unsub.clear();
        unmatched_pt_mult_unsub.clear();
        missed_truth_pt_iter.clear();
        missed_truth_pt_area.clear();
        missed_truth_pt_mult.clear();
        
        // vector of matched reco jets for each method
        std::vector<unsigned int> matched_area_jets, matched_mult_jets, matched_iter_jets;
        
        // loop over truth jets
        for(unsigned int ijet =0; ijet < truth_pt->size(); ijet++)
        {
            // apply truth cut 
            if(truth_pt->at(ijet) < pTmin_truth) continue;

            float dr = 999;
            float matched_area_pt, matched_area_unsubpt;
            float matched_mult_pt, matched_mult_unsubpt;
            float matched_iter_pt, matched_iter_unsubpt;
            unsigned int matched_idx_area, matched_idx_mult, matched_idx_iter;

            // match area jets to truth jets
            for(unsigned int jjet = 0; jjet < rhoA_pt->size(); jjet++)
            {   
                // apply reco jet cut
                if(rhoA_pt->at(jjet) < pTmin_reco) continue;

                // dr calculation
                float dphi = rhoA_phi->at(jjet) - truth_phi->at(ijet);
                float deta = rhoA_eta->at(jjet) - truth_eta->at(ijet);
                if(dphi > TMath::Pi()) dphi -= 2*TMath::Pi();
                if(dphi < -TMath::Pi()) dphi += 2*TMath::Pi();
                float dr_tmp = TMath::Sqrt(dphi*dphi + deta*deta);
                if(dr_tmp < dr && dr_tmp < dRMax)
                {
                    dr = dr_tmp;
                    matched_area_pt = rhoA_pt->at(jjet);
                    matched_area_unsubpt = rhoA_pt_unsub->at(jjet);
                    matched_idx_area = jjet;
                }
            }
            if(dr > dRMax){
                missed_truth_pt_area.push_back(truth_pt->at(ijet));
            }
            else{
                matched_truth_pt_area.push_back(truth_pt->at(ijet));
                matched_pt_area.push_back(matched_area_pt);
                matched_pt_area_unsub.push_back(matched_area_unsubpt);
                matched_area_jets.push_back(matched_idx_area);
            }

            // reset dr
            dr = 999;
            // match mult jets to truth jets
            for(unsigned int jjet = 0; jjet < mult_pt->size(); jjet++)
            {
                // apply reco jet cut
                if(mult_pt->at(jjet) < pTmin_reco) continue;

                // dr calculation
                float dphi = mult_phi->at(jjet) - truth_phi->at(ijet);
                float deta = mult_eta->at(jjet) - truth_eta->at(ijet);
                if(dphi > TMath::Pi()) dphi -= 2*TMath::Pi();
                if(dphi < -TMath::Pi()) dphi += 2*TMath::Pi();
                float dr_tmp = TMath::Sqrt(dphi*dphi + deta*deta);
                if(dr_tmp < dr && dr_tmp < dRMax)
                {
                    dr = dr_tmp;
                    matched_mult_pt = mult_pt->at(jjet);
                    matched_mult_unsubpt = mult_pt_unsub->at(jjet);
                    matched_idx_mult = jjet;
                } 
            }
            if (dr > dRMax){
                missed_truth_pt_mult.push_back(truth_pt->at(ijet));
            }
            else{
                matched_truth_pt_mult.push_back(truth_pt->at(ijet));
                matched_pt_mult.push_back(matched_mult_pt);
                matched_pt_mult_unsub.push_back(matched_mult_unsubpt);
                matched_mult_jets.push_back(matched_idx_mult);
            }

            // // reset dr
            dr = 999;
            // match iter jets to truth jets
            for (unsigned int jjet = 0; jjet < iter_pt->size(); jjet++)
            {
                // apply reco jet cut
                if(iter_pt->at(jjet) < pTmin_reco) continue;

                // dr calculation
                float dphi = iter_phi->at(jjet) - truth_phi->at(ijet);
                float deta = iter_eta->at(jjet) - truth_eta->at(ijet);
                if(dphi > TMath::Pi()) dphi -= 2*TMath::Pi();
                if(dphi < -TMath::Pi()) dphi += 2*TMath::Pi();
                float dr_tmp = TMath::Sqrt(dphi*dphi + deta*deta);
                if(dr_tmp < dr && dr_tmp < dRMax)
                {
                    dr = dr_tmp;
                    matched_iter_pt = iter_pt->at(jjet);
                    matched_iter_unsubpt = iter_pt_unsub->at(jjet);
                    matched_idx_iter = jjet;
                }
            }
            if (dr > dRMax){
                missed_truth_pt_iter.push_back(truth_pt->at(ijet));
            }
            else{
                matched_truth_pt_iter.push_back(truth_pt->at(ijet));
                matched_pt_iter.push_back(matched_iter_pt);
                matched_pt_iter_unsub.push_back(matched_iter_unsubpt);
                matched_iter_jets.push_back(matched_idx_iter);
            }
            
        
        }
        // fill unmatched  area jets
        for(unsigned int jjet = 0; jjet < rhoA_pt->size(); jjet++)
        {
            if(std::find(matched_area_jets.begin(), matched_area_jets.end(), jjet) != matched_area_jets.end()) continue;
            if(rhoA_pt->at(jjet) < pTmin_reco) continue;
            unmatched_pt_area.push_back(rhoA_pt->at(jjet));
            unmatched_pt_area_unsub.push_back(rhoA_pt_unsub->at(jjet));
        }
        // fill unmatched reco mult jets
        for(unsigned int jjet = 0; jjet < mult_pt->size(); jjet++)
        {
            if(std::find(matched_mult_jets.begin(), matched_mult_jets.end(), jjet) != matched_mult_jets.end()) continue;
            if(mult_pt->at(jjet) < pTmin_reco) continue;
            unmatched_pt_mult.push_back(mult_pt->at(jjet));
            unmatched_pt_mult_unsub.push_back(mult_pt_unsub->at(jjet));
        }
        // fill unmatched reco iter jets
        for(unsigned int jjet = 0; jjet < iter_pt->size(); jjet++)
        {
            if(std::find(matched_iter_jets.begin(), matched_iter_jets.end(), jjet) != matched_iter_jets.end()) continue;
            if(iter_pt->at(jjet) < pTmin_reco) continue;
            unmatched_pt_iter.push_back(iter_pt->at(jjet));
            unmatched_pt_iter_unsub.push_back(iter_pt_unsub->at(jjet));
        }

        // fill tree
        out_tree->Fill();
    }
    
    // write output file
    fout->Write();
    fout->Close();
    // close input file
    fin.Close();
    return 0;
}
    