using namespace std;

void Xi_evaluator_skimmer(string type = "cascade", bool useTruth = true, int maxFiles = -1)
{
  //string fileDir = "/sphenix/user/rosstom/TrackingAnalysis/Physics_Val_TF/sim/geomAccept/cascade_20260513/evaluatorFiles/cascade/20260513/";
  string fileDir = "/sphenix/tg/tg01/hf/cdean/LF_analysis/evaluatorFiles/cascade/20260520/"; 
  string PDG_ID = "3312";
  string eval_header = "evalFile_" + type + "_";
  string eval_trailer = ".root";

  string treeName = useTruth ? "ntp_gtrack" : "ntp_track";
  TChain *myChain = new TChain(treeName.c_str());
  TSystemDirectory dir("reco_files", fileDir.c_str());
  TList *files = dir.GetListOfFiles();
  TIter fileIterator(files);
  TSystemFile *file;
  
  int currentLoadedFiles = 0;
  while ((file = (TSystemFile*)fileIterator()))
  {
    string fileName(file->GetName());
    if (fileName.find(eval_header) == std::string::npos) continue;
    if (fileName.find(eval_trailer)!= std::string::npos)
    {
      ++currentLoadedFiles;
      if (currentLoadedFiles == maxFiles) break;
      string inputFile = fileDir + fileName;
      myChain->Add(inputFile.c_str());
      std::cout << "File " << inputFile << " added" << std::endl;
    }
  }
  std::cout << "Chain has " << myChain->GetEntries() << " total entries" << std::endl;
  
  string Bachelor_check = "abs(gflavor) == 211 && abs(gparentflavor) == 3312";
  string LambdaPion_check = "abs(gflavor) == 211 && abs(gparentflavor) == 3122";
  string LambdaProton_check = "abs(gflavor) == 2212 && abs(gparentflavor) == 3122";
  string Lambda_check = "abs(gflavor) == 3122 && abs(gparentflavor) == 3312"; 
  string Xi_check = "abs(gflavor) == 3312 && gprimary == 1"; 
 
  TCut total; 
  total += ("(" + Bachelor_check + ") || (" + LambdaPion_check + ") || (" + LambdaProton_check + ") || (" + Lambda_check + ") || (" + Xi_check + ")").c_str();

  string outFileName = "output_" + type + "_" + treeName + "_20260520_pTref3p0" + eval_trailer;
  TFile *outFile = TFile::Open(outFileName.c_str(),"RECREATE");
  if (!outFile || outFile->IsZombie())
  { 
    std::cerr << "ERROR: Could not open output file" << std::endl;
    return;
  }
  
  outFile->cd();
  TTree* dataWithCut = myChain->CopyTree(total);
  dataWithCut->Write();
  outFile->Close();
}
