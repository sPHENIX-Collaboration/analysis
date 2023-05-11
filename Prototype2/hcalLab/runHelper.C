//class hcalHelper;
//class hLabHelper;
void runHelper(char * runKind, int runnumber, int eventsToRead){
  gSystem->Load("$MY_INSTALL/lib/libhcalUtil.so");
  hcalHelper * hHelper = hcalHelper::getInstance();
  hHelper->setRunKind(runKind);
  hHelper->evLoop(runnumber, eventsToRead);
  if(std::cin.get()==113) return;  
  hLabHelper * hlHelper = hLabHelper::getInstance();
  hlHelper->renameAndCloseRF();
}
