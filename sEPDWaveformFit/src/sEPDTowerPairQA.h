#ifndef SEPDTOWERPAIRQA_H
#define SEPDTOWERPAIRQA_H

#include <fun4all/SubsysReco.h>

#include <string>
#include <vector>

class PHCompositeNode;
class TFile;
class TTree;
class TH1F;
class TH2F;
class TProfile;
class TProfile2D;
class CDBTTree;

class sEPDTowerPairQA : public SubsysReco
{
 public:
  explicit sEPDTowerPairQA(const std::string &name = "sEPDTowerPairQA");
  ~sEPDTowerPairQA() override = default;

  int Init(PHCompositeNode *topNode) override;
  int process_event(PHCompositeNode *topNode) override;
  int End(PHCompositeNode *topNode) override;

  void SetOutputFile(const std::string &f) { m_outputFile = f; }

  void SetNodeA(const std::string &n) { m_nodeA = n; }
  void SetNodeB(const std::string &n) { m_nodeB = n; }
  void SetLabelA(const std::string &l) { m_labelA = l; }
  void SetLabelB(const std::string &l) { m_labelB = l; }
  void SetAmplitudeCut(float c) { m_amp_cut = c; }

  void SetChannelMapFile(const std::string &f) { m_channelMapFile = f; }
  void SetTemplateAFile(const std::string &f) { m_templateAFile = f; }
  void SetTemplateBFile(const std::string &f) { m_templateBFile = f; }
  void SetCaptureEvents(int n) { m_captureEvents = n; }

 private:
  static const int NRING = 16;

  std::string m_outputFile = "sEPDTowerPairQA.root";
  std::string m_nodeA = "TOWERS_SEPD";
  std::string m_nodeB = "TOWERSNEW_SEPD";
  std::string m_labelA = "cdb";
  std::string m_labelB = "new";
  float m_amp_cut = 20.0;

  int m_event = 0;
  long m_npaired = 0;
  long m_nimproved = 0;
  double m_sum_dchi2 = 0.0;
  double m_sum_dchi2_frac = 0.0;

  TFile *m_file = nullptr;

  TH1F *h_dchi2 = nullptr;
  TH1F *h_dchi2_frac = nullptr;
  TH2F *h_chi2_scatter = nullptr;
  TProfile *p_dchi2_vs_amp = nullptr;
  TProfile *p_dchi2frac_vs_amp = nullptr;
  TProfile2D *p_dchi2frac_map = nullptr;

  TH1F *h_dtime = nullptr;
  TH2F *h_amp_scatter = nullptr;

  TTree *m_tree = nullptr;
  int t_ich = 0;
  int t_arm = 0;
  int t_ring = 0;
  float t_ampA = 0.f;
  float t_ampB = 0.f;
  float t_chi2A = 0.f;
  float t_chi2B = 0.f;
  float t_timeA = 0.f;
  float t_timeB = 0.f;

  std::string m_channelMapFile;                     //!
  std::string m_templateAFile;                      //!
  std::string m_templateBFile;                      //!
  std::string m_mapFieldName = "epd_channel_map2";  //!
  int m_captureEvents = 0;                          //!

  static const int MAXSAMP = 16;
  CDBTTree *m_sepdmap = nullptr;                     //!
  std::vector<int> m_idxpid;                         //!
  std::vector<int> m_idxch;                          //!
  bool m_mapValidated = false;                       //!
  bool m_mapOK = false;                              //!

  TProfile *m_tmplA = nullptr;                        //!
  TProfile *m_tmplB = nullptr;                        //!
  TTree *m_dtree = nullptr;                           //!
  int d_evt = 0;                                      //!
  int d_ich = 0;                                      //!
  int d_arm = 0;                                      //!
  int d_ring = 0;                                     //!
  int d_ns = 0;                                       //!
  float d_raw[MAXSAMP] = {0};                         //!
  float d_ampA = 0.f;                                 //!
  float d_timeA = 0.f;                                //!
  float d_chi2A = 0.f;                                //!
  float d_ampB = 0.f;                                 //!
  float d_timeB = 0.f;                                //!
  float d_chi2B = 0.f;                                //!

  static bool sepd_skip_channel(int ich, int pid);
  void build_index_map();
  TProfile *clone_template(const std::string &file, const char *newname);
};

#endif
