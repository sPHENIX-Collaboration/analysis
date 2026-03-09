#define clustEvent_cc

#include "clustEvent.hh"

clustEvent::clustEvent()
{
  colorset.push_back( kOrange - 3 );
  colorset.push_back( kOrange + 3 );
  colorset.push_back( kPink - 3 );
  colorset.push_back( kPink + 3 );
  colorset.push_back( kAzure - 3 );
  colorset.push_back( kAzure + 3 );
  colorset.push_back( kTeal - 3 );
  colorset.push_back( kTeal + 3 );
  colorset.push_back( kSpring - 3 );
  colorset.push_back( kSpring + 3 );
  colorset.push_back( kViolet - 3 );
  colorset.push_back( kViolet + 3 );
  
  // suggested by ChatGPT
  // (102, 255, 153)
  //   (255, 153, 102)
  //   (153, 102, 255)
  //   (255, 230, 102)
  //   (102, 204, 255)
}

void clustEvent::clear()
{
  for (auto itr = vtrack.begin(); itr != vtrack.end(); ++itr)
    {
      delete *itr;
    }
  vtrack.clear();
}

void clustEvent::makeonetrack()
{
  for (unsigned int i = 0; i < vtrack.size(); i++)
    {
      for (unsigned int j = i + 1; j < vtrack.size(); j++)
	{
	  bool share_point = ((vtrack[i]->p1 == vtrack[j]->p1) || (vtrack[i]->p2 == vtrack[j]->p2));
	  share_point = share_point || ((vtrack[i]->p1 == vtrack[j]->p2) || (vtrack[i]->p2 == vtrack[j]->p1));
	  if (share_point)
	    {
	      if (fabs(vtrack[i]->dca_2d) > fabs(vtrack[j]->dca_2d))
		{
		  vtrack[i]->is_deleted = true;
		  continue;
		}
	      else
		{
		  vtrack[j]->is_deleted = true;
		}
	    }
	}
    }
}

void clustEvent::dca_check(bool Debug)
{
  for (unsigned int i = 0; i < vtrack.size(); i++)
    {
      if (vtrack[i]->dca_2d > dca2d_max)
	vtrack[i]->dca2d_range_out = true;
      if (vtrack[i]->dca_2d < dca2d_min)
	vtrack[i]->dca2d_range_out = true;

      if (vtrack[i]->dca[2] > dcaz_max)
	vtrack[i]->dcaz_range_out = true;
      if (vtrack[i]->dca[2] < dcaz_min)
	vtrack[i]->dcaz_range_out = true;

      if (vtrack[i]->dcaz_range_out || vtrack[i]->dca2d_range_out)
	vtrack[i]->dca_range_out = true;

      // // if (Debug)
      // {
      //     if (vtrack[i]->dca_mean[2] > vtrack[i]->p1.z() && vtrack[i]->dca_mean[2] < vtrack[i]->p2.z())
      //         vtrack[i]->dca_range_out = true;
      //     if (vtrack[i]->dca_mean[2] > vtrack[i]->p2.z() && vtrack[i]->dca_mean[2] < vtrack[i]->p1.z())
      //         vtrack[i]->dca_range_out = true;
      // }
    }
}

void clustEvent::truth_check()
{
  for (unsigned int i = 0; i < vtruth.size(); i++)
    {
      double p = sqrt(vtruth[i]->m_truthpt * vtruth[i]->m_truthpt + vtruth[i]->m_truthpz * vtruth[i]->m_truthpz);

      if (vtruth[i]->m_status == 1 && fabs(vtruth[i]->m_trutheta) < 2 && p > (50 * 1e-3))
	{
	  if (fabs(vtruth[i]->m_truthz_out) < 22.8)
	    {
	      vtruth[i]->is_intt = true;
	    }
	}
    }
}

void clustEvent::cluster_check()
{
  for (unsigned int itrt = 0; itrt < vtruth.size(); itrt++)
    {
      for (unsigned int icls = 0; icls < vclus.size(); icls++)
	{
	  if (vclus[icls].layer < 2)
	    continue;

	  double d_phi = vclus[icls].getphi_clus() - vtruth[itrt]->m_truthphi;
	  d_phi -= -2 * (M_PI * (int)(d_phi / (M_PI)));

	  if (fabs(d_phi) < (0.0014 * 3))
	    {
	      vtruth[itrt]->have_cluster = true;
	      break;
	    }
	}
    }
}

void clustEvent::track_check()
{
  for (unsigned int i = 0; i < vtruth.size(); i++)
    {
      for (unsigned int itrk = 0; itrk < vtrack.size(); itrk++)
	{
	  double d_phi = vtrack[itrk]->getphi() - vtruth[i]->m_truthphi;
	  d_phi -= -2 * (M_PI * (int)(d_phi / (M_PI)));

	  if (fabs(d_phi) < (3.3e-4 * 3))
	    {
	      vtruth[i]->have_track = true;
	      break;
	    }
	}
    }
}

void clustEvent::charge_check()
{
  for (unsigned int i = 0; i < vtruth.size(); i++)
    {
      if ((abs(vtruth[i]->m_truthpid) != 22 && abs(vtruth[i]->m_truthpid) != 2112 && abs(vtruth[i]->m_truthpid) != 130 && abs(vtruth[i]->m_truthpid) != 310 && abs(vtruth[i]->m_truthpid) != 111))
	vtruth[i]->is_charged = true;
    }
}

void clustEvent::draw_frame(int mode, bool is_preliminary )
{
  TH1 *frame;
  string mag;

  if (mag_on)
    mag = "B-on";
  else
    mag = "B-off";

  string title;
  if( mode == 0 ) // x-y
    {
      if (run_no == 1)
	title = Form("x-y plane {MC(%s) event %d};x [cm];y [cm]", mag.c_str(), ievt);
      else
	title = Form("x-y plane {run%d(%s) event %d};x [cm];y [cm]", run_no, mag.c_str(), ievt);
      
      frame = gPad->DrawFrame(-15, -15, 15, 15, title.c_str());
      
      this->draw_signature( is_preliminary );
    }
  else if (mode == 1) // z-r
    {
      if (run_no == 1)
	title = Form("z-r plane {MC(%s) event %d};z [cm];r [cm]", mag.c_str(), ievt);
      else
	title = Form("z-r plane {run%d(%s) event %d};z [cm];r [cm]", run_no, mag.c_str(), ievt);

      //frame = gPad->DrawFrame(-25, -15, 25, 15, title.c_str());
      frame = gPad->DrawFrame(-30, -15, 30, 15, title.c_str());
      this->draw_date();
      
    }

  frame->GetXaxis()->CenterTitle();
  frame->GetYaxis()->CenterTitle();
  
}

string clustEvent::GetDate()
{
  return "9/13/2024";
  
  TDatime dt;
  int year	= dt.GetYear();
  int month	= dt.GetMonth();
  int day	= dt.GetDay();

  // format: mm/dd/yyyy
  std::stringstream ss;
  ss << month << "/" << day << "/" << year;

  return ss.str();
}

void clustEvent::draw_signature( bool is_preliminary = false )
{

  ///////////////////////////////////////////////////////////////////////////////
  // Writting words in the canvas                                              //
  ///////////////////////////////////////////////////////////////////////////////
  TLatex* tex = new TLatex();
  double line_height = 0.05;
  double first_margin = 0.005;
  double top_margin = 0.1;  
  double pos_y = 1.0 - top_margin + first_margin;// - line_height;
  pos_y = 1.0 - line_height + 0.004 + 0.01;

  // sPHENIX Internal or sPHENIX Prelimnary
  // pos_y -= line_height - first_margin + 0.025;
  double pos_x = 0.175;
  if( is_preliminary == false )
    {
      tex->DrawLatexNDC( pos_x, pos_y, "#it{#bf{sPHENIX}} Internal" );
    }
  else
    {
      if( bco_intt == 396907563635 ||
	  bco_intt == 396904684235 ||
	  bco_intt == 396904268555 )
	tex->DrawLatexNDC( pos_x, pos_y, "#it{#bf{sPHENIX}} Preliminary" );
    }

    // p+p 200 GeV
  pos_y -= line_height;
  tex->DrawLatexNDC( pos_x, pos_y,
		     ( string("Run-24 #it{p+p} 200 GeV Run" ) + to_string( run_no ) ).c_str()
		     );

  // tex->DrawLatexNDC( 0.2, pos_y, "Run-24 #it{p+p} 200 GeV" ); // no run number version 

}

void clustEvent::draw_date()
{
  TLatex* tex = new TLatex();
  double line_height = 0.05;
  double first_margin = 0.005;
  double top_margin = 0.1;  
  double pos_y = 1.0 - top_margin + first_margin;// - line_height;
  pos_y = 1.0 - line_height + 0.004 + 0.01;

  // top line
  if( run_no == 50889 )
    {
      //pos_y -= line_height;
      tex->DrawLatexNDC( 0.175, pos_y, "INTT Streaming readout" );

    }

  // second line
  pos_y -= line_height;
  // Date
  tex->DrawLatexNDC( 0.7, pos_y,
		     string("#it{" + GetDate() + "}").c_str() );

  // tex->DrawLatexNDC( 0.175, pos_y,
  // 		     (string("BCO ") + to_string( bco_intt ) ).c_str()
  // 		     );
  tex->DrawLatexNDC( 0.175, pos_y, "Single crossing" );

  return;
}

void clustEvent::draw_intt(int mode)
{

  const double kLayer_radii[4] = {7.1888, 7.800, 9.680, 10.330};
  if (mode == 0)
    {
      for (int i = 0; i < 4; i++)
	{
	  auto circle = new TEllipse(0.0, 0.0, kLayer_radii[i]);
	  circle->SetLineColorAlpha(kGray, 0.5);
	  circle->SetLineWidth(2);
	  circle->SetFillStyle(0);
	  circle->Draw("same");
	}
    }
  else if (mode == 1)
    {
      for (int j = 0; j < 2; j++)
	{
	  for (int i = 0; i < 4; i++)
	    {
	      TLine *line = new TLine(-22.8, (2 * j - 1) * kLayer_radii[i], 22.8, (2 * j - 1) * kLayer_radii[i]);
	      line->SetLineColorAlpha(kGray, 0.5);
	      line->SetLineWidth(2);
	      line->Draw("same");
	    }
	}
    }
}

void clustEvent::draw_tracklets(int mode = 0, bool does_overlay = false, int color = kBlack, bool dca_range_cut = false, bool is_deleted = false, bool reverse = false)
{

  bool condition = !reverse;
  for (unsigned int i = 0; i < vtrack.size(); i++)
    {
      if (vtrack[i]->dca_range_out == condition && dca_range_cut == true)
	{
	  continue;
	}

      if (vtrack[i]->is_deleted == condition && is_deleted == true)
	{
	  continue;
	}

      TGraph *g_temp = new TGraph();
      g_temp->SetMarkerColor(color);
      g_temp->SetMarkerStyle(20);
      g_temp->SetMarkerSize(0.8);

      if (mode == 0)
	{
	  g_temp->SetPoint(0, vtrack[i]->p1.x(), vtrack[i]->p1.y());
	  g_temp->SetPoint(1, vtrack[i]->p2.x(), vtrack[i]->p2.y());
	}
      else if (mode == 1)
	{
	  g_temp->SetPoint(0, vtrack[i]->p1.z(), vtrack[i]->getpointr(1));
	  g_temp->SetPoint(1, vtrack[i]->p2.z(), vtrack[i]->getpointr(2));
	}
      g_temp->Draw("PLsame");
    }
}

void clustEvent::draw_trackline(int mode,
				bool does_overlay,
				//int color,
				bool dca_range_cut,
				bool is_deleted,
				bool reverse)
{
  bool condition = !reverse;    
  int color_ = 0;

  bool is_first = true;
  for (unsigned int i = 0; i < vtrack.size(); i++)
    {

      if (vtrack[i]->dca_range_out == condition && dca_range_cut == true)
	{
	  continue;
	}

      if (vtrack[i]->is_deleted == condition && is_deleted == true)
	{
	  continue;
	}
	
      color_ = color_ % colorset.size();

      TGraph *g_temp = new TGraph();
      g_temp->SetMarkerColor(colorset[color_]);
      // g_temp->SetLineColor(color);
      g_temp->SetMarkerStyle(20);
      g_temp->SetMarkerSize(0.8);

      if (mode == 0)
	{
	  vtrack[i]->track_xy->SetLineColorAlpha(colorset[color_], 0.5);
	  // vtrack[i]->track_xy->SetLineColorAlpha(color, 0.5);
	  if (vtrack[i]->dca_mean[0] < vtrack[i]->p1.x())
	    {
	      vtrack[i]->track_xy->SetRange(vtrack[i]->dca_mean[0], 15);
	    }
	  else
	    {
	      vtrack[i]->track_xy->SetRange(-15, vtrack[i]->dca_mean[0]);
	    }

	  vtrack[i]->track_xy->Draw("Lsame");
	  g_temp->SetPoint(0, vtrack[i]->dca_mean[0], vtrack[i]->dca_mean[1]);
	  g_temp->SetPoint(1, vtrack[i]->p1.x(), vtrack[i]->p1.y());
	  g_temp->SetPoint(2, vtrack[i]->p2.x(), vtrack[i]->p2.y());
	}
      else if (mode == 1)
	{


	  if( isinf(vtrack[i]->track_rz->GetParameter(1)) )
	    {
	      TLine *line = nullptr;
	      if (vtrack[i]->p1.y() > 0)
		line = new TLine(vtrack[i]->dca_mean[2], vtrack[i]->getpointr(3), vtrack[i]->dca_mean[2], 15);
	      else
		line = new TLine(vtrack[i]->dca_mean[2], -15, vtrack[i]->dca_mean[2], vtrack[i]->getpointr(3));

	      // line->SetLineColorAlpha(color, 0.5);
	      line->SetLineColorAlpha(colorset[color_], 0.5);
	      line->SetLineWidth(2);
	      line->Draw("same");
	    }
	  else
	    {

	      vtrack[i]->track_rz->SetLineColorAlpha(colorset[color_], 0.5);
	      // vtrack[i]->track_rz->SetLineColorAlpha(color, 0.5);

	      int y = 0;
	      if (vtrack[i]->p1.y() > 0)
		y = 15;
	      else
		y = -15;

	      double r1 = vtrack[i]->track_rz_inv->Eval(vtrack[i]->getpointr(3));
	      double r2 = vtrack[i]->track_rz_inv->Eval(y);

	      if (r1 <= r2)
		vtrack[i]->track_rz->SetRange(r1, r2);
	      else
		vtrack[i]->track_rz->SetRange(r2, r1);

	      vtrack[i]->track_rz->Draw("Lsame");
	    }

	  if( is_first ) // drawing graph only once is enough for z-r
	    {
	      g_temp->SetPoint(0, vtrack[i]->dca_mean[2], vtrack[i]->getpointr(3));
	      is_first = false;
	    }

	  // draw hit strip
	  vector < TLine * > zlines;
	  for (int j = 0; j < 2; j++)
	    {
	      TLine *line = new TLine(vtrack[i]->zline[j][0],
				      vtrack[i]->getpointr(1 + j),
				      vtrack[i]->zline[j][1],
				      vtrack[i]->getpointr(1 + j) );
	      line->SetLineColor(colorset[color_]);
	      line->SetLineWidth(3);
	      zlines.push_back(line);
	    }

	  for ( auto& line : zlines )
	    line->Draw("same");
	  
	  // g_temp->SetPoint(1, vtrack[i]->p1.z(), vtrack[i]->getpointr(1));
	  // g_temp->SetPoint(2, vtrack[i]->p2.z(), vtrack[i]->getpointr(2));
	}
    
      if( g_temp->GetN() > 0 )
	g_temp->Draw("Psame");
      
      color_++;

      if( color_ >= colorset.size() )
	color_ = 0;
	  
    }

}


double clustEvent::rad_deg(double Rad)
{
  Rad = Rad / M_PI * 180;
  if (Rad < 0)
    Rad += 360;

  return Rad;
}

void clustEvent::draw_curve2(int mode, double px, double py, double pz, double rad, double cx, double cy, int color, const vector<double> &V)
{
  if (V.size() == 2)
    {
      // cout<<"cross : "<<V.at(0)<<", "<<V.at(1)<<endl;
      double phi1 = atan2(V.at(1) - cy, V.at(0) - cx);
      phi1 = rad_deg(phi1);

      double phic = atan2(dca_mean[1] - cy, dca_mean[0] - cx);
      phic = rad_deg(phic);
      double phi_min, phi_max;

      phi_min = (phi1 < phic) ? phi1 : phic;
      phi_max = (phi1 < phic) ? phic : phi1;
      TEllipse *circle = nullptr;
      // TEllipse *circle2 = nullptr;
      if ((phi_max - phi_min) > 180)
	circle = new TEllipse(cx, cy, rad, 0, -(360 - phi_max), phi_min);
      else
	circle = new TEllipse(cx, cy, rad, 0, phi_min, phi_max);

      if (mode == 0)
	{
	  circle->SetLineColorAlpha(color, 0.5);
	  circle->SetFillStyle(0);
	  circle->SetLineWidth(2);
	  circle->SetLineStyle(1);
	  circle->SetNoEdges(1);
	  circle->Draw("same");
	}
      else if (mode == 1)
	{
	  circle->SetLineColorAlpha(color, 0.9);
	  circle->SetFillStyle(0);
	  circle->SetLineWidth(3);
	  circle->SetLineStyle(9);
	  circle->SetNoEdges(1);
	  circle->Draw("same");
	}
    }

  // TEllipse *circle2 = new TEllipse(cx, cy, rad, 0, 0, 360);
  // circle2->SetLineColorAlpha(kGray, 0.2);
  // circle2->SetFillStyle(0);
  // circle2->SetLineWidth(2);
  // circle2->SetLineStyle(1);
  // circle2->SetNoEdges(1);
  // circle2->Draw("same");
}

void clustEvent::draw_trackcurve( int mode,
				  bool does_overlay,
				  bool dca_range_cut,
				  bool is_deleted,
				  bool reverse )
{
  int color_ = 0;
  bool condition = !reverse;

  for (unsigned int i = 0; i < vtrack.size(); i++ )
    {
      if( color_ == colorset.size() )
	color_ = 0;
      
      if (vtrack[i]->dca_range_out == condition && dca_range_cut == true)
	{
	  continue;
	}

      if (vtrack[i]->is_deleted == condition && is_deleted == true)
	{
	  continue;
	}
      
      color_ = color_ % colorset.size();

      TGraph *g_temp = new TGraph();
      g_temp->SetMarkerColor(colorset[color_]);
      g_temp->SetMarkerStyle(20);
      g_temp->SetMarkerSize(0.8);

      if (mode == 0)
	{
	  vector < double > cross_framepoint = get_crossframe(vtrack[i]->cx, vtrack[i]->cy, vtrack[i]->rad, vtrack[i]->p1.x(), vtrack[i]->p1.y());
	  // cout<<"cross_framepoint.size() : "<<cross_framepoint.size()<<endl;
	  // if(cross_framepoint.size() == 2)
	  // cout<<"cross : "<<cross_framepoint.at(0)<<", "<<cross_framepoint.at(1)<<endl;
	  draw_curve2(0,
		      vtrack[i]->p_reco[0],
		      vtrack[i]->p_reco[1],
		      vtrack[i]->p_reco[2],
		      vtrack[i]->rad,
		      vtrack[i]->cx,
		      vtrack[i]->cy,
		      colorset[color_],
		      cross_framepoint);

	  // cout<<"p1 : "<<vtrack[i]->p1.x()<<"  "<<vtrack[i]->p1.y()<<endl;

	  g_temp->AddPoint( vtrack[i]->dca_mean[0], vtrack[i]->dca_mean[1]);
	  g_temp->AddPoint( vtrack[i]->p1.x(), vtrack[i]->p1.y());
	  g_temp->AddPoint( vtrack[i]->p2.x(), vtrack[i]->p2.y());

	  // cout << "cross_emc_size : " << vtrack[i]->cross_emc.size() << endl;
	}

      color_++;
      if( color_ >= colorset.size() )
	color_ = 0;

      g_temp->Draw("Psame");
    }
}

void clustEvent::draw_truthcurve(int mode = 0, bool does_overlay = false, int color = kBlack, bool only_far_cluster = false, bool only_far_track = false)
{
  for (unsigned int i = 0; i < vtruth.size(); i++)
    {
      if (vtruth[i]->is_intt == false)
	continue;

      if (vtruth[i]->is_charged == false)
	continue;

      if (vtruth[i]->have_track == true && only_far_track)
	continue;

      if (vtruth[i]->have_cluster == true && only_far_cluster)
	continue;

      TGraph *g_temp = new TGraph();
      g_temp->SetMarkerColor(color);
      g_temp->SetLineColor(color);
      g_temp->SetMarkerStyle(20);
      g_temp->SetMarkerSize(0.8);

      if (mode == 0)
	{
	  vector<double> cross_framepoint = get_crossframe(vtruth[i]->center[0], vtruth[i]->center[1], vtruth[i]->m_rad, vtruth[i]->m_truthpx, vtruth[i]->m_truthpy);
	  draw_curve2(1, vtruth[i]->m_truthpx, vtruth[i]->m_truthpy, vtruth[i]->m_truthpz, vtruth[i]->m_rad, vtruth[i]->center[0], vtruth[i]->center[1], color, cross_framepoint);

	  g_temp->SetPoint(0, vtruth[i]->m_xvtx, vtruth[i]->m_yvtx);
	}

      g_temp->Draw("Psame");
    }
}

void clustEvent::draw_truthline(int mode = 0, bool does_overlay = false, int color = kBlack, bool only_far_cluster = false, bool only_far_track = false)
{
  for (unsigned int i = 0; i < vtruth.size(); i++)
    {
      if (vtruth[i]->is_intt == false)
	continue;

      if (vtruth[i]->is_charged == false)
	continue;

      if (vtruth[i]->have_track == true && only_far_track)
	continue;

      if (vtruth[i]->have_cluster == true && only_far_cluster)
	continue;

      TGraph *g_temp = new TGraph();
      g_temp->SetMarkerColor(color);
      g_temp->SetLineColor(color);
      g_temp->SetMarkerStyle(20);
      g_temp->SetMarkerSize(0.8);

      if (mode == 0)
	{
	  vtruth[i]->truth_xy->SetLineColorAlpha(color, 0.8);
	  vtruth[i]->truth_xy->SetLineStyle(9);
	  vtruth[i]->truth_xy->SetLineWidth(3);

	  if (vtruth[i]->m_truthpx > 0)
	    {
	      vtruth[i]->truth_xy->SetRange(vtruth[i]->m_xvtx, 15);
	    }
	  else
	    {
	      vtruth[i]->truth_xy->SetRange(-15, vtruth[i]->m_xvtx);
	    }
	  vtruth[i]->truth_xy->Draw("Lsame");

	  g_temp->SetPoint(0, vtruth[i]->m_xvtx, vtruth[i]->m_yvtx);
	}
      else if (mode == 1)
	{
	  vtruth[i]->truth_rz->SetLineColorAlpha(color, 0.8);
	  vtruth[i]->truth_rz->SetLineStyle(9);
	  vtruth[i]->truth_rz->SetLineWidth(3);

	  int y = 0;
	  if (vtruth[i]->m_truthpy > 0)
	    y = 15;
	  else
	    y = -15;
	  double r_min = vtruth[i]->truth_rz->GetX(vtruth[i]->getpointr(3));
	  double r_max = vtruth[i]->truth_rz->GetX(y);
	  if (r_min < r_max)
	    vtruth[i]->truth_rz->SetRange(r_min, r_max);
	  else
	    vtruth[i]->truth_rz->SetRange(r_max, r_min);

	  vtruth[i]->truth_rz->Draw("Lsame");
	  g_temp->SetPoint(0, vtruth[i]->m_zvtx, vtruth[i]->getpointr(3));
	}
      g_temp->Draw("Psame");
    }
}

void clustEvent::draw_clusters(int mode, bool does_overlay, int color )
{
  for (unsigned int i = 0; i < vclus.size(); i++)
    {
      TGraph *g_temp = new TGraph();
      g_temp->SetMarkerColorAlpha(color, 0.5);
      // g_temp->SetLineColor(color);
      g_temp->SetMarkerStyle(20);
      g_temp->SetMarkerSize(0.8);
      
      if (mode == 0)
	{
	  g_temp->SetPoint(0, vclus[i].x, vclus[i].y);
	  g_temp->Draw("psame");
	}
      else if (mode == 1)
	{
	  TLine *line = new TLine( vclus[i].zline[0], vclus[i].r, vclus[i].zline[1], vclus[i].r );
	  line->SetLineColorAlpha(color, 0.5);
	  line->SetLineWidth(3);
	  line->Draw("same");
	  // g_temp->SetPoint(0, vclus[i].z, vclus[i].r);
	}
      // g_temp->Draw("psame");
    }
}

//////////////////////////////////
//////////////////////////////////

// vector<double> get_crossline(double x1, double y1, double r1)
// {
//     vector<double> cross_co;
//     // ax + by + c = 0
//     cross_co.push_back(2 * (cx - x1));                                                           // a
//     cross_co.push_back(2 * (cy - y1));                                                           // b
//     cross_co.push_back((x1 + cx) * (x1 - cx) + (y1 + cy) * (y1 - cy) + (rad + r1) * (rad - r1)); // c

//     return cross_co;
// }

vector<double> clustEvent::get_crosspoint(const vector<double> &V, double cx, double cy, double rad, double p1_x, double p1_y) // ax + by + c = 0
{
  vector<double> cross;
  double a = V[0];
  double b = V[1];
  double c = V[2];

  double d = fabs(a * cx + b * cy + c) / sqrt(a * a + b * b);
  // cout << "d ; " << d << " " << rad << endl;

  double theta = atan2(b, a);

  if (d > rad)
    {
      // cout << "d > rad" << endl;
    }
  else if (d == rad)
    {
      // cout << "d == rad" << endl;

      if (a * cx + b * cy + c > 0)
	theta += M_PI;
      cross.push_back(rad * cos(theta) + cx);
      cross.push_back(rad * sin(theta) + cy);
    }
  else
    {
      // cout << "d < rad" << endl;
      double alpha, beta, phi;
      phi = acos(d / rad);
      alpha = theta - phi;
      beta = theta + phi;
      if (a * cx + b * cy + c > 0)
	{
	  alpha += M_PI;
	  beta += M_PI;
	}
      // double temp_cross[2][2];
      vector<vector<double>> temp_cross = {{rad * cos(alpha) + cx, rad * cos(beta) + cx}, {rad * sin(alpha) + cy, rad * sin(beta) + cy}};
      // for (unsigned int j = 0; j < temp_cross.at(0).size(); j++)
      // {
      //     cout << "temp_cross : ";
      //     for (unsigned int i = 0; i < temp_cross.size(); i++)
      //         cout << temp_cross.at(j).at(i) << "  ";
      //     cout << endl;
      // }
      cross = get_closestpoint(temp_cross, p1_x, p1_y);
    }
  // cout << "cross size : " << cross.size() << endl;
  // for (int i = 0; i < cross.size(); i++)
  //     cout << cross[i] << endl;
  return cross;
}

vector<double> clustEvent::get_closestpoint(const vector<vector<double>> VV, double p1_x, double p1_y)
{
  vector<double> closest;
  double pre_phi = 999;
  double phi_p1 = atan2(p1_y - dca_mean[1], p1_x - dca_mean[0]);
  for (unsigned int i = 0; i < VV.at(0).size(); i++)
    {
      // cout << "dca_mean : " << dca_mean[0] << "  " << dca_mean[1] << endl;
      // cout << "VV : " << VV[0][i] << "  " << VV[1][i] << endl;
      // cout << "pVV : " << VV[0][i] - dca_mean[0] << "  " << VV[1][i] - dca_mean[1] << endl;
      // cout << "p1  : " << p1.x() - dca_mean[0] << "  " << p1.y() - dca_mean[1] << endl;
      // cout << VV.at(i).at(0) << endl;
      double dot = (VV[0][i] - dca_mean[0]) * (p1_x - dca_mean[0]) + (VV[1][i] - dca_mean[1]) * (p1_y - dca_mean[1]);
      double temp_phi = atan2(VV[1][i] - dca_mean[1], VV[0][i] - dca_mean[0]);
      // cout << "dot : " << dot << endl;
      if (dot > 0)
	{
	  if (fabs(temp_phi - phi_p1) < fabs(pre_phi - phi_p1))
	    {
	      closest.erase(closest.begin(), closest.end());
	      // cout << "VV size : " << VV.size() << " " << VV.at(0).size() << endl;
	      for (int j = 0; j < 2; j++)
		closest.push_back(VV[j][i]);
	      pre_phi = temp_phi;
	    }
	}
    }
  // closest = temp_closest;
  // for (unsigned int i = 0; i < VV.at(0).size(); i++)
  // {
  //     for (int j = 0; j < 2; j++)
  //         closest.push_back(VV[j][i]);
  // }
  // for (unsigned int i = 0; i < closest.size(); i++)
  //  // cout << closest[i] << endl;

  return closest;
}

vector<double> clustEvent::get_crossframe(double cx, double cy, double rad, double p1_x, double p1_y)
{
  // cout << endl;
  // cout << "calc frame" << endl;
  double size = 15;
  vector<vector<double>> crossframe(2, vector<double>(0));
  for (int j = 0; j < 2; j++)
    {
      for (int i = 0; i < 2; i++)
	{
	  double a_ = (j == 0) ? 1 : 0;
	  double b_ = (j == 0) ? 0 : 1;
	  double c_ = (2 * i - 1) * size;
	  // cout << "kesu : " << a_ << " " << b_ << " " << c_ << endl;
	  vector<double> co_ = {a_, b_, c_};
	  vector<double> temp_crossframe = get_crosspoint(co_, cx, cy, rad, p1_x, p1_y);
	  if (temp_crossframe.size() == 2)
	    {
	      // cout << "temp_crossframe : " << temp_crossframe[0] << "  " << temp_crossframe[1] << endl;
	      crossframe.at(0).push_back(temp_crossframe[0]);
	      crossframe.at(1).push_back(temp_crossframe[1]);
	    }
	}
    }
  // cout << "crossframe" << endl;
  // cout << "size : " << crossframe.size() << "  " << crossframe.at(0).size() << endl;
  for (int j = 0; j < crossframe.at(0).size(); j++)
    {
      for (int i = 0; i < crossframe.size(); i++)
	{
	  // cout << crossframe[i][j] << ", ";
	}
      // cout << endl;
    }
  vector<double> result;
  if (crossframe.at(0).size() != 0)
    result = get_closestpoint(crossframe, p1_x, p1_y);
  // cout << "closest frame" << endl;
  for (int i = 0; i < result.size(); i++)
    {
      // cout << result[i] << ", ";
    }
  // cout << endl;
  // cout << "calc end" << endl;

  return result;
}

int clustEvent::GetGoodTrackNum( bool dca_range_cut,
				 bool is_deleted,
				 bool reverse )

{
  bool condition = !reverse;
  int counter = 0;
  for (unsigned int i = 0; i < vtrack.size(); i++ )
    {
      
      if (vtrack[i]->dca_range_out == condition && dca_range_cut == true)
	{
	  continue;
	}
      
      if (vtrack[i]->is_deleted == condition && is_deleted == true)
	{
	  continue;
	}
      
      counter++;
    }
  
  return counter;
}

double clustEvent::GetTrackLeftRightAsymmetry()
{
  int num_right = 0, num_left = 0;
  for( auto& track : vtrack )
    {
      double phi = track->getphi_tracklet(); // in rad

      //          phi = pi/2
      //                   |
      //           up-left | up-right
      //                   |
      //                   |
      // phi = pi ---------+--------- phi = 0
      //                   |
      //                   |
      //         down-left | down-right
      //                   |
      //                   phi = 3 pi / 4
      //
      // LR asymmetry = (L - R) / (L + R)
      // UD asymmetry = (U - D) / (U + D)
      //
      if( 0 <= phi && phi < TMath::Pi()/2 )
	num_right++;
      else if( 3.0 * TMath::Pi()/2 < phi )
	num_right++;
      else
	num_left++;
    }

  if( vtrack.size() == 0 )
    return 9999;

  if( num_right == 0 )
    return -100;
  else if( num_left == 0 )
    return 100;

  
  return 1.0 * (num_left - num_right) / (num_left + num_right);
}

double clustEvent::GetTrackUpDownAsymmetry()
{
  int num_up = 0, num_down = 0;
  for( auto& track : vtrack )
    {
      double phi = track->getphi_tracklet(); // in rad

      if( 0 <= phi && phi < TMath::Pi() )
	num_up++;
      else
	num_down++;
    }

  if( vtrack.size() == 0 )
    return 9999;

  if( num_up == 0 )
    return -100;
  else if( num_down == 0 )
    return 100;

  
  return 1.0 * (num_up - num_down) / (num_up + num_down);
}


void clustEvent::SetTrackInfoToCluster( ) // int i, bool flag, double theta, double phi )
{

  for( auto& track : vtrack )
    {
      for( auto& index : track->cluster_ids )
	{
	  vclus[index].is_associated = true; // flag;
	  vclus[index].track_incoming_theta = track->track_theta;
	  vclus[index].track_incoming_phi = 0.0; // not done yet
	    
	}
    }

  return;
}

void clustEvent::Print()
{
  cout << "+" << string( 100, '-' ) << "+" << endl;
  cout << "| " << "Event: " << ievt << endl;
  cout << "| " << "Magnet: " << mag_on << endl;
  cout << "| " << "#cluster: " << vclus.size() << endl;
  cout << "| " << "#truth track: " << vtruth.size() << endl;
  cout << "| " << "#track: " << vtrack.size() << endl;
  cout << "| " << "DCA: "
       << "x = " << dca_mean[0] << ", " 
       << "y = " << dca_mean[1] << ", " 
       << "z = " << dca_mean[2] << endl;
  cout << "+" << string( 100, '-' ) << "+" << endl;

}
