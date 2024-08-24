#pragma once

template < typename TH >
TPaletteAxis* DrawPaletteAxis( TH* hist,
			       double xmin, double ymin,
			       double xmax, double ymax,
			       double label_size)

{

  gPad->Update();
  TPaletteAxis *pal = (TPaletteAxis*)hist->GetListOfFunctions()->FindObject("palette");
  if( pal == nullptr )
    {
      string color_red = "\033[31m";
      string color_cancel = "\033[m";

      cerr << color_red << endl;
      cerr << " " << string( 50, '=' ) << endl;
      cerr << "template <typename TH> TPaletteAxis* DrawPaletteAxis( TH* hist , double xmin, double ymin, double xmax, double ymax , double label_size)" << endl;
      cerr << "TPaletteAxis *pal = (TPaletteAxis*)hist->GetListOfFunctions()->FindObject(\"palette\")  ---> nullptr" << endl;
      cerr << " Someting wrong!!!" << endl;
      cerr << " " << string( 50, '=' ) << endl;
      cerr << color_cancel << endl;

      return new TPaletteAxis();
    }

  pal->GetAxis()->SetLabelSize( label_size );
  pal->GetAxis()->CenterTitle();

  pal->SetX1NDC( xmin );
  pal->SetX2NDC( xmax );

  pal->SetY1NDC( ymin );
  pal->SetY2NDC( ymax );
  pal->Draw();

  return pal;
}
