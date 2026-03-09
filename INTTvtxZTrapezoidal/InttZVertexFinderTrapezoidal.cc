//____________________________________________________________________________..
//
// This is a template for a Fun4All SubsysReco module with all methods from the
// $OFFLINE_MAIN/include/fun4all/SubsysReco.h baseclass
// You do not have to implement all of them, you can just remove unused methods
// here and in InttZVertexFinderTrapezoidal.h.
//
// InttZVertexFinderTrapezoidal(const std::string &name = "InttZVertexFinderTrapezoidal")
// everything is keyed to InttZVertexFinderTrapezoidal, duplicate names do work but it makes
// e.g. finding culprits in logs difficult or getting a pointer to the module
// from the command line
//
// InttZVertexFinderTrapezoidal::~InttZVertexFinderTrapezoidal()
// this is called when the Fun4AllServer is deleted at the end of running. Be
// mindful what you delete - you do loose ownership of object you put on the node tree
//
// int InttZVertexFinderTrapezoidal::Init(PHCompositeNode *topNode)
// This method is called when the module is registered with the Fun4AllServer. You
// can create historgrams here or put objects on the node tree but be aware that
// modules which haven't been registered yet did not put antyhing on the node tree
//
// int InttZVertexFinderTrapezoidal::InitRun(PHCompositeNode *topNode)
// This method is called when the first event is read (or generated). At
// this point the run number is known (which is mainly interesting for raw data
// processing). Also all objects are on the node tree in case your module's action
// depends on what else is around. Last chance to put nodes under the DST Node
// We mix events during readback if branches are added after the first event
//
// int InttZVertexFinderTrapezoidal::process_event(PHCompositeNode *topNode)
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
// int InttZVertexFinderTrapezoidal::ResetEvent(PHCompositeNode *topNode)
// If you have internal data structures (arrays, stl containers) which needs clearing
// after each event, this is the place to do that. The nodes under the DST node are cleared
// by the framework
//
// int InttZVertexFinderTrapezoidal::EndRun(const int runnumber)
// This method is called at the end of a run when an event from a new run is
// encountered. Useful when analyzing multiple runs (raw data). Also called at
// the end of processing (before the End() method)
//
// int InttZVertexFinderTrapezoidal::End(PHCompositeNode *topNode)
// This is called at the end of processing. It needs to be called by the macro
// by Fun4AllServer::End(), so do not forget this in your macro
//
// int InttZVertexFinderTrapezoidal::Reset(PHCompositeNode *topNode)
// not really used - it is called before the dtor is called
//
// void InttZVertexFinderTrapezoidal::Print(const std::string &what) const
// Called from the command line - useful to print information when you need it
//
//____________________________________________________________________________..

#include "InttZVertexFinderTrapezoidal.h"

//____________________________________________________________________________..
InttZVertexFinderTrapezoidal::InttZVertexFinderTrapezoidal(
  const std::string &name,
  std::pair<double, double> vertexXYIncm_in, // note : cm
  bool IsFieldOn_in,
  bool IsDCACutApplied_in,
  std::pair<std::pair<double,double>,std::pair<double,double>> DeltaPhiCutInDegree_in, // note : in degree
  std::pair<std::pair<double,double>,std::pair<double,double>> DCAcutIncm_in, // note : in cm
  int ClusAdcCut_in,
  int ClusPhiSizeCut_in,
  bool PrintRecoDetails_in
):
  SubsysReco(name),
  vertexXYIncm(vertexXYIncm_in),
  IsFieldOn(IsFieldOn_in),
  IsDCACutApplied(IsDCACutApplied_in),
  DeltaPhiCutInDegree(DeltaPhiCutInDegree_in),
  DCAcutIncm(DCAcutIncm_in),
  ClusAdcCut(ClusAdcCut_in),
  ClusPhiSizeCut(ClusPhiSizeCut_in),
  PrintRecoDetails(PrintRecoDetails_in)
  // m_vertexXYInmm(std::make_pair(vertexXYIncm.first*10., vertexXYIncm.second*10.)),
  // m_DCAcutInmm(std::make_pair(std::make_pair(DCAcutIncm.first.first*10., DCAcutIncm.first.second*10.), std::make_pair(DCAcutIncm.second.first*10., DCAcutIncm.second.second*10.)))
{
  std::cout << "InttZVertexFinderTrapezoidal::InttZVertexFinderTrapezoidal(const std::string &name) Calling ctor" << std::endl;

    evt_possible_z = new TH1D("","evt_possible_z",50, evt_possible_z_range.first, evt_possible_z_range.second);
    evt_possible_z -> SetLineWidth(1);
    evt_possible_z -> GetXaxis() -> SetTitle("Z [cm]");
    evt_possible_z -> GetYaxis() -> SetTitle("Entry");


    line_breakdown_hist = new TH1D("", "line_breakdown_hist", 2*zvtx_hist_Nbins+1, -1*(fine_bin_width*zvtx_hist_Nbins + fine_bin_width/2.), fine_bin_width*zvtx_hist_Nbins + fine_bin_width/2.);
    line_breakdown_hist -> SetLineWidth(1);
    line_breakdown_hist -> GetXaxis() -> SetTitle("Z [cm]");
    line_breakdown_hist -> GetYaxis() -> SetTitle("Entries [A.U.]");
    line_breakdown_hist -> GetXaxis() -> SetNdivisions(705);

    combination_zvtx_range_shape = new TH1D(
        "",
        "",
        line_breakdown_hist -> GetNbinsX(),
        line_breakdown_hist -> GetXaxis() -> GetXmin(),
        line_breakdown_hist -> GetXaxis() -> GetXmax()
    );

    temp_sPH_inner_nocolumn_vec.clear();
    temp_sPH_outer_nocolumn_vec.clear();

    gaus_fit = new TF1("gaus_fit",gaus_func,evt_possible_z_range.first,evt_possible_z_range.second,4);
    gaus_fit -> SetLineColor(2);
    gaus_fit -> SetLineWidth(1);
    gaus_fit -> SetNpx(1000);

    gaus_fit_vec.clear();
    for (int i = 0; i < number_of_gaus; i++) 
    {
        gaus_fit_vec.push_back(new TF1("gaus_fit_vec",gaus_func,evt_possible_z_range.first,evt_possible_z_range.second,4));
        gaus_fit_vec[i] -> SetLineColor(TColor::GetColor(color_code[i].c_str()));
        gaus_fit_vec[i] -> SetLineWidth(1);
        gaus_fit_vec[i] -> SetNpx(1000);
    }

    

}

//____________________________________________________________________________..
InttZVertexFinderTrapezoidal::~InttZVertexFinderTrapezoidal()
{
  std::cout << "InttZVertexFinderTrapezoidal::~InttZVertexFinderTrapezoidal() Calling dtor" << std::endl;
}

//____________________________________________________________________________..
int InttZVertexFinderTrapezoidal::Init(PHCompositeNode *topNode)
{
  std::cout << "InttZVertexFinderTrapezoidal::Init(PHCompositeNode *topNode) Initializing" << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int InttZVertexFinderTrapezoidal::InitRun(PHCompositeNode *topNode)
{
  std::cout << "InttZVertexFinderTrapezoidal::InitRun(PHCompositeNode *topNode) Initializing for Run XXX" << std::endl;

  if (createNodes(topNode) == Fun4AllReturnCodes::ABORTEVENT)
  {
    std::cout << PHWHERE << "Could not create nodes. Exiting." << std::endl;
    return Fun4AllReturnCodes::ABORTEVENT;
  }

  return Fun4AllReturnCodes::EVENT_OK;
}

int InttZVertexFinderTrapezoidal::PrepareClusterVec(PHCompositeNode *topNode)
{
  // std::cout << "InttZVertexFinderTrapezoidal::PrepareClusterVec(PHCompositeNode *topNode) Preparing Cluster Vector" << std::endl;

  ActsGeometry* tGeometry = findNode::getClass<ActsGeometry>(topNode, "ActsGeometry");
  if (!tGeometry)
  {
    std::cout << PHWHERE << "No ActsGeometry on node tree. Bailing." << std::endl;
    return Fun4AllReturnCodes::ABORTEVENT;
  }

  TrkrClusterContainer* clusterMap =
      findNode::getClass<TrkrClusterContainer>(topNode, "TRKR_CLUSTER");
  if (!clusterMap)
  {
    std::cout << PHWHERE << "TrkrClusterContainer node is missing." << std::endl;
    return Fun4AllReturnCodes::ABORTEVENT;
  }


  for (unsigned int inttlayer = 0; inttlayer < 4; inttlayer++)
  {
    std::vector<clu_info>* p_temp_sPH_nocolumn_vec =
        (inttlayer < 2) ? (&temp_sPH_inner_nocolumn_vec)
                        : (&temp_sPH_outer_nocolumn_vec);

    for (const auto& hitsetkey : clusterMap->getHitSetKeys(TrkrDefs::TrkrId::inttId, inttlayer + 3))
    {
      auto range = clusterMap->getClusters(hitsetkey);

      for (auto clusIter = range.first; clusIter != range.second; ++clusIter)
      {
        NClusAll += 1;
        NClusAllInner += (inttlayer < 2) ? 1 : 0;

        const auto cluskey = clusIter->first;
        const auto cluster = clusIter->second;

        const auto globalPos = tGeometry->getGlobalPosition(cluskey, cluster);
        double clu_x = globalPos(0);  //note: unit in cm
        double clu_y = globalPos(1);  //note: unit in cm
        double clu_z = globalPos(2);  //note: unit in cm

        // double clu_phi = (clu_y < 0) ? atan2(clu_y, clu_x) * (180. / M_PI) + 360
        //                              : atan2(clu_y, clu_x) * (180. / M_PI);

        // double clu_radius = sqrt(pow(clu_x, 2) + pow(clu_y, 2));

        // int size = cluster->getSize();
        int phisize = cluster->getPhiSize();
        int adc = cluster->getAdc();
        int sensorZID = InttDefs::getLadderZId(cluskey);

        if (adc <= ClusAdcCut || phisize > ClusPhiSizeCut) {continue;}

        p_temp_sPH_nocolumn_vec->push_back({
          clu_x,
          clu_y,
          clu_z,
          adc,
          phisize,
          sensorZID
        });
      }
    }
  }


  return Fun4AllReturnCodes::EVENT_OK;
}

int InttZVertexFinderTrapezoidal::PrepareINTTvtxZ()
{
  for (int inner_i = 0; inner_i < int(temp_sPH_inner_nocolumn_vec.size()); inner_i++) {
      double Clus_InnerPhi_Offset = (temp_sPH_inner_nocolumn_vec[inner_i].y - vertexXYIncm.second < 0) ? atan2(temp_sPH_inner_nocolumn_vec[inner_i].y - vertexXYIncm.second, temp_sPH_inner_nocolumn_vec[inner_i].x - vertexXYIncm.first) * (180./TMath::Pi()) + 360 : atan2(temp_sPH_inner_nocolumn_vec[inner_i].y - vertexXYIncm.second, temp_sPH_inner_nocolumn_vec[inner_i].x - vertexXYIncm.first) * (180./TMath::Pi());
      inner_clu_phi_map[ int(Clus_InnerPhi_Offset) ].push_back({false,temp_sPH_inner_nocolumn_vec[inner_i]});
  }
  for (int outer_i = 0; outer_i < int(temp_sPH_outer_nocolumn_vec.size()); outer_i++) {
      double Clus_OuterPhi_Offset = (temp_sPH_outer_nocolumn_vec[outer_i].y - vertexXYIncm.second < 0) ? atan2(temp_sPH_outer_nocolumn_vec[outer_i].y - vertexXYIncm.second, temp_sPH_outer_nocolumn_vec[outer_i].x - vertexXYIncm.first) * (180./TMath::Pi()) + 360 : atan2(temp_sPH_outer_nocolumn_vec[outer_i].y - vertexXYIncm.second, temp_sPH_outer_nocolumn_vec[outer_i].x - vertexXYIncm.first) * (180./TMath::Pi());
      outer_clu_phi_map[ int(Clus_OuterPhi_Offset) ].push_back({false,temp_sPH_outer_nocolumn_vec[outer_i]});
  }

  // note : prepare good tracklet
  for (int inner_phi_i = 0; inner_phi_i < 360; inner_phi_i++) // note : each phi cell (1 degree)
  {
      // note : N cluster in this phi cell
      for (int inner_phi_clu_i = 0; inner_phi_clu_i < int(inner_clu_phi_map[inner_phi_i].size()); inner_phi_clu_i++)
      {
          if (inner_clu_phi_map[inner_phi_i][inner_phi_clu_i].first == true) {continue;}

          double Clus_InnerPhi_Offset = (inner_clu_phi_map[inner_phi_i][inner_phi_clu_i].second.y - vertexXYIncm.second < 0) ? atan2(inner_clu_phi_map[inner_phi_i][inner_phi_clu_i].second.y - vertexXYIncm.second, inner_clu_phi_map[inner_phi_i][inner_phi_clu_i].second.x - vertexXYIncm.first) * (180./TMath::Pi()) + 360 : atan2(inner_clu_phi_map[inner_phi_i][inner_phi_clu_i].second.y - vertexXYIncm.second, inner_clu_phi_map[inner_phi_i][inner_phi_clu_i].second.x - vertexXYIncm.first) * (180./TMath::Pi());

          // todo: change the outer phi scan range
          // note : the outer phi index, -1, 0, 1
          // note : the outer phi index, -5, -4, -3, -2, -1, 0, 1, 2, 3, 4, 5 for the scan test
          for (int scan_i = -5; scan_i < 6; scan_i++)
          {
              int true_scan_i = ((inner_phi_i + scan_i) < 0) ? 360 + (inner_phi_i + scan_i) : ((inner_phi_i + scan_i) > 359) ? (inner_phi_i + scan_i)-360 : inner_phi_i + scan_i;

              // note : N clusters in that outer phi cell
              for (int outer_phi_clu_i = 0; outer_phi_clu_i < int(outer_clu_phi_map[true_scan_i].size()); outer_phi_clu_i++)
              {
                  if (outer_clu_phi_map[true_scan_i][outer_phi_clu_i].first == true) {continue;}

                  double Clus_OuterPhi_Offset = (outer_clu_phi_map[true_scan_i][outer_phi_clu_i].second.y - vertexXYIncm.second < 0) ? atan2(outer_clu_phi_map[true_scan_i][outer_phi_clu_i].second.y - vertexXYIncm.second, outer_clu_phi_map[true_scan_i][outer_phi_clu_i].second.x - vertexXYIncm.first) * (180./TMath::Pi()) + 360 : atan2(outer_clu_phi_map[true_scan_i][outer_phi_clu_i].second.y - vertexXYIncm.second, outer_clu_phi_map[true_scan_i][outer_phi_clu_i].second.x - vertexXYIncm.first) * (180./TMath::Pi());
                  double delta_phi = get_delta_phi(Clus_InnerPhi_Offset, Clus_OuterPhi_Offset);
                  
                  double delta_phi_correct = delta_phi; // note : this is for the further correction, potentially 

                  // note : ----------------------------------------------------------------------------------------------------------------------------------
                  // note : delta phi cut
                  if (!IsFieldOn){ // note : field off
                    if (delta_phi_correct <= DeltaPhiCutInDegree.first.first || delta_phi_correct >= DeltaPhiCutInDegree.first.second) {continue;}
                  }
                  // note : field ON
                  else{
                    if (
                          (delta_phi_correct > DeltaPhiCutInDegree.first.first && delta_phi_correct < DeltaPhiCutInDegree.first.second) == false && 
                          (delta_phi_correct > DeltaPhiCutInDegree.second.first && delta_phi_correct < DeltaPhiCutInDegree.second.second) == false
                       ) 
                       {continue;}
                  }

                  double DCA_sign = calculateAngleBetweenVectors(
                      outer_clu_phi_map[true_scan_i][outer_phi_clu_i].second.x, outer_clu_phi_map[true_scan_i][outer_phi_clu_i].second.y,
                      inner_clu_phi_map[inner_phi_i][inner_phi_clu_i].second.x, inner_clu_phi_map[inner_phi_i][inner_phi_clu_i].second.y,
                      vertexXYIncm.first, vertexXYIncm.second
                  );   

                  // note : ----------------------------------------------------------------------------------------------------------------------------------
                  // note : DCA cut
                  if (IsDCACutApplied)
                  {
                    if (!IsFieldOn){ // note : field off
                      if (DCA_sign <= DCAcutIncm.first.first || DCA_sign >= DCAcutIncm.first.second) {continue;}
                    }
                    else{
                      if (
                          (DCA_sign > DCAcutIncm.first.first && DCA_sign < DCAcutIncm.first.second) == false && 
                          (DCA_sign > DCAcutIncm.second.first && DCA_sign < DCAcutIncm.second.second) == false
                       ) 
                       {continue;}
                    }
                  }

                  // note : ----------------------------------------------------------------------------------------------------------------------------------
                  // note : good proto-tracklet
                  std::pair<double,double> z_range_info = Get_possible_zvtx( 
                      0., // get_radius(vertexXYIncm.first,vertexXYIncm.second), 
                      {
                        get_radius(inner_clu_phi_map[inner_phi_i][inner_phi_clu_i].second.x - vertexXYIncm.first, inner_clu_phi_map[inner_phi_i][inner_phi_clu_i].second.y - vertexXYIncm.second), 
                        inner_clu_phi_map[inner_phi_i][inner_phi_clu_i].second.z,
                        double(inner_clu_phi_map[inner_phi_i][inner_phi_clu_i].second.sensorZID)
                      }, // note : unsign radius
                      
                      {
                        get_radius(outer_clu_phi_map[true_scan_i][outer_phi_clu_i].second.x - vertexXYIncm.first, outer_clu_phi_map[true_scan_i][outer_phi_clu_i].second.y - vertexXYIncm.second), 
                        outer_clu_phi_map[true_scan_i][outer_phi_clu_i].second.z,
                        double(outer_clu_phi_map[true_scan_i][outer_phi_clu_i].second.sensorZID)
                      }  // note : unsign radius
                  );

                  // note : ----------------------------------------------------------------------------------------------------------------------------------
                  // note : check the coverage
                  if (evt_possible_z_range.first < z_range_info.first && z_range_info.first < evt_possible_z_range.second) {
                      evt_possible_z -> Fill(z_range_info.first);

                      // note : fill the line_breakdown histogram as well as a vector for the width determination
                      trapezoidal_line_breakdown( 
                          line_breakdown_hist, 
                          
                          // note : inner_r and inner_z
                          get_radius(inner_clu_phi_map[inner_phi_i][inner_phi_clu_i].second.x - vertexXYIncm.first, inner_clu_phi_map[inner_phi_i][inner_phi_clu_i].second.y - vertexXYIncm.second), 
                          inner_clu_phi_map[inner_phi_i][inner_phi_clu_i].second.z,
                          inner_clu_phi_map[inner_phi_i][inner_phi_clu_i].second.sensorZID,

                          // note : outer_r and outer_z
                          get_radius(outer_clu_phi_map[true_scan_i][outer_phi_clu_i].second.x - vertexXYIncm.first, outer_clu_phi_map[true_scan_i][outer_phi_clu_i].second.y - vertexXYIncm.second), 
                          outer_clu_phi_map[true_scan_i][outer_phi_clu_i].second.z,
                          outer_clu_phi_map[true_scan_i][outer_phi_clu_i].second.sensorZID
                      );
                  }
                  
              }
          } // note : end of outer clu loop
      } 

  } // note : end of inner clu loop

  // todo : some of the events have the special behavior which is having the high entry in the both edges
  // note : prepare the vertex Z by INTT
  for (int bin_i = 0; bin_i < line_breakdown_hist->GetNbinsX(); bin_i++){
      if (line_breakdown_hist -> GetBinCenter(bin_i+1) < edge_rejection.first || line_breakdown_hist -> GetBinCenter(bin_i+1) > edge_rejection.second){
          line_breakdown_hist -> SetBinContent(bin_i+1,0);
      }
  }

  // note : no good proto-tracklet
  if (line_breakdown_hist->Integral(-1,-1) == 0) {
    return Fun4AllReturnCodes::EVENT_OK;
  }

  std::vector<double> N_group_info = find_Ngroup(evt_possible_z);


  std::vector<double> N_group_info_detail = find_Ngroup(line_breakdown_hist);
  double Half_N_group_half_width = fabs(N_group_info_detail[3] - N_group_info_detail[2]) / 2.;
  double line_breakdown_hist_max_content = line_breakdown_hist -> GetBinContent( line_breakdown_hist -> GetMaximumBin() );
  double line_breakdown_hist_max_center = line_breakdown_hist -> GetBinCenter( line_breakdown_hist -> GetMaximumBin() );
  
  // note : first fit is for the width, so apply the constraints on the Gaussian offset
  gaus_fit -> SetParameters(line_breakdown_hist_max_content, line_breakdown_hist_max_center, Half_N_group_half_width, 0);
  gaus_fit -> SetParLimits(0,0,100000);  // note : size 
  gaus_fit -> SetParLimits(2,0.5,1000);   // note : Width
  gaus_fit -> SetParLimits(3,0,10000);   // note : offset
  // todo : the width fit range is 60% of the peak width, // todo : try to use single gaus to fit the distribution
  double width_fit_range_l = line_breakdown_hist_max_center - Half_N_group_half_width * 0.6;
  double width_fit_range_r = line_breakdown_hist_max_center + Half_N_group_half_width * 0.6;
  line_breakdown_hist -> Fit(gaus_fit, "NQ", "", width_fit_range_l, width_fit_range_r); 
  
  // std::cout<<"evnet_i: "<<", width fitting, SetParameters: "<<line_breakdown_hist_max_content<<" "<<line_breakdown_hist_max_center<<" "<<Half_N_group_half_width<<std::endl;
  // std::cout<<"evnet_i: "<<", width fitting, FitRange: "<<width_fit_range_l<<" "<<width_fit_range_r<<std::endl;
  // std::cout<<"evnet_i: "<<", width fitting, result: "<<gaus_fit -> GetParameter(0)<<" "<<gaus_fit -> GetParameter(1)<<" "<<gaus_fit -> GetParameter(2)<<" "<<gaus_fit -> GetParameter(3)<<std::endl;
  

  for (int fit_i = 0; fit_i < int(gaus_fit_vec.size()); fit_i++) // note : special_tag, uses a vector of gaus fit to determine the vertex Z
  {
      gaus_fit_vec[fit_i] -> SetParameters(line_breakdown_hist_max_content, line_breakdown_hist_max_center, Half_N_group_half_width, 0);
      gaus_fit_vec[fit_i] -> SetParLimits(0,0,100000);  // note : size 
      gaus_fit_vec[fit_i] -> SetParLimits(2,0.5,1000);   // note : Width
      gaus_fit_vec[fit_i] -> SetParLimits(3,-200,10000);   // note : offset
      double N_width_ratio = 0.2 + 0.15 * fit_i;
      double mean_fit_range_l = line_breakdown_hist_max_center - Half_N_group_half_width * N_width_ratio;
      double mean_fit_range_r = line_breakdown_hist_max_center + Half_N_group_half_width * N_width_ratio;

      line_breakdown_hist -> Fit(gaus_fit_vec[fit_i], "NQ", "", mean_fit_range_l, mean_fit_range_r);
      gaus_fit_vec[fit_i] -> SetRange(mean_fit_range_l, mean_fit_range_r);

      fit_mean_mean_vec.push_back(gaus_fit_vec[fit_i] -> GetParameter(1));
      fit_mean_reducedChi2_vec.push_back(gaus_fit_vec[fit_i] -> GetChisquare() / double(gaus_fit_vec[fit_i] -> GetNDF()));
      fit_mean_width_vec.push_back(fabs(gaus_fit_vec[fit_i] -> GetParameter(2)));
  }

  INTTvtxZ = vector_average(fit_mean_mean_vec);
  INTTvtxZError = vector_stddev(fit_mean_mean_vec);

  NgroupTrapezoidal = N_group_info_detail[0];
  NgroupCoarse = N_group_info[0];
  TrapezoidalFitWidth = gaus_fit -> GetParameter(2);
  TrapezoidalFWHM = (fabs(N_group_info_detail[3] - N_group_info_detail[2]) / 2.);

  // for (int hist_i = 0; hist_i < line_breakdown_hist->GetNbinsX(); hist_i++){
  //     // std::cout<<"("<<hist_i+1<<", "<<line_breakdown_hist->GetBinContent(hist_i+1)<<"), ";
  //     std::cout<<"{"<<hist_i+1<<", "<<line_breakdown_hist->GetBinContent(hist_i+1)<<"}, "<<std::endl;
  // }
  // std::cout<<std::endl;

  return Fun4AllReturnCodes::EVENT_OK;
}

double InttZVertexFinderTrapezoidal::get_radius(double x, double y)
{
    return sqrt(pow(x,2)+pow(y,2));
}

double InttZVertexFinderTrapezoidal::get_delta_phi(double angle_1, double angle_2)
{
    std::vector<double> vec_abs = {std::fabs(angle_1 - angle_2), std::fabs(angle_1 - angle_2 + 360), std::fabs(angle_1 - angle_2 - 360)};
    std::vector<double> vec = {(angle_1 - angle_2), (angle_1 - angle_2 + 360), (angle_1 - angle_2 - 360)};
    return vec[std::distance(vec_abs.begin(), std::min_element(vec_abs.begin(),vec_abs.end()))];
}

double InttZVertexFinderTrapezoidal::calculateAngleBetweenVectors(double x1, double y1, double x2, double y2, double targetX, double targetY) {
    // Calculate the vectors vector_1 (point_1 to point_2) and vector_2 (point_1 to target)
    double vector1X = x2 - x1;
    double vector1Y = y2 - y1;

    double vector2X = targetX - x1;
    double vector2Y = targetY - y1;

    // Calculate the cross product of vector_1 and vector_2 (z-component)
    double crossProduct = vector1X * vector2Y - vector1Y * vector2X;
    
    // cout<<" crossProduct : "<<crossProduct<<endl;

    // Calculate the magnitudes of vector_1 and vector_2
    double magnitude1 = std::sqrt(vector1X * vector1X + vector1Y * vector1Y);
    double magnitude2 = std::sqrt(vector2X * vector2X + vector2Y * vector2Y);

    // Calculate the angle in radians using the inverse tangent of the cross product and dot product
    double dotProduct = vector1X * vector2X + vector1Y * vector2Y;

    double angleInRadians = std::atan2(std::abs(crossProduct), dotProduct);
    // Convert the angle from radians to degrees and return it
    double angleInDegrees __attribute__((unused)) = angleInRadians * 180.0 / M_PI;
    
    double angleInRadians_new = std::asin( crossProduct/(magnitude1*magnitude2) );
    double angleInDegrees_new __attribute__((unused)) = angleInRadians_new * 180.0 / M_PI;
    
    // cout<<"angle : "<<angleInDegrees_new<<endl;

    double DCA_distance = sin(angleInRadians_new) * magnitude2;

    return DCA_distance;
}

double InttZVertexFinderTrapezoidal::Get_extrapolation(double given_y, double p0x, double p0y, double p1x, double p1y) // note : x : z, y : r
{
    if ( fabs(p0x - p1x) < 0.000001 ){ // note : the line is vertical (if z is along the x axis)
        return p0x;
    }
    else {
        double slope = (p1y - p0y) / (p1x - p0x);
        double yIntercept = p0y - slope * p0x;
        double xCoordinate = (given_y - yIntercept) / slope;
        return xCoordinate;
    }
}

std::pair<double,double> InttZVertexFinderTrapezoidal::Get_possible_zvtx(double rvtx, std::vector<double> p0, std::vector<double> p1) // note : inner p0, outer p1, vector {r,z, zid}, -> {y,x}
{
    std::vector<double> p0_z_edge = { ( p0[2] == typeA_sensorZID[0] || p0[2] == typeA_sensorZID[1] ) ? p0[1] - typeA_sensor_half_length_incm : p0[1] - typeB_sensor_half_length_incm, ( p0[2] == typeA_sensorZID[0] || p0[2] == typeA_sensorZID[1] ) ? p0[1] + typeA_sensor_half_length_incm : p0[1] + typeB_sensor_half_length_incm}; // note : vector {left edge, right edge}
    std::vector<double> p1_z_edge = { ( p1[2] == typeA_sensorZID[0] || p1[2] == typeA_sensorZID[1] ) ? p1[1] - typeA_sensor_half_length_incm : p1[1] - typeB_sensor_half_length_incm, ( p1[2] == typeA_sensorZID[0] || p1[2] == typeA_sensorZID[1] ) ? p1[1] + typeA_sensor_half_length_incm : p1[1] + typeB_sensor_half_length_incm}; // note : vector {left edge, right edge}

    double edge_first  = Get_extrapolation(rvtx,p0_z_edge[0],p0[0],p1_z_edge[1],p1[0]);
    double edge_second = Get_extrapolation(rvtx,p0_z_edge[1],p0[0],p1_z_edge[0],p1[0]);

    double mid_point = (edge_first + edge_second) / 2.;
    double possible_width = fabs(edge_first - edge_second) / 2.;

    return {mid_point, possible_width}; // note : first : mid point, second : width

}

void InttZVertexFinderTrapezoidal::line_breakdown(TH1D* hist_in, std::pair<double,double> line_range)
{ 
    int first_bin = int((line_range.first  - hist_in->GetXaxis()->GetXmin()) /  hist_in->GetBinWidth(1)) + 1;
    int last_bin  = int((line_range.second - hist_in->GetXaxis()->GetXmin()) /  hist_in->GetBinWidth(1)) + 1;
    
    if (first_bin < 1) {first_bin = 0;}
    else if (first_bin > hist_in -> GetNbinsX()) {first_bin = hist_in -> GetNbinsX() + 1;}

    if (last_bin < 1) {last_bin = 0;}
    else if (last_bin > hist_in -> GetNbinsX()) {last_bin = hist_in -> GetNbinsX() + 1;}

    // double fill_weight = 1./fabs(line_range.second - line_range.first); // note : the entry is weitghted by the width of the line, by Akiba san's suggestion // special_tag cancel
    double fill_weight = 1.; // note : the weight is 1, it's for testing the trapezoidal method // special_tag

    // cout<<"Digitize the bin : "<<first_bin<<" "<<last_bin<<endl;

    // note : if first:last = (0:0) or (N+1:N+1) -> the subtraction of them euqals to zero.
    for (int i = 0; i < (last_bin - first_bin) + 1; i++){
        hist_in -> SetBinContent(first_bin + i,  hist_in -> GetBinContent(first_bin + i) + fill_weight ); // note : special_tag
        // line_breakdown_vec.push_back(hist_in -> GetBinCenter(first_bin + i));
    }

}

// note : for each combination
// note : corrected inner_r and outer_r
void InttZVertexFinderTrapezoidal::trapezoidal_line_breakdown(TH1D * hist_in, double inner_r, double inner_z, int inner_zid, double outer_r, double outer_z, int outer_zid)
{
    combination_zvtx_range_shape -> Reset("ICESM");

    std::vector<double> inner_edge = { ( inner_zid == typeA_sensorZID[0] || inner_zid == typeA_sensorZID[1] ) ? inner_z - typeA_sensor_half_length_incm : inner_z - typeB_sensor_half_length_incm, ( inner_zid == typeA_sensorZID[0] || inner_zid == typeA_sensorZID[1] ) ? inner_z + typeA_sensor_half_length_incm : inner_z + typeB_sensor_half_length_incm}; // note : vector {left edge, right edge}
    std::vector<double> outer_edge = { ( outer_zid == typeA_sensorZID[0] || outer_zid == typeA_sensorZID[1] ) ? outer_z - typeA_sensor_half_length_incm : outer_z - typeB_sensor_half_length_incm, ( outer_zid == typeA_sensorZID[0] || outer_zid == typeA_sensorZID[1] ) ? outer_z + typeA_sensor_half_length_incm : outer_z + typeB_sensor_half_length_incm}; // note : vector {left edge, right edge}

    for (int possible_i = 0; possible_i < 2001; possible_i++) // todo : the segmentation of the strip
    {
        // double random_inner_z = rand_gen -> Uniform(inner_edge[0], inner_edge[1]);
        double random_inner_z = inner_edge[0] + ((inner_edge[1] - inner_edge[0]) / 2000.) * possible_i; 
        double edge_first  = Get_extrapolation(0, random_inner_z, inner_r, outer_edge[1], outer_r);
        double edge_second = Get_extrapolation(0, random_inner_z, inner_r, outer_edge[0], outer_r);

        double mid_point = (edge_first + edge_second) / 2.;
        double possible_width = fabs(edge_first - edge_second) / 2.;

        line_breakdown(combination_zvtx_range_shape, {mid_point - possible_width, mid_point + possible_width});
    }

    combination_zvtx_range_shape -> Scale(1./ combination_zvtx_range_shape -> Integral(-1,-1));

    for (int bin_i = 1; bin_i <= combination_zvtx_range_shape -> GetNbinsX(); bin_i++)
    {
        hist_in -> SetBinContent(bin_i, hist_in -> GetBinContent(bin_i) + combination_zvtx_range_shape -> GetBinContent(bin_i));
    }
}

std::vector<double> InttZVertexFinderTrapezoidal::find_Ngroup(TH1D * hist_in)
{
    double Highest_bin_Content  = hist_in -> GetBinContent(hist_in -> GetMaximumBin());
    double Highest_bin_Center   = hist_in -> GetBinCenter(hist_in -> GetMaximumBin());

    int group_Nbin = 0;
    int peak_group_ID = -9999;
    double group_entry = 0;
    double peak_group_ratio;
    std::vector<int> group_Nbin_vec; group_Nbin_vec.clear();
    std::vector<double> group_entry_vec; group_entry_vec.clear();
    std::vector<double> group_widthL_vec; group_widthL_vec.clear();
    std::vector<double> group_widthR_vec; group_widthR_vec.clear();

    for (int i = 0; i < hist_in -> GetNbinsX(); i++){
        // todo : the background rejection is here : Highest_bin_Content/2. for the time being
        double bin_content = ( hist_in -> GetBinContent(i+1) <= Highest_bin_Content/2.) ? 0. : ( hist_in -> GetBinContent(i+1) - Highest_bin_Content/2. );

        if (bin_content != 0){
            
            if (group_Nbin == 0) {
                group_widthL_vec.push_back(hist_in -> GetBinCenter(i+1) - (hist_in -> GetBinWidth(i+1)/2.));
            }

            group_Nbin += 1; 
            group_entry += bin_content;
        }
        else if (bin_content == 0 && group_Nbin != 0){
            group_widthR_vec.push_back(hist_in -> GetBinCenter(i+1) - (hist_in -> GetBinWidth(i+1)/2.));
            group_Nbin_vec.push_back(group_Nbin);
            group_entry_vec.push_back(group_entry);
            group_Nbin = 0;
            group_entry = 0;
        }
    }
    if (group_Nbin != 0) {
        group_Nbin_vec.push_back(group_Nbin);
        group_entry_vec.push_back(group_entry);
        group_widthR_vec.push_back(hist_in -> GetXaxis()->GetXmax());
    } // note : the last group at the edge

    // note : find the peak group
    for (int i = 0; i < int(group_Nbin_vec.size()); i++){
        if (group_widthL_vec[i] < Highest_bin_Center && Highest_bin_Center < group_widthR_vec[i]){
            peak_group_ID = i;
            break;
        }
    }
    
    // note : On Nov 6, 2024, we no longer need to calculate the ratio of the peak group
    // double denominator = (accumulate( group_entry_vec.begin(), group_entry_vec.end(), 0.0 ));
    // denominator = (denominator <= 0) ? 1. : denominator;
    // peak_group_ratio = group_entry_vec[peak_group_ID] / denominator;
    peak_group_ratio = -999;

    double peak_group_left  = (double(group_Nbin_vec.size()) == 0) ? -999 : group_widthL_vec[peak_group_ID];
    double peak_group_right = (double(group_Nbin_vec.size()) == 0) ? 999  : group_widthR_vec[peak_group_ID];

    // for (int i = 0; i < group_Nbin_vec.size(); i++)
    // {
    //     cout<<" "<<endl;
    //     cout<<"group size : "<<group_Nbin_vec[i]<<endl;
    //     cout<<"group entry : "<<group_entry_vec[i]<<endl;
    //     cout<<group_widthL_vec[i]<<" "<<group_widthR_vec[i]<<endl;
    // }

    // cout<<" "<<endl;
    // cout<<"N group : "<<group_Nbin_vec.size()<<endl;
    // cout<<"Peak group ID : "<<peak_group_ID<<endl;
    // cout<<"peak group width : "<<group_widthL_vec[peak_group_ID]<<" "<<group_widthR_vec[peak_group_ID]<<endl;
    // cout<<"ratio : "<<peak_group_ratio<<endl;
    
    // note : {N_group, ratio (if two), peak widthL, peak widthR}
    return {double(group_Nbin_vec.size()), peak_group_ratio, peak_group_left, peak_group_right};
}

double  InttZVertexFinderTrapezoidal::vector_average (std::vector <double> input_vector) {
	return accumulate( input_vector.begin(), input_vector.end(), 0.0 ) / double(input_vector.size());
}

double InttZVertexFinderTrapezoidal::vector_stddev (std::vector <double> input_vector){
	
	double sum_subt = 0;
	double average  = accumulate( input_vector.begin(), input_vector.end(), 0.0 ) / double(input_vector.size());
	
	// cout<<"average is : "<<average<<endl;

	for (int i=0; i<int(input_vector.size()); i++){ sum_subt += pow((input_vector[i] - average),2); }

	//cout<<"sum_subt : "<<sum_subt<<endl;
	// cout<<"print from the function, average : "<<average<<" std : "<<stddev<<endl;

	return sqrt( sum_subt / double(input_vector.size()-1) );
}	

//____________________________________________________________________________..
int InttZVertexFinderTrapezoidal::process_event(PHCompositeNode *topNode)
{
  // std::cout << "InttZVertexFinderTrapezoidal::process_event(PHCompositeNode *topNode) Processing Event" << std::endl;
  
  temp_sPH_inner_nocolumn_vec.clear();
  temp_sPH_outer_nocolumn_vec.clear();
  evt_possible_z -> Reset("ICESM");
  line_breakdown_hist -> Reset("ICESM");

  INTTvtxZ = std::nan("");
  INTTvtxZError = std::nan("");
  NgroupTrapezoidal = std::nan("");
  NgroupCoarse = std::nan("");
  TrapezoidalFitWidth = std::nan("");
  TrapezoidalFWHM = std::nan("");
  NClusAll = 0;
  NClusAllInner = 0;

  inner_clu_phi_map.clear();
  outer_clu_phi_map.clear();
  inner_clu_phi_map = std::vector<std::vector<std::pair<bool,clu_info>>>(360);
  outer_clu_phi_map = std::vector<std::vector<std::pair<bool,clu_info>>>(360);

  fit_mean_mean_vec.clear();
  fit_mean_reducedChi2_vec.clear();
  fit_mean_width_vec.clear();

  PrepareClusterVec(topNode);

  if (temp_sPH_inner_nocolumn_vec.size() < 1 || temp_sPH_outer_nocolumn_vec.size() < 1) {return Fun4AllReturnCodes::EVENT_OK;}

  PrepareINTTvtxZ();

  inttzobj->INTTvtxZ = INTTvtxZ;
  inttzobj->INTTvtxZError = INTTvtxZError;
  inttzobj->NgroupTrapezoidal = NgroupTrapezoidal;
  inttzobj->NgroupCoarse = NgroupCoarse;
  inttzobj->TrapezoidalFitWidth = TrapezoidalFitWidth;
  inttzobj->TrapezoidalFWHM = TrapezoidalFWHM; 
  inttzobj->NClusAll = NClusAll;
  inttzobj->NClusAllInner = NClusAllInner;

  // note : print everything 
  if (PrintRecoDetails){
    std::cout<<"NClusGood: "<<temp_sPH_inner_nocolumn_vec.size() + temp_sPH_outer_nocolumn_vec.size()<<", INTTvtxZ : "<<INTTvtxZ<<" INTTvtxZError : "<<INTTvtxZError<<" NgroupTrapezoidal : "<<NgroupTrapezoidal<<" NgroupCoarse : "<<NgroupCoarse<<" TrapezoidalFitWidth : "<<TrapezoidalFitWidth<<" TrapezoidalFWHM : "<<TrapezoidalFWHM<<std::endl;
  }
  
  
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int InttZVertexFinderTrapezoidal::ResetEvent(PHCompositeNode *topNode)
{
  // std::cout << "InttZVertexFinderTrapezoidal::ResetEvent(PHCompositeNode *topNode) Resetting internal structures, prepare for next event" << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int InttZVertexFinderTrapezoidal::EndRun(const int runnumber)
{
  std::cout << "InttZVertexFinderTrapezoidal::EndRun(const int runnumber) Ending Run for Run " << runnumber << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int InttZVertexFinderTrapezoidal::End(PHCompositeNode *topNode)
{
  std::cout << "InttZVertexFinderTrapezoidal::End(PHCompositeNode *topNode) This is the End..." << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int InttZVertexFinderTrapezoidal::Reset(PHCompositeNode *topNode)
{
 std::cout << "InttZVertexFinderTrapezoidal::Reset(PHCompositeNode *topNode) being Reset" << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
void InttZVertexFinderTrapezoidal::Print(const std::string &what) const
{
  std::cout << "InttZVertexFinderTrapezoidal::Print(const std::string &what) const Printing info for " << what << std::endl;
}


int InttZVertexFinderTrapezoidal::createNodes(PHCompositeNode* topNode)
{
  PHNodeIterator iter(topNode);
  PHCompositeNode* dstNode = dynamic_cast<PHCompositeNode*>(iter.findFirst("PHCompositeNode", "DST"));
  if (!dstNode)
  {
    std::cout << PHWHERE << "DST Node missing doing nothing" << std::endl;
    return Fun4AllReturnCodes::ABORTEVENT;
  }

  // ---                                                                                                                                                     
  PHCompositeNode* inttNode = dynamic_cast<PHCompositeNode*>(iter.findFirst("PHCompositeNode", "INTT"));
  if (!inttNode)
  {
    inttNode = new PHCompositeNode("INTT");
    dstNode->addNode(inttNode);
  }

  // m_inttvertexmap = findNode::getClass<InttVertexMap>(inttNode, "InttVertexMap");                                                                         
  if (true)
  {
    inttzobj = new INTTvtxZF4AObj();
    PHIODataNode<PHObject>* inttzobjNode = new PHIODataNode<PHObject>(inttzobj, "inttzobj", "PHObject");
    inttNode->addNode(inttzobjNode);
  }

  return Fun4AllReturnCodes::EVENT_OK;
}