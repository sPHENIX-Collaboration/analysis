bool accept_electron( float eta, float p )
{
  float Emin = 3;

  if ( p > Emin )
    {
      if ( ( eta > -3 && eta < 1.1 ) ||
           ( eta > 1.45 && eta < 3.5 ) )
        {
          return true;
        }
    }

  return false;
}


bool accept_pion( float eta, float p )
{
  float pmin = 1;
  bool accept = false;

  if ( p > pmin )
    {
      /* Particle found within PID acceptance for Gas-RICH? */
      if ( ( eta > 1.45 && eta < 3.5 && p > 3 && p < 50 ) )
	accept = true;

      /* Particle found within PID acceptance for mRICH? */
      else if ( ( eta > 1.45 && eta < 2 && p < 6 ) )
	accept = true;

      /* Particle found within PID acceptance for DIRC? */
      else if ( ( eta > -1.1 && eta < 1.1 && p < 3.5 ) )
	accept = true;
    }

  return accept;
}


bool accept_kaon( float eta, float p )
{
  float pmin = 1;
  bool accept = false;

  if ( p > pmin )
    {
      /* Particle found within PID acceptance for Gas-RICH? */
      if ( ( eta > 1.45 && eta < 3.5 && p > 3 && p < 50 ) )
	accept = true;

      /* Particle found within PID acceptance for mRICH? */
      else if ( ( eta > 1.45 && eta < 2 && p < 6 ) )
	accept = true;

      /* Particle found within PID acceptance for DIRC? */
      else if ( ( eta > -1.1 && eta < 1.1 && p < 3.5 ) )
	accept = true;
    }

  return accept;
}
