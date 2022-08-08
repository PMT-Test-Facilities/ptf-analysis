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

  //double Gain[4] = {960,1033.5,1136,1253.5};
  double Gain[4] = {3,5,10,20};
  //double gain[3] = {5,10,20};
  //double gan[2] = {10,20};
  //double TTS[4] = {1.889,1.827,1.636,1.609};
  //double TTS[4] = {2.598,2.233,2.19,2.002};
  //double eGain[4] = {0,0,0,0};
  double TTS[4] = {2.658,2.411,2.201,2.121};
  double eGain[4] = {0,0,0,0};
  //double eTTS[4] = {0.0288,0.0089,0.0079,0.0099};
  //double eTTS[4] = {0.0121,0.0093,0.0049,0.0036};
  double eTTS[4] = {0,0,0,0};
  double TTS2[4] = {1.969,1.899,1.721,1.727};
  double eTTS2[4] = {0.00787,0.0041,0.0054,0.0029};
  double TTS3[4] = {3.482,2.337,2.007,1.789};
  double eTTS3[4] = {0.0377,0.0068,0.0225,0.0191};

  TMultiGraph* mg = new TMultiGraph();
  TGraphErrors* gr1 = new TGraphErrors(4, Gain, TTS, eGain, eTTS);
  TGraphErrors* gr2 = new TGraphErrors(4, Gain, TTS2, eGain, eTTS2);
  TGraphErrors* gr3 = new TGraphErrors(4, Gain, TTS3, eGain, eTTS3);
  mg->SetTitle(";PMT Gain (#times10^{6});FWHM (ns)");
  
  gr1->SetMarkerStyle(20);
  gr1->SetMarkerColor(4);
  gr1->SetMarkerSize(1.2);
  gr1->SetLineWidth(2);
  gr1->SetLineColor(1);

  gr2->SetMarkerStyle(21);
  gr2->SetMarkerColor(2);
  gr2->SetMarkerSize(1.2);
  gr2->SetLineWidth(2);
  gr2->SetLineColor(1);

  gr3->SetMarkerStyle(22);
  gr3->SetMarkerColor(3);
  gr3->SetMarkerSize(1.2);
  gr3->SetLineWidth(2);
  gr3->SetLineColor(1);

  mg->Add(gr1);
  mg->Add(gr2);
  //mg->Add(gr3);
  
  mg->Draw("ap");
  
  TLegend* legend = new TLegend();
  //legend->AddEntry(gr3, "#times1 Op-amp gain", "p");
  legend->AddEntry(gr1, "New Data", "p");
  legend->AddEntry(gr2, "Old Data", "p");
  legend->SetTextSize(0.033);
  legend->Draw();

  c1->Update();
  c1->GetFrame()->SetFillColor(19);
  c1->GetFrame()->SetBorderSize(12);
  c1->Modified();
  
  //c1->Print("SuperGain" + (TString)penningText + ".png");
  //c1->Close();

}
