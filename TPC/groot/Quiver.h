#ifndef __QUIVER_H__
#define __QUIVER_H__

//
//  Hello Quiver Fans:
//
//  It has become inconventient to continually
//  change "if (true)" in MyFavoriteMartin to
//  "if (false)" and recompile. Here we are
//  inventing (Hawkeye's) Quiver that belongs to groot.
//
//  This is a class that is simply filled with a 
//  bunch of static member variables. All we shall
//  do is be able to set them via the prompt in
//  order to pass flow control data to any program
//  (especially MyFavoriteMartin) at the root prompt.
//
//                                      TKH, Vlad
//                                      2018-11-06
//

class Quiver
{
public:
  Quiver() {}
  virtual ~Quiver() {}


  //  OK..we're going for it...
  //  Flags for MyFavoriteMartin operation
  static int  RunNumber;
  static bool UseRunCalibs;
  static bool AnalyzeScope;
  static bool AnalyzeSRS;
  static bool ProcessCrystals;
  static bool ProcessTrackers;
  static bool FreeFit;
  static bool ProcessMpcEx;

  static bool MpcExUnique;

  static bool DebugTracker;
  static bool DebugMpcEx;
  static bool DebugExShower;
  static bool DebugMpc;

  static bool DrawZigzags;  
  static bool DrawBlobs;

  static int PaddingLimit;

  static bool BlobTimeCut;  // If true, zigzags cannot become a member of a blob unless their time seems OK...

protected:

};

#endif /* __QUIVER_H__ */
