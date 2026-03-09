#include "CheckEpdMap.h"

#include <phool/PHCompositeNode.h>
#include <phool/getClass.h>
#include <phool/recoConsts.h> 
#include <phool/PHNodeIterator.h>

#include <cdbobjects/CDBTTree.h>
#include <ffamodules/CDBInterface.h>

#include <calobase/TowerInfoDefs.h>
#include <calobase/TowerInfoContainerv4.h>
#include <epd/EpdGeomV2.h>

#include <fun4all/Fun4AllReturnCodes.h>

#include <TFile.h>
#include <TTree.h>
#include <TProfile2D.h>
#include <TH1.h>
#include <TH2.h>
#include <TCanvas.h>
#include <TLatex.h>
#include <TColor.h>
#include <TStyle.h>

#include <iostream>
#include <iomanip> 
#include <cmath>
#include <ffamodules/CDBInterface.h>

#include <g4main/PHG4HitContainer.h>
#include <g4main/PHG4HitDefs.h>
#include <g4main/PHG4Hit.h>
#include <g4main/PHG4InEvent.h>
#include <g4main/PHG4Particlev2.h>
#include <epd/EPDDefs.h>


//_____________________________________________________________________________
CheckEpdMap::CheckEpdMap(const std::string& name)
  : SubsysReco(name)
{}

//_____________________________________________________________________________
int CheckEpdMap::InitRun(PHCompositeNode* topNode)
{
  std::cout << "Initializing CheckEpdMap" << std::endl;
  geom = findNode::getClass<EpdGeom>(topNode, "TOWERGEOM_EPD");
  if (!geom)
  {
    std::cerr << Name() << ": EpdGeom node TOWERGEOM_EPD not found\n";
    return Fun4AllReturnCodes::ABORTEVENT;
  }

  std::cout << "Found TOWERGEOM_EPD" << std::endl;

  auto [vkey, mapped_values] = build_key_vector(); 
  //auto vkey, mapped_values = build_key_vector();
  if (vkey.empty())
  {
    std::cerr << Name() << ": failed to build vkey vector\n";
    return Fun4AllReturnCodes::ABORTEVENT;
  }

  check_map(vkey, mapped_values);

  m_keyVec = std::move(vkey); //store key vector for later use

  const char* title[2][2] = {
    { "South  old (key vector)", "North old (key vector)" },
    { "South  new (geom loop)" , "North  new (geom loop)" }
  };

  for (int arm = 0; arm < 2; ++arm)
  {
    m_profOld[arm] = new TProfile2D(Form("profOld_%d",arm),  title[0][arm],
                                    NRING, -0.5, NRING-0.5,
                                    NPHI , -0.5, NPHI -0.5);
    m_profNew[arm] = new TProfile2D(Form("profNew_%d",arm),  title[1][arm],
                                    NRING, -0.5, NRING-0.5,
                                    NPHI , -0.5, NPHI -0.5);
  }

  m_diffCount = new TH1I("diffCount",
                       "index where old (channel) =/= new (key);channel index",
                       744, -0.5, 743.5);

    
  m_diffADC   = new TH2F("diffADC",
                        "ADC(old) – ADC(new);ring;#phi sector",
                        NRING, -0.5, NRING-0.5,
                        NPHI , -0.5, NPHI -0.5);

  m_hitCenterDist = new TH1F(
    "hitCenterDist",
    "Distance between true hit and tile center;#Delta r (cm)",
    400,   // bins
    0.0,   // min
    200.0  // max
  );

  constexpr int NBINS = 1100;
  constexpr float RANGE = 110.;

  for (unsigned arm = 0; arm < 2; ++arm) {
    for (unsigned r = 0; r < NRING; ++r)
    {
      const unsigned maxPhi = (r == 0 ? 12 : 24);
      for (unsigned phi = 0; phi < maxPhi; ++phi)
      {
          m_tileHist[arm][r][phi] = new TH2F(
            Form("tileHit_arm%u_ring%u_phi%u", arm, r, phi),
            Form("Tile hits: arm=%u ring=%u phi=%u;X–X_{ctr} (cm);Z–Z_{ctr} (cm)",
                arm, r, phi),
            NBINS, -RANGE, RANGE,
            NBINS, -RANGE, RANGE);

      }

    }
  }



  BuildSimMap();
  InitializeSimMatchMap();




  if (!m_simulationMode){
    towers = findNode::getClass<TowerInfoContainer>(topNode,
      "TOWERINFO_CALIB_SEPD");
    if (!towers) return Fun4AllReturnCodes::ABORTEVENT;
  }
  

  return Fun4AllReturnCodes::EVENT_OK;
}

//_____________________________________________________________________________
int CheckEpdMap::process_event(PHCompositeNode* topNode)
{
  
  if (! m_simulationMode) {
    return processEventDataMode(topNode);
  }
  else {
    return processEventSimulationMode(topNode);
  }

  

}

//_____________________________________________________________________________
std::pair<std::vector<unsigned>, std::vector<int>>
CheckEpdMap::build_key_vector() const
{

  std::cout << "CheckEpdMap::build_key_vector called..." << std::endl;

  static const std::string sEPDMapName   = "SEPD_CHANNELMAP";   // domain
  static const std::string sEPDFieldName = "epd_channel_map";   // branch

  std::string calibFile = CDBInterface::instance()->getUrl(sEPDMapName);
  if (calibFile.empty())
  {
    std::cerr << "CheckEpdMap : no sEPD mapping file for domain "
              << sEPDMapName << " found – aborting\n";
    std::exit(1);
  }

  std::cout << "Found sEPD mapping file" << std::endl;

  std::unique_ptr<CDBTTree> cdbttree( new CDBTTree(calibFile) );

  std::cout << "Created CDBTTree..." << std::endl;


  std::vector<unsigned> vkey;
  std::vector<int> mapped_vals;
  vkey.reserve(768);               

  std::cout << "Building key vector... " << std::endl;

  for (int ch = 0; ch < 768; ++ch)
  {
    int keymap = cdbttree->GetIntValue(ch, sEPDFieldName);

    if (keymap == 999)    
      continue;

    unsigned key = TowerInfoDefs::encode_epd(static_cast<unsigned>(keymap));
    vkey.push_back(key);
    mapped_vals.push_back(keymap);
  }

  std::cout << "Key vector built with " << vkey.size() << " keys" << std::endl;

  return { std::move(vkey), std::move(mapped_vals) };
}

//_____________________________________________________________________________
void CheckEpdMap::check_map(
  const std::vector<unsigned>& vkey, const std::vector<int> mapped_vals) const
{
std::cout << "CheckEpdMap::check_map called..." << std::endl;

//constexpr unsigned NRING = 16;
//constexpr unsigned NPHI  = 24;
int reverse[2][NRING][NPHI];
std::fill_n(&reverse[0][0][0], 2*NRING*NPHI, -1);

  std::cout << "tower index that maps to zeroth key: " << mapped_vals[0] << std::endl;

for (unsigned ch = 0; ch < vkey.size(); ++ch)
{
  unsigned key   = vkey[ch];
  //unsigned key = TowerInfoDefs::encode_epd(ch);
  //int mapped_ch = mapped_vals[ch]; //this is the "channel number" that should be returned by decode_epd



  int mapped_ch = ch;
  //we are passing mapped_ch to get_tower_at_ch, so this is what we should map to

  
unsigned arm   = TowerInfoDefs::get_epd_arm   (key);
unsigned rbin  = TowerInfoDefs::get_epd_rbin  (key);
unsigned phibin= TowerInfoDefs::get_epd_phibin(key);

if (arm < 2 && rbin < NRING && phibin < NPHI)
reverse[arm][rbin][phibin] = mapped_ch; 
else
std::cerr << Name() << ": decoded bins out of range for ch=" << ch << "\n";
}

std::memcpy(m_reverse,reverse, sizeof m_reverse); //store truth table for later

std::cout << "Truth table built!" << std::endl;


  std::size_t nBad = 0;
  for (unsigned arm = 0; arm < 2; ++arm) {
    for (unsigned r = 0; r < NRING; ++r)
    {
      unsigned maxPhi = (r == 0 ? 12 : 24);
      for (unsigned phi = 0; phi < maxPhi; ++phi)
      {
        int ch_truth = reverse[arm][r][phi];

        //to generate map directly from (arm,r,phi) -> tower index/"channel" number
        
        unsigned key_tmp = TowerInfoDefs::encode_epd(arm, r, phi);
        int ch_code      = TowerInfoDefs::decode_epd(key_tmp);

        if (ch_truth != ch_code)
        {
          std::cout << Name() << ": mismatch arm="<<arm
                    << " r="<<r<<" phi="<<phi
                    << "  code="<<ch_code
                    << "  truth="<<ch_truth << '\n';
          ++nBad;
        }
      }
    }

  }


  std::cout << Name() << ": total mismatches = " << nBad << std::endl;

}

int CheckEpdMap::End(PHCompositeNode*)
{
  

  TH2F* h[2][2];   // [row][col] : row 0 = OLD, row 1 = NEW ; col 0 = SOUTH, col 1 = NORTH
  for (int arm = 0; arm < 2; ++arm)
  {
    h[0][arm] = (TH2F*)m_profOld[arm]->ProjectionXY(
                  Form("hOld_%d",arm));
    h[1][arm] = (TH2F*)m_profNew[arm]->ProjectionXY(
                  Form("hNew_%d",arm));
  }

  TCanvas c("c","sEPD compare maps",1800,900);
  c.Divide(2,2);

  TLatex lab; lab.SetTextAlign(22); lab.SetTextSize(0.025);

  auto draw = [&](TH2F* hh, int pad, bool isNorth, bool isOld)
  {
  c.cd(pad);
  hh->SetMinimum(0);
  hh->Draw("colz");

  const unsigned arm = isNorth ? 1 : 0;

  for (int bx = 1; bx <= hh->GetNbinsX(); ++bx)
    for (int by = 1; by <= hh->GetNbinsY(); ++by)
    {
      unsigned ring = bx - 1;
      unsigned phi  = by - 1;
      if (ring == 0 && phi > 11) continue;   // dummy bins in innermost ring

      int chan;
      if (isOld)           // ------- “old” (CDB‑based) mapping -------
      {
        chan = m_reverse[arm][ring][phi];
        //let's brute force a check here
        unsigned key  = TowerInfoDefs::encode_epd(arm, ring, phi);
        int geom_chan = TowerInfoDefs::decode_epd(key);
        if(chan != geom_chan){
          std::cout << "Mismatched channel found: (arm, r, phi) = (" << arm << ", " <<
          ring << ", " << phi << "). reverse channel mapping: " << chan << ", new channel mapping: " <<
          geom_chan << std::endl;
        }
      }
      else                 // ------- “new” (geometry) mapping -------
      {
        unsigned key  = TowerInfoDefs::encode_epd(arm, ring, phi);
        chan = TowerInfoDefs::decode_epd(key);
        //and another one
        int old_chan = m_reverse[arm][ring][phi];
        if(chan != old_chan){
          std::cout << "Mismatched channel found: (arm, r, phi) = (" << arm << ", " <<
          ring << ", " << phi << "). reverse channel mapping: " << old_chan << ", new channel mapping: " <<
          chan << std::endl;
        }
      }

      lab.DrawLatex(hh->GetXaxis()->GetBinCenter(bx),
                    hh->GetYaxis()->GetBinCenter(by),
                    Form("%u", chan));
    }
  };


  //north: h[0][new/old]

  draw(h[0][1], 1, true ,  true );   // north – old 

  draw(h[1][1], 2, true ,  false);   // north – new 

  draw(h[0][0], 3, false,  true );   // south – old

  draw(h[1][0], 4, false,  false);   // south – new

  c.SaveAs("sEPD_compare_channelMap.pdf");
  
  TCanvas cDiffADC("cDiffADC","ADC(old) - ADC(new)",800,700);
  cDiffADC.SetRightMargin(0.12); 
  m_diffADC->Draw("colz");
  cDiffADC.Print("diffADC.pdf"); 

  TCanvas cDiffIdx("cDiffIdx","channel indices with ADC mismatch",800,600);
  m_diffCount->Draw();
  cDiffIdx.Print("diffCount.pdf");

  PrintReverseTable();

  TCanvas cDist("cDist","Δ(hit,center)",800,600);
  m_hitCenterDist->Draw();
  cDist.Print("hitCenterDistance.pdf");

  std::cout << "Attempting to write output histsogram " << outputFile << std::endl;

  if (! outputFile.empty())
  {
    TFile fout(outputFile.c_str(), "RECREATE");
    if (fout.IsOpen())
    {
      m_hitCenterDist->Write("hitCenterDist");
      fout.Close();
      std::cout << Name()
                << ": wrote histogram to "
                << outputFile << std::endl;
    }
    else
    {
      std::cerr << Name()
                << ": ERROR opening output file "
                << outputFile << std::endl;
    }
  }

  //example: draw tile (0,10,7)

  TCanvas cTile("cTile","example tile: (0,10,7)", 800,600);
  m_tileHist[0][10][7]->Draw("COLZ");
  cTile.Print("example_tile.pdf");


  Int_t palette[2] = { kRed, kGreen };
  gStyle->SetPalette(2, palette);
  gStyle->SetNumberContours(2);


  TCanvas cSimMatchPolar("cSimMatchPolar",
                        "sEPD Simulation Match (polar)",-1,0,
                        1400, 600);
  cSimMatchPolar.SetRightMargin(0.15);
  cSimMatchPolar.Divide(2,1);

  cSimMatchPolar.cd(1);
  gPad->SetTicks(1,1);

  gPad->DrawFrame(-3.8, -3.8, 3.8, 3.8);
  m_simMatchPolarS ->Draw("same COLZ pol AH");
  m_simMatchPolarS01->Draw("same COLZ pol AH");
  TLatex ts; ts.SetNDC(); ts.SetTextSize(0.05);
  ts.DrawLatex(0.30, 0.92, "sEPD South Sim-Match");

 
  cSimMatchPolar.cd(2);
  gPad->SetTicks(1,1);
  gPad->DrawFrame(-3.8, -3.8, 3.8, 3.8);
  m_simMatchPolarN ->Draw("same COLZ pol AH");
  m_simMatchPolarN01->Draw("same COLZ pol AH");
  TLatex tn; tn.SetNDC(); tn.SetTextSize(0.05);
  tn.DrawLatex(0.30, 0.92, "sEPD North Sim-Match");


  cSimMatchPolar.SaveAs("sEPD_sim_match_polar.pdf");



  //c.Write(); 

  return Fun4AllReturnCodes::EVENT_OK;
}


//_____________________________________________________________________________
void CheckEpdMap::PrintReverseTable(std::ostream &os /* = std::cout */) const
{
  using std::setw;

  os << "\nstatic constexpr int epdchnlmap[2]"
        "[" << NRING << "][" << NPHI << "] = {\n";

  for (unsigned arm = 0; arm < 2; ++arm)
  {
    os << "  { // arm " << arm << '\n';
    for (unsigned r = 0; r < NRING; ++r)
    {
      os << "    { ";
      for (unsigned phi = 0; phi < NPHI; ++phi)
      {
        int ch = m_reverse[arm][r][phi];
        if (ch < 0) ch = 999;          // mark unused slots
        os << setw(4) << ch;
        if (phi + 1 < NPHI) os << ", ";
      }
      os << " }";
      if (r + 1 < NRING) os << ',';   
      os << '\n';
    }
    os << "  }";                       
    if (arm == 0) os << ',';          
    os << '\n';
  }
  os << "};\n" << std::endl;         
}



int CheckEpdMap::processEventDataMode(PHCompositeNode * /*topNode*/)
{

  for (unsigned int ch = 0; ch < towers->size(); ++ch)
  {
    unsigned key = m_keyVec[ch];
    unsigned arm    = TowerInfoDefs::get_epd_arm   (key);
    unsigned rbin   = TowerInfoDefs::get_epd_rbin  (key);
    unsigned phibin = TowerInfoDefs::get_epd_phibin(key);
    auto *twr_ch = towers->get_tower_at_channel(ch);
    m_profOld[arm]->Fill(rbin, phibin, twr_ch->get_energy());
  }


  for (unsigned arm = 0; arm < 2; ++arm)
  {
    for (unsigned r = 0; r < NRING; ++r)
    {
      unsigned maxPhi = (r == 0 ? 12 : 24);
      for (unsigned phi = 0; phi < maxPhi; ++phi)
      {
        unsigned key = TowerInfoDefs::encode_epd(arm, r, phi);
        if (auto *twr_geom = towers->get_tower_at_key(key))
        {
          m_profNew[arm]->Fill(r, phi, twr_geom->get_energy());
        }
      }
    }
  }

  for (unsigned arm  = 0; arm < 2;      ++arm)
  for (unsigned ring = 0; ring < NRING; ++ring)
  {
    unsigned maxPhi = (ring == 0 ? 12 : 24);
    for (unsigned phi = 0; phi < maxPhi; ++phi)
    {
      int ch_idx = m_reverse[arm][ring][phi];
      if (ch_idx < 0) continue;
      float adc_old = towers->get_tower_at_channel(ch_idx)->get_energy();

      uint32_t key_new = TowerInfoDefs::encode_epd(arm, ring, phi);
      auto *tw_new     = towers->get_tower_at_key(key_new);
      float adc_new    = tw_new->get_energy();

      if (adc_old != adc_new)
      {
        m_diffCount->Fill(ch_idx);
        m_diffADC  ->Fill(ring, phi, adc_old - adc_new);
      }
    }
  }

  return Fun4AllReturnCodes::EVENT_OK;
}

void CheckEpdMap::BuildSimMap() {
  //build map from (arm,sector,tile_idx) -> (arm,rbin,phibin)
  //taken from PHG4EPDModuleReco
  for (unsigned int arm = 0; arm < 2; arm++) //arm
  {
    for (int sec = 0; sec < 12; sec++) //sectors
    {
      for (int tile_id = 0; tile_id < 31; tile_id++) //tile_idx
      {
        unsigned int phi = Getphimap(tile_id) + 2 * sec;
        unsigned int r = Getrmap(tile_id);
        
        if (r == 0)
        {
          phi = sec;
        }
        
        m_tile_bin[arm][sec][tile_id] = std::make_tuple(arm, r, phi);
        m_module_coords[arm][r][phi] = std::make_tuple(arm,sec,tile_id);

      }
    }
  }
}


int CheckEpdMap::Getphimap(int phiindex)
{
  static const int phimap[31] = {0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1};

  return phimap[phiindex];
}

int CheckEpdMap::Getrmap(int rindex)
{
  static const int rmap[31] = {0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11, 12, 12, 13, 13, 14, 14, 15, 15};

  return rmap[rindex];
}



int CheckEpdMap::processEventSimulationMode(PHCompositeNode *topNode)
{
  auto *g4hit = findNode::getClass<PHG4HitContainer>(topNode, "G4HIT_EPD");
  if (!g4hit)
  {
    std::cerr << Name() << ": no G4HIT_EPD node found in sim‐mode\n";
    return Fun4AllReturnCodes::ABORTEVENT;
  }



  auto* simTowers = findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_SIM_EPD");
  if (!simTowers)
  {
    std::cerr << Name() << ": no TOWERINFO_SIM_EPD node found in sim-mode\n";
    return Fun4AllReturnCodes::ABORTEVENT;
  }



  auto range = g4hit->getHits();

  // skip the event if there are no g4hits
  if (range.first == range.second) {
    return Fun4AllReturnCodes::EVENT_OK;
  }


  uint32_t arm   = 0;
  uint32_t ring  = 0;
  uint32_t phi   = 0;
  bool     found_match = false;


  for (auto iter = range.first; iter != range.second; ++iter) {
    PHG4Hit* hit        = iter->second;
    uint32_t sim_tileid = hit->get_hit_id() >> PHG4HitDefs::hit_idbits;

    //get coords
    int this_tile   = EPDDefs::get_tileid(sim_tileid);
    int this_sector = EPDDefs::get_sector(sim_tileid);
    int this_arm    = EPDDefs::get_arm(sim_tileid);


    std::tie(arm, ring, phi)
      = m_tile_bin[this_arm][this_sector][this_tile];


    unsigned key = TowerInfoDefs::encode_epd(arm, ring, phi);
    auto*   twr = simTowers->get_tower_at_key(key);
    if (!twr) {
      std::cout << "WARNING: no tower found at location (arm,ring,phi) = (" <<
      arm << ", " << ring << ", " << phi << ")." << std::endl;
      //Fun4AllReturnCodes::ABORTEVENT;
    }
    float   e   = twr ? twr->get_energy() : 0;

    if (e == -1.f) {
      std::cout << "Found match!" << std::endl;
      found_match = true;
      break; 
    }
  }

  std::cout << "Setting histogram bin (" << ring << ", " <<
  phi << ") to " << found_match << " with arm = " << arm << std::endl;  

  double w = ( found_match ? +1.0 : 0.1 );



  int rbin = ring +1;
  int phibin = phi +1;

  std::cout << "phibin" << phibin << " rbin " << rbin << " w = " << w << std::endl;

  if (ring == 0)
  {
    if (arm == 0) m_simMatchPolarS01->SetBinContent(phibin, rbin, w);
    else          m_simMatchPolarN01->SetBinContent(phibin, rbin, w);
  }
  else
  {
    if (arm == 0) m_simMatchPolarS->SetBinContent(phibin, rbin, w);
    else          m_simMatchPolarN->SetBinContent(phibin, rbin, w);
  }

  std::cout << "Filled histogram!" << std::endl;



  return Fun4AllReturnCodes::EVENT_OK;
}

void CheckEpdMap::InitializeSimMatchMap()
{
  std::cout << "Booking sim histos!" << std::endl;
  if (!m_simMatchPolarS)
  {
    std::cout << "initializing m_simMatchPolarS" << std::endl;
 
    m_simMatchPolarS = new TH2F(
      "h_simMatch_south", "Sim-match South;phi;ring",
      24, 0, 2*M_PI,
      16, 0.15, 3.5
    );
    m_simMatchPolarS->SetMaximum(1);
    m_simMatchPolarS->SetMinimum(0);
  }
  if (!m_simMatchPolarN)
  {
    m_simMatchPolarN = new TH2F(
      "h_simMatch_north", "Sim-match North;phi;ring",
      24, 0, 2*M_PI,
      16, 0.15, 3.5
    );
    m_simMatchPolarN->SetStats(0);
    m_simMatchPolarN->SetMaximum(1);
    m_simMatchPolarN->SetMinimum(0);
  }

  if (!m_simMatchPolarS01)
  {
    m_simMatchPolarS01 = new TH2F(
      "h_simMatch_south01","Sim-match South inner;phi;ring",
      12, 0, 2*M_PI,
      16, 0.15, 3.5
    );
    m_simMatchPolarS01->SetStats(0);
    m_simMatchPolarS01->SetMaximum(1);
    m_simMatchPolarS01->SetMinimum(0);
  }
  if (!m_simMatchPolarN01)
  {
    m_simMatchPolarN01 = new TH2F(
      "h_simMatch_north01","Sim-match North inner;phi;ring",
      12, 0, 2*M_PI,
      16, 0.15, 3.5
    );
    m_simMatchPolarN01->SetStats(0);
    m_simMatchPolarN01->SetMaximum(1);
    m_simMatchPolarN01->SetMinimum(0);
  }

  std::cout << "InitializeSimMatchMap: Done!" << std::endl;

}
