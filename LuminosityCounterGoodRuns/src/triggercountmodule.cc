#include "triggercountmodule.h"
#include <globalvertex/GlobalVertexMap.h>
#include <globalvertex/GlobalVertexMapv1.h>
#include <globalvertex/GlobalVertex.h>
#include <fun4all/Fun4AllReturnCodes.h>
#include <phool/PHCompositeNode.h>
#include <phool/getClass.h>
#include <ffaobjects/EventHeaderv1.h>
#include <ffarawobjects/Gl1Packetv2.h>
using namespace std;


//____________________________________________________________________________..
triggercountmodule::triggercountmodule(const std::string &filename, int rn, int segn, int maxseg, int debug, const std::string &name):
  SubsysReco(name)//).c_str())
{
  _rn = rn;
  _filename = filename;
  _name = name;

  _tree = new TTree("_tree","a tree to count events");
  _debug = debug;
  _nseg = segn;
  _lastseg = maxseg;
}

//____________________________________________________________________________..
triggercountmodule::~triggercountmodule()
{

}

//____________________________________________________________________________..
int triggercountmodule::Init(PHCompositeNode *topNode)
{
  
  if(_debug > 1) cout << "Begin init: " << endl;
  
  _tree->Branch("badFlag",&_badFlag,"badFlag/I");
  _tree->Branch("startLive",_startLive,"startLive[64]/l");
  _tree->Branch("endLive",_endLive,"endLive[64]/l");
  _tree->Branch("startScal",_startScal,"startScal[64]/l");
  _tree->Branch("endScal",_endScal,"endScal[64]/l");
  _tree->Branch("nevt",&_evtn,"nevt/l");
  _tree->Branch("avgPS",_avgPS,"avgPS[64]/D");
  _tree->Branch("trigCounts",_trigCounts,"trigCounts[6][64]/l");
  _tree->Branch("eMBDlive",_eMBDlive,"eMBDlive[5]/D");
  _tree->Branch("startBCO",&_startBCO,"startBCO/l");
  _tree->Branch("endBCO",&_endBCO,"endBCO/l");
  _tree->Branch("nBunch",&_nBunch,"nBunch/I");
  _tree->Branch("startRaw",_startRaw,"startRaw[64]/l");
  _tree->Branch("endRaw",_endRaw,"endRaw[64]/l");
  _mbzhist = new TH1D("mbzhist","",600,-300,300);
  _bunchHist = new TH1D("bunchHist","",121,0,121);
  if(_debug > 0)
    {
      cout << "Printing tree: " << endl;
      _tree->Print();
    }
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int triggercountmodule::InitRun(PHCompositeNode *topNode)
{
  if(_debug > 1) cout << "Initializing!" << endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..


int triggercountmodule::process_event(PHCompositeNode *topNode)
{

  if(_debug > 1) cout << endl << endl << endl << "triggercountmodule: Beginning event processing" << endl;

    Gl1Packetv2* gl1 = findNode::getClass<Gl1Packetv2>(topNode, "GL1Packet");
  if(!gl1)
    {
      cout << "No trigger info!" << endl;
      return Fun4AllReturnCodes::ABORTRUN;
    }

  _bunchHist->Fill(gl1->lValue(0,"BunchNumber"));
  
  if(_evtn == 0)
    {
      _startBCO = gl1->lValue(0,"BCO");
      for(int i=0; i<64; ++i)
	{
	  _startLive[i] = gl1->lValue(i,1);
	  if(_debug > 2) cout << gl1->lValue(i,1) << endl;
	  _startScal[i] = gl1->lValue(i,2);
	  _startRaw[i] = gl1->lValue(i,0);
	}
    }
  //cout << _evtn << endl;
  ++_evtn;
  _endBCO = gl1->lValue(0,"BCO");
  for(int i=0; i<64; ++i)
    {
      _endLive[i] = gl1->lValue(i,1);
      if(_debug > 2) cout << "end" << gl1->lValue(i,1) << endl;
      _endScal[i] = gl1->lValue(i,2);
      _endRaw[i] = gl1->lValue(i,0);
    }
  
  if(_debug > 1) cout << "Getting gl1 trigger vector from: " << gl1 << endl;
  long long unsigned int _trigvec = gl1->getScaledVector();

  int ismbtrig = (_trigvec >> 10) & 1;

  GlobalVertexMap* gvtxmap = findNode::getClass<GlobalVertexMapv1>(topNode, "GlobalVertexMap");

  float zvtx = NAN;

  if(gvtxmap)
    {
      GlobalVertex* gvtx = gvtxmap->begin()->second;
      if(gvtx)
        {
          auto startIter = gvtx->find_vertexes(_vtxtype);
          auto endIter = gvtx->end_vertexes();
          for(auto iter = startIter; iter != endIter; ++iter)
            {
              const auto &[type, vertexVec] = *iter;
              if(type != _vtxtype) continue;
              for(const auto *vertex : vertexVec)
                {
                  if(!vertex) continue;
                  zvtx = vertex->get_z();
                }
            }
        }
    }
  
  int zthresh[5] = {30,60,200,10,1000};



  for(int i=0; i<5; ++i)
    {
      if(abs(zvtx) < zthresh[i] && !std::isnan(zvtx))
	{
	  for(int j=0; j<64; ++j)
	    {
	      if((_trigvec >> j) & 1) _trigCounts[i][j]++;
	    }
	}
    }

  for(int j=0; j<64; ++j)
    {
      if((_trigvec >> j) & 1) _trigCounts[5][j]++;
    }

  if(!ismbtrig)
    {
      if(_debug > 2) cout << "non MB event, not counting" << endl;
      return Fun4AllReturnCodes::ABORTEVENT;
    }

  if(std::isnan(zvtx))
    {
      if(_debug > 0) cout << "zvtx is NAN after attempting to grab it. ABORT EVENT!" << endl;
      return Fun4AllReturnCodes::ABORTEVENT;
    }
  if(abs(zvtx) > 300)
    {
      if(_debug > 2) cout << "zvtx > 300. zvtx: " << zvtx << endl;
      return Fun4AllReturnCodes::ABORTEVENT;
    }

  _mbzhist->Fill(zvtx);

  return Fun4AllReturnCodes::EVENT_OK;
    
}
//____________________________________________________________________________..
int triggercountmodule::ResetEvent(PHCompositeNode *topNode)
{
  if (Verbosity() > 0)
    {
      std::cout << "triggercountmodule::ResetEvent(PHCompositeNode *topNode) Resetting internal structures, prepare for next event" << std::endl;
    }
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int triggercountmodule::EndRun(const int runnumber)
{
  if (Verbosity() > 0)
    {
      std::cout << "triggercountmodule::EndRun(const int runnumber) Ending Run for Run " << runnumber << std::endl;
    }
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int triggercountmodule::End(PHCompositeNode *topNode)
{
  if (Verbosity() > 0)
    {
      std::cout << "triggercountmodule::End(PHCompositeNode *topNode) This is the End..." << std::endl;
    }
  //cout << _evtn << endl;
  if(_nseg != _lastseg && _evtn < 49500)
    {
      _badFlag = 1;
    }

  for(int i=0; i<64; ++i)
    {
      if(_endScal[i] == _startScal[i]) _avgPS[i] = 0;
      else _avgPS[i] = ((float)(_endLive[i]-_startLive[i]))/(_endScal[i]-_startScal[i]);
      if(_debug > 2) cout << "realend" << _endLive[i] << endl;
    }
  for(int i=0; i<5; ++i)
    {
      _eMBDlive[i] = _avgPS[10]*_trigCounts[i][10];
    }

  int avgBunchCounts = _bunchHist->Integral() / 121.0;
  
  for(int i=1; i<122; ++i)
    {
      if(_bunchHist->GetBinContent(i) > avgBunchCounts/5)
	{
	  _nBunch++;
	}
    }
  _outfile = new TFile(_filename.c_str(), "RECREATE");
  _outfile->cd();
  _bunchHist->Write();
  _mbzhist->Write();
  if(_debug > 0) _tree->Print();
  _tree->Fill();
  _tree->Write();
  _outfile->Write();
  _outfile->Close();


  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int triggercountmodule::Reset(PHCompositeNode *topNode)
{
  if (Verbosity() > 0)
    {
      std::cout << "triggercountmodule::Reset(PHCompositeNode *topNode) being Reset" << std::endl;
    }
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
void triggercountmodule::Print(const std::string &what) const
{
  std::cout << "triggercountmodule::Print(const std::string &what) const Printing info for " << what << std::endl;
}
