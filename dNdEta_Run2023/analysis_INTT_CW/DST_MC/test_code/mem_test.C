void mem_test()
{
    TH2F * aaa = new TH2F("aaa","aaa",100,0,100,100,0,100);
    aaa -> Fill(3,5);
    aaa -> Fill(3,5);
    aaa -> Fill(3,5);
    aaa -> Fill(3,5);
    aaa -> Fill(7,20);

    TH2F * BBB; 
    TProfile * CCC;

    for (int i = 0; i < 100; i++)
    {
        cout<<"trial index : "<<i<<endl;
        for (int i1 = 0; i1 < 500; i1++)
        {
            vector<double> DDD = {1,2,3,4,54,6,7,8,9};
            cout<<"test : "<<i1<<endl;
            BBB = (TH2F *) aaa -> Clone();
            CCC = BBB -> ProfileX("CCC");
            // BBB -> Reset("ICESM");
            delete BBB;
        }

        sleep(1);        

    }
}