#ifndef __INTTHOTMAP_H__
#define __INTTHOTMAP_H__

#include <vector>

class InttHotMap {
  public:
    InttHotMap() : fail_counter_(0), debug_(false) {}
    virtual ~InttHotMap() {}

    bool Readfile(const char* hotfile);
    void setDebug(bool flag) { debug_ = flag; }

    bool isHot(unsigned int felix, unsigned int ladder, unsigned int chip, unsigned int channel);

  class hotchannel {
    public:
      hotchannel(unsigned int Felix, unsigned int Ladder, unsigned int Chip, unsigned int Channel):
       felix(Felix), ladder(Ladder), chip(Chip), channel(Channel)
       {}

      unsigned int felix;  // 0-7
      unsigned int ladder; // 0-13
      unsigned int chip;   // 0-25
      unsigned int channel;// 0-127
  };

  private:
    std::vector<hotchannel> vHot;

    int fail_counter_;
    bool debug_;
};

#endif
