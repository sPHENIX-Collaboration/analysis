#ifndef __ABLOB_H__
#define __ABLOB_H__

#include <vector>

//
//  Hello ABlob Fans:
//
//  The pattern recognition will create a vector of Blob objects.
//  In the current implementation, each Blob object will be made
//  as a collection of several zigzags where the charge on the "center"
//  zigzag is greater than all the left zigzags and greater than or equal
//  to all the right zigzag.
//
//  The blob class contains POINTERS (not owned locally so they should
//  not be deleted upon destruction) to the zigzags in question.
//  It then implements methods to return the total blob charge, and the
//  coordinate of the centroid.
//
//  Since we know that there is always some level of differential non-linearity
//  in  segmented pad plane response, we expect that the simple centroid
//  calculation will need some kind of correction.  We opt to NOT put
//  the correction function as a part of the Blob object, but expect the user
//  to write external codes to manipulate and set the corrected charges and
//  positions.
//
//  Also worth noting is the intended use of the CorrectedQ (corrected charge).
//  Charge is divided among X,Y, and U coordinates roughly equally (1/3 each).
//  The corrected charge is intended to take away this factor so that each of
//  of the blobs makes its own estimate of the TOTAL charge deposit of the
//  ORIGINAL particle. This is the proper value for matching at the pattern 
//  recognition stage.
//
//                                                TKH, Vlad, Niv
//                                                2018-10-09
//

class AZigzag;
class TH1;
class TF1;

class ABlob
{
public:
  ABlob(std::vector<AZigzag*> MANYZIGZAGS);
  virtual ~ABlob() {}

  // sets...
  void SetCorrectedCentroid(double CC) {correctedcentroid = CC;}
  void SetCorrectedQ       (double CQ) {correctedq = CQ;}

  // gets...
  double CentroidX();
  double CentroidY();
  double CentroidZ();
  double CentroidR();
  double CentroidP();
  double Q        (); //total charge in the blob
  double maxT     (); //time of zigzag with the max charge

  double CorrectedCentroid() {return correctedcentroid;}
  double CorrectedQ       () {return correctedq; }

  void Draw();
  void Report();

  int numZigs() {return manyZigs.size();}
  std::vector<AZigzag*> manyZigs;

  static bool RecalibrateCharge;

  static bool GaussPosition;
  static TH1* BlobPos;
  static TH1* BlobSigma;
  static TF1* BlobFit;

  double GetPHI();

protected:
  void FixTheCharges();

  double correctedcentroid;
  double correctedq       ;

  double Precalc_PHI;
  double Precalc_R;

};

#endif /* __ABLOB_H__ */
