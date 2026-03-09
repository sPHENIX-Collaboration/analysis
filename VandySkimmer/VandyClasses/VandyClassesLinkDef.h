#ifdef __CINT__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

// The + tells ROOT to enable streamers for these classes
#pragma link C++ class EventInfo+;
#pragma link C++ class Tower+;
#pragma link C++ class JetInfo+;

// These fix the "vector does not have a compiled CollectionProxy" error
#pragma link C++ class std::vector<Tower>+;
#pragma link C++ class std::vector<JetInfo>+;

#endif
