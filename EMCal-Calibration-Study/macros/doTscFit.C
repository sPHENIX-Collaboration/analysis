// -- c++ includes --
#include <string>

// -- root includes --
#include <TSystem.h>
#include <TFile.h>
#include <TH2F.h>

#include <cdbobjects/CDBTTree.h>  // for CDBTTree
#include <litecaloeval/LiteCaloEval.h>
// TowerInfo
#include <calobase/TowerInfoDefs.h>

#include <sPhenixStyle.C>

using std::string;
using std::cout;
using std::endl;

R__LOAD_LIBRARY(libLiteCaloEvalTowSlope.so)
R__LOAD_LIBRARY(libcdbobjects)


void TSCtoCDBTTree(const char * infile, const char * outputfile, const string &m_fieldname);
void mergeCDBTTrees(const char * infile1, const char * infile2, const char * outputfile, const string &m_fieldname);

void doTscFit(const string &hist_fname = "base/combine_out/out1.root", const string &calib_fname = "base/local_calib_copy.root",Int_t iter=1, const string &m_fieldname = "CEMC_calib_ADC_to_ETower")
{

  string fitoutfile = Form("tsc_fitout_it%d.root",iter);

  if (iter <= 2){
    LiteCaloEval modlce;
    modlce.CaloType(LiteCaloEval::CEMC);
    modlce.set_spectra_binWidth(0.02);
    modlce.Get_Histos(hist_fname.c_str(),fitoutfile.c_str());
    modlce.m_myminbin =  0;  
    modlce.m_mymaxbin =  96; 
    modlce.setFitMin(0.40f);
    modlce.setFitMax(1.2f);
    modlce.set_doQA();
    //if (iter==2) modlce.set_doQA(true);
    modlce.FitRelativeShifts(&modlce,110);
  }

  if (iter==3) {
    SetsPhenixStyle();
    LiteCaloEval modlce;
    modlce.CaloType(LiteCaloEval::CEMC);
    modlce.set_spectra_binWidth(0.005);
    modlce.Get_Histos(hist_fname.c_str(),fitoutfile.c_str());
    modlce.plot_cemc("figures");
  }
    

   // create the cdbttree from tsc output andd multiply the corrections 
   // into the base calibration to pickup for pi0 first iteration

  TSCtoCDBTTree(fitoutfile.c_str(),Form("tsc_output_cdb_it%d.root",iter), m_fieldname);
  mergeCDBTTrees(Form("tsc_output_cdb_it%d.root",iter),calib_fname.c_str(),calib_fname.c_str(), m_fieldname);

  size_t pos = calib_fname.find_last_of('.');
  string f_calib_save_name = calib_fname;
  f_calib_save_name.insert(pos,Form("_postTSC_it%d",iter));

  TFile* f_calib_mod = new TFile(calib_fname.c_str());
  f_calib_mod->Cp(f_calib_save_name.c_str());

  gSystem->Exit(0);
}


void mergeCDBTTrees(const char * infile1, const char * infile2, const char * outputfile, const string &m_fieldname)
{

  CDBTTree *cdbttree1 = new CDBTTree(infile1);  
  CDBTTree *cdbttree2 = new CDBTTree(infile2);  
  CDBTTree *cdbttreeOut = new CDBTTree(outputfile);

  for(UInt_t i = 0; i < 96 ; i++)
  {
    for(UInt_t j = 0; j < 256; j++)
    {
      UInt_t key = TowerInfoDefs::encode_emcal(i,j);
      float val1 = cdbttree1->GetFloatValue(static_cast<Int_t>(key), m_fieldname);
      float val2 = cdbttree2->GetFloatValue(static_cast<Int_t>(key), m_fieldname);
      cdbttreeOut->SetFloatValue(static_cast<Int_t>(key),m_fieldname,val1*val2);
    }
  }

  cdbttreeOut->Commit();
  cdbttreeOut->WriteCDBTTree();
  delete cdbttreeOut;
  delete cdbttree1;
  delete cdbttree2;

}//end macro



void TSCtoCDBTTree(const char * infile, const char * outputfile, const string &m_fieldname)
{

  bool chk4file = gSystem->AccessPathName(infile);
  TFile *f = nullptr;

  if(!chk4file)
  {
    f = new TFile(infile,"READ");
  }
  else
  {
    std::cout << "File " << infile << " cant be found in current directory." << std::endl;
    exit(0);
  }

  //write to cdb tree
  CDBTTree *cdbttree = new CDBTTree(outputfile);

  //gain values lie in the 2d histogram called corrPat
  TH2F *cp = static_cast<TH2F*>(f->Get("corrPat"));

  for(Int_t i = 0; i < 96 ; i++)
  {
    for(Int_t j = 0; j < 256; j++)
    {
      UInt_t key = TowerInfoDefs::encode_emcal(static_cast<UInt_t>(i),static_cast<UInt_t>(j));
      float gain = static_cast<Float_t>(1.0 / cp->GetBinContent(i+1,j+1));
       if (cp->GetBinContent(i+1,j+1) <= 0) gain = 0;
       if (std::isnan(cp->GetBinContent(i+1,j+1))) {gain = 0; cout << "nan calib from tsc " << i << "," << j << endl;}
      cdbttree->SetFloatValue(static_cast<Int_t>(key),m_fieldname.c_str(),gain);
    }
  }

  cdbttree->Commit();
  cdbttree->WriteCDBTTree();
  //cdbttree->Print();
  f->Close();
  delete f;
  delete cdbttree;

}

# ifndef __CINT__
Int_t main(Int_t argc, const char* const argv[]) {
if(argc < 3 || argc > 5){
        cout << "usage: ./doTscFit hist_fname calib_fname [iter] [m_fieldname]" << endl;
        return 1;
    }

    Int_t iter = 1;
    string m_fieldname = "CEMC_calib_ADC_to_ETower";

    if(argc >= 4) {
        iter = std::atoi(argv[3]);
    }
    if(argc >= 5) {
        m_fieldname = argv[4];
    }

    doTscFit(argv[1], argv[2], iter, m_fieldname);

    cout << "======================================" << endl;
    cout << "done" << endl;
    return 0;
}
# endif
