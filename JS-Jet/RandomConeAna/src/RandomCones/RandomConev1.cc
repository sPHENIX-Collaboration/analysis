#include "RandomCone.h"
#include "RandomConev1.h"


#include <iostream>
#include <string>
#include <vector>
#include <utility>
#include <cmath>

#include <jetbase/Jet.h>

#include <ostream>
#include <cstdlib> // for exit


void RandomConev1::identify(std::ostream& os) const
{
  os << "=========RandomCone===========" << std::endl;
  os << "\t cone type: " << m_cone_type << std::endl;
  os << "\t pt: " << m_pt << std::endl;
  os << "\t eta: " << m_eta << std::endl;
  os << "\t phi: " << m_phi << std::endl;
  os << "\t R: " << m_R << std::endl;
  os << "\t n_clustered: " << n_clustered() << std::endl;
  os << "===============================";  
  return ;
}

void RandomConev1::Reset()
{
  m_pt = 0.0;
  m_eta = NAN;
  m_phi = NAN;
  m_R = NAN;
  m_cone_type = RandomCone::ConeType::VOID;
  _comp_ids.clear();
  return ;

}

void RandomConev1::add_constituent(Jet* particle)
{
  float e = particle->get_e();
  float pt = e/cosh(particle->get_eta());
  // m_pt += particle->get_pt();
  m_pt += pt;
  auto compvec = particle->get_comp_vec();
  for (auto comp : compvec)
  {
    _comp_ids.push_back(std::make_pair(comp.first, comp.second));
  }

  return ;
}



