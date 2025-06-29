#forestFile=/eos/cms/store/group/phys_heavyions/vpant/ppref2024output/PPRefZeroBiasPlusForward4/crab_ppref2024/250324_080237/0000/HiForestMiniAOD_1.root


#skimFile=/home/kdeverea/MITHIGAnalysis2024/SampleGeneration/20250518_ForestReducer_RAAOxygenOxygen/Output/Skim_HiForestMiniAOD_ppchargedhadron2024_debugfile.root
#name=pprefdebug

#skimFile=/data00/kdeverea/OOsamples/Skims/20250629_Skim_OOMCforJing.root
#name=OOMCforJing

forestFile=/eos/cms/store/group/phys_heavyions/jdlang/Run3_OO_2025Data_FastPrivateReco/TEMP/HiForestMiniAOD_PhysicsIonPhysics0_393767.root
skimFile=/data00/kdeverea/OOsamples/Skims/Run3_OO_2025Data_FastPrivateReco_393767.root
name=Run3_OO_2025Data_FastPrivateReco_393767

# event cut, track cut
# forest
root -l -b -q "skimValidation.C(\"$forestFile\", \"output/$name-forest.root\", true, false, false)"

# skim
root -l -b -q "skimValidation.C(\"$skimFile\", \"output/$name-skim.root\", false, false, false)"

# plot
root -l -b -q "plotForest.C(\"output/$name-forest.root\", \"output/$name-skim.root\", \"plots/$name\")"
