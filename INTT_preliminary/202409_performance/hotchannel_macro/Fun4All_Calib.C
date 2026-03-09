#include <GlobalVariables.C>
#include <fun4all/Fun4AllDstOutputManager.h>
#include <fun4all/Fun4AllInputManager.h>
#include <fun4all/Fun4AllOutputManager.h>
#include <fun4all/Fun4AllServer.h>
#include <fun4allraw/Fun4AllStreamingInputManager.h>
#include <fun4allraw/InputManagerType.h>
#include <fun4allraw/SingleGl1PoolInput.h>
#include <fun4allraw/SingleInttPoolInput.h>
#include <inttcalib/InttCalib.h>
R__LOAD_LIBRARY(libinttcalib.so)
#include <phool/recoConsts.h>

#include <ffamodules/FlagHandler.h>

R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libffamodules.so)
R__LOAD_LIBRARY(libfun4allraw.so)
R__LOAD_LIBRARY(libffarawmodules.so)

#include <filesystem>
namespace fs = std::filesystem;

void Fun4All_Calib(int nEvents = 100000, int RunNumber = 50889, bool isStreaming = true,
                             const string &input_gl1file = "gl1daq.list",
                             const string &input_inttfile00 = "intt0.list",
                             const string &input_inttfile01 = "intt1.list",
                             const string &input_inttfile02 = "intt2.list",
                             const string &input_inttfile03 = "intt3.list",
                             const string &input_inttfile04 = "intt4.list",
                             const string &input_inttfile05 = "intt5.list",
                             const string &input_inttfile06 = "intt6.list",
                             const string &input_inttfile07 = "intt7.list"
                             )
{
// GL1 which provides the beam clock reference (if we ran with GL1)
  vector<string> gl1_infile;
  gl1_infile.push_back(input_gl1file);

// INTT
  vector<string> intt_infile;
  intt_infile.push_back(input_inttfile00);
  intt_infile.push_back(input_inttfile01);
  intt_infile.push_back(input_inttfile02);
  intt_infile.push_back(input_inttfile03);
  intt_infile.push_back(input_inttfile04);
  intt_infile.push_back(input_inttfile05);
  intt_infile.push_back(input_inttfile06);
  intt_infile.push_back(input_inttfile07);

  Fun4AllServer *se = Fun4AllServer::instance();
  se->Verbosity(1);
  recoConsts *rc = recoConsts::instance();
  rc->set_StringFlag("CDB_GLOBALTAG", "ProdA_2024");
  rc->set_uint64Flag("TIMESTAMP", RunNumber);
  Fun4AllStreamingInputManager *in = new Fun4AllStreamingInputManager("Comb");
  
  int i = 0;

  if(isStreaming) // Streaming mode ; Combining with GL1
  {
    for (auto iter : gl1_infile)
    {
      SingleGl1PoolInput *gl1_sngl = new SingleGl1PoolInput("GL1_" + to_string(i));
      //    gl1_sngl->Verbosity(3);
      gl1_sngl->AddListFile(iter);
      in->registerStreamingInput(gl1_sngl, InputManagerType::GL1);
      i++;
      }
      i = 0;
      for (auto iter : intt_infile)
      {
        SingleInttPoolInput *intt_sngl = new SingleInttPoolInput("INTT_" + to_string(i));
        intt_sngl->SetNegativeBco(120-23);
        intt_sngl->SetBcoRange(120);
        intt_sngl->AddListFile(iter);
        std::cout<<iter<<std::endl;
        in->registerStreamingInput(intt_sngl, InputManagerType::INTT);
        i++;
        } 
        i = 0;
    }
else // Triggered mode ; Combining without GL1
{
std::cout<<" Trigger mode selected " << std::endl;
std::cout<< " Combining without GL1"<<std::endl;
 for (auto iter : intt_infile)
  {
    SingleInttPoolInput *intt_sngl = new SingleInttPoolInput("INTT_" + to_string(i));
    intt_sngl->SetNegativeBco(1);
    intt_sngl->SetBcoRange(2);
    intt_sngl->AddListFile(iter);
    std::cout<<iter<<std::endl;
    in->registerStreamingInput(intt_sngl, InputManagerType::INTT);
    i++;
  } 
  i = 0;
}

 
  
  se->registerInputManager(in);
  
  FlagHandler *flag = new FlagHandler();
  se->registerSubsystem(flag);

  InttCalib *inttcalib = new InttCalib();
  inttcalib->Verbosity(0);
  fs::path currentPath = fs::current_path();
  std::string RunNumberStr = std::to_string(RunNumber);

  // Construct file paths
  std::string hotmap_cdb_file = (currentPath / "hotmap_cdb" / ("hotmap_cdb_" + RunNumberStr + ".root")).string();
  std::string hotmap_save_file = (currentPath / "hotmap_png" / ("hotmap_png_" + RunNumberStr + ".root")).string(); // .root .png .pdf
  std::string bcomap_cdb_file = (currentPath / "bcomap_cdb" / ("bcomap_cdb_" + RunNumberStr + ".root")).string();
  std::string bcomap_save_file = (currentPath / "bcomap_png" / ("bcomap_png_" + RunNumberStr + ".root")).string(); //.root .png .pdf
  
  inttcalib->SetHotMapCdbFile(hotmap_cdb_file);
  inttcalib->SetHotMapPngFile(hotmap_save_file);
  inttcalib->SetBcoMapCdbFile(bcomap_cdb_file);
  inttcalib->SetBcoMapPngFile(bcomap_save_file);
  se->registerSubsystem(inttcalib);

  if (inttcalib->Verbosity())
  {
    std::cout << "\n"
              << "hotmap_cdb_file: " << hotmap_cdb_file << "\n"
              << "hotmap_save_file: " << hotmap_save_file << "\n"
              << "bcomap_cdb_file: " << bcomap_cdb_file << "\n"
              << "bcomap_save_file: " << bcomap_save_file << "\n"
              << std::endl;
  }
  bool isTrkrHitOn = false;
  if(isTrkrHitOn) // Make DST file if needed
  {
   Fun4AllOutputManager *out = new Fun4AllDstOutputManager("out", "streaming.root");
   se->registerOutputManager(out);
  }

  if (nEvents < 0)
  {
    return;
  }
  se->run(nEvents);

  se->End();
  delete se;
  cout << "all done" << endl;
  gSystem->Exit(0);
}

