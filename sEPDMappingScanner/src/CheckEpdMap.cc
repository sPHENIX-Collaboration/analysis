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


#include <iostream>
#include <iomanip> 
#include <cmath>
#include <ffamodules/CDBInterface.h>

//_____________________________________________________________________________
CheckEpdMap::CheckEpdMap(const std::string& name)
  : SubsysReco(name)
{}

//_____________________________________________________________________________
int CheckEpdMap::InitRun(PHCompositeNode* topNode)
{
  std::cout << "Initializing CheckEpdMap" << std::endl;
  auto* geom = findNode::getClass<EpdGeom>(topNode, "TOWERGEOM_EPD");
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

  towers = findNode::getClass<TowerInfoContainer>(topNode,
    "TOWERINFO_CALIB_SEPD");
  if (!towers) return Fun4AllReturnCodes::ABORTEVENT;

  return Fun4AllReturnCodes::EVENT_OK;
}

//_____________________________________________________________________________
int CheckEpdMap::process_event(PHCompositeNode*)
{
  

  for (unsigned int ch = 0; ch < towers->size(); ++ch)
  {
    //const float adc  = towers->get_tower_at_channel(ch)->get_energy();
    unsigned key = m_keyVec[ch];    
    //unsigned key = TowerInfoDefs::encode_epd(ch);         

    const unsigned arm    = TowerInfoDefs::get_epd_arm   (key);
    const unsigned rbin   = TowerInfoDefs::get_epd_rbin  (key);
    const unsigned phibin = TowerInfoDefs::get_epd_phibin(key);

    if (auto *twr_ch = towers->get_tower_at_channel(ch)) {
      m_profOld[arm]->Fill(rbin, phibin, twr_ch->get_energy());      
    }


  }

  for (unsigned arm = 0; arm < 2; ++arm) {
    for (unsigned r = 0; r < NRING; ++r)
    {
      const unsigned maxPhi = (r == 0 ? 12 : 24);
      for (unsigned phi = 0; phi < maxPhi; ++phi)
      {
        unsigned key = TowerInfoDefs::encode_epd(arm, r, phi,true);
        if (auto* twr_geom = towers->get_tower_at_key(key))
          m_profNew[arm]->Fill(r, phi, twr_geom->get_energy());
      }
    }
  }

  for (unsigned arm  = 0; arm < 2;      ++arm)
  for (unsigned ring = 0; ring < NRING; ++ring)
  {
    const unsigned maxPhi = (ring == 0 ? 12 : 24);
    for (unsigned phi = 0; phi < maxPhi; ++phi)
    {
      /* -------- OLD path: channel index from the CDB truth table ----- */
      int ch_idx = m_reverse[arm][ring][phi];  
      if (ch_idx < 0) continue;                   

      float adc_old = towers->get_tower_at_channel(ch_idx)->get_energy();

      /* -------- NEW path: key built from geometry -------------------- */
      uint32_t key_new = TowerInfoDefs::encode_epd(arm, ring, phi,true);
      auto* tw_new     = towers->get_tower_at_key(key_new);
      if (!tw_new) {
        std::cerr << "encode_epd("<<arm<<','<<ring<<','<<phi
                  << ") → key " << key_new
                  << " but container has no tower for that key!\n";
        continue;
      }
      float adc_new = tw_new->get_energy();


      if (adc_old != adc_new)
      {
   
        m_diffCount->Fill(ch_idx);

        m_diffADC->Fill(ring, phi, adc_old - adc_new);
      }

      //direct check
      //auto* tw_old = towers->get_tower_at_channel(ch_idx);

      //if(tw_new != tw_old) {
      //  std::cout << "Mismatched towers! tw_new = " << tw_new << ", tw_old = " << tw_old << std::endl;
      
      //}

    }
  }



  return Fun4AllReturnCodes::EVENT_OK;
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
        
        unsigned key_tmp = TowerInfoDefs::encode_epd(arm, r, phi,true);
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
        unsigned key  = TowerInfoDefs::encode_epd(arm, ring, phi,true);
        int geom_chan = TowerInfoDefs::decode_epd(key);
        if(chan != geom_chan){
          std::cout << "Mismatched channel found: (arm, r, phi) = (" << arm << ", " <<
          ring << ", " << phi << "). reverse channel mapping: " << chan << ", new channel mapping: " <<
          geom_chan << std::endl;
        }
      }
      else                 // ------- “new” (geometry) mapping -------
      {
        unsigned key  = TowerInfoDefs::encode_epd(arm, ring, phi,true);
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
