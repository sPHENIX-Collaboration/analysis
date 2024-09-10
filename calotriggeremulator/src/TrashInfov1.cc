#include "TrashInfov1.h"

#include <ostream>

void TrashInfov1::identify(std::ostream& os) const
{
  os << "TrashInfo: " << std::endl;
  os << "  IsTrash = " << (_isTrash ? "Yes" : "No") << std::endl;
  os << "  Energy  = " << _energy << std::endl;
  os << "  R1      = " << _r1 << std::endl;
  os << "  Spread  = " << _spread << std::endl;
  os << "  EMEnergy= " << _emcal_energy << std::endl;
  return;
}

void TrashInfov1::Reset()
{
  _isTrash = false;
  _r1 = 0;
  _spread = 0;
  _emcal_energy = 0;
  _energy = 0;

}
