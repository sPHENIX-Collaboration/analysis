#ifndef __PLOTS_H__
#define __PLOTS_H__
#include "caloplots.h"
#include <string>
#include <vector>
#include <map>
class caloplots;

struct plots{
				int z_bin=0;
				float zl=0, zh=1;
				bool sim=false;
				caloplots* em=new caloplots("EMCAL",zl,zh,sim);
				caloplots* ihcal=new caloplots("IHCAL", zl, zh, sim);
				caloplots* ohcal=new caloplots("OHCAL", zl, zh, sim);
				caloplots* total=new caloplots("ALL", zl, zh, sim);
				std::vector<std::map<std::string, float>> event_data;
};

#endif
