// compareHists.C
//
// David Adams
// November 2015

int compareHists() {
  HistoCompare hc("mu35tnewperf.root", "mu35toldperf.root");
  int rstat = hc.compare35t("raw", 1, 5);
  // Exit ROOT with the # bad comparisons.
  if ( rstat ) {
    cout << "compareHists: FAILED: " << rstat << "/" << hc.nhst << " histograms differ." << endl;
  }
  exit(rstat);
}
