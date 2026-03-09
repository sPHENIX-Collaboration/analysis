#define __CINT__

#ifdef __linux__
#include "/home/gnukazuk/soft/MyLibrary/include/MyLibrary.hh"
// #include "/home/gnukazuk/cernbox/Analysis_new/COMPASS_DY/common_macro/include/CommonInclude.hh"

#elif __APPLE__
// #include "/Users/nukazuka/local/MyLibrary/include/MyLibrary.hh"
#include "/Users/genki/soft/MyLibrary/include/MyLibrary.hh"
// #include "/Users/nukazuka/cernbox/Analysis_new/COMPASS_DY/common_macro/include/CommonInclude.hh"
// #include "/Users/gnukazuk/cernbox/Analysis_new/COMPASS_DY/common_macro/include/CommonInclude.hh"

#endif


bool IsInAcceptance( double z, double theta )
{
  double slope = TMath::Tan( theta );
  double intercept = -slope * z;

  double slope_min = 10 /( 20 - z );
  double slope_max = 10 / (-20 - z );

  if( slope_min < slope && slope < slope_max )
    return true;

  
  return false;
}

int theta_dist()
{
  TRandom3* randamer = new TRandom3( 0 );

  TH2D* hist = new TH2D( "z_theta", "z_{vtx} vs #theta;z_{vtx} (cm);#theta (degree);Entries", 1000, -50, 50, 360, 0, 360 );
  TH1D* hist_theta = new TH1D( "theta_dist", "#theta distribution;#theta (degree);Entries", 360, 0, 360 );
  TH1D* hist_z = new TH1D( "z_dist", "z_{vtx} distribution;z_{vtx} (cm);Entresi", 1000, -50, 50 );
  double z = -50;
  for( int i=0; i<1e2; i++ )
    {
      //double z = randamer->Gaus( 0, 10 );
      double theta = randamer->Uniform( 0,360 );
      theta = 14;
      // hist_theta->Fill( randamer->Uniform( 0,360 ) );
      // hist_z->Fill( randamer->Gaus( 0, 10 ) );
      hist->Fill(z, theta );

      cout << setw(10) << z << " "
	   << setw(10) << theta << " "
	   << IsInAcceptance( z, theta )
	   << endl;

      z += 1;
    }

  //  hist_theta->Draw();
  //hist_z->Draw();
  hist->Draw( "colz" );

  return 0;
}
