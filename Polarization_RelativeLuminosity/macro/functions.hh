#pragma once

int GetSphenixColor()
{
  // RGB = (62, 136, 209)
  Int_t color_index = TColor::GetFreeColorIndex();
  TColor* my_color = new TColor(color_index,
                                62.0 / 255.0,
                                136.0 / 255.0,
                                209.0 / 255.0);

  return color_index;  // my_color->GetColor();
}

void WritesPhenix(int mode = 0, double pos_x = 0.2, double pos_y = 0.96, double text_size = 0.04)
{
  TLatex* tex = new TLatex();
  tex->SetTextSize(text_size);

  if (mode == 0)
    tex->DrawLatexNDC(pos_x, pos_y, "#it{#bf{sPHENIX}} Internal");
  else if (mode == 1)
    tex->DrawLatexNDC(pos_x, pos_y, "#it{#bf{sPHENIX}} Preliminary");
  else if (mode == 2)
    tex->DrawLatexNDC(pos_x, pos_y, "#it{#bf{sPHENIX}} Work in Progress");
  else if (mode == 3)
    tex->DrawLatexNDC(pos_x, pos_y, "#it{#bf{sPHENIX}} Performance");
  else if (mode == 4)
    tex->DrawLatexNDC(pos_x, pos_y, "#it{#bf{sPHENIX}} Simulation Performance");

  delete tex;
};

void WriteRunCondition(double pos_x = 0.2, double pos_y = 0.675, bool is_mc = false, double text_size = 0.04, bool is_single_line=false )
{
  TLatex* tex = new TLatex();
  tex->SetTextSize(text_size);

  if (is_mc == false)
  {
    if( is_single_line == false )
    {
      tex->DrawLatexNDC(pos_x, pos_y, "p^{#uparrow}+p^{#uparrow} Run2024 #sqrt{s} = 200 GeV");
    }
    else
    {
      tex->DrawLatexNDC(pos_x, pos_y, "p^{#uparrow}+p^{#uparrow} Run2024");
      tex->DrawLatexNDC(pos_x - 0.01, pos_y - 0.05, "#sqrt{s} = 200 GeV");
    }
  }
  else
    tex->DrawLatexNDC(pos_x, pos_y, "Simulation");

  delete tex;
}

void WriteDate(double pos_x = 0.775, double pos_y = 0.955, double text_size = 0.04, string fixed_date = "" )
{
  TLatex* tex = new TLatex();
  tex->SetTextSize(text_size);

  TDatime now;
  int year = now.GetYear();
  int month = now.GetMonth();
  int day = now.GetDay();
  stringstream message;
  message << month << "/" << day << "/" << year;

  if( fixed_date == "" )
    tex->DrawLatexNDC(pos_x, pos_y, message.str().c_str());
  else
    tex->DrawLatexNDC(pos_x, pos_y, fixed_date.c_str());

  delete tex;
};

void HistSetting(TH1D* hist, int color)
{
  hist->SetLineColor(color);
  hist->SetFillColorAlpha(color, 0.1);
}
