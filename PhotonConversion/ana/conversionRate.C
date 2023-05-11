
TChain* handleFile(string name, string extension, string treename, unsigned int filecount){
  TChain *all = new TChain(treename.c_str());
  string temp;
  for (int i = 0; i < filecount; ++i)
  {

    ostringstream s;
    s<<i;
    temp = name+string(s.str())+extension;
    all->Add(temp.c_str());
  }
  return all;
}

/**makes a TEff holding the hists for a uniform conversion rate
 * @param out_file file data is saved to this is used for read and write needs converted and truth pT spectra to be inside these are created by photonEff
 * if the pT spectra are not inside the file @param ttree and @param allTree will be used to calculate these spectra
 * @param ttree tree for converted data 
 * @param allTree tree for nonconverted data 
 * @return NULL if the spectra are not in the existing data and the passes trees are not valid*/
TEfficiency* makepTRes(TFile* out_file,TChain* ttree=NULL,TTree* allTree=NULL){
  //check if the spectra are in the file
  out_file->ReOpen("READ");
  if(out_file->Get("converted_photon_truth_pT")&&out_file->Get("all_photon_truth_pT")) 
    return new TEfficiency(*(TH1F*)out_file->Get("converted_photon_truth_pT"),*(TH1F*)out_file->Get("all_photon_truth_pT"));
  //if they are not in the file check if the trees are NULL
  else if(!ttree||!allTree){
    return NULL;
  }
  out_file->ReOpen("UPDATE");
  float pT;
  float tpT;
  float track_pT;
  ttree->SetBranchAddress("photon_pT",&pT);
  ttree->SetBranchAddress("tphoton_pT",&tpT);

  vector<float> *allpT=NULL;
  allTree->SetBranchAddress("photon_pT",&allpT);

  TH1F *pTeffPlot = new TH1F("#frac{#it{p}^{T}}{#it{p}_{#it{truth}}^{T}}","",40,-2,2);
  TH2F *pTefffuncPlot = new TH2F("pT_resolution_to_truthpt","",40,1,35,40,-1.5,1.5);
  TH1F *converted_pTspec = new TH1F("converted_photon_truth_pT","",20,5,25);
  TH1F *all_pTspec = new TH1F("all_photon_truth_pT","",20,5,25);
  //TH1F *trackpTDist = new TH1F("truthpt","",40,0,35);
  pTeffPlot->Sumw2();
  converted_pTspec->Sumw2();
  all_pTspec->Sumw2();
  pTefffuncPlot->Sumw2();
  //TODO need to turn off other branches 
  //make the pT spectra fo converted
  for (int event = 0; event < ttree->GetEntries(); ++event)
  {
    ttree->GetEvent(event);
    if(pT>0)pTeffPlot->Fill(pT/tpT);
    converted_pTspec->Fill(tpT);
    if(pT>0)pTefffuncPlot->Fill(tpT,pT/tpT);
    //trackpTDist->Fill(track_pT); 
  }
  //make the pT spectra for unconverted
  for (int event = 0; event < allTree->GetEntries(); ++event)
  {
    allTree->GetEvent(event);
    for(auto i : *allpT){
      all_pTspec->Fill(i);
    }
  }
  //format and save the data
  TEfficiency* uni_rate = new TEfficiency(*converted_pTspec,*all_pTspec);
  uni_rate->SetName("uni_rate");
  uni_rate->Write();
  pTeffPlot->Scale(1./ttree->GetEntries(),"width");
  pTefffuncPlot->Scale(1./ttree->GetEntries());
  TProfile* resProfile = pTefffuncPlot->ProfileX("func_prof",5,30);
  resProfile->Write();
  //trackpTDist->Scale(1./ttree->GetEntries(),"width");
  out_file->Write();
  ttree->ResetBranchAddresses();
  return uni_rate;
}

unsigned totalMB(string path, string extension, string hardname, string softname){
  string name = path+hardname+extension;
  TFile *hardFile=new TFile(name.c_str(),"READ");
  name=path+softname+extension;
  TFile *softFile=new TFile(name.c_str(),"READ");
  TTree* noPhoton = (TTree*) softFile->Get("nophotonTree");
  unsigned r=noPhoton->GetEntries()*2000000;
  noPhoton = (TTree*) hardFile->Get("nophotonTree");
  r+=noPhoton->GetEntries()*2000000;
  return r;
}

/**@param rate the uniform conversion rate
 * @param file file to get the pythia pT spectra from
 * @return the pT dependent conversion rate*/
TH1F* calculateRate(TEfficiency* rate,TFile* file,unsigned nMB){
  //get the combined pythiaspec from the file then clone it to rate
  TH1F* conversion_rate = (TH1F*)((TH1F*) file->Get("combinedpythia"))->Clone("rate");
  //make a histogram for the uniform rate
  TH1* uni_rate = (TH1F*)rate->GetPassedHistogram()->Clone("uni_rate");
  uni_rate->Divide(rate->GetTotalHistogram());
  conversion_rate->Multiply(uni_rate);
  conversion_rate->Scale(1./nMB);
  file->ReOpen("UPDATE");
  conversion_rate->Write();
  return conversion_rate;
}

void derivitvePlot(TH1F* finalrate){
  TH1F *dplot = new TH1F("derivative","",finalrate->GetNbinsX(),5,25);
  for (int i = 1; i < finalrate->GetNbinsX(); ++i)
  {
    double error;
    dplot->SetBinContent(i,finalrate->IntegralAndError(i,finalrate->GetNbinsX(),error));
    dplot->SetBinError(i,error);
  }
  dplot->Write();
}

void conversionRate(){
  TFile *out_file = new TFile("effplots.root","UPDATE");
  TEfficiency* uni_rate=makepTRes(out_file);
  string treeExtension = ".root";
  if(!uni_rate){
    string treePath = "/sphenix/user/vassalli/gammasample/truthconversiononlineanalysis";
    unsigned int nFiles=200;
    TChain *ttree = handleFile(treePath,treeExtension,"cutTreeSignal",nFiles);
    TChain *observations = handleFile(treePath,treeExtension,"observTree",nFiles);
    cout<<"Got tree: "<<ttree->GetName()<<" and "<<ttree->GetEntries()<<" entries"<<endl;
    cout<<"Got tree: "<<observations->GetName()<<" and "<<observations->GetEntries()<<" entries"<<endl;
    uni_rate=makepTRes(out_file,ttree,observations);
  }
  out_file->ReOpen("READ");
  string treePath="/sphenix/user/vassalli/minBiasPythia/";
  string softname = "softana";
  string hardname = "hard4ana";
  auto conversion_rate=calculateRate(uni_rate,out_file,totalMB(treePath,treeExtension,softname,hardname));
  derivitvePlot(conversion_rate);
}
