#include <uspin/SpinDBContent.h>
#include <uspin/SpinDBOutput.h>


R__LOAD_LIBRARY(libuspin.so)

// Bad spin pattern runs
// blue spin pattern is all 10 or all 0:
// Fill 34762: 47571 
// Fill 34896: 49956 49959 49964 49969 
// Fill 34899: 50033 50044 50045 50046 50048 50067 50070 50075 
// Fill 35127: 53554

// blue and yellow spin patterns are all 0:
// Fill 35009: 51914 51915

// blue spin pattern is all 10 (This is a 28x28 fill for both beams but blue got messed up)
// Fill 34897: 49998 50004 50005 50015
   
// Unknown (for now) (looks like blue beam is multiplied by negative 1):
// Fill 34899: 50068 50069 50073 50074 50076 50077 50083 50085 50086


void checkSpinPatterns(int runnumber = 45729, unsigned int qa_level = 0xffff)
{
  SpinDBContent spin_cont;
  SpinDBOutput spin_out("phnxrc");

  spin_out.StoreDBContent(runnumber,runnumber);
  //spin_out.StoreDBContent(runnumber,runnumber,qa_level);
  
  spin_out.GetDBContentStore(spin_cont,runnumber);

  ////////////////////////////////////////////////////
  // Get fill number
  int fillnumber = spin_cont.GetFillNumber();
  std::cout << "Fill Number: " << fillnumber << std::endl;
  ////////////////////////////////////////////////////

  ////////////////////////////////////////////////////
  // Get spin patterns
  int bluespin[120] = {0};
  int yellspin[120] = {0};
  std::cout << "Blue spin pattern: [";
  for (int i = 0; i < 120; i++)
  {
    bluespin[i] = spin_cont.GetSpinPatternBlue(i);
    std::cout << bluespin[i];
    if (i < 119)std::cout << ", ";
  }
  std::cout << "]" << std::endl;

  std::cout << "Yellow spin pattern: [";
  for (int i = 0; i < 120; i++)
  {
    yellspin[i] = spin_cont.GetSpinPatternYellow(i);
    std::cout << yellspin[i];
    if (i < 119)std::cout << ", ";
  }
  std::cout << "]" << std::endl;
  ////////////////////////////////////////////////////


}