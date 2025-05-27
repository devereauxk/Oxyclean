#include <TFile.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TF1.h>
#include <iostream>

const int ccolors[14] = {
    kBlue+2,     // deep blue
    kAzure+7,    // cyan
    kTeal+3,     // teal
    kGreen+2,    // green
    kSpring+7,   // turquoise-green
    kOrange+7,   // orange (avoiding yellow)
    kRed,        // red
    kPink+7,     // pink
    kMagenta+2,  // magenta
    kViolet+7,   // purple
    kGray+2,     // gray
    kBlack,      // black
    kOrange+2,   // extra orange
    kCyan+2      // extra cyan
};

const int scolors[4] = {
    kAzure+7,    // cyan
    kOrange+7,   // orange (avoiding yellow)
    kTeal+3,     // teal
    kPink+7,     // pink
};

void divideByWidth(TH1D* input) {
    if (!input) {
        std::cerr << "Error: Null histogram pointer passed to divideByWidth function." << std::endl;
        return;
    }

    int nBins = input->GetNbinsX();
    for (int i = 1; i <= nBins; ++i) {
        double binContent = input->GetBinContent(i);
        double binError = input->GetBinError(i);
        double binWidth = input->GetBinWidth(i);

        if (binWidth != 0) {
            input->SetBinContent(i, binContent / binWidth);
            input->SetBinError(i, binError / binWidth);
        } else {
            std::cerr << "Warning: Bin width is zero for bin " << i << ". Skipping division for this bin." << std::endl;
        }
    }
}

void setErrors(TH1D* hRatio, TH1D* num, TH1D* denom){
    for (int bin = 1; bin <= hRatio->GetNbinsX(); bin++) {
        double content1 = num->GetBinContent(bin);
        double error1 = num->GetBinError(bin);
        double content2 = denom->GetBinContent(bin);
        double error2 = denom->GetBinError(bin);

        double ratioError = 0;
        if (content2 > 0) {
            ratioError = sqrt((error1 * error1) / (content2 * content2) +
                              (content1 * content1 * error2 * error2) / (content2 * content2 * content2 * content2));
        }
        hRatio->SetBinError(bin, ratioError);
    }
}

void plotRatioLogy(vector<TH1D*> hists, const char* title, vector<string> labels,
    const char* xTitle, double xmin, double xmax,
    const char* yTitle, double ymin, double ymax,
    const char* rTitle, double rmin, double rmax,
    int baseline = 0, bool logy = true) {

    TLegend *leg = new TLegend(0.65, 0.65, 0.85, 0.85);
    leg->SetBorderSize(0); // Remove legend box
    leg->SetTextSize(0.04); // Reduce font size

    TPad *pad1 = new TPad(title, title, 0, 0.3, 1, 1);
    pad1->SetBottomMargin(0);
    logy ? pad1->SetLogy() : pad1->SetLogy(0);
    pad1->Draw();
    TPad *pad2 = new TPad(title, title, 0, 0, 1, 0.3);
    pad2->SetTopMargin(0);
    pad2->SetBottomMargin(0.2);
    pad2->Draw();

    for (int i = 0; i < hists.size(); i++) {
        pad1->cd();

        TH1D* hist = hists[i];

        hist->SetStats(0);
        hist->SetTitle(title);
        hist->GetXaxis()->SetTitle(xTitle);
        hist->GetXaxis()->SetRangeUser(xmin, xmax);
        hist->GetYaxis()->SetTitle(yTitle);
        hist->GetYaxis()->SetRangeUser(ymin, ymax);
        hist->GetYaxis()->SetTitleSize(0.05);
        hist->SetLineColor(ccolors[i]);

        if (i == baseline) {
            hist->Draw("E SAME");
        } else {
            hist->Draw("HIST SAME");
        }
        leg->AddEntry(hist, Form("%s", labels[i].c_str()), "l");

        pad2->cd();
        if (i != baseline) {
            TH1D* hRatio = (TH1D*)hist->Clone(Form("ratio_TrkPt_%d", i));
            hRatio->Divide(hists[baseline]);
            hRatio->SetTitle("");
            hRatio->SetStats(0);
            hRatio->GetXaxis()->SetTitle(xTitle);
            hRatio->GetXaxis()->SetTitleSize(0.1);
            hRatio->GetXaxis()->SetLabelSize(0.08);
            hRatio->GetXaxis()->SetTitleOffset(0.4);
            hRatio->GetYaxis()->SetTitle(rTitle);
            hRatio->GetYaxis()->SetTitleSize(0.1);
            hRatio->GetYaxis()->SetLabelSize(0.08);
            hRatio->GetYaxis()->SetTitleOffset(0.4);
            hRatio->GetYaxis()->SetRangeUser(rmin, rmax);
            hRatio->SetLineColor(ccolors[i]);

            // Calculate statistical errors for the ratio
            setErrors(hRatio, hist, hists.at(baseline));

            cout<<title<<" "<<labels[i]<<" "<<hRatio->Integral()<<endl;

            hRatio->Draw("E SAME");

            TLine *line = new TLine(xmin, 1, xmax, 1);
            line->SetLineColor(kGray+2);
            line->SetLineStyle(2);
            line->Draw("SAME");
        }
    }
    pad1->cd();
    leg->Draw("SAME");
}

void plot2D(TH2D* hist, const char* title,
    const char* xTitle, double xmin, double xmax,
    const char* yTitle, double ymin, double ymax) {

    TPad *pad1 = new TPad(title, title, 0, 0, 1, 1);
    pad1->SetBottomMargin(0.18); // Increase bottom margin for x-axis labels/ticks
    pad1->SetLogx(0);
    pad1->SetLogy(0);
    pad1->SetLogz(1);
    pad1->Draw();
    pad1->cd();

    hist->SetStats(0);
    hist->SetTitle(title);
    hist->SetTitleSize(0.03, "XYZ"); // Decrease title size
    hist->GetXaxis()->SetTitle(xTitle);
    hist->GetXaxis()->SetTitleSize(0.05);
    hist->GetXaxis()->SetLabelSize(0.045);
    hist->GetXaxis()->SetRangeUser(xmin, xmax);
    hist->GetYaxis()->SetTitle(yTitle);
    hist->GetYaxis()->SetTitleSize(0.05);
    hist->GetYaxis()->SetLabelSize(0.045);
    hist->GetYaxis()->SetRangeUser(ymin, ymax);

    hist->Draw("COLZ");
}

void plotSimple(vector<TH1D*> hists, const char* title, vector<string> labels,
    const char* xTitle, double xmin, double xmax,
    const char* yTitle, double ymin, double ymax,
    bool logx = false, bool logy = false, bool binnums = false) {

    TPad *pad1 = new TPad(title, title, 0, 0, 1, 1);
    pad1->SetBottomMargin(0.18); // Increase bottom margin for x-axis labels/ticks
    logx ? pad1->SetLogx() : pad1->SetLogx(0);
    logy ? pad1->SetLogy() : pad1->SetLogy(0);
    pad1->Draw();
    pad1->cd();

    TLegend *leg = new TLegend(0.68, 0.75, 0.58, 0.85); // smaller, upper right
    leg->SetBorderSize(0);
    leg->SetTextSize(0.035);

    double global_min = 1e30, global_max = -1e30;

    // Find global min/max for all hists in the visible x range
    for (size_t ih = 0; ih < hists.size(); ++ih) {
        TH1D* hist = hists[ih];
        double x1 = (xmin < xmax) ? xmin : hist->GetXaxis()->GetXmin();
        double x2 = (xmin < xmax) ? xmax : hist->GetXaxis()->GetXmax();
        int binmin = hist->GetXaxis()->FindBin(x1);
        int binmax = hist->GetXaxis()->FindBin(x2);
        for (int i = binmin; i <= binmax; ++i) {
            double val = hist->GetBinContent(i);
            if (val < global_min) global_min = val;
            if (val > global_max) global_max = val;
        }
    }
    // Add some margin
    double margin = 0.2 * (global_max - global_min);

    for (size_t ih = 0; ih < hists.size(); ++ih) {
        TH1D* hist = hists[ih];
        hist->SetStats(0);
        hist->SetTitle(title);
        hist->GetXaxis()->SetTitle(xTitle);
        hist->GetXaxis()->SetTitleSize(0.05);
        hist->GetXaxis()->SetLabelSize(0.045);

        // Auto-scale x axis if xmin >= xmax
        if (xmin < xmax) {
            hist->GetXaxis()->SetRangeUser(xmin, xmax);
        } else {
            hist->GetXaxis()->SetRangeUser(hist->GetXaxis()->GetXmin(), hist->GetXaxis()->GetXmax());
        }

        hist->GetYaxis()->SetTitle(yTitle);
        hist->GetYaxis()->SetTitleSize(0.05);
        hist->GetYaxis()->SetLabelSize(0.03);
        hist->GetYaxis()->SetTitleOffset(1);

        // Auto-scale y axis if ymin < ymax, otherwise use global min/max
        if (ymin < ymax) {
            hist->GetYaxis()->SetRangeUser(ymin, ymax);
        } else {
            hist->GetYaxis()->SetRangeUser(global_min - margin, global_max + margin);
        }

        hist->SetLineColor(scolors[ih]);
        hist->SetLineWidth(2);

        // Set dashed line style for the second curve (ih == 1)
        if (ih == 1) {
            hist->SetLineStyle(2); // dashed
        } else {
            hist->SetLineStyle(1); // solid
        }

        if (ih == 0) {
            binnums ? hist->Draw("HIST TEXT0") : hist->Draw("HIST");
        } else {
            binnums ? hist->Draw("HIST TEXT0 SAME") : hist->Draw("HIST SAME");
        }

        if (labels.size() > ih)
            leg->AddEntry(hist, labels[ih].c_str(), "l");
    }

    leg->Draw("SAME");
}

// example usage
// root -l -b -q "plotForest.C(\"skimValidation.root\", \"plots\")"
// root -l -b -q "plotForest.C(\"skimValidation_cut.root\", \"plots\", \"cut\")"

/*
void plotForestOnly(const char* input="skimValidation.root", const char* outputDir="plots", const char* suffix="cut") {

    // Open the input ROOT file
    TFile* finput = TFile::Open(input, "READ");
    if (!finput || finput->IsZombie()) {
        std::cerr << "Error: Unable to open file " << input << std::endl;
        return;
    }

    // Read in histograms
    TH1D* hNEvtPassCuts = (TH1D*)finput->Get("hNEvtPassCuts");
    TH1D* hNRun = (TH1D*)finput->Get("hnRun");
    TH1D* hNEvt = (TH1D*)finput->Get("hnEv");
    TH1D* hNLumi = (TH1D*)finput->Get("hnLumi");
    TH1D* hNVtx = (TH1D*)finput->Get("hnVtx");
    TH1D* hNpart = (TH1D*)finput->Get("hNpart");
    TH1D* hNcoll = (TH1D*)finput->Get("hNcoll");
    TH1D* hhiBin = (TH1D*)finput->Get("hhiBin");
    TH1D* hhiHF_pf = (TH1D*)finput->Get("hhiHF_pf");

    TH3D* bestVertexXYZ = (TH3D*)finput->Get("bestVertexXYZ");
    TH3D* bestVertexXYZErr = (TH3D*)finput->Get("bestVertexXYZErr");

    // make canvas
    TCanvas* c1 = new TCanvas("c1", "c1", 2400, 2400);
    c1->Divide(3, 4);

    c1->cd(1);
    plotSimple(
        hNEvtPassCuts, "hNEvtPassCuts",
        "hNEvtPassCuts", -1, -1,
        "Counts", 35000, 36500,
        false, false,
        true
    );

    c1->cd(2);
    plotSimple(
        hNRun, "hNRun",
        "hNRun", -1, -1,
        "Counts", -1, -1,
        false, false
    );

    c1->cd(3);
    plotSimple(
        hNEvt, "hNEvt",
        "hNEvt", -1, -1,
        "Counts", -1, -1,
        false, false
    );

    c1->cd(4);
    plotSimple(
        hNLumi, "hNLumi",
        "hNLumi", -1, -1,
        "Counts", -1, -1,
        false, false
    );

    c1->cd(5);
    plotSimple(
        hNVtx, "hNVtx",
        "hNVtx", -1, -1,
        "Counts", -1, -1,
        false, false
    );

    c1->cd(6);
    plotSimple(
        hNpart, "hNpart",
        "hNpart", -1, -1,
        "Counts", -1, -1,
        false, false
    );

    c1->cd(7);
    plotSimple(
        hNcoll, "hNcoll",
        "hNcoll", -1, -1,
        "Counts", -1, -1,
        false, false
    );

    c1->cd(8);
    plotSimple(
        hhiBin, "hhiBin",
        "hhiBin", -1, -1,
        "Counts", -1, -1,
        false, false
    );

    c1->cd(9);
    plotSimple(
        hhiHF_pf, "hhiHF_pf",
        "hhiHF_pf", -1, -1,
        "Counts", -1, -1,
        false, false
    );

    c1->cd(10);
    plotSimple(
        bestVertexXYZ->ProjectionX("bestVertexXYX_x"), "bestVertexX",
        "X [cm]", -1, -1,
        "Counts", -1, -1,
        false, false
    );

    c1->cd(11);
    plotSimple(
        bestVertexXYZ->ProjectionY("bestVertexXYX_y"), "bestVertexY",
        "Y [cm]", -1, -1,
        "Counts", -1, -1,
        false, false
    );

    c1->cd(12);
    plotSimple(
        bestVertexXYZ->ProjectionZ("bestVertexXYX_z"), "bestVertexZ",
        "Z [cm]", -1, -1,
        "Counts", -1, -1,
        false, false
    );


    // Save as png
    c1->SaveAs(Form("%s/forest-%s.png", outputDir, suffix));
}
*/

void plotForest(const char* forestInput =   "output/skimValidation_forest.root",
                const char* skimInput =     "output/skimValidation_skim.root",
                const char* output =        "plots/forest_and_skim.png") {

    // Open the input ROOT files
    TFile* fForest = TFile::Open(forestInput, "READ");
    if (!fForest || fForest->IsZombie()) {
        std::cerr << "Error: Unable to open forest file " << forestInput << std::endl;
        return;
    }
    TFile* fSkim = TFile::Open(skimInput, "READ");
    if (!fSkim || fSkim->IsZombie()) {
        std::cerr << "Error: Unable to open skim file " << skimInput << std::endl;
        return;
    }

    // Read in histograms from both files and fill vectors
    std::vector<string> labels = {"forest", "skim"};    
    // Fetch and uniquely name 1D histograms
    TH1D* hNEvtPassCuts_forest = (TH1D*)fForest->Get("hNEvtPassCuts"); hNEvtPassCuts_forest->SetName("hNEvtPassCuts_forest");
    TH1D* hNEvtPassCuts_skim   = (TH1D*)fSkim->Get("hNEvtPassCuts");   hNEvtPassCuts_skim->SetName("hNEvtPassCuts_skim");
    std::vector<TH1D*> hNEvtPassCuts = { hNEvtPassCuts_forest, hNEvtPassCuts_skim };

    TH1D* hNRun_forest = (TH1D*)fForest->Get("hnRun"); hNRun_forest->SetName("hNRun_forest");
    TH1D* hNRun_skim   = (TH1D*)fSkim->Get("hnRun");   hNRun_skim->SetName("hNRun_skim");
    std::vector<TH1D*> hNRun = { hNRun_forest, hNRun_skim };

    TH1D* hNEvt_forest = (TH1D*)fForest->Get("hnEv"); hNEvt_forest->SetName("hNEvt_forest");
    TH1D* hNEvt_skim   = (TH1D*)fSkim->Get("hnEv");   hNEvt_skim->SetName("hNEvt_skim");
    std::vector<TH1D*> hNEvt = { hNEvt_forest, hNEvt_skim };

    TH1D* hNLumi_forest = (TH1D*)fForest->Get("hnLumi"); hNLumi_forest->SetName("hNLumi_forest");
    TH1D* hNLumi_skim   = (TH1D*)fSkim->Get("hnLumi");   hNLumi_skim->SetName("hNLumi_skim");
    std::vector<TH1D*> hNLumi = { hNLumi_forest, hNLumi_skim };

    TH1D* hNVtx_forest = (TH1D*)fForest->Get("hnVtx"); hNVtx_forest->SetName("hNVtx_forest");
    TH1D* hNVtx_skim   = (TH1D*)fSkim->Get("hnVtx");   hNVtx_skim->SetName("hNVtx_skim");
    std::vector<TH1D*> hNVtx = { hNVtx_forest, hNVtx_skim };

    TH1D* hNpart_forest = (TH1D*)fForest->Get("hNpart"); hNpart_forest->SetName("hNpart_forest");
    TH1D* hNpart_skim   = (TH1D*)fSkim->Get("hNpart");   hNpart_skim->SetName("hNpart_skim");
    std::vector<TH1D*> hNpart = { hNpart_forest, hNpart_skim };

    TH1D* hNcoll_forest = (TH1D*)fForest->Get("hNcoll"); hNcoll_forest->SetName("hNcoll_forest");
    TH1D* hNcoll_skim   = (TH1D*)fSkim->Get("hNcoll");   hNcoll_skim->SetName("hNcoll_skim");
    std::vector<TH1D*> hNcoll = { hNcoll_forest, hNcoll_skim };

    TH1D* hhiBin_forest = (TH1D*)fForest->Get("hhiBin"); hhiBin_forest->SetName("hhiBin_forest");
    TH1D* hhiBin_skim   = (TH1D*)fSkim->Get("hhiBin");   hhiBin_skim->SetName("hhiBin_skim");
    std::vector<TH1D*> hhiBin = { hhiBin_forest, hhiBin_skim };

    TH1D* hhiHF_pf_forest = (TH1D*)fForest->Get("hhiHF_pf"); hhiHF_pf_forest->SetName("hhiHF_pf_forest");
    TH1D* hhiHF_pf_skim   = (TH1D*)fSkim->Get("hhiHF_pf");   hhiHF_pf_skim->SetName("hhiHF_pf_skim");
    std::vector<TH1D*> hhiHF_pf = { hhiHF_pf_forest, hhiHF_pf_skim };

    // Fetch and uniquely name 3D histograms
    TH3D* VXYZ_forest = (TH3D*)fForest->Get("VXYZ"); VXYZ_forest->SetName("VXYZ_forest");
    TH3D* VXYZ_skim   = (TH3D*)fSkim->Get("VXYZ");   VXYZ_skim->SetName("VXYZ_skim");
    std::vector<TH3D*> VXYZ = { VXYZ_forest, VXYZ_skim };

    TH3D* VXYZErr_forest = (TH3D*)fForest->Get("VXYZErr"); VXYZErr_forest->SetName("VXYZErr_forest");
    TH3D* VXYZErr_skim   = (TH3D*)fSkim->Get("VXYZErr");   VXYZErr_skim->SetName("VXYZErr_skim");
    std::vector<TH3D*> VXYZErr = { VXYZErr_forest, VXYZErr_skim };

    // Projections with unique names
    std::vector<TH1D*> VX = {
        (TH1D*)VXYZ[0]->ProjectionX("VX_forest"),
        (TH1D*)VXYZ[1]->ProjectionX("VX_skim")
    };
    std::vector<TH1D*> VY = {
        (TH1D*)VXYZ[0]->ProjectionY("VY_forest"),
        (TH1D*)VXYZ[1]->ProjectionY("VY_skim")
    };
    std::vector<TH1D*> VZ = {
        (TH1D*)VXYZ[0]->ProjectionZ("VZ_forest"),
        (TH1D*)VXYZ[1]->ProjectionZ("VZ_skim")
    };
    std::vector<TH1D*> VXErr = {
        (TH1D*)VXYZErr[0]->ProjectionX("VXErr_forest"),
        (TH1D*)VXYZErr[1]->ProjectionX("VXErr_skim")
    };
    std::vector<TH1D*> VYErr = {
        (TH1D*)VXYZErr[0]->ProjectionY("VYErr_forest"),
        (TH1D*)VXYZErr[1]->ProjectionY("VYErr_skim")
    };
    std::vector<TH1D*> VZErr = {
        (TH1D*)VXYZErr[0]->ProjectionZ("VZErr_forest"),
        (TH1D*)VXYZErr[1]->ProjectionZ("VZErr_skim")
    };


    // make canvas
    TCanvas* c1 = new TCanvas("c1", "c1", 2400, 3000);
    c1->Divide(3, 5);

    c1->cd(1);
    plotSimple(
        hNEvtPassCuts, "hNEvtPassCuts", labels,
        "hNEvtPassCuts", -1, -1,
        "Counts", 35000, 36500,
        false, false,
        true
    );

    c1->cd(2);
    plotSimple(
        hNRun, "hNRun", labels,
        "hNRun", -1, -1,
        "Counts", -1, -1,
        false, false
    );

    /*
    c1->cd(3);
    plotSimple(
        hNEvt, "hNEvt", labels,
        "hNEvt", -1, -1,
        "Counts", -1, -1,
        false, false
    );
    */

    c1->cd(4);
    plotSimple(
        hNLumi, "hNLumi", labels,
        "hNLumi", -1, -1,
        "Counts", -1, -1,
        false, false
    );

    c1->cd(5);
    plotSimple(
        hNVtx, "hNVtx", labels,
        "hNVtx", -1, -1,
        "Counts", -1, -1,
        false, false
    );

    c1->cd(6);
    plotSimple(
        hNpart, "hNpart", labels,
        "hNpart", -1, -1,
        "Counts", -1, -1,
        false, false
    );

    c1->cd(7);
    plotSimple(
        hNcoll, "hNcoll", labels,
        "hNcoll", -1, -1,
        "Counts", -1, -1,
        false, false
    );

    c1->cd(8);
    plotSimple(
        hhiBin, "hhiBin", labels,
        "hhiBin", -1, -1,
        "Counts", -1, -1,
        false, false
    );

    c1->cd(9);
    plotSimple(
        hhiHF_pf, "hhiHF_pf", labels,
        "hhiHF_pf", -1, -1,
        "Counts", -1, -1,
        false, false
    );

    c1->cd(10);
    plotSimple(
        VX, "VX", labels,
        "X [cm]", -0.02, 0.1,
        "Counts", -1, -1,
        false, false
    );

    c1->cd(11);
    plotSimple(
        VY, "VY", labels,
        "Y [cm]", -0.6, -0.6,
        "Counts", -1, -1,
        false, false
    );

    c1->cd(12);
    plotSimple(
        VZ, "VZ", labels,
        "Z [cm]", -1, -1,
        "Counts", -1, -1,
        false, false
    );

    c1->cd(13);
    plotSimple(
        VXErr, "VXErr", labels,
        "X [cm]", -0.02, 0.1,
        "Counts", -1, -1,
        false, false
    );

    c1->cd(14);
    plotSimple(
        VYErr, "VYErr", labels,
        "Y [cm]", -0.6, -0.6,
        "Counts", -1, -1,
        false, false
    );

    c1->cd(15);
    plotSimple(
        VZErr, "VZErr", labels,
        "Z [cm]", -1, -1,
        "Counts", -1, -1,
        false, false
    );


    // Save as png
    c1->SaveAs(Form("%s", output));
}