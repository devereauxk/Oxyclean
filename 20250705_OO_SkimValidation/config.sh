# OO quick forest check
#export forestFile=/eos/cms/store/group/phys_heavyions/jdlang/Run3_OO_2025Data_QuickForest/OO_394153_PhysicsIonPhysics0/crab_OO_394153_PhysicsIonPhysics0/250705_074244/0000/HiForestMiniAOD_1.root
#export skimFile=/data00/bakovacs/OOsamples/Skims/20250705_OO_Kyle_crosscheck.root
#export name=crab_OO_394153_PhysicsIonPhysics0_debug_EvtTrkSel
#export ApplyEventSelection=true
#export ApplyTrackSelection=true


# OO prompt forest check on one file from one PD, low pT PD
#export forestFile=null
#export skimFile=/data00/kdeverea/OOsamples/Skims/output_20250713_Skim_OO_IonPhysics0_LowPtV2_250711_104114/0000/output_0.root
#export name=skim_OO_IonPhysics0_LowPtV2_250711_104114
#export ApplyEventSelection=true
#export ApplyTrackSelection=true

# OO prompt forest check on one file from one PD, high pT PD
#export forestFile=null
#export skimFile=/data00/kdeverea/OOsamples/Skims/output_20250715_Skim_OO_IonPhysics5_HighPtV2_250711_104159_40files/0000.root
#export name=skim_OO_IonPhysics5_HightPtV2_250711_104159
#export ApplyEventSelection=true
#export ApplyTrackSelection=true

# Skimmer overhaul, OO test
export forestFile=/eos/cms/store/group/phys_heavyions/jdlang/Run3_OxygenRAA/PromptForest/IonPhysics0/crab_OO_IonPhysics0_LowPtV2/250711_104114/0000/HiForest_2025OO_LowPtCfg_1.root
export skimFile=/home/kdeverea/MITHIGAnalysis2024/SampleGeneration/20250518_ForestReducer_RAAOxygenOxygen/tempOO_noTriggerNoEvt.root
export name=energency
export ApplyEventSelection=false
export ApplyTrackSelection=true
