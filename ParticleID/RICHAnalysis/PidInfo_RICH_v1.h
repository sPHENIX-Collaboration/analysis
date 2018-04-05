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
  int isValid() const { return _pid_candidate_prob.size() > 0; }

  void identify(std::ostream& os=std::cout) const {
    os << "This is the PidInfo_RICH_v1 object" << std::endl;
  }

  int get_track_id() const { return _trackid; }

  float get_probability(const PidInfo::PID_CANDIDATE candidate);

  void set_track_id(const int id) { _trackid = id;}

  void  set_probability(const PID_CANDIDATE candidate, const float candidate_prob) { _pid_candidate_prob[candidate] = candidate_prob; }

private:
  float _trackid;

  std::map<PID_CANDIDATE,float> _pid_candidate_prob;

  ClassDef(PidInfo_RICH_v1,1)

};

#endif /*PIDINFOV1_H__ */
