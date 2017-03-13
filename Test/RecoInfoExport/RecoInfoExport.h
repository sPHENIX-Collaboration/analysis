#ifndef __RecoInfoExport_H__
#define __RecoInfoExport_H__

#include <fun4all/SubsysReco.h>
#include <string>
#include <vector>
#include <TH1D.h>
#include <TH2D.h>

class PHCompositeNode;

class RecoInfoExport : public SubsysReco
{

public:

  explicit
  RecoInfoExport(const std::string &name = "RecoInfoExport");

  int
  Init(PHCompositeNode *topNode);
  int
  process_event(PHCompositeNode *topNode);
  int
  End(PHCompositeNode *topNode);

  void
  set_file_prefix(const std::string &s)
  {
    _file_prefix = s;
  }

private:

  int _event;
  std::string _file_prefix;

  std::vector<std::string> _calo_names;
};

#endif // __RecoInfoExport_H__
