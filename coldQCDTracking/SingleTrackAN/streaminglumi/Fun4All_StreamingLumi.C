
#include <bcolumicount/StreamingLumiReco.h>

#include <ffamodules/SyncReco.h>

#include <fun4all/Fun4AllServer.h>
#include <fun4all/Fun4AllInputManager.h>
#include <fun4all/Fun4AllDstInputManager.h>
#include <fun4all/Fun4AllOutputManager.h>
#include <fun4all/Fun4AllDstOutputManager.h>

#include <fun4allraw/Fun4AllPrdfInputManager.h>


#include <phool/recoConsts.h>

#include <Rtypes.h> // defines R__LOAD_LIBRARY macro for clang-tidy
#include <TSystem.h>
#include <fstream>

R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libbcolumicount.so)
R__LOAD_LIBRARY(libffamodules.so)

void Fun4All_StreamingLumi(const int nEvents = 0,
		      const std::string &infile = "/sphenix/lustre01/sphnxpro/production/run3pp/physics/ana573_2026p003_v001/DST_TRKR_TRACKS/run_00079500_00079600/DST_TRKR_TRACKS_run3pp_ana573_2026p003_v001-00079516-00000.root",
          const std::string &outdir = "/sphenix/tg/tg01/coldqcd/dillfitz/singletrackan/lumi/"
        )
{

  std::string streaming_bcodst = "../streamingbcodsts/DST_STREAMING_BCOINFO-";

  std::string outfile = outdir;
  outfile += "DST_STREAMING_LUMIINFO-";
  //std::ifstream file(inlist);
  //if (!file.is_open()) {
  //  std::cerr << "Failed to open file\n";
  //}

  // There is probably a better way to do this... revisit!
  //std::string infilename;
  //if (std::getline(file, infilename)) {
      //std::string run_segments = infilename.substr(infilename.size() - 17, 8);
      // if reading in the gl1rwhitdst lists from /sphenix/lustre01/sphnxpro/production/run3pp/physics/ana532_nocdbtag_v001/DST_STREAMING_EVENT_ebdc00_0/run_00081100_00081200/*81100*
    std::string run_segments = infile.substr(infile.size() - 19, 14);
    std::string runs = infile.substr(infile.size() - 19, 8);
    outfile += run_segments;
    outfile += ".root";
    streaming_bcodst += runs;
    streaming_bcodst += ".root";
  //} else {
  //    std::cout << "File is empty or read failed\n";
 // }
  gSystem->Load("libg4dst.so");
  Fun4AllServer *se = Fun4AllServer::instance();
  //se->Verbosity(1);

  StreamingLumiReco *streaming_lumi = new StreamingLumiReco();
  //streaming_lumi->Verbosity(10);

  se->registerSubsystem(streaming_lumi);

  Fun4AllInputManager *in_gl1_dst = new Fun4AllDstInputManager("GL1_DSTin");
  //in_gl1_dst->AddListFile(inlist);
  in_gl1_dst->AddFile(infile);
  se->registerInputManager(in_gl1_dst);

  Fun4AllInputManager *in_bco_dst = new Fun4AllDstInputManager("BCO_DSTin");
  in_bco_dst->AddFile(streaming_bcodst);
  se->registerInputManager(in_bco_dst);

  Fun4AllOutputManager *out = new Fun4AllDstOutputManager("out",outfile);
  se->registerOutputManager(out);

  se->run(nEvents);

  se->End();
  delete se;
  gSystem->Exit(0);
}
