#include "Fun4All_BkgSub.C"

void Fun4All_RandomCones(const std::string &flist_dst_calofit = "DST_CALOFITTING_run3auau_pro001_pcdb001_v001-00068144-00000.root",
                         const std::string &flist_dst_zdc = "/direct/sphenix+tg+tg01/jets/anarde/run3auau/ZDC/68144/DST_ZDC_CALIB_run3auau_pro001_pcdb001_v001-00068144-00000.root",
                         const std::string &flist_dst_sepd = "DST_SEPD_RAW_run3auau_pro001_pcdb001_v001-00068144-00000.root",
                         const std::string &input_QVecCalib = "default",
                         const std::string &output = "test.root",
                         const std::string &output_tree = "tree.root",
                         int do_flow = 3,
                         int nEvents = 100,
                         int nSkip = 0,
                         int event_id = 0,
                         const std::string &dbtag = "newcdbtag",
                         const std::string &eta_calib_direct_path = "",
                         const std::string &event_list = "")
{
  Fun4All_BkgSub(flist_dst_calofit, flist_dst_zdc, flist_dst_sepd, input_QVecCalib,
                 output, output_tree, do_flow, nEvents, nSkip, event_id,
                 dbtag, eta_calib_direct_path, event_list, /*do_rcone=*/true, /*do_mult=*/false);
}
