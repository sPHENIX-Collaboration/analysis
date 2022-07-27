// Tell emacs that this is a C++ source
//  -*- C++ -*-.
#ifndef PROTOTYPE4_GENERICUNPACKPRDF_H
#define PROTOTYPE4_GENERICUNPACKPRDF_H

#include <fun4all/SubsysReco.h>

#include <map>
#include <string>
#include <utility>  // for pair

class PHCompositeNode;
class RawTowerContainer;

class GenericUnpackPRDF : public SubsysReco
{
 public:
  GenericUnpackPRDF(const std::string &detector);

  int InitRun(PHCompositeNode *topNode);

  int process_event(PHCompositeNode *topNode);

  void CreateNodeTree(PHCompositeNode *topNode);

  //! add stuff to be unpacked
  void add_channel(const int packet_id,  //! packet id
                   const int channel,    //! channel in packet
                   const int tower_id    //! output tower id
  );

 private:
  std::string _detector;

  //! packet_id, channel number to define a hbd_channel
  typedef std::pair<int, int> channel_typ;

  //! list of hbd_channel -> channel id which is also tower id
  typedef std::map<channel_typ, int> channel_map;

  channel_map _channel_map;

  // output -> Towers
  RawTowerContainer *_towers;
};

#endif
