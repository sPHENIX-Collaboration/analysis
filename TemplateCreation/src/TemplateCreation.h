// Tell emacs that this is a C++ source
//  -*- C++ -*-.
#ifndef TEMPLATECREATION_H
#define TEMPLATECREATION_H

#include <fun4all/SubsysReco.h>

#include <string>

class PHCompositeNode;
class TProfile;
class TH2D;
class TemplateCreation : public SubsysReco
{
 public:

  TemplateCreation(const std::string &name = "TemplateCreation");

  ~TemplateCreation() override;

 
  int Init(PHCompositeNode *topNode) override;
  int InitRun(PHCompositeNode *topNode) override;
  int process_event(PHCompositeNode *topNode) override;
  int ResetEvent(PHCompositeNode *topNode) override;
  int EndRun(const int runnumber) override;
  int End(PHCompositeNode *topNode) override;
  int Reset(PHCompositeNode * /*topNode*/) override;
  
  void set_outfile(std::string outfilename)
  {
    s_outfilename = outfilename;
  }  
  void set_nsamples(int samples)
  {
    nsamples = samples;
  }

  void set_packetrange(int lowpacket, int highpacket)
  {
    m_packet_low = lowpacket;
    m_packet_high = highpacket;
  }



 private:
  double rising_shape(double *x, double *par);
  std::string s_outfilename;
  TProfile *hp_all_particle_fine;
  TH2D *h2_all_particle_fine;
  int nsamples;
  int m_packet_low;
  int m_packet_high;
  int m_nchannels;
  double target_hm;
  int binscale = 8;


};

#endif // TEMPLATECREATION_H
