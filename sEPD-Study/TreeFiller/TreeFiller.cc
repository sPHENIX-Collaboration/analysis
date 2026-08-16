#include "TreeFiller.h"

#include <fun4all/Fun4AllHistoManager.h>
#include <fun4all/Fun4AllReturnCodes.h>
#include <fun4all/Fun4AllServer.h>

#include <phool/phool.h>

#include <TTree.h>

#include <iostream>

TreeFiller *TreeFiller::m_instance = nullptr;
TTree *TreeFiller::m_tree = nullptr;

//____________________________________________________________________________..
TreeFiller::TreeFiller(const std::string &tree_name, const std::string &hm_name)
  : SubsysReco("TreeFiller_" + tree_name)
  , m_tree_name(tree_name)
  , m_hm_name(hm_name)
{
  if (!m_instance)
  {
    m_instance = this;
  }
}

//____________________________________________________________________________..
TreeFiller::TreeFiller(const std::string &name, const std::string &tree_name, const std::string &hm_name)
  : SubsysReco(name)
  , m_tree_name(tree_name)
  , m_hm_name(hm_name)
{
  if (!m_instance)
  {
    m_instance = this;
  }
}

//____________________________________________________________________________..
TTree *TreeFiller::getTree(const std::string &tree_name,
                           const std::string &hm_name,
                           const std::string &tree_title)
{
  Fun4AllServer *se = Fun4AllServer::instance();
  Fun4AllHistoManager *hm = se->getHistoManager(hm_name);
  if (!hm)
  {
    hm = new Fun4AllHistoManager(hm_name);
    se->registerHistoManager(hm);
  }

  TTree *tree = nullptr;
  if (hm->isHistoRegistered(tree_name))
  {
    tree = dynamic_cast<TTree *>(hm->getHisto(tree_name));
  }
  if (!tree)
  {
    tree = new TTree(tree_name.c_str(), tree_title.c_str());
    hm->registerHisto(tree);
  }
  m_tree = tree;
  return tree;
}

//____________________________________________________________________________..
int TreeFiller::Init(PHCompositeNode * /*topNode*/)
{
  Fun4AllServer *se = Fun4AllServer::instance();
  Fun4AllHistoManager *hm = se->getHistoManager(m_hm_name);
  if (hm && !m_outfilename.empty())
  {
    hm->setOutfileName(m_outfilename);
  }
  // Pre-fetch or create the TTree in Fun4AllHistoManager
  m_tree = getTree(m_tree_name, m_hm_name);
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int TreeFiller::InitRun(PHCompositeNode * /*topNode*/)
{
  if (!m_tree)
  {
    m_tree = getTree(m_tree_name, m_hm_name);
  }
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int TreeFiller::process_event(PHCompositeNode * /*topNode*/)
{
  if (!m_tree)
  {
    m_tree = getTree(m_tree_name, m_hm_name);
  }

  if (m_tree)
  {
    m_tree->Fill();
  }
  else
  {
    std::cout << PHWHERE << "TreeFiller: Could not find or create TTree '"
              << m_tree_name << "' in HistoManager '" << m_hm_name << "'!" << std::endl;
    return Fun4AllReturnCodes::ABORTRUN;
  }

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int TreeFiller::End(PHCompositeNode * /*topNode*/)
{
  if (Verbosity() > 0 && m_tree)
  {
    std::cout << "TreeFiller::End - Finished filling tree '" << m_tree_name
              << "' with " << m_tree->GetEntries() << " entries." << std::endl;
  }
  return Fun4AllReturnCodes::EVENT_OK;
}
