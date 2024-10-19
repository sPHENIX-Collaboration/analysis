#include "InttHotMap.h"

#include <iostream>
#include <fstream>

using namespace std;

bool InttHotMap::Readfile(const char* hotfile) {

  ifstream fin(hotfile);
  if(!fin){
    cout<<"failed to open : "<<hotfile<<endl;
    return false;
  }

  int felix, ladder, chip, channel, nhit;
  while(fin>>felix>>ladder>>chip>>channel>>nhit){
    hotchannel hot(felix, ladder, chip, channel);
    vHot.push_back(hot);
  }

  cout<<"Nhot channel : "<<vHot.size()<<endl;

  return true;
}

bool InttHotMap::isHot(unsigned int felix, unsigned int ladder, unsigned int chip, unsigned int channel) 
{
  for(auto itr=vHot.begin(); itr!=vHot.end(); ++itr){
    if(  itr->felix   == felix
      && itr->ladder  == ladder
      && itr->chip    == chip
      && itr->channel == channel) 
    {
      if(debug_ && fail_counter_<10) {
        cout<<"Hotchannel is removed : "<< felix <<" "<<ladder<<" "<<chip<<" "<<channel<<endl;
      }

      fail_counter_++;

      return true; 
    }
  }

  return false;
}

