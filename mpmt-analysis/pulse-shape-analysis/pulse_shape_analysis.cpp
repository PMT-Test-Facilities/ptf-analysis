#include "TCanvas.h"
#include "TFile.h"
#include "TH1D.h"
#include "TStyle.h"
#include "TLegend.h"

#include <iostream>
#include <cstring>
#include <string>
using namespace std;

int main( int argc, char* argv[] ) {

    TCanvas *c1 = new TCanvas("C1","C1",1600,1300);
    TLegend *legend = new TLegend(0.6,0.1,0.9,0.4);
    legend->SetHeader("Legend","C");

    // TH1F * pulse_shapes[7];
    // TFile * files[7] = {    new TFile("mpmt_Analysis_run0983_0.1ns.root"),
    //                         new TFile("mpmt_Analysis_run0984_0.1ns.root"),
    //                         new TFile("mpmt_Analysis_run0985_0.1ns.root"),
    //                         new TFile("mpmt_Analysis_run0986_0.1ns.root"),
    //                         new TFile("mpmt_Analysis_run0987_0.1ns.root"),
    //                         new TFile("mpmt_Analysis_run0993_0.1ns.root"),
    //                         new TFile("mpmt_Analysis_run0994_0.1ns.root")};
    // string num_pe[7] = {"0.10V", "0.50V", "1.00V", "2.00V", "3.00V", "4.50V", "5.00V"};

    // int color[7]={1, 633, 600, 419, 618, 807, 989}; //,891,14,861,413,803,874,430,895,873,870,801,637,625,602};
    TH1F * pulse_shapes[2];
    TFile * files[2] = {    new TFile("~/hunter/ptf-analysis/bin/mpmt_Analysis_run01042.root"),
                            new TFile("~/hunter/ptf-analysis/bin/mpmt_Analysis_run01075.root")};
    string num_pe[2] = {"Ch0", "Ch2"};

    int color[2]={1, 633}; //,891,14,861,413,803,874,430,895,873,870,801,637,625,602};

    // Loop through ROOT files of different runs (1pe, 5pe, 10pe, 20pe, 30pe, 40pe)
    // and save the pulse shape.
    // for (int i=0; i<7; i++) {
        for (int i = 0; i < 2; i++) {
        pulse_shapes[i] = (TH1F*)files[i]->Get("pulse_shape");
        string name = num_pe[i] + " pulse shape";
        pulse_shapes[i]->SetName(name.c_str());
        pulse_shapes[i]->SetLineColor(color[i]);

        // Set up pad for current pulse shape
        gStyle->SetOptStat(11);
        TPad *pad = new TPad("pad", "", 0,0,1,1);
        if (i!=0) pad->SetFillStyle(4000);

        // Find baseline and pulse height
        float baseline = 0;
        for (int j=50; j<100; j++) baseline+=pulse_shapes[i]->GetBinContent(j);
        baseline = baseline/50;
        // for (int j=40; j<60; j++) baseline+=pulse_shapes[i]->GetBinContent(j);
        // baseline = baseline/20;
        // for (int j=400; j<600; j++) baseline+=pulse_shapes[i]->GetBinContent(j);
        // baseline = baseline/200;
        // float ph = pulse_shapes[i]->GetBinContent(pulse_shapes[i]->GetMaximumBin())-pulse_shapes[i]->GetBinContent(pulse_shapes[i]->GetMinimumBin());
        float ph = baseline - pulse_shapes[i]->GetBinContent(pulse_shapes[i]->GetMinimumBin());

        // Pulse width calculation added by H.Ma (Oct 2021)
        float half_height = baseline - ph / 2;
        float intersection[2] = {0, 0};     // used to record the x-coordinate
        for (int j = 0; j < 180; j++) {
            float y_value = pulse_shapes[i]->GetBinContent(j);
            if (y_value - half_height < 0) {
                float y_value_prev = pulse_shapes[i]->GetBinContent(j - 1);
                float slope = y_value_prev - y_value;                                   // a = (y2 - y1) / (x2 - x1)
                float y_intercept = y_value - slope * j;                                // b = y1 - a * x1
                intersection[0] = (half_height - y_intercept) / slope + 2000;           // x = (y - b) / a
                break;
            }
        }
        for (int k = 180; k < 400; k++) {
            float y_value = pulse_shapes[i]->GetBinContent(k);
            if (y_value - half_height > 0) {
                float y_value_prev = pulse_shapes[i]->GetBinContent(k - 1);
                float slope = y_value_prev - y_value;                                   // a = (y2 - y1) / (x2 - x1)
                float y_intercept = y_value - slope * k;                                // b = y1 - a * x1
                intersection[1] = (half_height - y_intercept) / slope + 2000;           // x = (y - b) / a
                break;
            }
        }
        float width = intersection[1] - intersection[0];
        cout << name << " has width " << width << "ns with left coordinate at " << intersection[0] << "ns and right coordinate at " << intersection[1] << "ns" << endl;
        
        // Scale pad to align baseline and minimum of pulse
        double ymin = pulse_shapes[i]->GetBinContent(pulse_shapes[i]->GetMinimumBin()) - 0.1*ph;
        // double ymax = baseline+0.3*ph;
        double ymax = baseline+0.9*ph;
        double dy = (ymax-ymin)/0.8;
        double xmin = 2000;
        double xmax = 2400;
        // double xmin = 2020;
        // double xmax = 2220;
        //double xmin = 2180;
        //double xmax = 2200;
        double dx = (xmax-xmin)/0.8;
        if (i==0) pulse_shapes[i]->GetYaxis()->SetRangeUser(ymin,ymax);pulse_shapes[i]->GetXaxis()->SetRangeUser(xmin,xmax);
        pad->Range(xmin-0.1*dx,ymin-0.1*dy,xmax+0.1*dx,ymax+0.1*dy);
        pad->Draw();

        // Draw current pulse shape
        pad->cd();
        pulse_shapes[i]->SetLineColor(color[i]);
        pulse_shapes[i]->GetYaxis()->SetAxisColor(color[i]);
        if (i==0) pulse_shapes[i]->Draw("C");
        else pulse_shapes[i]->Draw("SAMES C");
        pad->Update();
        c1->cd();

        legend->AddEntry(pulse_shapes[i],name.c_str(),"l");
    }

    legend->Draw();
    c1->SaveAs("pulse_shape_analog.root");
    c1->SaveAs("pulse_shape_analog.png");

    return 0;
}
