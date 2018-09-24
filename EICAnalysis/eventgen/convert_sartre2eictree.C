// Code takes a Sartre  HepMC output and converts it to a MILOU ascii output, which is then built into an EICTree.

// 4 final state particles
// Scattered electron (KS=2)
// Scattered proton (KS=1)
// Decay electron and positron (KS=1)

int convert_sartre2eictree() 
{ 
    std::ifstream file("18x275_sartre.txt");


   
    std::ofstream output("18x275_sartre.out");
    output << "MILOU\n============================================\nI, ievent, linesnum, weight, genprocess, radcorr,        truex, trueQ2, truey, truet, treuphi, phibelgen, phibelres,       phibelrec\n============================================\nI, K(I,1)  K(I,2)  K(I,3)  K(I,4)  K(I,5)             P(I,1)  P(I,2)  P(I,3)  P(I,4)  P(I,5) V(I,1)  V(I,2)  V(I,3)\n============================================\n";
    std::string str; 
    std::string file_contents;
    int event_count = 0;
    int particle_count = 0;
    while (std::getline(file, str))
    {
      file_contents = str;
      if(file_contents.size()==0)
	continue;
      if(file_contents.at(0)=='U')
        {
	  // New event
	  // 1) Add the arbitrary event info
	  output << get_arbitrary_event(event_count++);
	  particle_count = 0;
	  if(event_count%100==0)
	    {
	      cout << "Processing Event " << event_count << endl;
	    }
	}
      if(file_contents.at(0)=='P')
	{
	  // Found a particle
	  // See if it is final state
	  int index = file_contents.find(" 1 0 0 ");
	  if(index>0)
	    {
	      output << ++particle_count;
	      output << "\t";
	      if(particle_count==1)
		{
		  output<< "2\t";
		}
	      else
		{
		  output << "1\t";
		}
	      string tmp;            // A string to store the word on each iteration.
	      stringstream str_strm(file_contents);
	      vector<string> words;     // Create vector to hold our words
    
	      int counter = 0;
	      while (str_strm >> tmp) {
		if(counter==2)
		  {
		    output << tmp;
		    output <<"\t 0 \t 0 \t 0 \t";
		  }
		if(counter>=3&&counter<=7)
		  {
		    tmp=tmp.replace(tmp.find("e"),1,"E");
		    output << tmp;
		    output << "\t";
		  }
		counter++;
	      }

	      output << "0 \t 0 \t 0\n";
	    }
	}
      if(particle_count==4)
	{
	  output<<" =============== Event finished =============== \n";
	  particle_count=0;
	}
    }
    output.close();
    gSystem->Load("$OPT_SPHENIX/eic-smear_root-5.34.36/lib/libeicsmear.so");

    BuildTree("sartre_ascii.out");

  return 0;
}

std::string get_arbitrary_event(int x)
{
  std::string alpha;
  stringstream ss;
  ss << "0" << "\t" << x << "           5   1.00000000               0           0   7.56711757E-04   1.13123262      0.373815268      0.177931070       0.00000000       2.9 2728496       110.619263     -0.310856074\n ============================================\n";
  alpha = ss.str();
  return alpha;
}
