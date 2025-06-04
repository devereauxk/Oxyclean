forestFile=Forests/crab_HiForest_250520_Hijing_MinimumBias_b015_OO_5362GeV_250518.root
skimFile=bakovacs/skim_v1_20250602_HiForest_250520_Hijing_MinimumBias_b015_OO_5362GeV_250518.root
name=Hijing_bakovacsskim

# forest, no cut
root -l -b -q "skimValidation.C(\"$forestFile\", \"output/Hijing_forest_nocut.root\", true, false)"

# skim, no cut
root -l -b -q "skimValidation.C(\"$skimFile\", \"output/Hijing_skim_nocut.root\", false, false)"

# plot no cut
root -l -b -q "plotForest.C(\"output/Hijing_forest_nocut.root\", \"output/$name-nocut.root\", \"plots/$name-nocut.png\")"

exit

# forest, cut
root -l -b -q "skimValidation.C(\"$forestFile\", \"output/Hijing_forest_cut.root\", true, true)"

# skim, cut
#root -l -b -q "skimValidation.C(\"$skimFile\", \"output/Hijing_skim_cut.root\", false, true)"

# plot cut
root -l -b -q "plotForest.C(\"output/Hijing_forest_cut.root\", \"output/$name-cut.root\", \"plots/$name-cut.png\")"



###############################################################3
# skim that includes ptSumVtx info
#root -l -b -q "skimValidation.C(\"skim_HiForest_Hijing_withPtsumVtx.root\", \"output/skimValidation_withPtsumVtx.root\", false, true)"