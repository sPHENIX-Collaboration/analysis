//-------------------------------------------------------
//	w.j. llope, wjllope@wayne.edu, run-2 2024
//
#include "Collect.h"

//---- for tracking...
#include <limits>
#include <map>
#include <fun4all/Fun4AllReturnCodes.h>
#include <phool/phool.h>
#include <phool/getClass.h>
#include <phool/PHCompositeNode.h>
#include <phool/PHNodeIterator.h>
#include <trackbase/ClusterErrorPara.h>
#include <trackbase/InttDefs.h>
#include <trackbase/MvtxDefs.h>
#include <trackbase/TpcDefs.h>
#include <trackbase/TrackFitUtils.h>
#include <trackbase/TrkrCluster.h>
#include <trackbase/TrkrClusterContainer.h>
#include <trackbase/TrkrHit.h>
#include <trackbase/TrkrHitSet.h>
#include <trackbase/TrkrHitSetContainer.h>
#include <g4detectors/PHG4CylinderGeomContainer.h>
#include <g4detectors/PHG4TpcGeom.h>
#include <g4detectors/PHG4TpcGeomContainer.h>
#include <globalvertex/GlobalVertex.h>
#include <globalvertex/GlobalVertexMap.h>
#include <globalvertex/SvtxVertex.h>
#include <globalvertex/SvtxVertexMap.h>
// #include <globalvertex/SvtxVertexMap_v1.h>
// #include <trackbase_historic/ActsTransformations.h>
// #include <trackbase_historic/SvtxAlignmentState.h>
// #include <trackbase_historic/SvtxAlignmentStateMap.h>
// #include <trackbase_historic/SvtxTrackMap.h>
#include <trackbase_historic/TrackSeed.h>
#include <trackbase_historic/TrackSeedContainer.h>
#include <tpc/TpcGlobalPositionWrapper.h>
#include <tpc/TpcDistortionCorrectionContainer.h>

#include <trackbase_historic/SvtxTrack.h>
#include <trackbase_historic/SvtxTrackMap.h>
#include <trackbase_historic/SvtxTrackState.h>
// #include <trackbase_historic/SvtxTrack_v4.h>
// #include <trackbase_historic/SvtxTrackMap_v2.h>
// #include <trackbase_historic/SvtxTrackState_v3.h>
#include <trackbase_historic/SvtxTrackSeed_v1.h>
#include <trackbase_historic/TrackAnalysisUtils.h>

// Calo includes
#include <calobase/RawCluster.h>
#include <calobase/RawClusterContainer.h>
#include <calobase/RawClusterUtility.h>
#include <calobase/RawTower.h>
#include <calobase/RawTowerContainer.h>
#include <calobase/RawTowerGeom.h>
#include <calobase/RawTowerGeomContainer.h>
#include <calotrigger/CaloTriggerInfo.h>
#include <calobase/TowerInfo.h>
#include <calobase/TowerInfoContainer.h>
#include <calobase/TowerInfoDefs.h>

//#include <KFParticle.h>
//#include <trackbase/ActsGeometry.h>
#include <Acts/Definitions/Algebra.hpp>

#include <ffaobjects/RunHeader.h>
#include <ffaobjects/EventHeader.h>
#include <ffarawobjects/Gl1Packet.h>
#include <ffarawobjects/Gl1RawHit.h>

#include <epd/EpdGeom.h>
#include "eventplaneinfo/Eventplaneinfo.h"
#include "eventplaneinfo/EventplaneinfoMap.h"
#include "eventplaneinfo/EventplaneinfoMapv1.h"
#include "eventplaneinfo/Eventplaneinfov1.h"

#include <TDatabasePDG.h>

#include <kfparticle_sphenix/KFParticle_Container.h>
#include <kfparticle_sphenix/KFParticle_Tools.h>
#include "KFPTrack.h"
//KFParticle_Tools kf_tools;
//#include <KFParticle.h>

#include <vector>
using std::cout;
using std::endl;
#include <stdexcept>

// #include <odbc++/connection.h>
// #include <odbc++/drivermanager.h>
// #include <odbc++/resultset.h>
// #include <odbc++/statement.h>
// #include <odbc++/types.h>

double EdgeEMeta[97] = {-1.13381,-1.11254,-1.09107,-1.06949,-1.04781,-1.02602,-1.00413,-0.98214,-0.960045,-0.937848,-0.915549,-0.89315,-0.870648,-0.848046,-0.825343,
            -0.802541,-0.779637,-0.756634,-0.733531,-0.710327,-0.687024,-0.66362,-0.640116,-0.616512,-0.592806,-0.569,-0.545091,-0.52108,-0.496965,-0.472747,
            -0.448423,-0.423994,-0.399457,-0.374812,-0.350056,-0.325188,-0.300207,-0.27511,-0.249896,-0.224561,-0.199104,-0.173523,-0.148255,-0.123759,-0.0996162,
            -0.0754161,-0.0511715,-0.0268971,0.,0.0268971,0.0511715,0.0754161,0.0996162,0.123759,0.148255,0.173523,0.199104,0.224561,0.249896,0.27511,
            0.300207,0.325188,0.350056,0.374812,0.399457,0.423994,0.448423,0.472747,0.496965,0.52108,0.545091,0.569,0.592806,0.616512,0.640116,
            0.66362,0.687024,0.710327,0.733531,0.756634,0.779637,0.802541,0.825343,0.848046,0.870648,0.89315,0.915549,0.937848,0.960045,0.98214,
            1.00413,1.02602,1.04781,1.06949,1.09107,1.11254,1.13381};
double EdgeIHeta[25] = {-1.1,-1.00833,-0.916667,-0.825,-0.733333,-0.641667,-0.55,-0.458333,-0.366667,-0.275,-0.183333,-0.0916667,0.,0.0916667,0.183333,
            0.275,0.366667,0.458333,0.55,0.641667,0.733333,0.825,0.916667,1.00833,1.1};
double EdgeOHeta[25] = {-1.1,-1.00833,-0.916667,-0.825,-0.733333,-0.641667,-0.55,-0.458333,-0.366667,-0.275,-0.183333,-0.0916667,0.,0.0916667,0.183333,
            0.275,0.366667,0.458333,0.55,0.641667,0.733333,0.825,0.916667,1.00833,1.1};

#define FIXFLOAT(x) std::fixed<<std::setprecision(3)<<(x)

//  enum CAL_LAYER
//   {
//     PRES = 0,
//     CEMC = 1,
//     HCALIN = 2,
//     HCALOUT = 3,
//     OUTER_CEMC = 4,
//     OUTER_HCALIN = 5,
//     OUTER_HCALOUT = 6
//   };

//----- take arbitrary phi value in sPH coord system and get phi index of calorimeter tower
int Collect::GetIPHI(int layer, double state_phi){
	double anphi=-1.;				// number of towers azimuthally
	if (layer==SvtxTrack::CEMC){
		anphi	= 256.; 
	} else if ( layer==SvtxTrack::HCALIN || layer==SvtxTrack::HCALOUT ){	
		anphi	=  64.; 
	}
	double phib	= state_phi + 2.0*M_PI/64.;
	if (phib < 0.     ){ phib += 2.0*M_PI; } 
	if (phib >=2.*M_PI){ phib -= 2.0*M_PI; }
	int iphi	= phib / (2.0*M_PI/anphi);
	return iphi;
}
//----- take arbitrary eta value in sPH coord system and get eta index of calorimeter tower
int Collect::GetIETA(int layer, double state_eta){
	double aneta=-1;
	double limlo=-1,limup=-1;
	if ( layer==SvtxTrack::CEMC ){
		aneta	= 96.;
		limlo	= EdgeEMeta[0];
		limup	= EdgeEMeta[int(aneta)];	// note there are NETA+1 values in this array!
	} else if ( layer==SvtxTrack::HCALIN || layer==SvtxTrack::HCALOUT ){
		aneta	= 24.; 
		limlo	= EdgeIHeta[0];
		limup	= EdgeIHeta[int(aneta)];	// note there are NETA+1 values in this array!
	}
	int ieta	= -1;
	if ( state_eta>=limlo && state_eta<limup ){
		for (int ie=0;ie<int(aneta);ie++){ 
			if (layer==SvtxTrack::CEMC    && state_eta<EdgeEMeta[ie+1]){ ieta=ie; break; } 
			if (layer==SvtxTrack::HCALIN  && state_eta<EdgeIHeta[ie+1]){ ieta=ie; break; } 
			if (layer==SvtxTrack::HCALOUT && state_eta<EdgeOHeta[ie+1]){ ieta=ie; break; }
		}
	}
	return ieta;
}

//____________________________________________________________________________..
Collect::Collect(const std::string &name):SubsysReco(name){
	//
	fEvtSeqFirst	= -1;
	//
	kVerbosity		= 0;
	DEBUG			= false;
	DEBUG2			= false;
	foutfileName	= TString("new.root");
	for (int i=0;i<MAXNTRACKS;i++){
		fTprimary[i]		= 0;
	}
	fKillSplitTracks	= false;	// off until validated on real data - see README_SplitTracks.md
	ievtSeen		= -1;			// incremented at TOP of process_event!
	fTtriggerVector.clear();		//
	fNTracks	= 0;
	fTnv0		= 0;
	sumNfoundKS	= 0;
	sumNfoundLA	= 0;
	sumNfoundAL	= 0;
	fHasCaloGeo	= false;
	//
	THRESH_em	= 0.150;
	THRESH_ih	= 0.012;
	THRESH_oh	= 0.06;
	THRESH_ep	= 0.30;
	Kshort_PDGID	=   310;
	Lambda_PDGID	=  3122;
	ALambda_PDGID	= -3122;
	//
	//std::cout << "Collect::Collect(const std::string &name) Calling ctor" << std::endl;
}

//____________________________________________________________________________..
Collect::~Collect(){
	//std::cout << "Collect::~Collect() Calling dtor" << std::endl;
}

//____________________________________________________________________________..
int Collect::Init(PHCompositeNode *topNode){
	//std::cout << "Collect::Init -- Initializing" << std::endl;
	//
	bool TREETRACKS	= true;
	bool TREEV0		= true;
	//
	//---- book
	std::cout<<"Collect::Init -- opening "<<foutfileName<<std::endl;
	fout	= new TFile(foutfileName,"RECREATE");
	//
	hnvtx_frame	= new TH1D("hnvtx_frame","No. Vertices/TriggerFrame vs triggerframe",10003,-1.5,10001.5);
	hdXing      = new TH1D("hdXing","Vertexed Crossings Interval (crossings)",500,-0.5,499.5);
	hech_em		= new TH1D("hech_em","channel energy em ",2000,-0.5,1.5);
	hech_ih		= new TH1D("hech_ih","channel energy ih ",2000,-0.5,1.5);
	hech_oh		= new TH1D("hech_oh","channel energy oh ",2000,-0.5,1.5);
	hech_ep		= new TH1D("hech_ep","channel energy epd",2000,-0.5,4.5);
	//
//	MatchCounter	= 0;
// 	for (int im=0;im<100;im++){
// 		hmatch_EMetaphiE[im]	= new TH2D(Form("hmatch_EMetaphiE_%d",im),Form("hmatch_EMetaphiE_%d",im),96,-1.1,1.1,256,0.,2.*M_PI);
// 		hmatch_HCetaphiE[im]	= new TH2D(Form("hmatch_HCetaphiE_%d",im),Form("hmatch_HCetaphiE_%d",im),24,-1.1,1.1, 64,0.,2.*M_PI);
// 		gmatch_EMetaphi[im]		= new TGraph();		// position of track in eta,phi at EM layer
// 		gmatch_HCetaphi[im]		= new TGraph();		// position of track in eta,phi at HC layer
// 		gmatch_EMetaphi[im]		->SetMarkerStyle(20);
// 		gmatch_HCetaphi[im]		->SetMarkerStyle(20);
// 		gmatch_EMetaphi[im]		->SetMarkerColor(418);	// kGreen+2
// 		gmatch_HCetaphi[im]		->SetMarkerColor(600);	// kBlue
// 	}
	//
	if (CALODETAIL){
		//
		hmapEM_eta	= new TProfile("hmapEM_eta","hmapEM_eta"  ,10.* 96,-1.152,1.152,-1,300.);
		hmapEM_phi	= new TProfile("hmapEM_phi","hmapEM_phi"  ,10.*256,0.,2.*M_PI,-1,300.);
		hmapIH_eta	= new TProfile("hmapIH_eta","hmapIH_eta"  ,10.* 24,-1.2,  1.2,-1,300.);
		hmapIH_phi	= new TProfile("hmapIH_phi","hmapIH_phi"  ,10.* 64,0.,2.*M_PI,-1,300.);
		hmapOH_eta	= new TProfile("hmapOH_eta","hmapOH_eta"  ,10.* 24,-1.2,  1.2,-1,300.);
		hmapOH_phi	= new TProfile("hmapOH_phi","hmapOH_phi"  ,10.* 64,0.,2.*M_PI,-1,300.);
		hmapEM_eta2	= new TProfile("hmapEM_eta2","hmapEM_eta2",10.* 96,-1.152,1.152,-1,300.);
		hmapEM_phi2	= new TProfile("hmapEM_phi2","hmapEM_phi2",10.*256,0.,2.*M_PI,-1,300.);
		hmapIH_eta2	= new TProfile("hmapIH_eta2","hmapIH_eta2",10.* 24,-1.2,  1.2,-1,300.);
		hmapIH_phi2	= new TProfile("hmapIH_phi2","hmapIH_phi2",10.* 64,0.,2.*M_PI,-1,300.);
		hmapOH_eta2	= new TProfile("hmapOH_eta2","hmapOH_eta2",10.* 24,-1.2,  1.2,-1,300.);
		hmapOH_phi2	= new TProfile("hmapOH_phi2","hmapOH_phi2",10.* 64,0.,2.*M_PI,-1,300.);
		hmapEM_ieta	= new TProfile("hmapEM_ieta","hmapEM_ieta", 96,0, 96,    -10,10);
		hmapEM_iphi	= new TProfile("hmapEM_iphi","hmapEM_iphi",256,0,256,    -10,10);
		hmapIH_ieta	= new TProfile("hmapIH_ieta","hmapIH_ieta", 24,0, 24,    -10,10);
		hmapIH_iphi	= new TProfile("hmapIH_iphi","hmapIH_iphi", 64,0, 64,    -10,10);
		hmapOH_ieta	= new TProfile("hmapOH_ieta","hmapOH_ieta", 24,0, 24,    -10,10);
		hmapOH_iphi	= new TProfile("hmapOH_iphi","hmapOH_iphi", 64,0, 64,    -10,10);
		//
		for (int iev=0;iev<MAXEVTDETAIL;iev++){
			for (int ilayer=0;ilayer<3;ilayer++){
				int nbeta=24,nbphi=64;
				if (ilayer==0){ nbeta=96; nbphi=256;}
				hlayer[ilayer][iev]			= new TH2D(Form("hlayer_%d_%d"      ,ilayer,iev),Form("hlayers_%d_%d"      ,ilayer,iev),nbeta,-0.5,((double)nbeta)-0.5,nbphi,-0.5,((double)nbphi)-0.5);
				if (ilayer==0){ 
					hlayer_efDET[ilayer][iev]	= new TH2D(Form("hlayer_efDET_%d_%d",ilayer,iev),Form("hlayers_efDET_%d_%d",ilayer,iev),nbeta,EdgeEMeta,nbphi,-M_PI,M_PI);
					hPTOT_etaDETem[iev]			= new TH1D(Form("hPTOT_etaDETem_%d",iev),Form("hPTOT_etaDETem_%d",iev),nbeta,EdgeEMeta );
					hPTOT_phiDETem[iev]			= new TH1D(Form("hPTOT_phiDETem_%d",iev),Form("hPTOT_phiDETem_%d",iev),nbphi,-M_PI,M_PI);
				} else if (ilayer==1){
					hlayer_efDET[ilayer][iev]	= new TH2D(Form("hlayer_efDET_%d_%d",ilayer,iev),Form("hlayers_efDET_%d_%d",ilayer,iev),nbeta,EdgeIHeta,nbphi,-M_PI,M_PI);
					hPTOT_etaDETih[iev]			= new TH1D(Form("hPTOT_etaDETih_%d",iev),Form("hPTOT_etaDETih_%d",iev),nbeta,EdgeIHeta );
					hPTOT_phiDETih[iev]			= new TH1D(Form("hPTOT_phiDETih_%d",iev),Form("hPTOT_phiDETih_%d",iev),4*nbphi,-M_PI,M_PI);
				} else if (ilayer==2){
					hlayer_efDET[ilayer][iev]	= new TH2D(Form("hlayer_efDET_%d_%d",ilayer,iev),Form("hlayers_efDET_%d_%d",ilayer,iev),nbeta,EdgeOHeta,nbphi,-M_PI,M_PI);
					hPTOT_etaDEToh[iev]			= new TH1D(Form("hPTOT_etaDEToh_%d",iev),Form("hPTOT_etaDEToh_%d",iev),nbeta,EdgeOHeta );
					hPTOT_phiDEToh[iev]			= new TH1D(Form("hPTOT_phiDEToh_%d",iev),Form("hPTOT_phiDEToh_%d",iev),4*nbphi,-M_PI,M_PI);
				}
			}
		}
		hPedestalEM	= new TProfile2D("hPedestalEM","Pedestal EM vs (etaDET,phiDET)",96,EdgeEMeta,256,-M_PI,M_PI,"s");	// ERRORS ARE STD DEV!!!
		hPedestalIH	= new TProfile2D("hPedestalIH","Pedestal IH vs (etaDET,phiDET)",24,EdgeIHeta, 64,-M_PI,M_PI,"s");	// ERRORS ARE STD DEV!!!
		hPedestalOH	= new TProfile2D("hPedestalOH","Pedestal OH vs (etaDET,phiDET)",24,EdgeOHeta, 64,-M_PI,M_PI,"s");	// ERRORS ARE STD DEV!!!
	// 	hPedestalCorrEM = new TH2D("hPedestalCorrEM","Pedestal Corr EM ",100,-THRESH_em,THRESH_em,100,-THRESH_em,THRESH_em);
	// 	hPedestalCorrIH = new TH2D("hPedestalCorrIH","Pedestal Corr IH ",100,-THRESH_ih,THRESH_ih,100,-THRESH_ih,THRESH_ih);
	// 	hPedestalCorrOH = new TH2D("hPedestalCorrOH","Pedestal Corr OH ",100,-THRESH_oh,THRESH_oh,100,-THRESH_oh,THRESH_oh);
		for (int iet=0;iet<96;iet++){
			for (int ifi=0;ifi<256;ifi++){
					hEevt_em[iet][ifi]	= new TH1D(Form("hEevt_em_%d_%d",iet,ifi),Form("hEevt_em ieta=%d iphi=%d",iet,ifi),10003,-1.5,10001.5);
				if (iet<24&&ifi<64){
					hEevt_ih[iet][ifi]	= new TH1D(Form("hEevt_ih_%d_%d",iet,ifi),Form("hEevt_ih ieta=%d iphi=%d",iet,ifi),10003,-1.5,10001.5);
					hEevt_oh[iet][ifi]	= new TH1D(Form("hEevt_oh_%d_%d",iet,ifi),Form("hEevt_oh ieta=%d iphi=%d",iet,ifi),10003,-1.5,10001.5);
				}
			}
		}
	}	// end CALODETAIL
	//
	outTree	= new TTree("outTree","Collect Tree");
	//
	outTree->Branch("run"     ,&fRunNum     ,"run/I");
	outTree->Branch("segment" ,&fSegment    ,"segment/I");
	outTree->Branch("evt"     ,&fEvtSeq     ,"evt/I");
	outTree->Branch("trigVec" ,&fTtriggerVector);
	outTree->Branch("nvtx"    ,&fTnvtx      ,"nvtx/I");
	outTree->Branch("vtxntr"  ,&fTvtxntr    ,"vtxntr/I");
	outTree->Branch("vtxx"    ,&fTvtxx      ,"vtxx/D");
	outTree->Branch("vtxy"    ,&fTvtxy      ,"vtxy/D");
	outTree->Branch("vtxz"    ,&fTvtxz      ,"vtxz/D");
//?	outTree->Branch("vtxzGlob",&fTvtxzGlob  ,"vtxzGlob/D");
	outTree->Branch("crossing",&fTcrossing  ,"crossing/I");
	outTree->Branch("etotem"  ,&fTTotE_em   ,"etotem/D");
	outTree->Branch("etotih"  ,&fTTotE_ih   ,"etotih/D");
	outTree->Branch("etotoh"  ,&fTTotE_oh   ,"etotoh/D");
	outTree->Branch("etotioh" ,&fTTotE_ioh  ,"etotioh/D");
	outTree->Branch("etotepd" ,&fTTotE_epd  ,"etotepd/D");
	//
	if (TREETRACKS){
		cout<<"Collect::Init .. Enabled TREETRACKS"<<endl;
		outTree->Branch("ntr"     ,&fNTracks    ,"ntr/I");
		outTree->Branch("pid"     ,fTpid        ,"pid[ntr]/B");		// =-1 undefined, =0 pion, =1 kaon, =2 proton (ID'd as V0 daughters) - sec.12c: was /I
		outTree->Branch("indv0"   ,fTindv0      ,"indv0[ntr]/S");	// if >=0, this is the index of the v0 mother of this track in v0 tree! - sec.12c: was /I
		outTree->Branch("primary" ,fTprimary    ,"primary[ntr]/B");	// if ==1, particle attached to PV - sec.12c: was /I
		outTree->Branch("quality" ,fTquality    ,"quality[ntr]/F");	// sec.12c: was /D (this block down through dedxKFP)
		outTree->Branch("chisq"   ,fTchisq      ,"chisq[ntr]/F");
		outTree->Branch("chg"     ,fTchg        ,"chg[ntr]/F");
//		outTree->Branch("px"      ,fTpx         ,"px[ntr]/D");
//		outTree->Branch("py"      ,fTpy         ,"py[ntr]/D");
//		outTree->Branch("pz"      ,fTpz         ,"pz[ntr]/D");
		outTree->Branch("ptot"    ,fTptot       ,"ptot[ntr]/F");
		outTree->Branch("eta"     ,fTeta        ,"eta[ntr]/F");
		outTree->Branch("phi"     ,fTphi        ,"phi[ntr]/F");
		outTree->Branch("pt"      ,fTpt         ,"pt[ntr]/F");
		outTree->Branch("seedeta" ,fTseedeta    ,"seedeta[ntr]/F");
		outTree->Branch("seedphi" ,fTseedphi    ,"seedphi[ntr]/F");
		outTree->Branch("seedpt"  ,fTseedpt     ,"seedpt[ntr]/F");
		outTree->Branch("ntpc"    ,fTnhitstpc   ,"ntpc[ntr]/b");	// sec.12c: was /I
		outTree->Branch("nmvtx"   ,fTnhitsmvt   ,"nmvtx[ntr]/b");
		outTree->Branch("nintt"   ,fTnhitsint   ,"nintt[ntr]/b");
		outTree->Branch("dcaxy"   ,fTdcaxy      ,"dcaxy[ntr]/F");
		outTree->Branch("dcaz"    ,fTdcaz       ,"dcaz[ntr]/F");
		outTree->Branch("dedx70n" ,fTdedx70n    ,"dedx70n[ntr]/b");		//  70% of the hits! - sec.12c: was /I
		outTree->Branch("dedx70s" ,fTdedx70s    ,"dedx70s[ntr]/F");		//  70% of the hits! - sec.12c: was /D
		outTree->Branch("dedxKFP" ,fTdedxKFP    ,"dedxKFP[ntr]/F");		//  TrackAnalysisUtils::calc_dedx() - see Collect.h - sec.12c: was /D
		outTree->Branch("layermask",fTlayermask ,"layermask[ntr]/l");	// bit per absolute TrkrDefs layer (0-2 MVTX, 3-6 INTT, 7-54 TPC) with a cluster on this track - lets outread_stream compute STAR-style SL itself for any pair, sibling or mixed (see README_SplitTracks.md sec. 8)
		outTree->Branch("siclukey" ,fTsiclukey   ,"siclukey[ntr][7]/l");	// raw silicon cluster key per absolute Si layer 0-6 (slot == layermask bit index); sentinel ~0ULL means no cluster on that layer (see README_SplitTracks.md sec. 12)
		outTree->Branch("tpcsector",fTtpcsector  ,"tpcsector[ntr][48]/b");	// TpcDefs sector*2+side per TPC layer (slot = layer-7, 0-47); 255 = no cluster (see README_SplitTracks.md sec. 12b)
		outTree->Branch("tpcarclen",fTtpcarclen  ,"tpcarclen[ntr][48]/B");	// local arc-length within that sector, 2.0 mm/count; valid only where tpcsector!=255
		outTree->Branch("tpcz"     ,fTtpcz       ,"tpcz[ntr][48]/S");	// global z, 0.1 mm/count; valid only where tpcsector!=255
		if (fAddTowersToTrackTree){
			cout<<"Collect::Init .. Adding calorimeter energy grids!"<<endl;
			outTree->Branch("stateTI_x" , fTstateTI_x ,"stateTI_x[ntr]/D");
			outTree->Branch("stateTI_y" , fTstateTI_y ,"stateTI_y[ntr]/D");
			outTree->Branch("stateTI_z" , fTstateTI_z ,"stateTI_z[ntr]/D");
			outTree->Branch("stateTO_x" , fTstateTO_x ,"stateTO_x[ntr]/D");
			outTree->Branch("stateTO_y" , fTstateTO_y ,"stateTO_y[ntr]/D");
			outTree->Branch("stateTO_z" , fTstateTO_z ,"stateTO_z[ntr]/D");
			outTree->Branch("stateEM_x" , fTstateEM_x ,"stateEM_x[ntr]/D");
			outTree->Branch("stateEM_y" , fTstateEM_y ,"stateEM_y[ntr]/D");
			outTree->Branch("stateEM_z" , fTstateEM_z ,"stateEM_z[ntr]/D");
			outTree->Branch("stateIH_x" , fTstateIH_x ,"stateIH_x[ntr]/D");
			outTree->Branch("stateIH_y" , fTstateIH_y ,"stateIH_y[ntr]/D");
			outTree->Branch("stateIH_z" , fTstateIH_z ,"stateIH_z[ntr]/D");
			outTree->Branch("stateOH_x" , fTstateOH_x ,"stateOH_x[ntr]/D");
			outTree->Branch("stateOH_y" , fTstateOH_y ,"stateOH_y[ntr]/D");
			outTree->Branch("stateOH_z" , fTstateOH_z ,"stateOH_z[ntr]/D");
			outTree->Branch("egridEM"   , fTegridEM   ,"egridEM[ntr][7][7]/D");
			outTree->Branch("egridIH"   , fTegridIH   ,"egridIH[ntr][7][7]/D");
			outTree->Branch("egridOH"   , fTegridOH   ,"egridOH[ntr][7][7]/D");
			outTree->Branch("eopEM"     , fTeopEM     ,"eopEM[ntr]/D");	// EMCal 7x7-sum / track p ("E/p"), -9 if not available
			outTree->Branch("eopHC"     , fTeopHC     ,"eopHC[ntr]/D");	// (IHCal+OHCal) 7x7-sum / track p, -9 if neither available
			outTree->Branch("eopCA"     , fTeopCA     ,"eopCA[ntr]/D");	// (EMCal+IHCal+OHCal) 7x7-sum / track p, -9 if none available
		}	// end addtowers check...
	}
	//
	if (TREEV0){
		cout<<"Collect::Init .. Enabled TREEVO"<<endl;
		outTree->Branch("nv0"       ,&fTnv0         ,"nv0/I");
		outTree->Branch("v0pid"     ,fTv0pid        ,"v0pid[nv0]/I");
		outTree->Branch("v0x"       ,fTv0x          ,"v0x[nv0]/D");
		outTree->Branch("v0y"       ,fTv0y          ,"v0y[nv0]/D");
		outTree->Branch("v0z"       ,fTv0z          ,"v0z[nv0]/D");
		outTree->Branch("v0px"      ,fTv0px         ,"v0px[nv0]/D");
		outTree->Branch("v0py"      ,fTv0py         ,"v0py[nv0]/D");
		outTree->Branch("v0pz"      ,fTv0pz         ,"v0pz[nv0]/D");
		outTree->Branch("v0pt"      ,fTv0pt         ,"v0pt[nv0]/D");
		outTree->Branch("v0ptot"    ,fTv0ptot       ,"v0ptot[nv0]/D");
		outTree->Branch("v0eta"     ,fTv0eta        ,"v0eta[nv0]/D");
		outTree->Branch("v0phi"     ,fTv0phi        ,"v0phi[nv0]/D");
		outTree->Branch("v0ene"     ,fTv0ene        ,"v0ene[nv0]/D");
		outTree->Branch("v0mass"    ,fTv0mass       ,"v0mass[nv0]/D");
		outTree->Branch("v0ctau"    ,fTv0ctau       ,"v0ctau[nv0]/D");
		outTree->Branch("v0decaylen",fTv0decaylen   ,"v0decaylen[nv0]/D");
		outTree->Branch("v0chi2ndf" ,fTv0chi2ndf    ,"v0chi2ndf[nv0]/D");
		outTree->Branch("v0dira"    ,fTv0dira       ,"v0dira[nv0]/D");	// cos(pointing angle) wrt PV
		outTree->Branch("v0pvdca"   ,fTv0pvdca      ,"v0pvdca[nv0]/D");	// V0 trajectory DCA to PV (cm)
		outTree->Branch("v0decayx"  ,fTv0decayx     ,"v0decayx[nv0]/D");	// daughter-pair decay vtx (what dira/pvdca use, NOT v0x/y/z)
		outTree->Branch("v0decayy"  ,fTv0decayy     ,"v0decayy[nv0]/D");
		outTree->Branch("v0decayz"  ,fTv0decayz     ,"v0decayz[nv0]/D");
		outTree->Branch("v0indtr1"  ,fTv0indtr1     ,"v0indtr1[nv0]/I");	// index to entry for track1 in Tracks OR PFLOW tree!
		outTree->Branch("v0indtr2"  ,fTv0indtr2     ,"v0indtr2[nv0]/I");	// index to entry for track2 in Tracks OR PFLOW tree!
	}
	//
	//	to get to EMCal:	pT >~ 0.20
	//	to get to IHCal:	pT >~ 0.25
	//	to get to OHCal:	pT >~ 0.36
	//
	PTLL	= 0.2;	// LL -- same track-quality floor regardless of fAddTowersToTrackTree, so track/v0 content doesn't depend on it
	cout<<"Collect::Init -- pT lower limit: PTLL= "<<PTLL<<endl;
	//
	//
	ntpcmin	= 20;	// LL
	nmvtmin	=  2;	// LL
	nintmin	=  1;	// LL
	cout<<"Collect::Init -- Ntpc  lower limit = "<<ntpcmin<<endl;
	cout<<"Collect::Init -- Nmvtx lower limit = "<<nmvtmin<<endl;
	cout<<"Collect::Init -- Nintt lower limit = "<<nintmin<<endl;
	//
	fPrimaryDCAxyLimit	= 1.5;	// cm
	fPrimaryDCAzLimit	= 1.5;	// cm
	cout<<"Collect::Init -- primary-flag DCA limits: xy="<<fPrimaryDCAxyLimit<<" z="<<fPrimaryDCAzLimit<<endl;
	//
	return Fun4AllReturnCodes::EVENT_OK;
	//
	//---- done init
}

//____________________________________________________________________________..
int Collect::InitRun(PHCompositeNode *topNode){
	std::cout << "Collect::InitRun -- Initializing for Run XXX" << std::endl;

	// crossing+distortion-corrected TPC global position (README_SplitTracks.md sec.12b) -
	// loads ActsGeometry + the Tpc distortion-correction-container nodes once per run.
	m_globalPositionWrapper.loadNodes(topNode);

	if (kVerbosity==0){ DEBUG = false; DEBUG2 = false; } else
	if (kVerbosity==1){ DEBUG = true;  DEBUG2 = false; } else
	if (kVerbosity>=2){ DEBUG = true;  DEBUG2 = true;  }

	//---- calo tower geometry comes from a Fun4AllRunNodeInputManager-supplied
	//---- Run node - not guaranteed populated yet at Init() time, but is by
	//---- InitRun() (called once the framework has synced to an actual run).
	RawTowerGeomContainer*	EMCalGeo = findNode::getClass<RawTowerGeomContainer>(topNode, "TOWERGEOM_CEMC");
	RawTowerGeomContainer*	IHCalGeo = findNode::getClass<RawTowerGeomContainer>(topNode, "TOWERGEOM_HCALIN");
	RawTowerGeomContainer*	OHCalGeo = findNode::getClass<RawTowerGeomContainer>(topNode, "TOWERGEOM_HCALOUT");
	fHasCaloGeo	= (EMCalGeo && IHCalGeo && OHCalGeo);
	if (!fHasCaloGeo){
		//---- no calo geometry available (e.g. running off a seed-level/tracking-only input) --
		//---- track + KFParticle v0 reconstruction still runs, just without the calo-layer-around-track info.
		std::cout<<"Collect::InitRun -- calorimeter geometry not found (EMCalGeo="<<(bool)EMCalGeo
				 <<" IHCalGeo="<<(bool)IHCalGeo<<" OHCalGeo="<<(bool)OHCalGeo
				 <<") -- running without calo-layer-around-track info"<<std::endl;
	} else {
		caloRadiusInnerEMCal 	= EMCalGeo->get_radius();		//  93.5    cm
		caloRadiusInnerIHCal 	= IHCalGeo->get_radius();		// 115.0    cm
		caloRadiusInnerOHCal 	= OHCalGeo->get_radius();		// 177.423  cm
		caloThicknessEMCal 		= EMCalGeo->get_thickness();	//  20.4997 cm
		caloThicknessIHCal 		= IHCalGeo->get_thickness();	//  25.005  cm
		caloThicknessOHCal 		= OHCalGeo->get_thickness();	//  96.894  cm
		caloRadiusOuterEMCal 	= caloRadiusInnerEMCal + caloThicknessEMCal;
		caloRadiusOuterIHCal 	= caloRadiusInnerIHCal + caloThicknessIHCal;
		caloRadiusOuterOHCal 	= caloRadiusInnerOHCal + caloThicknessOHCal;
		cout<<"Collect::InitRun -- caloRadiusInnerEMCal= "<<caloRadiusInnerEMCal<<endl;
		cout<<"Collect::InitRun -- caloRadiusOuterEMCal= "<<caloRadiusOuterEMCal<<endl;
		cout<<"Collect::InitRun -- caloRadiusInnerIHCal= "<<caloRadiusInnerIHCal<<endl;
		cout<<"Collect::InitRun -- caloRadiusOuterIHCal= "<<caloRadiusOuterIHCal<<endl;
		cout<<"Collect::InitRun -- caloRadiusInnerOHCal= "<<caloRadiusInnerOHCal<<endl;
		cout<<"Collect::InitRun -- caloRadiusOuterOHCal= "<<caloRadiusOuterOHCal<<endl;
		caloRadiusMidEMCal 	= caloRadiusInnerEMCal + caloThicknessEMCal/2.;
		caloRadiusMidIHCal 	= caloRadiusInnerIHCal + caloThicknessIHCal/2.;
		caloRadiusMidOHCal 	= caloRadiusInnerOHCal + caloThicknessOHCal/2.;
		cout<<"Collect::InitRun -- caloRadiusMidEMCal= "<<caloRadiusMidEMCal<<endl;
		cout<<"Collect::InitRun -- caloRadiusMidIHCal= "<<caloRadiusMidIHCal<<endl;
		cout<<"Collect::InitRun -- caloRadiusMidOHCal= "<<caloRadiusMidOHCal<<endl;
	}

	return Fun4AllReturnCodes::EVENT_OK;
}

// ============================================================================
// DIAGTEMP-20260920: throwaway merged-track exploration (README_SplitTracks.md
// sec.12b discussion, 2026-09-20). NOT for commit - every block tagged
// DIAGTEMP-20260920 below (and in process_event()/End()) should be deleted
// once this exploration is done. Goal: (1) how often does a literal TPC
// TrkrDefs::cluskey get claimed by 2+ tracks in the same event, and (2) does
// per-cluster (untruncated) adc show a second population near ~2x the bulk
// peak for tracks in a broad, charge-agnostic kinematic-proximity window (the
// STAR "double-pion dE/dx band" the user described from 30 years at STAR).
// ============================================================================
struct DiagTempClusterInfo20260920 { int layer; float adc; TrkrDefs::cluskey key; float gx; float gy; float gz; };
static TH1D* sDiagAdcAll20260920        = nullptr;	// every TPC cluster's normalized adc, all tracks
static TH1D* sDiagAdcSharedKey20260920  = nullptr;	// adc of clusters whose key is claimed by 2+ tracks this event
static TH1D* sDiagAdcClosePair20260920  = nullptr;	// adc of clusters on a shared occupied layer between a broad-window close pair
static TH1D* sDiagDist20260920          = nullptr;	// DIAGTEMP-20260920 v2: 3D global-position separation (cm) between same-layer cluster pairs of a close track pair
static TH2D* sDiagAdcVsDist20260920     = nullptr;	// DIAGTEMP-20260920 v2: adc vs that separation - the real STAR-style FMH-adjacent test
static TProfile* sDiagGapRateVsDist20260920 = nullptr;	// DIAGTEMP-20260920 v4: rate of a layermask hit-presence asymmetry on L+-1 vs the L-separation of the pair
static long  sDiagNTracks20260920       = 0;
static long  sDiagNClusters20260920     = 0;
static long  sDiagNSharedKeyInstances20260920 = 0;	// (track,cluster) instances where the key is also claimed elsewhere
static long  sDiagNEventsWithSharedKey20260920 = 0;
static long  sDiagNClosePairsChecked20260920   = 0;
static long  sDiagNClosePairsSharedKeyToo20260920 = 0;
static long  sDiagNGapChecksClose20260920 = 0;	// DIAGTEMP-20260920 v4: neighbor-layer checks where the confirming pair's dist3d<2cm
static long  sDiagNGapAsymClose20260920   = 0;
static long  sDiagNGapChecksFar20260920   = 0;	// dist3d>10cm control
static long  sDiagNGapAsymFar20260920     = 0;
// DIAGTEMP-20260920 v5: is TpcDefs::getSectorId/getSide(key) - decodable from the raw
// cluster key with zero position data, exactly like siclukey's design - a good enough
// proxy for "these two clusters are really close" to be usable downstream without
// persisting full 3D position? Test it against the real dist3d we already compute.
static TH1D* sDiagDistSameSector20260920 = nullptr;
static TH1D* sDiagDistDiffSector20260920 = nullptr;
static TProfile* sDiagGapRateVsDistSameSector20260920 = nullptr;
static TProfile* sDiagGapRateVsDistDiffSector20260920 = nullptr;

// DIAGTEMP-20260920: per-vertex analysis, called right after FlagSplitTracks()
// so it sees the same fNTracks/fTeta/fTphi/fTchg this vertex just filled.
static void DiagMergedTracksExplore20260920(
		int nTracks, const uint64_t* tlayermask,
		const std::vector<std::vector<DiagTempClusterInfo20260920>>& clustersByTrack,
		const std::map<TrkrDefs::cluskey,std::vector<int>>& keyOwners){
	//
	if (!sDiagAdcAll20260920){
		sDiagAdcAll20260920       = new TH1D("hDiagAdcAll20260920","DIAGTEMP adc, all TPC clusters;normalized adc",300,0,6000);
		sDiagAdcSharedKey20260920 = new TH1D("hDiagAdcSharedKey20260920","DIAGTEMP adc, shared-key clusters;normalized adc",300,0,6000);
		sDiagAdcClosePair20260920 = new TH1D("hDiagAdcClosePair20260920","DIAGTEMP adc, close-pair shared-layer clusters;normalized adc",300,0,6000);
		sDiagDist20260920         = new TH1D("hDiagDist20260920","DIAGTEMP 3D separation, same-layer clusters of a close track pair;cm",300,0,30);
		sDiagAdcVsDist20260920    = new TH2D("hDiagAdcVsDist20260920","DIAGTEMP adc vs 3D separation;cm;normalized adc",150,0,30,300,0,6000);
		sDiagGapRateVsDist20260920 = new TProfile("hDiagGapRateVsDist20260920","DIAGTEMP L+-1 hit-presence asymmetry rate vs 3D separation;cm;asymmetry rate",150,0,30);
		sDiagAdcAll20260920->SetDirectory(0);
		sDiagAdcSharedKey20260920->SetDirectory(0);
		sDiagAdcClosePair20260920->SetDirectory(0);
		sDiagDist20260920->SetDirectory(0);
		sDiagAdcVsDist20260920->SetDirectory(0);
		sDiagGapRateVsDist20260920->SetDirectory(0);
		sDiagDistSameSector20260920 = new TH1D("hDiagDistSameSector20260920","DIAGTEMP dist3d, TpcDefs sector+side match;cm",300,0,30);
		sDiagDistDiffSector20260920 = new TH1D("hDiagDistDiffSector20260920","DIAGTEMP dist3d, TpcDefs sector+side differ;cm",300,0,30);
		sDiagGapRateVsDistSameSector20260920 = new TProfile("hDiagGapRateVsDistSameSector20260920","DIAGTEMP gap rate vs dist, sector+side match;cm;rate",150,0,30);
		sDiagGapRateVsDistDiffSector20260920 = new TProfile("hDiagGapRateVsDistDiffSector20260920","DIAGTEMP gap rate vs dist, sector+side differ;cm;rate",150,0,30);
		sDiagDistSameSector20260920->SetDirectory(0);
		sDiagDistDiffSector20260920->SetDirectory(0);
		sDiagGapRateVsDistSameSector20260920->SetDirectory(0);
		sDiagGapRateVsDistDiffSector20260920->SetDirectory(0);
	}
	//
	bool eventHasSharedKey = false;
	for (int it=0; it<nTracks; it++){
		sDiagNTracks20260920++;
		for (const auto &ci : clustersByTrack[it]){
			sDiagNClusters20260920++;
			sDiagAdcAll20260920->Fill(ci.adc);
			auto ownIt = keyOwners.find(ci.key);
			if (ownIt!=keyOwners.end() && ownIt->second.size()>=2){
				sDiagNSharedKeyInstances20260920++;
				sDiagAdcSharedKey20260920->Fill(ci.adc);
				eventHasSharedKey = true;
			}
		}
	}
	if (eventHasSharedKey) sDiagNEventsWithSharedKey20260920++;
	//
	// DIAGTEMP-20260920 v3: NO kinematic (deta,dphi) pre-gate here anymore - v1/v2
	// copied the split-track gate, which assumes one real particle refit twice
	// (so its fragments share nearly the same trajectory everywhere). Two merged
	// tracks are two different particles that can be close at one radius and
	// diverge elsewhere in eta/phi - a global kinematic gate would (and, per the
	// v2 distance histogram, apparently did) throw away most of the real
	// candidate population before it ever reached the per-layer position check.
	// Scan every same-event pair directly instead; with ~4-5 tracks/vertex here
	// this is trivially cheap (O(N^2 x 48 layers)), so no shortcut is needed.
	for (int i=0;i<nTracks;i++){
		for (int j=i+1;j<nTracks;j++){
			sDiagNClosePairsChecked20260920++;
			bool pairSharesKeyToo = false;
			// index clusters of j by layer for a quick lookup
			std::map<int,const DiagTempClusterInfo20260920*> jByLayer;
			for (const auto &cj : clustersByTrack[j]) jByLayer[cj.layer] = &cj;
			for (const auto &ci : clustersByTrack[i]){
				auto itj = jByLayer.find(ci.layer);
				if (itj==jByLayer.end()) continue;
				sDiagAdcClosePair20260920->Fill(ci.adc);
				sDiagAdcClosePair20260920->Fill(itj->second->adc);
				if (ci.key==itj->second->key) pairSharesKeyToo = true;
					// DIAGTEMP-20260920 v2: real 3D separation between the two same-layer clusters
					double ddx = ci.gx - itj->second->gx;
					double ddy = ci.gy - itj->second->gy;
					double ddz = ci.gz - itj->second->gz;
	double dist3d = sqrt(ddx*ddx + ddy*ddy + ddz*ddz);
					sDiagDist20260920->Fill(dist3d);
					sDiagAdcVsDist20260920->Fill(dist3d, ci.adc);
					sDiagAdcVsDist20260920->Fill(dist3d, itj->second->adc);
					// DIAGTEMP-20260920 v5: TpcDefs::getSectorId/getSide decode from the raw key
					// alone (no position needed) - is "same sector+side" a good enough proxy for
					// "really close" to be usable downstream from a persisted key, instead of
					// needing full 3D position (much bigger branch, see README sec.12c size debate)?
					bool sameSector = (TpcDefs::getSectorId(ci.key)==TpcDefs::getSectorId(itj->second->key)) &&
									   (TpcDefs::getSide(ci.key)==TpcDefs::getSide(itj->second->key));
					if (sameSector){ sDiagDistSameSector20260920->Fill(dist3d); }
					else            { sDiagDistDiffSector20260920->Fill(dist3d); }
					// DIAGTEMP-20260920 v4: does a neighboring TPC layer show a hit-presence
					// asymmetry (one track has it, the other doesn't) right next to this
					// confirmed close encounter? User's hypothesis: with hit assignment
					// apparently globally exclusive (zero shared keys, section 12 chat
					// 2026-09-20), two genuinely close real tracks can't double-count a
					// contested cluster the way STAR's tracker tolerated - the "loser"
					// just goes missing at that padrow instead. Test on L-1/L+1 (not L
					// itself, which by construction has a hit for both here).
					for (int dl : {-1, 1}){
						int nl = ci.layer + dl;
						if (nl<7 || nl>54) continue;
						bool hitI = (tlayermask[i] >> nl) & 1ULL;
						bool hitJ = (tlayermask[j] >> nl) & 1ULL;
						bool asym = (hitI != hitJ);
						sDiagGapRateVsDist20260920->Fill(dist3d, asym?1.0:0.0);
						if (dist3d<2.0){ sDiagNGapChecksClose20260920++; if (asym) sDiagNGapAsymClose20260920++; }
						if (dist3d>10.0){ sDiagNGapChecksFar20260920++;  if (asym) sDiagNGapAsymFar20260920++;  }
						if (sameSector){ sDiagGapRateVsDistSameSector20260920->Fill(dist3d, asym?1.0:0.0); }
						else            { sDiagGapRateVsDistDiffSector20260920->Fill(dist3d, asym?1.0:0.0); }
					}
			}
			if (pairSharesKeyToo) sDiagNClosePairsSharedKeyToo20260920++;
		}
	}
}
// ============================================================================
// end DIAGTEMP-20260920 declarations
// ============================================================================

//____________________________________________________________________________..
int Collect::process_event(PHCompositeNode *topNode){

	bool SHOWGRIDS = false;		// show calorimeter energy grids if DEBUG=true

	++ievtSeen;

	//---- get some collections...
	//
 	auto trackmap 		= findNode::getClass<SvtxTrackMap>(topNode, "SvtxTrackMap");
	auto clustermap 	= findNode::getClass<TrkrClusterContainer>(topNode, "TRKR_CLUSTER");
//?	auto globvertexmap	= findNode::getClass<GlobalVertexMap>(topNode, "GlobalVertexMap");
	auto svtxvertexmap	= findNode::getClass<SvtxVertexMap>(topNode, "SvtxVertexMap");
	auto tpcGeom		= findNode::getClass<PHG4TpcGeomContainer>(topNode, "TPCGEOMCONTAINER");
	auto geometry 		= findNode::getClass<ActsGeometry>(topNode, "ActsGeometry");
	//auto mvtxGeom		= findNode::getClass<PHG4CylinderGeomContainer>(topNode, "CYLINDERGEOM_MVTX");
	//auto inttGeom 	= findNode::getClass<PHG4CylinderGeomContainer>(topNode, "CYLINDERGEOM_INTT");
	//auto mmGeom		= findNode::getClass<PHG4CylinderGeomContainer>(topNode, "CYLINDERGEOM_MICROMEGAS_FULL");
	//
	TowerInfoContainer* towersEM	= findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_CALIB_CEMC");	//
	TowerInfoContainer* towersIH	= findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_CALIB_HCALIN");
	TowerInfoContainer* towersOH	= findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_CALIB_HCALOUT");
    RawTowerGeomContainer* EMCalGeo	= findNode::getClass<RawTowerGeomContainer>(topNode, "TOWERGEOM_CEMC");
    RawTowerGeomContainer* IHCalGeo	= findNode::getClass<RawTowerGeomContainer>(topNode, "TOWERGEOM_HCALIN");
    RawTowerGeomContainer* OHCalGeo	= findNode::getClass<RawTowerGeomContainer>(topNode, "TOWERGEOM_HCALOUT");	

	
	//---- header info...
	EventHeader *evtHeader = findNode::getClass<EventHeader>(topNode, "EventHeader");
	//int bunch		= -1;
	//float epang	= 0;
	if (!evtHeader){
		if (DEBUG) std::cout << "Collect::process_event -- no EventHeader... " << std::endl;
		fRunNum	= -1;	// flag rather than silently keep the previous event's run/evt numbers
		fEvtSeq	= -1;
	} else {
		fRunNum	= evtHeader->get_RunNumber();
		fEvtSeq	= evtHeader->get_EvtSequence();
		//if (DEBUG) cout<<fRunNum<<" "<<fEvtSeq<<endl;
	}
	if (fEvtSeqFirst<0){
		fEvtSeqFirst	= fEvtSeq;
		cout<<"Collect::process_event -- fEvtSeqFirst= "<<fEvtSeqFirst<<"\t ievtSeen= "<<ievtSeen<<endl;	
	}

//	if (fEvtSeq==11||fEvtSeq==13||fEvtSeq==15){ DEBUG = true; } else { DEBUG = false; }
	if (DEBUG) cout<<"COLLECT START ------------- \t fEvtSeq-0= "<<fEvtSeq-fEvtSeqFirst<<"\t ievtSeen= "<<ievtSeen<<endl;

	//---- Grab the GL1 data
	Gl1Packet* gl1	= findNode::getClass<Gl1Packet>(topNode, "GL1RAWHIT");
 	if (!gl1){
		if (DEBUG) std::cout<<"Collect::process_event -- no GL1RAWHIT..."<<std::endl;
		fTtriggerVector.clear();	// flag rather than silently keep the previous event's trigger vector
	} else {
		//uint64_t evtseq     = gl1->getEvtSequence();
		uint64_t triggervec = gl1->getScaledVector();
 		fTtriggerVector.clear();
 		for (int i = 0; i < 64; i++){
 			bool trig_decision = ((triggervec & 0x1U) == 0x1U);
 			if (DEBUG){ if (trig_decision){ cout<<"1"; }else{ cout<<"0"; } }
 			fTtriggerVector.push_back(trig_decision);
 			triggervec = (triggervec >> 1U) & 0xffffffffU;
 		}	if (DEBUG){ cout<<endl; }
	}

	//---- KFParticle area...
	//
	nv0tf	= 0;	// number of V0's total in this TRIGGER FRAME across all three V0 species
	int NfoundKS	= 0;
	int NfoundLA	= 0;
	int NfoundAL	= 0;
	//
	NfoundKS = GetKFP( topNode, Kshort_PDGID,  "KFPrecoKS_KFParticle_Container", "KFPrecoKS_SvtxTrackMap" );
	NfoundLA = GetKFP( topNode, Lambda_PDGID,  "KFPrecoLA_KFParticle_Container", "KFPrecoLA_SvtxTrackMap" );
	NfoundAL = GetKFP( topNode, ALambda_PDGID, "KFPrecoAL_KFParticle_Container", "KFPrecoAL_SvtxTrackMap" );
	//
	sumNfoundKS+=NfoundKS;  sumNfoundLA+=NfoundLA;  sumNfoundAL+=NfoundAL;
	if ((fEvtSeq-fEvtSeqFirst)%10==0) std::cout << "processing event " << fEvtSeq << " (" << fEvtSeq-fEvtSeqFirst << ") from run " << fRunNum
					<< ".  SUM Nk,l,a: " << sumNfoundKS << " " << sumNfoundLA << " " << sumNfoundAL << std::endl;
	bool v0found	= false;
	if (NfoundKS||NfoundLA||NfoundAL) v0found = true;
	if (v0found && DEBUG){
		std::cout	<<"KFP Done... nv0 = "<<fTnv0
					//<<"\t Nk,l,a= "<<NfoundKS<<" "<<NfoundLA<<" "<<NfoundAL
					<<"\t SUM Nk,l,a= "<<sumNfoundKS<<" "<<sumNfoundLA<<" "<<sumNfoundAL
					<<endl;
	}
	//
	//---- reject any v0's that share the same daughter tracks! (in the same crossing obvs) 
	//----	pick the v0 with the lower chi2/ndf...
	if (nv0tf>1){
		for (int iv0=0;iv0<nv0tf;iv0++){
			if (fTv0TFuseit[iv0]){
				unsigned int trkid1	= V0Track1id[iv0];
				unsigned int trkid2	= V0Track2id[iv0];
				for (int jv0=0;jv0<iv0;jv0++){
					if (fTv0TFuseit[jv0]){
						if ( V0Crossing[iv0]==V0Crossing[jv0]	// make sure these two V0s are even in the same crossing!
						 && ( trkid1 == V0Track1id[jv0] ||		//    is trk1 duplicated in 2 different V0s? 
						      trkid2 == V0Track2id[jv0]) ){		// OR is trk2 duplicated in 2 different V0s?
							double chi2i	= fTv0TFchi2ndf[iv0];
							double chi2j	= fTv0TFchi2ndf[jv0];
							if (chi2i>=chi2j){ fTv0TFuseit[iv0] = false; } else
							if (chi2i< chi2j){ fTv0TFuseit[jv0] = false; }
							if (DEBUG) cout<<"V0 DAUGHTER TRACK DUPLICATED .. chi2: "<<chi2i<<" "<<chi2j
								<<"\t iv0: "<<trkid1<<" "<<trkid2<<"\t jv0: "<<V0Track1id[jv0]<<" "<<V0Track2id[jv0]
								<<endl;
						}	// end check of track ids used in these two V0s...
					}	// end check use jv0...
				}	// end loop jv0...
			}	// end check use iv0...
		}	// end loop iv0...
	}	// end check of >1 V0
	//
	//---- end KFP....


	//---- calorimeter info...
	//
	fTTotE_em		= 0.;
	fTTotE_ih		= 0.;
	fTTotE_oh		= 0.;
	fTTotE_ioh		= 0.;
	fTTotE_epd		= 0.;
	double TOTE_em	= 0.;
	double TOTE_ih	= 0.;
	double TOTE_oh	= 0.;
	double TOTE_ioh	= 0.;
	double TOTE_epd	= 0.;
	//
	double calophishift	= 2.0*M_PI/64.;
	//
	if(towersEM){
		for (unsigned int i=0; i<towersEM->size(); i++){			// i is towerIndex...
			TowerInfo* towerInfo	= towersEM->get_tower_at_channel(i);
			double etow				= towerInfo->get_energy();
			if (CALODETAIL){
				unsigned int towkey		= towersEM->encode_key(i);
				int ieta     = towersEM->getTowerEtaBin(towkey);
				int iphi     = towersEM->getTowerPhiBin(towkey);
				double eta=-9,phi=-9;
				if (ievtSeen<MAXEVTDETAIL){
					RawTowerDefs::keytype key = RawTowerDefs::encode_towerid(RawTowerDefs::CalorimeterId::CEMC, ieta, iphi);
					RawTowerGeom* towergeom = EMCalGeo->get_tower_geometry(key);
					int iphit=-9;
					if (towergeom){
						eta		= towergeom->get_eta();				// eta-DET
						phi		= towergeom->get_phi();				// phi-DET
						double phib		= phi + calophishift;		// shift is 4*bw
						if (phib< 0.     ) phib += 2.*M_PI;
						if (phib>=2.*M_PI) phib -= 2.*M_PI;
						double phibw	= 2.0*M_PI/256.;
						iphit			= phib/phibw;
						hmapEM_eta	->Fill(eta,ieta);
						hmapEM_phi	->Fill(phi,iphi);
		// 				hmapEM_eta2	->Fill(eta,ietat-ieta);
						hmapEM_phi2	->Fill(phi,iphit-iphi);
						hmapEM_ieta	->Fill(ieta,eta);
						hmapEM_iphi	->Fill(iphi,phib);
					} 
				}
				hEevt_em[ieta][iphi]			->Fill(fEvtSeq,etow);
				if (ievtSeen<MAXEVTDETAIL){
					hlayer[0][ievtSeen]			->Fill(ieta,iphi,etow);
					hlayer_efDET[0][ievtSeen]	->Fill( eta, phi,etow);
				}
				if (etow>=-THRESH_em&&etow<THRESH_em){ 
					if (CALODETAIL) hPedestalEM	->Fill(eta,phi,etow);	
				}
			}	// end CALODETAIL
			if (etow>=THRESH_em){ 
				TOTE_em 	+= etow;
			}
			hech_em		->Fill(etow);
//			delete towergeom;
		}	// end tower loop
	}	// end towersEM
	//
	if(towersIH){
		for(unsigned int i = 0; i < towersIH->size(); i++){
			TowerInfo* towerInfo	= towersIH->get_tower_at_channel(i);
			double etow	 			= towerInfo->get_energy();
			if (CALODETAIL){
				unsigned int towkey		= towersIH->encode_key(i);
				int ieta     = towersIH->getTowerEtaBin(towkey);
				int iphi     = towersIH->getTowerPhiBin(towkey);
				double eta=-9,phi=-9;
				if (ievtSeen<MAXEVTDETAIL){
					RawTowerDefs::keytype key = RawTowerDefs::encode_towerid(RawTowerDefs::CalorimeterId::HCALIN, ieta, iphi);
					RawTowerGeom* towergeom = IHCalGeo->get_tower_geometry(key);
					int iphit=-9;
					if (towergeom){
						eta		= towergeom->get_eta();				// eta-DET
						phi		= towergeom->get_phi();				// phi-DET
						double phib		= phi + calophishift;		// shift is 1*bw
						if (phib< 0.     ) phib += 2.*M_PI;
						if (phib>=2.*M_PI) phib -= 2.*M_PI;
						double phibw	= 2.0*M_PI/64.;
						iphit			= phib/phibw;
						//cout<<eta<<" "<<ieta<<" "<<ietat<<"\t "<<phi<<" "<<iphi<<" "<<iphit<<endl;
						hmapIH_eta	->Fill(eta,ieta);
						hmapIH_phi	->Fill(phi,iphi);
		// 				hmapIH_eta2	->Fill(eta,ietat-ieta);
						hmapIH_phi2	->Fill(phi,iphit-iphi);
						hmapIH_ieta	->Fill(ieta,eta);
						hmapIH_iphi	->Fill(iphi,phib);
					} 
				}
				hEevt_ih[ieta][iphi]			->Fill(fEvtSeq,etow);
				if (ievtSeen<MAXEVTDETAIL){
					hlayer[1][ievtSeen]			->Fill(ieta,iphi,etow);
					hlayer_efDET[1][ievtSeen]	->Fill( eta, phi,etow);
				}
				if (etow>=-THRESH_ih&&etow<THRESH_ih){ 
					if (CALODETAIL) hPedestalIH	->Fill(eta,phi,etow);	
				}
			}	// end CALODETAIL
			if (etow>=THRESH_ih){ 
				TOTE_ih 	+= etow;
			}
			hech_ih		->Fill(etow);
			//delete towergeom;
		}	// end tower loop
	}	// end towersEM
	//
	if(towersOH){
		for(unsigned int i = 0; i < towersOH->size(); i++){
			TowerInfo* towerInfo	= towersOH->get_tower_at_channel(i);
			double etow	 			= towerInfo->get_energy();
			if (CALODETAIL){
				unsigned int towkey		= towersOH->encode_key(i);
				int ieta     = towersOH->getTowerEtaBin(towkey);
				int iphi     = towersOH->getTowerPhiBin(towkey);
				double eta=-9,phi=-9;
				if (ievtSeen<MAXEVTDETAIL){
					RawTowerDefs::keytype key = RawTowerDefs::encode_towerid(RawTowerDefs::CalorimeterId::HCALOUT, ieta, iphi);
					RawTowerGeom* towergeom = OHCalGeo->get_tower_geometry(key);
					int iphit=-9;
					if (towergeom){
						eta		= towergeom->get_eta();			// eta-DET
						phi		= towergeom->get_phi();			// phi-DET
						double phib		= phi + calophishift;	// shift is 1*bw
						if (phib< 0.     ) phib += 2.*M_PI;
						if (phib>=2.*M_PI) phib -= 2.*M_PI;
						double phibw	= 2.0*M_PI/64.;
						iphit			= phib/phibw;
						//cout<<eta<<" "<<ieta<<" "<<ietat<<"\t "<<phi<<" "<<iphi<<" "<<iphit<<endl;
						hmapOH_eta	->Fill(eta,ieta);
						hmapOH_phi	->Fill(phi,iphi);
		// 				hmapOH_eta2	->Fill(eta,ietat-ieta);
						hmapOH_phi2	->Fill(phi,iphit-iphi);
						hmapOH_ieta	->Fill(ieta,eta);
						hmapOH_iphi	->Fill(iphi,phib);
					} 
				}
				hEevt_oh[ieta][iphi]			->Fill(fEvtSeq,etow);
				if (ievtSeen<MAXEVTDETAIL){
					hlayer[2][ievtSeen]			->Fill(ieta,iphi,etow);
					hlayer_efDET[2][ievtSeen]	->Fill( eta, phi,etow);
				}
				if (etow>=-THRESH_oh&&etow<THRESH_oh){
					if (CALODETAIL) hPedestalOH	->Fill(eta,phi,etow);
				}
			}	// end CALODETAIL
			if (etow>=THRESH_oh){
				TOTE_oh 	+= etow;
			}
			hech_oh		->Fill(etow);
			//delete towergeom;
		}	// end tower loop
	}	// end towersEM
	TOTE_ioh	= TOTE_ih + TOTE_oh;
	//
	//---- sepd info...
	TowerInfoContainer* epdtowerinfo	= findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_CALIB_SEPD");
	EpdGeom* epdgeom 					= findNode::getClass<EpdGeom>(topNode, "TOWERGEOM_EPD");
 	if ( !epdtowerinfo || !epdgeom ){	// require both!
		// EPD not present in this dataset - silently skip, not an error.
	} else {
		TowerInfo *towerInfo = nullptr;
		for(unsigned int i=0; i<epdtowerinfo->size(); i++){
			towerInfo = epdtowerinfo->get_tower_at_channel(i);
			double etow	 = towerInfo->get_energy();
			if (etow>=THRESH_ep) TOTE_epd 	+= etow;
			hech_ep		->Fill(etow);
		}
		//
 		unsigned int ntowers = epdtowerinfo->size();
 		if (DEBUG){ cout<<"epd ntowers = "<<ntowers<<" tote_epd = "<<TOTE_epd<<endl; }
		//
	}
	//
	if (DEBUG) cout<<"Eemc="<<TOTE_em<<"\t Eih="<<TOTE_ih<<"\t Eoh="<<TOTE_oh<<"\t Eepd="<<TOTE_epd<<endl;

	//
	std::vector<int> Xings;
	Xings.clear();				// is this needed?????
	//

	//---- loop over tracking vertices (from tpc/silicon)
	//
	int	nverticeskept	= 0;
	if (svtxvertexmap){
		fTnvtx		= svtxvertexmap->size();		// number of vertices found in this event
		hnvtx_frame	->Fill(fEvtSeq-fEvtSeqFirst,fTnvtx);
		if (DEBUG) cout<<"----- Vertex NVERT = "<<fTnvtx<<endl;
		//
		//---- Protect against multiple vertices sharing the same crossing (real
		//     pileup, or a split-vertex reconstruction artifact - either way,
		//     giving both their own row double-counts every track/v0 whose
		//     crossing matches, since the track/v0 inclusion below is
		//     crossing-based, not vertex-based - see the collect_v0_track_index_bug
		//     memory, "vertex-row duplication" finding, for how this was found).
		//     Keep only the vertex with the most attached tracks per crossing.
		//     Independent of fRequireXingZero, which is a separate decision
		//     (whether to look at crossing zero only) - this generalizes what
		//     used to be a crossing-zero-only protection to every crossing.
		std::map<int, unsigned int> vertexIdKeepByXing;	// crossing -> vertex id with the most attached tracks
		std::map<int, unsigned int> maxNtrkByXing;			// crossing -> that vertex's track count
		for (const auto& [key, vertex] : *svtxvertexmap){	// loop over vertices...
			int          m_vertex_crossing	= vertex->get_beam_crossing();
			// Hotness metric (2026-09-18): count tracks in this crossing with
			// get_vertex_id()==key AND 3D DCA to this vertex within
			// fPrimaryDCAxyLimit/fPrimaryDCAzLimit - the same "primary" test
			// applied below, NOT gated by the ntpc/nmvtx/nintt/pT track-quality
			// floor used for tree inclusion. Author request: a genuinely busy
			// real vertex can have many real but lower-quality tracks attached,
			// and should not lose the "hottest" pick to a vertex with fewer but
			// higher-quality tracks just because of that floor. Replaces
			// vertex->size_tracks(), an opaque vertex-finder-internal count we
			// do not otherwise use or trust anywhere else in this file.
			unsigned int ntrk = 0;
			if (trackmap){
				for (auto &iter2 : *trackmap){
					SvtxTrack *trk2	= iter2.second;
					if (!trk2) continue;
					if (trk2->get_crossing()  != m_vertex_crossing) continue;
					if (trk2->get_vertex_id() != key) continue;
					double dxy=NAN, dxyerr=NAN, dz=NAN, dzerr=NAN;
					calc3DDCA(trk2, svtxvertexmap, key, dxy, dxyerr, dz, dzerr);
					if (fabs(dxy)<=fPrimaryDCAxyLimit && fabs(dz)<=fPrimaryDCAzLimit) ntrk++;
				}
			}
			auto it = maxNtrkByXing.find(m_vertex_crossing);
			if (it==maxNtrkByXing.end() || ntrk>it->second){
				maxNtrkByXing[m_vertex_crossing]		= ntrk;
				vertexIdKeepByXing[m_vertex_crossing]	= key;
			}
		}
		//
		for (const auto& [key, vertex] : *svtxvertexmap){	// loop over vertices...
			unsigned int m_vertexid 	= key;
			int    m_vertex_crossing 	= vertex->get_beam_crossing();
			//
			if (fRequireXingZero && m_vertex_crossing!=0) continue;		// keep only crossing zero if requested
			if (m_vertexid != vertexIdKeepByXing[m_vertex_crossing]){
				if (DEBUG) cout<<"Skipping vertex ID= "<<m_vertexid<<"\t crossing= "<<m_vertex_crossing
							<<"\t Ntracks= "<<vertex->size_tracks()
							<<" (keeping ID= "<<vertexIdKeepByXing[m_vertex_crossing]<<" instead)"<<endl;
				continue;		// not the hottest vertex at this crossing - skip to avoid double-counting
			}
			//
			Xings.push_back(m_vertex_crossing);
			double m_vx 				= vertex->get_x();
			double m_vy 				= vertex->get_y();
			double m_vz 				= vertex->get_z();
			int	   m_ntracks			= maxNtrkByXing[m_vertex_crossing];	// DCA-based hotness count, not vertex->size_tracks() (see above)
			if (DEBUG) cout	<<"-----   vtx: "<<m_vertexid<<" crossing="<<m_vertex_crossing
							<<" x="<<m_vx<<" y="<<m_vy<<" z="<<m_vz
							<<"\t ntrkattached = "<<m_ntracks<<" ------ "<<endl;
			fTvtxntr			= m_ntracks;			// to outTree... total number of tracks assigned to chosen vertex
			fTvtxx				= m_vx;					// to outTree... PV x 
			fTvtxy				= m_vy;					// to outTree... PV y
			fTvtxz				= m_vz;					// to outTree... PV z
			fTcrossing			= m_vertex_crossing;
			if (m_vertex_crossing == 0){
				fTTotE_em		= TOTE_em;
				fTTotE_ih		= TOTE_ih;
				fTTotE_oh		= TOTE_oh;
				fTTotE_ioh		= TOTE_ioh;
				fTTotE_epd		= TOTE_epd;
			} else {
				fTTotE_em=fTTotE_ih=fTTotE_oh=fTTotE_ioh=fTTotE_epd=-1.0;
			}
//?			fTvtxzGlob	= Crossing2GlobVtxZ[m_vertex_crossing];
			//
			fNTracks	= 0;
			fTnv0		= 0;
			std::vector<std::vector<DiagTempClusterInfo20260920>> diagClustersByTrack20260920;	// DIAGTEMP-20260920
			std::map<TrkrDefs::cluskey,std::vector<int>> diagKeyOwners20260920;					// DIAGTEMP-20260920
			for (int it=0;it<MAXNTRACKS;it++){	//---- before track loop, zero arrays for towers near tracks...
				//for (int idphi=-3;idphi<=3;idphi++){
				//	for (int ideta=-3;ideta<=3;ideta++){
				//		fTegridEM[it][3+ideta][3+idphi]	= 0.;
				//		fTegridIH[it][3+ideta][3+idphi]	= 0.;
				//		fTegridOH[it][3+ideta][3+idphi]	= 0.;
				//	}
				//}
				fTstateTI_x[it] = fTstateTO_x[it] = 0.; 
				fTstateTI_y[it] = fTstateTO_y[it] = 0.; 
				fTstateTI_z[it] = fTstateTO_z[it] = 0.; 
				fTstateEM_x[it] = fTstateIH_x[it] = fTstateOH_x[it] = 0.; 
				fTstateEM_y[it] = fTstateIH_y[it] = fTstateOH_y[it] = 0.; 
				fTstateEM_z[it] = fTstateIH_z[it] = fTstateOH_z[it] = 0.; 
				fTegridEMsum7x7[it] = fTegridIHsum7x7[it] = fTegridOHsum7x7[it] = 0.;
				fTeopEM[it] = fTeopIH[it] = fTeopOH[it] = -9.;
			}
			//
			//---- find the V0s in this crossing.
			if (DEBUG2) cout<<"finding v0s in this crossing:  nv0tf= "<<nv0tf<<endl;
			std::vector<int> trk1inds;
			std::vector<int> trk2inds;
			for (int iv0=0;iv0<nv0tf;iv0++){
				if (m_vertex_crossing	== V0Crossing[iv0]	// take V0s from /this/ crossing
				 && fTv0TFuseit[iv0]    ){					// & make sure these V0s have distinct daughters!
					fTv0pid[fTnv0]		= fTv0TFpid[iv0];
					fTv0x[fTnv0]		= fTv0TFx[iv0];
					fTv0y[fTnv0]		= fTv0TFy[iv0];
					fTv0z[fTnv0]		= fTv0TFz[iv0];
					fTv0px[fTnv0]		= fTv0TFpx[iv0];
					fTv0py[fTnv0]		= fTv0TFpy[iv0];
					fTv0pz[fTnv0]		= fTv0TFpz[iv0];
					fTv0pt[fTnv0]		= fTv0TFpt[iv0];
					fTv0ptot[fTnv0]		= fTv0TFptot[iv0];
					{	// DIRA & PV_DCA: daughter-pair decay vtx (V0DecayX/Y/Z, NOT fTv0x/y/z - see
						// Collect.h comment: the mother KFParticle stored in fTv0x/y/z has already
						// been TransportToProductionVertex()'d by KFParticle_DST before Collect ever
						// reads it, so it sits ~at the PV, not the decay vertex - confirmed empirically
						// 2026-09-17, |v0pos-PV| was uncorrelated with v0decaylen and dira was scattered
						// from -0.75 to +0.1 on a DIRA>=0.99 sample) vs PV (m_vx/m_vy/m_vz), along fTv0p{x,y,z}
						fTv0decayx[fTnv0]	= V0DecayX[iv0];
						fTv0decayy[fTnv0]	= V0DecayY[iv0];
						fTv0decayz[fTnv0]	= V0DecayZ[iv0];
						double dx	= fTv0decayx[fTnv0] - m_vx;
						double dy	= fTv0decayy[fTnv0] - m_vy;
						double dz	= fTv0decayz[fTnv0] - m_vz;
						double dmag	= sqrt(dx*dx + dy*dy + dz*dz);
						double pmag	= fTv0ptot[fTnv0];
						fTv0dira[fTnv0]		= (dmag>0. && pmag>0.) ? (fTv0px[fTnv0]*dx + fTv0py[fTnv0]*dy + fTv0pz[fTnv0]*dz)/(dmag*pmag) : -9.;
						double crx	= dy*fTv0pz[fTnv0] - dz*fTv0py[fTnv0];
						double cry	= dz*fTv0px[fTnv0] - dx*fTv0pz[fTnv0];
						double crz	= dx*fTv0py[fTnv0] - dy*fTv0px[fTnv0];
						fTv0pvdca[fTnv0]	= (pmag>0.) ? sqrt(crx*crx + cry*cry + crz*crz)/pmag : -9.;
					}
					fTv0eta[fTnv0]		= fTv0TFeta[iv0];
					fTv0phi[fTnv0]		= fTv0TFphi[iv0];	
					fTv0ene[fTnv0]		= fTv0TFene[iv0];	
					fTv0mass[fTnv0]		= fTv0TFmass[iv0];
					fTv0ctau[fTnv0]		= fTv0TFctau[iv0];		
					fTv0decaylen[fTnv0]	= fTv0TFdecaylen[iv0];	
					fTv0chi2ndf[fTnv0]	= fTv0TFchi2ndf[iv0];	
					fTv0indtr1[fTnv0]	= fTv0TFindtr1[iv0];	// still set in vertex&track loops below!
					fTv0indtr2[fTnv0]	= fTv0TFindtr2[iv0];	// still set in vertex&track loops below!
					fV0FinalToTF[fTnv0]	= iv0;					// remember which trigger-frame slot this final slot came from (dedup skips shift these out of alignment!)
					++fTnv0;
				}
			}
			if (DEBUG2) cout<<"done finding v0s in this crossing:  nv0tf= "<<nv0tf<<endl;
			//
			//---- loop over tracks attached to this vertex...
			//for (SvtxVertex::TrackIter iter2=vertex->begin_tracks(); iter2!=vertex->end_tracks();++iter2 ){
			//	SvtxTrack *track 		= trackmap->get(*iter2);
			//---- loop over ALL tracks in this trigger frame, and require crossing #s match vertex->track!
			//
			//
			if (DEBUG2) cout<<"looping over tracks... "<<trackmap->size()<<endl;
			for (auto &iter : *trackmap){
		 		SvtxTrack *track	= iter.second;
				if (!track) continue;
				int m_track_crossing	= track->get_crossing();
				if (m_track_crossing != m_vertex_crossing) continue; 
				//
				unsigned int trackid 	= track->get_id();
				double chg				= track->get_charge();
				double px				= track->get_px();
				double py				= track->get_py();
				double pz				= track->get_pz();
				double chisq			= track->get_chisq();
				double quality			= track->get_quality();
				double pt				= sqrt(px*px + py*py);
				double ptot				= sqrt(pt*pt + pz*pz);
				double eta				= track->get_eta();
				double phi				= track->get_phi();
				double dca3d_xy			= NAN;
				double dca3d_xy_error	= NAN;
				double dca3d_z			= NAN;
				double dca3d_z_error	= NAN;
				if (DEBUG2) cout<<"TrackLoop: getting dca..."<<endl;
				if (svtxvertexmap){
					calc3DDCA(track, svtxvertexmap, m_vertexid, dca3d_xy, dca3d_xy_error, dca3d_z, dca3d_z_error);
				} else {
					cout<<"no vertex map."<<endl;
				}
				//
				int vertexFlag	= 0;
				if (track->get_vertex_id() == m_vertexid) vertexFlag = 1; 	// this track is attached to this vertex!  (otherwise just same crossing)
				if (vertexFlag==1){
					if (fabs(dca3d_xy) > fPrimaryDCAxyLimit || fabs(dca3d_z) > fPrimaryDCAzLimit){
						vertexFlag = 0;	// vertex finder says it's attached, but PCA to the vertex is too large
					}
				}
				//
				if (DEBUG2) cout<<"TrackLoop: doing dedx..."<<endl;
				double	dedx70n		=    0;
				double	dedx70s		= -999;
				double	dedxKFP		= -999;
				int 	nmaps	=    0;
				int 	nintt	=    0;
				int 	ntpc	=    0;
				int 	nmms	=    0;
				//double  seedpx	=	 0;	
				//double  seedpy	=	 0;	
				//double  seedpz	=	 0;	
				double  seedpt	=	 0;
				double  seedeta	=	 0;
				double  seedphi	=	 0;
				uint64_t layermask	= 0;	// one bit per absolute TrkrDefs layer with a cluster on this track - see fTlayermask in Collect.h
				TrkrDefs::cluskey siclukey_local[7] = {~0ULL,~0ULL,~0ULL,~0ULL,~0ULL,~0ULL,~0ULL};	// raw Si cluster key per layer 0-6; ~0ULL sentinel (0 itself is a valid key - see README_SplitTracks.md sec. 12.3) - see fTsiclukey in Collect.h
				UChar_t tpcsector_local[48];	// see fTtpcsector in Collect.h - README_SplitTracks.md sec. 12b
				Char_t  tpcarclen_local[48];
				Short_t tpcz_local[48];
				for (int itl=0;itl<48;itl++){ tpcsector_local[itl]=255; tpcarclen_local[itl]=0; tpcz_local[itl]=0; }
				std::vector<DiagTempClusterInfo20260920> thisTrackClusters20260920;	// DIAGTEMP-20260920
				auto tpcseed = track->get_tpc_seed();
				if (tpcseed){
					//
					//seedpx	= tpcseed->get_px();
					//seedpy	= tpcseed->get_py();
					//seedpz	= tpcseed->get_pz();
					//seedpt	= sqrt(seedpx*seedpx + seedpy*seedpy);
					seedpt	= tpcseed->get_pt();
					seedeta	= tpcseed->get_eta();
					seedphi	= tpcseed->get_phi();
					bool  signsame = std::signbit(eta) == std::signbit(seedeta);
					if ( !signsame ) seedeta = -seedeta;
					//
					std::vector<double> dedxvalues	= calc_dedx(tpcseed, clustermap, tpcGeom);
					//
					if (dedxvalues.size()==2){
						dedx70n		= dedxvalues.at(0);		//     Nclus ,  70% truncation
						dedx70s		= dedxvalues.at(1);		//     <dedx>,  70% truncation
					}
					//
					// same call KFParticle_Tools::get_dEdx() makes internally for the usePID()
					// dE/dx cut - written as its own branch (dedxKFP) so it can be compared
					// directly against our own dedx70s above without reimplementing anything
					// (see README_localKFPchanges.md, 2026-09-18 dE/dx scale hack entry)
					{
						float layerThicknesses[4] = {0.0, 0.0, 0.0, 0.0};
						layerThicknesses[0] = tpcGeom->GetLayerCellGeom(7 )->get_thickness();
						layerThicknesses[1] = tpcGeom->GetLayerCellGeom(8 )->get_thickness();
						layerThicknesses[2] = tpcGeom->GetLayerCellGeom(27)->get_thickness();
						layerThicknesses[3] = tpcGeom->GetLayerCellGeom(50)->get_thickness();
						dedxKFP = TrackAnalysisUtils::calc_dedx(tpcseed, clustermap, geometry, layerThicknesses);
					}
					//
					for (auto it = tpcseed->begin_cluster_keys(); it != tpcseed->end_cluster_keys(); ++it){
						auto ckey = *it;
						if (TrkrDefs::getTrkrId(ckey) == TrkrDefs::TrkrId::tpcId       ){ ntpc++;  } 
						if (TrkrDefs::getTrkrId(ckey) == TrkrDefs::TrkrId::micromegasId){ nmms++;  } 
						if (TrkrDefs::getTrkrId(ckey) == TrkrDefs::TrkrId::tpcId ){
							auto cluster	= clustermap->findCluster(ckey);
							if (!cluster) continue; 	//!!!!! PROTECTION DST MISMATCH !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
							auto surf 		= geometry->maps().getSurface(ckey, cluster);
							//if (surf){
							//	Acts::Vector3 global_in = geometry->getGlobalPosition(key, cluster);
							//}
							unsigned int cluslayer	= TrkrDefs::getLayer(ckey);
							if (cluslayer<=54){ layermask |= (1ULL << cluslayer); }	// TPC bits (7-54) of fTlayermask
						if (cluslayer>=7 && cluslayer<=54){
							// Merged-track support (README_SplitTracks.md sec.12b): compact per-layer
							// local position. Real merging can only happen within one TPC sector
							// (sectors are separated by dead-space frames), so we persist a sector ID
							// plus a local (arc-length, z) position rather than full global 3D position -
							// same information content where it matters, far fewer bytes. See Collect.h
							// for the fixed mm/count scales (chosen with margin, not per-layer geometry).
							int tpcidx = (int)cluslayer - 7;
							Acts::Vector3 tpcGlobalPos = m_globalPositionWrapper.getGlobalPositionDistortionCorrected(ckey, cluster, (short)m_track_crossing);
							double gx = tpcGlobalPos.x();
							double gy = tpcGlobalPos.y();
							double gz = tpcGlobalPos.z();
							double phiGlobal = atan2(gy, gx);
							double radiusCm  = sqrt(gx*gx + gy*gy);
							constexpr double TPC_SECTOR_WIDTH_RAD = 2.0*M_PI / 12.0;	// TpcDefs::NSectors
							double phiWrapped = fmod(phiGlobal, TPC_SECTOR_WIDTH_RAD);
							if (phiWrapped < 0) phiWrapped += TPC_SECTOR_WIDTH_RAD;
							if (phiWrapped > TPC_SECTOR_WIDTH_RAD/2.0) phiWrapped -= TPC_SECTOR_WIDTH_RAD;
							double localArcLenMM = radiusCm * 10.0 * phiWrapped;	// cm -> mm
							long arclenCount = lround(localArcLenMM / 2.0);	// 2.0 mm/count, see Collect.h
							if (arclenCount >  127) arclenCount =  127;
							if (arclenCount < -127) arclenCount = -127;
							long zCount = lround(gz * 100.0);	// 0.1 mm/count, see Collect.h
							if (zCount >  32767) zCount =  32767;
							if (zCount < -32767) zCount = -32767;
							int sectorId = (int)TpcDefs::getSectorId(ckey);
							int sideId   = (int)TpcDefs::getSide(ckey);
							tpcsector_local[tpcidx] = (UChar_t)(sectorId*2 + sideId);	// 0-23, never 255 (the sentinel)
							tpcarclen_local[tpcidx] = (Char_t)arclenCount;
							tpcz_local[tpcidx]      = (Short_t)zCount;
						}
							{	// DIAGTEMP-20260920: same normalized-adc formula as calc_dedx(), just tagged with layer+key
								PHG4TpcGeom* diagGeo20260920 = tpcGeom->GetLayerCellGeom(cluslayer);
								if (diagGeo20260920){
									float diagAdc20260920	= cluster->getAdc();
									float diagThick20260920	= diagGeo20260920->get_thickness();
									float diagR20260920		= diagGeo20260920->get_radius();
									float diagAlpha20260920	= (diagR20260920*diagR20260920) / (2*diagR20260920*TMath::Abs(1.0/tpcseed->get_qOverR()));
									float diagBeta20260920	= atan(tpcseed->get_slope());
									diagAdc20260920 /= diagThick20260920;
									diagAdc20260920 *= cos(diagAlpha20260920);
									diagAdc20260920 *= cos(diagBeta20260920);
									// DIAGTEMP-20260920 v2: real global position, crossing-corrected (v6, 2026-09-20 - see README sec.12b)
									Acts::Vector3 diagGlobalPos20260920 = m_globalPositionWrapper.getGlobalPositionDistortionCorrected(ckey, cluster, (short)m_track_crossing);
									thisTrackClusters20260920.push_back({(int)cluslayer, diagAdc20260920, ckey,
										(float)diagGlobalPos20260920.x(), (float)diagGlobalPos20260920.y(), (float)diagGlobalPos20260920.z()});
								}
							}
						}	// end tpcId check...
					}	// end cluster key loop...
					//
				}	// tpcseed exists...
				//if (DEBUG) cout<<"TrackLoop: done doing dedx..."<<endl;
				//
				if (DEBUG2) cout<<"TrackLoop: count silicon hits..."<<endl;
				auto silseed = track->get_silicon_seed();
				if (silseed){
					for (auto it = silseed->begin_cluster_keys(); it != silseed->end_cluster_keys(); ++it){
						auto ckey = *it;
						unsigned int siliconlayer = TrkrDefs::getLayer(ckey);
						switch (TrkrDefs::getTrkrId(ckey)){
							case TrkrDefs::mvtxId:
								nmaps++;
								layermask |= (1ULL << siliconlayer);	// MVTX bits (0-2) of fTlayermask
								siclukey_local[siliconlayer] = ckey;	// MVTX slots (0-2) of fTsiclukey
								break;
							case TrkrDefs::inttId:
								nintt++;
								layermask |= (1ULL << siliconlayer);	// INTT bits (3-6) of fTlayermask
								siclukey_local[siliconlayer] = ckey;	// INTT slots (3-6) of fTsiclukey
								break;
						}	// ensd detId check...
					}	// end cluster key loop...
				}	// silseed exists...
				//if (DEBUG) cout<<"TrackLoop: done counting silicon hits..."<<endl;
				//
				//---- good track selection (daughters from v0s are kept even if they fail this goodtrack cut!)
				bool KEEPTRK			  	  = true;
				if (ntpc   < ntpcmin  ){ KEEPTRK = false; } else	// open in general, tight if adding towers to track tree!
				if (nmaps  < nmvtmin  ){ KEEPTRK = false; } else
				if (nintt  < nintmin  ){ KEEPTRK = false; } else
				if (pt     < PTLL     ){ KEEPTRK = false; } else
				if (pt    >= PTLL+20. ){ KEEPTRK = false; }
				//
				//
				//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
				//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
				// Collect::process_event -- caloRadiusInnerEMCal= 93.5
				// Collect::process_event -- caloRadiusOuterEMCal= 114
				// Collect::process_event -- caloRadiusInnerIHCal= 115
				// Collect::process_event -- caloRadiusOuterIHCal= 140.005
				// Collect::process_event -- caloRadiusInnerOHCal= 177.423
				// Collect::process_event -- caloRadiusOuterOHCal= 274.317
				// Collect::process_event -- caloRadiusMidEMCal= 103.75
				// Collect::process_event -- caloRadiusMidIHCal= 127.502
				// Collect::process_event -- caloRadiusMidOHCal= 225.87
				//
				//---- look at the calorimeters near this track!
				if (DEBUG2) cout<<"TrackLoop: Swim to calos..."<<endl;
				//
				//SvtxTrackState *thisState	= nullptr;
				SvtxTrackState *thisStateI	= nullptr;
				SvtxTrackState *thisStateO	= nullptr;
				double stx=0,sty=0,stz=0;
				//
				//---- loop over states on this track to find first & last tpc states...	
				double rhomin		= 999.; 
				double rhomax		=   0.; 
				for (SvtxTrack::ConstStateIter state_itr = track->begin_states();
											   state_itr != track->end_states(); 
											   state_itr++){	
					SvtxTrackState* astate = dynamic_cast<SvtxTrackState*>(state_itr->second);
					stx	= astate->get_x();
					sty	= astate->get_y();
					stz	= astate->get_z();
					double rho	= sqrt( stx*stx + sty*sty );
					//cout<<track->get_id()<<"\t "<<astate->get_pathlength()<<"\t "<<rho<<endl;
					if (rho>30. && rho<rhomin){
						rhomin					= rho;
						fTstateTI_x[fNTracks]	= stx;	// first TPC state on track
						fTstateTI_y[fNTracks]	= sty;	// first TPC state on track
						fTstateTI_z[fNTracks]	= stz;	// first TPC state on track
					}
					if (rho<76. && rho>rhomax){
						rhomax					= rho;
						fTstateTO_x[fNTracks]	= stx;	// last TPC state on track
						fTstateTO_y[fNTracks]	= sty;	// last TPC state on track
						fTstateTO_z[fNTracks]	= stz;	// last TPC state on track
					}
				}
				//
				if (fHasCaloGeo && fAddTowersToTrackTree){
				//---- take track to emcal depth
				thisStateI	= track->get_state(caloRadiusInnerEMCal);
				thisStateO	= track->get_state(caloRadiusOuterEMCal);
				if ( thisStateI && !thisStateO){ 
					stx	= thisStateI->get_x();
					sty	= thisStateI->get_y();
					stz	= thisStateI->get_z();
				} else if ( !thisStateI && thisStateO){ 
					stx	= thisStateO->get_x();
					sty	= thisStateO->get_y();
					stz	= thisStateO->get_z();
				} else if ( thisStateI && thisStateO){ 
					stx	= (thisStateI->get_x() + thisStateO->get_x())/2.;
					sty	= (thisStateI->get_y() + thisStateO->get_y())/2.;
					stz	= (thisStateI->get_z() + thisStateO->get_z())/2.;
				} 
				if (thisStateI||thisStateO){
					fTstateEM_x[fNTracks]	= stx;
					fTstateEM_y[fNTracks]	= sty;
					fTstateEM_z[fNTracks]	= stz;
					fTstateEM_eta[fNTracks]	= asinh(stz/sqrt(stx*stx + sty*sty));
					fTstateEM_phi[fNTracks]	= atan2(sty,stx);
					int ieta				= GetIETA(SvtxTrack::CEMC, fTstateEM_eta[fNTracks]);
					int iphi				= GetIPHI(SvtxTrack::CEMC, fTstateEM_phi[fNTracks]);
 					if (ieta>=0&&ieta<96&&iphi>=0&&iphi<256){	// >>>---> ieta,iphi in hand for this state! 
						//RawTowerDefs::keytype keyRaw	= RawTowerDefs::encode_towerid(RawTowerDefs::CalorimeterId::CEMC, ieta, iphi);
						//RawTowerGeom* towergeom		= EMCalGeo->get_tower_geometry(keyRaw);
						//double toweta					= towergeom->get_eta();
						//double towphi					= towergeom->get_phi();
						//unsigned int keyInfo	= TowerInfoDefs::encode_emcal(ieta,iphi);
						//unsigned int ichannel	= TowerInfoDefs::decode_emcal(keyInfo);
						//TowerInfo* towinfo 	= towersEM->get_tower_at_channel(ichannel);
						double esum =  0.;
						for (int idphi=-3;idphi<=3;idphi++){
							for (int ideta=-3;ideta<=3;ideta++){
								int ietatow	= ieta + ideta;
								int iphitow	= iphi + idphi;
								if (iphitow<   0) iphitow+=256; 
								if (iphitow>=256) iphitow-=256; 
								double etow	= -9.;
								if (ietatow>=0&&ietatow<96&&iphitow>=0&&iphitow<256){	// >>>---> ieta,iphi in hand for this state! 
									unsigned int keytow		 = TowerInfoDefs::encode_emcal(ietatow,iphitow);	// ieta,iphi->key
									unsigned int ichan		 = TowerInfoDefs::decode_emcal(keytow);				//       key->chan
									TowerInfo* towi 		 = towersEM->get_tower_at_channel(ichan);			//      chan->info
									etow					 = towi->get_energy();
									esum					+= etow;
								} 	
								fTegridEM[fNTracks][3+ideta][3+idphi]	 = etow;
							}	// end ideta
						}	// end idphi
						fTegridEMsum7x7[fNTracks]	= esum;
						fTeopEM[fNTracks]			= esum/track->get_p();
 						if (SHOWGRIDS && track->get_p()>5.){
 							cout<<fEvtSeq<<"\tEM "<<track->get_id()<<" "<<track->get_p()<<" "<<fTegridEMsum7x7[fNTracks]<<endl;
							for (int idphi=-3;idphi<=3;idphi++){
								for (int ideta=-3;ideta<=3;ideta++){
									cout<<FIXFLOAT(fTegridEM[fNTracks][3+ideta][3+idphi])<<" ";
								}	cout<<endl;
							}		cout<<endl;
						}	// end DEBUG check
						//
						if (CALODETAIL && ievtSeen<MAXEVTDETAIL){
							hPTOT_etaDETem[ievtSeen]	->Fill( fTstateEM_eta[fNTracks], track->get_p() );
							hPTOT_phiDETem[ievtSeen]	->Fill( fTstateEM_phi[fNTracks], track->get_p() );
						}
						//
 					} else {
						fTstateEM_x[fNTracks]	=  0.;
						fTstateEM_y[fNTracks]	=  0.;
						fTstateEM_z[fNTracks]	=  0.;
						fTstateEM_eta[fNTracks]	= -9.;
						fTstateEM_phi[fNTracks]	= -9.;
						for (int idphi=-3;idphi<=3;idphi++){
							for (int ideta=-3;ideta<=3;ideta++){
								fTegridEM[fNTracks][3+ideta][3+idphi]	= -9;
							}
						}
						fTeopEM[fNTracks]	= -9.;
					}	//	end ieta,iphi check
 				} else {
					fTstateEM_x[fNTracks]	=  0.;
					fTstateEM_y[fNTracks]	=  0.;
					fTstateEM_z[fNTracks]	=  0.;
					fTstateEM_eta[fNTracks]	= -9.;
					fTstateEM_phi[fNTracks]	= -9.;
					for (int idphi=-3;idphi<=3;idphi++){
						for (int ideta=-3;ideta<=3;ideta++){
							fTegridEM[fNTracks][3+ideta][3+idphi]	= -9;
						}
					}
					fTeopEM[fNTracks]	= -9.;
				}	//	end thisState
				//
				//---- take track to ihcal depth
				thisStateI	= track->get_state(caloRadiusInnerIHCal);
				thisStateO	= track->get_state(caloRadiusOuterIHCal);
				if ( thisStateI && !thisStateO){ 
					stx	= thisStateI->get_x();
					sty	= thisStateI->get_y();
					stz	= thisStateI->get_z();
				} else if ( !thisStateI && thisStateO){ 
					stx	= thisStateO->get_x();
					sty	= thisStateO->get_y();
					stz	= thisStateO->get_z();
				} else if ( thisStateI && thisStateO){ 
					stx	= (thisStateI->get_x() + thisStateO->get_x())/2.;
					sty	= (thisStateI->get_y() + thisStateO->get_y())/2.;
					stz	= (thisStateI->get_z() + thisStateO->get_z())/2.;
				} 
				if (thisStateI||thisStateO){
					fTstateIH_x[fNTracks]	= stx;
					fTstateIH_y[fNTracks]	= sty;
					fTstateIH_z[fNTracks]	= stz;
					fTstateIH_eta[fNTracks]	= asinh(stz/sqrt(stx*stx + sty*sty));
					fTstateIH_phi[fNTracks]	= atan2(sty,stx);
					int ieta				= GetIETA(SvtxTrack::HCALIN, fTstateIH_eta[fNTracks]);
					int iphi				= GetIPHI(SvtxTrack::HCALIN, fTstateIH_phi[fNTracks]);
 					if (ieta>=0&&ieta<24&&iphi>=0&&iphi<64){	// >>>---> ieta,iphi in hand for this state! 
						//RawTowerDefs::keytype keyRaw	= RawTowerDefs::encode_towerid(RawTowerDefs::CalorimeterId::HCALIN, ieta, iphi);
						//RawTowerGeom* towergeom		= IHCalGeo->get_tower_geometry(keyRaw);
						//double toweta					= towergeom->get_eta();
						//double towphi					= towergeom->get_phi();
						//unsigned int keyInfo	= TowerInfoDefs::encode_hcal(ieta,iphi);
						//unsigned int ichannel	= TowerInfoDefs::decode_hcal(keyInfo);
						//TowerInfo* towinfo 	= towersIH->get_tower_at_channel(ichannel);
						double esum =  0.;
						for (int idphi=-3;idphi<=3;idphi++){
							for (int ideta=-3;ideta<=3;ideta++){
								int ietatow	= ieta + ideta;
								int iphitow	= iphi + idphi;
								if (iphitow<   0) iphitow+= 64; 
								if (iphitow>= 64) iphitow-= 64; 
								double etow	= -9.;
								if (ietatow>=0&&ietatow<24&&iphitow>=0&&iphitow<64){	// >>>---> ieta,iphi in hand for this state! 
									unsigned int keytow		 = TowerInfoDefs::encode_hcal(ietatow,iphitow);		// ieta,iphi->key
									unsigned int ichan		 = TowerInfoDefs::decode_hcal(keytow);				//       key->chan
									TowerInfo* towi 		 = towersIH->get_tower_at_channel(ichan);			//      chan->info
									etow					 = towi->get_energy();
									esum					+= etow;
								} 	
								fTegridIH[fNTracks][3+ideta][3+idphi]	 = etow;
							}	// end ideta
						}	// end idphi
						fTegridIHsum7x7[fNTracks]	= esum;
						fTeopIH[fNTracks]			= esum/track->get_p();
 						if (SHOWGRIDS && track->get_p()>5.){
 							cout<<fEvtSeq<<"\tIH "<<track->get_id()<<" "<<track->get_p()<<" "<<fTegridIHsum7x7[fNTracks]<<endl;
							for (int idphi=-3;idphi<=3;idphi++){
								for (int ideta=-3;ideta<=3;ideta++){
									cout<<FIXFLOAT(fTegridIH[fNTracks][3+ideta][3+idphi])<<" ";
								}	cout<<endl;
							}		cout<<endl;
						}	// end DEBUG check
						//
						if (CALODETAIL && ievtSeen<MAXEVTDETAIL){
							hPTOT_etaDETih[ievtSeen]	->Fill( fTstateIH_eta[fNTracks], track->get_p() );
							hPTOT_phiDETih[ievtSeen]	->Fill( fTstateIH_phi[fNTracks], track->get_p() );
						}
						//
 					} else {
						fTstateIH_x[fNTracks]	=  0.;
						fTstateIH_y[fNTracks]	=  0.;
						fTstateIH_z[fNTracks]	=  0.;
						fTstateIH_eta[fNTracks]	= -9.;
						fTstateIH_phi[fNTracks]	= -9.;
						for (int idphi=-3;idphi<=3;idphi++){
							for (int ideta=-3;ideta<=3;ideta++){
								fTegridIH[fNTracks][3+ideta][3+idphi]	= -9;
							}
						}
						fTeopIH[fNTracks]	= -9.;
					}	//	end ieta,iphi check
				} else {
					fTstateIH_x[fNTracks]	=  0.;
					fTstateIH_y[fNTracks]	=  0.;
					fTstateIH_z[fNTracks]	=  0.;
					fTstateIH_eta[fNTracks]	= -9.;
					fTstateIH_phi[fNTracks]	= -9.;
					for (int idphi=-3;idphi<=3;idphi++){
						for (int ideta=-3;ideta<=3;ideta++){
							fTegridIH[fNTracks][3+ideta][3+idphi]	= -9;
						}
					}
					fTeopIH[fNTracks]	= -9.;
				}	//	end thisState
				//
				//---- take track to ohcal depth
				thisStateI	= track->get_state(caloRadiusInnerOHCal);
				thisStateO	= track->get_state(caloRadiusOuterOHCal);
				if ( thisStateI && !thisStateO){ 
					stx	= thisStateI->get_x();
					sty	= thisStateI->get_y();
					stz	= thisStateI->get_z();
				} else if ( !thisStateI && thisStateO){ 
					stx	= thisStateO->get_x();
					sty	= thisStateO->get_y();
					stz	= thisStateO->get_z();
				} else if ( thisStateI && thisStateO){ 
					stx	= (thisStateI->get_x() + thisStateO->get_x())/2.;
					sty	= (thisStateI->get_y() + thisStateO->get_y())/2.;
					stz	= (thisStateI->get_z() + thisStateO->get_z())/2.;
				} 
				if (thisStateI||thisStateO){
					fTstateOH_x[fNTracks]	= stx;
					fTstateOH_y[fNTracks]	= sty;
					fTstateOH_z[fNTracks]	= stz;
					fTstateOH_eta[fNTracks]	= asinh(stz/sqrt(stx*stx + sty*sty));
					fTstateOH_phi[fNTracks]	= atan2(sty,stx);
					int ieta				= GetIETA(SvtxTrack::HCALOUT, fTstateOH_eta[fNTracks]);
					int iphi				= GetIPHI(SvtxTrack::HCALOUT, fTstateOH_phi[fNTracks]);
 					if (ieta>=0&&ieta<24&&iphi>=0&&iphi<64){	// >>>---> ieta,iphi in hand for this state! 
						//RawTowerDefs::keytype keyRaw	= RawTowerDefs::encode_towerid(RawTowerDefs::CalorimeterId::HCALOUT, ieta, iphi);
						//RawTowerGeom* towergeom		= OHCalGeo->get_tower_geometry(keyRaw);
						//double toweta					= towergeom->get_eta();
						//double towphi					= towergeom->get_phi();
						//unsigned int keyInfo	= TowerInfoDefs::encode_hcal(ieta,iphi);
						//unsigned int ichannel	= TowerInfoDefs::decode_hcal(keyInfo);
						//TowerInfo* towinfo 	= towersOH->get_tower_at_channel(ichannel);
						double esum =  0.;
						for (int idphi=-3;idphi<=3;idphi++){
							for (int ideta=-3;ideta<=3;ideta++){
								int ietatow	= ieta + ideta;
								int iphitow	= iphi + idphi;
								if (iphitow<   0) iphitow+= 64; 
								if (iphitow>= 64) iphitow-= 64; 
								double etow	= -9.;
								if (ietatow>=0&&ietatow<24&&iphitow>=0&&iphitow<64){	// >>>---> ieta,iphi in hand for this state! 
									unsigned int keytow		 = TowerInfoDefs::encode_hcal(ietatow,iphitow);		// ieta,iphi->key
									unsigned int ichan		 = TowerInfoDefs::decode_hcal(keytow);				//       key->chan
									TowerInfo* towi 		 = towersOH->get_tower_at_channel(ichan);			//      chan->info
									etow					 = towi->get_energy();
									esum					+= etow;
								} 	
								fTegridOH[fNTracks][3+ideta][3+idphi]	= etow;
							}	// end ideta
						}	// end idphi
						fTegridOHsum7x7[fNTracks]	= esum;
						fTeopOH[fNTracks]			= esum/track->get_p();
 						if (SHOWGRIDS && track->get_p()>5.){
 							cout<<fEvtSeq<<"\tOH "<<track->get_id()<<" "<<track->get_p()<<" "<<fTegridOHsum7x7[fNTracks]<<endl;
							for (int idphi=-3;idphi<=3;idphi++){
								for (int ideta=-3;ideta<=3;ideta++){
									cout<<FIXFLOAT(fTegridOH[fNTracks][3+ideta][3+idphi])<<" ";
								}	cout<<endl;
							}		cout<<endl;
						}	// end DEBUG check
						//
						if (CALODETAIL && ievtSeen<MAXEVTDETAIL){
							hPTOT_etaDEToh[ievtSeen]	->Fill( fTstateOH_eta[fNTracks], track->get_p() );
							hPTOT_phiDEToh[ievtSeen]	->Fill( fTstateOH_phi[fNTracks], track->get_p() );
						}
						//
					} else {
						fTstateOH_x[fNTracks]	=  0.;
						fTstateOH_y[fNTracks]	=  0.;
						fTstateOH_z[fNTracks]	=  0.;
						fTstateOH_eta[fNTracks]	= -9.;
						fTstateOH_phi[fNTracks]	= -9.;
						for (int idphi=-3;idphi<=3;idphi++){
							for (int ideta=-3;ideta<=3;ideta++){
								fTegridOH[fNTracks][3+ideta][3+idphi]	= -9;
							}
						}
						fTeopOH[fNTracks]	= -9.;
 					}	//	end ieta,iphi check
				} else {
					fTstateOH_x[fNTracks]	=  0.;
					fTstateOH_y[fNTracks]	=  0.;
					fTstateOH_z[fNTracks]	=  0.;
					fTstateOH_eta[fNTracks]	= -9.;
					fTstateOH_phi[fNTracks]	= -9.;
					for (int idphi=-3;idphi<=3;idphi++){
						for (int ideta=-3;ideta<=3;ideta++){
							fTegridOH[fNTracks][3+ideta][3+idphi]	= -9;
						}
					}
					fTeopOH[fNTracks]	= -9.;
				}	//	end thisState
				} else {	// !fHasCaloGeo or !fAddTowersToTrackTree -- calo info unavailable or not wanted, leave calo-layer-around-track fields at "no info" defaults
					fTstateEM_eta[fNTracks] = fTstateIH_eta[fNTracks] = fTstateOH_eta[fNTracks] = -9.;
					fTstateEM_phi[fNTracks] = fTstateIH_phi[fNTracks] = fTstateOH_phi[fNTracks] = -9.;
					fTegridEMsum7x7[fNTracks] = fTegridIHsum7x7[fNTracks] = fTegridOHsum7x7[fNTracks] = 0.;
					fTeopEM[fNTracks] = fTeopIH[fNTracks] = fTeopOH[fNTracks] = -9.;
					for (int idphi=-3;idphi<=3;idphi++){
						for (int ideta=-3;ideta<=3;ideta++){
							fTegridEM[fNTracks][3+ideta][3+idphi] = -9;
							fTegridIH[fNTracks][3+ideta][3+idphi] = -9;
							fTegridOH[fNTracks][3+ideta][3+idphi] = -9;
						}
					}
				}	// end fHasCaloGeo
				{	// eopHC/eopCA: sum of per-layer eop values (same denominator
					// track->get_p(), so addition is exact) - a hadronic shower does
					// not respect the IH/OH boundary, so the combined hadronic
					// (HC=IH+OH) and total-calorimeter (CA=EM+IH+OH) ratios are more
					// physically meaningful than the individual IH/OH ratios alone.
					bool hasEM = (fTeopEM[fNTracks] > -8.5);
					bool hasIH = (fTeopIH[fNTracks] > -8.5);
					bool hasOH = (fTeopOH[fNTracks] > -8.5);
					fTeopHC[fNTracks] = (hasIH||hasOH) ? (hasIH?fTeopIH[fNTracks]:0.) + (hasOH?fTeopOH[fNTracks]:0.) : -9.;
					fTeopCA[fNTracks] = (hasEM||hasIH||hasOH) ? (hasEM?fTeopEM[fNTracks]:0.) + (hasIH?fTeopIH[fNTracks]:0.) + (hasOH?fTeopOH[fNTracks]:0.) : -9.;
				}
				if (DEBUG2) cout<<fNTracks<<"\t CALO "<<fTegridEMsum7x7[fNTracks]<<" "<<fTegridIHsum7x7[fNTracks]<<" "<<fTegridOHsum7x7[fNTracks]<<" "<<endl;
				//
				//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
				//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
				//				
				//
				//---- loop through v0 crossing info and find any daughters
				if (DEBUG2) cout<<"TrackLoop: loop over v0s and find daughters..."<<endl;
				int thispid		=  0;
				int thisindv0	= -1;
				if (v0found){
					for (int iv0=0;iv0<fTnv0;iv0++){
						int itf			= fV0FinalToTF[iv0];	// fTv0*[iv0] came from trigger-frame slot itf - V0Crossing/V0pid/V0Track1id/V0Track2id below are STILL in trigger-frame space, NOT final space, so they must be read at itf, not iv0!
						int thisv0xing	= V0Crossing[itf];
						if (thisv0xing == m_track_crossing		// this v0 is in the same crossing as this track!
						 && thispid    == 0 ){					// this line prevents assigning this track to multiple V0s!
							if (trackid== V0Track1id[itf]){
								fTv0indtr1[iv0]	= fNTracks;		// set index to this track(1) in v0 tree!
							}
							if (trackid== V0Track2id[itf]){
								fTv0indtr2[iv0]	= fNTracks;		// set index to this track(2) in v0 tree!
							}
							if (trackid==V0Track1id[itf] && V0pid[itf]== Kshort_PDGID){ thispid = 1; }	//trk1 pi+
							if (trackid==V0Track2id[itf] && V0pid[itf]== Kshort_PDGID){ thispid = 1; }	//trk2 pi-
							if (trackid==V0Track1id[itf] && V0pid[itf]== Lambda_PDGID){ thispid = 3; }	//trk1 proton
							if (trackid==V0Track2id[itf] && V0pid[itf]== Lambda_PDGID){ thispid = 1; }	//trk2 pi-
							if (trackid==V0Track1id[itf] && V0pid[itf]==ALambda_PDGID){ thispid = 1; }	//trk1 pi+
							if (trackid==V0Track2id[itf] && V0pid[itf]==ALambda_PDGID){ thispid = 3; }	//trk2 antiproton
							//if (DEBUG) cout	<<"V0 LOOP .. iv0="<<iv0<<"\t v0xing: "<<thisv0xing<<" "<<m_track_crossing
							//				<<"\t trkid= "<<trackid<<"\t fNTracks= "<<fNTracks
							//				<<"\t pid= "<<thispid<<"\t thisindv0= "<<thisindv0
							//				<<endl;
							if (thispid>0){		//---- set index to this V0
								thisindv0	= iv0;
								if (DEBUG) cout<<"V0-Track Match: iv0="<<iv0
									<<"\t trackid= "<<trackid
									<<"\t V0trackid= "<<V0Track1id[itf]<<" "<<V0Track2id[itf]
									<<"\t trkxing= "<<m_track_crossing
									//<<"\t v0xing= "<<thisv0xing
									<<"\t pid,chg= "<<thispid<<" "<<chg
									<<"\t indv0= "<<thisindv0
									<<endl;
							}
						}
					}
				}	// end v0found...
				//
				if (thispid>0 && !KEEPTRK) KEEPTRK = true;	//!!!!! force keep if v0 daughter..
				//
				//if (DEBUG) cout<<"TrackLoop: done loop over v0s and find daughters..."<<endl;
				//
				if (KEEPTRK){
					//
					if (fNTracks>=MAXNTRACKS-1){
						std::cout<<"Collect::process_event -- Arrays for tree are full!  Increase MAXNTRACKS."<<std::endl;
						//exit(0);
					}
					if (fNTracks<MAXNTRACKS-1){
						fTpid[fNTracks]			= thispid;
						fTindv0[fNTracks]		= thisindv0;
						fTprimary[fNTracks]		= vertexFlag;
						fTquality[fNTracks]		= quality;
						fTchisq[fNTracks]		= chisq;
						fTchg[fNTracks]			= chg;
						fTpx[fNTracks]			= px;
						fTpy[fNTracks]			= py;
						fTpz[fNTracks]			= pz;
						fTptot[fNTracks]		= ptot;
						fTeta[fNTracks]			= eta;
						fTphi[fNTracks]			= phi;
						fTpt[fNTracks]			= pt;
						fTseedeta[fNTracks]		= seedeta;
						fTseedphi[fNTracks]		= seedphi;
						fTseedpt[fNTracks]		= seedpt;
						fTnhitstpc[fNTracks]	= ntpc;
						fTnhitsmvt[fNTracks]	= nmaps;
						fTnhitsint[fNTracks]	= nintt;
						fTdcaxy[fNTracks]		= dca3d_xy;
						fTdcaz[fNTracks]		= dca3d_z;
						fTdedx70n[fNTracks]		= (int)dedx70n;
						fTdedx70s[fNTracks]		= dedx70s;
						fTdedxKFP[fNTracks]		= dedxKFP;
						fTlayermask[fNTracks]	= layermask;
						for (int isl=0;isl<7;isl++){ fTsiclukey[fNTracks][isl] = siclukey_local[isl]; }
						for (int itl=0;itl<48;itl++){
							fTtpcsector[fNTracks][itl] = tpcsector_local[itl];
							fTtpcarclen[fNTracks][itl] = tpcarclen_local[itl];
							fTtpcz[fNTracks][itl]      = tpcz_local[itl];
						}
						if (DEBUG){
							std::cout<<trackid<<"\t chg="<<chg<<"\t"
									//<<deltapt<<"\t"
									<<vertexFlag<<" zv="<<fTvtxz<<" "<<fTvtxzGlob<<"\t "
									//<<px<<" "<<py<<" "<<pz<<" "<<ptot<<"\t"
									<<"\t eta="<<eta<<" ("<<seedeta<<")"
									<<"\t phi="<<phi<<" ("<<seedphi<<")"
									<<"\t pt=" <<pt <<" ("<<seedpt <<")"
									//<<vx<<" "<<vy<<" "<<vz<<"\t"
									//<<pcax<<" "<<pcay<<" "<<pcaz<<"\t"
									<<"\t dedx="<<dedx70n<<" "<<dedx70s
									<<"\t Ncl="<<nmaps<<" "<<nintt<<" "<<ntpc
									<<"\t CALO: "<<fTegridEMsum7x7[fNTracks]<<" "<<fTegridIHsum7x7[fNTracks]<<" "<<fTegridOHsum7x7[fNTracks]
									<<std::endl;
						}
						// DIAGTEMP-20260920: commit this track's cluster list under its fNTracks index
						diagClustersByTrack20260920.push_back(thisTrackClusters20260920);
						for (const auto &ci20260920 : thisTrackClusters20260920){
							diagKeyOwners20260920[ci20260920.key].push_back(fNTracks);
						}
						++fNTracks;
						//
					} else {
						std::cout<<"Collect::process_event -- Track "<<trackid<<" could not be saved... "<<std::endl;
					}	// end array protection on fNTracks...
				}	// end KEEPTRK
				//
			}	// end loop over tracks...
			//
			if (fNTracks>0) FlagSplitTracks();
			if (fNTracks>0) DiagMergedTracksExplore20260920(fNTracks, fTlayermask, diagClustersByTrack20260920, diagKeyOwners20260920);	// DIAGTEMP-20260920
			//
			if (fNTracks){
				++nverticeskept;
				outTree->Fill();
			}
			//
			if (fNTracks>0){
				for (int i=0;i<=fNTracks;i++){
					fTprimary[i]	= 0;		// reset track/vertex flag
				}
			}
			//
		}	// end loop over vertices...
	}	// end svtxvertexmap check...
	//
	if (Xings.size()>=2){
		std::stable_sort(Xings.begin(), Xings.end());
		for (unsigned int ix=0;ix<Xings.size()-1;ix++){
			double dXing	= Xings[ix+1] - Xings[ix];
			hdXing			->Fill(dXing);
		}
	}
	//
	//
	//std::cout<<"Event "<<ievtSeen-1<<"\t nvertices="<<nverticeskept<<endl;
	//
	//
	return Fun4AllReturnCodes::EVENT_OK;
	//
}


//------------------------------------------------------------------------
//
int Collect::GetKFP( PHCompositeNode *topNode, const int parentpdgid, const std::string &containername, const std::string &trackmapname ){
	//
	int linecount		= 0;
	int initialnv0tf	= nv0tf;			// V0 counter INCLUDING v0's found in previous calls to this routine!!!
	int thisv0id		= initialnv0tf;		// V0 counter INCLUDING v0's found in previous calls to this routine!!!
	//if (DEBUG) cout<<"Collect::GetKFP START ..  nv0tf= "<<initialnv0tf<<endl;
	//
	KFParticle_Container* kfpContainerKS = findNode::getClass<KFParticle_Container>(topNode, containername);
	if (kfpContainerKS){
		//
		int pdgidA=0,pdgidB=0,trkidA=0,trkidB=0;
		double posxA=0,posyA=0,poszA=0,posxB=0,posyB=0,poszB=0;
		//
		for (KFParticle_Container::Iter kfp_iter = kfpContainerKS->begin(); kfp_iter != kfpContainerKS->end(); ++kfp_iter){
			KFParticle *thisKFpart 		= kfp_iter->second;
			if (DEBUG) cout<<"KScontainer .. "<<thisKFpart->GetPDG()<<" "<<thisKFpart->Id()<<" "<<thisv0id<<endl;
			//
			if        (linecount%3==0){
				pdgidA = thisKFpart->GetPDG();
				trkidA = thisKFpart->Id();
				posxA  = thisKFpart->GetX();
				posyA  = thisKFpart->GetY();
				poszA  = thisKFpart->GetZ();
			} else if (linecount%3==1){
				pdgidB = thisKFpart->GetPDG();
				trkidB = thisKFpart->Id();
				posxB  = thisKFpart->GetX();
				posyB  = thisKFpart->GetY();
				poszB  = thisKFpart->GetZ();
			} else if (linecount%3==2){
				if (thisv0id < MAXNV0S){
					V0pid[thisv0id]			= parentpdgid;
					V0DecayX[thisv0id]		= 0.5*(posxA+posxB);
					V0DecayY[thisv0id]		= 0.5*(posyA+posyB);
					V0DecayZ[thisv0id]		= 0.5*(poszA+poszB);
					TParticlePDG* particleA = TDatabasePDG::Instance()->GetParticle(pdgidA);
					if (particleA->Charge()>0.){			// particleA is positive so it's particle 1
						V0Track1id[thisv0id]	= trkidA;	//		set A as 1 because it's positive
						V0Track2id[thisv0id]	= trkidB;	//		set B as 2 because it's negative
					} else if (particleA->Charge()<0.){		// particleB is positive so it's particle 1
						V0Track1id[thisv0id]	= trkidB;	//		set B as 1 because it's positive
						V0Track2id[thisv0id]	= trkidA;	//		set A as 2 because it's negative
	 				}										//
					if (DEBUG) cout<<"KScont view .. "<<pdgidA<<" "<<pdgidB<<"\t "<<trkidA<<" "<<trkidB
								<<"\t v0id: "<<thisv0id<<" trkids: "<<V0Track1id[thisv0id]<<" "<<V0Track2id[thisv0id]<<endl;
				} else {
					std::cout<<"Collect::GetKFP -- Pass 1: too many V0s for MAXNV0S="<<MAXNV0S<<", dropping v0id="<<thisv0id<<std::endl;
				}
				++thisv0id;
			}
			++linecount;
		}
		//
		if (linecount==0){
			return Fun4AllReturnCodes::EVENT_OK;	// nothing to do, so leave. 
		}
		//
		//----- get v0s from containers (one container, many crossings!) TF=TriggerFrame
		for (auto &iter : *kfpContainerKS){
			if (iter.second->GetPDG() == parentpdgid){
				KFParticle* thisKFpart	= iter.second;
				double tmass	= thisKFpart->GetMass();
				double tpx		= thisKFpart->GetPx();
				double tpy		= thisKFpart->GetPy();
				double tpz		= thisKFpart->GetPz();
				if (nv0tf < MAXNV0S-1 ){
					fTv0TFuseit[nv0tf]		= true;
					fTv0TFpid[nv0tf]		= parentpdgid;
					fTv0TFx[nv0tf]			= thisKFpart->GetX();
					fTv0TFy[nv0tf]			= thisKFpart->GetY();
					fTv0TFz[nv0tf]			= thisKFpart->GetZ();
					fTv0TFpx[nv0tf]			= tpx;
					fTv0TFpy[nv0tf]			= tpy;
					fTv0TFpz[nv0tf]			= tpz;
					fTv0TFpt[nv0tf]			= sqrt(tpx*tpx + tpy*tpy);
					fTv0TFptot[nv0tf]		= sqrt(tpz*tpz + fTv0TFpt[nv0tf]*fTv0TFpt[nv0tf]);
					fTv0TFeta[nv0tf]		= thisKFpart->GetEta();
					fTv0TFphi[nv0tf]		= thisKFpart->GetPhi();
					fTv0TFene[nv0tf]		= thisKFpart->GetE();
					fTv0TFmass[nv0tf]		= tmass;
					fTv0TFctau[nv0tf]		= thisKFpart->GetLifeTime();
					fTv0TFdecaylen[nv0tf]	= thisKFpart->GetDecayLength();
					double chi2ndf			= thisKFpart->GetChi2();
					if (thisKFpart->GetNDF()){ chi2ndf /= thisKFpart->GetNDF(); }else{ chi2ndf = -1; }
					fTv0TFchi2ndf[nv0tf]	= chi2ndf;
					fTv0TFindtr1[nv0tf]		= -1;		// set in later code! 
					fTv0TFindtr2[nv0tf]		= -1;		// set in later code! 
					++nv0tf;
				} else {
					std::cout<<"Collect .. TOO MANY V0s!!!!!!!   fix this. "<<std::endl;
					return Fun4AllReturnCodes::ABORTPROCESSING;
				}
				if (DEBUG)
				cout<<"KFP v0 .. "<<nv0tf			<<"\t"
					<<"mass=" <<thisKFpart->GetMass() 			<<"\t"
					<<"pdgid="<<thisKFpart->GetPDG() 			<<"\t"
					<<"eta="  <<thisKFpart->GetEta() 			<<" "
					<<"phi="  <<thisKFpart->GetPhi() 			<<" "
					<<"pt="   <<thisKFpart->GetPt()  			<<"\t"
					<<"X="    <<thisKFpart->GetX()  			<<" "
					<<"Y="    <<thisKFpart->GetY()  			<<" "
					<<"Z="    <<thisKFpart->GetZ()  			<<"\t"
					<<"ctau=" <<thisKFpart->GetLifeTime()  		<<"\t"
					<<"DL="   <<thisKFpart->GetDecayLength()	<<"\t"
					<<"chi2=" <<thisKFpart->GetChi2()<<" "<<thisKFpart->GetNDF() <<"\t"
					<<endl;
			}
		}
//!!		for (KFParticle_Container::Iter kfp_iter = kfpContainerKS->begin(); kfp_iter != kfpContainerKS->end(); ++kfp_iter){
//!!			KFParticle *thisKFpart 		= kfp_iter->second;
//!!			unsigned int thisKFtrackid	= thisKFpart->Id();
//!!			int pdgid					= thisKFpart->GetPDG();
//!!			//cout<<"TEST "<<thisKFtrackid<<" "<<pdgid<<" "<<thisKFpart->GetEta()<<endl;
//!!		}		
	}	// end KFPcontainer KS......
	//
	SvtxTrackMap* KFP_trackMapKS = findNode::getClass<SvtxTrackMap>(topNode, trackmapname);
	if(KFP_trackMapKS){
		// Look up each v0's crossing directly by its daughter track ID (already
		// correctly assigned above, in the KFParticle_Container pass) instead of
		// scanning this map positionally in assumed [daughter,daughter,mother]
		// triples. KFP_trackMapKS is sorted by key, and KFParticle_DST::
		// fillParticleNode_Track gives mother/intermediate pseudo-tracks huge
		// keys near UINT_MAX (a decrementing resonanceCounter) so they don't
		// collide with real track IDs - so with 2+ candidates of this species
		// in one event, every real daughter (small key) sorts before every
		// mother placeholder (huge key), breaking the triple-per-candidate
		// assumption the old position-based scan relied on. That silently
		// left V0Crossing[] holding a mother-placeholder's bogus crossing
		// (typically 32767, a sentinel) for every candidate past the first,
		// which then could never match any real vertex's crossing and was
		// dropped from Collect's v0 tree with no trace - see the
		// collect_v0_track_index_bug memory for how this was found.
		for (int iv0 = initialnv0tf; iv0 < nv0tf && iv0 < MAXNV0S; ++iv0){
			SvtxTrack* daughter1 = KFP_trackMapKS->get(V0Track1id[iv0]);
			if (daughter1){
				V0Crossing[iv0] = daughter1->get_crossing();
			}
			if (DEBUG){
				std::cout<<"KFP trk .. indexV0: "<<iv0
						<<"\t trackids: "<<V0Track1id[iv0]<<" "<<V0Track2id[iv0]
						<<"\t xing="<<V0Crossing[iv0]
						<<std::endl;
			}
		}
	}
	//
	if (DEBUG) cout<<"Collect::GetKFP END ..  running nv0tf= "<<initialnv0tf<<"\t this nv0tf= "<<nv0tf-initialnv0tf<<"\t ?: "<<thisv0id<<endl;
	return nv0tf-initialnv0tf;
	//
}	// end GetKFP



//----------------------------------------------------------------------
//
//	modified WJL 
//	now gives dca values even if the given track is /not attached/ to the given vertex!
//
void Collect::calc3DDCA(SvtxTrack* track, SvtxVertexMap* vertexmap, unsigned int vtxid,
			   double& dca3d_xy, double& dca3d_xy_error,
			   double& dca3d_z , double& dca3d_z_error )
{
  Acts::Vector3 pos(track->get_x(),
		    track->get_y(),
		    track->get_z());
  Acts::Vector3 mom(track->get_px(),
		    track->get_py(),
		    track->get_pz());
//WJL  auto vtxid = track->get_vertex_id();
  auto svtxVertex = vertexmap->get(vtxid);
  Acts::Vector3 vertex(svtxVertex->get_x(),
		       svtxVertex->get_y(),
		       svtxVertex->get_z());
  pos -= vertex;
  Acts::ActsMatrix<3,3> posCov;
  for(int i = 0; i < 3; ++i){
      for(int j = 0; j < 3; ++j){
	      posCov(i, j) = track->get_error(i, j);
      } 
  }
  Acts::Vector3 r = mom.cross(Acts::Vector3(0.,0.,1.));
  double phi = atan2(r(1), r(0));
  Acts::RotationMatrix3 rot;
  Acts::RotationMatrix3 rot_T;
  rot(0,0) = cos(phi);
  rot(0,1) = -sin(phi);
  rot(0,2) = 0;
  rot(1,0) = sin(phi);
  rot(1,1) = cos(phi);
  rot(1,2) = 0;
  rot(2,0) = 0;
  rot(2,1) = 0;
  rot(2,2) = 1;
  rot_T = rot.transpose();
  Acts::Vector3 pos_R = rot * pos;
  Acts::ActsMatrix<3,3> rotCov = rot * posCov * rot_T;
  dca3d_xy= pos_R(0);
  dca3d_z = pos_R(2);
  dca3d_xy_error = sqrt(rotCov(0,0));
  dca3d_z_error = sqrt(rotCov(2,2));
}

//____________________________________________________________________________..
//
//	this method produces several different "dedx" values
//		70% truncated mean 
//		85% truncated mean 
//		100% mean
//		mean-0.2*sig
//	also saved available is first 3 moments: mean, variance (rms^2), skewness
//
//double Collect::calc_dedx(TrackSeed* tpcseed, TrkrClusterContainer* clustermap, PHG4TpcGeomContainer* tpcGeom){
//
std::vector<double> Collect::calc_dedx(TrackSeed* tpcseed, TrkrClusterContainer* clustermap, PHG4TpcGeomContainer* tpcGeom){
//
	//
	if (!clustermap){
		cout<<"no cluster map... cannot continue"<<endl;
		return {};
	}
	if (!tpcGeom){
		cout<<"no tpc geom container (TPCGEOMCONTAINER)... cannot continue"<<endl;
		return {};
	}
	//
	std::vector<TrkrDefs::cluskey> clusterKeys;
	clusterKeys.insert(clusterKeys.end(), tpcseed->begin_cluster_keys(), tpcseed->end_cluster_keys());
	std::vector<float> dedxlist;
	for (unsigned long cluster_key : clusterKeys){
		auto detid = TrkrDefs::getTrkrId(cluster_key);
		if (detid != TrkrDefs::TrkrId::tpcId){ continue; }	 // the micromegas clusters are added to the TPC seeds
		TrkrCluster* cluster 				= clustermap->findCluster(cluster_key);
		if (!cluster) continue; 	//!!!!! PROTECTION DST MISMATCH !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		unsigned int layer_local 			= TrkrDefs::getLayer(cluster_key);
		PHG4TpcGeom* GeoLayer_local = tpcGeom->GetLayerCellGeom(layer_local);
		if (!GeoLayer_local) continue; 	//!!!!! PROTECTION: layer not in this geometry container !!!!!
		float adc 		= cluster->getAdc();
		float thick 	= GeoLayer_local->get_thickness();
		float r 		= GeoLayer_local->get_radius();
		float alpha 	= (r * r) / (2 * r * TMath::Abs(1.0 / tpcseed->get_qOverR()));
		float beta 		= atan(tpcseed->get_slope());
		float alphacorr	= cos(alpha);
		float betacorr	= cos(beta);		// betacorr is exactly the same as 1./cosh(tpcseed->get_eta())		
//?		if( alphacorr<0 || alphacorr >4 ){ alphacorr=4; }
//?		if( betacorr <0 || betacorr  >4 ){ betacorr =4; }
		adc	/= thick;
		adc	*= alphacorr;
		adc	*= betacorr;
		dedxlist.push_back(adc);
//if (DEBUG) cout<<"DEDX: got a cluster? adc= "<<adc<<endl;
	}
//if (DEBUG) cout<<"DEDX: done clus loop. dedxsize= "<<dedxlist.size()<<endl;
	if (dedxlist.size() < 1){
		return {};
	}
//if (DEBUG) cout<<"DEDX: sorting... "<<endl;
	sort(dedxlist.begin(), dedxlist.end());
//if (DEBUG) cout<<"DEDX: sorted size= "<<dedxlist.size()<<endl;
	//
	int trunc_max	= (int)(dedxlist.size() * 0.70);
	double dedx70n	= 0.;
	double dedx70s	= 0.;
	for (int i = 0; i < trunc_max; ++i){
		dedx70s += dedxlist.at(i);
		dedx70n++;
	}
	if (dedx70n>0) dedx70s /= dedx70n;
	//
	return { dedx70n, dedx70s };
	//
}

//____________________________________________________________________________..
// STAR-style Splitting Level (SL) split-track logic - Y. Adams et al. (STAR),
// nucl-ex/0411036 sec. 2.4.1 - using real TPC layer (pad-row) occupancy from
// fTlayermask (persisted; see Collect.h), masked here to just the TPC bits
// (7-54): MVTX/INTT bits are deliberately excluded from the SL arithmetic
// because a real split pair shares one common silicon seed by construction
// (README_SplitTracks.md sec. 3), so those bits are identical for both
// fragments and would only dilute/bias the TPC-specific SL value, not
// discriminate anything.
//
// This function exists SOLELY to support fKillSplitTracks's own production-
// time culling of SIBLING (same trigger-frame) pairs - nothing it computes is
// written to the tree. Mixed-event pairs are structurally invisible to
// Collect (two different trigger frames, processed at two different times),
// so outread_stream/CalcRm.cxx must implement this identical SL(i,j)
// calculation itself from fTlayermask to cover mixed pairs anyway (see README
// sec. 8) - once that general function exists there, it also covers the
// sibling case, making a separately-persisted sibling-only answer here purely
// redundant (and a second, possibly-diverging "which pairs are split"
// bookkeeping to keep in sync). If fKillSplitTracks stays false (the
// default), this function is a no-op past the pairing loop.
//
// Pairing is restricted to same reconstructed charge: splitting is one real
// particle refit as two tracks, so it can never flip charge (matches STAR's
// own LS-only scope for this cut). The (deta,dphi) gate below is purely a
// computational shortcut to skip obviously-unrelated pairs before the
// popcount - SL itself is what actually discriminates split tracks, and
// Collect never thresholds on SL's value to decide anything.
void Collect::FlagSplitTracks(){
	//
	if (!fKillSplitTracks) return;
	//
	constexpr uint64_t TPC_LAYER_MASK	= ((1ULL<<48)-1) << 7;	// bits 7-54 set
	const double DETA_GATE	= 0.01;		// = outread_loop.cxx's already-validated window (README_SplitTracks.md sec. 3/5) -
	const double DPHI_GATE	= 0.03;		// the CF spike this whole investigation started from is ~single-bin sharp, so there is
										// no evidence real splits extend beyond this; deliberately NOT opened up further.
	//
	std::vector<double> splitSL(fNTracks, 9.);		// local only - sentinel: no close same-charge pair found
	std::vector<int>    splitPartner(fNTracks, -1);	// local only
	std::vector<int>    splitLoser(fNTracks, 0);	// local only
	//
	for (int i=0;i<fNTracks;i++){
		uint64_t maski = fTlayermask[i] & TPC_LAYER_MASK;
		if (maski==0) continue;
		for (int j=i+1;j<fNTracks;j++){
			uint64_t maskj = fTlayermask[j] & TPC_LAYER_MASK;
			if (maskj==0) continue;
			if (fTchg[i] != fTchg[j]) continue;			// splitting cannot flip charge
			double deta	= fabs(fTeta[i]-fTeta[j]);
			double dphi	= fabs(fTphi[i]-fTphi[j]);
			if (dphi>M_PI) dphi = 2.*M_PI - dphi;
			if (deta>DETA_GATE || dphi>DPHI_GATE) continue;
			//
			int nboth	= __builtin_popcountll( maski & maskj );
			int nxor	= __builtin_popcountll( maski ^ maskj );
			int n1		= __builtin_popcountll( maski );
			int n2		= __builtin_popcountll( maskj );
			if ((n1+n2)==0) continue;
			double SL	= ( (double)nxor - (double)nboth ) / (double)(n1+n2);		// STAR eq.: SL=-0.5 -> same track split, SL=1.0 -> independent tracks
			//
			if (SL < splitSL[i]){ splitSL[i] = SL; splitPartner[i] = j; }		// keep the most split-like (lowest SL) partner per track
			if (SL < splitSL[j]){ splitSL[j] = SL; splitPartner[j] = i; }
		}	// end j loop...
	}	// end i loop...
	//
	//---- ALWAYS decide winner/loser for every found pair - independent of SL's value, gated only by having a partner at all
	for (int it=0;it<fNTracks;it++){
		int jt = splitPartner[it];
		if (jt<0) continue;
		//
		if      (fTnhitstpc[it] != fTnhitstpc[jt]){ splitLoser[it] = (fTnhitstpc[it] < fTnhitstpc[jt]) ? 1 : 0; }	// fewer TPC hits loses - directly which fragment got the smaller share of the real hits (README sec. 3)
		else if (fTquality[it]  != fTquality[jt] ){ splitLoser[it] = (fTquality[it]  > fTquality[jt])  ? 1 : 0; }	// ...tie-break: worse (higher) quality loses - direct fit-consistency metric (README sec. 5)
		// else: both tied on ntpc AND quality - no independently-justified way to pick a loser (pt is only a
		// correlated echo of ntpc, not new evidence), so leave both at their default splitLoser=0 - don't touch a
		// track without real evidence something is wrong with it.
	}	// end winner/loser loop...
	//
	for (int it=0;it<fNTracks;it++){
		if (!splitLoser[it]) continue;
		if (fTindv0[it] >= 0) continue;					// never kill a V0 daughter
		fTprimary[it] = -1;								// sentinel already understood downstream as "skip this track"
	}	// end kill loop...
	//
}

//____________________________________________________________________________..
//---- reset any counters/arrays used in/for single events
int Collect::ResetEvent(PHCompositeNode *topNode){
	//std::cout << "Collect::ResetEvent -- Resetting internal structures, prepare for next event" << std::endl;
	fNTracks	= 0;
	return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
//---- called at end of sim run or end of entire job
int Collect::EndRun(const int runnumber){
	std::cout << "Collect::EndRun -- Ending Run for Run " << runnumber << std::endl;
	return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
//
//	take a specific tprofile and find the calorimeter tower edges in eta
//
void Collect::DoEdgesEta( TProfile* hmap, std::vector<double>& edges, TString layerstr ){
	//
	edges.clear();
	if (!hmap){ cout<<"Collect::DoEdgesEta -- no map data. "<<endl; return; }
	//
	//---- get some info from map hists...
	int nbins		= hmap->GetNbinsX();
	double detamin	= fabs(hmap->GetBinContent(2)-hmap->GetBinContent(1));
	double detamax	= fabs(hmap->GetBinContent(nbins)-hmap->GetBinContent(nbins-1));
	for (int ib=1;ib<=hmap->GetNbinsX();ib++){
		double etac	= hmap->GetBinContent(ib);
		double deta=0.,val=0.;
		if (ib==1){ 
			val		= etac-detamin/2.;
		} else if (ib> 1){ 
			deta	= hmap->GetBinContent(ib) - hmap->GetBinContent(ib-1);
			val		= etac-deta/2.;
		}
		if (fabs(val)<1.0E-10) val=0.0;
		edges.push_back(val);
		if (ib==nbins){				// now add one more (the upper limit)
			edges.push_back(etac+detamax/2.);
		}
	}
	//	
	cout<<"\tdouble Edge"<<layerstr.Data()<<"eta["<<edges.size()<<"] = {";
	int icnt = 0;
	for (size_t i=0;i<edges.size();i++){
		cout<<edges.at(i);
		if (i<edges.size()-1) cout<<",";
		icnt++;
		if (icnt==15){ cout<<endl<<"\t\t\t\t"; icnt=0; }
	}	cout<<"};"<<endl;
	//
}

//____________________________________________________________________________..
//
//	take a specific tprofile and find the calorimeter tower edges in phi
//
void Collect::DoEdgesPhi( TProfile* hmap, std::vector<double>& edges ){
	//
	edges.clear();
	if (!hmap){ cout<<"Collect::DoEdgesEta -- no map data. "<<endl; return; }
	//
	//---- get some info from map hists...
	int nbins		= hmap->GetNbinsX();
	double dphi		= fabs(hmap->GetBinContent(12)-hmap->GetBinContent(11));
	for (int ib=1;ib<=hmap->GetNbinsX();ib++){
		double phic	= hmap->GetBinContent(ib);
		double val=0.;
		val	= phic-dphi/2.;
		if (fabs(val)<1.0E-10) val=0.0;
		edges.push_back(val);
		if (ib==nbins){				// now add one more (the upper limit)
			edges.push_back(phic+dphi/2.);
		}
	}
	//
}

//____________________________________________________________________________..
//---- called at end of all processing in this job
int Collect::End(PHCompositeNode *topNode){

	std::cout << "Collect::End -- This is the End... " << std::endl;

	if (CALODETAIL){
		std::vector<double> EdgesEM_eta;	
		DoEdgesEta( hmapEM_ieta, EdgesEM_eta, TString("EM") );
		std::vector<double> EdgesIH_eta;	
		DoEdgesEta( hmapIH_ieta, EdgesIH_eta, TString("IH") );
		std::vector<double> EdgesOH_eta;	
		DoEdgesEta( hmapOH_ieta, EdgesOH_eta, TString("OH") );
		std::vector<double> EdgesEM_phi;	
		DoEdgesPhi( hmapEM_iphi, EdgesEM_phi );
		std::vector<double> EdgesIH_phi;	
		DoEdgesPhi( hmapIH_iphi, EdgesIH_phi );
		std::vector<double> EdgesOH_phi;	
		DoEdgesPhi( hmapOH_iphi, EdgesOH_phi );
	}
	
	//---- write and close...
	std::cout << "Collect::End -- This is the End...  Writing root objects... " << std::endl;
	fout->cd();	
	TString origtitle	= hnvtx_frame->GetTitle();
	TString newtitle	= origtitle + TString(Form(", Offset= %d, RunNum= %d",fEvtSeqFirst,fRunNum));
	hnvtx_frame->SetTitle(newtitle);
	hnvtx_frame->Write();
	hdXing->Write();
	hech_em->Write();
	hech_ih->Write();
	hech_oh->Write();
	hech_ep->Write();
	//
	if (CALODETAIL) {
		hmapEM_eta->Write();
		hmapEM_phi->Write();
		hmapIH_eta->Write();
		hmapIH_phi->Write();
		hmapOH_eta->Write();
		hmapOH_phi->Write();
		hmapEM_eta2->Write();
		hmapEM_phi2->Write();
		hmapIH_eta2->Write();
		hmapIH_phi2->Write();
		hmapOH_eta2->Write();
		hmapOH_phi2->Write();
		hmapEM_ieta->Write();
		hmapEM_iphi->Write();
		hmapIH_ieta->Write();
		hmapIH_iphi->Write();
		hmapOH_ieta->Write();
		hmapOH_iphi->Write();
		//for (int im=0;im<MatchCounter;im++){
		//	hmatch_EMetaphiE[im]->Write();
		//	hmatch_HCetaphiE[im]->Write();
		//}
		for (int iev=0;iev<MAXEVTDETAIL;iev++){
			for (int ilayer=0;ilayer<3;ilayer++){
				hlayer[ilayer][iev]			->Write();
				hlayer_efDET[ilayer][iev]	->Write();
			}
			hPTOT_etaDETem[iev]	->Write();
			hPTOT_phiDETem[iev]	->Write();
			hPTOT_etaDETih[iev]	->Write();
			hPTOT_phiDETih[iev]	->Write();
			hPTOT_etaDEToh[iev]	->Write();
			hPTOT_phiDEToh[iev]	->Write();
		}
		hPedestalEM	->Write();
		hPedestalIH	->Write();
		hPedestalOH	->Write();
		for (int iet=0;iet<96;iet++){
			for (int ifi=0;ifi<256;ifi++){
					hEevt_em[iet][ifi]->Write();
				if (iet<24&&ifi<64){
					hEevt_ih[iet][ifi]->Write();
					hEevt_oh[iet][ifi]->Write();
				}
			}
		}
	}	// end CALODETAIL
	//
	// DIAGTEMP-20260920: throwaway merged-track exploration output - see README_SplitTracks.md sec.12b discussion
	fout->cd();
	std::cout<<"DIAGTEMP-20260920: nTracks="<<sDiagNTracks20260920<<" nClusters="<<sDiagNClusters20260920
			<<" nSharedKeyInstances="<<sDiagNSharedKeyInstances20260920<<" nEventsWithSharedKey="<<sDiagNEventsWithSharedKey20260920
			<<" nClosePairsChecked="<<sDiagNClosePairsChecked20260920<<" nClosePairsSharedKeyToo="<<sDiagNClosePairsSharedKeyToo20260920
			<<std::endl;
	std::cout<<"DIAGTEMP-20260920 gap-check: close(dist<2cm) asym="<<sDiagNGapAsymClose20260920<<"/"<<sDiagNGapChecksClose20260920
			<<" far(dist>10cm) asym="<<sDiagNGapAsymFar20260920<<"/"<<sDiagNGapChecksFar20260920
			<<std::endl;
	if (sDiagAdcAll20260920)       sDiagAdcAll20260920->Write();
	if (sDiagAdcSharedKey20260920) sDiagAdcSharedKey20260920->Write();
	if (sDiagAdcClosePair20260920) sDiagAdcClosePair20260920->Write();
	if (sDiagDist20260920)         sDiagDist20260920->Write();
	if (sDiagAdcVsDist20260920)    sDiagAdcVsDist20260920->Write();
	if (sDiagGapRateVsDist20260920) sDiagGapRateVsDist20260920->Write();
	if (sDiagDistSameSector20260920) sDiagDistSameSector20260920->Write();
	if (sDiagDistDiffSector20260920) sDiagDistDiffSector20260920->Write();
	if (sDiagGapRateVsDistSameSector20260920) sDiagGapRateVsDistSameSector20260920->Write();
	if (sDiagGapRateVsDistDiffSector20260920) sDiagGapRateVsDistDiffSector20260920->Write();
	//
	outTree->Write();
	fout->Close();
	std::cout << "Collect::End -- This is the End...  rootfile closed!" << std::endl;
	return Fun4AllReturnCodes::EVENT_OK;
	//
}

//____________________________________________________________________________..
//---- not really used...
int Collect::Reset(PHCompositeNode *topNode){
	std::cout << "Collect::Reset -- resetting..." << std::endl;
	return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
void Collect::Print(const std::string &what) const{
	std::cout << "Collect::Print -- Printing info for " << what << std::endl;
}

