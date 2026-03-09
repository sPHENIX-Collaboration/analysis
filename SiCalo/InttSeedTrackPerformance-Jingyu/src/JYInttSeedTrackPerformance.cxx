#ifndef InttSeedTrackPerformance_cxx
#define InttSeedTrackPerformance_cxx

#include "JYInttSeedTracking.h"
#include "JYInttSeedTrackPerformance.h"

#include <TStyle.h>
#include <TCanvas.h>

#include "SPHTracKuma.h"

bool bCaloClu = true;
Int_t strockEvents = 1;

void InttSeedTrackPerformance::Loop(Int_t runNum)
{
    if (fChain == 0) return;
    
    // initial the histogram what we need
    HistInit();

    Long64_t nentries = fChain->GetEntries();
    Long64_t nbytes = 0, nb = 0;

    bool bTargetEV = false; // wether use the target event?
    std::vector<Int_t> vTargetEvents = {};

    for (Long64_t jentry=(1000*runNum); jentry<(1000*runNum + 200); jentry++) 
    {
        Long64_t tempJEntry = jentry; // store the jentry
        if(bTargetEV) jentry =  vTargetEvents.at(jentry); //用指定的runnumber
        pubEvNum = jentry;

        Long64_t ientry = LoadTree(jentry);
        if (ientry < 0) break;
        nb = fChain->GetEntry(jentry);   
        nbytes += nb;

        if(jentry%100 == 0) std::cout << "===== event" << jentry << " =====" << std::endl; // cout to check current runnumber

        // Extract the properties of primary particles, and store these data into std::vector<hitStruct> container that can be analyzed
        CheckPrimP(m_TruthParticle);
        // std::cout << "truth size = " << m_TruthParticle.size() << std::endl;

        // get track subcluster information to create hitStruct(R,z,phi,eta), then store to the correspondence vector<hitStruct>  
        ReadInttHitting(m_fMvtxHits, m_sMvtxHits, m_tMvtxHits, m_iInttHits, m_oInttHits, m_TpcHits);

        // get calo information to create hitStruct(R,z,phi,eta), then store to the correspondence vector<hitStruct>  
        if(bCaloClu) ReadCalCluHitting(m_emcalHits, m_iHCalHits, m_oHCalHits); // calo subcluster information into vector<hitStruct>
        else ReadCalHitting(m_emcalHits, m_iHCalHits, m_oHCalHits);            // elseif subtower information into vector<hitStruct>

        // std::cout << "== event No." << jentry << "  =========================== "<< std::endl;
        // ShowEventInfo();
 
        // vector<vector<hitStruct>>, vector of vector
        m_vTruthParticle.push_back(m_TruthParticle);
        m_viInttHits.push_back(m_iInttHits);
        m_voInttHits.push_back(m_oInttHits);
        m_vTpcHits.push_back(m_TpcHits);
        m_vemcalHits.push_back(m_emcalHits);
        m_viHCalHits.push_back(m_iHCalHits);
        m_voHCalHits.push_back(m_oHCalHits);   

        if(jentry%strockEvents == 0)
        {
            for(Int_t iEvent = 0; iEvent < strockEvents; iEvent++)
            {
                m_iInttHits = m_viInttHits.at(iEvent);
                m_oInttHits = m_voInttHits.at(iEvent);
                m_TpcHits = m_vTpcHits.at(iEvent);
                m_emcalHits = m_vemcalHits.at(iEvent);

                InttSeedTracking *TracKumaContainer = new InttSeedTracking(m_tracks, m_fMvtxHits, m_sMvtxHits, m_tMvtxHits, m_viInttHits.at(iEvent), m_voInttHits.at(iEvent), m_vemcalHits.at(iEvent), m_viHCalHits.at(iEvent), m_voHCalHits.at(iEvent));

                CaloEnergyQA(0, m_vemcalHits.at(iEvent));
                CaloEnergyQA(1, m_viHCalHits.at(iEvent));
                CaloEnergyQA(2, m_voHCalHits.at(iEvent));

                TrackQA(m_vTruthParticle.at(iEvent), m_vemcalHits.at(iEvent));

                m_bDecayEvent = false;
                // ShowEventInfo();
            }
            AllResetValuesForEvent(); // clear then shrink_to_fit vector of vector
        }
        PartResetValuesForEvent(); // clear then shrink_to_fit vector of hitStruct

        if(bTargetEV) jentry = tempJEntry;
    }

    WrightHists();

    std::cout << "std::vector<Int_t > vTargetEvents = {";
    for(Int_t i = 0; i < m_vTargetEvents.size(); i++) std::cout << m_vTargetEvents.at(i) << ", ";
    std::cout << "};" << std::endl;
}


void InttSeedTrackPerformance::HistInit()
{
    m_HINTTHitMap = new TH2D( "m_HINTTHitMap", "Global Hit Map;x [cm];y [cm]",
	    		    2000, -100., 100., 2000, -100., 100. );

    m_HGlobalHitMap = new TH2D( "m_HGlobalHitMap", "Global Hit Map;x [cm];y [cm]",
	    		    5000, -250., 250., 2500, 0., 250. );


    m_HMatchCalEVsHighestCalE = new TH1D( "m_HMatchCalEVsHighestCalE", \
       "EMCal Energy disparsion map;phi [rad];E [MeV]", \
       320, -1.6, 1.6);

    m_HdPhiInttdPhiECal = new TH2D( "m_HdPhiInttdPhiECal", \
       "EMCal Energy disparsion map;d#it{#phi}_{INTT} [rad];d#it{#phi}_{ECal} [rad]", \
          320, -1.6, 1.6, 320, -1.6, 1.6);
    m_HTruthPtVsdPhiInttdPhiECal = new TH2D( "m_HTruthPtVsdPhiInttdPhiECal", \
       "truth pT vs dPhi (INTT to ECal);#it{p}_{T, truth} [cm];dPhi [rad]", \
          150, 0., 15.,  6, -3., 3. );
    
    m_HTruthPtVsdPhiIIntt = new TH2D( "m_HTruthPtVsdPhiIIntt", \
       "truth pT vs dPhi (vertex to iINTT);#it{p}_{T, truth} [cm];dPhi [rad]",
	    	150, 0., 15., 6, -3., 3. );
    m_HTruthPtVsdPhiOIntt = new TH2D( "m_HTruthPtVsdPhiOIntt", \
       "truth pT vs dPhi (vertex to oINTT);#it{p}_{T, truth} [cm];dPhi [rad]",
	    	150, 0., 15.,  6, -3., 3. );


    // === pT resolution histograms  ==========
    m_HTruthPtVsSagittaPt_rough = new TH3D( "m_HTruthPtVsSagittaPt_rough", \
       "truth pT vs delta pt;#it{p}_{T} [GeV/#it{c}];#it{#delta}#it{p}_{T}/#it{p}_{T};#eta",
	    	150, 0., 15., 250, -2., 2., 20, -1., 1);
    m_HTruthPtVsSagittaPt_VtxIntt = new TH3D( "m_HTruthPtVsSagittaPt_VtxIntt", \
       "truth pT vs delta pt;#it{p}_{T} [GeV/#it{c}];#it{#delta}#it{p}_{T}/#it{p}_{T};#eta",
	    	150, 0., 15., 250, -2., 2., 20, -1., 1);
    m_HTruthPtVsSagittaPt_InttEmcal = new TH3D( "m_HTruthPtVsSagittaPt_InttEmcal", \
       "truth pT vs delta pt;#it{p}_{T} [GeV/#it{c}];#it{#delta}#it{p}_{T}/#it{p}_{T};#eta",
	    	150, 0., 15., 250, -2., 2., 20, -1., 1);
    m_HTruthPtVsSagittaPt_VtxInttEmcal = new TH3D( "m_HTruthPtVsSagittaPt_VtxInttEmcal", \
       "truth pT vs delta pt;#it{p}_{T} [GeV/#it{c}];#it{#delta}#it{p}_{T}/#it{p}_{T};#eta",
	    	150, 0., 15., 250, -2., 2., 20, -1., 1);
    m_HTruthPtVsSagittaPt_MvtxInttEmcal = new TH3D( "m_HTruthPtVsSagittaPt_MvtxInttEmcal", \
       "truth pT vs delta pt;#it{p}_{T} [GeV/#it{c}];#it{#delta}#it{p}_{T}/#it{p}_{T};#eta",
	    	150, 0., 15., 250, -2., 2., 20, -1., 1);
    m_HTruthPtVsSagittaPt_VtxMvtxInttEmcal = new TH3D( "m_HTruthPtVsSagittaPt_VtxMvtxInttEmcal", \
       "truth pT vs delta pt;#it{p}_{T} [GeV/#it{c}];#it{#delta}#it{p}_{T}/#it{p}_{T};#eta",
	    	150, 0., 15., 250, -2., 2., 20, -1., 1);

    m_HTruthPtVsFitFuncPt_IInttOInttEmcal = new TH3D( "m_HTruthPtVsFitFuncPt_IInttOInttEmcal", \
       "truth pT vs delta pt;#it{p}_{T} [GeV/#it{c}];#it{#delta}#it{p}_{T}/#it{p}_{T};#eta",
	    	150, 0., 15., 250, -2., 2., 20, -1., 1);

    m_HTruthPtVsFitFuncPt_VtxIInttEmcal = new TH3D( "m_HTruthPtVsFitFuncPt_VtxIInttEmcal", \
        "truth pT vs delta pt;#it{p}_{T} [GeV/#it{c}];#it{#delta}#it{p}_{T}/#it{p}_{T};#eta",
		150, 0., 15., 250, -2., 2., 20, -1., 1);
    m_HTruthPtVsFitFuncPt_VtxOInttEmcal = new TH3D( "m_HTruthPtVsFitFuncPt_VtxOInttEmcal", \
        "truth pT vs delta pt;#it{p}_{T} [GeV/#it{c}];#it{#delta}#it{p}_{T}/#it{p}_{T};#eta",
        150, 0., 15., 250, -2., 2., 20, -1., 1);
    m_HTruthPtVsFitFuncPt_VtxInttEmcal = new TH3D( "m_HTruthPtVsFitFuncPt_VtxInttEmcal", \
        "truth pT vs delta pt;#it{p}_{T} [GeV/#it{c}];#it{#delta}#it{p}_{T}/#it{p}_{T};#eta",
        150, 0., 15., 250, -2., 2., 20, -1., 1);
    m_HTruthPtVsFitFuncPt_MvtxIInttEmcal = new TH3D( "m_HTruthPtVsFitFuncPt_MVtxIInttEmcal", \
        "truth pT vs delta pt;#it{p}_{T} [GeV/#it{c}];#it{#delta}#it{p}_{T}/#it{p}_{T};#eta",
        150, 0., 15., 250, -2., 2., 20, -1., 1);
    m_HTruthPtVsFitFuncPt_MvtxOInttEmcal = new TH3D( "m_HTruthPtVsFitFuncPt_MVtxOInttEmcal", \
        "truth pT vs delta pt;#it{p}_{T} [GeV/#it{c}];#it{#delta}#it{p}_{T}/#it{p}_{T};#eta",
        150, 0., 15., 250, -2., 2., 20, -1., 1);
    m_HTruthPtVsFitFuncPt_MvtxInttEmcal = new TH3D( "m_HTruthPtVsFitFuncPt_MVtxInttEmcal", \
        "truth pT vs delta pt;#it{p}_{T} [GeV/#it{c}];#it{#delta}#it{p}_{T}/#it{p}_{T};#eta",
        150, 0., 15., 250, -2., 2., 20, -1., 1);


    // === theta resolution histograms  ==========
    m_HTruthPVsTheta_InttEmcal = new TH2D( "m_HTruthPVsTheta_InttEmcal", \
       "truth p vs delta pt;#it{p} [GeV/#it{c}];#it{#delta}#it{#theta}",
	    	150, 0., 15., 100, -0.5, 0.5 );
    m_HTruthPVsTheta_MvtxInttEmcal = new TH2D( "m_HTruthPVsTheta_MvtxInttEmcal", \
       "truth p vs delta p;#it{p} [GeV/#it{c}];#it{#delta}#it{#theta}",
	    	150, 0., 15., 100, -0.5, 0.5 );

    // === phi resolution histograms  ==========
    m_HTruthPVsPhi_InttEmcal = new TH2D( "m_HTruthPVsPhi_InttEmcal", \
       "truth p vs delta pt;#it{p} [GeV/#it{c}];#it{#delta}#it{#phi}",
	    	150, 0., 15., 100, -0.5, 0.5 );
    m_HTruthPVsPhi_MvtxInttEmcal = new TH2D( "m_HTruthPVsPhi_MvtxInttEmcal", \
       "truth p vs delta p;#it{p} [GeV/#it{c}];#it{#delta}#it{#phi}",
	    	150, 0., 15., 100, -0.5, 0.5 );

    // === dp histograms  ==========
    m_HTruthPVsRecoP_InttEmcal = new TH2D( "m_HTruthPVsRecoP_InttEmcal", \
       "truth p vs delta p;#it{p} [GeV/#it{c}];#it{#delta}#it{p}/#it{p}",
    	150, 0., 15., 50, -2.5, 2.5 );
    m_HTruthPVsRecoP_MvtxInttEmcal = new TH2D( "m_HTruthPVsRecoP_MvtxInttEmcal", \
       "truth p vs delta p;#it{p} [GeV/#it{c}];#it{#delta}#it{p}/#it{p}",
    	150, 0., 15., 50, -2.5, 2.5 );
    m_HTruthPVsRecoP_FitFunc = new TH2D( "m_HTruthPVsRecoP_FitFunc", \
       "truth p vs delta p;#it{p} [GeV/#it{c}];#it{#delta}#it{p}/#it{p}",
    	150, 0., 15., 50, -2.5, 2.5 );

    // === dE histograms  ==========
    m_HDE = new TH3D( "m_HDE", "truth E vs Reco E;#it{E} [MeV];#it{#delta}#it{E}/#it{E};#eta",
	    200, 0., 20., 50, -2.5, 2.5, 20, -1., 1);


    // === E/p histograms  ==========
    m_HTruthPVsEOverP_InttEmcal = new TH2D( "m_HTruthPVsEOverP_InttEmcal", \
        "truth p vs delta pt;#it{p} [GeV/#it{c}];#it{E}/#it{p}",
	    	150, 0., 15., 60, 0., 3. );
    m_HTruthPVsEOverP_MvtxInttEmcal = new TH2D( "m_HTruthPVsEOverP_MvtxInttEmcal", \
        "truth p vs delta p;#it{p} [GeV/#it{c}];#it{E}/#it{p}",
	    	150, 0., 15., 60, 0., 3. );
    m_HTruthPVsEOverP_FitFunc = new TH2D( "m_HTruthPVsEOverP_FitFunc", \
        "truth p vs delta p;#it{p} [GeV/#it{c}];#it{E}/#it{p}",
		    150, 0., 15., 60, 0., 3. );

    // === dVtx histograms  ==========
    m_dVtxXY_InttEmcal = new TH2D( "m_dVtxXY_InttEmcal", "vertexReso;d#it{X}_{(reco - truth)} [cm];d#it{Y}_{(reco - truth)} [cm];count", 1000, -50., 50.,  1000, -50., 50.);
    m_dVtxXY_MvtxInttEmcal = new TH2D( "m_dVtxXY_MvtxInttEmcal", "vertexReso;d#it{X}_{(reco - truth)} [cm];d#it{Y}_{(reco - truth)} [cm];count", 1000, -50., 50., 1000, -50., 50.);

    m_dVtxR_InttEmcal = new TH1D( "m_dVtxR_InttEmcal", "vertexReso;d#it{R}_{(reco - truth)} [cm];count", 500, 0., 50.);
    m_dVtxR_MvtxInttEmcal = new TH1D( "m_dVtxR_MvtxInttEmcal", "vertexReso;d#it{R}_{(reco - truth)} [cm];count", 500, 0., 50.);

    m_dVtxZ_InttEmcal = new TH1D( "m_dVtxZ_InttEmcal", "vertexReso;d#it{Z}_{(reco - truth)} [cm];count", 1000, -50., 50.);
    m_dVtxZ_MvtxInttEmcal = new TH1D( "m_dVtxZ_MvtxInttEmcal", "vertexReso;d#it{Z}_{(reco - truth)} [cm];count", 1000, -50., 50.);

    // == s == Calo QA Histograms ==================================
    m_HECalPhiVsE  = new TProfile2D( "m_HECalPhiVsE",  "EMCal Energy disparsion map;#phi [rad];#eta;E [MeV]", 320, -1.6, 1.6, 300, -1.5, 1.5);
    m_HIHCalPhiVsE = new TProfile2D( "m_HIHCalPhiVsE", "iHCal Energy disparsion map;#phi [rad];#eta;E [MeV]", 320, -1.6, 1.6, 300, -1.5, 1.5);
    m_HOHCalPhiVsE = new TProfile2D( "m_HOHCalPhiVsE", "oHCal Energy disparsion map;#phi [rad];#eta;E [MeV]", 320, -1.6, 1.6, 300, -1.5, 1.5);
    // == e == Calo QA  Histograms ==================================

    // == s == Magnetic shift Histograms ==================================
    m_HDPhiVsDStraightVsTruPt = new TH3D("m_HDPhiVsDStraightVsTruPt", "dPhi/dl between vertex and inner INTT;d#it{l} [cm];d#it{phi} [rad];#it{p}_{T} [GeV/#it{c}];", 150, 0., 150.,  100, -0.05, 0.05, 150, 0., 15.);

    m_HDPhiDStraight_VtxIIntt = new TH2D("m_HDPhiDStraight_VtxIIntt", "dPhi/dl between vertex and inner INTT;#it{p}_{T} [GeV/#it{c}];d#it{phi}/d#it{l} [rad/cm]", 150, 0., 15.,  100, -0.05, 0.05);
    m_HDPhiDStraight1D_VtxIIntt = new TH1D("m_HDPhiDStraight1D_VtxIIntt", "dPhi/dl between vertex and inner INTT;d#it{phi}/d#it{l} [rad/cm];count", 100, -0.05, 0.05);

    m_HDPhiDStraight_IInttOIntt = new TH2D("m_HDPhiDStraight_IInttOIntt", "dPhi/dl between vertex and inner INTT;#it{p}_{T} [GeV/#it{c}];d#it{phi}/d#it{l} [rad/cm]", 150, 0., 15.,  100, -0.05, 0.05);
    m_HDPhiDStraight1D_IInttOIntt = new TH1D("m_HDPhiDStraight1D_IInttOIntt", "dPhi/dl between vertex and inner INTT;d#it{phi}/d#it{l} [rad/cm];count", 100, -0.05, 0.05);

    m_HTruthPtVsDdPhiddStraight_IInttOInttEmcal = new TH2D("m_HTruthPtVsDdPhiddStraight_IInttOInttEmcal", "dPhi/dl between vertex and inner INTT;#it{p}_{T} [GeV/#it{c}];d#it{phi}/  d#it{l} [rad/cm]", 150, 0., 15., 100, -0.05, 0.05);

    m_HDPhiDStraight_OInttEmcal = new TH2D("m_HDPhiDStraight_OInttEmcal", "dPhi/dl between vertex and inner INTT;#it{p}_{T} [GeV/#it{c}];d#it{phi}/d#it{l} [rad/cm]", 150, 0., 15.,  100, -0.05, 0.05);
    m_HDPhiDStraight1D_OInttEmcal = new TH1D("m_HDPhiDStraight1D_OInttEmcal", "dPhi/dl between vertex and inner INTT;d#it{phi}/d#it{l} [rad/cm];count", 100, -0.05, 0.05);


    m_HDLVsTruthPtVsEta_VtxIIntt = new TH3D( "m_HDLVsTruthPtVsEta_VtxIIntt", \
        "truth pT vs shift distanse (L);L [cm];#it{p}_{T} [GeV/#it{c}];#eta",
	    1200, -10., 10., 1000, -50., 50., 20, -1., 1);
    m_HDL1D_VtxIIntt = new TH1D( "m_HDL1D_VtxIIntt", \
        "truth pT vs shift distanse (L);L [cm]; count", 200, -10., 10.);
    m_HDLVsTruthPtVsEta_IInttOIntt = new TH3D( "m_HDLVsTruthPtVsEta_IInttOIntt", \
        "truth pT vs shift distanse (L);L [cm];#it{p}_{T} [GeV/#it{c}];#eta",
	    200, -10., 10., 1000, -50., 50., 20, -1., 1);
    m_HDL1D_IInttOIntt = new TH1D( "m_HDL1D_IInttOIntt", \
        "truth pT vs shift distanse (L);L [cm]; count", 200, -10., 10.);
    m_HDLVsTruthPtVsEta_OInttEmcal = new TH3D( "m_HDLVsTruthPtVsEta_OInttEmcal", \
        "truth pT vs shift distanse (L);L [cm];#it{p}_{T} [GeV/#it{c}];#eta",
	    1000, -50., 50., 1000, -50., 50., 20, -1., 1);
    m_HDL1D_OInttEmcal = new TH1D( "m_HDL1D_OInttEmcal", \
        "truth pT vs shift distanse (L);L [cm]; count", 1000, -50., 50.);


    m_HDPhiVsTruthPtVsEta_VtxIIntt = new TH3D( "m_HDPhiVsTruthPtVsEta_VtxIIntt", \
        "truth pT vs shift distanse (L);#phi [rad];#it{p}_{T} [GeV/#it{c}];#eta",
	    200, -1., 1., 150, 0., 15., 10, -1., 1);
    m_HDPhiVsTruthPtVsEta_IInttOIntt = new TH3D( "m_HDPhiVsTruthPtVsEta_IInttOIntt", \
        "truth pT vs shift distanse (L);#phi [rad];#it{p}_{T} [GeV/#it{c}];#eta",
	    200, -1., 1., 150, 0., 15., 10, -1., 1);
    m_HDPhiVsTruthPtVsEta_OInttEmcal = new TH3D( "m_HDPhiVsTruthPtVsEta_OInttEmcal", \
        "truth pT vs shift distanse (L);#phi [rad];#it{p}_{T} [GeV/#it{c}];#eta",
	    200, -1., 1., 150, 0., 15., 10, -1., 1);

    m_HDPhiVsTruthPtVsEta_IInttOInttEmcal = new TH3D( "m_HDPhiVsTruthPtVsEta_IInttOInttEmcal", \
        "truth pT vs shift distanse (L);#phi [rad];#it{p}_{T} [GeV/#it{c}];#eta",
		200, -1., 1., 150, 0., 15., 10, -1., 1);

    m_HDPhiVsTruthPtVsEta_VtxIInttEmcal = new TH3D( "m_HDPhiVsTruthPtVsEta_VtxIInttEmcal", \
        "truth pT vs shift distanse (L);#phi [rad];#it{p}_{T} [GeV/#it{c}];#eta",
	    200, -1., 1., 150, 0., 15., 10, -1., 1);
    m_HDPhiVsTruthPtVsEta_VtxOInttEmcal = new TH3D( "m_HDPhiVsTruthPtVsEta_VtxOInttEmcal", \
        "truth pT vs shift distanse (L);#phi [rad];#it{p}_{T} [GeV/#it{c}];#eta",
	    200, -1., 1., 150, 0., 15., 10, -1., 1);
    m_HDPhiVsTruthPtVsEta_VtxInttEmcal = new TH3D( "m_HDPhiVsTruthPtVsEta_VtxInttEmcal", \
        "truth pT vs shift distanse (L);#phi [rad];#it{p}_{T} [GeV/#it{c}];#eta",
	    200, -1., 1., 150, 0., 15., 10, -1., 1);

    m_HDPhiVsTruthPtVsEta_MvtxIInttEmcal = new TH3D( "m_HDPhiVsTruthPtVsEta_MvtxIInttEmcal", \
        "truth pT vs shift distanse (L);#phi [rad];#it{p}_{T} [GeV/#it{c}];#eta",
	    200, -1., 1., 150, 0., 15., 10, -1., 1);
    m_HDPhiVsTruthPtVsEta_MvtxOInttEmcal = new TH3D( "m_HDPhiVsTruthPtVsEta_MvtxOInttEmcal", \
        "truth pT vs shift distanse (L);#phi [rad];#it{p}_{T} [GeV/#it{c}];#eta",
	    200, -1., 1., 150, 0., 15., 10, -1., 1);
    m_HDPhiVsTruthPtVsEta_MvtxInttEmcal = new TH3D( "m_HDPhiVsTruthPtVsEta_MvtxInttEmcal", \
        "truth pT vs shift distanse (L);#phi [rad];#it{p}_{T} [GeV/#it{c}];#eta",
	    200, -1., 1., 150, 0., 15., 10, -1., 1);


    m_HDPhiVsTruthPtProf_IInttOInttEmcal = new TProfile( "m_HDPhiVsTruthPtProf_IInttOInttEmcal", \
        "truth pT vs shift distanse (L);#phi [rad];#it{p}_{T} [GeV/#it{c}];", 200, -1., 1.);

    m_HDPhiVsTruthPtProf_VtxIIntt = new TProfile( "m_HDPhiVsTruthPtProf_VtxIIntt", \
        "truth pT vs shift distanse (L);#phi [rad];#it{p}_{T} [GeV/#it{c}];", 200, -1., 1.);
    m_HDPhiVsTruthPtProf_IInttOIntt = new TProfile( "m_HDPhiVsTruthPtProf_IInttOIntt", \
        "truth pT vs shift distanse (L);#phi [rad];#it{p}_{T} [GeV/#it{c}]", 200, -1., 1.);
    m_HDPhiVsTruthPtProf_OInttEmcal = new TProfile( "m_HDPhiVsTruthPtProf_OInttEmcal", \
        "truth pT vs shift distanse (L);#phi [rad];#it{p}_{T} [GeV/#it{c}]", 200, -1., 1.);

    m_HDPhiVsTruthPtProf_VtxIInttEmcal = new TProfile( "m_HDPhiVsTruthPtProf_VtxIInttEmcal", \
        "truth pT vs shift distanse (L);#phi [rad];#it{p}_{T} [GeV/#it{c}]", 200, -1., 1.);
    m_HDPhiVsTruthPtProf_VtxOInttEmcal = new TProfile( "m_HDPhiVsTruthPtProf_VtxOInttEmcal", \
        "truth pT vs shift distanse (L);#phi [rad];#it{p}_{T} [GeV/#it{c}]",200, -1., 1.);
    m_HDPhiVsTruthPtProf_VtxInttEmcal = new TProfile( "m_HDPhiVsTruthPtProf_VtxInttEmcal", \
        "truth pT vs shift distanse (L);#phi [rad];#it{p}_{T} [GeV/#it{c}]", 200, -1., 1.);

    m_HDPhiVsTruthPtProf_MvtxIInttEmcal = new TProfile( "m_HDPhiVsTruthPtProf_MvtxIInttEmcal", \
        "truth pT vs shift distanse (L);#phi [rad];#it{p}_{T} [GeV/#it{c}]",200, -1., 1.);
    m_HDPhiVsTruthPtProf_MvtxOInttEmcal = new TProfile( "m_HDPhiVsTruthPtProf_MVtxOInttEmcal", \
        "truth pT vs shift distanse (L);#phi [rad];#it{p}_{T} [GeV/#it{c}]", 200, -1., 1.);
    m_HDPhiVsTruthPtProf_MvtxInttEmcal = new TProfile( "m_HDPhiVsTruthPtProf_MVtxInttEmcal", \
        "truth pT vs shift distanse (L);#phi [rad];#it{p}_{T} [GeV/#it{c}]", 200, -1., 1.);

    m_HPtEfficiency = new TH1D( "m_HPtEfficiency", \
        "reconstrucntion efficinecy for pT;#it{p}_{T} [GeV/#it{c}];Efficiency [%]", 150, 0., 15);
    m_HTruTrackNum = new TH1D( "m_HTruTrackNum", \
        "reconstrucntion efficinecy for pT;#it{p}_{T} [GeV/#it{c}];Efficiency [%]", 150, 0., 15);
}   

// Extract the properties of primary particles, and store these data into std::vector<hitStruct> container that can be analyzed
void InttSeedTrackPerformance::CheckPrimP(std::vector<hitStruct>& vTruthParticle)
{
    // 检查粒子数量  number of TruthParticle
    Int_t numOfP = PrimaryG4P_PID->size();
    if(numOfP == 0) return;
    
    // 提取粒子属性: 对每个粒子，提取属性, pPhi, pEta, pE, pPt
    for(Int_t iP = 0; iP < numOfP; iP++)
    {
        hitStruct truthParticle;
        Double_t pPhi = PrimaryG4P_Phi->at(iP);
        Double_t pEta = PrimaryG4P_Eta->at(iP);
        Double_t pE = PrimaryG4P_E->at(iP);
        Double_t pPt = PrimaryG4P_Pt->at(iP);

        truthParticle.phi = pPhi;
        truthParticle.eta = pEta;
        truthParticle.pt = pPt;
        truthParticle.energy = pE;

        vTruthParticle.push_back(truthParticle);
    }
}

// get track subcluster information to create hitStruct(R,z,phi,eta), then store to the correspondence vector<hitStruct>  
void InttSeedTrackPerformance::ReadInttHitting(std::vector<hitStruct >& vFMvtxHits,\
   std::vector<hitStruct >& vSMvtxHits, std::vector<hitStruct >& vTMvtxHits,\
   std::vector<hitStruct >& vIInttHits, std::vector<hitStruct >& vOInttHits,\
   std::vector<hitStruct >& vTpcHits)
{
    Int_t numOfClu = trk_system->size(); // 获取所有命中点的数量, trk_system - tree branch 
    if(numOfClu == 0) return;

    // 遍历所有命中点：
    for(Int_t iClu = 0; iClu < numOfClu; iClu++)
    {
        Int_t CluSysId = trk_system->at(iClu); // system - mvtx, intt, tpc
        Int_t CluLayId = trk_layer->at(iClu);  // layer  - 0-2 , 3-6,  7 ? 

        // get cluster xyz for calculate R Phi theta(eta)
        Double_t CluX = trk_X->at(iClu); 
        Double_t CluY = trk_Y->at(iClu);
        Double_t CluZ = trk_Z->at(iClu);

        Double_t CluR = sqrt(CluX*CluX + CluY*CluY); 
        Double_t CluPhi = std::atan(CluY/CluX); // -pi/2 to pi/2, need to turn to -pi ~ pi (since 2,3rd phase cannot be get from atan)
        if((CluPhi < 0)&&(CluX < 0)) CluPhi += TMath::Pi();
        else if((CluPhi > 0)&&(CluX < 0)) CluPhi -= TMath::Pi();

        Double_t CluTheta = std::atan(CluR/CluZ);// -pi - pi
        Double_t CluEta = (CluZ/std::abs(CluZ)) * (- log(std::abs(std::tan(CluTheta/2))));

        // why need to xyz -> R phi eta -> xyz ?
        CluX = CluR*std::cos(CluPhi);
        CluY = CluR*std::sin(CluPhi);
        m_HINTTHitMap->Fill(CluX, CluY); // not only INTT? mvtx + intt + tpc

        hitStruct cluHit;
        cluHit.r = CluR;
        cluHit.z = CluZ;
        cluHit.phi = CluPhi;
        cluHit.eta = CluEta;

        if(CluSysId == 0) // mvtx
        {
            if(CluLayId == 0) vFMvtxHits.push_back(cluHit);
            else if(CluLayId == 1) vSMvtxHits.push_back(cluHit);
            else if(CluLayId == 2) vTMvtxHits.push_back(cluHit);
        }
        else if(CluSysId == 1) // intt
        {
            if((CluLayId == 3) || (CluLayId == 4)) vIInttHits.push_back(cluHit);
            else if((CluLayId == 5) || (CluLayId == 6)) vOInttHits.push_back(cluHit);
        }
        else // tpc
        {
            vTpcHits.push_back(cluHit);
        }
    }
}

// get calo subtower information to create hitStruct(R,z,phi,eta), then store to the correspondence vector<hitStruct>  
void InttSeedTrackPerformance::ReadCalHitting(std::vector<hitStruct >& vEmcalHits,\
   std::vector<hitStruct >& vIHCalHits, std::vector<hitStruct >& vOHcalHits)
{
    Int_t numOfCalTower = tower_Phi->size();
    if(numOfCalTower == 0) return;

    for(Int_t iCalTower = 0; iCalTower < numOfCalTower; iCalTower++)
    {
        Int_t calId = tower_system->at(iCalTower);

        Double_t caloX = tower_X->at(iCalTower);
        Double_t caloY = tower_Y->at(iCalTower);
        Double_t caloZ = tower_Z->at(iCalTower);
        Double_t caloPhi = tower_Phi->at(iCalTower);

        Double_t caloEta = tower_Eta->at(iCalTower);
        Double_t caloE = tower_edep->at(iCalTower);

        hitStruct caloHit;
        caloHit.r = std::sqrt(caloX*caloX + caloY*caloY);
        caloHit.z = caloZ;
        caloHit.phi = caloPhi;
        caloHit.eta = caloEta;
        caloHit.energy = caloE;

        if(calId == 0) vEmcalHits.push_back(caloHit);
        else if(calId == 1) vIHCalHits.push_back(caloHit);
        else if(calId == 2) vOHcalHits.push_back(caloHit);

        // if(caloE > 0.1){
        //    std::cout << "calE = " << caloE << ", bin(phi, eta) = " << tower_Phi_bin->at(iCalTower) << ", " << tower_Eta_bin->at(iCalTower) << std::endl;
        // }
    }
}

// get emcal cluster information to create hitStruct(R,z,phi,eta), then store to the correspondence vector<hitStruct>  
void InttSeedTrackPerformance::ReadCalCluHitting(std::vector<hitStruct >& vEmcalHits,\
   std::vector<hitStruct >& vIHCalHits, std::vector<hitStruct >& vOHcalHits)
{
    Int_t numOfCaloCluster = caloClus_Phi->size();
    if(numOfCaloCluster == 0) return;

    for(Int_t iCaloCluster = 0; iCaloCluster < numOfCaloCluster; iCaloCluster++)
    {
        Int_t calId = caloClus_system->at(iCaloCluster); // cal system: 0-emcal, 1-ihcal, 2-ohcal

        Double_t caloX = caloClus_X->at(iCaloCluster);
        Double_t caloY = caloClus_Y->at(iCaloCluster);
        Double_t caloZ = caloClus_Z->at(iCaloCluster);

        Double_t caloPhi = caloClus_Phi->at(iCaloCluster);
        Double_t caloE = caloClus_edep->at(iCaloCluster);

        hitStruct caloHit;
        caloHit.r = std::sqrt(caloX*caloX + caloY*caloY);
        caloHit.z = caloZ;
        caloHit.phi = caloPhi;

        Double_t caloTheta = std::atan(caloHit.r/caloHit.z);
        Double_t caloEta = (caloZ/std::abs(caloZ)) * (- log(std::abs(std::tan(caloTheta/2))));
        caloHit.eta = caloEta;
        caloHit.energy = caloE;

        if(calId == 0) vEmcalHits.push_back(caloHit);
        else if(calId == 1) vIHCalHits.push_back(caloHit);
        else if(calId == 2) vOHcalHits.push_back(caloHit);
    }
}


// == s == Track QA Functions  ###############################################
void InttSeedTrackPerformance::TrackQA(std::vector<hitStruct> vTruthPs, std::vector<hitStruct> vEmcalHits)
{
    std::vector<Int_t > matchiedRecoTrkId = {};
    Int_t numOfP = PrimaryG4P_PID->size();
    for(Int_t iP = 0; iP < numOfP; iP++)
    {
        m_HTruTrackNum->Fill(vTruthPs.at(iP).pt);
        if(m_tracks.size() == 0) continue;

        Int_t trkId = TruRecoMatching(vTruthPs.at(iP), m_tracks, matchiedRecoTrkId);
        if(trkId == 9999) continue;

        DeltaPtPerform(vTruthPs.at(iP), m_tracks.at(trkId));
        EstimateMagneticShift(vTruthPs.at(iP), m_tracks.at(trkId));

        Double_t truE = vTruthPs.at(iP).energy;
        Double_t recoE = m_tracks.at(trkId).getTrackE();
        m_HDE->Fill(truE, (recoE - truE)/truE, vTruthPs.at(iP).eta);
        m_HPtEfficiency->Fill(vTruthPs.at(iP).pt);
    }
}

Int_t InttSeedTrackPerformance::TruRecoMatching(hitStruct truthP, std::vector<tracKuma > vRecoTrk, std::vector<Int_t > vMatchiedRecoTrkId)
{
    Int_t matchiedRecoTrkId = 9999;
    Double_t closestDPt = 9999.;
    for(Int_t iRecoTrk = 0; iRecoTrk < vRecoTrk.size(); iRecoTrk++)
    {
        Double_t tempTruTheta = 2*atan(std::exp(-truthP.eta));
        Double_t tempRecoTheta = vRecoTrk.at(iRecoTrk).getTrackTheta();
        Double_t tempRecoPhi = 0.;
        if(vRecoTrk.at(iRecoTrk).getHitIs(4)) tempRecoPhi = vRecoTrk.at(iRecoTrk).getHitPhi(4);
        else if(vRecoTrk.at(iRecoTrk).getHitIs(5)) tempRecoPhi = vRecoTrk.at(iRecoTrk).getHitPhi(5);
        // std::cout << "recoTheta, truTheta, dTheta = " << tempRecoTheta << ", " << tempTruTheta << ", " << std::abs(tempRecoTheta - tempTruTheta) << std::endl;

        if((std::abs(tempRecoTheta - tempTruTheta) < 0.1))
        {
            // std::cout << "dPhi = " << std::abs(tempRecoPhi - truthP.phi) << std::endl;
            if((std::abs(tempRecoPhi - truthP.phi) < 0.3))
            {
                // std::cout << "dPt = " << std::abs(vRecoTrk.at(iRecoTrk).getTrackPt() - truthP.pt) << std::endl;
                if(closestDPt > std::abs(vRecoTrk.at(iRecoTrk).getTrackPt() - truthP.pt))
                {
                    bool matchedIs = std::find( vMatchiedRecoTrkId.begin(),  vMatchiedRecoTrkId.end(), iRecoTrk) !=  vMatchiedRecoTrkId.end();
                    if(matchedIs) continue;
                    closestDPt = std::abs(vRecoTrk.at(iRecoTrk).getTrackPt() - truthP.pt);
                    matchiedRecoTrkId = matchedIs;
                }
            }
        }
    }
    vMatchiedRecoTrkId.push_back(matchiedRecoTrkId);
    
    return matchiedRecoTrkId;
}

void InttSeedTrackPerformance::DeltaPtPerform(hitStruct truthP, tracKuma trk){
   InttSeedTracking truckF;

   Double_t truthPt = truthP.pt;
   Double_t truthEta = truthP.eta;

   Double_t sagittaR = 9999.;
   Double_t centerX = 9999.;
   Double_t centerY = 9999.;

   Double_t HitsXY[3][2];
   truckF.Set3PointsXY(HitsXY, trk, 0);
   truckF.RoughEstiSagittaCenter3Point(sagittaR, centerX, centerY, HitsXY);
   Double_t sagittaPt = truckF.CalcSagittaPt(sagittaR);
   Double_t tempdPt = (sagittaPt - truthPt)/truthPt;
   m_HTruthPtVsSagittaPt_rough->Fill(truthPt, tempdPt, truthEta);

   std::vector<Int_t > subDetIds_VtxIntt = {0, 4, 5};
   std::vector<Double_t > hitsR_VtxIntt = {};
   std::vector<Double_t > hitsPhi_VtxIntt = {};
   if(truckF.ReturnHitsRPhiVect(hitsR_VtxIntt, hitsPhi_VtxIntt, subDetIds_VtxIntt, trk)){
      Double_t tempHitOInttPhi = trk.getHitPhi(4);
      Double_t tempHitEmcalPhi = trk.getHitPhi(6);
      truckF.SagittaRByCircleFit(centerX, centerY, sagittaR, hitsR_VtxIntt, hitsPhi_VtxIntt, tempHitOInttPhi, tempHitEmcalPhi);
      Double_t recoPt = truckF.CalcSagittaPt(sagittaR);
      Double_t dPt = (sagittaPt - truthPt)/truthPt;
      m_HTruthPtVsSagittaPt_VtxIntt->Fill(truthPt, dPt, truthEta);
   }
   
   // Double_t HitsXY2[3][2];
   // truckF.Set3PointsXY(HitsXY2, trk, 0);
   // truckF.RoughEstiSagittaCenter3Point(sagittaR, centerX, centerY, HitsXY);

   std::vector<Int_t > subDetIds_InttEmcal = {4, 5, 6};
   std::vector<Double_t > hitsR_InttEmcal = {};
   std::vector<Double_t > hitsPhi_InttEmcal = {};
   if(truckF.ReturnHitsRPhiVect(hitsR_InttEmcal, hitsPhi_InttEmcal, subDetIds_InttEmcal, trk)){
      Double_t tempHitOInttPhi = trk.getHitPhi(4);
      Double_t tempHitEmcalPhi = trk.getHitPhi(6);
      truckF.SagittaRByCircleFit(centerX, centerY, sagittaR, hitsR_InttEmcal, hitsPhi_InttEmcal, tempHitOInttPhi, tempHitEmcalPhi);
      Double_t recoPt = truckF.CalcSagittaPt(sagittaR);
      Double_t dPt = (recoPt - truthPt)/truthPt;
      m_HTruthPtVsSagittaPt_InttEmcal->Fill(truthPt, dPt, truthEta);

      TrackOtherPropertiesWTruth(truthP, trk, sagittaR, centerX, centerY, recoPt,\
            m_HTruthPVsTheta_InttEmcal, m_HTruthPVsPhi_InttEmcal, m_HTruthPVsRecoP_InttEmcal, m_HTruthPVsEOverP_InttEmcal,\
            m_dVtxXY_InttEmcal, m_dVtxR_InttEmcal, m_dVtxZ_InttEmcal);
   }
   
   std::vector<Int_t > subDetIds_VtxInttEmcal = {0, 4, 5, 6};
   std::vector<Double_t > hitsR_VtxInttEmcal = {};
   std::vector<Double_t > hitsPhi_VtxInttEmcal = {};
   if(truckF.ReturnHitsRPhiVect(hitsR_VtxInttEmcal, hitsPhi_VtxInttEmcal, subDetIds_VtxInttEmcal, trk)){
      Double_t tempHitOInttPhi = trk.getHitPhi(4);
      Double_t tempHitEmcalPhi = trk.getHitPhi(6);
      truckF.SagittaRByCircleFit(centerX, centerY, sagittaR, hitsR_VtxInttEmcal, hitsPhi_VtxInttEmcal, tempHitOInttPhi, tempHitEmcalPhi);
      Double_t recoPt = truckF.CalcSagittaPt(sagittaR);
      Double_t dPt = (recoPt - truthPt)/truthPt;
      m_HTruthPtVsSagittaPt_VtxInttEmcal->Fill(truthPt, dPt, truthEta);
   }

   std::vector<Int_t > subDetIds_MvtxInttEmcal = {1, 2, 3, 4, 5, 6};
   std::vector<Double_t > hitsR_MvtxInttEmcal = {};
   std::vector<Double_t > hitsPhi_MvtxInttEmcal = {};
   if(truckF.ReturnHitsRPhiVect(hitsR_MvtxInttEmcal, hitsPhi_MvtxInttEmcal, subDetIds_MvtxInttEmcal, trk)){
      Double_t tempHitOInttPhi = trk.getHitPhi(4);
      Double_t tempHitEmcalPhi = trk.getHitPhi(6);
      truckF.SagittaRByCircleFit(centerX, centerY, sagittaR, hitsR_MvtxInttEmcal, hitsPhi_MvtxInttEmcal, tempHitOInttPhi, tempHitEmcalPhi);
      Double_t recoPt = truckF.CalcSagittaPt(sagittaR);
      Double_t dPt = (recoPt - truthPt)/truthPt;
      m_HTruthPtVsSagittaPt_MvtxInttEmcal->Fill(truthPt, dPt, truthEta);
      // std::cout << "pubEvNum = " << pubEvNum << std::endl;
      // EventJudge(pubEvNum, dPt, -2., 2., false); //CheckumaDaYo!!!

      TrackOtherPropertiesWTruth(truthP, trk, sagittaR, centerX, centerY, recoPt,\
         m_HTruthPVsTheta_MvtxInttEmcal, m_HTruthPVsPhi_MvtxInttEmcal,\
         m_HTruthPVsRecoP_MvtxInttEmcal, m_HTruthPVsEOverP_MvtxInttEmcal,\
         m_dVtxXY_MvtxInttEmcal, m_dVtxR_MvtxInttEmcal, m_dVtxZ_MvtxInttEmcal);

      // std::cout << "dPt = " << dPt << std::endl;
      // CheckumaDaYo!! oooooooo
      // ShowTrackInfo(trk, dPt, centerX, centerY, sagittaR);
            
   }

   std::vector<Int_t > subDetIds_VtxMvtxInttEmcal = {0, 1, 2, 3, 4, 5, 6};
   std::vector<Double_t > hitsR_VtxMvtxInttEmcal = {};
   std::vector<Double_t > hitsPhi_VtxMvtxInttEmcal = {};
   if(truckF.ReturnHitsRPhiVect(hitsR_VtxMvtxInttEmcal, hitsPhi_VtxMvtxInttEmcal, subDetIds_VtxMvtxInttEmcal, trk)){
      Double_t tempHitOInttPhi = trk.getHitPhi(4);
      Double_t tempHitEmcalPhi = trk.getHitPhi(6);
      truckF.SagittaRByCircleFit(centerX, centerY, sagittaR, hitsR_VtxMvtxInttEmcal, hitsPhi_VtxMvtxInttEmcal, tempHitOInttPhi, tempHitEmcalPhi);
      Double_t recoPt = truckF.CalcSagittaPt(sagittaR);
      Double_t dPt = (recoPt - truthPt)/truthPt;
      m_HTruthPtVsSagittaPt_VtxMvtxInttEmcal->Fill(truthPt, dPt, truthEta);
   }

   Double_t dPhiOInttEmcal = truckF.dPhiOInttEmcalEsti(trk);
   Double_t OriFunTrackPt = truckF.FitFunctionPt(dPhiOInttEmcal);
   Double_t dPtOriFun = (OriFunTrackPt - truthPt)/truthPt;
   m_HTruthPtVsFitFuncPt_IInttOInttEmcal->Fill(truthPt, dPtOriFun, truthEta);
   EventJudge(pubEvNum, dPtOriFun ,-2, -1, true);
   // std::cout << "111 dPt = " << dPtOriFun << std::endl;
   // CheckumaDaYo!! oooooooo

   Double_t dPhiVtxIInttEmcal = truckF.dPhiVtxIInttEmcalEsti(trk);
   Double_t OriFunTrackPt_VtxIInttEmcal = truckF.FitFunctionPt_VtxIInttEmcal(dPhiOInttEmcal);
   Double_t dPtOriFun_VtxIInttEmcal = (OriFunTrackPt_VtxIInttEmcal - truthPt)/truthPt;
   m_HTruthPtVsFitFuncPt_VtxIInttEmcal->Fill(truthPt, dPtOriFun_VtxIInttEmcal, truthEta);

   Double_t dPhiVtxOInttEmcal = truckF.dPhiVtxOInttEmcalEsti(trk);
   Double_t OriFunTrackPt_VtxOInttEmcal = truckF.FitFunctionPt_VtxOInttEmcal(dPhiVtxOInttEmcal);
   Double_t dPtOriFun_VtxOInttEmcal = (OriFunTrackPt_VtxOInttEmcal - truthPt)/truthPt;
   m_HTruthPtVsFitFuncPt_VtxOInttEmcal->Fill(truthPt, dPtOriFun_VtxOInttEmcal, truthEta);

   Double_t dPhiVtxInttEmcal = truckF.dPhiVtxInttEmcalEsti(trk);
   Double_t OriFunTrackPt_VtxInttEmcal = truckF.FitFunctionPt_VtxInttEmcal(dPhiVtxInttEmcal);
   Double_t dPtOriFun_VtxInttEmcal = (OriFunTrackPt_VtxInttEmcal - truthPt)/truthPt;
   m_HTruthPtVsFitFuncPt_VtxInttEmcal->Fill(truthPt, dPtOriFun_VtxInttEmcal, truthEta);

   Double_t dPhiMvtxIInttEmcal = truckF.dPhiMvtxIInttEmcalEsti(trk);
   Double_t OriFunTrackPt_MvtxIInttEmcal = truckF.FitFunctionPt_MVtxIInttEmcal(dPhiMvtxIInttEmcal);
   Double_t dPtOriFun_MvtxIInttEmcal = (OriFunTrackPt_MvtxIInttEmcal - truthPt)/truthPt;
   m_HTruthPtVsFitFuncPt_MvtxIInttEmcal->Fill(truthPt, dPtOriFun_MvtxIInttEmcal, truthEta);

   Double_t dPhiMvtxOInttEmcal = truckF.dPhiMvtxOInttEmcalEsti(trk);
   Double_t OriFunTrackPt_MvtxOInttEmcal = truckF.FitFunctionPt_MVtxInttEmcal(dPhiMvtxOInttEmcal);
   Double_t dPtOriFun_MvtxOInttEmcal = (OriFunTrackPt_MvtxOInttEmcal - truthPt)/truthPt;
   m_HTruthPtVsFitFuncPt_MvtxOInttEmcal->Fill(truthPt, dPtOriFun_MvtxOInttEmcal, truthEta);

   Double_t dPhiMvtxInttEmcal = truckF.dPhiMvtxInttEmcalEsti(trk);
   Double_t OriFunTrackPt_MvtxInttEmcal = truckF.FitFunctionPt_MVtxInttEmcal(dPhiMvtxInttEmcal);
   Double_t dPtOriFun_MvtxInttEmcal = (OriFunTrackPt_MvtxInttEmcal - truthPt)/truthPt;
   m_HTruthPtVsFitFuncPt_MvtxInttEmcal->Fill(truthPt, dPtOriFun_MvtxInttEmcal, truthEta);
}



void InttSeedTrackPerformance::TrackOtherPropertiesWTruth(hitStruct truthP, tracKuma trk,\
   Double_t sagittaR, Double_t centerX, Double_t centerY, Double_t recoPt,\
   TH2D* hTruthPVsTheta, TH2D* hTruthPVsPhi, TH2D* hTruthPVsRecoP,TH2D* hTruthPVsEOverP,\
   TH2D* hDVtxXY, TH1D* hDVtxR, TH1D* hDVtxZ){
   InttSeedTracking truckF;

   Double_t truthPt = truthP.pt;
   Double_t truthPhi = truthP.phi;
   Double_t truthTheta = 2*atan(std::exp(-truthP.eta));
   Double_t truVtxX = 0.; //TruthPV_trig_x
   Double_t truVtxY = 0.; //TruthPV_trig_y
   Double_t truVtxZ = 0.; //TruthPV_trig_y

   Double_t recoTheta = truckF.EstimateRecoTheta(trk, 0);
   
   Double_t trackE = trk.getTrackE();
   Double_t recoP = recoPt/sin(recoTheta);

   Double_t vtxX = 0.;
   Double_t vtxY = 0.;
   //    truckF.EstiVertex(vtxX, vtxY, sagittaR, centerX, centerY);
   truckF.CrossLineCircle(vtxX, vtxY, centerX, centerY, sagittaR);

   Double_t vtxR = std::sqrt(vtxX*vtxX + vtxY*vtxY);
   Double_t vtxPhi = atan(vtxY/vtxX);
   if((vtxPhi < 0)&&(vtxX < 0)) vtxPhi += TMath::Pi();
   else if((vtxPhi > 0)&&(vtxY < 0)) vtxPhi -= TMath::Pi();

   Double_t trkPhi = -1/std::tan(vtxPhi);
   Double_t vtxZ = 0.;
   if(trk.getHitIs(6)){
      vtxZ = trk.getHitZ(6) - trk.getHitZ(6)/std::tan(recoTheta);
   }

   Double_t dVtxX = 9999.;
   Double_t dVtxY = 9999.;
   Double_t dVtxR = 9999.;
   Double_t dVtxZ = 9999.;
   dVtxX = vtxX - truVtxX;
   dVtxY = vtxY - truVtxY;
   dVtxR = std::sqrt(dVtxX*dVtxX + dVtxY*dVtxY);
   dVtxZ = vtxZ - truVtxZ;
   // std::cout << "dvtx r = " << dVtxR << std::endl;

   hTruthPVsTheta->Fill(truthPt, recoTheta - truthTheta);
   // hTruthPVsRecoP->Fill(recoP, (recoP - )/recoP);
   hTruthPVsEOverP->Fill(truthPt, trackE/recoP);
   hTruthPVsPhi->Fill(truthPt, trkPhi - truthPhi);

   hDVtxXY->Fill(dVtxX, dVtxY);
   hDVtxR->Fill(dVtxR);
   hDVtxZ->Fill(dVtxZ);
}

void InttSeedTrackPerformance::EstimateMagneticShift(hitStruct truthP, tracKuma trk){
   InttSeedTracking truckF;

   Double_t truthPt  = truthP.pt;
   Double_t truthEta = truthP.eta;
   Double_t truthPhi = truthP.phi;

   Double_t refPhi = truthPhi;
   refPhi = TMath::Pi()/2 - refPhi;
   Double_t tempTruPhi = truthPhi + refPhi;


   Double_t fMvtxHitR = trk.getHitR(1);
   Double_t fMvtxHitPhi = trk.getHitPhi(1) + refPhi;
   Double_t sMvtxHitR = trk.getHitR(2);
   Double_t sMvtxHitPhi = trk.getHitPhi(2) + refPhi;
   Double_t tMvtxHitR = trk.getHitR(3);
   Double_t tMvtxHitPhi = trk.getHitPhi(3) + refPhi;
   Double_t MvtxHitR = (fMvtxHitR + sMvtxHitR + tMvtxHitR)/3;
   Double_t MvtxHitPhi = (fMvtxHitPhi + sMvtxHitPhi + tMvtxHitPhi)/3;

   Double_t iInttHitR = trk.getHitR(4);
   Double_t iInttHitPhi = trk.getHitPhi(4) + refPhi;
   Double_t oInttHitR = trk.getHitR(5);
   Double_t oInttHitPhi = trk.getHitPhi(5) + refPhi;
   Double_t InttHitR = (iInttHitR + oInttHitR)/2;
   Double_t InttHitPhi = (iInttHitPhi + oInttHitPhi)/2;

   Double_t emcalHitR = trk.getHitR(6);
   Double_t emcalHitPhi = trk.getHitPhi(6) + refPhi;


   Double_t dStraight_VtxIIntt = iInttHitR*sin(iInttHitPhi);
   Double_t dShiftL_VtxIIntt   = iInttHitR*cos(iInttHitPhi);

   Double_t slopeIInttOIntt = 0.;
   Double_t sectionIInttOIntt = 0.;
   truckF.CalcLineEq(slopeIInttOIntt, sectionIInttOIntt, \
      iInttHitR*cos(iInttHitPhi), iInttHitR*sin(iInttHitPhi),\
         oInttHitR*cos(oInttHitPhi), oInttHitR*sin(oInttHitPhi));
   
   Double_t crossX1 = 0.;
   Double_t crossY1 = 0.;
   Double_t slopeIINTT = dStraight_VtxIIntt/dShiftL_VtxIIntt;
   Double_t sectionIINTT = - slopeIINTT*iInttHitR*cos(iInttHitPhi) + iInttHitR*sin(iInttHitPhi);
   truckF.CalcCrossPoint(crossX1, crossY1, slopeIINTT, sectionIINTT, \
      oInttHitR*cos(oInttHitPhi), oInttHitR*sin(oInttHitPhi));
   Double_t dShiftL_VtxOIntt   = oInttHitR * cos(oInttHitPhi);
   Double_t dShiftL_IInttOIntt = dShiftL_VtxOIntt - dShiftL_VtxIIntt;
   Double_t dStraight_IInttOIntt = \
   std::sqrt((crossX1 - iInttHitR*cos(iInttHitPhi))*(crossX1 - iInttHitR*cos(iInttHitPhi))\
            + (crossY1 - iInttHitR*sin(iInttHitPhi))*(crossY1 - iInttHitR*sin(iInttHitPhi)));


   Double_t slopeOInttEmcal = 0.;
   Double_t sectionOInttEmcal = 0.;
   truckF.CalcLineEq(slopeOInttEmcal, sectionOInttEmcal, oInttHitR*cos(oInttHitPhi), oInttHitR*sin(oInttHitPhi),\
      emcalHitR*cos(emcalHitPhi), emcalHitR*sin(emcalHitPhi));
   Double_t crossX2 = 0.;
   Double_t crossY2 = 0.;
   truckF.CalcCrossPoint(crossX2, crossY2, slopeIInttOIntt, sectionIInttOIntt, \
      emcalHitR*cos(emcalHitPhi), emcalHitR*sin(emcalHitPhi));
   Double_t dStraight_OInttEmcal \
      = std::sqrt((crossX2 - oInttHitR*cos(oInttHitPhi))*(crossX2 - oInttHitR * cos(oInttHitPhi))\
            + (crossY2 - oInttHitR*sin(oInttHitPhi))*(crossY2 - oInttHitR * sin(oInttHitPhi)));
   

   Double_t dShiftL_VtxEmcal   = emcalHitR*cos(emcalHitPhi);
   Double_t dStraight_VtxEmcal = emcalHitR*sin(emcalHitPhi);
   Double_t dShiftL_OInttEmcal = dShiftL_VtxEmcal - dShiftL_VtxOIntt;

   Double_t dPhi_VtxIIntt = TMath::Pi()/2 - iInttHitPhi;
   Double_t temp_dPhi_IInttOIntt = atan(dStraight_IInttOIntt/dShiftL_IInttOIntt);
   if(temp_dPhi_IInttOIntt < 0) temp_dPhi_IInttOIntt += TMath::Pi();
   Double_t dPhi_IInttOIntt = TMath::Pi()/2 - temp_dPhi_IInttOIntt - dPhi_VtxIIntt;
   Double_t temp_dPhi_OInttEmcal = atan(dStraight_OInttEmcal/dShiftL_OInttEmcal);
   if(temp_dPhi_OInttEmcal < 0) temp_dPhi_OInttEmcal += TMath::Pi();
   Double_t dPhi_OInttEmcal = TMath::Pi()/2 - temp_dPhi_OInttEmcal - dPhi_VtxIIntt - dPhi_IInttOIntt;

   Double_t dPhiDStraight_VtxIIntt = dPhi_VtxIIntt/dStraight_VtxIIntt;
   m_HDPhiDStraight_VtxIIntt->Fill(truthPt, dPhiDStraight_VtxIIntt);
   m_HDPhiDStraight1D_VtxIIntt->Fill(dPhiDStraight_VtxIIntt);

   Double_t dPhiDStraight_IInttOIntt = dPhi_IInttOIntt/dStraight_IInttOIntt;
   m_HDPhiDStraight_IInttOIntt->Fill(truthPt,  dPhiDStraight_IInttOIntt);
   m_HDPhiDStraight1D_IInttOIntt->Fill(dPhiDStraight_IInttOIntt);

   Double_t dPhiDStraight_OInttEmcal = dPhi_OInttEmcal/dStraight_OInttEmcal;
   m_HDPhiDStraight_OInttEmcal->Fill(truthPt, dPhiDStraight_OInttEmcal);
   m_HDPhiDStraight1D_OInttEmcal->Fill(dPhiDStraight_OInttEmcal);

   Double_t ddPhiddStraight_IInttOInttEmcal = dPhiDStraight_OInttEmcal - dPhiDStraight_IInttOIntt;
   m_HTruthPtVsDdPhiddStraight_IInttOInttEmcal->Fill(truthPt, ddPhiddStraight_IInttOInttEmcal);
   
   m_HDPhiVsDStraightVsTruPt->Fill(dStraight_VtxIIntt, dPhi_VtxIIntt, truthPt);
   m_HDPhiVsDStraightVsTruPt->Fill(dStraight_IInttOIntt, dPhi_IInttOIntt, truthPt);
   m_HDPhiVsDStraightVsTruPt->Fill(dPhiDStraight_OInttEmcal, dPhi_OInttEmcal, truthPt);

   m_HDLVsTruthPtVsEta_VtxIIntt->Fill(dShiftL_VtxIIntt, truthPt, truthEta);
   m_HDL1D_VtxIIntt->Fill(dShiftL_VtxIIntt);
   m_HDLVsTruthPtVsEta_IInttOIntt->Fill(dShiftL_IInttOIntt, truthPt, truthEta);
   m_HDL1D_IInttOIntt->Fill(dShiftL_IInttOIntt);
   m_HDLVsTruthPtVsEta_OInttEmcal->Fill(dShiftL_OInttEmcal, truthPt, truthEta);
   m_HDL1D_OInttEmcal->Fill(dShiftL_OInttEmcal);


   Double_t tempMvtxX = MvtxHitR*std::cos(MvtxHitPhi);
   Double_t tempMvtxY = MvtxHitR*std::sin(MvtxHitPhi);
   Double_t tempIInttX = iInttHitR*std::cos(iInttHitPhi);
   Double_t tempIInttY = iInttHitR*std::sin(iInttHitPhi);
   Double_t tempOInttX = oInttHitR*std::cos(oInttHitPhi);
   Double_t tempOInttY = oInttHitR*std::sin(oInttHitPhi);
   Double_t tempInttX = InttHitR*std::cos(InttHitPhi);
   Double_t tempInttY = InttHitR*std::sin(InttHitPhi);
   Double_t tempEmcalX = emcalHitR*std::cos(emcalHitPhi);
   Double_t tempEmcalY = emcalHitR*std::sin(emcalHitPhi);

   Double_t dPhiMvtxIIntt = std::atan((tempIInttY - tempMvtxY)/(tempIInttX - tempMvtxX));
   if(dPhiMvtxIIntt < 0) dPhiMvtxIIntt += TMath::Pi();
   Double_t dPhiVtxMvtxIIntt = dPhiMvtxIIntt - MvtxHitPhi;

   Double_t dPhiMvtxOIntt = std::atan((tempOInttY - tempMvtxY)/(tempOInttX - tempMvtxX));
   if(dPhiMvtxOIntt < 0) dPhiMvtxOIntt += TMath::Pi();
   Double_t dPhiVtxMvtxOIntt = dPhiMvtxOIntt - MvtxHitPhi;

   Double_t dPhiMvtxIntt = std::atan((tempInttY - tempMvtxY)/(tempInttX - tempMvtxX));
   if(dPhiMvtxIntt < 0) dPhiMvtxIntt += TMath::Pi();
   Double_t dPhiVtxMvtxIntt = dPhiMvtxIntt - MvtxHitPhi;

   // Double_t tempHitsXY[3][2];
   // truckF.Set3PointsXY(tempHitsXY, trk, 3);
   // Double_t tempR = 0.;
   // Double_t tempXc = 0.;
   // Double_t tempYc = 0.;
   // truckF.RoughEstiSagittaCenter3Point(tempR, tempXc, tempYc, tempHitsXY);
   // Double_t crossXcal = 0.;
   // Double_t crossYcal = 0.;
   // Double_t calPhi = truckF.CrossCircleCircle(crossXcal, crossYcal, tempXc, tempYc, tempR, trk.getHitPhi(5));

   // if(std::abs(trk.getHitPhi(6) - calPhi) > 0.3) bDecayEvent = true;
   // std::cout << "x, y = " << crossXcal << ",  " << crossYcal << std::endl;
   // std::cout << "dPhi = " << std::abs(trk.getHitPhi(6) - calPhi) << std::endl;

   if(!m_bDecayEvent){
      Double_t dPhiIInttEmcal = std::atan((tempEmcalY - tempIInttY)/(tempEmcalX - tempIInttX));
      if(dPhiIInttEmcal < 0) dPhiIInttEmcal += TMath::Pi();
      Double_t dPhiVtxIInttEmcal = dPhiIInttEmcal - iInttHitPhi;
      Double_t dPhiMvtxIInttEmcal = dPhiIInttEmcal - dPhiMvtxIIntt;

      Double_t dPhiOInttEmcal = std::atan((tempEmcalY - tempOInttY)/(tempEmcalX - tempOInttX));
      if(dPhiOInttEmcal < 0) dPhiOInttEmcal += TMath::Pi();
      Double_t dPhiVtxOInttEmcal = dPhiOInttEmcal - oInttHitPhi;
      Double_t dPhiMvtxOInttEmcal = dPhiOInttEmcal - dPhiMvtxOIntt;

      Double_t dPhiInttEmcal = std::atan((tempEmcalY - tempInttY)/(tempEmcalX - tempInttX));
      if(dPhiInttEmcal < 0) dPhiInttEmcal += TMath::Pi();
      Double_t dPhiVtxInttEmcal = dPhiInttEmcal - InttHitPhi;
      Double_t dPhiMvtxInttEmcal = dPhiInttEmcal - dPhiMvtxIntt;

      Double_t dPhiIInttOIntt = std::atan((tempOInttY - tempIInttY)/(tempOInttX - tempIInttX));
      if(dPhiIInttOIntt < 0) dPhiIInttOIntt += TMath::Pi();
      Double_t dPhi_IInttOInttEmcal = dPhiOInttEmcal - dPhiIInttOIntt;

      m_HDPhiVsTruthPtVsEta_VtxIIntt->Fill(dPhi_VtxIIntt, truthPt, truthEta);
      m_HDPhiVsTruthPtVsEta_IInttOIntt->Fill(dPhi_IInttOIntt, truthPt, truthEta);
      m_HDPhiVsTruthPtVsEta_OInttEmcal->Fill(dPhi_OInttEmcal, truthPt, truthEta);

      m_HDPhiVsTruthPtProf_VtxIIntt->Fill(dPhi_VtxIIntt, truthPt);
      m_HDPhiVsTruthPtProf_IInttOIntt->Fill(dPhi_IInttOIntt, truthPt);
      m_HDPhiVsTruthPtProf_OInttEmcal->Fill(dPhi_OInttEmcal, truthPt);
    
      m_HDPhiVsTruthPtVsEta_IInttOInttEmcal->Fill(dPhi_IInttOInttEmcal, truthPt, truthEta);
      m_HDPhiVsTruthPtProf_IInttOInttEmcal->Fill(dPhi_IInttOInttEmcal, truthPt);

      m_HDPhiVsTruthPtVsEta_VtxIInttEmcal->Fill(dPhiVtxIInttEmcal, truthPt, truthEta);
      m_HDPhiVsTruthPtVsEta_VtxOInttEmcal->Fill(dPhiVtxOInttEmcal, truthPt, truthEta);
      m_HDPhiVsTruthPtVsEta_VtxInttEmcal->Fill(dPhiVtxInttEmcal, truthPt, truthEta);

      m_HDPhiVsTruthPtVsEta_MvtxIInttEmcal->Fill(dPhiMvtxIInttEmcal, truthPt, truthEta);
      m_HDPhiVsTruthPtVsEta_MvtxOInttEmcal->Fill(dPhiMvtxOInttEmcal, truthPt, truthEta);
      m_HDPhiVsTruthPtVsEta_MvtxInttEmcal->Fill(dPhiMvtxInttEmcal, truthPt, truthEta);

      m_HDPhiVsTruthPtProf_VtxIInttEmcal->Fill(dPhiVtxIInttEmcal, truthPt);
      m_HDPhiVsTruthPtProf_VtxOInttEmcal->Fill(dPhiVtxOInttEmcal, truthPt);
      m_HDPhiVsTruthPtProf_VtxInttEmcal->Fill(dPhiVtxInttEmcal, truthPt);

      m_HDPhiVsTruthPtProf_MvtxIInttEmcal->Fill(dPhiMvtxIInttEmcal, truthPt);
      m_HDPhiVsTruthPtProf_MvtxOInttEmcal->Fill(dPhiMvtxOInttEmcal, truthPt);
      m_HDPhiVsTruthPtProf_MvtxInttEmcal->Fill(dPhiMvtxInttEmcal, truthPt);
   }
}

void InttSeedTrackPerformance::CaloEnergyQA(Int_t CalId, std::vector<hitStruct > calHits)
{
    Int_t highestEID = 9999;
    Double_t highestE = 0.;
    Int_t numOfCalT = calHits.size();
    for(Int_t iCalTower = 0; iCalTower < numOfCalT; iCalTower++)
    {
        if(highestE < calHits.at(iCalTower).energy)
        {
            highestE = calHits.at(iCalTower).energy;
            highestEID = iCalTower;
        }
    }
    if(highestEID == 9999) return;
    Double_t refCalPhi = calHits.at(highestEID).phi;
    Double_t refCalEta = calHits.at(highestEID).eta;
    for(Int_t iCalT = 0; iCalT < numOfCalT; iCalT++)
    {
        Double_t phi = calHits.at(iCalT).phi - refCalPhi;
        Double_t eta = calHits.at(iCalT).eta - refCalEta;
        Double_t E = calHits.at(iCalT).energy;
        if(CalId == 0) m_HECalPhiVsE->Fill(phi, eta, E);
        else if(CalId == 1) m_HIHCalPhiVsE->Fill(phi, eta, E);
        else if(CalId == 2) m_HOHCalPhiVsE->Fill(phi, eta, E);
    }
}


bool InttSeedTrackPerformance::ParticleDecayCheck(tracKuma trk, std::vector<hitStruct > vEmcalHits){
   bool bIsDecay = false;
   Int_t numOfCal = vEmcalHits.size();
   for(Int_t iCal = 0; iCal < numOfCal; iCal++){
      if((vEmcalHits.at(iCal).energy > 0.1)\
         && (std::abs(vEmcalHits.at(iCal).phi - trk.getHitPhi(6)) > 0.04)){
         bIsDecay = true;
      }
   }
   return bIsDecay;
}
// == e == Track QA Functions  ###############################################



void InttSeedTrackPerformance::ResetValuesForEvent(){
   m_tracks.clear();
   m_tracks.shrink_to_fit();

   m_TruthParticle.clear();
   m_TruthParticle.shrink_to_fit();

   m_fMvtxHits.clear();
   m_sMvtxHits.clear();
   m_tMvtxHits.clear();

   m_iInttHits.clear();
   m_oInttHits.clear();

   m_TpcHits.clear();

   m_emcalHits.clear();
   m_iHCalHits.clear();
   m_oHCalHits.clear();

   // ==
   m_fMvtxHits.shrink_to_fit();
   m_sMvtxHits.shrink_to_fit();
   m_tMvtxHits.shrink_to_fit();

   m_iInttHits.shrink_to_fit();
   m_oInttHits.shrink_to_fit();

   m_TpcHits.shrink_to_fit();

   m_emcalHits.shrink_to_fit();
   m_iHCalHits.shrink_to_fit();
   m_oHCalHits.shrink_to_fit();

}

void InttSeedTrackPerformance::AllResetValuesForEvent()
{   
    m_tracks.clear();
    m_tracks.shrink_to_fit();

    m_vTruthParticle.clear();
    m_viInttHits.clear();
    m_voInttHits.clear();
    m_vTpcHits.clear();
    m_vemcalHits.clear();
    m_viHCalHits.clear();
    m_voHCalHits.clear();
    
    m_vTruthParticle.shrink_to_fit();
    m_viInttHits.shrink_to_fit();
    m_voInttHits.shrink_to_fit();
    m_vTpcHits.shrink_to_fit();
    m_vemcalHits.shrink_to_fit();
    m_viHCalHits.shrink_to_fit();
    m_voHCalHits.shrink_to_fit();
    
    m_TruthParticle.clear();
    m_TruthParticle.shrink_to_fit();

    m_fMvtxHits.clear();
    m_sMvtxHits.clear();
    m_tMvtxHits.clear();

    m_iInttHits.clear();
    m_oInttHits.clear();

    m_TpcHits.clear();

    m_emcalHits.clear();
    m_iHCalHits.clear();
    m_oHCalHits.clear();

    // ==
    m_fMvtxHits.shrink_to_fit();
    m_sMvtxHits.shrink_to_fit();
    m_tMvtxHits.shrink_to_fit();

    m_iInttHits.shrink_to_fit();
    m_oInttHits.shrink_to_fit();

    m_TpcHits.shrink_to_fit();

    m_emcalHits.shrink_to_fit();
    m_iHCalHits.shrink_to_fit();
    m_oHCalHits.shrink_to_fit();
}

void InttSeedTrackPerformance::PartResetValuesForEvent()
{
    m_TruthParticle.clear();
    m_TruthParticle.shrink_to_fit();

    m_iInttHits.clear();
    m_oInttHits.clear();

    m_TpcHits.clear();

    m_emcalHits.clear();
    m_iHCalHits.clear();
    m_oHCalHits.clear();

    // ==
    m_iInttHits.shrink_to_fit();
    m_oInttHits.shrink_to_fit();

    m_TpcHits.shrink_to_fit();

    m_emcalHits.shrink_to_fit();
    m_iHCalHits.shrink_to_fit();
    m_oHCalHits.shrink_to_fit();
}



void InttSeedTrackPerformance::WrightHists()
{
    oFile = new TFile(fOutput.c_str(), "recreate");
    oFile->cd();
    m_HINTTHitMap->Write();

    m_HGlobalHitMap->Write();

    m_HECalPhiVsE->Write();
    m_HIHCalPhiVsE->Write();
    m_HOHCalPhiVsE->Write();

    m_HMatchCalEVsHighestCalE->Write();

    m_HdPhiInttdPhiECal->Write();
    m_HTruthPtVsdPhiInttdPhiECal->Write();

    m_HTruthPtVsSagittaPt_rough->Write();
    m_HTruthPtVsSagittaPt_VtxIntt->Write();
    m_HTruthPtVsSagittaPt_InttEmcal->Write();
    m_HTruthPtVsSagittaPt_VtxInttEmcal->Write();
    m_HTruthPtVsSagittaPt_MvtxInttEmcal->Write();
    m_HTruthPtVsSagittaPt_VtxMvtxInttEmcal->Write();

    m_HTruthPtVsFitFuncPt_IInttOInttEmcal->Write();

    m_HTruthPtVsFitFuncPt_VtxIInttEmcal->Write();
    m_HTruthPtVsFitFuncPt_VtxOInttEmcal->Write();
    m_HTruthPtVsFitFuncPt_VtxInttEmcal->Write();

    m_HTruthPtVsFitFuncPt_MvtxIInttEmcal->Write();
    m_HTruthPtVsFitFuncPt_MvtxOInttEmcal->Write();
    m_HTruthPtVsFitFuncPt_MvtxInttEmcal->Write();

    m_HTruthPVsRecoP_InttEmcal->Write();
    m_HTruthPVsRecoP_MvtxInttEmcal->Write();
    m_HTruthPVsRecoP_FitFunc->Write();

    m_HTruthPVsEOverP_InttEmcal->Write();
    m_HTruthPVsEOverP_MvtxInttEmcal->Write();
    m_HTruthPVsEOverP_FitFunc->Write();

    m_HTruthPVsTheta_InttEmcal->Write();
    m_HTruthPVsTheta_MvtxInttEmcal->Write();
    
    m_HTruthPVsPhi_InttEmcal->Write();
    m_HTruthPVsPhi_MvtxInttEmcal->Write();
    

    m_dVtxXY_InttEmcal->Write();
    m_dVtxXY_MvtxInttEmcal->Write();

    m_dVtxR_InttEmcal->Write();
    m_dVtxR_MvtxInttEmcal->Write();

    m_dVtxZ_InttEmcal->Write();
    m_dVtxZ_MvtxInttEmcal->Write();

    m_HDE->Write();

    m_HDPhiVsDStraightVsTruPt->Write();

    m_HDPhiDStraight_VtxIIntt->Write();
    m_HDPhiDStraight1D_VtxIIntt->Write();
    m_HDPhiDStraight_IInttOIntt->Write();
    m_HDPhiDStraight1D_IInttOIntt->Write();
    m_HDPhiDStraight_OInttEmcal->Write();
    m_HDPhiDStraight1D_OInttEmcal->Write();
    m_HTruthPtVsDdPhiddStraight_IInttOInttEmcal->Write();


    m_HDLVsTruthPtVsEta_VtxIIntt->Write();
    m_HDL1D_VtxIIntt->Write();
    m_HDLVsTruthPtVsEta_IInttOIntt->Write();
    m_HDL1D_IInttOIntt->Write();
    m_HDLVsTruthPtVsEta_OInttEmcal->Write();
    m_HDL1D_OInttEmcal->Write();

    m_HDPhiVsTruthPtVsEta_VtxIIntt->Write();
    m_HDPhiVsTruthPtVsEta_IInttOIntt->Write();
    m_HDPhiVsTruthPtVsEta_OInttEmcal->Write();
    
    m_HDPhiVsTruthPtVsEta_IInttOInttEmcal->Write();

    m_HDPhiVsTruthPtVsEta_VtxIInttEmcal->Write();
    m_HDPhiVsTruthPtVsEta_VtxOInttEmcal->Write();
    m_HDPhiVsTruthPtVsEta_VtxInttEmcal->Write();
    m_HDPhiVsTruthPtVsEta_MvtxIInttEmcal->Write();
    m_HDPhiVsTruthPtVsEta_MvtxOInttEmcal->Write();
    m_HDPhiVsTruthPtVsEta_MvtxInttEmcal->Write();
    m_HDPhiVsTruthPtProf_VtxIIntt->Write();

    m_HDPhiVsTruthPtProf_IInttOInttEmcal->Write();
    m_HDPhiVsTruthPtProf_IInttOIntt->Write();
    m_HDPhiVsTruthPtProf_OInttEmcal->Write();
    m_HDPhiVsTruthPtProf_VtxIInttEmcal->Write();
    m_HDPhiVsTruthPtProf_VtxOInttEmcal->Write();
    m_HDPhiVsTruthPtProf_VtxInttEmcal->Write();
    m_HDPhiVsTruthPtProf_MvtxIInttEmcal->Write();
    m_HDPhiVsTruthPtProf_MvtxOInttEmcal->Write();
    m_HDPhiVsTruthPtProf_MvtxInttEmcal->Write();

    m_HPtEfficiency->Divide(m_HPtEfficiency, m_HTruTrackNum, 1.0, 1.0, "b");
    m_HPtEfficiency->Write();
    m_HTruTrackNum->Write();

    oFile->Close();

    std::cout << "root " << fOutput << std::endl;
}

// MakeClass Original Functions  ###############################################
InttSeedTrackPerformance::InttSeedTrackPerformance(TTree *tree, std::string fInputName, std::string fOutputName, Int_t runNum) : fChain(0) 
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   fInput = fInputName;
   fOutput = fOutputName + std::to_string(runNum) + ".root";

   if (tree == 0) {
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject(fInput.c_str());
      if (!f || !f->IsOpen()) {
         f = new TFile(fInput.c_str());
      }
      f->GetObject("tree",tree);

   }
   Init(tree);
}

InttSeedTrackPerformance::~InttSeedTrackPerformance()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t InttSeedTrackPerformance::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}

Long64_t InttSeedTrackPerformance::LoadTree(Long64_t entry)
{
// Set the environment to read one entry
   if (!fChain) return -5;
   Long64_t centry = fChain->LoadTree(entry);
   if (centry < 0) return centry;
   if (fChain->GetTreeNumber() != fCurrent) {
      fCurrent = fChain->GetTreeNumber();
   }
   return centry;
}

void InttSeedTrackPerformance::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set object pointer
   trk_system = 0;
   trk_layer = 0;
   trk_adc = 0;
   trk_X = 0;
   trk_Y = 0;
   trk_Z = 0;
   trk_size = 0;
   trk_phi_size = 0;
   trk_Z_size = 0;

   tower_system = 0;
   tower_X = 0;
   tower_Y = 0;
   tower_Z = 0;
   tower_Eta = 0;
   tower_Phi = 0;
   tower_Eta_test = 0;
   tower_Phi_test = 0;
   tower_Eta_bin = 0;
   tower_Phi_bin = 0;
   tower_edep = 0;

   caloClus_system = 0;
   caloClus_X = 0;
   caloClus_Y = 0;
   caloClus_Z = 0;
   caloClus_R = 0;
   caloClus_Phi = 0;
   caloClus_edep = 0;

   PrimaryG4P_Pt = 0;
   PrimaryG4P_Eta = 0;
   PrimaryG4P_Phi = 0;
   PrimaryG4P_E = 0;
   PrimaryG4P_PID = 0;
   PrimaryG4P_isChargeHadron = 0;
   // Set branch addresses and branch pointers
   if (!tree) return;
   fChain = tree;
   fCurrent = -1;
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("trk_NClus", &trk_NClus, &b_trk_NClus);
   fChain->SetBranchAddress("trk_system", &trk_system, &b_trk_system);
   fChain->SetBranchAddress("trk_layer", &trk_layer, &b_trk_layer);
   fChain->SetBranchAddress("trk_adc", &trk_adc, &b_trk_adc);
   fChain->SetBranchAddress("trk_X", &trk_X, &b_trk_X);
   fChain->SetBranchAddress("trk_Y", &trk_Y, &b_trk_Y);
   fChain->SetBranchAddress("trk_Z", &trk_Z, &b_trk_Z);
   fChain->SetBranchAddress("trk_size", &trk_size, &b_trk_size);
   fChain->SetBranchAddress("trk_phi_size", &trk_phi_size, &b_trk_phi_size);
   fChain->SetBranchAddress("trk_Z_size", &trk_Z_size, &b_trk_Z_size);

   fChain->SetBranchAddress("nTowers", &nTowers, &b_nTowers);
   fChain->SetBranchAddress("tower_system", &tower_system, &b_tower_system);
   fChain->SetBranchAddress("tower_X", &tower_X, &b_tower_X);
   fChain->SetBranchAddress("tower_Y", &tower_Y, &b_tower_Y);
   fChain->SetBranchAddress("tower_Z", &tower_Z, &b_tower_Z);
   fChain->SetBranchAddress("tower_Eta", &tower_Eta, &b_tower_Eta);
   fChain->SetBranchAddress("tower_Phi", &tower_Phi, &b_tower_Phi);
   fChain->SetBranchAddress("tower_Eta_test", &tower_Eta_test, &b_tower_Eta_test);
   fChain->SetBranchAddress("tower_Phi_test", &tower_Phi_test, &b_tower_Phi_test);
   fChain->SetBranchAddress("tower_Eta_bin", &tower_Eta_bin, &b_tower_Eta_bin);
   fChain->SetBranchAddress("tower_Phi_bin", &tower_Phi_bin, &b_tower_Phi_bin);
   fChain->SetBranchAddress("tower_edep", &tower_edep, &b_tower_edep);

   //checkumaDaYooo!!
   if(bCaloClu){
      fChain->SetBranchAddress("nCaloClus", &nCaloClus, &b_nCaloClus);
      fChain->SetBranchAddress("caloClus_system", &caloClus_system, &b_caloClus_system);
      fChain->SetBranchAddress("caloClus_X", &caloClus_X, &b_caloClus_X);
      fChain->SetBranchAddress("caloClus_Y", &caloClus_Y, &b_caloClus_Y);
      fChain->SetBranchAddress("caloClus_Z", &caloClus_Z, &b_caloClus_Z);
      fChain->SetBranchAddress("caloClus_R", &caloClus_R, &b_caloClus_R);
      fChain->SetBranchAddress("caloClus_Phi", &caloClus_Phi, &b_caloClus_Phi);
      fChain->SetBranchAddress("caloClus_edep", &caloClus_edep, &b_caloClus_edep);
   }


   fChain->SetBranchAddress("NTruthVtx", &NTruthVtx, &b_NTruthVtx);
   fChain->SetBranchAddress("TruthPV_trig_x", &TruthPV_trig_x, &b_TruthPV_trig_x);
   fChain->SetBranchAddress("TruthPV_trig_y", &TruthPV_trig_y, &b_TruthPV_trig_y);
   fChain->SetBranchAddress("TruthPV_trig_z", &TruthPV_trig_z, &b_TruthPV_trig_z);
   fChain->SetBranchAddress("NPrimaryG4P", &NPrimaryG4P, &b_NPrimaryG4P);
   fChain->SetBranchAddress("PrimaryG4P_Pt", &PrimaryG4P_Pt, &b_PrimaryG4P_Pt);
   fChain->SetBranchAddress("PrimaryG4P_Eta", &PrimaryG4P_Eta, &b_PrimaryG4P_Eta);
   fChain->SetBranchAddress("PrimaryG4P_Phi", &PrimaryG4P_Phi, &b_PrimaryG4P_Phi);
   fChain->SetBranchAddress("PrimaryG4P_E", &PrimaryG4P_E, &b_PrimaryG4P_E);
   fChain->SetBranchAddress("PrimaryG4P_PID", &PrimaryG4P_PID, &b_PrimaryG4P_PID);
   fChain->SetBranchAddress("PrimaryG4P_isChargeHadron", &PrimaryG4P_isChargeHadron, &b_PrimaryG4P_isChargeHadron);
   
}
// MakeClass Original Functions  ###############################################


void InttSeedTrackPerformance::DrawEventDisplay(Int_t eventId, std::vector<tracKuma > trks){
   if(qaEvent != 9999){
      TCanvas* c = new TCanvas("c","c: pT",800,850);
      c->cd();
      TPad* pad1 = new TPad("pad1", "pad1", 0.1, 0.1, 1, 1.0);
      pad1->SetBottomMargin(0.15);
      pad1->SetLeftMargin(0.15);
      pad1->SetRightMargin(0.05);
      pad1->SetTopMargin(0.05);
      pad1->Draw();
      pad1->cd();

      TH2D* hHitMapED = new TH2D( "hHitMapED", "hHitMapED", 200, -100, 100, 200, -100, 100);
      std::vector<TArc* > trkLines;
      for(Int_t iTrk = 0; iTrk < trks.size(); iTrk++){
         tracKuma trk = trks.at(iTrk);
         for(Int_t iHit = 1; iHit < 7; iHit++){
            Double_t tempX = trk.getHitR(iHit)*std::cos(trk.getHitPhi(iHit));
            Double_t tempY = trk.getHitR(iHit)*std::sin(trk.getHitPhi(iHit));
            hHitMapED->Fill(tempX, tempY);
         }

         Double_t tempR = trk.getTrackSagR();
         Double_t tempXc = trk.getTrackSagX();
         Double_t tempYc = trk.getTrackSagY();
         
         Double_t edgeX1 = 0.;
         Double_t edgeY1 = 0.;         
         Double_t edgePhi1 = std::atan((edgeY1 - tempYc)/(edgeX1 - tempXc));
         if((edgePhi1 < 0)&&((edgeX1 - tempXc) < 0)) edgePhi1 += TMath::Pi();
         else if((edgePhi1 > 0)&&((edgeX1 - tempXc) < 0))  edgePhi1 += TMath::Pi();

         Double_t edgeX2 = trk.getHitR(6)*std::cos(trk.getHitPhi(6));
         Double_t edgeY2 = trk.getHitR(6)*std::sin(trk.getHitPhi(6));
         Double_t edgePhi2 = std::atan((edgeY2 - tempYc)/(edgeX2 - tempXc));
         if((edgePhi2 < 0)&&((edgeX2 - tempXc) < 0)) edgePhi2 += TMath::Pi();
         else if((edgePhi2 > 0)&&((edgeX2 - tempXc) < 0))  edgePhi2 += TMath::Pi();

         Double_t diffPhi1 = std::abs(edgePhi2 - edgePhi1);
         Double_t diffPhi2 = std::abs((edgePhi2 - 2*TMath::Pi()) - edgePhi1);
         if(diffPhi2 < diffPhi1) edgePhi2 -= 2*TMath::Pi();

         Double_t tempMinPhi = 0.;
         Double_t tempMaxPhi = 360.;
         if(edgePhi1 < edgePhi2){
            tempMinPhi = edgePhi1 - 10.;
            tempMaxPhi = edgePhi2 + 10.;
         }else{
            tempMinPhi = edgePhi2 - 10.;
            tempMaxPhi = edgePhi1 + 10.;
         }

         auto arc = new TArc(tempXc, tempYc, tempR, tempMinPhi, tempMaxPhi);
         arc->SetFillStyle(0);

         trkLines.push_back(arc);
      }

      hHitMapED->SetFillColor(3);
      hHitMapED->Draw("BOX");
      for(Int_t iTrk = 0; iTrk < trkLines.size(); iTrk++){
         trkLines.at(iTrk)->Draw("only");
      }

      c->SaveAs("hQaSagittaFit.root");
      c->Close();
      // oFile->cd();
      // hHitMap->Write();
      // fCircleQA->Write();
      qaEvent = 9999;
   }
}

void InttSeedTrackPerformance::ShowEventInfo(){
   std::cout << "TH2D* hHitMapMvtx = new TH2D( \"hHitMapMvtx\", \"hHitMap\", 2000, -100, 100, 2000, -100, 100);" << std::endl;
   std::cout << "TH2D* hHitMapIntt = new TH2D( \"hHitMapIntt\", \"hHitMap\", 2000, -100, 100, 2000, -100, 100);" << std::endl;
   std::cout << "TH2D* hHitMapTpc = new TH2D( \"hHitMapTpc\", \"hHitMap\", 2000, -100, 100, 2000, -100, 100);" << std::endl;
   std::cout << "TH2D* hHitMapEmcal = new TH2D( \"hHitMapEmcal\", \"hHitMap\", 2000, -100, 100, 2000, -100, 100);" << std::endl;
   std::cout << "hHitMapMvtx->SetLineWidth(8);" << std::endl;
   std::cout << "hHitMapIntt->SetLineWidth(8);" << std::endl;
   std::cout << "hHitMapTpc->SetLineWidth(8);" << std::endl;
   std::cout << "hHitMapEmcal->SetLineWidth(8);" << std::endl;
   std::cout << "hHitMapMvtx->SetLineColor(860);" << std::endl;
   std::cout << "hHitMapIntt->SetLineColor(875);" << std::endl;
   std::cout << "hHitMapTpc->SetLineColor(870);" << std::endl;
   std::cout << "hHitMapEmcal->SetLineColor(807);" << std::endl;


   for(Int_t i=0; i<m_fMvtxHits.size(); i++){
      std::cout << "hHitMapMvtx->Fill(" << m_fMvtxHits.at(i).r*std::cos(m_fMvtxHits.at(i).phi) \
         << ", " << m_fMvtxHits.at(i).r*std::sin(m_fMvtxHits.at(i).phi) << ");" << std::endl;
   }
   for(Int_t i=0; i<m_sMvtxHits.size(); i++){
      std::cout << "hHitMapMvtx->Fill(" << m_sMvtxHits.at(i).r*std::cos(m_sMvtxHits.at(i).phi) \
         << ", " << m_sMvtxHits.at(i).r*std::sin(m_sMvtxHits.at(i).phi) << ");" << std::endl;
   }
   for(Int_t i=0; i<m_tMvtxHits.size(); i++){
      std::cout << "hHitMapMvtx->Fill(" << m_tMvtxHits.at(i).r*std::cos(m_tMvtxHits.at(i).phi) \
         << ", " << m_tMvtxHits.at(i).r*std::sin(m_tMvtxHits.at(i).phi) << ");" << std::endl;
   }
   for(Int_t i=0; i< m_iInttHits.size(); i++){
      std::cout << "hHitMapIntt->Fill(" << m_iInttHits.at(i).r*std::cos(m_iInttHits.at(i).phi) \
         << ", " << m_iInttHits.at(i).r*std::sin(m_iInttHits.at(i).phi) << ");" << std::endl;
   }
   for(Int_t i=0; i< m_oInttHits.size(); i++){
      std::cout << "hHitMapIntt->Fill(" << m_oInttHits.at(i).r*std::cos(m_oInttHits.at(i).phi) \
         << ", " << m_oInttHits.at(i).r*std::sin(m_oInttHits.at(i).phi) << ");" << std::endl;
   }
   for(Int_t i=0; i< m_TpcHits.size(); i++){
      std::cout << "hHitMapTpc->Fill(" << m_TpcHits.at(i).r*std::cos(m_TpcHits.at(i).phi) \
         << ", " << m_TpcHits.at(i).r*std::sin(m_TpcHits.at(i).phi) << ");" << std::endl;
   }
   for(Int_t i=0; i< m_emcalHits.size(); i++){
      if(m_emcalHits.at(i).energy > 0.1) std::cout << "hHitMapEmcal->Fill(" << m_emcalHits.at(i).r*std::cos(m_emcalHits.at(i).phi) \
         << ", " << m_emcalHits.at(i).r*std::sin(m_emcalHits.at(i).phi) << ");" << std::endl;
   }

   std::cout << "hHitMapMvtx->Draw(\"BOX\");" << std::endl;
   std::cout << "hHitMapIntt->Draw(\"BOX same\");" << std::endl;
   std::cout << "hHitMapTpc->Draw(\"BOX same\");" << std::endl;
   std::cout << "hHitMapEmcal->Draw(\"BOX same\");" << std::endl;
}


void InttSeedTrackPerformance::ShowTrackInfo(tracKuma trk, Double_t dPt, Double_t centerX, Double_t centerY, Double_t sagittaR){
   std::cout << "222: dPt, R, Xc, Yc = " << dPt << ", " << sagittaR << ", " << centerX << ", " << centerY << std::endl;
   std::cout << "auto arc = new  TArc(" << centerX << ", " << centerY << ", " << sagittaR << ");"  << std::endl;
   std::cout << "arc->SetFillStyle(0);" << std::endl;
   std::cout << "arc->Draw(\"only\");" << std::endl;


   std::cout << "TH2D* hHitMapMvtxTrk = new TH2D( \"hHitMapMvtxTrk\", \"hHitMap\", 2000, -100, 100, 2000, -100, 100);" << std::endl;
   std::cout << "TH2D* hHitMapInttTrk = new TH2D( \"hHitMapInttTrk\", \"hHitMap\", 2000, -100, 100, 2000, -100, 100);" << std::endl;
   std::cout << "TH2D* hHitMapTpcTrk = new TH2D( \"hHitMapTpcTrk\", \"hHitMap\", 2000, -100, 100, 2000, -100, 100);" << std::endl;
   std::cout << "TH2D* hHitMapEmcalTrk = new TH2D( \"hHitMapEmcalTrk\", \"hHitMap\", 2000, -100, 100, 2000, -100, 100);" << std::endl;
   std::cout << "hHitMapMvtxTrk->SetLineWidth(3);" << std::endl;
   std::cout << "hHitMapInttTrk->SetLineWidth(3);" << std::endl;
   std::cout << "hHitMapTpcTrk->SetLineWidth(3);" << std::endl;
   std::cout << "hHitMapEmcalTrk->SetLineWidth(3);" << std::endl;
   std::cout << "hHitMapMvtxTrk->SetLineColor(632);" << std::endl;
   std::cout << "hHitMapInttTrk->SetLineColor(632);" << std::endl;
   std::cout << "hHitMapTpcTrk->SetLineColor(632);" << std::endl;
   std::cout << "hHitMapEmcalTrk->SetLineColor(632);" << std::endl;

   std::cout << "hHitMapMvtxTrk->Fill(" << trk.getHitR(1)*std::cos(trk.getHitPhi(1)) \
            << ", " << trk.getHitR(1)*std::sin(trk.getHitPhi(1)) << ");" << std::endl;
   std::cout << "hHitMapMvtxTrk->Fill(" << trk.getHitR(2)*std::cos(trk.getHitPhi(2)) \
            << ", " << trk.getHitR(2)*std::sin(trk.getHitPhi(2)) << ");" << std::endl;
   std::cout << "hHitMapMvtxTrk->Fill(" << trk.getHitR(3)*std::cos(trk.getHitPhi(3)) \
            << ", " << trk.getHitR(3)*std::sin(trk.getHitPhi(3)) << ");" << std::endl;
   std::cout << "hHitMapInttTrk->Fill(" << trk.getHitR(4)*std::cos(trk.getHitPhi(4)) \
            << ", " << trk.getHitR(4)*std::sin(trk.getHitPhi(4)) << ");" << std::endl;
   std::cout << "hHitMapInttTrk->Fill(" << trk.getHitR(5)*std::cos(trk.getHitPhi(5)) \
            << ", " << trk.getHitR(5)*std::sin(trk.getHitPhi(5)) << ");" << std::endl;
   std::cout << "hHitMapEmcalTrk->Fill(" << trk.getHitR(6)*std::cos(trk.getHitPhi(6)) \
            << ", " << trk.getHitR(6)*std::sin(trk.getHitPhi(6)) << ");" << std::endl;

   std::cout << "hHitMapMvtxTrk->Draw(\"BOX same\");" << std::endl;
   std::cout << "hHitMapInttTrk->Draw(\"BOX same\");" << std::endl;
   std::cout << "hHitMapTpcTrk->Draw(\"BOX same\");" << std::endl;
   std::cout << "hHitMapEmcalTrk->Draw(\"BOX same\");" << std::endl;
}

void InttSeedTrackPerformance::EventJudge(Int_t eventNum, Double_t targetVal, Double_t minLim, Double_t maxLim, bool bIn){
   if(bIn){
      if((targetVal > minLim)&&(targetVal < maxLim)){
         m_vTargetEvents.push_back(eventNum);
      }
   }else{
      if((targetVal < minLim)||(targetVal > maxLim)){
         m_vTargetEvents.push_back(eventNum);
      }
   }

}


void InttSeedTrackPerformance::ChecKuma(std::string checkNo){
    std::cout <<"CheeeeeeeeeeeeeeeeeeeeeeeeeeeeeeecKuma"<< checkNo << std::endl;
}


#endif // #ifdef InttSeedTrackPerformance_cxx





