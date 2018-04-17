#ifndef PIDINFOV1_H__
#define PIDINFOV1_H__

#include "PidInfo.h"
#include <vector>
#include <map>

class PidInfo_RICH_v1 : public PidInfo {

public:

  PidInfo_RICH_v1( const int );
  virtual ~PidInfo_RICH_v1() {}

  void Reset();
  int isValid() const { return _pid_candidate_loglikelihood.size() > 0; }

  void identify(std::ostream& os=std::cout) const {
    os << "This is the PidInfo_RICH_v1 object" << std::endl;
  }

  int get_track_id() const { return _trackid; }

  SvtxTrackState* get_track_state() const { return _track_state; }

  float get_likelihood(const PidInfo::PID_CANDIDATE candidate);

  void set_track_id(const int id) { _trackid = id;}

  void set_track_state(SvtxTrackState* state) { _track_state = state;}

  void  set_likelihood(const PID_CANDIDATE candidate, const float candidate_loglikelihood) { _pid_candidate_loglikelihood[candidate] = candidate_loglikelihood; }

private:
  float _trackid;

  SvtxTrackState* _track_state;

  std::map<PID_CANDIDATE,float> _pid_candidate_loglikelihood;

  ClassDef(PidInfo_RICH_v1,1)

};

#endif /*PIDINFOV1_H__ */
