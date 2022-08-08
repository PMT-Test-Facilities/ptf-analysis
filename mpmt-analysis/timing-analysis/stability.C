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

void stability() {

  TCanvas *c1 = new TCanvas();
  
  c1->SetFillColor(0);//45
  c1->SetGrid();
  
  
  //TH1F *hist = new TH1F("hist", "", 30, 2.28, 2.4);
  ifstream file, file1, file2;
  file.open("LaserStability.txt");
  file1.open("TimingData_1450.txt");
  file2.open("Pulses.txt");
  double value;
  double pulseHeight[451], TTS[451], pulses[451], time[451];
  for (int i = 0; i < 451; i++){
    file >> pulseHeight[i];
    file1 >> TTS[i];
    file2 >> pulses[i];
    time[i] = (451.4/451.0)*(i+1);
    if (TTS[i] <= 0) TTS[i] = (TTS[i-1]+TTS[i-2])/2.0;
    if (pulses[i] <= 0) pulses[i] = (pulses[i-1]+pulses[i-2])/2.0;
  }
  //TMultiGraph* mg = new TMultiGraph();
  TGraph *gr1 = new TGraph(451, time, pulseHeight);
  TGraph *gr2 = new TGraph(451, time, TTS);
  TGraph *gr3 = new TGraph(451, time, pulses);  
  gr1->SetTitle(";Time (min); Mean Pulse Height (PE)");
  gr2->SetTitle(";Time (min); Timing Resolution (FWHM)");
  gr3->SetTitle(";Time (min); Number of 1 PE pulses");
  gr1->SetMarkerStyle(20);
  gr1->SetMarkerColor(4);
  gr1->SetMarkerSize(1.2);
  gr1->SetLineWidth(2);
  gr1->SetLineColor(1);
  gr1->GetHistogram()->SetMaximum(1.2);
  gr1->GetHistogram()->SetMinimum(0.6);
  gr1->Draw("ap");
  c1->Update();
  c1->GetFrame()->SetFillColor(19);
  c1->GetFrame()->SetBorderSize(12);
  c1->Modified();
  c1->Print("LaserStability.png");
  
  gr2->SetMarkerStyle(20);
  gr2->SetMarkerColor(4);
  gr2->SetMarkerSize(1.2);
  gr2->SetLineWidth(2);
  gr2->SetLineColor(1);
  gr2->GetHistogram()->SetMaximum(2.7);
  gr2->GetHistogram()->SetMinimum(1.2);  
  gr2->Draw("ap");
  c1->Update();
  c1->GetFrame()->SetFillColor(19);
  c1->GetFrame()->SetBorderSize(12);
  c1->Modified();
  c1->Print("TTSStability.png");
  
  gr3->SetMarkerStyle(20);
  gr3->SetMarkerColor(4);
  gr3->SetMarkerSize(1.2);
  gr3->SetLineWidth(2);
  gr3->SetLineColor(1);
  gr3->Draw("ap");
  c1->Update();
  c1->GetFrame()->SetFillColor(19);
  c1->GetFrame()->SetBorderSize(12);
  c1->Modified();
  c1->Print("PulseStability.png");
  
  file.close();
  file1.close();
  file2.close();
  
}
