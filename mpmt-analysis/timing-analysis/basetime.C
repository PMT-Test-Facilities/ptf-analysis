#include <iostream>
#include <fstream>
#include <string>
#include <cmath>

#include <TApplication.h>
#include "TStyle.h"
#include "TGraph.h"
#include "TAxis.h"
#include "TFrame.h"
#include <TROOT.h>
#include <TH1F.h>

using namespace std;

void basetime() {

  TCanvas *c1 = new TCanvas();
  gStyle->SetLabelSize(0.039, "X");
  gStyle->SetLabelSize(0.039, "Y");  
  gStyle->SetTitleXSize(0.044);  
  gStyle->SetTitleYSize(0.044);
  gStyle->SetTitleXOffset(1.1);
  gStyle->SetTitleYOffset(1.1); //1.4
  
  c1->SetFillColor(0);
  c1->SetGrid();

  TFile* file = new TFile("mpmt_Analysis_run01341.root");
  TTreeReader myReader1("ptfanalysis0", file);
  TTreeReader myReader2("ptfanalysis1", file);
  //TTreeReader myReader3("ptfanalysis0", file);
  
  TTreeReaderValue<Float_t> tts0(myReader1, "mean");
  TTreeReaderValue<Float_t> tts1(myReader2, "mean");
  TTreeReaderValue<Float_t> baseline(myReader1, "bbaseline");

  TH2F *base_tts = new TH2F("base_tts", "Baseline Vs TTS", 200, 0.98, 1, 200, 54, 61);

  Float_t Tts0[149999], Tts1[149999], Baseline[149999], Tdiff[149999];
  int count = 0;
  while (myReader1.Next()) {
    Tts0[count] = *tts0;
    Baseline[count] = *baseline;
    cout << "Actual baseline: " << *baseline << " array value: " << Baseline[count] << endl;
    //cout << *tts0 << endl;
    //cout << count << endl;
    count ++;
  }

  int count2 = 0;
  while (myReader2.Next()) {
    Tts1[count2] = *tts1;
    //cout << "Actual TTS1: " << *tts1 << " array value: " << Basel[count] << endl;
    //cout << *tts1;
    count2 ++;
  }

  for (int i = 0; i < 149999; i++) {
    //cout << Tts0[i]-Tts1[i] << " " << Baseline[i] << endl;
    Tdiff[i] = Tts0[i]-Tts1[i];
    //cout << Baseline[i] << " " << Tdiff[i] << endl;
    base_tts->Fill(Baseline[i],Tdiff[i]);
  }

  base_tts->Draw("colz");
  
  c1->Update();
  c1->GetFrame()->SetFillColor(19);
  c1->GetFrame()->SetBorderSize(12);
  c1->Modified();
  
  //c1->Print("SuperGain" + (TString)penningText + ".png");
  //c1->Close();

}
