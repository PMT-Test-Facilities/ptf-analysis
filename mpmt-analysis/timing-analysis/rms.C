#include <iostream>
#include <fstream>
#include <string>
#include <cmath>

#include <TApplication.h>
#include "TStyle.h"
#include "TPad.h"
#include "TGraph.h"
#include "TAxis.h"

using namespace std;

void rms() {

  TCanvas *c1 = new TCanvas();
  
  c1->SetFillColor(0);//45
  c1->SetGrid();

  int runNo;
  cout << "Run Number? ";
  cin >> runNo;
  
  TH1F *hist = new TH1F("hist", "", 25, 1.5, 1.7);
  ifstream file;
  file.open("TimingData_" + TString::Itoa(runNo,10) + ".txt");
  double value;
  int count = 0;
  while(1)
  {
    if(file.eof()) break;
    file >> value;
    if (count) {
    cout << value << endl;
    hist->Fill(value);
    }
    count++;
  }
  file.close();
  hist->GetYaxis()->SetTitle("Counts");
  hist->GetXaxis()->SetTitle("FWHM");
  hist->SetLineColor(kOrange-3);
  hist->SetFillColor(kRed-4);
  hist->Draw();
  ofstream file1;
  file1.open("rmsValues.txt", ios::app);
  double rms;
  rms = hist->GetRMS();
  file1 << "RMS value of run " << runNo << " is " << rms << std::endl;
  file1.close();
  cout << "RMS is : " << rms << endl;
  c1->Update();
  c1->GetFrame()->SetFillColor(19);
  c1->GetFrame()->SetBorderSize(12);
  c1->Modified();
  c1->Print("RMS_" + TString::Itoa(runNo,10) + ".png");
  
}
