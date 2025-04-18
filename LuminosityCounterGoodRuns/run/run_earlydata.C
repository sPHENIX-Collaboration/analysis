#include <fun4all/Fun4AllServer.h>
#include <fun4all/Fun4AllInputManager.h>
#include <fun4all/Fun4AllDstInputManager.h>
#include <fun4all/SubsysReco.h>
#include <phool/recoConsts.h>
#include <ffamodules/CDBInterface.h>
#include <fun4all/Fun4AllRunNodeInputManager.h>
#include <triggercountmodule/triggercountmodule.h>
using namespace std;

R__LOAD_LIBRARY(libfun4allraw.so)
R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libffamodules.so)
R__LOAD_LIBRARY(libtriggercountmodule.so)

int run_earlydata(int nproc = 0, int debug = 0, int rn = 0, int nfile = 0, string dir = ".")
{
  string triggercountfilename = dir+"/trigout/triggercounter_"+to_string(rn)+"_"+to_string(nproc)+".root";
  Fun4AllServer *se = Fun4AllServer::instance();
  recoConsts *rc =  recoConsts::instance();
  rc->set_StringFlag("CDB_GLOBALTAG","ProdA_2024");
  rc->set_uint64Flag("TIMESTAMP",rn);
  Fun4AllInputManager *in_1 = new Fun4AllDstInputManager("DSTin1");
  string line1;
  line1 = "./dsts/"+to_string(rn)+"/"+to_string(rn)+"_"+to_string(nproc)+".root";
  in_1->AddFile(line1);
  se->registerInputManager( in_1 );
  triggercountmodule* trig = new triggercountmodule(triggercountfilename, rn, nproc, nfile);
  se->registerSubsystem(trig);
  CDBInterface::instance()->Verbosity(0);
  se->run(0);
  se->Print("NODETREE");
  se->End();
  se->PrintTimer();
  delete se;
  return 0;

}
