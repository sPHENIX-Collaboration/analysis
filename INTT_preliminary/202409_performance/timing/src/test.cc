
//vector < bool > GetTriggerInfo( int val )
vector < int > GetTriggerInfo( int val )
{
  vector < int > rtn;
  while( val != 0 )
    {
      int a = 0 ;
      a = val & 1;
      cout << std::bitset<32>(val) << "  "
	   << a << "\t";

      val = val >> 1;

      cout << std::bitset<32>(val) << endl;

      rtn.push_back( a );
      // if( a == 1 )
      // 	rtn.push_back( true );
      // else
      // 	rtn.push_back( false );

    }

  return rtn;
}

int test()
{

  int num = 1234567;
  auto trigger_info = GetTriggerInfo( num );
  for( auto& val : trigger_info )
    cout << val << endl;

  return 0;
}
