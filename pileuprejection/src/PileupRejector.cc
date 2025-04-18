#include "PileupRejector.h"

#include <mbd/MbdPmtContainer.h>
#include <mbd/MbdPmtHit.h>
#include <phool/PHCompositeNode.h>
#include <phool/getClass.h>
#include <cmath>
int PileupRejector::decodeEvent(PHCompositeNode *topNode)
{

  MbdPmtContainer *pmts_mbd = findNode::getClass<MbdPmtContainer>(topNode, "MbdPmtContainer");

  float maxtime[2] = {-100, -100};
  float mintime[2] = {100, 100};
  float rmstime[2] = {0,0};
  float sumtime[2] = {0,0};
  float npmt[2] = {0,0};

  m_chargesum = 0;
  m_prodsigma = 0;
  m_avgsigma = 0;
  m_maxsigma = 0;
  m_proddelta = 0;
  m_avgdelta = 0;
  m_maxdelta = 0;

  if (pmts_mbd)
    {
      for (int i = 0 ; i < 128; i++)
	{
	  MbdPmtHit *tmp_pmt = pmts_mbd->get_pmt(i);
	  
	  float charge = tmp_pmt->get_q();
	  float time = tmp_pmt->get_time();

	  if (fabs(time) < 25. && charge > 0.4)
	    {
	      m_chargesum += charge;
	      int side = i/64;
	      npmt[side]++;
	      if (time > maxtime[side])
		maxtime[side] = time;
	      if (time < mintime[side])
		mintime[side] = time;

      	      rmstime[side] += time*time;
      	      sumtime[side] += time;

	    }
	}
      if (npmt[0] < 1 && npmt[1] < 1) 
	{
	  m_pileup = false;
	  return 0;
	}

      if (npmt[0] >= m_hitcut)
      	{
      	  rmstime[0]/=(float)npmt[0];
      	  sumtime[0]/=(float)npmt[0];
      	  rmstime[0] = sqrt(rmstime[0] -sumtime[0]*sumtime[0]);
	}
      else
	{
	  maxtime[0] = 0;
	  mintime[0] = 0;
	  rmstime[0] = 0;
	  sumtime[0] = 0;
	}
      if (npmt[1] >= m_hitcut)
      	{
      	  rmstime[1]/=(float)npmt[1];
      	  sumtime[1]/=(float)npmt[1];
      	  rmstime[1] = sqrt(rmstime[1] -sumtime[1]*sumtime[1]);
	}
      else
	{
	  maxtime[1] = 0;
	  mintime[1] = 0;
	  rmstime[1] = 0;
	  sumtime[1] = 0;
	}

      m_prodsigma = rmstime[0]*rmstime[1];
      m_avgsigma = (rmstime[0] + rmstime[1])/2.;
      m_maxsigma = std::max(rmstime[0], rmstime[1]);
      
      m_proddelta = (maxtime[1] - mintime[1])*(maxtime[0] - mintime[0]);
      m_avgdelta = ((maxtime[1] - mintime[1]) + (maxtime[0] - mintime[0]))/2.;
      m_maxdelta = std::max(maxtime[1] - mintime[1], maxtime[0] - mintime[0]);
      

    }

  return 0;
}

bool PileupRejector::isPileup()
{
  m_pileup = false;
  if (m_cutstrength == PileupCutStrength::COMFORT)
    {
      if (m_prodsigma >= m_comfort_time_cut) m_pileup = true;
    }
  if (m_cutstrength == PileupCutStrength::STRICT)
    {
      if (m_avgsigma >= m_strict_time_cut) m_pileup = true;
    }
  if (m_cutstrength == PileupCutStrength::DRACONIAN)
    {
      if (m_avgsigma >= m_draconian_time_cut) m_pileup = true;
    }
  return m_pileup;
}
void PileupRejector::Print()
{
  
  std::cout << " PileupRejector: " << std::endl;
  std::cout << "    Is Pileup : " << (m_pileup ? "Yes" : "No" ) << std::endl;

}

