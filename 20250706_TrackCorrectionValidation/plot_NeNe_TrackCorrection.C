#include <TFile.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TF1.h>
#include <iostream>

#include "include/plotting.h" // Kyle's plotting utilities

void plot_NeNe_TrackCorrection() {

    // NeNe private
    const char* input =   "../../MITHIGAnalysis2024/CommonCode/root/PrivateNeNeMC_Hijing_5p36TeV.root";
    const char* input_Fak =   "../../MITHIGAnalysis2024/CommonCode/root/PrivateNeNeMC_Hijing_5p36TeV.root";
    const char* output =  "plots/20250802_PrivateNeNeMC_Hijing_5p36TeV";

    TFile* fin = TFile::Open(input, "READ");
    if (!fin || fin->IsZombie()) {
        std::cerr << "Error: Unable to open file " << fin << std::endl;
        return;
    }

    TFile* fin_Fak = TFile::Open(input_Fak, "READ");
    if (!fin_Fak || fin_Fak->IsZombie()) {
        std::cerr << "Error: Unable to open file " << fin_Fak << std::endl;
        return;
    }

    TH2F* hEff = (TH2F*)fin->Get("rEff");
    TH2F* hMul = (TH2F*)fin->Get("rMul");
    TH2F* hFak = (TH2F*)fin_Fak->Get("rFak");
    TH2F* hSec = (TH2F*)fin->Get("rSec");

    // Calculate total correction: (1 - fake) * (1 - secondary) / (efficiency * (1 + multipleReco))
    TH2F* hTotal = (TH2F*)hEff->Clone("hTotal");
    hTotal->Reset();

    int nBinsX = hEff->GetNbinsX();
    int nBinsY = hEff->GetNbinsY();

    for (int ix = 1; ix <= nBinsX; ++ix) {
        for (int iy = 1; iy <= nBinsY; ++iy) {
            double eff = hEff->GetBinContent(ix, iy);
            double mul = hMul->GetBinContent(ix, iy);
            double fak = hFak->GetBinContent(ix, iy);
            double sec = hSec->GetBinContent(ix, iy);

            double denom = eff * (1.0 + mul);
            double numer = (1.0 - fak) * (1.0 - sec);

            double corr = (denom > 0) ? numer / denom : 0.0;
            hTotal->SetBinContent(ix, iy, corr);
        }
    }

    // make canvas
    TCanvas* c1 = new TCanvas("c1", "c1", 1600, 2400);
    c1->Divide(2, 3);
    c1->cd(1);
    plot2D(
        hEff, "hEff",
        "Track #eta", -2.4, 2.4,
        "Track pT [GeV/c]", 0.4, 100,
        "", 0, 1.1,
        false, true, false
    );

    c1->cd(2);
    plot2D(
        hMul, "hMul",
        "Track #eta", -2.4, 2.4,
        "Track pT [GeV/c]", 0.4, 100,
        "", 0, 4e-3,
        false, true, false
    );

    c1->cd(3);
    plot2D(
        hFak, "hFak",
        "Track #eta", -2.4, 2.4,
        "Track pT [GeV/c]", 0.4, 100,
        "", 0, 0.1,
        false, true, false
    );

    c1->cd(4);
    plot2D(
        hSec, "hSec",
        "Track #eta", -2.4, 2.4,
        "Track pT [GeV/c]", 0.4, 100,
        "", 0, 0.02,
        false, true, false
    );

    /*
    c1->cd(5);
    plot2D(
        hTotal, "hTotal",
        "Track #eta", -2.4, 2.4,
        "Track pT [GeV/c]", 0.4, 100,
        false, true, false
    );
    */

    c1->SaveAs(Form("%s.png", output));

}