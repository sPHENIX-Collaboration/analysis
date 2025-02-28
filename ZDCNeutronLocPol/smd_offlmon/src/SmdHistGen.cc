#include "SmdHistGen.h"

#include <fun4all/Fun4AllReturnCodes.h>

#include <phool/PHCompositeNode.h>
#include <phool/PHIODataNode.h>    // for PHIODataNode
#include <phool/PHNode.h>          // for PHNode
#include <phool/PHNodeIterator.h>  // for PHNodeIterator
#include <phool/PHObject.h>        // for PHObject
#include <phool/getClass.h>

//to analyse PRDF
#include <caloreco/CaloWaveformFitting.h>
#include <ffarawobjects/CaloPacketContainerv1.h>
#include <ffarawobjects/CaloPacket.h>
/* #include <Event/Event.h> */
/* #include <Event/EventTypes.h> */
/* #include <Event/packet.h> */

//to analyse DST
#include <calobase/TowerInfoContainer.h>
#include <calobase/TowerInfo.h>
#include <ffaobjects/RunHeaderv1.h>
#include <ffarawobjects/Gl1Packetv1.h>

//spin database stuff
#include <uspin/SpinDBContent.h>
#include <uspin/SpinDBOutput.h>

//ROOT stuff
#include <TFile.h>
#include <TH1.h>
#include <TH2.h>
/* #include <TGraphErrors.h> */
#include <TString.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <TStyle.h>

#include <algorithm>

//____________________________________________________________________________..
SmdHistGen::SmdHistGen(const std::string& name, const std::string& which_mode, const char* outname):
  SubsysReco(name),
  mode(which_mode),
  outfilename(outname),
  outfile(nullptr)
{
  std::cout << "SmdHistGen::SmdHistGen(const std::string &name) Calling ctor" << std::endl;
}

//____________________________________________________________________________..
SmdHistGen::~SmdHistGen()
{
  std::cout << "SmdHistGen::~SmdHistGen() Calling dtor" << std::endl;
}

//____________________________________________________________________________..
int SmdHistGen::Init(PHCompositeNode *topNode)
{
  std::cout << "SmdHistGen::Init(PHCompositeNode *topNode) Initializing" << std::endl;

  // Create output file
  outfile = new TFile(outfilename, "RECREATE");
  outfile->cd();

  // Create hitograms

  // hits per channel and hit multiply
  smd_hor_north_total_hits = new TH1F("smd_hor_north_total_hits", "Total Hits by Channel, SMD North Horizontal;Channel;Counts", 8, -0.5, 7.5);
  smd_ver_north_total_hits = new TH1F("smd_ver_north_total_hits", "Total Hits by Channel, SMD North Vertical;Channel;Counts", 7, -0.5, 6.5);
  smd_hor_north_neutron_hits = new TH1F("smd_hor_north_neutron_hits", "Neutron Hits by Channel, SMD North Horizontal;Channel;Counts", 8, -0.5, 7.5);
  smd_ver_north_neutron_hits = new TH1F("smd_ver_north_neutron_hits", "Neutron Hits by Channel, SMD North Vertical;Channel;Counts", 7, -0.5, 6.5);
  smd_hor_north_multiplicity = new TH1F("smd_hor_north_multiplicity", Form("In-Time Hit Multiplicity, ADC>%d, SMD North Horizontal;# Hits;Counts", minSMDcut), 9, -0.5, 8.5);
  smd_ver_north_multiplicity = new TH1F("smd_ver_north_multiplicity", Form("In-Time Hit Multiplicity, ADC>%d, SMD North Vertical;# Hits;Counts", minSMDcut), 8, -0.5, 7.5);
  smd_hor_south_total_hits = new TH1F("smd_hor_south_total_hits", "Total Hits by Channel, SMD South Horizontal;Channel;Counts", 8, -0.5, 7.5);
  smd_ver_south_total_hits = new TH1F("smd_ver_south_total_hits", "Total Hits by Channel, SMD South Vertical;Channel;Counts", 7, -0.5, 6.5);
  smd_hor_south_neutron_hits = new TH1F("smd_hor_south_neutron_hits", "Neutron Hits by Channel, SMD South Horizontal;Channel;Counts", 8, -0.5, 7.5);
  smd_ver_south_neutron_hits = new TH1F("smd_ver_south_neutron_hits", "Neutron Hits by Channel, SMD South Vertical;Channel;Counts", 7, -0.5, 6.5);
  smd_hor_south_multiplicity = new TH1F("smd_hor_south_multiplicity", Form("In-Time Hit Multiplicity, ADC>%d, SMD South Horizontal;# Hits;Counts", minSMDcut), 9, -0.5, 8.5);
  smd_ver_south_multiplicity = new TH1F("smd_ver_south_multiplicity", Form("In-Time Hit Multiplicity, ADC>%d, SMD South Vertical;# Hits;Counts", minSMDcut), 8, -0.5, 7.5);

  // signals and waveforms
  float signal_max = 5000;
  smd_north_signal = new TH1F("smd_north_signal", "North SMD Peak ADC, All Channels;ADC;Counts", 1000, 0.0, signal_max);
  smd_south_signal = new TH1F("smd_south_signal", "South SMD Peak ADC, All Channels;ADC;Counts", 1000, 0.0, signal_max);
  for (int i=0; i<15; i++)
  {
    smd_north_signals[i] = new TH1F(Form("smd_north_signal_%d", i), Form("North SMD Channel %d", i), 1000, 0.0, signal_max);
    smd_north_channel_waveforms[i] = new TH2F(Form("smd_north_waveforms_%d", i), Form("North SMD Waveforms Channel %d", i), 128, -0.5, 16.5, 256, 0.0, signal_max);
    smd_south_signals[i] = new TH1F(Form("smd_south_signal_%d", i), Form("South SMD Channel %d", i), 1000, 0.0, signal_max);
    smd_south_channel_waveforms[i] = new TH2F(Form("smd_south_waveforms_%d", i), Form("South SMD Waveforms Channel %d", i), 128, -0.5, 16.5, 256, 0.0, signal_max);
  }
  smd_sum_hor_north = new TH1F ("smd_sum_hor_north", "SMD North y", 512, 0, 2048);
  smd_sum_ver_north = new TH1F ("smd_sum_ver_north", "SMD North x", 512, 0, 2048);
  smd_sum_hor_south = new TH1F ("smd_sum_hor_south", "SMD South y", 640, 0, 2560);
  smd_sum_ver_south = new TH1F ("smd_sum_ver_south", "SMD South x", 640, 0, 2560);
  smd_north_waveforms = new TH2F("smd_north_waveforms", "North SMD Waveform;Time;Max ADC", 128, -0.5, 16.5, 256, 0.0, signal_max);
  smd_north_waveforms_zoomed = new TH2F("smd_north_waveforms_zoomed", "North SMD Waveform;Time;Max ADC", 128, -0.5, 16.5, 250, -0.5, 249.5);
  smd_north_waveforms_raw = new TH2F("smd_north_waveforms_raw", "North SMD Waveform;Time;Max ADC", 17, -0.5, 16.5, 1000, 0.0, signal_max);
  smd_south_waveforms = new TH2F("smd_south_waveforms", "South SMD Waveform;Time;Max ADC", 128, -0.5, 16.5, 256, 0.0, signal_max);
  smd_south_waveforms_zoomed = new TH2F("smd_south_waveforms_zoomed", "South SMD Waveform;Time;Max ADC", 128, -0.5, 16.5, 250, -0.5, 249.5);
  smd_south_waveforms_raw = new TH2F("smd_south_waveforms_raw", "South SMD Waveform;Time;Max ADC", 17, -0.5, 16.5, 1000, 0.0, signal_max);
  smd_north_pedestal = new TH1F("smd_north_pedestal", "North SMD Pedestal;ADC;Counts", 1000, 0.0, signal_max);
  smd_south_pedestal = new TH1F("smd_south_pedestal", "South SMD Pedestal;ADC;Counts", 1000, 0.0, signal_max);
  zdc1_north = new TH1F("zdc1_north", "North ZDC1 Signal;ADC;Counts", 256, 0, signal_max);
  zdc2_north = new TH1F("zdc2_north", "North ZDC2 Signal;ADC;Counts", 256, 0, signal_max);
  zdc_north_waveforms = new TH2F("zdc_north_waveforms", "North ZDC Waveform;Time;Max ADC", 128, -0.5, 16.5, 256, 0.0, signal_max);
  zdc1_south = new TH1F("zdc1_south", "South ZDC1 Signal;ADC;Counts", 256, 0, signal_max);
  zdc2_south = new TH1F("zdc2_south", "South ZDC2 Signal;ADC;Counts", 256, 0, signal_max);
  zdc_south_waveforms = new TH2F("zdc_south_waveforms", "South ZDC Waveform;Time;Max ADC", 128, -0.5, 16.5, 256, 0.0, signal_max);
  vetofront_north = new TH1F("vetofront_north", "North Front Veto Signal;ADC;Counts", 256, 0, signal_max);
  vetoback_north = new TH1F("vetoback_north", "North Back Veto Signal;ADC;Counts", 256, 0, signal_max);
  veto_north_waveforms = new TH2F("veto_north_waveforms", "North Veto Waveform;Time;Max ADC", 128, -0.5, 16.5, 256, 0.0, signal_max);
  vetofront_south = new TH1F("vetofront_south", "South Front Veto Signal;ADC;Counts", 256, 0, signal_max);
  vetoback_south = new TH1F("vetoback_south", "South Back Veto Signal;ADC;Counts", 256, 0, signal_max);
  veto_south_waveforms = new TH2F("veto_south_waveforms", "South Veto Waveform;Time;Max ADC", 128, -0.5, 16.5, 256, 0.0, signal_max);

  // xy distributions
  int nbins_xy = 50;
  smd_xy_all_north = new TH2F("smd_xy_all_north", "SMD hit position north, all good hits;x;y", 110, -5.5, 5.5, 119, -5.92, 5.92);
  smd_xy_all_corrected_north = new TH2F("smd_xy_all_corrected_north", "Center-Corrected SMD hit position north, all good hits;x;y", 110, -5.5, 5.5, 119, -5.92, 5.92);
  smd_xy_neutron_north = new TH2F("smd_xy_neutron_north", "SMD hit position north, neutron hits only;x;y", 110, -5.5, 5.5, 119, -5.92, 5.92);
  smd_xy_neutron_corrected_north = new TH2F("smd_xy_neutron_corrected_north", "Center-Corrected SMD hit position north, neutron hits only;x;y", 110, -5.5, 5.5, 119, -5.92, 5.92);
  smd_xy_all_south = new TH2F("smd_xy_all_south", "SMD hit position south, all good hits;x;y", 110, -5.5, 5.5, 119, -5.92, 5.92);
  smd_xy_all_corrected_south = new TH2F("smd_xy_all_corrected_south", "Center-Corrected SMD hit position south, all good hits;x;y", 110, -5.5, 5.5, 119, -5.92, 5.92);
  smd_xy_neutron_south = new TH2F("smd_xy_neutron_south", "SMD hit position south, neutron hits only;x;y", 110, -5.5, 5.5, 119, -5.92, 5.92);
  smd_xy_neutron_corrected_south = new TH2F("smd_xy_neutron_corrected_south", "Center-Corrected SMD hit position south, neutron hits only;x;y", 110, -5.5, 5.5, 119, -5.92, 5.92);

  // 1D spin dependent x and y
  smd_hor_north = new TH1F("smd_hor_north", "Neutron centroid distribution, SMD North y", nbins_xy, -5.92, 5.92);
  smd_ver_north = new TH1F("smd_ver_north", "Neutron centroid distribution, SMD North x", nbins_xy, -5.5, 5.5);
  smd_hor_north_up = new TH1F("smd_hor_north_up", "Neutron centroid distribution, SMD North y, Spin Up", nbins_xy, -5.92, 5.92);
  smd_ver_north_up = new TH1F("smd_ver_north_up", "Neutron centroid distribution, SMD North x, Spin Up", nbins_xy, -5.5, 5.5);
  smd_hor_north_down = new TH1F("smd_hor_north_down", "Neutron centroid distribution, SMD North y, Spin Down", nbins_xy, -5.92, 5.92);
  smd_ver_north_down = new TH1F("smd_ver_north_down", "Neutron centroid distribution, SMD North x, Spin Down", nbins_xy, -5.5, 5.5);
  smd_hor_south = new TH1F("smd_hor_south", "Neutron centroid distribution, SMD South y", nbins_xy, -5.92, 5.92);
  smd_ver_south = new TH1F("smd_ver_south", "Neutron centroid distribution, SMD South x", nbins_xy, -5.5, 5.5);
  smd_hor_south_up = new TH1F("smd_hor_south_up", "Neutron centroid distribution, SMD South y, Spin Up", nbins_xy, -5.92, 5.92);
  smd_ver_south_up = new TH1F("smd_ver_south_up", "Neutron centroid distribution, SMD South x, Spin Up", nbins_xy, -5.5, 5.5);
  smd_hor_south_down = new TH1F("smd_hor_south_down", "Neutron centroid distribution, SMD South y, Spin Down", nbins_xy, -5.92, 5.92);
  smd_ver_south_down = new TH1F("smd_ver_south_down", "Neutron centroid distribution, SMD South x, Spin Down", nbins_xy, -5.5, 5.5);

  // beam center correction
  smd_hor_north_corrected = new TH1F("smd_hor_north_corrected", "Center-Corrected Neutron centroid distribution, SMD North y", nbins_xy, -5.92, 5.92);
  smd_ver_north_corrected = new TH1F("smd_ver_north_corrected", "Center-Corrected Neutron centroid distribution, SMD North x", nbins_xy, -5.5, 5.5);
  smd_hor_north_corrected_up = new TH1F("smd_hor_north_corrected_up", "Center-Corrected Neutron centroid distribution, SMD North y, Spin Up", nbins_xy, -5.92, 5.92);
  smd_ver_north_corrected_up = new TH1F("smd_ver_north_corrected_up", "Center-Corrected Neutron centroid distribution, SMD North x, Spin Up", nbins_xy, -5.5, 5.5);
  smd_hor_north_corrected_down = new TH1F("smd_hor_north_corrected_down", "Center-Corrected Neutron centroid distribution, SMD North y, Spin Down", nbins_xy, -5.92, 5.92);
  smd_ver_north_corrected_down = new TH1F("smd_ver_north_corrected_down", "Center-Corrected Neutron centroid distribution, SMD North x, Spin Down", nbins_xy, -5.5, 5.5);
  smd_hor_south_corrected = new TH1F("smd_hor_south_corrected", "Center-Corrected Neutron centroid distribution, SMD South y", nbins_xy, -5.92, 5.92);
  smd_ver_south_corrected = new TH1F("smd_ver_south_corrected", "Center-Corrected Neutron centroid distribution, SMD South x", nbins_xy, -5.5, 5.5);
  smd_hor_south_corrected_up = new TH1F("smd_hor_south_corrected_up", "Center-Corrected Neutron centroid distribution, SMD South y, Spin Up", nbins_xy, -5.92, 5.92);
  smd_ver_south_corrected_up = new TH1F("smd_ver_south_corrected_up", "Center-Corrected Neutron centroid distribution, SMD South x, Spin Up", nbins_xy, -5.5, 5.5);
  smd_hor_south_corrected_down = new TH1F("smd_hor_south_corrected_down", "Center-Corrected Neutron centroid distribution, SMD South y, Spin Down", nbins_xy, -5.92, 5.92);
  smd_ver_south_corrected_down = new TH1F("smd_ver_south_corrected_down", "Center-Corrected Neutron centroid distribution, SMD South x, Spin Down", nbins_xy, -5.5, 5.5);

  // phi distributions for asymmetry
  smd_north_phi_up = new TH1F("smd_north_phi_up", "Spin-up #phi distribution, SMD North;#phi;Counts", 8, -PI, PI);
  smd_north_phi_down = new TH1F("smd_north_phi_down", "Spin-down #phi distribution, SMD North;#phi;Counts", 8, -PI, PI);
  smd_north_phi_sum = new TH1F("smd_north_phi_sum", "Up+Down #phi distribution, SMD North;#phi;Counts", 8, -PI, PI);
  smd_north_phi_diff = new TH1F("smd_north_phi_diff", "Up-Down #phi distribution, SMD North;#phi;Counts", 8, -PI, PI);
  smd_north_phi_L_up = new TH1F("smd_north_phi_L_up", "Spin-up #phi distribution, SMD North;#phi;Counts", 8, -PI, 0.0);
  smd_north_phi_L_down = new TH1F("smd_north_phi_L_down", "Spin-down #phi distribution, SMD North;#phi;Counts", 8, -PI, 0.0);
  smd_north_phi_R_up = new TH1F("smd_north_phi_R_up", "Spin-up #phi distribution, SMD North;#phi;Counts", 8, 0.0, PI);
  smd_north_phi_R_down = new TH1F("smd_north_phi_R_down", "Spin-down #phi distribution, SMD North;#phi;Counts", 8, 0.0, PI);
  smd_south_phi_up = new TH1F("smd_south_phi_up", "Spin-up #phi distribution, SMD South;#phi;Counts", 8, -PI, PI);
  smd_south_phi_down = new TH1F("smd_south_phi_down", "Spin-down #phi distribution, SMD South;#phi;Counts", 8, -PI, PI);
  smd_south_phi_sum = new TH1F("smd_south_phi_sum", "Up+Down #phi distribution, SMD South;#phi;Counts", 8, -PI, PI);
  smd_south_phi_diff = new TH1F("smd_south_phi_diff", "Up-Down #phi distribution, SMD South;#phi;Counts", 8, -PI, PI);
  smd_south_phi_L_up = new TH1F("smd_south_phi_L_up", "Spin-up #phi distribution, SMD South;#phi;Counts", 8, -PI, 0.0);
  smd_south_phi_L_down = new TH1F("smd_south_phi_L_down", "Spin-down #phi distribution, SMD South;#phi;Counts", 8, -PI, 0.0);
  smd_south_phi_R_up = new TH1F("smd_south_phi_R_up", "Spin-up #phi distribution, SMD South;#phi;Counts", 8, 0.0, PI);
  smd_south_phi_R_down = new TH1F("smd_south_phi_R_down", "Spin-down #phi distribution, SMD South;#phi;Counts", 8, 0.0, PI);

  // beam center correction
  smd_north_phi_up_corrected = new TH1F("smd_north_phi_up_corrected", "Center-Corrected Spin-up #phi distribution, SMD North;#phi;Counts", 8, -PI, PI);
  smd_north_phi_down_corrected = new TH1F("smd_north_phi_down_corrected", "Center-Corrected Spin-down #phi distribution, SMD North;#phi;Counts", 8, -PI, PI);
  smd_north_phi_sum_corrected = new TH1F("smd_north_phi_sum_corrected", "Center-Corrected Up+Down #phi distribution, SMD North;#phi;Counts", 8, -PI, PI);
  smd_north_phi_diff_corrected = new TH1F("smd_north_phi_diff_corrected", "Center-Corrected Up-Down #phi distribution, SMD North;#phi;Counts", 8, -PI, PI);
  smd_north_phi_L_up_corrected = new TH1F("smd_north_phi_L_up_corrected", "Center-Corrected Spin-up #phi distribution, SMD North;#phi;Counts", 8, -PI, 0.0);
  smd_north_phi_L_down_corrected = new TH1F("smd_north_phi_L_down_corrected", "Center-Corrected Spin-down #phi distribution, SMD North;#phi;Counts", 8, -PI, 0.0);
  smd_north_phi_R_up_corrected = new TH1F("smd_north_phi_R_up_corrected", "Center-Corrected Spin-up #phi distribution, SMD North;#phi;Counts", 8, 0.0, PI);
  smd_north_phi_R_down_corrected = new TH1F("smd_north_phi_R_down_corrected", "Center-Corrected Spin-down #phi distribution, SMD North;#phi;Counts", 8, 0.0, PI);
  smd_south_phi_up_corrected = new TH1F("smd_south_phi_up_corrected", "Center-Corrected Spin-up #phi distribution, SMD South;#phi;Counts", 8, -PI, PI);
  smd_south_phi_down_corrected = new TH1F("smd_south_phi_down_corrected", "Center-Corrected Spin-down #phi distribution, SMD South;#phi;Counts", 8, -PI, PI);
  smd_south_phi_sum_corrected = new TH1F("smd_south_phi_sum_corrected", "Center-Corrected Up+Down #phi distribution, SMD South;#phi;Counts", 8, -PI, PI);
  smd_south_phi_diff_corrected = new TH1F("smd_south_phi_diff_corrected", "Center-Corrected Up-Down #phi distribution, SMD South;#phi;Counts", 8, -PI, PI);
  smd_south_phi_L_up_corrected = new TH1F("smd_south_phi_L_up_corrected", "Center-Corrected Spin-up #phi distribution, SMD South;#phi;Counts", 8, -PI, 0.0);
  smd_south_phi_L_down_corrected = new TH1F("smd_south_phi_L_down_corrected", "Center-Corrected Spin-down #phi distribution, SMD South;#phi;Counts", 8, -PI, 0.0);
  smd_south_phi_R_up_corrected = new TH1F("smd_south_phi_R_up_corrected", "Center-Corrected Spin-up #phi distribution, SMD South;#phi;Counts", 8, 0.0, PI);
  smd_south_phi_R_down_corrected = new TH1F("smd_south_phi_R_down_corrected", "Center-Corrected Spin-down #phi distribution, SMD South;#phi;Counts", 8, 0.0, PI);

  // Set relative gain values to unity for now
  for (int i=0; i<16; i++) {
    smd_north_rgain[i] = 1.0;
    smd_south_rgain[i] = 1.0;
  }

  // The next block will be how we read the relative gain values from a 
  // calibrations file
  /*
  // read our calibrations from ZdcMonData.dat
  const char *zdccalib = getenv("ZDCCALIB");
  if (!zdccalib)
  {
    std::cout << "ZDCCALIB environment variable not set" << std::endl;
    exit(1);
  }

  //getting gains
  float col1, col2, col3;
  std::string gainfile = std::string(zdccalib) + "/" + "/ZdcCalib.pmtgain";
  std::ifstream gain_infile(gainfile);
  
  if (!gain_infile)
  {
    std::cout << gainfile << " could not be opened." ;
    exit(1);
  }

  for (int i = 0; i < 32; i++)
  {
    gain_infile >> col1 >> col2 >> col3;
    gain[i] = col1;
  }

  for (int i = 0; i < 16; i++)  // relative gains of SMD channels
  {
    smd_south_rgain[i] = gain[i];  // 0-7: y channels, 8-14: x channels, 15: analog sum
    smd_north_rgain[i] = gain[i + 16];  // same as above
  }

  gain_infile.close();
  */
  // done reading gains from file

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int SmdHistGen::InitRun(PHCompositeNode *topNode)
{
  std::cout << "SmdHistGen::InitRun(PHCompositeNode *topNode) Initializing for Run XXX" << std::endl;

  // Get run number
  runHeader = findNode::getClass<RunHeaderv1>(topNode, "RunHeader");
  if (!runHeader)
  {
    std::cout << PHWHERE << ":: Missing RunHeader! Exiting!" << std::endl;
    exit(1);
  }
  runNum = runHeader->get_RunNumber();
  std::cout << "Run number is " << runNum << std::endl;

  // Get spin pattern info
  int spinDB_status = GetSpinPatterns();
  if (spinDB_status)
  {
    std::cout << PHWHERE << ":: Run number " << runNum << " not found in spin DB! Exiting!" << std::endl;
    /* exit(1); */
  }

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int SmdHistGen::process_event(PHCompositeNode *topNode)
{
  /* std::cout << "SmdHistGen::process_event(PHCompositeNode *topNode) Processing Event" << std::endl; */
  
  evtctr++;
  if (evtctr%10000 == 0) std::cout << "Event " << evtctr << std::endl;

  // Get the bunch number for this event
  Gl1Packetv1* gl1 = findNode::getClass<Gl1Packetv1>(topNode, "GL1Packet");
  if (gl1)
  {
    bunchNum = gl1->getBunchNumber();
    bunchNum = (bunchNum + crossingShift)%NBUNCHES;
    /* std::cout << "Got bunch number = " << bunchNum << std::endl; */
  }
  else
  {
    // for testing
    std::cout << "GL1 missing!" << std::endl;
    bunchNum = evtctr%4;
  }
  /* gl1event++; */
  /* std::cout << Form("evtctr=%d, gl1event=%d, smdevent=%d", evtctr, gl1event, smdevent) << std::endl; */
  
  // To account for the GL1-ZDC event # mismatch:
  // Compute the position and asymmetry-related parts *before* getting the new
  // ADC values for this event -- that way, the hit positions etc. are computed
  // based on the *previous* event's ADC values

  /* if (evtctr == 1) */
  /* { */
  /*   towerinfosZDC = findNode::getClass<TowerInfoContainer>(topNode, "TOWERS_ZDC"); */
  /*   packetsZDC = findNode::getClass<CaloPacketContainerv1>(topNode, "ZDCPackets"); */
  /*   GetAdcs(); */
  /*   return Fun4AllReturnCodes::EVENT_OK; */
  /* } */

  /*
  // Get the ADC values
  towerinfosZDC = findNode::getClass<TowerInfoContainer>(topNode, "TOWERS_ZDC");
  packetsZDC = findNode::getClass<CaloPacketContainerv1>(topNode, "ZDCPackets");
  GetAdcs();
  */

  // call the functions
  CompSmdAdc();
  CompSmdPos();
  /* std::cout << Form("Event %d start: smd_adc = {", evtctr); */
  /* for (int i=0; i<15; i++) { */
  /*     std::cout << smd_adc[i] << ", "; */
  /* } */
  /* std::cout << smd_adc[15] << "}" << std::endl; */
  /* std::cout << Form("Event %d start: smd_time = {", evtctr); */
  /* for (int i=0; i<15; i++) { */
  /*     std::cout << smd_time[i] << ", "; */
  /* } */
  /* std::cout << smd_time[15] << "}" << std::endl; */
  CompSmdPosCorr();
  CompSumSmd();
  CountSMDHits();
  if ((n_hor_numhits > minSMDhits) && (n_ver_numhits > minSMDhits))
  {
    smd_xy_all_north->Fill(smd_pos[1], smd_pos[0]);
    smd_xy_all_corrected_north->Fill(smd_pos_corr[1], smd_pos_corr[0]);
  }
  if ((s_hor_numhits > minSMDhits) && (s_ver_numhits > minSMDhits))
  {
    smd_xy_all_south->Fill(smd_pos[3], smd_pos[2]);
    smd_xy_all_corrected_south->Fill(smd_pos_corr[3], smd_pos_corr[2]);
  }
  bool n_neutron = NeutronSelection("north");
  bool s_neutron = NeutronSelection("south");

  // FILLING OUT THE HISTOGRAMS
  if (n_neutron)
  {
    smd_hor_north->Fill(smd_pos[0]);
    smd_ver_north->Fill(smd_pos[1]);
    smd_sum_ver_north->Fill(smd_sum[1]);
    smd_sum_hor_north->Fill(smd_sum[0]);
    smd_xy_neutron_north->Fill(smd_pos[1], smd_pos[0]);
    //std::cout<<" smd sum ver north = "<<smd_sum[1]<<std::endl;
    //std::cout<<" smd sum hor north = "<<smd_sum[0]<<std::endl;

    // SMD hits by channel
    for ( int i = 0; i < 8; i++)
    {
      if ( smd_adc[i] > minSMDcut ) {smd_hor_north_neutron_hits->Fill(i);} 
    }
    for ( int i = 0; i < 7; i++)
    {
      if ( smd_adc[i + 8] > minSMDcut ) {smd_ver_north_neutron_hits->Fill(i);} 
    }

    // Separate spin up and down centroid positions
    float north_x = smd_pos[1];
    float north_y = smd_pos[0];
    float north_phi = atan2(north_y, north_x);
    int blueSpin = spinPatternBlue[bunchNum];
    if (blueSpin == 1)
    {
      smd_hor_north_up->Fill(north_y);
      smd_ver_north_up->Fill(north_x);
      smd_north_phi_up->Fill(north_phi);
      smd_north_phi_L_up->Fill(north_phi);
      smd_north_phi_R_up->Fill(north_phi);
    }
    if (blueSpin == -1)
    {
      smd_hor_north_down->Fill(north_y);
      smd_ver_north_down->Fill(north_x);
      smd_north_phi_down->Fill(north_phi);
      smd_north_phi_L_down->Fill(north_phi);
      smd_north_phi_R_down->Fill(north_phi);
    }
  }

  bool n_neutron_corr = NeutronSelection("north", true);
  if (n_neutron_corr)
  {
    smd_hor_north_corrected->Fill(smd_pos_corr[0]);
    smd_ver_north_corrected->Fill(smd_pos_corr[1]);
    smd_xy_neutron_corrected_north->Fill(smd_pos_corr[1], smd_pos_corr[0]);

    // Separate spin up and down centroid positions
    float north_x_corr = smd_pos_corr[1];
    float north_y_corr = smd_pos_corr[0];
    float north_phi_corr = atan2(north_y_corr, north_x_corr);
    int blueSpin = spinPatternBlue[bunchNum];
    if (blueSpin == 1)
    {
      smd_hor_north_corrected_up->Fill(north_y_corr);
      smd_ver_north_corrected_up->Fill(north_x_corr);
      smd_north_phi_up_corrected->Fill(north_phi_corr);
      smd_north_phi_L_up_corrected->Fill(north_phi_corr);
      smd_north_phi_R_up_corrected->Fill(north_phi_corr);
    }
    if (blueSpin == -1)
    {
      smd_hor_north_corrected_down->Fill(north_y_corr);
      smd_ver_north_corrected_down->Fill(north_x_corr);
      smd_north_phi_down_corrected->Fill(north_phi_corr);
      smd_north_phi_L_down_corrected->Fill(north_phi_corr);
      smd_north_phi_R_down_corrected->Fill(north_phi_corr);
    }
  }

  if (s_neutron)
  {
    smd_hor_south->Fill(smd_pos[2]);
    smd_ver_south->Fill(smd_pos[3]);
    smd_sum_ver_south->Fill(smd_sum[3]);
    smd_sum_hor_south->Fill(smd_sum[2]);
    smd_xy_neutron_south->Fill(smd_pos[3], smd_pos[2]);
    //std::cout<<" smd sum hor south = "<<smd_sum[2]<<std::endl;
    //std::cout<<" smd sum ver south = "<<smd_sum[3]<<std::endl;

    // SMD hits by channel
    for ( int i = 0; i < 8; i++)
    {
      if ( smd_adc[i] > minSMDcut ) {smd_hor_south_neutron_hits->Fill(i);} 
    }
    for ( int i = 0; i < 7; i++)
    {
      if ( smd_adc[i + 8] > minSMDcut ) {smd_ver_south_neutron_hits->Fill(i);} 
    }

    // Separate spin up and down centroid positions
    float south_x = smd_pos[3];
    float south_y = smd_pos[2];
    float south_phi = atan2(south_y, south_x);
    int yellowSpin = spinPatternYellow[bunchNum];
    if (yellowSpin == 1)
    {
      smd_hor_south_up->Fill(south_y);
      smd_ver_south_up->Fill(south_x);
      smd_south_phi_up->Fill(south_phi);
      smd_south_phi_L_up->Fill(south_phi);
      smd_south_phi_R_up->Fill(south_phi);
    }
    if (yellowSpin == -1)
    {
      smd_hor_south_down->Fill(south_y);
      smd_ver_south_down->Fill(south_x);
      smd_south_phi_down->Fill(south_phi);
      smd_south_phi_L_down->Fill(south_phi);
      smd_south_phi_R_down->Fill(south_phi);
    }
  }

  bool s_neutron_corr = NeutronSelection("south", true);
  if (s_neutron_corr)
  {
    smd_hor_south_corrected->Fill(smd_pos_corr[2]);
    smd_ver_south_corrected->Fill(smd_pos_corr[3]);
    smd_xy_neutron_corrected_south->Fill(smd_pos_corr[3], smd_pos_corr[2]);
    //std::cout<<" smd sum hor south = "<<smd_sum[2]<<std::endl;
    //std::cout<<" smd sum ver south = "<<smd_sum[3]<<std::endl;

    // Separate spin up and down centroid positions
    float south_x_corr = smd_pos_corr[3];
    float south_y_corr = smd_pos_corr[2];
    float south_phi_corr = atan2(south_y_corr, south_x_corr);
    int yellowSpin = spinPatternYellow[bunchNum];
    if (yellowSpin == 1)
    {
      smd_hor_south_corrected_up->Fill(south_y_corr);
      smd_ver_south_corrected_up->Fill(south_x_corr);
      smd_south_phi_up_corrected->Fill(south_phi_corr);
      smd_south_phi_L_up_corrected->Fill(south_phi_corr);
      smd_south_phi_R_up_corrected->Fill(south_phi_corr);
    }
    if (yellowSpin == -1)
    {
      smd_hor_south_corrected_down->Fill(south_y_corr);
      smd_ver_south_corrected_down->Fill(south_x_corr);
      smd_south_phi_down_corrected->Fill(south_phi_corr);
      smd_south_phi_L_down_corrected->Fill(south_phi_corr);
      smd_south_phi_R_down_corrected->Fill(south_phi_corr);
    }
  }

  // Now update the ADC values for the next event
  towerinfosZDC = findNode::getClass<TowerInfoContainer>(topNode, "TOWERS_ZDC");
  packetsZDC = findNode::getClass<CaloPacketContainerv1>(topNode, "ZDCPackets");
  GetAdcs();
  /* smdevent++; */
  /* CompSmdPos(); */
  /* std::cout << Form("Event %d end: smd_adc = {", evtctr); */
  /* for (int i=0; i<15; i++) { */
  /*     std::cout << smd_adc[i] << ", "; */
  /* } */
  /* std::cout << smd_adc[15] << "}" << std::endl; */
  /* std::cout << Form("Event %d end: smd_time = {", evtctr); */
  /* for (int i=0; i<15; i++) { */
  /*     std::cout << smd_time[i] << ", "; */
  /* } */
  /* std::cout << smd_time[15] << "}" << std::endl; */
  /* Gl1Packetv1* gl1 = findNode::getClass<Gl1Packetv1>(topNode, "GL1Packet"); */
  /* if (gl1) */
  /* { */
  /*   bunchNum = gl1->getBunchNumber(); */
  /*   bunchNum = (bunchNum + crossingShift)%NBUNCHES; */
  /*   /1* std::cout << "Got bunch number = " << bunchNum << std::endl; *1/ */
  /* } */

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int SmdHistGen::ResetEvent(PHCompositeNode *topNode)
{
    /* std::cout << "SmdHistGen::ResetEvent(PHCompositeNode *topNode) Resetting internal structures, prepare for next event" << std::endl; */
    return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int SmdHistGen::EndRun(const int runnumber)
{
  std::cout << "SmdHistGen::EndRun(const int runnumber) Ending Run for Run " << runnumber << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int SmdHistGen::End(PHCompositeNode *topNode)
{
  std::cout << "SmdHistGen::End(PHCompositeNode *topNode) This is the End..." << std::endl;
  std::cout << "Processed " << evtctr << " total events" << std::endl;

  // Raw asymmetry
  smd_north_phi_up->Sumw2();
  smd_north_phi_down->Sumw2();
  smd_north_phi_sum->Add(smd_north_phi_up, smd_north_phi_down, 1.0, 1.0);
  smd_north_phi_diff->Add(smd_north_phi_up, smd_north_phi_down, 1.0, -1.0);
  smd_south_phi_up->Sumw2();
  smd_south_phi_down->Sumw2();
  smd_south_phi_sum->Add(smd_south_phi_up, smd_south_phi_down, 1.0, 1.0);
  smd_south_phi_diff->Add(smd_south_phi_up, smd_south_phi_down, 1.0, -1.0);
  smd_north_phi_up_corrected->Sumw2();
  smd_north_phi_down_corrected->Sumw2();
  smd_north_phi_sum_corrected->Add(smd_north_phi_up_corrected, smd_north_phi_down_corrected, 1.0, 1.0);
  smd_north_phi_diff_corrected->Add(smd_north_phi_up_corrected, smd_north_phi_down_corrected, 1.0, -1.0);
  smd_south_phi_up_corrected->Sumw2();
  smd_south_phi_down_corrected->Sumw2();
  smd_south_phi_sum_corrected->Add(smd_south_phi_up_corrected, smd_south_phi_down_corrected, 1.0, 1.0);
  smd_south_phi_diff_corrected->Add(smd_south_phi_up_corrected, smd_south_phi_down_corrected, 1.0, -1.0);

  // Plot some histograms
  /*
  gStyle->SetOptStat(0);
  TCanvas* c1 = new TCanvas("c1", "c1", 800, 800);
  c1->cd();
  smd_hor_north->SetLineColor(kBlack);
  smd_hor_north_up->SetLineColor(kBlue);
  smd_hor_north_down->SetLineColor(kRed);
  smd_hor_north->Draw();
  smd_hor_north_up->Draw("same");
  smd_hor_north_down->Draw("same");
  TLegend* leg = new TLegend(0.7, 0.7, 0.9, 0.9);
  leg->AddEntry(smd_hor_north, "Total");
  leg->AddEntry(smd_hor_north_up, "Spin up");
  leg->AddEntry(smd_hor_north_down, "Spin down");
  leg->Draw();
  c1->Update();
  c1->SaveAs("plots/smd_hor_north_42200.png");
  delete leg;

  smd_ver_north->SetLineColor(kBlack);
  smd_ver_north_up->SetLineColor(kBlue);
  smd_ver_north_down->SetLineColor(kRed);
  smd_ver_north->Draw();
  smd_ver_north_up->Draw("same");
  smd_ver_north_down->Draw("same");
  leg = new TLegend(0.7, 0.7, 0.9, 0.9);
  leg->AddEntry(smd_ver_north, "Total");
  leg->AddEntry(smd_ver_north_up, "Spin up");
  leg->AddEntry(smd_ver_north_down, "Spin down");
  leg->Draw();
  c1->Update();
  c1->SaveAs("plots/smd_ver_north_42200.png");
  delete leg;

  smd_hor_north_neutron_hits->Draw();
  c1->Update();
  c1->SaveAs("plots/smd_hor_neutron_hits_42200.png");
  smd_ver_north_neutron_hits->Draw();
  c1->Update();
  c1->SaveAs("plots/smd_ver_neutron_hits_42200.png");

  smd_north_waveforms->Draw("COLZ");
  gPad->SetLogz();
  c1->Update();
  c1->SaveAs("plots/smd_waveforms_42200.png");
  zdc_north_waveforms->Draw("COLZ");
  gPad->SetLogz();
  c1->Update();
  c1->SaveAs("plots/zdc_waveforms_42200.png");
  veto_north_waveforms->Draw("COLZ");
  gPad->SetLogz();
  c1->Update();
  c1->SaveAs("plots/veto_waveforms_42200.png");
  */

  outfile->cd();
  outfile->Write();
  outfile->Close();

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int SmdHistGen::Reset(PHCompositeNode *topNode)
{
  std::cout << "SmdHistGen::Reset(PHCompositeNode *topNode) being Reset" << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
void SmdHistGen::Print(const std::string &what) const
{
   std::cout << "SmdHistGen::Print(const std::string &what) const Printing info for " << what << std::endl;
}

int SmdHistGen::GetSpinPatterns()
{
  // Get the spin patterns from the spin DB

  //  0xffff is the qa_level from XingShiftCal //////
  unsigned int qa_level = 0xffff;
  SpinDBContent spin_cont;
  SpinDBOutput spin_out("phnxrc");
      
  spin_out.StoreDBContent(runNum,runNum,qa_level);
  spin_out.GetDBContentStore(spin_cont,runNum);
      
  // Get crossing shift
  crossingShift = spin_cont.GetCrossingShift();
  if (crossingShift == -999) crossingShift = 0;
  std::cout << "Crossing shift: " << crossingShift << std::endl;

  std::cout << "Blue spin pattern: [";
  for (int i = 0; i < 120; i++)
  {
    spinPatternBlue[i] = spin_cont.GetSpinPatternBlue(i);
    std::cout << spinPatternBlue[i];
    if (i < 119)std::cout << ", ";
  }
  std::cout << "]" << std::endl;

  std::cout << "Yellow spin pattern: [";
  for (int i = 0; i < 120; i++)
  {
    spinPatternYellow[i] = spin_cont.GetSpinPatternYellow(i);
    std::cout << spinPatternYellow[i];
    if (i < 119)std::cout << ", ";
  }
  std::cout << "]" << std::endl;

  if (spinPatternYellow[0] == -999) {return 1;}
  return 0;
}

void SmdHistGen::GetAdcs() // populate the adc arrays
{
  if (mode == "dst") {GetAdcsDst();}
  else if (mode == "raw") {GetAdcsRaw();}
  else
  {
    std::cout << PHWHERE << ":: invalid mode=" << mode << " ... Exiting!" << std::endl;
    exit(1);
  }
}

void SmdHistGen::GetAdcsDst()
{
  if(!towerinfosZDC)
  {
    std::cout << PHWHERE << ":: No TOWERS_ZDC!" << std::endl; exit(1);
  }

  int nchannels_zdc = towerinfosZDC->size();
  /* std::cout << "towerinfosZDC has " << nchannels_zdc << " channels" << std::endl; */
  for (int channel = 0; channel < nchannels_zdc;channel++)
  {
    // Channel mapping: ZDCS: 0-8; ZDCN: 9-15; SMDN: 16-31; SMDS: 32-47; Veto Counter N: 48 (front); Veto Counter N: 49 (back);  Veto Counter S: 50 (front); Veto Counter S: 51 (back)
    float zdc_e = towerinfosZDC->get_tower_at_channel(channel)->get_energy();
    float zdc_t = towerinfosZDC->get_tower_at_channel(channel)->get_time();

    if (channel < 16) // ZDC
    {
      // ZDC Mapping:
      // even high gain, odd low gain
      // 0,1 S1; 2,3 S2; 4,5 S3; 6,7 Ssum
      // 8,9 N1; 10,11 N2; 12,13 N3; 14,15 Nsum
      zdc_adc[channel] = zdc_e;
      zdc_time[channel] = zdc_t;
      if (channel == 0) zdc1_south->Fill(zdc_e);
      if (channel == 2) zdc2_south->Fill(zdc_e);
      if (channel==0 || channel==2 || channel==4) zdc_south_waveforms->Fill(zdc_t, zdc_e);
      if (channel == 8) zdc1_north->Fill(zdc_e);
      if (channel == 10) zdc2_north->Fill(zdc_e);
      if (channel==8 || channel==10 || channel==12) zdc_north_waveforms->Fill(zdc_t, zdc_e);
    }
    if (channel >= 16 && channel < 48) // SMD
    {
      // SMD Mapping:
      // 0-7 North H1-8; 8-14 North V1-7; 15 North sum
      // 16-23 South H1-8; 24-30 South V1-7; 31 South sum
      int smd_channel = channel - 16;
      if (smd_channel < 15)
      {
	smd_north_signals[smd_channel]->Fill(zdc_e);
	smd_north_waveforms->Fill(zdc_t, zdc_e);
	smd_north_waveforms_zoomed->Fill(zdc_t, zdc_e);
	smd_north_channel_waveforms[smd_channel]->Fill(zdc_t, zdc_e);
      }
      if (smd_channel > 15)
      {
	smd_south_signals[smd_channel-16]->Fill(zdc_e);
	smd_south_waveforms->Fill(zdc_t, zdc_e);
	smd_south_waveforms_zoomed->Fill(zdc_t, zdc_e);
	smd_south_channel_waveforms[smd_channel-16]->Fill(zdc_t, zdc_e);
      }
      // Set the ADC to 0 if we're below threshold or out of time
      if (zdc_e < minSMDcut) zdc_e = 0;
      if (smd_channel < 16)
      {
	if ((zdc_t < smd_north_t_low) || (zdc_t > smd_north_t_high)) zdc_e = 0;
      }
      else
      {
	if ((zdc_t < smd_south_t_low) || (zdc_t > smd_south_t_high)) zdc_e = 0;
      }
      smd_adc[smd_channel] = zdc_e;
      smd_time[smd_channel] = zdc_t;
    }
    if (channel >= 48 && channel < 52) // Veto
    {
      // Veto Mapping:
      // 0 N front; 1 N back; 2 S front; 3 S back
      int veto_channel = channel - 48;
      veto_adc[veto_channel] = zdc_e;
      veto_time[veto_channel] = zdc_t;
      if (veto_channel == 0) vetofront_north->Fill(zdc_e);
      if (veto_channel == 1) vetoback_north->Fill(zdc_e);
      if (veto_channel < 2) veto_north_waveforms->Fill(zdc_t, zdc_e);
    }
  }
}

void SmdHistGen::GetAdcsRaw()
{
  if(!packetsZDC)
  {
    std::cout << PHWHERE << ":: No ZDCPackets!" << std::endl; exit(1);
  }

  /* packetsZDC->identify(); */
  CaloPacket *packetZDC = packetsZDC->getPacketbyId(packet_smd);
  /* packetZDC->identify(); */
  
  int Nchannels = std::min(128, packetZDC->iValue(0, "CHANNELS"));
  for (int channel = 0; channel < Nchannels; channel++)
  {
    // Channel mapping: ZDCS: 0-8; ZDCN: 9-15; SMDN: 48-63; SMDS: 112-127; Veto Counter N: 16 (front); Veto Counter N: 17 (back);  Veto Counter S: 80 (front); Veto Counter S: 81 (back)
    std::vector<float> resultFast = anaWaveformFast(packetZDC, channel);  // fast waveform fitting
    float zdc_e = resultFast.at(0);
    float zdc_t = resultFast.at(1);
    float zdc_ped = resultFast.at(2);

    if (channel < 16) // ZDC
    {
      zdc_adc[channel] = zdc_e;
      zdc_time[channel] = zdc_t;
      if (channel == 8) zdc1_north->Fill(zdc_e);
      if (channel == 10) zdc2_north->Fill(zdc_e);
      if (channel==8 || channel==10 || channel==12) zdc_north_waveforms->Fill(zdc_t, zdc_e);
      continue;
    }

    // North Veto counters
    if (channel == 16)
    {
      veto_adc[0] = zdc_e;
      veto_time[0] = zdc_t;
      vetofront_north->Fill(zdc_e);
      veto_north_waveforms->Fill(zdc_t, zdc_e);
      continue;
    }
    if (channel == 17)
    {
      veto_adc[1] = zdc_e;
      veto_time[1] = zdc_t;
      vetoback_north->Fill(zdc_e);
      veto_north_waveforms->Fill(zdc_t, zdc_e);
      continue;
    }
    
    // North SMD
    if (channel >= 48 && channel < 63)
    {
      int smd_channel = channel - 48;
      if (smd_channel < 15)
      {
	smd_north_signal->Fill(zdc_e);
	smd_north_signals[smd_channel]->Fill(zdc_e);
	smd_north_waveforms->Fill(zdc_t, zdc_e);
	smd_north_waveforms_zoomed->Fill(zdc_t, zdc_e);
	smd_north_channel_waveforms[smd_channel]->Fill(zdc_t, zdc_e);
	smd_north_pedestal->Fill(zdc_ped);
      }
      // Set the ADC to 0 if we're below threshold or out of time
      if (zdc_e < minSMDcut) zdc_e = 0;
      if ((zdc_t < smd_north_t_low) || (zdc_t > smd_north_t_high)) zdc_e = 0;
      smd_adc[smd_channel] = zdc_e;
      smd_time[smd_channel] = zdc_t;
      continue;
    }
    
    // South Veto counters
    if (channel == 80)
    {
      veto_adc[2] = zdc_e;
      veto_time[2] = zdc_t;
      vetofront_south->Fill(zdc_e);
      veto_south_waveforms->Fill(zdc_t, zdc_e);
      continue;
    }
    if (channel == 81)
    {
      veto_adc[3] = zdc_e;
      veto_time[3] = zdc_t;
      vetoback_south->Fill(zdc_e);
      veto_south_waveforms->Fill(zdc_t, zdc_e);
      continue;
    }

    // South SMD
    if (channel >= 112 && channel < 127)
    {
      int smd_channel = channel - 112;
      if (smd_channel < 15)
      {
	smd_south_signal->Fill(zdc_e);
	smd_south_signals[smd_channel]->Fill(zdc_e);
	smd_south_waveforms->Fill(zdc_t, zdc_e);
	smd_south_waveforms_zoomed->Fill(zdc_t, zdc_e);
	smd_south_channel_waveforms[smd_channel]->Fill(zdc_t, zdc_e);
	smd_south_pedestal->Fill(zdc_ped);
      }
      // Set the ADC to 0 if we're below threshold or out of time
      if (zdc_e < minSMDcut) zdc_e = 0;
      if ((zdc_t < smd_south_t_low) || (zdc_t > smd_south_t_high)) zdc_e = 0;
      smd_adc[smd_channel+16] = zdc_e;
      smd_time[smd_channel+16] = zdc_t;
      continue;
    }
  }
}

void SmdHistGen::CompSmdAdc() // mulitplying by relative gains
{
  for (int i = 0; i < 15; i++) // last one is reserved for analogue sum
  {
    // multiply SMD channels with their gain factors
    // to get the absolute ADC values in the same units
    //rgains come from CompSmdAdc()
    smd_adc[i] = smd_adc[i] * smd_north_rgain[i]; // sout -> north for PHENIX -> sPHENIX
    smd_adc[i + 16] = smd_adc[i + 16] * smd_south_rgain[i]; // north -> south for PHENIX-> sPHENIX

    /* for (int i=0; i<32; i++) { */
    /*   std::cout << "smd_adc[" << i << "] = " << smd_adc[i] << ", smd_time[" << i << "] = " << smd_time[i] << std::endl; */
    /* } */
    /* std::cout << std::endl; */
  
  }
}

void SmdHistGen::CompSmdPos() //computing position with weighted averages
{
  float w_ave[4]; // 0 -> north hor; 1 -> noth vert; 2 -> south hor; 3 -> south vert.
  float weights[4] = {0};
  memset(weights, 0, sizeof(weights)); // memset float works only for 0
  float w_sum[4];
  memset(w_sum, 0, sizeof(w_sum));


  // these constants convert the SMD channel number into real dimensions (cm's)
  const float hor_scale = 2.0 * 11.0 / 10.5 * sin(PI/4); // from gsl_math.h
  const float ver_scale = 1.5 * 11.0 / 10.5;
  float hor_offset = (hor_scale * 8 / 2.0) * (7.0 / 8.0);
  float ver_offset = (ver_scale * 7 / 2.0) * (6.0 / 7.0);

  for (int i = 0; i < 8; i++)
  {
    weights[0] += smd_adc[i]; //summing weights
    weights[2] += smd_adc[i + 16];
    w_sum[0] += (float)i * smd_adc[i]; //summing for the average
    w_sum[2] += ((float)i + 16.) * smd_adc[i + 16];
  }
  for (int i = 0; i < 7; i++)
  {
    weights[1] += smd_adc[i + 8];
    weights[3] += smd_adc[i + 24];
    w_sum[1] += ((float)i + 8.) * smd_adc[i + 8];
    w_sum[3] += ((float)i + 24.) * smd_adc[i + 24];
  }

  if ( weights[0] > 0.0 )
  {
    w_ave[0] = w_sum[0] / weights[0]; //average = sum / sumn of weights...
    smd_pos[0] = hor_scale * w_ave[0] - hor_offset;
  }
  else
  {
    smd_pos[0] = 0;
  }
  if ( weights[1] > 0.0 )
  {
    w_ave[1] = w_sum[1] / weights[1];
    smd_pos[1] = ver_scale * (w_ave[1] - 8.0) - ver_offset;
  }
  else
  {
    smd_pos[1] = 0;
  }

  if ( weights[2] > 0.0 )
  {
    w_ave[2] = w_sum[2] / weights[2];
    smd_pos[2] = hor_scale * (w_ave[2] - 16.0) - hor_offset;
  }
  else
  {
    smd_pos[2] = 0;
  }

  if ( weights[3] > 0.0 )
  {
    w_ave[3] = w_sum[3] / weights[3];
    smd_pos[3] = ver_scale * (w_ave[3] - 24.0) - ver_offset;
  }
  else
  {
    smd_pos[3] = 0;
  }
}

void SmdHistGen::CompSmdPosCorr()
{
  /* float north_x_ctr = 0.351; */
  /* float north_y_ctr = 0.754; */
  /* float south_x_ctr = -0.334; */
  /* float south_y_ctr = -0.067; */
  float north_x_ctr = 0.407-0.031;
  float north_y_ctr = 0.790+0.149;
  float south_x_ctr = -0.393-0.176-0.062-0.019;
  float south_y_ctr = 0.082+0.037+0.014;

  smd_pos_corr[1] = smd_pos[1] - north_x_ctr;
  smd_pos_corr[0] = smd_pos[0] - north_y_ctr;
  smd_pos_corr[3] = smd_pos[3] - south_x_ctr;
  smd_pos_corr[2] = smd_pos[2] - south_y_ctr;
}

void SmdHistGen::CompSumSmd() //compute 'digital' sum
{
  memset(smd_sum, 0, sizeof(smd_sum));

  for (int i = 0; i < 8; i++)
  {
    smd_sum[0] += smd_adc[i]; // north horizontal
    smd_sum[2] += smd_adc[i + 16]; // south horizontal
  }
  for (int i = 0; i < 7; i++)
  {
    smd_sum[1] += smd_adc[i + 8]; // north vertical
    smd_sum[3] += smd_adc[i + 24]; // south vertical
  }
}

void SmdHistGen::CountSMDHits()
{
  // reset multiplicities
  n_hor_numhits = 0;
  n_ver_numhits = 0;
  s_hor_numhits = 0;
  s_ver_numhits = 0;

  std::vector<int> n_hor(8, -999);
  std::vector<int> n_ver(7, -999);
  std::vector<int> s_hor(8, -999);
  std::vector<int> s_ver(7, -999);
  int n_hor_adjacent;
  int n_ver_adjacent;
  int s_hor_adjacent;
  int s_ver_adjacent;

  // north
  for ( int i = 0; i < 8; i++)
  {
    n_hor[i] = 10*(i+1);
    if ( smd_adc[i] > minSMDcut )
    {
      // timing requirement
      if (smd_time[i]>=smd_north_t_low && smd_time[i]<=smd_north_t_high)
      {
        n_hor_numhits ++;
        smd_hor_north_total_hits->Fill(i);
	n_hor[i] = 0;
      }
    }
  }
  smd_hor_north_multiplicity->Fill(n_hor_numhits);
  for ( int i = 0; i < 7; i++)
  {
    n_ver[i] = 10*(i+1);
    if ( smd_adc[i + 8] > minSMDcut )
    {
      // timing requirement
      if (smd_time[i+8]>smd_north_t_low && smd_time[i+8]<=smd_north_t_high)
      {
        n_ver_numhits ++;
	smd_ver_north_total_hits->Fill(i);
	n_ver[i] = 0;
      }
    }
  }
  smd_ver_north_multiplicity->Fill(n_ver_numhits);

  // south
  for ( int i = 0; i < 8; i++)
  {
    s_hor[i] = 10*(i+1);
    if ( smd_adc[i + 16] > minSMDcut )
    {
      if ( smd_time[i+16]>=smd_south_t_low && smd_time[i+16]<=smd_south_t_high )
      {
	s_hor_numhits++;
	smd_hor_south_total_hits->Fill(i);
	s_hor[i] = 0;
      }
    }
  }
  smd_hor_south_multiplicity->Fill(s_hor_numhits);
  for ( int i = 0; i < 7; i++)
  {
    s_ver[i] = 10*(i+1);
    if ( smd_adc[i + 24] > minSMDcut )
    {
      if ( smd_time[i+24]>=smd_south_t_low && smd_time[i+24]<=smd_south_t_high )
      {
	s_ver_numhits++;
	smd_ver_south_total_hits->Fill(i);
	s_ver[i] = 0;
      }
    }
  }
  smd_ver_south_multiplicity->Fill(s_ver_numhits);

  n_hor_adjacent = CountAdjacentHits(n_hor);
  n_ver_adjacent = CountAdjacentHits(n_ver);
  s_hor_adjacent = CountAdjacentHits(s_hor);
  s_ver_adjacent = CountAdjacentHits(s_ver);
  // for testing
  if (false)
  {
    n_hor_numhits = n_hor_adjacent;
    n_ver_numhits = n_ver_adjacent;
    s_hor_numhits = s_hor_adjacent;
    s_ver_numhits = s_ver_adjacent;
  }
}

bool SmdHistGen::NeutronSelection(std::string which, bool centerCorrected)
{
  // Requirements:
  // veto ADC<200
  // ZDC2 ADC>20 (use high gain channels)
  // num SMD hits > 1
  // all hits in correct time window (north ZDC&Veto 5-8, north SMD 9-12)
  int frontveto, backveto, zdc1, zdc2, smdhitshor, smdhitsver;
  int frontveto_t, backveto_t, zdc1_t, zdc2_t, veto_t_low, veto_t_high;
  float smd_x, smd_y;
  if (which == "north") {
    frontveto = veto_adc[0];
    backveto = veto_adc[1];
    zdc1 = zdc_adc[8];
    zdc2 = zdc_adc[10];
    frontveto_t = veto_time[0];
    backveto_t = veto_time[1];
    zdc1_t = zdc_time[8];
    zdc2_t = zdc_time[10];
    smdhitshor = n_hor_numhits;
    smdhitsver = n_ver_numhits;
    veto_t_low = veto_north_t_low;
    veto_t_high = veto_north_t_high;
    smd_x = smd_pos[1];
    smd_y = smd_pos[0];
    if (centerCorrected)
    {
      smd_x = smd_pos_corr[1];
      smd_y = smd_pos_corr[0];
    }
  }
  else if (which == "south") {
    frontveto = veto_adc[2];
    backveto = veto_adc[3];
    zdc1 = zdc_adc[0];
    zdc2 = zdc_adc[2];
    frontveto_t = veto_time[2];
    backveto_t = veto_time[3];
    zdc1_t = zdc_time[0];
    zdc2_t = zdc_time[2];
    smdhitshor = s_hor_numhits;
    smdhitsver = s_ver_numhits;
    veto_t_low = veto_south_t_low;
    veto_t_high = veto_south_t_high;
    smd_x = smd_pos[3];
    smd_y = smd_pos[2];
    if (centerCorrected)
    {
      smd_x = smd_pos_corr[3];
      smd_y = smd_pos_corr[2];
    }
  }
  else {
    std::cout << "NeutronSelection: invalid string " << which << std::endl;
    return false;
  }

  // timing requirement
  if (zdc1_t<zdc_t_low || zdc1_t>zdc_t_high) {return false;}
  if (zdc2_t<zdc_t_low || zdc2_t>zdc_t_high) {return false;}
  if (frontveto_t<veto_t_low || frontveto_t>veto_t_high) {return false;}
  if (backveto_t<veto_t_low || backveto_t>veto_t_high) {return false;}
  // ADC requirements
  if (frontveto > maxVetocut) {return false;}
  if (backveto > maxVetocut) {return false;}
  if (zdc1 < minZDC1cut) {return false;}
  if (zdc2 < minZDC2cut) {return false;}
  // SMD hit requirement
  if (smdhitshor < minSMDhits) {return false;}
  if (smdhitsver < minSMDhits) {return false;}
  // Radial position cut
  float r = sqrt(smd_x*smd_x + smd_y*smd_y);
  if (r < radius_low) {return false;}
  if (r > radius_high) {return false;}
  // passed all cuts
  return true;
}

std::vector<float> SmdHistGen::anaWaveformFast(CaloPacket *p, const int channel)
{
  std::vector<float> waveform;
  for (int s = 0; s < p->iValue(0, "SAMPLES"); s++)
  {
    waveform.push_back(p->iValue(s, channel));
    // fill waveform histograms
    if (channel >= 48 && channel < 63)
    {
      smd_north_waveforms_raw->Fill(s, p->iValue(s, channel));
    }
    if (channel >= 112 && channel < 127)
    {
      smd_south_waveforms_raw->Fill(s, p->iValue(s, channel));
    }
  }
  std::vector<std::vector<float>> multiple_wfs;
  multiple_wfs.push_back(waveform);

  std::vector<std::vector<float>> fitresults_zdc;
  fitresults_zdc = WaveformProcessingFast->calo_processing_fast(multiple_wfs);

  std::vector<float> result;
  result = fitresults_zdc.at(0);
  return result;
}

int SmdHistGen::CountAdjacentHits(std::vector<int> channels)
{
  // this function assumes each channel with a valid hit has value 0,
  // all other channels have *distinct* nonzero values
  std::vector<int> hit_indices;
  for (unsigned int i=0; i<channels.size(); i++)
  {
    if (channels[i] == 0) {hit_indices.push_back(i);}
  }
  int nhits = hit_indices.size();
  // if there is only 1 hit, let's say it is adjacent to itself
  if (nhits < 2) {return nhits;}
  int n_adjacent = 1;
  for (int i=0; i<(nhits-1); i++)
  {
    int delta = hit_indices[i+1] - hit_indices[i];
    if (delta == 1) {n_adjacent++;}
  }
  return n_adjacent;
}
