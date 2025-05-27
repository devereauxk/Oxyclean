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
                    bool applyCuts = true) {

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

    TH1D* hnRun =   new TH1D("hnRun", "Run Number", 10, 0, 10);
    TH1D* hnEv =    new TH1D("hnEv", "Event Number", 50, 0, 100);
    TH1D* hnLumi =  new TH1D("hnLumi", "Lumi Section", 50, 0, 1000);
    TH1D* hnVtx =   new TH1D("hnVtx", "Number of Vertices", 10, 0, 10);
    TH1D* hNpart =  new TH1D("hNpart", "Number of Participants", 50, 0, 50);
    TH1D* hNcoll =  new TH1D("hNcoll", "Number of Binary Collisions", 75, 0, 75);
    TH1D* hhiBin =  new TH1D("hhiBin", "Centrality Bin", 200, 0, 200);
    TH1D* hhiHF_pf =new TH1D("hhiHF_pf", "HF Energy Density", 100, 0, 650);

    TH3D* VXYZ = new TH3D("VXYZ", "Best Vertex XYZ", 50, -0.1, 0.1, 50, -0.1, 0.1, 50, -25, 25);
    TH3D* VXYZErr = new TH3D("VXYZErr", "Best Vertex XYZ Error", 50, -0.1, 0.1, 50, -0.1, 0.1, 50, -10, 10);

    // Define branch variables
    TTree *tree;

    int nRun, nEv, nLumi, nVtx, hiBin;
    float Npart, Ncoll, hiHF_pf, VX, VY, VZ, VXError, VYError, VZError, ptSumVtx_skim;
    Long64_t Event;

    int ClusterCompatibilityFilter, PVFilter;

    std::vector<float> *ptSumVtx = nullptr, *xVtx = nullptr, *yVtx = nullptr, *zVtx = nullptr;
    std::vector<float> *xErrVtx = nullptr, *yErrVtx = nullptr, *zErrVtx = nullptr;

    std::vector<float> *trkPt = nullptr;

    // if forest, get forest
    if (isForest) {
        tree = (TTree *)finput->Get("hltanalysis/HltTree");
        tree->AddFriend("skimanalysis/HltTree"); // Add the skim tree as a friend
        tree->AddFriend("hiEvtAnalyzer/HiTree"); // Add the event tree as a friend
        tree->AddFriend("ppTracks/trackTree");   // Add the track tree as a friend

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
        
        // track-level
        tree->SetBranchAddress("trkPt", &trkPt);

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
        if (applyCuts && !passedCuts) continue;

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
    
    foutput->Close();
    finput->Close();

}
