#ifndef COPYCUTVERTEX_H
#define COPYCUTVERTEX_H

#include <fun4all/SubsysReco.h>

#include <string>

// Forward declarations
class PHCompositeNode;

class CopyCutVertex : public SubsysReco
{
 public:
  //! constructor
  CopyCutVertex(const std::string& name = "CopyCutVertex");

  //! destructor
  ~CopyCutVertex() override;

  void new_vertex_nodename(const std::string &nodename) { m_nodename = nodename; }
  //! full initialization
  int Init(PHCompositeNode* topNode) override;

  //! event processing method
  int process_event(PHCompositeNode*) override;

  //! end of run method
  int End(PHCompositeNode*) override;

 private:
  std::string m_nodename{"RealVertexMap"};

};

#endif
