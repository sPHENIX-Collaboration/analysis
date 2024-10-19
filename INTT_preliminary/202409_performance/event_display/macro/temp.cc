
class aaa
{
public:  
  vector < double > a;
  vector < double > b;
  vector < double > c;
  vector < vector < double > >  vv;
  
  template < class T >
  void Print( vector < T >& v )
  {
    cout << "#elements: " << v.size() << endl;
    for( auto& val : v )
      cout << val << endl;
    
    cout << endl;
  }

  template < class T >
  void Clear( vector < vector < T > > & vv )
  {
    for( auto& v : vv )
      v.clear();
  }

  void Init()
  {
    a.push_back( 1 );
    a.push_back( 1 );
    a.push_back( 1 );
    
    b.push_back( 1 );
    b.push_back( 1 );
    b.push_back( 1 );
    
    c.push_back( 1 );
    c.push_back( 1 );
    c.push_back( 1 );
    
    //    Print( a );
    
    // a.clear();
    // Print( a );
    
    vv.push_back( a );
    vv.push_back( b );
    vv.push_back( c );
  }
  
  aaa()
  {

    this->Init();
    
    for( auto& v : vv )
      Print( v );
    
    this->Clear( vv );
    
    for( auto& v : vv )
      Print( v );
    
  }

};

int temp()
{

  aaa* qwerty = new aaa();

  return 0;
}
