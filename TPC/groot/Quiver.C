#include <iostream>

#include "Quiver.h"

//Running parameters

using namespace std;

int  Quiver::RunNumber = -999;
bool Quiver::UseRunCalibs = false;
bool Quiver::AnalyzeScope = true;
bool Quiver::AnalyzeSRS   = true;
bool Quiver::ProcessTrackers = true;
bool Quiver::ProcessCrystals = true;
bool Quiver::FreeFit = false;
bool Quiver::ProcessMpcEx = false;
bool Quiver::MpcExUnique = false;
bool Quiver::DebugTracker = true;
bool Quiver::DebugMpcEx = false;
bool Quiver::DebugExShower = false;
bool Quiver::DebugMpc = false;
bool Quiver::DrawZigzags = true;
bool Quiver::DrawBlobs = true;
bool Quiver::BlobTimeCut = false;
int  Quiver::PaddingLimit = 1;
