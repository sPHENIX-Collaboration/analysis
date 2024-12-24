#include "InttStreamingTiming.h"

using namespace std;

//____________________________________________________________________________..
InttStreamingTiming::InttStreamingTiming(const std::string &name) : 
  SubsysReco(name) 
{
  
}

//____________________________________________________________________________..
InttStreamingTiming::~InttStreamingTiming()
{

}

/////////////////////////////////////////////////////////////////////////
// private
/////////////////////////////////////////////////////////////////////////
int InttStreamingTiming::GetNodes(PHCompositeNode *topNode)
{

  /////////////////////////////////////////////////////////////////////////
  // GL1 raw hit node
  string gl1_raw_node_name = "GL1RAWHIT";
  gl1_ = findNode::getClass<Gl1Packetv2>( topNode, gl1_raw_node_name);
  //Gl1Packet* gl1  = findNode::getClass<Gl1Packet>(topNode, "GL1RAWHIT");
  
  if( !gl1_ )
    {
      
      cerr << PHWHERE << gl1_raw_node_name << " node is missing." << endl;
      return Fun4AllReturnCodes::ABORTEVENT;
    }
  
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

vector < std::pair < uint16_t, int > > InttStreamingTiming::GetBcoEventCounter()
{
  //cout << "vector < std::pair < uint16_t, int > > InttStreamingTiming::GetBcoEventCounter()" << endl;
  vector < pair < InttNameSpace::Online_s,  unsigned int > > online_hits = this->GetHits(  node_trkrhitset_map_->getHitSets() );

  vector < InttRawHit* > raw_hits = this->GetRawHitsWithoutClone();
  vector < std::pair < uint16_t, int > > rtn;
  
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

	  auto bco_raw		= raw_hit->get_FPHX_BCO();
	  int event_counter_raw	= raw_hit->get_event_counter(); // uint32_t IttRawHit::get_event_counter()
	  pair < uint16_t, int > bco_event_counter( bco_raw, event_counter_raw );
	  rtn.push_back( bco_event_counter );
	  counter++;
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

std::vector < int > InttStreamingTiming::GetTriggerBits()
{

  //uint64_t trigger_vector = gl1_->getScaledVector();
  uint64_t trigger_vector = gl1_->getLiveVector();
  
  vector < int > rtn;
  while( trigger_vector != 0 )
    {
      int this_bit = 0 ;
      this_bit = trigger_vector & 1;
      // cout << std::bitset<32>(trigger_vector) << "  "
      // 	   << this_bit << "\t";

      trigger_vector = trigger_vector >> 1;

      //cout << std::bitset<32>(trigger_vector) << endl;

      rtn.push_back( this_bit );
    }


  // for( int i=0; i<rtn.size() ; i++ )
  //   {
  //     if( i == 6 || i == 7 )
  // 	continue;
      
  //     if( rtn[i] != 0 )
  // 	{
  // 	  cout << trigger_names_[ i ] << ",\t";
  // 	}
  //   }
  // cout << endl;
  
  return rtn;
}

bool InttStreamingTiming::IsSame( InttRawHit* hit1, InttRawHit* hit2 )
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

vector < InttRawHit* > InttStreamingTiming::GetRawHits()
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

vector < InttRawHit* > InttStreamingTiming::GetRawHitsWithoutClone()
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
InttStreamingTiming::GetHits( TrkrHitSetContainer::ConstRange hitsets )
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

void InttStreamingTiming::SetOutputDir( string dir )
{
  if( dir != "" )
    {
      output_dir_ = dir;
    }

  string run_num_str = string( 8 - to_string(run_num_).size(), '0' ) + to_string( run_num_ );
  output_root_ = output_dir_ + output_basename_ + run_num_str + ".root";
  output_txt_ = output_dir_ + output_basename_ + run_num_str + ".dat";
  
}

/////////////////////////////////////////////////////////////////////////
// Fun4All stuff
/////////////////////////////////////////////////////////////////////////

int InttStreamingTiming::Init(PHCompositeNode *topNode)
{
  return Fun4AllReturnCodes::EVENT_OK;
}

int InttStreamingTiming::InitRun(PHCompositeNode *topNode)
{

  // If an user sets the run number to recoConsts, it works. Can I assume it?
  recoConsts *rc = recoConsts::instance();
  run_num_ = rc->get_IntFlag( "RUNNUMBER" );

  this->SetOutputDir();
  tf_output_ = new TFile( output_root_.c_str(), "RECREATE" );

  // just a FPHX BCO distribution
  hist_fphx_bco_ = new TH1D( "fphx_bco", "FPHX BCO;INTT Local Clock [BCO];Counts",
			     120, 0, 120 );

  hist_fphx_bco_raw_ = new TH1D( "fphx_bco_raw", "FPHX BCO (raw);INTT Local Clock [BCO];Counts",
				 120, 0, 120 );

  hist_streaming_offset_ = new TH1D( "streaming_offset",
				     "BCO_{GL1} - BCO_{INTT GTM}; BCO_{GL1} - BCO_{INTT GTM} [BCO]; Counts",
				     256, -128, 128 );
    

  ofs_ = ofstream( output_txt_.c_str() ); // overwrite mode
  ofs_ << "#event" << "\t" // # means comment out
      << "GL1_BCO" << "\t"
      << "INTT_GTM_BCO" << "\t";
  for( int i=0; i<120; i++ )
    {
      ofs_ << "FPHX" << i << "\t";
    }

  ofs_ << "\n"; // don't forget line break
  
  return Fun4AllReturnCodes::EVENT_OK;
}

int InttStreamingTiming::process_event(PHCompositeNode *topNode)
{
  
  // Get nodes to access data
  auto status = this->GetNodes(topNode);
  
  // If a node is not available, skip this event
  if (status == Fun4AllReturnCodes::ABORTEVENT)
    return Fun4AllReturnCodes::ABORTEVENT;

  // If no hit is found, skip it
  if( node_trkrhitset_map_->size() == 0 )
    {
      return Fun4AllReturnCodes::EVENT_OK;
    }

  ////////////////////////////////////////////////////////////////////////
  // Get parameters of this event                                       //
  ////////////////////////////////////////////////////////////////////////
  event_counter_by_myself_++;
  auto bco_gl1 =  (gl1_->getBCO() & 0xFFFFFFFFFF);
  auto bco_intt = (node_inttrawhit_map_->get_hit( 0 )->get_bco());  

  auto trigger_bits = this->GetTriggerBits(); // 0, 1, 2, ...
  // if MBD N&S >= 1 triiger was not fired, skip it
  if( required_trigger_bit_ != -1 )
    if( trigger_bits[ required_trigger_bit_ ] == 0 )
      {
	return Fun4AllReturnCodes::EVENT_OK;      
      }

  event_counter_++;

  ////////////////////////////////////////////////////////////////////////
  // Get TrkrHit on INTT                                                //
  ////////////////////////////////////////////////////////////////////////
  // get TrkrHitSet of INTT only, this is ConstRange = std::pair< ConstIterator, ConstIterator >
  auto hits = GetHits( node_trkrhitset_map_->getHitSets() );
  // FPHX BCO and event counter
  auto bco_event_counter_pair = this->GetBcoEventCounter(); // first: uint16_t (FPHX BCO), second: int (event counter)

  ////////////////////////////////////////////////////////////////////////
  // loop over all hits                                                 //
  ////////////////////////////////////////////////////////////////////////
  int counts[ 128 ] = { 0 }; // BCO bin for this event
  for( int i=0; i<hits.size(); i++ )
    {
      auto adc = hits[i].second;
      int bco_fphx = bco_event_counter_pair[i].first;

      //      counts[ bco_fphx ]++;
      counts[ bco_fphx ]++;
      hist_fphx_bco_raw_->Fill( bco_event_counter_pair[i].first ); // take all hits
      if( adc > 35 )
	{
	  //counts[ bco_fphx ]++;
	  hist_fphx_bco_->Fill( bco_event_counter_pair[i].first ); // take hits with ADC>1
	  
	  auto bco_hit = bco_intt + bco_fphx;
	  int diff = bco_hit - bco_gl1;
	  hist_streaming_offset_->Fill( diff );
	} // end of if( adc > 35 )

    } // end of for( int i=0; i<hits.size(); i++ )

  auto raw_hits = this->GetRawHitsWithoutClone();
  for( auto& hit : raw_hits )
    {
      int bco_fphx = hit->get_FPHX_BCO();
      counts[ bco_fphx ]++;
    }
  
  // write #hit for each BCO in this event to a text file
  if( event_counter_by_myself_ < event_max_ )
    {
      //      stringstream ss;
      ofs_ << event_counter_by_myself_ << "\t"
	   << bco_gl1 << "\t" 
	   << bco_intt << "\t";
      
      for( auto& val : counts )
	ofs_ << val << "\t";
      
      ofs_ << "\n";

      //ofs_ << ss.str();      
    }
  
  return Fun4AllReturnCodes::EVENT_OK;
}

int InttStreamingTiming::ResetEvent(PHCompositeNode *topNode)
{

  return Fun4AllReturnCodes::EVENT_OK;
}

int InttStreamingTiming::EndRun(const int runnumber)
{

  ofs_.close();

  // Barrel correlation
  tf_output_->WriteTObject( hist_fphx_bco_, hist_fphx_bco_->GetName() );
  tf_output_->WriteTObject( hist_fphx_bco_raw_, hist_fphx_bco_raw_->GetName() );
  tf_output_->WriteTObject( hist_streaming_offset_, hist_streaming_offset_->GetName() );
  
  // Close the ROOT file
  tf_output_->Close();
  return Fun4AllReturnCodes::EVENT_OK;
}

int InttStreamingTiming::End(PHCompositeNode *topNode)
{

  return Fun4AllReturnCodes::EVENT_OK;
}

int InttStreamingTiming::Reset(PHCompositeNode *topNode)
{

  return Fun4AllReturnCodes::EVENT_OK;
}

void InttStreamingTiming::Print(const std::string &what) const
{

  int width = 100;
  cout << string( width, '-' ) << endl;
  cout << "InttStreamingTiming" << endl;
  cout << "  - Output (ROOT): " << output_root_ << endl;
  cout << string( width, '-' ) << endl;
}
