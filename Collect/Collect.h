//  -*- C++ -*-.
#ifndef COLLECT_H
#define COLLECT_H

#include <trackbase/ActsGeometry.h>
#include <trackbase/ClusterErrorPara.h>
#include <trackbase/TrkrDefs.h>
#include <tpc/TpcGlobalPositionWrapper.h>
#include <fun4all/SubsysReco.h>
#include <TFile.h>
#include <TTree.h>
#include <TH1.h>
#include <TH2.h>
#include <TGraph.h>
#include <TProfile.h>
#include <TProfile2D.h>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <limits>
#include <string>

class RunHeader;
class EventHeader;
class TrkrCluster;
class PHCompositeNode;
class ActsGeometry;
//class SvtxTrackMap_v2;		//
//class SvtxTrack_v4;			//
class SvtxTrackMap;			//
class SvtxTrack;			//
class SvtxVertexMap;		//
class SvtxVertex;			//
class GlobalVertexMap;		//
class GlobalVertex;			//
class TrackSeed;
class TrkrClusterContainer;
class TrkrHitSetContainer;
class PHG4TpcGeomContainer;
class PHG4CylinderGeomContainer;
class TpcDistortionCorrection;
class TpcDistortionCorrectionContainer;
class TTree;
class TFile;
class KFParticle_Container;

class Collect : public SubsysReco
{
 public:

	Collect(const std::string &name = "Collect");
	~Collect() override;
	int Init(PHCompositeNode *topNode) override;
	int InitRun(PHCompositeNode *topNode) override;
	int process_event(PHCompositeNode *topNode) override;
	int ResetEvent(PHCompositeNode *topNode) override;
	int EndRun(const int runnumber) override;
	int End(PHCompositeNode *topNode) override;
	int Reset(PHCompositeNode * /*topNode*/) override;
	void Print(const std::string &what = "ALL") const override;
	//
	void outfileName(const std::string &name) { 
						foutfileName = name; 
						std::cout<<"Collect::outfileName -- "<<foutfileName<<std::endl; 
					}
	void setRequireXingZero(bool value){
						fRequireXingZero	= value;
						if ( fRequireXingZero){ std::cout<<"Collect::fRequireXingZero is TRUE "<<std::endl; } else
						if (!fRequireXingZero){ std::cout<<"Collect::fRequireXingZero is FALSE"<<std::endl; }
					}
	void setAddTowersToTrackTree(bool value){
						fAddTowersToTrackTree		= value;
						if ( fAddTowersToTrackTree){ std::cout<<"Collect::AddTowersToTrackTree is TRUE "<<std::endl; } else
						if (!fAddTowersToTrackTree){ std::cout<<"Collect::AddTowersToTrackTree is FALSE"<<std::endl; }
					}
	void setVerbosity(int iverb){ 
						kVerbosity = iverb; 
						     if (kVerbosity==0){ DEBUG=false; DEBUG2=false; }
						else if (kVerbosity==1){ DEBUG=true;  DEBUG2=false; }
						else if (kVerbosity> 1){ DEBUG=true;  DEBUG2=true;  }
					}
	void setSegment(int seg){
						fSegment = seg;
					}
	// Take "segment" per event from the filename the named input manager is
	// currently reading (overrides setSegment) - for jobs reading several
	// segments. Empty (default) = keep the fixed setSegment() value.
	void setSegmentFromInputManager(const std::string &name){
						fSegmentInputManager = name;
					}
	void setKillSplitTracks(bool value){
						fKillSplitTracks	= value;
						if ( fKillSplitTracks){ std::cout<<"Collect::fKillSplitTracks is TRUE "<<std::endl; } else
						if (!fKillSplitTracks){ std::cout<<"Collect::fKillSplitTracks is FALSE"<<std::endl; }
					}
	//
	void calc3DDCA(SvtxTrack* track, SvtxVertexMap* vertexmap, unsigned int vtxid,
		 double& dca3d_xy, double& dca3d_xy_error,
		 double& dca3d_z , double& dca3d_z_error );
	// 
	std::vector<double> calc_dedx(TrackSeed *tpcseed,
				TrkrClusterContainer *clusters, 
				PHG4TpcGeomContainer *tpcGeom);
	//
	int GetKFP( PHCompositeNode *topNode,
				 const int parentpdgid,
				 const std::string &containername,
				 const std::string &ctrackmapname );
	//
	// STAR-style ("SL", nucl-ex/0411036 sec. 2.4.1) split-track logic, using the
	// persisted per-track layer-occupancy mask (fTlayermask) - see
	// README_SplitTracks.md sec. 5-8. SL/pair-finding/winner-loser are computed
	// here purely to support fKillSplitTracks's own production-time culling of
	// SIBLING (same trigger-frame) pairs - none of it is written to the tree as
	// separate branches, since mixed-event pairs (which this function structurally
	// cannot see) also need the identical calculation redone downstream anyway;
	// outread_stream must implement its own general SL(i,j) from fTlayermask for
	// both sibling and mixed pairs regardless of what Collect does internally, so
	// there is no benefit to also persisting Collect's sibling-only, gate-frozen
	// answer (see README sec. 8). Called once per tree row, after the track loop
	// fills fT*[0..fNTracks-1], before Fill(). Never touches a V0 daughter
	// (fTindv0>=0), and does nothing at all unless fKillSplitTracks is true.
	void FlagSplitTracks();
	//
	void DoEdgesEta( TProfile* h, std::vector<double>& edges, TString str );
	void DoEdgesPhi( TProfile* h, std::vector<double>& edges );
	int GetIETA( int layer, double e );
	int GetIPHI( int layer, double f );
	//
 private:
 	//
	static const bool CALODETAIL = false;	// enables extra plots, BIG
	//
   	static const int MAXNTRACKS		= 500;
  	static const int MAXNV0S		=  50;
	static const int MAXEVTDETAIL	=  20;
	double THRESH_em;
	double THRESH_ih;
	double THRESH_oh;
	double THRESH_ep;
	int Kshort_PDGID;
	int Lambda_PDGID;
	int ALambda_PDGID;
	//
	TH1D*	hnvtx_frame;
 	TH1D*	hdXing;
	TH1D*	hech_em;
	TH1D*	hech_ih;
	TH1D*	hech_oh;
	TH1D*	hech_ep;
	TProfile*	hmapEM_eta;
	TProfile*	hmapEM_phi;
	TProfile*	hmapIH_eta;
	TProfile*	hmapIH_phi;
	TProfile*	hmapOH_eta;
	TProfile*	hmapOH_phi;
	TProfile*	hmapEM_eta2;
	TProfile*	hmapEM_phi2;
	TProfile*	hmapIH_eta2;
	TProfile*	hmapIH_phi2;
	TProfile*	hmapOH_eta2;
	TProfile*	hmapOH_phi2;
	TProfile*	hmapEM_ieta;
	TProfile*	hmapEM_iphi;
	TProfile*	hmapIH_ieta;
	TProfile*	hmapIH_iphi;
	TProfile*	hmapOH_ieta;
	TProfile*	hmapOH_iphi;
// 	int			MatchCounter;
// 	TH2D*		hmatch_EMetaphiE[100];
// 	TH2D*		hmatch_HCetaphiE[100];
// 	TGraph*		gmatch_EMetaphi[100];
// 	TGraph*		gmatch_HCetaphi[100];
	TH2D*	hlayer[3][MAXEVTDETAIL];
	TH2D*	hlayer_efDET[3][MAXEVTDETAIL];
	TH1D*	hPTOT_etaDETem[MAXEVTDETAIL];
	TH1D*	hPTOT_phiDETem[MAXEVTDETAIL];
	TH1D*	hPTOT_etaDETih[MAXEVTDETAIL];
	TH1D*	hPTOT_phiDETih[MAXEVTDETAIL];
	TH1D*	hPTOT_etaDEToh[MAXEVTDETAIL];
	TH1D*	hPTOT_phiDEToh[MAXEVTDETAIL];
	TProfile2D*	hPedestalEM;
	TProfile2D*	hPedestalIH;
	TProfile2D*	hPedestalOH;
// 	TProfile2D*	hPedestalCorrEM;
// 	TProfile2D*	hPedestalCorrIH;
// 	TProfile2D*	hPedestalCorrOH;
	TH1D* hEevt_em[96][256];
	TH1D* hEevt_ih[24][64];
	TH1D* hEevt_oh[24][64];
 	//
	TpcGlobalPositionWrapper m_globalPositionWrapper;	// crossing+distortion-corrected TPC global position - README_SplitTracks.md sec.12b
	bool	fRequireXingZero;
	bool	fAddTowersToTrackTree;
	bool	fHasCaloGeo;
	bool	fKillSplitTracks;		// see FlagSplitTracks() / README_SplitTracks.md; default off until validated on real data
	double	PTLL;		// track pT lower limit (usually 0.1)
	double	fPrimaryDCAxyLimit;		// |dca3d_xy| limit (cm) for the "primary" track flag
	double	fPrimaryDCAzLimit;		// |dca3d_z|  limit (cm) for the "primary" track flag
	//
	int		kVerbosity;
	bool	DEBUG;		// basic verbosity
	bool	DEBUG2;		// enhanced verbosity
 	TString foutfileName;
 	TFile*	fout;
 	TTree*	outTree;
 	int		fRunNum;
 	int		fSegment;
 	std::string	fSegmentInputManager;	// see setSegmentFromInputManager()
 	int		fEvtSeq;
 	int		fEvtSeqFirst;
 	int		ievtSeen;
 	int		ntpcmin;
 	int		nmvtmin;
 	int		nintmin;
	//
	double caloRadiusInnerEMCal;
	double caloRadiusInnerOHCal;
	double caloRadiusInnerIHCal;
	double caloThicknessEMCal;
	double caloThicknessOHCal;
	double caloThicknessIHCal;
	double caloRadiusOuterEMCal;
	double caloRadiusOuterOHCal;
	double caloRadiusOuterIHCal;
	double caloRadiusMidEMCal;
	double caloRadiusMidOHCal;
	double caloRadiusMidIHCal;
	//
	int sumNfoundKS;
	int sumNfoundLA;
	int sumNfoundAL;
	//
	int 			V0Crossing[MAXNV0S]	;
	int 			V0pid[MAXNV0S]		;
	unsigned int 	V0Track1id[MAXNV0S];
	unsigned int 	V0Track2id[MAXNV0S];
	double			V0DecayX[MAXNV0S];	// daughter-pair vertex position (NOT the mother's own GetX/Y/Z! -
	double			V0DecayY[MAXNV0S];	// see 2026-09-17 dev-log entry: the mother KFParticle is transported
	double			V0DecayZ[MAXNV0S];	// to the production vertex before Collect ever reads it, so its own
										// position is ~the PV, not the decay vertex - daughters are not
										// transported and still sit at the decay vertex where they were fit.
	//
	std::vector<bool> fTtriggerVector;
	ULong64_t	fTbco;					// GL1 BCO of this trigger frame (absolute beam clock) - lets outread_stream tell whether two
										// rows' trigger frames could overlap in time; ~0ULL if no GL1RAWHIT (README_SplitTracks.md sec.12a.6)
 	double  fTTotE_em;
 	double  fTTotE_ih;
 	double  fTTotE_oh;
 	double  fTTotE_ioh;
 	double  fTTotE_epd;
 	int		fTnvtx;
 	int		fTvtxntr;
 	double	fTvtxx;
 	double	fTvtxy;
 	double	fTvtxz;
 	double	fTvtxzGlob;
 	int		fTcrossing;
 	//
 	int 	fNTracks;
	Char_t	fTpid[MAXNTRACKS];		// tag for v0 daughters!  0=unknown, 1=pi, 2=K, 3=p - README_SplitTracks.md sec.12c: was Int_t, now Char_t (signed, small range)
	Short_t	fTindv0[MAXNTRACKS];	// index of v0 in tree v0 arrays, -1 if this track not a daughter of a found v0 - sec.12c: was Int_t, now Short_t
	Char_t	fTprimary[MAXNTRACKS];	// =1 is matched to hottest vertex, =2 if matched to /a/ vertex, =0 otherwise, =-1 killed by FlagSplitTracks() - sec.12c: was Int_t, now Char_t (signed - -1 is real)
	Float_t	fTquality[MAXNTRACKS];	// sec.12c: was Double_t, now Float_t (throughout this block)
	Float_t	fTchisq[MAXNTRACKS];
	Float_t	fTchg[MAXNTRACKS];
	double	fTpx[MAXNTRACKS];
	double	fTpy[MAXNTRACKS];
	double	fTpz[MAXNTRACKS];
	Float_t	fTptot[MAXNTRACKS];
	Float_t	fTeta[MAXNTRACKS];
	Float_t	fTphi[MAXNTRACKS];
	Float_t	fTpt[MAXNTRACKS];
	Float_t	fTseedeta[MAXNTRACKS];
	Float_t	fTseedphi[MAXNTRACKS];
	Float_t	fTseedpt[MAXNTRACKS];
	UChar_t	fTnhitstpc[MAXNTRACKS];	// sec.12c: was Int_t, now UChar_t (always >=0, <=48)
	UChar_t	fTnhitsmvt[MAXNTRACKS];
	UChar_t	fTnhitsint[MAXNTRACKS];
	Float_t	fTdcaxy[MAXNTRACKS];
	Float_t	fTdcaz[MAXNTRACKS];
	//
	UChar_t	fTdedx70n[MAXNTRACKS];	// sec.12c: was Int_t, now UChar_t
	Float_t	fTdedx70s[MAXNTRACKS];	// sec.12c: was Double_t, now Float_t
	Float_t	fTdedxKFP[MAXNTRACKS];	// TrackAnalysisUtils::calc_dedx() - the exact function KFParticle_Tools::get_dEdx()
									// calls internally for the usePID() dE/dx cut - written alongside our own
									// dedx70s as a direct apples-to-apples cross-check (2026-09-18)
	//
	uint64_t	fTlayermask[MAXNTRACKS];	// one bit per absolute TrkrDefs layer (0-2 MVTX, 3-6 INTT, 7-54 TPC) with a cluster on this track -
										// persisted (branch "layermask") so outread_stream can compute STAR-style SL itself for ANY pair
										// (sibling or mixed - see README_SplitTracks.md sec. 8); Collect's own internal use (FlagSplitTracks(),
										// for fKillSplitTracks) masks this to the TPC bits only before running the SL arithmetic.
	TrkrDefs::cluskey fTsiclukey[MAXNTRACKS][7];	// raw silicon cluster key per absolute Si layer (slot index == layermask's bit 0-6,
										// 0-2 MVTX / 3-6 INTT); sentinel ~0ULL for "no cluster on this layer" (0 itself is NOT safe -
										// see README_SplitTracks.md sec. 12.3). Additive next to layermask: lets outread_stream check
										// literal shared-cluster identity between two tracks, not just coverage-pattern agreement
										// (README_SplitTracks.md sec. 12).
	// Merged-track ("STAR FMH"-adjacent) support - README_SplitTracks.md sec.12b.
	// Compact per-TPC-layer local position, slot index = absolute layer - 7 (0-47,
	// TPC layers 7-54). All three arrays share one sentinel: tpcsector==255 means
	// "no TPC cluster on this layer" - tpcarclen/tpcz are don't-care in that case,
	// they do NOT carry an independent sentinel. Real merging is only physically
	// possible within one TPC sector (sectors are separated by dead-space frames -
	// section 12b design discussion, 2026-09-20), so two tracks' local positions
	// are only meaningfully comparable when their tpcsector values match.
	UChar_t	fTtpcsector[MAXNTRACKS][48];	// TpcDefs sector*2+side, 0-23; 255 = no cluster on this layer (sentinel)
	Char_t	fTtpcarclen[MAXNTRACKS][48];	// local arc-length within that sector, signed, 2.0 mm/count (fixed scale,
										// sized for the outermost-layer sector half-width with margin - same scale
										// for every layer, no per-layer geometry lookup needed downstream)
	Short_t	fTtpcz[MAXNTRACKS][48];		// global z, signed, 0.1 mm/count (huge margin vs the ~110cm TPC half-length)
 	//
	double	  fTstateEM_eta[MAXNTRACKS];	
	double	  fTstateEM_phi[MAXNTRACKS];	
	double	    fTstateEM_x[MAXNTRACKS];	
	double	    fTstateEM_y[MAXNTRACKS];	
	double	    fTstateEM_z[MAXNTRACKS];	
	double	fTegridEM[MAXNTRACKS][7][7];
	double	fTegridEMsum7x7[MAXNTRACKS];
	double	fTeopEM[MAXNTRACKS];	// EMCal 7x7-sum / track momentum ("E/p")
	double	  fTstateIH_eta[MAXNTRACKS];
	double	  fTstateIH_phi[MAXNTRACKS];	
	double	    fTstateIH_x[MAXNTRACKS];	
	double	    fTstateIH_y[MAXNTRACKS];	
	double	    fTstateIH_z[MAXNTRACKS];	
	double	fTegridIH[MAXNTRACKS][7][7];
	double	fTegridIHsum7x7[MAXNTRACKS];
	double	fTeopIH[MAXNTRACKS];	// IHCal 7x7-sum / track momentum ("E/p") - internal ingredient for eopHC/eopCA, not itself a branch
	double	  fTstateOH_eta[MAXNTRACKS];
	double	  fTstateOH_phi[MAXNTRACKS];	
	double	    fTstateOH_x[MAXNTRACKS];	
	double	    fTstateOH_y[MAXNTRACKS];	
	double	    fTstateOH_z[MAXNTRACKS];	
	double	fTegridOH[MAXNTRACKS][7][7];
	double	fTegridOHsum7x7[MAXNTRACKS];
	double	fTeopOH[MAXNTRACKS];	// OHCal 7x7-sum / track momentum ("E/p") - internal ingredient for eopHC/eopCA, not itself a branch
	double	fTeopHC[MAXNTRACKS];	// (IHCal+OHCal) 7x7-sum / track momentum - "E/p" for hadronic calorimetry combined
	double	fTeopCA[MAXNTRACKS];	// (EMCal+IHCal+OHCal) 7x7-sum / track momentum - "E/p" for all 3 calorimeters combined
	//
	double	    fTstateTI_x[MAXNTRACKS];	
	double	    fTstateTI_y[MAXNTRACKS];	
	double	    fTstateTI_z[MAXNTRACKS];	
	double	    fTstateTO_x[MAXNTRACKS];	
	double	    fTstateTO_y[MAXNTRACKS];	
	double	    fTstateTO_z[MAXNTRACKS];	
 	//
	int		nv0tf;
	bool	fTv0TFuseit[MAXNV0S];		// internal only
	int		fTv0TFxing[MAXNV0S];		// internal only
	int		fTv0TFpid[MAXNV0S];
	double	fTv0TFx[MAXNV0S];
	double	fTv0TFy[MAXNV0S];
	double	fTv0TFz[MAXNV0S];
	double	fTv0TFpx[MAXNV0S];
	double	fTv0TFpy[MAXNV0S];
	double	fTv0TFpz[MAXNV0S];
	double	fTv0TFptot[MAXNV0S];
	double	fTv0TFeta[MAXNV0S];
  	double	fTv0TFphi[MAXNV0S];
	double	fTv0TFpt[MAXNV0S];
	double	fTv0TFene[MAXNV0S];
	double	fTv0TFmass[MAXNV0S];
	double	fTv0TFctau[MAXNV0S];
	double	fTv0TFdecaylen[MAXNV0S];
	double	fTv0TFchi2ndf[MAXNV0S];
	int		fTv0TFindtr1[MAXNV0S];	// index of this v0's track1 in tracks tree!
	int		fTv0TFindtr2[MAXNV0S];	// index of this v0's track2 in tracks tree!
	//
	int		fTnv0;		// MAXNV0S
	int		fTv0pid[MAXNV0S];
	double	fTv0x[MAXNV0S];
	double	fTv0y[MAXNV0S];
	double	fTv0z[MAXNV0S];
	double	fTv0px[MAXNV0S];
	double	fTv0py[MAXNV0S];
	double	fTv0pz[MAXNV0S];
	double	fTv0ptot[MAXNV0S];
	double	fTv0eta[MAXNV0S];
  	double	fTv0phi[MAXNV0S];
	double	fTv0pt[MAXNV0S];
	double	fTv0ene[MAXNV0S];
	double	fTv0mass[MAXNV0S];
	double	fTv0ctau[MAXNV0S];
	double	fTv0decaylen[MAXNV0S];
	double	fTv0chi2ndf[MAXNV0S];
	double	fTv0dira[MAXNV0S];		// cos(pointing angle): V0 momentum vs (decay vtx - PV)
	double	fTv0pvdca[MAXNV0S];		// V0 trajectory DCA to the primary vertex (cm)
	double	fTv0decayx[MAXNV0S];	// daughter-pair decay vertex (see V0DecayX comment) - what dira/pvdca are computed from
	double	fTv0decayy[MAXNV0S];
	double	fTv0decayz[MAXNV0S];
	int		fTv0indtr1[MAXNV0S];	// index of this v0's track1 in tracks tree!
	int		fTv0indtr2[MAXNV0S];	// index of this v0's track2 in tracks tree!
	int		fV0FinalToTF[MAXNV0S];	// internal only: final (fTv0*) index -> trigger-frame (V0Track1id/etc) index, so the daughter-matching loop reads the right trigger-frame slot
	// needs v0 chi2, decayLength, ctau
	//
// 	unsigned int thisndedx;
 	//
};

#endif // COLLECT_H
