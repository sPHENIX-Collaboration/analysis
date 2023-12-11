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
				caloplots* em=new caloplots("EMCAL",zl,zh);
				caloplots* ihcal=new caloplots("IHCAL", zl, zh);
				caloplots* ohcal=new caloplots("OHCAL", zl, zh);
				caloplots* total=new caloplots("ALL", zl, zh);
				std::vector<std::map<std::string, float>> event_data;
};

#endif
