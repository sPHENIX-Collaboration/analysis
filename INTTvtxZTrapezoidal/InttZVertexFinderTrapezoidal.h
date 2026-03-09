// Tell emacs that this is a C++ source
//  -*- C++ -*-.
#ifndef INTTZVERTEXFINDERTRAPEZOIDAL_H
#define INTTZVERTEXFINDERTRAPEZOIDAL_H

#include <fun4all/SubsysReco.h>

#include <fun4all/Fun4AllReturnCodes.h>
#include <trackbase/ActsGeometry.h>
#include <trackbase/InttDefs.h>
#include <trackbase/TrkrCluster.h>
#include <trackbase/TrkrClusterContainer.h>
#include <trackbase/TrkrHit.h>
#include <trackbase/TrkrHitSet.h>
#include <trackbase/TrkrHitSetContainer.h>

#include <phool/PHCompositeNode.h>
#include <phool/getClass.h>

#include <fun4all/Fun4AllReturnCodes.h>

#include <string>
#include <cmath>
#include <cstdlib>
#include <filesystem>
#include <iterator>
#include <algorithm>


#include <TH1.h>
#include <TF1.h>
#include <TMath.h>
#include <TColor.h>

#include "INTTvtxZF4AObj.h"

class PHCompositeNode;
class INTTvtxZF4AObj;


class InttZVertexFinderTrapezoidal : public SubsysReco
{
 public:

  InttZVertexFinderTrapezoidal(
    const std::string &name = "InttZVertexFinderTrapezoidal",
    std::pair<double, double> vertexXYIncm_in = {0, 0}, // note : in cm
    bool IsFieldOn_in = false,
    bool IsDCACutApplied_in = true,
    std::pair<std::pair<double,double>,std::pair<double,double>> DeltaPhiCutInDegree_in = {{-1,1},{-1000.,1000.}}, // note : in degree
    std::pair<std::pair<double,double>,std::pair<double,double>> DCAcutIncm_in = {{-1,1},{-1000.,1000.}}, // note : in cm
    int ClusAdcCut_in = 35,
    int ClusPhiSizeCut_in = 8,
    bool PrintRecoDetails_in = false
  );

  ~InttZVertexFinderTrapezoidal() override;

  /** Called during initialization.
      Typically this is where you can book histograms, and e.g.
      register them to Fun4AllServer (so they can be output to file
      using Fun4AllServer::dumpHistos() method).
   */
  int Init(PHCompositeNode *topNode) override;

  /** Called for first event when run number is known.
      Typically this is where you may want to fetch data from
      database, because you know the run number. A place
      to book histograms which have to know the run number.
   */
  int InitRun(PHCompositeNode *topNode) override;

  /** Called for each event.
      This is where you do the real work.
   */
  int process_event(PHCompositeNode *topNode) override;

  /// Clean up internals after each event.
  int ResetEvent(PHCompositeNode *topNode) override;

  /// Called at the end of each run.
  int EndRun(const int runnumber) override;

  /// Called at the end of all processing.
  int End(PHCompositeNode *topNode) override;

  /// Reset
  int Reset(PHCompositeNode * /*topNode*/) override;

  void Print(const std::string &what = "ALL") const override;

 private:
    std::pair<double, double> vertexXYIncm;
    bool IsFieldOn;
    bool IsDCACutApplied;
    std::pair<std::pair<double,double>,std::pair<double,double>> DeltaPhiCutInDegree; // note : this is prepared for the field on also
    std::pair<std::pair<double,double>,std::pair<double,double>> DCAcutIncm; // note : this is prepared for the field on also
    int ClusAdcCut;
    int ClusPhiSizeCut;
    bool PrintRecoDetails;

    // note : prepare clsuter vec
    int PrepareClusterVec(PHCompositeNode *topNode);
    int PrepareINTTvtxZ();

    // note : the private member functions
    double get_radius(double x, double y);
    double get_delta_phi(double angle_1, double angle_2);
    double calculateAngleBetweenVectors(double x1, double y1, double x2, double y2, double targetX, double targetY);
    double Get_extrapolation(double given_y, double p0x, double p0y, double p1x, double p1y);
    std::pair<double,double> Get_possible_zvtx(double rvtx, std::vector<double> p0, std::vector<double> p1);
    void line_breakdown(TH1D* hist_in, std::pair<double,double> line_range);
    void trapezoidal_line_breakdown(TH1D * hist_in, double inner_r, double inner_z, int inner_zid, double outer_r, double outer_z, int outer_zid);
    std::vector<double> find_Ngroup(TH1D * hist_in);
    double vector_average (std::vector <double> input_vector);
    double vector_stddev (std::vector <double> input_vector);

    // note : create node tree
    int createNodes(PHCompositeNode *topNode);
    INTTvtxZF4AObj * inttzobj;

    // note : the cluster structure 
    struct clu_info{
        double x;
        double y;
        double z;

        int adc;
        int phi_size;
        int sensorZID;
    };

    static double gaus_func(double *x, double *par)
    {
        // note : par[0] : size
        // note : par[1] : mean
        // note : par[2] : width
        // note : par[3] : offset 
        return par[0] * TMath::Gaus(x[0],par[1],par[2]) + par[3];
    };


    // note : objects for the vertex Z reco
    TH1D * evt_possible_z;
    TH1D * line_breakdown_hist;
    TH1D * combination_zvtx_range_shape;
    std::vector<clu_info> temp_sPH_inner_nocolumn_vec;
    std::vector<clu_info> temp_sPH_outer_nocolumn_vec;
    std::vector<std::vector<std::pair<bool,clu_info>>> inner_clu_phi_map; // note: phi
    std::vector<std::vector<std::pair<bool,clu_info>>> outer_clu_phi_map; // note: phi
    // std::pair<double, double> m_vertexXYInmm;
    // std::pair<std::pair<double,double>,std::pair<double,double>> m_DCAcutInmm;
    
    TF1 * gaus_fit; // note : cm
    std::vector<TF1 *> gaus_fit_vec; // note : cm
    std::vector<double> fit_mean_mean_vec;
    std::vector<double> fit_mean_reducedChi2_vec;
    std::vector<double> fit_mean_width_vec;
    

    // note : for the INTT vertex Z out
    // note : unit [cm]
    double INTTvtxZ;
    double INTTvtxZError;
    double NgroupTrapezoidal;
    double NgroupCoarse;
    double TrapezoidalFitWidth;
    double TrapezoidalFWHM;
    long long NClusAll;
    long long NClusAllInner;


    // note: some constants
    const int zvtx_hist_Nbins = 1200;   // note : N bins for each side, regardless the bin at zero 
    const double fine_bin_width = 0.05;  // note : bin width with the unit [cm]
    const std::pair<double,double> evt_possible_z_range = {-70, 70}; // note : [cm]
    const std::pair<double,double> edge_rejection = {-50, 50}; // note : [cm]
    const double typeA_sensor_half_length_incm = 0.8; // note : [cm]
    const double typeB_sensor_half_length_incm = 1.0; // note : [cm] 
    const std::vector<int> typeA_sensorZID = {0,2}; // note : sensor Z ID for type A // note -> 1, 0, 2, 3
    const std::vector<std::string> color_code = {
        "#9e0142",
        "#d53e4f",
        "#f46d43",
        "#fdae61",
        "#fee08b",
        "#e6f598",
        "#abdda4",
        "#66c2a5",
        "#3288bd",
        "#5e4fa2" 
    };
    
    // note : constants for the INTT vtxZ
    const int number_of_gaus = 7;

};

#endif // INTTZVERTEXFINDERTRAPEZOIDAL_H
