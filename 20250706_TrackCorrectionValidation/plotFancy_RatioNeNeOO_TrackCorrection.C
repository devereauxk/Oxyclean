#include <TFile.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TF1.h>
#include <iostream>

#include "include/plotting.h" // Kyle's plotting utilities

void plotFancy_RatioNeNeOO_TrackCorrection() {

    // NeNe private hijing
    //const char* inputNeNe =   "../../MITHIGAnalysis2024/CommonCode/root/PrivateNeNeMC_Hijing_5p36TeV.root";
    //const char* inputOO =   "../../MITHIGAnalysis2024/CommonCode/root/Eff_OO_2025_Hijing_MB_NoPU_2D_Nominal_Official.root";
    //TFile* speciesBySpeciesNeNe = TFile::Open("../../MITHIGAnalysis2024/CommonCode/root/AbsoluteEfficiency_PrivateNeNeMC_Hijing_5p36TeV_byPdgId.root", "READ");
    //TFile* speciesBySpeciesOO = TFile::Open("../../MITHIGAnalysis2024/CommonCode/root/AbsoluteEfficiency_OfficialOOMC_Hijing_5p36TeV_byPdgId.root", "READ");
    //const char* output =  "plots/20250802_NeNe_over_OO_hijing";
    //const char* label = "Hijing";
    
    // NeNe private pythia+hijing
    const char* inputNeNe =   "PrivateNeNeMC_HijingPythia_5p36TeV.root";
    const char* inputOO =   "../../MITHIGAnalysis2024/CommonCode/root/Eff_OO_2025_PythiaHijing_QCD_pThat15_NoPU_pThatweight_2D_Nominal_Official.root";
    TFile* speciesBySpeciesNeNe = TFile::Open("/afs/cern.ch/user/k/kdeverea/CMSSW_15_0_8/src/HITrackingStudies/HITrackingStudies/test/plottingMacro/files_byPdgId/AbsoluteEfficiency_PrivateNeNeMC_PythiaHijing_5p36TeV_byPdgId.root", "READ");
    TFile* speciesBySpeciesOO = TFile::Open("/afs/cern.ch/user/k/kdeverea/CMSSW_15_0_8/src/HITrackingStudies/HITrackingStudies/test/plottingMacro/files_byPdgId/AbsoluteEfficiency_OfficialOOMC_PythiaHijing_5p36TeV_byPdgId.root", "READ");
    const char* output =  "plots/20250815_NeNe_over_OO_pythiahijing";
    const char* label = "Pythia+Hijing";



    TFile* fin = TFile::Open(inputNeNe, "READ");
    if (!fin || fin->IsZombie()) {
        std::cerr << "Error: Unable to open file " << fin << std::endl;
        return;
    }

    TFile* fin_OO = TFile::Open(inputOO, "READ");
    if (!fin_OO || fin_OO->IsZombie()) {
        std::cerr << "Error: Unable to open file " << fin_OO << std::endl;
        return;
    }

    TH2F* hEff = (TH2F*)fin->Get("rEff");
    TH2F* hMul = (TH2F*)fin->Get("rMul");
    TH2F* hFak = (TH2F*)fin->Get("rFak");
    TH2F* hSec = (TH2F*)fin->Get("rSec");

    TH2F* hEff_OO = (TH2F*)fin_OO->Get("hEff_2D");
    TH2F* hMul_OO = (TH2F*)fin_OO->Get("hMul_2D");
    TH2F* hFak_OO = (TH2F*)fin_OO->Get("hFak_2D");
    TH2F* hSec_OO = (TH2F*)fin_OO->Get("hSec_2D");

    TH2F* rEff = (TH2F*)hEff->Clone("rEff_NeNe_over_OO");
    rEff->Divide(hEff_OO);
    TH2F* rMul = (TH2F*)hMul->Clone("rMul_NeNe_over_OO");
    rMul->Divide(hMul_OO);
    TH2F* rFak = (TH2F*)hFak->Clone("rFak_NeNe_over_OO");
    rFak->Divide(hFak_OO);
    TH2F* rSec = (TH2F*)hSec->Clone("rSec_NeNe_over_OO");
    rSec->Divide(hSec_OO);


    // ======================================
    // Absolute efficiency
    // ======================================
    TCanvas* c1 = new TCanvas("c1", "c1", 650, 600);

    TPad* pad1 = (TPad*) plotCMSSimple2D(
        c1, rEff, "",
        "#eta", -2.4, 2.4,
        "p_{T}", 0.4, 100,
        "", 0, 2,
        false, true, false
    );

    AddCMSHeader(
        pad1,
        "Internal",
        false
    );

    AddUPCHeader(
        pad1, "5.26 TeV", label
    );
    pad1->Update();

    c1->SaveAs(Form("%s_AbsoluteEfficiency.pdf", output));

    // ======================================
    // Fakes Rate
    // ======================================
    TCanvas* c2 = new TCanvas("c2", "c2", 650, 600);

    TPad* pad2 = (TPad*) plotCMSSimple2D(
        c2, rFak, "",
        "#eta", -2.4, 2.4,
        "p_{T}", 0.4, 100,
        "", 0, 2,
        false, true, false,
        true
    );

    AddCMSHeader(
        pad2,
        "Internal",
        false
    );

    AddUPCHeader(
        pad2, "5.26 TeV", label
    );
    pad2->Update();

    c2->SaveAs(Form("%s_FakeRate.pdf", output));

    // ======================================
    // Secondary Reconstruction
    // ======================================
    TCanvas* c3 = new TCanvas("c3", "c3", 650, 600);

    TPad* pad3 = (TPad*) plotCMSSimple2D(
        c3, rSec, "",
        "#eta", -2.4, 2.4,
        "p_{T}", 0.4, 100,
        "", 0, 2,
        false, true, false
    );

    AddCMSHeader(
        pad3,
        "Internal",
        false
    );

    AddUPCHeader(
        pad3, "5.26 TeV", label
    );
    pad3->Update();

    c3->SaveAs(Form("%s_SecondaryReconstruction.pdf", output));

    // ======================================
    // Multiple Reconstruction
    // ======================================
    TCanvas* c4 = new TCanvas("c4", "c4", 650, 600);

    TPad* pad4 = (TPad*) plotCMSSimple2D(
        c4, rMul, "",
        "#eta", -2.4, 2.4,
        "p_{T}", 0.4, 100,
        "", 0, 2,
        false, true, false
    );

    AddCMSHeader(
        pad4,
        "Internal",
        false
    );

    AddUPCHeader(
        pad4, "5.26 TeV", label
    );
    pad4->Update();

    c4->SaveAs(Form("%s_MultipleReconstruction.pdf", output));


    // ======================================
    // 1D efficiency
    // ======================================

    TH1D* hEff_1D_NeNe = (TH1D*)speciesBySpeciesNeNe->Get("rEff_1D_0");
    TH1D* hEff_1D_OO = (TH1D*)speciesBySpeciesOO->Get("rEff_1D_0");

    TH1D* rEff_1D = (TH1D*) hEff_1D_NeNe->Clone("rEff_1D_NeNe_over_OO");
    rEff_1D->Divide(hEff_1D_OO);

    TCanvas* c5 = new TCanvas("c5", "c5", 800, 600);

    TPad* pad5 = (TPad*) plotCMSSimple(
        c5, {rEff_1D}, "", {"NeNe / OO"},
        {cmsRed}, {0}, {cmsBlue}, {0},
        "p_{T} [GeV/c]", 3, 40,
        "Efficiency", 0.99, 1.01,
        true, false, false
    );

    AddCMSHeader(
        pad5,
        "Internal",
        false
    );

    AddUPCHeader(
        pad5, "5.26 TeV", label
    );
    pad5->Update();

    c5->SaveAs(Form("%s_1D_Efficiency.pdf", output));

}