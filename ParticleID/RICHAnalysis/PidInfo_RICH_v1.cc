#include "PidInfo_RICH_v1.h"

using namespace std;

PidInfo_RICH_v1::PidInfo_RICH_v1( const int id ):
  PidInfo( id ),
  _trackid( id ),
  _track_state( nullptr ),
  _pid_candidate_loglikelihood()
{}

void
PidInfo_RICH_v1::Reset()
{
  _pid_candidate_loglikelihood.clear();
}

float PidInfo_RICH_v1::get_likelihood(const PidInfo::PID_CANDIDATE candidate)
{
  std::map<PidInfo::PID_CANDIDATE,float>::const_iterator citer = _pid_candidate_loglikelihood.find(candidate);
  if (citer == _pid_candidate_loglikelihood.end()) return NAN;
  return citer->second;
}

/* //(for copy constructor:)

// copy over particle id probabilities
std::vector<PID_CANDIDATE> pid_candidates;
pid_candidates.push_back(SvtxTrack::ELECTRON);
pid_candidates.push_back(SvtxTrack::CHARGEDPION);
pid_candidates.push_back(SvtxTrack::CHARGEDKAON);
pid_candidates.push_back(SvtxTrack::PROTON);

_pid_candidate_loglikelihood.clear();

for (unsigned int i=0; i<pid_candidates.size(); ++i) {
if ( !isnan( track.get_pid_likelihood ( pid_candidates[i] ) ) )
set_pid_likelihood( pid_candidates[i], track.get_pid_likelihood( pid_candidates[i] ) );
}
*/
