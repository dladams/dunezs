typedef vector<string> NameList;
int figs() {
  NameList evt;
  evt.push_back("1");
  evt.push_back("2");
  evt.push_back("3");
  evt.push_back("4");
  evt.push_back("5");
  NameList apa;
  apa.push_back("2z2");
  apa.push_back("2u");
  apa.push_back("2v");
  NameList var;
  var.push_back("raw");
  var.push_back("dco");
  double xmin = 0.0;
  double xmax = 0.0;
  for ( int ievt=0; ievt<evt.size(); ++ievt ) {
    for ( int iapa=0; iapa<apa.size(); ++iapa ) {
      for ( int ivar=0; ivar<var.size(); ++ivar ) {
        double zmax = 40;
        if ( var[ivar] == "dco" ) zmax = 20;
        string hname = "h" + evt[ievt] + "_" + var[ivar] + "apa" + apa[iapa];
        draw(hname, 0, xmin, xmax, zmax);
        dxprint("", "", 2);
      }
    }
  }
  return 0;
}
