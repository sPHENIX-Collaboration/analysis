#include "InttHitCorrelation.h"

using namespace std;

//____________________________________________________________________________..
InttHitCorrelation::InttHitCorrelation(const std::string &name, bool is_official ):
  SubsysReco(name), is_official_(is_official)
{

  
}

//____________________________________________________________________________..
InttHitCorrelation::~InttHitCorrelation()
{

}

/////////////////////////////////////////////////////////////////////////
// private
/////////////////////////////////////////////////////////////////////////
int InttHitCorrelation::GetNodes(PHCompositeNode *topNode)
{
  
  /////////////////////////////////////////////////////////////////////////
  // INTT raw hit
  string node_name_inttrawhit = "INTTRAWHIT";
  node_inttrawhit_map_ =
    findNode::getClass<InttRawHitContainer>(topNode, node_name_inttrawhit);
  
  if (!node_inttrawhit_map_)
    {
      cerr << PHWHERE << node_name_inttrawhit << " node is missing." << endl;
      return Fun4AllReturnCodes::ABORTEVENT;
    }
  
  /////////////////////////////////////////////////////////////////////////
  // Trkr hit
  // TRKR_HITSET (IO,TrkrHitSetContainerv1)
  
  string node_name_trkr_hitset = "TRKR_HITSET";
  // node_inttrawhit_map_ =
  node_trkrhitset_map_ = 
    findNode::getClass<TrkrHitSetContainer>(topNode, node_name_trkr_hitset);
  
  if (!node_trkrhitset_map_)
    {
      cerr << PHWHERE << node_name_trkr_hitset << " node is missing." << endl;
      return Fun4AllReturnCodes::ABORTEVENT;
    }

  return 0;
}

vector < std::pair < uint16_t, int > > InttHitCorrelation::GetBcoEventCounter()
{
  //cout << "vector < std::pair < uint16_t, int > > InttHitCorrelation::GetBcoEventCounter()" << endl;
  vector < pair < InttNameSpace::Online_s,  unsigned int > > online_hits = this->GetHits(  node_trkrhitset_map_->getHitSets() );

  vector < InttRawHit* > raw_hits = this->GetRawHitsWithoutClone();
  vector < std::pair < uint16_t, int > > rtn;
  
  // if( raw_hits.size() > 20 )
  //   return rtn;
  InttDacMap* dac_map = new InttDacMap();
  
  int counter = 0;
  for( int i=0; i<online_hits.size(); i++ )
    {
      ////////////////////////////////////////////
      // struct Online_s                        //
      //   int lyr, int ldr,  int arm           //
      //   int chp, int chn                     //
      ////////////////////////////////////////////
      auto online_hit = online_hits[i].first;
      auto dac = online_hits[i].second;

      ////////////////////////////////////////////
      // struct RawData_s:                      //
      //   int felix_server, int felix_channel  //
      //   int chip,         int channel,       //
      ////////////////////////////////////////////
      // InttNameSpace::RawData_s
      auto raw_data  = InttNameSpace::ToRawData( online_hit );
      
      ////////////////////////////////////////////
      // struct Offline_s                       //
      //   int layer,     int ladder_phi,       //
      //   int ladder_z,  int strip_x,          //
      //   int strip_y,                         //
      ////////////////////////////////////////////
      // InttNameSpace::Offline_s
      auto offline_hit = InttNameSpace::ToOffline( online_hit );

      // consistency confirmed, no need anymore
      // if( online_hit.chp != raw_data.chip
      // 	  || online_hit.chn != raw_data.channel )
      // 	{
      
      // 	  cout << setw(4) << online_hit.chp
      // 	       << setw(5) << online_hit.chn
      // 	       << endl;
	  
      // 	  cout << setw(4) << raw_data.chip
      // 	       << setw(5) << raw_data.channel
      // 	       << " | "
      // 	       << setw(3) << raw_data.felix_server
      // 	       << setw(5) << raw_data.felix_channel
      // 	       << endl;
	  
      // 	  cout << "---------------------------------------------------" << endl;
      // 	}

      // cout << "TrkrHit: "
      // 	   << setw(4) << i
      // 	   << ": " 
      // 	   << setw(3) << raw_data.felix_server
      // 	   << setw(5) << raw_data.felix_channel
      // 	   << setw(4) << raw_data.chip
      // 	   << setw(6) << raw_data.channel
      // 	   << setw(4) << dac
      // 	   << " vs ";
	//	   << endl;
      vector < int > used_index( raw_hits.size() );
      
      for( int j=0; j<raw_hits.size(); j++ )
	{
	  
	  auto raw_hit = raw_hits[j];

	  int chan_raw		= raw_hit->get_channel_id();
	  // first selections
	  if( chan_raw != raw_data.channel )
	    continue;
	  
	  // uint16_t InttRawHit::get_chip_id
	  int chip_raw		= raw_hit->get_chip_id();    
	  if(chip_raw  > InttQa::kChip_num)
	    chip_raw = chip_raw - InttQa::kChip_num;

	  chip_raw--; // to change from 1-base to 0-base
	  if( chip_raw != raw_data.chip )
	    continue;

	  int felix_raw		= raw_hit->get_packetid() - InttQa::kFirst_pid;
	  int felix_ch_raw	= raw_hit->get_fee();
	  bool is_same = (felix_raw == raw_data.felix_server
			  || felix_ch_raw == raw_data.felix_channel );
			  // || adc_raw == adc );

	  if( is_same == false )
	    continue;

	  auto adc_raw		= raw_hit->get_adc();

	  
	  auto bco_raw		= raw_hit->get_FPHX_BCO();
	  int event_counter_raw	= raw_hit->get_event_counter(); // uint32_t IttRawHit::get_event_counter()
	  pair < uint16_t, int > bco_event_counter( bco_raw, event_counter_raw );
	  rtn.push_back( bco_event_counter );
	  counter++;
	  //if( felix_raw == raw_data.felix_server )
	  //	  if( felix_ch_raw == raw_data.felix_channel && felix_ch_raw != 0 )
	    // cout << "RawHit: "
	    // 	 << setw(4) << j
	    // 	 << ": " 
	    // 	 << setw(3) << felix_raw
	    // 	 << setw(5) << felix_ch_raw
	    // 	 << setw(4) << chip_raw
	    // 	 << setw(6) << chan_raw
	    // 	 << setw(4) << adc_raw
	    // 	 << endl;
	  		
	  // cout << "( "
	  //      << setw(3) << bco_raw << ", " << setw(1) << adc_raw << "-> " << dac
	  //      << "), ";
	  //used_index.push_back( j );
	  //raw_hits.erase( raw_hits.begin() + j );
	  //break;
	}
      //      cout << endl;
      
    }


  // cout << counter << " / " << online_hits.size() << " found their corresponding raw hit while there are "
  //      << raw_hits.size() << " raw hits in this event." 
  //      << endl;
  return rtn;
}

bool InttHitCorrelation::IsSame( InttRawHit* hit1, InttRawHit* hit2 )
{

  if( hit1->get_word() != hit2->get_word() ) // only this might be enough
    return false;
  else if( hit1->get_bco() != hit2->get_bco() )
    return false;
  else if( hit1->get_FPHX_BCO() != hit2->get_FPHX_BCO() )
    return false;
  else if( hit1->get_channel_id() != hit2->get_channel_id() )
    return false;
  else if( hit1->get_chip_id() != hit2->get_chip_id() )
    return false;
  else if( hit1->get_fee() != hit2->get_fee() )
    return false;
  else if( hit1->get_packetid() != hit2->get_packetid() )
    return false;
  else if( hit1->get_adc() != hit2->get_adc() )
    return false;
  else if( hit1->get_full_FPHX() != hit2->get_full_FPHX() )
    return false;
  // else if( hit1->get_full_ROC() != hit2->get_full_ROC() ) // maybe no value is assigned?
  //   return false;
  // else if( hit1->get_amplitude()!= hit2->get_amplitude()) // maybe no value is assigned?
  //   return false;

  // hits pass all comparison are the same
  return true;
};

vector < InttRawHit* > InttHitCorrelation::GetRawHits()
{
  vector < InttRawHit* > hits;
  auto raw_hit_num = node_inttrawhit_map_->get_nhits();

  for (unsigned int i = 0; i < raw_hit_num; i++)
    {
      auto hit = node_inttrawhit_map_->get_hit(i);

      hits.push_back( hit );
    }

  return hits;
}

vector < InttRawHit* > InttHitCorrelation::GetRawHitsWithoutClone()
{

  auto hits = this->GetRawHits();
  vector < InttRawHit* > rtn;

  for( int i=0; i<hits.size(); i++ )
    {

      bool is_same_found = false;
      for( int j=i+1; j<hits.size(); j++ )
        {

          is_same_found = is_same_found || this->IsSame( hits[i], hits[j] );
          if( is_same_found == true )
            {
              break;
            }

        } // end of for( hit_j )

      // if this hit is still unique (same hit is not found), keep it
      if( is_same_found == false )
	{
	  rtn.push_back( hits[i] );
	}

    } // end of for( hit_i )

  return rtn;
}

vector < pair < InttNameSpace::Online_s,  unsigned int > >
InttHitCorrelation::GetHits( TrkrHitSetContainer::ConstRange hitsets )
{
  /*!
    @param ConstRange hitsets This is ConstRange = std::pair< ConstIterator, ConstIterator >
  */

  vector < pair < InttNameSpace::Online_s, unsigned int > > hits_information;
  auto current = hitsets.first;
  while( current != hitsets.second )
    {
      auto hitset = (*current).second;

      auto layer       = TrkrDefs::getLayer( (*current).first );
      // layer: 0, 1, 2   : MVTX
      //        3, 4, 5, 6: INTT
      //        6<        : TPC

      // Take hits from only INTT
      if( !(3<= layer && layer <= 6) )
	continue;
      
      auto detector_id = TrkrDefs::getTrkrId( (*current).first );
      auto phi         = InttDefs::getLadderPhiId( (*current).first ); //TrkrDefs::getPhiElement( (*current).first );
      auto z           = InttDefs::getLadderZId( (*current).first ); // TrkrDefs::getZElement( (*current).first );
      // cout << "-----> "
      // 	   << int(detector_id) << "\t" << int(layer) << "\t"
      // 	   << int(phi) << "\t" << int(z) ;
      // 	//<< endl;
      // cout << " + " << (*current).first << "\t"
      // 	   << hitset->size()
      // 	   << endl;

      auto hits = hitset->getHits(); // ConstRange = std::pair< ConstIterator, ConstIterator >
      auto current2 = hits.first; // ConstIterator = Map::const_iterator, here, Map = std::map < TrkrDefs::hitkey, TrkrHit* >, This Map is different from the Map for TrkrHitSetContainer!!!
      while( current2 != hits.second )
	{
	  auto key = (*current2).first;
	  auto hit = (*current2).second;

	  auto col = InttDefs::getCol( key );
	  auto row = InttDefs::getRow( key );

	  InttNameSpace::Offline_s offline_hit;
	  offline_hit.layer = layer;
	  offline_hit.ladder_phi = phi;
	  offline_hit.ladder_z = z;
	  offline_hit.strip_x = row;
	  offline_hit.strip_y = col;

	  InttNameSpace::Online_s online_hit = InttNameSpace::ToOnline( offline_hit );
	  
	  auto hit_info = pair < InttNameSpace::Online_s, unsigned int >( online_hit, hit->getAdc() );
	  hits_information.push_back( hit_info );
	  // cout << "\t\tHit " << key << "\t"
	  //      << col << "\t"
	  //      << row << "\t"
	  //      << hit->getAdc() << "\t|  "
	  //      << online_hit.ldr << "\t"
	  //      << online_hit.arm
	  //      << endl;
	  
	  current2++;
	}
      
      current++;
    }

  return hits_information;
}

/////////////////////////////////////////////////////////////////////////
// public
/////////////////////////////////////////////////////////////////////////

void InttHitCorrelation::SetOutputDir( string dir )
{
  if( dir != "" )
    {
      output_dir_ = dir;
    }

  string run_num_str = string( 8 - to_string(run_num_).size(), '0' ) + to_string( run_num_ );
  output_root_ = output_dir_ + output_basename_ + run_num_str;

  if( fphx_bco_in_use_ == -1 )
    output_root_ += ".root";
  else
    output_root_ += "_FPHX_BCO_" + to_string( fphx_bco_in_use_ ) + ".root";
  
}

/////////////////////////////////////////////////////////////////////////
// Fun4All stuff
/////////////////////////////////////////////////////////////////////////

int InttHitCorrelation::Init(PHCompositeNode *topNode)
{
  return Fun4AllReturnCodes::EVENT_OK;
}

int InttHitCorrelation::InitRun(PHCompositeNode *topNode)
{

  // If an user sets the run number to recoConsts, it works. Can I assume it?
  recoConsts *rc = recoConsts::instance();
  run_num_ = rc->get_IntFlag( "RUNNUMBER" );

  this->SetOutputDir();
  tf_output_ = new TFile( output_root_.c_str(), "RECREATE" );

  hist_barrel_correlation_ = new TH2D( "inner_outer_barrels",
				       "Inner barrel vs Outer barrel;#hit_{Inner};#hit_{Outer};Entries",
				       500, 0, 500,
				       500, 0, 500 );
  
  hist_barrel_correlation_no_adc0_ = new TH2D( "inner_outer_barrels_no_adc0",
					       "Inner barrel vs Outer barrel;#hit_{Inner};#hit_{Outer};Entries",
					       500, 0, 500,
					       500, 0, 500 );
  
  
  return Fun4AllReturnCodes::EVENT_OK;
}

int InttHitCorrelation::process_event(PHCompositeNode *topNode)
{

  // Get nodes to access data
  auto status = this->GetNodes(topNode);

  // If a node is not available, skip this event
  if (status == Fun4AllReturnCodes::ABORTEVENT)
    return Fun4AllReturnCodes::ABORTEVENT;

  if( node_trkrhitset_map_->size() == 0 )
    return Fun4AllReturnCodes::EVENT_OK;

  event_counter_by_myself_++;
  
  auto hits = GetHits( node_trkrhitset_map_->getHitSets() ); // get TrkrHitSet of INTT only, this is ConstRange = std::pair< ConstIterator, ConstIterator >

  //int bco_full = 0;

  // FPHX BCO and event counter
  auto bco_event_counter_pair = this->GetBcoEventCounter(); // first: uint16_t (FPHX BCO), second: int (event counter)

  int hit_num_barrel[ 2 ][ InttQa::kBco_max ] = { { 0 } }; // 0: inner, 1:outer
  int hit_num_barrel_no_adc0[ 2 ][ InttQa::kBco_max ] = { { 0 } }; // 0: inner, 1:outer
  
  for( int i=0; i<hits.size(); i++ )
    {
      ////////////////////////////////////////////
      // struct Online_s                        //
      //   int lyr, int ldr,  int arm           //
      //   int chp, int chn                     //
      ////////////////////////////////////////////
      auto online_hit = hits[i].first;

      ////////////////////////////////////////////
      // struct RawData_s:                      //
      //   int felix_server, int felix_channel  //
      //   int chip,         int channel,       //
      //----------------------------------------//
      //  chip == Online_s::chp                 //
      //  channel == online_s::chn  (0-255)     //
      ////////////////////////////////////////////
      // InttNameSpace::RawData_s
      auto raw_data  = InttNameSpace::ToRawData( online_hit );
      
      ////////////////////////////////////////////
      // struct Offline_s                       //
      //   int layer,     int ladder_phi,       //
      //   int ladder_z (0-3),                  //
      //   int strip_x :                        //
      //   int strip_y : chip column            //
      ////////////////////////////////////////////
      // InttNameSpace::Offline_s
      auto offline_hit = InttNameSpace::ToOffline( online_hit );

      //////////////////////////////////////////////////////////
      //////////////////////////////////////////////////////////
      //void TH2INTT::SetLadderIContent(int barrel_id, int layer_id, int ladder_id, int side, double content)
      int barrel = ( offline_hit.layer <= 4     ? 0 : 1 ); // 0: innter barrel, 1: outer, maybe
      int layer  = ( offline_hit.layer % 2 == 1 ? 0 : 1 ); // 0: inner layer, 1: outer layer, maybe
      int side   = ( offline_hit.ladder_z <= 1  ? 0 : 1 ); // 0: south, 1; north
      // cout << setw(3) << offline_hit.layer
      // 	   << setw(3) << barrel
      // 	   << setw(3) << layer
      // 	   << setw(4) << offline_hit.ladder_phi
      // 	   << setw(3) << side
      // 	   << setw(3) << offline_hit.ladder_z
      // 	   << endl;
      
      hit_num_barrel[ barrel ][  bco_event_counter_pair[i].first ]++;

      auto adc = hits[i].second;
      // ADC > 0, DAC0 can be 30 or 35. adc > 35 works for both cases
      if( adc > 35 )
	hit_num_barrel_no_adc0[ barrel ][  bco_event_counter_pair[i].first ]++; 
	
    }

  for( int bco=0; bco<InttQa::kBco_max; bco++ )
    {

      // if no hit on INTT, skip it
      if( hit_num_barrel[ 0 ][  bco ] == 0 
	  && hit_num_barrel[ 1 ][  bco ] == 0 )
	continue;
      
      hist_barrel_correlation_->Fill( hit_num_barrel[ 0 ][ bco ], hit_num_barrel[ 1 ][ bco ] );
      hist_barrel_correlation_no_adc0_->Fill( hit_num_barrel_no_adc0[ 0 ][ bco ], hit_num_barrel_no_adc0[ 1 ][ bco ] );
    }
  
  return Fun4AllReturnCodes::EVENT_OK;
}

int InttHitCorrelation::ResetEvent(PHCompositeNode *topNode)
{

  return Fun4AllReturnCodes::EVENT_OK;
}

int InttHitCorrelation::EndRun(const int runnumber)
{
   
  // Barrel correlation
  tf_output_->WriteTObject( hist_barrel_correlation_, hist_barrel_correlation_->GetName() );
  tf_output_->WriteTObject( hist_barrel_correlation_no_adc0_, hist_barrel_correlation_no_adc0_->GetName() );
  
  // Close the ROOT file
  tf_output_->Close();
  return Fun4AllReturnCodes::EVENT_OK;
}

int InttHitCorrelation::End(PHCompositeNode *topNode)
{

  return Fun4AllReturnCodes::EVENT_OK;
}

int InttHitCorrelation::Reset(PHCompositeNode *topNode)
{

  return Fun4AllReturnCodes::EVENT_OK;
}

void InttHitCorrelation::Print(const std::string &what) const
{

  int width = 100;
  cout << string( width, '-' ) << endl;
  cout << "InttHitCorrelation" << endl;
  cout << "  - Output (ROOT): " << output_root_ << endl;
  cout << string( width, '-' ) << endl;
}
