#ifndef PIDINFOCONTAINER_H__
#define PIDINFOCONTAINER_H__

#include <phool/PHObject.h>
#include <phool/phool.h>
#include <iostream>
#include <map>

class PidInfo;

class PidInfoContainer : public PHObject
{

public:

  typedef std::map<int, PidInfo *> Map;
  typedef Map::iterator Iterator;
  typedef Map::const_iterator ConstIterator;
  typedef std::pair<Iterator, Iterator> Range;
  typedef std::pair<ConstIterator, ConstIterator> ConstRange;

  PidInfoContainer() {}
  virtual ~PidInfoContainer() {}

  void Reset();
  int isValid() const;
  void identify(std::ostream& os=std::cout) const;
  ConstIterator AddPidInfo(PidInfo *pidinfo);
  PidInfo *getPidInfo(const int trackid);
  //! return all pid infos
  ConstRange getPidInfos( void ) const;
  Range getPidInfos( void );

  unsigned int size() const {return _pidinfos.size();}

protected:
  Map _pidinfos;

  ClassDef(PidInfoContainer,1)
};

#endif /* PIDINFOCONTAINER_H__ */
