#include "InttClusterCorrelation.h"

using namespace std;

//____________________________________________________________________________..
InttClusterCorrelation::InttClusterCorrelation(const std::string &name, bool is_official ):
  SubsysReco(name), is_official_(is_official)
{

  
}

//____________________________________________________________________________..
InttClusterCorrelation::~InttClusterCorrelation()
{

}

/////////////////////////////////////////////////////////////////////////
// private
/////////////////////////////////////////////////////////////////////////
int InttClusterCorrelation::GetNodes(PHCompositeNode *topNode)
{

  /////////////////////////////////////////////////////////////////////////
  // TrkrCluster
  // TRKR_CLUSTER node: Information of TrkrCluster
  string node_name_trkr_cluster = "TRKR_CLUSTER";
  node_cluster_map_ =
    findNode::getClass<TrkrClusterContainerv4>(topNode, node_name_trkr_cluster);

  if(!node_cluster_map_ )
    {
      std::cerr << PHWHERE << node_name_trkr_cluster << " node is missing." << std::endl;
      return Fun4AllReturnCodes::ABORTEVENT;
    }

  return 0;
}

/////////////////////////////////////////////////////////////////////////
// public
/////////////////////////////////////////////////////////////////////////

void InttClusterCorrelation::SetOutputDir( string dir )
{
  if( dir != "" )
    {
      output_dir_ = dir;
    }

  string run_num_str = string( 8 - to_string(run_num_).size(), '0' ) + to_string( run_num_ );
  output_root_ = output_dir_ + output_basename_ + run_num_str;

  if( fphx_bco_in_use_ == -1 )
    //output_root_ += "_with_gl1.root";
    output_root_ += ".root";
  else
    output_root_ += "_FPHX_BCO_" + to_string( fphx_bco_in_use_ ) + ".root";
  

}

/////////////////////////////////////////////////////////////////////////
// Fun4All stuff
/////////////////////////////////////////////////////////////////////////

int InttClusterCorrelation::Init(PHCompositeNode *topNode)
{

  return Fun4AllReturnCodes::EVENT_OK;
}

int InttClusterCorrelation::InitRun(PHCompositeNode *topNode)
{
  // I want to get the run number here... 
  // auto status = this->GetNodes( topNode );
  // if (status == Fun4AllReturnCodes::ABORTEVENT)
  //   return Fun4AllReturnCodes::ABORTEVENT;

  // If an user sets the run number to recoConsts, it works. Can I assume it?
  recoConsts *rc = recoConsts::instance();
  run_num_ = rc->get_IntFlag( "RUNNUMBER" );

  this->SetOutputDir();
  tf_output_ = new TFile( output_root_.c_str(), "RECREATE" );
  hist_barrel_correlation_ = new TH2D( "inner_outer_barrels",
				       "Inner barrel vs Outer barrel;#cluster_{Inner};#cluster_{Outer};Entries",
				       500, 0, 500,
				       500, 0, 500 );
  
  hist_barrel_correlation_no_adc0_ = new TH2D( "inner_outer_barrels_no_adc0",
					       "Inner barrel vs Outer barrel;#cluster_{Inner};#cluster_{Outer};Entries",
					       500, 0, 500,
					       500, 0, 500 );
  
  return Fun4AllReturnCodes::EVENT_OK;
}

int InttClusterCorrelation::process_event(PHCompositeNode *topNode)
{
  // Get nodes to access data
  auto status = this->GetNodes(topNode);

  // If a node is not available, skip this event
  if (status == Fun4AllReturnCodes::ABORTEVENT)
    {
      // for counting the number of events
      hist_barrel_correlation_->Fill( -1, -1 );
      hist_barrel_correlation_no_adc0_->Fill( -1, -1 );
      return Fun4AllReturnCodes::ABORTEVENT;
    }

  int cluster_num_inner = 0;
  int cluster_num_outer = 0;
  int hit_num_barrel[ 2 ] = { 0 }; // 0: inner, 1: outer
  int hit_num_barrel_no_adc0[ 2 ] = { 0 }; // 0: inner, 1: outer

  ///  vector < TrkrCluster* > clusters;

  for (unsigned int inttlayer = 0; inttlayer < 4; inttlayer++)
    {

      // loop over all hits
      for (const auto &hitsetkey : node_cluster_map_->getHitSetKeys(TrkrDefs::TrkrId::inttId, inttlayer + 3) )
	{

	  // type: std::pair<ConstIterator, ConstIterator> ConstRange
	  // here, MMap_::const_iterator ConstIterator;
	  auto range = node_cluster_map_->getClusters(hitsetkey);
	  
	  // loop over iterators of this cluster
	  for (auto clusIter = range.first; clusIter != range.second; ++clusIter)
	    {
	      const auto cluskey = clusIter->first;
	      const auto cluster = clusIter->second;
	      

	      int layer_index = 0; // 0: inner, 1: outer
	      if( inttlayer < 2 )
		{
		  cluster_num_inner++;
		}
	      else
		{
		  layer_index = 1;
		  cluster_num_outer++;
		}

	      hit_num_barrel[ layer_index ]++;
	      
	      if( cluster->getAdc() > 35 )
		hit_num_barrel_no_adc0[ layer_index ]++;

	      // getLocalX ()	;  getLocalY	() ;
	      // getAdc ()		;  getMaxAdc	() ;  getOverlap () ;
	      // getEdge ()	;  getTime	() ;  getSize () ;  getPhiSize () ;  getZSize () ;
	      // getPhiError ()	;  getRPhiError () ;  getZError () ;      
	      // getX ()		;  getY		() ;  getZ () ;  getPosition (int) ;
	      // isGlobal ()	;  getError (unsigned int, unsigned int) ;  getSize (unsigned int, unsigned int) ;
	      // cout << cluster->getTime() << "\t"
	      // 	   << cluster->getSize() << "\t"
	      // 	   << cluster->getPhiSize() << "\t"
	      // 	   << cluster->getZSize()
	      // 	   << endl;
	      
	      //clusters.push_back( cluster );
	    }
	}
    }
  
  hist_barrel_correlation_->Fill( cluster_num_inner, cluster_num_outer );
  hist_barrel_correlation_no_adc0_->Fill( hit_num_barrel_no_adc0[0], hit_num_barrel_no_adc0[1] );
  
  event_counter_by_myself_++;
  
  return Fun4AllReturnCodes::EVENT_OK;
}

int InttClusterCorrelation::ResetEvent(PHCompositeNode *topNode)
{

  return Fun4AllReturnCodes::EVENT_OK;
}

int InttClusterCorrelation::EndRun(const int runnumber)
{

  tf_output_->WriteTObject( hist_barrel_correlation_, hist_barrel_correlation_->GetName() );
  tf_output_->WriteTObject( hist_barrel_correlation_no_adc0_, hist_barrel_correlation_no_adc0_->GetName() );

  // Close the ROOT file
  tf_output_->Close();
  
  return Fun4AllReturnCodes::EVENT_OK;
}

int InttClusterCorrelation::End(PHCompositeNode *topNode)
{

  return Fun4AllReturnCodes::EVENT_OK;
}

int InttClusterCorrelation::Reset(PHCompositeNode *topNode)
{

  return Fun4AllReturnCodes::EVENT_OK;
}

void InttClusterCorrelation::Print(const std::string &what) const
{
  int width = 100;
  cout << string( width, '-' ) << endl;
  cout << "InttClusterCorrelation" << endl;
  cout << "  - Output (ROOT): " << output_root_ << endl;
  cout << string( width, '-' ) << endl;
}
