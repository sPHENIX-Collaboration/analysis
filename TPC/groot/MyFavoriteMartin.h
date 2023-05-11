#ifndef __MYFAVORITEMARTIN_H__
#define __MYFAVORITEMARTIN_H__

#include <Event.h>

int process_event (Event *e); //++CINT 
int prdfopen ( const char *filename);
int prdfclose ();

/*nr-th event function prototypes*/
int getFirstEventNr();
void setFirstEventNr(const int nr);
int b2G(int b);

#endif /* __MYFAVORITEMARTIN_H__ */
