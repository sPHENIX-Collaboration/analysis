#include "SetOKStyle.C"

TTree *tree;
const int nbins = 17;
float bins[nbins];
//organized as fmodeta, fmodphi
TH1F *resolutions3x3[nbins-1][nbins-1]; 
TF1 *fits3x3[nbins-1][nbins-1];
float recal_consts3x3[nbins-1][nbins-1];
TH1F *resolutions5x5[nbins-1][nbins-1]; 
TF1 *fits5x5[nbins-1][nbins-1];
float recal_consts5x5[nbins-1][nbins-1];

void make_fmod_calibrations()
{
  for(int i=0; i<nbins; i++)
    bins[i] = 0. + i * 2. / (float)(nbins-1);


  gSystem->Load("libPrototype3.so");
  gSystem->Load("libProto3ShowCalib.so");
  SetOKStyle();
  instantiate();
  TFile *f =TFile::Open("allfmods.root");
  tree = (TTree*)f->Get("T");
 
 
  loop_tree();

  fit();

  write();

  write_recals_to_text();

}
void instantiate(){

  ostringstream name;
  for(int i=0; i<nbins-1; i++){
    for(int j=0; j<nbins-1; j++){
      name.str("");
      name<<"response_3x3_fmodeta_"<<i<<"_fmodphi_"<<j;
      resolutions3x3[i][j] = new TH1F(name.str().c_str(),"",
				      50,0.5,1.5);

      name.str("");
      name<<"response_5x5_fmodeta_"<<i<<"_fmodphi_"<<j;
      resolutions5x5[i][j] = new TH1F(name.str().c_str(),"",
				      50,0.5,1.5);
    }
  }
     
}
void write()
{

  TFile *outfile = new TFile("fmodoutfile.root","recreate");
  for(int i=0; i<nbins-1; i++){
    for(int j=0; j<nbins-1; j++){
      resolutions3x3[i][j]->Write();
      resolutions5x5[i][j]->Write();
    }
  }
}
void write_recals_to_text()
{

  ofstream file3x3;
  file3x3.open("3x3clus_posdep_recals_fromsim.txt");

  //write them to the text file in eta bins as a function of phi
  for(int i=0; i<nbins-1; i++){
    for(int j=0; j<nbins-1; j++){
      file3x3 << recal_consts3x3[i][j]<<" ";

    }
    file3x3 << "\n";
  }
  file3x3.close();


  ofstream file5x5;
  file5x5.open("5x5clus_posdep_recals_fromsim.txt");

  for(int i=0; i<nbins-1; i++){
    for(int j=0; j<nbins-1; j++){
      file5x5 << recal_consts5x5[i][j] <<" ";
    }
    file5x5 << "\n";
  }
  file5x5.close();



}
void fit()
{
  
  for(int i=0; i<nbins-1; i++){
    for(int j=0; j<nbins-1; j++){
      ostringstream name;
      name.str("");
      name<<"fit3x3_eta_"<<i<<"_phi_"<<j;
      fits3x3[i][j] = new TF1(name.str().c_str(),"gaus",
			      resolutions3x3[i][j]->GetMean()
			      - 2. * resolutions3x3[i][j]->GetRMS(),
			      resolutions3x3[i][j]->GetMean()
			      + 2. * resolutions5x5[i][j]->GetRMS());

      name.str("");
      name<<"fit5x5_eta_"<<i<<"_phi_"<<j;
      fits5x5[i][j] = new TF1(name.str().c_str(),"gaus",
			      resolutions5x5[i][j]->GetMean()
			      - 2. * resolutions5x5[i][j]->GetRMS(),
			      resolutions5x5[i][j]->GetMean()
			      + 2. * resolutions5x5[i][j]->GetRMS());

      
      resolutions3x3[i][j]->Fit(fits3x3[i][j],"nqr");
      recal_consts3x3[i][j] = fits3x3[i][j]->GetParameter(1);

      resolutions5x5[i][j]->Fit(fits5x5[i][j],"nqr");
      recal_consts5x5[i][j] = fits5x5[i][j]->GetParameter(1);

      if(recal_consts3x3[i][j]<0)
	cout<<"3x3: "<<i<<"_"<<j<<endl;
      if(recal_consts5x5[i][j]<0)
	cout<<"5x5: "<<i<<"_"<<j<<endl;


    }
  }





}

void loop_tree()
{


  Proto3ShowerCalib::Eval_Cluster *prod3x3, *prod5x5; 
  Proto3ShowerCalib::Eval_Run *run;
  tree->SetBranchAddress("clus_3x3_prod",&prod3x3);
  tree->SetBranchAddress("clus_5x5_prod",&prod5x5);
  tree->SetBranchAddress("info",&run);
  

  for(int i=0; i<tree->GetEntries(); i++){
    tree->GetEntry(i);
    
    float beam_mom = run->getbeammom();
  
    float fmodphi3x3 = prod3x3->getfmodphi();
    float fmodeta3x3 = prod3x3->getfmodeta();
    float sume3x3 = prod3x3->getsumE();

    float fmodphi5x5 = prod5x5->getfmodphi();
    float fmodeta5x5 = prod5x5->getfmodeta();
    float sume5x5 = prod5x5->getsumE();

    int fmodphi3x3bin = -99;
    int fmodeta3x3bin = -99;
    int fmodphi5x5bin = -99;
    int fmodeta5x5bin = -99;

    for(int j=0; j<nbins-1; j++)
      if(fmodphi3x3>bins[j] && fmodphi3x3<=bins[j+1])
	fmodphi3x3bin = j;
    for(int j=0; j<nbins-1; j++)
      if(fmodeta3x3>bins[j] && fmodeta3x3<=bins[j+1])
	fmodeta3x3bin = j;
    for(int j=0; j<nbins-1; j++)
      if(fmodphi5x5>bins[j] && fmodphi5x5<=bins[j+1])
	fmodphi5x5bin = j;
    for(int j=0; j<nbins-1; j++)
      if(fmodeta5x5>bins[j] && fmodeta5x5<=bins[j+1])
	fmodeta5x5bin = j;

    if(fmodphi3x3bin>=0 && fmodeta3x3bin>=0){

      //fill 3x3 histos
      resolutions3x3[fmodeta3x3bin][fmodphi3x3bin]->Fill(sume3x3
							 /beam_mom);
    }
    if(fmodphi5x5bin>=0 && fmodeta5x5bin>=0){

      //fill 5x5 histos
      resolutions5x5[fmodeta5x5bin][fmodphi5x5bin]->Fill(sume5x5
							 /beam_mom);
      
    }

  }

}


vector<double> GetBeamMom()
{

  vector<double> mom;

  TH1F * hbeam_mom = new TH1F("hbeam_mom", ";beam momentum (GeV)",
			      32,0.5,32.5);

  TText * t;
  TCanvas *c1 = new TCanvas("GetBeamMom", "GetBeamMom", 1800, 900);

  tree->Draw("abs(info.beam_mom)>>hbeam_mom");

  for (int bin = 1; bin < hbeam_mom->GetNbinsX(); bin++)
    {
      if (hbeam_mom->GetBinContent(bin) > 40)
        {
          const double momentum = hbeam_mom->GetBinCenter(bin);

          if (momentum == 1 || momentum == 2 || momentum == 3 || momentum == 4
              || momentum == 6 || momentum == 8 || momentum == 12
              || momentum == 16 || momentum == 24 || momentum == 32)
            {
              mom.push_back(momentum);

              cout << "GetBeamMom - " << momentum << " GeV for "
                  << hbeam_mom->GetBinContent(bin) << " event" << endl;
            }
        }
    }
  return mom;
}
