#include "corral_class.h"
#include <fstream>
//size_t	getCurrentRSS();
//size_t	getPeakRSS();

void DisplayHelp();

int main(int argc, char **argv) { 
	//
	delete 	gRandom;
	gRandom	= new TRandom3;
	//
	TTree	*tree	=  0;	//
 	int		ntd     =  0;	// nToDo
// 	int		idata	=  0; 	// kDataSet
// 	int		kcent	=  2;	//OBSOLETE =1 rmreader, =2 rm2reader, =3 rm3reader, bbc, emce, emcn
// 	int		kcs		=  0;	// cuts set...
// 	int		kpart	=  0; 	// 0=chg, 1=pi, 2=k, 3=p 	....OBSOLETE....
// 	int		kchu	= -1;	// default -1 means read all events in same job... >=0 means process events from specific chunk
// 	//int	kmod	=  0;	// =0 real data, =1 urqmd, ....... 
// 	float	fmod	=  0;	// =0 real data, =1 urqmd, ....... generally integer, except some models e.g. 4.1 (smashSkySym)
 	TString runstr	= TString("");
	TString outname	= TString("");		// -o
	TString listname	= TString("");		// -l
	TString dataset	= TString("");		// -d
	//
//	if (argc==1){
//		DisplayHelp();
//		exit(0);
//	}
	//
//	for (int i=1; i<argc; i++) {
//		cout<<"INPUT "<<i<<" "<<argv[i]<<endl;
//	}
	//
	//---- get -d (dataset) information first... 
// 	bool finalize	= false; 
// 	for (int i=1; i<argc; i++) {
// 		if (argc>1){
// 			if (argv[i][0] == '-') { 
// 				switch (argv[i][1]) {
// 					case 'h': 
// 						DisplayHelp();
// 						exit(0);		
// 						break;
// 					case 'd': 
// 						idata		= atoi(argv[++i]);
// 						//if (idata==0) finalize	= true;
// 						break;
// 				}
// 			} 
// 		}
// 	}
	//
	//---- -h / --help: usage + every actionable RunString fragment, then exit
	for (int i=1; i<argc; i++) {
		TString a(argv[i]);
		if (a=="-h" || a=="--help"){ DisplayHelp(); exit(0); }
	}
	for (int i=1; i<argc; i++) {
		//cout<<i<<" "<<argc<<" "<<argv[i]<<endl;
		if (argc>1){
			if (argv[i][0] == '-') { 
				switch (argv[i][1]) {
					case 'n': 
						ntd			= atoi(argv[++i]);
						//cout<<"n="<<ntodo<<endl;
						break;
// 					case 'm': 
// 						//kmod		= atoi(argv[++i]);
// 						fmod		= atof(argv[++i]);
// 						//cout<<"fmod="<<fmod<<endl;
// 						break;
// 					case 'c': 
// 						kcs			= atoi(argv[++i]);
// 						//cout<<"kcs="<<kcs<<endl;
// 						break;
					case 'l':		// job list: one Collect file path per line (lists/<production>/list_NN.txt)
						listname	= TString(argv[++i]);
						break;
					case 'd':		// dataset (Finalize): lists/<dataset>/, root/<dataset>/chunks/, root/<dataset>/corral_m.root
						dataset		= TString(argv[++i]);
						break;
					case 'o':		// output base name: root/<name>.root, pdf/<name>.pdf (not scanned for RunString tokens)
						outname		= TString(argv[++i]);
						break;
					case 's':
						runstr		= TString(argv[++i]);
						//cout<<"runstr="<<runstr.Data()<<endl;
						break;
// 					case 'x': 
// 						kchu		= atoi(argv[++i]);
// 						//cout<<"kchu="<<kchu<<endl;
// 						break;
				}
			} 
		}
	}
	//
// 	cout<<"..... main      fmod = "<<fmod	<<endl;
// 	cout<<"..... main     idata = "<<idata	<<endl;
// 	cout<<"..... main     ntodo = "<<ntodo	<<endl;
// 	cout<<"..... main       kcs = "<<kcs	<<endl;
// 	cout<<"..... main      kchu = "<<kchu	<<endl;
// 	cout<<"..... main  finalize = "<<(int)finalize<<endl;
// 	cout<<"..... main runstring = "<<runstr.Data()<<endl;
// 	//
// 	cout<<"main -- RAM(GB) = "<<getCurrentRSS()/1024./1024./1024.<<","<<getPeakRSS()/1024./1024./1024.<<" ... begin"<<endl;
	//

	if (listname!=""){
		TChain *chain	= new TChain("outTree","Collect tree chain (job list)");
		std::ifstream fl(listname.Data());
		if (!fl.good()){ cout<<"main -- cannot open list "<<listname<<endl; exit(1); }
		std::string line; int nf=0;
		while (std::getline(fl,line)){ if (line.empty()) continue; chain->Add(line.c_str()); ++nf; }
		cout<<"main -- list "<<listname<<": "<<nf<<" files"<<endl;
		if (nf==0){ cout<<"main -- empty list, exit"<<endl; exit(1); }
		tree	= chain;
	}
	cout<<"Instantiating corral..."<<endl;
	corral *read	= new corral(tree, ntd, runstr, outname);
	read->Dataset	= dataset;
	if (runstr=="Finalize")	read->Finalize();	// README_Finalize.md (temporary switch): combine chunk outputs
	else					read->Loop();

	//
// 	cout<<"main -- RAM(GB) = "<<getCurrentRSS()/1024./1024./1024.<<","<<getPeakRSS()/1024./1024./1024.<<" ... end program"<<endl;
// 	cout<<"main -- Program done ......................... "<<endl;
	//	
} 

void DisplayHelp(){
	//---- keep in sync with the RunString parsing in corral_utils.cxx (corral::corral)
	cout<<endl;
	cout<<"USAGE: ./corral_m [OPTIONS]"<<endl;
	cout<<"   -h, --help    display this help"<<endl;
	cout<<"   -n [ntodo]    do [ntodo] events (= tree entries: one crossing with a vertex), 0 for all"<<endl;
	cout<<"   -l [listfile] read the Collect files listed in [listfile] (one path per line) instead of the whole"<<endl;
	cout<<"                 directory, e.g. lists/ana532/list_07.txt (see lists/make_lists.bash)"<<endl;
	cout<<"                 Without -l, all files matching $CORRAL_TREES are read (default: the WSU copy of ana532,"<<endl;
	cout<<"                 /rs/rs_grp_rhi/sPHENIX/ana532/outputCollect_*.root)"<<endl;
	cout<<"   -d [dataset]  dataset for Finalize: reads lists/[dataset]/, root/[dataset]/chunks/corral_m_NN.root and"<<endl;
	cout<<"                 the reference root/[dataset]/corral_m.root (default: FINALIZE_SET in src/finalize_hists.h)"<<endl;
	cout<<"   -o [name]     output base name: root/[name].root, pdf/[name].pdf (NOT scanned for fragments)"<<endl;
	cout<<"   -s [string]   run string: appended to the output filenames (root/corral_m_<string>.root,"<<endl;
	cout<<"                 pdf/...pdf; none -> corral_m.root) AND scanned for the fragments below"<<endl;
	cout<<endl;
	cout<<"DEFAULTS (no fragments): LS split-track removal ON (SL<0.18 & SiSplitScore>=0.10, plus RadialGap>=0.45),"<<endl;
	cout<<"  ULS opposite-charge veto ON (SiSplitScore>=0.05, any angle), cross-crossing duplicate cleaner ON (strict),"<<endl;
	cout<<"  adjacent-trigger-frame mixing exclusion ON, crossing correction ON (pt-ordering), ntpc>=18, no Q cut, 32x36 (dy,dphi) binning."<<endl;
	cout<<endl;
	cout<<"RUNSTRING FRAGMENTS (substring match, case-insensitive unless marked [case]; NN = two digits):"<<endl;
	cout<<"  split tracks, like-sign (README_SplitTracks sec 13-17)"<<endl;
	cout<<"    noLS          turn OFF the whole LS split-track removal pre-pass (both paths)"<<endl;
	cout<<"    noRG          turn OFF only the RadialGap path (path 2); SL/SKF path stays on"<<endl;
	cout<<"    removecutNN   [case] SL threshold = 0.NN (default 0.18); 'removecut-NN' for negative"<<endl;
	cout<<"    skfNN         [case] pre-pass SiSplitScore gate = 0.NN (default 0.10)"<<endl;
	cout<<"    radialgapNN   [case] RadialGap threshold = 0.NN (default 0.45)"<<endl;
	cout<<"  split tracks, opposite-charge (sec 15, 18.10, 18.24)"<<endl;
	cout<<"    noulstest     turn OFF the ULS track-level veto"<<endl;
	cout<<"    ulstestNN     ULS veto ON at SiSplitScore threshold 0.NN (default 0.05)"<<endl;
	cout<<"  cross-crossing / trigger-frame duplicates (sec 18.20-18.25)"<<endl;
	cout<<"    noXTF         turn OFF the duplicate cleaner AND the adjacent-TF mixing exclusion (old behavior)"<<endl;
	cout<<"    XTFclean      cleaner ON, NON-strict (the 'neither copy matches INTT' case keeps the better copy)"<<endl;
	cout<<"    XTFcleanStrict cleaner ON, strict (default): 'neither' case drops both copies"<<endl;
	cout<<"    mixAdjTF      turn OFF the adjacent-TF (|devt|==1) mixed-event-pair exclusion"<<endl;
	cout<<"    mixNoAdjTF    adjacent-TF exclusion ON (default; with noXTF it widens to |devt|<=1)"<<endl;
	cout<<"    noTFdup       keep collisions copied into overlapping TFs (default: skip later copies,"<<endl;
	cout<<"                  key run+bco+crossing; needs the Collect bco branch, else no effect)"<<endl;
	cout<<"  trigger-frame (TF) event selections (diagnostics, sec 18.21; all OFF by default, cost statistics)"<<endl;
	cout<<"    onlyfirsttf   keep only the first event of each TF (the lowest, often negative, crossing)"<<endl;
	cout<<"    Xing0         keep only crossing==0 events (triggered collisions)"<<endl;
	cout<<"    XingPos       keep only the first crossing>0 event of each TF (streaming)"<<endl;
	cout<<"  other"<<endl;
	cout<<"    nocross       turn OFF the crossing correction (pt-ordering at fill + (dy,dphi) bin correction)"<<endl;
	cout<<"    ntpcNN        [case] track cut ntpc>=NN (default 18)"<<endl;
	cout<<"    qcut          reject pairs with qinv < 0.150 GeV"<<endl;
	cout<<"    548           fine binning: 5 dy x 48 dphi bins"<<endl;
	cout<<endl;
	cout<<"  Beware substring collisions when naming tags: e.g. 'mixNoAdjTF' contains 'NoAdjTF', 'XTFcleanStrict'"<<endl;
	cout<<"  contains 'XTFclean', 'noulstest' contains 'ulstest', and any word containing 'nocross' turns off the crossing correction."<<endl;
	cout<<endl;
}
