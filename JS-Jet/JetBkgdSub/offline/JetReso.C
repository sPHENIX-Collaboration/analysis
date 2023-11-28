#include <iostream>
#include <vector>

using namespace std;


Int_t JetReso(TString infile="matched.root", TString outfile="resohists.root")
{

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

    // get branches
    int centrality = 0;
    double rho_area = 0;
    double rho_mult = 0;
    float weight = 0;
    std::vector<float> *matched_pt_iter = 0;
    std::vector<float> *matched_pt_area = 0;
    std::vector<float> *matched_pt_mult = 0;
    std::vector<float> *matched_pt_iter_unsub = 0;
    std::vector<float> *matched_pt_area_unsub = 0;
    std::vector<float> *matched_pt_mult_unsub = 0;
    std::vector<float> *matched_truth_pt_iter = 0;
    std::vector<float> *matched_truth_pt_area = 0;
    std::vector<float> *matched_truth_pt_mult = 0;
    std::vector<float> *unmatched_pt_iter = 0;
    std::vector<float> *unmatched_pt_area = 0;
    std::vector<float> *unmatched_pt_mult = 0;
    std::vector<float> *unmatched_pt_iter_unsub = 0;
    std::vector<float> *unmatched_pt_area_unsub = 0;
    std::vector<float> *unmatched_pt_mult_unsub = 0;
    std::vector<float> *missed_truth_pt_iter = 0;
    std::vector<float> *missed_truth_pt_area = 0;
    std::vector<float> *missed_truth_pt_mult = 0;

    tree->SetBranchAddress("weight", &weight);
    tree->SetBranchAddress("centrality", &centrality);
    tree->SetBranchAddress("rho_area", &rho_area);
    tree->SetBranchAddress("rho_mult", &rho_mult);
    tree->SetBranchAddress("matched_pt_iter", &matched_pt_iter);
    tree->SetBranchAddress("matched_pt_area", &matched_pt_area);
    tree->SetBranchAddress("matched_pt_mult", &matched_pt_mult);
    tree->SetBranchAddress("matched_pt_iter_unsub", &matched_pt_iter_unsub);
    tree->SetBranchAddress("matched_pt_area_unsub", &matched_pt_area_unsub);
    tree->SetBranchAddress("matched_pt_mult_unsub", &matched_pt_mult_unsub);
    tree->SetBranchAddress("matched_truth_pt_iter", &matched_truth_pt_iter);
    tree->SetBranchAddress("matched_truth_pt_area", &matched_truth_pt_area);
    tree->SetBranchAddress("matched_truth_pt_mult", &matched_truth_pt_mult);
    tree->SetBranchAddress("unmatched_pt_iter", &unmatched_pt_iter);
    tree->SetBranchAddress("unmatched_pt_area", &unmatched_pt_area);
    tree->SetBranchAddress("unmatched_pt_mult", &unmatched_pt_mult);
    tree->SetBranchAddress("unmatched_pt_iter_unsub", &unmatched_pt_iter_unsub);
    tree->SetBranchAddress("unmatched_pt_area_unsub", &unmatched_pt_area_unsub);
    tree->SetBranchAddress("unmatched_pt_mult_unsub", &unmatched_pt_mult_unsub);
    tree->SetBranchAddress("missed_truth_pt_iter", &missed_truth_pt_iter);
    tree->SetBranchAddress("missed_truth_pt_area", &missed_truth_pt_area);
    tree->SetBranchAddress("missed_truth_pt_mult", &missed_truth_pt_mult);

    //=======================================================
    // declare histograms
    //=======================================================
    const double pt_range[] = {10,15,20,25,30,35,40,45,50,60,80};
    const int pt_N = sizeof(pt_range)/sizeof(double) - 1;

    const int resp_N = 100;
    double resp_bins[resp_N+1];
    for(int i = 0; i < resp_N+1; i++){
        resp_bins[i] = 2.0/resp_N * i;
    }

    const double cent_bins[] = {-1, 0, 10, 30, 50, 80}; //the first bin is for inclusive in centrality/pp events
    const int cent_N = sizeof(cent_bins)/sizeof(double) - 1;

    TH3D * h3_reso_area = new TH3D("h3_reso_area", "h3_reso_area", pt_N, pt_range, resp_N, resp_bins, cent_N, cent_bins);
    TH3D * h3_reso_mult = new TH3D("h3_reso_mult", "h3_reso_mult", pt_N, pt_range, resp_N, resp_bins, cent_N, cent_bins);
    TH3D * h3_reso_iter = new TH3D("h3_reso_iter", "h3_reso_iter", pt_N, pt_range, resp_N, resp_bins, cent_N, cent_bins);

    // fill histograms
    int nentries = tree->GetEntries();
    for (int iEvent = 0; iEvent < nentries; iEvent++){
        // get entry
        tree->GetEntry(iEvent);

        for (unsigned int ijet =0; ijet < matched_pt_iter->size(); ijet++){
            h3_reso_iter->Fill(matched_truth_pt_iter->at(ijet), matched_pt_iter->at(ijet)/matched_truth_pt_iter->at(ijet), centrality, weight);
            h3_reso_iter->Fill(matched_truth_pt_iter->at(ijet), matched_pt_iter->at(ijet)/matched_truth_pt_iter->at(ijet), -1, weight);
        }
        for (unsigned int ijet =0; ijet < matched_pt_area->size(); ijet++){
            h3_reso_area->Fill(matched_truth_pt_area->at(ijet), matched_pt_area->at(ijet)/matched_truth_pt_area->at(ijet), centrality, weight);
            h3_reso_area->Fill(matched_truth_pt_area->at(ijet), matched_pt_area->at(ijet)/matched_truth_pt_area->at(ijet), -1, weight);
        }
        for (unsigned int ijet =0; ijet < matched_pt_mult->size(); ijet++){
            h3_reso_mult->Fill(matched_truth_pt_mult->at(ijet), matched_pt_mult->at(ijet)/matched_truth_pt_mult->at(ijet), centrality, weight);
            h3_reso_mult->Fill(matched_truth_pt_mult->at(ijet), matched_pt_mult->at(ijet)/matched_truth_pt_mult->at(ijet), -1, weight);
        }
    }

    //=======================================================
    // JES and JER
    //=======================================================
    TH1D * h_jes_area[cent_N];
    TH1D * h_jer_area[cent_N];
    TH1D * h_jes_mult[cent_N];
    TH1D * h_jer_mult[cent_N];
    TH1D * h_jes_iter[cent_N];
    TH1D * h_jer_iter[cent_N];

    for (int icent = 1; icent <= cent_N; icent++){
        h_jes_area[icent-1] = new TH1D(Form("h_jes_area_%d", icent), Form("h_jes_area_%d", icent), pt_N, pt_range);
        h_jer_area[icent-1] = new TH1D(Form("h_jer_area_%d", icent), Form("h_jer_area_%d", icent), pt_N, pt_range);
        h_jes_mult[icent-1] = new TH1D(Form("h_jes_mult_%d", icent), Form("h_jes_mult_%d", icent), pt_N, pt_range);
        h_jer_mult[icent-1] = new TH1D(Form("h_jer_mult_%d", icent), Form("h_jer_mult_%d", icent), pt_N, pt_range);
        h_jes_iter[icent-1] = new TH1D(Form("h_jes_iter_%d", icent), Form("h_jes_iter_%d", icent), pt_N, pt_range);
        h_jer_iter[icent-1] = new TH1D(Form("h_jer_iter_%d", icent), Form("h_jer_iter_%d", icent), pt_N, pt_range);
    }

    for(int icent = 0; icent < cent_N; ++icent)
    {
        // area 
        for (int i = 0; i < pt_N; ++i)
        {
            // fit function
            TF1 *func = new TF1("func","gaus",0, 1.2);
            h3_reso_area->GetXaxis()->SetRange(i+1,i+1);
            h3_reso_area->GetZaxis()->SetRange(icent+1,icent+1);
            TH1D * h_temp = (TH1D*)h3_reso_area->Project3D("y");
            h_temp->SetName(Form("h_jes_area_%d_%d", icent, i));
            h_temp->Fit(func,"","",0,1.2);
            h_temp->Fit(func,"","",func->GetParameter(1)-1.5*func->GetParameter(2),func->GetParameter(1)+1.5*func->GetParameter(2));          
            float dsigma = func -> GetParError(2);
            float denergy = func -> GetParError(1);
            float sigma = func -> GetParameter(2);
            float energy = func -> GetParameter(1);
            float djer = dsigma/energy + (sigma*denergy)/(energy*energy);
            h_jes_area[icent]->SetBinContent(i+1, energy);
            h_jes_area[icent]->SetBinError(i+1, denergy);
            h_jer_area[icent]->SetBinContent(i+1, sigma/energy);
            h_jer_area[icent]->SetBinError(i+1, djer);
        }

        // mult
        for (int i = 0; i < pt_N; ++i)
        {
            // fit function
            TF1 *func = new TF1("func","gaus",0, 1.2);
            h3_reso_mult->GetXaxis()->SetRange(i+1,i+1);
            h3_reso_mult->GetZaxis()->SetRange(icent+1,icent+1);
            TH1D * h_temp = (TH1D*)h3_reso_mult->Project3D("y");
            h_temp->SetName(Form("h_jes_mult_%d_%d", icent, i));
            h_temp->Fit(func,"","",0,1.2);
            h_temp->Fit(func,"","",func->GetParameter(1)-1.5*func->GetParameter(2),func->GetParameter(1)+1.5*func->GetParameter(2));          
            float dsigma = func -> GetParError(2);
            float denergy = func -> GetParError(1);
            float sigma = func -> GetParameter(2);
            float energy = func -> GetParameter(1);
            float djer = dsigma/energy + (sigma*denergy)/(energy*energy);
            h_jes_mult[icent]->SetBinContent(i+1, energy);
            h_jes_mult[icent]->SetBinError(i+1, denergy);
            h_jer_mult[icent]->SetBinContent(i+1, sigma/energy);
            h_jer_mult[icent]->SetBinError(i+1, djer);
        }

        // iter
        for (int i = 0; i < pt_N; ++i)
        {
            // fit function
            TF1 *func = new TF1("func","gaus",0, 1.2);
            h3_reso_iter->GetXaxis()->SetRange(i+1,i+1);
            h3_reso_iter->GetZaxis()->SetRange(icent+1,icent+1);
            TH1D * h_temp = (TH1D*)h3_reso_iter->Project3D("y");
            h_temp->SetName(Form("h_jes_iter_%d_%d", icent, i));
            h_temp->Fit(func,"","",0,1.2);
            h_temp->Fit(func,"","",func->GetParameter(1)-1.5*func->GetParameter(2),func->GetParameter(1)+1.5*func->GetParameter(2));          
            float dsigma = func -> GetParError(2);
            float denergy = func -> GetParError(1);
            float sigma = func -> GetParameter(2);
            float energy = func -> GetParameter(1);
            float djer = dsigma/energy + (sigma*denergy)/(energy*energy);
            h_jes_iter[icent]->SetBinContent(i+1, energy);
            h_jes_iter[icent]->SetBinError(i+1, denergy);
            h_jer_iter[icent]->SetBinContent(i+1, sigma/energy);
            h_jer_iter[icent]->SetBinError(i+1, djer);
        }


    }

    //=======================================================
    //================ Output File ==========================
    //=======================================================
    std::cout << "Output file: " << outfile.Data() << std::endl;
    TFile * fout = new TFile(outfile.Data(), "RECREATE");
    h3_reso_area->Write();
    h3_reso_mult->Write();
    h3_reso_iter->Write();
    for (int icent = 1; icent <= cent_N; icent++){
        h_jes_area[icent-1]->Write();
        h_jer_area[icent-1]->Write();
        h_jes_mult[icent-1]->Write();
        h_jer_mult[icent-1]->Write();
        h_jes_iter[icent-1]->Write();
        h_jer_iter[icent-1]->Write();
    }
    fout->Close();

    // close input file
    fin.Close();
    return 0;



}