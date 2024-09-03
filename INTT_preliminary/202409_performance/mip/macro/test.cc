

template < class T >
void PrintVector( vector < T > values )
{
  cout << "Size: " << values.size() << endl;
  for( auto& val : values )
    cout << val << endl;
}

template < class T >
void PrintVector2D( vector < vector < T > > vv )
{
  cout << "Size: " << vv.size() << endl;
  for( auto& val : vv )
    PrintVector( val );
}

int test()
{
  vector < int > values1;
  values1.push_back( 0 );
  values1.push_back( 1 );
  values1.push_back( 2 );
  PrintVector( values1 );

  vector < int > values2;
  values2.push_back( 2 );
  values2.push_back( 1 );
  values2.push_back( 0 );
  PrintVector( values2 );


  vector < vector < int > > vv;
  vv.push_back( values1 );
  vv.push_back( values2 );
  PrintVector2D( vv );

  for( auto& v : vv )
    v.erase( v.begin(), v.end() );
  
  PrintVector2D( vv );
  
  return 0;
}
