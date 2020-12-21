#ifndef __FILLBLOBHIST_H__
#define __FILLBLOBHIST_H__

void FillBlobHist();

//  Do the algebra right for tracks...
int line_circle(double m, double b, double r, double& Xi1, double& Yi1, double& Xi2, double& Yi2);
int line_circle(double X1, double Y1, double X2, double Y2, double r,  double& Xi1, double& Yi1, double& Xi2, double& Yi2);


#endif /*__FILLBLOBHIST_H__*/
