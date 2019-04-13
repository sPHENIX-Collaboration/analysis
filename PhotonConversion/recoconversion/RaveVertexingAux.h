/*!
 *  \file   RaveVertexingAux.h
 *  \brief    Find vtx for 2 SvtxTracks.
 *  \author   Francesco Vassalli <Francesco.Vassalli>
 */

#ifndef __RaveVertexingAux_H__
#define __RaveVertexingAux_H__

#include <GenFit/GFRaveVertex.h>
#include <GenFit/GFRaveVertexFactory.h>
#include <GenFit/Track.h>
#include <fun4all/SubsysReco.h>
#include <string>
#include <vector>

namespace PHGenFit
{
class Track;
} /* namespace PHGenFit */

namespace genfit
{
class GFRaveVertexFactory;
} /* namespace genfit */
class SvtxTrack;
namespace PHGenFit
{
class Fitter;
} /* namespace PHGenFit */


class PHTimer;
class SvtxTrackMap;
class SvtxVertexMap;
class SvtxVertex;
class PHCompositeNode;
class PHG4TruthInfoContainer;


class RaveVertexingAux
{
public:
  //! Default constructor
  RaveVertexingAux(PHCompositeNode* topNode);
  //!dtor
  ~RaveVertexingAux();
  
  SvtxVertex* makeVtx(SvtxTrack *t1, SvtxTrack *t2);

  inline int get_primary_pid_guess() const
  {
    return _primary_pid_guess;
  }

  inline void set_primary_pid_guess(int primaryPidGuess)
  {
    _primary_pid_guess = primaryPidGuess;
  }
  inline double get_vertex_min_ndf() const
  {
    return _vertex_min_ndf;
  }

  inline void set_vertex_min_ndf(double vertexMinPT)
  {
    _vertex_min_ndf = vertexMinPT;
  }
  inline bool hasErrors(){
    return !_noErrors;
  }
  inline int Verbosity(int v){
    _verbosity=v;
    return v;
  }
  inline int Verbosity(){
    return _verbosity;
  }
private:
  genfit::Track* TranslateSvtxToGenFitTrack(SvtxTrack* svtx);
  PHGenFit::Fitter* _fitter;
  int _primary_pid_guess;
  double _vertex_min_ndf;
  genfit::GFRaveVertexFactory* _vertex_finder;
  std::string _vertexing_method;
  bool _noErrors=true;
  int _verbosity=0;
};
#endif //__RaveVertexingAux_H__
