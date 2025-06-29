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
    TH1D* hNEvtPassCuts = new TH1D("hNEvtPassCuts", "Number of events passing cuts", 6, 0.5, 6.5);
    hNEvtPassCuts->GetXaxis()->SetBinLabel(1, "Total Events");
    hNEvtPassCuts->GetXaxis()->SetBinLabel(2, "[no CC for ppref]");
    hNEvtPassCuts->GetXaxis()->SetBinLabel(3, "+ PV");
    hNEvtPassCuts->GetXaxis()->SetBinLabel(4, "+ nVtx>0");
    hNEvtPassCuts->GetXaxis()->SetBinLabel(5, "+ !isFakeVtx");
    hNEvtPassCuts->GetXaxis()->SetBinLabel(6, "+ abs(VZ)<15");

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
    TH1D* hisFakeVtx    =   new TH1D("hisFakeVtx", "Fake Vertex Count", 2, -0.5, 1.5);
    TH1D* hAllisFakeVtx =   new TH1D("hAllisFakeVtx", "All Fake Vertex Count", 2, -0.5, 1.5);
    TH1D* hnTracksVtx =     new TH1D("hnTracksVtx", "Number of Tracks per Vertex", 100, 0, 100);
    TH1D* hAllnTracksVtx =  new TH1D("hAllnTracksVtx", "All Tracks per Vertex", 100, 0, 100);
    TH1D* hchi2Vtx =        new TH1D("hchi2Vtx", "Vertex Chi2", 100, 0, 100);
    TH1D* hAllchi2Vtx =     new TH1D("hAllchi2Vtx", "All Vertex Chi2", 100, 0, 100);
    TH1D* hndofVtx =        new TH1D("hndofVtx", "Vertex NDOF", 100, 0, 100);
    TH1D* hAllndofVtx =     new TH1D("hAllndofVtx", "All Vertex NDOF", 100, 0, 100);
    TH1D* hptSumVtx =       new TH1D("hptSumVtx", "Best Vertex pT Sum", 100, 0, 100);
    TH1D* hAllptSumVtx =    new TH1D("hAllptSumVtx", "All Vertex pT Sum", 100, 0, 1000);

    TH1D* htrkDxyAssociatedVtx  =   new TH1D("htrkDxyAssociatedVtx", "Track Dxy Associated to Vertex", 100, -0.1, 0.1);
    TH1D* htrkDxyErrAssociatedVtx = new TH1D("htrkDxyErrAssociatedVtx", "Track Dxy Error Associated to Vertex", 100, 0, 0.02);
    TH1D* htrkDzAssociatedVtx  =    new TH1D("htrkDzAssociatedVtx", "Track Dz Associated to Vertex", 100, -0.1, 0.1);
    TH1D* htrkDzErrAssociatedVtx =  new TH1D("htrkDzErrAssociatedVtx", "Track Dz Error Associated to Vertex", 100, 0, 0.02);
    TH1D* htrkAssociatedVtxIndx =    new TH1D("htrkAssociatedVtxIndx", "Track Associated Vertex Index", 10, -0.5, 10.5);

    TH3D* VXYZ =    new TH3D("VXYZ", "Best Vertex XYZ", 50, -0.1, 0.1, 50, -0.1, 0.1, 50, -25, 25);
    TH3D* VXYZErr = new TH3D("VXYZErr", "Best Vertex XYZ Error", 50, 0, 0.02, 50, 0, 0.02, 50, 0, 0.01);
    TH3D* htrkPtEtaHighPurity = new TH3D("trkPtEtaHighPurity", "Track pT vs Eta vs High Purity", 70, 0, 5, 100, -4, 4, 2, -0.5, 1.5);

    TH3D* AllVXYZ =   new TH3D("AllVXYZ", "All Vertices XYZ", 50, -0.1, 0.1, 50, -0.1, 0.1, 50, -25, 25);
    TH3D* AllVXYZErr = new TH3D("AllVXYZErr", "All Vertices XYZ Error", 50, 0, 0.02, 50, 0, 0.02, 50, 0, 0.01);

    // Define branch variables
    TTree *tree;

    int nRun, nEv, nLumi, nVtx, hiBin;
    float Npart, Ncoll, hiHF_pf, VX, VY, VZ, VXError, VYError, VZError, ptSumVtx_skim;
    float HFEMaxPlus, HFEMaxPlus2, HFEMaxPlus3, HFEMaxMinus, HFEMaxMinus2, HFEMaxMinus3;
    float chi2Vtx_skim, ndofVtx_skim;
    Long64_t Event;
    bool isFakeVtx_skim;
    int nTracksVtx_skim;

    int ClusterCompatibilityFilter, PVFilter;

    std::vector<float> *ptSumVtx = nullptr, *xVtx = nullptr, *yVtx = nullptr, *zVtx = nullptr;
    std::vector<float> *xErrVtx = nullptr, *yErrVtx = nullptr, *zErrVtx = nullptr;

    std::vector<float>  *trkPt = nullptr, *trkEta = nullptr, *highPurity = nullptr, 
                        *trkPhi = nullptr, *trkCharge = nullptr, *trkNHits = nullptr,
                        *trkNPixHits = nullptr, *trkNLayers = nullptr, *trkNormChi2 = nullptr,
                        *pfEnergy = nullptr,
                        *pfE = nullptr, *pfEta = nullptr;
    std::vector<float>  *trkDxyAssociatedVtx = nullptr, *trkDxyErrAssociatedVtx = nullptr,
                        *trkDzAssociatedVtx = nullptr, *trkDzErrAssociatedVtx = nullptr,
                        *trkAssociatedVtxIndx = nullptr;
    std::vector<bool>   *isFakeVtx = nullptr;
    std::vector<int>    *pfId = nullptr, *AllnTracksVtx = nullptr, *nTracksVtx = nullptr;
    std::vector<float>  *chi2Vtx = nullptr, *ndofVtx = nullptr;

    // if forest, get forest
    if (isForest) {
        tree = (TTree *)finput->Get("hltanalysis/HltTree");
        tree->AddFriend("skimanalysis/HltTree"); // Add the skim tree as a friend
        tree->AddFriend("hiEvtAnalyzer/HiTree"); // Add the event tree as a friend
        tree->AddFriend("ppTracks/trackTree");   // Add the track tree as a friend
        //tree->AddFriend("particleFlowAnalyser/pftree"); // Add the particle flow tree as a friend

        // event-level
        tree->SetBranchAddress("nRun", &nRun);
        tree->SetBranchAddress("nEv", &nEv);
        tree->SetBranchAddress("nLumi", &nLumi);
        tree->SetBranchAddress("nVtx", &nVtx);
        tree->SetBranchAddress("hiBin", &hiBin);
        //tree->SetBranchAddress("Npart", &Npart);
        //tree->SetBranchAddress("Ncoll", &Ncoll);
        tree->SetBranchAddress("hiHF_pf", &hiHF_pf);
        //tree->SetBranchAddress("pclusterCompatibilityFilter", &ClusterCompatibilityFilter);
        tree->SetBranchAddress("pprimaryVertexFilter", &PVFilter);

        // vertex-level
        tree->SetBranchAddress("ptSumVtx", &ptSumVtx); // best vertex is the one with the highest ptSum
        tree->SetBranchAddress("xVtx", &xVtx);
        tree->SetBranchAddress("yVtx", &yVtx);
        tree->SetBranchAddress("zVtx", &zVtx);
        tree->SetBranchAddress("xErrVtx", &xErrVtx);
        tree->SetBranchAddress("yErrVtx", &yErrVtx);
        tree->SetBranchAddress("zErrVtx", &zErrVtx);
        tree->SetBranchAddress("isFakeVtx", &isFakeVtx);
        tree->SetBranchAddress("nTracksVtx", &nTracksVtx);
        tree->SetBranchAddress("chi2Vtx", &chi2Vtx);
        tree->SetBranchAddress("ndofVtx", &ndofVtx);

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
        //tree->SetBranchAddress("pfId", &pfId);
        //tree->SetBranchAddress("pfE", &pfE);
        //tree->SetBranchAddress("pfEta", &pfEta);
        tree->SetBranchAddress("trkDxyAssociatedVtx", &trkDxyAssociatedVtx);
        tree->SetBranchAddress("trkDxyErrAssociatedVtx", &trkDxyErrAssociatedVtx);
        tree->SetBranchAddress("trkDzAssociatedVtx", &trkDzAssociatedVtx);
        tree->SetBranchAddress("trkDzErrAssociatedVtx", &trkDzErrAssociatedVtx);
        tree->SetBranchAddress("trkAssociatedVtxIndx", &trkAssociatedVtxIndx);

        // vipul's method
        tree->SetBranchAddress("vz", &VZ);

    } // if just skim
    else {
        tree = (TTree*)finput->Get("Tree");

        // event-level
        tree->SetBranchAddress("Run", &nRun);
        tree->SetBranchAddress("Event", &Event);
        tree->SetBranchAddress("Lumi", &nLumi);
        tree->SetBranchAddress("nVtx", &nVtx);
        tree->SetBranchAddress("hiBin", &hiBin);
        //tree->SetBranchAddress("Npart", &Npart);
        //tree->SetBranchAddress("Ncoll", &Ncoll);
        tree->SetBranchAddress("hiHF_pf", &hiHF_pf);
        //tree->SetBranchAddress("ClusterCompatibilityFilter", &ClusterCompatibilityFilter);
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
        tree->SetBranchAddress("isFakeVtx", &isFakeVtx_skim);
        tree->SetBranchAddress("nTracksVtx", &nTracksVtx_skim);
        tree->SetBranchAddress("chi2Vtx", &chi2Vtx_skim);
        tree->SetBranchAddress("ndofVtx", &ndofVtx_skim);

        // vertex-level
        tree->SetBranchAddress("AllxVtx", &xVtx);
        tree->SetBranchAddress("AllyVtx", &yVtx);
        tree->SetBranchAddress("AllzVtx", &zVtx);
        tree->SetBranchAddress("AllxVtxError", &xErrVtx);
        tree->SetBranchAddress("AllyVtxError", &yErrVtx);
        tree->SetBranchAddress("AllzVtxError", &zErrVtx);
        tree->SetBranchAddress("AllisFakeVtx", &isFakeVtx);
        tree->SetBranchAddress("AllnTracksVtx", &nTracksVtx);
        tree->SetBranchAddress("Allchi2Vtx", &chi2Vtx);
        tree->SetBranchAddress("AllndofVtx", &ndofVtx);
        tree->SetBranchAddress("AllptSumVtx", &ptSumVtx);

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
        tree->SetBranchAddress("trkDxyAssociatedVtx", &trkDxyAssociatedVtx);
        tree->SetBranchAddress("trkDxyErrAssociatedVtx", &trkDxyErrAssociatedVtx);
        tree->SetBranchAddress("trkDzAssociatedVtx", &trkDzAssociatedVtx);
        tree->SetBranchAddress("trkDzErrAssociatedVtx", &trkDzErrAssociatedVtx);
        tree->SetBranchAddress("trkAssociatedVtxIndx", &trkAssociatedVtxIndx);

    }

    // Loop over the events
    int nEntries = tree->GetEntries();
    int printCounter = 0;
    for (int i = 0; i < nEntries; ++i) {

        if (i % 1000 == 0) {
            std::cout << "Processing event " << i << " / " << nEntries << std::endl;
        }

        bool passedCuts = false;

        tree->GetEntry(i);

        // determine the best vertex if forest
        // if skim, best vertex is already found (VX, VY, VZ)
        int bestVertexIdx = -1;
        if (isForest) {
            for (int j = 0; j < nVtx; ++j) {
                if (bestVertexIdx == -1 || (bestVertexIdx != -1 && ptSumVtx->at(j) > ptSumVtx->at(bestVertexIdx))) {
                    bestVertexIdx = j;
                }
            }
        }

        // Fill the event count histogram
        hNEvtPassCuts->Fill(1); // Total events

        // Apply the CC filter, not for ppref
        hNEvtPassCuts->Fill(2); // CC filter passed

        // Apply the PV filter
        if ( PVFilter) {
            hNEvtPassCuts->Fill(3); // PV filter passed
        }

        // Apply the nVtx filter
        if ( PVFilter && nVtx > 0) {
            hNEvtPassCuts->Fill(4); // nVtx filter passed
        }

        // Apply the isFakeVtx filter
        if (isForest) {
            if ( PVFilter && nVtx > 0 && bestVertexIdx > -1 && !isFakeVtx->at(bestVertexIdx)) {
                hNEvtPassCuts->Fill(5); // isFakeVtx filter passed
                //passedCuts = true; // mark as passed if all conditions are met
            }
        } else {
            if ( PVFilter && nVtx > 0 && !isFakeVtx_skim) {
                hNEvtPassCuts->Fill(5); // isFakeVtx filter passed
                //passedCuts = true; // mark as passed if all conditions are met
            }
        }

        // Apply the VZ filter
        if ( PVFilter && nVtx > 0 && 
            ((isForest && bestVertexIdx > -1 && fabs(zVtx->at(bestVertexIdx)) < 15.0) || 
             (!isForest && fabs(VZ) < 15.0))) {
            hNEvtPassCuts->Fill(6); // VZ filter passed
            passedCuts = true;
        }

        if (passedCuts && bestVertexIdx != -1 && VZ != zVtx->at(bestVertexIdx) && fabs(VZ) < 15.0 && fabs(zVtx->at(bestVertexIdx)) > 15.0) {
            std::cerr << bestVertexIdx << " "<< VZ << " " << zVtx->at(bestVertexIdx) << " " << ptSumVtx->at(bestVertexIdx) << " " << nVtx << " " << nEv << std::endl;
            printCounter++;
        }

        // apply cuts if requested
        if (applyEventCuts && !passedCuts) continue;

        // event-level
        hnRun->Fill(nRun);
        isForest ? hnEv->Fill(nEv) : hnEv->Fill(Event);
        hnLumi->Fill(nLumi);
        hnVtx->Fill(nVtx);
        hNpart->Fill(Npart);
        hNcoll->Fill(Ncoll);
        hhiBin->Fill(hiBin);
        hhiHF_pf->Fill(hiHF_pf);

        // vertex-level
        for (int j = 0; j < nVtx; ++j) {
            AllVXYZ->Fill(xVtx->at(j), yVtx->at(j), zVtx->at(j));
            AllVXYZErr->Fill(xErrVtx->at(j), yErrVtx->at(j), zErrVtx->at(j));
            hAllisFakeVtx->Fill(isFakeVtx->at(j));
            hAllnTracksVtx->Fill(nTracksVtx->at(j));
            hAllchi2Vtx->Fill(chi2Vtx->at(j));
            hAllndofVtx->Fill(ndofVtx->at(j));
            hAllptSumVtx->Fill(ptSumVtx->at(j));
        }

        // determine the best vertex if forest
        // if skim, best vertex is already found (VX, VY, VZ)
        if (isForest) {
            // use best vertex
            if (bestVertexIdx > -1) {
                VXYZ->Fill(xVtx->at(bestVertexIdx), yVtx->at(bestVertexIdx), zVtx->at(bestVertexIdx));
                VXYZErr->Fill(xErrVtx->at(bestVertexIdx), yErrVtx->at(bestVertexIdx), zErrVtx->at(bestVertexIdx));
                if (isFakeVtx->at(bestVertexIdx)) {
                    hisFakeVtx->Fill(1);
                } else {
                    hisFakeVtx->Fill(0);
                }
                hnTracksVtx->Fill(nTracksVtx->at(bestVertexIdx));
                hchi2Vtx->Fill(chi2Vtx->at(bestVertexIdx));
                hndofVtx->Fill(ndofVtx->at(bestVertexIdx));
                hptSumVtx->Fill(ptSumVtx->at(bestVertexIdx));
            }
        } else {
            VXYZ->Fill(VX, VY, VZ);
            VXYZErr->Fill(VXError, VYError, VZError);
            if (isFakeVtx_skim) {
                hisFakeVtx->Fill(1);
            } else {
                hisFakeVtx->Fill(0);
            }
            hnTracksVtx->Fill(nTracksVtx_skim);
            hchi2Vtx->Fill(chi2Vtx_skim);
            hndofVtx->Fill(ndofVtx_skim);
            hptSumVtx->Fill(ptSumVtx_skim);
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
            htrkDxyAssociatedVtx->Fill(trkDxyAssociatedVtx->at(j));
            htrkDxyErrAssociatedVtx->Fill(trkDxyErrAssociatedVtx->at(j));
            htrkDzAssociatedVtx->Fill(trkDzAssociatedVtx->at(j));
            htrkDzErrAssociatedVtx->Fill(trkDzErrAssociatedVtx->at(j));
            htrkAssociatedVtxIndx->Fill(trkAssociatedVtxIndx->at(j));
        }

        // determine leading 1st, 2nd, 3rd PF candidates
        /*
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
        */
        
    }

    std::cout << "Total number of events printed: " << printCounter << std::endl;

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
    AllVXYZ->Write();
    AllVXYZErr->Write();
    hisFakeVtx->Write();
    hAllisFakeVtx->Write();
    hnTracksVtx->Write();
    hAllnTracksVtx->Write();
    hchi2Vtx->Write();
    hAllchi2Vtx->Write();
    hndofVtx->Write();
    hAllndofVtx->Write();
    hptSumVtx->Write();
    hAllptSumVtx->Write();
    htrkDxyAssociatedVtx->Write();
    htrkDxyErrAssociatedVtx->Write();
    htrkDzAssociatedVtx->Write();
    htrkDzErrAssociatedVtx->Write();
    htrkAssociatedVtxIndx->Write();

    foutput->Close();
    finput->Close();

}
