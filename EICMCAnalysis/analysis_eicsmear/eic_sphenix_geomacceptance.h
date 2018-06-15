bool accept_electron( float eta, float p )
{
  float Emin = 3;

  if ( p > Emin )
    {
      if ( ( eta > -4 && eta < 4 ) )
        {
          return true;
        }
    }

  return false;
}


bool accept_pion( float eta, float p )
{
  float pmin = 1; // GeV
  bool accept = false;

  if ( p > pmin )
    {
      /* Particle found within PID acceptance for e-going mRICH? */
      if ( ( eta > 1.242 && eta < 1.85 && p < 6 ) )
	accept = true;

      /* Particle found within PID acceptance for barrel DIRC? */
      else if ( ( eta > -1.55 && eta < 1.242 && p < 6 ) )
	accept = true;

      /* Particle found within PID acceptance for h-going Gas-RICH? */
      else if ( ( eta > 1.242 && eta < 4 && p > 3 && p < 50 ) )
	accept = true;

      /* Particle found within PID acceptance for h-going mRICH? */
      else if ( ( eta > 1.242 && eta < 1.85 && p < 6 ) )
	accept = true;

      /* Particle found within PID acceptance for h-going TOF? */
      else if ( ( eta > 1.85 && eta < 4 && p < 5 ) )
	accept = true;
    }

  return accept;
}


bool accept_kaon( float eta, float p )
{
  bool accept = false;

  accept = accept_pion( eta, p );

  return accept;
}
