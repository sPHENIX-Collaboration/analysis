#include "V0DuplicateReader.h"

void skim_KFParticle_trees()
{
  TFile* Ks_file = TFile::Open("/sphenix/tg/tg01/hf/cdean/LF_analysis/data_nTuples/output_Kshort_run3pp_looseCuts_20260608.root");
  TFile* lambda_file = TFile::Open("/sphenix/tg/tg01/hf/cdean/LF_analysis/data_nTuples/output_Lambda0_run3pp_looseCuts_20260608.root");

  TTree* ks_tree = (TTree*)Ks_file->Get("DecayTree");
  TTree* lambda_tree = (TTree*)lambda_file->Get("DecayTree");

  V0DuplicateReader ks_reader(ks_tree, V0DuplicateReader::ParticleType::K0s);
  V0DuplicateReader lambda_reader(lambda_tree, V0DuplicateReader::ParticleType::Lambda);

  TFile* Ks_outfile = new TFile("/sphenix/tg/tg01/hf/mjpeters/LightFlavorProduction/bcoSkimmedDataSample/Kshort_run3pp_looseCuts_20260608_bco_skimmed.root","RECREATE");
  TTree* ks_outtree = (TTree*)ks_tree->CloneTree(0);

  TFile* lambda_outfile = new TFile("/sphenix/tg/tg01/hf/mjpeters/LightFlavorProduction/bcoSkimmedDataSample/Lambda_run3pp_looseCuts_20260608_bco_skimmed.root","RECREATE");
  TTree* lambda_outtree = (TTree*)lambda_tree->CloneTree(0);

  ks_reader.enableDeltaBCOCut(0, 350);
  lambda_reader.enableDeltaBCOCut(0, 350);

  for(Long64_t i = 0; i < ks_reader.entries(); ++i)
  {
    if(i % 10000 == 0) std::cout << "processing BCO for Kshorts entry " << i << " / " << ks_reader.entries() << std::endl;
    ks_reader.loadEntry(i);

    if(ks_reader.passesDeltaBCOCut() && ks_reader.isCurrentEntryUnique())
    {
      ks_outtree->Fill();
    }
  }

  for(Long64_t i = 0; i < lambda_reader.entries(); ++i)
  {
    if(i % 10000 == 0) std::cout << "processing BCO for lambda entry " << i << " / " << lambda_reader.entries() << std::endl;
    lambda_reader.loadEntry(i);

    if(lambda_reader.passesDeltaBCOCut() && lambda_reader.isCurrentEntryUnique())
    {
      lambda_outtree->Fill();
    }
  }

  Ks_outfile->cd();
  ks_outtree->Write();
  lambda_outfile->cd();
  lambda_outtree->Write();
}
