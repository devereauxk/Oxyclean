#forestFile=HiForestMiniAOD_Hijing_merged_DEBUG.root
#skimFile=skim_HiForest_Hijing_merged_DEBUG.root

forestFile=crab_HiForest_250520_Hijing_MinimumBias_b015_OO_5362GeV_250518.root
skimFile=skim_HiForest_250520_Hijing_MinimumBias_b015_OO_5362GeV_250518.root

# forest, no cut
root -l -b -q "skimValidation.C(\"$forestFile\", \"output/skimValidation_forest_nocut.root\", true, false)"

# skim, no cut
root -l -b -q "skimValidation.C(\"$skimFile\", \"output/skimValidation_skim_nocut.root\", false, false)"

# plot no cut
root -l -b -q "plotForest.C(\"output/skimValidation_forest_nocut.root\", \"output/skimValidation_skim_nocut.root\", \"plots/forest_and_skim-nocut.png\")"

# forest, cut
root -l -b -q "skimValidation.C(\"$forestFile\", \"output/skimValidation_forest_cut.root\", true, true)"

# skim, cut
root -l -b -q "skimValidation.C(\"$skimFile\", \"output/skimValidation_skim_cut.root\", false, true)"

# plot cut
root -l -b -q "plotForest.C(\"output/skimValidation_forest_cut.root\", \"output/skimValidation_skim_cut.root\", \"plots/forest_and_skim-cut.png\")"

###############################################################3
# skim that includes ptSumVtx info
#root -l -b -q "skimValidation.C(\"skim_HiForest_Hijing_withPtsumVtx.root\", \"output/skimValidation_withPtsumVtx.root\", false, true)"