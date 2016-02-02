// figs.C
//
// David Adams
// January 2016
//
// Used to generate plots shown to FD sim/reco meeting February 1, 2016.

typedef vector<string> NameList;

int figs(bool allevt =false) {
  NameList evt;
  evt.push_back("1");
  if ( allevt ) {
    evt.push_back("2");
    evt.push_back("3");
    evt.push_back("4");
    evt.push_back("5");
  }
  NameList apa;
  apa.push_back("2z2");
  apa.push_back("2u");
  apa.push_back("2v");
  NameList var;
  var.push_back("sim");
  var.push_back("raw");
  var.push_back("dco");
  double xmin = 0.0;
  double xmax = 0.0;
  double xminint = 2600;
  double xmaxint = 2700;
  for ( int ievt=0; ievt<evt.size(); ++ievt ) {
    for ( int iapa=0; iapa<apa.size(); ++iapa ) {
      double asim = 0.0;
      double adco = 0.0;
      double ymaxsim = 0.0;
      double ymaxdco = 0.0;
      TH1* phticksim = 0;
      TH1* phtickdco = 0;
      TH1* phchansim = 0;
      TH1* phchandco = 0;
      for ( int ivar=0; ivar<var.size(); ++ivar ) {
        double zmax = 40;
        if ( var[ivar] == "dco" ) zmax = 20;
        if ( var[ivar] == "sim" ) zmax = 1;
        string hname = "h" + evt[ievt] + "_" + var[ivar] + "apa" + apa[iapa];
        DrawResult res = draw(hname, 0, xmin, xmax, zmax);
        dxprint("", "", 2);
        draw1d(res.channels());
        dxprint("", "", 2);
        double xmin = 0.0;
        double xmax = 0.0;
        double ymin = 0.0;
        double ymax = 0.0;
        int rebin = 0;
        if ( var[ivar] == "sim" ) {
          rebin = 5;
          phchansim = res.channels();
          phticksim = res.ticks();
          asim = res.ticks()->Integral(xminint, xmaxint);
          ymaxsim = res.ticks()->GetMaximum();
        }
        if ( var[ivar] == "dco" ) {
          phchandco = res.channels();
          phtickdco = res.ticks();
          adco = res.ticks()->Integral(xminint, xmaxint);
          ymaxdco = res.ticks()->GetMaximum();
          if ( asim > 0.0 && ymaxsim > 0.0 ) {
            ymin = 0.0;
            ymax = adco/asim*ymaxsim;
          }
        }
        draw1d(res.ticks(), xmin, xmax, ymin, ymax, rebin);
        dxprint("", "", 2);
      }
      double ratmax = 400.0;
      int ratrebin = 10;
      // Ratio of dco/sim vs tick.
      if ( phticksim!=0 && phtickdco!=0 ) {
        string hnam = "hdcosimrat_tick_";
        hnam += apa[iapa];
        cout << "Creating ratio hist " << hnam << endl;
        TH1* phrat = dynamic_cast<TH1*>(phtickdco->Clone(hnam.c_str()));
        phrat->Divide(phticksim);
        phrat->GetYaxis()->SetTitle("Charge/energy [fC/MeV]");
        string htit = "Deconvoluted/SimChannel";
        string htitold = phrat->GetTitle();
        string::size_type ipos = htitold.find(" for ");
        htit += htitold.substr(ipos);
        phrat->SetTitle(htit.c_str());
        phrat->GetYaxis()->SetTitle("Charge/energy [fC/MeV]");
        draw1d(phrat, 0, 0, 0, ratmax, ratrebin);
        dxprint("", "", 2);
      }
      // Ratio of dco/sim vs channel.
      if ( phchansim!=0 && phchandco!=0 ) {
        string hnam = "hdcosimrat_channel_";
        hnam += apa[iapa];
        cout << "Creating ratio hist " << hnam << endl;
        TH1* phrat = dynamic_cast<TH1*>(phchandco->Clone(hnam.c_str()));
        phrat->Divide(phchansim);
        phrat->GetYaxis()->SetTitle("Charge/energy [fC/MeV]");
        string htit = "Deconvoluted/SimChannel";
        string htitold = phrat->GetTitle();
        string::size_type ipos = htitold.find(" for ");
        htit += htitold.substr(ipos);
        phrat->SetTitle(htit.c_str());
        phrat->GetYaxis()->SetTitle("Charge/energy [fC/MeV]");
        draw1d(phrat, 0, 0, 0, ratmax, ratrebin);
        dxprint("", "", 2);
      }
    }
  }
  return 0;
}
