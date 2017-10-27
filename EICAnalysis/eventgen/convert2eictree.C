int convert2eictree()
{
  gSystem->Load("$OPT_SPHENIX/eic-smear_root-5.34.36/lib/libeicsmear.so");

  BuildTree("data_milou/milou_5x50.out");

  return 0;
}
