#include "CaloTransverseEnergy.h"
//#include "sPhenixStyle.h"
//#include "sPhenixStyle.C"
#include <fun4all/Fun4AllServer.h>
#include <fun4all/Fun4AllDstInputManager.h>
#include <fun4all/SubsysReco.h>
#include <sstream>
#include <string.h>

int main(int argc, const char* argv[])
{
	//taking in a data file and will run over all data 
	//std::cout<<"input of " <<argc <<std::endl;
	//std::string filename (argv[1]);
	std::string filename="/sphenix/tg/tg01/jets/ahodges/run23_production/21518/DST-00021518-0000.root";
	std::cout<<"Input file is " <<filename <<std::endl;
	Fun4AllServer *se=Fun4AllServer::instance();
	Fun4AllDstInputManager *in = new Fun4AllDstInputManager("in");
	in->AddFile(filename);
	CaloTransverseEnergy* cte=new CaloTransverseEnergy(filename);
	std::stringstream subs(filename);
	std::string substr;
	int is_input=0;
	//load in the DST Segment and run number
	while(std::getline(subs, substr, '-')){
		if(substr.find("DST")!=std::string::npos) is_input=1; 
		if(is_input==1){
			 cte->run_number=std::stoi(substr);
			 is_input=2;
		}
		if(is_input==2){
			std::stringstream sss(substr);
			std::string subsub;
			while(std::getline(sss, subsub, '.')){
			try{
			 	cte->DST_Segment=std::stoi(substr);
				}	
			catch(std::exception* e) {}
			}
		} 
	}
	se->registerSubsystem(cte);
//	se->run();
	return 0;				
}
	
