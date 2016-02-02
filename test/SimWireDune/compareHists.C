// compareHists.C
//
// David Adams
// November 2015

int compareHists(string sam1, string sam2, int comdbg =0) {
  string file1 = sam1 + "_perf.root";
  string file2 = sam2 + "_perf.root";
  cout << "Comparing hists with debug level " << comdbg << endl;
  cout << "     Test sample: " << sam1 << endl;
  cout << "Reference sample: " << sam2 << endl;
  HistoCompare hc(file1, file2, comdbg);
  int rstat = hc.compare35t("raw", 1, 5);
  // Exit ROOT with the # bad comparisons.
  if ( rstat ) {
    cout << "compareHists: FAILED: " << rstat << "/" << hc.nhst << " histograms differ" << endl;
    cout << "                      " << hc.nbinbad << "/" << hc.nbin << " bins differ" << endl;
  }
  exit(rstat);
}
