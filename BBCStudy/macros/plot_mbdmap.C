// plot_mbdmap.C
//
// Plots the PMT layout of the MBD (south and north arms) using the
// channel positions from MbdGeomV2. Each PMT is drawn as a marker at
// its (x,y) location with the channel number labeled next to it.
//
// South channels: 0-63 (left plot)
// North channels: 64-127 (right plot)
//
// Usage:
//   root -b -q plot_mbdmap.C

R__LOAD_LIBRARY(libmbd.so)

#include <mbd/MbdGeomV2.h>

#include <TCanvas.h>
#include <TH2F.h>
#include <TLatex.h>
#include <TMarker.h>
#include <TMath.h>
#include <TPolyLine.h>
#include <TStyle.h>

#include <cmath>

// Finds the nearest-neighbor spacing among the given channel range, so the
// hexagons can be sized to exactly tile the PMT grid without overlapping.
double FindPitch(MbdGeom *mbdgeom, unsigned int chlo, unsigned int chhi)
{
  double mind = 1e9;
  for (unsigned int i = chlo; i < chhi; i++)
  {
    double xi = mbdgeom->get_x(i);
    double yi = mbdgeom->get_y(i);
    for (unsigned int j = chlo; j < chhi; j++)
    {
      if (i == j)
      {
        continue;
      }
      double dx = mbdgeom->get_x(j) - xi;
      double dy = mbdgeom->get_y(j) - yi;
      double d = std::sqrt((dx * dx) + (dy * dy));
      if (d < mind)
      {
        mind = d;
      }
    }
  }
  return mind;
}

// Draws a flat-top regular hexagon of circumradius R centered at (x,y).
void DrawHex(double x, double y, double R, int color = kBlue)
{
  const int npts = 7;
  double vx[npts];
  double vy[npts];
  for (int i = 0; i < 6; i++)
  {
    double theta = i * 60.0 * TMath::DegToRad();
    vx[i] = x + (R * std::cos(theta));
    vy[i] = y + (R * std::sin(theta));
  }
  vx[6] = vx[0];
  vy[6] = vy[0];

  TPolyLine *hex = new TPolyLine(npts, vx, vy);
  hex->SetLineColor(color);
  hex->Draw();
}

void plot_mbdmap()
{
  gStyle->SetOptStat(0);

  MbdGeomV2 *mbdgeom = new MbdGeomV2();

  // Nearest-neighbor PMT spacing is the same for both arms, so the hexagon
  // apothem (center-to-edge) is half that spacing, and the circumradius
  // (center-to-vertex) follows from the regular-hexagon geometry.
  double pitch = FindPitch(mbdgeom, 0, 64);
  double hex_apothem = pitch / 2.0;
  double hex_R = hex_apothem / std::cos(30.0 * TMath::DegToRad());

  TCanvas *c1 = new TCanvas("c1", "MBD PMT Map", 1200, 600);
  c1->Divide(2, 1);

  const double axrange = 15.0;

  // South (left pad)
  c1->cd(1);
  TH2F *h_south = new TH2F("h_south", "South MBD;x (cm);y (cm)",
                            100, -axrange, axrange, 100, -axrange, axrange);
  h_south->Draw();

  for (unsigned int ch = 0; ch < 64; ch++)
  {
    double x = mbdgeom->get_x(ch);
    double y = mbdgeom->get_y(ch);

    /*
    TMarker *m = new TMarker(x, y, 20);
    m->SetMarkerSize(1.2);
    m->Draw();
    */

    DrawHex(x, y, hex_R);

    TLatex *tl = new TLatex(x, y, Form("%u", ch));
    tl->SetTextSize(0.04);
    tl->SetTextAlign(22);
    tl->SetTextColor(kBlack);
    tl->Draw();
  }

  // North (right pad)
  c1->cd(2);
  TH2F *h_north = new TH2F("h_north", "North MBD;x (cm);y (cm)",
                            100, -axrange, axrange, 100, -axrange, axrange);
  h_north->Draw();

  for (unsigned int ch = 64; ch < 128; ch++)
  {
    double x = mbdgeom->get_x(ch);
    double y = mbdgeom->get_y(ch);

    /*
    TMarker *m = new TMarker(x, y, 20);
    m->SetMarkerSize(1.2);
    m->Draw();
    */

    DrawHex(x, y, hex_R);

    TLatex *tl = new TLatex(x, y, Form("%u", ch));
    tl->SetTextSize(0.04);
    tl->SetTextAlign(22);
    tl->SetTextColor(kBlack);
    tl->Draw();
  }

  c1->cd(0);
  c1->Update();
  c1->SaveAs("mbdmap.png");
}
