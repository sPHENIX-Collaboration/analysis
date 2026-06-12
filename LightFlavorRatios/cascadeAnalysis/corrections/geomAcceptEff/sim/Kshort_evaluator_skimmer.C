using namespace std;

void Kshort_evaluator_skimmer(string type = "Kshort", bool useTruth = true, int maxFiles = -1)
{
  //string fileDir = "/sphenix/tg/tg01/hf/frawley/sims/pythia_fromhits_detroit_volcut/";
  //string fileDir = type == "Kshort" ? "/sphenix/tg/tg01/hf/cdean/LF_analysis/evaluatorFiles/Kshort2pipi/20260504/"
  //                                  : "/sphenix/tg/tg01/hf/cdean/LF_analysis/evaluatorFiles/Lambda2ppi/20260504/";
  string fileDir = "/sphenix/tg/tg01/hf/cdean/LF_analysis/evaluatorFiles/Kshort2pipi/20260520/";
  string PDG_ID = type == "Kshort" ? "310" : "3122";
  string eval_header = "evalFile_Kshort2pipi";
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
  
  //string mother_check = "abs(gparentflavor) == " + PDG_ID;
  string mother_check = "(abs(gflavor) == 310 && gprimary == 1)";

  string daughter_check = type == "Kshort" ? "(abs(gflavor) == 211 && abs(gparentflavor) == 310)" : "((abs(gflavor) == 211) || (abs(gflavor) == 2212))";
 
  TCut total;
  total += (mother_check + " || " + daughter_check).c_str();  
 
  string outFileName = "output_" + type + "_" + treeName + "_20260520_pTref3p0" + eval_trailer;
  TFile *outFile = TFile::Open(outFileName.c_str(),"RECREATE");
  outFile->cd();
  TTree* dataWithCut = myChain->CopyTree(total);
  dataWithCut->Write();
  outFile->Close();
}
