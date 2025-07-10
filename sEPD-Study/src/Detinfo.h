// Tell emacs that this is a C++ source
//  -*- C++ -*-.
#pragma once

#include <fun4all/SubsysReco.h>

#include <string>
#include <vector>
#include <memory>
#include <TMath.h>
#include <TH1.h>

#include <cdbobjects/CDBTTree.h>

class PHCompositeNode;

class Detinfo : public SubsysReco
{
 public:

  explicit Detinfo(const std::string &name = "Detinfo");

  Int_t Init(PHCompositeNode *topNode) override;
  Int_t process_event(PHCompositeNode *topNode) override;
  Int_t End(PHCompositeNode *topNode) override;

  void set_filename(const std::string &file) {
      _outfile_name = file;
  }

 private:

 Int_t m_event;

 std::string m_sEPDMapName;
 std::string m_sEPDfieldname;
 Bool_t m_overrideSEPDMapName;
 Bool_t m_overrideSEPDFieldName;
 std::unique_ptr<CDBTTree> cdbttree;
 std::string _outfile_name;
};
