// compareHists.C
//
// David Adams
// November 2015

int compareHists(int comdbg =0) {
  cout << "Comparing hists with debug level " << comdbg << endl;
  HistoCompare hc("mu35tnewperf.root", "mu35toldperf.root", comdbg);
  int rstat = hc.compare35t("raw", 1, 5);
  // Exit ROOT with the # bad comparisons.
  if ( rstat ) {
    cout << "compareHists: FAILED: " << rstat << "/" << hc.nhst << " histograms differ" << endl;
    cout << "                      " << hc.nbinbad << "/" << hc.nbin << " bins differ" << endl;
  }
  exit(rstat);
}
