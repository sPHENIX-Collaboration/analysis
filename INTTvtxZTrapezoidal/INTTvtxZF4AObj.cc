#include "INTTvtxZF4AObj.h"

INTTvtxZF4AObj::INTTvtxZF4AObj()
:
INTTvtxZ(std::nan("")),
INTTvtxZError(std::nan("")),
NgroupTrapezoidal(std::nan("")),
NgroupCoarse(std::nan("")),
TrapezoidalFitWidth(std::nan("")),
TrapezoidalFWHM(std::nan("")),
NClusAll(-1),
NClusAllInner(-1)
{
    return;
}

void INTTvtxZF4AObj::Reset()
{

  INTTvtxZ = std::nan("");
  INTTvtxZError = std::nan("");
  NgroupTrapezoidal = std::nan("");
  NgroupCoarse = std::nan("");
  TrapezoidalFitWidth = std::nan("");
  TrapezoidalFWHM = std::nan("");
  NClusAll = -1;
  NClusAllInner = -1;

  // std::cout << PHWHERE << "ERROR Reset() not implemented by daughter class" << std::endl;
  return;
}


// void INTTvtxZF4AObj::identify(std::ostream &out) const
// {
//   std::cout << "INTTvtxZF4AObj information" << std::endl;
  
//   return;
// }

int INTTvtxZF4AObj::isValid() const
{
  std::cout << PHWHERE << "isValid not implemented by daughter class" << std::endl;
  return 0;
}