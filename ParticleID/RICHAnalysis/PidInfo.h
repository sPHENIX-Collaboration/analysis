#ifndef PIDINFO_H__
#define PIDINFO_H__

#include <phool/PHObject.h>
#include <phool/phool.h>
#include <cmath> // def of NAN
#include <vector>
#include <map>

#include <g4hough/SvtxTrackState.h>

class PidInfo : public PHObject {

public:

  enum PID_CANDIDATE {ELECTRON=0,CHARGEDPION=1,CHARGEDKAON=2,PROTON=3};

  virtual ~PidInfo() {}

  virtual void Reset() { PHOOL_VIRTUAL_WARNING; }
  virtual int isValid() const { PHOOL_VIRTUAL_WARNING; return 0; }
  virtual void identify(std::ostream& os=std::cout) const { PHOOL_VIRTUAL_WARNING; }

  virtual int get_track_id() const { PHOOL_VIRTUAL_WARN("get_track_id()"); return 0; }

  virtual SvtxTrackState* get_track_state() const { PHOOL_VIRTUAL_WARN("get_track_state()"); return 0; }

  virtual float get_likelihood(const PID_CANDIDATE candidate) { PHOOL_VIRTUAL_WARN("get_likelihood(const PID_CANDIDATE)"); return 0;};

  virtual void set_track_id(const int id) { PHOOL_VIRTUAL_WARN("set_track_id(const int id)");}

  virtual void set_track_state(SvtxTrackState* state) { PHOOL_VIRTUAL_WARN("set_track_state(const SvtxTrackState* state)");}

  virtual void set_likelihood(const PID_CANDIDATE candidate, const float candidate_prob) { PHOOL_VIRTUAL_WARN("set_likelihood(const PID_CANDIDATE, const float)");}


protected:
  PidInfo( int id ) {} // make sure nobody calls ctor of base class

  ClassDef(PidInfo,1)

};

#endif /*PIDINFO_H__ */
