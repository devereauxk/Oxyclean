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


void plotForest(const char* forestInput =   "output/skimValidation_forest.root",
                const char* skimInput =     "output/skimValidation_skim.root",
                const char* output =        "plots/forest_and_skim") {

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

    TH1D* hTrkPhi_forest = (TH1D*)fForest->Get("hTrkPhi"); hTrkPhi_forest->SetName("hTrkPhi_forest");
    TH1D* hTrkPhi_skim   = (TH1D*)fSkim->Get("hTrkPhi");   hTrkPhi_skim->SetName("hTrkPhi_skim");
    std::vector<TH1D*> hTrkPhi = { hTrkPhi_forest, hTrkPhi_skim };

    TH1D* hTrkCharge_forest = (TH1D*)fForest->Get("hTrkCharge"); hTrkCharge_forest->SetName("hTrkCharge_forest");
    TH1D* hTrkCharge_skim   = (TH1D*)fSkim->Get("hTrkCharge");   hTrkCharge_skim->SetName("hTrkCharge_skim");
    std::vector<TH1D*> hTrkCharge = { hTrkCharge_forest, hTrkCharge_skim };

    TH1D* hTrkNHits_forest = (TH1D*)fForest->Get("hTrkNHits"); hTrkNHits_forest->SetName("hTrkNHits_forest");
    TH1D* hTrkNHits_skim   = (TH1D*)fSkim->Get("hTrkNHits");   hTrkNHits_skim->SetName("hTrkNHits_skim");
    std::vector<TH1D*> hTrkNHits = { hTrkNHits_forest, hTrkNHits_skim };

    TH1D* hTrkNPixHits_forest = (TH1D*)fForest->Get("hTrkNPixHits"); hTrkNPixHits_forest->SetName("hTrkNPixHits_forest");
    TH1D* hTrkNPixHits_skim   = (TH1D*)fSkim->Get("hTrkNPixHits");   hTrkNPixHits_skim->SetName("hTrkNPixHits_skim");
    std::vector<TH1D*> hTrkNPixHits = { hTrkNPixHits_forest, hTrkNPixHits_skim };

    TH1D* hTrkNLayers_forest = (TH1D*)fForest->Get("hTrkNLayers"); hTrkNLayers_forest->SetName("hTrkNLayers_forest");
    TH1D* hTrkNLayers_skim   = (TH1D*)fSkim->Get("hTrkNLayers");   hTrkNLayers_skim->SetName("hTrkNLayers_skim");
    std::vector<TH1D*> hTrkNLayers = { hTrkNLayers_forest, hTrkNLayers_skim };

    TH1D* hTrkNormChi2_forest = (TH1D*)fForest->Get("hTrkNormChi2"); hTrkNormChi2_forest->SetName("hTrkNormChi2_forest");
    TH1D* hTrkNormChi2_skim   = (TH1D*)fSkim->Get("hTrkNormChi2");   hTrkNormChi2_skim->SetName("hTrkNormChi2_skim");
    std::vector<TH1D*> hTrkNormChi2 = { hTrkNormChi2_forest, hTrkNormChi2_skim };

    TH1D* hpfEnergy_forest = (TH1D*)fForest->Get("hpfEnergy"); hpfEnergy_forest->SetName("hpfEnergy_forest");
    TH1D* hpfEnergy_skim   = (TH1D*)fSkim->Get("hpfEnergy");   hpfEnergy_skim->SetName("hpfEnergy_skim");
    std::vector<TH1D*> hpfEnergy = { hpfEnergy_forest, hpfEnergy_skim };

    TH1D* hHFEMaxPlus_forest = (TH1D*)fForest->Get("hHFEMaxPlus"); hHFEMaxPlus_forest->SetName("hHFEMaxPlus_forest");
    TH1D* hHFEMaxPlus_skim   = (TH1D*)fSkim->Get("hHFEMaxPlus");   hHFEMaxPlus_skim->SetName("hHFEMaxPlus_skim");
    std::vector<TH1D*> hHFEMaxPlus = { hHFEMaxPlus_forest, hHFEMaxPlus_skim };

    TH1D* hHFEMaxPlus2_forest = (TH1D*)fForest->Get("hHFEMaxPlus2");
    hHFEMaxPlus2_forest->SetName("hHFEMaxPlus2_forest");
    TH1D* hHFEMaxPlus2_skim   = (TH1D*)fSkim->Get("hHFEMaxPlus2");
    hHFEMaxPlus2_skim->SetName("hHFEMaxPlus2_skim");
    std::vector<TH1D*> hHFEMaxPlus2 = { hHFEMaxPlus2_forest, hHFEMaxPlus2_skim };
    
    TH1D* hHFEMaxPlus3_forest = (TH1D*)fForest->Get("hHFEMaxPlus3");
    hHFEMaxPlus3_forest->SetName("hHFEMaxPlus3_forest");
    TH1D* hHFEMaxPlus3_skim   = (TH1D*)fSkim->Get("hHFEMaxPlus3");
    hHFEMaxPlus3_skim->SetName("hHFEMaxPlus3_skim");
    std::vector<TH1D*> hHFEMaxPlus3 = { hHFEMaxPlus3_forest, hHFEMaxPlus3_skim };

    TH1D* hHFEMaxMinus_forest = (TH1D*)fForest->Get("hHFEMaxMinus"); hHFEMaxMinus_forest->SetName("hHFEMaxMinus_forest");
    TH1D* hHFEMaxMinus_skim   = (TH1D*)fSkim->Get("hHFEMaxMinus");   hHFEMaxMinus_skim->SetName("hHFEMaxMinus_skim");
    std::vector<TH1D*> hHFEMaxMinus = { hHFEMaxMinus_forest, hHFEMaxMinus_skim };

    TH1D* hHFEMaxMinus2_forest = (TH1D*)fForest->Get("hHFEMaxMinus2");
    hHFEMaxMinus2_forest->SetName("hHFEMaxMinus2_forest");
    TH1D* hHFEMaxMinus2_skim   = (TH1D*)fSkim->Get("hHFEMaxMinus2");
    hHFEMaxMinus2_skim->SetName("hHFEMaxMinus2_skim");
    std::vector<TH1D*> hHFEMaxMinus2 = { hHFEMaxMinus2_forest, hHFEMaxMinus2_skim };

    TH1D* hHFEMaxMinus3_forest = (TH1D*)fForest->Get("hHFEMaxMinus3");
    hHFEMaxMinus3_forest->SetName("hHFEMaxMinus3_forest");
    TH1D* hHFEMaxMinus3_skim   = (TH1D*)fSkim->Get("hHFEMaxMinus3");
    hHFEMaxMinus3_skim->SetName("hHFEMaxMinus3_skim");
    std::vector<TH1D*> hHFEMaxMinus3 = { hHFEMaxMinus3_forest, hHFEMaxMinus3_skim };

    // Fetch and uniquely name 3D histograms
    TH3D* VXYZ_forest = (TH3D*)fForest->Get("VXYZ"); VXYZ_forest->SetName("VXYZ_forest");
    TH3D* VXYZ_skim   = (TH3D*)fSkim->Get("VXYZ");   VXYZ_skim->SetName("VXYZ_skim");
    std::vector<TH3D*> VXYZ = { VXYZ_forest, VXYZ_skim };

    TH3D* VXYZErr_forest = (TH3D*)fForest->Get("VXYZErr"); VXYZErr_forest->SetName("VXYZErr_forest");
    TH3D* VXYZErr_skim   = (TH3D*)fSkim->Get("VXYZErr");   VXYZErr_skim->SetName("VXYZErr_skim");
    std::vector<TH3D*> VXYZErr = { VXYZErr_forest, VXYZErr_skim };

    TH3D* htrkPtEtaHighPurity_forest = (TH3D*)fForest->Get("trkPtEtaHighPurity");
    htrkPtEtaHighPurity_forest->SetName("trkPtEtaHighPurity_forest");
    TH3D* htrkPtEtaHighPurity_skim   = (TH3D*)fSkim->Get("trkPtEtaHighPurity");
    htrkPtEtaHighPurity_skim->SetName("trkPtEtaHighPurity_skim");
    std::vector<TH3D*> htrkPtEtaHighPurity = { htrkPtEtaHighPurity_forest, htrkPtEtaHighPurity_skim };

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
    std::vector<TH1D*> htrkPt = {
        (TH1D*)htrkPtEtaHighPurity[0]->ProjectionX("trkPt_forest"),
        (TH1D*)htrkPtEtaHighPurity[1]->ProjectionX("trkPt_skim")
    };
    std::vector<TH1D*> htrkEta = {
        (TH1D*)htrkPtEtaHighPurity[0]->ProjectionY("trkEta_forest"),
        (TH1D*)htrkPtEtaHighPurity[1]->ProjectionY("trkEta_skim")
    };
    std::vector<TH1D*> hhighPurity = {
        (TH1D*)htrkPtEtaHighPurity[0]->ProjectionZ("highPurity_forest"),
        (TH1D*)htrkPtEtaHighPurity[1]->ProjectionZ("highPurity_skim")
    };
    // Project trkPt for |eta| < 1 and highPurity bin centered at 1
    int eta_min_bin = htrkPtEtaHighPurity[0]->GetYaxis()->FindBin(-1.0 + 1e-6);
    int eta_max_bin = htrkPtEtaHighPurity[0]->GetYaxis()->FindBin(1.0 - 1e-6);
    int purity_bin = htrkPtEtaHighPurity[0]->GetZaxis()->FindBin(1.0);

    std::vector<TH1D*> htrkPt_eta1 = {
        (TH1D*)htrkPtEtaHighPurity[0]->ProjectionX("trkPt_Eta1_forest", eta_min_bin, eta_max_bin, 0, -1),
        (TH1D*)htrkPtEtaHighPurity[1]->ProjectionX("trkPt_Eta1_skim", eta_min_bin, eta_max_bin, 0, -1)
    };
    std::vector<TH1D*> htrkPt_highPurity = {
        (TH1D*)htrkPtEtaHighPurity[0]->ProjectionX("trkPt_highPurity_forest", 0, -1, purity_bin, purity_bin),
        (TH1D*)htrkPtEtaHighPurity[1]->ProjectionX("trkPt_highPurity_skim", 0, -1, purity_bin, purity_bin)
    };

    std::vector<TH1D*> htrkPt_eta1_highPurity = {
        (TH1D*)htrkPtEtaHighPurity[0]->ProjectionX("trkPt_Eta1_highPurity_forest", eta_min_bin, eta_max_bin, purity_bin, purity_bin),
        (TH1D*)htrkPtEtaHighPurity[1]->ProjectionX("trkPt_Eta1_highPurity_skim", eta_min_bin, eta_max_bin, purity_bin, purity_bin)
    };
    std::vector<TH1D*> htrkEta_highPurity = {
        (TH1D*)htrkPtEtaHighPurity[0]->ProjectionY("trkEta_highPurity_forest", 0, -1, purity_bin, purity_bin),
        (TH1D*)htrkPtEtaHighPurity[1]->ProjectionY("trkEta_highPurity_skim", 0, -1, purity_bin, purity_bin)
    };


    // make canvas
    TCanvas* c1 = new TCanvas("c1", "c1", 2400, 5400);
    c1->Divide(3, 8);

    c1->cd(1);
    plotSimple(
        hNEvtPassCuts, "NEvtPassCuts", labels,
        "NEvtPassCuts", -1, -1,
        "Counts", 0.97e6, 1e6,
        false, false,
        true
    );

    c1->cd(2);
    plotSimple(
        hNRun, "NRun", labels,
        "NRun", -1, -1,
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
        hNLumi, "NLumi", labels,
        "NLumi", -1, -1,
        "Counts", 17000, 21000,
        false, false
    );

    c1->cd(5);
    plotSimple(
        hNVtx, "NVtx", labels,
        "NVtx", -1, -1,
        "Counts", -1, -1,
        false, false
    );

    c1->cd(6);
    plotSimple(
        hNpart, "Npart", labels,
        "Npart", -1, -1,
        "Counts", -1, -1,
        false, false
    );

    c1->cd(7);
    plotSimple(
        hNcoll, "Ncoll", labels,
        "Ncoll", -1, -1,
        "Counts", -1, -1,
        false, false
    );

    c1->cd(8);
    plotSimple(
        hpfEnergy, "pfEnergy", labels,
        "PF Energy [GeV]", -1, -1,
        "Counts", -1, -1,
        false, false
    );

    c1->cd(9);
    plotSimple(
        hhiHF_pf, "hiHF_pf", labels,
        "hiHF_pf", -1, -1,
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
        "X [cm]", -1, -1,
        "Counts", -1, -1,
        false, false
    );

    c1->cd(14);
    plotSimple(
        VYErr, "VYErr", labels,
        "Y [cm]", -1, -1,
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

    c1->cd(16);
    plotSimple(
        htrkPt, "trkPt", labels,
        "p_{T} [GeV/c]", -1, -1,
        "Counts", -1, -1,
        false, false
    );

    c1->cd(17);
    plotSimple(
        htrkEta, "trkEta", labels,
        "#eta", -1, -1,
        "Counts", -1, -1,
        false, false
    );

    c1->cd(18);
    plotSimple(
        hhighPurity, "highPurity", labels,
        "High Purity Tracks", -1, -1,
        "Counts", -1, -1,
        false, false
    );

    c1->cd(19);
    plotSimple(
        htrkPt_eta1, "trkPt_Eta1", labels,
        "p_{T} [GeV/c]", -1, -1,
        "Counts", -1, -1,
        false, false
    );

    c1->cd(20);
    plotSimple(
        htrkPt_highPurity, "trkPt_highPurity", labels,
        "p_{T} [GeV/c]", -1, -1,
        "Counts", -1, -1,
        false, false
    );

    c1->cd(21);
    plotSimple(
        htrkPt_eta1_highPurity, "trkPt_Eta1_highPurity", labels,
        "p_{T} [GeV/c]", -1, -1,
        "Counts", -1, -1,
        false, false
    );

    c1->cd(23);
    plotSimple(
        htrkEta_highPurity, "trkEta_highPurity", labels,
        "#eta", -1, -1,
        "Counts", -1, -1,
        false, false
    );

    
    // Save as png
    c1->SaveAs(Form("%s.png", output));

    TCanvas* c2 = new TCanvas("c2", "c2", 2400, 2400);
    c2->Divide(3, 4);

    c2->cd(1);
    plotSimple(
        hHFEMaxPlus, "HFEMaxPlus", labels,
        "HF E^{+} Max [GeV]", -1, -1,
        "Counts", -5e3, 3e4,
        false, false
    );

    c2->cd(2);
    plotSimple(
        hHFEMaxPlus2, "HFEMaxPlus2", labels,
        "HF E^{+} Max 2 [GeV]", -1, -1,
        "Counts", -5e3, 3e4,
        false, false
    );

    c2->cd(3);
    plotSimple(
        hHFEMaxPlus3, "HFEMaxPlus3", labels,
        "HF E^{+} Max 3 [GeV]", -1, -1,
        "Counts", -5e3, 3e4,
        false, false
    );

    c2->cd(4);
    plotSimple(
        hHFEMaxMinus, "HFEMaxMinus", labels,
        "HF E^{-} Max [GeV]", -1, -1,
        "Counts", -5e3, 3e4,
        false, false
    );

    c2->cd(5);
    plotSimple(
        hHFEMaxMinus2, "HFEMaxMinus2", labels,
        "HF E^{-} Max 2 [GeV]", -1, -1,
        "Counts", -5e3, 3e4,
        false, false
    );

    c2->cd(6);
    plotSimple(
        hHFEMaxMinus3, "HFEMaxMinus3", labels,
        "HF E^{-} Max 3 [GeV]", -1, -1,
        "Counts", -5e3, 3e4,
        false, false
    );

    c2->cd(7);
    plotSimple(
        hTrkPhi, "TrkPhi", labels,
        "#phi", -3.2, 3.2,
        "Counts", -1, -1,
        false, false
    );

    c2->cd(8);
    plotSimple(
        hTrkCharge, "TrkCharge", labels,
        "Charge", -2, 2,
        "Counts", -1, -1,
        false, false
    );

    c2->cd(9);
    plotSimple(
        hTrkNHits, "TrkNHits", labels,
        "Number of Hits", -1, -1,
        "Counts", -1, -1,
        false, false
    );

    c2->cd(10);
    plotSimple(
        hTrkNPixHits, "TrkNPixHits", labels,
        "Number of Pixel Hits", -1, -1,
        "Counts", -1, -1,
        false, false
    );

    c2->cd(11);
    plotSimple(
        hTrkNLayers, "TrkNLayers", labels,
        "Number of Layers", -1, -1,
        "Counts", -1, -1,
        false, false
    );

    c2->cd(12);
    plotSimple(
        hTrkNormChi2, "TrkNormChi2", labels,
        "Norm #chi^{2}", -1, -1,
        "Counts", -1, -1,
        false, false
    );

    // Save as png
    c2->SaveAs(Form("%s-2.png", output));
}