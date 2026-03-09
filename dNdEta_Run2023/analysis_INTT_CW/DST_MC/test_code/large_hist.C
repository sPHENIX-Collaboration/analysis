void large_hist()
{
  vector<vector<TH1F * >> aaa;
  aaa.clear();
    
  aaa = vector<vector<TH1F *>>(10);
    
  for (int i = 0; i < 10; i++)
    {
      for (int i1 = 0; i1 < 350; i1++)
        {
	  aaa[i].push_back(new TH1F("","",10,0,10));
        }
    }

  TRandom * rand = new TRandom3();

  for (int i = 0; i < 1000000; i++)
    {
      int rand1 = int(rand->Uniform(0,10));
      int rand2 = int(rand->Uniform(0,350));
      int rand3 = int(rand->Uniform(0,10));
        
      if(i%2000 == 0) {cout<<i<<" "<<rand1<<" "<<rand2<<" "<<rand3<<endl;}

      aaa[rand1][rand2]->Fill(rand3);
    }
}
