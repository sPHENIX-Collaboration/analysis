void myText(Double_t x,Double_t y,Color_t color,const char *text, Double_t tsize = 0.05, double angle = -1);


void myText(Double_t x,Double_t y,Color_t color, 
	    const char *text, Double_t tsize, double angle) {

  TLatex l; //l.SetTextAlign(12); 
  l.SetTextSize(tsize); 
  l.SetNDC();
  l.SetTextColor(color);
  if (angle > 0) l.SetTextAngle(angle);
  l.DrawLatex(x,y,text);
}
