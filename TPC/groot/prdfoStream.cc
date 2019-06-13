#include <prdfoStream.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "EvtConstants.h"

using namespace std;

// the constructor first ----------------
prdfoStream::prdfoStream (const char *filename, const int length)
{
  buffer_sequence = 1;

  _filename = filename;

  int *b = new int [length];
  bptr = ( buffer_ptr ) b;

  data_ptr = &(bptr->data[0]);
  max_length = length;   // in 32bit units
  max_size = max_length;
  bptr->ID = -64;
  bptr->Bufseq = buffer_sequence;

  bptr->Runnr = 0;
  has_buffer = 0;
  defunct = 0;
 
  // test if the file exists, do not overwrite
  fd =  open(_filename.c_str(), O_WRONLY | O_CREAT | O_EXCL | O_LARGEFILE , 
		  S_IRWXU | S_IROTH | S_IRGRP );
  if (fd < 0) 
    {
      cout << " error opening file " << _filename << endl;
      perror ( _filename.c_str());
      fd = 0;
      defunct = 1;
    }

}


// the destructor... ----------------
prdfoStream::~prdfoStream ()
{
  writeout();
  int *b = (int *)  bptr;
  delete [] b;
  if (fd) close (fd);
}



int prdfoStream::prepare_next( const int iseq
			  , const int irun)
{
  if (defunct) 
    {
      cout << "defunct oStream object " << endl;
      return -1;
    }

  // re-initialize the event header length
  bptr->Length =  BUFFERHEADERLENGTH*4;
  bptr->ID = -64;
  bptr->Bufseq = iseq;
  if (irun>0) bptr->Runnr = irun;

  current_index = 0;
  left = max_size - BUFFERHEADERLENGTH - EOBLENGTH; 
  has_end = 0;
  has_buffer =1;
  return 0;
}

// ---------------------------------------------------------
int prdfoStream::addEvent(Event * evt)
{
  if (defunct) 
    {
      cout << "defunct oStream object " << endl;
      return -1;
    }

  if ( ! has_buffer) prepare_next (buffer_sequence++, evt->getRunNumber() );

  int evtsize = evt->getEvtLength();

  if (evtsize*4 > left-EOBLENGTH) 
    {
      //cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++Prdf buffer being written" << endl;
      writeout();
    }
					     
  int nw;
  evt->Copy( &(bptr->data[current_index]), evtsize, &nw);
  left -= evtsize;
  current_index += evtsize;
  bptr->Length  += evtsize*4;
  return 0;
}


// ----------------------------------------------------------
int prdfoStream::addEoB()
{
  if (has_end) return -1;
  bptr->data[current_index++] = 2;  
  bptr->data[current_index++] = 0;
  bptr->Length  += 2*4;

  has_end = 1;
  //  cout << "addind EOB" << endl;
  return 0;
}

unsigned int prdfoStream::writeout ()
{
  if ( bptr->Length ==  BUFFERHEADERLENGTH*4) return 0;
  if (!has_end) addEoB();

  unsigned int bytes;

  int blockcount = ( bptr->Length + 8192 -1)/8192;
  int bytecount = blockcount*8192;
  bytes = writen ( fd, (char *) bptr , bytecount );
  has_buffer = 0;
  return bytes;
}


unsigned int prdfoStream::writen (int fd, char *ptr, const unsigned int nbytes)
{

  unsigned int nleft, nwritten;
  nleft = nbytes;
  while ( nleft>0 )
    {
      nwritten = write (fd, ptr, nleft);
      if ( nwritten < 0 ) 
	return nwritten;

      nleft -= nwritten;
      ptr += nwritten;
    }
  return (nbytes-nleft);
}
