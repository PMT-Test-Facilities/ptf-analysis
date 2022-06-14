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

void GainVsTTS() {

  TCanvas *c1 = new TCanvas();
  gStyle->SetLabelSize(0.039, "X");
  gStyle->SetLabelSize(0.039, "Y");  
  gStyle->SetTitleXSize(0.044);  
  gStyle->SetTitleYSize(0.044);
  gStyle->SetTitleXOffset(1.1);
  gStyle->SetTitleYOffset(1.1); //1.4
  
  c1->SetFillColor(0);
  c1->SetGrid();
  //c1->SetLogy(1);
  //c1->SetLogx(1);

  double Gain[4] = {3,5,10,20};
  double TTS[4] = {1.889,1.827,1.636,1.609};
  double eGain[4] = {0,0,0,0};
  double eTTS[4] = {0.0288,0.0089,0.0079,0.0099};

  TGraphErrors* gr1 = new TGraphErrors(4, Gain, TTS, eGain, eTTS);
  gr1->SetTitle(";Gain (#times10^{6});FWHM (ns)");
  
  gr1->SetMarkerStyle(20);
  gr1->SetMarkerColor(4);
  gr1->SetMarkerSize(1.2);
  gr1->SetLineWidth(2);
  gr1->SetLineColor(1);

  gr1->Draw("ap");

  c1->Update();
  c1->GetFrame()->SetFillColor(19);
  c1->GetFrame()->SetBorderSize(12);
  c1->Modified();
  
  //c1->Print("SuperGain" + (TString)penningText + ".png");
  //c1->Close();

}
