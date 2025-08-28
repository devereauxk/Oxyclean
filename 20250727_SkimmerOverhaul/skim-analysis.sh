#!/bin/bash
# ============================================================
# Process skim by hand
# ============================================================

# ppref data
# forest: /eos/cms/store/group/phys_heavyions/vpant/ppref2024output/PPRefZeroBiasPlusForward4/crab_ppref2024/250324_080237/0000/HiForestMiniAOD_1.root
#oldSkim=/data00/kdeverea/OOsamples/Skims/output_20250727_Skim_ppref2024_data_noEvtSel_test/output_0.root
#newSkim=/home/kdeverea/MITHIGAnalysis2024/SampleGeneration/20250518_ForestReducer_RAAOxygenOxygen/temppp.root
#name=20250802_pTcutPR_ppref
#ApplyEventSelection=true
#ApplyTrackSelection=true


# OO low pT data
# forest: /eos/cms/store/group/phys_heavyions/jdlang/Run3_OxygenRAA/PromptForest/IonPhysics0/crab_OO_IonPhysics0_LowPtV2/250711_104114/0000/HiForest_2025OO_LowPtCfg_1.root
oldSkim=/data00/kdeverea/OOsamples/Skims/output_20250730_Skim_OO_IonPhysics0_LowPtV2_250711_104114_MB_CROSSCHECK/output/output_0.root
newSkim=/home/kdeverea/MITHIGAnalysis2024/SampleGeneration/20250518_ForestReducer_RAAOxygenOxygen/tempOO.root
name=20250802_emergency_OO
ApplyEventSelection=true
ApplyTrackSelection=true


#============================================================


# isForest=false, event cut, track cut
# skim
root -l -b -q "skimValidation.C(\"$oldSkim\", \"output/$name-oldSkim.root\", false, $ApplyEventSelection, $ApplyTrackSelection)"

root -l -b -q "skimValidation.C(\"$newSkim\", \"output/$name-newSkim.root\", false, $ApplyEventSelection, $ApplyTrackSelection)"

root -l -b -q "plotComparison.C(\"output/$name-oldSkim.root\", \"output/$name-newSkim.root\", \"plots/$name\")"
