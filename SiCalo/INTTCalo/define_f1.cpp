{
  TF1* f1=new TF1("f1","gaus(0)+pol0(3)");
  f1->SetParameters(1000,0.,0.01,0);
  TF1* f2=new TF1("f2","gaus(0)+pol2(3)");
  f2->SetParameters(1000,0.,0.01,0,0,0);
}
