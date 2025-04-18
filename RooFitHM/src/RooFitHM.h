#ifndef RooFitHM_H
#define RooFitHM_H

#include <fun4all/SubsysReco.h>

class RooFFTConvPdf;
class TF1;

class RooFitHM : public SubsysReco
{
 public:
  RooFitHM(const std::string &name = "RooFitHM");
  ~RooFitHM() override {}

  //! Called once at the beginning of the job
  int Init(PHCompositeNode*) override;

  //! Called on every event (we do nothing here)
  int process_event(PHCompositeNode*) override { return 0; }

  //! Called at the end of the job
  int End(PHCompositeNode*) override;

 private:
  RooFFTConvPdf* m_pdf{nullptr};
  TF1* m_func{nullptr};
};

#endif  // RooFitHM_H
