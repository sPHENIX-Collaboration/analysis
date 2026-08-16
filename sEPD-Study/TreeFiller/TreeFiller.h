// Tell emacs that this is a C++ source
//  -*- C++ -*-.
#ifndef TREEFILLER_H
#define TREEFILLER_H

#include <fun4all/Fun4AllHistoManager.h>
#include <fun4all/Fun4AllServer.h>
#include <fun4all/SubsysReco.h>

#include <string>

class PHCompositeNode;
class TTree;

class TreeFiller : public SubsysReco
{
 public:
  static TreeFiller *instance()
  {
    if (m_instance)
    {
      return m_instance;
    }
    m_instance = new TreeFiller();
    return m_instance;
  }

  explicit TreeFiller(const std::string &tree_name = "T",
                      const std::string &hm_name = "ANALYSIS_HM");

  TreeFiller(const std::string &name,
             const std::string &tree_name,
             const std::string &hm_name);

  ~TreeFiller() override = default;

  // Rule of 5: explicitly delete copy and move semantics
  TreeFiller(const TreeFiller &) = delete;
  TreeFiller &operator=(const TreeFiller &) = delete;
  TreeFiller(TreeFiller &&) = delete;
  TreeFiller &operator=(TreeFiller &&) = delete;

  int Init(PHCompositeNode *topNode) override;
  int InitRun(PHCompositeNode *topNode) override;
  int process_event(PHCompositeNode *topNode) override;
  int End(PHCompositeNode *topNode) override;

  void setTreeName(const std::string &tree_name) { m_tree_name = tree_name; }
  const std::string &getTreeName() const { return m_tree_name; }

  void setHistoManagerName(const std::string &hm_name) { m_hm_name = hm_name; }
  const std::string &getHistoManagerName() const { return m_hm_name; }

  void setOutfileName(const std::string &filename)
  {
    m_outfilename = filename;
    Fun4AllServer *se = Fun4AllServer::instance();
    Fun4AllHistoManager *hm = se->getHistoManager(m_hm_name);
    if (!hm)
    {
      hm = new Fun4AllHistoManager(m_hm_name);
      se->registerHistoManager(hm);
    }
    hm->setOutfileName(filename);
  }
  const std::string &getOutfileName() const { return m_outfilename; }

  //! Method to retrieve or create a TTree managed by Fun4AllHistoManager
  static TTree *getTree(const std::string &tree_name = "T",
                        const std::string &hm_name = "ANALYSIS_HM",
                        const std::string &tree_title = "Analysis Tree");

 private:
  static TreeFiller *m_instance;
  static TTree *m_tree;

  std::string m_tree_name{"T"};
  std::string m_hm_name{"ANALYSIS_HM"};
  std::string m_outfilename;
};

#endif  // TREEFILLER_H
