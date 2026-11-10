#ifndef DIJETTREEMAKER_H
#define DIJETTREEMAKER_H

#include <fun4all/SubsysReco.h>
#include <string>
#include <vector>

#include <TTree.h>

class PHCompositeNode;
class PHParameters;

class userinfotest : public SubsysReco
{
 public:

  userinfotest(const std::string &name = "userinfotest", const std::string &outfilename = "trees_test.root");

  virtual ~userinfotest();

  int Init(PHCompositeNode *topNode) override;

  int process_event(PHCompositeNode *topNode) override;

  int End(PHCompositeNode *topNode) override;

  int Reset(PHCompositeNode * /*topNode*/) override;

 private:

  TTree *_tree{nullptr};
  std::string _foutname;

  int b_test = 0;

};

#endif 
