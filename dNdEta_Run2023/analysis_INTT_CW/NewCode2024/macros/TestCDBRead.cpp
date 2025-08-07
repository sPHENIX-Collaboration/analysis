#include <ffamodules/CDBInterface.h>

#include <phool/recoConsts.h>

R__LOAD_LIBRARY(libffamodules.so)
R__LOAD_LIBRARY(libphool.so)

void TestCDBRead()
{
  recoConsts *rc = recoConsts::instance();
// please choose a unique name, if it is your username it's easier to see who created it

  // rc->set_StringFlag("CDB_GLOBALTAG","2024p011"); 
  // rc->set_uint64Flag("TIMESTAMP",54280);

  rc->set_StringFlag("CDB_GLOBALTAG","ProdA_2024"); 
  rc->set_uint64Flag("TIMESTAMP",54912);

  // rc->set_StringFlag("CDB_GLOBALTAG","MDC2"); 
  // rc->set_uint64Flag("TIMESTAMP",6);


// 1000000 is the insert timestamp. Higher timestamps work, lower time stamps do not
  CDBInterface *cdb = CDBInterface::instance();
  cout <<"INTT_HotMap : "<< cdb->getUrl("INTT_HotMap") << endl;
  cout <<"INTT_HOTMAP : " <<cdb->getUrl("INTT_HOTMAP") << endl;
  cout <<"INTT_BCOMAP : " <<cdb->getUrl("INTT_BCOMAP") << endl;
  cout <<"INTT_DACMAP : " <<cdb->getUrl("INTT_DACMAP") << endl;
  cout <<"Tracking_Geometry : " <<cdb->getUrl("Tracking_Geometry") << endl;
  cout <<"TRACKINGALIGNMENT : " <<cdb->getUrl("TRACKINGALIGNMENT") << endl;
  
  gSystem->Exit(0);
  return;
}