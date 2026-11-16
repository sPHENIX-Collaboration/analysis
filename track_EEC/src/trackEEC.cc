#include "trackEEC.h"

#include <fastjet/ClusterSequence.hh>

#include "TMath.h"

#include <trackbase_historic/SvtxTrack.h>
#include <trackbase_historic/SvtxTrackMap.h>
#include <trackbase_historic/TrackSeed.h>


//____________________________________________________________________________..
trackEEC::trackEEC(const std::string &name)
  : SubsysReco(name)
{
}

//____________________________________________________________________________..
int trackEEC::Init(PHCompositeNode* /*topNode*/)
{

    outfile = new TFile(m_outfileName.c_str(), "RECREATE");

    const int nBins = 40;
    double bins[nBins+1];
    double min_dR = 1e-3;
    double max_dR = 0.8;
    double dR_shift = 0.0;
    double dRWidth = (log(max_dR+dR_shift) - log(min_dR + dR_shift))/nBins;
    for(int i=0; i<=nBins; i++){
        bins[i] = (min_dR + dR_shift)*exp(i*dRWidth) - dR_shift;
    }

    h_nEvents = new TH1D("h_nEvents","Number of Events",3,-0.5,2.5);
    h_crossing = new TH1D("h_crossing",";crossing",651,-150.5,500.5);
    h_nintt = new TH1D("h_nintt",";nintt",6,-0.5,5.5);
    h_chi2_ndf = new TH1D("h_chi2_ndf",";#chi^{2}/ndf",3000,0,300);
    h_nClus = new TH1D("h_nClus",";Number of Clusters on Track",81,-0.5,80.5);
    h_trackSpec = new TH1D("h_trackSpec","Track Spectrum;p_{T} [GeV]",200,0,100);
    h_track_etaPhi = new TH2D("h_track_etaPhi","Tracks;#phi;#eta",100,-TMath::Pi(),TMath::Pi(),50,-1.1,1.1);
    h_jetCons = new TH1D("h_jetCons",";Number of Jet Constituents",31,-0.5,30.5);
    h_jetSpec = new TH1D("h_jetSpec","Jet Spectrum;p_{T} [GeV]",100,0,100);
    h_jetConsSpec = new TH2D("h_jetConsSpec",";p_{T} [GeV];Number of Jet Constituents",100,0,100,31,-0.5,30.5);
    h_jet_etaPhi = new TH2D("h_jet_etaPhi","Jets;#phi;#eta",100,-TMath::Pi(),TMath::Pi(),50,-1.1,1.1);
    h_jetMass = new TH1D("h_jetMass","Jet Mass;M [GeV]",1000,0,100);
    h_inJetAll = new TH1D("hEEC_inJetAll","In-jet EEC all jet p_{T};#Delta R",nBins,&bins[0]);
    h_inJetAll_Q2 = new TH1D("hEEC_inJetAll_Q2","In-jet EEC all jet p_{T} with Q^{2};#Delta R",nBins,&bins[0]);
    h_wEEC = new TH1D("h_wEEC","wEEC, no track jet requirement;#Delta#phi",22,0.0,TMath::Pi());
    h_wEEC_jet = new TH1D("h_wEEC_jet","wEEC at least one jet;#Delta#phi",22,0.0,TMath::Pi());
    h_wEEC_jet_Q2 = new TH1D("h_wEEC_jet_Q2","wEEC at least one jet with Q^{2};#Delta#phi",22,0.0,TMath::Pi());
    h_wEEC_dijet = new TH1D("h_wEEC_dijet","wEEC dijet All Jet p_{T};#Delta#phi",22,0.0,TMath::Pi());
    h_wEEC_dijet_Q2 = new TH1D("h_wEEC_dijet_Q2","wEEC dijet with Q^{2} All Jet p_{T};#Delta#phi",22,0.0,TMath::Pi());


    for(int i=0; i<5; i++)
    {
        h_inJet[i] = new TH1D(std::format("hEEC_inJet_{:.0f}_{:.0f}",jet_pTs[i],jet_pTs[i+1]).c_str(),std::format("In-jet EEC {:.0f} < p_{{T}}^{{jet}} < {:.0f};#Delta R",jet_pTs[i],jet_pTs[i+1]).c_str(),nBins,&bins[0]);
        h_inJet_Q2[i] = new TH1D(std::format("hEEC_inJetAll_Q2_{:.0f}_{:.0f}",jet_pTs[i],jet_pTs[i+1]).c_str(),std::format("In-jet EEC with Q^2 {:.0f} < p_{{T}}^{{jet}} < {:.0f};#Delta R",jet_pTs[i],jet_pTs[i+1]).c_str(),nBins,&bins[0]);
    }

    return Fun4AllReturnCodes::EVENT_OK;
}

int trackEEC::process_event(PHCompositeNode *topNode)
{


    auto *trackmap = findNode::getClass<SvtxTrackMap>(topNode, m_trackMapName);
    if(!trackmap)
    {
        return Fun4AllReturnCodes::ABORTEVENT;
    }

    h_nEvents->Fill(0);

    std::vector<fastjet::PseudoJet> parts;

    for (const auto& [key, track] : *trackmap)
    {
        if (!track)
        {
            continue;
        }

        //track->identify(std::cout);

        h_crossing->Fill(track->get_crossing());
        if(track->get_crossing() != 0)
        {
            continue;
        }

        float px = track->get_px();
        float py = track->get_py();
        float pz = track->get_pz();

        float pt = sqrt(px*px + py*py);
        if(pt < 0.5) continue;
        //if(pt > 5) continue;
        float eta = std::atanh(pz / std::sqrt(pt*pt + pz*pz));
        float phi = std::atan2(py, px);

        int n_intt = 0;

        std::vector<TrkrDefs::cluskey> clus_keys;
        //std::copy(track->begin_cluster_keys(), track->end_cluster_keys(), std::back_inserter(clus_keys));

        int nClus = 0;
        TrackSeed *sil_seed = track->get_silicon_seed();
        if(sil_seed)
        {
            for(auto iter = sil_seed->begin_cluster_keys(); iter != sil_seed->end_cluster_keys(); ++iter)
            {
                TrkrDefs::cluskey ckey = *iter;
                TrkrDefs::TrkrId id = static_cast<TrkrDefs::TrkrId>(TrkrDefs::getTrkrId(ckey));
                //std::cout << "working on sil seed " << nClus << " out of " << sil_seed->size_cluster_keys() << " cluskey: " << ckey << " with TrkrId: " << id << " with name: " << TrkrDefs::TrkrNames.at(id)<< std::endl;
                if(id == TrkrDefs::inttId) n_intt++;
                nClus++;
            }
        }
        int nTPC = 0;
        TrackSeed *tpc_seed = track->get_tpc_seed();
        if(tpc_seed)
        {
            for(auto iter = tpc_seed->begin_cluster_keys(); iter != tpc_seed->end_cluster_keys(); ++iter)
            {
                TrkrDefs::cluskey ckey = *iter;
                TrkrDefs::TrkrId id = static_cast<TrkrDefs::TrkrId>(TrkrDefs::getTrkrId(ckey));
                //std::cout << "working on tpc seed " << nTPC << " out of " << tpc_seed->size_cluster_keys() << " cluskey: " << ckey << " with TrkrId: " << id << " with name: " << TrkrDefs::TrkrNames.at(id)<< std::endl;
                if(id == TrkrDefs::inttId) n_intt++;
                nClus++;
                nTPC++;
            }
        }

        h_nintt->Fill(n_intt);
        h_nClus->Fill(nClus);

        if(n_intt <= 0) continue;

        h_chi2_ndf->Fill(1.0 * track->get_chisq() / track->get_ndf());
        
        if(1.0 * track->get_chisq() / track->get_ndf() > 20) continue;

        h_trackSpec->Fill(pt);
        h_track_etaPhi->Fill(phi,eta);

        float p = sqrt(px*px + py*py + pz*pz);
        float E = sqrt(p*p + m_pi*m_pi);

        fastjet::PseudoJet tmp_pj(px, py, pz, E);
        parts.push_back(tmp_pj);
    }

    fastjet::ClusterSequence cs(parts, jetDef);
    std::vector<fastjet::PseudoJet> jets = sorted_by_pt( jetSel( cs.inclusive_jets() ) );

    bool goodDijet = false;
    if((int)jets.size() >= 2)
    {
        double dPhiJets = std::abs(jets[0].phi_std() - jets[1].phi_std());
        if(dPhiJets > TMath::Pi()) dPhiJets = 2*TMath::Pi() - dPhiJets;

        if(dPhiJets > 3.0*TMath::Pi()/4.0) goodDijet = true;
    }

    if((int)jets.size() >= 1) h_nEvents->Fill(1);
    if(goodDijet) h_nEvents->Fill(2);

    for(int i=0; i<(int)parts.size(); i++)
    {
       for(int j=i+1; j<(int)parts.size(); j++)
       {
            double dPhi = std::abs(parts[i].phi_std() - parts[j].phi_std());
            if(dPhi > TMath::Pi()) dPhi = 2*TMath::Pi() - dPhi;

            h_wEEC->Fill(dPhi, parts[i].perp() * parts[j].perp());
            if(jets.size() >= 1)
            {
                h_wEEC_jet->Fill(dPhi, parts[i].perp() * parts[j].perp());
                h_wEEC_jet_Q2->Fill(dPhi, parts[i].perp() * parts[j].perp()/pow(jets[0].perp(),2));
            }
            if(goodDijet)
            {
                h_wEEC_dijet->Fill(dPhi, parts[i].perp() * parts[j].perp());
                h_wEEC_dijet_Q2->Fill(dPhi, parts[i].perp() * parts[j].perp() / pow(0.5*(jets[0].perp() + jets[1].perp()),2));
            }
       }
    }

    std::vector<fastjet::PseudoJet> EECCons;
    for(auto jet : jets)
    {

        //if(jet.m() < 0.5) continue;

        h_jetCons->Fill((int) jet.constituents().size());

        h_jetSpec->Fill(jet.perp());
        h_jetConsSpec->Fill(jet.perp(), (int) jet.constituents().size());
        h_jet_etaPhi->Fill(jet.phi_std(), jet.pseudorapidity());
        h_jetMass->Fill(jet.m());

        int jet_pT_index = -1;
        for(int i=0; i<5; i++)
        {
            if(jet.perp() >= jet_pTs[i] && jet.perp() < jet_pTs[i+1])
            {
                jet_pT_index = i;
                break;
            }
        }

        EECCons.clear();
        for(auto part : parts)
        {
            double dPhi = std::abs(jet.phi_std() - part.phi_std());
            if(dPhi > TMath::Pi()) dPhi = 2*TMath::Pi() - dPhi;

            double dEta = jet.pseudorapidity() - part.pseudorapidity();
            
            double dR = sqrt(dPhi*dPhi + dEta*dEta);

            if(dR < 0.4) EECCons.push_back(part);
        }

        for(int i=0; i<(int)EECCons.size(); i++)
        {
            for(int j=i+1; j<(int)EECCons.size(); j++)
            {
                double dPhi = std::abs(EECCons[i].phi_std() - EECCons[j].phi_std());
                if(dPhi > TMath::Pi()) dPhi = 2*TMath::Pi() - dPhi;
                double dEta = EECCons[i].pseudorapidity() - EECCons[j].pseudorapidity();
                double dR = sqrt(dPhi*dPhi + dEta*dEta);
                
                h_inJetAll->Fill(dR, EECCons[i].perp() * EECCons[j].perp());
                h_inJetAll_Q2->Fill(dR, EECCons[i].perp() * EECCons[j].perp() / pow(jet.perp(),2));

                if(jet_pT_index >= 0)
                {
                    h_inJet[jet_pT_index]->Fill(dR, EECCons[i].perp() * EECCons[j].perp());
                    h_inJet_Q2[jet_pT_index]->Fill(dR, EECCons[i].perp() * EECCons[j].perp() / pow(jet.perp(),2));
                }
            }
        }
    }

    return Fun4AllReturnCodes::EVENT_OK;
}

int trackEEC::End(PHCompositeNode* /*topNode */)
{

    outfile->cd();

    h_nEvents->Write();
    h_crossing->Write();
    h_nintt->Write();
    h_chi2_ndf->Write();
    h_nClus->Write();
    h_trackSpec->Write();
    h_track_etaPhi->Write();
    h_jetCons->Write();
    h_jetSpec->Write();
    h_jetConsSpec->Write();
    h_jet_etaPhi->Write();
    h_jetMass->Write();
    h_inJetAll->Write();
    h_inJetAll_Q2->Write();
    for(int i=0; i<5; i++)
    {
        h_inJet[i]->Write();
        h_inJet_Q2[i]->Write();
    }
    h_wEEC->Write();
    h_wEEC_jet->Write();
    h_wEEC_jet_Q2->Write();
    h_wEEC_dijet->Write();
    h_wEEC_dijet_Q2->Write();

    outfile->Close();

    return Fun4AllReturnCodes::EVENT_OK;
}