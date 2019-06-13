#ifndef __ATRACK_H__
#define __ATRACK_H__

//
//  Hello ATrack Fans:
//
//  A Track is a decision on the solution to the pattern
//  recognition process.  It consists of one Cluster from each
//  tracking station.
//
//                                                NF
//                                                09-18-2013
//

#include <vector>

#include "ABlob.h"
#include "TMath.h"

class ATrack
{
public:
  ATrack();
  virtual ~ATrack() {}

  // gets...
  double X() {return GetXForZ(0); }
  double Y() {return GetYForZ(0); }
  double GetXForZ( double z ){ return (x0 + dxdz * ( z - z0 )); };
  double GetYForZ( double z ){ return (y0 + dydz * ( z - z0 )); };
  double Slope;
  double Offset;
  void SetBlobs( std::vector<ABlob*> );
  std::vector<ABlob*> GetBlobs(){ return blobs; };
  void SetSlope(double s) {Slope=s;}
  void SetOffset(double o) {Offset=o;}
  double check() {return 5;}


protected:
  /* tracks are straight lines defined by these parameters: */
  double x0;
  double y0;
  double z0;
  double dxdz;
  double dydz;

  void DetermineParameters();
  
  /* vector of blobs associated with this track */
  std::vector<ABlob*> blobs;
  
};

#endif /* __ATRACK_H__ */
