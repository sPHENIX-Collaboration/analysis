#include "TruthConversionEval.h"
#include <fun4all/Fun4AllServer.h>
#include <fun4all/Fun4AllDstInputManager.h>
#include <phool/recoConsts.h>
int main()
{
  
  std::string OUT_LOCATION="/sphenix/user/vassalli/gammasample/background/";
  std::string OUT_FILE=OUT_LOCATION+"test/fourembededonlineanalysis0.root";
  std::string IN_FILE=OUT_LOCATION+"fourembededout0.root";
  Fun4AllServer *se = Fun4AllServer::instance();
  se->Verbosity(0);
//  recoConsts *rc = recoConsts::instance();

  Fun4AllInputManager *hitsin = new Fun4AllDstInputManager("DSTin");
  hitsin->fileopen( IN_FILE );
  se->registerInputManager(hitsin);
 
  TruthConversionEval *truther = new TruthConversionEval(OUT_FILE,0,2,3,true);
  se->registerSubsystem(truther);
//  ConvertedPhotonReconstructor *converter=new ConvertedPhotonReconstructor(outfile);
  //se->registerSubsystem(converter);
/*  outfile+="cTtree.root";

  TreeMaker *tt = new TreeMaker( outfile,3,truther);
  se->registerSubsystem( tt );
  */
  se->run();
  se->End();
  delete se;
  std::cout << "All done" << std::endl;
  return 0;
}
