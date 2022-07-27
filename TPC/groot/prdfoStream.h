#ifndef __PRDFOSTREAM_H
#define __PRDFOSTREAM_H

#include <EventTypes.h>
#include <Event.h>
#include <BufferConstants.h>


class prdfoStream {

public:

  //** Constructors

  prdfoStream(const char *filename, const int bufferlength = 4*1024*1024);

  virtual ~prdfoStream();

  int addEvent( Event *);

  int is_defunct() const { return defunct; };


 protected:

  //  add an end-of-buffer
  int addEoB();

  // now the write routine
  unsigned int writeout ();

  int prepare_next( const int iseq, const int irun=0);

  static unsigned int writen (int fd, char *ptr, const unsigned int nbytes);

  std::string _filename;

  typedef struct 
  { 
    int Length;
    int ID;
    int Bufseq;
    int Runnr;
    int data[1];
  } *buffer_ptr;

  int has_buffer;
  int buffer_sequence;

  buffer_ptr bptr;
  int *data_ptr;
  int current_index;
  int max_length;
  int max_size;
  int left;
  int has_end;

  int fd;
  int defunct;
};

#endif

