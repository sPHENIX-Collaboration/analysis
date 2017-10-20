/**
 * Create an outline for a magnet and represent it as a ROOT PolyLine- allow for rotation
 */
TPolyLine* TraceBox( float angle, float center_z, float center_x, float length, float aperture_radius, float outer_radius )
{
  /* center of magnet box */
  TVector3* vcenter = new TVector3( center_z,
				    center_x,
				    0.0 );

  /* fill corner positions in 3D vector TVector objects for easier rotation */
  const unsigned npts = 5;
  TObjArray a;

  /* corner points in unrotated coordinate system */
  a.AddLast( new TVector3( 0.0 - ( length / 2. ),
			   0.0 + outer_radius,
			   0.0 ) );

  a.AddLast( new TVector3( 0.0 + ( length / 2. ),
			   0.0 + outer_radius,
			   0.0 ) );

  a.AddLast( new TVector3( 0.0 + ( length / 2. ),
			   0.0 + aperture_radius,
			   0.0 ) );

  a.AddLast( new TVector3( 0.0 - ( length / 2. ),
			   0.0 + aperture_radius,
			   0.0 ) );

  a.AddLast( new TVector3( 0.0 - ( length / 2. ),
			   0.0 + outer_radius,
			   0.0 ) );

  /* loop over array and rotate points coordinate system and move to real center */
  for ( int i = 0; i < npts; i++ )
    {
      /* calculated rotated x, y */
      float rotated_x = ( (TVector3*)a[i] )->X() * cos( angle ) - ( (TVector3*)a[i] )->Y() * sin( angle );
      float rotated_y = ( (TVector3*)a[i] )->X() * sin( angle ) + ( (TVector3*)a[i] )->Y() * cos( angle );

      /* add center offset */
      rotated_x += center_z;
      rotated_y += center_x;

      /* set vector coordinates to new x, y */
      ( (TVector3*)a[i] )->SetX( rotated_x );
      ( (TVector3*)a[i] )->SetY( rotated_y );
    }

  /* extract two 1-D arrays from array of vectors with corner points coordinates */
  float xarr[ npts ];
  float yarr[ npts ];

  for ( int i = 0; i < npts; i++ )
    {
      xarr[i] = ( (TVector3*)a[i] )->X();
      yarr[i] = ( (TVector3*)a[i] )->Y();
    }

  /* create and return TPolyLine object */
  return new TPolyLine(npts, xarr, yarr);
}
