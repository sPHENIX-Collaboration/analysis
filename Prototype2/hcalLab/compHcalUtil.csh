rootcint -f hcalUtilDict.cxx -c -I$OFFLINE_MAIN/include -I$ONLINE_MAIN/include -I$ROOTSYS/include  hcalUtil.h hcalUtilLinkDef.h
g++ -g -m32 -Wno-deprecated  -I.  -I`root-config  --incdir` -I$MY_INSTALL/include  -I$ROOTSYS/include  -I$OFFLINE_MAIN/include -c hcalUtil.C hcalUtilDict.cxx
g++ -m32 -Wno-deprecated -shared hcalUtil.o  hcalUtilDict.o -o hcalUtil.so
