#include "TPCGainDebug.h"

#include <trackbase/ActsGeometry.h>
// #include <trackbase/ClusterErrorPara.h>
// #include <trackbase/TrkrCluster.h>
#include <trackbase/TrkrDefs.h>
#include <trackbase/TrkrHit.h>
#include <trackbase/TrkrHitSetContainer.h>
#include <trackbase/TrackFitUtils.h>

#include <trackbase_historic/TrackSeedContainer_v1.h>

#include <trackbase/TpcDefs.h>

// #include <trackbase/TrkrClusterContainer.h>
// #include <trackbase/TrkrClusterHitAssoc.h>
// #include <trackbase/TrkrClusterIterationMapv1.h>
#include <trackbase/TrkrHitSet.h>

#include <globalvertex/GlobalVertex.h>
#include <globalvertex/GlobalVertexMap.h>
#include <globalvertex/SvtxVertex.h>
#include <globalvertex/SvtxVertexMap.h>

#include <trackbase_historic/TrackAnalysisUtils.h>
#include <trackbase_historic/ActsTransformations.h>
#include <trackbase_historic/SvtxTrack.h>
#include <trackbase_historic/SvtxTrackMap.h>
#include <trackbase_historic/TrackSeed.h>

#include <g4detectors/PHG4TpcCylinderGeom.h>
#include <g4detectors/PHG4TpcCylinderGeomContainer.h>
#include <trackermillepedealignment/HelicalFitter.h>

#include <fun4all/Fun4AllReturnCodes.h>
#include <fun4all/SubsysReco.h>

#include <phool/PHTimer.h>
#include <phool/getClass.h>
#include <phool/phool.h>
#include <phool/recoConsts.h>

#include <TFile.h>
#include <TH1F.h>
#include <TH1I.h>
#include <TNtuple.h>
#include <TVector3.h>
#include <TEnv.h>

#include <cmath>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <map>
#include <memory>  // for shared_ptr
#include <set>     // for _Rb_tree_cons...
#include <utility>
#include <vector>

using namespace std;

// TF1* cleverGaus(TH1* h, const char* title="h", Float_t c = 2.5, bool quietMode=true);
TPCGainDebug::TPCGainDebug(const string& /*name*/, const string& filename, const int runnumber)
  : SubsysReco("TPCGainDebug")
  , _ievent(0)
  , _iseed(0)
  , m_fSeed(NAN)
  , _save_ntuple(true)
  , _do_gain(true)
  , _save_debugHist(true)
  , _do_pedSigmaCut(true)
  // , _do_gain(true)
  , _save_sharkFin(false)
  , _isSharkFin(false)
  // , _isSharkFinForRun(false)
  // , _ntp_hit(nullptr)
  , _filename(filename)
  , _runnumber(runnumber)
  , _tfile(nullptr)
  , _timer(nullptr)
  , AllHitADCThreshold(-99999)
{
}

TPCGainDebug::~TPCGainDebug()
{
  if (Verbosity() >= 1)
    cout << "TPCGainDebug:: DONE "  << endl;
  delete _timer;
  delete gaintree;
  delete _ntp_hit;
  // delete pedhist;
}

int TPCGainDebug::Init(PHCompositeNode* /*topNode*/)
{
  _ievent = 0;
  m_run = _runnumber;

  // pedhist = new TH1F("pedhist", "pedhist", 251, -0.5, 1000.5);

  _tfile = new TFile(_filename.c_str(), "RECREATE");
  _tfile->SetCompressionLevel(7);

  if (_save_ntuple || _save_sharkFin){
    _ntp_hit = new TTree("ntp_allhit",Form("all hits > %.2f", AllHitADCThreshold));
    _ntp_hit->Branch("run",&m_run);
    _ntp_hit->Branch("event",&m_event);
    _ntp_hit->Branch("side",&a_side);
    _ntp_hit->Branch("sector",&a_sector);
    _ntp_hit->Branch("layer",&a_layer);
    _ntp_hit->Branch("phibin",&a_phibin);
    _ntp_hit->Branch("phi",&a_phi);
    _ntp_hit->Branch("pedmean",&a_pedmean);
    _ntp_hit->Branch("pedwidth",&a_pedwidth);
    _ntp_hit->Branch("tbin",&a_tbin);
    _ntp_hit->Branch("adc",&a_adc);
    _ntp_hit->Branch("x",&a_x);
    _ntp_hit->Branch("y",&a_y);
    _ntp_hit->Branch("z",&a_z);
    if (_save_sharkFin){
      _ntp_hit->Branch("isSharkFin",&a_isSharkFin);
    }
    // _ntp_hit = new TNtuple("ntp_allhit", "svtxhit => max truth",
    //     "run:event:seed:hitID:e:adc:layer:sector:side:"
    //     "phibin:tbin:phi:x:y:z"
    //     );
    a_tbin.reserve(360);
    a_adc.reserve(360);
    a_x.reserve(360);
    a_y.reserve(360);
    a_z.reserve(360);
  }

  if(_save_sharkFin){
    h1I_event_shark = new TH1I("h1I_event_shark", ";event number;", 25000, 0, 25000);

  }
  if(_do_gain){
    gaintree = new TTree("tgain","TPC Gain Tree");
    gaintree->Branch("run",&m_run);
    gaintree->Branch("event",&m_event);
    gaintree->Branch("side",&m_side);
    gaintree->Branch("sector",&m_sector);
    gaintree->Branch("layer",&m_layer);
    gaintree->Branch("phibin",&m_phibin);
    gaintree->Branch("tbin",&m_tbin);
    gaintree->Branch("adcsum",&m_adcsum);
    gaintree->Branch("pathlength",&m_pathlength);
    gaintree->Branch("phisize",&m_phisize);
    // gaintree->Branch("errorcode",&m_errorcode);
  }

  // float tbin_i = 100;
  // float tbin_f = 250;
  // h1F_tbin = new TH1F("h1F_tbin", ";tbin;", tbin_f-tbin_i,tbin_i,tbin_f);
  // h1F_tbin_prev = (TH1F*) h1F_tbin->Clone("h1F_tbin_prev");
  // h1F_tbin_next = (TH1F*) h1F_tbin->Clone("h1F_tbin_next");
  _timer = new PHTimer("_eval_timer");
  _timer->stop();

  return Fun4AllReturnCodes::EVENT_OK;
}

int TPCGainDebug::InitRun(PHCompositeNode*)
{
  cout << "Fun4AllServer::BeginRun: InitRun for TPCGainDebug" << endl; 
  return Fun4AllReturnCodes::EVENT_OK;
}

int TPCGainDebug::process_event(PHCompositeNode* topNode)
{
  if ((Verbosity() > 1))
  {
    cout << "TPCGainDebug::process_event test" << endl;
  }

  if(_ievent<startevt || _ievent>endevt){
    ++_ievent;
    return Fun4AllReturnCodes::DISCARDEVENT;
  }
  _isSharkFin = false;
  sf_side = 0;
  sf_layer = 0;
  sf_phibin = 0;

  if ((Verbosity() > 1) && (_ievent % 100 == 0))
  {
    cout << "TPCGainDebug::process_event - Event = " << _ievent << endl;
  }

  recoConsts* rc = recoConsts::instance();
  if (rc->FlagExist("RANDOMSEED"))
  {
    _iseed = rc->get_IntFlag("RANDOMSEED");
    m_fSeed = _iseed;
  }
  else
  {
    _iseed = 0;
    m_fSeed = NAN;
  }
  if (Verbosity() > 1)
  {
    cout << "TPCGainDebug::process_event - Seed = " << _iseed << endl;
  }

  ResetEvent(topNode);
  //-----------------------------------
  // calculate gain! 
  //-----------------------------------
  CalculateGain(topNode);

  //---------------------------
  // fill the NTuples
  //---------------------------
  if(_save_sharkFin && _isSharkFin){
    if (Verbosity() > 1)
    {
      cout << "TPCGainDebug::save shark fin "  << endl;
    }
    
    fillOutputNtuplesAllhits(topNode);
    h1I_event_shark->Fill(_ievent+_eventOffset-1);
  }

  //--------------------------------------------------
  // Print out the ancestry information for this event
  //--------------------------------------------------

  // printOutputInfo(topNode);

  _ievent++;
  return Fun4AllReturnCodes::EVENT_OK;
}

int TPCGainDebug::End(PHCompositeNode* /*topNode*/)
{
  _tfile->cd();
  if (_do_gain){ gaintree->Write();}
  if (_save_ntuple || _save_sharkFin){ _ntp_hit->Write();}
  if (_save_sharkFin){
    h1I_event_shark->Write();
  }

  TEnv outConfig;
  outConfig.SetValue("do_pedSigmaCut", _do_pedSigmaCut);
  outConfig.SetValue("adcSigmaThreshold", adcSigmaThreshold);
  outConfig.SetValue("adcThreshold", adcThreshold);
  outConfig.SetValue("ntp_allhit_adcThreshold", AllHitADCThreshold);
  outConfig.SetValue("clusterPhibinArea", clusterPhibinArea);
  outConfig.SetValue("clusterTbinArea", clusterTbinArea);
  outConfig.SetValue("seedPhibinDistance", seedPhibinDistance);
  outConfig.SetValue("seedTbinDistance", seedTbinDistance);
  outConfig.SetValue("AdcClockPeriod", AdcClockPeriod);
  outConfig.SetValue("binWidthForPedEst", binWidthForPedEst);
  if(!_save_sharkFin) 
    outConfig.Write("config", TObject::kOverwrite);
  _tfile->Close();

  delete _tfile;

  if (Verbosity() > 1)
  {
    cout << "========================= TPCGainDebug::End() ============================" << endl;
    cout << " " << _ievent << " events of output written to: " << _filename << endl;
    cout << "===========================================================================" << endl;
  }

  return Fun4AllReturnCodes::EVENT_OK;
}

//////////////////////////////////////////////////////////
void TPCGainDebug::CalculateGain(PHCompositeNode* topNode)
{
  if (Verbosity() > 1)
  {
    cout << "TPCGainDebug::CalculateGain() entered" << endl;
  }

  // auto m_tGeometry = findNode::getClass<ActsGeometry>(topNode, "ActsGeometry");

  if (Verbosity() > 1)
  {
    cout << "Calculating TPC gain, time reset" << endl;
    _timer->restart();
  }
  // need things off of the DST...
  TrkrHitSetContainer* hitmap = findNode::getClass<TrkrHitSetContainer>(topNode, "TRKR_HITSET");
  if (!hitmap)
  {
    std::cout << PHWHERE << "ERROR: Can't find hitmap TRKR_HITSET" << std::endl;
    return;
  }

  PHG4TpcCylinderGeomContainer* geom_container =
    findNode::getClass<PHG4TpcCylinderGeomContainer>(topNode, "CYLINDERCELLGEOM_SVTX");
  if (!geom_container)
  {
    std::cout << PHWHERE << "ERROR: Can't find node CYLINDERCELLGEOM_SVTX" << std::endl;
    return;
  }
  auto m_tGeometry = findNode::getClass<ActsGeometry>(topNode, "ActsGeometry");

  TH1F pedhist("pedhist", "pedhist", 325, -0.5, 1299.5);
  // TH1F pedhist("pedhist", "pedhist", 251, -0.5, 1000.5);
  // pedhist->Reset();
  float hpedestal = 0;
  float hpedwidth = 0;
  m_event = _ievent+_eventOffset;
  for(int iside=0; iside< maxside; ++iside){
    for(int isector=0; isector< maxsector; ++isector){
      resizeHitsPerLayer();
      clearHitsPerLayer();
      std::vector<unsigned int> noisyChannel;
      for (int ilayer=0; ilayer < maxlayer; ++ilayer){
        const TrkrDefs::hitsetkey hitset_key = TpcDefs::genHitSetKey(ilayer, isector, iside);

        if (TrkrDefs::getTrkrId(hitset_key) != TrkrDefs::TrkrId::tpcId){
          if (Verbosity() > 1){
            cout << "The hitsetkey is not associated with TPC" << endl;
          }
          continue;
        }

        TrkrHitSet* hitset = hitmap->findHitSet(hitset_key);
        if(!hitset){
          if (Verbosity() > 3){
            cout << "No hits found in TPC (side " << iside << ", sector " << isector << ", layer " << ilayer << ")" << endl;
          }
          continue;
        }

        PHG4TpcCylinderGeom* GeoLayer_local = geom_container->GetLayerCellGeom(ilayer);
        double radius = GeoLayer_local->get_radius();
        unsigned short NTBins = (unsigned short) GeoLayer_local->get_zbins();
        if (Verbosity() > 2){
          if(NTBins!=360) cout << "NTBins = " << NTBins << endl;
          cout << "YJTEST (side " << iside << ", sector " << isector << ", layer " << ilayer << ")"  << ", phibins: " 
            << GeoLayer_local->get_phibins() << ", " 
            << GeoLayer_local->get_binning() << ", " 
            << endl;
        }

        // for (uint16_t sampleNum = 0; sampleNum < sampleMax; sampleNum++)
        // {
        //   auto hit_key_temp = TpcDefs::genHitKey(currentPhiBin, (unsigned int) sampleNum);
        //   auto hit_temp = hitset->getHit(hit_key_temp);

        //   uint16_t adc = hit_temp->getAdc();
        //   pedhist.Fill(adc);
        // }

        if (Verbosity() > 2){
          cout << "TrkrHitSet Start!! "<< endl;
        }

        int currentPhiBin = -1;
        float currentPhi = -999;
        bool isFirst = true;
        int testn = 0;
        TrkrHitSet::ConstRange hitrangei = hitset->getHits();
        for (TrkrHitSet::ConstIterator hitr = hitrangei.first;
            hitr != hitrangei.second;
            ++hitr)
        {
          TrkrDefs::hitkey hit_key = hitr->first;
          TrkrHit* hit = hitr->second;
          float _phibin = (float) TpcDefs::getPad(hit_key);
          float _phi = GeoLayer_local->get_phicenter(_phibin);

          // calculate pedestals 
          if(currentPhiBin != _phibin){

            //fill all hits tree when 
            if(a_tbin.size()!=0 && !isFirst && !_save_sharkFin){
              a_side = iside;
              a_sector = isector;
              a_layer = ilayer;
              a_phibin = currentPhiBin;
              a_phi = currentPhi;
              a_pedmean = hpedestal;
              a_pedwidth = hpedwidth;

              _ntp_hit->Fill();

              a_tbin.clear();
              a_adc.clear();
              a_x.clear();
              a_y.clear();
              a_z.clear();
              if (Verbosity() > 2)
              {
                cout << "TPCGainDebug:: STORE TEST all hits in (side, sector, layer = " << iside << ", " << isector << ", " << ilayer << ") phibin = " << currentPhiBin << ", testn = " << testn<< endl; 
              }
            }
            testn = 0;

            //this is the start of a new phibin!
            currentPhiBin = _phibin;
            currentPhi = _phi;
            hpedestal = 0;
            hpedwidth = 0;
            pedhist.Reset();
            
            //fill ADC distribution
            uint16_t finaladc = 0;
            for (uint16_t sampleNum = 0; sampleNum < sampleMax; sampleNum++)
            {
              auto hit_key_temp = TpcDefs::genHitKey(currentPhiBin, (unsigned int) sampleNum);
              auto hit_temp = hitset->getHit(hit_key_temp);

              uint16_t adc = hit_temp->getAdc();
              pedhist.Fill(adc);
              if(sampleNum==sampleMax-1) finaladc=adc;
            }
            int hmaxbin = pedhist.GetMaximumBin();

            // calc peak position
            double adc_sum = 0.0;
            double ibin_sum = 0.0;
            double ibin2_sum = 0.0;

            // calculate pedestal mean and sigma
            if(pedhist.GetMaximum()==360 || pedhist.GetEntries()==0){
              hpedestal = finaladc;
              hpedwidth = 0;
            } else{
              for (int isum = -1*binWidthForPedEst; isum <= binWidthForPedEst; isum++)
              {
                float val = pedhist.GetBinContent(hmaxbin + isum);
                float center = pedhist.GetBinCenter(hmaxbin + isum);
                ibin_sum += center * val;
                ibin2_sum += center * center * val;
                adc_sum += val;
              }
              hpedestal = ibin_sum / adc_sum;
              hpedwidth = sqrt(ibin2_sum / adc_sum - (hpedestal * hpedestal));
            }

            //noisy channel masking
            if(hpedwidth<0.5 || hpedestal<10){
              unsigned int temp_noisyKey = _phibin*100 + ilayer*1;
              noisyChannel.push_back(temp_noisyKey);
              if (Verbosity() > 2)
              {
                cout << "TPCGainDebug:: noisy channel of " << std::fixed << temp_noisyKey << " phibin, layer, sector, side = " << _phibin << ", " << ilayer << ", " << isector << ", " << iside << endl;
              }
            }

            //////////////////////////////////////////////
            // check if this event is shark fin 

            if(_save_sharkFin){
              int sharkFinBinPrevTbin = 0;
              // bool signChange = false;
              int sharkFinBinCount = 0;
              for (uint16_t sampleNum = 0; sampleNum < sampleMax; sampleNum++)
              {
                auto hit_key_temp = TpcDefs::genHitKey(currentPhiBin, (unsigned int) sampleNum);
                auto hit_temp = hitset->getHit(hit_key_temp);

                uint16_t adc = hit_temp->getAdc();
                if((adc < hpedestal-hpedwidth*5) && hpedwidth!=0 && hpedwidth > 1){
                  if(sharkFinBinPrevTbin+2 == sampleNum){
                    sharkFinBinCount++;
                  } 
                  sharkFinBinPrevTbin = sampleNum;

                  if (Verbosity() > 1)
                  {
                    cout << "TPCGainDebug1:: sharkFin  phibin, layer, sector, side = " << _phibin << ", " << ilayer << ", " << isector << ", " << iside << " :: pedestal, width, adc = " << hpedestal << ", " << hpedwidth << ", " << adc << " :: sharkFinCount, sharkFinBinPrevTbin, tbin = " << sharkFinBinCount << ", " << sharkFinBinPrevTbin << ", " << sampleNum << endl;
                  }
                } 

                if(sharkFinBinPrevTbin+1 == sampleNum){
                  if (Verbosity() > 1)
                  {
                    cout << "TPCGainDebug2:: sharkFin  phibin, layer, sector, side = " << _phibin << ", " << ilayer << ", " << isector << ", " << iside << " :: pedestal, width, adc = " << hpedestal << ", " << hpedwidth << ", " << adc << " :: sharkFinCount, sharkFinBinPrevTbin, tbin = " << sharkFinBinCount << ", " << sharkFinBinPrevTbin << ", " << sampleNum << endl;
                  }
                  // if( (adc > hpedestal-hpedwidth*5) && hpedwidth!=0 && hpedwidth > 1) {
                  //   signChange= true;
                  // } else{
                  //   signChange= false;
                  //   sharkFinBinCount = 0;
                  // }
                  if( !((adc > hpedestal+hpedwidth*5) && hpedwidth!=0 && hpedwidth > 1)) {
                    sharkFinBinCount = 0;
                  }
                }

                if(sharkFinBinCount >= 3){ 
                  if (Verbosity() > 1)
                  {
                    cout << "TPCGainDebug3:: sharkFin  phibin, layer, sector, side = " << _phibin << ", " << ilayer << ", " << isector << ", " << iside << " :: pedestal, width, adc = " << hpedestal << ", " << hpedwidth << ", " << adc << " :: sharkFinCount, sharkFinBinPrevTbin, tbin = " << sharkFinBinCount << ", " << sharkFinBinPrevTbin << ", " << sampleNum << endl;
                  }
                // if(signChange == true && sharkFinBinCount >= 3){ 
                  _isSharkFin=true; 
                  sf_side = iside;
                  sf_layer = ilayer;
                  sf_phibin = _phibin;

                  if (Verbosity() > 1)
                  {
                    cout << "TPCGainDebug:: sharkFin found " << endl;
                  }
                  return;
                }
              }
            }
            // if(isSharkFin) break;
          }//if(currentPhiBin != _phibin) // the calculation of pedestal should be done once per each phibin

          /// pedestal subtraction
          // float adc = hit->getAdc();
          float adc = hit->getAdc() - hpedestal;
          ihit _hit;
          _hit.adc = adc; 
          _hit.phibin = _phibin; 
          _hit.tbin = (float) TpcDefs::getTBin(hit_key);

          if (Verbosity() >= 3)
          {
            cout << "TPCGainDebug:: TEST all hits in (side, sector, layer = " << iside << ", " << isector << ", " << ilayer << ") = adc " << adc << ", phibin = " << _phibin << endl;
          }

          if((_save_ntuple && adc>AllHitADCThreshold) || (_save_sharkFin && (adc < -1*hpedwidth*adcSigmaThreshold))){
            float x = NAN;
            float y = NAN;
            float z = NAN;

            double zdriftlength = _hit.tbin * m_tGeometry->get_drift_velocity() * AdcClockPeriod;
            // convert z drift length to z position in the TPC
            //    cout << " tbin: " << tbin << " vdrift " <<m_tGeometry->get_drift_velocity() << " l drift: " << zdriftlength  <<endl;
            double m_tdriftmax = AdcClockPeriod * NTBins / 2.0;
            double clusz = (m_tdriftmax * m_tGeometry->get_drift_velocity()) - zdriftlength;
            if (iside == 0)
            {
              clusz = -clusz;
            }
            z = clusz;
            x = radius * cos(_phi);
            y = radius * sin(_phi);

            if (Verbosity() > 3)
            {
              cout << "TPCGainDebug:: TEST all hits in (side, sector, layer = " << iside << ", " << isector << ", " << ilayer << ") phibin = " << currentPhiBin 
                << ", tbin = " << _hit.tbin 
                << ", adc = " << _hit.adc 
                << endl; 
            }
            a_tbin.push_back(_hit.tbin);
            a_adc.push_back(hit->getAdc());
            a_x.push_back(x);
            a_y.push_back(y);
            a_z.push_back(z);
            testn++;
          }//if(currentPhiBin != _phibin)
          isFirst=false;


          if(!_do_gain) continue;

          // remove some noise channels for the gain seed finding! 
          if(hpedwidth<=0.5) continue; // to remove stuck channels (all same ADC values throughout tbin) 
          if(_do_pedSigmaCut){
            if(adc <= (hpedwidth*adcSigmaThreshold)) continue;
            // int temp_noisyKey = phibin*100000 + ilayer*1000 + isector*10 + iside;
            unsigned int temp_noisyKey = _phibin*100 + ilayer*1;
            if(std::find(noisyChannel.begin(), noisyChannel.end(), temp_noisyKey) != noisyChannel.end()){
              if (Verbosity() > 1)
              {
                cout << "TPCGainDebug:: skip this seed for this noisy channel of " << temp_noisyKey << endl;
              }
              continue;
            }
          } else{
            if(adc<=adcThreshold) continue;
          }
          if (Verbosity() > 2)
          {
            cout << "TPCGainDebug:: TEST all hits in (side, sector, layer = " << iside << ", " << isector << ", " << ilayer << ") phibin = " << currentPhiBin << ", tbin = " << _hit.tbin <<  ", adc = " << adc; 
            cout << ", hpedestal, hpedwidth = " << hpedestal << ", " << hpedwidth << endl;
          }

          
          // if the seed is in other position, add it 
          // (taking into account of multiple tracks in one sector in a given event) 
          // also find the local maximum
          bool isLocal = true;
          int temp_seed_size = hpl_seed[ilayer].size();
          if(temp_seed_size==0){
            hpl_seed[ilayer].push_back(_hit);
          } else{
            for(int iseed=0; iseed<temp_seed_size; ++iseed){
              if(  (abs(hpl_seed[ilayer][iseed].phibin - _hit.phibin) < seedPhibinDistance) 
                  && (abs(hpl_seed[ilayer][iseed].tbin - _hit.tbin) < seedTbinDistance) )
              { 
                isLocal = true;
                if(hpl_seed[ilayer][iseed].adc < _hit.adc){
                  hpl_seed[ilayer][iseed].adc = _hit.adc;
                  hpl_seed[ilayer][iseed].phibin = _hit.phibin;
                  hpl_seed[ilayer][iseed].tbin = _hit.tbin;
                }
              } else{
                isLocal = false;
              }
            } // seed loop
            if(!isLocal)
              hpl_seed[ilayer].push_back(_hit);
          }

        }// hits

        //fill all hits tree for the last phibin in (layer, sector, side)
        if(a_tbin.size()!=0 && !_save_sharkFin){
          a_side = iside;
          a_sector = isector;
          a_layer = ilayer;
          a_phibin = currentPhiBin;
          a_phi = currentPhi;
          a_pedmean = hpedestal;
          a_pedwidth = hpedwidth;

          _ntp_hit->Fill();

          a_tbin.clear();
          a_adc.clear();
          a_x.clear();
          a_y.clear();
          a_z.clear();
          if (Verbosity() > 3)
          {
            cout << "TPCGainDebug:: STORE TEST all hits in (side, sector, layer = " << iside << ", " << isector << ", " << ilayer << ") phibin = " << currentPhiBin << ", testn = " << testn<< endl; 
          }
        }
        testn = 0;

      }//layer

      if(!_do_gain) continue;

      // print out all gain seeds!
      if (Verbosity() > 1){
        cout << " >>> DONE Finding seeds " 
          << " for side " << iside << ", sector " << isector << endl;
        for (int ilayer=0; ilayer < maxlayer; ++ilayer){
          if(hpl_seed[ilayer].size() ==0) continue;
          for(unsigned int it=0; it< hpl_seed[ilayer].size(); it++){
            cout << " >>> Seed adc, phibin, tbin for layer " << ilayer << " = "  
              << hpl_seed[ilayer][it].adc << ", " 
              << hpl_seed[ilayer][it].phibin << ", " 
              << hpl_seed[ilayer][it].tbin << ", " 
              << endl;
          }
        }
      }
      
      //-----------------------------------
      // calculate gain 
      //-----------------------------------
      for (int ilayer=1; ilayer < maxlayer-1; ++ilayer){
        unsigned int seedsize_prev = hpl_seed[ilayer-1].size();
        unsigned int seedsize_next = hpl_seed[ilayer+1].size();
        if(seedsize_prev==0 || seedsize_next==0) continue;

        PHG4TpcCylinderGeom* GeoLayer_local = geom_container->GetLayerCellGeom(ilayer);
        float radius = GeoLayer_local->get_radius();

        std::vector<igain> gain;
        for (unsigned int j=0; j < seedsize_prev; ++j){//n-1
          for (unsigned int k=0; k < seedsize_next; ++k){//n+1
            //////////////// track should be perpendicular to the sectors, no tangential tracks
            if(abs(hpl_seed[ilayer+1][k].phibin - hpl_seed[ilayer-1][j].phibin)>seedPhibinDistance) continue; 
            if(abs(hpl_seed[ilayer+1][k].tbin - hpl_seed[ilayer-1][j].tbin)>seedTbinDistance) continue; 
            
            // h1F_tbin->Reset();
            // h1F_tbin_prev->Reset();
            // h1F_tbin_next->Reset();

            // if these seeds in layer n-1 and n+1 seem like from different tracks,  
            float phibin_n = (hpl_seed[ilayer+1][k].phibin + hpl_seed[ilayer-1][j].phibin)/2.; 
            float tbin_n = (hpl_seed[ilayer+1][k].tbin + hpl_seed[ilayer-1][j].tbin)/2.; 

            // look for if we already calculated  
            bool gainfound=false;
            int temp_gain_size = gain.size();
            if(temp_gain_size >= 1){
              for(int ig = 0; ig < temp_gain_size; ig++){
                if((abs(gain[ig].phibin-phibin_n) < seedPhibinDistance) && (abs(gain[ig].tbin-tbin_n) < seedTbinDistance)){
                  gainfound = true;
                  break;
                }
              }
            }
            if(gainfound) continue;


            //////////////////////////////
            // find maxADC per each pad around the seeds in n-th layer 
            const TrkrDefs::hitsetkey hitset_key = TpcDefs::genHitSetKey(ilayer, isector, iside);
            if (TrkrDefs::getTrkrId(hitset_key) != TrkrDefs::TrkrId::tpcId){
              continue;
            }
            TrkrHitSet* hitset = hitmap->findHitSet(hitset_key);
            if(!hitset){
              continue;
            }

            if (Verbosity() > 2){
              cout << "TrkrHitSet Start!! "<< endl;
            }

            std::map<float, ADCInfo> maxADCperPad; // pad, pair of (maxADC, tbin), highest ADC value over samples (time bins)
            // std::map<float, std::pair<float, float>> maxADCperPad; // pad, pair of (maxADC, tbin), highest ADC value over samples (time bins)
            TrkrHitSet::ConstRange hitrangei = hitset->getHits();

            int currentPhiBin = -1;
            for (TrkrHitSet::ConstIterator hitr = hitrangei.first;
                hitr != hitrangei.second;
                ++hitr)
            {
              TrkrDefs::hitkey hit_key = hitr->first;
              TrkrHit* hit = hitr->second;
              float phibin = (float) TpcDefs::getPad(hit_key);
              if((phibin < (phibin_n - clusterPhibinArea)) || (phibin > (phibin_n + clusterPhibinArea))) continue; 
              unsigned int temp_noisyKey = phibin*100 + ilayer*1;
              if(std::find(noisyChannel.begin(), noisyChannel.end(), temp_noisyKey) != noisyChannel.end()){
                continue;
                if (Verbosity() > 1)
                {
                  cout << "TPCGainDebug:: skip this cluster for this noisy channel of " << temp_noisyKey << endl;
                }
              }

              // calculate pedestals 
              if(currentPhiBin != phibin){

                //this is the start of a new phibin!
                currentPhiBin = phibin;
                hpedestal = 0;
                hpedwidth = 0;
                pedhist.Reset();

                //fill ADC distribution
                uint16_t finaladc = 0;
                for (uint16_t sampleNum = 0; sampleNum < sampleMax; sampleNum++)
                {
                  auto hit_key_temp = TpcDefs::genHitKey(currentPhiBin, (unsigned int) sampleNum);
                  auto hit_temp = hitset->getHit(hit_key_temp);

                  uint16_t adc = hit_temp->getAdc();
                  pedhist.Fill(adc);
                  if(sampleNum==sampleMax-1) finaladc=adc;
                }
                int hmaxbin = pedhist.GetMaximumBin();

                // calc peak position
                double adc_sum = 0.0;
                double ibin_sum = 0.0;
                double ibin2_sum = 0.0;

                // calculate pedestal mean and sigma
                if(pedhist.GetMaximum()==360 || pedhist.GetEntries()==0){
                  hpedestal = finaladc;
                  hpedwidth = 0;
                } else{
                  for (int isum = -1*binWidthForPedEst; isum <= binWidthForPedEst; isum++)
                  {
                    float val = pedhist.GetBinContent(hmaxbin + isum);
                    float center = pedhist.GetBinCenter(hmaxbin + isum);
                    ibin_sum += center * val;
                    ibin2_sum += center * center * val;
                    adc_sum += val;
                  }
                  hpedestal = ibin_sum / adc_sum;
                  hpedwidth = sqrt(ibin2_sum / adc_sum - (hpedestal * hpedestal));
                }
              }
              if(hpedwidth==0) continue;

              float tbin = (float) TpcDefs::getTBin(hit_key);
              if((tbin < (tbin_n - clusterTbinArea)) || (tbin > (tbin_n + clusterTbinArea))) continue; 

              ////////////////////////////////////////////
              // subtract pedestal from ADC in nth layer 
              float adc = hit->getAdc()-hpedestal;
              ////////////////////////////////////////////

              if (maxADCperPad.find(phibin) == maxADCperPad.end() || adc > maxADCperPad[phibin].maxADC) {
                ADCInfo tempADCInfo; 
                tempADCInfo.maxADC = adc;
                tempADCInfo.tbin = tbin;
                tempADCInfo.isSignal = (hpedwidth != 0 && adc>=hpedwidth*adcSigmaThreshold_phisize) ? 1 : 0;
                maxADCperPad[phibin] = tempADCInfo;
                // maxADCperPad[phibin] = std::make_pair(adc, tbin);
              }

              // adcsum += adc;
              
              // if(_save_debugHist){
              //   h1F_tbin->Fill(tbin,adc);  
              // }
              if (Verbosity() > 2)
              {
                cout << "TPCGainDebug:: all hits around the seeds in (side, sector, layer = " << iside << ", " << isector << ", " << ilayer << ") = adc " << adc << ", phibin = " << phibin << ", tbin = " << tbin << endl;
              }
            }// hits

            if(maxADCperPad.empty()){
              continue; // it can't be... but let's be safe 
            }

            //////////////////////////////
            // find position of max of maxADC among all pads around the seeds in n-th layer (find the highest phibin, tbin positionin a localized phi and time window) 
            // also add up ADC values over all pads (phi bins) around the seeds 
            float adcsum = 0;
            float maxADCofAll = 0;
            int padcenter = -1;
            int tcenter = -1;
            int phisize_ = 0;
            for (auto it = maxADCperPad.begin(); it != maxADCperPad.end(); ++it) {
              float adc_ = it->second.maxADC;
              // float adc_ = it->second.first;  
              adcsum += adc_;
              if (Verbosity() > 2)
              {
                cout << "TPCGainDebug:: max ADC hit around the seeds in (side, sector, layer = " << iside << ", " << isector << ", " << ilayer << ") for pad " << it->first << " =  adc " << adc_ << ", tbin = " << it->second.tbin << endl;
              }
              if(maxADCofAll < adc_){
                maxADCofAll = adc_;
                padcenter = it->first; 
                tcenter = it->second.tbin;
              }
              if(it->second.isSignal==1) phisize_++;
            }
            // if(adcsum==0) continue;

            //////////////////////////////
            //calculate pathlength
            float dphibin = abs(hpl_seed[ilayer+1][k].phibin - hpl_seed[ilayer-1][j].phibin); 
            float dz = abs(hpl_seed[ilayer+1][k].tbin - hpl_seed[ilayer-1][j].tbin) * m_tGeometry->get_drift_velocity() * AdcClockPeriod; 
            float drphi = radius*abs(GeoLayer_local->get_phicenter(padcenter)-GeoLayer_local->get_phicenter((padcenter-dphibin<0) ? padcenter+dphibin : padcenter-dphibin));
            float dR = 0;// (ilayer <=22 ? 0.6 : 1.2); 
            if(ilayer==7) dR=0.687;
            else if(ilayer<=22 && ilayer%2==0) dR=0.595;
            else if(ilayer<=22 && ilayer%2!=0) dR=0.534;
            else if(ilayer>=23 && ilayer<=38) dR=1.012;
            else if(ilayer>=39) dR=1.088;
            else dR=999999;
            float _pathlength = dz*dz + drphi*drphi + dR*dR; 

            if (Verbosity() > 1)
            {
              cout << "TPCGainDebug:: max of max ADC hit around the seeds in (side, sector, layer = " << iside << ", " << isector << ", " << ilayer << ") ::::: pad = " << padcenter << ", adc = " << maxADCofAll << ", tbin = " << tcenter << ", pathlength = " << _pathlength << endl;
            }

            // if(_save_debugHist && adcsum!=0){
            //   _tfile->cd();
            //   h1F_tbin->Write(Form("h1F_tbin_side%d_sector%d_layer%d_event%d", iside, isector, ilayer,_ievent));  
            //   h1F_tbin_prev->Write(Form("h1F_tbin_side%d_sector%d_layer%d_event%d_prev", iside, isector, ilayer,_ievent));  
            //   h1F_tbin_next->Write(Form("h1F_tbin_side%d_sector%d_layer%d_event%d_next", iside, isector, ilayer,_ievent));  
            //   // _tfile->Close();
            // }
            

            igain temp_gain;
            temp_gain.adc=adcsum;
            temp_gain.phibin=padcenter;
            temp_gain.tbin=tcenter;
            temp_gain.pathlength=_pathlength;
            temp_gain.phisize=phisize_;
            // temp_gain.errorcode=_errorcode;

            gain.push_back(temp_gain);

            // delete hitset;

          }//n+1 layer
        }//n-1 layer

        int gain_size= gain.size();
        if(gain_size==0) continue;

        for(int ig = 0; ig < gain_size; ig++){
          m_side.push_back(iside);
          m_sector.push_back(isector);
          m_layer.push_back(ilayer);
          m_phibin.push_back(gain[ig].phibin);
          m_tbin.push_back(gain[ig].tbin);
          m_adcsum.push_back(gain[ig].adc);
          m_pathlength.push_back(gain[ig].pathlength);
          m_phisize.push_back(gain[ig].phisize);
          // m_errorcode.push_back(_errorcode);

          if (Verbosity() > 2)
          {
            cout << "TPCGainDebug::FINAL ADC sum in layer " << ilayer << " (side, sector, phibin, tbin = " <<  iside << ", " << isector << ", " << gain[ig].phibin << ", " << gain[ig].tbin << ") = " << gain[ig].adc << endl;
          }
        }//gain size

      }//nth layer
    }//sector
  }//side 

  if(_do_gain && m_side.size()!=0)
    gaintree->Fill();

  // if(_save_ntuple && a_side.size()!=0)
  //   _ntp_hit->Fill();
  // delete hitmap;
  // delete geom_container;

  return;
}

//////////////////////////////////////////////////////////
void TPCGainDebug::fillOutputNtuplesAllhits(PHCompositeNode* topNode)
{
  if (Verbosity() > 1)
  {
    cout << "TPCGainDebug::fillOutputNtuplesAllhits() entered" << endl;
  }

  // need things off of the DST...
  TrkrHitSetContainer* hitmap = findNode::getClass<TrkrHitSetContainer>(topNode, "TRKR_HITSET");
  if (!hitmap)
  {
    std::cout << PHWHERE << "ERROR: Can't find hitmap TRKR_HITSET" << std::endl;
    return;
  }

  PHG4TpcCylinderGeomContainer* geom_container =
    findNode::getClass<PHG4TpcCylinderGeomContainer>(topNode, "CYLINDERCELLGEOM_SVTX");
  if (!geom_container)
  {
    std::cout << PHWHERE << "ERROR: Can't find node CYLINDERCELLGEOM_SVTX" << std::endl;
    return;
  }
  auto m_tGeometry = findNode::getClass<ActsGeometry>(topNode, "ActsGeometry");

  TH1F pedhist("pedhist", "pedhist", 325, -0.5, 1299.5);
  // TH1F pedhist("pedhist", "pedhist", 251, -0.5, 1000.5);
  // pedhist->Reset();
  float hpedestal = 0;
  float hpedwidth = 0;
  m_event = _ievent+_eventOffset;
  for(int iside=0; iside< maxside; ++iside){
    for(int isector=0; isector< maxsector; ++isector){
      for (int ilayer=0; ilayer < maxlayer; ++ilayer){
        const TrkrDefs::hitsetkey hitset_key = TpcDefs::genHitSetKey(ilayer, isector, iside);

        if (TrkrDefs::getTrkrId(hitset_key) != TrkrDefs::TrkrId::tpcId){
          if (Verbosity() > 1){
            cout << "The hitsetkey is not associated with TPC" << endl;
          }
          continue;
        }

        TrkrHitSet* hitset = hitmap->findHitSet(hitset_key);
        if(!hitset){
          if (Verbosity() > 3){
            cout << "No hits found in TPC (side " << iside << ", sector " << isector << ", layer " << ilayer << ")" << endl;
          }
          continue;
        }

        PHG4TpcCylinderGeom* GeoLayer_local = geom_container->GetLayerCellGeom(ilayer);
        double radius = GeoLayer_local->get_radius();
        unsigned short NTBins = (unsigned short) GeoLayer_local->get_zbins();
        if (Verbosity() > 2){
          if(NTBins!=360) cout << "NTBins = " << NTBins << endl;
          cout << "YJTEST (side " << iside << ", sector " << isector << ", layer " << ilayer << ")"  << ", phibins: " 
            << GeoLayer_local->get_phibins() << ", " 
            << GeoLayer_local->get_binning() << ", " 
            << endl;
        }

        if (Verbosity() > 2){
          cout << "TrkrHitSet Start!! "<< endl;
        }

        int currentPhiBin = -1;
        float currentPhi = -999;
        bool isFirst = true;
        int testn = 0;
        TrkrHitSet::ConstRange hitrangei = hitset->getHits();
        for (TrkrHitSet::ConstIterator hitr = hitrangei.first;
            hitr != hitrangei.second;
            ++hitr)
        {
          TrkrDefs::hitkey hit_key = hitr->first;
          TrkrHit* hit = hitr->second;
          float _phibin = (float) TpcDefs::getPad(hit_key);
          float _phi = GeoLayer_local->get_phicenter(_phibin);

          // calculate pedestals 
          if(currentPhiBin != _phibin){

            //fill all hits tree when 
            if(a_tbin.size()!=0 && !isFirst){
              a_side = iside;
              a_sector = isector;
              a_layer = ilayer;
              a_phibin = currentPhiBin;
              a_phi = currentPhi;
              a_pedmean = hpedestal;
              a_pedwidth = hpedwidth;
              if(_save_sharkFin){
                if(sf_side == iside && sf_layer == ilayer && sf_phibin == currentPhiBin) a_isSharkFin = 1;
                else a_isSharkFin = 0;
              }

              _ntp_hit->Fill();

              a_tbin.clear();
              a_adc.clear();
              a_x.clear();
              a_y.clear();
              a_z.clear();
              if (Verbosity() > 2)
              {
                cout << "TPCGainDebug:: STORE TEST all hits in (side, sector, layer = " << iside << ", " << isector << ", " << ilayer << ") phibin = " << currentPhiBin << ", testn = " << testn<< endl; 
              }
            }
            testn = 0;

            //this is the start of a new phibin!
            currentPhiBin = _phibin;
            currentPhi = _phi;
            hpedestal = 0;
            hpedwidth = 0;
            pedhist.Reset();
            
            //fill ADC distribution
            uint16_t finaladc = 0;
            for (uint16_t sampleNum = 0; sampleNum < sampleMax; sampleNum++)
            {
              auto hit_key_temp = TpcDefs::genHitKey(currentPhiBin, (unsigned int) sampleNum);
              auto hit_temp = hitset->getHit(hit_key_temp);

              uint16_t adc = hit_temp->getAdc();
              pedhist.Fill(adc);
              if(sampleNum==sampleMax-1) finaladc=adc;
            }
            int hmaxbin = pedhist.GetMaximumBin();

            // calc peak position
            double adc_sum = 0.0;
            double ibin_sum = 0.0;
            double ibin2_sum = 0.0;

            // calculate pedestal mean and sigma
            if(pedhist.GetMaximum()==360 || pedhist.GetEntries()==0){
              hpedestal = finaladc;
              hpedwidth = 0;
            } else{
              for (int isum = -1*binWidthForPedEst; isum <= binWidthForPedEst; isum++)
              {
                float val = pedhist.GetBinContent(hmaxbin + isum);
                float center = pedhist.GetBinCenter(hmaxbin + isum);
                ibin_sum += center * val;
                ibin2_sum += center * center * val;
                adc_sum += val;
              }
              hpedestal = ibin_sum / adc_sum;
              hpedwidth = sqrt(ibin2_sum / adc_sum - (hpedestal * hpedestal));
            }
          }//if(currentPhiBin != _phibin) // the calculation of pedestal should be done once per each phibin

          /// pedestal subtraction
          float adc = hit->getAdc() - hpedestal;
          ihit _hit;
          _hit.adc = adc; 
          _hit.phibin = _phibin; 
          _hit.tbin = (float) TpcDefs::getTBin(hit_key);

          if (Verbosity() >= 3)
          {
            cout << "TPCGainDebug:: TEST all hits in (side, sector, layer = " << iside << ", " << isector << ", " << ilayer << ") = adc " << adc << ", phibin = " << _phibin << endl;
          }

          float x = NAN;
          float y = NAN;
          float z = NAN;

          double zdriftlength = _hit.tbin * m_tGeometry->get_drift_velocity() * AdcClockPeriod;
          double m_tdriftmax = AdcClockPeriod * NTBins / 2.0;
          double clusz = (m_tdriftmax * m_tGeometry->get_drift_velocity()) - zdriftlength;
          if (iside == 0)
          {
            clusz = -clusz;
          }
          z = clusz;
          x = radius * cos(_phi);
          y = radius * sin(_phi);

          if (Verbosity() > 3)
          {
            cout << "TPCGainDebug:: TEST all hits in (side, sector, layer = " << iside << ", " << isector << ", " << ilayer << ") phibin = " << currentPhiBin 
              << ", tbin = " << _hit.tbin 
              << ", adc = " << _hit.adc 
              << endl; 
          }
          a_tbin.push_back(_hit.tbin);
          a_adc.push_back(hit->getAdc());
          a_x.push_back(x);
          a_y.push_back(y);
          a_z.push_back(z);
          testn++;

          isFirst=false;

          if (Verbosity() > 2)
          {
            cout << "TPCGainDebug:: TEST all hits in (side, sector, layer = " << iside << ", " << isector << ", " << ilayer << ") phibin = " << currentPhiBin << ", tbin = " << _hit.tbin <<  ", adc = " << adc; 
            cout << ", hpedestal, hpedwidth = " << hpedestal << ", " << hpedwidth << endl;
          }
        }//hits

        //fill all hits tree for the last phibin in (layer, sector, side)
        if(a_tbin.size()!=0){
          a_side = iside;
          a_sector = isector;
          a_layer = ilayer;
          a_phibin = currentPhiBin;
          a_phi = currentPhi;
          a_pedmean = hpedestal;
          a_pedwidth = hpedwidth;
          if(_save_sharkFin){
            if(sf_side == iside && sf_layer == ilayer && sf_phibin == currentPhiBin) a_isSharkFin = 1;
            else a_isSharkFin = 0;
          }

          _ntp_hit->Fill();

          a_tbin.clear();
          a_adc.clear();
          a_x.clear();
          a_y.clear();
          a_z.clear();
        }
      }//layer
    }//sector
  }//side
}

// void TPCGainDebug::fillOutputNtuplesAllhits(PHCompositeNode* topNode)
// {
//   if (Verbosity() > 1)
//   {
//     cout << "TPCGainDebug::fillOutputNtuplesAllhits() entered" << endl;
//   }

//   if (_ntp_hit)
//   {
//     auto m_tGeometry = findNode::getClass<ActsGeometry>(topNode, "ActsGeometry");

//     if (Verbosity() > 1)
//     {
//       cout << "Filling ntp_hit " << endl;
//       _timer->restart();
//     }
//     // need things off of the DST...
//     TrkrHitSetContainer* hitmap = findNode::getClass<TrkrHitSetContainer>(topNode, "TRKR_HITSET");

//     PHG4TpcCylinderGeomContainer* geom_container =
//       findNode::getClass<PHG4TpcCylinderGeomContainer>(topNode, "CYLINDERCELLGEOM_SVTX");
//     if (!geom_container)
//     {
//       std::cout << PHWHERE << "ERROR: Can't find node CYLINDERCELLGEOM_SVTX" << std::endl;
//       return;
//     }

//     // for(float i=7; i<55; i++){
//     //   PHG4TpcCylinderGeom* geom = geom_container->GetLayerCellGeom(i);
//     //   std::cout << "PHG4TpcCylinderGeom - layer: " << geom->get_layer() << std::endl;
//     //   std::cout 
//     //     << "  binnig: " << geom->get_binning()
//     //     << ", radius: " << geom->get_radius()
//     //     << ", nzbins: " << geom->get_zbins()
//     //     << ", zmin: " << geom->get_zmin()
//     //     << ", zstep: " << geom->get_zstep()
//     //     << ", nphibins: " << geom->get_phibins()
//     //     << ", phimin: " << geom->get_phimin()
//     //     << ", phistep: " << geom->get_phistep()
//     //     << ", thickness: " << geom->get_thickness()
//     //     << std::endl;

//     //   // std::cout << "  sector_R_bias: " << geom.sector_R_bias << std::endl;
//     //   // std::cout << "  sector_Phi_bias: " << geom.sector_Phi_bias << std::endl;
//     //   // std::cout << "  sector_min_Phi: " << geom.sector_min_Phi << std::endl;
//     //   // std::cout << "  sector_max_Phi: " << geom.sector_max_Phi << std::endl;
//     //   // cout << GeoLayer_local;
//     // }

//     if (!hitmap)
//     {
//       std::cout << PHWHERE << "ERROR: Can't find hitmap TRKR_HITSET" << std::endl;
//       return;
//     }

//     TrkrHitSetContainer::ConstRange all_hitsets = hitmap->getHitSets();
//     for (TrkrHitSetContainer::ConstIterator iter = all_hitsets.first;
//         iter != all_hitsets.second;
//         ++iter)
//     {
//       const TrkrDefs::hitsetkey hitset_key = iter->first;
//       TrkrHitSet* hitset = iter->second;
//       // get all hits for this hitset
//       TrkrHitSet::ConstRange hitrangei = hitset->getHits();
//       for (TrkrHitSet::ConstIterator hitr = hitrangei.first;
//           hitr != hitrangei.second;
//           ++hitr)
//       {
//         TrkrDefs::hitkey hit_key = hitr->first;
//         TrkrHit* hit = hitr->second;
//         float run = _runnumber;
//         float event = _ievent;
//         float hitID = hit_key;
//         float e = hit->getEnergy();
//         float adc = hit->getAdc();
//         float layer_local = TrkrDefs::getLayer(hitset_key);
//         float sector = TpcDefs::getSectorId(hitset_key);
//         float side = TpcDefs::getSide(hitset_key);
        
//         if(adc<AllHitADCThreshold) continue;
//         float phibin = NAN;
//         float tbin = NAN;
//         float phi = NAN;
//         float phi_center = NAN;
//         float x = NAN;
//         float y = NAN;
//         float z = NAN;

//         if (TrkrDefs::getTrkrId(hitset_key) == TrkrDefs::TrkrId::tpcId)
//         {
//           PHG4TpcCylinderGeom* GeoLayer_local = geom_container->GetLayerCellGeom(layer_local);
//           double radius = GeoLayer_local->get_radius();
//           phibin = (float) TpcDefs::getPad(hit_key);
//           tbin = (float) TpcDefs::getTBin(hit_key);
//           phi = GeoLayer_local->get_phicenter(phibin);

//           double zdriftlength = tbin * m_tGeometry->get_drift_velocity() * AdcClockPeriod;
//           // convert z drift length to z position in the TPC
//           //    cout << " tbin: " << tbin << " vdrift " <<m_tGeometry->get_drift_velocity() << " l drift: " << zdriftlength  <<endl;
//           unsigned short NTBins = (unsigned short) GeoLayer_local->get_zbins();
//           double m_tdriftmax = AdcClockPeriod * NTBins / 2.0;
//           double clusz = (m_tdriftmax * m_tGeometry->get_drift_velocity()) - zdriftlength;
//           if (side == 0)
//           {
//             clusz = -clusz;
//           }
//           z = clusz;
//           phi_center = GeoLayer_local->get_phicenter(phibin);
//           x = radius * cos(phi_center);
//           y = radius * sin(phi_center);
//         }

//         float hit_data[] = {
//           run, 
//           event,
//           (float) _iseed,
//           hitID,
//           e,
//           adc,
//           layer_local,
//           sector,
//           side,
//           (float) phibin,
//           (float) tbin,
//           phi,
//           x,
//           y,
//           z,
//         };

//         _ntp_hit->Fill(hit_data);
//       }
//     }
//     if (Verbosity() > 1)
//     {
//       _timer->stop();
//       cout << "hit time:                " << _timer->get_accumulated_time() / 1000. << " sec" << endl;
//     }
//   }
//   return;
// }

int TPCGainDebug::ResetEvent(PHCompositeNode *)
{
  m_side.clear();
  m_sector.clear();
  m_layer.clear();
  m_phibin.clear();
  m_tbin.clear();
  m_adcsum.clear();
  m_pathlength.clear();
  m_phisize.clear();

  a_tbin.clear();
  a_adc.clear();
  a_x.clear();
  a_y.clear();
  a_z.clear();

  return Fun4AllReturnCodes::EVENT_OK;
}

//bool SampleFit_PowerLawDoubleExp(        //
//    const std::vector<double> &samples,  //
//    double &peak,                        //
//    double &peak_sample,                 //
//    double &pedestal,                    //
//    std::map<int, double> &parameters_io,
//    const int verbosity)
//{
//  static const int n_parameter = 7;

//  // inital guesses
//  int peakPos = 0.;

//  //  assert(samples.size() == n_samples);
//  const int n_samples = samples.size();

//  TGraph gpulse(n_samples);
//  for (int i = 0; i < n_samples; i++)
//  {
//    (gpulse.GetX())[i] = i;

//    (gpulse.GetY())[i] = samples[i];
//  }

//  //Saturation correction - Abhisek
//  //  for (int ipoint = 0; ipoint < gpulse.GetN(); ipoint++)
//  //    if ((gpulse.GetY())[ipoint] >= ((1 << 10) - 10)  // drop point if touching max or low limit on ADCs
//  //        or (not isnormal((gpulse.GetY())[ipoint])))
//  //    {
//  //      gpulse.RemovePoint(ipoint);
//  //      ipoint--;
//  //    }

//  pedestal = gpulse.GetY()[0];  //(double) PEDESTAL;
//  double peakval = pedestal;
//  const double risetime = 1.5;

//  for (int iSample = 0; iSample < n_samples - risetime * 3; iSample++)
//  {
//    if (abs(gpulse.GetY()[iSample] - pedestal) > abs(peakval - pedestal))
//    {
//      peakval = gpulse.GetY()[iSample];
//      peakPos = iSample;
//    }
//  }
//  peakval -= pedestal;

//  if (verbosity)
//  {
//    cout << "SampleFit_PowerLawDoubleExp - "
//         << "pedestal = " << pedestal << ", "
//         << "peakval = " << peakval << ", "
//         << "peakPos = " << peakPos << endl;
//  }




//double SignalShape_PowerLawExp(double *x, double *par)
//{
//  double pedestal = par[4];
//  //                        + ((x[0] - 1.5 * par[1]) > 0) * par[5];  // quick fix on exting tails on the signal function
//  if (x[0] < par[1])
//    return pedestal;
//  //double  signal = (-1)*par[0]*pow((x[0]-par[1]),par[2])*exp(-(x[0]-par[1])*par[3]);
//  double signal = par[0] * pow((x[0] - par[1]), par[2]) * exp(-(x[0] - par[1]) * par[3]);
//  return pedestal + signal;
//}

//double SignalShape_PowerLawDoubleExp(double *x, double *par)
//{
//  double pedestal = par[4];
//  //                        + ((x[0] - 1.5 * par[1]) > 0) * par[5];  // quick fix on exting tails on the signal function
//  if (x[0] < par[1])
//    return pedestal;
//  //double  signal = (-1)*par[0]*pow((x[0]-par[1]),par[2])*exp(-(x[0]-par[1])*par[3]);
//  //  peak / pow(fits.GetParameter(2) / fits.GetParameter(3), fits.GetParameter(2)) * exp(fits.GetParameter(2)) = fits.GetParameter(0);  // exact peak height is (p0*Power(p2/p3,p2))/Power(E,p2)
//  //  fits.GetParameter(2) / peak_shift =  fits.GetParameter(3);  // signal peak time

//  double signal =                                                                                         //
//    par[0]                                                                                              //
//    * pow((x[0] - par[1]), par[2])                                                                      //
//    * (((1. - par[5]) / pow(par[3], par[2]) * exp(par[2])) * exp(-(x[0] - par[1]) * (par[2] / par[3]))  //
//        + (par[5] / pow(par[6], par[2]) * exp(par[2])) * exp(-(x[0] - par[1]) * (par[2] / par[6]))       //
//      );
//  return pedestal + signal;
//}
//TF1* cleverGaus(TH1* h, const char* title, Float_t c, bool quietMode)
//{
//  if ( h->GetEntries() == 0 )
//  {
//    TF1 *fit0 = new TF1(title,"gaus",-1,1);
//    fit0->SetParameters(0,0,0);
//    return fit0;
//  }

//  Int_t peakBin  = h->GetMaximumBin();
//  Double_t peak =  h->GetBinCenter(peakBin);
//  Double_t sigma = h->GetRMS();

//  TF1 *fit1 = new TF1(title,"gaus",peak-c*sigma,peak+c*sigma);
//  fit1->SetParameter(1, peak);
//  //fit1->SetParameter(1, 0.0005);
//  //if (quietMode) h->Fit(fit1,"Q R");
//  if (quietMode) h->Fit(fit1,"LL M O Q R");
//  else    h->Fit(fit1,"LL M O R");
//  return fit1;
//}
