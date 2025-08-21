// sPHENIX includes
#include <CDBUtils.C>

// c++ includes
#include <string>
#include <iostream>
#include <format>

void checkCDB(unsigned int runnumber, const std::string &dbtag = "newcdbtag") {
    setGlobalTag(dbtag);

    std::string centrality                      = getCalibration("Centrality", runnumber);
    std::string centrality_scale                = getCalibration("CentralityScale", runnumber);
    std::string centrality_vertex_scale         = getCalibration("CentralityVertexScale", runnumber);
    std::string centrality_default              = getCalibration("Centrality_default", runnumber);
    std::string centrality_scale_default        = getCalibration("CentralityScale_default", runnumber);
    std::string centrality_vertex_scale_default = getCalibration("CentralityVertexScale_default", runnumber);
    std::string mbd_qfit                        = getCalibration("MBD_QFIT", runnumber);
    std::string mbd_qfit_default                = getCalibration("MBD_QFIT_default", runnumber);
    std::string emcal_hotmap                    = getCalibration("CEMC_BadTowerMap", runnumber);

    std::cout << std::format("Centrality: {}", centrality) << std::endl;
    std::cout << std::format("Centrality default: {}", centrality_default) << std::endl;
    std::cout << std::format("Centrality Scale: {}", centrality_scale) << std::endl;
    std::cout << std::format("Centrality Scale default: {}", centrality_scale_default) << std::endl;
    std::cout << std::format("Centrality Vertex Scale: {}", centrality_vertex_scale) << std::endl;
    std::cout << std::format("Centrality Vertex Scale default: {}", centrality_vertex_scale_default) << std::endl;
    std::cout << std::format("MBD QFIT: {}", mbd_qfit) << std::endl;
    std::cout << std::format("MBD QFIT Default: {}", mbd_qfit_default) << std::endl;
    std::cout << std::format("EMCal Bad Tower Map: {}", emcal_hotmap) << std::endl;
}

# ifndef __CINT__
int main(int argc, const char* const argv[])
{
  const std::vector<std::string> args(argv, argv + argc);

  if (args.size() < 2 || args.size() > 3)
  {
    std::cerr << "usage: " << args[0] << " <runnumber> [dbtag]" << std::endl;
    std::cerr << "  input_DST: path to the input list file" << std::endl;
    std::cerr << "  runnumber: Run" << std::endl;
    std::cerr << "  dbtag: (optional) database tag (default: newcdbtag)" << std::endl;
    return 1;  // Indicate error
  }

  unsigned int runnumber = static_cast<unsigned int>(std::stoul(args[1]));
  std::string dbtag = "newcdbtag";

  if (args.size() >= 3)
  {
    dbtag = args[2];
  }

  checkCDB(runnumber, dbtag);

  std::cout << "======================================" << std::endl;
  std::cout << "done" << std::endl;
  return 0;
}
# endif
