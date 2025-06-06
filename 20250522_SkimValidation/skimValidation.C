#include <TFile.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TF1.h>
#include <iostream>

// example usage
// root -l -b -q "skimValidation.C(\"HiForestMiniAOD_Hijing_merged_DEBUG.root\", \"skimValidation.root\", false)"
// root -l -b -q "skimValidation.C(\"HiForestMiniAOD_Hijing_merged_DEBUG.root\", \"skimValidation_cut.root\", true)"

void skimValidation(const char* inputFile = "HiForestMiniAOD_Hijing_merged_DEBUG.root", 
                    const char* outputFile = "skimValidation.root",
                    bool isForest = true,
                    bool applyEventCuts = true,
                    bool applyTrackCuts = true) {

    // Open the input ROOT file
    TFile* finput = TFile::Open(inputFile, "READ");
    if (!finput || finput->IsZombie()) {
        std::cerr << "Error: Unable to open file " << inputFile << std::endl;
        return;
    }

    // output histogram initialization
    TH1D* hNEvtPassCuts = new TH1D("hNEvtPassCuts", "Number of events passing cuts", 4, 0.5, 4.5);
    hNEvtPassCuts->GetXaxis()->SetBinLabel(1, "Total Events");
    hNEvtPassCuts->GetXaxis()->SetBinLabel(2, "CC Filter");
    hNEvtPassCuts->GetXaxis()->SetBinLabel(3, "CC+PV Filter");
    hNEvtPassCuts->GetXaxis()->SetBinLabel(4, "CC+PV+nVtx>0 Filter");

    TH1D* hnRun =           new TH1D("hnRun", "Run Number", 10, 0, 10);
    TH1D* hnEv =            new TH1D("hnEv", "Event Number", 50, 0, 100);
    TH1D* hnLumi =          new TH1D("hnLumi", "Lumi Section", 50, 0, 1000);
    TH1D* hnVtx =           new TH1D("hnVtx", "Number of Vertices", 10, 0, 10);
    TH1D* hNpart =          new TH1D("hNpart", "Number of Participants", 50, 0, 50);
    TH1D* hNcoll =          new TH1D("hNcoll", "Number of Binary Collisions", 75, 0, 75);
    TH1D* hhiBin =          new TH1D("hhiBin", "Centrality Bin", 200, 0, 200);
    TH1D* hhiHF_pf =        new TH1D("hhiHF_pf", "HF Energy Density", 100, 0, 650);
    TH1D* hTrkPhi =         new TH1D("hTrkPhi", "Track Phi", 100, -3.14, 3.14);
    TH1D* hTrkCharge =      new TH1D("hTrkCharge", "Track Charge", 3, -1.5, 1.5);
    TH1D* hTrkNHits =       new TH1D("hTrkNHits", "Track Number of Hits", 50, 0, 50);
    TH1D* hTrkNPixHits =    new TH1D("hTrkNPixHits", "Track Number of Pixel Hits", 50, 0, 20);
    TH1D* hTrkNLayers =     new TH1D("hTrkNLayers", "Track Number of Layers", 16, -0.5, 15.5);
    TH1D* hTrkNormChi2 =    new TH1D("hTrkNormChi2", "Track Normalized Chi2", 100, 0, 10);
    TH1D* hpfEnergy =       new TH1D("hpfEnergy", "PF Energy", 50, 0, 30);
    TH1D* hHFEMaxPlus =     new TH1D("hHFEMaxPlus", "HF E Max Plus", 100, 0, 100);
    TH1D* hHFEMaxPlus2 =    new TH1D("hHFEMaxPlus2", "HF E Max Plus 2", 100, 0, 100);
    TH1D* hHFEMaxPlus3 =    new TH1D("hHFEMaxPlus3", "HF E Max Plus 3", 100, 0, 100);
    TH1D* hHFEMaxMinus =    new TH1D("hHFEMaxMinus", "HF E Max Minus", 100, 0, 100);
    TH1D* hHFEMaxMinus2 =   new TH1D("hHFEMaxMinus2", "HF E Max Minus 2", 100, 0, 100);
    TH1D* hHFEMaxMinus3 =   new TH1D("hHFEMaxMinus3", "HF E Max Minus 3", 100, 0, 100);

    TH3D* VXYZ =    new TH3D("VXYZ", "Best Vertex XYZ", 50, -0.1, 0.1, 50, -0.1, 0.1, 50, -25, 25);
    TH3D* VXYZErr = new TH3D("VXYZErr", "Best Vertex XYZ Error", 50, 0, 0.02, 50, 0, 0.02, 50, 0, 0.01);
    TH3D* htrkPtEtaHighPurity = new TH3D("trkPtEtaHighPurity", "Track pT vs Eta vs High Purity", 70, 0, 5, 100, -4, 4, 2, -0.5, 1.5);

    // Define branch variables
    TTree *tree;

    int nRun, nEv, nLumi, nVtx, hiBin;
    float Npart, Ncoll, hiHF_pf, VX, VY, VZ, VXError, VYError, VZError, ptSumVtx_skim;
    float HFEMaxPlus, HFEMaxPlus2, HFEMaxPlus3, HFEMaxMinus, HFEMaxMinus2, HFEMaxMinus3;
    Long64_t Event;

    int ClusterCompatibilityFilter, PVFilter;

    std::vector<float> *ptSumVtx = nullptr, *xVtx = nullptr, *yVtx = nullptr, *zVtx = nullptr;
    std::vector<float> *xErrVtx = nullptr, *yErrVtx = nullptr, *zErrVtx = nullptr;

    std::vector<float>  *trkPt = nullptr, *trkEta = nullptr, *highPurity = nullptr, 
                        *trkPhi = nullptr, *trkCharge = nullptr, *trkNHits = nullptr,
                        *trkNPixHits = nullptr, *trkNLayers = nullptr, *trkNormChi2 = nullptr,
                        *pfEnergy = nullptr,
                        *pfE = nullptr, *pfEta = nullptr;
    std::vector<int>    *pfId = nullptr;

    // if forest, get forest
    if (isForest) {
        tree = (TTree *)finput->Get("hltanalysis/HltTree");
        tree->AddFriend("skimanalysis/HltTree"); // Add the skim tree as a friend
        tree->AddFriend("hiEvtAnalyzer/HiTree"); // Add the event tree as a friend
        tree->AddFriend("ppTracks/trackTree");   // Add the track tree as a friend
        tree->AddFriend("particleFlowAnalyser/pftree"); // Add the particle flow tree as a friend

        // event-level
        tree->SetBranchAddress("nRun", &nRun);
        tree->SetBranchAddress("nEv", &nEv);
        tree->SetBranchAddress("nLumi", &nLumi);
        tree->SetBranchAddress("nVtx", &nVtx);
        tree->SetBranchAddress("hiBin", &hiBin);
        tree->SetBranchAddress("Npart", &Npart);
        tree->SetBranchAddress("Ncoll", &Ncoll);
        tree->SetBranchAddress("hiHF_pf", &hiHF_pf);
        tree->SetBranchAddress("pclusterCompatibilityFilter", &ClusterCompatibilityFilter);
        tree->SetBranchAddress("pprimaryVertexFilter", &PVFilter);

        // vertex-level
        tree->SetBranchAddress("ptSumVtx", &ptSumVtx); // best vertex is the one with the highest ptSum
        tree->SetBranchAddress("xVtx", &xVtx);
        tree->SetBranchAddress("yVtx", &yVtx);
        tree->SetBranchAddress("zVtx", &zVtx);
        tree->SetBranchAddress("xErrVtx", &xErrVtx);
        tree->SetBranchAddress("yErrVtx", &yErrVtx);
        tree->SetBranchAddress("zErrVtx", &zErrVtx);

        // track-level
        tree->SetBranchAddress("trkPt", &trkPt);
        tree->SetBranchAddress("trkEta", &trkEta);
        tree->SetBranchAddress("highPurity", &highPurity);
        tree->SetBranchAddress("trkPhi", &trkPhi);
        tree->SetBranchAddress("trkCharge", &trkCharge);
        tree->SetBranchAddress("trkNHits", &trkNHits);
        tree->SetBranchAddress("trkNPixHits", &trkNPixHits);
        tree->SetBranchAddress("trkNLayers", &trkNLayers);
        tree->SetBranchAddress("trkNormChi2", &trkNormChi2);
        tree->SetBranchAddress("pfEnergy", &pfEnergy);
        tree->SetBranchAddress("pfId", &pfId);
        tree->SetBranchAddress("pfE", &pfE);
        tree->SetBranchAddress("pfEta", &pfEta);

    } // if just skim
    else {
        tree = (TTree*)finput->Get("Tree");

        // event-level
        tree->SetBranchAddress("Run", &nRun);
        tree->SetBranchAddress("Event", &Event);
        tree->SetBranchAddress("Lumi", &nLumi);
        tree->SetBranchAddress("nVtx", &nVtx);
        tree->SetBranchAddress("hiBin", &hiBin);
        tree->SetBranchAddress("Npart", &Npart);
        tree->SetBranchAddress("Ncoll", &Ncoll);
        tree->SetBranchAddress("hiHF_pf", &hiHF_pf);
        tree->SetBranchAddress("ClusterCompatibilityFilter", &ClusterCompatibilityFilter);
        tree->SetBranchAddress("PVFilter", &PVFilter);
        tree->SetBranchAddress("VX", &VX);
        tree->SetBranchAddress("VY", &VY);
        tree->SetBranchAddress("VZ", &VZ);
        tree->SetBranchAddress("VXError", &VXError);
        tree->SetBranchAddress("VYError", &VYError);
        tree->SetBranchAddress("VZError", &VZError);
        tree->SetBranchAddress("ptSumVtx", &ptSumVtx_skim);
        tree->SetBranchAddress("HFEMaxPlus", &HFEMaxPlus);
        tree->SetBranchAddress("HFEMaxPlus2", &HFEMaxPlus2);
        tree->SetBranchAddress("HFEMaxPlus3", &HFEMaxPlus3);
        tree->SetBranchAddress("HFEMaxMinus", &HFEMaxMinus);
        tree->SetBranchAddress("HFEMaxMinus2", &HFEMaxMinus2);
        tree->SetBranchAddress("HFEMaxMinus3", &HFEMaxMinus3);
        
        // track-level
        tree->SetBranchAddress("trkPt", &trkPt);
        tree->SetBranchAddress("trkEta", &trkEta);
        tree->SetBranchAddress("highPurity", &highPurity);
        tree->SetBranchAddress("trkPhi", &trkPhi);
        tree->SetBranchAddress("trkCharge", &trkCharge);
        tree->SetBranchAddress("trkNHits", &trkNHits);
        tree->SetBranchAddress("trkNPixHits", &trkNPixHits);
        tree->SetBranchAddress("trkNLayers", &trkNLayers);
        tree->SetBranchAddress("trkNormChi2", &trkNormChi2);
        tree->SetBranchAddress("pfEnergy", &pfEnergy);

    }

    // Loop over the events
    int nEntries = tree->GetEntries();
    for (int i = 0; i < nEntries; ++i) {

        if (i % 1000 == 0) {
            std::cout << "Processing event " << i << " / " << nEntries << std::endl;
        }

        bool passedCuts = false;

        tree->GetEntry(i);

        // Fill the event count histogram
        hNEvtPassCuts->Fill(1); // Total events

        // Apply the CC filter
        if (ClusterCompatibilityFilter) {
            hNEvtPassCuts->Fill(2); // CC filter passed
        }

        // Apply the PV filter
        if (ClusterCompatibilityFilter && PVFilter) {
            hNEvtPassCuts->Fill(3); // PV filter passed
        }

        // Apply the nVtx filter
        if (ClusterCompatibilityFilter && PVFilter && nVtx > 0) {
            hNEvtPassCuts->Fill(4); // nVtx filter passed
            passedCuts = true;
        }

        // apply cuts if requested
        if (applyEventCuts && !passedCuts) continue;

        hnRun->Fill(nRun);
        isForest ? hnEv->Fill(nEv) : hnEv->Fill(Event);
        hnLumi->Fill(nLumi);
        hnVtx->Fill(nVtx);
        hNpart->Fill(Npart);
        hNcoll->Fill(Ncoll);
        hhiBin->Fill(hiBin);
        hhiHF_pf->Fill(hiHF_pf);

        // determine the best vertex if forest
        // if skim, best vertex is already found (VX, VY, VZ)
        if (isForest) {
            // find best vertex based on ptSumVtx
            int bestVertexIdx = -1;
            for (int j = 0; j < nVtx; ++j) {
                if ((bestVertexIdx == -1 || (ptSumVtx->at(j) > ptSumVtx->at(bestVertexIdx)))) {
                    bestVertexIdx = j;
                }
            }

            // use best vertex
            if (bestVertexIdx > -1) {
                VXYZ->Fill(xVtx->at(bestVertexIdx), yVtx->at(bestVertexIdx), zVtx->at(bestVertexIdx));
                VXYZErr->Fill(xErrVtx->at(bestVertexIdx), yErrVtx->at(bestVertexIdx), zErrVtx->at(bestVertexIdx));
            }
        } else {
            VXYZ->Fill(VX, VY, VZ);
            VXYZErr->Fill(VXError, VYError, VZError);
        }

        // loop over tracks
        for (int j = 0; j < trkPt->size(); ++j) {
            htrkPtEtaHighPurity->Fill(trkPt->at(j), trkEta->at(j), highPurity->at(j));

            // apply track cuts
            if (applyTrackCuts && highPurity->at(j) < 1) continue;

            hTrkPhi->Fill(trkPhi->at(j));
            hTrkCharge->Fill(trkCharge->at(j));
            hTrkNHits->Fill(trkNHits->at(j));
            hTrkNPixHits->Fill(trkNPixHits->at(j));
            hTrkNLayers->Fill(trkNLayers->at(j));
            hTrkNormChi2->Fill(trkNormChi2->at(j));
            hpfEnergy->Fill(pfEnergy->at(j));
        }

        // determine leading 1st, 2nd, 3rd PF candidates
        if (isForest) {
            HFEMaxPlus = 0;
            HFEMaxPlus2 = 0;
            HFEMaxPlus3 = 0;
            HFEMaxMinus = 0;
            HFEMaxMinus2 = 0;
            HFEMaxMinus3 = 0;
            for (int j = 0; j < pfId->size(); ++j) {

                // check if is PF Candidate
                float absEta = abs(pfEta->at(j));
                if (absEta < 3 || absEta > 5.2) continue;

                // compare to current 1st 2nd 3rd candidates
                // minus HFEMax corresponds with negative rapidity
                // plus HFEMax corresponds with positive rapidity
                if (pfEta->at(j) < 0) { // HF Minus
                    if (pfE->at(j) > HFEMaxMinus) {
                        HFEMaxMinus3 = HFEMaxMinus2;
                        HFEMaxMinus2 = HFEMaxMinus;
                        HFEMaxMinus = pfE->at(j);
                    } else if (pfE->at(j) > HFEMaxMinus2) {
                        HFEMaxMinus3 = HFEMaxMinus2;
                        HFEMaxMinus2 = pfE->at(j);
                    } else if (pfE->at(j) > HFEMaxMinus3) {
                        HFEMaxMinus3 = pfE->at(j);
                    }
                } else {
                    if (pfE->at(j) > HFEMaxPlus) {
                        HFEMaxPlus3 = HFEMaxPlus2;
                        HFEMaxPlus2 = HFEMaxPlus;
                        HFEMaxPlus = pfE->at(j);
                    } else if (pfE->at(j) > HFEMaxPlus2) {
                        HFEMaxPlus3 = HFEMaxPlus2;
                        HFEMaxPlus2 = pfE->at(j);
                    } else if (pfE->at(j) > HFEMaxPlus3) {
                        HFEMaxPlus3 = pfE->at(j);
                    }
                }
            }
        }
        if (HFEMaxPlus > 0)     hHFEMaxPlus->Fill(HFEMaxPlus);
        if (HFEMaxPlus2 > 0)    hHFEMaxPlus2->Fill(HFEMaxPlus2);
        if (HFEMaxPlus3 > 0)    hHFEMaxPlus3->Fill(HFEMaxPlus3);
        if (HFEMaxMinus > 0)    hHFEMaxMinus->Fill(HFEMaxMinus);
        if (HFEMaxMinus2 > 0)   hHFEMaxMinus2->Fill(HFEMaxMinus2);
        if (HFEMaxMinus3 > 0)   hHFEMaxMinus3->Fill(HFEMaxMinus3);
        
    }

    // write to output file
    TFile* foutput = new TFile(outputFile, "RECREATE");
    hNEvtPassCuts->Write();
    hnRun->Write();
    hnEv->Write();
    hnLumi->Write();
    hnVtx->Write();
    hNpart->Write();
    hNcoll->Write();
    hhiBin->Write();
    hhiHF_pf->Write();
    VXYZ->Write();
    VXYZErr->Write();
    htrkPtEtaHighPurity->Write();
    hTrkPhi->Write();
    hTrkCharge->Write();
    hTrkNHits->Write();
    hTrkNPixHits->Write();
    hTrkNLayers->Write();
    hTrkNormChi2->Write();
    hpfEnergy->Write();
    hHFEMaxPlus->Write();
    hHFEMaxPlus2->Write();
    hHFEMaxPlus3->Write();
    hHFEMaxMinus->Write();
    hHFEMaxMinus2->Write();
    hHFEMaxMinus3->Write();

    foutput->Close();
    finput->Close();

}
