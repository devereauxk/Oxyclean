#include <TFile.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TF1.h>
#include <iostream>

#include "include/plotting.h" // Kyle's plotting utilities

void plot_ppref_TrackCorrection() {

    const char* ppref_input =   "../../MITHIGAnalysis2024/CommonCode/root/Eff_ppref_2024_Pythia_QCDptHat15_NopU_2D_vzpthatWeight_Nominal_10thJune2025.root";
    const char* output =  "plots/ppref_trackCorrection.root";

    TFile* fin = TFile::Open(ppref_input, "READ");
    if (!fin || fin->IsZombie()) {
        std::cerr << "Error: Unable to open file " << fin << std::endl;
        return;
    }

    TH2F* hEff = (TH2F*)fin->Get("hEff_2D");
    TH2F* hMul = (TH2F*)fin->Get("hMul_2D");
    TH2F* hFak = (TH2F*)fin->Get("hFak_2D");
    TH2F* hSec = (TH2F*)fin->Get("hSec_2D");

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
    c1->Divide(2, 4);
    c1->cd(1);
    plot2D(
        hEff, "hEff",
        "Track #eta", -2.4, 2.4,
        "Track pT [GeV/c]", 0.4, 30,
        false
    );

    c1->cd(2);
    plot2D(
        hMul, "hMul",
        "Track #eta", -2.4, 2.4,
        "Track pT [GeV/c]", 0.4, 30,
        false
    );

    c1->cd(3);
    plot2D(
        hFak, "hFak",
        "Track #eta", -2.4, 2.4,
        "Track pT [GeV/c]", 0.4, 30,
        false
    );

    c1->cd(4);
    plot2D(
        hSec, "hSec",
        "Track #eta", -2.4, 2.4,
        "Track pT [GeV/c]", 0.4, 30,
        false
    );

    c1->cd(5);
    plot2D(
        hTotal, "hTotal",
        "Track #eta", -2.4, 2.4,
        "Track pT [GeV/c]", 0.4, 30,
        false
    );

    c1->SaveAs(Form("%s.png", output));

}