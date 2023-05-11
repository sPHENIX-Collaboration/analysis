#include <iostream>
#include <fstream>
#include "sHelix.h"
#include "TMath.h"

//=================
sHelix::sHelix() :
  fR(0),
  fW(0),
  fC(0),
  fPhi(0),
  fX0(0),
  fY0(0),
  fZ0(0),
  fDebug(false)
{
}
//=================
sHelix::sHelix(float x0, float y0, float z0, float px, float py, float pz, float q, float b) { // [cm] [GeV] [e] [Tesla]
  fX0 = x0;
  fY0 = y0;
  fZ0 = z0;
  float qB = q*b; //[e]x[Tesla]
  fW = qB; //[angular frequency]
  fPhi = TMath::Pi()+TMath::ATan2(-py,-px); //[rad]
  float pt = TMath::Sqrt(px*px+py*py);
  float p = TMath::Sqrt(px*px+py*py+pz*pz);;
  fR = pt/(0.3*qB)*100; //[cm]
  fC = pz/pt*fR*fW; //[pitch]
  //std::cout << "sHelix:: p " << p << " | pt " << pt << " | phi " << fPhi*180/TMath::Pi() << std::endl;
  fDebug = false;
}
//=================
void sHelix::breakIntoPieces(float t1, float t2, float xx[100][3]) {
  // returns coordinates of 100 equidistant pieces of this track
  float dt = (t2-t1)/101;
  for(int n=0;n!=100; ++n) {
    float ti = t1+(n+1)*dt;
    xx[n][0] = x(ti);
    xx[n][1] = y(ti);
    xx[n][2] = z(ti);
  }
  return;
}
//=================
float sHelix::findFirstInterceptTo(float rd, float hz) {
  if(fDebug)
    std::cout << "***** findFirstInterceptTo r:" << rd << " z:" << hz << std::endl;
  // returns t of first intercept of the helix to a cylinder(r,hz);
  float t_1, t_2, t_3, t_4;

  // step 1: solving X^2 + Y^2 = r^2
  // equation: a cosx + b sinx + c = 0
  // a = - 2 R Y0 - 2 R^2 Cos(Phi)
  // b = 2 R X0 - 2 R^2 Sin(Phi)
  // c = 2 R^2 + X0^2 + Y0^2 - r^2 + 2 R ( Y0 Cos(Phi) - X0 Sin(Phi) )
  // x = Phi - W T
  // solution: (a^2 + b^2) sinx = -bc +- sqrt( a^2 (a^2+b^2-c^2) )
  float a = 2*fR*( fY0 - fR*TMath::Cos(fPhi) );
  float b = -2*fR*( fX0 + fR*TMath::Sin(fPhi) );
  float c = 2*fR*fR + fX0*fX0 + fY0*fY0 - rd*rd + 2*fR*(fX0*TMath::Sin(fPhi) - fY0*TMath::Cos(fPhi));
  //std::cout << "a =" << a << std::endl;
  //std::cout << "b =" << b << std::endl;
  //std::cout << "c =" << c << std::endl;
  if( a*a + b*b < c*c ) { // warranties a^2 + b^2 is more than zero
    if(fDebug) {
      std::cout << "FAIL: solution is not a real number\n";
      std::cout << "a*a =" << a*a << std::endl;
      std::cout << "b*b =" << b*b << std::endl;
      std::cout << "c*c =" << c*c << std::endl;
    }
    return 0;
  }
  float sinx_1 = ( -b*c + TMath::Sqrt(a*a*(a*a+b*b-c*c)) ) / ( a*a + b*b );
  float sinx_2 = ( -b*c - TMath::Sqrt(a*a*(a*a+b*b-c*c)) ) / ( a*a + b*b );
  if(fDebug) {
    std::cout << "Sinx_1 " << sinx_1 << std::endl;
    std::cout << "Sinx_2 " << sinx_2 << std::endl;
    //std::cout << "r = x_1 / CosPhi " << (-fR*sinx_1+fX0+fR*TMath::Sin(fPhi)) / TMath::Cos(fPhi) << std::endl;
    //std::cout << "r = x_2 / CosPhi " << (-fR*sinx_2+fX0+fR*TMath::Sin(fPhi)) / TMath::Cos(fPhi) << std::endl;
  }
  t_1=t_2=t_3=t_4=99999;
  // arc sin returns an angle from -Pi/4 to +Pi/4 so I created two more variables to extend to full 2pi
  float asinx_1, asinx_2, asinx_3, asinx_4;
  asinx_1 = TMath::ASin(sinx_1);
  asinx_2 = TMath::ASin(sinx_2);
  if(asinx_1<0) {
    asinx_3 = TMath::Pi() - asinx_1;
    asinx_1+=TMath::TwoPi();
  } else {
    asinx_3 = TMath::Pi() - asinx_1;
  }
  if(asinx_2<0) {
    asinx_4 = TMath::Pi() - asinx_2;
    asinx_2+=TMath::TwoPi();
  } else {
    asinx_4 = TMath::Pi() - asinx_2;
  }

  //phi-wt = asin
  if(fDebug) {
    std::cout << "PHI0 " << fPhi << std::endl;
    std::cout << "ASinx_1 " << asinx_1 << std::endl;
    std::cout << "ASinx_2 " << asinx_2 << std::endl;
    std::cout << "ASinx_3 " << asinx_3 << std::endl;
    std::cout << "ASinx_4 " << asinx_4 << std::endl;
  }

  t_1 = (fPhi - asinx_1)/fW;
  t_2 = (fPhi - asinx_2)/fW;
  t_3 = (fPhi - asinx_3)/fW;
  t_4 = (fPhi - asinx_4)/fW;

  if(fDebug) {
    std::cout << "IN R: | x(t) | y(t) || r(t) z(t)" << std::endl;
    std::cout << "t_1 " << t_1 << " | " << x(t_1) << " " << y(t_1) << " || " << r(t_1) << " " << z(t_1) << std::endl;
    std::cout << "t_2 " << t_2 << " | " << x(t_2) << " " << y(t_2) << " || " << r(t_2) << " " << z(t_2) << std::endl;
    std::cout << "t_3 " << t_3 << " | " << x(t_3) << " " << y(t_3) << " || " << r(t_3) << " " << z(t_3) << std::endl;
    std::cout << "t_4 " << t_4 << " | " << x(t_4) << " " << y(t_4) << " || " << r(t_4) << " " << z(t_4) << std::endl;
  }
  if(t_1<0||!TMath::AreEqualAbs(rd,r(t_1),0.1)) t_1 = 99999;
  if(t_2<0||!TMath::AreEqualAbs(rd,r(t_2),0.1)) t_2 = 99999;
  if(t_3<0||!TMath::AreEqualAbs(rd,r(t_3),0.1)) t_3 = 99999;
  if(t_4<0||!TMath::AreEqualAbs(rd,r(t_4),0.1)) t_4 = 99999;

  float t1 = TMath::Min( t_1, t_2 );
  t1 = TMath::Min( t1, t_3 );
  t1 = TMath::Min( t1, t_4 );

  // step 2: solving |z| = hz
  t_1 = ( hz - fZ0)/fC;
  t_2 = (-hz - fZ0)/fC;
  if(fDebug) {
    std::cout << "IN Z: | x(t) | y(t) || r(t) z(t)" << std::endl;
    std::cout << "t_1 " << t_1 << " | " << x(t_1) << " " << y(t_1) << " || " << r(t_1) << " " << z(t_1) << std::endl;
    std::cout << "t_2 " << t_2 << " | " << x(t_2) << " " << y(t_2) << " || " << r(t_2) << " " << z(t_2) << std::endl;
  }
  if(t_1<0||!TMath::AreEqualAbs(hz,z(t_1),0.1)) t_1 = 99999;
  if(t_2<0||!TMath::AreEqualAbs(hz,z(t_2),0.1)) t_2 = 99999;
  float t2 = TMath::Min( t_1, t_2 );
  float tt = TMath::Min(t1,t2);
  if(fDebug) std::cout << "  chosen time " << tt << std::endl;
  if(tt>4) tt = 99999;
  return tt;
}
//=================
void sHelix::SaveTracktoRootScript(float ri, float ro, float hz, char *filec) {
  float t1 = findFirstInterceptTo(ri,hz);
  float t2 = findFirstInterceptTo(ro,hz);
  float xx[100],yy[100],zz[100], xt[100],yt[100],zt[100];
  for(int t=0; t!=100; ++t) {
    float ti = t/float(100);
    xx[t] = x(ti);
    yy[t] = y(ti);
    zz[t] = z(ti);
    float tt = t1 + t/100.0*(t2-t1);
    xt[t] = x(tt);
    yt[t] = y(tt);
    zt[t] = z(tt);
  }
  std::ofstream out(filec);
  out << "{\n";
  out << "float x[100] = {";
  for(int t=0; t!=100; ++t) out << (t==0?"":",") << xx[t];
  out << "};\n";
  out << "float y[100] = {";
  for(int t=0; t!=100; ++t) out << (t==0?"":",") << yy[t];
  out << "};\n";
  out << "float z[100] = {";
  for(int t=0; t!=100; ++t) out << (t==0?"":",") << zz[t];
  out << "};\n";
  out << "float xt[100] = {";
  for(int t=0; t!=100; ++t) out << (t==0?"":",") << xt[t];
  out << "};\n";
  out << "float yt[100] = {";
  for(int t=0; t!=100; ++t) out << (t==0?"":",") << yt[t];
  out << "};\n";
  out << "float zt[100] = {";
  for(int t=0; t!=100; ++t) out << (t==0?"":",") << zt[t];
  out << "};\n";
  out << "TPolyLine3D *pol1 = new TPolyLine3D(100,x,y,z);\n";
  out << "TPolyLine3D *pol2 = new TPolyLine3D(100,xt,yt,zt);\n";
  out << "pol2->SetLineColor(kRed);\n";
  out << "pol2->SetLineWidth(3);\n";
  out << "pol1->Draw();\n";
  out << "pol2->Draw();\n";
  out << "}\n";
  out.close();
  return;
}
