#ifndef TRIGGERSELECT_H
#define TRIGGERSELECT_H

#include <fun4all/SubsysReco.h>

#include <string>

// Forward declarations
class PHCompositeNode;

class GL1TriggerSelect : public SubsysReco
{
 public:
  //! constructor
  GL1TriggerSelect(const std::string& name = "GL1TriggerSelect");

  //! destructor
  ~GL1TriggerSelect() override;

  void select_trigger(unsigned int bit) { m_triggerbit = bit; }
  //! full initialization
  int Init(PHCompositeNode*) override;

  //! event processing method
  int process_event(PHCompositeNode*) override;

  //! end of run method
  int End(PHCompositeNode*) override;

 private:

  unsigned int m_triggerbit = 10;
  int _eventcounter{0};
  int _range{1};

};

#endif
