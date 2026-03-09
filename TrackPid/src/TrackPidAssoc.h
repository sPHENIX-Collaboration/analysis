/**
 * @file trackbase/TrkrClusterHitAssoc.h
 * @author D. McGlinchey
 * @date June 2018
 * @brief Class for associating clusters to the hits that went into them
 */
#ifndef TRACKPID_TRACKPIDASSOC_H
#define TRACKPID_TRACKPIDASSOC_H

#include <phool/PHObject.h>

#include <iostream>          // for cout, ostream
#include <map>
#include <utility>           // for pair

/**
 * @brief Class for associating particle ID categories to tracks 
 *
 * Store the associations between particle ID categories and tracks 
 */
class TrackPidAssoc : public PHObject
{
public:
  //! typedefs for convenience
  typedef std::multimap<unsigned int, unsigned int> MMap;
  typedef MMap::iterator Iterator;
  typedef MMap::const_iterator ConstIterator;
  typedef std::pair<Iterator, Iterator> Range;
  typedef std::pair<ConstIterator, ConstIterator> ConstRange;
  //! ctor
  TrackPidAssoc();
  //! dtor
  virtual ~TrackPidAssoc();

  void Reset();

  void identify(std::ostream &os = std::cout) const;

  /**
   * @brief Add association between particle ID and track
   * @param[in] PID index
   * @param[in] Index of the track
   */
  void addAssoc(unsigned int pid, unsigned int trid);

  /**
   * @brief Get all the tracks associated with PID index
   * @param[in] pid particle id index
   * @param[out] Range of tracks associated with pid
   */
  ConstRange getTracks(unsigned int pid);

  static const unsigned int electron = 1;
  static const unsigned int hadron = 2;


private:
  MMap m_map;
  ClassDef(TrackPidAssoc, 1);
};

#endif // TRACKPID_TRACKPIDASSOC_H
