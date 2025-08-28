#include <TFile.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TF1.h>
#include <iostream>

#include "include/plotting.h" // Kyle's plotting utilities

void plotFancy_NeNe_TrackCorrection() {

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

    // ======================================
    // Absolute efficiency
    // ======================================
    TCanvas* c1 = new TCanvas("c1", "c1", 650, 600);

    TPad* pad1 = (TPad*) plotCMSSimple2D(
        c1, hEff, "",
        "#eta", -2.4, 2.4,
        "p_{T}", 0.4, 100,
        "", 0, 1.1,
        false, true, false
    );

    AddCMSHeader(
        pad1,
        "Internal",
        false
    );

    AddUPCHeader(
        pad1, "5.26 TeV", "NeNe HIJING"
    );
    pad1->Update();

    c1->SaveAs(Form("%s_AbsoluteEfficiency.pdf", output));

    // ======================================
    // Fakes Rate
    // ======================================
    TCanvas* c2 = new TCanvas("c2", "c2", 650, 600);

    TPad* pad2 = (TPad*) plotCMSSimple2D(
        c2, hFak, "",
        "#eta", -2.4, 2.4,
        "p_{T}", 0.4, 100,
        "", 0, 0.1,
        false, true, false
    );

    AddCMSHeader(
        pad2,
        "Internal",
        false
    );

    AddUPCHeader(
        pad2, "5.26 TeV", "NeNe HIJING"
    );
    pad2->Update();

    c2->SaveAs(Form("%s_FakeRate.pdf", output));

    // ======================================
    // Secondary Reconstruction
    // ======================================
    TCanvas* c3 = new TCanvas("c3", "c3", 650, 600);

    TPad* pad3 = (TPad*) plotCMSSimple2D(
        c3, hSec, "",
        "#eta", -2.4, 2.4,
        "p_{T}", 0.4, 100,
        "", 0, 0.02,
        false, true, false
    );

    AddCMSHeader(
        pad3,
        "Internal",
        false
    );

    AddUPCHeader(
        pad3, "5.26 TeV", "NeNe HIJING"
    );
    pad3->Update();

    c3->SaveAs(Form("%s_SecondaryReconstruction.pdf", output));

    // ======================================
    // Multiple Reconstruction
    // ======================================
    TCanvas* c4 = new TCanvas("c4", "c4", 650, 600);

    TPad* pad4 = (TPad*) plotCMSSimple2D(
        c4, hMul, "",
        "#eta", -2.4, 2.4,
        "p_{T}", 0.4, 100,
        "", 0, 4e-3,
        false, true, false
    );

    AddCMSHeader(
        pad4,
        "Internal",
        false
    );

    AddUPCHeader(
        pad4, "5.26 TeV", "NeNe HIJING"
    );
    pad4->Update();

    c4->SaveAs(Form("%s_MultipleReconstruction.pdf", output));


}