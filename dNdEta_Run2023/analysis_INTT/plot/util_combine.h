#include <algorithm>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <limits>
#include <utility>
#include <vector>

using namespace std;

int GetMbinNum(const std::string &fstr)
{
    if (fstr.find("Centrality0to3") != std::string::npos)
        return 0;
    if (fstr.find("Centrality3to6") != std::string::npos)
        return 1;
    if (fstr.find("Centrality6to10") != std::string::npos)
        return 2;
    if (fstr.find("Centrality10to15") != std::string::npos)
        return 3;
    if (fstr.find("Centrality15to20") != std::string::npos)
        return 4;
    if (fstr.find("Centrality20to25") != std::string::npos)
        return 5;
    if (fstr.find("Centrality25to30") != std::string::npos)
        return 6;
    if (fstr.find("Centrality30to35") != std::string::npos)
        return 7;
    if (fstr.find("Centrality35to40") != std::string::npos)
        return 8;
    if (fstr.find("Centrality40to45") != std::string::npos)
        return 9;
    if (fstr.find("Centrality45to50") != std::string::npos)
        return 10;
    if (fstr.find("Centrality50to55") != std::string::npos)
        return 11;
    if (fstr.find("Centrality55to60") != std::string::npos)
        return 12;
    if (fstr.find("Centrality60to65") != std::string::npos)
        return 13;
    if (fstr.find("Centrality65to70") != std::string::npos)
        return 14;
    if (fstr.find("Centrality0to70") != std::string::npos)
        return 70;

    return -1; // Default case if no match is found
}