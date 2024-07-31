#include <string>
#include <fun4all/Fun4AllBase.h>
#include <fun4all/DSTInputManager.h>

R__LOAD_LIBRARY("libCalorimeterTowerENC.so")

int CompareParticlesToTowers(std::string truthjet, std::string calotowers, std::string truthreco, std::string globalreco)
{
	return 1;
}
