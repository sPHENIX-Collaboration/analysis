#include <string>

using namespace std;

void Fun4All_TestBeam(int nEvents = 10,
                      const char *input_file = "/sphenix/data/data03//phnxreco/sphenix/t1044/fnal/beam/beam_00000499-0000.prdf",
											const char *output_file = "beam_00000499.root"
                      )
{
  gSystem->Load("libfun4all");
  gSystem->Load("libPrototype4.so");
	gSystem->Load("libMvtxPrototype1.so");

  Fun4AllServer *se = Fun4AllServer::instance();
  //se->Verbosity(0);
  se->Verbosity(Fun4AllServer::VERBOSITY_SOME);

	char ifile[strlen(input_file)+1]; // + 1 for the \0 which marks the end of string
	strcpy(ifile,input_file);
	strtok(ifile,"_");
	int runnumber = atoi(strtok(0,"-"));
	int segnumber = atoi(strtok(strtok(0,"-"),"."));
	cout << "runnumber: " << runnumber << " segment " << segnumber << endl;

  recoConsts *rc = recoConsts::instance();
  rc->set_IntFlag("RUNNUMBER",runnumber);

	MvtxRunInfoUnpackPRDF *unpack_run = new MvtxRunInfoUnpackPRDF();
	unpack_run->Verbosity(0);
	se->registerSubsystem(unpack_run);

	MvtxUnpackPRDF *unpack = new MvtxUnpackPRDF();
	if ( runnumber>=89 && runnumber<=97 ){ //brick runs
		unpack->SetCutOccupPerChip(1e-3);
	}else{
		unpack->SetCutOccupPerChip(1e-4);
	}
	unpack->Verbosity(0);
	se->registerSubsystem(unpack);

	MvtxApplyHotDead *hotdead = new MvtxApplyHotDead("MvtxApplyHotDead",Form("hotmap/hotmap_testbeam_%08d.txt",runnumber));
	hotdead->Verbosity(0);
	hotdead->PrintHotDeadMap();
	se->registerSubsystem(hotdead);

	MvtxClusterizer *clus = new MvtxClusterizer();
	clus->Verbosity(0);
	se->registerSubsystem(clus);

	MvtxAlign *align = new MvtxAlign("MvtxAlign");
	align->SetAlignmentParFileDir("beamcenter/");
	align->PrintAlignmentPars();
	align->Verbosity(0);
	se->registerSubsystem(align);

	MvtxQAHisto *qa = new MvtxQAHisto();
	qa->set_filename(Form("MvtxQAHisto-%08d-%04d.root",runnumber,segnumber+200));
	se->registerSubsystem(qa);

  Fun4AllInputManager *in = new Fun4AllPrdfInputManager("PRDFin");
  in->fileopen(input_file);
  se->registerInputManager(in);

	//Fun4AllDstOutputManager *out_Manager = new Fun4AllDstOutputManager("DSTOUT",output_file);
	//se->registerOutputManager(out_Manager);

  se->run(nEvents);

  se->End();
}
