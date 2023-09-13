#include "sPhenixStyle.h"
#include "sPhenixStyle.C"
#include "Math/Vector4D.h"

std::vector<TH1F*> get_spectras(TChain *data, TFile* f, string generator)
{
  vector<TH1F*> hists;
  vector<float> *eta = 0;
  vector<float> *phi = 0;
  vector<float> *pt = 0;
  vector<float> *e = 0;
  vector<float> *subtracted_et = 0;
  vector<float> *truthEta = 0;
  vector<float> *truthPhi = 0;
  vector<float> *truthPt = 0;
  vector<float> *truthE = 0;
  vector<float> *subseedEta = 0;
  vector<float> *subseedPhi = 0;
  vector<float> *subseedPt = 0;
  vector<float> *subseedE = 0;
  vector<int> *subseedCut = 0;
  vector<float> *rawseedEta = 0;
  vector<float> *rawseedPhi = 0;
  vector<float> *rawseedPt = 0;
  vector<float> *rawseedE = 0;
  vector<int> *rawseedCut = 0;
  float *totalCalo = 0;
  int cent;
  vector<TLorentzVector*> four_momentum;
  vector<float> *mass=0;

  data->SetBranchAddress("eta", &eta);
  data->SetBranchAddress("phi", &phi);
  data->SetBranchAddress("pt", &pt);
  data->SetBranchAddress("e", &e);
  data->SetBranchAddress("subtracted_et", &subtracted_et); 
  data->SetBranchAddress("truthEta", &truthEta);
  data->SetBranchAddress("truthPhi", &truthPhi);
  data->SetBranchAddress("truthPt", &truthPt);
  data->SetBranchAddress("truthE", &truthE);
  data->SetBranchAddress("subseedEta", &subseedEta);
  data->SetBranchAddress("subseedPhi", &subseedPhi);
  data->SetBranchAddress("subseedPt", &subseedPt);
  data->SetBranchAddress("subseedE", &subseedE);
  data->SetBranchAddress("subseedCut", &subseedCut);
  data->SetBranchAddress("rawseedEta", &rawseedEta);
  data->SetBranchAddress("rawseedPhi", &rawseedPhi);
  data->SetBranchAddress("rawseedE", &rawseedE);
  data->SetBranchAddress("rawseedPt", &rawseedPt);
  data->SetBranchAddress("rawseedCut", &rawseedCut);
  data->SetBranchAddress("totalCalo", &totalCalo);
  data->SetBranchAddress("cent", &cent);	
  
  const int trigger=30; 
  const int nbins=(60-trigger)/3;
  TH1F* jet_pt=new TH1F(Form("jet_pt_%s", generator.c_str()),Form("Jet Truth p_{T} from %s generator with 30 GeV trigger; Jet p_{T} (GeV)", generator.c_str()), nbins, trigger, 60); 
  TH1F* jet_e=new TH1F(Form("jet_e_%s", generator.c_str()),Form("Jet Truth E from %s generator with 30 GeV p_{T} trigger; Jet E (GeV)", generator.c_str()), 50, 25, 75); 
  TH1F* jet_m=new TH1F(Form("jet_m_%s", generator.c_str()),Form("Jet Truth mass from %s generator with 30 GeV p_{T} trigger;Jet Mass (GeV)", generator.c_str()), 50, 0, 15); 
  TH1F* jet_eta=new TH1F(Form("jet_eta_%s", generator.c_str()),Form("Jet Truth #eta from %s generator with 30 GeV p_{T} trigger; #eta", generator.c_str()), 50, -1, 1); 
  TH1F* jet_phi=new TH1F(Form("jet_phi_%s", generator.c_str()),Form("Jet Truth #phi from %s generator with 30 GeV p_{T} trigger; #phi", generator.c_str()), 50, -TMath::Pi(), TMath::Pi()); 
  TH1F* dijet_pt=new TH1F(Form("dijet_pt_%s", generator.c_str()),Form("Dijet Truth p_{T} from %s generator with 30 GeV trigger; p_{T, jj}", generator.c_str()), nbins, trigger, 80); 
  TH1F* dijet_e=new TH1F(Form("dijet_e_%s", generator.c_str()),Form("Dijet Truth E from %s generator with 30 GeV p_{T} trigger; E_{jj}", generator.c_str()), 100, 0, 100); 
  TH1F* dijet_m=new TH1F(Form("dijet_m_%s", generator.c_str()),Form("Dijet Truth mass from %s generator with 30 GeV p_{T} trigger; m_{jj}", generator.c_str()), 50, 50, 120); 
  TH1F* dijet_eta=new TH1F(Form("Dijet_eta_%s", generator.c_str()),Form("Dijet Truth #Delta #eta from %s generator with 30 GeV p_{T} trigger; #Delta #eta_{jj}", generator.c_str()), 50, -1, 1); 
  TH1F* dijet_phi=new TH1F(Form("dijet_phi_%s", generator.c_str()),Form("Dijet Truth #Delta #phi from %s generator with 30 GeV p_{T} trigger; #Delta #phi_{jj}", generator.c_str()), 50, -TMath::Pi(), TMath::Pi()); 
  TH1F* dijet_imb=new TH1F(Form("dijet_imb_%s", generator.c_str()),Form("Dijet Imbalence Truth p_{T} from %s generator with 30 GeV trigger; A_{jj}", generator.c_str()), 50, 0, 0.2); 
 int n_events=data->GetEntries();
for(int j=0; j<n_events; j++) 
{
  data->GetEntry(j);
  const int njets=truthPt->size();  
  for(int i=0; i<njets; i++)
  {
     //fill jets in with a trigger cut
     //
     if (truthPt->at(i)<trigger) continue;
     else jet_pt->Fill(truthPt->at(i));
  }
  for(int i=0; i<njets; i++)
  { 
 	if(truthPt->at(i)<trigger) continue;
 	jet_e->Fill(truthE->at(i)); 
 	jet_eta->Fill(truthEta->at(i)); 
	jet_phi->Fill(truthPhi->at(i)); 
	TLorentzVector* p4m=new TLorentzVector(); 
	p4m->SetPtEtaPhiE(truthPt->at(i), truthEta->at(i), truthPhi->at(i), truthE->at(i));
	jet_m->Fill(p4m->M());
        four_momentum.push_back(p4m); 
  }
  if(njets < 2) continue;
  for(int i=0; i<njets-1; i++)
  {
    if(truthPt->at(i) < trigger) continue;
    float jpt=truthPt->at(i);
    float jph=truthPhi->at(i), jet=truthEta->at(i);
    for(int k=i; k<njets; k++)
    {
	  if(truthPt->at(k) <trigger) continue;
    	  float jph2=truthPhi->at(k), jet2=truthEta->at(k), jpt2=truthPt->at(k);
          float Dph=abs(jph2-jph), Det=(jet+jet2);
	  if(Det > 0.1 || Dph < 3 || Dph > 3.5 ) continue; 
	  float A=abs(jpt-jpt2)/(jpt+jpt2);
	  dijet_imb->Fill(A); 
          dijet_phi->Fill(Dph);
	  dijet_eta->Fill(Det);
	  TLorentzVector* p4m=new TLorentzVector(); 
	  p4m->SetPtEtaPhiE(truthPt->at(i), truthEta->at(i), truthPhi->at(i), truthE->at(i));
	  TLorentzVector* p4m2=new TLorentzVector(); 
	  p4m2->SetPtEtaPhiE(truthPt->at(k), truthEta->at(k), truthPhi->at(k), truthE->at(k));
	  *p4m+=*p4m2;
	  float m=p4m->M();
	  dijet_m->Fill(m);
    }	  
}
}
  hists.push_back(jet_e);
  jet_e->Write();
  hists.push_back(jet_m);
  jet_m->Write();
  hists.push_back(jet_phi);
  jet_phi->Write(); 
  hists.push_back(jet_eta);
  jet_eta->Write();
  hists.push_back(jet_pt);
  jet_pt->Write();
  //four_momentum->Write();
  hists.push_back(dijet_imb);
  dijet_imb->Write();
  hists.push_back(dijet_phi);
  dijet_phi->Write();
  hists.push_back(dijet_eta);
  dijet_eta->Write();
  hists.push_back(dijet_m);
  dijet_m->Write();
  //f->Write();
/*  jet_e->Delete();
  jet_m->Delete();
  jet_phi->Delete(); 
  jet_eta->Delete();
  jet_pt->Delete();
  //four_momentum->Delete();
  dijet_imb->Delete();
  dijet_phi->Delete();
  dijet_eta->Delete();
  dijet_m->Delete();*/
  return hists;
}
void DrawPlots(TH1F* hw, TH1F* py)
{
	TCanvas* c1 = new TCanvas(); 
 	TPad* p1 = new TPad("value", "Value", 0, 0.3, 1, 1);
	TPad* p2 = new TPad("dif", "Div", 0, 0, 1, 0.28);
	c1->cd();
	p1->cd();
	std::cout<<hw->GetTitle()<<std::endl;
	float hw_min, hw_max;
	hw_min=hw->GetXaxis()->GetXmin();
	hw_max=hw->GetXaxis()->GetXmax();
	py->GetXaxis()->SetRangeUser(hw_min, hw_max);
	hw->Scale(1/hw->Integral()); 
	py->Scale(1/py->Integral());
	hw->SetLineColor(1); 
	hw->SetMarkerColor(1); 
	py->SetLineColor(2);
	py->SetMarkerColor(2);
	string title=hw->GetXaxis()->GetTitle();
	hw->GetYaxis()->SetTitle("Arbitrary Units, Scaled to 1");      
	if(title.find("p_{T") !=std::string::npos || title.find("E") !=std::string::npos) p1->SetLogy(); 
	hw->Draw("e1"); 
	py->Draw("same, e1");
	TLegend* l = new TLegend(.5,.75, .95, .95);
	l->AddEntry("a", Form("Truth jet %s with a p_{T} > 30 GeV cut", title.c_str()), 0);
	l->AddEntry(hw, "Herwig");
	l->AddEntry(py, "Pythia");
	l->Draw();
	c1->cd();
	p1->Draw();
	p2->cd(); 
	TH1F* h2=(TH1F*)hw->Clone();
	h2->Divide(py); 
	h2->GetYaxis()->SetRangeUser(0,4); 
	h2->GetYaxis()->SetLabelSize(0.15f);
	h2->GetYaxis()->SetTitle("");
	h2->GetXaxis()->SetTitle("");
	h2->Draw("e1");
	TLine* l1=new TLine(hw_min,1, hw_max,1);
	l1->SetLineColor(3);
	l1->SetLineStyle(9);
	l1->Draw();
	c1->cd();
	p2->Draw();
	string ftitle=hw->GetName();
	ftitle+=".pdf";
	c1->SaveAs(ftitle.c_str());
	h2->Delete(); 
	p1->Delete();
	p2->Delete(); 
}
void Herwig_spectra() 
{
  SetsPhenixStyle();
  TH1::SetDefaultSumw2();
  TH2::SetDefaultSumw2();

  TChain * herwig = new TChain("T");
  //if you want to run one file use this
  herwig->Add("../macro/output_herwig.root");
  TChain * pythia = new TChain("T");
  pythia->Add("../macro/output_pythia.root");
  TFile* f = new TFile("spectras.root","RECREATE");
  map<string, vector<TH1F*>> hists; 
  //hists.reserve(2); 
  //std::cout<<hists.size() <<std::endl;
  //for (int i=0; i<9; i++){
 //	vector <TH1F*>  hist_pair;
//	std::cout<<"hist pair created"<<std::endl;
	//hists.push_back(hist_pair);
  //}
  std::cout<<"have set the maps "<< hists.size() <<std::endl;
  hists["herwig"]=get_spectras(herwig, f, "herwig");
  hists["pythia"]=get_spectras(pythia, f, "pythia");
   std::cout<<"made spectra plots"<<std::endl;
  for (int i=0; i<hists["herwig"].size(); i++){
	TH1F* h=hists["herwig"].at(i), *p= hists["pythia"].at(i);
	std::cout<<h->GetTitle() <<std::endl;
	DrawPlots(h,p);
  }
  
}
