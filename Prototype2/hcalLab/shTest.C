void shTest(){
  Double_t par[] = {0.3164, 3., 1.5};   //  guesses
  Double_t points(24);
  TF1 * sT = new TF1("sT","0.3164*pow(x,4)*exp(-x*1.5)",0., points);
  sT->Draw();
  Double_t shapeInt = sT->Integral(0., points);
  Double_t shapeMax = sT->GetMaximum(0.,points);
  Double_t shapePeak= sT->GetMaximumX(0.,points);
  Double_t shapeRMS = sqrt(sT->Moment(2,0.,points)-sT->Moment(1,0.,points)*sT->Moment(1,0.,points));
  cout<<"SHAPE  Peak = "<<shapePeak<<"  Max = "<<shapeMax<<"  RMS = "<<shapeRMS<<"  Integral = "<<shapeInt<<endl;
  // return par[0]*pow(x1,4.)*(exp(-x1*16./fN));    //  fN is set to (float)NSAMPLES
}


Double_t signalShape(Double_t *x, Double_t * par){
  return par[0]*pow((x-par[1]),par[2])*exp(-(x-par[1])*par[3]);	       
}
		       
