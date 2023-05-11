#ifndef __FILLHOUGHHIST_H__
#define __FILLHOUGHHIST_H__

void FillHoughHist();

double inverseSlope(double x1, double y1, double x2, double y2);
double intercept   (double x1, double y1, double x2, double y2);

#define Med_inverseSlope -0.0018
#define Med_Offset 426.526
#define MAD_inverseSlope 0.0091081
#define MAD_Offset 426.528
#define HFACTOR 5.0 // Number of MADs that go into a single hough cell, 5.0 is kinda like +/- 2 sigma.

#endif /*__FILLHOUGHHIST_H__*/
