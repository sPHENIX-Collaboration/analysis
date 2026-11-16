#include <uspin/SpinDBContent.h>
#include <uspin/SpinDBOutput.h>


R__LOAD_LIBRARY(libuspin.so)





//****** Good GL1p scaler runs that need to be corrected *******//
//******Xingshift should be set to 0 (and badrunqa set to 0) ********//

// Unknown what went wrong (crossing shift set to -999 from 0xFFFF qa_level)
//45891 46460 46620 47065 48821 49051 49221 49247 49377

//this is 56 by 56 fill (fill 34907) but xingshiftcal gives xingshift = -999 when it should be 0
//50441

// this is an actual 28x28 fill (fill 34897), the crossing shift is properly set to 0 but spindbqa gets confused
//49997 49998 49999 50000 50001 50002 50004 50005 50006 50007 50008 50009
//50010 50011 50012 50013 50014 50015 50016 50017 50018 50019 
//50020 50021 50022 50023 50024 50025  

// 6x6 fill (fill 34886) xingshiftcal correct with xingshift = 0 but spindbqa gets confused
//49678 49679 49680 49681 49682 49684 49685 49687
//49688 49683 49694 49697 49699 49703 49705 49706 
//49707 49708 49709 49710 49711 49712 49713 49714
//49715 49716 49717 49718 49719 49720 49721 49722

// 6x6 fill (fill 35020) xingshiftcal correct with xingshift = 0 but spindbqa gets confused
// 52077 52078
// *********** //


//****** Bad GL1p scaler runs (set crossing shift to -999) *******//
// These are runs where the bunch number distributions are all flat (i.e. no abort gap)
//45237 45238 45239 45240 45241 45532 47075 47328 47388 47448 47449 47563
//47564 47565 48308 48330 48332 48948 49343 50488 51608 51609 51610 51611
//52893

// Low counts (crossing shift set to -999 from 0xFFFF qa_level)
//45393 47387 45436 45437 45442 45474
//45475 45476 45533 45736 45743 46033 46621 46638
//46639 46467 46468 46469 46470 46504 46734 46780
//47069 47070 47071 47072 47076 47077 47085 47171 47172 47173 47174 47176 47206
//47207 47209 47214 47215 47221 47223 47224 47225 47228
//47400 47446 47447 47535 47536 47537 47544 47545 47546
//47598 47600 47615 47648 47655 47773 47776
//47850 47885 47887 48005 48094 48170 48171 48283 48316
//48328 48333 48334 48340 48362 48401 48442
//48443 48502 48503 48507 48508 48515 48644 48647 48648 48649 48650 48651 48652 48653
//48654 48659 48718 48809 48833 48855 48978 49129
//49461 49738 50042 50043 50371 50479 50480 50481 
//50523 50532 50566 53682

// Very low scaler counts (badrunqa=1?)           
//45263 45320 45381 45382 45385 45396 45473 45523 45536
//45556 45695 45720 45873 45931 46054 46426 46439 46489
//46490 46491 46508 46509 46510 46511 46512 46513 46514
//46515 46516 46517 46518 46519 46520 46521 46522 46578
//46579 46580 46582 46592 46607 47073 47074 47326 47327
//47335 47399 47401 47402 47418 47534 47560 47561 47562
//47571 47774 48331 48339 48468 48642 48643 48992 48993
//50038 50085 50086 52216 53621

//MBD scalers are empty (or very close) but zdc scalers look fine (more or less)? (badrunqa=1?)
//45387 45388 45694 45696 45697 45698 45699 45700 45707
//45708 45709 45710 45711 45712 47107 47698 47753 48936
//51223 51859 53214

// In fill 34597, Bunch 0 has really low counts (badrunqa=1?)
// looks like it would get messed up by zdcns because bunch 0 has 0 counts
//45294 45295 45296 45297
//45298 45299 45300 45301 45302 45303 45304 45305 45309 
//45310 45311 45312 45314 45316 45318 45319

//same bunch 0 issue as above but xingshiftcal has set xingshift to -999 (badrunqa=1?)
//48400 48402 48403 48404 48405 48406 48407 48419 48420

// something really weird here where it has assigned xingshift = 0 at qa_level = 0xFFFF but the gl1p scaler distributions
// looks like they have large suppression in final 26 bunches? (badrunqa=1?)
//51838 51840 51841 51842 51843 51848 51854 51855 51856 51858 51860

// mbd scalers look good until the last bunch 119 has a lot of counts (same as non abort gap bunches) (badrunqa=1?)
//45383 51369 

//spindbqa gets confused (usually sees 10 0's near abort gap for zdcns) b.c. statistics are too low (just set crossingshift to -999 and badrunqa=1)
//50937 51250

//weird up and down alternating behavior of scalers even though it is 111x111 fill (badrunqa=1?)
//49344 51157

// 6x6 fill (fill 34886) xingshiftcal messes up with xingshift = 7 (badrunqa=1?)
//49690

// strange behavior in mbd scalers where abort gap looks like only last 4 bunches
// and then first 5 bunches are about a quarter of what they are nominally (zdcns looks fine) (badrunqa=1?)
//45526 

// *********** //



void checkGL1pScalers(int runnumber = 46515, unsigned int qa_level = 0xffff)
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
  // Get crossing shift
  int crossingshift = spin_cont.GetCrossingShift();
  std::cout << "Crossing shift: " << crossingshift << std::endl;
  ////////////////////////////////////////////////////

  ////////////////////////////////////////////////////
  // Get MBD NS GL1p scalers
  long long mbdns[120] = {0};
  std::cout << "MBDNS GL1p scalers: [";
  for (int i = 0; i < 120; i++)
  {
    mbdns[i] = spin_cont.GetScalerMbdNoCut(i);
    std::cout << mbdns[i];
    if (i < 119)std::cout << ", ";
  }
  std::cout << "]" << std::endl;
  ////////////////////////////////////////////////////

  ////////////////////////////////////////////////////
  // Get MBD VTX GL1p scalers
  long long mbdvtx[120] = {0};
  std::cout << "MBD VTX GL1p scalers: [";
  for (int i = 0; i < 120; i++)
  {
    mbdvtx[i] = spin_cont.GetScalerMbdVertexCut(i);
    std::cout << mbdvtx[i];
    if (i < 119)std::cout << ", ";
  }
  std::cout << "]" << std::endl;
  ////////////////////////////////////////////////////


  ////////////////////////////////////////////////////
  // Get ZDC NS GL1p scalers
  long long zdcns[120] = {0};
  std::cout << "ZDC NS GL1p scalers: [";
  for (int i = 0; i < 120; i++)
  {
    zdcns[i] = spin_cont.GetScalerZdcNoCut(i);
    std::cout << zdcns[i];
    if (i < 119)std::cout << ", ";
  }
  std::cout << "]" << std::endl;
  ////////////////////////////////////////////////////


}