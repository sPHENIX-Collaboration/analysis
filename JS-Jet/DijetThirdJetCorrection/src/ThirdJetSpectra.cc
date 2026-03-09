//____________________________________________________________________________..
//
// This is a template for a Fun4All SubsysReco module with all methods from the
// $OFFLINE_MAIN/include/fun4all/SubsysReco.h baseclass
// You do not have to implement all of them, you can just remove unused methods
// here and in ThirdJetSpectra.h.
//
// ThirdJetSpectra(const std::string &name = "ThirdJetSpectra")
// everything is keyed to ThirdJetSpectra, duplicate names do work but it makes
// e.g. finding culprits in logs difficult or getting a pointer to the module
// from the command line
//
// ThirdJetSpectra::~ThirdJetSpectra()
// this is called when the Fun4AllServer is deleted at the end of running. Be
// mindful what you delete - you do loose ownership of object you put on the node tree
//
// int ThirdJetSpectra::Init(PHCompositeNode *topNode)
// This method is called when the module is registered with the Fun4AllServer. You
// can create historgrams here or put objects on the node tree but be aware that
// modules which haven't been registered yet did not put antyhing on the node tree
//
// int ThirdJetSpectra::InitRun(PHCompositeNode *topNode)
// This method is called when the first event is read (or generated). At
// this point the run number is known (which is mainly interesting for raw data
// processing). Also all objects are on the node tree in case your module's action
// depends on what else is around. Last chance to put nodes under the DST Node
// We mix events during readback if branches are added after the first event
//
// int ThirdJetSpectra::process_event(PHCompositeNode *topNode)
// called for every event. Return codes trigger actions, you find them in
// $OFFLINE_MAIN/include/fun4all/Fun4AllReturnCodes.h
//   everything is good:
//     return Fun4AllReturnCodes::EVENT_OK
//   abort event reconstruction, clear everything and process next event:
//     return Fun4AllReturnCodes::ABORT_EVENT; 
//   proceed but do not save this event in output (needs output manager setting):
//     return Fun4AllReturnCodes::DISCARD_EVENT; 
//   abort processing:
//     return Fun4AllReturnCodes::ABORT_RUN
// all other integers will lead to an error and abort of processing
//
// int ThirdJetSpectra::ResetEvent(PHCompositeNode *topNode)
// If you have internal data structures (arrays, stl containers) which needs clearing
// after each event, this is the place to do that. The nodes under the DST node are cleared
// by the framework
//
// int ThirdJetSpectra::EndRun(const int runnumber)
// This method is called at the end of a run when an event from a new run is
// encountered. Useful when analyzing multiple runs (raw data). Also called at
// the end of processing (before the End() method)
//
// int ThirdJetSpectra::End(PHCompositeNode *topNode)
// This is called at the end of processing. It needs to be called by the macro
// by Fun4AllServer::End(), so do not forget this in your macro
//
// int ThirdJetSpectra::Reset(PHCompositeNode *topNode)
// not really used - it is called before the dtor is called
//
// void ThirdJetSpectra::Print(const std::string &what) const
// Called from the command line - useful to print information when you need it
//
//____________________________________________________________________________..

#include "ThirdJetSpectra.h"

#include <fun4all/Fun4AllReturnCodes.h>

#include <phool/PHCompositeNode.h>

//____________________________________________________________________________..
ThirdJetSpectra::ThirdJetSpectra(std::string numb, const std::string &name):
 SubsysReco(name)
{
		this->segment_numb=numb;
  std::cout << "ThirdJetSpectra::ThirdJetSpectra(const std::string &name) Calling ctor" << std::endl;
		xj = new TH1F("xj", "Dijet Momentum Imballance leading to subleading; x_{j}; N_{counts}", 20, 0,1);
		xj_strict =new TH1F("xj_strict", "Dijet Momentum imballance leading to subleading (Events passing Dijet Event Cuts); x_{j}; N_{counts}", 20, 0, 1);
		xj_onl =new TH1F("xj_onl", "Dijet Momentum imballance leading to subleading without a third jet; x_{j}; N_{counts}", 20, 0, 1);
		xj_strict_onl =new TH1F("xj_strict_onl", "Dijet Momentum imballance leading to subleading without a third jet (Events passing Dijet Event Cuts); x_{j}; N_{counts}", 20, 0, 1);
		first_jet_pt=new TH1F("first_jet_pt", "p_{T} of leading jet; p_{T} [GeV/c]; N_{counts}", 60, -0.5, 59.5);
		second_jet_pt=new TH1F("second_jet_pt", "p_{T} of subleading jet; p_{T} [GeV/c]; N_{counts}", 60, -0.5, 59.5);
		third_jet_pt=new TH1F("third_jet_pt", "p_{T} of subsubleading jet; p_{T} [GeV/c]; N_{counts}", 60, -0.5, 59.5);
		first_jet_E=new TH1F("first_jet_E", "E of leading jet; E [GeV/c]; N_{counts}", 60, -0.5, 59.5);
		second_jet_E=new TH1F("second_jet_E", "E of subleading jet; E [GeV/c]; N_{counts}", 60, -0.5, 59.5);
		third_jet_E=new TH1F("third_jet_E", "E of subsubleading jet; E [GeV/c]; N_{counts}", 60, -0.5, 59.5);
		first_jet_phi=new TH1F("first_jet_phi", "#varphi of leading jet; #varphi; N_{counts}", 64, -0.5, 2*PI);
		second_jet_phi=new TH1F("second_jet_phi", "#varphi of leading jet; #varphi; N_{counts}", 64, 0, 2*PI);
		third_jet_phi=new TH1F("third_jet_phi", "#varphi of leading jet; #varphi; N_{counts}", 64, 0, 2*PI);
		first_jet_eta=new TH1F("first_jet_eta", "#eta of leading jet; #eta; N_{counts}", 24, -1.11, 1.11);
		second_jet_eta=new TH1F("second_jet_eta", "#eta of leading jet; #eta; N_{counts}", 24, -1.11, 1.11);
		third_jet_eta=new TH1F("third_jet_eta", "#eta of leading jet; #eta; N_{counts}", 24, -1.11, 1.11);
		dphi_12=new TH1F("dphi_12", "#Delta #varphi of leading jet to subleading; #Delta #varphi; N_{counts}", 64, -PI, PI);
		dphi_13=new TH1F("dphi_13", "#Delta #varphi of leading jet to subsubleading; #Delta #varphi; N_{counts}", 64, -PI, PI);
		dphi_23=new TH1F("dphi_23", "#Delta #varphi of subleading jet to subsubleading;  #Delta #varphi; N_{counts}", 64, -PI, PI);
		xj_12 =new TH1F("xj_12", "Dijet Momentum imballance leading to subleading (with third); x_{j}; N_{counts}", 100, 0, 1);
		xj_13 =new TH1F("xj_13", "Dijet Momentum imballance leading to subsubleading; x_{j}; N_{counts}", 100, 0, 1);
		xj_23 =new TH1F("xj_23", "Dijet Momentum imballance subleading to subsubleading; x_{j}; N_{counts}", 100, 0, 1);
		xj_strict_12 =new TH1F("xj_strict_12", "Dijet Momentum imballance leading to subleading with third (Events passing Dijet Event Cuts); x_{j}; N_{counts}", 100, 0, 1);
		xj_strict_13 =new TH1F("xj_strict_13", "Dijet Momentum imballance leading to subsubleading (Events passing Dijet Event Cuts); x_{j}; N_{counts}", 100, 0, 1);
		xj_strict_23 =new TH1F("xj_strict_23", "Dijet Momentum imballance subleading to subsubleading (Events passing Dijet Event Cuts); x_{j}; N_{counts}", 100, 0, 1);
		third_jet_pt_dec = new TH1F("third_jet_pt_dec", "Average Decorrelation of leading and subleading jet as a function of subsubleading jet p_{T}; p_{T} [GeV]; < |#pi - #Delta #varphi| >", 60, -0.5, 59.5);	
		third_jet_pt_dec_strict = new TH1F("third_jet_pt_dec_strict", "Average Decorrelation of leading and subleading jet as a function of subsubleading jet p_{T} (Events passing Dijet Event Cuts); p_{T} [GeV]; < |#pi - #Delta #varphi| >", 60, -0.5, 59.5);	
		decorr = new TH1F("decorr", "Decorrelation of leading to subleading jets; | #pi -#Delta #varphi |; N_{counts}", 64, 0, PI); 
		decorr_strict = new TH1F("decorr_strict", "Decorrelation of leading to subleading jets (Events passing Dijet Event Cuts); | #pi -#Delta #varphi |; N_{counts}", 64, 0, PI);
		third_jet_pt_dec_n = new TH2F( "third_jet_pt_dec_n", "Decorrelation of leadidng to subleading jets as fuction of third jet pt; p_{T}^{ssl-jet} [GeV]; |#pi - #Delta #varphi |; N_{counts}", 60, -0.5, 59.5, 64, 0, PI);
		third_jet_pt_dec_strict_n = new TH2F("third_jet_pt_dec_strict_n", "Decorrelation of leadidng to subleading jets as fuction of third jet pt (Events passing Dijet Event Cuts); p_{T}^{ssl-jet} [GeV]; |#pi - #Delta #varphi |; N_{counts}", 60, -0.5, 59.5,64,  0, PI);

		dphi_123=new TH3F( "dphi_123", "azimuthal angle of jet objects; #varphi_{leading}; #varphi_{subleading}; #varphi_{subsubleading}; N_{events}", 64, 0, 2*PI, 64, 0, 2*PI, 64, 0, 2*PI);
		pt_123= new TH3F("pt123", "Transverse momentum of the jets; p_{T}^{leading} [GeV/c]; p_{T}^{subleading}[GeV/c]; p_{T}^{subsubleading} [GeV/c]; N_{events}", 60, -0.5, 59.5, 60, -0.5, 59.5, 60, -0.5, 59.5); 
		DiJEC=new DijetEventCuts(30., 10., 1000., PI/2. );	
		DiJEC_strict=new DijetEventCuts(15., 8., 0.7, 3*PI/4. ); //actual kinematics	
}

//____________________________________________________________________________..
ThirdJetSpectra::~ThirdJetSpectra()
{
  std::cout << "ThirdJetSpectra::~ThirdJetSpectra() Calling dtor" << std::endl;
}

//____________________________________________________________________________..
int ThirdJetSpectra::Init(PHCompositeNode *topNode)
{
  std::cout << "ThirdJetSpectra::Init(PHCompositeNode *topNode) Initializing" << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int ThirdJetSpectra::InitRun(PHCompositeNode *topNode)
{
  std::cout << "ThirdJetSpectra::InitRun(PHCompositeNode *topNode) Initializing for Run XXX" << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

std::vector<fastjet::PseudoJet> ThirdJetSpectra::findAllJets(HepMC::GenEvent* e1)
{
	//do the fast jet clustering, antikt r=-0.4
	fastjet::JetDefinition jetdef(fastjet::antikt_algorithm, 0.4);
	std::vector<fastjet::PseudoJet> input, output;
	for(HepMC::GenEvent::particle_const_iterator iter = e1->particles_begin(); iter != e1->particles_end(); ++iter)
	{
		if(!(*iter)->end_vertex() && (*iter)->status() == 1){
			auto p=(*iter)->momentum();
			fastjet::PseudoJet pj( p.px(), p.py(), p.pz(), p.e());
			pj.set_user_index((*iter)->barcode());
			input.push_back(pj);
		}
	}
	if(input.size() == 0 ) return input;
	fastjet::ClusterSequence js (input, jetdef);
	auto j = fastjet::sorted_by_pt(js.inclusive_jets(3.));
	for(auto j1:j){
		if(j1.pt() < 3.) continue;
		/*if(j1.pt() < output.back.pt())*/ output.push_back(j1); //just keep in the corect format
	}
			
	return output;
}
void ThirdJetSpectra::getJetTripplet(JetContainerv1* jc, std::vector<Jetv2*>* ds, bool strict)
{
	float dphi=PI/2.;
	if(strict){
		 dphi=3*PI/4.;
	}
	
	for(auto j1:*jc){
		for(auto j2:*jc){
			if(j2->get_pt() >= j1->get_pt() ) continue;
			float p1=j1->get_phi();
			float p2=j2->get_phi();
			if(std::abs(p1-p2) > dphi && std::abs(p1-p2) < PI+dphi ){
				ds->push_back((Jetv2*)j1);
				ds->push_back((Jetv2*)j2);
			}
			float maxpt=0;
			Jetv2* thj;
			for(auto j3:*jc){
				if(j3->get_phi() == p1 || j3->get_phi() == p2) continue;
				if(j3->get_pt() > maxpt){
					maxpt=j3->get_pt();
					thj=(Jetv2*)j3;
				}
			}
			ds->push_back(thj);
		}
	}
	return;
}
void ThirdJetSpectra::getJetPair(JetContainerv1* jc, std::vector<Jetv2*>* ds, bool strict) 
{ 
	float dphi=PI/2.;
	if(strict){
		dphi=3*PI/4.;
	}
	for(auto j1:*jc){
		for(auto j2:*jc){
			float p1=j1->get_phi();
			float p2=j2->get_phi();
			if(j2->get_pt() >= j1->get_pt() ) continue;
			if(std::abs(p1-p2) > dphi && std::abs(p1-p2) < PI+dphi ){
				ds->push_back((Jetv2*)j1);
				ds->push_back((Jetv2*)j2);
			}
		}
	}
	return;
}
//____________________________________________________________________________..
int ThirdJetSpectra::process_event(PHCompositeNode *topNode)
{
	PHHepMCGenEventMap* em = findNode::getClass<PHHepMCGenEventMap>(topNode, "PHHepMCGenEventMap");
	if(!em) return Fun4AllReturnCodes::EVENT_OK;

	for(PHHepMCGenEventMap::ConstIter eventIter = em->begin(); eventIter != em->end(); ++eventIter)
	{
		bool is_three=false;
		PHHepMCGenEvent* pHe=eventIter->second;
		if(!pHe ||  pHe->get_embedding_id() != 0 ) continue;
		HepMC::GenEvent* truthEvent = pHe->getEvent();
		if(!truthEvent) continue;
		auto jets=findAllJets(truthEvent);
		if(jets.size() < 2) continue;
		n_try++;
		std::cout<<jets.at(0).pt() <<" is lead jet pt" <<std::endl;
		JetContainerv1* jc=new JetContainerv1();
		std::vector<float> fake_ratio; //this is just to be able to use my dijet cut object
		for(auto i:jets){
			fake_ratio.push_back(0.5);	
			auto j = jc->add_jet();
			j->set_px(i.px());
			j->set_py(i.py());
			j->set_pz(i.pz());
			j->set_e(i.e());
			//for(auto cmp: i.constituents()){j->insert_comp(Jet::SRC::PARTICLE, cmp.user_index());}
		}
		std::array<float, 3> vertex {0,0,0};	
		bool is_good_loose=DiJEC->passesTheCut(jc, fake_ratio, fake_ratio, 0.5, vertex, fake_ratio);
		bool is_good_strict=DiJEC_strict->passesTheCut(jc, fake_ratio, fake_ratio, 0.5, vertex, fake_ratio);	
		if(jets.size() >= 3) pt_123->Fill(jets.at(0).pt(), jets.at(1).pt(), jets.at(2).pt());
		if(!is_good_loose) continue;
		if (jets.size() >= 3){
			n_three++;
			is_three=true;
		}
		n_evts++;
		std::vector<Jetv2*> dijet_set, dijet_set_strict;
		if(is_three){
			getJetTripplet(jc, &dijet_set, false);
			if(is_good_strict) getJetTripplet(jc, &dijet_set_strict, true);
		}
		else{
			getJetPair(jc, &dijet_set, false);
			if(is_good_strict) getJetPair(jc, &dijet_set_strict, true);
		}
		float pt1=dijet_set[0]->get_pt();
		float pt2=dijet_set[1]->get_pt();
		Jetv2* j1=dijet_set[0], *j2=dijet_set[1];
		float phi1=j1->get_phi(), phi2=j2->get_phi();
		first_jet_pt->Fill(pt1);
		first_jet_phi->Fill(j1->get_phi());
		first_jet_eta->Fill(j1->get_eta());
		first_jet_E->Fill(j1->get_e());
		second_jet_pt->Fill(pt2);
		second_jet_phi->Fill(j2->get_phi());
		second_jet_eta->Fill(j2->get_eta());
		second_jet_E->Fill(j2->get_e());
		dphi_12->Fill(phi1 - phi2);
		float x_j_12 = pt2/pt1;
		xj->Fill(x_j_12);
		if(!is_three) xj_onl->Fill(x_j_12);
		if(is_good_strict){
			float xjs_12=dijet_set_strict[1]->get_pt()/dijet_set_strict[0]->get_pt();
			xj_strict_12->Fill(xjs_12);
			xj_strict->Fill(xjs_12);
			if(!is_three) xj_strict_onl->Fill(xjs_12);
		}
		if(is_three){
			Jetv2* j3=dijet_set[3];
			float pt3=j3->get_pt();
			float phi3=j3->get_phi();
			std::cout<<x_j_12<<std::endl;
			xj_12->Fill(x_j_12);
			third_jet_pt->Fill(pt3);
			third_jet_phi->Fill(phi3);
			third_jet_eta->Fill(j3->get_eta());
			third_jet_E->Fill(j3->get_e());
			dphi_13->Fill(phi1-phi3);
			dphi_23->Fill(phi2-phi3);
			float x13=pt3/pt1;
			float x23=pt3/pt2;
			xj_13->Fill(x13);
			xj_23->Fill(x23);
			float decor=std::abs(PI - phi1+phi2);
			decorr->Fill(decor);
			dphi_123->Fill(std::abs(phi1-phi2), std::abs(phi1-phi3), std::abs(phi2-phi3));
			third_jet_pt_dec_n->Fill(pt3, decor);
			third_jet_pt_dec->Fill(pt3, decor);
			if(is_good_strict){
				Jetv2* j1_s=dijet_set_strict[0];
				Jetv2* j2_s=dijet_set_strict[1];
				Jetv2* j3_s=dijet_set_strict[2];
				float xjs_13=j3_s->get_pt()/j1_s->get_pt();
				float xjs_23=j3_s->get_pt()/j2_s->get_pt();
				float dc_s=std::abs(PI-j1_s->get_phi()+j2_s->get_phi());
				xj_strict_13->Fill(xjs_13);
				xj_strict_23->Fill(xjs_23);
				third_jet_pt_dec_strict->Fill(j3_s->get_pt(), dc_s);	
				third_jet_pt_dec_strict_n->Fill(j3_s->get_pt(), dc_s);	
				decorr_strict->Fill(dc_s);
			}
		}
		
	}
  	return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int ThirdJetSpectra::ResetEvent(PHCompositeNode *topNode)
{
  //std::cout << "ThirdJetSpectra::ResetEvent(PHCompositeNode *topNode) Resetting internal structures, prepare for next event" << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int ThirdJetSpectra::EndRun(const int runnumber)
{
  std::cout << "ThirdJetSpectra::EndRun(const int runnumber) Ending Run for Run " << runnumber << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int ThirdJetSpectra::End(PHCompositeNode *topNode)
{
  std::cout << "ThirdJetSpectra::End(PHCompositeNode *topNode) This is the End..." << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int ThirdJetSpectra::Reset(PHCompositeNode *topNode)
{
// std::cout << "ThirdJetSpectra::Reset(PHCompositeNode *topNode) being Reset" << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
void ThirdJetSpectra::Print(const std::string &what) const
{
	std::cout << "ThirdJetSpectra::Print(const std::string &what) const Printing info for " << what << std::endl;
	//I will need to normalize the decorreclation average after the fact, but thats ok 
  	std::cout<<"The number of events with at least three final state jets of pt> 3 GeV " <<n_three <<" and the number of events with an ID'ed dijet pair " <<n_evts <<std::endl;
	std::cout<<"looked at " <<n_try<<" events" <<std::endl; 
  	TFile* f1=new TFile(Form("Third_Jet_Contrib_%s.root", segment_numb.c_str()), "RECREATE");
	first_jet_pt->Write();
	first_jet_E->Write();
	first_jet_eta->Write();
	first_jet_phi->Write();	
	second_jet_pt->Write();
	second_jet_E->Write();
	second_jet_eta->Write();
	second_jet_phi->Write();	
	third_jet_pt->Write();
	third_jet_E->Write();
	third_jet_eta->Write();
	third_jet_phi->Write();	

	dphi_12->Write();
	dphi_13->Write();
	dphi_23->Write();
	dphi_123->Write();
	pt_123->Write();

	decorr->Write();
	decorr_strict->Write();
	third_jet_pt_dec->Write();
	third_jet_pt_dec_strict->Write();
	third_jet_pt_dec_n->Write();
	third_jet_pt_dec_strict_n->Write();

	xj->Write();
	xj_strict->Write();
	xj_onl->Write();
	xj_strict_onl->Write();
	xj_12->Write();
	xj_13->Write();
	xj_23->Write();
	xj_strict_12->Write();
	xj_strict_13->Write();
	xj_strict_23->Write();

	f1->Write();
	f1->Close();	
}
